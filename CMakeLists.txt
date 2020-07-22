cmake_minimum_required(VERSION 3.10)
project(realtrender)

add_definitions("-std=c++11")
find_package(OpenCV REQUIRED)

#include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
#conan_basic_setup()
include_directories(
        src
)

add_executable(realtrender src/main.cpp src/opengl_viewer.cpp src/opengl_camera.cpp src/opengl_shader.cpp  shaders/camera_render.vs shaders/camera_render.fs)

target_link_libraries(realtrender ${OpenCV_LIBS})
