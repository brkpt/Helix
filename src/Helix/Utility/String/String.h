#ifndef HSTRING_H
#define HSTRING_H

#include <windows.h>
#include <crtdbg.h>

#define HELIX_NOINLINE

#pragma warning(disable:4786)	// avoid 255-character limit warnings

#include <stddef.h>

#ifndef _INC_NEW
#include <new.h>
#endif

#include <stdio.h>
#include <wchar.h>
#include <limits.h>
#include <mbstring.h>

//#include <atlconv.h>
//#include <atlmem.h>

//#ifndef _AFX
//#define _AFX_FUNCNAME(_Name) _Name
//#endif


namespace Helix
{

struct CStringData;

class StringManagerBase
{
public:
	// Allocate a new CStringData
	virtual CStringData* Allocate( int nAllocLength, int nCharSize ) throw() = 0;

	// Free an existing CStringData
	virtual void Free( CStringData* pData ) throw() = 0;

	// Change the size of an existing CStringData
	virtual CStringData* Reallocate( CStringData* pData, int nAllocLength, int nCharSize ) throw() = 0;

	// Get the CStringData for a Nil string
	virtual CStringData* GetNilString() throw() = 0;

	virtual StringManagerBase* Clone() throw() = 0;
};

// This struct have the same memory layout as CString and is used to enable
// const statically initialized CString without making a copy on the heap
template <class StringType,int t_nChars> struct ConstFixedStringT
{
	CStringData m_data;
	typename StringType::XCHAR m_achData[t_nChars];
};

#define IMPLEMENT_CONST_STRING_PTR(StringType, value, name) const ConstFixedStringT<StringType, sizeof(value)/sizeof(StringType::XCHAR)> _init##name ={ 	{NULL,		sizeof(value)/sizeof(StringType::XCHAR)-1, 	sizeof(value)/sizeof(StringType::XCHAR)-1, 	-1},			value	};	const StringType::XCHAR* const _value##name = _init##name.m_achData;	extern const StringType* const name = CONST_STRING_PTR(StringType, name);
#define DECLARE_CONST_STRING_PTR(StringType, name) extern const StringType* const name;
#define CONST_STRING_PTR(StringType, name) reinterpret_cast<const StringType* const>(&_value##name)

/////////////////////////////////////////////////////////////////////////////
//

struct CStringData
{
	StringManagerBase* pStringMgr;  // String manager for this CStringData
	int nDataLength;  // Length of currently used data in XCHARs (not including terminating null)
	int nAllocLength;  // Length of allocated data in XCHARs (not including terminating null)
	long nRefs;     // Reference count: negative == locked
	// XCHAR data[nAllocLength+1]  // A CStringData is always followed in memory by the actual array of character data

	void* data() throw()
	{
		return (this+1);
	}

	void AddRef() throw()
	{
		_ASSERT(nRefs > 0);
		InterlockedIncrement(&nRefs);
	}
	bool IsLocked() const throw()
	{
		return nRefs < 0;
	}
	bool IsShared() const throw()
	{
		return( nRefs > 1 ); 
	}
	void Lock() throw()
	{
		_ASSERT( nRefs <= 1 );
		nRefs--;  // Locked buffers can't be shared, so no interlocked operation necessary
		if( nRefs == 0 )
		{
			nRefs = -1;
		}
	}
	void Release() throw()
	{
		_ASSERT( nRefs != 0 );

		if( InterlockedDecrement( &nRefs ) <= 0 )
		{
			pStringMgr->Free( this );
		}
	}
	void Unlock() throw()
	{
		_ASSERT( IsLocked() );

		if(IsLocked())
		{
			nRefs++;  // Locked buffers can't be shared, so no interlocked operation necessary
			if( nRefs == 0 )
			{
				nRefs = 1;
			}
		}
	}
};

class CNilStringData :
	public CStringData
{
public:
	CNilStringData() throw()
	{
		pStringMgr = NULL;
		nRefs = 2;  // Never gets freed by StringManagerBase
		nDataLength = 0;
		nAllocLength = 0;
		achNil[0] = 0;
		achNil[1] = 0;
	}

	void SetManager( __in StringManagerBase* pMgr ) throw()
	{
		_ASSERT( pStringMgr == NULL );
		pStringMgr = pMgr;
	}

public:
	wchar_t achNil[2];
};

template< typename BaseType, const int t_nSize >
class StaticString
{
public:
	StaticString( __in const BaseType* psz ) :
		m_psz( psz )
	{
	}

	operator const BaseType*() const
	{
		return m_psz;
	}

