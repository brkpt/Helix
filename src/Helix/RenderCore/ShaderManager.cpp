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
		return iter->second;

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

	std::string fullPath = "Shaders/";
	fullPath += shaderName;
	fullPath += ".lua";

	LuaState *state = LuaState::Create();
	_ASSERT(state != NULL);
	
	LoadLuaFileAsync<ShaderManager>(fullPath, *this, &ShaderManager::LuaLoadCallback);

	int retVal = state->DoFile(fullPath.c_str());
	_ASSERT(retVal == 0);

	LuaObject shaderObj = state->GetGlobals()["Shader"];
	_ASSERT(shaderObj.IsTable());

	shader = new Shader(shaderName,shaderObj,m_effectPool);
	_ASSERT(shader != NULL);

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
void ShaderManager::LuaLoadCallback(LuaState *state)
{
	LuaState::Destroy(state);
}

} // namespace Helix