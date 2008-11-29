// ****************************************************************************
// ****************************************************************************
#include "stdafx.h"
#include "Triangle.h"
#include "TheGame.h"
#include "Camera.h"

// ****************************************************************************
// ****************************************************************************
Triangle::Triangle(void)
{
	TheGame *game = TheGame::Instance();
	IDirect3DDevice9 *pDevice = game->GetDevice();
	HRESULT hr;

	hr = D3DXCreateEffectFromFile(pDevice,"Effects\\texture.fxo",NULL,NULL,0,NULL,&m_pEffect, NULL);
	_ASSERT(hr == D3D_OK);

	hr = D3DXCreateTextureFromFile(pDevice,"Effects\\Carsen.tga",&m_texture);
	_ASSERT(hr == D3D_OK);

	// Retrieve technique desc
	D3DXEFFECT_DESC desc;
		m_pEffect->GetDesc( &desc );
	m_iNumberOfTechniques = desc.Techniques;
	m_iCurrentTechnique = 0;

	D3DXTECHNIQUE_DESC techdesc;
	m_pEffect->GetTechniqueDesc( m_pEffect->GetTechnique(m_iCurrentTechnique ), &techdesc );
	swprintf( m_strTechnique, 512, L"%S", techdesc.Name );

	// Define the vertex elements and
	// Create a vertex declaration from the element descriptions.
	D3DVERTEXELEMENT9 VertexElements[3] =
	{
		{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};

	pDevice->CreateVertexDeclaration( VertexElements, &m_pVertexDecl );

	// Create the vertex buffer. Here we are allocating enough memory
	// (from the default pool) to hold all our 3 custom vertices. 
	hr = pDevice->CreateVertexBuffer( 3*sizeof(TriangleVertex),D3DUSAGE_WRITEONLY, NULL,D3DPOOL_MANAGED, &m_pVB, NULL );
	_ASSERT(SUCCEEDED(hr));

	// Now we fill the vertex buffer. To do this, we need to Lock() the VB to
	// gain access to the vertices. This mechanism is required because the
	// vertex buffer may still be in use by the GPU. This can happen if the
	// CPU gets ahead of the GPU. The GPU could still be rendering the previous
	// frame.
	TriangleVertex m_Vertices[] =
	{
		{  0.0f, -1.1547f, 0.0f, 0.5f, 1.0f}, // x, y, z, uv
		{ -1.0f,  0.5777f, 0.0f, 0.0f, 0.0f},
		{  1.0f,  0.5777f, 0.0f, 1.0f, 0.0f},
	};

	TriangleVertex* pVertices;
	hr = m_pVB->Lock( 0, 0, (void**)&pVertices, 0 );
	_ASSERT(SUCCEEDED(hr));

	memcpy( pVertices, m_Vertices, 3*sizeof(TriangleVertex) );
	m_pVB->Unlock();

	// Create our index buffer
	hr = pDevice->CreateIndexBuffer(3*2,0,D3DFMT_INDEX16,D3DPOOL_DEFAULT,&m_pIB,NULL);
	_ASSERT(SUCCEEDED(hr));

	// Fill it in
	unsigned short indexArray[] = { 0, 1, 2};
	unsigned short *indices;
	m_pIB->Lock(0,3*2,(VOID **)&indices,0);
	memcpy(indices,indexArray,3*2);
	m_pIB->Unlock();
}

Triangle::~Triangle(void)
{
	m_pVB->Release();
	m_pIB->Release();
	m_pEffect->Release();
}

// ****************************************************************************
// ****************************************************************************
void Triangle::OrthoNormalization(D3DXMATRIX &mat)
{
	D3DXVECTOR3 xAxis(mat._11, mat._12, mat._13);
	D3DXVECTOR3 xAxisNorm;
	D3DXVec3Normalize(&xAxisNorm,&xAxis);

	D3DXVECTOR3 yAxis(mat._21, mat._22, mat._23);
	D3DXVECTOR3 yAxisNorm;
	D3DXVec3Normalize(&yAxisNorm,&yAxis);

	D3DXVECTOR3 zAxis;
	D3DXVec3Cross(&zAxis,&xAxis,&yAxis);
	D3DXVECTOR3 zAxisNorm;
	D3DXVec3Normalize(&zAxisNorm,&zAxis);

	mat._11 = xAxisNorm.x;
	mat._12 = xAxisNorm.y;
	mat._13 = xAxisNorm.z;
	mat._21 = yAxisNorm.x;
	mat._22 = yAxisNorm.y;
	mat._23 = yAxisNorm.z;
	mat._31 = zAxisNorm.x;
	mat._32 = zAxisNorm.y;
	mat._33 = zAxisNorm.z;
}

// ****************************************************************************
// ****************************************************************************
void Triangle::Update(float frameTime)
{
	//D3DXMATRIX	root;
	//float rot = -3.1415926f/2.0f;
	//D3DXMatrixRotationX(&root,rot);

	//D3DXMATRIX child1;
	//D3DXMatrixIdentity(&child1);
	//D3DXMatrixTranslation(&child1,0.0f,0.0f,10.0f);

	//D3DXMATRIX child2;
	//D3DXMatrixIdentity(&child2);
	//D3DXMatrixTranslation(&child2,0.0f,0.0f,2.0f);

	//D3DXMATRIX child1World;
	//D3DXMatrixMultiply(&child1World,&child1,&root);

	//D3DXMATRIX child2World;
	//D3DXMatrixMultiply(&child2World,&child2,&child1World);

	//float det = 0.0f;

	//D3DXMATRIX rootInverse;
	//D3DXMatrixInverse(&rootInverse,&det,&root);

	//D3DXMATRIX rootToChild1;
	//D3DXMatrixMultiply(&rootToChild1,&child1World,&rootInverse);

	//D3DXMATRIX child1Inverse;
	//D3DXMatrixInverse(&child1Inverse,&det,&child1World);

	//D3DXMATRIX child1ToChild2;
	//D3DXMatrixMultiply(&child1ToChild2, &child2World, &child1Inverse);

	//D3DXMATRIX child2Inverse;
	//D3DXMatrixInverse(&child2Inverse,&det,&child2World);

	//D3DXMATRIX newChild1;
	//D3DXMatrixMultiply(&newChild1,&rootToChild1,&root);

	//D3DXMATRIX newChild2;
	//D3DXMatrixMultiply(&newChild2,&child1ToChild2,&newChild1);

	//char buffer[1024];

	//sprintf(buffer, "Initial child1World\n");
	//OutputDebugString(buffer);
	//sprintf(buffer, "  %0.8f %0.8f %0.8f\n",child1World._11, child1World._12, child1World._13);
	//OutputDebugString(buffer);
	//sprintf(buffer, "  %0.8f %0.8f %0.8f\n",child1World._21, child1World._22, child1World._23);
	//OutputDebugString(buffer);
	//sprintf(buffer, "  %0.8f %0.8f %0.8f\n",child1World._31, child1World._32, child1World._33);
	//OutputDebugString(buffer);
	//sprintf(buffer, "  %0.8f %0.8f %0.8f\n",child1World._41, child1World._42, child1World._43);
	//OutputDebugString(buffer);

	//sprintf(buffer, "\n");
	//OutputDebugString(buffer);

	//sprintf(buffer, "Initial newChild1\n");
	//OutputDebugString(buffer);
	//sprintf(buffer, "  %0.8f %0.8f %0.8f\n",newChild1._11, newChild1._12, newChild1._13);
	//OutputDebugString(buffer);
	//sprintf(buffer, "  %0.8f %0.8f %0.8f\n",newChild1._21, newChild1._22, newChild1._23);
	//OutputDebugString(buffer);
	//sprintf(buffer, "  %0.8f %0.8f %0.8f\n",newChild1._31, newChild1._32, newChild1._33);
	//OutputDebugString(buffer);
	//sprintf(buffer, "  %0.8f %0.8f %0.8f\n",newChild1._41, newChild1._42, newChild1._43);
	//OutputDebugString(buffer);

	//sprintf(buffer, "\n");
	//OutputDebugString(buffer);

	//sprintf(buffer, "Initial child2World\n");
	//OutputDebugString(buffer);
	//sprintf(buffer, "  %0.8f %0.8f %0.8f\n",child2World._11, child2World._12, child2World._13);
	//OutputDebugString(buffer);
	//sprintf(buffer, "  %0.8f %0.8f %0.8f\n",child2World._21, child2World._22, child2World._23);
	//OutputDebugString(buffer);
	//sprintf(buffer, "  %0.8f %0.8f %0.8f\n",child2World._31, child2World._32, child2World._33);
	//OutputDebugString(buffer);
	//sprintf(buffer, "  %0.8f %0.8f %0.8f\n",child2World._41, child2World._42, child2World._43);
	//OutputDebugString(buffer);

	//sprintf(buffer, "\n");
	//OutputDebugString(buffer);

	//sprintf(buffer, "Initial newChild2\n");
	//OutputDebugString(buffer);
	//sprintf(buffer, "  %0.8f %0.8f %0.8f\n",newChild2._11, newChild2._12, newChild2._13);
	//OutputDebugString(buffer);
	//sprintf(buffer, "  %0.8f %0.8f %0.8f\n",newChild2._21, newChild2._22, newChild2._23);
	//OutputDebugString(buffer);
	//sprintf(buffer, "  %0.8f %0.8f %0.8f\n",newChild2._31, newChild2._32, newChild2._33);
	//OutputDebugString(buffer);
	//sprintf(buffer, "  %0.8f %0.8f %0.8f\n",newChild2._41, newChild2._42, newChild2._43);
	//OutputDebugString(buffer);

	//sprintf(buffer, "\n");
	//OutputDebugString(buffer);
}

// ****************************************************************************
// ****************************************************************************
void Triangle::Render(void)
{
	TheGame *game = TheGame::Instance();

	IDirect3DDevice9 *pDevice = game->GetDevice();
	D3DXMATRIX viewMatrix = game->CurrentCamera()->GetViewMatrix();
	D3DXMATRIX projMatrix = game->CurrentCamera()->GetProjectionMatrix();

	D3DXHANDLE hTechnique = m_pEffect->GetTechnique( m_iCurrentTechnique );
	D3DXHANDLE hPass      = m_pEffect->GetPass( hTechnique, 0 );
	D3DXHANDLE hWorldView	= m_pEffect->GetParameterByName(NULL, "WorldView");
	D3DXHANDLE hProjection = m_pEffect->GetParameterByName(NULL, "Projection");
	D3DXHANDLE hTexture = m_pEffect->GetParameterByName(NULL,"textureImage");

	// Set effect parameters
	m_pEffect->SetMatrix( hWorldView, &viewMatrix );
	m_pEffect->SetMatrix( hProjection, &projMatrix);
	m_pEffect->SetTexture( hTexture, m_texture);

	UINT numPasses;
	m_pEffect->Begin( &numPasses, D3DXFX_DONOTSAVESTATE );
	m_pEffect->BeginPass( 0 );

	m_pEffect->CommitChanges();

	// Render triangle here
	pDevice->SetStreamSource(0,m_pVB,0,sizeof(TriangleVertex));
	pDevice->SetIndices(m_pIB);
	pDevice->SetVertexDeclaration(m_pVertexDecl);
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,3,0,1);

	m_pEffect->EndPass();
	m_pEffect->End();
}