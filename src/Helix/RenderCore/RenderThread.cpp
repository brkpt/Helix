#include "stdafx.h"
#include <process.h>
#include <crtdbg.h>
#include "RenderThread.h"
#include "RenderMgr.h"
#include "VDecls.h"
#include "Light.h"
#include "Materials.h"

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
ID3D11Device *				m_D3DDevice =				NULL;
ID3D11DeviceContext *		m_context =					NULL;
IDXGISwapChain *			m_swapChain =				NULL;

ID3D11RenderTargetView *	m_backBufferView = NULL;
ID3D11Texture2D *			m_backDepthStencil = NULL;
ID3D11DepthStencilView *	m_backDepthStencilView = NULL;

enum { ALBEDO = 0, NORMAL, DEPTH, MAX_TARGETS };

ID3D11Texture2D *			m_Texture[MAX_TARGETS];
ID3D11RenderTargetView *	m_RTView[MAX_TARGETS];
ID3D11ShaderResourceView *	m_SRView[MAX_TARGETS];

HXMaterial *				m_ambientMat = NULL;
HXMaterial *				m_dirLightMat = NULL;
HXMaterial *				m_pointLightMat = NULL;
HXMaterial *				m_showNormalMat = NULL;
HXMaterial *				m_showLightLocMat = NULL;
ID3D11Texture2D *			m_depthStencilTexture = NULL;
ID3D11DepthStencilView *	m_depthStencilDSView = NULL;
ID3D11ShaderResourceView *	m_depthStencilSRView = NULL;
ID3D11Buffer *				m_quadVB = NULL;
ID3D11Buffer *				m_quadIB = NULL;
ID3D11RasterizerState *		m_RState = NULL;
ID3D11BlendState *			m_GBufferBlendState = NULL;
ID3D11DepthStencilState *	m_GBufferDSState = NULL;
ID3D11BlendState *			m_lightingBlendState = NULL;
ID3D11DepthStencilState *	m_lightingDSState = NULL;

D3DXVECTOR3					m_sunlightDir(0.0f, -1.0f, 0.0f);		// Sunlight vector
DXGI_RGB					m_sunlightColor = {1.0f, 1.0f, 1.0f};	// Sunlight color
DXGI_RGB					m_ambientColor = {0.25f, 0.25f, 0.25f};	// Ambient color

struct VS_CONSTANT_BUFFER_FRAME
{
	D3DXMATRIX		m_viewMatrix;
	D3DXMATRIX		m_projMatrix;
	D3DXMATRIX		m_invViewMatrix;
	D3DXMATRIX		m_view3x3;
	D3DXMATRIX		m_invViewProj;
	D3DXMATRIX		m_invProj;
};

struct VS_CONSTANT_BUFFER_OBJECT
{
	D3DXMATRIX		m_worldViewMatrix;
	D3DXMATRIX		m_worldViewIT;
	D3DXMATRIX		m_invWorldViewProj;
};

struct PS_CONSTANT_BUFFER_FRAME
{
	D3DXVECTOR4		m_sunDirection;
	D3DXVECTOR4		m_sunColor;
	D3DXVECTOR4		m_ambientColor;
};

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

Light			m_renderLights[MAX_LIGHTS];
int				m_numRenderLights = 0;

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

ID3D11Buffer	*m_VSFrameConstants = NULL;
ID3D11Buffer	*m_VSObjectConstants = NULL;
ID3D11Buffer	*m_PSFrameConstants = NULL;

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
void	CreateConstantBuffers();

void	FillGBuffer();
void	DoLighting();
void	ShowNormals();

// ****************************************************************************
// ****************************************************************************
// Thread function
// ****************************************************************************
void RenderThreadFunc(void *data);

void	InitializeRenderThread();
void	InitializeThreadLoader();
// ****************************************************************************
// ****************************************************************************
inline int SubmissionIndex()
{
	return m_submissionIndex;
}

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
	ID3D11Texture2D* pBuffer;
	HRESULT hr = m_swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBuffer );
	_ASSERT( SUCCEEDED(hr) );

	D3D11_TEXTURE2D_DESC backBufferSurfaceDesc;
	pBuffer->GetDesc( &backBufferSurfaceDesc );

	// Save off our width/height
	m_backbufferWidth = backBufferSurfaceDesc.Width;
	m_backbufferHeight = backBufferSurfaceDesc.Height;

	hr = m_D3DDevice->CreateRenderTargetView( pBuffer, NULL, &m_backBufferView );
	pBuffer->Release();
	_ASSERT( SUCCEEDED(hr) );

	HXAddTexture(new HXTexture(m_backBufferView),"[backbuffer]");

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = m_backbufferWidth;
	descDepth.Height = m_backbufferHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = m_D3DDevice->CreateTexture2D( &descDepth, NULL, &m_backDepthStencil );
	_ASSERT( SUCCEEDED(hr) );

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	memset(&descDSV, 0, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	//if( descDepth.SampleDesc.Count > 1 )
	//    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	//else
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = m_D3DDevice->CreateDepthStencilView( m_backDepthStencil, &descDSV, &m_backDepthStencilView );

	HXAddTexture(new HXTexture(m_backDepthStencilView), "[backdepthstencil]");
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
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = m_backbufferWidth;
	desc.Height = m_backbufferHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //DXGI_FORMAT_R10G10B10A2_UNORM ;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	// Create the texture
	HRESULT hr = m_D3DDevice->CreateTexture2D( &desc, NULL, &m_Texture[ALBEDO] );
	_ASSERT( SUCCEEDED(hr) );

	// Create the render target view
	D3D11_RENDER_TARGET_VIEW_DESC rtDesc;
	rtDesc.Format = desc.Format;
	rtDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtDesc.Texture2D.MipSlice = 0;

	hr = m_D3DDevice->CreateRenderTargetView( m_Texture[ALBEDO], &rtDesc, &m_RTView[ALBEDO] );
	_ASSERT( SUCCEEDED(hr) );

	HXAddTexture(new HXTexture(m_RTView[ALBEDO]), "[albedotarget]");

	// Create the shader input view
	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
	srDesc.Format = desc.Format;
	srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MostDetailedMip = 0;
	srDesc.Texture2D.MipLevels = 1;

	hr = m_D3DDevice->CreateShaderResourceView( m_Texture[ALBEDO], &srDesc, &m_SRView[ALBEDO] );
	_ASSERT( SUCCEEDED(hr) );

	HXAddTexture(new HXTexture(m_SRView[ALBEDO]), "[albedoshader]");
}

