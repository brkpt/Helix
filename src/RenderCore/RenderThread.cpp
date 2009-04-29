#include "stdafx.h"
#include <process.h>
#include <crtdbg.h>
#include "RenderThread.h"

namespace Render
{
const int	STACK_SIZE =				16*1024;	// 16k
const int	NUM_SUBMISSION_BUFFERS	=	2;
bool		m_initialized =				false;
bool		m_shutdown =				false;
bool		m_inRender =				false;
HANDLE		m_hThread	=				NULL;
HANDLE		m_startRenderEvent	=		NULL;


//int			m_submissionIndex = 0;
//Instance *	m_submissionBuffers[NUM_SUBMISSION_BUFFERS];

// ****************************************************************************
// Thread function
// ****************************************************************************
void RenderThreadFunc(void *data);

// ****************************************************************************
// ****************************************************************************
void InitializeRenderThread()
{
	_ASSERT(m_initialized == false);
	m_initialized = true;

	m_startRenderEvent = CreateEvent(NULL,true,false,"RenderEvent");
	_ASSERT(m_startRenderEvent != NULL);

	//for(int i=0;i<NUM_SUBMISSION_BUFFERS; i++)
	//{
	//	m_submissionBuffers[i] = NULL;
	//}

	// Now create the thread
	m_hThread = (HANDLE)_beginthread( Render::RenderThreadFunc, STACK_SIZE, NULL );
}

// ****************************************************************************
// ****************************************************************************
bool GetShutdown()
{
	return m_shutdown;
}

// ****************************************************************************
// ****************************************************************************
void SetShutdown(bool shutdown)
{
	m_shutdown = shutdown;
}

// ****************************************************************************
// ****************************************************************************
void RenderScene()
{
	DWORD result = SetEvent(m_startRenderEvent);
	_ASSERT(result != 0);
}

// ****************************************************************************
// ****************************************************************************
void RenderThreadFunc(void *data)
{
	while(!GetShutdown())
	{
		DWORD result = WaitForSingleObject(m_startRenderEvent,INFINITE);
		_ASSERT(result == WAIT_OBJECT_0);

		int a = 1;
	}
}

}
