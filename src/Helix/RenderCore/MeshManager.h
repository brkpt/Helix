#ifndef MESHMANAGER_H
#define MESHMANAGER_H

#include <string>
#include <map>
#include "LuaPlus.h"

namespace Helix {

class Mesh;

class MeshManager
{
public:
	static void Create()
	{
		GetInstance();
	}

	static MeshManager & MeshManager::GetInstance()
	{
		static MeshManager	instance;
		return instance;
	}

	~MeshManager() {};

	Mesh *	GetMesh(const std::string &meshName);
	Mesh *	Load(const std::string &meshName);
	Mesh *	Load(const std::string &meshName, const std::string &filename);
	Mesh *	Load(const std::string &meshname, LuaPlus::LuaObject &meshObj);

private:
	MeshManager() {}
	MeshManager(const MeshManager &other) {}
	MeshManager & operator=(const MeshManager &other) {}

	typedef std::map<const std::string, Mesh *> MeshMap;
	MeshMap		m_database;
};

} // namespace Helix

#endif