// ****************************************************************************
// ****************************************************************************
void CreateDepthTarget()
{
	// Create some empty render targets
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = m_backbufferWidth;
	desc.Height = m_backbufferHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R16_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	// Create the texture
	HRESULT hr = m_D3DDevice->CreateTexture2D( &desc, NULL, &m_Texture[DEPTH] );
	_ASSERT( SUCCEEDED(hr) );

	// Create the render target view
	D3D11_RENDER_TARGET_VIEW_DESC rtDesc;
	rtDesc.Format = desc.Format;
	rtDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtDesc.Texture2D.MipSlice = 0;

	hr = m_D3DDevice->CreateRenderTargetView( m_Texture[DEPTH], &rtDesc, &m_RTView[DEPTH] );
	_ASSERT( SUCCEEDED(hr) );

	HXAddTexture(new HXTexture(m_RTView[DEPTH]), "[depthtarget]");

	// Create the shader input view
	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
	srDesc.Format = desc.Format;
	srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MostDetailedMip = 0;
	srDesc.Texture2D.MipLevels = 1;

	hr = m_D3DDevice->CreateShaderResourceView( m_Texture[DEPTH], &srDesc, &m_SRView[DEPTH] );
	_ASSERT( SUCCEEDED(hr) );

	HXAddTexture(new HXTexture(m_SRView[DEPTH]), "[depthshader]");
}
// ****************************************************************************
// - Create a depth/stencil target texture (DXGI_FORMAT_R16_TYPELESS)
// - Create a depth/stencil view
// - Create a shader resource view for our depth/stencil 
// ****************************************************************************
void CreateDepthStencilTarget()
{
	// Create a depth/stencil texture
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = m_backbufferWidth;
	desc.Height = m_backbufferHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R16_TYPELESS ;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	HRESULT hr = m_D3DDevice->CreateTexture2D( &desc, NULL, &m_depthStencilTexture );
	_ASSERT( SUCCEEDED(hr) );

	// Create the depth/stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc;
	ZeroMemory(&dsDesc,sizeof(dsDesc));
	dsDesc.Format = DXGI_FORMAT_D16_UNORM;
	dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsDesc.Texture2D.MipSlice = 0;

	hr = m_D3DDevice->CreateDepthStencilView(m_depthStencilTexture,&dsDesc,&m_depthStencilDSView );
	_ASSERT( SUCCEEDED(hr) );

	HXAddTexture(new HXTexture(m_depthStencilDSView), "[depthstenciltarget]");

	// Create our shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
	ZeroMemory(&srDesc, sizeof(srDesc));
	srDesc.Format = DXGI_FORMAT_R16_UNORM;
	srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MostDetailedMip = 0;
	srDesc.Texture2D.MipLevels = 1;

	hr = m_D3DDevice->CreateShaderResourceView(m_depthStencilTexture,&srDesc,&m_depthStencilSRView );
	_ASSERT( SUCCEEDED(hr) );

	HXAddTexture(new HXTexture(m_depthStencilSRView), "[depthstencilshader]");
}

