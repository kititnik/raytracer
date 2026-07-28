#pragma once

#include "intersection.hpp"
#include "material.hpp"

struct Hit {
    Intersection intersection;
    const Material* material;
    Vector normal;
};
