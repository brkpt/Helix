#include "stdafx.h"

namespace Helix {
// ****************************************************************************
// ****************************************************************************
ShaderManager::ShaderManager()
: m_effectPool(NULL)
{
	// Create a shared effects parameter pool
	HRESULT hr = D3DXCreateEffectPool(&m_effectPool);
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
	Load("shared");
}

// ****************************************************************************
// ****************************************************************************
void ShaderManager::SetSharedParameter(const std::string &paramName, D3DXMATRIX &matrix)
{
	// Get our shared shader
	Shader *shader = GetShader("shared");
	_ASSERT(shader != NULL);

	shader->SetShaderParameter(paramName, matrix);
}

} // namespace Helix