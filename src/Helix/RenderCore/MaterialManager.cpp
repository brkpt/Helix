#include "stdafx.h"

using namespace Helix;

typedef std::map<const std::string, Material *>	MaterialMap;
MaterialMap		*m_database;

// ****************************************************************************
// ****************************************************************************
void HXInitializeMaterials()
{
	m_database = new MaterialMap;
}


// ****************************************************************************
// ****************************************************************************
Material * HXGetMaterial(const std::string &name)
{
	MaterialMap::const_iterator iter = m_database->find(name);
	if(iter != m_database->end())
		return iter->second;

	return NULL;
}

// ****************************************************************************
// ****************************************************************************
Material * HXLoadMaterial(const std::string &name)
{
	Material *mat = HXGetMaterial(name);
	if(mat != NULL)
		return mat;

	std::string fullPath = "Materials/";
	fullPath += name;
	fullPath += ".lua";

	LuaState *state = LuaState::Create();
	_ASSERT(state != NULL);
	
	int retVal = state->DoFile(fullPath.c_str());
	_ASSERT(retVal == 0);

	LuaObject shaderObj = state->GetGlobals()["Material"];
	_ASSERT(shaderObj.IsTable());

	mat = new Material(name,shaderObj);
	_ASSERT(mat != NULL);

	(*m_database)[name] = mat;
	return mat;
}
