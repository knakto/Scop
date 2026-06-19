#include "Object.hpp"
#include "Image.hpp"
#include "Math.hpp"
#include <algorithm>
#include <climits>
#include <mutex>
#include <ostream>
#include <sstream>
#include <stdexcept>

std::vector<std::vector<std::vector<int> > >
earClipping(std::vector<std::vector<int> > face, const std::vector<t_vec3>& vertex);

Object::~Object(void) {}
Object::Object(const std::string& fileName): _objFile(fileName)
{
  std::cout << "Load object file . . . ";
  parsObject();
  std::cout << "[ ✔ ]" << std::endl;

  #ifdef OUTSTANDING
  std::cout << "Load material file . . . ";
  parsMaterial();
  std::cout << "[ ✔ ]" << std::endl;
  #else
  std::cout << "Load default texture . . . ";
  defaultMaterial();
  std::cout << "[ ✔ ]" << std::endl;
  #endif

  getVertexs();
}

void  Object::defaultMaterial()
{
  t_material mtl;

  unsigned char* bmp = loadBMP("resources/bigcat.bmp",
      mtl.__map_Kd.__height, mtl.__map_Kd.__width);
  int dataSize = mtl.__map_Kd.__width * mtl.__map_Kd.__height * 3;
  mtl.__map_Kd.__data.assign(bmp, bmp + dataSize);

  _meterial.push_back(mtl);
  delete[] bmp;
}

bool checkFileType(const std::string& name, const std::string& type)
{
	size_t pos = name.rfind(".");
	return (name.substr(pos) == type);
}

void  Object::parsObject(void)
{
	std::fstream	file;

	if (!checkFileType(_objFile, ".obj"))
		throw std::runtime_error("file name not an object type (.obj).");

	file.open(_objFile, std::ios::in);
	if (!file.is_open())
		throw std::runtime_error("file object not found.");

	parseFileObj(file);
	file.close();
  normalizeCenter(_objInfo.__v);
}

void	Object::parseFileObj(std::fstream& file)
{
	try{
		std::string	input_line;
		int       line_count = 1;

		while (std::getline(file, input_line))
		{
			std::istringstream line(strtrim(input_line));
			std::string word;

			if (line.str().size() == 0 || line.str().at(0) == '#')
      {
        line_count++;
				continue ;
      }
			line >> word;
      if (word == "mtllib")
        _mtlFile = line.str().substr(word.size() + 1);
      else if (word == "o")
        line >> _objInfo.__name;
      else if (word == "v")
        vec3Collect(line, _objInfo.__v);
      else if (word == "vt")
        vec2Collect(line, _objInfo.__vt);
      else if (word == "vn")
        vec3Collect(line, _objInfo.__vn);
      else if (word == "f")
        faceCollect(line, _objInfo.__f, _objInfo.__v);
      else if (word == "usemtl")
      {
        t_object_group group{};
        group.__useMtlAt = _objInfo.__f.size();
        std::getline(line, group.__mtlName);
        _objInfo.__group.push_back(group);
      }
      else if (word == "s"){}
      else if (word == "g"){}
      else
				invalidFormat(line_count);
			line_count++;
		}
	} catch ( const std::runtime_error& e )
	{
		file.close();
		throw e;
	}
}

std::string strtrim(const std::string& str)
{
	size_t front_pos = str.find_first_not_of(" \t\n\r\f\v");
	size_t back_pos = str.find_last_not_of(" \t\n\r\f\v");
	if (front_pos == std::string::npos || back_pos  == std::string::npos)
		return "";
	else if (front_pos == 0 && back_pos == str.length() - 1)
		return str;
	return str.substr(front_pos, back_pos);
}

void invalidFormat(size_t line_count)
{
	std::stringstream str_line_count;
	str_line_count << line_count;
	throw std::runtime_error("line " + str_line_count.str() + \
		": invalid format, please use [v/f/vt/vn/o/mtllib/usemtl] only");
}