	static int __cdecl GetLength() 
	{
		return (t_nSize/sizeof( BaseType ))-1;
	}

private:
	const BaseType* m_psz;

private:
	StaticString( const StaticString& str ) throw();
	StaticString& operator=( const StaticString& str ) throw();
};

#define _ST( psz ) ATL::StaticString< TCHAR, sizeof( _T( psz ) ) >( _T( psz ) )
#define _SA( psz ) ATL::StaticString< char, sizeof( psz ) >( psz )
#define _SW( psz ) ATL::StaticString< wchar_t, sizeof( L##psz ) >( L##psz )
#define _SO( psz ) _SW( psz )


template< typename BaseType = char >
class ChTraitsBase
{
public:
	typedef char XCHAR;
	typedef LPSTR PXSTR;
	typedef LPCSTR PCXSTR;
	typedef wchar_t YCHAR;
	typedef LPWSTR PYSTR;
	typedef LPCWSTR PCYSTR;
};

template<>
class ChTraitsBase< wchar_t >
{
public:
	typedef wchar_t XCHAR;
	typedef LPWSTR PXSTR;
	typedef LPCWSTR PCXSTR;
	typedef char YCHAR;
	typedef LPSTR PYSTR;
	typedef LPCSTR PCYSTR;
};

template< typename _CharType = char >
class ChTraitsCRT :
	public ChTraitsBase< _CharType >
{
public:
	static char* __cdecl CharNext( const char* p ) throw()
	{
		//return reinterpret_cast< char* >( _mbsinc( reinterpret_cast< const unsigned char* >( p ) ) );
		return (char *)++p;
	}

	static int __cdecl IsDigit(char ch ) throw()
	{
		//return _ismbcdigit( ch );
		return isdigit(ch);
	}

	static int __cdecl IsSpace(char ch ) throw()
	{
		//return _ismbcspace( ch );
		return isspace(ch);
	}

	static int __cdecl StringCompare( LPCSTR pszA, LPCSTR pszB ) throw()
	{
		//return _mbscmp( reinterpret_cast< const unsigned char* >( pszA ), reinterpret_cast< const unsigned char* >( pszB ) );
		return strcmp(pszA, pszB);
	}

	static int __cdecl StringCompareIgnore( LPCSTR pszA, LPCSTR pszB ) throw()
	{
		//return _mbsicmp( reinterpret_cast< const unsigned char* >( pszA ), reinterpret_cast< const unsigned char* >( pszB ) );
		return stricmp(pszA, pszB);
	}

	static int __cdecl StringCollate( LPCSTR pszA, LPCSTR pszB ) throw()
	{
		//return _mbscoll( reinterpret_cast< const unsigned char* >( pszA ), reinterpret_cast< const unsigned char* >( pszB ) );
		return strcoll(pszA, pszB);
	}

	static int __cdecl StringCollateIgnore( LPCSTR pszA, LPCSTR pszB ) throw()
	{
		//return _mbsicoll( reinterpret_cast< const unsigned char* >( pszA ), reinterpret_cast< const unsigned char* >( pszB ) );
		return _stricoll(pszA,pszB);
	}

	static LPCSTR __cdecl StringFindString( LPCSTR pszBlock, LPCSTR pszMatch ) throw()
	{
		//return reinterpret_cast< LPCSTR >( _mbsstr( reinterpret_cast< const unsigned char* >( pszBlock ),
		//	reinterpret_cast< const unsigned char* >( pszMatch ) ) );
		return strstr(pszBlock, pszMatch);
	}

	static LPSTR __cdecl StringFindString( LPSTR pszBlock, LPCSTR pszMatch ) throw()
	{
		//return( const_cast< LPSTR >( StringFindString( const_cast< LPCSTR >( pszBlock ), pszMatch ) ) );
		return strstr(pszBlock, pszMatch);
	}

	static LPCSTR __cdecl StringFindChar( LPCSTR pszBlock,char chMatch ) throw()
	{
		//return reinterpret_cast< LPCSTR >( _mbschr( reinterpret_cast< const unsigned char* >( pszBlock ), (unsigned char)chMatch ) );
		return strchr(pszBlock, chMatch);
	}

	static LPCSTR __cdecl StringFindCharRev( LPCSTR psz,char ch ) throw()
	{
		//return reinterpret_cast< LPCSTR >( _mbsrchr( reinterpret_cast< const unsigned char* >( psz ), (unsigned char)ch ) );
		return strrchr(psz, ch);
	}

	static LPCSTR __cdecl StringScanSet( LPCSTR pszBlock, LPCSTR pszMatch ) throw()
	{
		//return reinterpret_cast< LPCSTR >( _mbspbrk( reinterpret_cast< const unsigned char* >( pszBlock ),
		//	reinterpret_cast< const unsigned char* >( pszMatch ) ) );
		return strpbrk(pszBlock, pszMatch);
	}

	static int __cdecl StringSpanIncluding( LPCSTR pszBlock, LPCSTR pszSet ) throw()
	{
		//return (int)_mbsspn( reinterpret_cast< const unsigned char* >( pszBlock ), reinterpret_cast< const unsigned char* >( pszSet ) );
		return strspn(pszBlock, pszSet);
	}

	static int __cdecl StringSpanExcluding( LPCSTR pszBlock, LPCSTR pszSet ) throw()
	{
		//return (int)_mbscspn( reinterpret_cast< const unsigned char* >( pszBlock ), reinterpret_cast< const unsigned char* >( pszSet ) );
		return strcspn(pszBlock, pszSet);
	}

	static LPSTR __cdecl StringUppercase( LPSTR psz ) throw()
	{
#pragma warning (push)
#pragma warning(disable : 4996)
		//return reinterpret_cast< LPSTR >( _mbsupr( reinterpret_cast< unsigned char* >( psz ) ) );
		return _strupr(psz);
#pragma warning (pop)
	}

	static LPSTR __cdecl StringLowercase( LPSTR psz ) throw()
	{
#pragma warning (push)
#pragma warning(disable : 4996)
		//return reinterpret_cast< LPSTR >( _mbslwr( reinterpret_cast< unsigned char* >( psz ) ) );
		return _strlwr(psz);
#pragma warning (pop)
	}

	static LPSTR __cdecl StringUppercase( LPSTR psz,size_t size ) throw()
	{
		//mbsupr_s(reinterpret_cast< unsigned char* >( psz ), size);
		//return psz;
		strupr_s(psz, size);
		return psz;
	}

	static LPSTR __cdecl StringLowercase( LPSTR psz,size_t size ) throw()
	{
		//mbslwr_s( reinterpret_cast< unsigned char* >( psz ), size );
		//return psz;
		strlwr_w(psz, size);
		return psz;
	}

	static LPSTR __cdecl StringReverse( LPSTR psz ) throw()
	{
		//return reinterpret_cast< LPSTR >( _mbsrev( reinterpret_cast< unsigned char* >( psz ) ) );
		return _strrev(psz);
	}

	static int __cdecl GetFormattedLength(LPCSTR pszFormat, va_list args ) throw()
	{
		return _vscprintf( pszFormat, args );
	}

	static int __cdecl Format( LPSTR pszBuffer,LPCSTR pszFormat, va_list args ) throw()
	{
		#pragma warning (push)
		#pragma warning(disable : 4996)
		return vsprintf( pszBuffer, pszFormat, args );
		#pragma warning (pop)

	}
	static int __cdecl Format( LPSTR pszBuffer,size_t nlength,LPCSTR pszFormat, va_list args ) throw()
	{
		return vsprintf_s( pszBuffer, nlength, pszFormat, args );
	}

	static int __cdecl GetBaseTypeLength( LPCSTR pszSrc ) throw()
	{
		// Returns required buffer length in XCHARs
		return int( strlen( pszSrc ) );
	}

	static int __cdecl GetBaseTypeLength( LPCSTR pszSrc, int nLength ) throw()
	{
		(void)pszSrc;
		// Returns required buffer length in XCHARs
		return nLength;
	}

	static void __cdecl ConvertToBaseType( LPSTR pszDest,int nDestLength, LPCSTR pszSrc, int nSrcLength = -1 ) throw()
	{
		if (nSrcLength == -1) { nSrcLength=1 + GetBaseTypeLength(pszSrc); }
		// nLen is in XCHARs
		memcpy_s( pszDest, nDestLength*sizeof( char ), pszSrc, nSrcLength*sizeof( char ) );
	}

	//static void ConvertToOem(_CharType* pstrString) throw()
	//{
	//	BOOL fSuccess=::CharToOemA(pstrString, pstrString);
	//	// old version can't report error
	//	_ASSERT(fSuccess);
	//}

	//static void ConvertToAnsi(_CharType* pstrString) throw()
	//{
	//	BOOL fSuccess=::OemToCharA(pstrString, pstrString);
	//	// old version can't report error
	//	_ASSERT(fSuccess);
	//}

	//static void ConvertToOem(_CharType* pstrString,size_t size)
	//{
	//	if(size>UINT_MAX)
	//	{
	//		// API only allows DWORD size
	//		AtlThrow(E_INVALIDARG);
	//	}
	//	DWORD dwSize=static_cast<DWORD>(size);
	//	BOOL fSuccess=::CharToOemBuffA(pstrString, pstrString, dwSize);
	//	if(!fSuccess)
	//	{
	//		AtlThrowLastWin32();
	//	}
	//}

	//static void ConvertToAnsi(_CharType* pstrString,size_t size)
	//{
	//	if(size>UINT_MAX)
	//	{
	//		// API only allows DWORD size
	//		_ASSERT(0);
	//	}
	//	DWORD dwSize=static_cast<DWORD>(size);
	//	BOOL fSuccess=::OemToCharBuffA(pstrString, pstrString, dwSize);
	//	if(!fSuccess)
	//	{
	//		AtlThrowLastWin32();
	//	}
	//}

	static void __cdecl FloodCharacters(char ch,int nLength, char* pch ) throw()
	{
		// nLength is in XCHARs
		memset( pch, ch, nLength );
	}

	//static BSTR __cdecl AllocSysString( const char* pchData, int nDataLength ) throw()
	//{
	//	int nLen = ::MultiByteToWideChar( _AtlGetConversionACP(), 0, pchData, nDataLength,
	//		NULL, NULL );
	//	BSTR bstr = ::SysAllocStringLen( NULL, nLen );
	//	if( bstr != NULL )
	//	{
	//		::MultiByteToWideChar( _AtlGetConversionACP(), 0, pchData, nDataLength,
	//			bstr, nLen );
	//	}

	//	return bstr;
	//}

	//static BOOL __cdecl ReAllocSysString( const char* pchData, BSTR* pbstr,int nDataLength ) throw()
	//{
	//	int nLen = ::MultiByteToWideChar( _AtlGetConversionACP(), 0, pchData, nDataLength, NULL, NULL );
	//	BOOL bSuccess = ::SysReAllocStringLen( pbstr, NULL, nLen );
	//	if( bSuccess )
	//	{
	//		::MultiByteToWideChar( _AtlGetConversionACP(), 0, pchData, nDataLength, *pbstr, nLen );
	//	}

	//	return bSuccess;
	//}

	static DWORD __cdecl FormatMessage(DWORD dwFlags, LPCVOID pSource,
		DWORD dwMessageID,DWORD dwLanguageID, LPSTR pszBuffer,
		DWORD nSize, va_list* pArguments ) throw()
	{
		return ::FormatMessageA( dwFlags, pSource, dwMessageID, dwLanguageID,
				pszBuffer, nSize, pArguments );
	}

	static int __cdecl SafeStringLen( LPCSTR psz ) throw()
	{
		// returns length in bytes
		return (psz != NULL) ? int( strlen( psz ) ) : 0;
	}

	static int __cdecl SafeStringLen( LPCWSTR psz ) throw()
	{
		// returns length in wchar_ts
		return (psz != NULL) ? int( wcslen( psz ) ) : 0;
	}

	static int __cdecl GetCharLen( const wchar_t* pch ) throw()
	{
		(void)pch;
		// returns char length
		return 1;
	}

	static int __cdecl GetCharLen( const char* pch ) throw()
	{
		// returns char length
		return int( _mbclen( reinterpret_cast< const unsigned char* >( pch ) ) );
	}

	//static DWORD __cdecl GetEnvironmentVariable( LPCSTR pszVar,
	//	LPSTR pszBuffer,DWORD dwSize ) throw()
	//{
	//	return ::GetEnvironmentVariableA( pszVar, pszBuffer, dwSize );
	//}
};

// specialization for wchar_t
template<>
class ChTraitsCRT< wchar_t > :
	public ChTraitsBase< wchar_t >
{
	//static DWORD __cdecl _GetEnvironmentVariableW( LPCWSTR pszName, LPWSTR pszBuffer,DWORD nSize ) throw()
	//{
	//	return ::GetEnvironmentVariableW( pszName, pszBuffer, nSize );
	//}

public:
	static LPWSTR __cdecl CharNext( LPCWSTR psz ) throw()
	{
		return const_cast< LPWSTR >( psz+1 );
	}

	static int __cdecl IsDigit(wchar_t ch ) throw()
	{
		return iswdigit( static_cast<unsigned short>(ch) );
	}

	static int __cdecl IsSpace(wchar_t ch ) throw()
	{
		return iswspace( static_cast<unsigned short>(ch) );
	}

	static int __cdecl StringCompare( LPCWSTR pszA, LPCWSTR pszB ) throw()
	{
		return wcscmp( pszA, pszB );
	}

	static int __cdecl StringCompareIgnore( LPCWSTR pszA, LPCWSTR pszB ) throw()
	{
		return _wcsicmp( pszA, pszB );
	}

	static int __cdecl StringCollate( LPCWSTR pszA, LPCWSTR pszB ) throw()
	{
		return wcscoll( pszA, pszB );
	}

	static int __cdecl StringCollateIgnore( LPCWSTR pszA, LPCWSTR pszB ) throw()
	{
		return _wcsicoll( pszA, pszB );
	}

	static LPCWSTR __cdecl StringFindString( LPCWSTR pszBlock, LPCWSTR pszMatch ) throw()
	{
		return wcsstr( pszBlock, pszMatch );
	}

	static LPWSTR __cdecl StringFindString( LPWSTR pszBlock, LPCWSTR pszMatch ) throw()
	{
		return( const_cast< LPWSTR >( StringFindString( const_cast< LPCWSTR >( pszBlock ), pszMatch ) ) );
	}

	static LPCWSTR __cdecl StringFindChar( LPCWSTR pszBlock,wchar_t chMatch ) throw()
	{
		return wcschr( pszBlock, chMatch );
	}

	static LPCWSTR __cdecl StringFindCharRev( LPCWSTR psz,wchar_t ch ) throw()
	{
		return wcsrchr( psz, ch );
	}

	static LPCWSTR __cdecl StringScanSet( LPCWSTR pszBlock, LPCWSTR pszMatch ) throw()
	{
		return wcspbrk( pszBlock, pszMatch );
	}

	static int __cdecl StringSpanIncluding( LPCWSTR pszBlock, LPCWSTR pszSet ) throw()
	{
		return (int)wcsspn( pszBlock, pszSet );
	}

	static int __cdecl StringSpanExcluding( LPCWSTR pszBlock, LPCWSTR pszSet ) throw()
	{
		return (int)wcscspn( pszBlock, pszSet );
	}

	static LPWSTR __cdecl StringUppercase( LPWSTR psz ) throw()
	{
#pragma warning (push)
#pragma warning(disable : 4996)
		return _wcsupr( psz );
#pragma warning (pop)
	}

	static LPWSTR __cdecl StringLowercase( LPWSTR psz ) throw()
	{
#pragma warning (push)
#pragma warning(disable : 4996)
		return _wcslwr( psz );
#pragma warning (pop)
	}

	static LPWSTR __cdecl StringUppercase( LPWSTR psz,size_t size ) throw()
	{
		errno_t err = _wcsupr_s( psz, size );
		return (err == 0) ? psz : NULL;
	}

	static LPWSTR __cdecl StringLowercase( LPWSTR psz,size_t size ) throw()
	{
		errno_t err = _wcslwr_s( psz, size );
		return (err == 0) ? psz : NULL;
	}

	static LPWSTR __cdecl StringReverse( LPWSTR psz ) throw()
	{
		return _wcsrev( psz );
	}

	static int __cdecl GetFormattedLength(LPCWSTR pszFormat, va_list args) throw()
	{
		return _vscwprintf( pszFormat, args );
	}

	static int __cdecl Format( LPWSTR pszBuffer,LPCWSTR pszFormat, va_list args) throw()
	{
		#pragma warning (push)
		#pragma warning(disable : 4996)
		return vswprintf( pszBuffer, pszFormat, args );
		#pragma warning (pop)
	}
	static int __cdecl Format
		( LPWSTR pszBuffer,size_t nLength,LPCWSTR pszFormat, va_list args) throw()
	{
		return vswprintf_s( pszBuffer, nLength, pszFormat, args );
	}

	static int __cdecl GetBaseTypeLength( LPCWSTR pszSrc ) throw()
	{
		// Returns required buffer size in wchar_ts
		return (int)wcslen( pszSrc );
	}

	static int __cdecl GetBaseTypeLength( LPCWSTR pszSrc,int nLength ) throw()
	{
		(void)pszSrc;
		// Returns required buffer size in wchar_ts
		return nLength;
	}

	static void __cdecl ConvertToBaseType( LPWSTR pszDest,int nDestLength, LPCWSTR pszSrc, int nSrcLength = -1 ) throw()
	{		
		if (nSrcLength == -1) { nSrcLength=1 + GetBaseTypeLength(pszSrc); }
		// nLen is in wchar_ts
		wmemcpy_s(pszDest, nDestLength, pszSrc, nSrcLength);
	}

	static void __cdecl FloodCharacters(wchar_t ch,int nLength, LPWSTR psz ) throw()
	{
		// nLength is in XCHARs
		for( int i = 0; i < nLength; i++ )
		{
			psz[i] = ch;
		}
	}

	//static BSTR __cdecl AllocSysString( const wchar_t* pchData,int nDataLength ) throw()
	//{
	//	return ::SysAllocStringLen( pchData, nDataLength );
	//}

	//static BOOL __cdecl ReAllocSysString( const wchar_t* pchData, BSTR* pbstr,int nDataLength ) throw()
	//{
	//	return ::SysReAllocStringLen( pbstr, pchData, nDataLength );
	//}

	static int __cdecl SafeStringLen( LPCSTR psz ) throw()
	{
		// returns length in bytes
		return (psz != NULL) ? (int)strlen( psz ) : 0;
	}

	static int __cdecl SafeStringLen( LPCWSTR psz ) throw()
	{
		// returns length in wchar_ts
		return (psz != NULL) ? (int)wcslen( psz ) : 0;
	}

	static int __cdecl GetCharLen( const wchar_t* pch ) throw()
	{
		(void)pch;
		// returns char length
		return 1;
	}

	static int __cdecl GetCharLen( const char* pch ) throw()
	{
		// returns char length
		return (int)( _mbclen( reinterpret_cast< const unsigned char* >( pch ) ) );
	}

	//static DWORD __cdecl GetEnvironmentVariable( LPCWSTR pszVar, LPWSTR pszBuffer,DWORD dwSize ) throw()
	//{
	//	return _GetEnvironmentVariableW( pszVar, pszBuffer, dwSize );
	//}

	//static void __cdecl ConvertToOem( __reserved LPWSTR /*psz*/ )
	//{
	//	_ASSERT(FALSE); // Unsupported Feature 
	//}

	//static void __cdecl ConvertToAnsi( __reserved LPWSTR /*psz*/ )
	//{
	//	_ASSERT(FALSE); // Unsupported Feature 
	//}

	//static void __cdecl ConvertToOem( __reserved LPWSTR /*psz*/, size_t )
	//{
	//	_ASSERT(FALSE); // Unsupported Feature 
	//}

	//static void __cdecl ConvertToAnsi( __reserved LPWSTR /*psz*/, size_t ) 
	//{
	//	_ASSERT(FALSE); // Unsupported Feature 
	//}

public:
	static DWORD __cdecl FormatMessage(DWORD dwFlags, LPCVOID pSource,
		DWORD dwMessageID,DWORD dwLanguageID, LPWSTR pszBuffer,
		DWORD nSize, va_list* pArguments ) throw()
	{
		return ::FormatMessageW( dwFlags, pSource, dwMessageID, dwLanguageID,
				pszBuffer, nSize, pArguments );
	}
};

class IAtlStringMgrf
{
public:
	// Allocate a new CStringData
	CStringData* Allocate( int nAllocLength, int nCharSize ) throw();
	// Free an existing CStringData
	void Free( CStringData* pData ) throw();
	// Change the size of an existing CStringData
	CStringData* Reallocate( CStringData* pData, int nAllocLength, int nCharSize ) throw();
	// Get the CStringData for a Nil string
	CStringData* GetNilString() throw();
	StringManagerBase* Clone() throw();
};

template< typename TCharType, bool t_bMFCDLL = false >
class StrBufT;

template< typename BaseType , bool t_bMFCDLL = false>
class SimpleStringT
{
public:
	typedef typename ChTraitsBase< BaseType >::XCHAR XCHAR;
	typedef typename ChTraitsBase< BaseType >::PXSTR PXSTR;
	typedef typename ChTraitsBase< BaseType >::PCXSTR PCXSTR;
	typedef typename ChTraitsBase< BaseType >::YCHAR YCHAR;
	typedef typename ChTraitsBase< BaseType >::PYSTR PYSTR;
	typedef typename ChTraitsBase< BaseType >::PCYSTR PCYSTR;

public:
	explicit SimpleStringT( StringManagerBase* pStringMgr )
	{
		_ASSERT( pStringMgr != NULL );
		CStringData* pData = pStringMgr->GetNilString();
		Attach( pData );
	}
	
