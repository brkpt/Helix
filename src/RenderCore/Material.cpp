#include "stdafx.h"
#include "Material.h"
#include "LuaPlus.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "Shader.h"
#include "Texture.h"

// ****************************************************************************
// ****************************************************************************
Material::Material(const std::string &name, LuaObject &object)
: m_shader(NULL)
, m_texture(NULL)
{
	m_name = name;

	// Load our shader
	LuaObject obj = object["Shader"];
	_ASSERT(obj.IsString());
	std::string itemName = obj.GetString();

	m_shader = ShaderManager::GetInstance().Load(itemName);
	_ASSERT(m_shader != NULL);

	obj = object["Texture"];
	_ASSERT(obj.IsString());
	itemName = obj.GetString();

	m_texture = TextureManager::GetInstance().LoadTexture(itemName);
	_ASSERT(m_texture != NULL);
}

// ****************************************************************************
// ****************************************************************************
void Material::SetParameters()
{
	ID3DXEffect *effect = m_shader->GetEffect();

	D3DXHANDLE hTexture = effect->GetParameterByName(NULL,"textureImage");
	HRESULT hr = effect->SetTexture( hTexture, m_texture->TexData());
	_ASSERT(SUCCEEDED(hr));

}