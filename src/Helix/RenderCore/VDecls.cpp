#include "stdafx.h"
#include "VDecls.h"
#include "RenderMgr.h"

// Maps used to store delcaration information
typedef std::map<const std::string, HXVertexDecl *>	DeclMap;
typedef std::map<const std::string, DXGI_FORMAT>	FormatMap;
typedef std::map<const std::string, int>			ClassificationMap;

// Global vertex declaration state information
struct VertexDeclState {
	FormatMap			m_inputLayoutFormatMap;
	ClassificationMap	m_inputLayoutClassificationMap;
	DeclMap				m_database;
};

VertexDeclState *	m_vertexDeclState;
// ****************************************************************************
// ****************************************************************************
// Helper macros for adding all for the different formats and classifications to the maps
#define HX_ADD_FORMAT(x)			m_vertexDeclState->m_inputLayoutFormatMap[#x] = x
#define HX_ADD_CLASSIFICATION(x)	m_vertexDeclState->m_inputLayoutClassificationMap[#x] = x

void HXInitializeVertexDecls()
{
	m_vertexDeclState = new VertexDeclState();

	HX_ADD_FORMAT( DXGI_FORMAT_UNKNOWN );
	HX_ADD_FORMAT( DXGI_FORMAT_R32G32B32A32_TYPELESS );
	HX_ADD_FORMAT( DXGI_FORMAT_R32G32B32A32_FLOAT );
	HX_ADD_FORMAT( DXGI_FORMAT_R32G32B32A32_UINT );
	HX_ADD_FORMAT( DXGI_FORMAT_R32G32B32A32_SINT );
	HX_ADD_FORMAT( DXGI_FORMAT_R32G32B32_TYPELESS );
	HX_ADD_FORMAT( DXGI_FORMAT_R32G32B32_FLOAT );
	HX_ADD_FORMAT( DXGI_FORMAT_R32G32B32_UINT );
	HX_ADD_FORMAT( DXGI_FORMAT_R32G32B32_SINT );
	HX_ADD_FORMAT( DXGI_FORMAT_R16G16B16A16_TYPELESS );
	HX_ADD_FORMAT( DXGI_FORMAT_R16G16B16A16_FLOAT );
	HX_ADD_FORMAT( DXGI_FORMAT_R16G16B16A16_UNORM );
	HX_ADD_FORMAT( DXGI_FORMAT_R16G16B16A16_UINT );
	HX_ADD_FORMAT( DXGI_FORMAT_R16G16B16A16_SNORM );
	HX_ADD_FORMAT( DXGI_FORMAT_R16G16B16A16_SINT );
	HX_ADD_FORMAT( DXGI_FORMAT_R32G32_TYPELESS );
	HX_ADD_FORMAT( DXGI_FORMAT_R32G32_FLOAT );
	HX_ADD_FORMAT( DXGI_FORMAT_R32G32_UINT );
	HX_ADD_FORMAT( DXGI_FORMAT_R32G32_SINT );
	HX_ADD_FORMAT( DXGI_FORMAT_R32G8X24_TYPELESS );
	HX_ADD_FORMAT( DXGI_FORMAT_D32_FLOAT_S8X24_UINT );
	HX_ADD_FORMAT( DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS );
	HX_ADD_FORMAT( DXGI_FORMAT_X32_TYPELESS_G8X24_UINT );
	HX_ADD_FORMAT( DXGI_FORMAT_R10G10B10A2_TYPELESS );
	HX_ADD_FORMAT( DXGI_FORMAT_R10G10B10A2_UNORM );
	HX_ADD_FORMAT( DXGI_FORMAT_R10G10B10A2_UINT );
	HX_ADD_FORMAT( DXGI_FORMAT_R11G11B10_FLOAT );
	HX_ADD_FORMAT( DXGI_FORMAT_R8G8B8A8_TYPELESS );
	HX_ADD_FORMAT( DXGI_FORMAT_R8G8B8A8_UNORM );
	HX_ADD_FORMAT( DXGI_FORMAT_R8G8B8A8_UNORM_SRGB );
	HX_ADD_FORMAT( DXGI_FORMAT_R8G8B8A8_UINT );
	HX_ADD_FORMAT( DXGI_FORMAT_R8G8B8A8_SNORM );
	HX_ADD_FORMAT( DXGI_FORMAT_R8G8B8A8_SINT );
	HX_ADD_FORMAT( DXGI_FORMAT_R16G16_TYPELESS );
	HX_ADD_FORMAT( DXGI_FORMAT_R16G16_FLOAT );
	HX_ADD_FORMAT( DXGI_FORMAT_R16G16_UNORM );
	HX_ADD_FORMAT( DXGI_FORMAT_R16G16_UINT );
	HX_ADD_FORMAT( DXGI_FORMAT_R16G16_SNORM );
	HX_ADD_FORMAT( DXGI_FORMAT_R16G16_SINT );
	HX_ADD_FORMAT( DXGI_FORMAT_R32_TYPELESS );
	HX_ADD_FORMAT( DXGI_FORMAT_D32_FLOAT );
	HX_ADD_FORMAT( DXGI_FORMAT_R32_FLOAT );
	HX_ADD_FORMAT( DXGI_FORMAT_R32_UINT );
	HX_ADD_FORMAT( DXGI_FORMAT_R32_SINT );
	HX_ADD_FORMAT( DXGI_FORMAT_R24G8_TYPELESS );
	HX_ADD_FORMAT( DXGI_FORMAT_D24_UNORM_S8_UINT );
	HX_ADD_FORMAT( DXGI_FORMAT_R24_UNORM_X8_TYPELESS );
	HX_ADD_FORMAT( DXGI_FORMAT_X24_TYPELESS_G8_UINT );
	HX_ADD_FORMAT( DXGI_FORMAT_R8G8_TYPELESS );
	HX_ADD_FORMAT( DXGI_FORMAT_R8G8_UNORM );
	HX_ADD_FORMAT( DXGI_FORMAT_R8G8_UINT );
	HX_ADD_FORMAT( DXGI_FORMAT_R8G8_SNORM );
	HX_ADD_FORMAT( DXGI_FORMAT_R8G8_SINT );
	HX_ADD_FORMAT( DXGI_FORMAT_R16_TYPELESS );
	HX_ADD_FORMAT( DXGI_FORMAT_R16_FLOAT );
	HX_ADD_FORMAT( DXGI_FORMAT_D16_UNORM );
	HX_ADD_FORMAT( DXGI_FORMAT_R16_UNORM );
	HX_ADD_FORMAT( DXGI_FORMAT_R16_UINT );
	HX_ADD_FORMAT( DXGI_FORMAT_R16_SNORM );
	HX_ADD_FORMAT( DXGI_FORMAT_R16_SINT );
	HX_ADD_FORMAT( DXGI_FORMAT_R8_TYPELESS );
	HX_ADD_FORMAT( DXGI_FORMAT_R8_UNORM );
	HX_ADD_FORMAT( DXGI_FORMAT_R8_UINT );
	HX_ADD_FORMAT( DXGI_FORMAT_R8_SNORM );
	HX_ADD_FORMAT( DXGI_FORMAT_R8_SINT );
	HX_ADD_FORMAT( DXGI_FORMAT_A8_UNORM );
	HX_ADD_FORMAT( DXGI_FORMAT_R1_UNORM );
	HX_ADD_FORMAT( DXGI_FORMAT_R9G9B9E5_SHAREDEXP );
	HX_ADD_FORMAT( DXGI_FORMAT_R8G8_B8G8_UNORM );
	HX_ADD_FORMAT( DXGI_FORMAT_G8R8_G8B8_UNORM );
	HX_ADD_FORMAT( DXGI_FORMAT_BC1_TYPELESS );
	HX_ADD_FORMAT( DXGI_FORMAT_BC1_UNORM );
	HX_ADD_FORMAT( DXGI_FORMAT_BC1_UNORM_SRGB );
	HX_ADD_FORMAT( DXGI_FORMAT_BC2_TYPELESS );
	HX_ADD_FORMAT( DXGI_FORMAT_BC2_UNORM );
	HX_ADD_FORMAT( DXGI_FORMAT_BC2_UNORM_SRGB );
	HX_ADD_FORMAT( DXGI_FORMAT_BC3_TYPELESS );
	HX_ADD_FORMAT( DXGI_FORMAT_BC3_UNORM );
	HX_ADD_FORMAT( DXGI_FORMAT_BC3_UNORM_SRGB );
	HX_ADD_FORMAT( DXGI_FORMAT_BC4_TYPELESS );
	HX_ADD_FORMAT( DXGI_FORMAT_BC4_UNORM );
	HX_ADD_FORMAT( DXGI_FORMAT_BC4_SNORM );
	HX_ADD_FORMAT( DXGI_FORMAT_BC5_TYPELESS );
	HX_ADD_FORMAT( DXGI_FORMAT_BC5_UNORM );
	HX_ADD_FORMAT( DXGI_FORMAT_BC5_SNORM );
	HX_ADD_FORMAT( DXGI_FORMAT_B5G6R5_UNORM );
	HX_ADD_FORMAT( DXGI_FORMAT_B5G5R5A1_UNORM );
	HX_ADD_FORMAT( DXGI_FORMAT_B8G8R8A8_UNORM );
	HX_ADD_FORMAT( DXGI_FORMAT_B8G8R8X8_UNORM );

	HX_ADD_CLASSIFICATION( D3D10_INPUT_PER_VERTEX_DATA );
	HX_ADD_CLASSIFICATION( D3D10_INPUT_PER_INSTANCE_DATA );
}

