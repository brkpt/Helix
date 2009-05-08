#ifndef MATERIAL_H
#define MATERIAL_H

#include "Kernel/RefCount.h"

namespace Helix {

class Shader;
class Texture;

class Material : public ReferenceCountable
{
public:
	Material(const std::string &matName, LuaObject &object);

	const std::string &	GetShaderName()		{ return m_shaderName; }
	const std::string &	GetTextureName()	{ return m_textureName; }

	void		SetViewMatrix(D3DXMATRIX *viewMatrix);
	void		SetProjMatrix(D3DXMATRIX *projMatrix);
	void		SetParameters();

private:
	std::string		m_name;
	std::string		m_shaderName;
	std::string		m_textureName;
};

} // namespace Helix

#endif // MATERIAL_H