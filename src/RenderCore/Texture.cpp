#include "stdafx.h"
#include "RenderMgr.h"
#include "Texture.h"

// ****************************************************************************
// ****************************************************************************
Texture::Texture()
: m_texture(NULL)
{
}

// ****************************************************************************
// ****************************************************************************
Texture::~Texture()
{
	if(m_texture)
	{
		m_texture->Release();
		m_texture = NULL;
	}
}
// ****************************************************************************
// ****************************************************************************
bool Texture::Load(const std::string &filename)
{
	std::string fullPath = "Textures/";
	fullPath += filename;

	IDirect3DDevice9 *pDevice = RenderMgr::GetInstance().GetDevice();

	HRESULT hr;
	hr = D3DXCreateTextureFromFile(pDevice,fullPath.c_str(),&m_texture);
	_ASSERT(hr == D3D_OK);

	return SUCCEEDED(hr);
}

