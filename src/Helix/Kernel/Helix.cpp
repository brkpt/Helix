#include "stdafx.h"
#include "Helix.h"
#include "RenderCore/Materials.h"
#include "RenderCore/Shaders.h"
#include "RenderCore/Textures.h"
//#include "RenderCore/DeclManager.h"
#include "RenderCore/InstanceManager.h"
#include "RenderCore/RenderThread.h"
//#include "RenderCore/ShaderManager.h"
//#include "RenderCore/TextureManager.h"
#include "RenderCore/MeshManager.h"
#include "RenderCore/Light.h"
#include "ThreadLoad/ThreadLoad.h"

namespace Helix
{
// ****************************************************************************
// ****************************************************************************
void Initialize(ID3D11Device* dev, ID3D11DeviceContext *context, IDXGISwapChain *swapChain)
{
	InitializeRenderer(dev,context, swapChain);


}

}