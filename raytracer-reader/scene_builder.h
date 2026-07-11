#pragma once

#include <filesystem>
#include <sstream>
#include <unordered_map>
#include <vector>
#include "light.h"
#include "object.h"
#include "scene.h"
#include "../raytracer-geom/vector.h"

class SceneBuilder {
public:
    SceneBuilder(const std::filesystem::path& path);
    void AddVertex(std::istringstream& ss);
    void AddVertexNormals(std::istringstream& ss);
    void AddFace(std::istringstream& ss);
    void AddSphere(std::istringstream& ss);
    void AddPointLight(std::istringstream& ss);
    void AddMtLib(std::istringstream& ss);
    void AddUseMtl(std::istringstream& ss);

    Scene BuildScene();

private:
    std::filesystem::path path_;
    std::vector<Vector> vertexes_;
    std::vector<Vector> normals_;
    std::vector<Object> objects_;
    std::vector<SphereObject> sphere_objects_;
    std::vector<Light> lights_;
    Material* current_material_ = nullptr;
    std::unordered_map<std::string, Material> materials_;
};
