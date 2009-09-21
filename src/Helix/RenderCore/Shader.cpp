#include "stdafx.h"
#include "RenderMgr.h"
#include "DeclManager.h"

namespace Helix {
// ****************************************************************************
// ****************************************************************************
Shader::Shader(const std::string &shaderName, LuaObject &shader, ID3D10EffectPool *effectPool)
: m_pEffect(NULL)
, m_decl(NULL)
{
	m_shaderName = shaderName;

	// Load our vertex declaration
	LuaObject obj = shader["Declaration"];
	_ASSERT(obj.IsString());
	std::string name = obj.GetString();

	m_decl = DeclManager::GetInstance().Load(name);
	_ASSERT(m_decl);

	// Load our effect
	obj = shader["FX"];
	_ASSERT(obj.IsString());
	std::string fxName = obj.GetString();
	std::string fxPath = "Effects/";
	fxPath += fxName;
	fxPath += ".fx";

	DWORD dwShaderFlags = D3D10_SHADER_DEBUG | D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY;

	ID3D10Device *pDevice = RenderMgr::GetInstance().GetDevice();
	HRESULT hr;
	hr = D3DX10CreateEffectFromFile(fxPath.c_str(), NULL,NULL,"fx_4_0",dwShaderFlags,D3D10_EFFECT_COMPILE_CHILD_EFFECT, pDevice, effectPool, NULL, &m_pEffect, NULL, NULL);
	_ASSERT(hr == D3D_OK);

	// Now create the layout if it hasn't been created already
	m_decl->BuildLayout(this);
}

Shader::~Shader()
{
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