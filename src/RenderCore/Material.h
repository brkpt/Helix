#ifndef MATERIAL_H
#define MATERIAL_H

class Shader;
class Texture;

class Material
{
public:
	Material(const std::string &matName, LuaObject &object);

	Shader &	GetShader() { return *m_shader; }
	Texture &	GetTexture() { return *m_texture; }

	void		SetViewMatrix(D3DXMATRIX *viewMatrix);
	void		SetProjMatrix(D3DXMATRIX *projMatrix);
	void		SetParameters();

private:
	std::string		m_name;
	Shader *		m_shader;
	Texture *		m_texture;
};

#endif // MATERIAL_H