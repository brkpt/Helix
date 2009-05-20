#ifndef TEXTURE_H
#define TEXTURE_H

#include <d3d9.h>

namespace Helix {
class Texture : public ReferenceCountable
{
public:
	Texture();
	~Texture();

	bool	Load(const std::string &path);
	ID3D10ShaderResourceView *	GetResourceView() { return m_textureRV; }

private:
	ID3D10ShaderResourceView *	m_textureRV;
};

} // namespace Helix
#endif //TEXTURE_H
