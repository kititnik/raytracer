#pragma once

#include "intersection.h"
#include "material.h"

struct Hit {
    Intersection intersection;
    const Material* material;
    Vector normal;
};
