/**
 * Application
 * - initWindow: hint window and create window context
 * - initGL: - init shader
 *           - init VAO
 */

#include "Application.hpp"
#include "ObjectData.hpp"
#include <vector>

static void initShaderProgram(const char *vertexShaderSource, const char *fragmentShaderSource, unsigned int *shaderProgram);
static GLuint compile_shader(GLenum type, const char *shaderSource);
/**
 * init window
 * - init glfw
 * - hint window
 *   - hint GLFW version 3.3
 *   - hint to use default openGL profile
 * - create window
 * - make context window
 * - set SwapInterval to link FPS and refreshrate
 * - init glew
 * - setViewport
 */
void Application::initWindow(void)
{
  int init_succcess = glfwInit();
  if (init_succcess != GLFW_TRUE)
    throw std::runtime_error("Init GLFW failed.");

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  _window = glfwCreateWindow(_width, _height, _name, nullptr, nullptr);
  if (!_window)
  {
    glfwTerminate();
    throw std::runtime_error("Create window failed.");
  }

  glfwMakeContextCurrent(_window);
  glfwSwapInterval(1);

  GLenum glew_success = glewInit();
  if (glew_success != GLEW_OK)
  {
    glfwDestroyWindow(_window);
    glfwTerminate();
    throw std::runtime_error("Init Glew failed.");
  }

  glViewport(0, 0, _width, _height);
}

/**
 * init OpenGl engine
 * - init shader program: - init vertexShader
 *                        - init fragmentShader
 *                        - create programShader
 *                        - delete vertexShader and fragmentShader
 * - setup z buffer flags
 * - create VBO(vertex buffer object)
 * - register VAO(vertex array object)
 *
 */
void Application::initGL(ObjectData* obj)
{
  initShaderProgram(_vertexShaderSource, _fragmentShaderSource, &_shaderProgram);

  glDisable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  setupVAO(obj);
}

static void initShaderProgram(const char *vertexShaderSource, const char *fragmentShaderSource, unsigned int *shaderProgram)
{
  unsigned int vertexShader = compile_shader(GL_VERTEX_SHADER, vertexShaderSource);
  unsigned int fragmentShader = compile_shader(GL_FRAGMENT_SHADER, fragmentShaderSource);

  *shaderProgram = glCreateProgram();
  glAttachShader(*shaderProgram, vertexShader);
  glAttachShader(*shaderProgram, fragmentShader);
  glLinkProgram(*shaderProgram);

  GLint success;
  glGetProgramiv(*shaderProgram, GL_LINK_STATUS, &success);
  if (!success)
  {
    GLint length;
    glGetProgramiv(*shaderProgram, GL_INFO_LOG_LENGTH, &length);
    if (length == 0)
      throw std::runtime_error("Compile shader failed, with no log.");
    std::string log(length, 0);
    glGetProgramInfoLog(*shaderProgram, length, nullptr, log.data());
    throw std::runtime_error("Compile shader failed: \n" + log);
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  glUseProgram(*shaderProgram);
}

static GLuint compile_shader(GLenum type, const char *shaderSource)
{
  unsigned int shader = glCreateShader(type);
  glShaderSource(shader, 1, &shaderSource, nullptr);
  glCompileShader(shader);

  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    GLint length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    if (length == 0)
      throw std::runtime_error("Compile shader failed, with no log.");
    std::string log(length, 0);
    glGetShaderInfoLog(shader, length, nullptr, log.data());
    throw std::runtime_error("Compile shader failed: \n" + log);
  }

  return shader;
}

