#include "stdafx.h"
#include "Instance.h"
#include "InstanceManager.h"
#include "RenderMgr.h"

// ****************************************************************************
// ****************************************************************************
Instance::Instance()
{
}

// ****************************************************************************
// ****************************************************************************
Instance::~Instance()
{
	m_pVB->Release();
	m_pIB->Release();
	m_pEffect->Release();
	m_texture->Release();
}

// ****************************************************************************
// ****************************************************************************
bool Instance::LoadEffectFromFile(const wchar_t *filePath)
{
	IDirect3DDevice9 *pDevice = RenderMgr::GetInstance().GetDevice();
	HRESULT hr;
	hr = D3DXCreateEffectFromFile(pDevice,filePath,NULL,NULL,0,NULL,&m_pEffect, NULL);
	_ASSERT(hr == D3D_OK);

	return SUCCEEDED(hr);
}

// ****************************************************************************
// ****************************************************************************
bool Instance::LoadTextureFromFile(const wchar_t *filePath)
{
	IDirect3DDevice9 *pDevice = RenderMgr::GetInstance().GetDevice();

	HRESULT hr;
	hr = D3DXCreateTextureFromFile(pDevice,filePath,&m_texture);
	_ASSERT(hr == D3D_OK);

	return SUCCEEDED(hr);
}

// ****************************************************************************
// ****************************************************************************
bool Instance::CreateVertexDecl(const D3DVERTEXELEMENT9 *vertexDecl)
{
	IDirect3DDevice9 *pDevice = RenderMgr::GetInstance().GetDevice();

	HRESULT hr;
	hr = pDevice->CreateVertexDeclaration( vertexDecl, &m_pVertexDecl );
	_ASSERT(hr == D3D_OK);

	return SUCCEEDED(hr);
}

// ****************************************************************************
// ****************************************************************************
bool Instance::CreateVertexBuffer(const int numVertices, const int vertexSize)
{
	IDirect3DDevice9 *pDevice = RenderMgr::GetInstance().GetDevice();

	// Create the vertex buffer. Here we are allocating enough memory
	// (from the default pool) to hold all our 3 custom vertices. 
	HRESULT hr;
	hr = pDevice->CreateVertexBuffer( numVertices*vertexSize,D3DUSAGE_WRITEONLY, NULL,D3DPOOL_MANAGED, &m_pVB, NULL );
	_ASSERT(SUCCEEDED(hr));

	return SUCCEEDED(hr);
}
// ****************************************************************************
// ****************************************************************************
bool Instance::CreateIndexBuffer(const int numIndices)
{
	IDirect3DDevice9 *pDevice = RenderMgr::GetInstance().GetDevice();

	// Create our index buffer
	HRESULT hr;
	hr = pDevice->CreateIndexBuffer(numIndices*2,0,D3DFMT_INDEX16,D3DPOOL_DEFAULT,&m_pIB,NULL);
	_ASSERT(SUCCEEDED(hr));

	return SUCCEEDED(hr);
}
