#ifndef SHADER_H
#define SHADER_H

#include <string>

namespace Helix {
class VertexDecl;

class Shader
{
public:
	Shader(const std::string &shaderName, LuaObject &shader, ID3DXEffectPool *effectPool);
	~Shader();

	VertexDecl &	GetDecl() { return *m_decl; }
	ID3DXEffect *	GetEffect() { return m_pEffect; }

	void	SetShaderParameter(const std::string &paramName, D3DXMATRIX &value);
	void	SetShaderParameter(const std::string &paramName, float value);

private:
	std::string		m_shaderName;
	VertexDecl *	m_decl;
	ID3DXEffect *	m_pEffect;
};

} // namespace Helix
#endif // SHADER_H
