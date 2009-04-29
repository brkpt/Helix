#include "stdafx.h"
#include "MeshManager.h"
#include "Mesh.h"

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


}