void	vec3Collect(std::istringstream& line, std::vector<t_vec3>& collection)
{
	float	x;
	float	y;
	float	z;
	t_vec3	vec;

	// std::cout << "line: " << line.str() << std::endl; 
	if (!(line >> x >> y >> z))
		throw std::runtime_error("Line not a 3D vector.");
	vec.x = x;
	vec.y = y;
	vec.z = z;
	collection.push_back(vec);
}

void	vec2Collect(std::istringstream& line, std::vector<t_vec2>& collection)
{
	float	x;
	float	y;
	t_vec2	vec;

	// std::cout << "line: " << line.str() << std::endl; 
	if (!(line >> x >> y))
		throw std::runtime_error("Line not a 2D vector.");
	vec.x = x;
	vec.y = 1.0f - y;
	collection.push_back(vec);
}

/**
 * @param face line "1/1/1 2/2/2 3/3/3 4/4/4"
 */
std::vector<std::string> splitFpoints(std::istringstream& face)
{
	std::vector<std::string>  linked;
	std::string				        templink;
	size_t						        count = 0;

	// split "f 1/2/3 3/2/1 2/3/1" (v/vt/vn)
	// to [1/2/3] [3/2/1] [2/3/1]
	while ((face >> templink))
	{
		linked.push_back(templink);
		count++;
	}
	if (count == 0)
	  throw std::runtime_error("Link face failed, Require argument.");
  return linked;
}

/**
 * @param fpoint in string format like "5/2/1" or "3//3" or "1/2"
*/
t_fpoint translateFpoint(const std::string& str_fpoint)
{
  std::vector<std::string>  v;
  std::stringstream         istr_fpoint(str_fpoint);
  t_fpoint                  fpoint;
  std::string               token;

  while (std::getline(istr_fpoint, token, '/'))
    v.push_back(token);

  if (v.size() > 3)
    throw std::runtime_error("face must have only 3 argument v/vt/vn.");

  for (unsigned int i = 0; i < 3; i++)
  {
    std::stringstream ss;
    int               number;

    if (i > v.size() - 1)
      v.push_back("-1");
    else if (i != 0 && v[i].empty())
      v[i] = "-1";
    else
    {
      if (v[i].empty())
        throw std::runtime_error("face musbe number integer and not minus");
      for (unsigned int j = 0; j < v[i].length(); j++)
        if (!(v[i][j] >= '0' && v[i][j] <= '9'))
          throw std::runtime_error("face musbe number integer and not minus");
    }
    ss << v[i];
    ss >> number;
    fpoint.push_back(number);
  }
  return fpoint;
}

void faceCollect(std::istringstream& line, t_faces& collection, const std::vector<t_vec3>& v)
{
  std::vector<std::string> fpoints = splitFpoints(line);
  t_face face;
	for (std::vector<std::string>::iterator fpoint = fpoints.begin(); fpoint != fpoints.end(); fpoint++)
    face.push_back(translateFpoint(*fpoint));

  if (face.size() > 3)
  {
    std::vector<std::vector<std::vector<int> > > array_face = earClipping(face, v);
    collection.insert(collection.end(), array_face.begin(), array_face.end());
  }
  else
    collection.push_back(face);
}

t_vec3 getVertexFromNumber(int vertexNumber, const std::vector<t_vec3>& v)
{
  return v.at(vertexNumber - 1);
}

t_vec3 getNormalFromNumber(int normalNumber, const std::vector<t_vec3>& vn, const t_faces::iterator& face, const std::vector<t_vec3>& v)
{
  if (normalNumber == -1)
  {
    std::vector<t_vec3> triangle;
    for (t_face::iterator fpoint = face->begin(); fpoint != face->end(); fpoint++)
      triangle.push_back(getVertexFromNumber(fpoint->at(0), v));
    return findFaceNormal(triangle[0], triangle[1], triangle[2]);
  }
  return vn.at(normalNumber - 1);
}

