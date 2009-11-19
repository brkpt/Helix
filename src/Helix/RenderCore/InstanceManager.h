#ifndef INSTANCE_MGR_H
#define INSTANCE_MGR_H

namespace Helix {

class Instance;

class InstanceManager
{
public:
	static void Create()
	{
		GetInstance();
	}

	Instance * CreateInstance(const std::string &instanceName);

	static InstanceManager & GetInstance()
	{
		static InstanceManager	instance;
		return instance;
	}

	Instance *	Get(const std::string &name);
	Instance *	Load(const std::string &name);

	void	SubmitInstances();

private:
	InstanceManager();
	~InstanceManager() {}
	InstanceManager(const InstanceManager &other) {}
	InstanceManager &	operator=(const InstanceManager &other) {}

	typedef std::map<const std::string, Instance *>	InstanceMap;

	InstanceMap		m_database;
};

} // namespace Helix
#endif INSTANCE_MGR_H
