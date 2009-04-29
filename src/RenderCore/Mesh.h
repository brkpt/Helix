#ifndef MESH_H
#define MESH_H

class Material;

class Mesh
{
public:
	Mesh();
	~Mesh();

	bool	Load(const std::string &path);

	void						Render(int pass);
	std::string &				GetMaterialName() { return m_materialName; }
	IDirect3DVertexBuffer9 *	GetVertexBuffer() { return m_vertexBuffer; }
	IDirect3DIndexBuffer9 *		GetIndexBuffer()  { return m_indexBuffer; }

private:
	bool	CreatePlatformData(const std::string &path, LuaObject &obj);

	IDirect3DVertexBuffer9 *	m_vertexBuffer;
	IDirect3DIndexBuffer9 *		m_indexBuffer;
	unsigned int				m_numVertices;
	unsigned int				m_numIndices;
	unsigned int				m_numTriangles;
	std::string					m_materialName;
	std::string					m_meshName;
};
#endif // MESH_H
