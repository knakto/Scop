#pragma once

# include <algorithm>
# include <iostream>
# include <sstream>
# include <fstream>
# include <string>
# include <vector>
# include <cmath>
# include <map>


struct t_vec3
{
  float x, y, z;
};

struct t_vec2
{
  float x, y;
};

class Object{
private:
  std::vector<t_vec3>                           _v;
  std::vector<t_vec2>                           _vt;
  std::vector<t_vec3>                           _vn;
  std::vector<std::vector<std::vector<int> > >  _f;
  std::string                                   _name;

  Object(void);
  void	parseFile(std::fstream& file);
public:
  Object(const std::string& fileName);
  ~Object(void);

  std::vector<float>        getVertexs(void) const;
  std::vector<unsigned int> getIndices(void) const;
};

bool        checkFileType(const std::string& name, const std::string& type);
std::string strtrim(const std::string& str);
void        invalidFormat(size_t line_count);
void        vec3Collect(std::istringstream& line, std::vector<t_vec3>& collection);
void        vec2Collect(std::istringstream& line, std::vector<t_vec2>& collection);
void        faceCollect(std::istringstream& line, std::vector<std::vector<std::vector<int> > >& collection);
t_vec3      findVn(t_vec3 a, t_vec3 b, t_vec3 c);
