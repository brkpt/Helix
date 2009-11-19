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
	void				SetName(const std::string &name) { m_name = name; }
	const std::string &	GetName() const { return m_name; }
	void				SetMeshName(const std::string &name) { m_meshName = name; }
	const std::string &	GetMeshName() const { return m_meshName; }
	const D3DXMATRIX &	GetWorldMatrix() const { return m_worldMatrix; }
	void				Render(int pass);

private:
	std::string		m_name;
	std::string		m_meshName;

	D3DXMATRIX		m_worldMatrix;
};
} // namespace

#endif INSTANCE_H
