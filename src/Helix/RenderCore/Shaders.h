#ifndef SHADERS_H
#define SHADERS_H

#include <string>
#include <map>
#include "Math/Matrix.h"

struct HXVertexDecl;

struct HXShader
{
	HXShader(const std::string &name) : m_decl(NULL), m_vshader(NULL), m_pshader(NULL), m_loading(false), m_needsProcessing(false) 
	{
		m_shaderName = name;
	}

	std::string				m_shaderName;
	HXVertexDecl *			m_decl;
	ID3D11VertexShader *	m_vshader;
	ID3D11PixelShader *		m_pshader;

	union {
		unsigned long	flags;
		struct {
			bool m_loading : 1;
			bool m_needsProcessing : 1;
		}; 
	};
}; 

void		HXInitializeShaders();
HXShader *	HXGetShaderByName(const std::string &shaderName);
HXShader *	HXLoadShader(const std::string &shaderName);
void		HXSetSharedParameter(const std::string &paramName, Helix::Matrix4x4 &matrix);


#endif // SHADERS_H
