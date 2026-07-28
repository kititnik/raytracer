#include "object.hpp"

const Vector* Object::GetNormal(size_t index) const {
    auto& normal = normals.at(index);
    if (normal.has_value()) {
        return &normal.value();
    }
    return nullptr;
}
