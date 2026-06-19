#pragma once

# include "Transform.hpp"
# include <algorithm>
# include "Image.hpp"
# include <iostream>
# include "Math.hpp"
# include <istream>
# include <ostream>
# include <sstream>
# include <fstream>
# include <float.h>
# include <string>
# include <vector>
# include <cmath>
# include <map>

struct t_texture
{
  bool                        __enable;
  std::vector<unsigned char>  __data;
  int                         __height;
  int                         __width;
  int                         __format;
  t_vec3                      __scale;
};

typedef std::vector<int>      t_fpoint;
typedef std::vector<t_fpoint> t_face;
typedef std::vector<t_face>   t_faces;

// for split mtl
struct t_object_group {
  std::string   __mtlName;
  int           __useMtlAt;
};

struct t_object_info
{
  std::string                   __name;
  std::vector<t_vec3>           __v;
  std::vector<t_vec3>           __vn;
  std::vector<t_vec2>           __vt;
  t_faces                       __f;
  std::vector<t_object_group>   __group;
};

struct t_material
{
  std::string     __name;
  float           __d; // Dissolve / Opacity
  t_vec3          __kd; // Diffuse Color
  t_vec3          __ka; // Ambient Color
  t_vec3          __ks; // Specular Color
  float           __ns; // Specular Exponent / Shininess
  int             __illum; // Illumination Model
  t_texture       __map_Kd; // defual texture
  t_texture       __map_Bump;

  t_material(void){
    __name = "noname";
    __d = 1;
    __kd = {1, 1, 1,};
    __ka = {0, 0, 0};
    __ks = {0, 0, 0};
    __ns = 0;
    __illum = 2;
    __map_Kd = {false, {255, 255 ,255}, 1, 1, 3, {1, 1, 1 }};
    __map_Bump = {false, {255, 255 ,255}, 1, 1, 3, {1, 1, 1 }};
  }
};

typedef std::vector<t_material>     t_materials;

class Object {
private:
  t_object_info   _objInfo;
  t_materials     _meterial;
  Transform       _transform;
  std::string     _objFile;
  std::string     _mtlFile;

                  Object(void);
  void            parsObject(void);
  void            parsMaterial(void);
  void            loadTexture(void);
  void            parseFileObj(std::fstream& file);
  void            parseFileMtl(std::fstream& file);
  void            defaultMaterial(void);
public:
                      Object(const std::string& objFile);
                      ~Object(void);
  std::vector<float>          getVertexs(void);
  std::vector<unsigned int>   getIndices(void) const;
  std::vector<unsigned int>   getSizeOfEachIndices(void) const;
  t_materials                 getMaterialList(void) const;
  std::vector<int>            getMaterialNumber(void) const;
  Transform&                  getTransform(void);
};

bool        checkFileType(const std::string& name, const std::string& type);
std::string strtrim(const std::string& str);
void        invalidFormat(size_t line_count);
void        vec3Collect(std::istringstream& line, std::vector<t_vec3>& collection);
void        vec2Collect(std::istringstream& line, std::vector<t_vec2>& collection);
void        faceCollect(std::istringstream& line, std::vector<std::vector<std::vector<int> > >& collection, const std::vector<t_vec3>& v);
t_vec3      findFaceNormal(t_vec3 a, t_vec3 b, t_vec3 c);
void        normalizeCenter(std::vector<t_vec3>& v);

std::ostream& operator<<(std::ostream& os, const t_vec3& v);