	SimpleStringT( const SimpleStringT& strSrc )
	{
		CStringData* pSrcData = strSrc.GetData();
		CStringData* pNewData = CloneData( pSrcData );
		Attach( pNewData );
	}
	
	SimpleStringT( const SimpleStringT<BaseType, !t_bMFCDLL>& strSrc )
	{
		CStringData* pSrcData = strSrc.GetData();
		CStringData* pNewData = CloneData( pSrcData );
		Attach( pNewData );
	}
	
	SimpleStringT( PCXSTR pszSrc, StringManagerBase* pStringMgr )
	{
		_ASSERT( pStringMgr != NULL );

		int nLength = StringLength( pszSrc );
		CStringData* pData = pStringMgr->Allocate( nLength, sizeof( XCHAR ) );
		if( pData == NULL )
		{
			ThrowMemoryException();
		}
		Attach( pData );
		SetLength( nLength );
        CopyChars( m_pszData, nLength, pszSrc, nLength );
	}
	SimpleStringT( const XCHAR* pchSrc, int nLength, StringManagerBase* pStringMgr )
	{
		_ASSERT( pStringMgr != NULL );
		
		if(pchSrc == NULL && nLength != 0)
			AtlThrow(E_INVALIDARG);

		CStringData* pData = pStringMgr->Allocate( nLength, sizeof( XCHAR ) );
		if( pData == NULL )
		{
			_ASSERT(0);
			//ThrowMemoryException();
		}
		Attach( pData );
		SetLength( nLength );
        CopyChars( m_pszData, nLength, pchSrc, nLength );
	}
	~SimpleStringT() throw()
	{
		CStringData* pData = GetData();
		pData->Release();
	}
	
	operator SimpleStringT<BaseType, !t_bMFCDLL>&()
	{
		return *(SimpleStringT<BaseType, !t_bMFCDLL>*)this;
	}

	SimpleStringT& operator=( const SimpleStringT& strSrc )
	{
		CStringData* pSrcData = strSrc.GetData();
		CStringData* pOldData = GetData();
		if( pSrcData != pOldData)
		{
			if( pOldData->IsLocked() || pSrcData->pStringMgr != pOldData->pStringMgr )
			{
				SetString( strSrc.GetString(), strSrc.GetLength() );
			}
			else
			{
				CStringData* pNewData = CloneData( pSrcData );
				pOldData->Release();
				Attach( pNewData );
			}
		}

		return( *this );
	}
	
	SimpleStringT& operator=( const SimpleStringT<BaseType, !t_bMFCDLL>& strSrc )
	{
		CStringData* pSrcData = strSrc.GetData();
		CStringData* pOldData = GetData();
		if( pSrcData != pOldData)
		{
			if( pOldData->IsLocked() || pSrcData->pStringMgr != pOldData->pStringMgr )
			{
				SetString( strSrc.GetString(), strSrc.GetLength() );
			}
			else
			{
				CStringData* pNewData = CloneData( pSrcData );
				pOldData->Release();
				Attach( pNewData );
			}
		}

		return( *this );
	}
	
	SimpleStringT& operator=( PCXSTR pszSrc )
	{
		SetString( pszSrc );

		return( *this );
	}

	SimpleStringT& operator+=( const SimpleStringT& strSrc )
	{
		Append( strSrc );

		return( *this );
	}
	template <bool bMFCDLL>
	SimpleStringT& operator+=( const SimpleStringT<BaseType, bMFCDLL>& strSrc )
	{
		Append( strSrc );

		return( *this );
	}
	
	SimpleStringT& operator+=( PCXSTR pszSrc )
	{
		Append( pszSrc );

		return( *this );
	}
	template< int t_nSize >
	SimpleStringT& operator+=( const StaticString< XCHAR, t_nSize >& strSrc )
	{
		Append( static_cast<const XCHAR *>(strSrc), strSrc.GetLength() );

		return( *this );
	}
	SimpleStringT& operator+=( char ch )
	{
		AppendChar( XCHAR( ch ) );

		return( *this );
	}
	SimpleStringT& operator+=( unsigned char ch )
	{
		AppendChar( XCHAR( ch ) );

		return( *this );
	}
	SimpleStringT& operator+=( wchar_t ch )
	{
		AppendChar( XCHAR( ch ) );

		return( *this );
	}

	XCHAR operator[]( int iChar ) const
	{
		_ASSERT( (iChar >= 0) && (iChar <= GetLength()) );  // Indexing the '\0' is OK
		
		if( (iChar < 0) || (iChar > GetLength()) )
			_ASSERT(0);
			//AtlThrow(E_INVALIDARG);
			
		return( m_pszData[iChar] );
	}

	operator PCXSTR() const throw()
	{
		return( m_pszData );
	}

	void Append( PCXSTR pszSrc )
	{
		Append( pszSrc, StringLength( pszSrc ) );
	}
	void Append( PCXSTR pszSrc, int nLength )
	{
		// See comment in SetString() about why we do this
		UINT_PTR nOffset = pszSrc-GetString();

		UINT nOldLength = GetLength();
		if (nOldLength < 0)
		{
			// protects from underflow
			nOldLength = 0;
		}
		int nNewLength = nOldLength+nLength;
		PXSTR pszBuffer = GetBuffer( nNewLength );
		if( nOffset <= nOldLength )
		{
			pszSrc = pszBuffer+nOffset;
			// No need to call CopyCharsOverlapped, since the destination is
			// beyond the end of the original buffer
		}
        CopyChars( pszBuffer+nOldLength, nLength, pszSrc, nLength );
		ReleaseBufferSetLength( nNewLength );
	}
	void AppendChar( XCHAR ch )
	{
		UINT nOldLength = GetLength();
		int nNewLength = nOldLength+1;
		PXSTR pszBuffer = GetBuffer( nNewLength );
		pszBuffer[nOldLength] = ch;
		ReleaseBufferSetLength( nNewLength );
	}
	void Append( const SimpleStringT& strSrc )
	{
		Append( strSrc.GetString(), strSrc.GetLength() );
	}
	template <bool bMFCDLL>
	void Append( const SimpleStringT<BaseType, bMFCDLL>& strSrc )
	{
		Append( strSrc.GetString(), strSrc.GetLength() );
	}	
	void Empty() throw()
	{
		CStringData* pOldData = GetData();
		StringManagerBase* pStringMgr = pOldData->pStringMgr;
		if( pOldData->nDataLength == 0 )
		{
			return;
		}

		if( pOldData->IsLocked() )
		{
			// Don't reallocate a locked buffer that's shrinking
			SetLength( 0 );
		}
		else
		{
			pOldData->Release();
			CStringData* pNewData = pStringMgr->GetNilString();
			Attach( pNewData );
		}
	}
	void FreeExtra()
	{
		CStringData* pOldData = GetData();
		int nLength = pOldData->nDataLength;
		StringManagerBase* pStringMgr = pOldData->pStringMgr;
		if( pOldData->nAllocLength == nLength )
		{
			return;
		}

		if( !pOldData->IsLocked() )  // Don't reallocate a locked buffer that's shrinking
		{
			CStringData* pNewData = pStringMgr->Allocate( nLength, sizeof( XCHAR ) );
			if( pNewData == NULL )
			{
				SetLength( nLength );
				return;
			}
			
            CopyChars( PXSTR( pNewData->data() ), nLength, 
				PCXSTR( pOldData->data() ), nLength );

			pOldData->Release();
			Attach( pNewData );
			SetLength( nLength );
		}
	}

