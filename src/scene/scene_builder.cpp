#include "scene_builder.hpp"
#include <cstddef>
#include <stdexcept>
#include <cstdint>
#include "line_parser.hpp"
#include "triangle.hpp"

size_t ResolveFaceTokenIndex(int idx, size_t arr_size);

SceneBuilder::SceneBuilder(const std::filesystem::path& path) {
    path_ = path;
}

Scene SceneBuilder::BuildScene() {
    Scene scene(std::move(objects_), std::move(sphere_objects_), std::move(lights_),
                std::move(materials_));
    return scene;
}

void SceneBuilder::AddVertex(std::istringstream& ss) {
    Vector vertex = ParseVector(ss);
    vertexes_.push_back(vertex);
}

void SceneBuilder::AddVertexNormals(std::istringstream& ss) {
    Vector vertex = ParseVector(ss);
    normals_.push_back(vertex);
}

void SceneBuilder::AddFace(std::istringstream& ss) {
    auto parsed_tokens = ParseFace(ss);
    if (parsed_tokens.size() < 3) {
        throw std::runtime_error("Face should have at least 3 vertices");
    }
    for (size_t i = 1; i + 1 < parsed_tokens.size(); i++) {
        Vector vertex_a =
            vertexes_[ResolveFaceTokenIndex(parsed_tokens[0].vertice_idx, vertexes_.size())];
        Vector vertex_b =
            vertexes_[ResolveFaceTokenIndex(parsed_tokens[i].vertice_idx, vertexes_.size())];
        Vector vertex_c =
            vertexes_[ResolveFaceTokenIndex(parsed_tokens[i + 1].vertice_idx, vertexes_.size())];
        Object obj{current_material_, Triangle(vertex_a, vertex_b, vertex_c), {}};
        if (parsed_tokens[0].normal_index) {
            obj.normals[0] = normals_[ResolveFaceTokenIndex(parsed_tokens[0].normal_index.value(),
                                                            normals_.size())];
        }
        if (parsed_tokens[i].normal_index) {
            obj.normals[1] = normals_[ResolveFaceTokenIndex(parsed_tokens[i].normal_index.value(),
                                                            normals_.size())];
        }
        if (parsed_tokens[i + 1].normal_index) {
            obj.normals[2] = normals_[ResolveFaceTokenIndex(
                parsed_tokens[i + 1].normal_index.value(), normals_.size())];
        }
        objects_.push_back(obj);
    }
}

void SceneBuilder::AddSphere(std::istringstream& ss) {
    Sphere sphere = ParseSphere(ss);
    sphere_objects_.emplace_back(current_material_, sphere);
}

void SceneBuilder::AddPointLight(std::istringstream& ss) {
    Light light = ParsePointLight(ss);
    lights_.emplace_back(light);
}

void SceneBuilder::AddMtLib(std::istringstream& ss) {
    std::string mtl_path = ParseMaterialPath(ss);
    auto mats = ReadMaterials(path_.parent_path().append(mtl_path));
    materials_.insert(mats.begin(), mats.end());
}

void SceneBuilder::AddUseMtl(std::istringstream& ss) {
    std::string mat_name = ParseMaterialName(ss);
    if (!materials_.contains(mat_name)) {
        throw std::runtime_error("No material found with name " + mat_name);
    }
    current_material_ = &materials_[mat_name];
}

size_t ResolveFaceTokenIndex(int idx, size_t arr_size) {
    int64_t resolved = 0;
    if (idx == 0) {
        throw std::runtime_error("Face index cannot be zero");
    } else if (idx > 0) {
        resolved = idx - 1;
    } else if (idx < 0) {
        resolved = static_cast<int64_t>(arr_size) + idx;
    }
    if (resolved < 0 || static_cast<size_t>(resolved) >= arr_size) {
        throw std::runtime_error("Face index out of range");
    }
    return static_cast<size_t>(resolved);
}
