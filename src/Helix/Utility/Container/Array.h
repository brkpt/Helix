#ifndef ARRAY_H
#define ARRAY_H

#include "Utility/Container/ElementTraits.h"
#include "Utility/Memory/HeapAlloc.h"

namespace Helix
{

template< typename E, class ETraits = ElementTraits< E >, class Allocator = HeapAllocator >
class Array : Allocator 
{
public:
	typedef typename ETraits::INARGTYPE INARGTYPE;
	typedef typename ETraits::OUTARGTYPE OUTARGTYPE;

public:
	Array() throw();

	size_t GetCount() const throw();
	bool IsEmpty() const throw();
	bool SetCount( size_t nNewSize, int nGrowBy = -1 );

	void FreeExtra() throw();
	void RemoveAll() throw();

	const E& GetAt( size_t iElement ) const;
	void SetAt( size_t iElement, INARGTYPE element );
	E& GetAt( size_t iElement );

	const E* GetData() const throw();
	E* GetData() throw();

	void SetAtGrow( size_t iElement, INARGTYPE element );
	// Add an empty element to the end of the array
	size_t Add();
	// Add an element to the end of the array
	size_t Add( INARGTYPE element );
	size_t Append( const Array< E, ETraits, Allocator >& aSrc );
	void Copy( const Array< E, ETraits, Allocator >& aSrc );

	const E& operator[]( size_t iElement ) const;
	E& operator[]( size_t iElement );

	void InsertAt( size_t iElement, INARGTYPE element, size_t nCount = 1 );
	void InsertArrayAt( size_t iStart, const Array< E, ETraits, Allocator >* paNew );
	void RemoveAt( size_t iElement, size_t nCount = 1 );

#ifdef _DEBUG
	void AssertValid() const;
#endif  // _DEBUG

private:
	bool GrowBuffer( size_t nNewSize );

// Implementation
private:
	E* m_pData;
	size_t m_nSize;
	size_t m_nMaxSize;
	int m_nGrowBy;

private:
	static void CallConstructors( E* pElements, size_t nElements );
	static void CallDestructors( E* pElements, size_t nElements ) throw();

public:
	~Array() throw();

private:
	// Private to prevent use
	Array( const Array& ) throw();
	Array& operator=( const Array& ) throw();
};

} // namespace Helix

#include "Array.inl"

#endif // ARAY_H