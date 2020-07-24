cmake_minimum_required(VERSION 3.10)
project(realtrender)


add_definitions("-std=c++11")
find_package(OpenCV REQUIRED)

include_directories(
        mnn/include
        src
)

#find_package(OpenGL REQUIRED)
#include_directories(${OPENGL_INCLUDE_DIR}) 

set(OPENGL_INCLUDE_DIR /usr/include/)
set(OPENGL_LIBRARYS /usr/lib/x86_64-linux-gnu)
include_directories(${OPENGL_INCLUDE_DIR}) 

#find_package(PkgConfig REQUIRED)
#pkg_check_modules(GLFW REQUIRED glfw3)

#find_package(glfw3 REQUIRED)
#include_directories(${GLFW_INCLUDE_DIR})

set(GLFW_INCLUDE_DIR /usr/local/include/)
set(GLFW_LIBRARYS /usr/local/lib/libglfw3.a)
include_directories(${glfw_INCLUDE_DIR}) 

#message(${OPENGL_LIBRARIES})
#message(${GLFW_LIBRARIES})

link_directories(mnn/lib)

add_executable(realtrender src/main.cpp src/opengl_viewer.cpp src/opengl_camera.cpp src/opengl_shader.cpp src/Pfld.cpp src/UltraFace.cpp)

target_link_libraries(realtrender ${OpenCV_LIBS} ${OPENGL_LIBRARYS} ${GLFW_LIBRARIES} MNN -lglfw3 -lGL -lGLEW -lm -lXrandr -lXi -lX11 -lXxf86vm -lpthread -ldl -lXinerama -lXcursor)