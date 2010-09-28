#include "stdafx.h"
#include "Helix.h"
#include "RenderCore/MaterialManager.h"
#include "RenderCore/DeclManager.h"
#include "RenderCore/InstanceManager.h"
#include "RenderCore/RenderThread.h"
#include "RenderCore/ShaderManager.h"
#include "RenderCore/TextureManager.h"
#include "RenderCore/MeshManager.h"
#include "RenderCore/Light.h"
#include "ThreadLoad/ThreadLoad.h"

namespace Helix
{
// ****************************************************************************
// ****************************************************************************
void Initialize(ID3D10Device* dev, IDXGISwapChain *swapChain)
{
	SetDevice(dev,swapChain);

	TextureManager::Create();
	MaterialManager::Create();
	DeclManager::Create();
	ShaderManager::Create();
	MeshManager::Create();
	InstanceManager::GetInstance();
	InitializeLights();

	InitializeRenderThread();
	InitializeThreadLoader();

}

}