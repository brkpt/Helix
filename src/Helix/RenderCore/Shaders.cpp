#include "stdafx.h"
#include "Shaders.h"
#include "RenderMgr.h"
#include "ThreadLoad/ThreadLoad.h"

typedef std::map<const std::string, HXShader *>	ShaderMap;
struct ShaderState
{
	ShaderMap			m_shaderMap;
};

ShaderState *	m_shaderState = NULL;

// ****************************************************************************
// ****************************************************************************
struct AsyncData
{
	AsyncData(HXShader *shader) 
	: m_shader(shader)
	{}

	HXShader *			m_shader;
};

struct CONSTANT_BUFFER_FRAME
{
	D3DXVECTOR3		sunDirection;
	D3DXVECTOR3		sunColor;
	D3DXVECTOR3		ambientColor;
	D3DXMATRIX		projMatrix;
	D3DXMATRIX		invProj;
};

struct CONSTANT_BUFFFER_OBJECT
{
	D3DXMATRIX		worldViewMatrix;
	D3DXMATRIX		viweMatrix;
	D3DXMATRIX		invViewMatrix;
	D3DXMATRIX		view3x3;
	D3DXMATRIX		worldViewIT;
	D3DXMATRIX		invWorldViewProj;
	D3DXMATRIX		invViewProj;
};

// ****************************************************************************
// ****************************************************************************
void HXInitializeShaders()
{
	_ASSERT(m_shaderState == NULL);
	m_shaderState = new ShaderState;
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
void HXLoadShader(HXShader &shader, LuaPlus::LuaObject &shaderObj)
{
	// Load our vertex declaration
	LuaPlus::LuaObject obj = shaderObj["Declaration"];
	_ASSERT(obj.IsString());
	std::string name = obj.GetString();

	shader.m_decl = HXLoadVertexDecl(name);
	_ASSERT(shader.m_decl);

	obj = shaderObj["VSEntry"];
	_ASSERT(obj.IsString());
	std::string vsEntry = obj.GetString();

	obj = shaderObj["PSEntry"];
	_ASSERT(obj.IsString());
	std::string psEntry = obj.GetString();

	obj = shaderObj["VSProfile"];
	_ASSERT(obj.IsString());
	std::string vsProfile = obj.GetString();

	obj = shaderObj["PSProfile"];
	_ASSERT(obj.IsString());
	std::string psProfile = obj.GetString();

	// Load our effect
	obj = shaderObj["HLSL"];
	_ASSERT(obj.IsString());
	std::string fxName = obj.GetString();
	std::string fxPath = "Effects/";
	fxPath += fxName;

	DWORD dwShaderFlags = D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY;
#if defined(_DEBUG)
	dwShaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION | D3D10_SHADER_DEBUG ;
#endif

	ID3D11Device *pDevice = Helix::RenderMgr::GetInstance().GetDevice();

	// Load the vertex shader
	ID3DBlob *errorBlob = NULL;
	ID3DBlob *pShaderBlob = NULL;
	HRESULT hr = D3DX11CompileFromFile(fxPath.c_str(), NULL, NULL, vsEntry.c_str(),vsProfile.c_str(),dwShaderFlags, 0, NULL, &pShaderBlob, &errorBlob, NULL);
	if(hr != S_OK)
	{
		OutputDebugString(static_cast<char *>(errorBlob->GetBufferPointer()) );
	}
	_ASSERT(hr == S_OK);
	if(errorBlob)
		errorBlob->Release();
	errorBlob = NULL;

	// Now create the vertex shader
	hr = pDevice->CreateVertexShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &shader.m_vshader);
	_ASSERT(hr == S_OK);

	pShaderBlob = NULL;
	hr = D3DX11CompileFromFile(fxPath.c_str(), NULL, NULL, psEntry.c_str(), psProfile.c_str(), dwShaderFlags, 0, NULL, &pShaderBlob, &errorBlob, NULL);
	if(hr != S_OK)
	{
		OutputDebugString(static_cast<char *>(errorBlob->GetBufferPointer()) );
	}
	_ASSERT(hr == S_OK);
	if(errorBlob)
		errorBlob->Release();
	errorBlob = NULL;

	// Now create the pixel shader
	hr = pDevice->CreatePixelShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &shader.m_pshader);
	_ASSERT(hr == S_OK);

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

	LuaPlus::LuaStateAuto state;
	state = LuaPlus::LuaState::Create();

	int retVal = state->DoFile(fullPath.c_str());
	_ASSERT(retVal == 0);

	LuaPlus::LuaObject shaderObj = state->GetGlobals()["Shader"];
	_ASSERT(shaderObj.IsTable());

	HXLoadShader(*shader,shaderObj);

	m_shaderState->m_shaderMap[shaderName] = shader;

	return shader;
}

// ****************************************************************************
// ****************************************************************************
void HXSetSharedParameter(const std::string &paramName, D3DXMATRIX &matrix)
{
	//// Get the shared parameter
	//ID3D11Effect* pPoolEffect = m_shaderState->m_effectPool->AsEffect();
	//ID3D11EffectMatrixVariable *param = pPoolEffect->GetVariableByName( paramName.c_str() )->AsMatrix()	;
	//_ASSERT(param != NULL);
	//float *fArray = (float *)&matrix;
	//param->SetMatrix( (float *)&matrix );
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

	LuaPlus::LuaState *state = LuaPlus::LuaState::Create();
	_ASSERT(state != NULL);

	int retVal = state->DoString(static_cast<char *>(zbuffer) );
	_ASSERT(retVal == 0);

	delete [] zbuffer;

	// NOTE: This structure is not correct. 
	LuaPlus::LuaObject shaderObj = state->GetGlobals()["Shader"];
	_ASSERT(shaderObj.IsTable());

	AsyncData *asyncData = static_cast<AsyncData *>(userData);

	HXLoadShader(*(asyncData->m_shader),shaderObj);
	asyncData->m_shader->m_loading=false;
	
	delete asyncData;
	LuaPlus::LuaState::Destroy(state);
}
