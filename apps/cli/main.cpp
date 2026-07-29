#include "raytracer.hpp"

#include <getopt.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

static void PrintHelp(const char* program);
static Vector ParseVec3(const char* arg);
static RenderMode ParseRenderMode(const char* arg);

int main(int argc, char* argv[]) {
    std::filesystem::path input;
    std::filesystem::path output;
    CameraOptions camera{};
    RenderOptions render{};
    render.depth = 5;

    static const option long_options[] = {
        {"input",     required_argument, nullptr, 'i'},
        {"output",    required_argument, nullptr, 'o'},
        {"width",     required_argument, nullptr, 'w'},
        {"height",    required_argument, nullptr, 'h'},
        {"mode",      required_argument, nullptr, 'm'},
        {"depth",     required_argument, nullptr, 'd'},
        {"look-from", required_argument, nullptr,  2 },
        {"look-to",   required_argument, nullptr,  3 },
        {"help",      no_argument,       nullptr,  1 },
        {nullptr,     0,                 nullptr,  0 },
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "i:o:w:h:m:d:", long_options, nullptr)) != -1) {
        switch (opt) {
            case 'i': input = optarg; break;
            case 'o': output = optarg; break;
            case 'w': camera.screen_width = std::stoi(optarg); break;
            case 'h': camera.screen_height = std::stoi(optarg); break;
            case 'm': ParseRenderMode(optarg); break;
            case 'd': render.depth = std::stoi(optarg); break;
            case 2:   camera.look_from = ParseVec3(optarg); break;
            case 3:   camera.look_to   = ParseVec3(optarg); break;
            case 1:   PrintHelp(argv[0]); return 0;
            case '?': PrintHelp(argv[0]); return 1;
        }
    }

    if (input.empty() || output.empty() || !camera.screen_width || !camera.screen_height) {
        std::cerr << "Error: --input, --output, --width and --height are required\n";
        return 1;
    }

    try {
        Image image = Render(input, camera, render);
        image.WriteImage(output);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

static Vector ParseVec3(const char* arg) {
    std::string s = arg;
    size_t first = s.find(',');
    size_t second = s.find(',', first + 1);
    if (first == std::string::npos || second == std::string::npos) {
        throw std::runtime_error(std::string("Invalid vec3 format (expected x,y,z): ") + arg);
    }
    double x = std::stod(s.substr(0, first));
    double y = std::stod(s.substr(first + 1, second - first - 1));
    double z = std::stod(s.substr(second + 1));
    return {x, y, z};
}

static RenderMode ParseRenderMode(const char* arg) {
    std::string_view mode = optarg;
    if (mode == "depth") {
        return RenderMode::kDepth;
    }
    if (mode == "normal") {
        return RenderMode::kNormal;
    }
    if (mode == "full") {
        return RenderMode::kFull;
    }
    std::cerr << "Unknown mode: " << mode << "\n";
    exit(1);
}

static void PrintHelp(const char* program) {
    std::cout << "Usage: " << program << " [OPTIONS]\n\n"
              << "Required:\n"
              << "  -i, --input  <path>              Input .obj file\n"
              << "  -o, --output <path>              Output .png file\n"
              << "  -w, --width  <int>               Image width in pixels\n"
              << "  -h, --height <int>               Image height in pixels\n\n"
              << "Optional:\n"
              << "  -m, --mode      depth|normal|full   Render mode (default: full)\n"
              << "  -d, --depth     <int>               Ray recursion depth (default: 5)\n"
              << "      --look-from <x,y,z>             Camera position (default: 0,0,0)\n"
              << "      --look-to   <x,y,z>             Camera target (default: 0,0,-1)\n"
              << "      --help                          Show this help\n";
}
