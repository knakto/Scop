#pragma once

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>

typedef struct s_vec2
{
	float	x;
	float	y;
}	t_vec2;

typedef struct s_vec3
{
	float	x;
	float	y;
	float	z;
}	t_vec3;

/**
 * @Brief facelink store a iterator of point that want to link
 */
typedef struct s_facelink
{
	bool	is_have_v;
	bool	is_have_vn;
	bool	is_have_vt;
	std::map<size_t, t_vec3>::iterator	v;
	std::map<size_t, t_vec3>::iterator	vn;
	std::map<size_t, t_vec2>::iterator	vt;
}	t_facelink;

class ObjectData {
	typedef std::vector<t_facelink> t_face;

	private:
		ObjectData(void);
		void		parser(const std::string& file);
		void		readfile(std::fstream& file);
		void		facelink(std::istringstream&, size_t& current_pos);
		t_facelink	findPointOfFace(const int& v, const int& vt, const int& vn);

		std::string					_name;
		std::string					_mtllib;

		/**
		 * Its a face index, that meterail is start using.
		 */
		size_t						_usemtl;
		std::map<size_t, t_vec3>	_v;
		std::map<size_t, t_vec3>	_vn;
		std::map<size_t, t_vec2>	_vt;
		std::map<size_t, t_face>	_f;

	public:
		ObjectData(const std::string& file);
		~ObjectData(void);

		std::map<size_t, t_vec3>	getv(void) const;
		std::map<size_t, t_vec3>	getvn(void) const;
		std::map<size_t, t_vec2>	getvt(void) const;
		std::map<size_t, t_face>	getf(void) const;
		std::string					      getName(void) const;
		std::string					      getMtllibName(void) const;

    std::vector<float>        getVertexs(void) const;
    std::vector<unsigned int>          getIndices(void) const;
};
