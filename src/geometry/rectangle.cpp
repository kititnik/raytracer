#include "rectangle.hpp"

Rectangle::Rectangle(const Vector& min, const Vector& max) : min_point_(min), max_point_(max) {}

const Vector& Rectangle::GetMin() const {
    return min_point_;
}

const Vector& Rectangle::GetMax() const {
    return max_point_;
}