t_vec2 getTextureFromNumber(int textureNumber, const std::vector<t_vec2>& vt)
{
  if (textureNumber == -1)
    return {0, 0};
  return vt.at(textureNumber - 1);
}

std::vector<float>        Object::getVertexs(void)
{
  std::vector<float> res;
  for (std::vector<t_object_group>::iterator group = _objInfo.__group.begin();
  group != _objInfo.__group.end(); group++)
  {
    for (t_faces::iterator face = _objInfo.__f.begin(); face != _objInfo.__f.end(); face++)
    {
      for (t_face::iterator fpoint = face->begin(); fpoint != face->end(); fpoint++)
      {
        t_vec3 v = getVertexFromNumber(fpoint->at(0), _objInfo.__v);
        t_vec2 vt = getTextureFromNumber(fpoint->at(1), _objInfo.__vt);
        t_vec3 vn = getNormalFromNumber(fpoint->at(2), _objInfo.__vn, face, _objInfo.__v);
        res.push_back(v.x); res.push_back(v.y); res.push_back(v.z);
        res.push_back(vt.x); res.push_back(vt.y);
        res.push_back(vn.x); res.push_back(vn.y); res.push_back(vn.z);
      }
    }
  }
  return res;
}

std::vector<unsigned int> Object::getSizeOfEachIndices(void) const
{
  std::vector<unsigned int> res;
  int old = 0;

  for (std::vector<t_object_group>::const_iterator group = _objInfo.__group.begin();
  group != _objInfo.__group.end(); group++)
  {
    if (group->__useMtlAt == 0)
      continue ;
    res.push_back((group->__useMtlAt - old) * 3);
    old = group->__useMtlAt;
  }
  res.push_back((_objInfo.__f.size() - old) * 3);
  return res;
}

std::vector<unsigned int> Object::getIndices(void) const
{
  std::vector<unsigned int> res;
  unsigned int count = 0;
  for (t_faces::const_iterator face = _objInfo.__f.begin(); face != _objInfo.__f.end(); face++)
  {
    res.push_back(count++);
    res.push_back(count++);
    res.push_back(count++);
  }
  return res;
}

t_materials                 Object::getMaterialList(void) const
{
  return _meterial;
}

std::vector<int>            Object::getMaterialNumber(void) const
{
  std::vector<int> res;
  for (std::vector<t_object_group>::const_iterator group = _objInfo.__group.begin();
  group != _objInfo.__group.end(); group++)
  {
    int count = 0;
    for (t_materials::const_iterator it = _meterial.begin(); it != _meterial.end(); it++)
    {
      if (it->__name == group->__mtlName)
        res.push_back(count);
      else
        count++;
    }
  }
  return res;
}

// std::vector<float>        Object::getVertexs(void) const
// {
//   std::vector<float> res;
//
//   // for (std::vector<t_vec3>::const_iterator it = _v.begin(); it != _v.end(); it++)
//   // {
//   //   res.push_back(it->x);
//   //   res.push_back(it->y);
//   //   res.push_back(it->z);
//   // }
//   for (std::vector<std::vector<std::vector<int> > >::const_iterator it = _f.begin(); it != _f.end(); it++)
//   {
//     for (std::vector<std::vector<int> >::const_iterator f = it->begin(); f != it->end(); f++)
//     {
//       t_vec3 vertex = _v[f->at(0) - 1];
//       res.push_back(vertex.x);
//       res.push_back(vertex.y);
//       res.push_back(vertex.z);
//
//       if (f->at(1) == -1)
//       {
//         res.push_back(0);
//         res.push_back(0);
//       }
//       else
//       {
//         t_vec2 vertexTexture = _vt[f->at(1) - 1];
//         res.push_back(vertexTexture.x);
//         res.push_back(vertexTexture.y);
//       }
//
//
//       t_vec3 vertexNormal;
//       if (f->at(2) == -1)
//         vertexNormal = findVn(_v[it->at(0)[0] - 1], _v[it->at(1)[0] - 1], _v[it->at(2)[0] - 1]);
//       else
//         vertexNormal = _vn[f->at(2) - 1];
//       res.push_back(vertexNormal.x);
//       res.push_back(vertexNormal.y);
//       res.push_back(vertexNormal.z);
//     }
//   }
//   // int a = 0;
//   // for (auto f: res)
//   // {
//   //   std::cout << "v: " << f << " ";
//   //   a++;
//   //   if (a % 3 == 0)
//   //     std::cout << std::endl;
//   // }
//   return res;
// }