void Application::setupVAO(ObjectData* obj)
{
  std::vector<float> v = obj->getVertexs();
  std::vector<unsigned int> f = obj->getIndices();
  // float vertices[] = {
  //   -.5f, .5f, .5f,
  //   -.5f, .5f, -.5f,
  //   .5f, .5f, -.5f,
  //   .5f, .5f, .5f,
  //   -.5f, -.5f, .5f,
  //   -.5f, -.5f, -.5f,
  //   .5f, -.5f, -.5f,
  //   .5f, -.5f, .5f,
  // };

  float color[] = {
     1, 1, 1,
     0, 0,  0,
    1, 1,  1,
    0, 0, 1,
     1,  1, 1,
     0,  1,  0,
    // 1,  1,  1,
    // 1,  0, 0
     1, 1, 1,
     0, 0,  0,
    1, 1,  1,
    0, 0, 1,
     1,  1, 1,
     0,  1,  0,
     1, 1, 1,
     0, 0,  0,
    1, 1,  1,
    0, 0, 1,
     1,  1, 1,
     0,  1,  0,
     1, 1, 1,
     0, 0,  0,
    1, 1,  1,
    0, 0, 1,
     1,  1, 1,
     0,  1,  0,
     1, 1, 1,
     0, 0,  0,
    1, 1,  1,
    0, 0, 1,
     1,  1, 1,
     0,  1,  0,
     1, 1, 1,
     0, 0,  0,
    1, 1,  1,
    0, 0, 1,
     1,  1, 1,
     0,  1,  0,
     1, 1, 1,
     0, 0,  0,
    1, 1,  1,
    0, 0, 1,
     1,  1, 1,
     0,  1,  0,
  };

  // unsigned int indices[] = {
  //   // หน้า Front (ด้านหน้า Z = 0.5)
  //   0, 4, 7,
  //   7, 3, 0,
  //
  //   // หน้า Back (ด้านหลัง Z = -0.5)
  //   1, 2, 6,
  //   6, 5, 1,
  //
  //   // หน้า Left (ด้านซ้าย X = -0.5)
  //   0, 1, 5,
  //   5, 4, 0,
  //
  //   // หน้า Right (ด้านขวา X = 0.5)
  //   3, 7, 6,
  //   6, 2, 3,
  //
  //   // หน้า Top (ด้านบน Y = 0.5)
  //   0, 3, 2,
  //   2, 1, 0,
  //
  //   // หน้า Bottom (ด้านล่าง Y = -0.5)
  //   4, 5, 6,
  //   6, 7, 4
  // };

  glCreateBuffers(1, &_vbo);
  glNamedBufferStorage(_vbo, v.size() * sizeof(float), v.data(), 0);

  glCreateBuffers(1, &_cbo);
  glNamedBufferStorage(_cbo, sizeof color, color, 0);

  glCreateBuffers(1, &_ebo);
  glNamedBufferStorage(_ebo, f.size() * sizeof(unsigned int), f.data(), 0);

  glCreateVertexArrays(1, &_vao);
  glVertexArrayElementBuffer(_vao, _ebo);
  glVertexArrayVertexBuffer(_vao, 0, _vbo, 0, 3 * sizeof(float));
  glVertexArrayVertexBuffer(_vao, 1, _cbo, 0, 3 * sizeof(float));
  glVertexArrayBindingDivisor(_vao, 0, 0);

  glVertexArrayAttribBinding(_vao, 0, 0);
  glVertexArrayAttribFormat(_vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
  glEnableVertexArrayAttrib(_vao, 0);

  glVertexArrayAttribBinding(_vao, 1, 1);
  glVertexArrayAttribFormat(_vao, 1, 3, GL_FLOAT, GL_FALSE, 0);
  glEnableVertexArrayAttrib(_vao, 1);

  glBindVertexArray(_vao);
}

struct Material {
    float Ka[3]; // แสงรอบทิศ
    float Kd[3]; // สีหลัก
    float Ks[3]; // สีเงาสะท้อน
    float Ns;    // ความมันวาว
};

void Application::mainloop(ObjectData* obj)
{
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  Matrix4x4 matrix{1, 1, 0.01};
  Material material= {
    {0.0f, 0.0f, 0.0f},       // Ka
    {0.64f, 0.64f, 0.64f},    // Kd
    {0.5f, 0.5f, 0.5f},       // Ks
    96.078431f                // Ns
  };
  while (!glfwWindowShouldClose(this->_window))
  {
    if(glfwGetKey(this->_window, GLFW_KEY_Q) == GLFW_PRESS)
      glfwSetWindowShouldClose(this->_window, true);
    if(glfwGetKey(this->_window, GLFW_KEY_W) == GLFW_PRESS)
      matrix.rotateX(Matrix4x4::UP);
    if(glfwGetKey(this->_window, GLFW_KEY_S) == GLFW_PRESS)
      matrix.rotateX(Matrix4x4::DOWN);
    if(glfwGetKey(this->_window, GLFW_KEY_D) == GLFW_PRESS)
      matrix.rotateY(Matrix4x4::UP);
    if(glfwGetKey(this->_window, GLFW_KEY_A) == GLFW_PRESS)
      matrix.rotateY(Matrix4x4::DOWN);
    if(glfwGetKey(this->_window, GLFW_KEY_K) == GLFW_PRESS)
      matrix.translateY(Matrix4x4::UP);
    if(glfwGetKey(this->_window, GLFW_KEY_J) == GLFW_PRESS)
      matrix.translateY(Matrix4x4::DOWN);
    if(glfwGetKey(this->_window, GLFW_KEY_L) == GLFW_PRESS)
      matrix.translateX(Matrix4x4::UP);
    if(glfwGetKey(this->_window, GLFW_KEY_H) == GLFW_PRESS)
      matrix.translateX(Matrix4x4::DOWN);
    if(glfwGetKey(this->_window, GLFW_KEY_Z) == GLFW_PRESS)
      matrix.zoomMatrix(Matrix4x4::UP);
    if(glfwGetKey(this->_window, GLFW_KEY_X) == GLFW_PRESS)
      matrix.zoomMatrix(Matrix4x4::DOWN);
    glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    matrix.calculateMatrix();

    int matrixLoc = glGetUniformLocation(this->_shaderProgram, "matrix");
    glUniformMatrix4fv(matrixLoc, 1, GL_FALSE, matrix);

    unsigned int ambientLoc  = glGetUniformLocation(_shaderProgram, "material.ambient");
    unsigned int diffuseLoc  = glGetUniformLocation(_shaderProgram, "material.diffuse");
    unsigned int specularLoc = glGetUniformLocation(_shaderProgram, "material.specular");
    unsigned int shineLoc    = glGetUniformLocation(_shaderProgram, "material.shininess");

    // ส่งตัวเลขจาก Struct ของเราพุ่งตรงเข้าการ์ดจอ!
    glUniform3f(ambientLoc,  material.Ka[0], material.Ka[1], material.Ka[2]);
    glUniform3f(diffuseLoc,  material.Kd[0], material.Kd[1], material.Kd[2]);
    glUniform3f(specularLoc, material.Ks[0], material.Ks[1], material.Ks[2]);
    glUniform1f(shineLoc,    material.Ns);

    glDrawElements(GL_TRIANGLES, obj->getIndices().size(), GL_UNSIGNED_INT, nullptr);
    glfwSwapBuffers(this->_window);
  }
}

void Application::clean(void)
{
  glDeleteVertexArrays(1, &_vao);
  glDeleteBuffers(1, &_vbo);
  glDeleteBuffers(1, &_ebo);
  glDeleteProgram(_shaderProgram);

  glfwDestroyWindow(_window);
  glfwTerminate();
}
