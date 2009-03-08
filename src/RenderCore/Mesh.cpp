#include "stdafx.h"
#include "Mesh.h"
#include "LuaPlus.h"
#include "MaterialManager.h"
#include "Material.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexDecl.h"
#include "RenderMgr.h"

// ****************************************************************************
// ****************************************************************************
Mesh::Mesh()
: m_vertexBuffer(NULL)
, m_indexBuffer(NULL)
, m_material(NULL)
, m_numVertices(0)
, m_numIndices(0)
, m_numTriangles(0)
{
}

Mesh::~Mesh()
{
	if(m_vertexBuffer)
		m_vertexBuffer->Release();

	if(m_indexBuffer)
		m_indexBuffer->Release();
}

// ****************************************************************************
// ****************************************************************************
bool Mesh::Load(const std::string &path)
{
	std::string fullPath;
	fullPath = "Meshes/";
	fullPath += path;
	fullPath += ".lua";

	LuaState *state = LuaState::Create();
	_ASSERT(state != NULL);
	
	int retVal = state->DoFile(fullPath.c_str());
	_ASSERT(retVal == 0);

	LuaObject meshList = state->GetGlobals()["MeshList"];
	_ASSERT(meshList.IsTable());

	return Load(path,meshList[1]);
}
// ****************************************************************************
// ****************************************************************************
bool Mesh::Load(const std::string &name, LuaObject &meshObj)
{
	_ASSERT(meshObj.IsTable());

	LuaObject faceListObj = meshObj["Faces"];
	_ASSERT(faceListObj.IsTable());

	LuaObject vertObj = meshObj["Vertices"];
	_ASSERT(vertObj.IsTable());

	LuaObject normalsObj = meshObj["Normals"];
	_ASSERT(normalsObj.IsTable());

	LuaObject uvSetsObj = meshObj["UVSets"];
	_ASSERT(uvSetsObj.IsTable());

	LuaObject nameObj = meshObj["Name"];
	_ASSERT(nameObj.IsString());

	LuaObject matObj = meshObj["Material"];
	_ASSERT(matObj.IsString());
	std::string materialName = matObj.GetString();
	m_material = MaterialManager::GetInstance().Load(materialName);
	_ASSERT(m_material != NULL);

	// Fill in the vertex buffer
	VertexDecl &decl = m_material->GetShader().GetDecl();
	int vertexSize = decl.VertexSize();
	m_numTriangles = faceListObj.GetTableCount();
	for(unsigned int faceIndex=1;faceIndex <= m_numTriangles; faceIndex++)
	{
		LuaObject &faceObj = faceListObj[faceIndex];

		// Triangles only
		_ASSERT(faceObj.GetTableCount() == 3);
		m_numVertices += 3;
	}

	_ASSERT(m_vertexBuffer == NULL);
	_ASSERT(m_indexBuffer == NULL);

	// Create our vertex buffer
	IDirect3DDevice9 *pDevice = RenderMgr::GetInstance().GetDevice();

	// Create the vertex buffer. Here we are allocating enough memory
	// (from the default pool) to hold all our 3 custom vertices. 
	HRESULT hr;
	hr = pDevice->CreateVertexBuffer( m_numVertices*vertexSize,D3DUSAGE_WRITEONLY, NULL,D3DPOOL_MANAGED, &m_vertexBuffer, NULL );
	_ASSERT(SUCCEEDED(hr));

	// Fill in VB
	struct TestVert
	{
		float	pos[3];
		float	uv[2];
	};

	void *vb = NULL;
	hr = m_vertexBuffer->Lock(0,0,&vb, 0);
	_ASSERT(SUCCEEDED(hr));

	char *vertPos = static_cast<char *>(vb);
	for(unsigned int faceIndex=1;faceIndex <= m_numTriangles; faceIndex++)
	{
		LuaObject faceObj = faceListObj[faceIndex];
		
		int numVerts = faceObj.GetTableCount();
		for(int faceVertIdx=1; faceVertIdx <= numVerts; faceVertIdx++)
		{
			char *dataPos = vertPos;

			// Get face vertex information
			LuaObject faceVertObj = faceObj[faceVertIdx];
			{
				float *posData = reinterpret_cast<float *>(dataPos);

				// Get position information
				LuaObject posIdxObj = faceVertObj["VertexIndex"];
				_ASSERT(posIdxObj.IsInteger());

				int vertIndex = posIdxObj.GetInteger();
				LuaObject PosObj = vertObj[vertIndex+1];
				
				posData[0] = PosObj[1].GetFloat();
				posData[1] = PosObj[2].GetFloat();
				posData[2] = PosObj[3].GetFloat();

				dataPos += 3 * sizeof(float);
			}

			{
				// Get UV information
				LuaObject uvIndicesObj = faceVertObj["UVIndices"];
				for(int uvSetIdx=1;uvSetIdx <= uvIndicesObj.GetTableCount(); uvSetIdx++)
				{
					float *uvData = reinterpret_cast<float *>(dataPos);
					LuaObject uvIdxObj = uvIndicesObj[uvSetIdx];
					_ASSERT(uvIdxObj.IsInteger());

					int uvIndex = uvIdxObj.GetInteger();
					LuaObject uvSetObj = uvSetsObj[uvSetIdx];
					LuaObject uvObj = uvSetObj[uvIndex+1];

					
					uvData[0] = uvObj[1].GetFloat();
					uvData[1] = uvObj[2].GetFloat();
					dataPos += 2*sizeof(float);
				}
			}

			// Update vertex pos;
			vertPos = dataPos;
		}
	}

	m_vertexBuffer->Unlock();

	// Create our index buffer
	_ASSERT(m_numVertices < 0xffff);
	m_numIndices = m_numTriangles * 3;
	hr = pDevice->CreateIndexBuffer(m_numIndices * 2,0,D3DFMT_INDEX16,D3DPOOL_DEFAULT,&m_indexBuffer,NULL);
	_ASSERT(SUCCEEDED(hr));

	void *ib = NULL;
	hr = m_indexBuffer->Lock(0,0,&ib,0);
	_ASSERT(SUCCEEDED(hr));
	unsigned short *idxPos = static_cast<unsigned short*>(ib);

	int indexIndex=0;
	int vertexIndex=0;
	for(unsigned int i=0; i<m_numTriangles; i++)
	{
		idxPos[indexIndex + 0] = vertexIndex;
		idxPos[indexIndex + 1] = vertexIndex + 1;
		idxPos[indexIndex + 2] = vertexIndex + 2;

		vertexIndex += 3;
		indexIndex += 3;
	}

	m_indexBuffer->Unlock();
	_ASSERT(indexIndex == m_numIndices);

	return true;
}

