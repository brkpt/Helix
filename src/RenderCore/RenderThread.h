#ifndef RENDERTHREAD_H
#define RENDERTHREAD_H

#include <windows.h>
#include "Instance.h"

namespace Render
{
	void	InitializeRenderThread();
	bool	GetShutdown();
	void	SetShutdown();
	void	RenderScene();
}



#endif RENDERTHREAD_H