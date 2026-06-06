#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <iostream>
#include <cstddef>
#include <cmath>

#ifndef MYSHADER
# define MYSHADER
# define VERTEXT_SRC \
"#version 330 core\n" \
"layout (location = 0) in vec3 aPos;\n" \
"uniform mat4 matrix;" \
"void main()\n" \
"{\n" \
"   gl_Position = matrix * vec4(aPos, 1.0);\n" \
"}\0"
# define FRAGMENT_SRC \
"#version 330 core\n" \
"out vec4 FragColor;\n" \
"void main()\n" \
"{\n" \
"   FragColor = vec4(1.0f);\n" \
"}\0"
#endif

class Application{
private:
  GLFWwindow *_window;
  const char *_vertexShaderSource;
  const char *_fragmentShaderSource;
  unsigned int _shaderProgram;
  GLuint _vao;
  GLuint _vbo;

  void initWindow(void);
  void initGL(void);
  void mainloop(void);
  void clean(void);
  void setupVAO(void);
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

  void run(void){
    initWindow();
    initGL();
    mainloop();
    clean();
  }
};
