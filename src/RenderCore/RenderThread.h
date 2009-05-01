#ifndef RENDERTHREAD_H
#define RENDERTHREAD_H

namespace Helix{

class Instance;

	void	InitializeRenderThread();
	bool	GetRenderThreadShutdown();
	void	ShutDownRenderThread();
	void	RenderScene();

	void	SubmitInstance(Instance *inst);

} // namespace Helix



#endif RENDERTHREAD_H