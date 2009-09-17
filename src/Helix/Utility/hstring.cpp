#include "hstring.h"

namespace Helix {

class StringAMgr : public StringManagerBase 
{
public:
	StringAMgr()
	{
		m_nil.SetManager(this);
	}

	CStringData * Allocate(int nAllocLength, int nCharSize) throw()
	{
		unsigned int totalSize = sizeof(CStringData) + nAllocLength*nCharSize;
		CStringData *pData = reinterpret_cast<CStringData *>(new char[totalSize+1]);
		pData->pStringMgr = this;
		pData->nRefs = 1;
		pData->nAllocLength = nAllocLength+1;
		pData->nDataLength = 0;
		return pData;
	}

	void Free(CStringData *pData) throw()
	{
		delete pData;
	}

	CStringData *Reallocate( CStringData* pData, int nAllocLength, int nCharSize ) throw()
	{
		if(pData == NULL)
		{
			return Allocate(nAllocLength, nCharSize);
		}

		if(nAllocLength == 0)
		{
			Free(pData);
			return NULL;
		}

		unsigned int totalSize = sizeof(CStringData) + nAllocLength*nCharSize;
		CStringData *pRet = reinterpret_cast<CStringData *>(realloc(pData,totalSize+1));
		
		pRet->nAllocLength = totalSize;
		return pRet;
	}

	inline CStringData * GetNilString() throw()
	{
		m_nil.AddRef();
		return &m_nil;
	}

	inline StringManagerBase * Clone() throw()
	{
		return this;
	}

private:
	CNilStringData	m_nil;
};

StringManagerBase *StrTraitsA::s_StringManager = NULL;
StringManagerBase *StrTraitsA::InitializeManager()
{
	s_StringManager = new StringAMgr;
	return s_StringManager;
}

StringManagerBase *StrTraitsW::s_StringManager = NULL;
StringManagerBase *StrTraitsW::InitializeManager()
{
	return s_StringManager;
}

} // namespace Helix
