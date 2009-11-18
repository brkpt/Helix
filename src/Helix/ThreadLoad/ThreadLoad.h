#ifndef THREADLOAD_H
#define THREADLOAD_H

#include <string>
#include "LuaPlus.h"
#include "Kernel/Callback.h"

namespace Helix { 
	void	InitializeThreadLoader();
	bool	GetLoadThreadShutdown();
	void	ShutdownLoadThread();

	//template <class T>
	//bool	LoadFileAsync(const std::string &file, T &object, void (T::*callbackFn)(void *), void *data);
	//bool	LoadLuaFileAsync(const std::string &file, void (*callbackFn)(LuaState *), void *data);
	bool	LoadFileAsync(const std::string &file, void (*callbackFn)(void *,void*), void *data);

	typedef Helix::StaticCallback2<void *, void *> ThreadLoadCallback;

	struct LoadObject
	{
		std::string			filename;
		ThreadLoadCallback *callback;
		LoadObject *		next;
		void *				userData;
	};

	extern bool			m_threadLoaderInitialized;
	extern bool			m_loadThreadShutdown;
	extern HANDLE		m_hLoadThread;
	extern HANDLE		m_hStartLoading;
	extern HANDLE		m_hLoadList;
	extern LoadObject *	m_loadList;

#include "ThreadLoad.inl"
}

#endif // THREADLOAD_H