// ****************************************************************************
// ****************************************************************************
void CreateNormalTarget()
{
	// Create some empty render targets
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = m_backbufferWidth;
	desc.Height = m_backbufferHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R16G16B16A16_SNORM ;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	// Create the texture
	HRESULT hr = m_D3DDevice->CreateTexture2D( &desc, NULL, &m_Texture[NORMAL] );
	_ASSERT( SUCCEEDED(hr) );

	// Create the render target view
	D3D11_RENDER_TARGET_VIEW_DESC rtDesc;
	rtDesc.Format = desc.Format;
	rtDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtDesc.Texture2D.MipSlice = 0;

	hr = m_D3DDevice->CreateRenderTargetView( m_Texture[NORMAL], &rtDesc, &m_RTView[NORMAL] );
	_ASSERT( SUCCEEDED(hr) );

	HXAddTexture(new HXTexture(m_RTView[NORMAL]), "[normaltarget]");

	// Create the shader input view
	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
	srDesc.Format = desc.Format;
	srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MostDetailedMip = 0;
	srDesc.Texture2D.MipLevels = 1;

	hr = m_D3DDevice->CreateShaderResourceView( m_Texture[NORMAL], &srDesc, &m_SRView[NORMAL] );
	_ASSERT( SUCCEEDED(hr) );

	HXAddTexture(new HXTexture(m_SRView[NORMAL]), "[normalshader]");
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
	m_ambientMat = HXLoadMaterial("ambient");
	_ASSERT( m_ambientMat != NULL); 

	// Load our directional light 
	m_dirLightMat = HXLoadMaterial("dirlight");
	_ASSERT( m_dirLightMat != NULL);

	m_pointLightMat = HXLoadMaterial("pointlight");
	_ASSERT( m_pointLightMat != NULL);

	m_showNormalMat = HXLoadMaterial("shownormals");
	_ASSERT(m_showNormalMat != NULL);

	m_showLightLocMat = HXLoadMaterial("showlightloc");
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

	D3D11_BUFFER_DESC bufferDesc = {0};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(quadVerts);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	// Data initialization descriptor
	D3D11_SUBRESOURCE_DATA initData = {0};
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
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(ibData);
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
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
	D3D11_RASTERIZER_DESC rDesc;
	memset(&rDesc,0,sizeof(rDesc));
	rDesc.FillMode = D3D11_FILL_SOLID;
	rDesc.CullMode = D3D11_CULL_BACK;
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
	D3D11_BLEND_DESC blendStateDesc;
	memset(&blendStateDesc,0,sizeof(blendStateDesc));
	blendStateDesc.AlphaToCoverageEnable = false;
	blendStateDesc.IndependentBlendEnable = true;
	for(int i=0;i<8;i++)
	{
		blendStateDesc.RenderTarget[i].BlendEnable = FALSE;
		blendStateDesc.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_COLOR;
		blendStateDesc.RenderTarget[i].DestBlend = D3D11_BLEND_DEST_COLOR;
		blendStateDesc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
		blendStateDesc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
		blendStateDesc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_DEST_ALPHA;
		blendStateDesc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendStateDesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL ;
	}
	hr = m_D3DDevice->CreateBlendState(&blendStateDesc,&m_GBufferBlendState);
	_ASSERT( SUCCEEDED( hr ) );
	
	// Depth/stencil for creating GBuffer
	D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
	memset(&blendStateDesc,0,sizeof(depthStencilStateDesc));

	depthStencilStateDesc.DepthEnable = true;								// Enable depth testing
	depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;		// Write to depth/stencil
	depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;			// Pass if closer
	depthStencilStateDesc.StencilEnable = FALSE;							// No stencil
	depthStencilStateDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_WRITE_MASK; 
	depthStencilStateDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	// Dummy structures for the front/back stencil operations
	D3D11_DEPTH_STENCILOP_DESC stencilOp;
	memset(&blendStateDesc,0,sizeof(stencilOp));
	stencilOp.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	stencilOp.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	stencilOp.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	stencilOp.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilStateDesc.FrontFace = stencilOp;
	depthStencilStateDesc.BackFace = stencilOp;

	ID3D11DepthStencilState *depthStencilState = NULL;
	hr = m_D3DDevice->CreateDepthStencilState(&depthStencilStateDesc,&m_GBufferDSState);
	_ASSERT( SUCCEEDED( hr ) );

	// Depth/stencil for light blending
	memset(&blendStateDesc,0,sizeof(depthStencilStateDesc));
	depthStencilStateDesc.DepthEnable = true;								// Enable depth testing
	depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;		// Don't write to depth/stencil
	depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL ;			// Pass if closer
	depthStencilStateDesc.StencilEnable = FALSE;							// No stencil
	depthStencilStateDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_WRITE_MASK; 
	depthStencilStateDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	memset(&blendStateDesc,0,sizeof(stencilOp));
	stencilOp.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	stencilOp.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	stencilOp.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	stencilOp.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilStateDesc.FrontFace = stencilOp;
	depthStencilStateDesc.BackFace = stencilOp;

	depthStencilState = NULL;
	hr = m_D3DDevice->CreateDepthStencilState(&depthStencilStateDesc,&m_lightingDSState);
	_ASSERT( SUCCEEDED( hr ) );

	// Create a blend state for light blending
	memset(&blendStateDesc,0,sizeof(blendStateDesc));
	memset(&blendStateDesc,0,sizeof(blendStateDesc));
	blendStateDesc.AlphaToCoverageEnable = false;
	blendStateDesc.IndependentBlendEnable = true;
	for(int i=0;i<8;i++)
	{
		blendStateDesc.RenderTarget[i].BlendEnable = FALSE;
		blendStateDesc.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_COLOR;
		blendStateDesc.RenderTarget[i].DestBlend = D3D11_BLEND_DEST_COLOR;
		blendStateDesc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
		blendStateDesc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
		blendStateDesc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_DEST_ALPHA;
		blendStateDesc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendStateDesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL ;
	}

	hr = m_D3DDevice->CreateBlendState(&blendStateDesc,&m_lightingBlendState);
	_ASSERT( SUCCEEDED( hr ) );

}

// ****************************************************************************
// ****************************************************************************
void CreateConstantBuffers()
{
    D3D11_BUFFER_DESC bufferDesc;

	// Create per-frame constant buffer
	//memset(&bufferDesc,0,sizeof(bufferDesc));

    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufferDesc.MiscFlags = 0;
	//bufferDesc.StructureByteStride = 0;

    bufferDesc.ByteWidth = sizeof( VS_CONSTANT_BUFFER_FRAME );
    HRESULT hr = m_D3DDevice->CreateBuffer( &bufferDesc, NULL, &m_VSFrameConstants );
	_ASSERT( SUCCEEDED( hr ) );

    bufferDesc.ByteWidth = sizeof( VS_CONSTANT_BUFFER_OBJECT );
    hr = m_D3DDevice->CreateBuffer( &bufferDesc, NULL, &m_VSObjectConstants );
	_ASSERT( SUCCEEDED( hr ) );

    bufferDesc.ByteWidth = sizeof( PS_CONSTANT_BUFFER_FRAME );
	hr = m_D3DDevice->CreateBuffer( &bufferDesc, NULL, &m_PSFrameConstants );
	_ASSERT( SUCCEEDED( hr ) );

}


