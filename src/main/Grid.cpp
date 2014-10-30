#include "Grid.h"
#include "TheGame.h"
#include "Camera.h"

#define	NUM_GRID_POINTS	50

// ****************************************************************************
// ****************************************************************************
Grid::Grid(void)
{
//	TheGame *game = TheGame::Instance();
//	IDirect3DDevice9 *pDevice = game->GetDevice();
//	HRESULT hr;
//
//	DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE;
//	hr = D3DXCreateEffectFromFile(pDevice,"Effects/diffuse.fxo",NULL,NULL,dwShaderFlags,NULL,&m_pLineEffect, NULL);
//	_ASSERT(hr == D3D_OK);
//
//	// Retrieve technique desc
//	D3DXEFFECT_DESC desc;
//	m_pLineEffect->GetDesc( &desc );
//	m_iNumberOfTechniques = desc.Techniques;
//	m_iCurrentTechnique = 0;
//
//	D3DXTECHNIQUE_DESC techdesc;
//	m_pLineEffect->GetTechniqueDesc( m_pLineEffect->GetTechnique(m_iCurrentTechnique ), &techdesc );
//	swprintf( m_strTechnique, 512, L"%S", techdesc.Name );
//
//	// Define the vertex elements and
//	// Create a vertex declaration from the element descriptions.
//	D3DVERTEXELEMENT9 VertexElements[3] =
//	{
//		{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
//		{ 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
//		D3DDECL_END()
//	};
//
//	pDevice->CreateVertexDeclaration( VertexElements, &m_pVertexDecl );
//
//	// Create the vertex buffer. Here we are allocating enough memory
//	// (from the default pool) to hold all our 3 custom vertices. 
//	hr = pDevice->CreateVertexBuffer( NUM_GRID_POINTS*4*sizeof(LineVertex),D3DUSAGE_WRITEONLY, 0,D3DPOOL_DEFAULT, &m_pLineVerts, NULL );
////	_ASSERT(SUCCEEDED(hr));
//
//	// Create our vertex buffer
//	LineVertex vertices[NUM_GRID_POINTS*4];
//	ZeroMemory(vertices,sizeof(vertices));
//	int vertIndex = 0;
//	float currX = (float)(-1*NUM_GRID_POINTS/2);
//	float currZ = (float)(-1*NUM_GRID_POINTS/2);
//
//	for(int i=0;i<NUM_GRID_POINTS;i++,vertIndex++)
//	{
//		vertices[vertIndex].x = (float)(-1*NUM_GRID_POINTS/2);
//		vertices[vertIndex].y = 0.0f;
//		vertices[vertIndex].z = currZ;
//		vertices[vertIndex].Color = 0x7fa0a0a0;
//
//		vertIndex++;
//		vertices[vertIndex].x = (float)(NUM_GRID_POINTS/2);
//		vertices[vertIndex].y = 0.0f;
//		vertices[vertIndex].z = currZ;
//		vertices[vertIndex].Color = 0x7fa0a0a0;
//		currZ+=1.0f;
//	}
//
//	currX = (float)(-1*NUM_GRID_POINTS/2);
//	currZ = (float)(-1*NUM_GRID_POINTS/2);
//	for(int i=0;i<NUM_GRID_POINTS;i++,vertIndex++)
//	{
//		vertices[vertIndex].x = currX;
//		vertices[vertIndex].y = 0.0;
//		vertices[vertIndex].z = (float)(-1*NUM_GRID_POINTS/2);
//		vertices[vertIndex].Color = 0x7fa0a0a0;
//
//		vertIndex++;
//		vertices[vertIndex].x = currX;
//		vertices[vertIndex].y = 0.0f;
//		vertices[vertIndex].z = (float)(NUM_GRID_POINTS/2);
//		vertices[vertIndex].Color = 0x7fa0a0a0;
//		currX+=1.0f;
//	}
//	
//
//
//	LineVertex* pVertices;
//	hr = m_pLineVerts->Lock( 0, 0, (void**)&pVertices, 0 );
////	_ASSERT(SUCCEEDED(hr));
//
//	memcpy( pVertices, vertices, NUM_GRID_POINTS*4*sizeof(LineVertex) );
//	m_pLineVerts->Unlock();
//
//	// Create our index buffer
//	hr = pDevice->CreateIndexBuffer(NUM_GRID_POINTS*4*2,0,D3DFMT_INDEX16,D3DPOOL_DEFAULT,&m_pLineIndices,NULL);
////	_ASSERT(SUCCEEDED(hr));
//
//	// Fill it in
//	unsigned short indexArray[NUM_GRID_POINTS*4];
//	ZeroMemory(indexArray,sizeof(indexArray));
//
//	int index=0;
//	for(int i=0;i<NUM_GRID_POINTS*2;i++)
//	{
//		indexArray[index] = index;
//		indexArray[index+1] = index+1;
//		index+=2;
//	}
//
//	unsigned short *indices;
//	m_pLineIndices->Lock(0,NUM_GRID_POINTS*4*2,(VOID **)&indices,0);
//	memcpy(indices,indexArray,NUM_GRID_POINTS*4*2);
//	m_pLineIndices->Unlock();
}

// ****************************************************************************
// ****************************************************************************
Grid::~Grid(void)
{
	//m_pLineVerts->Release();
	//m_pLineIndices->Release();
	//m_pLineEffect->Release();
}
// ****************************************************************************
// ****************************************************************************
void Grid::Update(float frameTime)
{
}

// ****************************************************************************
// ****************************************************************************
void Grid::Render(void)
{
	//HRESULT hr;
	//TheGame *game = TheGame::Instance();
	//IDirect3DDevice9 *pDevice = game->GetDevice();

	//D3DXMATRIX viewMatrix = game->CurrentCamera()->GetViewMatrix();
	//D3DXMATRIX projectionMatrix = game->CurrentCamera()->GetProjectionMatrix();
	//D3DXHANDLE hTechnique = m_pLineEffect->GetTechnique( m_iCurrentTechnique );
	//D3DXHANDLE hPass      = m_pLineEffect->GetPass( hTechnique, 0 );
	//D3DXHANDLE hWorldView	= m_pLineEffect->GetParameterByName(NULL, "WorldView");
	//D3DXHANDLE hProjection = m_pLineEffect->GetParameterByName(NULL, "Projection");

	//// Set effect parameters
	//hr = m_pLineEffect->SetMatrix( hWorldView, &viewMatrix );

	//hr = m_pLineEffect->SetMatrix( hProjection, &projectionMatrix);

	//UINT numPasses;
	//hr = m_pLineEffect->Begin( &numPasses, D3DXFX_DONOTSAVESTATE );

	//hr = m_pLineEffect->BeginPass( 0 );

	//hr = m_pLineEffect->CommitChanges();

	//// Render triangle here
	//pDevice->SetStreamSource(0,m_pLineVerts,0,sizeof(LineVertex));
	//pDevice->SetIndices(m_pLineIndices);
	//pDevice->SetVertexDeclaration(m_pVertexDecl);
	//pDevice->DrawIndexedPrimitive(D3DPT_LINELIST,0,0,NUM_GRID_POINTS*4,0,2*NUM_GRID_POINTS);

	//m_pLineEffect->EndPass();
	//m_pLineEffect->End();
}
