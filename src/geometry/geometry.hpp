#pragma once

#include "box.hpp"
#include "vector.hpp"
#include "sphere.hpp"
#include "intersection.hpp"
#include "triangle.hpp"
#include "ray.hpp"

#include <optional>

std::optional<Intersection> GetIntersection(const Ray& ray, const Sphere& sphere);
std::optional<Intersection> GetIntersection(const Ray& ray, const Triangle& triangle);
std::optional<Intersection> GetIntersection(const Ray& ray, const Box& box);

Vector Reflect(const Vector& ray, const Vector& normal);
std::optional<Vector> Refract(const Vector& ray, const Vector& normal, double eta);
Vector GetBarycentricCoords(const Triangle& triangle, const Vector& point);
