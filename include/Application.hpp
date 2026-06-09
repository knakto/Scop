#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "ObjectData.hpp"
#include "RotationMatrix.hpp"
#include <cstdio>
#include <stdexcept>
#include <iostream>
#include <cstddef>
#include <cmath>

#ifndef MYSHADER
# define MYSHADER
# define VERTEXT_SRC R"(#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 0) out vec3 oColor;
uniform mat4 matrix;
void main()
{
   gl_Position = matrix * vec4(aPos, 1.0);
   oColor = aColor;
})"
# define FRAGMENT_SRC R"(#version 450 core
layout (location = 0) in vec3 color;
out vec4 FragColor;
void main()
{
   FragColor = vec4(color, 1.0f);
})"
#endif

class Application{
private:
  GLFWwindow *_window;
  const char *_vertexShaderSource;
  const char *_fragmentShaderSource;
  unsigned int _shaderProgram;
  GLuint _vao; // vertex array object
  GLuint _vbo; // vertex buffer object
  GLuint _ebo; // element buffer object
  GLuint _cbo;

  void initWindow(void);
  void initGL(ObjectData* obj);
  void mainloop(ObjectData* obj);
  void clean(void);
  void setupVAO(ObjectData* obj);
public:
  const char  *_name;
  std::size_t _width;
  std::size_t _height;

  Application(void)
  : _vertexShaderSource(VERTEXT_SRC), _fragmentShaderSource(FRAGMENT_SRC), _name("Application"), _width(1920), _height(1080) {}
  Application(std::string name, std::size_t width, std::size_t height)
  : _vertexShaderSource(VERTEXT_SRC), _fragmentShaderSource(FRAGMENT_SRC), _name(name.c_str()), _width(width), _height(height) {}
  Application(std::string name, std::size_t width, std::size_t height, const char *vertextSrc, const char *fragmentSrc)
  : _vertexShaderSource(vertextSrc), _fragmentShaderSource(fragmentSrc), _name(name.c_str()), _width(width), _height(height) {}

  void run(ObjectData* obj){
    initWindow();
    initGL(obj);
    mainloop(obj);
    clean();
  }
};
