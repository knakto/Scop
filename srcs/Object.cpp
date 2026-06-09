#include "Object.hpp"
#include <sstream>
#include <stdexcept>

Object::~Object(void) {}
Object::Object(const std::string& fileName)
{
	std::fstream	file;

	if (!checkFileType(fileName, ".obj"))
		throw std::runtime_error("file name not an object type (.obj).");

	file.open(fileName, std::ios::in);
	if (!file.is_open())
		throw std::runtime_error("file object not found.");

	parseFile(file);
	file.close();
}

bool checkFileType(const std::string& name, const std::string& type)
{
	size_t pos = name.rfind(".");
	return (name.substr(pos) == type);
}

void	Object::parseFile(std::fstream& file)
{
	try{
		std::string	input_line;
		size_t		line_count = 1;

		while (std::getline(file, input_line))
		{
			std::istringstream line(strtrim(input_line));
			std::string word;

			if (line.str().size() == 0 || line.str().at(0) == '#')
				continue ;
			line >> word;
			if (word == "v")
				vec3Collect(line, _v);
			else if (word == "vt")
				vec2Collect(line, _vt);
			else if (word == "vn")
				vec3Collect(line, _vn);
			else if (word == "f")
				faceCollect(line, _f);
			else if (word == "o")
				line >> this->_name;
			else if (word == "mtllib"){}
			else if (word == "usemtl"){}
			else if (word == "s"){}
			else
				invalidFormat(line_count);
			line_count++;
		}
	} catch ( const std::exception& e )
	{
		file.close();
		throw std::runtime_error(e.what());
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
	vec.y = y;
	collection.push_back(vec);
}

void faceCollect(std::istringstream& line, std::vector<std::vector<std::vector<int> > >& collection)
{
	std::vector<std::string>  linked;
	std::string				        templink;
	size_t						        count = 0;

	// split "f 1/2/3 3/2/1 2/3/1"
	// to [1/2/3] [3/2/1] [2/3/1]
	while ((line >> templink))
	{
		linked.push_back(templink);
		count++;
	}
	if (count == 0)
	  throw std::runtime_error("Link face failed, Require argument.");

  std::vector<std::vector<int> > face;
	// split "[1/2/3]"
	// to [1], [2], [3]
	// and find that iterator point
	for (std::vector<std::string>::iterator it = linked.begin(); it != linked.end(); it++)
	{
    std::vector<std::string>  v;
    std::stringstream         ss(*it);
    std::vector<int>          tmpface;
    std::string               token;

    while (std::getline(ss, token, '/'))
      v.push_back(token);

    if (v.size() > 3)
      throw std::runtime_error("face must have only 3 argument v/vt/vn.");

    for (int i = 0; i < 3; i++)
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
        for (int j = 0; j < v[i].length(); j++)
          if (!(v[i][j] >= '0' && v[i][j] <= '9'))
            throw std::runtime_error("face musbe number integer and not minus");
      }
      ss << v[i];
      ss >> number;
      tmpface.push_back(number);
    }
    face.push_back(tmpface);
  }

  // protect f 1 1 1 1 1 (more than 4 vertex)
  if (face.size() > 4)
    throw std::runtime_error("This program no handle no more 4 facelink.");
  else if (face.size() < 3)
    throw std::runtime_error("can not process less than 3 facelink.");
  else if (face.size() == 4)
  {
    std::vector<std::vector<int> > face1;
    std::vector<std::vector<int> > face2;

    face1.push_back(face[0]);
    face1.push_back(face[1]);
    face1.push_back(face[3]);

    face2.push_back(face[3]);
    face2.push_back(face[1]);
    face2.push_back(face[2]);

    collection.push_back(face1);
    collection.push_back(face2);
  }
  else
    collection.push_back(face);
}

std::vector<float>        Object::getVertexs(void) const
{
  std::vector<float> res;

  // for (std::vector<t_vec3>::const_iterator it = _v.begin(); it != _v.end(); it++)
  // {
  //   res.push_back(it->x);
  //   res.push_back(it->y);
  //   res.push_back(it->z);
  // }
  for (std::vector<std::vector<std::vector<int> > >::const_iterator it = _f.begin(); it != _f.end(); it++)
  {
    for (std::vector<std::vector<int> >::const_iterator f = it->begin(); f != it->end(); f++)
    {
      t_vec3 vertex = _v[f->at(0) - 1];
      res.push_back(vertex.x);
      res.push_back(vertex.y);
      res.push_back(vertex.z);

      if (f->at(1) == -1)
      {
        res.push_back(0);
        res.push_back(0);
      }
      else
      {
        t_vec2 vertexTexture = _vt[f->at(1) - 1];
        res.push_back(vertexTexture.x);
        res.push_back(vertexTexture.y);
      }


      t_vec3 vertexNormal;
      if (f->at(2) == -1)
        vertexNormal = findVn(_v[it->at(0)[0] - 1], _v[it->at(1)[0] - 1], _v[it->at(2)[0] - 1]);
      else
        vertexNormal = _vn[f->at(2) - 1];
      res.push_back(vertexNormal.x);
      res.push_back(vertexNormal.y);
      res.push_back(vertexNormal.z);
    }
  }
  // int a = 0;
  // for (auto f: res)
  // {
  //   std::cout << "v: " << f << " ";
  //   a++;
  //   if (a % 3 == 0)
  //     std::cout << std::endl;
  // }
  return res;
}

std::vector<unsigned int> Object::getIndices(void) const
{
  std::vector<unsigned int> res;
  unsigned int i = 0;

  for (std::vector<std::vector<std::vector<int> > >::const_iterator it = _f.begin(); it != _f.end(); it++)
  {
    for (std::vector<std::vector<int> >::const_iterator f = it->begin(); f != it->end(); f++)
      // res.push_back(f->at(0) - 1);
      res.push_back(i++);
  }
  // int a = 0;
  // for (auto f: res)
  // {
  //   std::cout << f << " ";
  //   a++;
  //   if (a % 3 == 0)
  //     std::cout << std::endl;
  // }
  // std::cout << "cut" << std::endl;
  return res;
}

t_vec3 findVn(t_vec3 a, t_vec3 b, t_vec3 c)
{
    
    // ขั้นที่ 1: สร้างเวกเตอร์ A และ B
    t_vec3 AB = { b.x - a.x, b.y - a.y, b.z - a.z };
    t_vec3 AC = { c.x - a.x, c.y - a.y, c.z - a.z };
    
    // ขั้นที่ 2: สูตร Cross Product
    t_vec3 N;
    N.x = (AB.y * AC.z) - (AB.z * AC.y);
    N.y = (AB.z * AC.x) - (AB.x * AC.z);
    N.z = (AB.x * AC.y) - (AB.y * AC.x);
    
    // ขั้นที่ 3: ทำให้ยาว 1 หน่วย (Normalize) - สำคัญมากสำหรับระบบแสง!
    float length = std::sqrt(N.x * N.x + N.y * N.y + N.z * N.z);
    if (length > 0.0f) {
        N.x /= length;
        N.y /= length;
        N.z /= length;
    }
    
    return N;
}
