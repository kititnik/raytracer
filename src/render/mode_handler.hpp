#pragma once

#include <cstddef>
#include <optional>
#include "hit.hpp"
#include "image.hpp"
#include "intersection.hpp"
#include "ray.hpp"
#include "scene.hpp"
#include "vector.hpp"

std::optional<Hit> FindNearestIntersection(const Ray& ray, const Scene& scene);
std::optional<Vector> InterpolateNormals(const Object& object, const Intersection& intersection,
                                         const Ray& ray);

const double kEps = 1e-6;

class Renderer {
public:
    Renderer(int width, int height);
    virtual void SetPixel(int x, int y, const Ray& ray, const Scene& scene) = 0;
    virtual Image BuildImage() = 0;
    virtual ~Renderer() = default;

protected:
    Image image_;
};
