#ifndef MATERIALMANAGER_H
#define MATERIALMANAGER_H

#include <map>
#include <string>

class Material;

class MaterialManager
{
public:
	static MaterialManager & GetInstance()
	{
		static MaterialManager	instance;
		return instance;
	}
	
	~MaterialManager();

	Material *	GetMaterial(const std::string &name);
	Material *	Load(const std::string &name);

private:
	MaterialManager();
	MaterialManager(const MaterialManager &other) {}
	MaterialManager & operator=(const MaterialManager &other) {}

	typedef std::map<const std::string, Material *>	MaterialMap;
	MaterialMap		m_database;
};

#endif // MATERIALMANAGER_H
