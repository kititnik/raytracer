#pragma once

#include "mode_handler.hpp"
#include "scene.hpp"
#include "vector.hpp"

Vector operator*(const Vector& rgb1, const Vector& rgb2);
Vector operator/(const Vector& rgb1, const Vector& rgb2);
Vector operator+(double scalar, const Vector& vec);
Vector CalculateIllumination(const Ray& ray, const Hit& hit, const Scene& scene);
Vector TraceRay(const Scene& scene, const Ray& ray, int depth, bool inside);

class FullRenderer : public Renderer {
public:
    FullRenderer(int width, int height, int depth);
    void SetPixel(int x, int y, const Ray& ray, const Scene& scene);
    Image BuildImage();

private:
    std::vector<std::optional<Vector>> buffer_;
    double max_scalar_ = 0;
    int depth_;
};
