#include "stdafx.h"
#include <process.h>
#include <crtdbg.h>
#include "RenderThread.h"
#include "RenderMgr.h"
#include "VertexDecl.h"

namespace Helix {

const int					STACK_SIZE =				16*1024;	// 16k
const int					NUM_SUBMISSION_BUFFERS	=	2;
int							m_backbufferWidth = 0;
int							m_backbufferHeight = 0;
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

enum { ALBEDO = 0, NORMAL, DEPTH, MAX_TARGETS };

ID3D10Texture2D *			m_Texture[MAX_TARGETS];
ID3D10RenderTargetView *	m_RTView[MAX_TARGETS];
ID3D10ShaderResourceView *	m_SRView[MAX_TARGETS];

Material *					m_ambientMat = NULL;
Material *					m_dirLightMat = NULL;
Material *					m_pointLightMat = NULL;
Material *					m_showNormalMat = NULL;
Material *					m_showLightLocMat = NULL;
ID3D10Texture2D *			m_depthStencilTexture = NULL;
ID3D10DepthStencilView *	m_depthStencilDSView = NULL;
ID3D10ShaderResourceView *	m_depthStencilSRView = NULL;
ID3D10Buffer *				m_quadVB = NULL;
ID3D10Buffer *				m_quadIB = NULL;
ID3D10RasterizerState *		m_RState = NULL;
ID3D10BlendState *			m_GBufferBlendState = NULL;
ID3D10DepthStencilState *	m_GBufferDSState = NULL;
ID3D10BlendState *			m_lightingBlendState = NULL;
ID3D10DepthStencilState *	m_lightingDSState = NULL;

D3DXVECTOR3					m_sunlightDir(0.0f, -1.0f, 0.0f);		// Sunlight vector
DXGI_RGB					m_sunlightColor = {1.0f, 1.0f, 1.0f};	// Sunlight color
DXGI_RGB					m_ambientColor = {1.0f, 1.0f, 1.0f};	// Ambient color
D3DXVECTOR3					m_pointLightLoc(-8.85f, -9.25f, 8.706f);	// Point light location
DXGI_RGB					m_pointLightColor = {0.0f, 0.0f, 1.0f};		// Point light color

struct RenderData
{
	D3DXMATRIX		worldMatrix;
	std::string		meshName;
	std::string		materialName;
	RenderData *	next;
};

int				m_submissionIndex = 0;
int				m_renderIndex = 0;
RenderData *	m_submissionBuffers[NUM_SUBMISSION_BUFFERS];
D3DXMATRIX		m_viewMatrix[NUM_SUBMISSION_BUFFERS];
D3DXMATRIX		m_projMatrix[NUM_SUBMISSION_BUFFERS];

float			m_cameraNear = 0;
float			m_cameraFar = 0;
float			m_imageWidth = 0;
float			m_imageHeight = 0;
float			m_fovY = 0;
float			m_fov = 0;
float			m_viewAspect;
float			m_invTanHalfFOV = 0;

bool			m_showNormals = false;
bool			m_showLightLocs = false;

struct QuadVert {
	float	pos[3];
	float	uv[2];
};

void	CreateViews();
void	CreateBackbufferViews();
void	CreateColorTarget();
void	CreateDepthTarget();
void	CreateDepthStencilTarget();
void	CreateNormalTarget();
void	CreateQuad();
void	CreateRenderStates();

void	FillGBuffer();
void	DoLighting();
void	ShowNormals();

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
void SetSunlightDir(const D3DXVECTOR3 &dir)
{
	// Normalize our vector
	D3DXVec3Normalize(&m_sunlightDir,&dir);
}

// ****************************************************************************
// ****************************************************************************
void SetSunlightColor(const DXGI_RGB &color)
{	
	m_sunlightColor = color;

	// Clamp values (0..1)
	m_sunlightColor.Red = m_sunlightColor.Red < 0.0f ? 0.0f : m_sunlightColor.Red;
	m_sunlightColor.Green = m_sunlightColor.Green < 0.0f ? 0.0f : m_sunlightColor.Green;
	m_sunlightColor.Blue = m_sunlightColor.Blue < 0.0f ? 0.0f : m_sunlightColor.Blue;

	m_sunlightColor.Red = m_sunlightColor.Red > 1.0f ? 1.0f : m_sunlightColor.Red;
	m_sunlightColor.Green = m_sunlightColor.Green > 1.0f ? 1.0f : m_sunlightColor.Green;
	m_sunlightColor.Blue = m_sunlightColor.Blue > 1.0f ? 1.0f : m_sunlightColor.Blue;
}

// ****************************************************************************
// ****************************************************************************
void SetAmbientColor(const DXGI_RGB &color)
{
	m_ambientColor = color;
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
// - Creates a render target view of the backbuffer device data
// - Creates a depth/stencil texture for the hardware (DXGI_FORMAT_D32_FLOAT)
// - Creates a depth/stencil view 
// ****************************************************************************
void CreateBackbufferViews()
{
	// Get the back buffer and desc
    ID3D10Texture2D* pBuffer;
    HRESULT hr = m_swapChain->GetBuffer( 0, __uuidof( ID3D10Texture2D ), ( LPVOID* )&pBuffer );
	_ASSERT( SUCCEEDED(hr) );

    D3D10_TEXTURE2D_DESC backBufferSurfaceDesc;
    pBuffer->GetDesc( &backBufferSurfaceDesc );

	// Save off our width/height
	m_backbufferWidth = backBufferSurfaceDesc.Width;
	m_backbufferHeight = backBufferSurfaceDesc.Height;

    hr = m_D3DDevice->CreateRenderTargetView( pBuffer, NULL, &m_backBufferView );
    pBuffer->Release();
	_ASSERT( SUCCEEDED(hr) );

	TextureManager::GetInstance().AddTexture(new Texture(m_backBufferView),"[backbuffer]");

	// Create depth stencil texture
    D3D10_TEXTURE2D_DESC descDepth;
    descDepth.Width = m_backbufferWidth;
    descDepth.Height = m_backbufferHeight;
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

	TextureManager::GetInstance().AddTexture(new Texture(m_backDepthStencilView), "[backdepthstencil]");
	_ASSERT( SUCCEEDED(hr) );
}

// ****************************************************************************
// - Create a color render target texture (DXGI_FORMAT_R8G8B8A8_UNORM)
// - Create a render target view for the color render target
// - Create a shader view for the color render target
// ****************************************************************************
void CreateColorTarget()
{
	// Create some empty render targets
	D3D10_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = m_backbufferWidth;
	desc.Height = m_backbufferHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //DXGI_FORMAT_R10G10B10A2_UNORM ;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D10_USAGE_DEFAULT;
	desc.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;

	// Create the texture
	HRESULT hr = m_D3DDevice->CreateTexture2D( &desc, NULL, &m_Texture[ALBEDO] );
	_ASSERT( SUCCEEDED(hr) );

	// Create the render target view
	D3D10_RENDER_TARGET_VIEW_DESC rtDesc;
	rtDesc.Format = desc.Format;
	rtDesc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;
	rtDesc.Texture2D.MipSlice = 0;

	hr = m_D3DDevice->CreateRenderTargetView( m_Texture[ALBEDO], &rtDesc, &m_RTView[ALBEDO] );
	_ASSERT( SUCCEEDED(hr) );

	TextureManager::GetInstance().AddTexture(new Texture(m_RTView[ALBEDO]), "[albedotarget]");

	// Create the shader input view
	D3D10_SHADER_RESOURCE_VIEW_DESC srDesc;
	srDesc.Format = desc.Format;
	srDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MostDetailedMip = 0;
	srDesc.Texture2D.MipLevels = 1;

	hr = m_D3DDevice->CreateShaderResourceView( m_Texture[ALBEDO], &srDesc, &m_SRView[ALBEDO] );
	_ASSERT( SUCCEEDED(hr) );

	TextureManager::GetInstance().AddTexture(new Texture(m_SRView[ALBEDO]), "[albedoshader]");
}

// ****************************************************************************
// ****************************************************************************
void CreateDepthTarget()
{
	// Create some empty render targets
	D3D10_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = m_backbufferWidth;
	desc.Height = m_backbufferHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D10_USAGE_DEFAULT;
	desc.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;

	// Create the texture
	HRESULT hr = m_D3DDevice->CreateTexture2D( &desc, NULL, &m_Texture[DEPTH] );
	_ASSERT( SUCCEEDED(hr) );

	// Create the render target view
	D3D10_RENDER_TARGET_VIEW_DESC rtDesc;
	rtDesc.Format = desc.Format;
	rtDesc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;
	rtDesc.Texture2D.MipSlice = 0;

	hr = m_D3DDevice->CreateRenderTargetView( m_Texture[DEPTH], &rtDesc, &m_RTView[DEPTH] );
	_ASSERT( SUCCEEDED(hr) );

	TextureManager::GetInstance().AddTexture(new Texture(m_RTView[DEPTH]), "[depthtarget]");

	// Create the shader input view
	D3D10_SHADER_RESOURCE_VIEW_DESC srDesc;
	srDesc.Format = desc.Format;
	srDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MostDetailedMip = 0;
	srDesc.Texture2D.MipLevels = 1;

	hr = m_D3DDevice->CreateShaderResourceView( m_Texture[DEPTH], &srDesc, &m_SRView[DEPTH] );
	_ASSERT( SUCCEEDED(hr) );

	TextureManager::GetInstance().AddTexture(new Texture(m_SRView[DEPTH]), "[depthshader]");
}
// ****************************************************************************
// - Create a depth/stencil target texture (DXGI_FORMAT_R16_TYPELESS)
// - Create a depth/stencil view
// - Create a shader resource view for our depth/stencil 
// ****************************************************************************
void CreateDepthStencilTarget()
{
	// Create a depth/stencil texture
	D3D10_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = m_backbufferWidth;
	desc.Height = m_backbufferHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R16_TYPELESS ;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D10_USAGE_DEFAULT;
	desc.BindFlags = D3D10_BIND_DEPTH_STENCIL | D3D10_BIND_SHADER_RESOURCE;

	HRESULT hr = m_D3DDevice->CreateTexture2D( &desc, NULL, &m_depthStencilTexture );
	_ASSERT( SUCCEEDED(hr) );

	// Create the depth/stencil view
	D3D10_DEPTH_STENCIL_VIEW_DESC dsDesc;
	ZeroMemory(&dsDesc,sizeof(dsDesc));
    dsDesc.Format = DXGI_FORMAT_D16_UNORM;
	dsDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
    dsDesc.Texture2D.MipSlice = 0;

    hr = m_D3DDevice->CreateDepthStencilView(m_depthStencilTexture,&dsDesc,&m_depthStencilDSView );
	_ASSERT( SUCCEEDED(hr) );

	TextureManager::GetInstance().AddTexture(new Texture(m_depthStencilDSView), "[depthstenciltarget]");

	// Create our shader resource view
	D3D10_SHADER_RESOURCE_VIEW_DESC srDesc;
	ZeroMemory(&srDesc, sizeof(srDesc));
	srDesc.Format = DXGI_FORMAT_R16_UNORM;
	srDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MostDetailedMip = 0;
	srDesc.Texture2D.MipLevels = 1;

	hr = m_D3DDevice->CreateShaderResourceView(m_depthStencilTexture,&srDesc,&m_depthStencilSRView );
	_ASSERT( SUCCEEDED(hr) );

	TextureManager::GetInstance().AddTexture(new Texture(m_depthStencilSRView), "[depthstencilshader]");
}

// ****************************************************************************
// ****************************************************************************
void CreateNormalTarget()
{
	// Create some empty render targets
	D3D10_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = m_backbufferWidth;
	desc.Height = m_backbufferHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R16G16B16A16_SNORM ;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D10_USAGE_DEFAULT;
	desc.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;

	// Create the texture
	HRESULT hr = m_D3DDevice->CreateTexture2D( &desc, NULL, &m_Texture[NORMAL] );
	_ASSERT( SUCCEEDED(hr) );

	// Create the render target view
	D3D10_RENDER_TARGET_VIEW_DESC rtDesc;
	rtDesc.Format = desc.Format;
	rtDesc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;
	rtDesc.Texture2D.MipSlice = 0;

	hr = m_D3DDevice->CreateRenderTargetView( m_Texture[NORMAL], &rtDesc, &m_RTView[NORMAL] );
	_ASSERT( SUCCEEDED(hr) );

	TextureManager::GetInstance().AddTexture(new Texture(m_RTView[NORMAL]), "[normaltarget]");

	// Create the shader input view
	D3D10_SHADER_RESOURCE_VIEW_DESC srDesc;
	srDesc.Format = desc.Format;
	srDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MostDetailedMip = 0;
	srDesc.Texture2D.MipLevels = 1;

	hr = m_D3DDevice->CreateShaderResourceView( m_Texture[NORMAL], &srDesc, &m_SRView[NORMAL] );
	_ASSERT( SUCCEEDED(hr) );

	TextureManager::GetInstance().AddTexture(new Texture(m_SRView[NORMAL]), "[normalshader]");
}

// ****************************************************************************
// Creates our backbuffer views (color/depth/stencil) as well as our various
// render targets
// ****************************************************************************
void CreateViews()
{
	CreateBackbufferViews();
	CreateColorTarget();
	CreateDepthTarget();
	CreateDepthStencilTarget();
	CreateNormalTarget();
}
// ****************************************************************************
// ****************************************************************************
void LoadLightShaders()
{
	// Load our ambient light
	m_ambientMat = MaterialManager::GetInstance().Load("ambient");
	_ASSERT( m_ambientMat != NULL); 

	// Load our directional light 
	m_dirLightMat = MaterialManager::GetInstance().Load("dirlight");
	_ASSERT( m_dirLightMat != NULL);

	m_pointLightMat = MaterialManager::GetInstance().Load("pointlight");
	_ASSERT( m_pointLightMat != NULL);

	m_showNormalMat = MaterialManager::GetInstance().Load("shownormals");
	_ASSERT(m_showNormalMat != NULL);

	m_showLightLocMat = MaterialManager::GetInstance().Load("showlightloc");
	_ASSERT(m_showLightLocMat != NULL);
}

// ****************************************************************************
// ****************************************************************************
void LoadShapes()
{
	MeshManager::GetInstance().Load("[lightsphere]","lightsphere");
}

// ****************************************************************************
// ****************************************************************************
void CreateQuad()
{
	// Vertex buffer descriptor
	int vertexSize = 3*sizeof(float) + 2*sizeof(float);
	QuadVert quadVerts[4];

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
	HRESULT hr = m_D3DDevice->CreateBuffer(&bufferDesc,&initData,&m_quadVB);
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

}

// ****************************************************************************
// ****************************************************************************
void CreateRenderStates()
{
	// Setup our rasterizer state
	D3D10_RASTERIZER_DESC rDesc;
	memset(&rDesc,0,sizeof(rDesc));
	rDesc.FillMode = D3D10_FILL_SOLID;
	rDesc.CullMode = D3D10_CULL_BACK;
	rDesc.FrontCounterClockwise = false;
	rDesc.DepthBias = 0;
	rDesc.DepthBiasClamp = 0.0f;
	rDesc.SlopeScaledDepthBias = 0.0f;
	rDesc.DepthClipEnable = true;
	rDesc.ScissorEnable = false;
	rDesc.MultisampleEnable = false;
	rDesc.AntialiasedLineEnable = false;
	HRESULT hr = m_D3DDevice->CreateRasterizerState(&rDesc, &m_RState);
	_ASSERT( SUCCEEDED(hr) );

	// Create a blend state for creating GBuffer
	D3D10_BLEND_DESC blendStateDesc;
	memset(&blendStateDesc,0,sizeof(blendStateDesc));
	blendStateDesc.AlphaToCoverageEnable = false;
	for(int i=0;i<8;i++)
		blendStateDesc.BlendEnable[i] = FALSE;
	blendStateDesc.SrcBlend = D3D10_BLEND_SRC_COLOR;
	blendStateDesc.DestBlend = D3D10_BLEND_DEST_COLOR;
	blendStateDesc.BlendOp = D3D10_BLEND_OP_ADD;
	blendStateDesc.SrcBlendAlpha = D3D10_BLEND_SRC_ALPHA;
	blendStateDesc.DestBlendAlpha = D3D10_BLEND_DEST_ALPHA;
	blendStateDesc.BlendOpAlpha = D3D10_BLEND_OP_ADD;
	for(int i=0;i<8;i++)
		blendStateDesc.RenderTargetWriteMask[i] = D3D10_COLOR_WRITE_ENABLE_ALL ;

	hr = m_D3DDevice->CreateBlendState(&blendStateDesc,&m_GBufferBlendState);
	_ASSERT(hr == S_OK);
	
	// Depth/stencil for creating GBuffer
	D3D10_DEPTH_STENCIL_DESC depthStencilStateDesc;
	memset(&blendStateDesc,0,sizeof(depthStencilStateDesc));

	depthStencilStateDesc.DepthEnable = true;								// Enable depth testing
	depthStencilStateDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;		// Don't write to depth/stencil
	depthStencilStateDesc.DepthFunc = D3D10_COMPARISON_LESS_EQUAL;			// Pass if closer
	depthStencilStateDesc.StencilEnable = FALSE;							// No stencil
	depthStencilStateDesc.StencilReadMask = D3D10_DEFAULT_STENCIL_WRITE_MASK; 
	depthStencilStateDesc.StencilWriteMask = D3D10_DEFAULT_STENCIL_WRITE_MASK;

	D3D10_DEPTH_STENCILOP_DESC stencilOp;
	memset(&blendStateDesc,0,sizeof(stencilOp));
	stencilOp.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	stencilOp.StencilDepthFailOp = D3D10_STENCIL_OP_KEEP;
	stencilOp.StencilPassOp = D3D10_STENCIL_OP_KEEP;
	stencilOp.StencilFunc = D3D10_COMPARISON_ALWAYS;

	depthStencilStateDesc.FrontFace = stencilOp;
	depthStencilStateDesc.BackFace = stencilOp;

	ID3D10DepthStencilState *depthStencilState = NULL;
	hr = m_D3DDevice->CreateDepthStencilState(&depthStencilStateDesc,&m_GBufferDSState);
	_ASSERT(hr == S_OK);

	// Depth/stencil for light blending
	memset(&blendStateDesc,0,sizeof(depthStencilStateDesc));
	depthStencilStateDesc.DepthEnable = false;								// Enable depth testing
	depthStencilStateDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ZERO;		// Don't write to depth/stencil
	depthStencilStateDesc.DepthFunc = D3D10_COMPARISON_LESS_EQUAL;			// Pass if closer
	depthStencilStateDesc.StencilEnable = FALSE;							// No stencil
	depthStencilStateDesc.StencilReadMask = D3D10_DEFAULT_STENCIL_WRITE_MASK; 
	depthStencilStateDesc.StencilWriteMask = D3D10_DEFAULT_STENCIL_WRITE_MASK;

	memset(&blendStateDesc,0,sizeof(stencilOp));
	stencilOp.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	stencilOp.StencilDepthFailOp = D3D10_STENCIL_OP_KEEP;
	stencilOp.StencilPassOp = D3D10_STENCIL_OP_KEEP;
	stencilOp.StencilFunc = D3D10_COMPARISON_ALWAYS;

	depthStencilStateDesc.FrontFace = stencilOp;
	depthStencilStateDesc.BackFace = stencilOp;

	depthStencilState = NULL;
	hr = m_D3DDevice->CreateDepthStencilState(&depthStencilStateDesc,&m_lightingDSState);
	_ASSERT(hr == S_OK);

	// Create a blend state for light blending
	memset(&blendStateDesc,0,sizeof(blendStateDesc));
	blendStateDesc.AlphaToCoverageEnable = false;
	blendStateDesc.BlendEnable[0] = TRUE;
	blendStateDesc.SrcBlend = D3D10_BLEND_ONE;
	blendStateDesc.DestBlend = D3D10_BLEND_ONE;
	blendStateDesc.BlendOp = D3D10_BLEND_OP_ADD;
	blendStateDesc.SrcBlendAlpha = D3D10_BLEND_SRC_ALPHA;
	blendStateDesc.DestBlendAlpha = D3D10_BLEND_DEST_ALPHA;
	blendStateDesc.BlendOpAlpha = D3D10_BLEND_OP_ADD;
	for(int i=0;i<8;i++)
		blendStateDesc.RenderTargetWriteMask[i] = D3D10_COLOR_WRITE_ENABLE_ALL ;

	hr = m_D3DDevice->CreateBlendState(&blendStateDesc,&m_lightingBlendState);
	_ASSERT(hr == S_OK);

}

// ****************************************************************************
// ****************************************************************************
void SetDevice(ID3D10Device* dev, IDXGISwapChain *swapChain)
{
	_ASSERT(dev != NULL);
	_ASSERT( swapChain != NULL) ;
	m_D3DDevice = dev;
	m_swapChain = swapChain;

	CreateViews();
	LoadLightShaders();
	LoadShapes();
	CreateQuad();
	CreateRenderStates();


	// Set our viewport
	D3D10_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = m_backbufferWidth;
	vp.Height = m_backbufferHeight;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	m_D3DDevice->RSSetViewports(1, &vp);

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
	// This call happens from the main thread.  Setup our render/submission
	// indices before kicking off the renderer
	m_renderIndex = m_submissionIndex;
	m_submissionIndex = (m_submissionIndex + 1 ) % NUM_SUBMISSION_BUFFERS;
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

	m_cameraNear = 1.0f;
	m_cameraFar = 200.0f;
	m_imageWidth = 1024.0f;
	m_imageHeight = 768.0f;
	m_fovY = (float)D3DX_PI/4.0f; //D3DXToRadian(45);
	m_viewAspect = m_imageWidth/m_imageHeight;

	float d = .5f * m_imageHeight * 1.0f/tan(m_fovY/2.0f);
	float fov = 2 * atan(0.5f*m_imageWidth/d);
	//float deg =	static_cast<float>(D3DXToDegree(fov));
	//deg = static_cast<float>(D3DXToDegree(m_fovY));
	m_fov = fov;
	m_invTanHalfFOV = 1.0f/tan(fov/2.0f);
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
void ShowLightLocations()
{
	ID3D10Device *device = m_D3DDevice;

	// Set our textures as inputs
	Mesh *lightSphere = MeshManager::GetInstance().GetMesh("[lightsphere]");
	Shader *shader = ShaderManager::GetInstance().GetShader(m_showLightLocMat->GetShaderName());
	ID3D10Effect *effect = shader->GetEffect();

	// Set the world*view matrix for the point light
	D3DXMATRIX viewMat = m_viewMatrix[m_renderIndex];

	// TODO: Get the world matrix from the object.  
	// Use I for now
	D3DXMATRIX scaleMat;
	D3DXMatrixScaling(&scaleMat,1.0f, 1.0f, 1.0f);
	D3DXMATRIX transMat;
	D3DXMatrixTranslation(&transMat,m_pointLightLoc.x, m_pointLightLoc.y, m_pointLightLoc.z);

	D3DXMATRIX worldMat;
	D3DXMatrixMultiply(&worldMat,&scaleMat,&transMat);

	// Calculate the WorldView matrix
	D3DXMATRIX worldView;
	D3DXMatrixMultiply(&worldView,&worldMat,&viewMat);
	Helix::ShaderManager::GetInstance().SetSharedParameter("WorldView",worldView);

	// Position
	ID3D10EffectVectorVariable *vecVar = effect->GetVariableByName("pointLoc")->AsVector();
	_ASSERT( vecVar != NULL);
	vecVar->SetFloatVector(m_pointLightLoc);

	// Set our IB/VB
	unsigned int stride = shader->GetDecl().VertexSize();
	unsigned int offset = 0;
	ID3D10Buffer *vb = lightSphere->GetVertexBuffer();
	ID3D10Buffer *ib = lightSphere->GetIndexBuffer();
	device->IASetVertexBuffers(0,1,&vb,&stride,&offset);
	device->IASetIndexBuffer(ib,DXGI_FORMAT_R16_UINT,0);

	// Set our prim type
	device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	// Set our states
	device->RSSetState(m_RState);

	D3D10_TECHNIQUE_DESC techDesc;
	ID3D10EffectTechnique *technique = effect->GetTechniqueByIndex(0);
	technique->GetDesc(&techDesc);
	for( unsigned int passIndex = 0; passIndex < techDesc.Passes; passIndex++ )
	{
		technique->GetPassByIndex( passIndex )->Apply( 0 );
		device->DrawIndexed( lightSphere->NumIndices(), 0, 0 );
	}
}

// ****************************************************************************
// ****************************************************************************
void ShowNormals()
{
	ID3D10Device *device = m_D3DDevice;

	// Clear the backbuffer/depth/stencil
	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; // red, green, blue, alpha
	device->ClearRenderTargetView( m_backBufferView, ClearColor );
	device->ClearDepthStencilView( m_backDepthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0);

	// Switch to final backbuffer/depth/stencil
	device->OMSetRenderTargets(1,&m_backBufferView, m_backDepthStencilView);

	// Set our textures as inputs
	Shader *shader = ShaderManager::GetInstance().GetShader("shownormals");
	ID3D10Effect *effect = shader->GetEffect();

	// Normal texture
	ID3D10EffectShaderResourceVariable *shaderResource = effect->GetVariableByName("normalTexture")->AsShaderResource();
	_ASSERT(shaderResource != NULL);
	HRESULT hr = shaderResource->SetResource(m_SRView[NORMAL]);
	_ASSERT( SUCCEEDED(hr) );

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

	// Clear the inputs on the shader
	hr = shaderResource->SetResource(NULL);
	_ASSERT(SUCCEEDED(hr));

	for( unsigned int passIndex = 0; passIndex < techDesc.Passes; passIndex++ )
	{
		technique->GetPassByIndex( passIndex )->Apply( 0 );
	}

}
// ****************************************************************************
// ****************************************************************************
void RenderSunlight()
{
	ID3D10Device *device = m_D3DDevice;

	// Set our textures as inputs
	Shader *shader = ShaderManager::GetInstance().GetShader(m_dirLightMat->GetShaderName());
	ID3D10Effect *effect = shader->GetEffect();

	// Albedo texture
	ID3D10EffectShaderResourceVariable *shaderResource = effect->GetVariableByName("albedoTexture")->AsShaderResource();
	_ASSERT(shaderResource != NULL);
	HRESULT hr = shaderResource->SetResource(m_SRView[ALBEDO]);
	_ASSERT( SUCCEEDED(hr) );

	// Normal texture
	shaderResource = effect->GetVariableByName("normalTexture")->AsShaderResource();
	_ASSERT(shaderResource != NULL);
	hr = shaderResource->SetResource(m_SRView[NORMAL]);
	_ASSERT( SUCCEEDED(hr) );

	// *************
	// Setup sunlight vector
	// *************

	// Reverse the direction so that it points towards the sun. 
	// This makes it easier in the shader since the light vector
	// and the normal will be in the same direction.
	D3DXVECTOR3 vecData = -m_sunlightDir;

	// Make sure vector is normalized
	D3DXVec3Normalize(&vecData, &vecData);

	// Store
	ID3D10EffectVectorVariable *vecVar = effect->GetVariableByName("sunDir")->AsVector();
	_ASSERT( vecVar != NULL );
	vecVar->SetFloatVector(vecData);

	// Setup sunlight color
	vecData.x = m_sunlightColor.Red;
	vecData.y = m_sunlightColor.Green;
	vecData.z = m_sunlightColor.Blue;

	// Store
	vecVar = effect->GetVariableByName("sunColor")->AsVector();
	_ASSERT( vecVar != NULL);
	vecVar->SetFloatVector(vecData);

	// *************
	// Setup ambient color
	// *************
	vecData.x = m_ambientColor.Red;
	vecData.y = m_ambientColor.Green;
	vecData.z = m_ambientColor.Blue;

	// Make sure it is normalized
	D3DXVec3Normalize(&vecData, &vecData);

	vecVar = effect->GetVariableByName("ambientColor")->AsVector();
	_ASSERT( vecVar != NULL );
	vecVar->SetFloatVector(vecData);
	
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

	// Clear inputs on the shader
	hr = shaderResource->SetResource(NULL);
	_ASSERT(SUCCEEDED(hr));

	for( unsigned int passIndex = 0; passIndex < techDesc.Passes; passIndex++ )
	{
		technique->GetPassByIndex( passIndex )->Apply( 0 );
	}
}
// ****************************************************************************
// ****************************************************************************
void RenderAmbientLight()
{
	ID3D10Device *device = m_D3DDevice;

	// Set our textures as inputs
	Shader *shader = ShaderManager::GetInstance().GetShader(m_dirLightMat->GetShaderName());
	ID3D10Effect *effect = shader->GetEffect();

	// Albedo texture
	ID3D10EffectShaderResourceVariable *shaderResource = effect->GetVariableByName("albedoTexture")->AsShaderResource();
	_ASSERT(shaderResource != NULL);
	HRESULT hr = shaderResource->SetResource(m_SRView[ALBEDO]);
	_ASSERT( SUCCEEDED(hr) );

	//// Normal texture
	//ID3D10EffectShaderResourceVariable *normalResource = effect->GetVariableByName("normalTexture")->AsShaderResource();
	//_ASSERT(normalResource != NULL);
	//hr = normalResource->SetResource(m_SRView[NORMAL]);
	//_ASSERT( SUCCEEDED(hr) );

	// *************
	// Setup ambient color
	// *************
	D3DXVECTOR3 vecData;
	vecData.x = m_ambientColor.Red;
	vecData.y = m_ambientColor.Green;
	vecData.z = m_ambientColor.Blue;

	// Make sure it is normalized
	D3DXVec3Normalize(&vecData, &vecData);

	ID3D10EffectVectorVariable *vecVar = effect->GetVariableByName("ambientColor")->AsVector();
	_ASSERT( vecVar != NULL );
	vecVar->SetFloatVector(vecData);
	
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

	// Clear the inputs on the shader
	hr = shaderResource->SetResource(NULL);
	_ASSERT(SUCCEEDED(hr));

	//hr = normalResource->SetResource(NULL);
	//_ASSERT(SUCCEEDED(hr));

	for( unsigned int passIndex = 0; passIndex < techDesc.Passes; passIndex++ )
	{
		technique->GetPassByIndex( passIndex )->Apply( 0 );
	}
}

// ****************************************************************************
// ****************************************************************************
void RenderPointLight()
{
	ID3D10Device *device = m_D3DDevice;

	// Set our textures as inputs
	Mesh *lightSphere = MeshManager::GetInstance().GetMesh("[lightsphere]");
	Material *lightSphereMat = MaterialManager::GetInstance().GetMaterial(lightSphere->GetMaterialName());
	Shader *shader = ShaderManager::GetInstance().GetShader(lightSphereMat->GetShaderName());
	ID3D10Effect *effect = shader->GetEffect();

	// Set the world*view matrix for the point light
	D3DXMATRIX viewMat = m_viewMatrix[m_renderIndex];

	// TODO: Get the world matrix from the object.  
	// Use I for now
	D3DXMATRIX scaleMat;
	D3DXMatrixScaling(&scaleMat,5.0f, 5.0f, 5.0f);
	D3DXMATRIX transMat;
	D3DXMatrixTranslation(&transMat,m_pointLightLoc.x, m_pointLightLoc.y, m_pointLightLoc.z);

	D3DXMATRIX worldMat;
	D3DXMatrixMultiply(&worldMat,&scaleMat,&transMat);

	// Calculate the WorldView matrix
	D3DXMATRIX worldView;
	D3DXMatrixMultiply(&worldView,&worldMat,&viewMat);
	Helix::ShaderManager::GetInstance().SetSharedParameter("WorldView",worldView);

	// Albedo texture
	ID3D10EffectShaderResourceVariable *albedoResource = effect->GetVariableByName("albedoTexture")->AsShaderResource();
	_ASSERT(albedoResource != NULL);
	HRESULT hr = albedoResource->SetResource(m_SRView[ALBEDO]);
	_ASSERT( SUCCEEDED(hr) );

	// Normal texture
	ID3D10EffectShaderResourceVariable *normalResource = effect->GetVariableByName("normalTexture")->AsShaderResource();
	_ASSERT(normalResource != NULL);
	hr = normalResource->SetResource(m_SRView[NORMAL]);
	_ASSERT( SUCCEEDED(hr) );

	// Depth texture
	ID3D10EffectShaderResourceVariable *depthResource= effect->GetVariableByName("depthTexture")->AsShaderResource();
	_ASSERT(depthResource != NULL);
	hr = depthResource->SetResource(m_SRView[DEPTH]);
	_ASSERT( SUCCEEDED(hr) );

	ID3D10EffectScalarVariable *scalarVar = effect->GetVariableByName("cameraNear")->AsScalar();
	hr = scalarVar->SetFloat(m_cameraNear);

	scalarVar = effect->GetVariableByName("cameraFar")->AsScalar();
	hr = scalarVar->SetFloat(m_cameraFar);

	scalarVar = effect->GetVariableByName("imageWidth")->AsScalar();
	hr = scalarVar->SetFloat(m_imageWidth);

	scalarVar = effect->GetVariableByName("imageHeight")->AsScalar();
	hr = scalarVar->SetFloat(m_imageHeight);

	scalarVar = effect->GetVariableByName("fovY")->AsScalar();
	hr = scalarVar->SetFloat(m_fovY);

	scalarVar = effect->GetVariableByName("viewAspect")->AsScalar();
	hr = scalarVar->SetFloat(m_viewAspect);

	scalarVar = effect->GetVariableByName("invTanHalfFOV")->AsScalar();
	hr = scalarVar->SetFloat(m_invTanHalfFOV);

	// *************
	// Setup our point light
	// *************
	//static __int64 qpcStart = 0;
	//static double freq = 0.0;
	//static bool firstTime = true;
	//if(firstTime)
	//{
	//	__int64 qpcFreq;
	//	QueryPerformanceFrequency((LARGE_INTEGER *)&qpcFreq);
	//	freq = 1.0/(double)qpcFreq;
	//	QueryPerformanceCounter((LARGE_INTEGER *)&qpcStart);
	//	firstTime = false;
	//}

	//__int64 qpcNow = 0;
	//QueryPerformanceCounter((LARGE_INTEGER *)&qpcNow);
	//double timeInSec = (qpcNow - qpcStart) * freq;
	//double timeInMS = timeInSec * 1000.0;
	//float scale = static_cast<float>((2.0 * D3DX_PI)/2000.0);
	//float rot = static_cast<float>(timeInMS * scale);


	//D3DXMATRIX rotY;
	//D3DXMatrixRotationY(&rotY,rot);
	//D3DXVECTOR4 temp;
	//D3DXVec3Transform(&temp,&m_pointLightLoc,&rotY);
	//D3DXVECTOR3 newPos = temp;

	// Position
	ID3D10EffectVectorVariable *vecVar = effect->GetVariableByName("pointLoc")->AsVector();
	_ASSERT( vecVar != NULL);
	vecVar->SetFloatVector(m_pointLightLoc);

	// Color
	D3DXVECTOR3 vecData;
	vecData.x = m_pointLightColor.Red;
	vecData.y = m_pointLightColor.Green;
	vecData.z = m_pointLightColor.Blue;
	vecVar = effect->GetVariableByName("pointColor")->AsVector();
	_ASSERT( vecVar != NULL);
	vecVar->SetFloatVector(vecData);

	// *************
	// Setup ambient color
	// *************
	vecData.x = m_ambientColor.Red;
	vecData.y = m_ambientColor.Green;
	vecData.z = m_ambientColor.Blue;

	// Make sure it is normalized
	D3DXVec3Normalize(&vecData, &vecData);

	vecVar = effect->GetVariableByName("ambientColor")->AsVector();
	_ASSERT( vecVar != NULL );
	vecVar->SetFloatVector(vecData);
	
	// Set our IB/VB
	unsigned int stride = shader->GetDecl().VertexSize();
	unsigned int offset = 0;
	ID3D10Buffer *vb = lightSphere->GetVertexBuffer();
	ID3D10Buffer *ib = lightSphere->GetIndexBuffer();
	device->IASetVertexBuffers(0,1,&vb,&stride,&offset);
	device->IASetIndexBuffer(ib,DXGI_FORMAT_R16_UINT,0);

	// Set our prim type
	device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	// Set our states
	device->RSSetState(m_RState);

	D3D10_TECHNIQUE_DESC techDesc;
	ID3D10EffectTechnique *technique = effect->GetTechniqueByIndex(0);
	technique->GetDesc(&techDesc);
	for( unsigned int passIndex = 0; passIndex < techDesc.Passes; passIndex++ )
	{
		technique->GetPassByIndex( passIndex )->Apply( 0 );
		device->DrawIndexed( lightSphere->NumIndices(), 0, 0 );
	}

	// Clear the inputs on the shader
	hr = albedoResource->SetResource(NULL);
	_ASSERT(SUCCEEDED(hr));

	hr = normalResource->SetResource(NULL);
	_ASSERT(SUCCEEDED(hr));

	hr = depthResource->SetResource(NULL);
	_ASSERT(SUCCEEDED(hr));

	for( unsigned int passIndex = 0; passIndex < techDesc.Passes; passIndex++ )
	{
		technique->GetPassByIndex( passIndex )->Apply( 0 );
	}
}

// ****************************************************************************
// ****************************************************************************
void FillGBuffer()
{
	ID3D10Device *device = m_D3DDevice;
	_ASSERT(device);

	// Reset our view
	ID3D10ShaderResourceView*const pSRV[3] = { NULL,NULL,NULL };
	device->PSSetShaderResources( 0, 2, pSRV );

	//
	// Clear the render targets
	//
	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red, green, blue, alpha
	float ClearNormal[3] = { 0.0f, 0.0f, 0.0f };
	float ClearDepth[1] = { 0.0f };
	device->ClearRenderTargetView( m_RTView[ALBEDO], ClearColor );
	device->ClearRenderTargetView( m_RTView[NORMAL], ClearNormal );
	device->ClearRenderTargetView( m_RTView[DEPTH], ClearDepth) ;
	device->ClearDepthStencilView( m_depthStencilDSView, D3D10_CLEAR_DEPTH, 1.0f, 0);
	//device->ClearRenderTargetView( m_backBufferView, ClearColor );

	device->OMSetDepthStencilState(m_GBufferDSState,0);
	FLOAT blendFactor[4] = {0,0,0,0};
	device->OMSetBlendState(m_GBufferBlendState,blendFactor,0xffffffff);

	// Set our render targets
	device->OMSetRenderTargets(3, m_RTView, m_depthStencilDSView);
	//device->OMSetRenderTargets(1,&m_backBufferView,NULL);

	// Setup camera parameters
	Helix::ShaderManager::GetInstance().SetSharedParameter("Projection",m_projMatrix[m_renderIndex]);

	// Get the view matrix
	D3DXMATRIX viewMat = m_viewMatrix[m_renderIndex];
	Helix::ShaderManager::GetInstance().SetSharedParameter("View",viewMat);

	// TODO: Get the world matrix from the object.  
	// Use I for now
	D3DXMATRIX worldMat;
	D3DXMatrixIdentity(&worldMat);

	// Calculate the WorldView matrix
	D3DXMATRIX worldView;
	D3DXMatrixMultiply(&worldView,&worldMat,&viewMat);
	Helix::ShaderManager::GetInstance().SetSharedParameter("WorldView",worldView);

	// Generate the inverse transpose of the WorldView matrix
	// We don't use any non uniform scaling, so we can just send down the 
	// upper 3x3 of the world view matrix
	D3DXMATRIX worldViewIT;
	float det = 0;
	worldViewIT = worldView;
	worldViewIT._14 = worldViewIT._24 = worldViewIT._34 = worldViewIT._41 = worldViewIT._42 = worldViewIT._43 = 0;
	worldViewIT._44 = 1;
	D3DXMatrixTranspose(&worldViewIT,&worldViewIT);
	D3DXMatrixInverse(&worldViewIT,&det,&worldViewIT);
	Helix::ShaderManager::GetInstance().SetSharedParameter("WorldViewIT",worldViewIT);

	// The upper 3x3 of the view matrx
	// Used to transform directional lights
	D3DXMATRIX view3x3 = viewMat;
	view3x3._14 = view3x3._24 = view3x3._34 = view3x3._41 = view3x3._42 = view3x3._43 = 0;
	view3x3._44 = 1;
	Helix::ShaderManager::GetInstance().SetSharedParameter("View3x3",view3x3);

	// Go through all of our render objects
	RenderData *obj = m_submissionBuffers[m_renderIndex];
	if (obj == NULL)
	{
		int a =1;
	}
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
}

// ****************************************************************************
// ****************************************************************************
void DoLighting()
{
	ID3D10Device *device = m_D3DDevice;

	// Clear the backbuffer/depth/stencil
	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; // red, green, blue, alpha
	device->ClearRenderTargetView( m_backBufferView, ClearColor );
	device->ClearDepthStencilView( m_backDepthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0);

	// Copy the albedo texture to the backbuffer

	// Switch to final backbuffer/depth/stencil
	device->OMSetRenderTargets(1,&m_backBufferView, m_backDepthStencilView);

	// Render the scene with ambient into the backbuffer
	RenderAmbientLight();

	device->OMSetDepthStencilState(m_lightingDSState,0);
	FLOAT blendFactor[4] = {0,0,0,0};
	device->OMSetBlendState(m_lightingBlendState,blendFactor,0xffffffff);

	//RenderSunlight();
	//device->ClearDepthStencilView( m_backDepthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0);

	RenderPointLight();
	if(m_showLightLocs)
	{
		ShowLightLocations();
	}

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

		FillGBuffer();

		if(m_showNormals)
		{
			ShowNormals();
		}
		else
		{
			DoLighting();
		}


		m_swapChain->Present(0,0);

		// Delete all our render objects
		RenderData *obj = m_submissionBuffers[m_renderIndex];
		while(obj != NULL)
		{
			RenderData *nextObj = obj->next;
			delete obj;
			obj = nextObj;
		}

		m_submissionBuffers[m_renderIndex] = NULL;

	}
}

} // namespace Helix
