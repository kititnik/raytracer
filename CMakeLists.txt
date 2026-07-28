cmake_minimum_required(VERSION 3.10)
project(raytracer)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/compile_commands.json
)

add_executable(raytracer
    src/geometry/geometry.cpp
    src/geometry/intersection.cpp
    src/geometry/ray.cpp
    src/geometry/sphere.cpp
    src/geometry/triangle.cpp
    src/geometry/vector.cpp
    src/scene/line_parser.cpp
    src/scene/materials_builder.cpp
    src/scene/object.cpp
    src/scene/scene.cpp
    src/scene/scene_builder.cpp
    src/render/mode_handler.cpp
    src/render/full_renderer.cpp
    src/render/normal_renderer.cpp
    src/render/raytracer.cpp
    apps/cli/main.cpp
)

target_include_directories(raytracer PRIVATE
    src/geometry
    src/scene
    src/render
    src/render/options
    src/image
)
