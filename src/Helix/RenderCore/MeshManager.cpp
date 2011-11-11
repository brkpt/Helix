#include "stdafx.h"

namespace Helix {

// ****************************************************************************
// ****************************************************************************
Mesh * MeshManager::GetMesh(const std::string &meshName)
{
	MeshMap::const_iterator iter = m_database.find(meshName);
	if(iter != m_database.end())
		return iter->second;

	return NULL;
}

// ****************************************************************************
// ****************************************************************************
Mesh * MeshManager::Load(const std::string &meshName)
{
	Mesh *mesh = GetMesh(meshName);
	if(mesh != NULL)
	{
		return mesh;
	}

	mesh = new Mesh;
	mesh->Load(meshName);
	m_database[meshName] = mesh;

	return mesh;
}

// ****************************************************************************
// ****************************************************************************
Mesh * MeshManager::Load(const std::string &meshName, const std::string &filename)
{
	Mesh *mesh = GetMesh(meshName);
	_ASSERT(mesh == NULL);

	mesh = new Mesh;
	mesh->Load(filename);
	m_database[meshName] = mesh;

	return mesh;
}
// ****************************************************************************
// ****************************************************************************
Mesh * MeshManager::Load(const std::string &meshName, LuaPlus::LuaObject &meshObj)
{
	Mesh *mesh = GetMesh(meshName);
	if(mesh != NULL)
	{
		return mesh;
	}

	mesh = new Mesh;
	mesh->Load(meshObj);
	m_database[meshName] = mesh;

	return mesh;
}

} // namespace Helix