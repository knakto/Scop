#include "ObjectData.hpp"

static bool checkFileType(const std::string& name, const std::string& type);
static std::string strtrim(const std::string& str);
static void invalidFormat(size_t line_count);
static void	vec3Collect(std::istringstream& line, std::map<size_t, t_vec3>& collection, size_t& current_pos);
static void	vec2Collect(std::istringstream& line, std::map<size_t, t_vec2>& collection, size_t& current_pos);

ObjectData::ObjectData(const std::string& objectFile)
{
	this->parser(objectFile);
	for (std::map<size_t, t_vec3>::iterator it = this->_v.begin(); it != this->_v.end(); it++)
	{
		std::cout << "V vect: " << it->first << ", x: " << it->second.x << ", y: " << it->second.y << ", z: " << it->second.z << std::endl;
	}
	for (std::map<size_t, t_vec3>::iterator it = this->_vn.begin(); it != this->_vn.end(); it++)
	{
		std::cout << "VN vect: " << it->first << ", x: " << it->second.x << ", y: " << it->second.y << ", z: " << it->second.z << std::endl;
	}
	for (std::map<size_t, t_vec2>::iterator it = this->_vt.begin(); it != this->_vt.end(); it++)
	{
		std::cout << "VT vect: " << it->first << ", x: " << it->second.x << ", y: " << it->second.y << std::endl;
	}
	for (std::map<size_t, t_face>::iterator it = this->_f.begin(); it != this->_f.end(); it++)
	{
		t_face face = it->second;
    std::cout << "f: ";
		for (std::vector<t_facelink>::iterator fl = face.begin(); fl != face.end(); fl++)
		{
			if (fl->is_have_v)
				std::cout << fl->v->first;
			if (fl->is_have_vt)
			{
				std::cout << "/";
				std::cout << fl->vt->first;
			}
			if (fl->is_have_vn)
			{
				std::cout << "/";
				std::cout << fl->vn->first;
			}
			std::cout << " ";
		}
		std::cout << std::endl;
	}
}

ObjectData::~ObjectData(void) {}
std::map<size_t, t_vec3>				ObjectData::getv(void) const { return this->_v; }
std::map<size_t, t_vec3>				ObjectData::getvn(void) const { return this->_vn; }
std::map<size_t, t_vec2>				ObjectData::getvt(void) const { return this->_vt; };
std::map<size_t, ObjectData::t_face>	ObjectData::getf(void) const { return this->_f; };
std::string								ObjectData::getName(void) const { return this->_name; };
std::string								ObjectData::getMtllibName(void) const { return this->_mtllib; };

/*
 *	parser object.obj file
 *	- check filename is type ".obj"
 *	- check file is exists
 *	- read and sorting each type of value
 */
void	ObjectData::parser(const std::string& file)
{
	std::fstream	obj;

	if (!checkFileType(file, ".obj"))
		throw std::runtime_error("file name not an object type (.obj).");

	obj.open(file, std::ios::in);
	if (!obj.is_open())
		throw std::runtime_error("file object not found.");

	readfile(obj);
	obj.close();
}

void	ObjectData::facelink(std::istringstream& line, size_t& current_pos)
{
	std::vector<std::string>	linked;
	std::string					templink;
	size_t						count = 0;

	// split "f 1/2/3 3/2/1 2/3/1"
	// to [1/2/3] [3/2/1] [2/3/1]
	while ((line >> templink))
	{
		linked.push_back(templink);
		count++;
	}
	if (count == 0)
		throw std::runtime_error("Link face failed, Require argument.");

	t_face face;
	// split "[1/2/3]"
	// to [1], [2], [3]
	// and find that iterator point
	for (std::vector<std::string>::iterator it = linked.begin(); it != linked.end(); it++)
	{
		int					count_slash = std::count(it->begin(), it->end(), '/');
		std::istringstream	arg(*it);
		int					v = -1, vt = -1, vn = -1;
		char				s1, s2;

		if (count_slash > 2)
			throw std::runtime_error("have / more than 2");

		if (count_slash >= 0)
			arg >> v;
		if (count_slash >= 1)
		{
			arg >> s1;
			arg >> vt;
		}
		if (count_slash == 2)
		{
			arg >> s2;
			arg >> vn;
		}
		if (!arg.eof())
			throw std::runtime_error("buffer is not end.");
		face.push_back(findPointOfFace(v, vt, vn));
		// std::cout << "value: " << v << " " << vt << " " << vn << std::endl;
		// std::cout << "count f arg " << current_pos << " : " << count_slash << std::endl;
	}
	this->_f.insert(std::make_pair(current_pos++, face));
}

