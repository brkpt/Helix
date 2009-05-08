#include "stdafx.h"
#include "RenderMgr.h"
#include "DeclManager.h"

namespace Helix {
// ****************************************************************************
// ****************************************************************************
Shader::Shader(const std::string &shaderName, LuaObject &shader, ID3DXEffectPool *effectPool)
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
	fxPath += ".fxo";

	IDirect3DDevice9 *pDevice = RenderMgr::GetInstance().GetDevice();
	HRESULT hr;
	hr = D3DXCreateEffectFromFile(pDevice,fxPath.c_str(),NULL,NULL,0,effectPool,&m_pEffect, NULL);
	_ASSERT(hr == D3D_OK);
}

Shader::~Shader()
{
}

// ****************************************************************************
// ****************************************************************************
void Shader::SetShaderParameter(const std::string &paramName, D3DXMATRIX &value)
{
	D3DXHANDLE effectParam = m_pEffect->GetParameterByName(NULL, paramName.c_str());
	HRESULT hr = m_pEffect->SetMatrix(effectParam, &value);
	_ASSERT(SUCCEEDED(hr));
}

// ****************************************************************************
// ****************************************************************************
void Shader::SetShaderParameter(const std::string &paramName, float value)
{
	D3DXHANDLE effectParam = m_pEffect->GetParameterByName(NULL, paramName.c_str());
	HRESULT hr = m_pEffect->SetFloat(effectParam, value);
	_ASSERT(SUCCEEDED(hr));
}

} // namespace Helix