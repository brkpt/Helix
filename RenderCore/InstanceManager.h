#ifndef INSTANCE_MGR_H
#define INSTANCE_MGR_H

#include <crtdbg.h>
#include "Instance.h"

class InstanceManager
{
	enum { MAX_INSTANCES = 1000 };

public:
	static InstanceManager & GetInstance()
	{
		static InstanceManager	instance;
		return instance;
	}

	Instance &	NewInstance()
	{
		_ASSERT(m_nextInstanceIndex < MAX_INSTANCES);
		return m_instanceArray[m_nextInstanceIndex++];
	}

private:
	InstanceManager();
	~InstanceManager() {}
	InstanceManager(const InstanceManager &other) {}
	InstanceManager &	operator=(const InstanceManager &other) {}

	Instance		m_instanceArray[MAX_INSTANCES];
	unsigned int	m_nextInstanceIndex;
};

#endif INSTANCE_MGR_H
