#ifndef RENDERTHREAD_H
#define RENDERTHREAD_H

namespace Render
{
	void	InitializeRenderThread();
	bool	GetShutdown();
	void	SetShutdown();
	void	RenderScene();
}



#endif RENDERTHREAD_H