/**
 * Application
 * - initWindow: hint window and create window context
 * - initGL: - init shader
 *           - init VAO
 */
#include "Application.hpp"
#include "Object.hpp"
#include "RotationMatrix.hpp"
#include "Transform.hpp"
#include <GLFW/glfw3.h>
#include <vector>

static void initShaderProgram(const char *vertexShaderSource,
                              const char *fragmentShaderSource,
                              unsigned int *shaderProgram);
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
void Application::initWindow(void) {
  std::cout << "Initialize glfw . . . ";
  int init_succcess = glfwInit();
  if (init_succcess != GLFW_TRUE)
    throw std::runtime_error("Init GLFW failed.");
  std::cout << "[ ✔ ]" << std::endl;

  std::cout << "Creating window . . . ";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  _appInfo.__window =
      glfwCreateWindow(_appInfo.__windowWidth, _appInfo.__windowHeight,
                       _appInfo.__name, nullptr, nullptr);
  if (!_appInfo.__window) {
    glfwTerminate();
    throw std::runtime_error("Create window failed.");
  }

  glfwMakeContextCurrent(_appInfo.__window);
  glfwSwapInterval(1);

  glViewport(0, 0, _appInfo.__windowWidth, _appInfo.__windowHeight);
  std::cout << "[ ✔ ]" << std::endl;

  std::cout << "Initialize glew . . . ";
  GLenum glew_success = glewInit();
  if (glew_success != GLEW_OK) {
    glfwDestroyWindow(_appInfo.__window);
    glfwTerminate();
    throw std::runtime_error("Init Glew failed.");
  }
  std::cout << "[ ✔ ]" << std::endl;

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
void Application::initGL(void) {
  std::cout << "Initialize shader program . . . ";
  initShaderProgram(_appInfo.__vss, _appInfo.__fss, &_appInfo.__shaderProgram);
  std::cout << "[ ✔ ]" << std::endl;

  std::cout << "Setup program attribute . . . ";
  glDisable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  std::cout << "[ ✔ ]" << std::endl;

  std::cout << "Setup vertex array object . . . ";
  setupVAO();
  std::cout << "[ ✔ ]" << std::endl;

  std::cout << "Setup texture . . . ";
  setupTexture();
  std::cout << "[ ✔ ]" << std::endl;
}

static void initShaderProgram(const char *vertexShaderSource,
                              const char *fragmentShaderSource,
                              unsigned int *shaderProgram) {
  unsigned int vertexShader =
      compile_shader(GL_VERTEX_SHADER, vertexShaderSource);
  unsigned int fragmentShader =
      compile_shader(GL_FRAGMENT_SHADER, fragmentShaderSource);

  *shaderProgram = glCreateProgram();
  glAttachShader(*shaderProgram, vertexShader);
  glAttachShader(*shaderProgram, fragmentShader);
  glLinkProgram(*shaderProgram);

  GLint success;
  glGetProgramiv(*shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
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

static GLuint compile_shader(GLenum type, const char *shaderSource) {
  unsigned int shader = glCreateShader(type);
  glShaderSource(shader, 1, &shaderSource, nullptr);
  glCompileShader(shader);

  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
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

void Application::setupTexture(void) {
  t_materials m = _obj->getMaterialList();
  for (t_materials::iterator it = m.begin(); it != m.end(); it++)
  {
    GLuint textureID;
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    GLenum chennel = (it->__map_Kd.__format == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, chennel, it->__map_Kd.__width,
                 it->__map_Kd.__height, 0, chennel, GL_UNSIGNED_BYTE,
                 it->__map_Kd.__data.data());

    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    _appInfo.__tbo.push_back(textureID);
  }
}

void Application::setupVAO(void) {
  std::vector<float> v = _obj->getVertexs();
  std::vector<unsigned int> f = _obj->getIndices();

  std::cout << "Vector size: " << v.size() <<  " . . . ";
  
  glCreateBuffers(1, &_appInfo.__vbo);
  glNamedBufferStorage(_appInfo.__vbo, v.size() * sizeof(float), v.data(), 0);

  glCreateBuffers(1, &_appInfo.__ebo);
  glNamedBufferStorage(_appInfo.__ebo, f.size() * sizeof(unsigned int), f.data(), 0);

  glCreateVertexArrays(1, &_appInfo.__vao);
  glVertexArrayElementBuffer(_appInfo.__vao, _appInfo.__ebo);
  glVertexArrayVertexBuffer(_appInfo.__vao, 0, _appInfo.__vbo, 0,
                            8 * sizeof(float));
  glVertexArrayBindingDivisor(_appInfo.__vao, 0, 0);

  glVertexArrayAttribBinding(_appInfo.__vao, 0, 0);
  glVertexArrayAttribFormat(_appInfo.__vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
  glEnableVertexArrayAttrib(_appInfo.__vao, 0);

  glVertexArrayAttribBinding(_appInfo.__vao, 2, 0);
  glVertexArrayAttribFormat(_appInfo.__vao, 2, 2, GL_FLOAT, GL_FALSE,
                            3 * sizeof(float));
  glEnableVertexArrayAttrib(_appInfo.__vao, 2);

  glVertexArrayAttribBinding(_appInfo.__vao, 3, 0);
  glVertexArrayAttribFormat(_appInfo.__vao, 3, 3, GL_FLOAT, GL_FALSE,
                            5 * sizeof(float));
  glEnableVertexArrayAttrib(_appInfo.__vao, 3);

  glBindVertexArray(_appInfo.__vao);
}

void userInput(GLFWwindow *window, Transform &matrix) {
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    matrix.rotateUP(Transform::X_AXIS);
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    matrix.rotateDOWN(Transform::X_AXIS);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    matrix.rotateUP(Transform::Y_AXIS);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    matrix.rotateDOWN(Transform::Y_AXIS);
  if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
    matrix.moveUp();
  if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
    matrix.moveDown();
  if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
    matrix.moveRight();
  if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
    matrix.moveLeft();
  if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
    matrix.moveFront();
  if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
    matrix.moveBack();
  if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    matrix.scaleUp();
  if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
    matrix.scaleDown();
}

void keyCallbackWrapper(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  (void)scancode;
  (void)mods;
  Transform* transform = static_cast<Transform*>(glfwGetWindowUserPointer(window));

  if (key == GLFW_KEY_T && action == GLFW_PRESS) {
    transform->_var.isWireFrame = !transform->_var.isWireFrame;
    if (transform->_var.isWireFrame)
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
  if (key == GLFW_KEY_U && action == GLFW_PRESS) {
    if (transform->_var.mode == 4)
      transform->_var.mode = 0;
    else if (transform->_var.mode == 0)
      transform->_var.mode = 4;
  }
  if (key == GLFW_KEY_0 && action == GLFW_PRESS)
    transform->_var.mode = 0;
  if (key == GLFW_KEY_1 && action == GLFW_PRESS)
    transform->_var.mode = 1;
  if (key == GLFW_KEY_2 && action == GLFW_PRESS)
    transform->_var.mode = 2;
  if (key == GLFW_KEY_3 && action == GLFW_PRESS)
    transform->_var.mode = 3;
  if (key == GLFW_KEY_4 && action == GLFW_PRESS)
    transform->_var.mode = 4;
  if (key == GLFW_KEY_C && action == GLFW_PRESS)
    transform->_var.isSpin = !transform->_var.isSpin;
}

void scrollCallbackWrapper(GLFWwindow* window, double xoffset, double yoffset)
{
  (void)xoffset;
  Transform* transform = static_cast<Transform*>(glfwGetWindowUserPointer(window));
  float scale = transform->getMove(Transform::Z_AXIS) * 0.1;
  transform->move(Transform::Z_AXIS, -(yoffset * scale));
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
  (void)mods;
  Transform* transform = static_cast<Transform*>(glfwGetWindowUserPointer(window));
  if (button == GLFW_MOUSE_BUTTON_LEFT)
  {
    if (action == GLFW_PRESS)
    {
      transform->_var.mouseClick = true;
      glfwGetCursorPos(window, (double*)&transform->_var.mousePosX, (double*)&transform->_var.mousePosY);
    }
    else
      transform->_var.mouseClick = false;

  }
  if (button == GLFW_MOUSE_BUTTON_RIGHT)
  {
    if (action == GLFW_PRESS)
    {
      transform->_var.mouseClick = true;
      transform->_var.notRotate = true;
      glfwGetCursorPos(window, (double*)&transform->_var.mousePosX, (double*)&transform->_var.mousePosY);
    }
    else
    {
      transform->_var.mouseClick = false;
      transform->_var.notRotate = false;
    }

  }
}

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
  Transform* transform = static_cast<Transform*>(glfwGetWindowUserPointer(window));
  if (transform->_var.mouseClick)
  {
    double deltaX = xpos - transform->_var.mousePosX;
    double deltaY = ypos - transform->_var.mousePosY;

    transform->_var.mousePosX = xpos;
    transform->_var.mousePosY = ypos;
    if (!transform->_var.notRotate)
    {
      float sensitivity = 0.2f;
      transform->rotate(Transform::Y_AXIS, deltaX * sensitivity);
      transform->rotate(Transform::X_AXIS, deltaY * sensitivity);
    }
    else
    {
      float sensitivity = transform->getMove(Transform::Z_AXIS) * -0.001f;
      transform->move(Transform::X_AXIS, deltaX * sensitivity);
      transform->move(Transform::Y_AXIS, deltaY * -sensitivity);
    }
  }
}

void sendMaterial(t_material mtl, GLuint shaderProgram, GLuint textureID)
{
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textureID);
  glUniform1i(glGetUniformLocation(shaderProgram, "material.map"),
              0);
  glUniform3f(glGetUniformLocation(shaderProgram, "material.Ka"),
              mtl.__ka.x, mtl.__ka.y, mtl.__ka.z);
  glUniform3f(glGetUniformLocation(shaderProgram, "material.Kd"),
              mtl.__kd.x, mtl.__kd.y, mtl.__kd.z);
  glUniform1i(glGetUniformLocation(shaderProgram, "material.enable_map"),
              mtl.__map_Kd.__enable);
  glUniform2f(glGetUniformLocation(shaderProgram, "textureScale"),
              mtl.__map_Kd.__scale.x, mtl.__map_Kd.__scale.y);
}

void sendUniform(Transform& transform, GLuint shaderProgram)
{
  glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "matrix"),
                     1, GL_TRUE, &transform.getMP()[0]);
  glUniform1i(glGetUniformLocation(shaderProgram, "mode"),
              transform._var.mode);
  glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"),
              0.8, 0.8, 0.8);
  glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"),
              -0.5, 0.5, -0.5);
}

