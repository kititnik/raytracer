#include "normal_renderer.hpp"

NormalRenderer::NormalRenderer(int width, int height) : Renderer(width, height) {
}

void NormalRenderer::SetPixel(int x, int y, const Ray& ray, const Scene& scene) {
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

Image NormalRenderer::BuildImage() {
    return std::move(image_);
}
