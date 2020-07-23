#ifndef opengl_shader_hpp
#define opengl_shader_hpp

// #pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>


class Shader
{
public:
    unsigned int ID;
    Shader();
    std::string read_shader_file(const char* FilePath);
    // void init(const char* vertexPath, const char* fragmentPath);
    int init(std::string vertexPath, std::string fragmentPath);
    void use();
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec4f(const std::string &name, float x, float y, float z, float w) const;
    void setMat4f(const std::string &name, float* mat) const;
private:
    void checkCompileErrors(unsigned int shader, std::string type);
    std::string vertexCode;
    std::string fragmentCode;
    std::string init_vertexPath;
    std::string init_fragmentPath;
};


#endif /* opengl_shader_hpp */
