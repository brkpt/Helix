#ifndef THREADLOAD_H
#define THREADLOAD_H

#include <string>
#include "LuaPlus.h"
#include "Kernel/Callback.h"

namespace Helix { 
	void	InitializeThreadLoader();
	bool	GetLoadThreadShutdown();
	void	ShutdownLoadThread();

	template <class T>
	bool	LoadFileAsync(const std::string &file, T &object, void (T::*callbackFn)(void *), void *data);

	template <class T>
	bool	LoadLuaFileAsync(const std::string &file, T &object, void (T::*callbackFn)(LuaState *));

	bool	LoadFileAsync(const std::string &file, void (*callbackFn)(void *), void *data);

	struct LoadObject
	{
		std::string			filename;
		Helix::Callback *	callback;
		LuaState *			state;
		LoadObject *		next;
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