#ifndef RENDERTHREAD_H
#define RENDERTHREAD_H

#include <windows.h>
#include "Instance.h"

class RenderThread
{
public:
	static RenderThread &GetInstance()
	{
		static RenderThread	instance;

		return instance;
	}

	void	Initialize();
	bool	GetShutdown()	{ return m_shutdown; }
	void	SetShutdown()	{ m_shutdown = true; }

	static const int	STACK_SIZE;
	enum { NUM_SUBMISSION_BUFFERS = 2 } ;

private:
	// Make a singleton
	RenderThread();
	~RenderThread();
	RenderThread(const RenderThread &other) {}
	RenderThread &operator=(const RenderThread &other) {} 

	static void	RenderThreadFunc(void *data);

	bool		m_initialized;
	HANDLE		m_threadHandle;
	HANDLE		m_mutexHandle;
	HANDLE		m_triggerRender;
	bool		m_shutdown;

	int			m_submissionIndex;
	Instance *	m_submissionBuffers[NUM_SUBMISSION_BUFFERS];
};


#endif RENDERTHREAD_H