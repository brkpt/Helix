#include "stdafx.h"
#include "VertexDecl.h"
#include "RenderMgr.h"

namespace Helix {
// ****************************************************************************
// ****************************************************************************
Mesh::Mesh()
: m_vertexBuffer(NULL)
, m_indexBuffer(NULL)
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
bool Mesh::Load(const std::string &name)
{
	std::string fullPath;
	fullPath = "Meshes/";
	fullPath += name;
	fullPath += ".lua";

	LuaState *state = LuaState::Create();
	_ASSERT(state != NULL);
	
	int retVal = state->DoFile(fullPath.c_str());
	_ASSERT(retVal == 0);

	LuaObject meshList = state->GetGlobals()["MeshList"];
	_ASSERT(meshList.IsTable());

	LuaObject meshObj = meshList[1];

	LuaObject matObj = meshObj["Material"];
	_ASSERT(matObj.IsString());
	m_materialName = matObj.GetString();

	m_meshName = name;
	return CreatePlatformData(name,meshObj);
}
// ****************************************************************************
// ****************************************************************************
bool Mesh::CreatePlatformData(const std::string &name, LuaObject &meshObj)
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

	// Fill in the vertex buffer
	Material *mat = MaterialManager::GetInstance().Load(m_materialName);
	_ASSERT(mat != NULL);

	Shader *shader = ShaderManager::GetInstance().GetShader(mat->GetShaderName());
	_ASSERT(shader != NULL);

	// TODO: Setup vertex format based off of annotation
	ID3D10Effect *shaderEffect = shader->GetEffect();
	_ASSERT(shaderEffect != NULL);

	D3D10_EFFECT_DESC effectDesc;
	HRESULT hr = shaderEffect->GetDesc(&effectDesc);
	_ASSERT( SUCCEEDED(hr) );

	ID3D10EffectTechnique *technique = shaderEffect->GetTechniqueByIndex(0);
	technique = shaderEffect->GetTechniqueByName("ForwardRender");
	_ASSERT( technique!=NULL );

	D3D10_TECHNIQUE_DESC techDesc;
	hr = technique->GetDesc(&techDesc);
	_ASSERT( SUCCEEDED(hr) );

	for(unsigned int annIndex=0;annIndex < techDesc.Annotations; annIndex++)
	{
		ID3D10EffectVariable *annData = technique->GetAnnotationByIndex(annIndex);
		_ASSERT(annData != NULL);
		_ASSERT(annData->IsValid());

		D3D10_EFFECT_VARIABLE_DESC varDesc;
		hr = annData->GetDesc(&varDesc);
		_ASSERT( SUCCEEDED(hr) );

		ID3D10EffectStringVariable *var = annData->AsString();
		LPCSTR varData = NULL;
		hr = var->GetString(&varData);
		_ASSERT( SUCCEEDED(hr) );
	}

	bool havePosData = true;
	bool haveNormData = true;
	bool haveTex1Data = true;

	VertexDecl &decl = shader->GetDecl();
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

	// Create a system memory buffer for our vertices
	char *vb = new char[ m_numVertices * vertexSize ];

	// Fill it in
	char *vertPos = vb;
	for(unsigned int faceIndex=1;faceIndex <= m_numTriangles; faceIndex++)
	{
		LuaObject faceObj = faceListObj[faceIndex];
		
		int numVerts = faceObj.GetTableCount();
		for(int faceVertIdx=1; faceVertIdx <= numVerts; faceVertIdx++)
		{
			char *dataPos = vertPos;

			// Get face vertex information
			LuaObject faceVertObj = faceObj[faceVertIdx];
			if(havePosData)
			{
				// Get position information
				float *posData = reinterpret_cast<float *>(dataPos);

				LuaObject posIdxObj = faceVertObj["VertexIndex"];
				_ASSERT(posIdxObj.IsInteger());

				int vertIndex = posIdxObj.GetInteger();
				LuaObject PosObj = vertObj[vertIndex+1];
				
				posData[0] = PosObj[1].GetFloat();
				posData[1] = PosObj[2].GetFloat();
				posData[2] = PosObj[3].GetFloat();

				dataPos += 3 * sizeof(float);
			}

			if(haveNormData)
			{
				// Get normal information
				float *normData = reinterpret_cast<float *>(dataPos);

				LuaObject normIdxObj = faceVertObj["NormalIndex"];
				_ASSERT(normIdxObj.IsInteger());

				int normIndex = normIdxObj.GetInteger();
				LuaObject normObj = normalsObj[normIndex+1];
				normData[0] = normObj[1].GetFloat();
				normData[1] = normObj[2].GetFloat();
				normData[2] = normObj[3].GetFloat();

				dataPos += 3 * sizeof(float);
			}
			
			if(haveTex1Data)
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

	// Create our vertex buffer
	ID3D10Device *pDevice = RenderMgr::GetInstance().GetDevice();

	// Vertex buffer descriptor
	D3D10_BUFFER_DESC desc = {0};
	desc.Usage = D3D10_USAGE_DEFAULT;
	desc.ByteWidth = m_numVertices*vertexSize;
	desc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	// Data initialization descriptor
	D3D10_SUBRESOURCE_DATA initData = {0};
	initData.pSysMem = vb;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	// Create the buffer
	hr = pDevice->CreateBuffer(&desc,&initData,&m_vertexBuffer);
	_ASSERT( SUCCEEDED(hr) );

	// Destroy the system buffer
	delete vb;

	// Create our index buffer
	_ASSERT(m_numVertices < 0xffff);
	m_numIndices = m_numTriangles * 3;

	// Create a system buffer to hold the data
	unsigned short *ib = new unsigned short[ m_numIndices ] ;
	unsigned short *idxPos = ib;

	// Now fill it in
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

	_ASSERT(indexIndex == m_numIndices);

	// Create the index buffer
	memset(&desc,0,sizeof(desc));
	desc.Usage = D3D10_USAGE_DEFAULT;
	desc.ByteWidth = m_numIndices * 3;
	desc.BindFlags = D3D10_BIND_INDEX_BUFFER;
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

// ****************************************************************************
// ****************************************************************************
void Mesh::Render(int pass)
{
	// Set the parameters
	Material *mat = MaterialManager::GetInstance().GetMaterial(m_materialName);
	mat->SetParameters();

	ID3D10Device *dev = RenderMgr::GetInstance().GetDevice();

	// Set our input assembly buffers
	Shader *shader = ShaderManager::GetInstance().GetShader(mat->GetShaderName());
	unsigned int stride = shader->GetDecl().VertexSize();
	unsigned int offset = 0;
	dev->IASetVertexBuffers(0,1,&m_vertexBuffer,&stride,&offset);
	dev->IASetIndexBuffer(m_indexBuffer,DXGI_FORMAT_R16_UINT,0);

	// Set our prim type
	dev->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	ID3D10Effect *effect = shader->GetEffect();
	D3D10_TECHNIQUE_DESC techDesc;
	ID3D10EffectTechnique *technique = effect->GetTechniqueByIndex(0);
	technique->GetDesc(&techDesc);
	for( unsigned int passIndex = 0; passIndex < techDesc.Passes; passIndex++ )
	{
		technique->GetPassByIndex( passIndex )->Apply( 0 );
		dev->DrawIndexed( m_numIndices, 0, 0 );
	}
}

} // namespace Helix
