#ifndef TEXTURE_H
#define TEXTURE_H

#include <d3d9.h>

namespace Helix {
class Texture : public ReferenceCountable
{
public:
	Texture();
	explicit Texture(ID3D10ShaderResourceView *view);
	explicit Texture(ID3D10RenderTargetView *view);
	explicit Texture(ID3D10DepthStencilView *view);
	~Texture();

	enum ViewType { INVALID=-1, SHADER_VIEW, TARGET_VIEW, DEPTHSTENCIL_VIEW };

	bool	Load(const std::string &path);
	ID3D10ShaderResourceView *	GetShaderView() { return m_shaderView; }
	ID3D10RenderTargetView *	GetTargetView() { return m_targetView; }
	ID3D10DepthStencilView *	GetDepthStencilView() { return m_depthStencilView; }
	ViewType	Type() { return m_type; }

private:
	
	ViewType	m_type;
	union {
		ID3D10ShaderResourceView *	m_shaderView;
		ID3D10RenderTargetView *	m_targetView;
		ID3D10DepthStencilView *	m_depthStencilView;
		void *						m_raw;
	};
};

} // namespace Helix
#endif //TEXTURE_H
