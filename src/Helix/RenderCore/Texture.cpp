#include "stdafx.h"
#include "RenderMgr.h"

namespace Helix {
// ****************************************************************************
// ****************************************************************************
Texture::Texture()
: m_shaderView(NULL)
, m_targetView(NULL)
, m_type(INVALID)
{
}

// ****************************************************************************
// ****************************************************************************
Texture::~Texture()
{
	switch(m_type)
	{
		case SHADER_VIEW:
			m_shaderView->Release();
			m_shaderView = NULL;
			break;

		case TARGET_VIEW:
			m_targetView->Release();
			m_targetView = NULL;
			break;
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

	m_type = SHADER_VIEW;
	ID3D10ShaderResourceView *pSRView = NULL;
	HRESULT hr = D3DX10CreateShaderResourceViewFromFile( pDevice, fullPath.c_str(), &loadInfo, NULL, &m_shaderView, NULL );
	return SUCCEEDED(hr);
}

} // namespace Helix
