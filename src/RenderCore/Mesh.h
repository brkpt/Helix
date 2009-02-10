#ifndef MESH_H
#define MESH_H

class Material;

class Mesh
{
public:
	Mesh();
	~Mesh();

	bool	Load(const std::string &path);
	bool	Load(const std::string &path, LuaObject &obj);

	void		Render();
	Material &	GetMaterial() { return *m_material; }
	IDirect3DVertexBuffer9 *	GetVertexBuffer() { return m_vertexBuffer; }
	IDirect3DIndexBuffer9 *		GetIndexBuffer()  { return m_indexBuffer; }

private:
	IDirect3DVertexBuffer9 *	m_vertexBuffer;
	IDirect3DIndexBuffer9 *		m_indexBuffer;
	Material *					m_material;	
};
#endif // MESH_H
