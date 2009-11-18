#include "stdafx.h"
#include "RenderMgr.h"
#include "ThreadLoad/ThreadLoad.h"

namespace Helix {
// ****************************************************************************
// ****************************************************************************
ShaderManager::ShaderManager()
: m_effectPool(NULL)
{
	// Create a shared effects parameter pool
	//HRESULT hr = D3DXCreateEffectPool(&m_effectPool);
	std::string fullPath = "Effects/shared.fx";
	ID3D10Device *pDevice = RenderMgr::GetInstance().GetDevice();
	HRESULT hr = D3DX10CreateEffectPoolFromFile(fullPath.c_str(), NULL, NULL, "fx_4_0", D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY, 0, pDevice, NULL, &m_effectPool, NULL, NULL);
	_ASSERT(hr == S_OK);
}

ShaderManager::~ShaderManager()
{
}

// ****************************************************************************
// ****************************************************************************
Shader * ShaderManager::GetShader(const std::string &name)
{
	ShaderMap::const_iterator iter = m_shaderMap.find(name);
	if(iter != m_shaderMap.end())
	{
		//while(iter->second->IsLoading())
		//{
		//	Sleep(1);
		//}
		return iter->second;
	}

	return NULL;
}

// ****************************************************************************
// ****************************************************************************
Shader * ShaderManager::Load(const std::string &shaderName)
{
	Shader *shader = GetShader(shaderName);
	if(shader != NULL)
	{
		return shader;
	}

	shader = new Shader(shaderName);

	std::string fullPath = "Shaders/";
	fullPath += shaderName;
	fullPath += ".lua";

	//AsyncData *asyncData = new AsyncData(shader, m_effectPool);
	//shader->SetLoadingFlag();
	//LoadFileAsync(fullPath, &ShaderManager::ShaderLoadCallback, asyncData);

	LuaStateAuto state;
	state = LuaState::Create();

	int retVal = state->DoFile(fullPath.c_str());
	_ASSERT(retVal == 0);

	LuaObject shaderObj = state->GetGlobals()["Shader"];
	_ASSERT(shaderObj.IsTable());

	shader->Load(shaderObj, m_effectPool);

	m_shaderMap[shaderName] = shader;

	return shader;
}

// ****************************************************************************
// ****************************************************************************
void ShaderManager::LoadShared()
{
	//Load("shared");
}

// ****************************************************************************
// ****************************************************************************
void ShaderManager::SetSharedParameter(const std::string &paramName, D3DXMATRIX &matrix)
{
	// Get the shared parameter
	ID3D10Effect* pPoolEffect = m_effectPool->AsEffect();
	ID3D10EffectMatrixVariable *param = pPoolEffect->GetVariableByName( paramName.c_str() )->AsMatrix()	;
	_ASSERT(param != NULL);
	float *fArray = (float *)&matrix;
	param->SetMatrix( (float *)&matrix );
}

// ****************************************************************************
// ****************************************************************************
void ShaderManager::ShaderLoadCallback(void *buffer, long bufferSize, void *userData)
{
	// We need to NULL terminate our buffer before passing it to Lua
	char *zbuffer = new char[bufferSize+1];
	memcpy(zbuffer,buffer,bufferSize);
	zbuffer[bufferSize] = 0;
	delete [] buffer;

	LuaState *state = LuaState::Create();
	_ASSERT(state != NULL);

	int retVal = state->DoString(static_cast<char *>(zbuffer) );
	_ASSERT(retVal == 0);

	delete [] zbuffer;

	LuaObject shaderObj = state->GetGlobals()["Shader"];
	_ASSERT(shaderObj.IsTable());

	AsyncData *asyncData = static_cast<AsyncData *>(userData);

	asyncData->m_shader->Load(shaderObj,asyncData->m_effectPool);
	asyncData->m_shader->ClearLoadingFlag();
	
	delete asyncData;
	LuaState::Destroy(state);
}

} // namespace Helix