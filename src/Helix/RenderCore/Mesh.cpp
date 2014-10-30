#include "VDecls.h"
#include "RenderMgr.h"
#include "Materials.h"

namespace Helix {
// ****************************************************************************
// ****************************************************************************
Mesh::Mesh()
: m_vertexBuffer(NULL)
, m_indexBuffer(NULL)
, m_numVertices(0)
, m_numIndices(0)
, m_numTriangles(0)
, m_32bitIndices(false)
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
bool Mesh::Load(const std::string &filename)
{
	std::string fullPath;
	fullPath = "Meshes/";
	fullPath += filename;
	fullPath += ".lua";

	LuaPlus::LuaState *state = LuaPlus::LuaState::Create();
	_ASSERT(state != NULL);
	
	int retVal = state->DoFile(fullPath.c_str());
	_ASSERT(retVal == 0);

	LuaPlus::LuaObject meshList = state->GetGlobals()["MeshList"];
	_ASSERT(meshList.IsTable());

	LuaPlus::LuaObject meshObj = meshList[1];

	return Load(meshObj);
}
// ****************************************************************************
// ****************************************************************************
bool Mesh::Load(LuaPlus::LuaObject &meshObj)
{
	LuaPlus::LuaObject matObj = meshObj["Material"];
	_ASSERT(matObj.IsString());
	m_materialName = matObj.GetString();

	LuaPlus::LuaObject nameObj = meshObj["Name"];
	_ASSERT(nameObj.IsString());
	m_meshName = nameObj.GetString();

	return CreatePlatformData(m_meshName,meshObj);
}
// ****************************************************************************
// ****************************************************************************
bool Mesh::CreatePlatformData(const std::string &name, LuaPlus::LuaObject &meshObj)
{
	_ASSERT(meshObj.IsTable());

	LuaPlus::LuaObject faceListObj = meshObj["Faces"];
	_ASSERT(faceListObj.IsTable());

	LuaPlus::LuaObject vertObj = meshObj["Vertices"];
	_ASSERT(vertObj.IsTable());

	LuaPlus::LuaObject normalsObj = meshObj["Normals"];
	_ASSERT(normalsObj.IsTable());

	LuaPlus::LuaObject uvSetsObj = meshObj["UVSets"];
	_ASSERT(uvSetsObj.IsTable());

	LuaPlus::LuaObject nameObj = meshObj["Name"];
	_ASSERT(nameObj.IsString());

	// Fill in the vertex buffer
	HXMaterial *mat = HXLoadMaterial(m_materialName);
	_ASSERT(mat != NULL);

	HXShader *shader = HXGetShaderByName(mat->m_shaderName);
	_ASSERT(shader != NULL);

	int posOffset = 0;
	int normOffset = 0;
	int tex1Offset = 0;

	HXVertexDecl &decl = *shader->m_decl;
	int vertexSize = decl.m_vertexSize;
	bool havePosData = HXDeclHasSemantic(decl,"POSITION",posOffset);
	bool haveNormData = HXDeclHasSemantic(decl,"NORMAL",normOffset);
	bool haveTex1Data = HXDeclHasSemantic(decl,"TEXCOORD",tex1Offset);

	m_numTriangles = faceListObj.GetTableCount();
	for(unsigned int faceIndex=1;faceIndex <= m_numTriangles; faceIndex++)
	{
		LuaPlus::LuaObject &faceObj = faceListObj[faceIndex];

		// Triangles only
		_ASSERT(faceObj.GetTableCount() == 3);
		m_numVertices += 3;
	}

	_ASSERT(m_vertexBuffer == NULL);
	_ASSERT(m_indexBuffer == NULL);

	// Create a system memory buffer for our vertices
	char *vb = new char[ m_numVertices * vertexSize ];

	// Fill it in
	char *vertPos = vb;
	for(unsigned int faceIndex=1;faceIndex <= m_numTriangles; faceIndex++)
	{
		LuaPlus::LuaObject faceObj = faceListObj[faceIndex];
		
		int numVerts = faceObj.GetTableCount();
		for(int faceVertIdx=1; faceVertIdx <= numVerts; faceVertIdx++)
		{
			char *dataPos = vertPos;

			// Get face vertex information
			LuaPlus::LuaObject faceVertObj = faceObj[faceVertIdx];
			if(havePosData)
			{
				// Get position information
				float *posData = reinterpret_cast<float *>(dataPos);

				LuaPlus::LuaObject posIdxObj = faceVertObj["VertexIndex"];
				_ASSERT(posIdxObj.IsInteger());

				int vertIndex = posIdxObj.GetInteger();
				LuaPlus::LuaObject PosObj = vertObj[vertIndex+1];
				
				posData[0] = PosObj[1].GetFloat();
				posData[1] = PosObj[2].GetFloat();
				posData[2] = PosObj[3].GetFloat();

				dataPos += 3 * sizeof(float);
			}

			if(haveNormData)
			{
				// Get normal information
				float *normData = reinterpret_cast<float *>(dataPos);

				LuaPlus::LuaObject normIdxObj = faceVertObj["NormalIndex"];
				_ASSERT(normIdxObj.IsInteger());

				int normIndex = normIdxObj.GetInteger();
				LuaPlus::LuaObject normObj = normalsObj[normIndex+1];
				normData[0] = normObj[1].GetFloat();
				normData[1] = normObj[2].GetFloat();
				normData[2] = normObj[3].GetFloat();

				dataPos += 3 * sizeof(float);
			}
			
			if(haveTex1Data)
			{
				// Get UV information
				LuaPlus::LuaObject uvIndicesObj = faceVertObj["UVIndices"];
				for(size_t uvSetIdx=1;uvSetIdx <= uvIndicesObj.GetTableCount(); uvSetIdx++)
				{
					float *uvData = reinterpret_cast<float *>(dataPos);
					LuaPlus::LuaObject uvIdxObj = uvIndicesObj[uvSetIdx];
					_ASSERT(uvIdxObj.IsInteger());

					int uvIndex = uvIdxObj.GetInteger();
					LuaPlus::LuaObject uvSetObj = uvSetsObj[uvSetIdx];
					LuaPlus::LuaObject uvObj = uvSetObj[uvIndex+1];

					
					uvData[0] = uvObj[1].GetFloat();
					uvData[1] = uvObj[2].GetFloat();
					dataPos += 2*sizeof(float);
				}
			}

			// Update vertex pos;
			vertPos = dataPos;
		}
	}

	// Create our vertex buffer
	ID3D11Device *pDevice = RenderMgr::GetInstance().GetDevice();

	// Vertex buffer descriptor
	D3D11_BUFFER_DESC desc = {0};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = m_numVertices*vertexSize;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	// Data initialization descriptor
	D3D11_SUBRESOURCE_DATA initData = {0};
	initData.pSysMem = vb;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	// Create the buffer
	HRESULT hr = pDevice->CreateBuffer(&desc,&initData,&m_vertexBuffer);
	_ASSERT( SUCCEEDED(hr) );

	// Destroy the system buffer
	delete vb;

	// Create our index buffer
	m_numIndices = m_numTriangles * 3;
	if(m_numVertices > 0xffff)
	{
		m_32bitIndices = true;
	}

	// Create a system buffer to hold the data
	int dataSize = m_32bitIndices ? m_numIndices * 4 : m_numIndices * 2;
	void *ib = static_cast<void *>(new unsigned char[ dataSize ]);
	unsigned short *usIdxPos = static_cast<unsigned short *>(ib);
	unsigned long *ulIdxPos = static_cast<unsigned long *>(ib);

	// Now fill it in
	int indexIndex=0;
	int vertexIndex=0;
	for(unsigned int i=0; i<m_numTriangles; i++)
	{
		if(m_32bitIndices)
		{
			ulIdxPos[indexIndex + 0] = vertexIndex;
			ulIdxPos[indexIndex + 1] = vertexIndex + 1;
			ulIdxPos[indexIndex + 2] = vertexIndex + 2;
		}
		else
		{
			usIdxPos[indexIndex + 0] = vertexIndex;
			usIdxPos[indexIndex + 1] = vertexIndex + 1;
			usIdxPos[indexIndex + 2] = vertexIndex + 2;
		}

		vertexIndex += 3;
		indexIndex += 3;
	}

	_ASSERT(indexIndex == m_numIndices);

	// Create the index buffer
	memset(&desc,0,sizeof(desc));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = dataSize;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	
	// Data initialization descriptor
	memset(&initData,0,sizeof(initData));
	initData.pSysMem = ib;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	hr = pDevice->CreateBuffer(&desc,&initData,&m_indexBuffer);
	_ASSERT( SUCCEEDED(hr) );

	// Destroy the system memory copy
	delete ib;

	return true;
}
//
//// ****************************************************************************
//// ****************************************************************************
//void Mesh::Render(int pass)
//{
//	// Set the parameters
//	HXMaterial *mat = HXGetMaterial(m_materialName);
//	mat->SetParameters();
//
//	ID3D11Device *dev = RenderMgr::GetInstance().GetDevice();
//
//	// Set our input assembly buffers
//	Shader *shader = ShaderManager::GetInstance().GetShader(mat->m_shaderName);
//	unsigned int stride = shader->GetDecl().VertexSize();
//	unsigned int offset = 0;
//	dev->IASetVertexBuffers(0,1,&m_vertexBuffer,&stride,&offset);
//	dev->IASetIndexBuffer(m_indexBuffer,m_32bitIndices ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT,0);
//
//	// Set our prim type
//	dev->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
//
//	ID3DX11Effect *effect = shader->GetEffect();
//	D3D11_TECHNIQUE_DESC techDesc;
//	ID3DX11EffectTechnique *technique = effect->GetTechniqueByIndex(0);
//	technique->GetDesc(&techDesc);
//	for( unsigned int passIndex = 0; passIndex < techDesc.Passes; passIndex++ )
//	{
//		technique->GetPassByIndex( passIndex )->Apply( 0 );
//		dev->DrawIndexed( m_numIndices, 0, 0 );
//	}
//}

} // namespace Helix
