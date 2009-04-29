#include "stdafx.h"
#include <process.h>
#include "ThreadLoad.h"

// ****************************************************************************
// ****************************************************************************
namespace Helix {
const int	STACK_SIZE	=	16*1024;
bool		m_initialized = false;
bool		m_shutdown = false;
HANDLE		m_hLoadThread = NULL;
HANDLE		m_hStartLoading = NULL;

// ****************************************************************************
// Thread function
// ****************************************************************************
void LoadThreadFunc(void *data);

// ****************************************************************************
// ****************************************************************************
void InitializeThreadLoader()
{
	_ASSERT(m_initialized == false);
	m_initialized = true;

	m_hStartLoading = CreateEvent(NULL,true,false,"StartLoading");
	_ASSERT(m_hStartLoading != NULL);

	// Now create the thread
	m_hLoadThread = (HANDLE)_beginthread( Helix::LoadThreadFunc, STACK_SIZE, NULL );
}

// ****************************************************************************
// ****************************************************************************
bool GetShutdown()
{
	return m_shutdown;
}

// ****************************************************************************
// ****************************************************************************
void Shutdown()
{
	m_shutdown = true;
}

// ****************************************************************************
// ****************************************************************************
void LoadThreadFunc(void *data)
{
	while(!GetShutdown())
	{
		DWORD result = WaitForSingleObject(m_hStartLoading,INFINITE);
		_ASSERT(result == WAIT_OBJECT_0);
	}
}

}