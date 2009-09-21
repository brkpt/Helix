#ifndef SHADER_H
#define SHADER_H

#include <string>
#include "Kernel/RefCount.h"

namespace Helix {
class VertexDecl;

class Shader : public ReferenceCountable
{
public:
	Shader(const std::string &shaderName, LuaObject &shader, ID3D10EffectPool *effectPool);
	~Shader();

	VertexDecl &	GetDecl() { return *m_decl; }
	ID3D10Effect *	GetEffect() { return m_pEffect; }

	void	SetShaderParameter(const std::string &paramName, D3DXMATRIX &value);
	void	SetShaderParameter(const std::string &paramName, float value);

private:
	std::string		m_shaderName;
	VertexDecl *	m_decl;
	ID3D10Effect *	m_pEffect;
};

} // namespace Helix
#endif // SHADER_H
