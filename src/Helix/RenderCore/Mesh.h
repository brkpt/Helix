#ifndef MESH_H
#define MESH_H

namespace Helix {

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

	int	NumVertices() { return m_numVertices; }
	int NumTriangles() { return m_numTriangles; }

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

} // namespace Helix

#endif // MESH_H
