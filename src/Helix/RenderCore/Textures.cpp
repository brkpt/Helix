#include "Textures.h"
#include "RenderMgr.h"
#include "WICTextureLoader.h"

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
	// Load the file
	std::string fullPath = "Textures/";
	fullPath += filename;
	HANDLE hFile = CreateFile(fullPath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	_ASSERT(hFile != INVALID_HANDLE_VALUE);

	DWORD fileSize = GetFileSize(hFile, NULL);
	_ASSERT(fileSize != INVALID_FILE_SIZE);
	uint8_t *buffer = new uint8_t[fileSize];
	DWORD bytesRead = 0;
	BOOL retVal = ReadFile(hFile, buffer, fileSize, &bytesRead, NULL);
	_ASSERT(retVal);

	// Create the texture 
	ID3D11Device *pDevice = Helix::RenderMgr::GetInstance().GetDevice();
	HRESULT hr = DirectX::CreateWICTextureFromMemory(pDevice, buffer, fileSize, &tex->m_resource, &tex->m_shaderView);
	delete buffer;

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
