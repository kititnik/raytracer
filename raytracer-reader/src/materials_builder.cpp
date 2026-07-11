#include "materials_builder.h"
#include "line_parser.h"
#include "material.h"
#include "vector.h"
#include <string>

void MaterialsBuilder::StartMaterial(std::istringstream& ss) {
    if (is_processing_) {
        materials_[current_material_.name] = current_material_;
        is_processing_ = false;
    }
    current_material_ = Material{};
    current_material_.name = ParseMaterialName(ss);
    current_material_.albedo = Vector(1, 0, 0);
    current_material_.specular_exponent = 1;
    current_material_.refraction_index = 1;
    is_processing_ = true;
}

void MaterialsBuilder::SetAmbientColor(std::istringstream& ss) {
    current_material_.ambient_color = ParseVector(ss);
}

void MaterialsBuilder::SetDiffuseColor(std::istringstream& ss) {
    current_material_.diffuse_color = ParseVector(ss);
}

void MaterialsBuilder::SetSpecularColor(std::istringstream& ss) {
    current_material_.specular_color = ParseVector(ss);
}

void MaterialsBuilder::SetIntensity(std::istringstream& ss) {
    current_material_.intensity = ParseVector(ss);
}

void MaterialsBuilder::SetSpecularExponent(std::istringstream& ss) {
    current_material_.specular_exponent = ParseScalar(ss);
}

void MaterialsBuilder::SetRefractionIndex(std::istringstream& ss) {
    current_material_.refraction_index = ParseScalar(ss);
}

void MaterialsBuilder::SetAlbedo(std::istringstream& ss) {
    current_material_.albedo = ParseVector(ss);
}

std::unordered_map<std::string, Material> MaterialsBuilder::BuildMaterials() {
    if (is_processing_) {
        materials_[current_material_.name] = current_material_;
        is_processing_ = false;
    }
    return materials_;
}
