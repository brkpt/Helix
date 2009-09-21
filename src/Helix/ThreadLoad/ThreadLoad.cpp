#include "stdafx.h"
#include <process.h>
#include "Kernel/Callback.h"
#include "ThreadLoad.h"

// ****************************************************************************
// ****************************************************************************
namespace Helix {

	const int	STACK_SIZE	=	16*1024;
	bool		m_threadLoaderInitialized = false;
	bool		m_loadThreadShutdown = false;
	HANDLE		m_hLoadThread = NULL;
	HANDLE		m_hStartLoading = NULL;
	HANDLE		m_hLoadList = NULL;
	LoadObject	*	m_loadList = NULL;

// ****************************************************************************
// Thread function
// ****************************************************************************
void LoadThreadFunc(void *data);

// ****************************************************************************
// ****************************************************************************
void InitializeThreadLoader()
{
	_ASSERT(m_threadLoaderInitialized == false);
	m_threadLoaderInitialized = true;

	m_hStartLoading = CreateEvent(NULL,false,false,"StartLoading");
	_ASSERT(m_hStartLoading != NULL);

	m_hLoadList = CreateMutex(NULL,false,"ThreadLoadList");
	_ASSERT(m_hLoadList != NULL);

	// Now create the thread
	m_hLoadThread = (HANDLE)_beginthread( Helix::LoadThreadFunc, STACK_SIZE, NULL );
}

// ****************************************************************************
// ****************************************************************************
bool GetLoadThreadShutdown()
{
	return m_loadThreadShutdown;
}

// ****************************************************************************
// ****************************************************************************
void ShutdownLoadThread()
{
	m_loadThreadShutdown = true;
}

// ****************************************************************************
// ****************************************************************************
void LoadThreadFunc(void *data)
{
	while(!GetLoadThreadShutdown())
	{
		DWORD result = WaitForSingleObject(m_hStartLoading,INFINITE);
		_ASSERT(result == WAIT_OBJECT_0);

		while(m_loadList!= NULL)
		{
			// Pop the next one
			result = WaitForSingleObject(m_hLoadList,INFINITE);
			_ASSERT(result == WAIT_OBJECT_0);

			LoadObject *loadObject = m_loadList;
			m_loadList = m_loadList->next;

			result = ReleaseMutex(m_hLoadList);
			_ASSERT(result != 0);

			loadObject->callback->Call();
			delete loadObject;
		}

		// The start loading event isn't cleared until we clear it.  This allows us to set it each 
		// time we add a file
		ResetEvent(m_hStartLoading);
	}
}

// ****************************************************************************
// ****************************************************************************
bool LoadFileAsync(const std::string &file, void (*callbackFn)(void *), void *data)
{
	return true;
}

}