#include "line_parser.h"
#include <format>
#include <string>
#include "light.h"
#include "sphere.h"
#include "vector.h"

void TryStream(std::istringstream& ss, std::string error_msg, std::string file, int line);
FaceToken ParseFaceToken(const std::string& token);

Vector ParseVector(std::istringstream& ss) {
    double x, y, z;
    ss >> x >> y >> z;
    TryStream(ss, "Error parsing vector", __FILE__, __LINE__);
    return Vector(x, y, z);
}

Sphere ParseSphere(std::istringstream& ss) {
    double x, y, z, r;
    ss >> x >> y >> z >> r;
    TryStream(ss, "Error parsing sphere", __FILE__, __LINE__);
    return Sphere(Vector(x, y, z), r);
}

Light ParsePointLight(std::istringstream& ss) {
    double x, y, z, r, g, b;
    ss >> x >> y >> z >> r >> g >> b;
    TryStream(ss, "Error parsing light", __FILE__, __LINE__);
    return Light(Vector(x, y, z), Vector(r, g, b));
}

std::vector<FaceToken> ParseFace(std::istringstream& ss) {
    std::string token;
    std::vector<FaceToken> parsed_tokens;
    while (ss >> token) {
        auto indices = ParseFaceToken(token);
        parsed_tokens.push_back(indices);
    }
    return parsed_tokens;
}

std::string ParseMaterialPath(std::istringstream& ss) {
    std::string material_path;
    ss >> material_path;
    TryStream(ss, "Error parsing mtlib", __FILE__, __LINE__);
    return material_path;
}

std::string ParseMaterialName(std::istringstream& ss) {
    std::string material_name;
    ss >> material_name;
    TryStream(ss, "Error parsing material name", __FILE__, __LINE__);
    return material_name;
}

double ParseScalar(std::istringstream& ss) {
    double value = 0;
    ss >> value;
    TryStream(ss, "Error parsing scalar", __FILE__, __LINE__);
    return value;
}

void TryStream(std::istringstream& ss, std::string error_msg, std::string file, int line) {
    if (ss.fail()) {
        throw std::runtime_error(std::format("{} {}:{}", error_msg, file, line));
    }
}

FaceToken ParseFaceToken(const std::string& token) {
    int v = 0;
    int vt = 0;
    int vn = 0;
    if (sscanf(token.c_str(), "%d/%d/%d", &v, &vt, &vn) == 3) {
        return FaceToken{v, vn};
    } else if (sscanf(token.c_str(), "%d//%d", &v, &vn) == 2) {
        return FaceToken{v, vn};
    } else if (sscanf(token.c_str(), "%d/%d", &v, &vt) == 2) {
        return FaceToken{v, std::nullopt};
    } else if (sscanf(token.c_str(), "%d", &v) == 1) {
        return FaceToken{v, std::nullopt};
    } else {
        throw std::runtime_error("Invalid face token: " + token);
    }
}
