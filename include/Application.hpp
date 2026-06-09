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
// layout (location = 1) in vec3 aColor;
// layout (location = 0) out vec3 oColor;
layout (location = 2) in vec2 aTex;
layout (location = 3) in vec3 aNormal;

out vec2 TexCoord;
out vec3 Normal;
uniform mat4 matrix;
void main()
{
   gl_Position = matrix * vec4(aPos, 1.0);
   // oColor = aColor;
   // ส่งข้อมูลดิบผ่านทะลุไปเลย
    TexCoord = aTex;
    
    // ทริค: หมุน Normal ไปตามโมเดลด้วย เพื่อให้แสงเงาหมุนตาม (ใช้ matrix ไปก่อนได้สำหรับเทสเบื้องต้น)
    Normal = mat3(matrix) * aNormal;
})"
# define FRAGMENT_SRC R"(#version 450 core
// layout (location = 0) in vec3 color;
in vec2 TexCoord;
in vec3 Normal;
out vec4 FragColor;
void main()
{
  // =========================================================
    // โหมด 1: เทส UV (vt)
    // ผลลัพธ์ที่ควรได้: โมเดลจะมีสี แดง-เขียว-เหลือง ไล่เฉดไปมา
    // เหตุผล: เราเอาแกน U ไปใส่สีแดง (X) และ V ไปใส่สีเขียว (Y)
    // =========================================================
    // FragColor = vec4(TexCoord.x, TexCoord.y, 0.0, 1.0);
    // return; 


    // =========================================================
    // โหมด 2: เทส Normal (vn) แบบ Color Mapping
    // ผลลัพธ์ที่ควรได้: หน้าที่หันไปทางขวา=สีแดง, ชี้ขึ้นบน=สีเขียว, หันเข้าหากล้อง=สีฟ้า
    // เหตุผล: เวกเตอร์วิ่งจาก -1 ถึง 1 เราเลยปรับสมการให้กลายเป็นสี 0 ถึง 1
    // =========================================================
    vec3 normalColor = normalize(Normal) * 0.5 + 0.5;
    FragColor = vec4(normalColor, 1.0);
    return;
  // vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0)); 
  //
  //   // Normal ต้องยาว 1 หน่วยเสมอ
  //   vec3 norm = normalize(Normal);
  //
  //   // หาค่าการชนของแสง (Dot Product)
  //   float diff = max(dot(norm, lightDir), 0.0);
  //
  //   // กำหนดสีโมเดลเป็นสีเทาอ่อน (0.8)
  //   vec3 objectColor = vec3(0.8, 0.8, 0.8);
  //
  //   // แสงตกกระทบ (diff) + แสงพยุงในเงามืด (ambient = 0.2)
  //   vec3 finalColor = objectColor * (diff + 0.2);
  //
  //   FragColor = vec4(finalColor, 1.0);
   // FragColor = vec4(1.0f);
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
  void initGL(Object& obj);
  void mainloop(Object& obj);
  void clean(void);
  void setupVAO(Object& obj);
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
