#ifndef TEXTURES_H
#define TEXTURES_H

#include <string>
#include <map>

struct HXTexture
{
	HXTexture() : m_type(INVALID), m_raw(NULL) {}
	explicit HXTexture(ID3D11ShaderResourceView *view)	: m_type(SHADER_VIEW), m_shaderView(view) {}
	explicit HXTexture(ID3D11RenderTargetView *view)	: m_type(TARGET_VIEW), m_targetView(view) {}
	explicit HXTexture(ID3D11DepthStencilView *view)	: m_type(DEPTHSTENCIL_VIEW), m_depthStencilView(view) {}

	enum ViewType { INVALID=-1, SHADER_VIEW, TARGET_VIEW, DEPTHSTENCIL_VIEW };
	ViewType	m_type;
	union {
		ID3D11ShaderResourceView *	m_shaderView;
		ID3D11RenderTargetView *	m_targetView;
		ID3D11DepthStencilView *	m_depthStencilView;
		void *						m_raw;
	};
};

void		HXInitializeTextures();
HXTexture *	HXGetTextureByName(const std::string &textureName);
HXTexture *	HXLoadTexture(const std::string &textureName);
void		HXAddTexture(HXTexture *tex, const std::string &textureName);

#endif // TEXTURES_H
