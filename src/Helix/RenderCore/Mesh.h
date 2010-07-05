#ifndef MESH_H
#define MESH_H

namespace Helix {

class Material;

class Mesh : public ReferenceCountable
{
public:
	Mesh();
	~Mesh();

	bool	Load(const std::string &filename);
	bool	Load(LuaObject &meshObj);

//	void			Render(int pass);
	std::string &	GetMaterialName() { return m_materialName; }
	ID3D10Buffer *	GetVertexBuffer() { return m_vertexBuffer; }
	ID3D10Buffer *	GetIndexBuffer()  { return m_indexBuffer; }

	int	NumVertices()	{ return m_numVertices; }
	int NumTriangles()	{ return m_numTriangles; }
	int NumIndices()	{ return m_numIndices; }

private:
	bool	CreatePlatformData(const std::string &path, LuaObject &obj);

	ID3D10Buffer *	m_vertexBuffer;
	ID3D10Buffer *	m_indexBuffer;
	unsigned int	m_numVertices;
	unsigned int	m_numIndices;
	unsigned int	m_numTriangles;
	std::string		m_materialName;
	std::string		m_meshName;
	bool			m_32bitIndices;
};

} // namespace Helix

#endif // MESH_H
