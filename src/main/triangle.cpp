// ****************************************************************************
// ****************************************************************************
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
void Triangle::OrthoNormalization(Helix::Matrix4x4 &mat)
{
	Helix::Vector3 xAxis(mat.r[0][0], mat.r[0][1], mat.r[0][2]);
	Helix::Vector3 xAxisNorm = xAxis;
	xAxisNorm.Normalize();

	Helix::Vector3 yAxis(mat.r[1][2], mat.r[1][1], mat.r[1][2]);
	Helix::Vector3 yAxisNorm = yAxis;
	yAxisNorm.Normalize();

	Helix::Vector3 zAxis;
	zAxis.Cross(xAxis, yAxis);
	Helix::Vector3 zAxisNorm = zAxis;
	zAxisNorm.Normalize();

	mat.r[0][0] = xAxisNorm.x;
	mat.r[0][1] = xAxisNorm.y;
	mat.r[0][2] = xAxisNorm.z;
	mat.r[1][0] = yAxisNorm.x;
	mat.r[1][1] = yAxisNorm.y;
	mat.r[1][2] = yAxisNorm.z;
	mat.r[2][0] = zAxisNorm.x;
	mat.r[2][1] = zAxisNorm.y;
	mat.r[2][2] = zAxisNorm.z;
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