#ifndef DECLMANAGER_H
#define DECLMANAGER_H

#include <string>
#include <map>

class VertexDecl;

class DeclManager
{
public:
	static DeclManager &GetInstance()
	{
		static DeclManager	instance;
		return instance;
	}

	~DeclManager();

	VertexDecl *	GetDecl(const std::string &declName);
	VertexDecl *	Load(const std::string &declName);

private:
	DeclManager();
	DeclManager(const DeclManager &other) {}
	DeclManager & operator=(const DeclManager &other) {}

	typedef std::map<const std::string, VertexDecl *>	DeclMap;
	DeclMap		m_database;
};

#endif