// ****************************************************************************
// ****************************************************************************
HXVertexDecl * HXGetVertexDecl(const std::string &name)
{
	DeclMap::const_iterator iter = m_vertexDeclState->m_database.find(name);
	if( iter == m_vertexDeclState->m_database.end() )
		return NULL;

	return iter->second;
}

// ****************************************************************************
// ****************************************************************************
bool GetFormat(DXGI_FORMAT &format, const std::string &str)
{
	FormatMap::const_iterator iter = m_vertexDeclState->m_inputLayoutFormatMap.find(str);
	if(iter == m_vertexDeclState->m_inputLayoutFormatMap.end())
	{
		return false;
	}

	format = iter->second;
	return true;
}

// ****************************************************************************
// ****************************************************************************
bool GetClassification(D3D10_INPUT_CLASSIFICATION &classification, const std::string &str)
{
	ClassificationMap::const_iterator iter = m_vertexDeclState->m_inputLayoutClassificationMap.find(str);
	if(iter == m_vertexDeclState->m_inputLayoutClassificationMap.end())
	{
		return false;
	}

	classification = static_cast<D3D10_INPUT_CLASSIFICATION>(iter->second);
	return true;
}

// ****************************************************************************
// ****************************************************************************
bool HXLoadVertexDecl(HXVertexDecl &decl, LuaObject &declObj)
{
	int numElements	= declObj.GetTableCount();
	_ASSERT(numElements > 0);

	decl.m_desc = new D3D10_INPUT_ELEMENT_DESC[numElements+1];

	for(int i=0;i<numElements;i++)
	{
		LuaObject elem = declObj[i+1];

		// Semantic
		LuaObject obj = elem[1];
		_ASSERT(obj.IsString());
		decl.m_desc[i].SemanticName = obj.GetString();

		// Semantic index
		obj = elem[2];
		_ASSERT(obj.IsInteger());
		decl.m_desc[i].SemanticIndex = obj.GetInteger();

		// Format
		obj = elem[3];
		_ASSERT(obj.IsString());
		std::string typeStr = obj.GetString();
		DXGI_FORMAT format;
		bool retVal = GetFormat(format,typeStr);
		_ASSERT(retVal);
		decl.m_desc[i].Format = format;

		// Input slot
		obj = elem[4];
		_ASSERT(obj.IsInteger());
		decl.m_desc[i].InputSlot = obj.GetInteger();

		// Aligned byte offset
		obj = elem[5];
		_ASSERT(obj.IsInteger());
		decl.m_desc[i].AlignedByteOffset = obj.GetInteger();

		// Classification
		obj = elem[6];
		_ASSERT(obj.IsString());
		std::string classification = obj.GetString();
		D3D10_INPUT_CLASSIFICATION inpClass;
		retVal = GetClassification(inpClass, classification);
		decl.m_desc[i].InputSlotClass = inpClass;
		_ASSERT(retVal);

		// Step rate
		obj = elem[7];
		_ASSERT(obj.IsInteger());
		decl.m_desc[i].InstanceDataStepRate = obj.GetInteger();		
	}

	decl.m_numElements = numElements;
	for(int index=0;index<decl.m_numElements;index++)
	{
		const D3D10_INPUT_ELEMENT_DESC &element = decl.m_desc[index];

		switch(element.Format)
		{
		case DXGI_FORMAT_R32G32B32A32_FLOAT:
		case DXGI_FORMAT_R32G32B32A32_UINT:
		case DXGI_FORMAT_R32G32B32A32_SINT:
			decl.m_vertexSize += 16;
			break;

		case DXGI_FORMAT_R32G32B32_FLOAT:
		case DXGI_FORMAT_R32G32B32_UINT:
		case DXGI_FORMAT_R32G32B32_SINT:
			decl.m_vertexSize += 12;
			break;

		case DXGI_FORMAT_R16G16B16A16_FLOAT:
		case DXGI_FORMAT_R16G16B16A16_UNORM:
		case DXGI_FORMAT_R16G16B16A16_UINT:
		case DXGI_FORMAT_R16G16B16A16_SNORM:
		case DXGI_FORMAT_R16G16B16A16_SINT:
		case DXGI_FORMAT_R32G32_FLOAT:
		case DXGI_FORMAT_R32G32_UINT:
		case DXGI_FORMAT_R32G32_SINT:
			decl.m_vertexSize += 8;
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
			decl.m_vertexSize += 4;
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
			decl.m_vertexSize += 2;
			break;

		case DXGI_FORMAT_R8_UNORM:
		case DXGI_FORMAT_R8_UINT:
		case DXGI_FORMAT_R8_SNORM:
		case DXGI_FORMAT_R8_SINT:
		case DXGI_FORMAT_A8_UNORM:
			decl.m_vertexSize += 1;
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
HXVertexDecl * HXLoadVertexDecl(const std::string &name)
{
	HXVertexDecl *vdecl = HXGetVertexDecl(name);
	if(vdecl != NULL)
		return vdecl;

	vdecl = new HXVertexDecl;

	std::string fullPath = "Shaders/";
	fullPath += name;
	fullPath += ".lua";
	LuaState *state = LuaState::Create();

	int luaRet = state->DoFile(fullPath.c_str());
	_ASSERT(luaRet == 0);

	LuaObject decl = state->GetGlobals()["VertexDeclaration"];
	_ASSERT(decl.IsTable());

	bool retVal = HXLoadVertexDecl(*vdecl,decl);
	_ASSERT(retVal);

	m_vertexDeclState->m_database[name] = vdecl;

	return vdecl;
}

// ****************************************************************************
// ****************************************************************************
ID3D10InputLayout * HXDeclBuildLayout(HXVertexDecl &decl, Helix::Shader *shader)
{
	if(decl.m_layout != NULL)
	{
		return decl.m_layout;
	}

	// Build the layout against the specific shader
	ID3D10Effect *effect = shader->GetEffect();
	ID3D10EffectTechnique *tech = effect->GetTechniqueByIndex(0);
	ID3D10EffectPass *pass = tech->GetPassByIndex(0);
	D3D10_PASS_DESC passDesc;
	HRESULT hr = pass->GetDesc(&passDesc);
	_ASSERT(hr == S_OK);

	hr = Helix::RenderMgr::GetInstance().GetDevice()->CreateInputLayout(decl.m_desc, decl.m_numElements, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &decl.m_layout);
	_ASSERT(hr == S_OK);

	return decl.m_layout;
}

// ****************************************************************************
// ****************************************************************************
bool HXDeclHasSemantic(HXVertexDecl &decl, const char *semanticName, int &offset)
{
	for(int elementIndex=0;elementIndex < decl.m_numElements; elementIndex++)
	{
		const D3D10_INPUT_ELEMENT_DESC const &element = decl.m_desc[elementIndex];
		if( _stricmp(semanticName,element.SemanticName) == 0)
		{
			offset = element.AlignedByteOffset;
			return true;
		}
	}
	return false;
}
