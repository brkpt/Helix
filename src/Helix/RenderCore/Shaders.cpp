#include "stdafx.h"
#include "Shaders.h"
#include "RenderMgr.h"
#include "ThreadLoad/ThreadLoad.h"

typedef std::map<const std::string, HXShader *>	ShaderMap;
struct ShaderState
{
	ShaderMap			m_shaderMap;
	ID3D10EffectPool *	m_effectPool;
};

ShaderState *	m_shaderState = NULL;

// ****************************************************************************
// ****************************************************************************
struct AsyncData
{
	AsyncData(HXShader *shader, ID3D10EffectPool *effectPool) 
	: m_shader(shader)
	, m_effectPool(effectPool) 
	{}

	HXShader *			m_shader;
	ID3D10EffectPool *	m_effectPool;
};

// ****************************************************************************
// ****************************************************************************
void HXInitializeShaders()
{
	_ASSERT(m_shaderState == NULL);
	m_shaderState = new ShaderState;

	std::string fullPath = "Effects/shared.fx";
	ID3D10Device *pDevice = Helix::RenderMgr::GetInstance().GetDevice();
	HRESULT hr = D3DX10CreateEffectPoolFromFile(fullPath.c_str(), NULL, NULL, "fx_4_0", D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY, 0, pDevice, NULL, &m_shaderState->m_effectPool, NULL, NULL);
	_ASSERT(hr == S_OK);
}

// ****************************************************************************
// ****************************************************************************
HXShader * HXGetShaderByName(const std::string &name)
{
	ShaderMap::const_iterator iter = m_shaderState->m_shaderMap.find(name);
	if(iter != m_shaderState->m_shaderMap.end())
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
void HXLoadShader(HXShader &shader, LuaPlus::LuaObject &shaderObj, ID3D10EffectPool *effectPool)
{
	// Load our vertex declaration
	LuaObject obj = shaderObj["Declaration"];
	_ASSERT(obj.IsString());
	std::string name = obj.GetString();

	shader.m_decl = HXLoadVertexDecl(name);
	_ASSERT(shader.m_decl);

	// Load our effect
	obj = shaderObj["FX"];
	_ASSERT(obj.IsString());
	std::string fxName = obj.GetString();
	std::string fxPath = "Effects/";
	fxPath += fxName;
	fxPath += ".fx";

	DWORD dwShaderFlags = D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY;
#if defined(_DEBUG)
	dwShaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION | D3D10_SHADER_DEBUG ;
#endif
	
	ID3D10Device *pDevice = Helix::RenderMgr::GetInstance().GetDevice();
	HRESULT hr;
	ID3D10Blob *errorBlob;
	D3D10CreateBlob(1024,&errorBlob);
	hr = D3DX10CreateEffectFromFile(fxPath.c_str(), NULL,NULL,"fx_4_0",dwShaderFlags,D3D10_EFFECT_COMPILE_CHILD_EFFECT, pDevice, effectPool, NULL, &shader.m_pEffect, &errorBlob, NULL);
	if(hr != D3D_OK)
	{
		OutputDebugString(static_cast<char *>(errorBlob->GetBufferPointer()) );
	}
	_ASSERT(hr == D3D_OK);

	// Now create the layout if it hasn't been created already
	HXDeclBuildLayout(*(shader.m_decl),&shader);
}

// ****************************************************************************
// ****************************************************************************
HXShader * HXLoadShader(const std::string &shaderName)
{
	HXShader *shader = HXGetShaderByName(shaderName);
	if(shader != NULL)
	{
		return shader;
	}

	shader = new HXShader(shaderName);

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

	HXLoadShader(*shader,shaderObj, m_shaderState->m_effectPool);

	m_shaderState->m_shaderMap[shaderName] = shader;

	return shader;
}

// ****************************************************************************
// ****************************************************************************
void HXSetSharedParameter(const std::string &paramName, D3DXMATRIX &matrix)
{
	// Get the shared parameter
	ID3D10Effect* pPoolEffect = m_shaderState->m_effectPool->AsEffect();
	ID3D10EffectMatrixVariable *param = pPoolEffect->GetVariableByName( paramName.c_str() )->AsMatrix()	;
	_ASSERT(param != NULL);
	float *fArray = (float *)&matrix;
	param->SetMatrix( (float *)&matrix );
}

// ****************************************************************************
// ****************************************************************************
void HXShaderLoadedCallback(void *buffer, long bufferSize, void *userData)
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

	HXLoadShader(*(asyncData->m_shader),shaderObj,asyncData->m_effectPool);
	asyncData->m_shader->m_loading=false;
	
	delete asyncData;
	LuaState::Destroy(state);
}
