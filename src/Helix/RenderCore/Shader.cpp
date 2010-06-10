#include "stdafx.h"
#include "RenderMgr.h"
#include "DeclManager.h"

namespace Helix {

// ****************************************************************************
// ****************************************************************************
Shader::Shader(const std::string &shaderName) 
: m_pEffect(NULL)
, m_decl(NULL)
, m_shaderName(shaderName)
, m_loading(false)
, m_needsProcessing(false)
{
}

// ****************************************************************************
// ****************************************************************************
Shader::Shader(const std::string &shaderName, LuaObject &shaderObj, ID3D10EffectPool *effectPool)
: m_pEffect(NULL)
, m_decl(NULL)
, m_shaderName(shaderName)
, m_loading(false)
{
	Load(shaderObj,effectPool);
}

Shader::~Shader()
{
}

// ****************************************************************************
// ****************************************************************************
void Shader::Load(LuaPlus::LuaObject &shaderObj, ID3D10EffectPool *effectPool)
{
	// Load our vertex declaration
	LuaObject obj = shaderObj["Declaration"];
	_ASSERT(obj.IsString());
	std::string name = obj.GetString();

	m_decl = DeclManager::GetInstance().Load(name);
	_ASSERT(m_decl);

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
	
	ID3D10Device *pDevice = RenderMgr::GetInstance().GetDevice();
	HRESULT hr;
	ID3D10Blob *errorBlob;
	D3D10CreateBlob(1024,&errorBlob);
	hr = D3DX10CreateEffectFromFile(fxPath.c_str(), NULL,NULL,"fx_4_0",dwShaderFlags,D3D10_EFFECT_COMPILE_CHILD_EFFECT, pDevice, effectPool, NULL, &m_pEffect, &errorBlob, NULL);
	if(hr != D3D_OK)
	{
		OutputDebugString(static_cast<char *>(errorBlob->GetBufferPointer()) );
	}
	_ASSERT(hr == D3D_OK);

	// Now create the layout if it hasn't been created already
	m_decl->BuildLayout(this);
}
// ****************************************************************************
// ****************************************************************************
void Shader::SetShaderParameter(const std::string &paramName, D3DXMATRIX &value)
{
	ID3D10EffectVariable *effectParam = m_pEffect->GetVariableByName(paramName.c_str());
	ID3D10EffectMatrixVariable *mat = effectParam->AsMatrix();
	HRESULT hr = mat->SetMatrix(value);
	_ASSERT(SUCCEEDED(hr));
}

// ****************************************************************************
// ****************************************************************************
void Shader::SetShaderParameter(const std::string &paramName, float value)
{
	ID3D10EffectVariable *effectParam = m_pEffect->GetVariableByName( paramName.c_str() );
	ID3D10EffectScalarVariable *scalar = effectParam->AsScalar();
	HRESULT hr = scalar->SetFloat(value);
	_ASSERT(SUCCEEDED(hr));
}

} // namespace Helix