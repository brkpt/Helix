#include "stdafx.h"
#include "RenderMgr.h"

namespace Helix {
// ****************************************************************************
// ****************************************************************************
Texture::Texture()
: m_textureRV(NULL)
{
}

// ****************************************************************************
// ****************************************************************************
Texture::~Texture()
{
	if(m_textureRV)
	{
		m_textureRV->Release();
		m_textureRV = NULL;
	}
}
// ****************************************************************************
// ****************************************************************************
bool Texture::Load(const std::string &filename)
{
	std::string fullPath = "Textures/";
	fullPath += filename;

	ID3D10Device *pDevice = RenderMgr::GetInstance().GetDevice();

	D3DX10_IMAGE_LOAD_INFO loadInfo;
	ZeroMemory( &loadInfo, sizeof(D3DX10_IMAGE_LOAD_INFO) );
	loadInfo.BindFlags = D3D10_BIND_SHADER_RESOURCE;
	loadInfo.Format = DXGI_FORMAT_BC1_UNORM;

	ID3D10ShaderResourceView *pSRView = NULL;
	HRESULT hr = D3DX10CreateShaderResourceViewFromFile( pDevice, fullPath.c_str(), &loadInfo, NULL, &m_textureRV, NULL );
	return SUCCEEDED(hr);
}

} // namespace Helix
