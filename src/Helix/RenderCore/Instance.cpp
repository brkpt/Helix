#include "stdafx.h"
#include "RenderMgr.h"

namespace Helix
{
// ****************************************************************************
// ****************************************************************************
Instance::Instance()
{
	D3DXMatrixIdentity(&m_worldMatrix);
}

// ****************************************************************************
// ****************************************************************************
Instance::~Instance()
{
}

// ****************************************************************************
// ****************************************************************************
//void Instance::Render(int pass)
//{
//	Mesh *mesh = MeshManager::GetInstance().GetMesh(m_meshName);
//	mesh->Render(pass);
//}

} // namespace Helix