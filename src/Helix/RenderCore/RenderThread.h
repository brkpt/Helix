#ifndef RENDERTHREAD_H
#define RENDERTHREAD_H

#include "Math/Vector.h"
#include "Math/Matrix.h"

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
	void	SetSunlightDir(Helix::Vector3 &dir);
	void	SetSunlightColor(const DXGI_RGB &color);

	// Set ambient
	void	SetAmbientColor(const DXGI_RGB &color);

	void	SubmitProjMatrix(Helix::Matrix4x4 &mat);
	void	SubmitViewMatrix(Helix::Matrix4x4 &mat);
	void	SubmitInstance(Instance &inst);


} // namespace Helix



#endif RENDERTHREAD_H