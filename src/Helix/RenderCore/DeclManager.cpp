#include "stdafx.h"
#include "DeclManager.h"
#include "VertexDecl.h"

namespace Helix
{
// ****************************************************************************
// ****************************************************************************
DeclManager::DeclManager()
{
}

DeclManager::~DeclManager()
{
}

// ****************************************************************************
// ****************************************************************************
VertexDecl * DeclManager::GetDecl(const std::string &name)
{
	DeclMap::const_iterator iter = m_database.find(name);
	if( iter == m_database.end() )
		return NULL;

	return iter->second;
}

// ****************************************************************************
// ****************************************************************************
VertexDecl * DeclManager::Load(const std::string &name)
{
	VertexDecl *decl = GetDecl(name);
	if(decl != NULL)
		return decl;

	decl = new VertexDecl;
	bool retVal = decl->Load(name);
	_ASSERT(retVal);

	m_database[name] = decl;

	return decl;
}

} // namespace Helix