void normalizeCenter(std::vector<t_vec3>& v)
{
  float minX = FLT_MAX;
  float maxX = FLT_MIN;
  float minY = FLT_MAX;
  float maxY = FLT_MIN;
  float minZ = FLT_MAX;
  float maxZ = FLT_MIN;
  for (std::vector<t_vec3>::iterator it = v.begin(); it != v.end(); it++)
  {
    if (it->x < minX)
      minX = it->x;
    if (it->x > maxX)
      maxX = it->x;

    if (it->y < minY)
      minY = it->y;
    if (it->y > maxY)
      maxY = it->y;

    if (it->z < minZ)
      minZ = it->z;
    if (it->z > maxZ)
      maxZ = it->z;
  }
  float centerX = (maxX + minX) / 2;
  float centerY = (maxY + minY) / 2;
  float centerZ = (maxZ + minZ) / 2;
  for (std::vector<t_vec3>::iterator it = v.begin(); it != v.end(); it++)
  {
    it->x -= centerX;
    it->y -= centerY;
    it->z -= centerZ;
  }
}

std::ostream& operator<<(std::ostream& os, const t_vec3& v)
{
  os << "x: " << v.x << ", y: " << v.y << ", z: " << v.z;
  return os;
}

bool isAnyPointInTriangle(t_vec3 a, t_vec3 b, t_vec3 c, std::vector<t_vec3> otherPoint)
{
  t_vec3 faceNormal = findFaceNormal(a, b, c);
  std::vector<t_vec3> triangle = {a, b, c};
  for (std::vector<t_vec3>::iterator d = otherPoint.begin(); d != otherPoint.end(); d++)
  {
    for (int i = 0; i < 3; i++)
    {
      t_vec3 dx = *d - triangle[i % 3];
      t_vec3 xy = triangle[(i + 1) % 3] - triangle[i % 3];
      t_vec3 curv = Math::cross_vec3(dx, xy);
      Math::normalize_vec3(curv);

      int dot = Math::dot_vec3(curv, faceNormal);
      if (dot >= 0)
        return false;
    }
  }
  return true;
}

std::vector<std::vector<std::vector<int> > >
earClipping(std::vector<std::vector<int> > face, const std::vector<t_vec3>& vertex)
{
  size_t i = 0;
  std::vector<std::vector<std::vector<int> > > new_face;
  while (face.size() > 3 && i < face.size())
  {
    int indexA = i % face.size();
    int indexB = (i + 1) % face.size();
    int indexC = (i + 2) % face.size();
    t_vec3 pointA = vertex[face[indexA][0] - 1];
    t_vec3 pointB = vertex[face[indexB][0] - 1];
    t_vec3 pointC = vertex[face[indexC][0] - 1];
    t_vec3 faceNormal = findFaceNormal(pointA, pointB, pointC);

    t_vec3 ab = pointB - pointA;
    t_vec3 bc = pointC - pointB;
    t_vec3 curv = Math::cross_vec3(ab, bc);
    Math::normalize_vec3(curv);

    int dot = Math::dot_vec3(curv, faceNormal);
    if (dot < 0)
    {
      i++;
      continue;
    }
    std::vector<t_vec3> pointlist;
    for (size_t j = 0; j < face.size() - 3; j++)
      pointlist.push_back(vertex[face[(i + j + 3) % face.size()][0] - 1]);
    if (!isAnyPointInTriangle(pointA, pointB, pointC, pointlist))
    {
      std::vector<std::vector<int> > tmpface = {face[indexA], face[indexB], face[indexC]};
      new_face.push_back(tmpface);
      face.erase(face.begin() + indexB);
    }
    else
      i++;
  }
  if (face.size() > 3)
    throw std::runtime_error("have face more than 3 after earclipping");
  new_face.push_back(face);
  return new_face;
}

