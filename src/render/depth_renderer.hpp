#pragma once

#include "mode_handler.hpp"

class DepthRenderer : public Renderer {
public:
    DepthRenderer(int width, int height);
    void SetPixel(int x, int y, const Ray& ray, const Scene& scene);
    Image BuildImage();

private:
    std::vector<std::optional<double>> buffer_;
    double max_distance_ = 0;
};
