#ifndef opengl_viewer_hpp
#define opengl_viewer_hpp

#pragma once

#include <GL/glew.h>
// #include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <opencv2/opencv.hpp>
#include <memory>

#include "opengl_shader.hpp"
#include "opengl_camera.hpp"
#include "stb_image.h"

#define STB_IMAGE_IMPLEMENTATION

typedef struct texture_config {
    unsigned int texture_num;
    std::string Types;
    std::string texture_name;
    int nums;
    cv::Mat image;
}Texture;

class OpenGLViewer
{
public:
    OpenGLViewer();
    ~OpenGLViewer();
    int RenderFrame(cv::Mat camera_image);
    void terminate();
    bool ShouldClose() const {return glfwWindowShouldClose(window_);};
private:
    int CreateWindow();
    int CreateGeometries();
    void CreateShader(std::string vertexPath, std::string fragmentPath);
    void Load_Texture(texture_config &texture);
    void Control_Texture();

    void on_scroll(GLFWwindow* window, double xoffset, double yoffset);
    void on_mouse(GLFWwindow* window, double xpos, double ypos);
    void on_window_resize(GLFWwindow* window, int width, int height);
    void ProcessInput();
    void ShowStereoCamera(bool show) { show_stereo_camera_=show;};
    std::unique_ptr<OpenGLCamera> camera_;
    Shader cam_render_shader_;
    unsigned int cam_render_texture_;
    unsigned int sub_render_texture_;
    Shader point_cloud_shader_;
    cv::Mat image;
    const GLuint window_width_;
    const GLuint window_height_;
    GLFWwindow* window_;
    bool first_mouse_;
    float last_x_;
    float last_y_;
    unsigned int camera_quad_vbo_, camera_quad_vao_, camera_quad_ebo_;
    unsigned int point_cloud_vao_,point_cloud_vbo_;
    float delta_time_;
    float last_frame_time_;
    bool show_stereo_camera_;
    bool load_shader_over;
    std::string decorate_texture = "null";
    Texture camera_texture={.texture_num=cam_render_texture_, 
                            .Types="RGB", 
                            .texture_name="vTexture", 
                            .nums=0};
    Texture sub_texture={.texture_num=sub_render_texture_, 
                            .Types="RGBA", 
                            .texture_name="vTexture", 
                            .nums=1};
};

#endif /* opengl_viewer_hpp */
