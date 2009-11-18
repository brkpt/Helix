#ifndef SHADER_H
#define SHADER_H

#include <string>
#include "Kernel/RefCount.h"

namespace Helix {
class VertexDecl;

class Shader : public ReferenceCountable
{
public:
	Shader(const std::string &shaderName);
	Shader(const std::string &shaderName, LuaObject &shader, ID3D10EffectPool *effectPool);
	~Shader();

	void	Load(LuaObject &shaderObj, ID3D10EffectPool *effectPool);

	// Flag management
	void	SetLoadingFlag()	{ m_loading = true; }
	bool	IsLoading()			{ return m_loading; }
	void	ClearLoadingFlag()	{ m_loading = false; }
	void	SetNeedsProcessingFlag()	{ m_needsProcessing = true; }
	bool	NeedsProcessing()		{ return m_needsProcessing; }
	void	ClearProcessingFlag()	{ m_needsProcessing = false; }

	VertexDecl &	GetDecl() { return *m_decl; }
	ID3D10Effect *	GetEffect() { return m_pEffect; }

	void	SetShaderParameter(const std::string &paramName, D3DXMATRIX &value);
	void	SetShaderParameter(const std::string &paramName, float value);

private:
	std::string		m_shaderName;
	VertexDecl *	m_decl;
	ID3D10Effect *	m_pEffect;

	union {
		unsigned long	flags;
		struct {
			bool m_loading : 1;
			bool m_needsProcessing : 1;
		}; 
	};
};

} // namespace Helix
#endif // SHADER_H
