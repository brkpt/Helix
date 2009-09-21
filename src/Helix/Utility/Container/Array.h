#ifndef ARRAY_H
#define ARRAY_H

#include "Utility/Container/ElementTraits.h"

namespace Helix
{

template< typename E, class ETraits = ElementTraits< E > >
class Array
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
	size_t Append( const Array< E, ETraits >& aSrc );
	void Copy( const Array< E, ETraits >& aSrc );

	const E& operator[]( size_t iElement ) const;
	E& operator[]( size_t iElement );

	void InsertAt( size_t iElement, INARGTYPE element, size_t nCount = 1 );
	void InsertArrayAt( size_t iStart, const Array< E, ETraits >* paNew );
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

// **********************
// **********************
template< typename E, class ETraits >
inline size_t Array< E, ETraits >::GetCount() const throw()
{
	return( m_nSize );
}

template< typename E, class ETraits >
inline bool Array< E, ETraits >::IsEmpty() const throw()
{
	return( m_nSize == 0 );
}

template< typename E, class ETraits >
inline void Array< E, ETraits >::RemoveAll() throw()
{
	SetCount( 0, -1 );
}

template< typename E, class ETraits >
inline const E& Array< E, ETraits >::GetAt( size_t iElement ) const
{
	_ASSERT( iElement < m_nSize );
	if(iElement >= m_nSize)
		_ASSERT(0);
		//AtlThrow(E_INVALIDARG);

	return( m_pData[iElement] );
}

template< typename E, class ETraits >
inline void Array< E, ETraits >::SetAt( size_t iElement, INARGTYPE element )
{
	_ASSERT( iElement < m_nSize );
	if(iElement >= m_nSize)
		_ASSERT(0);
		//AtlThrow(E_INVALIDARG);

	m_pData[iElement] = element;
}

template< typename E, class ETraits >
inline E& Array< E, ETraits >::GetAt( size_t iElement )
{
	_ASSERT( iElement < m_nSize );
	if(iElement >= m_nSize)
		_ASSERT(0);
		//AtlThrow(E_INVALIDARG);

	return( m_pData[iElement] );
}

template< typename E, class ETraits >
inline const E* Array< E, ETraits >::GetData() const throw()
{
	return( m_pData );
}

template< typename E, class ETraits >
inline E* Array< E, ETraits >::GetData() throw()
{
	return( m_pData );
}

template< typename E, class ETraits >
inline size_t Array< E, ETraits >::Add()
{
	size_t iElement;

	iElement = m_nSize;
	bool bSuccess=SetCount( m_nSize+1 );
	if( !bSuccess )
	{
		_ASSERT(0);
		//AtlThrow( E_OUTOFMEMORY );
	}

	return( iElement );
}

#pragma push_macro("new")
#undef new

template< typename E, class ETraits >
inline size_t Array< E, ETraits >::Add( INARGTYPE element )
{
	size_t iElement;

	iElement = m_nSize;
	if( iElement >= m_nMaxSize )
	{
		bool bSuccess = GrowBuffer( iElement+1 );
		if( !bSuccess )
		{
			_ASSERT(0);
			//AtlThrow( E_OUTOFMEMORY );
		}
	}
	::new( m_pData+iElement ) E( element );
	m_nSize++;

	return( iElement );
}

#pragma pop_macro("new")

template< typename E, class ETraits >
inline const E& Array< E, ETraits >::operator[]( size_t iElement ) const
{
	_ASSERT( iElement < m_nSize );
	if(iElement >= m_nSize)
		_ASSERT(0);
		//AtlThrow(E_INVALIDARG);

	return( m_pData[iElement] );
}

template< typename E, class ETraits >
inline E& Array< E, ETraits >::operator[]( size_t iElement ) 
{
	_ASSERT( iElement < m_nSize );
	if(iElement >= m_nSize)
		_ASSERT(0);
		//AtlThrow(E_INVALIDARG);

	return( m_pData[iElement] );
}

template< typename E, class ETraits >
Array< E, ETraits >::Array()  throw():
	m_pData( NULL ),
	m_nSize( 0 ),
	m_nMaxSize( 0 ),
	m_nGrowBy( 0 )
{
}

template< typename E, class ETraits >
Array< E, ETraits >::~Array() throw()
{
	if( m_pData != NULL )
	{
		CallDestructors( m_pData, m_nSize );
		free( m_pData );
	}
}

template< typename E, class ETraits >
bool Array< E, ETraits >::GrowBuffer( size_t nNewSize )
{
	if( nNewSize > m_nMaxSize )
	{
		if( m_pData == NULL )
		{
			size_t nAllocSize =  size_t( m_nGrowBy ) > nNewSize ? size_t( m_nGrowBy ) : nNewSize ;
			m_pData = static_cast< E* >( calloc( nAllocSize,sizeof( E ) ) );
			if( m_pData == NULL )
			{
				return( false );
			}
			m_nMaxSize = nAllocSize;
		}
		else
		{
			// otherwise, grow array
			size_t nGrowBy = m_nGrowBy;
			if( nGrowBy == 0 )
			{
				// heuristically determine growth when nGrowBy == 0
				//  (this avoids heap fragmentation in many situations)
				nGrowBy = m_nSize/8;
				nGrowBy = (nGrowBy < 4) ? 4 : ((nGrowBy > 1024) ? 1024 : nGrowBy);
			}
			size_t nNewMax;
			if( nNewSize < (m_nMaxSize+nGrowBy) )
				nNewMax = m_nMaxSize+nGrowBy;  // granularity
			else
				nNewMax = nNewSize;  // no slush

			_ASSERT( nNewMax >= m_nMaxSize );  // no wrap around
#ifdef SIZE_T_MAX
			_ASSERT( nNewMax <= SIZE_T_MAX/sizeof( E ) ); // no overflow
#endif
			E* pNewData = static_cast< E* >( calloc( nNewMax,sizeof( E ) ) );
			if( pNewData == NULL )
			{
				return false;
			}

			// copy new data from old
			ETraits::RelocateElements( pNewData, m_pData, m_nSize );

			// get rid of old stuff (note: no destructors called)
			free( m_pData );
			m_pData = pNewData;
			m_nMaxSize = nNewMax;
		}
	}

	return true;
}	

template< typename E, class ETraits >
bool Array< E, ETraits >::SetCount( size_t nNewSize, int nGrowBy )
{
	_ASSERT(this != NULL);

	if( nGrowBy != -1 )
	{
		m_nGrowBy = nGrowBy;  // set new size
	}

	if( nNewSize == 0 )
	{
		// shrink to nothing
		if( m_pData != NULL )
		{
			CallDestructors( m_pData, m_nSize );
			free( m_pData );
			m_pData = NULL;
		}
		m_nSize = 0;
		m_nMaxSize = 0;
	}
	else if( nNewSize <= m_nMaxSize )
	{
		// it fits
		if( nNewSize > m_nSize )
		{
			// initialize the new elements
			CallConstructors( m_pData+m_nSize, nNewSize-m_nSize );
		}
		else if( m_nSize > nNewSize )
		{
			// destroy the old elements
			CallDestructors( m_pData+nNewSize, m_nSize-nNewSize );
		}
		m_nSize = nNewSize;
	}
	else
	{
		bool bSuccess;

		bSuccess = GrowBuffer( nNewSize );
		if( !bSuccess )
		{
			return( false );
		}

		// construct new elements
		_ASSERT( nNewSize > m_nSize );
		CallConstructors( m_pData+m_nSize, nNewSize-m_nSize );

		m_nSize = nNewSize;
	}

	return true;
}

template< typename E, class ETraits >
size_t Array< E, ETraits >::Append( const Array< E, ETraits >& aSrc )
{
	_ASSERT(this != NULL);
	_ASSERT( this != &aSrc );   // cannot append to itself

	size_t nOldSize = m_nSize;
	bool bSuccess=SetCount( m_nSize+aSrc.m_nSize );
	if( !bSuccess )
	{
		AtlThrow( E_OUTOFMEMORY );
	}

	ETraits::CopyElements( m_pData+nOldSize, aSrc.m_pData, aSrc.m_nSize );

	return( nOldSize );
}

template< typename E, class ETraits >
void Array< E, ETraits >::Copy( const Array< E, ETraits >& aSrc )
{
	_ASSERT(this != NULL);
	_ASSERT( this != &aSrc );   // cannot append to itself

	bool bSuccess=SetCount( aSrc.m_nSize );
	if( !bSuccess )
	{
		_ASSERT(0);
		//AtlThrow( E_OUTOFMEMORY );
	}

	ETraits::CopyElements( m_pData, aSrc.m_pData, aSrc.m_nSize );
}

template< typename E, class ETraits >
void Array< E, ETraits >::FreeExtra() throw()
{
	//_ASSERT(this != NULL);

	if( m_nSize != m_nMaxSize )
	{
		// shrink to desired size
#ifdef SIZE_T_MAX
		ATLASSUME( m_nSize <= (SIZE_T_MAX/sizeof( E )) ); // no overflow
#endif
		E* pNewData = NULL;
		if( m_nSize != 0 )
		{
			pNewData = (E*)calloc( m_nSize,sizeof( E ) );
			if( pNewData == NULL )
			{
				return;
			}

			// copy new data from old
			ETraits::RelocateElements( pNewData, m_pData, m_nSize );
		}

		// get rid of old stuff (note: no destructors called)
		free( m_pData );
		m_pData = pNewData;
		m_nMaxSize = m_nSize;
	}
}

template< typename E, class ETraits >
void Array< E, ETraits >::SetAtGrow( size_t iElement, INARGTYPE element )
{
	//_ASSERT(this != NULL);
	size_t nOldSize;

	nOldSize = m_nSize;
	if( iElement >= m_nSize )
	{
		bool bSuccess=SetCount( iElement+1, -1 );
		if( !bSuccess )
		{
			_ASSERT(0);
			//AtlThrow( E_OUTOFMEMORY );
		}
	}

	try
	{
		m_pData[iElement] = element;
	}
	catch(...)
	{
		if( m_nSize != nOldSize )
		{
			SetCount( nOldSize, -1 );
		}
		throw;
	}
}

template< typename E, class ETraits >
void Array< E, ETraits >::InsertAt( size_t iElement, INARGTYPE element, size_t nElements /*=1*/)
{
	//_ASSERT(this != NULL);
	_ASSERT( nElements > 0 );     // zero size not allowed

	if( iElement >= m_nSize )
	{
		// adding after the end of the array
		bool bSuccess=SetCount( iElement+nElements, -1 );   // grow so nIndex is valid
		if( !bSuccess )
		{
			_ASSERT(0);
			//AtlThrow( E_OUTOFMEMORY );
		}
	}
	else
	{
		// inserting in the middle of the array
		size_t nOldSize = m_nSize;
		bool bSuccess=SetCount( m_nSize+nElements, -1 );  // grow it to new size
		if( !bSuccess )
		{
			_ASSERT(0);
			//AtlThrow( E_OUTOFMEMORY );
		}
		// destroy intial data before copying over it
		CallDestructors( m_pData+nOldSize, nElements );
		// shift old data up to fill gap
		ETraits::RelocateElements( m_pData+(iElement+nElements), m_pData+iElement,
			nOldSize-iElement );

		try
		{
			// re-init slots we copied from
			CallConstructors( m_pData+iElement, nElements );
		}
		catch(...)
		{
			ETraits::RelocateElements( m_pData+iElement, m_pData+(iElement+nElements),
				nOldSize-iElement );
			SetCount( nOldSize, -1 );
			throw;
		}
	}

	// insert new value in the gap
	_ASSERT( (iElement+nElements) <= m_nSize );
	for( size_t iNewElement = iElement; iNewElement < (iElement+nElements); iNewElement++ )
	{
		m_pData[iNewElement] = element;
	}
}

template< typename E, class ETraits >
void Array< E, ETraits >::RemoveAt( size_t iElement, size_t nElements )
{
	//_ASSERT(this != NULL);
	_ASSERT( (iElement+nElements) <= m_nSize );

	size_t newCount = iElement+nElements;
	if ((newCount < iElement) || (newCount < nElements) || (newCount > m_nSize))
		_ASSERT(0);
		//AtlThrow(E_INVALIDARG);		
		
	// just remove a range
	size_t nMoveCount = m_nSize-(newCount);
	CallDestructors( m_pData+iElement, nElements );
	if( nMoveCount > 0 )
	{
		ETraits::RelocateElements( m_pData+iElement, m_pData+(newCount),
			nMoveCount );
	}
	m_nSize -= nElements;
}

template< typename E, class ETraits >
void Array< E, ETraits >::InsertArrayAt( size_t iStartElement, 
	const Array< E, ETraits >* paNew )
{
	ATLASSERT_VALID( this );
	_ASSERT( paNew != NULL );
	//ATLASSERT_VALID( paNew );

	if( paNew->GetCount() > 0 )
	{
		InsertAt( iStartElement, paNew->GetAt( 0 ), paNew->GetCount() );
		for( size_t iElement = 0; iElement < paNew->GetCount(); iElement++ )
		{
			SetAt( iStartElement+iElement, paNew->GetAt( iElement ) );
		}
	}
}

#ifdef _DEBUG
template< typename E, class ETraits >
void Array< E, ETraits >::AssertValid() const
{
	if( m_pData == NULL )
	{
		_ASSERT( m_nSize == 0 );
		_ASSERT( m_nMaxSize == 0 );
	}
	else
	{
		_ASSERT( m_nSize <= m_nMaxSize );
		_ASSERT( AtlIsValidAddress( m_pData, m_nMaxSize * sizeof( E ) ) );
	}
}
#endif

#pragma push_macro("new")
#undef new

template< typename E, class ETraits >
void Array< E, ETraits >::CallConstructors( E* pElements, size_t nElements )
{
	size_t iElement = 0;

	try
	{
		for( iElement = 0; iElement < nElements; iElement++ )
		{
			::new( pElements+iElement ) E;
		}
	}
	catch(...)
	{
		while( iElement > 0 )
		{
			iElement--;
			pElements[iElement].~E();
		}

		throw;
	}
}

#pragma pop_macro("new")

template< typename E, class ETraits >
void Array< E, ETraits >::CallDestructors( E* pElements, size_t nElements ) throw()
{
	(void)pElements;

	for( size_t iElement = 0; iElement < nElements; iElement++ )
	{
		pElements[iElement].~E();
	}
}

} // namespace Helix

#endif // ARAY_H