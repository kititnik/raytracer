#include "scene.h"
#include <cstddef>
#include <exception>
#include <fstream>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include "scene_builder.h"

struct FaceToken;
using KeywordHandler = std::function<void(SceneBuilder&, std::istringstream&)>;

std::unordered_map<std::string, KeywordHandler> keywords_handlers{
    {"v", [](SceneBuilder& b, std::istringstream& ss) { b.AddVertex(ss); }},
    {"vn", [](SceneBuilder& b, std::istringstream& ss) { b.AddVertexNormals(ss); }},
    {"f", [](SceneBuilder& b, std::istringstream& ss) { b.AddFace(ss); }},
    {"S", [](SceneBuilder& b, std::istringstream& ss) { b.AddSphere(ss); }},
    {"P", [](SceneBuilder& b, std::istringstream& ss) { b.AddPointLight(ss); }},
    {"mtllib", [](SceneBuilder& b, std::istringstream& ss) { b.AddMtLib(ss); }},
    {"usemtl", [](SceneBuilder& b, std::istringstream& ss) { b.AddUseMtl(ss); }}};

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
        if (keyword[0] == '#') {
            continue;
        }
        auto it = keywords_handlers.find(keyword);
        if (it == keywords_handlers.end()) {
            continue;
        }
        try {
            it->second(scene_builder, ss);
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string(e.what()) +
                                     " At line: " + std::to_string(line_number));
        }
    }
    Scene scene = scene_builder.BuildScene();
    return scene;
}
