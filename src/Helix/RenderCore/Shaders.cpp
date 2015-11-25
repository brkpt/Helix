#include <D3Dcompiler.h>
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
	Helix::Vector3		sunDirection;
	Helix::Vector3		sunColor;
	Helix::Vector3		ambientColor;
	Helix::Matrix4x4	projMatrix;
	Helix::Matrix4x4	invProj;
};

struct CONSTANT_BUFFFER_OBJECT
{
	Helix::Matrix4x4	worldViewMatrix;
	Helix::Matrix4x4	viweMatrix;
	Helix::Matrix4x4	invViewMatrix;
	Helix::Matrix4x4	view3x3;
	Helix::Matrix4x4	worldViewIT;
	Helix::Matrix4x4	invWorldViewProj;
	Helix::Matrix4x4	invViewProj;
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
	std::string fxPath = "Shaders/";
	fxPath += fxName;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY;
#if defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_WARNINGS_ARE_ERRORS | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG;
#endif

	ID3D11Device *pDevice = Helix::RenderMgr::GetInstance().GetDevice();

	// Load the .hlsl file
	HANDLE hFile = CreateFile(fxPath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	_ASSERT(hFile != INVALID_HANDLE_VALUE);
	DWORD fileSize = GetFileSize(hFile, NULL);
	_ASSERT(fileSize != INVALID_FILE_SIZE);
	uint8_t *shaderBuffer = new uint8_t[fileSize];
	DWORD bytesRead = 0;
	BOOL retVal = ReadFile(hFile, shaderBuffer, fileSize, &bytesRead, NULL);
	_ASSERT(retVal);
	_ASSERT(bytesRead == fileSize);
	CloseHandle(hFile);

	// Compile the .hlsl into our vertex and pixel shaders
	// Compile the vertex shader
	ID3DBlob *errorBlob = NULL;
	ID3DBlob *pShaderBlob = NULL;

	// Compile vertex shader
	HRESULT hr = D3DCompile(shaderBuffer, fileSize, "Shaders\\", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, vsEntry.c_str(), vsProfile.c_str(), dwShaderFlags, 0, &pShaderBlob, &errorBlob);
	if(hr != S_OK)
	{
		// Display any errors
		OutputDebugString(static_cast<char *>(errorBlob->GetBufferPointer()) );
	}
	_ASSERT(hr == S_OK);
	if(errorBlob)
		errorBlob->Release();
	errorBlob = NULL;

	// Create vertex shader
	hr = pDevice->CreateVertexShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &shader.m_vshader);
	_ASSERT(hr == S_OK);

	// Create the layout for the vertex shader
	HXDeclBuildLayout(*(shader.m_decl),pShaderBlob);

	// Compile the pixel shader
	pShaderBlob->Release();
	pShaderBlob = NULL;
	hr = D3DCompile(shaderBuffer, fileSize, "Shaders\\", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, psEntry.c_str(), psProfile.c_str(), dwShaderFlags, 0, &pShaderBlob, &errorBlob);
	//hr = D3DX11CompileFromFile(fxPath.c_str(), NULL, NULL, psEntry.c_str(), psProfile.c_str(), dwShaderFlags, 0, NULL, &pShaderBlob, &errorBlob, NULL);
	if(hr != S_OK)
	{
		// Display any errors that occurred
		OutputDebugString(static_cast<char *>(errorBlob->GetBufferPointer()) );
	}
	_ASSERT(hr == S_OK);
	if(errorBlob)
		errorBlob->Release();
	errorBlob = NULL;

	// Now create the pixel shader
	hr = pDevice->CreatePixelShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &shader.m_pshader);
	_ASSERT(hr == S_OK);

	pShaderBlob->Release();
	pShaderBlob = NULL;
	delete shaderBuffer;

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
void HXSetSharedParameter(const std::string &paramName, Helix::Matrix4x4 &matrix)
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
