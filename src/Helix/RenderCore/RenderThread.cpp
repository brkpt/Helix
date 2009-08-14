#include "stdafx.h"
#include <process.h>
#include <crtdbg.h>
#include "RenderThread.h"
#include "RenderMgr.h"
#include "VertexDecl.h"

namespace Helix {

const int					STACK_SIZE =				16*1024;	// 16k
const int					NUM_SUBMISSION_BUFFERS	=	2;
bool						m_renderThreadInitialized =	false;
bool						m_renderThreadShutdown =	false;
bool						m_inRender =				false;
HANDLE						m_hThread	=				NULL;
HANDLE						m_startRenderEvent	=		NULL;
HANDLE						m_rendererReady =			NULL;
HANDLE						m_hSubmitMutex =			NULL;
ID3D10Device *				m_D3DDevice =				NULL;
IDXGISwapChain *			m_swapChain =				NULL;
ID3D10RenderTargetView *	m_backBufferView = NULL;
ID3D10Texture2D *			m_backDepthStencil = NULL;
ID3D10DepthStencilView *	m_backDepthStencilView = NULL;
ID3D10Texture2D *			m_albedoTexture = NULL;		// Albedo texture
ID3D10RenderTargetView *	m_albedoRTView = NULL;		// Albedo view - used when texture is render target
ID3D10ShaderResourceView *	m_albedoSRView = NULL;		// Albedo shader resource view - used when texture is input to shaders
Material *					m_albedoMaterial = NULL;
ID3D10Texture2D *			m_depthStencilTexture = NULL;
ID3D10DepthStencilView *	m_depthStencilDSView = NULL;
ID3D10ShaderResourceView *	m_depthStencilSRView = NULL;
ID3D10Buffer *				m_quadVB = NULL;
ID3D10Buffer *				m_quadIB = NULL;
ID3D10RasterizerState *		m_RState = NULL;
struct RenderData
{
	D3DXMATRIX		worldMatrix;
	std::string		meshName;
	std::string		materialName;
	RenderData *	next;
};

int				m_submissionIndex = 0;
RenderData *	m_submissionBuffers[NUM_SUBMISSION_BUFFERS];
D3DXMATRIX		m_viewMatrix[NUM_SUBMISSION_BUFFERS];
D3DXMATRIX		m_projMatrix[NUM_SUBMISSION_BUFFERS];

//#pragma pack push(1)
struct QuadVert {
	float	pos[3];
	float	uv[2];
};
//#pragma pack pop

// ****************************************************************************
// Thread function
// ****************************************************************************
void RenderThreadFunc(void *data);

// ****************************************************************************
// ****************************************************************************
inline bool AcquireMutex()
{
	DWORD result = WaitForSingleObject(m_hSubmitMutex,INFINITE);
	_ASSERT(result == WAIT_OBJECT_0);

	return result == WAIT_OBJECT_0;
}

// ****************************************************************************
// ****************************************************************************
inline bool ReleaseMutex()
{
	DWORD result = ::ReleaseMutex(m_hSubmitMutex);
	_ASSERT(result);

	return result != 0;
}

// ****************************************************************************
// ****************************************************************************
bool RenderThreadReady()
{
	DWORD result = WaitForSingleObject(m_rendererReady,INFINITE);
	_ASSERT(result == WAIT_OBJECT_0);

	return result == WAIT_OBJECT_0;
}

// ****************************************************************************
// ****************************************************************************
void SetDevice(ID3D10Device* dev, IDXGISwapChain *swapChain)
{
	_ASSERT(dev != NULL);
	_ASSERT( swapChain != NULL) ;
	m_D3DDevice = dev;
	m_swapChain = swapChain;

    // Create a render target view

	// Get the back buffer and desc
    ID3D10Texture2D* pBuffer;
    HRESULT hr = m_swapChain->GetBuffer( 0, __uuidof( ID3D10Texture2D ), ( LPVOID* )&pBuffer );
	_ASSERT( SUCCEEDED(hr) );

    D3D10_TEXTURE2D_DESC backBufferSurfaceDesc;
    pBuffer->GetDesc( &backBufferSurfaceDesc );

	// Set our viewport
	D3D10_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = backBufferSurfaceDesc.Width;
	vp.Height = backBufferSurfaceDesc.Height;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	m_D3DDevice->RSSetViewports(1, &vp);

    hr = m_D3DDevice->CreateRenderTargetView( pBuffer, NULL, &m_backBufferView );
    pBuffer->Release();
	_ASSERT( SUCCEEDED(hr) );

	// Create depth stencil texture
    D3D10_TEXTURE2D_DESC descDepth;
    descDepth.Width = backBufferSurfaceDesc.Width;
    descDepth.Height = backBufferSurfaceDesc.Height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D32_FLOAT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D10_USAGE_DEFAULT;
    descDepth.BindFlags = D3D10_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    hr = m_D3DDevice->CreateTexture2D( &descDepth, NULL, &m_backDepthStencil );
	_ASSERT( SUCCEEDED(hr) );

    // Create the depth stencil view
    D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
    descDSV.Format = descDepth.Format;
    //if( descDepth.SampleDesc.Count > 1 )
    //    descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2DMS;
    //else
	descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = m_D3DDevice->CreateDepthStencilView( m_backDepthStencil, &descDSV, &m_backDepthStencilView );
	_ASSERT( SUCCEEDED(hr) );
  
	// Create some empty render targets
	D3D10_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = backBufferSurfaceDesc.Width;
	desc.Height = backBufferSurfaceDesc.Height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //DXGI_FORMAT_R10G10B10A2_UNORM ;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D10_USAGE_DEFAULT;
	desc.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;

	// Create the texture
	hr = m_D3DDevice->CreateTexture2D( &desc, NULL, &m_albedoTexture );
	_ASSERT( SUCCEEDED(hr) );

	// Create the render target view
	D3D10_RENDER_TARGET_VIEW_DESC rtDesc;
	rtDesc.Format = desc.Format;
	rtDesc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;
	rtDesc.Texture2D.MipSlice = 0;

	hr = m_D3DDevice->CreateRenderTargetView( m_albedoTexture, &rtDesc, &m_albedoRTView );
	_ASSERT( SUCCEEDED(hr) );

	// Create the shader input view
	D3D10_SHADER_RESOURCE_VIEW_DESC srDesc;
	srDesc.Format = desc.Format;
	srDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MostDetailedMip = 0;
	srDesc.Texture2D.MipLevels = 1;

	hr = m_D3DDevice->CreateShaderResourceView( m_albedoTexture, &srDesc, &m_albedoSRView );
	_ASSERT( SUCCEEDED(hr) );

	// Create a depth/stencil texture
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = 1024;
	desc.Height = 768;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R16_TYPELESS ;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D10_USAGE_DEFAULT;
	desc.BindFlags = D3D10_BIND_DEPTH_STENCIL | D3D10_BIND_SHADER_RESOURCE;

	hr = m_D3DDevice->CreateTexture2D( &desc, NULL, &m_depthStencilTexture );
	_ASSERT( SUCCEEDED(hr) );

	// Create the depth/stencil view
	D3D10_DEPTH_STENCIL_VIEW_DESC dsDesc;
	ZeroMemory(&dsDesc,sizeof(dsDesc));
    dsDesc.Format = DXGI_FORMAT_D16_UNORM;
	dsDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
    dsDesc.Texture2D.MipSlice = 0;

    hr = m_D3DDevice->CreateDepthStencilView(m_depthStencilTexture,&dsDesc,&m_depthStencilDSView );
	_ASSERT( SUCCEEDED(hr) );

	// Create our shader resource view
	ZeroMemory(&srDesc, sizeof(srDesc));
	srDesc.Format = DXGI_FORMAT_R16_UNORM;
	srDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MostDetailedMip = 0;
	srDesc.Texture2D.MipLevels = 1;

	hr = m_D3DDevice->CreateShaderResourceView(m_depthStencilTexture,&srDesc,&m_depthStencilSRView );
	_ASSERT( SUCCEEDED(hr) );

	// Load our quad shader
	m_albedoMaterial = MaterialManager::GetInstance().Load("Quad");
	_ASSERT( m_albedoMaterial != NULL);

	// Vertex buffer descriptor
	int vertexSize = 3*sizeof(float) + 2*sizeof(float);
	int foo = sizeof(QuadVert);
	QuadVert quadVerts[4];
	foo = sizeof(quadVerts);

	quadVerts[0].pos[0] = -1.0f;
	quadVerts[0].pos[1] = -1.0f;
	quadVerts[0].pos[2] = 0.5f;
	quadVerts[0].uv[0] = 0.0f;
	quadVerts[0].uv[1] = 1.0f;

	quadVerts[1].pos[0] = -1.0f;
	quadVerts[1].pos[1] = 1.0f;
	quadVerts[1].pos[2] = 0.5f;
	quadVerts[1].uv[0] = 0.0f;
	quadVerts[1].uv[1] = 0.0f;

	quadVerts[2].pos[0] = 1.0f;
	quadVerts[2].pos[1] = 1.0f;
	quadVerts[2].pos[2] = 0.5f;
	quadVerts[2].uv[0] = 1.0f;
	quadVerts[2].uv[1] = 0.0f;

	quadVerts[3].pos[0] = 1.0f;
	quadVerts[3].pos[1] = -1.0f;
	quadVerts[3].pos[2] = 0.5f;
	quadVerts[3].uv[0] = 1.0f;
	quadVerts[3].uv[1] = 1.0f;

	D3D10_BUFFER_DESC bufferDesc = {0};
	bufferDesc.Usage = D3D10_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(quadVerts);
	bufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	// Data initialization descriptor
	D3D10_SUBRESOURCE_DATA initData = {0};
	initData.pSysMem = &quadVerts;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	// Create the buffer
	hr = m_D3DDevice->CreateBuffer(&bufferDesc,&initData,&m_quadVB);
	_ASSERT( SUCCEEDED(hr) );

	// Create our index buffer
	unsigned short ibData[4] = { 1, 2, 0, 3 };

	// Create the index buffer
	memset(&bufferDesc,0,sizeof(bufferDesc));
	bufferDesc.Usage = D3D10_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(ibData);
	bufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	
	// Data initialization descriptor
	memset(&initData,0,sizeof(initData));
	initData.pSysMem = ibData;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	hr = m_D3DDevice->CreateBuffer(&bufferDesc,&initData,&m_quadIB);
	_ASSERT( SUCCEEDED(hr) );

	// Setup our rasterizer state
	D3D10_RASTERIZER_DESC rDesc;
	memset(&rDesc,0,sizeof(rDesc));
	rDesc.FillMode = D3D10_FILL_SOLID;
	rDesc.CullMode = D3D10_CULL_NONE;
	rDesc.FrontCounterClockwise = false;
	rDesc.DepthBias = 0;
	rDesc.DepthBiasClamp = 0.0f;
	rDesc.SlopeScaledDepthBias = 0.0f;
	rDesc.DepthClipEnable = true;
	rDesc.ScissorEnable = false;
	rDesc.MultisampleEnable = false;
	rDesc.AntialiasedLineEnable = false;
	hr = m_D3DDevice->CreateRasterizerState(&rDesc, &m_RState);
	_ASSERT( SUCCEEDED(hr) );

}
// ****************************************************************************
// ****************************************************************************
void InitializeRenderThread()
{
	_ASSERT(m_renderThreadInitialized == false);
	m_renderThreadInitialized = true;

	m_startRenderEvent = CreateEvent(NULL, false, false,"RenderStartEvent");
	_ASSERT(m_startRenderEvent != NULL);

	m_rendererReady = CreateEvent(NULL, false, false, "RenderEndEvent");
	_ASSERT(m_rendererReady != NULL);

	m_submissionIndex = 0;
	for(int i=0;i<NUM_SUBMISSION_BUFFERS; i++)
	{
		m_submissionBuffers[i] = NULL;
	}

	m_hSubmitMutex = CreateMutex(NULL,false,"RenderSubmit");

	// Now create the thread
	m_hThread = (HANDLE)_beginthread( Helix::RenderThreadFunc, STACK_SIZE, NULL );
//	_ASSERT(m_hThread != 1L);

}

// ****************************************************************************
// ****************************************************************************
bool GetRenderThreadShutdown()
{
	return m_renderThreadShutdown;
}

// ****************************************************************************
// ****************************************************************************
void ShutDownRenderThread()
{
	m_renderThreadShutdown = true;

	// Wait for the thread to exit
	DWORD result = WaitForSingleObject(m_rendererReady,INFINITE);
	_ASSERT(result == WAIT_OBJECT_0);

	CloseHandle(m_startRenderEvent);
	CloseHandle(m_rendererReady);

	// Thread safety first!
	AcquireMutex();

	for(int i=0;i<NUM_SUBMISSION_BUFFERS;i++)
	{
		RenderData *obj = m_submissionBuffers[i];
		while(obj != NULL)
		{
			RenderData *nextObj = obj->next;
			delete obj;
			obj = nextObj;
		}
	}
	ReleaseMutex();
	CloseHandle(m_hSubmitMutex);
}

// ****************************************************************************
// ****************************************************************************
void RenderScene()
{
	DWORD result = SetEvent(m_startRenderEvent);
	_ASSERT(result != 0);
}

// ****************************************************************************
// I want to have the renderer take meshes and materials separately.
// But in the case where I am rendering an instance, I will pull the name of
// the material out of the mesh.
// ****************************************************************************
void SubmitInstance(Instance &inst)
{
	// Thread safety first
	AcquireMutex();

	// Create an object which holds our rendering information
	RenderData *obj = new RenderData;

	// Save the world matrix
	obj->worldMatrix = inst.GetWorldMatrix();

	// Save the name of the mesh
	obj->meshName = inst.GetMeshName();
	
	// Get the name of the material
	Mesh *mesh = MeshManager::GetInstance().GetMesh(obj->meshName);
	obj->materialName = mesh->GetMaterialName();

	// Setup the object
	obj->next = m_submissionBuffers[m_submissionIndex];

	// Put it at the head of the list
	m_submissionBuffers[m_submissionIndex] = obj;

	// Thread safety first
	ReleaseMutex();
}

// ****************************************************************************
// ****************************************************************************
void SubmitViewMatrix(D3DXMATRIX &mat)
{
	AcquireMutex();
	m_viewMatrix[m_submissionIndex] = mat;
	ReleaseMutex();
}

// ****************************************************************************
// ****************************************************************************
void SubmitProjMatrix(D3DXMATRIX &mat)
{
	AcquireMutex();
	m_projMatrix[m_submissionIndex] = mat;
	ReleaseMutex();
}

// ****************************************************************************
// ****************************************************************************
void RenderThreadFunc(void *data)
{
	while(!GetRenderThreadShutdown())
	{
		DWORD result = SetEvent(m_rendererReady);
		_ASSERT(result != 0);

		result = WaitForSingleObject(m_startRenderEvent,INFINITE);
		_ASSERT(result == WAIT_OBJECT_0);

		// Thread safety first
		AcquireMutex();

		int renderSubmissionIndex = m_submissionIndex;
		m_submissionIndex = (m_submissionIndex + 1 ) % NUM_SUBMISSION_BUFFERS;

		ReleaseMutex();

		ID3D10Device *device = m_D3DDevice;
		_ASSERT(device);

		// Reset our view
		ID3D10ShaderResourceView*const pSRV[1] = { NULL };
		device->PSSetShaderResources( 0, 1, pSRV );

		//
		// Clear the render targets
		//
		float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red, green, blue, alpha
		device->ClearRenderTargetView( m_albedoRTView, ClearColor );
		device->ClearDepthStencilView( m_depthStencilDSView, D3D10_CLEAR_DEPTH, 1.0f, 0);
		//device->ClearRenderTargetView( m_backBufferView, ClearColor );

		// Set our render targets
		device->OMSetRenderTargets(1, &m_albedoRTView, m_depthStencilDSView);
		//device->OMSetRenderTargets(1,&m_backBufferView,NULL);

		// Setup camera parameters
		Helix::ShaderManager::GetInstance().SetSharedParameter("WorldView",m_viewMatrix[renderSubmissionIndex]);
		Helix::ShaderManager::GetInstance().SetSharedParameter("Projection",m_projMatrix[renderSubmissionIndex]);

		// Go through all of our render objects
		RenderData *obj = m_submissionBuffers[renderSubmissionIndex];
		while(obj)
		{
			// Set the parameters
			Material *mat = MaterialManager::GetInstance().GetMaterial(obj->materialName);
			mat->SetParameters();

			// Set our input assembly buffers
			Mesh *mesh = MeshManager::GetInstance().GetMesh(obj->meshName);
			Shader *shader = ShaderManager::GetInstance().GetShader(mat->GetShaderName());

			// Set the input layout 
			device->IASetInputLayout(shader->GetDecl().GetLayout());

			// Set our vertex/index buffers
			unsigned int stride = shader->GetDecl().VertexSize();
			unsigned int offset = 0;
			ID3D10Buffer *vb = mesh->GetVertexBuffer();
			device->IASetVertexBuffers(0,1,&vb,&stride,&offset);
			device->IASetIndexBuffer(mesh->GetIndexBuffer(),DXGI_FORMAT_R16_UINT,0);

			// Set our prim type
			device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

			// Start rendering
			ID3D10Effect *effect = shader->GetEffect();
			D3D10_TECHNIQUE_DESC techDesc;
			ID3D10EffectTechnique *technique = effect->GetTechniqueByIndex(0);
			technique->GetDesc(&techDesc);
			for( unsigned int passIndex = 0; passIndex < techDesc.Passes; passIndex++ )
			{
				technique->GetPassByIndex( passIndex )->Apply( 0 );
				device->DrawIndexed( mesh->NumIndices(), 0, 0 );
			}

			// Next
			obj=obj->next;
		}

		// Clear the backbuffer/depth/stencil
		ClearColor[0] = 0.0f;
		ClearColor[1] = 0.0f;
		ClearColor[2] = 0.0f;
		ClearColor[3] = 1.0f;
		device->ClearRenderTargetView( m_backBufferView, ClearColor );
		device->ClearDepthStencilView( m_backDepthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0);

		// Copy the albedo texture to the backbuffer

		// Switch to final backbuffer/depth/stencil
		device->OMSetRenderTargets(1,&m_backBufferView, m_backDepthStencilView);

		// Set our albedo texture as the texture to use
		Shader *shader = ShaderManager::GetInstance().GetShader(m_albedoMaterial->GetShaderName());
		ID3D10Effect *effect = shader->GetEffect();
		ID3D10EffectShaderResourceVariable *shaderResource = effect->GetVariableByName("textureImage")->AsShaderResource();
		HRESULT hr = shaderResource->SetResource(m_albedoSRView);
		_ASSERT(SUCCEEDED(hr));

		// Set our IB/VB
		unsigned int stride = shader->GetDecl().VertexSize();
		unsigned int offset = 0;
		device->IASetVertexBuffers(0,1,&m_quadVB,&stride,&offset);
		device->IASetIndexBuffer(m_quadIB,DXGI_FORMAT_R16_UINT,0);

		// Set our prim type
		device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

		// Set our states
		device->RSSetState(m_RState);

		D3D10_TECHNIQUE_DESC techDesc;
		ID3D10EffectTechnique *technique = effect->GetTechniqueByIndex(0);
		technique->GetDesc(&techDesc);
		for( unsigned int passIndex = 0; passIndex < techDesc.Passes; passIndex++ )
		{
			technique->GetPassByIndex( passIndex )->Apply( 0 );
			device->DrawIndexed( 4, 0, 0 );
		}
		hr = shaderResource->SetResource(NULL);
		_ASSERT(SUCCEEDED(hr));
		m_swapChain->Present(0,0);

		// Delete all our render objects
		obj = m_submissionBuffers[renderSubmissionIndex];
		while(obj != NULL)
		{
			RenderData *nextObj = obj->next;
			delete obj;
			obj = nextObj;
		}

		m_submissionBuffers[renderSubmissionIndex] = NULL;

	}
}

} // namespace Helix
