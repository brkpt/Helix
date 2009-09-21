#ifndef FIXEDALLOC_H
#define FIXEDALLOC_H

namespace Helix
{

// ****************************************************************************
// Plex
// ****************************************************************************
struct Plex     // warning variable length structure
{
	Plex* pNext;

	// BYTE data[maxNum*elementSize];

	void* data() { return this+1; }

	static Plex* PASCAL Create(Plex*& head, UINT_PTR nMax, UINT_PTR cbElement);
			// like 'calloc' but no zero fill
			// may throw memory exceptions

	void FreeDataChain();       // free this one and links
};

inline Plex* PASCAL Plex::Create(Plex*& pHead, UINT_PTR nMax, UINT_PTR cbElement)
{
	_ASSERT(nMax > 0 && cbElement > 0);
	if (nMax == 0 || cbElement == 0)
	{
		_ASSERT(0);
//		AfxThrowInvalidArgException();
	}

	Plex* p = (Plex*) new BYTE[sizeof(Plex) + nMax * cbElement];
			// may throw exception
	p->pNext = pHead;
	pHead = p;  // change head (adds in reverse order for simplicity)
	return p;
}

inline void Plex::FreeDataChain()     // free this one and links
{
	Plex* p = this;
	while (p != NULL)
	{
		BYTE* bytes = (BYTE*) p;
		Plex* pNext = p->pNext;
		delete[] bytes;
		p = pNext;
	}
}

// ****************************************************************************
// ****************************************************************************
/////////////////////////////////////////////////////////////////////////////
// FixedAlloc

class FixedAllocNoSync
{
// Constructors
public:
	FixedAllocNoSync(UINT nAllocSize, UINT nBlockSize = 64)
	{
		_ASSERT(nAllocSize >= sizeof(Node));
		_ASSERT(nBlockSize > 1);
		
		if (nAllocSize < sizeof(Node))
			nAllocSize = sizeof(Node);
		if (nBlockSize <= 1)
			nBlockSize = 64;

		m_nObjectSize = nAllocSize;
		m_nNumBlocks = nBlockSize;
		m_pNodeFree = NULL;
		m_pBlocks = NULL;
	}

	FixedAllocNoSync::~FixedAllocNoSync()
	{
		FreeAll();
	}


// Attributes
	UINT GetAllocSize() { return m_nObjectSize; }

// Operations
public:
	// return a chunk of memory of nAllocSize
	void* Alloc()
	{
		if (m_pNodeFree == NULL)
		{
			// add another block
			Plex* pNewBlock = Plex::Create(m_pBlocks, m_nNumBlocks, m_nObjectSize);

			// chain them into free list
			Node* pNode = (Node*)pNewBlock->data();
			// free in reverse order to make it easier to debug
			(BYTE*&)pNode += (m_nObjectSize * m_nNumBlocks) - m_nObjectSize;
			for (int i = m_nNumBlocks-1; i >= 0; i--, (BYTE*&)pNode -= m_nObjectSize)
			{
				pNode->pNext = m_pNodeFree;
				m_pNodeFree = pNode;
			}
		}
		_ASSERT(m_pNodeFree != NULL);  // we must have something

		// remove the first available node from the free list
		void* pNode = m_pNodeFree;
		m_pNodeFree = m_pNodeFree->pNext;
		return pNode;
	}

	// free chunk of memory returned from Alloc
	void Free(void* p)
	{
		if (p != NULL)
		{
			// simply return the node to the free list
			Node* pNode = (Node*)p;
			pNode->pNext = m_pNodeFree;
			m_pNodeFree = pNode;
		}
	}

	// free everything allocated from this allocator
	void FreeAll()
	{
		m_pBlocks->FreeDataChain();
		m_pBlocks = NULL;
		m_pNodeFree = NULL;
	}

// Implementation
public:

protected:
	struct Node
	{
		Node* pNext;	// only valid when in free list
	};

