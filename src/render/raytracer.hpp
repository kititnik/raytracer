#pragma once

#include "options/camera_options.hpp"
#include "options/render_options.hpp"
#include "image.hpp"
#include "vector.hpp"
#include "scene.hpp"

#include <tuple>

std::tuple<Vector, Vector, Vector> LookAt(const CameraOptions& camera_options);
Vector RasterToCameraPixel(double x, double y, const CameraOptions& camera_options,
                           double aspect_ratio);
Image Render(const Scene& scene, const CameraOptions& camera_options,
             const RenderOptions& render_options);
