#include "triangle.h"
#include "vector.h"

Triangle::Triangle(const Vector& a, const Vector& b, const Vector& c) : sides_{a, b, c} {
}

const Vector& Triangle::operator[](size_t ind) const {
    return sides_.at(ind);
}

double Triangle::Area() const {
    return Length(CrossProduct(sides_[1] - sides_[0], sides_[2] - sides_[0])) / 2;
}
