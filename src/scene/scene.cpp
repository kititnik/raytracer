#include "scene.hpp"
#include <cstddef>
#include <exception>
#include <fstream>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include "scene_builder.hpp"
#include "materials_builder.hpp"

Scene::Scene(std::vector<Object> objects, std::vector<SphereObject> sphere_objects,
             std::vector<Light> lights, std::unordered_map<std::string, Material> materials)
    : objects_(std::move(objects)),
      sphere_objects_(std::move(sphere_objects)),
      lights_(std::move(lights)),
      materials_(std::move(materials)) {
}

const std::vector<Object>& Scene::GetObjects() const {
    return objects_;
}

const std::vector<SphereObject>& Scene::GetSphereObjects() const {
    return sphere_objects_;
}

const std::vector<Light>& Scene::GetLights() const {
    return lights_;
}

const std::unordered_map<std::string, Material>& Scene::GetMaterials() const {
    return materials_;
}

using KeywordHandler = std::function<void(SceneBuilder&, std::istringstream&)>;

const std::unordered_map<std::string, KeywordHandler> kKeywordsHandlers{
    {"v", [](SceneBuilder& b, std::istringstream& ss) { b.AddVertex(ss); }},
    {"vn", [](SceneBuilder& b, std::istringstream& ss) { b.AddVertexNormals(ss); }},
    {"f", [](SceneBuilder& b, std::istringstream& ss) { b.AddFace(ss); }},
    {"S", [](SceneBuilder& b, std::istringstream& ss) { b.AddSphere(ss); }},
    {"P", [](SceneBuilder& b, std::istringstream& ss) { b.AddPointLight(ss); }},
    {"mtllib", [](SceneBuilder& b, std::istringstream& ss) { b.AddMtLib(ss); }},
    {"usemtl", [](SceneBuilder& b, std::istringstream& ss) { b.AddUseMtl(ss); }}};

using MtlKeywordHandler = std::function<void(MaterialsBuilder&, std::istringstream&)>;

const std::unordered_map<std::string, MtlKeywordHandler> kMtlKeywordsHandlers{
    {"newmtl", [](MaterialsBuilder& b, std::istringstream& ss) { b.StartMaterial(ss); }},
    {"Ka", [](MaterialsBuilder& b, std::istringstream& ss) { b.SetAmbientColor(ss); }},
    {"Kd", [](MaterialsBuilder& b, std::istringstream& ss) { b.SetDiffuseColor(ss); }},
    {"Ks", [](MaterialsBuilder& b, std::istringstream& ss) { b.SetSpecularColor(ss); }},
    {"Ke", [](MaterialsBuilder& b, std::istringstream& ss) { b.SetIntensity(ss); }},
    {"Ns", [](MaterialsBuilder& b, std::istringstream& ss) { b.SetSpecularExponent(ss); }},
    {"Ni", [](MaterialsBuilder& b, std::istringstream& ss) { b.SetRefractionIndex(ss); }},
    {"al", [](MaterialsBuilder& b, std::istringstream& ss) { b.SetAlbedo(ss); }}};

Scene ReadScene(const std::filesystem::path& path) {
    SceneBuilder scene_builder(path);
    std::ifstream file_stream(path);
    if (!file_stream.is_open()) {
        throw std::runtime_error("Error opening file");
    }
    std::string line;
    int line_number = 0;
    while (getline(file_stream, line)) {
        line_number++;
        std::istringstream ss(line);
        std::string keyword;
        ss >> keyword;
        if (keyword.empty() || keyword[0] == '#') {
            continue;
        }
        auto it = kKeywordsHandlers.find(keyword);
        if (it == kKeywordsHandlers.end()) {
            continue;
        }
        try {
            it->second(scene_builder, ss);
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string(e.what()) + " At line: " +
                                     std::to_string(line_number) + " In file: " + path.string());
        }
    }
    Scene scene = scene_builder.BuildScene();
    file_stream.close();
    return scene;
}

std::unordered_map<std::string, Material> ReadMaterials(const std::filesystem::path& path) {
    MaterialsBuilder material_builder;
    std::ifstream file_stream(path);
    if (!file_stream.is_open()) {
        throw std::runtime_error("Error opening file");
    }
    std::string line;
    int line_number = 0;
    while (getline(file_stream, line)) {
        line_number++;
        std::istringstream ss(line);
        std::string keyword;
        ss >> keyword;
        if (keyword.empty() || keyword[0] == '#') {
            continue;
        }
        auto it = kMtlKeywordsHandlers.find(keyword);
        if (it == kMtlKeywordsHandlers.end()) {
            continue;
        }
        try {
            it->second(material_builder, ss);
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string(e.what()) + " At line: " +
                                     std::to_string(line_number) + " In file: " + path.string());
        }
    }
    std::unordered_map<std::string, Material> materials = material_builder.BuildMaterials();
    file_stream.close();
    return materials;
}
