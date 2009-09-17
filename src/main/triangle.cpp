// ****************************************************************************
// ****************************************************************************
#include "stdafx.h"
#include "Triangle.h"
#include "TheGame.h"
#include "Camera.h"
#include "LuaPlus.h"

// ****************************************************************************
// ****************************************************************************
Triangle::Triangle(void)
{
	m_instance = new Helix::Instance;
	m_instance->SetMeshName("Cube");

	Helix::Mesh *mesh = Helix::MeshManager::GetInstance().Load("Cube");
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
	Helix::SubmitInstance(*m_instance);
	//m_pEffect->End();
}