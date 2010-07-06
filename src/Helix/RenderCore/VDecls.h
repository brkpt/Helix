#ifndef VDECLS_H
#define VDECLS_H

#include <map>

struct HXShader;

struct HXVertexDecl
{
	HXVertexDecl() : m_numElements(0), m_vertexSize(0), m_desc(NULL), m_layout(NULL) {}
	int							m_numElements;
	int							m_vertexSize;
	D3D10_INPUT_ELEMENT_DESC *	m_desc;
	ID3D10InputLayout *			m_layout;
};

void							HXInitializeVertexDecls();
HXVertexDecl *					HXGetVertexDecl(const std::string &declName);
HXVertexDecl *					HXLoadVertexDecl(const std::string &declName);
ID3D10InputLayout *				HXDeclBuildLayout(HXVertexDecl &decl, HXShader *shader);
bool							HXDeclHasSemantic(HXVertexDecl &decl, const char *semanticName, int &offset);

	//ID3D10InputLayout *			GetLayout() { return m_layout; }
	//D3D10_INPUT_ELEMENT_DESC *	GetDecl() { return m_desc; }
	//int							VertexSize() { return m_vertexSize; }
	//bool						HasSemantic(const char *semanticName, int &offset);


#endif // VDECLS_H
