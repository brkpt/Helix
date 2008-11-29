#include "stdafx.h"
#include <process.h>
#include <crtdbg.h>
#include "RenderThread.h"

const int	RenderThread::STACK_SIZE =				16*1024;	// 16k

// ****************************************************************************
// ****************************************************************************
RenderThread::RenderThread()
: m_initialized(false)
, m_shutdown(false)
{
	m_threadHandle = (HANDLE)_beginthread( RenderThreadFunc, STACK_SIZE, NULL );
}

// ****************************************************************************
// ****************************************************************************
RenderThread::~RenderThread()
{
}

// ****************************************************************************
// ****************************************************************************
void RenderThread::Initialize()
{
	_ASSERT(m_initialized == false);
	m_initialized = true;

	m_mutexHandle = CreateMutex(NULL,true,L"RenderThread");
	_ASSERT(m_mutexHandle != NULL);

	m_triggerRender = CreateMutex(NULL,true,L"RenderTrigger");
	_ASSERT(m_triggerRender != NULL);

	for(int i=0;i<NUM_SUBMISSION_BUFFERS; i++)
	{
		m_submissionBuffers[i] = NULL;
	}
}

// ****************************************************************************
// ****************************************************************************
void RenderThread::RenderThreadFunc(void *data)
{
	RenderThread::GetInstance().Initialize();

	RenderThread &instance = RenderThread::GetInstance();
	while(!instance.GetShutdown())
	{
	}
}