#pragma once

#include <unordered_map>
#include <sstream>
#include "material.h"

class MaterialsBuilder {
public:
    void StartMaterial(std::istringstream& ss);
    void SetAmbientColor(std::istringstream& ss);
    void SetDiffuseColor(std::istringstream& ss);
    void SetSpecularColor(std::istringstream& ss);
    void SetIntensity(std::istringstream& ss);
    void SetSpecularExponent(std::istringstream& ss);
    void SetRefractionIndex(std::istringstream& ss);
    void SetAlbedo(std::istringstream& ss);

    std::unordered_map<std::string, Material> BuildMaterials();

private:
    Material current_material_;
    bool is_processing_ = false;
    std::unordered_map<std::string, Material> materials_;
};
