#include "stdafx.h"

namespace Helix {
// ****************************************************************************
// ****************************************************************************
TextureManager::TextureManager()
{
}

TextureManager::~TextureManager()
{
}

// ****************************************************************************
// ****************************************************************************
void TextureManager::AddTexture(Helix::Texture *tex, const std::string &textureName)
{
	TextureMap::const_iterator iter = m_database.find(textureName);

	_ASSERT(iter == m_database.end());

	m_database[textureName] = tex;
}

// ****************************************************************************
// ****************************************************************************
Texture * TextureManager::GetTexture(const std::string &textureName)
{
	TextureMap::const_iterator iter = m_database.find(textureName);
	if(iter == m_database.end())
	{
		return NULL;
	}

	return iter->second;
}

// ****************************************************************************
// ****************************************************************************
Texture * TextureManager::LoadTexture(const std::string &textureName)
{
	Texture *tex = GetTexture(textureName);
	if(tex != NULL)
		return tex;

	tex = new Texture;
	bool retVal = tex->Load(textureName);
	_ASSERT(retVal);

	m_database[textureName] = tex;
	return tex;

}

} //  namespace Helix