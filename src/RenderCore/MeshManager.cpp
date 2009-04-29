#include "stdafx.h"

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
		return mesh;

	mesh = new Mesh;
	mesh->Load(meshName);
	m_database[meshName] = mesh;
	return mesh;
}