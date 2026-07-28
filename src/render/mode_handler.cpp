#include "mode_handler.hpp"
#include "geometry.hpp"

Renderer::Renderer(int width, int height) : image_(width, height) {
}

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
        auto normal = InterpolateNormals(object, intersection.value(), ray);
        if (normal) {
            best_hit->normal = normal.value();
        }
    }
    for (const auto& sphere : scene.GetSphereObjects()) {
        auto intersection = GetIntersection(ray, sphere.sphere);
        check_intersection(intersection, sphere.material,
                           intersection ? intersection->GetNormal() : Vector());
    }
    return best_hit;
}

std::optional<Vector> InterpolateNormals(const Object& object, const Intersection& intersection,
                                         const Ray& ray) {
    const Vector* n0 = object.GetNormal(0);
    const Vector* n1 = object.GetNormal(1);
    const Vector* n2 = object.GetNormal(2);
    if (n0 == nullptr || n1 == nullptr || n2 == nullptr) {
        return std::nullopt;
    }
    Vector bar_coords = GetBarycentricCoords(object.polygon, intersection.GetPosition());
    Vector normal = bar_coords[0] * *n0 + bar_coords[1] * *n1 + bar_coords[2] * *n2;
    normal.Normalize();
    if (DotProduct(normal, ray.GetDirection()) > 0) {
        normal = (-1) * normal;
    }
    return normal;
}
