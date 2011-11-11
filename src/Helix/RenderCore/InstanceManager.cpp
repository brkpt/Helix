#include "stdafx.h"
#include "RenderThread.h"

namespace Helix {
// ****************************************************************************
// ****************************************************************************
InstanceManager::InstanceManager()
{
}

// ****************************************************************************
// ****************************************************************************
Instance * InstanceManager::CreateInstance(const std::string &instanceName)
{
	Instance *inst = Get(instanceName);
	_ASSERT(inst == NULL);

	inst = new Instance;
	inst->SetName(instanceName);
	
	m_database[instanceName] = inst;
	return inst;
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

	LuaPlus::LuaState *state = LuaPlus::LuaState::Create();
	_ASSERT(state != NULL);
	
	int retVal = state->DoFile(fullPath.c_str());
	_ASSERT(retVal == 0);

	LuaPlus::LuaObject shaderObj = state->GetGlobals()["MeshList"];
	_ASSERT(shaderObj.IsTable());

	inst = new Instance;
	_ASSERT(inst != NULL);

	m_database[name] = inst;
	return inst;

}

// ****************************************************************************
// ****************************************************************************
void InstanceManager::SubmitInstances()
{
	InstanceMap::iterator iter = m_database.begin();

	while(iter != m_database.end())
	{
		Instance *inst = iter->second;
		SubmitInstance(*inst);
		++iter;
	}
}

} // namespace Helix