	UINT m_nObjectSize;	// size of each block from Alloc
	UINT m_nNumBlocks;	// number of blocks to get at a time
	Plex* m_pBlocks;	// linked list of blocks (is nBlocks*nAllocSize)
	Node* m_pNodeFree;	// first free node (NULL if no free nodes)
};

class FixedAlloc : public FixedAllocNoSync
{
	typedef class FixedAllocNoSync base;

// Constructors
public:
	FixedAlloc(UINT nAllocSize, UINT nBlockSize = 64) : FixedAllocNoSync(nAllocSize, nBlockSize)
	{
		InitializeCriticalSection(&m_protect);
	}

	~FixedAlloc()
	{
		DeleteCriticalSection(&m_protect);
	}

// Operations
public:
	// return a chunk of memory of nAllocSize
	void* Alloc()
	{
		EnterCriticalSection(&m_protect);
		void* p = NULL;
		//TRY
		//{
			p = base::Alloc();
		//}
		//CATCH_ALL(e)
		//{
		//	LeaveCriticalSection(&m_protect);
		//	THROW_LAST();
		//}
		//END_CATCH_ALL

		LeaveCriticalSection(&m_protect);
		return p;
	}

	// free chunk of memory returned from Alloc
	void Free(void* p)
	{
		if (p != NULL)
		{		
			EnterCriticalSection(&m_protect);
			__try
			{
				base::Free(p);
			}
			__finally
			{
				LeaveCriticalSection(&m_protect);
			}
		}
	}

	// free everything allocated from this allocator
	void FreeAll()
	{
		EnterCriticalSection(&m_protect);
		__try
		{
			base::FreeAll();
		}
		__finally
		{
			LeaveCriticalSection(&m_protect);
		}
	}

// Implementation
public:

protected:
	CRITICAL_SECTION m_protect;
};

//#ifndef _DEBUG

#define UNUSED(x)	(x)

// DECLARE_FIXED_ALLOC -- used in class definition
#define DECLARE_FIXED_ALLOC(class_name) \
public: \
	void* operator new(size_t size) \
	{ \
		_ASSERT(size == s_alloc.GetAllocSize()); \
		UNUSED(size); \
		return s_alloc.Alloc(); \
	} \
	void* operator new(size_t, void* p) \
		{ return p; } \
	void operator delete(void* p) { s_alloc.Free(p); } \
	void* operator new(size_t size, LPCSTR, int) \
	{ \
		_ASSERT(size == s_alloc.GetAllocSize()); \
		UNUSED(size); \
		return s_alloc.Alloc(); \
	} \
protected: \
	static Helix::FixedAlloc s_alloc \

// IMPLEMENT_FIXED_ALLOC -- used in class implementation file
#define IMPLEMENT_FIXED_ALLOC(class_name, block_size) \
Helix::FixedAlloc class_name::s_alloc(sizeof(class_name), block_size) \

// DECLARE_FIXED_ALLOC -- used in class definition
#define DECLARE_FIXED_ALLOC_NOSYNC(class_name) \
public: \
	void* operator new(size_t size) \
	{ \
		_ASSERT(size == s_alloc.GetAllocSize()); \
		UNUSED(size); \
		return s_alloc.Alloc(); \
	} \
	void* operator new(size_t, void* p) \
		{ return p; } \
	void operator delete(void* p) { s_alloc.Free(p); } \
	void* operator new(size_t size, LPCSTR, int) \
	{ \
		_ASSERT(size == s_alloc.GetAllocSize()); \
		UNUSED(size); \
		return s_alloc.Alloc(); \
	} \
protected: \
	static FixedAllocNoSync s_alloc \

// IMPLEMENT_FIXED_ALLOC_NOSYNC -- used in class implementation file
#define IMPLEMENT_FIXED_ALLOC_NOSYNC(class_nbame, block_size) \
Helix::FixedAllocNoSync class_name::s_alloc(sizeof(class_name), block_size) \

//#else //!_DEBUG
//
//#define DECLARE_FIXED_ALLOC(class_name) // nothing in debug
//#define IMPLEMENT_FIXED_ALLOC(class_name, block_size) // nothing in debug
//#define DECLARE_FIXED_ALLOC_NOSYNC(class_name) // nothing in debug
//#define IMPLEMENT_FIXED_ALLOC_NOSYNC(class_name, block_size) // nothing in debug
//
//#endif //!_DEBUG

} // namespace Helix
#endif // FIXEDALLOC_H