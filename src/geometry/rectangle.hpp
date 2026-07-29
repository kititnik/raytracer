#pragma once

#include "vector.hpp"

class Rectangle {
public:
    Rectangle(const Vector& min, const Vector& max);

    const Vector& GetMin() const;
    const Vector& GetMax() const;
private:
    Vector max_point_;
    Vector min_point_;
};
