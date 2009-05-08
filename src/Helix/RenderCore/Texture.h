#ifndef TEXTURE_H
#define TEXTURE_H

#include <d3d9.h>

namespace Helix {
class Texture
{
public:
	Texture();
	~Texture();

	bool	Load(const std::string &path);
	IDirect3DTexture9 *	TexData() { return m_texture; }

private:
	IDirect3DTexture9	*m_texture;
};

} // namespace Helix
#endif //TEXTURE_H
