#ifndef INSTANCE_H
#define INSTANCE_H

#include <string>
#include "LuaPlus.h"

class Material;
class Mesh;

class Instance 
{
public:
	Instance();
	~Instance();

	bool				Load(const std::string &name, LuaObject &obj);
	void				SetMeshName(const std::string &name) { m_meshName = name; }
	const std::string &	GetMeshName() const { return m_meshName; }
	void				Render(int pass);

private:
	std::string		m_name;
	std::string		m_meshName;
};

#endif INSTANCE_H