// ****************************************************************************
// ****************************************************************************
void InitializeRenderer(ID3D11Device* dev, ID3D11DeviceContext *context, IDXGISwapChain *swapChain)
{
	_ASSERT(dev != NULL);
	_ASSERT( swapChain != NULL) ;
	m_D3DDevice = dev;
	m_context = context;
	m_swapChain = swapChain;

	// Initialize managers
	HXInitializeShaders();
	HXInitializeTextures();
	HXInitializeMaterials();
	MeshManager::Create();
	InstanceManager::GetInstance();

	CreateViews();
	LoadLightShaders();
	LoadShapes();
	CreateQuad();
	CreateRenderStates();
	CreateConstantBuffers();

	// Set our viewport
	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = static_cast<float>(m_backbufferWidth);
	vp.Height = static_cast<float>(m_backbufferHeight);
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	m_context->RSSetViewports(1, &vp);

	InitializeLights();

	// Initialize threading
	InitializeRenderThread();
	InitializeThreadLoader();
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
	// Copy light information
	AquireLightMutex();
	Light *submittedLights = SubmittedLights();
	int lightCount = SubmittedLightCount();
	memcpy(m_renderLights, submittedLights, lightCount * sizeof(Light));
	m_numRenderLights = lightCount;
	ResetLights();
	ReleaseLightMutex();

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
	//ID3D11Device *device = m_D3DDevice;

	//// Set our textures as inputs
	//Mesh *lightSphere = MeshManager::GetInstance().GetMesh("[lightsphere]");
	//Shader *shader = ShaderManager::GetInstance().GetShader(m_showLightLocMat->GetShaderName());
	//ID3DX11Effect *effect = shader->GetEffect();

	//// Set the world*view matrix for the point light
	//D3DXMATRIX viewMat = m_viewMatrix[m_renderIndex];

	//// TODO: Get the world matrix from the object.  
	//// Use I for now
	//D3DXMATRIX scaleMat;
	//D3DXMatrixScaling(&scaleMat,1.0f, 1.0f, 1.0f);
	//D3DXMATRIX transMat;
	//D3DXMatrixTranslation(&transMat,m_pointLightLoc.x, m_pointLightLoc.y, m_pointLightLoc.z);

	//D3DXMATRIX worldMat;
	//D3DXMatrixMultiply(&worldMat,&scaleMat,&transMat);

	//// Calculate the WorldView matrix
	//D3DXMATRIX worldView;
	//D3DXMatrixMultiply(&worldView,&worldMat,&viewMat);
	//Helix::ShaderManager::GetInstance().SetSharedParameter("WorldView",worldView);

	//// Position
	//ID3DX11EffectVectorVariable *vecVar = effect->GetVariableByName("pointLoc")->AsVector();
	//_ASSERT( vecVar != NULL);
	//vecVar->SetFloatVector(m_pointLightLoc);

	//// Set our IB/VB
	//unsigned int stride = shader->GetDecl().VertexSize();
	//unsigned int offset = 0;
	//ID3D11Buffer *vb = lightSphere->GetVertexBuffer();
	//ID3D11Buffer *ib = lightSphere->GetIndexBuffer();
	//device->IASetVertexBuffers(0,1,&vb,&stride,&offset);
	//device->IASetIndexBuffer(ib,DXGI_FORMAT_R16_UINT,0);

	//// Set our prim type
	//device->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	//// Set our states
	//device->RSSetState(m_RState);

	//D3D11_TECHNIQUE_DESC techDesc;
	//ID3DX11EffectTechnique *technique = effect->GetTechniqueByIndex(0);
	//technique->GetDesc(&techDesc);
	//for( unsigned int passIndex = 0; passIndex < techDesc.Passes; passIndex++ )
	//{
	//	technique->GetPassByIndex( passIndex )->Apply( 0 );
	//	device->DrawIndexed( lightSphere->NumIndices(), 0, 0 );
	//}
}

