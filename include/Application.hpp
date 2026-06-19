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
uniform vec2 textureScale;
uniform mat4 matrix;
void main()
{
    gl_Position = matrix * vec4(aPos, 1.0);
    TexCoord = aTex * textureScale;
    Normal = mat3(matrix) * aNormal;
})"
# define FRAGMENT_SRC R"(#version 450 core
struct Material {
    vec3 Ka;       // Ambient
    vec3 Kd;       // Diffuse
    vec3 Ks;       // Specular
    float Ns;      // Shininess
    sampler2D map;
    int enable_map;
};

uniform Material material;

in vec2 TexCoord;
in vec3 Normal;
out vec4 FragColor;
uniform int mode;
uniform vec3 lightPos;
uniform vec3 objectColor;
void main()
{
  if (mode == 1)
  {
    vec3 texture_color;
    if (material.enable_map == 1)
      texture_color = texture(material.map, TexCoord).rgb;
    else
      texture_color = vec3(1, 1, 1);
    vec3 lightAmbient = vec3(1, 1, 1);
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos);
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 ambient = lightAmbient * material.Ka;
    vec3 finalColor = material.Kd * (diff + 0.2);
    
    FragColor = vec4(texture_color, 1) * vec4(ambient + finalColor, 1);
  }
  else if (mode == 2)
  {
    // if (is_enable_texture == 1)
    FragColor = texture(material.map, TexCoord);
    // else
      // FragColor = vec4(1);
  }
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

struct t_application_info
{
  const char          *__name;
  std::size_t         __windowWidth;
  std::size_t         __windowHeight;
  GLFWwindow          *__window;
  const char          *__vss; //vertex shader source
  const char          *__fss; // fragment shader source
  GLuint              __shaderProgram; // buffer ID of shader program
  GLuint              __vao; // vertex array object
  GLuint              __vbo; // vertex buffer object
  GLuint              __ebo; // element buffer object
  std::vector<GLuint> __tbo; // texture buffer object
};

class Application{
private:
  t_application_info  _appInfo{};
  Object              *_obj;

  void initWindow(void);
  void initGL(void);
  void mainloop(void);
  void clean(void);
  void setupVAO(void);
  void setupTexture(void);
public:

  Application(void)
  {
    _appInfo.__vss = VERTEXT_SRC;
    _appInfo.__fss = FRAGMENT_SRC;
    _appInfo.__name = "Application";
    _appInfo.__windowWidth = 1920;
    _appInfo.__windowHeight = 1080;
    _obj = nullptr;
  }
  Application(std::string name, std::size_t width, std::size_t height)
  {
    _appInfo.__vss = VERTEXT_SRC;
    _appInfo.__fss = FRAGMENT_SRC;
    _appInfo.__name = name.c_str();
    _appInfo.__windowWidth = width;
    _appInfo.__windowHeight = height;
    _obj = nullptr;
  }
  Application(std::string name, std::size_t width, std::size_t height, const char *vertextSrc, const char *fragmentSrc)
  {
    _appInfo.__vss = vertextSrc;
    _appInfo.__fss = fragmentSrc;
    _appInfo.__name = name.c_str();
    _appInfo.__windowWidth = width;
    _appInfo.__windowHeight = height;
    _obj = nullptr;
  }
  ~Application(void)
  {
    if (_obj != nullptr)
      delete _obj;
  }
  void bindObjectByName(const std::string& fileName){
    _obj = new Object(fileName);
  }

  void run(void){
    initWindow();
    initGL();
    mainloop();
    clean();
  }
};
