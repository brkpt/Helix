#include "stdafx.h"
#include <process.h>
#include <crtdbg.h>
#include "RenderThread.h"

namespace Helix {

const int	STACK_SIZE =				16*1024;	// 16k
const int	NUM_SUBMISSION_BUFFERS	=	2;
bool		m_renderThreadInitialized =				false;
bool		m_renderThreadShutdown =				false;
bool		m_inRender =				false;
HANDLE		m_hThread	=				NULL;
HANDLE		m_startRenderEvent	=		NULL;
HANDLE		m_endRenderEvent =			NULL;
HANDLE		m_hSubmitMutex =			NULL;

struct RenderData
{
	std::string		instanceName;
	std::string		meshName;
	std::string		materialName;
	RenderData *	prev;
	RenderData *	next;
};

int				m_submissionIndex = 0;
RenderData *	m_submissionBuffers[NUM_SUBMISSION_BUFFERS];

// ****************************************************************************
// Thread function
// ****************************************************************************
void RenderThreadFunc(void *data);

// ****************************************************************************
// ****************************************************************************
void InitializeRenderThread()
{
	_ASSERT(m_renderThreadInitialized == false);
	m_renderThreadInitialized = true;

	m_startRenderEvent = CreateEvent(NULL, false, false,"RenderStartEvent");
	_ASSERT(m_startRenderEvent != NULL);

	m_endRenderEvent = CreateEvent(NULL, false, false, "RenderEndEvent");
	_ASSERT(m_endRenderEvent != NULL);

	m_submissionIndex = 0;
	for(int i=0;i<NUM_SUBMISSION_BUFFERS; i++)
	{
		m_submissionBuffers[i] = NULL;
	}

	m_hSubmitMutex = CreateMutex(NULL,false,"RenderSubmit");

	// Now create the thread
	m_hThread = (HANDLE)_beginthread( Helix::RenderThreadFunc, STACK_SIZE, NULL );
//	_ASSERT(m_hThread != 1L);

}

// ****************************************************************************
// ****************************************************************************
bool GetRenderThreadShutdown()
{
	return m_renderThreadShutdown;
}

// ****************************************************************************
// ****************************************************************************
void ShutDownRenderThread()
{
	m_renderThreadShutdown = true;

	// Wait for the thread to exit
	DWORD result = WaitForSingleObject(m_endRenderEvent,INFINITE);
	_ASSERT(result == WAIT_OBJECT_0);

	CloseHandle(m_startRenderEvent);
	CloseHandle(m_endRenderEvent);

	WaitForSingleObject(m_hSubmitMutex,INFINITE);
	for(int i=0;i<NUM_SUBMISSION_BUFFERS;i++)
	{
		RenderData *obj = m_submissionBuffers[i];
		while(obj != NULL)
		{
			RenderData *nextObj = obj->next;
			delete obj;
			obj = nextObj;
		}
	}
	ReleaseMutex(m_hSubmitMutex);
	CloseHandle(m_hSubmitMutex);
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
void SubmitInstance(Instance *inst)
{
	_ASSERT(inst);
	if(inst == NULL)
		return;

	DWORD result = WaitForSingleObject(m_hSubmitMutex,INFINITE);
	_ASSERT(result == WAIT_OBJECT_0);

	RenderData *obj = new RenderData;
	obj->instanceName = inst->GetName();
	obj->meshName = inst->GetMeshName();
	obj->next = m_submissionBuffers[m_submissionIndex];
	m_submissionBuffers[m_submissionIndex] = obj;

	result = ReleaseMutex(m_hSubmitMutex);
	_ASSERT(result);
}

// ****************************************************************************
// ****************************************************************************
void RenderThreadFunc(void *data)
{
	while(!GetRenderThreadShutdown())
	{
		DWORD result = WaitForSingleObject(m_startRenderEvent,INFINITE);
		_ASSERT(result == WAIT_OBJECT_0);

		result = WaitForSingleObject(m_hSubmitMutex,INFINITE);
		_ASSERT(result == WAIT_OBJECT_0);

		int renderSubmissionIndex = m_submissionIndex;
		m_submissionIndex = (m_submissionIndex + 1 ) % NUM_SUBMISSION_BUFFERS;

		result = ReleaseMutex(m_hSubmitMutex);
		_ASSERT(result);

		int a = 1;

		// Delete all our render objects
		RenderData *obj = m_submissionBuffers[renderSubmissionIndex];
		while(obj != NULL)
		{
			RenderData *nextObj = obj->next;
			delete obj;
			obj = nextObj;
		}

		m_submissionBuffers[renderSubmissionIndex] = NULL;

		result = SetEvent(m_endRenderEvent);
		_ASSERT(result != 0);
	}
}

} // namespace Helix
