#include "full_renderer.hpp"

#include <cmath>
#include "geometry.hpp"

FullRenderer::FullRenderer(int width, int height, int depth)
    : Renderer(width, height), depth_(depth) {
    buffer_.resize(width * height);
}

void FullRenderer::SetPixel(int x, int y, const Ray& ray, const Scene& scene) {
    Vector illumination = TraceRay(scene, ray, depth_, false);
    double max_part = std::max(illumination[0], std::max(illumination[1], illumination[2]));
    max_scalar_ = std::max(max_scalar_, max_part);
    buffer_[y * image_.GetWidth() + x] = illumination;
}

Image FullRenderer::BuildImage() {
    if (std::fabs(max_scalar_) <= 1e-9) {
        return std::move(image_);
    }
    double inv_max_scalar = 1 / (max_scalar_ * max_scalar_);
    for (size_t i = 0; i < buffer_.size(); i++) {
        if (!buffer_[i]) {
            continue;
        }
        int y = i / image_.GetWidth();
        int x = i % image_.GetWidth();
        Vector color = *buffer_[i];
        color = color * (1 + inv_max_scalar * color) / (1 + color);
        color[0] = std::pow(color[0], 1.0 / 2.2);
        color[1] = std::pow(color[1], 1.0 / 2.2);
        color[2] = std::pow(color[2], 1.0 / 2.2);
        int r = static_cast<int>(color[0] * 255);
        int g = static_cast<int>(color[1] * 255);
        int b = static_cast<int>(color[2] * 255);
        image_.SetPixel({r, g, b}, y, x);
    }
    return std::move(image_);
}

Vector operator*(const Vector& rgb1, const Vector& rgb2) {
    return Vector(rgb1[0] * rgb2[0], rgb1[1] * rgb2[1], rgb1[2] * rgb2[2]);
}

Vector operator+(double scalar, const Vector& vec) {
    return Vector(vec[0] + scalar, vec[1] + scalar, vec[2] + scalar);
}

Vector operator/(const Vector& rgb1, const Vector& rgb2) {
    return Vector(rgb1[0] / rgb2[0], rgb1[1] / rgb2[1], rgb1[2] / rgb2[2]);
}

Vector CalculateIllumination(const Ray& ray, const Hit& hit, const Scene& scene) {
    Vector light_ray_origin = hit.intersection.GetPosition() + kEps * hit.intersection.GetNormal();
    Vector diffusion_sum = Vector();
    Vector flare_sum = Vector();
    for (const auto& light : scene.GetLights()) {
        Vector direction = light.position - light_ray_origin;
        double distance = Length(direction);
        direction.Normalize();
        Ray light_ray = Ray(light_ray_origin, direction);
        auto obj_hit = FindNearestIntersection(light_ray, scene);
        if (obj_hit && (*obj_hit).intersection.GetDistance() < distance) {
            continue;
        }
        Vector diffusion = std::max(0.0, DotProduct(direction, hit.normal)) * light.intensity;
        Vector v_r = -1 * ray.GetDirection();
        Vector v_lr = Reflect(-1 * direction, hit.normal);
        Vector flare =
            std::pow(std::max(0.0, DotProduct(v_r, v_lr)), hit.material->specular_exponent) *
            light.intensity;

        diffusion_sum = diffusion_sum + diffusion;
        flare_sum = flare_sum + flare;
    }
    const Material* material = hit.material;
    Vector illumination = material->albedo[0] * (material->diffuse_color * diffusion_sum +
                                                 material->specular_color * flare_sum) +
                          material->ambient_color + material->intensity;
    return illumination;
}

Vector TraceRay(const Scene& scene, const Ray& ray, int depth, bool inside) {
    auto hit = FindNearestIntersection(ray, scene);
    if (!hit) {
        return {0, 0, 0};
    }
    Vector illumination = CalculateIllumination(ray, hit.value(), scene);
    if (hit->material->albedo[1] != 0 && !inside && depth > 0) {
        Vector reflected_dir = Reflect(ray.GetDirection(), hit->normal);
        Vector new_origin = hit->intersection.GetPosition() + kEps * hit->intersection.GetNormal();
        Ray reflected_ray = Ray(new_origin, reflected_dir);
        illumination = illumination +
                       hit->material->albedo[1] * TraceRay(scene, reflected_ray, depth - 1, inside);
    }
    if (hit->material->albedo[2] != 0 && depth > 0) {
        double eta =
            inside ? hit->material->refraction_index : 1.0 / hit->material->refraction_index;
        auto refracted_dir = Refract(ray.GetDirection(), hit->normal, eta);
        if (refracted_dir) {
            Vector new_origin =
                hit->intersection.GetPosition() - kEps * hit->intersection.GetNormal();
            Ray refracted_ray = Ray(new_origin, *refracted_dir);
            double transparency = inside ? 1.0 : hit->material->albedo[2];
            illumination =
                illumination + transparency * TraceRay(scene, refracted_ray, depth - 1, !inside);
        }
    }
    return illumination;
}
