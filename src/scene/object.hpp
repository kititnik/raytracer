#pragma once

#include <array>
#include <optional>
#include "triangle.hpp"
#include "material.hpp"
#include "sphere.hpp"
#include "vector.hpp"

struct Object {
    const Material* material = nullptr;
    Triangle polygon;
    std::array<std::optional<Vector>, 3> normals;

    const Vector* GetNormal(size_t index) const;
};

struct SphereObject {
    const Material* material = nullptr;
    Sphere sphere;
};
