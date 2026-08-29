#pragma once

#include "triangle.hpp"
#include "vector.hpp"

class Box {
public:
    Box(const Vector& min, const Vector& max);
    // Box with infinite vectors
    static Box Empty();
    double SurfaceArea() const;
    void Extend(const Box& other);
    void Extend(const Triangle& triangle);
    void Extend(const Vector& point);
    const Vector& GetMin() const;
    const Vector& GetMax() const;
private:
    Vector max_point_;
    Vector min_point_;
};