// ****************************************************************************
// ****************************************************************************
void ShowNormals()
{
	//ID3D11Device *device = m_D3DDevice;
	//ID3D11DeviceContext *context = m_context;

	//// Clear the backbuffer/depth/stencil
	//float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; // red, green, blue, alpha
	//context->ClearRenderTargetView( m_backBufferView, ClearColor );
	//context->ClearDepthStencilView( m_backDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	//// Switch to final backbuffer/depth/stencil
	//context->OMSetRenderTargets(1,&m_backBufferView, m_backDepthStencilView);

	//// Set our textures as inputs
	//HXShader *shader = HXGetShaderByName("shownormals");
	//ID3DX11Effect *effect = shader->m_pEffect;

	//// Normal texture
	//ID3DX11EffectShaderResourceVariable *shaderResource = effect->GetVariableByName("normalTexture")->AsShaderResource();
	//_ASSERT(shaderResource != NULL);
	//HRESULT hr = shaderResource->SetResource(m_SRView[NORMAL]);
	//_ASSERT( SUCCEEDED(hr) );

	//// Set our IB/VB
	//unsigned int stride = shader->m_decl->m_vertexSize;
	//unsigned int offset = 0;
	//context->IASetVertexBuffers(0,1,&m_quadVB,&stride,&offset);
	//context->IASetIndexBuffer(m_quadIB,DXGI_FORMAT_R16_UINT,0);

	//// Set our prim type
	//context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

	//// Set our states
	//context->RSSetState(m_RState);

	//D3D11_TECHNIQUE_DESC techDesc;
	//ID3DX11EffectTechnique *technique = effect->GetTechniqueByIndex(0);
	//technique->GetDesc(&techDesc);
	//for( unsigned int passIndex = 0; passIndex < techDesc.Passes; passIndex++ )
	//{
	//	technique->GetPassByIndex( passIndex )->Apply( 0 );
	//	context->DrawIndexed( 4, 0, 0 );
	//}

	//// Clear the inputs on the shader
	//hr = shaderResource->SetResource(NULL);
	//_ASSERT(SUCCEEDED(hr));

	//for( unsigned int passIndex = 0; passIndex < techDesc.Passes; passIndex++ )
	//{
	//	technique->GetPassByIndex( passIndex )->Apply( 0 );
	//}

}
// ****************************************************************************
// ****************************************************************************
void RenderSunlight()
{
	//ID3D11Device *device = m_D3DDevice;
	//ID3D11DeviceContext *context = m_context;

	//// Set our textures as inputs
	//HXShader *shader = HXGetShaderByName(m_dirLightMat->m_shaderName);
	//ID3DX11Effect *effect = shader->m_pEffect;

	//// Albedo texture
	//ID3DX11EffectShaderResourceVariable *shaderResource = effect->GetVariableByName("albedoTexture")->AsShaderResource();
	//_ASSERT(shaderResource != NULL);
	//HRESULT hr = shaderResource->SetResource(m_SRView[ALBEDO]);
	//_ASSERT( SUCCEEDED(hr) );

	//// Normal texture
	//shaderResource = effect->GetVariableByName("normalTexture")->AsShaderResource();
	//_ASSERT(shaderResource != NULL);
	//hr = shaderResource->SetResource(m_SRView[NORMAL]);
	//_ASSERT( SUCCEEDED(hr) );

	//// *************
	//// Setup sunlight vector
	//// *************

	//// Reverse the direction so that it points towards the sun. 
	//// This makes it easier in the shader since the light vector
	//// and the normal will be in the same direction.
	//D3DXVECTOR3 vecData = -m_sunlightDir;

	//// Make sure vector is normalized
	//D3DXVec3Normalize(&vecData, &vecData);

	//// Store
	//ID3DX11EffectVectorVariable *vecVar = effect->GetVariableByName("sunDir")->AsVector();
	//_ASSERT( vecVar != NULL );
	//vecVar->SetFloatVector(vecData);

	//// Setup sunlight color
	//vecData.x = m_sunlightColor.Red;
	//vecData.y = m_sunlightColor.Green;
	//vecData.z = m_sunlightColor.Blue;

	//// Store
	//vecVar = effect->GetVariableByName("sunColor")->AsVector();
	//_ASSERT( vecVar != NULL);
	//vecVar->SetFloatVector(vecData);

	//// *************
	//// Setup ambient color
	//// *************
	//vecData.x = m_ambientColor.Red;
	//vecData.y = m_ambientColor.Green;
	//vecData.z = m_ambientColor.Blue;

	//// Make sure it is normalized
	//D3DXVec3Normalize(&vecData, &vecData);

	//vecVar = effect->GetVariableByName("ambientColor")->AsVector();
	//_ASSERT( vecVar != NULL );
	//vecVar->SetFloatVector(vecData);
	//
	//// Set our IB/VB
	//unsigned int stride = shader->m_decl->m_vertexSize;
	//unsigned int offset = 0;
	//context->IASetVertexBuffers(0,1,&m_quadVB,&stride,&offset);
	//context->IASetIndexBuffer(m_quadIB,DXGI_FORMAT_R16_UINT,0);

	//// Set our prim type
	//context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

	//// Set our states
	//context->RSSetState(m_RState);

	//D3D11_TECHNIQUE_DESC techDesc;
	//ID3DX11EffectTechnique *technique = effect->GetTechniqueByIndex(0);
	//technique->GetDesc(&techDesc);
	//for( unsigned int passIndex = 0; passIndex < techDesc.Passes; passIndex++ )
	//{
	//	technique->GetPassByIndex( passIndex )->Apply( 0 );
	//	context->DrawIndexed( 4, 0, 0 );
	//}

	//// Clear inputs on the shader
	//hr = shaderResource->SetResource(NULL);
	//_ASSERT(SUCCEEDED(hr));

	//for( unsigned int passIndex = 0; passIndex < techDesc.Passes; passIndex++ )
	//{
	//	technique->GetPassByIndex( passIndex )->Apply( 0 );
	//}
}

