#ifndef TEXTURE_H
#define TEXTURE_H

#include <d3d9.h>

namespace Helix {
class Texture : public ReferenceCountable
{
public:
	Texture();
	~Texture();

	enum ViewType { INVALID=-1, SHADER_VIEW, TARGET_VIEW };

	bool	Load(const std::string &path);
	ID3D10ShaderResourceView *	GetShaderView() { return m_shaderView; }
	ID3D10RenderTargetView *	GetTargetView() { return m_targetView; }
	ViewType	Type() { return m_type; }

private:
	
	ViewType	m_type;
	union {
		ID3D10ShaderResourceView *	m_shaderView;
		ID3D10RenderTargetView *	m_targetView;
	};
};

} // namespace Helix
#endif //TEXTURE_H
