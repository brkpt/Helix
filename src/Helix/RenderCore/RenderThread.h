#ifndef RENDERTHREAD_H
#define RENDERTHREAD_H

struct D3DXMATRIX;

namespace Helix {

class Instance;

	void	InitializeRenderThread();
	bool	GetRenderThreadShutdown();
	void	ShutDownRenderThread();
	void	RenderScene();
	bool	RenderThreadReady();
	void	SetDevice(ID3D10Device *dev, IDXGISwapChain *swapChain);

	void	SubmitProjMatrix(D3DXMATRIX &mat);
	void	SubmitViewMatrix(D3DXMATRIX &mat);
	void	SubmitInstance(Instance &inst);


} // namespace Helix



#endif RENDERTHREAD_H