#ifndef MATERIALMANAGER_H
#define MATERIALMANAGER_H

#include <map>
#include <string>
#include "RenderCore/Material.h"

void				HXInitializeMaterials();
Helix::Material *	HXGetMaterial(const std::string &name);
Helix::Material *	HXLoadMaterial(const std::string &name);


#endif // MATERIALMANAGER_H
