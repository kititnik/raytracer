#pragma once

#include "depth_renderer.h"
#include "full_renderer.h"
#include "normal_renderer.h"
#include "ray.h"
#include "options/camera_options.h"
#include "options/render_options.h"
#include "image.h"
#include "scene.h"
#include "vector.h"
#include "mode_handler.h"

#include <cmath>
#include <cstddef>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <tuple>

std::tuple<Vector, Vector, Vector> LookAt(const CameraOptions& camera_options);
Vector RasterToCameraPixel(double x, double y, const CameraOptions& camera_options,
                           double aspect_ratio);

Image Render(const std::filesystem::path& path, const CameraOptions& camera_options,
             const RenderOptions& render_options) {
    Scene scene = ReadScene(path);
    auto [right, up, forward] = LookAt(camera_options);

    std::unique_ptr<Renderer> renderer = nullptr;
    if (render_options.mode == RenderMode::kNormal) {
        renderer = std::make_unique<NormalRenderer>(camera_options.screen_width,
                                                    camera_options.screen_height);
    } else if (render_options.mode == RenderMode::kDepth) {
        renderer = std::make_unique<DepthRenderer>(camera_options.screen_width,
                                                   camera_options.screen_height);
    } else if (render_options.mode == RenderMode::kFull) {
        renderer = std::make_unique<FullRenderer>(
            camera_options.screen_width, camera_options.screen_height, render_options.depth);
    } else {
        throw std::runtime_error("There is no such render mode. Expected kNormal/kDepth/kFull");
    }

    double aspect_ratio =
        static_cast<double>(camera_options.screen_width) / camera_options.screen_height;
    for (int y = 0; y < camera_options.screen_height; y++) {
        for (int x = 0; x < camera_options.screen_width; x++) {
            Vector pixel_pos = RasterToCameraPixel(x, y, camera_options, aspect_ratio);

            Vector ray_direction =
                pixel_pos[0] * right + pixel_pos[1] * up + pixel_pos[2] * forward;
            Ray ray = Ray(camera_options.look_from, ray_direction);

            renderer->SetPixel(x, y, ray, scene);
        }
    }
    return renderer->BuildImage();
}

Vector RasterToCameraPixel(double x, double y, const CameraOptions& camera_options,
                           double aspect_ratio) {
    double pixel_ndc_x = (x + 0.5) / camera_options.screen_width;
    double pixel_ndc_y = (y + 0.5) / camera_options.screen_height;
    double pixel_screen_x = 2 * pixel_ndc_x - 1;
    double pixel_screen_y = 1 - 2 * pixel_ndc_y;
    double pixel_camera_x = pixel_screen_x * aspect_ratio * std::tan(camera_options.fov / 2);
    double pixel_camera_y = pixel_screen_y * std::tan(camera_options.fov / 2);
    double pixel_camera_z = -1;
    return Vector(pixel_camera_x, pixel_camera_y, pixel_camera_z);
}

std::tuple<Vector, Vector, Vector> LookAt(const CameraOptions& camera_options) {
    Vector forward = camera_options.look_from - camera_options.look_to;
    forward.Normalize();
    Vector right = CrossProduct(Vector(0, 1, 0), forward);
    if (Length(right) < 1e-9) {
        right = Vector(1, 0, 0);
    }
    right.Normalize();
    Vector up = CrossProduct(forward, right);
    return std::make_tuple(right, up, forward);
}
