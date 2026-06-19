#pragma once

# include "Math.hpp"

struct t_var
{
  int   mode = 0;
  bool  isSpin = true;
  bool  isWireFrame = false;

  bool  notRotate = false;
  bool  mouseClick = false;
  double mousePosY = 0;
  double mousePosX = 0;
};

class Transform
{
public:
  enum e_axis
  {
    X_AXIS,
    Y_AXIS,
    Z_AXIS
  };
  t_var _var;
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
  float _zoom{1};

  float _moveSize;
  float _rotateSize;
  float _scaleSize;

  void setupProjection(void)
  {
    float fovDegrees = 60.0f;
    float aspectRatio = 1920.0f / 1080.0f;
    float zFar = 1000.0f;
    float zNear = 0.1f;
    
    float fovRadians = fovDegrees * (M_PI / 180.0f);
    float tanHalfFov = std::tan(fovRadians / 2.0f);
    float f = 1.0f / tanHalfFov;
    
    _projection[0] = f / aspectRatio; 
    _projection[5]  = f;               
    _projection[10] = -(zFar + zNear) / (zFar - zNear);
    _projection[11] = -1.0f;
    _projection[14] = -(2.0f * zFar * zNear) / (zFar - zNear);
    _projection[15] = 0;
  }

  void processModel(void)
  {
    _model = t_mat4();
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
        tmp[5] = c;
        tmp[10] = c;
        tmp[9] = s;
        tmp[6] = -s;
      }
      else if (i == 1)
      {
        //Y axis
        tmp[0] = c;
        tmp[10] = c;
        tmp[2] = -s;
        tmp[8] = s;
      }
      else
      {
        //Z axis
        tmp[0] = c;
        tmp[5] = c;
        tmp[4] = s;
        tmp[1] = -s;
      }
      _model = _model * tmp;
    }
    _model[3] = _moveX;
    _model[7] = _moveY;
    _model[11] = _moveZ;

    for (int col = 0; col < 3; col++)
      for (int row = 0; row < 3; row++)
        _model.m[col + row * 4] *= _zoom;
  }
public:
  Transform(void)
  : _moveSize(0.1), _rotateSize(1.5), _scaleSize(0.1)
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

  void move(e_axis axis, float n) {
    if (axis == Y_AXIS)
      _moveY += n;
    else if (axis == X_AXIS)
      _moveX += n;
    else if (axis == Z_AXIS)
      _moveZ += n;
  }

  float getMove(e_axis axis) {
    if (axis == Y_AXIS)
      return _moveY;
    else if (axis == X_AXIS)
      return _moveX;
    else if (axis == Z_AXIS)
      return _moveZ;
    return 0;
  }

  void rotate(e_axis axis, float n)
  {
    float *tmp;
    if (axis == X_AXIS)
      tmp = &_angleX;
    else if (axis == Y_AXIS)
      tmp = &_angleY;
    else
      tmp = &_angleZ;
    *tmp += n;
    if (*tmp > 360)
      tmp -= 360;
  }

  void scale(float n)
  {
    _zoom += n;
    if (_zoom > 5.5)
      _zoom = 5.5;
    if (_zoom < 0.1)
      _zoom = 0.1;
  }


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
  t_mat4 getView(void)const { return _view; }
  t_mat4 getProjection(void)const { return _projection; }
  t_mat4 getMVP(void) { return getProjection() * (getView() * getModel()); }
  t_mat4 getMP(void) { return getProjection() * getModel(); }
};
