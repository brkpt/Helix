#include "Textures.h"
#include "RenderMgr.h"

typedef std::map<const std::string, HXTexture *>	TextureMap;

struct TextureState
{
	TextureMap		m_database;
};

TextureState *	m_textureState = NULL;

// ****************************************************************************
// ****************************************************************************
void HXInitializeTextures()
{
	_ASSERT(m_textureState == NULL);
	m_textureState = new TextureState;
}

// ****************************************************************************
// ****************************************************************************
void HXAddTexture(HXTexture *tex, const std::string &textureName)
{
	TextureMap::const_iterator iter = m_textureState->m_database.find(textureName);

	_ASSERT(iter == m_textureState->m_database.end());

	m_textureState->m_database[textureName] = tex;
}

// ****************************************************************************
// ****************************************************************************
HXTexture * HXGetTextureByName(const std::string &textureName)
{
	TextureMap::const_iterator iter = m_textureState->m_database.find(textureName);
	if(iter == m_textureState->m_database.end())
	{
		return NULL;
	}

	return iter->second;
}

// ****************************************************************************
// ****************************************************************************
bool TextureLoad(HXTexture *tex, const std::string &filename)
{
	std::string fullPath = "Textures/";
	fullPath += filename;

	ID3D11Device *pDevice = Helix::RenderMgr::GetInstance().GetDevice();

	D3DX11_IMAGE_LOAD_INFO loadInfo;
	ZeroMemory( &loadInfo, sizeof(D3DX11_IMAGE_LOAD_INFO) );
	loadInfo.BindFlags = D3D10_BIND_SHADER_RESOURCE;
	loadInfo.Format = DXGI_FORMAT_BC1_UNORM;

	tex->m_type = HXTexture::SHADER_VIEW;
	ID3D11ShaderResourceView *pSRView = NULL;
	HRESULT hr = D3DX11CreateShaderResourceViewFromFile( pDevice, fullPath.c_str(), &loadInfo, NULL, &tex->m_shaderView, NULL );
	return SUCCEEDED(hr);
}

// ****************************************************************************
// ****************************************************************************
HXTexture * HXLoadTexture(const std::string &textureName)
{
	HXTexture *tex = HXGetTextureByName(textureName);
	if(tex != NULL)
		return tex;

	tex = new HXTexture;
	bool retVal = TextureLoad(tex, textureName);
	_ASSERT(retVal);

	m_textureState->m_database[textureName] = tex;
	return tex;

}
