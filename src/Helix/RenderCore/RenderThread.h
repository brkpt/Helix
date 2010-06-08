#ifndef RENDERTHREAD_H
#define RENDERTHREAD_H

/*
struct Flags
{
	union {
		struct {
			bool	flag1 : 1;
			bool	flag2 : 2;
		};
		unsigned long	flagData;
	};
}
*/

//struct D3DXMATRIX;

namespace Helix {

class Instance;

	void	InitializeRenderThread();
	bool	GetRenderThreadShutdown();
	void	ShutDownRenderThread();
	void	RenderScene();
	bool	RenderThreadReady();
	void	SetDevice(ID3D10Device *dev, IDXGISwapChain *swapChain);

	int		SubmissionIndex();

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