#pragma once

# include "Math.hpp"

class Transform
{
public:
  enum e_axis
  {
    X_AXIS,
    Y_AXIS,
    Z_AXIS
  };
private:
  t_mat4 _model;
  t_mat4 _view;
  t_mat4 _projection;

  float _angleX{};
  float _angleY{};
  float _angleZ{};
  float _moveX{};
  float _moveY{};
  float _moveZ{-0.5};
  float _zoom{0.1};

  float _moveSize;
  float _rotateSize;
  float _scaleSize;

  void setupProjection(void)
  {
    float fovDegrees = 60.0f;
    float aspectRatio = 1920.0f / 1080.0f;
    float zFar = 100.0f;
    float zNear = 0.001f;
    
    float fovRadians = fovDegrees * (M_PI / 180.0f);
    float tanHalfFov = std::tan(fovRadians / 2.0f);
    float f = 1.0f / tanHalfFov;
    
    _projection[0][0]  = f / aspectRatio; 
    _projection[1][1]  = f;               
    _projection[2][2] = -(zFar + zNear) / (zFar - zNear);
    _projection[3][2] = -1.0f;
    _projection[2][3] = -(2.0f * zFar * zNear) / (zFar - zNear);
  }

  void processModel(void)
  {
    float angles[3] = {_angleX, _angleY, _angleZ};
    for (int i = 0; i < 3; i++)
    {
      t_mat4 tmp;
      double rad = angles[i] * (M_PI / 180.0f);
      double c = cos(rad);
      double s = sin(rad);

      if (i == 0)
      {
        //X axis
        tmp[1][1] = c;
        tmp[3][3] = c;
        tmp[2][3] = -s;
        tmp[3][2] = s;
      }
      else if (i == 1)
      {
        //Y axis
        tmp[0][0] = c;
        tmp[2][2] = c;
        tmp[2][0] = -s;
        tmp[0][2] = s;
      }
      else
      {
        //Z axis
        tmp[0][0] = c;
        tmp[1][1] = c;
        tmp[0][1] = -s;
        tmp[1][0] = s;
      }
      _model = Math::multiply_mat4(_model, tmp);
    }
  }
public:
  Transform(void)
  : _moveSize(0.01), _rotateSize(1), _scaleSize(0.01)
  {setupProjection();}

  Transform(float moveSize, float rotateSize, float scaleSize)
  : _moveSize(moveSize), _rotateSize(rotateSize), _scaleSize(scaleSize)
  {setupProjection();}

  ~Transform(void) {}

  void moveUp(void) {_moveY += _moveSize;}
  void moveDown(void) {_moveY -= _moveSize;}
  void moveRight(void) {_moveX += _moveSize;}
  void moveLeft(void) {_moveX -= _moveSize;}
  void moveFront(void) {_moveZ += _moveSize;}
  void moveBack(void) {_moveZ -= _moveSize;}

  void rotateUP(e_axis axis) {
    float *tmp;
    if (axis == X_AXIS)
      tmp = &_angleX;
    else if (axis == Y_AXIS)
      tmp = &_angleY;
    else
      tmp = &_angleZ;
    *tmp += _rotateSize;
    if (*tmp > 360)
      tmp -= 360;
  }
  void rotateDOWN(e_axis axis)
  {
    float *tmp;
    if (axis == X_AXIS)
      tmp = &_angleX;
    else if (axis == Y_AXIS)
      tmp = &_angleY;
    else
      tmp = &_angleZ;
    *tmp -= _rotateSize;
    if (*tmp < 0)
      tmp += 360;
  }

  void scaleUp(void)
  {
    _zoom += _scaleSize;
    if (_zoom > 5.5)
      _zoom = 5.5;
  }
  void scaleDown(void)
  {
    _zoom -= _scaleSize;
    if (_zoom < 0.1)
      _zoom = 0.1;
  }

  t_mat4 getModel(void) {
    processModel();
    return _model;
  }
  t_mat4 getView(void)const {
    return _view;
  }
  t_mat4 getProjection(void)const { return _projection; }
  t_mat4 getMVP(void) {
    return Math::multiply_mat4(
      Math::multiply_mat4(
        getProjection(),
        getView()),
      getModel());
  }
};
