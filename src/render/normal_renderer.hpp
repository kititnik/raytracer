#pragma once

#include "mode_handler.hpp"

class NormalRenderer : public Renderer {
public:
    NormalRenderer(int width, int height);
    void SetPixel(int x, int y, const Ray& ray, const Scene& scene);
    Image BuildImage();
};
