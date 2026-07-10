#pragma once

#include <array>
#include <optional>
#include "triangle.h"
#include "material.h"
#include "sphere.h"
#include "vector.h"

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