// ****************************************************************************
// ****************************************************************************
void Mesh::Render(int pass)
{
	// Set the parameters
	m_material->SetParameters();
	ID3DXEffect *effect = m_material->GetShader().GetEffect();
	VertexDecl &decl = m_material->GetShader().GetDecl();

	//D3DXHANDLE hTechnique = m_pEffect->GetTechnique( m_iCurrentTechnique );
	//D3DXHANDLE hPass      = m_pEffect->GetPass( hTechnique, 0 );
	//D3DXHANDLE hWorldView	= m_pEffect->GetParameterByName(NULL, "WorldView");
	//D3DXHANDLE hProjection = m_pEffect->GetParameterByName(NULL, "Projection");
	//D3DXHANDLE hTexture = m_pEffect->GetParameterByName(NULL,"textureImage");

	//// Set effect parameters
	//m_pEffect->SetMatrix( hWorldView, &viewMatrix );
	//m_pEffect->SetMatrix( hProjection, &projMatrix);
	//m_pEffect->SetTexture( hTexture, m_texture);

	UINT numPasses;
	effect->Begin( &numPasses, D3DXFX_DONOTSAVESTATE );
	effect->BeginPass( 0 );

	effect->CommitChanges();

	// Render triangle here
	IDirect3DDevice9 *dev = RenderMgr::GetInstance().GetDevice();
	_ASSERT(dev);

	dev->SetStreamSource(0,m_vertexBuffer,0,decl.VertexSize());
	dev->SetIndices(m_indexBuffer);
	dev->SetVertexDeclaration(decl.GetDecl());
	dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,m_numVertices,0,m_numTriangles);

	effect->EndPass();
	effect->End();

}