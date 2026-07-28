#pragma once

#include <array>
#include <cstddef>

class Vector {
public:
    Vector();
    Vector(double x, double y, double z);

    double& operator[](size_t ind);
    double operator[](size_t ind) const;

    void Normalize();

private:
    std::array<double, 3> data_;
};

Vector operator-(const Vector& a, const Vector& b);
Vector operator+(const Vector& a, const Vector& b);
Vector operator*(const double scale, const Vector& v);
double DotProduct(const Vector& a, const Vector& b);
Vector CrossProduct(const Vector& a, const Vector& b);
double Length(const Vector& v);
