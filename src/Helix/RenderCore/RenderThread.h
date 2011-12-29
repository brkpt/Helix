#ifndef RENDERTHREAD_H
#define RENDERTHREAD_H

#include <D3DX10.h>

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

	void	InitializeRenderer(ID3D11Device *dev, ID3D11DeviceContext *context, IDXGISwapChain *swapChain);
	bool	GetRenderThreadShutdown();
	void	ShutDownRenderThread();
	void	RenderScene();
	bool	RenderThreadReady();

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