t_facelink	ObjectData::findPointOfFace(const int& v, const int& vt, const int& vn)
{
	t_facelink face;

	face.is_have_v = false;
	face.is_have_vn = false;
	face.is_have_vt = false;
	std::map<size_t, t_vec3>::iterator vFace = this->_v.find(v);
	std::map<size_t, t_vec3>::iterator vnFace = this->_vn.find(vn);
	std::map<size_t, t_vec2>::iterator vtFace = this->_vt.find(vt);
	if (vFace != this->_v.end())
		face.is_have_v = true;
	if (vnFace != this->_vn.end())
		face.is_have_vn = true;
	if (vtFace != this->_vt.end())
		face.is_have_vt = true;
	face.v = vFace;
	face.vn = vnFace;
	face.vt = vtFace;
	return face;
}

void	ObjectData::readfile(std::fstream& obj)
{
	try{
		std::string	input_line;
		size_t		line_count = 1;
		size_t		fCount = 1;
		size_t		vCount = 1;
		size_t		vnCount = 1;
		size_t		vtCount = 1;

		while (std::getline(obj, input_line))
		{
			std::istringstream line(strtrim(input_line));
			std::string word;

			if (line.str().size() == 0 || line.str().at(0) == '#')
				continue ;
			line >> word;
			if (word == "v")
				vec3Collect(line, this->_v, vCount);
			else if (word == "f")
				facelink(line, fCount);
			else if (word == "vt")
				vec2Collect(line, this->_vt, vtCount);
			else if (word == "vn")
				vec3Collect(line, this->_vn, vnCount);
			else if (word == "o")
				line >> this->_name;
			else if (word == "mtllib")
				line >> this->_mtllib;
			else if (word == "usemtl")
				this->_usemtl = fCount + 1;
			else if (word == "s")
			{
			}
			else
				invalidFormat(line_count);
			line_count++;
			// std::cout << line.str() << std::endl;
			// std::cout << word << std::endl;
		}
	} catch ( const std::exception& e )
	{
		obj.close();
		throw std::runtime_error(e.what());
	}
}

static bool checkFileType(const std::string& name, const std::string& type)
{
	size_t pos = name.rfind(".");
	return (name.substr(pos) == type);
}

static std::string strtrim(const std::string& str)
{
	size_t front_pos = str.find_first_not_of(" \t\n\r\f\v");
	size_t back_pos = str.find_last_not_of(" \t\n\r\f\v");
	if (front_pos == std::string::npos || back_pos  == std::string::npos)
		return "";
	else if (front_pos == 0 && back_pos == str.length() - 1)
		return str;
	return str.substr(front_pos, back_pos);
}

static void invalidFormat(size_t line_count)
{
	std::stringstream str_line_count;
	str_line_count << line_count;
	throw std::runtime_error("line " + str_line_count.str() + \
		": invalid format, please use [v/f/vt/vn/o/mtllib/usemtl] only");
}

static void	vec3Collect(std::istringstream& line, std::map<size_t, t_vec3>& collection, size_t& current_pos)
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
	collection.insert(std::make_pair(current_pos, vec));
	current_pos++;
}

static void	vec2Collect(std::istringstream& line, std::map<size_t, t_vec2>& collection, size_t& current_pos)
{
	float	x;
	float	y;
	t_vec2	vec;

	// std::cout << "line: " << line.str() << std::endl; 
	if (!(line >> x >> y))
		throw std::runtime_error("Line not a 2D vector.");
	vec.x = x;
	vec.y = y;
	collection.insert(std::make_pair(current_pos, vec));
	current_pos++;
}
