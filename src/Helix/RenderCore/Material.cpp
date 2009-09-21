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
	
	// Make sure we can load the associated texture
	if(m_textureName.compare("albedo"))
	{
		Texture *tex = TextureManager::GetInstance().LoadTexture(m_textureName);
		_ASSERT(tex != NULL);
	}
}

// ****************************************************************************
// ****************************************************************************
void Material::SetParameters()
{
	Shader *shader = ShaderManager::GetInstance().GetShader(m_shaderName);
	_ASSERT(shader != NULL);

	ID3D10Effect *effect = shader->GetEffect();

	ID3D10EffectShaderResourceVariable *shaderResource = effect->GetVariableByName("textureImage")->AsShaderResource();

	Texture *tex = TextureManager::GetInstance().GetTexture(m_textureName);

	if( tex != NULL)
	{
		// Mesh that only uses render targets as input textures 
		// may not have a texture 
		ID3D10ShaderResourceView *textureRV = tex->GetResourceView();
		HRESULT hr = shaderResource->SetResource(textureRV);
		_ASSERT(SUCCEEDED(hr));
	}

}

} // namespace Helix