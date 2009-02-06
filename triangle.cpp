// ****************************************************************************
// ****************************************************************************
#include "stdafx.h"
#include "Triangle.h"
#include "TheGame.h"
#include "Camera.h"
#include "LuaPlus.h"
#include "RenderCore/VertexDecl.h"

#include "RenderCore/InstanceManager.h"

// ****************************************************************************
// ****************************************************************************
Triangle::Triangle(void)
{
	Instance &instance = InstanceManager::GetInstance().NewInstance();

	VertexDecl	testDecl;
	std::string path("pos3_tex1.lua");
	testDecl.Load(path);

	instance.LoadEffectFromFile(L"Effects\\texture.fxo");
	instance.LoadTextureFromFile(L"Effects\\Carsen.tga");

	// Define the vertex elements and
	// Create a vertex declaration from the element descriptions.
	D3DVERTEXELEMENT9 vertexDeclaration[3] =
	{
		{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};

	instance.CreateVertexDecl(vertexDeclaration);

	// Retrieve technique desc
	ID3DXEffect *effect = instance.GetEffect();
	D3DXEFFECT_DESC desc;
	effect->GetDesc( &desc );
	m_iNumberOfTechniques = desc.Techniques;
	m_iCurrentTechnique = 0;

	D3DXTECHNIQUE_DESC techdesc;
	effect->GetTechniqueDesc( effect->GetTechnique(m_iCurrentTechnique ), &techdesc );
	swprintf( m_strTechnique, 512, L"%S", techdesc.Name );

	instance.CreateVertexBuffer(3,sizeof(TriangleVertex));
	instance.CreateIndexBuffer(3*2);

	IDirect3DVertexBuffer9 *vertexBuffer = instance.GetVertexBuffer();

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
	HRESULT hr = vertexBuffer->Lock( 0, 0, (void**)&pVertices, 0 );
	_ASSERT(SUCCEEDED(hr));
	memcpy( pVertices, m_Vertices, 3*sizeof(TriangleVertex) );
	vertexBuffer->Unlock();

	// Fill it in
	IDirect3DIndexBuffer9 *indexBuffer = instance.GetIndexBuffer();
	unsigned short indexArray[] = { 0, 1, 2};
	unsigned short *indices;
	indexBuffer->Lock(0,3*2,(VOID **)&indices,0);
	memcpy(indices,indexArray,3*2);
	indexBuffer->Unlock();

	m_instance = &instance;
}

Triangle::~Triangle(void)
{
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
}

// ****************************************************************************
// ****************************************************************************
void Triangle::Render(void)
{
	//TheGame *game = TheGame::Instance();

	//IDirect3DDevice9 *pDevice = game->GetDevice();
	//D3DXMATRIX viewMatrix = game->CurrentCamera()->GetViewMatrix();
	//D3DXMATRIX projMatrix = game->CurrentCamera()->GetProjectionMatrix();

	//D3DXHANDLE hTechnique = m_pEffect->GetTechnique( m_iCurrentTechnique );
	//D3DXHANDLE hPass      = m_pEffect->GetPass( hTechnique, 0 );
	//D3DXHANDLE hWorldView	= m_pEffect->GetParameterByName(NULL, "WorldView");
	//D3DXHANDLE hProjection = m_pEffect->GetParameterByName(NULL, "Projection");
	//D3DXHANDLE hTexture = m_pEffect->GetParameterByName(NULL,"textureImage");

	//// Set effect parameters
	//m_pEffect->SetMatrix( hWorldView, &viewMatrix );
	//m_pEffect->SetMatrix( hProjection, &projMatrix);
	//m_pEffect->SetTexture( hTexture, m_texture);

	//UINT numPasses;
	//m_pEffect->Begin( &numPasses, D3DXFX_DONOTSAVESTATE );
	//m_pEffect->BeginPass( 0 );

	//m_pEffect->CommitChanges();

	//// Render triangle here
	//pDevice->SetStreamSource(0,m_pVB,0,sizeof(TriangleVertex));
	//pDevice->SetIndices(m_pIB);
	//pDevice->SetVertexDeclaration(m_pVertexDecl);
	//pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,3,0,1);

	//m_pEffect->EndPass();
	//m_pEffect->End();
}