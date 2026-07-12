#pragma once

#include <cstddef>
#include "geometry.h"
#include "hit.h"
#include "image.h"
#include "ray.h"
#include "scene.h"
#include "vector.h"

std::optional<Hit> FindNearestIntersection(const Ray& ray, const Scene& scene);

class Renderer {
public:
    Renderer(int width, int height) : image_(width, height) {
    }
    virtual void SetPixel(int x, int y, const Ray& ray, const Scene& scene) = 0;
    virtual Image BuildImage() = 0;
    virtual ~Renderer() = default;

protected:
    Image image_;
};

class NormalRenderer : public Renderer {
public:
    NormalRenderer(int width, int height) : Renderer(width, height) {
    }
    void SetPixel(int x, int y, const Ray& ray, const Scene& scene) {
        auto hit = FindNearestIntersection(ray, scene);
        if (!hit) {
            image_.SetPixel({0, 0, 0}, y, x);
            return;
        }
        Vector color = 0.5 * hit->normal;
        color[0] += 0.5;
        color[1] += 0.5;
        color[2] += 0.5;
        int r = static_cast<int>(color[0] * 255);
        int g = static_cast<int>(color[1] * 255);
        int b = static_cast<int>(color[2] * 255);
        image_.SetPixel({r, g, b}, y, x);
    }

    Image BuildImage() {
        return std::move(image_);
    }
};

class DepthRenderer : public Renderer {
public:
    DepthRenderer(int width, int height) : Renderer(width, height) {
        buffer_.resize(width * height);
    }
    void SetPixel(int x, int y, const Ray& ray, const Scene& scene) {
        auto hit = FindNearestIntersection(ray, scene);
        if (!hit) {
            image_.SetPixel({255, 255, 255}, y, x);
            return;
        }
        max_distance_ = std::max(max_distance_, hit->intersection.GetDistance());
        buffer_[y * image_.Width() + x] = hit->intersection.GetDistance();
    }

    Image BuildImage() {
        double inv_dist = 1 / max_distance_;
        for (size_t i = 0; i < buffer_.size(); i++) {
            if (!buffer_[i]) {
                continue;
            }
            int y = i / image_.Width();
            int x = i % image_.Width();
            double color = *buffer_[i] * inv_dist;
            int r = static_cast<int>(color * 255);
            int g = static_cast<int>(color * 255);
            int b = static_cast<int>(color * 255);
            image_.SetPixel({r, g, b}, y, x);
        }
        return std::move(image_);
    }

private:
    std::vector<std::optional<double>> buffer_;
    double max_distance_ = 0;
};

std::optional<Hit> FindNearestIntersection(const Ray& ray, const Scene& scene) {
    std::optional<Hit> best_hit;

    auto check_intersection = [&best_hit](std::optional<Intersection> intersection,
                                          const Material* material, Vector normal) {
        if (intersection &&
            (!best_hit || intersection->GetDistance() < best_hit->intersection.GetDistance())) {
            best_hit = Hit{intersection.value(), material, normal};
            return true;
        }
        return false;
    };

    for (const auto& object : scene.GetObjects()) {
        auto intersection = GetIntersection(ray, object.polygon);
        if (!check_intersection(intersection, object.material,
                                intersection ? intersection->GetNormal() : Vector())) {
            continue;
        }
        const Vector* n0 = object.GetNormal(0);
        const Vector* n1 = object.GetNormal(1);
        const Vector* n2 = object.GetNormal(2);
        if (n0 == nullptr || n1 == nullptr || n2 == nullptr) {
            continue;
        }
        Vector bar_coords = GetBarycentricCoords(object.polygon, intersection->GetPosition());
        Vector normal = bar_coords[0] * *n0 + bar_coords[1] * *n1 + bar_coords[2] * *n2;
        normal.Normalize();
        if (DotProduct(normal, ray.GetDirection()) > 0) {
            normal = (-1) * normal;
        }
        best_hit->normal = normal;
    }
    for (const auto& sphere : scene.GetSphereObjects()) {
        auto intersection = GetIntersection(ray, sphere.sphere);
        check_intersection(intersection, sphere.material,
                           intersection ? intersection->GetNormal() : Vector());
    }
    return best_hit;
}
