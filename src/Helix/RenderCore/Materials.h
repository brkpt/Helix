#ifndef MATERIALS_H
#define MATERIALS_H

#include <map>
#include <string>

struct HXMaterial
{
	std::string		m_name;
	std::string		m_shaderName;
	std::string		m_textureName;
};

void			HXInitializeMaterials();
HXMaterial *	HXGetMaterial(const std::string &name);
HXMaterial *	HXLoadMaterial(const std::string &name);


#endif // MATERIALS_H