	int GetAllocLength() const throw()
	{
		return( GetData()->nAllocLength );
	}
	XCHAR GetAt( int iChar ) const
	{
		_ASSERT( (iChar >= 0) && (iChar <= GetLength()) );  // Indexing the '\0' is OK
		if( (iChar < 0) || (iChar > GetLength()) )
			_ASSERT(0);
			//AtlThrow(E_INVALIDARG);		
			
		return( m_pszData[iChar] );
	}
	PXSTR GetBuffer()
	{
		CStringData* pData = GetData();
		if( pData->IsShared() )
		{
			Fork( pData->nDataLength );
		}

		return( m_pszData );
	}
	PXSTR GetBuffer( int nMinBufferLength )
	{
		return( PrepareWrite( nMinBufferLength ) );
	}
	PXSTR GetBufferSetLength( int nLength )
	{
		PXSTR pszBuffer = GetBuffer( nLength );
		SetLength( nLength );

		return( pszBuffer );
	}
	int GetLength() const throw()
	{
		return( GetData()->nDataLength );
	}
	StringManagerBase* GetManager() const throw()
	{
		StringManagerBase* pStringMgr = GetData()->pStringMgr;
		return pStringMgr ? pStringMgr->Clone() : NULL;
	}

	PCXSTR GetString() const throw()
	{
		return( m_pszData );
	}
	bool IsEmpty() const throw()
	{
		return( GetLength() == 0 );
	}
	PXSTR LockBuffer()
	{
		CStringData* pData = GetData();
		if( pData->IsShared() )
		{
			Fork( pData->nDataLength );
			pData = GetData();  // Do it again, because the fork might have changed it
		}
		pData->Lock();

		return( m_pszData );
	}
	void UnlockBuffer() throw()
	{
		CStringData* pData = GetData();
		pData->Unlock();
	}
	void Preallocate( int nLength )
	{
		PrepareWrite( nLength );
	}
	void ReleaseBuffer( int nNewLength = -1 )
	{
		if( nNewLength == -1 )
		{
			nNewLength = StringLength( m_pszData );
		}
		SetLength( nNewLength );
	}
	void ReleaseBufferSetLength( int nNewLength )
	{
		_ASSERT( nNewLength >= 0 );
		SetLength( nNewLength );
	}
	void Truncate( int nNewLength )
	{
		_ASSERT( nNewLength <= GetLength() );
		GetBuffer( nNewLength );
		ReleaseBufferSetLength( nNewLength );
	}
	void SetAt( int iChar, XCHAR ch )
	{
		_ASSERT( (iChar >= 0) && (iChar < GetLength()) );

		if( (iChar < 0) || (iChar >= GetLength()) )
			_ASSERT(0);
			//AtlThrow(E_INVALIDARG);		
			
		int nLength = GetLength();
		PXSTR pszBuffer = GetBuffer();
		pszBuffer[iChar] = ch;
		ReleaseBufferSetLength( nLength );
			
	}
	void SetManager( StringManagerBase* pStringMgr )
	{
		_ASSERT( IsEmpty() );

		CStringData* pData = GetData();
		pData->Release();
		pData = pStringMgr->GetNilString();
		Attach( pData );
	}
	void SetString( PCXSTR pszSrc )
	{
		SetString( pszSrc, StringLength( pszSrc ) );
	}
	void SetString( PCXSTR pszSrc, int nLength )
	{
		if( nLength == 0 )
		{
			Empty();
		}
		else
		{
			// It is possible that pszSrc points to a location inside of our 
			// buffer.  GetBuffer() might change m_pszData if (1) the buffer 
			// is shared or (2) the buffer is too small to hold the new 
			// string.  We detect this aliasing, and modify pszSrc to point
			// into the newly allocated buffer instead.
			
			if(pszSrc == NULL)
				_ASSERT(0);
				//AtlThrow(E_INVALIDARG);			
				
			UINT nOldLength = GetLength();
			UINT_PTR nOffset = pszSrc-GetString();
			// If 0 <= nOffset <= nOldLength, then pszSrc points into our 
			// buffer

			PXSTR pszBuffer = GetBuffer( nLength );
			if( nOffset <= nOldLength )
			{
                CopyCharsOverlapped( pszBuffer, nLength, 
					pszBuffer+nOffset, nLength );
			}
			else
			{
				CopyChars( pszBuffer, nLength, pszSrc, nLength );
			}
			ReleaseBufferSetLength( nLength );
		}
	}

public:
	friend SimpleStringT operator+(
		const SimpleStringT& str1,
		const SimpleStringT& str2 )
	{
		SimpleStringT s( str1.GetManager() );

		Concatenate( s, str1, str1.GetLength(), str2, str2.GetLength() );

		return( s );
	}

	friend SimpleStringT operator+(
		const SimpleStringT& str1,
		PCXSTR psz2 )
	{
		SimpleStringT s( str1.GetManager() );

		Concatenate( s, str1, str1.GetLength(), psz2, StringLength( psz2 ) );

		return( s );
	}

	friend SimpleStringT operator+(
		PCXSTR psz1,
		const SimpleStringT& str2 )
	{
		SimpleStringT s( str2.GetManager() );

		Concatenate( s, psz1, StringLength( psz1 ), str2, str2.GetLength() );

		return( s );
	}

	static void __cdecl CopyChars
		( XCHAR* pchDest, const XCHAR* pchSrc, int nChars ) throw()
	{
		#pragma warning (push)
		#pragma warning(disable : 4996)
		memcpy( pchDest, pchSrc, nChars*sizeof( XCHAR ) );
		#pragma warning (pop)
	}
	static void __cdecl CopyChars
		( XCHAR* pchDest, size_t nDestLen, const XCHAR* pchSrc, int nChars ) throw()
	{
		memcpy_s( pchDest, nDestLen*sizeof( XCHAR ), 
			pchSrc, nChars*sizeof( XCHAR ) );
	}

	static void __cdecl CopyCharsOverlapped
		( XCHAR* pchDest, const XCHAR* pchSrc, int nChars ) throw()
	{
		#pragma warning (push)
		#pragma warning(disable : 4996)
		memmove( pchDest, pchSrc, nChars*sizeof( XCHAR ) );
		#pragma warning (pop)
	}
	static void __cdecl CopyCharsOverlapped
		( XCHAR* pchDest, size_t nDestLen, const XCHAR* pchSrc, int nChars ) throw()
	{
		memmove_s( pchDest, nDestLen*sizeof( XCHAR ), 
			pchSrc, nChars*sizeof( XCHAR ) );
	}
	static int __cdecl StringLength( const char* psz ) throw()
	{
		if( psz == NULL )
		{
			return( 0 );
		}
		return( int( strlen( psz ) ) );
	}
	static int __cdecl StringLength( const wchar_t* psz ) throw()
	{
		if( psz == NULL )
		{
			return( 0 );
		}
		return( int( wcslen( psz ) ) );
	}

protected:
	static void __cdecl Concatenate( __out SimpleStringT& strResult, __in_ecount(nLength1) PCXSTR psz1, int nLength1, __in_ecount(nLength2) PCXSTR psz2, int nLength2 )
	{
		int nNewLength = nLength1+nLength2;
		PXSTR pszBuffer = strResult.GetBuffer( nNewLength );
        CopyChars( pszBuffer, nLength1, psz1, nLength1 );
		CopyChars( pszBuffer+nLength1, nLength2, psz2, nLength2 );
		strResult.ReleaseBufferSetLength( nNewLength );
	}
	HELIX_NOINLINE __declspec( noreturn ) static void __cdecl ThrowMemoryException()
	{
		_ASSERT(0);
		//AtlThrow( E_OUTOFMEMORY );
	}

// Implementation
private:
	void Attach( CStringData* pData ) throw()
	{
		m_pszData = static_cast< PXSTR >( pData->data() );
	}
	HELIX_NOINLINE void Fork( int nLength )
	{
		CStringData* pOldData = GetData();
		int nOldLength = pOldData->nDataLength;
		CStringData* pNewData = pOldData->pStringMgr->Clone()->Allocate( nLength, sizeof( XCHAR ) );
		if( pNewData == NULL )
		{
			ThrowMemoryException();
		}
		int nCharsToCopy = ((nOldLength < nLength) ? nOldLength : nLength)+1;  // Copy '\0'
        CopyChars( PXSTR( pNewData->data() ), nCharsToCopy, 
			PCXSTR( pOldData->data() ), nCharsToCopy );
		pNewData->nDataLength = nOldLength;
		pOldData->Release();
		Attach( pNewData );
	}
	CStringData* GetData() const throw()
	{
		return( reinterpret_cast< CStringData* >( m_pszData )-1 );
	}
	PXSTR PrepareWrite( int nLength )
	{
		CStringData* pOldData = GetData();
		int nShared = 1-pOldData->nRefs;  // nShared < 0 means true, >= 0 means false
		int nTooShort = pOldData->nAllocLength-nLength;  // nTooShort < 0 means true, >= 0 means false
		if( (nShared|nTooShort) < 0 )  // If either sign bit is set (i.e. either is less than zero), we need to copy data
		{
			PrepareWrite2( nLength );
		}

		return( m_pszData );
	}
	HELIX_NOINLINE void PrepareWrite2( int nLength )
	{
		CStringData* pOldData = GetData();
		if( pOldData->nDataLength > nLength )
		{
			nLength = pOldData->nDataLength;
		}
		if( pOldData->IsShared() )
		{
			Fork( nLength );
		}
		else if( pOldData->nAllocLength < nLength )
		{
			// Grow exponentially, until we hit 1K.
			int nNewLength = pOldData->nAllocLength;
			if( nNewLength > 1024 )
			{
				nNewLength += 1024;
			}
			else
			{
				nNewLength *= 2;
			}
			if( nNewLength < nLength )
			{
				nNewLength = nLength;
			}
			Reallocate( nNewLength );
		}
	}
	HELIX_NOINLINE void Reallocate( int nLength )
	{
		CStringData* pOldData = GetData();
		_ASSERT( pOldData->nAllocLength < nLength );
		StringManagerBase* pStringMgr = pOldData->pStringMgr;
                if ( pOldData->nAllocLength >= nLength || nLength <= 0)
                {
			ThrowMemoryException();
                        return;
                }
		CStringData* pNewData = pStringMgr->Reallocate( pOldData, nLength, sizeof( XCHAR ) );
		if( pNewData == NULL )
		{
			ThrowMemoryException();
		}
		Attach( pNewData );
	}

	void SetLength( int nLength )
	{
		_ASSERT( nLength >= 0 );
		_ASSERT( nLength <= GetData()->nAllocLength );

		if( nLength < 0 || nLength > GetData()->nAllocLength)
			_ASSERT(0);
			//AtlThrow(E_INVALIDARG);
			
		GetData()->nDataLength = nLength;
		m_pszData[nLength] = 0;
	}

	static CStringData* __cdecl CloneData( CStringData* pData )
	{
		CStringData* pNewData = NULL;

		StringManagerBase* pNewStringMgr = pData->pStringMgr->Clone();
		if( !pData->IsLocked() && (pNewStringMgr == pData->pStringMgr) )
		{
			pNewData = pData;
			pNewData->AddRef();
		}
		else
		{
			pNewData = pNewStringMgr->Allocate( pData->nDataLength, sizeof( XCHAR ) );
			if( pNewData == NULL )
			{
				ThrowMemoryException();
			}
			pNewData->nDataLength = pData->nDataLength;
            CopyChars( PXSTR( pNewData->data() ), pData->nDataLength+1,
				PCXSTR( pData->data() ), pData->nDataLength+1 );  // Copy '\0'
		}

		return( pNewData );
	}

public :
	typedef StrBufT<BaseType, t_bMFCDLL> CStrBuf;
private:
	PXSTR m_pszData;
	
