#include "stdafx.h"
#include "InstanceManager.h"
#include "Instance.h"

// ****************************************************************************
// ****************************************************************************
InstanceManager::InstanceManager()
{
}

// ****************************************************************************
// ****************************************************************************
Instance * InstanceManager::Get(const std::string &name)
{
	InstanceMap::const_iterator iter = m_database.find(name);
	if( iter != m_database.end() )
		return iter->second;

	return NULL;
}

// ****************************************************************************
// ****************************************************************************
Instance * InstanceManager::Load(const std::string &name)
{
	Instance *inst = Get(name);
	if(inst != NULL)
		return inst;

	std::string fullPath = "Meshes/";
	fullPath += name;
	fullPath += ".lua";

	LuaState *state = LuaState::Create();
	_ASSERT(state != NULL);
	
	int retVal = state->DoFile(fullPath.c_str());
	_ASSERT(retVal == 0);

	LuaObject shaderObj = state->GetGlobals()["MeshList"];
	_ASSERT(shaderObj.IsTable());

	inst = new Instance;
	_ASSERT(inst != NULL);

	m_database[name] = inst;
	return inst;

}