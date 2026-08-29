#include "geometry.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <optional>
#include "intersection.hpp"
#include "triangle.hpp"
#include "vector.hpp"

std::optional<Intersection> GetIntersection(const Ray& ray, const Box& box) {
    double t_near = -std::numeric_limits<double>::infinity();
    double t_far = std::numeric_limits<double>::infinity();

    for (size_t i = 0; i < 3; ++i) {
        double origin = ray.GetOrigin()[i];
        double dir = ray.GetDirection()[i];

        if (std::fabs(dir) < 1e-9) {
            if (origin < box.GetMin()[i] || origin > box.GetMax()[i]) {
                return std::nullopt;
            }
            continue;
        }

        double t1 = (box.GetMin()[i] - origin) / dir;
        double t2 = (box.GetMax()[i] - origin) / dir;
        if (t1 > t2) {
            std::swap(t1, t2);
        }
        t_near = std::max(t_near, t1);
        t_far = std::min(t_far, t2);
        if (t_near > t_far) {
            return std::nullopt;
        }
    }
    if(t_far < 0) {
        return std::nullopt;
    }
    Vector position = ray.GetOrigin() + t_near * ray.GetDirection();
    return Intersection(position, Vector(), t_near);
}

std::optional<Intersection> GetIntersection(const Ray& ray, const Sphere& sphere) {
    // Substitute the ray equation into the sphere equation and get a quadratic equation
    // t^2 + 2*t*(direction * (origin - center)) + (origin - center)^2 - radius^2 = 0
    // a = 1; b = 2*(direction * (origin - center)); c = (origin - center)^2 - radius^2
    Vector origin_center = ray.GetOrigin() - sphere.GetCenter();
    double b = 2 * DotProduct(ray.GetDirection(), origin_center);
    double c = DotProduct(origin_center, origin_center) - sphere.GetRadius() * sphere.GetRadius();
    double discriminant = b * b - 4 * c;
    if (discriminant < 0) {
        return std::nullopt;
    }
    double t1 = (-b + sqrt(discriminant)) / 2;
    double t2 = (-b - sqrt(discriminant)) / 2;
    double distance = 0;
    if (t2 >= 0) {
        distance = t2;
    } else if (t1 >= 0) {
        distance = t1;
    } else {
        return std::nullopt;
    }
    Vector position = ray.GetOrigin() + distance * ray.GetDirection();
    Vector normal = position - sphere.GetCenter();
    normal.Normalize();
    if (DotProduct(normal, ray.GetDirection()) > 0) {
        normal = (-1) * normal;
    }
    return Intersection(position, normal, distance);
}

std::optional<Intersection> GetIntersection(const Ray& ray, const Triangle& triangle) {
    Vector side_b = triangle[1] - triangle[0];
    Vector side_c = triangle[2] - triangle[0];

    Vector pvec = CrossProduct(ray.GetDirection(), side_c);
    double det = DotProduct(side_b, pvec);

    if (std::fabs(det) < 1e-9) {
        return std::nullopt;
    }

    double inv_det = 1 / det;
    Vector tvec = ray.GetOrigin() - triangle[0];
    double u = DotProduct(tvec, pvec) * inv_det;
    if (u < 0 || u > 1) {
        return std::nullopt;
    }

    Vector qvec = CrossProduct(tvec, side_b);
    double v = DotProduct(ray.GetDirection(), qvec) * inv_det;
    if (v < 0 || u + v > 1) {
        return std::nullopt;
    }
    double distance = DotProduct(side_c, qvec) * inv_det;
    if (distance < 0) {
        return std::nullopt;
    }
    Vector position = ray.GetOrigin() + distance * ray.GetDirection();
    Vector normal = CrossProduct(side_b, side_c);
    normal.Normalize();
    if (DotProduct(normal, ray.GetDirection()) > 0) {
        normal = (-1) * normal;
    }
    return Intersection(position, normal, distance);
}

Vector Reflect(const Vector& ray, const Vector& normal) {
    return ray - 2.0 * DotProduct(normal, ray) * normal;
}

std::optional<Vector> Refract(const Vector& ray, const Vector& normal, double eta) {
    double k = 1.0 - eta * eta * (1.0 - DotProduct(normal, ray) * DotProduct(normal, ray));
    if (k < 0.0) {
        return std::nullopt;
    } else {
        return eta * ray - (eta * DotProduct(normal, ray) + sqrt(k)) * normal;
    }
}

Vector GetBarycentricCoords(const Triangle& triangle, const Vector& point) {
    Triangle triangle_pbc(point, triangle[1], triangle[2]);
    Triangle triangle_apc(triangle[0], point, triangle[2]);
    Triangle triangle_abp(triangle[0], triangle[1], point);
    double area_abc = triangle.Area();
    double area_pbc = triangle_pbc.Area();
    double area_apc = triangle_apc.Area();
    double area_abp = triangle_abp.Area();
    double x = area_pbc / area_abc;
    double y = area_apc / area_abc;
    double z = area_abp / area_abc;
    return Vector(x, y, z);
}
