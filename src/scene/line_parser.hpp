#pragma once

#include <optional>
#include <sstream>
#include <string>
#include <vector>
#include "light.hpp"
#include "sphere.hpp"
#include "vector.hpp"

struct FaceToken {
    int vertice_idx;
    std::optional<int> normal_index;
};

Vector ParseVector(std::istringstream& ss);
Sphere ParseSphere(std::istringstream& ss);
Light ParsePointLight(std::istringstream& ss);
std::vector<FaceToken> ParseFace(std::istringstream& ss);
std::string ParseMaterialPath(std::istringstream& ss);
std::string ParseMaterialName(std::istringstream& ss);
double ParseScalar(std::istringstream& ss);
