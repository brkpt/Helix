#include "stdafx.h"

namespace Helix {
// ****************************************************************************
// ****************************************************************************
Material::Material(const std::string &name, LuaObject &object)
{
	m_name = name;

	// Load our shader
	LuaObject obj = object["Shader"];
	_ASSERT(obj.IsString());
	m_shaderName = obj.GetString();

	obj = object["Texture"];
	_ASSERT(obj.IsString());
	m_textureName = obj.GetString();

	// Load the shader
	Shader *shader = ShaderManager::GetInstance().Load(m_shaderName);
	
	// Load the texture
	Texture *tex = TextureManager::GetInstance().LoadTexture(m_textureName);
	_ASSERT(tex != NULL);
}

// ****************************************************************************
// ****************************************************************************
void Material::SetParameters()
{
	Shader *shader = ShaderManager::GetInstance().GetShader(m_shaderName);
	_ASSERT(shader != NULL);

	ID3DXEffect *effect = shader->GetEffect();

	D3DXHANDLE hTexture = effect->GetParameterByName(NULL,"textureImage");

	Texture *tex = TextureManager::GetInstance().GetTexture(m_textureName);
	_ASSERT(tex != NULL);

	HRESULT hr = effect->SetTexture( hTexture, tex->TexData());
	_ASSERT(SUCCEEDED(hr));

}

} // namespace Helix