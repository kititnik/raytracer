#include "box.hpp"
#include "vector.hpp"
#include <algorithm>

Box::Box(const Vector& min, const Vector& max) : min_point_(min), max_point_(max) {}

static Box Empty() {
    return Box(Vector(std::numeric_limits<double>::infinity(), 
        std::numeric_limits<double>::infinity(), 
        std::numeric_limits<double>::infinity()),
    Vector(-std::numeric_limits<double>::infinity(), 
        -std::numeric_limits<double>::infinity(), 
        -std::numeric_limits<double>::infinity()));
}

double Box::SurfaceArea() const {
    Vector d = max_point_ - min_point_;
    return 2.0 * (d[0] * d[1] + d[1] * d[2] + d[2] * d[0]);
}

void Box::Extend(const Box& other) {
    for (size_t axis = 0; axis < 3; ++axis) {
        min_point_[axis] = std::min(min_point_[axis], other.GetMin()[axis]);
        max_point_[axis] = std::max(max_point_[axis], other.GetMax()[axis]);
    }
}

void Box::Extend(const Triangle& triangle) {
    for (size_t v = 0; v < 3; ++v) {
        for (size_t axis = 0; axis < 3; ++axis) {
            min_point_[axis] = std::min(min_point_[axis], triangle[v][axis]);
            max_point_[axis] = std::max(max_point_[axis], triangle[v][axis]);
        }
    }
}

void Box::Extend(const Vector& point) {
    for (size_t axis = 0; axis < 3; ++axis) {
        min_point_[axis] = std::min(min_point_[axis], point[axis]);
        max_point_[axis] = std::max(max_point_[axis], point[axis]);
    }
}

const Vector& Box::GetMin() const {
    return min_point_;
}

const Vector& Box::GetMax() const {
    return max_point_;
}