	friend class SimpleStringT<BaseType, !t_bMFCDLL>;
};

template< typename TCharType, bool t_bMFCDLL >
class StrBufT
{
public:
	typedef SimpleStringT< TCharType, t_bMFCDLL> StringType;
	typedef typename StringType::XCHAR XCHAR;
	typedef typename StringType::PXSTR PXSTR;
	typedef typename StringType::PCXSTR PCXSTR;

	static const DWORD AUTO_LENGTH = 0x01;  // Automatically determine the new length of the string at release.  The string must be null-terminated.
	static const DWORD SET_LENGTH = 0x02;  // Set the length of the string object at GetBuffer time

public:
	explicit StrBufT( __in StringType& str ) throw( ... ) :
		m_str( str ),
		m_pszBuffer( NULL ),
#ifdef _DEBUG
		m_nBufferLength( str.GetLength() ),
#endif
		m_nLength( str.GetLength() )
	{
		m_pszBuffer = m_str.GetBuffer();
	}
	
	StrBufT( __in StringType& str, __in int nMinLength, __in DWORD dwFlags = AUTO_LENGTH ) throw( ... ) :
		m_str( str ),
		m_pszBuffer( NULL ),
#ifdef _DEBUG
		m_nBufferLength( nMinLength ),
#endif
		m_nLength( (dwFlags&AUTO_LENGTH) ? -1 : nMinLength )
	{
		if( dwFlags&SET_LENGTH )
		{
			m_pszBuffer = m_str.GetBufferSetLength( nMinLength );
		}
		else
		{
			m_pszBuffer = m_str.GetBuffer( nMinLength );
		}
	}

	~StrBufT()
	{
		m_str.ReleaseBuffer( m_nLength );
	}

	operator PXSTR() throw()
	{
		return( m_pszBuffer );
	}
	operator PCXSTR() const throw()
	{
		return( m_pszBuffer );
	}

