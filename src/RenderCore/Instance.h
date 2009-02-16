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

	bool	Load(const std::string &name, LuaObject &obj);
	Mesh &	GetMesh() { return *m_mesh; }
	void	SetMesh(Mesh *newMesh);
	void	Render(int pass);

private:
	std::string		m_name;
	Mesh *			m_mesh;	
};

#endif INSTANCE_H
