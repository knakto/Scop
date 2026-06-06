#pragma once

#include <cmath>
#include <iostream>

class Matrix4x4{
public:
  typedef enum e_mode
  {
    UP,
    DOWN
  } t_mode;

private:
  float _matrix[16];
  float _angleX;
  float _angleY;
  float _angleZ;
  float _moveX;
  float _moveY;
  float _moveZ;
  float _zoom;

  float _moveScale;
  float _rotateScale;
  float _zoomScale;
public:
  Matrix4x4(void);
  Matrix4x4(float moveScale, float rotateScale, float zoomScale);
  ~Matrix4x4(void);

  void clearMatrix(void);

  void rotateX(e_mode mode);
  void rotateY(e_mode mode);
  void rotateZ(e_mode mode);

  void translateX(e_mode mode);
  void translateY(e_mode mode);
  void translateZ(e_mode mode);

  void zoomMatrix(e_mode mode);

  void calculateMatrix(void);

  operator float*() { 
    return _matrix; 
  }
  operator const float*() const { 
    return _matrix; 
  }
};