	void SetLength( __in int nLength )
	{
		ATLASSERT( nLength >= 0 );		
		ATLASSERT( nLength <= m_nBufferLength );

		if( nLength < 0 )
			AtlThrow(E_INVALIDARG);
		
		m_nLength = nLength;
	}

// Implementation
private:
	StringType& m_str;
	PXSTR m_pszBuffer;
	int m_nLength;
#ifdef _DEBUG
	int m_nBufferLength;
#endif

// Private copy constructor and copy assignment operator to prevent accidental use
private:
	StrBufT( const StrBufT& ) throw();
	StrBufT& operator=( const StrBufT& ) throw();
};

typedef SimpleStringT< TCHAR > CSimpleString;
typedef SimpleStringT< char > CSimpleStringA;
typedef SimpleStringT< wchar_t > CSimpleStringW;
typedef StrBufT< TCHAR > CStrBuf;
typedef StrBufT< char > CStrBufA;
typedef StrBufT< wchar_t > CStrBufW;

template< typename BaseType, class StringTraits >
class StringT :
	public SimpleStringT< BaseType >
{
public:
	typedef SimpleStringT< BaseType > ThisSimpleString;
	typedef StringTraits StrTraits;
	typedef typename ThisSimpleString::XCHAR XCHAR;
	typedef typename ThisSimpleString::PXSTR PXSTR;
	typedef typename ThisSimpleString::PCXSTR PCXSTR;
	typedef typename ThisSimpleString::YCHAR YCHAR;
	typedef typename ThisSimpleString::PYSTR PYSTR;
	typedef typename ThisSimpleString::PCYSTR PCYSTR;

public:
	StringT() throw() :
		ThisSimpleString( StringTraits::GetDefaultManager() )
	{
	}
	explicit StringT( StringManagerBase* pStringMgr ) throw() :
		ThisSimpleString( pStringMgr )
	{ 
	}

	static void __cdecl Construct( StringT* pString )
	{
		new( pString ) StringT;
	}

	// Copy constructor
	StringT( const StringT& strSrc ) :
		ThisSimpleString( strSrc )
	{
	}

	// Construct from SimpleStringT
	operator SimpleStringT<BaseType >&()
	{
		return *(SimpleStringT<BaseType >*)this;
	}
	template <bool bMFCDLL>
	StringT( const SimpleStringT<BaseType, bMFCDLL>& strSrc ) :
		ThisSimpleString( strSrc )
	{
	}

	StringT( const XCHAR* pszSrc ) :
		ThisSimpleString( StringTraits::GetDefaultManager() )
	{
		// nDestLength is in XCHARs
		*this = pszSrc;
	}

	StringT( LPCSTR pszSrc, StringManagerBase* pStringMgr ) :
		ThisSimpleString( pStringMgr )
	{
		// nDestLength is in XCHARs
		*this = pszSrc;
	}

	explicit StringT( const YCHAR* pszSrc ) :
		ThisSimpleString( StringTraits::GetDefaultManager() )
	{
		*this = pszSrc;
	}

	StringT( LPCWSTR pszSrc, StringManagerBase* pStringMgr ) :
		ThisSimpleString( pStringMgr )
	{
		*this = pszSrc;
	}

	// This template will compile only for
	// class SystemString == System::String

	explicit StringT( const unsigned char* pszSrc ) :
		ThisSimpleString( StringTraits::GetDefaultManager() )
	{
		*this = reinterpret_cast< const char* >( pszSrc );
	}

	explicit StringT( char* pszSrc ) :
		ThisSimpleString( StringTraits::GetDefaultManager() )
	{
		*this = reinterpret_cast< const char *>( pszSrc );
	}

	explicit StringT( unsigned char* pszSrc ) :
		ThisSimpleString( StringTraits::GetDefaultManager() )
	{
		*this = reinterpret_cast< const char *>( pszSrc );
	}

	explicit StringT( wchar_t* pszSrc ) :
		ThisSimpleString( StringTraits::GetDefaultManager() )
	{
		*this = reinterpret_cast< const wchar_t* >( pszSrc );
	}

	StringT( const unsigned char* pszSrc, StringManagerBase* pStringMgr ) :
		ThisSimpleString( pStringMgr )
	{
		*this = reinterpret_cast< const char* >( pszSrc );
	}

	explicit StringT( char ch, int nLength = 1 ) :
		ThisSimpleString( StringTraits::GetDefaultManager() )
	{
		_ASSERT( nLength >= 0 );
		if( nLength > 0 )
		{
			PXSTR pszBuffer = GetBuffer( nLength );
			StringTraits::FloodCharacters( XCHAR( ch ), nLength, pszBuffer );
			ReleaseBufferSetLength( nLength );
		}
	}

	explicit StringT( wchar_t ch, int nLength = 1 ) :
		ThisSimpleString( StringTraits::GetDefaultManager() )
	{
		_ASSERT( nLength >= 0 );
		if( nLength > 0 )
		{			
			//Convert ch to the BaseType
			wchar_t pszCh[2] = { ch , 0 };
			int nBaseTypeCharLen = 1;

			if(ch != L'\0')
			{
				nBaseTypeCharLen = StringTraits::GetBaseTypeLength(pszCh);
			}

			CTempBuffer<XCHAR,10> buffBaseTypeChar;			
			buffBaseTypeChar.Allocate(nBaseTypeCharLen+1);
			StringTraits::ConvertToBaseType( buffBaseTypeChar, nBaseTypeCharLen+1, pszCh, 1 );
			//Allocate enough characters in String and flood (replicate) with the (converted character)*nLength
			PXSTR pszBuffer = GetBuffer( nLength*nBaseTypeCharLen );
			if (nBaseTypeCharLen == 1)
			{   //Optimization for a common case - wide char translates to 1 ansi/wide char.
				StringTraits::FloodCharacters( buffBaseTypeChar[0], nLength, pszBuffer );				
			} else
			{
				XCHAR* p=pszBuffer;
				for (int i=0 ; i < nLength ;++i)
				{
					for (int j=0 ; j < nBaseTypeCharLen ;++j)
					{	
						*p=buffBaseTypeChar[j];
						++p;
					}
				}
			}
			ReleaseBufferSetLength( nLength*nBaseTypeCharLen );			
		}
	}

	StringT( const XCHAR* pch, int nLength ) :
		ThisSimpleString( pch, nLength, StringTraits::GetDefaultManager() )
	{
	}

	StringT( const XCHAR* pch, int nLength, StringManagerBase* pStringMgr ) :
		ThisSimpleString( pch, nLength, pStringMgr )
	{
	}

	StringT( const YCHAR* pch, int nLength ) :
		ThisSimpleString( StringTraits::GetDefaultManager() )
	{
		_ASSERT( nLength >= 0 );
		if( nLength > 0 )
		{
			if(pch == NULL)
				_ASSERT(0);

			int nDestLength = StringTraits::GetBaseTypeLength( pch, nLength );
			PXSTR pszBuffer = GetBuffer( nDestLength );
			StringTraits::ConvertToBaseType( pszBuffer, nDestLength, pch, nLength );
			ReleaseBufferSetLength( nDestLength );
		}
	}

	StringT( const YCHAR* pch, int nLength, StringManagerBase* pStringMgr ) :
		ThisSimpleString( pStringMgr )
	{
		_ASSERT( nLength >= 0 );
		if( nLength > 0 )
		{
			if(pch == NULL)
				_ASSERT(0);

			int nDestLength = StringTraits::GetBaseTypeLength( pch, nLength );
			PXSTR pszBuffer = GetBuffer( nDestLength );
			StringTraits::ConvertToBaseType( pszBuffer, nDestLength, pch, nLength );
			ReleaseBufferSetLength( nDestLength );
		}
	}

	// Destructor
	~StringT() throw()
	{
	}

	// Assignment operators
	StringT& operator=( const StringT& strSrc )
	{
		ThisSimpleString::operator=( strSrc );

		return( *this );
	}
	
	template <bool bMFCDLL>
	StringT& operator=( const SimpleStringT<BaseType, bMFCDLL>& strSrc )
	{
		ThisSimpleString::operator=( strSrc );

		return( *this );
	}

	StringT& operator=( PCXSTR pszSrc )
	{
		ThisSimpleString::operator=( pszSrc );

		return( *this );
	}

	StringT& operator=( PCYSTR pszSrc )
	{
		// nDestLength is in XCHARs
		int nDestLength = (pszSrc != NULL) ? StringTraits::GetBaseTypeLength( pszSrc ) : 0;
		if( nDestLength > 0 )
		{
			PXSTR pszBuffer = GetBuffer( nDestLength );
			StringTraits::ConvertToBaseType( pszBuffer, nDestLength, pszSrc);
			ReleaseBufferSetLength( nDestLength );
		}
		else
		{
			Empty();
		}

		return( *this );
	}

	StringT& operator=( const unsigned char* pszSrc )
	{
		return( operator=( reinterpret_cast< const char* >( pszSrc ) ) );
	}

	StringT& operator=( char ch )
	{
		char ach[2] = { ch, 0 };

		return( operator=( ach ) );
	}

	StringT& operator=( wchar_t ch )
	{
		wchar_t ach[2] = { ch, 0 };

		return( operator=( ach ) );
	}

	StringT& operator+=( const ThisSimpleString& str )
	{
		ThisSimpleString::operator+=( str );

		return( *this );
	}
	template <bool bMFCDLL>
	StringT& operator+=( const SimpleStringT<BaseType, bMFCDLL>& str )
	{
		ThisSimpleString::operator+=( str );

		return( *this );
	}
	
	StringT& operator+=( PCXSTR pszSrc )
	{
		ThisSimpleString::operator+=( pszSrc );

		return( *this );
	}
	template< int t_nSize >
	StringT& operator+=( const StaticString< XCHAR, t_nSize >& strSrc )
	{
		ThisSimpleString::operator+=( strSrc );

		return( *this );
	}
	StringT& operator+=( PCYSTR pszSrc )
	{
		StringT str( pszSrc, GetManager() );

		return( operator+=( str ) );
	}

	StringT& operator+=( char ch )
	{
		ThisSimpleString::operator+=( ch );

		return( *this );
	}

	StringT& operator+=( unsigned char ch )
	{
		ThisSimpleString::operator+=( ch );

		return( *this );
	}

	StringT& operator+=( wchar_t ch )
	{
		ThisSimpleString::operator+=( ch );

		return( *this );
	}

	StringT& operator+=( const VARIANT& var );

	// Override from base class
	StringManagerBase* GetManager() const throw()
	{
		StringManagerBase* pStringMgr = ThisSimpleString::GetManager();
		if(pStringMgr) { return pStringMgr; }

		pStringMgr = StringTraits::GetDefaultManager();
		return pStringMgr->Clone();
	}

	// Comparison

	int Compare( PCXSTR psz ) const
	{
		return( StringTraits::StringCompare( GetString(), psz ) );
	}

	int CompareNoCase( PCXSTR psz ) const throw()
	{
		return( StringTraits::StringCompareIgnore( GetString(), psz ) );
	}

	int Collate( PCXSTR psz ) const throw()
	{
		_ASSERT(  psz );
		return( StringTraits::StringCollate( GetString(), psz ) );
	}

	int CollateNoCase( PCXSTR psz ) const throw()
	{
		_ASSERT( psz );
		return( StringTraits::StringCollateIgnore( GetString(), psz ) );
	}

	// Advanced manipulation

	// Delete 'nCount' characters, starting at index 'iIndex'
	int Delete( int iIndex, int nCount = 1 )
	{
		if( iIndex < 0 )
			iIndex = 0;
		
		if( nCount < 0 )
			nCount = 0;

		int nLength = GetLength();
		if(nCount + iIndex > nLength )
		{
			nCount = nLength-iIndex;
		}
		if( nCount > 0 )
		{
			int nNewLength = nLength-nCount;
			int nXCHARsToCopy = nLength-(iIndex+nCount)+1;
			PXSTR pszBuffer = GetBuffer();
			memmove_s( pszBuffer+iIndex, nXCHARsToCopy*sizeof( XCHAR ), 
				pszBuffer+iIndex+nCount, nXCHARsToCopy*sizeof( XCHAR ) );
			ReleaseBufferSetLength( nNewLength );
		}

		return( GetLength() );
	}

	// Insert character 'ch' before index 'iIndex'
	int Insert( int iIndex, XCHAR ch )
	{
		if( iIndex < 0 )
			iIndex = 0;
			
		if( iIndex > GetLength() )
		{
			iIndex = GetLength();
		}
		int nNewLength = GetLength()+1;

		PXSTR pszBuffer = GetBuffer( nNewLength );

		// move existing bytes down 
        memmove_s( pszBuffer+iIndex+1, (nNewLength-iIndex)*sizeof( XCHAR ), 
			pszBuffer+iIndex, (nNewLength-iIndex)*sizeof( XCHAR ) );
		pszBuffer[iIndex] = ch;

		ReleaseBufferSetLength( nNewLength );

		return( nNewLength );
	}

	// Insert string 'psz' before index 'iIndex'
	int Insert( int iIndex, PCXSTR psz )
	{
		if( iIndex < 0 )
			iIndex = 0;

		if( iIndex > GetLength() )
		{
			iIndex = GetLength();
		}

		// nInsertLength and nNewLength are in XCHARs
		int nInsertLength = StringTraits::SafeStringLen( psz );
		int nNewLength = GetLength();
		if( nInsertLength > 0 )
		{
			nNewLength += nInsertLength;

			PXSTR pszBuffer = GetBuffer( nNewLength );
			// move existing bytes down 
            memmove_s( pszBuffer+iIndex+nInsertLength, (nNewLength-iIndex-nInsertLength+1)*sizeof( XCHAR ), 
				pszBuffer+iIndex, (nNewLength-iIndex-nInsertLength+1)*sizeof( XCHAR ) );
			memcpy_s( pszBuffer+iIndex, nInsertLength*sizeof( XCHAR ), 
				psz, nInsertLength*sizeof( XCHAR ) );
			ReleaseBufferSetLength( nNewLength );
		}

		return( nNewLength );
	}

	// Replace all occurrences of character 'chOld' with character 'chNew'
	int Replace( XCHAR chOld, XCHAR chNew )
	{
		int nCount = 0;

		// short-circuit the nop case
		if( chOld != chNew )
		{
			// otherwise modify each character that matches in the string
			bool bCopied = false;
			PXSTR pszBuffer = const_cast< PXSTR >( GetString() );  // We don't actually write to pszBuffer until we've called GetBuffer().

			int nLength = GetLength();
			int iChar = 0;
			while( iChar < nLength )
			{
				// replace instances of the specified character only
				if( pszBuffer[iChar] == chOld )
				{
					if( !bCopied )
					{
						bCopied = true;
						pszBuffer = GetBuffer( nLength );
					}
					pszBuffer[iChar] = chNew;
					nCount++;
				}
				iChar = int( StringTraits::CharNext( pszBuffer+iChar )-pszBuffer );
			}
			if( bCopied )
			{
				ReleaseBufferSetLength( nLength );
			}
		}

		return( nCount );
	}

	// Replace all occurrences of string 'pszOld' with string 'pszNew'
	int Replace( PCXSTR pszOld, PCXSTR pszNew )
	{
		// can't have empty or NULL lpszOld

		// nSourceLen is in XCHARs
		int nSourceLen = StringTraits::SafeStringLen( pszOld );
		if( nSourceLen == 0 )
			return( 0 );
		// nReplacementLen is in XCHARs
		int nReplacementLen = StringTraits::SafeStringLen( pszNew );

		// loop once to figure out the size of the result string
		int nCount = 0;
		{
			PCXSTR pszStart = GetString();
			PCXSTR pszEnd = pszStart+GetLength();
			while( pszStart < pszEnd )
			{
				PCXSTR pszTarget;
				while( (pszTarget = StringTraits::StringFindString( pszStart, pszOld ) ) != NULL)
				{
					nCount++;
					pszStart = pszTarget+nSourceLen;
				}
				pszStart += StringTraits::SafeStringLen( pszStart )+1;
			}
		}

		// if any changes were made, make them
		if( nCount > 0 )
		{
			// if the buffer is too small, just
			//   allocate a new buffer (slow but sure)
			int nOldLength = GetLength();
			int nNewLength = nOldLength+(nReplacementLen-nSourceLen)*nCount;

			PXSTR pszBuffer = GetBuffer( __max( nNewLength, nOldLength ) );

			PXSTR pszStart = pszBuffer;
			PXSTR pszEnd = pszStart+nOldLength;

			// loop again to actually do the work
			while( pszStart < pszEnd )
			{
				PXSTR pszTarget;
				while( (pszTarget = StringTraits::StringFindString( pszStart, pszOld ) ) != NULL )
				{
					int nBalance = nOldLength-int(pszTarget-pszBuffer+nSourceLen);
                    memmove_s( pszTarget+nReplacementLen, nBalance*sizeof( XCHAR ), 
						pszTarget+nSourceLen, nBalance*sizeof( XCHAR ) );
					memcpy_s( pszTarget, nReplacementLen*sizeof( XCHAR ), 
						pszNew, nReplacementLen*sizeof( XCHAR ) );
					pszStart = pszTarget+nReplacementLen;
					pszTarget[nReplacementLen+nBalance] = 0;
					nOldLength += (nReplacementLen-nSourceLen);
				}
				pszStart += StringTraits::SafeStringLen( pszStart )+1;
			}
			_ASSERT( pszBuffer[nNewLength] == 0 );
			ReleaseBufferSetLength( nNewLength );
		}

		return( nCount );
	}

	// Remove all occurrences of character 'chRemove'
	int Remove( XCHAR chRemove )
	{
		int nLength = GetLength();
		PXSTR pszBuffer = GetBuffer( nLength );

		PXSTR pszSource = pszBuffer;
		PXSTR pszDest = pszBuffer;
		PXSTR pszEnd = pszBuffer+nLength;

		while( pszSource < pszEnd )
		{
			PXSTR pszNewSource = StringTraits::CharNext( pszSource );
			if( *pszSource != chRemove )
			{
				// Copy the source to the destination.  Remember to copy all bytes of an MBCS character
	   			// Copy the source to the destination.  Remember to copy all bytes of an MBCS character
				size_t NewSourceGap = (pszNewSource-pszSource);
				PXSTR pszNewDest = pszDest + NewSourceGap;
				size_t i = 0;
				for (i = 0;  pszDest != pszNewDest && i < NewSourceGap; i++)
				{
					*pszDest = *pszSource;
					pszSource++;
					pszDest++;
				}
			}
			pszSource = pszNewSource;
		}
		*pszDest = 0;
		int nCount = int( pszSource-pszDest );
		ReleaseBufferSetLength( nLength-nCount );

		return( nCount );
	}

	StringT Tokenize( PCXSTR pszTokens, int& iStart ) const
	{
		_ASSERT( iStart >= 0 );
			
		if(iStart < 0)
			_ASSERT(0);
			
		if( (pszTokens == NULL) || (*pszTokens == (XCHAR)0) )
		{
			if (iStart < GetLength())
			{
				return( StringT( GetString()+iStart, GetManager() ) );
			}
		}
		else
		{
			PCXSTR pszPlace = GetString()+iStart;
			PCXSTR pszEnd = GetString()+GetLength();
			if( pszPlace < pszEnd )
			{
				int nIncluding = StringTraits::StringSpanIncluding( pszPlace,
					pszTokens );

				if( (pszPlace+nIncluding) < pszEnd )
				{
					pszPlace += nIncluding;
					int nExcluding = StringTraits::StringSpanExcluding( pszPlace, pszTokens );

					int iFrom = iStart+nIncluding;
					int nUntil = nExcluding;
					iStart = iFrom+nUntil+1;

					return( Mid( iFrom, nUntil ) );
				}
			}
		}

		// return empty string, done tokenizing
		iStart = -1;

		return( StringT( GetManager() ) );
	}

	// find routines

	// Find the first occurrence of character 'ch', starting at index 'iStart'
	int Find( XCHAR ch, int iStart = 0 ) const throw()
	{
		// iStart is in XCHARs
		_ASSERT( iStart >= 0 );

		// nLength is in XCHARs
		int nLength = GetLength();
		if( iStart < 0 || iStart >= nLength)
		{
			return( -1 );
		}

		// find first single character
		PCXSTR psz = StringTraits::StringFindChar( GetString()+iStart, ch );

		// return -1 if not found and index otherwise
		return( (psz == NULL) ? -1 : int( psz-GetString() ) );
	}

	// look for a specific sub-string

	// Find the first occurrence of string 'pszSub', starting at index 'iStart'
	int Find( PCXSTR pszSub, int iStart = 0 ) const throw()
	{
		// iStart is in XCHARs
		_ASSERT( iStart >= 0 );
		_ASSERT( pszSub );

		if(pszSub == NULL)
		{
			return( -1 );
		}
		// nLength is in XCHARs
		int nLength = GetLength();
		if( iStart < 0 || iStart > nLength )
		{
			return( -1 );
		}

		// find first matching substring
		PCXSTR psz = StringTraits::StringFindString( GetString()+iStart, pszSub );

		// return -1 for not found, distance from beginning otherwise
		return( (psz == NULL) ? -1 : int( psz-GetString() ) );
	}

	// Find the first occurrence of any of the characters in string 'pszCharSet'
	int FindOneOf( PCXSTR pszCharSet ) const throw()
	{
		_ASSERT( pszCharSet );
		PCXSTR psz = StringTraits::StringScanSet( GetString(), pszCharSet );
		return( (psz == NULL) ? -1 : int( psz-GetString() ) );
	}

	// Find the last occurrence of character 'ch'
	int ReverseFind( XCHAR ch ) const throw()
	{
		// find last single character
		PCXSTR psz = StringTraits::StringFindCharRev( GetString(), ch );

		// return -1 if not found, distance from beginning otherwise
		return( (psz == NULL) ? -1 : int( psz-GetString() ) );
	}

	// manipulation

	// Convert the string to uppercase
	StringT& MakeUpper()
	{
		int nLength = GetLength();
		PXSTR pszBuffer = GetBuffer( nLength );
		StringTraits::StringUppercase( pszBuffer, nLength+1 );
		ReleaseBufferSetLength( nLength );

		return( *this );
	}

	// Convert the string to lowercase
	StringT& MakeLower()
	{
		int nLength = GetLength();
		PXSTR pszBuffer = GetBuffer( nLength );
		StringTraits::StringLowercase( pszBuffer, nLength+1 );
		ReleaseBufferSetLength( nLength );

		return( *this );
	}

	// Reverse the string
	StringT& MakeReverse()
	{
		int nLength = GetLength();
		PXSTR pszBuffer = GetBuffer( nLength );
		StringTraits::StringReverse( pszBuffer );
		ReleaseBufferSetLength( nLength );

		return( *this );
	}

	// trimming

	// Remove all trailing whitespace
	StringT& TrimRight()
	{
		// find beginning of trailing spaces by starting
		// at beginning (DBCS aware)

		PCXSTR psz = GetString();
		PCXSTR pszLast = NULL;

		while( *psz != 0 )
		{
			if( StringTraits::IsSpace( *psz ) )
			{
				if( pszLast == NULL )
					pszLast = psz;
			}
			else
			{
				pszLast = NULL;
			}
			psz = StringTraits::CharNext( psz );
		}

		if( pszLast != NULL )
		{
			// truncate at trailing space start
			int iLast = int( pszLast-GetString() );

			Truncate( iLast );
		}

		return( *this );
	}

	// Remove all leading whitespace
	StringT& TrimLeft()
	{
		// find first non-space character

		PCXSTR psz = GetString();

		while( StringTraits::IsSpace( *psz ) )
		{
			psz = StringTraits::CharNext( psz );
		}

		if( psz != GetString() )
		{
			// fix up data and length
			int iFirst = int( psz-GetString() );
			PXSTR pszBuffer = GetBuffer( GetLength() );
			psz = pszBuffer+iFirst;
			int nDataLength = GetLength()-iFirst;
            memmove_s( pszBuffer, (nDataLength+1)*sizeof( XCHAR ), 
				psz, (nDataLength+1)*sizeof( XCHAR ) );
			ReleaseBufferSetLength( nDataLength );
		}

		return( *this );
	}

	// Remove all leading and trailing whitespace
	StringT& Trim()
	{
		return( TrimRight().TrimLeft() );
	}

	// Remove all leading and trailing occurrences of character 'chTarget'
	StringT& Trim( XCHAR chTarget )
	{
		return( TrimRight( chTarget ).TrimLeft( chTarget ) );
	}

	// Remove all leading and trailing occurrences of any of the characters in the string 'pszTargets'
	StringT& Trim( PCXSTR pszTargets )
	{
		return( TrimRight( pszTargets ).TrimLeft( pszTargets ) );
	}

	// trimming anything (either side)

	// Remove all trailing occurrences of character 'chTarget'
	StringT& TrimRight( XCHAR chTarget )
	{
		// find beginning of trailing matches
		// by starting at beginning (DBCS aware)

		PCXSTR psz = GetString();
		PCXSTR pszLast = NULL;

		while( *psz != 0 )
		{
			if( *psz == chTarget )
			{
				if( pszLast == NULL )
				{
					pszLast = psz;
				}
			}
			else
			{
				pszLast = NULL;
			}
			psz = StringTraits::CharNext( psz );
		}

		if( pszLast != NULL )
		{
			// truncate at left-most matching character  
			int iLast = int( pszLast-GetString() );
			Truncate( iLast );
		}

		return( *this );
	}

	// Remove all trailing occurrences of any of the characters in string 'pszTargets'
	StringT& TrimRight( PCXSTR pszTargets )
	{
		// if we're not trimming anything, we're not doing any work
		if( (pszTargets == NULL) || (*pszTargets == 0) )
		{
			return( *this );
		}

		// find beginning of trailing matches
		// by starting at beginning (DBCS aware)

		PCXSTR psz = GetString();
		PCXSTR pszLast = NULL;

		while( *psz != 0 )
		{
			if( StringTraits::StringFindChar( pszTargets, *psz ) != NULL )
			{
				if( pszLast == NULL )
				{
					pszLast = psz;
				}
			}
			else
			{
				pszLast = NULL;
			}
			psz = StringTraits::CharNext( psz );
		}

		if( pszLast != NULL )
		{
			// truncate at left-most matching character  
			int iLast = int( pszLast-GetString() );
			Truncate( iLast );
		}

		return( *this );
	}

	// Remove all leading occurrences of character 'chTarget'
	StringT& TrimLeft( XCHAR chTarget )
	{
		// find first non-matching character
		PCXSTR psz = GetString();

		while( chTarget == *psz )
		{
			psz = StringTraits::CharNext( psz );
		}

		if( psz != GetString() )
		{
			// fix up data and length
			int iFirst = int( psz-GetString() );
			PXSTR pszBuffer = GetBuffer( GetLength() );
			psz = pszBuffer+iFirst;
			int nDataLength = GetLength()-iFirst;
            memmove_s( pszBuffer, (nDataLength+1)*sizeof( XCHAR ), 
				psz, (nDataLength+1)*sizeof( XCHAR ) );
			ReleaseBufferSetLength( nDataLength );
		}

		return( *this );
	}

	// Remove all leading occurrences of any of the characters in string 'pszTargets'
	StringT& TrimLeft( PCXSTR pszTargets )
	{
		// if we're not trimming anything, we're not doing any work
		if( (pszTargets == NULL) || (*pszTargets == 0) )
		{
			return( *this );
		}

		PCXSTR psz = GetString();
		while( (*psz != 0) && (StringTraits::StringFindChar( pszTargets, *psz ) != NULL) )
		{
			psz = StringTraits::CharNext( psz );
		}

		if( psz != GetString() )
		{
			// fix up data and length
			int iFirst = int( psz-GetString() );
			PXSTR pszBuffer = GetBuffer( GetLength() );
			psz = pszBuffer+iFirst;
			int nDataLength = GetLength()-iFirst;
            memmove_s( pszBuffer, (nDataLength+1)*sizeof( XCHAR ), 
				psz, (nDataLength+1)*sizeof( XCHAR ) );
			ReleaseBufferSetLength( nDataLength );
		}

		return( *this );
	}

	// Convert the string to the OEM character set
	//void AnsiToOem()
	//{
	//	int nLength = GetLength();
	//	PXSTR pszBuffer = GetBuffer( nLength );
	//	StringTraits::ConvertToOem( pszBuffer, nLength+1 );
	//	ReleaseBufferSetLength( nLength );
	//}

	// Convert the string to the ANSI character set
	//void OemToAnsi()
	//{
	//	int nLength = GetLength();
	//	PXSTR pszBuffer = GetBuffer( nLength );
	//	StringTraits::ConvertToAnsi( pszBuffer, nLength+1 );
	//	ReleaseBufferSetLength( nLength );
	//}

	// Very simple sub-string extraction

	// Return the substring starting at index 'iFirst'
	StringT Mid( int iFirst ) const
	{
		return( Mid( iFirst, GetLength()-iFirst ) );
	}

	// Return the substring starting at index 'iFirst', with length 'nCount'
	StringT Mid( int iFirst, int nCount ) const
	{
		// nCount is in XCHARs

		// out-of-bounds requests return sensible things
		if (iFirst < 0)
			iFirst = 0;
		if (nCount < 0)
			nCount = 0;

		if( iFirst + nCount > GetLength() )
		{
			nCount = GetLength()-iFirst;
		}
		if( iFirst > GetLength() )
		{
			nCount = 0;
		}

		_ASSERT( (nCount == 0) || ((iFirst+nCount) <= GetLength()) );

		// optimize case of returning entire string
		if( (iFirst == 0) && ((iFirst+nCount) == GetLength()) )
		{
			return( *this );
		}

		return( StringT( GetString()+iFirst, nCount, GetManager() ) );
	}

	// Return the substring consisting of the rightmost 'nCount' characters
	StringT Right( int nCount ) const
	{
		// nCount is in XCHARs
		if (nCount < 0)
			nCount = 0;

		int nLength = GetLength();
		if( nCount >= nLength )
		{
			return( *this );
		}

		return( StringT( GetString()+nLength-nCount, nCount, GetManager() ) );
	}

	// Return the substring consisting of the leftmost 'nCount' characters
	StringT Left( int nCount ) const
	{
		// nCount is in XCHARs
		if (nCount < 0)
			nCount = 0;

		int nLength = GetLength();
		if( nCount >= nLength )
		{
			return( *this );
		}

		return( StringT( GetString(), nCount, GetManager() ) );
	}

	// Return the substring consisting of the leftmost characters in the set 'pszCharSet'
	StringT SpanIncluding( PCXSTR pszCharSet ) const
	{
		_ASSERT( pszCharSet );
		if(pszCharSet == NULL)
			_ASSERT(0)

		return( Left( StringTraits::StringSpanIncluding( GetString(), pszCharSet ) ) );
	}

	// Return the substring consisting of the leftmost characters not in the set 'pszCharSet'
	StringT SpanExcluding( PCXSTR pszCharSet ) const
	{
		_ASSERT( pszCharSet );
		if(pszCharSet == NULL)
			_ASSERT(0);

		return( Left( StringTraits::StringSpanExcluding( GetString(), pszCharSet ) ) );
 	}

	// Format data using format string 'pszFormat'
	void __cdecl Format( PCXSTR pszFormat, ... );

	// Format data using format string loaded from resource 'nFormatID'
	void __cdecl Format( UINT nFormatID, ... );

	// Append formatted data using format string loaded from resource 'nFormatID'
	void __cdecl AppendFormat( UINT nFormatID, ... );

	// Append formatted data using format string 'pszFormat'
	void __cdecl AppendFormat( PCXSTR pszFormat, ... );
	void AppendFormatV( PCXSTR pszFormat, va_list args )
	{
		_ASSERT( pszFormat );

		int nCurrentLength = GetLength();
		int nAppendLength = StringTraits::GetFormattedLength( pszFormat, args );
		PXSTR pszBuffer = GetBuffer( nCurrentLength+nAppendLength );
        StringTraits::Format( pszBuffer+nCurrentLength, 
			nAppendLength+1, pszFormat, args );
		ReleaseBufferSetLength( nCurrentLength+nAppendLength );
	}

	void FormatV( PCXSTR pszFormat, va_list args )
	{
		if(pszFormat == NULL)
			_ASSERT(0);

		int nLength = StringTraits::GetFormattedLength( pszFormat, args );
		PXSTR pszBuffer = GetBuffer( nLength );
        StringTraits::Format( pszBuffer, nLength+1, pszFormat, args );
		ReleaseBufferSetLength( nLength );
	}

	// Format a message using format string 'pszFormat'
	void __cdecl FormatMessage( PCXSTR pszFormat, ... );

	// Format a message using format string 'pszFormat' and va_list
	//void FormatMessageV( PCXSTR pszFormat, va_list* pArgList )
	//{
	//	// format message into temporary buffer pszTemp
	//	CHeapPtr< XCHAR, CLocalAllocator > pszTemp;
	//	DWORD dwResult = StringTraits::_AFX_FUNCNAME(FormatMessage)( FORMAT_MESSAGE_FROM_STRING|
	//		FORMAT_MESSAGE_ALLOCATE_BUFFER, pszFormat, 0, 0, reinterpret_cast< PXSTR >( &pszTemp ),
	//		0, pArgList );
	//	if( dwResult == 0 )
	//	{
	//		ThrowMemoryException();
	//	}

	//	*this = pszTemp;
	//}


	// Set the string to the value of environment variable 'pszVar'
	BOOL GetEnvironmentVariable( PCXSTR pszVar )
	{
		ULONG nLength = StringTraits::GetEnvironmentVariable( pszVar, NULL, 0 );
		BOOL bRetVal = FALSE;

		if( nLength == 0 )
		{
			Empty();
		}
		else
		{
			PXSTR pszBuffer = GetBuffer( nLength );
			StringTraits::GetEnvironmentVariable( pszVar, pszBuffer, nLength );
			ReleaseBuffer();
			bRetVal = TRUE;
		}

		return( bRetVal );
	}

	friend StringT operator+( const StringT& str1, const StringT& str2 )
	{
		StringT strResult( str1.GetManager() );

		Concatenate( strResult, str1, str1.GetLength(), str2, str2.GetLength() );

		return( strResult );
	}

	friend StringT operator+( const StringT& str1, PCXSTR psz2 )
	{
		StringT strResult( str1.GetManager() );

		Concatenate( strResult, str1, str1.GetLength(), psz2, StringLength( psz2 ) );

		return( strResult );
	}

	friend StringT operator+( PCXSTR psz1, const StringT& str2 )
	{
		StringT strResult( str2.GetManager() );

		Concatenate( strResult, psz1, StringLength( psz1 ), str2, str2.GetLength() );

		return( strResult );
	}

	friend StringT operator+( const StringT& str1, wchar_t ch2 )
	{
		StringT strResult( str1.GetManager() );
		XCHAR chTemp = XCHAR( ch2 );

		Concatenate( strResult, str1, str1.GetLength(), &chTemp, 1 );

		return( strResult );
	}

	friend StringT operator+( const StringT& str1, char ch2 )
	{
		StringT strResult( str1.GetManager() );
		XCHAR chTemp = XCHAR( ch2 );

		Concatenate( strResult, str1, str1.GetLength(), &chTemp, 1 );

		return( strResult );
	}

	friend StringT operator+( wchar_t ch1, const StringT& str2 )
	{
		StringT strResult( str2.GetManager() );
		XCHAR chTemp = XCHAR( ch1 );

		Concatenate( strResult, &chTemp, 1, str2, str2.GetLength() );

		return( strResult );
	}

	friend StringT operator+( char ch1, const StringT& str2 )
	{
		StringT strResult( str2.GetManager() );
		XCHAR chTemp = XCHAR( ch1 );

		Concatenate( strResult, &chTemp, 1, str2, str2.GetLength() );

		return( strResult );
	}

	friend bool operator==( const StringT& str1, const StringT& str2 ) throw()
	{
		return( str1.Compare( str2 ) == 0 );
	}

	friend bool operator==(
		const StringT& str1, PCXSTR psz2 ) throw()
	{
		return( str1.Compare( psz2 ) == 0 );
	}

	friend bool operator==(
		PCXSTR psz1, const StringT& str2 ) throw()
	{
		return( str2.Compare( psz1 ) == 0 );
	}

	friend bool operator==(
		const StringT& str1, PCYSTR psz2 ) throw( ... )
	{
		StringT str2( psz2, str1.GetManager() );

		return( str1 == str2 );
	}

	friend bool operator==(
		PCYSTR psz1, const StringT& str2 ) throw( ... )
	{
		StringT str1( psz1, str2.GetManager() );

		return( str1 == str2 );
	}

	friend bool operator!=(
		const StringT& str1, const StringT& str2 ) throw()
	{
		return( str1.Compare( str2 ) != 0 );
	}

	friend bool operator!=(
		const StringT& str1, PCXSTR psz2 ) throw()
	{
		return( str1.Compare( psz2 ) != 0 );
	}

	friend bool operator!=(
		PCXSTR psz1, const StringT& str2 ) throw()
	{
		return( str2.Compare( psz1 ) != 0 );
	}

	friend bool operator!=(
		const StringT& str1, PCYSTR psz2 ) throw( ... )
	{
		StringT str2( psz2, str1.GetManager() );

		return( str1 != str2 );
	}

	friend bool operator!=(
		PCYSTR psz1, const StringT& str2 ) throw( ... )
	{
		StringT str1( psz1, str2.GetManager() );

		return( str1 != str2 );
	}

	friend bool operator<( const StringT& str1, const StringT& str2 ) throw()
	{
		return( str1.Compare( str2 ) < 0 );
	}

	friend bool operator<( const StringT& str1, PCXSTR psz2 ) throw()
	{
		return( str1.Compare( psz2 ) < 0 );
	}

	friend bool operator<( PCXSTR psz1, const StringT& str2 ) throw()
	{
		return( str2.Compare( psz1 ) > 0 );
	}

	friend bool operator>( const StringT& str1, const StringT& str2 ) throw()
	{
		return( str1.Compare( str2 ) > 0 );
	}

	friend bool operator>( const StringT& str1, PCXSTR psz2 ) throw()
	{
		return( str1.Compare( psz2 ) > 0 );
	}

	friend bool operator>( PCXSTR psz1, const StringT& str2 ) throw()
	{
		return( str2.Compare( psz1 ) < 0 );
	}

	friend bool operator<=( const StringT& str1, const StringT& str2 ) throw()
	{
		return( str1.Compare( str2 ) <= 0 );
	}

	friend bool operator<=( const StringT& str1, PCXSTR psz2 ) throw()
	{
		return( str1.Compare( psz2 ) <= 0 );
	}

	friend bool operator<=( PCXSTR psz1, const StringT& str2 ) throw()
	{
		return( str2.Compare( psz1 ) >= 0 );
	}

	friend bool operator>=( const StringT& str1, const StringT& str2 ) throw()
	{
		return( str1.Compare( str2 ) >= 0 );
	}

	friend bool operator>=( const StringT& str1, PCXSTR psz2 ) throw()
	{
		return( str1.Compare( psz2 ) >= 0 );
	}

	friend bool operator>=( PCXSTR psz1, const StringT& str2 ) throw()
	{
		return( str2.Compare( psz1 ) <= 0 );
	}

	friend bool operator==( XCHAR ch1, const StringT& str2 ) throw()
	{
		return( (str2.GetLength() == 1) && (str2[0] == ch1) );
	}

	friend bool operator==( const StringT& str1, XCHAR ch2 ) throw()
	{
		return( (str1.GetLength() == 1) && (str1[0] == ch2) );
	}

	friend bool operator!=( XCHAR ch1, const StringT& str2 ) throw()
	{
		return( (str2.GetLength() != 1) || (str2[0] != ch1) );
	}

	friend bool operator!=( const StringT& str1, XCHAR ch2 ) throw()
	{
		return( (str1.GetLength() != 1) || (str1[0] != ch2) );
	}

private:
};

#ifndef _CSTRING_BUFFER_SIZE
#define _CSTRING_BUFFER_SIZE(_CStringObj) ((_CStringObj).GetAllocLength() + 1)
#endif

#pragma warning(push)
#pragma warning(disable : 4793)
// Format data using format string 'pszFormat'
template< typename BaseType, class StringTraits >
inline void __cdecl StringT<BaseType, StringTraits>::Format( PCXSTR pszFormat, ... )
{
	_ASSERT( pszFormat );

	va_list argList;
	va_start( argList, pszFormat );
	FormatV( pszFormat, argList );
	va_end( argList );
}

// Append formatted data using format string 'pszFormat'
template< typename BaseType, class StringTraits >
inline void __cdecl StringT<BaseType, StringTraits>::AppendFormat( PCXSTR pszFormat, ... )
{
	va_list argList;
	va_start( argList, pszFormat );

	AppendFormatV( pszFormat, argList );

	va_end( argList );
}

template< typename T >
class StringElementTraits
{
public:
	typedef typename T::PCXSTR INARGTYPE;
	typedef T& OUTARGTYPE;

