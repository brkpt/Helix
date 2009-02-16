#ifndef VERTEXDECL_H
#define VERTEXDECL_H

#include <string>
#include <map>

class VertexDecl
{
public:
	VertexDecl();
	~VertexDecl();

	bool	Load(const std::string &path);
	bool	Load(LuaObject &object);

	IDirect3DVertexDeclaration9 *	GetDecl() { return m_decl; }
	int								VertexSize() { return m_vertexSize; }

private:

	bool	GetUsage(D3DDECLUSAGE &usage, const std::string &str) const;
	bool	GetType(D3DDECLTYPE &type, const std::string &str) const;
	bool	GetMethod(D3DDECLMETHOD &method, const std::string &str) const;

	typedef std::map<const std::string,D3DDECLUSAGE>	UsageMap;
	typedef std::map<const std::string, D3DDECLTYPE>	TypeMap;
	typedef std::map<const std::string, D3DDECLMETHOD>	MethodMap;
	UsageMap			m_declUsageMap;
	TypeMap				m_declTypeMap;
	MethodMap			m_declMethodMap;
	int					m_numElements;
	int					m_vertexSize;
	IDirect3DVertexDeclaration9 *	m_decl;
};

#endif // VERTEXDECL_H