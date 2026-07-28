#include "vector.hpp"
#include <cmath>
#include <format>
#include <stdexcept>

Vector::Vector() : Vector(0, 0, 0) {
}

Vector::Vector(double x, double y, double z) : data_{x, y, z} {
}

double& Vector::operator[](size_t ind) {
    return data_.at(ind);
}

double Vector::operator[](size_t ind) const {
    return data_.at(ind);
}

void Vector::Normalize() {
    double len = Length(*this);
    if (std::fabs(len) < 1e-9) {
        throw std::runtime_error(
            std::format("Cannot normalize vector: vector length = 0. {}:{}", __FILE__, __LINE__));
    }
    data_[0] /= len;
    data_[1] /= len;
    data_[2] /= len;
}

double DotProduct(const Vector& a, const Vector& b) {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

Vector CrossProduct(const Vector& a, const Vector& b) {
    double x = a[1] * b[2] - a[2] * b[1];
    double y = a[2] * b[0] - a[0] * b[2];
    double z = a[0] * b[1] - a[1] * b[0];
    return Vector(x, y, z);
}

double Length(const Vector& v) {
    return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

Vector operator-(const Vector& a, const Vector& b) {
    return Vector(a[0] - b[0], a[1] - b[1], a[2] - b[2]);
}

Vector operator+(const Vector& a, const Vector& b) {
    return Vector(a[0] + b[0], a[1] + b[1], a[2] + b[2]);
}

Vector operator*(const double scale, const Vector& v) {
    return Vector(v[0] * scale, v[1] * scale, v[2] * scale);
}