	static void __cdecl CopyElements( T* pDest, const T* pSrc,size_t nElements )
	{
		for( size_t iElement = 0; iElement < nElements; iElement++ )
		{
			pDest[iElement] = pSrc[iElement];
		}
	}

	static void __cdecl RelocateElements( T* pDest, T* pSrc,size_t nElements )
	{
		memmove_s( pDest, nElements*sizeof( T ), pSrc, nElements*sizeof( T ) );
	}

	static ULONG __cdecl Hash(INARGTYPE str )
	{
		ATLENSURE( str != NULL );
		ULONG nHash = 0;
		const T::XCHAR* pch = str;
		while( *pch != 0 )
		{
			nHash = (nHash<<5)+nHash+(*pch);
			pch++;
		}

		return( nHash );
	}

	static bool __cdecl CompareElements(INARGTYPE str1,INARGTYPE str2 )
	{
		return( T::StrTraits::StringCompare( str1, str2 ) == 0 );
	}

	static int __cdecl CompareElementsOrdered(INARGTYPE str1,INARGTYPE str2 )
	{
		return( T::StrTraits::StringCompare( str1, str2 ) );
	}
};

class StrTraitsA;
class StrTraitsW;

typedef StringT< wchar_t, StrTraitsW > StringW;
typedef StringT< char, StrTraitsA > StringA;
typedef StringA String;
//typedef StringT< TCHAR, StrTraitMFC< TCHAR > > String;

class StrTraitsA : public ChTraitsCRT< char >
{
public:
	static StringManagerBase* GetDefaultManager() throw()
	{
		return s_StringManager ? s_StringManager : InitializeManager();
	}

	//static StringA& GetEmptyString() throw()
	//{
	//	return s_EmptyString ? *s_EmptyString : InitializeEmptyString();
	//}

private:
	static StringManagerBase* InitializeManager(); // throw();
	//static StringA& InitializeEmptyString() throw();
	static StringManagerBase* s_StringManager;
	//static StringA* s_EmptyString;
};


class StrTraitsW : public ChTraitsCRT< wchar_t >
{
public:
	static StringManagerBase* GetDefaultManager() throw()
	{
		return s_StringManager ? s_StringManager : InitializeManager();
	}

	//static StringW& GetEmptyString() throw()
	//{
	//	return s_EmptyString ? *s_EmptyString : InitializeEmptyString();
	//}

private:
	static StringManagerBase* InitializeManager();// throw();
	//static StringW& InitializeEmptyString() throw();
	static StringManagerBase* s_StringManager;
	//static StringW* s_EmptyString;
};

}	// namespace Helix


#endif //  HSTRING_H