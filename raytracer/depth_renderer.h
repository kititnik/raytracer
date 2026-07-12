#pragma once

#include "mode_handler.h"

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
