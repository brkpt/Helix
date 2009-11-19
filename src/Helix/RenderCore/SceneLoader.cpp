#include "stdafx.h"
#include "SceneLoader.h"

namespace Helix {

bool LoadScene(const std::string &sceneName)
{
	std::string fullPath;
	fullPath = "Scenes/";
	fullPath += sceneName;
	fullPath += ".lua";

	LuaStateAuto state;
	state = LuaState::Create();
	
	int retVal = state->DoFile(fullPath.c_str());
	_ASSERT(retVal==0);

	LuaObject meshList = state->GetGlobals()["MeshList"];
	_ASSERT(meshList.IsTable());

	int numMeshes = meshList.GetCount();
	for(int meshIndex=1;meshIndex <= meshList.GetCount(); meshIndex++)
	{
		LuaObject meshObj = meshList[meshIndex];
		LuaObject nameObj = meshObj["Name"];
		_ASSERT(nameObj.IsString());

		std::string meshName = nameObj.GetString();

		MeshManager::GetInstance().Load(meshName,meshObj);
		Instance *inst = InstanceManager::GetInstance().CreateInstance(meshName);
		inst->SetMeshName(meshName);

	}

	return true;
}
} // namespace Helix