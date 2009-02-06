#ifndef MATERIAL_H
#define MATERIAL_H

class Shader;
class Texture;

class Material
{
public:
	Material(const std::string &matName, LuaObject &object);

private:
	std::string		m_name;
	Shader *		m_shader;
	Texture *		m_texture;
};

#endif // MATERIAL_H