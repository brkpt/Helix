#include "stdafx.h"
#include "VertexDecl.h"

#define	ADD_DECL(x)		m_declUsageMap.insert(UsagePair(#x, x))
#define ADD_TYPE(x)		m_declTypeMap.insert(TypePair(#x, x))
#define	ADD_METHOD(x)	m_declMethodMap.insert(MethodPair(#x,x))

// ****************************************************************************
// ****************************************************************************
VertexDecl::VertexDecl()
: m_decl(NULL)
{
	ADD_DECL(D3DDECLUSAGE_POSITION);
	ADD_DECL(D3DDECLUSAGE_BLENDWEIGHT);
	ADD_DECL(D3DDECLUSAGE_BLENDINDICES);
	ADD_DECL(D3DDECLUSAGE_NORMAL);
	ADD_DECL(D3DDECLUSAGE_PSIZE);
	ADD_DECL(D3DDECLUSAGE_TEXCOORD);
	ADD_DECL(D3DDECLUSAGE_TANGENT);
	ADD_DECL(D3DDECLUSAGE_BINORMAL);
	ADD_DECL(D3DDECLUSAGE_TESSFACTOR);
	ADD_DECL(D3DDECLUSAGE_POSITIONT);
	ADD_DECL(D3DDECLUSAGE_COLOR);
	ADD_DECL(D3DDECLUSAGE_FOG);
	ADD_DECL(D3DDECLUSAGE_DEPTH);
	ADD_DECL(D3DDECLUSAGE_SAMPLE);

	ADD_TYPE(D3DDECLTYPE_FLOAT1);
	ADD_TYPE(D3DDECLTYPE_FLOAT2);
	ADD_TYPE(D3DDECLTYPE_FLOAT3);
	ADD_TYPE(D3DDECLTYPE_FLOAT4);
	ADD_TYPE(D3DDECLTYPE_D3DCOLOR);
	ADD_TYPE(D3DDECLTYPE_UBYTE4);
	ADD_TYPE(D3DDECLTYPE_SHORT2);
	ADD_TYPE(D3DDECLTYPE_UBYTE4N);
	ADD_TYPE(D3DDECLTYPE_SHORT2N);
	ADD_TYPE(D3DDECLTYPE_SHORT4N);
	ADD_TYPE(D3DDECLTYPE_USHORT2N);
	ADD_TYPE(D3DDECLTYPE_USHORT4N);
	ADD_TYPE(D3DDECLTYPE_UDEC3);
	ADD_TYPE(D3DDECLTYPE_DEC3N);
	ADD_TYPE(D3DDECLTYPE_FLOAT16_2);
	ADD_TYPE(D3DDECLTYPE_FLOAT16_4);
	ADD_TYPE(D3DDECLTYPE_UNUSED);

	ADD_METHOD(D3DDECLMETHOD_DEFAULT);
	ADD_METHOD(D3DDECLMETHOD_PARTIALU);
	ADD_METHOD(D3DDECLMETHOD_PARTIALV);
	ADD_METHOD(D3DDECLMETHOD_CROSSUV);
	ADD_METHOD(D3DDECLMETHOD_UV);
	ADD_METHOD(D3DDECLMETHOD_LOOKUP);
	ADD_METHOD(D3DDECLMETHOD_LOOKUPPRESAMPLED);
}

VertexDecl::~VertexDecl()
{
}

// ****************************************************************************
// ****************************************************************************
bool VertexDecl::GetUsage(D3DDECLUSAGE &usage, const std::string &str)
{
	UsageMap::const_iterator iter = m_declUsageMap.find(str);
	if(iter == m_declUsageMap.end())
	{
		return false;
	}

	usage = iter->second;
	return true;
}

// ****************************************************************************
// ****************************************************************************
bool VertexDecl::GetType(D3DDECLTYPE &type, const std::string &str)
{
	TypeMap::const_iterator iter = m_declTypeMap.find(str);
	if(iter == m_declTypeMap.end())
	{
		return false;
	}

	type = iter->second;
	return true;
}

// ****************************************************************************
// ****************************************************************************
bool VertexDecl::GetMethod(D3DDECLMETHOD &method, const std::string &str)
{
	MethodMap::const_iterator iter = m_declMethodMap.find(str);
	if(iter == m_declMethodMap.end())
	{
		return false;
	}

	method = iter->second;
	return true;
}

// ****************************************************************************
// ****************************************************************************
bool VertexDecl::Load(const std::string &path)
{
	std::string fullPath = "Effects/";
	fullPath += path;
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

	m_decl = new D3DVERTEXELEMENT9[numElements+1];

	for(int i=0;i<numElements;i++)
	{
		LuaObject elem = declObj[i+1];

		LuaObject obj = elem[1];
		_ASSERT(obj.IsInteger());
		int streamNum = obj.GetInteger();
		m_decl[i].Stream = streamNum; 

		obj = elem[2];
		_ASSERT(obj.IsInteger());
		int offset = obj.GetInteger();
		m_decl[i].Offset = offset;

		obj = elem[3];
		_ASSERT(obj.IsString());
		std::string typeStr = obj.GetString();
		D3DDECLTYPE declType;
		bool retVal = GetType(declType,typeStr);
		_ASSERT(retVal);
		m_decl[i].Type = declType;

		obj = elem[4];
		_ASSERT(obj.IsString());
		std::string methodStr = obj.GetString();
		D3DDECLMETHOD methodType;
		retVal = GetMethod(methodType,methodStr);
		_ASSERT(retVal);
		m_decl[i].Method = methodType;

		obj = elem[5];
		_ASSERT(obj.IsString());
		std::string usageStr = obj.GetString();
		D3DDECLUSAGE usageType;
		retVal = GetUsage(usageType,usageStr);
		_ASSERT(retVal);
		m_decl[i].Usage = usageType;

		obj = elem[6];
		_ASSERT(obj.IsInteger());
		int usageIndex = obj.GetInteger();
		m_decl[i].UsageIndex = usageIndex;
	}

	// Close off the struct
	m_decl[numElements].Stream = 0xFF;
	m_decl[numElements].Offset = 0;
	m_decl[numElements].Type = D3DDECLTYPE_UNUSED;
	m_decl[numElements].Method = 0;
	m_decl[numElements].Usage = 0;
	m_decl[numElements].UsageIndex = 0;

	return true;
}


