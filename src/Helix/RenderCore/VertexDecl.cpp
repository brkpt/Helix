#include "stdafx.h"
#include "LuaPlus.h"
#include "VertexDecl.h"
#include "RenderMgr.h"

namespace Helix {
#define ADD_FORMAT(x)			m_inputLayoutFormatMap[#x] = x
#define ADD_CLASSIFICATION(x)	m_inputLayoutClassificationMap[#x] = x

// ****************************************************************************
// ****************************************************************************
VertexDecl::VertexDecl()
: m_layout(NULL)
, m_desc(NULL)
, m_numElements(0)
, m_vertexSize(0)
{
	ADD_FORMAT( DXGI_FORMAT_UNKNOWN );
	ADD_FORMAT( DXGI_FORMAT_R32G32B32A32_TYPELESS );
	ADD_FORMAT( DXGI_FORMAT_R32G32B32A32_FLOAT );
	ADD_FORMAT( DXGI_FORMAT_R32G32B32A32_UINT );
	ADD_FORMAT( DXGI_FORMAT_R32G32B32A32_SINT );
	ADD_FORMAT( DXGI_FORMAT_R32G32B32_TYPELESS );
	ADD_FORMAT( DXGI_FORMAT_R32G32B32_FLOAT );
	ADD_FORMAT( DXGI_FORMAT_R32G32B32_UINT );
	ADD_FORMAT( DXGI_FORMAT_R32G32B32_SINT );
	ADD_FORMAT( DXGI_FORMAT_R16G16B16A16_TYPELESS );
	ADD_FORMAT( DXGI_FORMAT_R16G16B16A16_FLOAT );
	ADD_FORMAT( DXGI_FORMAT_R16G16B16A16_UNORM );
	ADD_FORMAT( DXGI_FORMAT_R16G16B16A16_UINT );
	ADD_FORMAT( DXGI_FORMAT_R16G16B16A16_SNORM );
	ADD_FORMAT( DXGI_FORMAT_R16G16B16A16_SINT );
	ADD_FORMAT( DXGI_FORMAT_R32G32_TYPELESS );
	ADD_FORMAT( DXGI_FORMAT_R32G32_FLOAT );
	ADD_FORMAT( DXGI_FORMAT_R32G32_UINT );
	ADD_FORMAT( DXGI_FORMAT_R32G32_SINT );
	ADD_FORMAT( DXGI_FORMAT_R32G8X24_TYPELESS );
	ADD_FORMAT( DXGI_FORMAT_D32_FLOAT_S8X24_UINT );
	ADD_FORMAT( DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS );
	ADD_FORMAT( DXGI_FORMAT_X32_TYPELESS_G8X24_UINT );
	ADD_FORMAT( DXGI_FORMAT_R10G10B10A2_TYPELESS );
	ADD_FORMAT( DXGI_FORMAT_R10G10B10A2_UNORM );
	ADD_FORMAT( DXGI_FORMAT_R10G10B10A2_UINT );
	ADD_FORMAT( DXGI_FORMAT_R11G11B10_FLOAT );
	ADD_FORMAT( DXGI_FORMAT_R8G8B8A8_TYPELESS );
	ADD_FORMAT( DXGI_FORMAT_R8G8B8A8_UNORM );
	ADD_FORMAT( DXGI_FORMAT_R8G8B8A8_UNORM_SRGB );
	ADD_FORMAT( DXGI_FORMAT_R8G8B8A8_UINT );
	ADD_FORMAT( DXGI_FORMAT_R8G8B8A8_SNORM );
	ADD_FORMAT( DXGI_FORMAT_R8G8B8A8_SINT );
	ADD_FORMAT( DXGI_FORMAT_R16G16_TYPELESS );
	ADD_FORMAT( DXGI_FORMAT_R16G16_FLOAT );
	ADD_FORMAT( DXGI_FORMAT_R16G16_UNORM );
	ADD_FORMAT( DXGI_FORMAT_R16G16_UINT );
	ADD_FORMAT( DXGI_FORMAT_R16G16_SNORM );
	ADD_FORMAT( DXGI_FORMAT_R16G16_SINT );
	ADD_FORMAT( DXGI_FORMAT_R32_TYPELESS );
	ADD_FORMAT( DXGI_FORMAT_D32_FLOAT );
	ADD_FORMAT( DXGI_FORMAT_R32_FLOAT );
	ADD_FORMAT( DXGI_FORMAT_R32_UINT );
	ADD_FORMAT( DXGI_FORMAT_R32_SINT );
	ADD_FORMAT( DXGI_FORMAT_R24G8_TYPELESS );
	ADD_FORMAT( DXGI_FORMAT_D24_UNORM_S8_UINT );
	ADD_FORMAT( DXGI_FORMAT_R24_UNORM_X8_TYPELESS );
	ADD_FORMAT( DXGI_FORMAT_X24_TYPELESS_G8_UINT );
	ADD_FORMAT( DXGI_FORMAT_R8G8_TYPELESS );
	ADD_FORMAT( DXGI_FORMAT_R8G8_UNORM );
	ADD_FORMAT( DXGI_FORMAT_R8G8_UINT );
	ADD_FORMAT( DXGI_FORMAT_R8G8_SNORM );
	ADD_FORMAT( DXGI_FORMAT_R8G8_SINT );
	ADD_FORMAT( DXGI_FORMAT_R16_TYPELESS );
	ADD_FORMAT( DXGI_FORMAT_R16_FLOAT );
	ADD_FORMAT( DXGI_FORMAT_D16_UNORM );
	ADD_FORMAT( DXGI_FORMAT_R16_UNORM );
	ADD_FORMAT( DXGI_FORMAT_R16_UINT );
	ADD_FORMAT( DXGI_FORMAT_R16_SNORM );
	ADD_FORMAT( DXGI_FORMAT_R16_SINT );
	ADD_FORMAT( DXGI_FORMAT_R8_TYPELESS );
	ADD_FORMAT( DXGI_FORMAT_R8_UNORM );
	ADD_FORMAT( DXGI_FORMAT_R8_UINT );
	ADD_FORMAT( DXGI_FORMAT_R8_SNORM );
	ADD_FORMAT( DXGI_FORMAT_R8_SINT );
	ADD_FORMAT( DXGI_FORMAT_A8_UNORM );
	ADD_FORMAT( DXGI_FORMAT_R1_UNORM );
	ADD_FORMAT( DXGI_FORMAT_R9G9B9E5_SHAREDEXP );
	ADD_FORMAT( DXGI_FORMAT_R8G8_B8G8_UNORM );
	ADD_FORMAT( DXGI_FORMAT_G8R8_G8B8_UNORM );
	ADD_FORMAT( DXGI_FORMAT_BC1_TYPELESS );
	ADD_FORMAT( DXGI_FORMAT_BC1_UNORM );
	ADD_FORMAT( DXGI_FORMAT_BC1_UNORM_SRGB );
	ADD_FORMAT( DXGI_FORMAT_BC2_TYPELESS );
	ADD_FORMAT( DXGI_FORMAT_BC2_UNORM );
	ADD_FORMAT( DXGI_FORMAT_BC2_UNORM_SRGB );
	ADD_FORMAT( DXGI_FORMAT_BC3_TYPELESS );
	ADD_FORMAT( DXGI_FORMAT_BC3_UNORM );
	ADD_FORMAT( DXGI_FORMAT_BC3_UNORM_SRGB );
	ADD_FORMAT( DXGI_FORMAT_BC4_TYPELESS );
	ADD_FORMAT( DXGI_FORMAT_BC4_UNORM );
	ADD_FORMAT( DXGI_FORMAT_BC4_SNORM );
	ADD_FORMAT( DXGI_FORMAT_BC5_TYPELESS );
	ADD_FORMAT( DXGI_FORMAT_BC5_UNORM );
	ADD_FORMAT( DXGI_FORMAT_BC5_SNORM );
	ADD_FORMAT( DXGI_FORMAT_B5G6R5_UNORM );
	ADD_FORMAT( DXGI_FORMAT_B5G5R5A1_UNORM );
	ADD_FORMAT( DXGI_FORMAT_B8G8R8A8_UNORM );
	ADD_FORMAT( DXGI_FORMAT_B8G8R8X8_UNORM );

	ADD_CLASSIFICATION( D3D10_INPUT_PER_VERTEX_DATA );
	ADD_CLASSIFICATION( D3D10_INPUT_PER_INSTANCE_DATA );
}

VertexDecl::~VertexDecl()
{
	delete m_desc;
	if(m_layout)
	{
		m_layout->Release();
	}
}

// ****************************************************************************
// ****************************************************************************
bool VertexDecl::GetFormat(DXGI_FORMAT &format, const std::string &str) const
{
	FormatMap::const_iterator iter = m_inputLayoutFormatMap.find(str);
	if(iter == m_inputLayoutFormatMap.end())
	{
		return false;
	}

	format = iter->second;
	return true;
}

// ****************************************************************************
// ****************************************************************************
bool VertexDecl::GetClassification(D3D10_INPUT_CLASSIFICATION &classification, const std::string &str) const
{
	ClassificationMap::const_iterator iter = m_inputLayoutClassificationMap.find(str);
	if(iter == m_inputLayoutClassificationMap.end())
	{
		return false;
	}

	classification = static_cast<D3D10_INPUT_CLASSIFICATION>(iter->second);
	return true;
}

// ****************************************************************************
// ****************************************************************************
bool VertexDecl::Load(const std::string &path)
{
	std::string fullPath = "Shaders/";
	fullPath += path;
	fullPath += ".lua";
	LuaState *state = LuaState::Create();

	int retVal = state->DoFile(fullPath.c_str());
	_ASSERT(retVal == 0);

	LuaObject decl = state->GetGlobals()["VertexDeclaration"];
	_ASSERT(decl.IsTable());

	return Load(decl);
}
// ****************************************************************************
// ****************************************************************************
bool VertexDecl::Load(LuaObject &declObj)
{
	int numElements	= declObj.GetTableCount();
	_ASSERT(numElements > 0);

	m_desc = new D3D10_INPUT_ELEMENT_DESC[numElements+1];

	for(int i=0;i<numElements;i++)
	{
		LuaObject elem = declObj[i+1];

		// Semantic
		LuaObject obj = elem[1];
		_ASSERT(obj.IsString());
		m_desc[i].SemanticName = obj.GetString();

		// Semantic index
		obj = elem[2];
		_ASSERT(obj.IsInteger());
		m_desc[i].SemanticIndex = obj.GetInteger();

		// Format
		obj = elem[3];
		_ASSERT(obj.IsString());
		std::string typeStr = obj.GetString();
		DXGI_FORMAT format;
		bool retVal = GetFormat(format,typeStr);
		_ASSERT(retVal);
		m_desc[i].Format = format;

		// Input slot
		obj = elem[4];
		_ASSERT(obj.IsInteger());
		m_desc[i].InputSlot = obj.GetInteger();

		// Aligned byte offset
		obj = elem[5];
		_ASSERT(obj.IsInteger());
		m_desc[i].AlignedByteOffset = obj.GetInteger();

		// Classification
		obj = elem[6];
		_ASSERT(obj.IsString());
		std::string classification = obj.GetString();
		D3D10_INPUT_CLASSIFICATION inpClass;
		retVal = GetClassification(inpClass, classification);
		m_desc[i].InputSlotClass = inpClass;
		_ASSERT(retVal);

		// Step rate
		obj = elem[7];
		_ASSERT(obj.IsInteger());
		m_desc[i].InstanceDataStepRate = obj.GetInteger();		
	}

	m_numElements = numElements;
	for(int index=0;index<m_numElements;index++)
	{
		const D3D10_INPUT_ELEMENT_DESC &element = m_desc[index];

		switch(element.Format)
		{
		case DXGI_FORMAT_R32G32B32A32_FLOAT:
		case DXGI_FORMAT_R32G32B32A32_UINT:
		case DXGI_FORMAT_R32G32B32A32_SINT:
			m_vertexSize += 16;
			break;

		case DXGI_FORMAT_R32G32B32_FLOAT:
		case DXGI_FORMAT_R32G32B32_UINT:
		case DXGI_FORMAT_R32G32B32_SINT:
			m_vertexSize += 12;
			break;

		case DXGI_FORMAT_R16G16B16A16_FLOAT:
		case DXGI_FORMAT_R16G16B16A16_UNORM:
		case DXGI_FORMAT_R16G16B16A16_UINT:
		case DXGI_FORMAT_R16G16B16A16_SNORM:
		case DXGI_FORMAT_R16G16B16A16_SINT:
		case DXGI_FORMAT_R32G32_FLOAT:
		case DXGI_FORMAT_R32G32_UINT:
		case DXGI_FORMAT_R32G32_SINT:
			m_vertexSize += 8;
			break;

		case DXGI_FORMAT_R10G10B10A2_UNORM:
		case DXGI_FORMAT_R10G10B10A2_UINT:
		case DXGI_FORMAT_R11G11B10_FLOAT:
		case DXGI_FORMAT_R8G8B8A8_UNORM:
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		case DXGI_FORMAT_R8G8B8A8_UINT:
		case DXGI_FORMAT_R8G8B8A8_SNORM:
		case DXGI_FORMAT_R8G8B8A8_SINT:
		case DXGI_FORMAT_R16G16_FLOAT:
		case DXGI_FORMAT_R16G16_UNORM:
		case DXGI_FORMAT_R16G16_UINT:
		case DXGI_FORMAT_R16G16_SNORM:
		case DXGI_FORMAT_R16G16_SINT:
		case DXGI_FORMAT_D32_FLOAT:
		case DXGI_FORMAT_R32_FLOAT:
		case DXGI_FORMAT_R32_UINT:
		case DXGI_FORMAT_R32_SINT:
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
			m_vertexSize += 4;
			break;

		case DXGI_FORMAT_R8G8_UNORM:
		case DXGI_FORMAT_R8G8_UINT:
		case DXGI_FORMAT_R8G8_SNORM:
		case DXGI_FORMAT_R8G8_SINT:
		case DXGI_FORMAT_R16_FLOAT:
		case DXGI_FORMAT_D16_UNORM:
		case DXGI_FORMAT_R16_UNORM:
		case DXGI_FORMAT_R16_UINT:
		case DXGI_FORMAT_R16_SNORM:
		case DXGI_FORMAT_R16_SINT:
			m_vertexSize += 2;
			break;

		case DXGI_FORMAT_R8_UNORM:
		case DXGI_FORMAT_R8_UINT:
		case DXGI_FORMAT_R8_SNORM:
		case DXGI_FORMAT_R8_SINT:
		case DXGI_FORMAT_A8_UNORM:
			m_vertexSize += 1;
			break;

		case DXGI_FORMAT_R1_UNORM:
		case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
		case DXGI_FORMAT_R8G8_B8G8_UNORM:
		case DXGI_FORMAT_G8R8_G8B8_UNORM:
		case DXGI_FORMAT_BC1_UNORM:
		case DXGI_FORMAT_BC1_UNORM_SRGB:
		case DXGI_FORMAT_BC2_UNORM:
		case DXGI_FORMAT_BC2_UNORM_SRGB:
		case DXGI_FORMAT_BC3_UNORM:
		case DXGI_FORMAT_BC3_UNORM_SRGB:
		case DXGI_FORMAT_BC4_UNORM:
		case DXGI_FORMAT_BC4_SNORM:
		case DXGI_FORMAT_BC5_UNORM:
		case DXGI_FORMAT_BC5_SNORM:
		case DXGI_FORMAT_B5G6R5_UNORM:
		case DXGI_FORMAT_B5G5R5A1_UNORM:
		case DXGI_FORMAT_B8G8R8A8_UNORM:
		case DXGI_FORMAT_B8G8R8X8_UNORM:
		default:
			_ASSERT(0);
			break;
		}
	}

	//HRESULT hr = RenderMgr::GetInstance().GetDevice()->CreateVertexDeclaration(decl,&m_decl);
	//_ASSERT(SUCCEEDED(hr));

	return true;
}

// ****************************************************************************
// ****************************************************************************
ID3D10InputLayout * VertexDecl::BuildLayout(Shader *shader)
{
	if(m_layout != NULL)
	{
		return m_layout;
	}

	// Build the layout against the specific shader
	ID3D10Effect *effect = shader->GetEffect();
	ID3D10EffectTechnique *tech = effect->GetTechniqueByIndex(0);
	ID3D10EffectPass *pass = tech->GetPassByIndex(0);
	D3D10_PASS_DESC passDesc;
	HRESULT hr = pass->GetDesc(&passDesc);
	_ASSERT(hr == S_OK);

	hr = RenderMgr::GetInstance().GetDevice()->CreateInputLayout(m_desc, m_numElements, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &m_layout);
	_ASSERT(hr == S_OK);

	return m_layout;
}

} // namespace Helix