#ifndef VERTEXDECL_H
#define VERTEXDECL_H

#include <d3d9.h>
#include <map>
#include <string>
#include "LuaPlus.h"


class VertexDecl
{
	typedef std::pair<const std::string, D3DDECLUSAGE>	UsagePair;
	typedef std::map<const std::string,D3DDECLUSAGE>	UsageMap;
	typedef std::pair<const std::string, D3DDECLTYPE>	TypePair;
	typedef std::map<const std::string, D3DDECLTYPE>	TypeMap;
	typedef std::pair<const std::string, D3DDECLMETHOD>	MethodPair;
	typedef std::map<const std::string, D3DDECLMETHOD>	MethodMap;

public:
	VertexDecl();
	~VertexDecl();

	bool	Load(const std::string &path);
	bool	Load(LuaObject &object);

	D3DVERTEXELEMENT9	*GetDecl() { return m_decl; }

private:

	bool	GetUsage(D3DDECLUSAGE &usage, const std::string &str);
	bool	GetType(D3DDECLTYPE &type, const std::string &str);
	bool	GetMethod(D3DDECLMETHOD &method, const std::string &str);

	UsageMap	m_declUsageMap;
	TypeMap		m_declTypeMap;
	MethodMap	m_declMethodMap;

	D3DVERTEXELEMENT9	*m_decl;
};

#endif // VERTEXDECL_H