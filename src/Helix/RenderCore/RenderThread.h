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

	// Set sunlight 
	void	SetSunlightDir(const D3DXVECTOR3 &dir);
	void	SetSunlightColor(const DXGI_RGB &color);

	// Set ambient
	void	SetAmbientColor(const DXGI_RGB &color);

	void	SubmitProjMatrix(D3DXMATRIX &mat);
	void	SubmitViewMatrix(D3DXMATRIX &mat);
	void	SubmitInstance(Instance &inst);


} // namespace Helix



#endif RENDERTHREAD_H