// ****************************************************************************
// ****************************************************************************
void RenderAmbientLight()
{
	// m_D3DDevice->CreateSamplerState();
	// m_context->PSSetSamplers(slot, count, *);
	// m_context->PSSetShaderResources(slot, count *);

	HRESULT hr = E_FAIL;
	//ID3D11Device *device = m_D3DDevice;
	//ID3D11DeviceContext *context = m_context;

	// Set our textures as inputs
	HXShader *shader = HXGetShaderByName(m_ambientMat->m_shaderName);

	// Set albedo texture as input
	m_context->PSSetShaderResources(0, 1, &m_SRView[ALBEDO]);

	// Set up constants
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	hr = m_context->Map(m_PSFrameConstants, NULL, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	_ASSERT( SUCCEEDED( hr ) );

	PS_CONSTANT_BUFFER_FRAME *cbFrame = reinterpret_cast<PS_CONSTANT_BUFFER_FRAME*>(mappedResource.pData);

	// *************
	// Setup ambient color
	// *************
	D3DXVECTOR4 vecData;
	vecData.x = m_ambientColor.Red;
	vecData.y = m_ambientColor.Green;
	vecData.z = m_ambientColor.Blue;
	vecData.w = 0.0f;

	// Make sure it is normalized
	D3DXVec4Normalize(&vecData, &vecData);

	cbFrame->m_ambientColor = vecData;
	
	// Set the input layout 
	m_context->IASetInputLayout(shader->m_decl->m_layout);

	// Set our IB/VB
	unsigned int stride = shader->m_decl->m_vertexSize;
	unsigned int offset = 0;
	m_context->IASetVertexBuffers(0,1,&m_quadVB,&stride,&offset);
	m_context->IASetIndexBuffer(m_quadIB,DXGI_FORMAT_R16_UINT,0);

	// Set our prim type
	m_context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

	// Set our states
	m_context->RSSetState(m_RState);

	//D3D11_TECHNIQUE_DESC techDesc;
	//ID3DX11EffectTechnique *technique = effect->GetTechniqueByIndex(0);
	//technique->GetDesc(&techDesc);
	//for( unsigned int passIndex = 0; passIndex < techDesc.Passes; passIndex++ )
	//{
	//	technique->GetPassByIndex( passIndex )->Apply( 0 );
	//	context->DrawIndexed( 4, 0, 0 );
	//}

	// Clear the inputs on the shader
	//hr = albedoResource->SetResource(NULL);
	//_ASSERT(SUCCEEDED(hr));

	//for( unsigned int passIndex = 0; passIndex < techDesc.Passes; passIndex++ )
	//{
	//	technique->GetPassByIndex( passIndex )->Apply( 0 );
	//}
}
// ****************************************************************************
// ****************************************************************************
void RenderPointLight(Light &light)
{
	//ID3D11Device *device = m_D3DDevice;
	//ID3D11DeviceContext *context = m_context;

	//// Get the mesh/material/shader/effect
	//Mesh *lightSphere = MeshManager::GetInstance().GetMesh("[lightsphere]");
	//HXMaterial *lightSphereMat = HXGetMaterial(lightSphere->GetMaterialName());
	//HXShader *shader = HXGetShaderByName(lightSphereMat->m_shaderName);
	//ID3DX11Effect *effect = shader->m_pEffect;

	//// Albedo texture
	//ID3DX11EffectShaderResourceVariable *albedoResource = effect->GetVariableByName("albedoTexture")->AsShaderResource();
	//_ASSERT(albedoResource != NULL);
	//HRESULT hr = albedoResource->SetResource(m_SRView[ALBEDO]);
	//_ASSERT( SUCCEEDED(hr) );

	//// Normal texture
	//ID3DX11EffectShaderResourceVariable *normalResource = effect->GetVariableByName("normalTexture")->AsShaderResource();
	//_ASSERT(normalResource != NULL);
	//hr = normalResource->SetResource(m_SRView[NORMAL]);
	//_ASSERT( SUCCEEDED(hr) );

	//// Depth texture
	//ID3DX11EffectShaderResourceVariable *depthResource= effect->GetVariableByName("depthTexture")->AsShaderResource();
	//_ASSERT(depthResource != NULL);
	//hr = depthResource->SetResource(m_SRView[DEPTH]);
	//_ASSERT( SUCCEEDED(hr) );

	//// Set common parameters
	//ID3DX11EffectScalarVariable *scalarVar = effect->GetVariableByName("cameraNear")->AsScalar();
	//hr = scalarVar->SetFloat(m_cameraNear);

	//scalarVar = effect->GetVariableByName("cameraFar")->AsScalar();
	//hr = scalarVar->SetFloat(m_cameraFar);

	//scalarVar = effect->GetVariableByName("imageWidth")->AsScalar();
	//hr = scalarVar->SetFloat(m_imageWidth);

	//scalarVar = effect->GetVariableByName("imageHeight")->AsScalar();
	//hr = scalarVar->SetFloat(m_imageHeight);

	//scalarVar = effect->GetVariableByName("fovY")->AsScalar();
	//hr = scalarVar->SetFloat(m_fovY);

	//scalarVar = effect->GetVariableByName("viewAspect")->AsScalar();
	//hr = scalarVar->SetFloat(m_viewAspect);

	//scalarVar = effect->GetVariableByName("invTanHalfFOV")->AsScalar();
	//hr = scalarVar->SetFloat(m_invTanHalfFOV);

	//// Setup a world matrix for the light position and size
	//D3DXMATRIX scaleMat,transMat, worldMat;
	//D3DXVECTOR3 lightPos(light.point.m_position);
	//D3DXMatrixScaling(&scaleMat,5.0f, 5.0f, 5.0f);
	//D3DXMatrixTranslation(&transMat,lightPos.x, lightPos.y, lightPos.z);
	//D3DXMatrixMultiply(&worldMat,&scaleMat,&transMat);

	//// Set the world*view matrix for the point light
	//D3DXMATRIX viewMat = m_viewMatrix[m_renderIndex];
	//D3DXMATRIX worldView;
	//D3DXMatrixMultiply(&worldView,&worldMat,&viewMat);
	//HXSetSharedParameter("WorldView",worldView);

	//// Position
	//ID3DX11EffectVectorVariable *vecVar = effect->GetVariableByName("pointLoc")->AsVector();
	//_ASSERT( vecVar != NULL);
	//vecVar->SetFloatVector(lightPos);

	//// Color
	//D3DXVECTOR3 vecData(light.m_color.a,light.m_color.g,light.m_color.b);
	//vecVar = effect->GetVariableByName("pointColor")->AsVector();
	//_ASSERT( vecVar != NULL);
	//vecVar->SetFloatVector(vecData);

	//// Radius
	//scalarVar = effect->GetVariableByName("pointRadius")->AsScalar();
	//hr = scalarVar->SetFloat(5.0f);

	//// Set the input layout 
	//context->IASetInputLayout(shader->m_decl->m_layout);

	//// Set our IB/VB
	//unsigned int stride = shader->m_decl->m_vertexSize;
	//unsigned int offset = 0;
	//ID3D11Buffer *vb = lightSphere->GetVertexBuffer();
	//ID3D11Buffer *ib = lightSphere->GetIndexBuffer();
	//context->IASetVertexBuffers(0,1,&vb,&stride,&offset);
	//context->IASetIndexBuffer(ib,DXGI_FORMAT_R16_UINT,0);

	//// Set our prim type
	//context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	//// Set our states
	//context->RSSetState(m_RState);

	//D3D11_TECHNIQUE_DESC techDesc;
	//ID3DX11EffectTechnique *technique = effect->GetTechniqueByIndex(0);
	//technique->GetDesc(&techDesc);
	//for( unsigned int passIndex = 0; passIndex < techDesc.Passes; passIndex++ )
	//{
	//	technique->GetPassByIndex( passIndex )->Apply( 0 );
	//	context->DrawIndexed( lightSphere->NumIndices(), 0, 0 );
	//}

	//// Clear the inputs on the shader
	//hr = albedoResource->SetResource(NULL);
	//_ASSERT(SUCCEEDED(hr));

	//hr = normalResource->SetResource(NULL);
	//_ASSERT(SUCCEEDED(hr));

	//hr = depthResource->SetResource(NULL);
	//_ASSERT(SUCCEEDED(hr));

	//for( unsigned int passIndex = 0; passIndex < techDesc.Passes; passIndex++ )
	//{
	//	technique->GetPassByIndex( passIndex )->Apply( 0 );
	//}
}

// ****************************************************************************
// ****************************************************************************
void RenderLights()
{
	//ID3D11Device *device = m_D3DDevice;
	//ID3D11DeviceContext *context = m_context;

	//// Set our textures as inputs
	//// Go render all lights
	//for(int iLightIndex=0;iLightIndex < m_numRenderLights; iLightIndex++)
	//{
	//	Light &light = m_renderLights[iLightIndex];

	//	switch(light.m_type)
	//	{
	//		case Light::POINT: 
	//			RenderPointLight(light);
	//			break;
	//	}

	//}

}

// ****************************************************************************
// ****************************************************************************
void SetMaterialParameters(HXMaterial *mat)
{
	HXShader *shader = HXGetShaderByName(mat->m_shaderName);
	_ASSERT(shader != NULL);

	HXTexture *tex = HXGetTextureByName(mat->m_textureName);

	if( tex != NULL)
	{
		// Mesh that only uses render targets as input textures 
		// may not have a texture 
		ID3D11ShaderResourceView *textureRV = tex->m_shaderView;
		m_context->PSSetShaderResources(0, 1, &textureRV);
	}
}
// ****************************************************************************
// ****************************************************************************
void FillGBuffer()
{
	// Reset our view
	ID3D11ShaderResourceView*const pSRV[3] = { NULL,NULL,NULL };
	m_context->PSSetShaderResources( 0, 2, pSRV );

	//
	// Clear the render targets
	//
	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red, green, blue, alpha
	float ClearNormal[3] = { 0.0f, 0.0f, 0.0f };
	float ClearDepth[1] = { 0.0f };
	m_context->ClearRenderTargetView( m_RTView[ALBEDO], ClearColor );
	m_context->ClearRenderTargetView( m_RTView[NORMAL], ClearNormal );
	m_context->ClearRenderTargetView( m_RTView[DEPTH], ClearDepth) ;
	m_context->ClearDepthStencilView( m_depthStencilDSView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	//device->ClearRenderTargetView( m_backBufferView, ClearColor );

	m_context->OMSetDepthStencilState(m_GBufferDSState,0);
	FLOAT blendFactor[4] = {0,0,0,0};
	m_context->OMSetBlendState(m_GBufferBlendState,blendFactor,0xffffffff);

	// Set our render targets
	m_context->OMSetRenderTargets(3, m_RTView, m_depthStencilDSView);
	//device->OMSetRenderTargets(1,&m_backBufferView,NULL);

	// Set our per frame VS constants
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = m_context->Map(m_VSFrameConstants, NULL, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	_ASSERT( SUCCEEDED( hr ) );

	VS_CONSTANT_BUFFER_FRAME *vsFrameConstants = reinterpret_cast<VS_CONSTANT_BUFFER_FRAME*>(mappedResource.pData);

	// Setup camera parameters
	D3DXMATRIX projMat = m_projMatrix[m_renderIndex];
	vsFrameConstants->m_projMatrix = projMat;

	// Get the view matrix
	D3DXMATRIX viewMat = m_viewMatrix[m_renderIndex];
	vsFrameConstants->m_viewMatrix = viewMat;

	// View inverse
	D3DXMATRIX invView;
	D3DXMATRIX *invRet = D3DXMatrixInverse(&invView,NULL,&viewMat);
	_ASSERT(invRet = &invView);
	vsFrameConstants->m_invViewMatrix = invView;

	// Inverse view/proj
	D3DXMATRIX viewProj;
	D3DXMatrixMultiply(&viewProj,&viewMat,&projMat);
	D3DXMATRIX invViewProj;
	invRet = D3DXMatrixInverse(&invViewProj,NULL,&viewProj);
	_ASSERT(invRet == &invViewProj);
	vsFrameConstants->m_invViewProj = invViewProj;

	// Inverse projection
	D3DXMATRIX invProj;
	D3DXMatrixInverse(&invProj,NULL,&projMat);
	_ASSERT(invRet = &invProj);
	vsFrameConstants->m_invProj = invProj;

	// The upper 3x3 of the view matrx
	// Used to transform directional lights
	D3DXMATRIX view3x3 = viewMat;
	view3x3._14 = view3x3._24 = view3x3._34 = view3x3._41 = view3x3._42 = view3x3._43 = 0;
	view3x3._44 = 1;
	vsFrameConstants->m_view3x3 = view3x3;

	// Done with per-frame VS constants
	m_context->Unmap(m_VSFrameConstants, NULL);

	// Set the per frame VS constants in slot 0
	m_context->VSSetConstantBuffers(0, 1, &m_VSFrameConstants);

	// Go through all of our render objects
	RenderData *obj = m_submissionBuffers[m_renderIndex];

	// Per object VS constants start at 1
	int objectConstantSlot = 1;
	while(obj)
	{
		// TODO: We only have a max of D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT (14) constant buffer
		// slots.  This uses one constant buffer per object, however each constant buffer can hold up to 4096 
		// constants.  Ideally, you'd pack a number of object constants enough to fill one constant buffer.
		_ASSERT(objectConstantSlot < D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT);
		hr = m_context->Map(m_VSObjectConstants, NULL, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		_ASSERT( SUCCEEDED( hr ) );
		VS_CONSTANT_BUFFER_OBJECT *vsObjectConstants = reinterpret_cast<VS_CONSTANT_BUFFER_OBJECT*>(mappedResource.pData);

		// TODO: Get the world matrix from the object.  
		// Use I for now
		D3DXMATRIX worldMat;
		D3DXMatrixIdentity(&worldMat);

		// Calculate the WorldView matrix
		D3DXMATRIX worldView;
		D3DXMatrixMultiply(&worldView,&worldMat,&viewMat);
		vsObjectConstants->m_worldViewMatrix = worldView;

		D3DXMATRIX worldViewProj;
		D3DXMatrixMultiply(&worldViewProj,&worldView,&projMat);

		D3DXMATRIX invWorldViewProj;
		invRet = D3DXMatrixInverse(&invWorldViewProj,NULL,&worldViewProj);
		_ASSERT(invRet == &invWorldViewProj);

		vsObjectConstants->m_invWorldViewProj = invWorldViewProj;

		// Generate the inverse transpose of the WorldView matrix
		// We don't use any non uniform scaling, so we can just send down the 
		// upper 3x3 of the world view matrix
		D3DXMATRIX worldViewIT;
		worldViewIT = worldView;
		worldViewIT._14 = worldViewIT._24 = worldViewIT._34 = worldViewIT._41 = worldViewIT._42 = worldViewIT._43 = 0;
		worldViewIT._44 = 1;
		D3DXMatrixTranspose(&worldViewIT,&worldViewIT);
		invRet = D3DXMatrixInverse(&worldViewIT,NULL,&worldViewIT);
		_ASSERT(invRet == &worldViewIT);

		vsObjectConstants->m_worldViewIT = worldViewIT;

		// Done with per-object VS constants
		m_context->Unmap(m_VSObjectConstants, NULL);

		// Set the per frame VS constants in slot 0
		m_context->VSSetConstantBuffers(objectConstantSlot++, 1, &m_VSObjectConstants);

		// Set the parameters
		HXMaterial *mat = HXGetMaterial(obj->materialName);
		SetMaterialParameters(mat);

		// Set our input assembly buffers
		Mesh *mesh = MeshManager::GetInstance().GetMesh(obj->meshName);
		HXShader *shader = HXGetShaderByName(mat->m_shaderName);

		// Set the input layout 
		m_context->IASetInputLayout(shader->m_decl->m_layout);

		// Set our vertex/index buffers
		unsigned int stride = shader->m_decl->m_vertexSize;
		unsigned int offset = 0;
		ID3D11Buffer *vb = mesh->GetVertexBuffer();
		m_context->IASetVertexBuffers(0,1,&vb,&stride,&offset);
		m_context->IASetIndexBuffer(mesh->GetIndexBuffer(),DXGI_FORMAT_R16_UINT,0);

		// Set our prim type
		m_context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

		// Set our shader
		m_context->VSSetShader(shader->m_vshader, NULL, 0);
		m_context->PSSetShader(shader->m_pshader, NULL, 0);
		m_context->GSSetShader(NULL, NULL, 0);

		// Draw
		m_context->DrawIndexed( mesh->NumIndices(), 0, 0 );

		// Start rendering
		//ID3DX11Effect *effect = shader->m_pEffect;
		//D3D11_TECHNIQUE_DESC techDesc;
		//ID3DX11EffectTechnique *technique = effect->GetTechniqueByIndex(0);
		//technique->GetDesc(&techDesc);
		//for( unsigned int passIndex = 0; passIndex < techDesc.Passes; passIndex++ )
		//{
		//	technique->GetPassByIndex( passIndex )->Apply( 0 );
		//	m_context->DrawIndexed( mesh->NumIndices(), 0, 0 );
		//}

		// Next
		obj=obj->next;
	}
}

// ****************************************************************************
// ****************************************************************************
void DoLighting()
{
	ID3D11Device *device = m_D3DDevice;
	ID3D11DeviceContext *context = m_context;

	// Clear the backbuffer/depth/stencil
	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; // red, green, blue, alpha
	context->ClearRenderTargetView( m_backBufferView, ClearColor );
	context->ClearDepthStencilView( m_backDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	// Copy the albedo texture to the backbuffer

	// Switch to final backbuffer/depth/stencil
	context->OMSetRenderTargets(1,&m_backBufferView, m_depthStencilDSView/*m_backDepthStencilView*/);

	context->OMSetDepthStencilState(m_lightingDSState,0);
	FLOAT blendFactor[4] = {0,0,0,0};
	context->OMSetBlendState(m_lightingBlendState,blendFactor,0xffffffff);

	// Render the scene with ambient into the backbuffer
	RenderAmbientLight();

	// Render with a directional sunlight vector
	//RenderSunlight();
	//device->ClearDepthStencilView( m_backDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	// Render our lights
	RenderLights();
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
