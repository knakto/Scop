#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Object.hpp"
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
layout (location = 2) in vec2 aTex;
layout (location = 3) in vec3 aNormal;

out vec2 TexCoord;
out vec3 Normal;
uniform mat4 matrix;
void main()
{
    gl_Position = matrix * vec4(aPos, 1.0);
    TexCoord = aTex;
    Normal = mat3(matrix) * aNormal;
})"
# define FRAGMENT_SRC R"(#version 450 core
in vec2 TexCoord;
in vec3 Normal;
out vec4 FragColor;
uniform sampler2D texture_image;
uniform int mode;
uniform vec3 lightPos;
uniform vec3 objectColor;
void main()
{
  if (mode == 1)
    FragColor = vec4(TexCoord.x, TexCoord.y, 0.0, 1.0);
  else if (mode == 2)
    FragColor = texture(texture_image, TexCoord);
  else if (mode == 3)
  {
    vec3 normalColor = normalize(Normal) * 0.5 + 0.5;
    FragColor = vec4(normalColor, 1.0);
  }
  else if (mode == 4)
  {
    vec3 lightDir = normalize(lightPos); 
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 finalColor = objectColor * (diff + 0.2);
    FragColor = vec4(finalColor, 1.0);
  }
  else
  {
    int faceIndex = gl_PrimitiveID;
    int colorMod = faceIndex % 10;
    if (colorMod == 0)
      FragColor = vec4(0.1, 0.1, 0.1, 1);
    else if (colorMod == 1)
      FragColor = vec4(0.2, 0.2, 0.2, 1);
    else if (colorMod == 2)
      FragColor = vec4(0.3, 0.3, 0.3, 1);
    else if (colorMod == 3)
      FragColor = vec4(0.4, 0.4, 0.4, 1);
    else if (colorMod == 4)
      FragColor = vec4(0.5, 0.5, 0.5, 1);
    else if (colorMod == 5)
      FragColor = vec4(0.6, 0.6, 0.6, 1);
    else if (colorMod == 6)
      FragColor = vec4(0.7, 0.7, 0.7, 1);
    else if (colorMod == 7)
      FragColor = vec4(0.8, 0.8, 0.8, 1);
    else if (colorMod == 8)
      FragColor = vec4(0.9, 0.9, 0.9, 1);
    else
      FragColor = vec4(1, 1, 1, 1);
  }
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
  GLuint _texture;

  void initWindow(void);
  void initGL(Object& obj);
  void mainloop(Object& obj);
  void clean(void);
  void setupVAO(Object& obj);
  void setupTexture(Object& obj);
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

  void run(Object& obj){
    initWindow();
    initGL(obj);
    mainloop(obj);
    clean();
  }
};
