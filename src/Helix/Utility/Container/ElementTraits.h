#ifndef ELEMENTTRAITS_H
#define ELEMENTTRAITS_H

namespace Helix
{

template< typename T >
class ElementTraitsBase
{
public:
	typedef const T& INARGTYPE;
	typedef T& OUTARGTYPE;

	static void CopyElements( T* pDest, const T* pSrc, size_t nElements )
	{
		for( size_t iElement = 0; iElement < nElements; iElement++ )
		{
			pDest[iElement] = pSrc[iElement];
		}
	}

	static void RelocateElements( T* pDest, T* pSrc, size_t nElements )
	{
		// A simple memmove works for nearly all types.
		// You'll have to override this for types that have pointers to their
		// own members.
		memmove_s( pDest, nElements*sizeof( T ), pSrc, nElements*sizeof( T ));
	}
};

template< typename T >
class DefaultHashTraits
{
public:
	static ULONG Hash( const T& element ) throw()
	{
		return( ULONG( ULONG_PTR( element ) ) );
	}
};

template< typename T >
class DefaultCompareTraits
{
public:
	static bool CompareElements( const T& element1, const T& element2 )
	{
		return( (element1 == element2) != 0 );  // != 0 to handle overloads of operator== that return BOOL instead of bool
	}

	static int CompareElementsOrdered( const T& element1, const T& element2 )
	{
		if( element1 < element2 )
		{
			return( -1 );
		}
		else if( element1 == element2 )
		{
			return( 0 );
		}
		else
		{
			_ASSERT( element1 > element2 );
			return( 1 );
		}
	}
};

template< typename T >
class DefaultElementTraits :
	public ElementTraitsBase< T >,
	public DefaultHashTraits< T >,
	public DefaultCompareTraits< T >
{
};

template< typename T >
class ElementTraits :
	public DefaultElementTraits< T >
{
};

template< typename T >
class PrimitiveElementTraits :
	public DefaultElementTraits< T >
{
public:
	typedef T INARGTYPE;
	typedef T& OUTARGTYPE;
};


#define _DECLARE_PRIMITIVE_TRAITS( T ) \
	template<> \
	class ElementTraits< T > : \
		public PrimitiveElementTraits< T > \
	{ \
	};

_DECLARE_PRIMITIVE_TRAITS( unsigned char )
_DECLARE_PRIMITIVE_TRAITS( unsigned short )
_DECLARE_PRIMITIVE_TRAITS( unsigned int )
_DECLARE_PRIMITIVE_TRAITS( unsigned long )
_DECLARE_PRIMITIVE_TRAITS( unsigned __int64 )
_DECLARE_PRIMITIVE_TRAITS( signed char )
_DECLARE_PRIMITIVE_TRAITS( char )
_DECLARE_PRIMITIVE_TRAITS( short )
_DECLARE_PRIMITIVE_TRAITS( int )
_DECLARE_PRIMITIVE_TRAITS( long )
_DECLARE_PRIMITIVE_TRAITS( __int64 )
_DECLARE_PRIMITIVE_TRAITS( float )
_DECLARE_PRIMITIVE_TRAITS( double )
_DECLARE_PRIMITIVE_TRAITS( bool )
#ifdef _NATIVE_WCHAR_T_DEFINED
_DECLARE_PRIMITIVE_TRAITS( wchar_t )
#endif
_DECLARE_PRIMITIVE_TRAITS( void* )

} // namespace helix

#endif // ELEMENTTRAITS.H