void animation(Transform& transform)
{
  if (transform._var.isSpin)
    transform.rotateDOWN(Transform::Y_AXIS);
}

void Application::setupCallback(void)
{
  glfwSetWindowUserPointer(_appInfo.__window, &this->_obj->getTransform());
  glfwSetKeyCallback(_appInfo.__window, keyCallbackWrapper);
  glfwSetScrollCallback(_appInfo.__window, scrollCallbackWrapper);
  glfwSetMouseButtonCallback(_appInfo.__window, mouseButtonCallback);
  glfwSetCursorPosCallback(_appInfo.__window, cursorPosCallback);
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}

void Application::mainloop(void) {
  setupCallback();
  std::vector<unsigned int> sizes = _obj->getSizeOfEachIndices();
  std::vector<int>  textureNumberID = _obj->getMaterialNumber();
  t_materials material = _obj->getMaterialList();
  while (!glfwWindowShouldClose(_appInfo.__window)) {
    userInput(_appInfo.__window, _obj->getTransform());
    glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    sendUniform(_obj->getTransform(), _appInfo.__shaderProgram);
    animation(_obj->getTransform());

    int sumIndicesSize = 0;
    for (size_t index = 0; index < sizes.size(); index++)
    {
      sendMaterial(material.at(textureNumberID.at(index)), _appInfo.__shaderProgram,
                   _appInfo.__tbo.at(textureNumberID.at(index)));
      glDrawElements(GL_TRIANGLES, sizes.at(index), GL_UNSIGNED_INT,
                     (void*)(sumIndicesSize * sizeof(unsigned int)));
      sumIndicesSize += sizes.at(index);
      glBindTexture(GL_TEXTURE_2D, 0);
    }
    glfwSwapBuffers(_appInfo.__window);
  }
}

void Application::clean(void) {
  glDeleteVertexArrays(1, &_appInfo.__vao);
  glDeleteBuffers(1, &_appInfo.__vbo);
  glDeleteBuffers(1, &_appInfo.__ebo);
  glDeleteProgram(_appInfo.__shaderProgram);

  glDeleteTextures(_appInfo.__tbo.size(), _appInfo.__tbo.data());

  glfwDestroyWindow(_appInfo.__window);
  glfwTerminate();
}
