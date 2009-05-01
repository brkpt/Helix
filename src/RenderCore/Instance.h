#ifndef INSTANCE_H
#define INSTANCE_H

#include <string>
#include "LuaPlus.h"
#include "Kernel/RefCount.h"

namespace Helix
{
class Material;
class Mesh;

class Instance : public ReferenceCountable
{
public:
	Instance();
	~Instance();

	bool				Load(const std::string &name, LuaObject &obj);
	void				SetMeshName(const std::string &name) { m_meshName = name; }
	const std::string &	GetMeshName() const { return m_meshName; }
	const std::string &	GetName() const { return m_name; }
	void				Render(int pass);

private:
	std::string		m_name;
	std::string		m_meshName;
};
} // namespace

#endif INSTANCE_H
