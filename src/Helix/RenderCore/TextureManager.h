#ifndef TEXTUREMGR_H
#define TEXTUREMGR_H

#include <string>
#include <map>

namespace Helix {
class Texture;
class TextureManager
{
public:
	static TextureManager &GetInstance()
	{
		static TextureManager	instance;
		return instance;
	}
	~TextureManager();

	Texture *	GetTexture(const std::string &textureName);
	Texture *	LoadTexture(const std::string &textureName);
	void		AddTexture(Texture *tex, const std::string &textureName);

private:
	TextureManager();
	TextureManager(const TextureManager &other) {}
	TextureManager &	operator=(const TextureManager &other) {}

	typedef std::map<const std::string, Texture *>	TextureMap;
	TextureMap		m_database;
};

} // namespace Helix

#endif // TEXTUREMGR_H