#pragma once

# include <iostream>
#include <ostream>
#include <stdexcept>
# include <vector>
# include <cmath>


struct t_vec3
{
  float x, y, z;

  t_vec3 operator-(t_vec3 source)
  {
    t_vec3 res;
    res.x = this->x - source.x;
    res.y = this->y - source.y;
    res.z = this->z - source.z;
    return res;
  }
  t_vec3 operator+(t_vec3 source)
  {
    t_vec3 res;
    res.x = this->x + source.x;
    res.y = this->y + source.y;
    res.z = this->z + source.z;
    return res;
  }
};

struct t_vec2
{
  float x, y;
};

/*
 * @brief this is a row major matrix
 *
 * @carefull in OpenGL use collumn major format matrix
*/
struct t_mat4
{
  float m[16];
  t_mat4()
  {
    for (int col = 0; col < 4; col++)
      for (int row = 0; row < 4; row++)
        m[col * 4 + row] = (col == row) ? 1 : 0;
  }
  float& operator[](int row) {
    return m[row];
  }
  const float& operator[](int row) const {
    return m[row];
  }
  t_mat4 operator*(const t_mat4& second)
  {
    t_mat4 res;

    for (int col = 0; col < 4; col++)
      for (int row = 0; row < 4; row++)
        res[row + col * 4]
          = (m[col + 0] * second[row + 0])
          + (m[col + 4] * second[row + 4])
          + (m[col + 8] * second[row + 8])
          + (m[col + 12] * second[row + 12]);
    return res;
  }
  void print(void)
  {
    for (int col = 0; col < 4; col++)
    {
      std::cout << "[ ";
      for (int row = 0; row < 4; row++)
        std::cout << m[col * 4 + row] << " ";
      std::cout << "]" << std::endl;
    }
  }
};


class Math
{
private:
  Math(void);
  ~Math(void);
public:
  static t_vec3 cross_vec3(const t_vec3& vec_a, const t_vec3& vec_b)
  {
    t_vec3 cross;
    
    cross.x = (vec_a.y * vec_b.z) - (vec_a.z * vec_b.y);
    cross.y = (vec_a.z * vec_b.x) - (vec_a.x * vec_b.z);
    cross.z = (vec_a.x * vec_b.y) - (vec_a.y * vec_b.x);
    return cross;
  }

  static float dot_vec3(const t_vec3& vec_a, const t_vec3& vec_b)
  {
    t_vec3 norm_a{vec_a.x, vec_a.y, vec_a.z};
    t_vec3 norm_b{vec_b.x, vec_b.y, vec_b.z};
    normalize_vec3(norm_a);
    normalize_vec3(norm_b);
    return (norm_a.x * norm_b.x) + (norm_a.y * norm_b.y) + (norm_a.z * norm_b.z);
  }

  static void normalize_vec3(t_vec3& vec)
  {
    float length = std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    if (length > 0.0f) {
        vec.x /= length;
        vec.y /= length;
        vec.z /= length;
    }
  }
};