t_vec3 findFaceNormal(t_vec3 a, t_vec3 b, t_vec3 c)
{
  t_vec3 AB = { b.x - a.x, b.y - a.y, b.z - a.z };
  t_vec3 AC = { c.x - a.x, c.y - a.y, c.z - a.z };

  t_vec3 N = Math::cross_vec3(AB, AC);
  Math::normalize_vec3(N);
  return N;
}

void Object::parsMaterial(void)
{
  if (_mtlFile == "")
    return ;

	std::fstream	file;
  
  // connect path with file
  size_t pos = _objFile.rfind('/');
  if (pos != std::string::npos)
      _mtlFile = _objFile.substr(0, pos + 1) + _mtlFile;

	if (!checkFileType(_mtlFile, ".mtl"))
		throw std::runtime_error("file name not a material type (.mtl).");

	file.open(_mtlFile, std::ios::in);
	if (!file.is_open())
		throw std::runtime_error("file material not found.");

	parseFileMtl(file);
  file.close();
}

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <algorithm>
void  textureCollect(std::istringstream& line, t_texture& texture, std::string objFile)
{
  if (line.str().find("-s") != std::string::npos)
  {
    std::string flag;
    line >> flag;
    line >> texture.__scale.x >> texture.__scale.y >> texture.__scale.z;
  }
  else
  {
    texture.__scale.x = 1;
    texture.__scale.y = 1;
    texture.__scale.z = 1;
  }
  std::string fileName;
  std::getline(line, fileName);
  fileName = strtrim(fileName);
  size_t pos = objFile.rfind('/');
  if (pos != std::string::npos)
      fileName = objFile.substr(0, pos + 1) + fileName;
  std::replace(fileName.begin(), fileName.end(), '\\', '/');
  unsigned char* data = stbi_load(fileName.c_str(), &texture.__width, &texture.__height, &texture.__format, 0); 

  if (data == nullptr)
    throw std::runtime_error("loading file '" + fileName + "' image error");

  int dataSize = texture.__width * texture.__height * texture.__format;
  texture.__data.clear();
  texture.__data.insert(texture.__data.end(), data, data + dataSize);

  stbi_image_free(data);
}

void	Object::parseFileMtl(std::fstream& file)
{
	try{
		std::string	input_line;

		while (std::getline(file, input_line))
		{
			std::istringstream line(strtrim(input_line));
			std::string word;

			if (line.str().size() == 0 || line.str().at(0) == '#')
				continue ;
			line >> word;
      if (word == "newmtl")
      {
        t_material mtl{};
        std::getline(line, mtl.__name);
        _meterial.push_back(mtl);
      }
      else if (_meterial.size() == 0)
        continue ;

      t_material& mtl = _meterial.at(_meterial.size() -1);
      if (word == "map_Kd")
      {
        mtl.__map_Kd.__enable = true;
        textureCollect(line, mtl.__map_Kd, _objFile);
      }
      else if (word == "d")
        line >> mtl.__d;
      else if (word == "Kd")
        line >> mtl.__kd.x >> mtl.__kd.y >> mtl.__kd.z;
      else if (word == "ka")
        line >> mtl.__ka.x >> mtl.__ka.y >> mtl.__ka.z;
      else if (word == "ks")
        line >> mtl.__ks.x >> mtl.__ks.y >> mtl.__ks.z;
      else if (word == "ns")
        line >> mtl.__ns;
      else if (word == "illum")
        line >> mtl.__illum;
		}
	} catch ( const std::runtime_error& e )
	{
		file.close();
		throw e;
	}
}

Transform& Object::getTransform(void)
{
  return _transform;
}
