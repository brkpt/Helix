#ifndef HSIMPSTR_H
#define HSIMPSTR_H

// ********
// InterlockedIncrement is in Windows.h
// ********

//#include <atldef.h>
//#include <atlcore.h>
//#include <atlexcept.h>
//#include <atlmem.h>

namespace Helix
{
struct StringData;

class StringMgr
{
public:
	// Allocate a new StringData
	StringData* Allocate( int nAllocLength, int nCharSize ) throw();
	// Free an existing StringData
	void Free( StringData* pData ) throw();
	// Change the size of an existing StringData
	StringData* Reallocate( StringData* pData, int nAllocLength, int nCharSize ) throw();
	// Get the StringData for a Nil string
	StringData* GetNilString() throw();
	StringMgr* Clone() throw();
};

struct StringData
{
	StringMgr* pStringMgr;  // String manager for this StringData
	int nDataLength;  // Length of currently used data in XCHARs (not including terminating null)
	int nAllocLength;  // Length of allocated data in XCHARs (not including terminating null)
	long nRefs;     // Reference count: negative == locked
	// XCHAR data[nAllocLength+1]  // A StringData is always followed in memory by the actual array of character data

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

class NilStringData :
	public StringData
{
public:
	NilStringData() throw()
	{
		pStringMgr = NULL;
		nRefs = 2;  // Never gets freed by StringMgr
		nDataLength = 0;
		nAllocLength = 0;
		achNil[0] = 0;
		achNil[1] = 0;
	}

	void SetManager( StringMgr* pMgr ) throw()
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
	StaticString( const BaseType* psz ) :
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

#define _ST( psz ) Helix::StaticString< TCHAR, sizeof( _T( psz ) ) >( _T( psz ) )
#define _SA( psz ) Helix::StaticString< char, sizeof( psz ) >( psz )
#define _SW( psz ) Helix::StaticString< wchar_t, sizeof( L##psz ) >( L##psz )
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

template< typename TCharType, bool t_bMFCDLL = false >
class StrBufT;

template< typename BaseType , bool t_bMFCDLL = false>
class CSimpleStringT
{
public:
	typedef typename ChTraitsBase< BaseType >::XCHAR XCHAR;
	typedef typename ChTraitsBase< BaseType >::PXSTR PXSTR;
	typedef typename ChTraitsBase< BaseType >::PCXSTR PCXSTR;
	typedef typename ChTraitsBase< BaseType >::YCHAR YCHAR;
	typedef typename ChTraitsBase< BaseType >::PYSTR PYSTR;
	typedef typename ChTraitsBase< BaseType >::PCYSTR PCYSTR;

public:
	explicit CSimpleStringT( StringMgr* pStringMgr )
	{
		_ASSERT(pStringMgr != NULL);
		StringData* pData = pStringMgr->GetNilString();
		Attach( pData );
	}
	
	CSimpleStringT( const CSimpleStringT& strSrc )
	{
		StringData* pSrcData = strSrc.GetData();
		StringData* pNewData = CloneData( pSrcData );
		Attach( pNewData );
	}
	
	CSimpleStringT( const CSimpleStringT<BaseType, !t_bMFCDLL>& strSrc )
	{
		StringData* pSrcData = strSrc.GetData();
		StringData* pNewData = CloneData( pSrcData );
		Attach( pNewData );
	}
	
	CSimpleStringT( PCXSTR pszSrc, StringMgr* pStringMgr )
	{
		ATLENSURE( pStringMgr != NULL );

		int nLength = StringLength( pszSrc );
		StringData* pData = pStringMgr->Allocate( nLength, sizeof( XCHAR ) );
		if( pData == NULL )
		{
			ThrowMemoryException();
		}
		Attach( pData );
		SetLength( nLength );
        CopyChars( m_pszData, nLength, pszSrc, nLength );
	}
	CSimpleStringT( const XCHAR* pchSrc, int nLength, StringMgr* pStringMgr )
	{
		_ASSERT( pStringMgr != NULL );
		
		if(pchSrc == NULL && nLength != 0)
			_ASSERT(0);

		StringData* pData = pStringMgr->Allocate( nLength, sizeof( XCHAR ) );
		if( pData == NULL )
		{
			ThrowMemoryException();
		}
		Attach( pData );
		SetLength( nLength );
        CopyChars( m_pszData, nLength, pchSrc, nLength );
	}
	~CSimpleStringT() throw()
	{
		StringData* pData = GetData();
		pData->Release();
	}
	
	operator CSimpleStringT<BaseType, !t_bMFCDLL>&()
	{
		return *(CSimpleStringT<BaseType, !t_bMFCDLL>*)this;
	}

	CSimpleStringT& operator=( const CSimpleStringT& strSrc )
	{
		StringData* pSrcData = strSrc.GetData();
		StringData* pOldData = GetData();
		if( pSrcData != pOldData)
		{
			if( pOldData->IsLocked() || pSrcData->pStringMgr != pOldData->pStringMgr )
			{
				SetString( strSrc.GetString(), strSrc.GetLength() );
			}
			else
			{
				StringData* pNewData = CloneData( pSrcData );
				pOldData->Release();
				Attach( pNewData );
			}
		}

		return( *this );
	}
	
	CSimpleStringT& operator=( const CSimpleStringT<BaseType, !t_bMFCDLL>& strSrc )
	{
		StringData* pSrcData = strSrc.GetData();
		StringData* pOldData = GetData();
		if( pSrcData != pOldData)
		{
			if( pOldData->IsLocked() || pSrcData->pStringMgr != pOldData->pStringMgr )
			{
				SetString( strSrc.GetString(), strSrc.GetLength() );
			}
			else
			{
				StringData* pNewData = CloneData( pSrcData );
				pOldData->Release();
				Attach( pNewData );
			}
		}

		return( *this );
	}
	
	CSimpleStringT& operator=( PCXSTR pszSrc )
	{
		SetString( pszSrc );

		return( *this );
	}

	CSimpleStringT& operator+=( const CSimpleStringT& strSrc )
	{
		Append( strSrc );

		return( *this );
	}
	template <bool bMFCDLL>
	CSimpleStringT& operator+=( const CSimpleStringT<BaseType, bMFCDLL>& strSrc )
	{
		Append( strSrc );

		return( *this );
	}
	
	CSimpleStringT& operator+=( PCXSTR pszSrc )
	{
		Append( pszSrc );

		return( *this );
	}
	template< int t_nSize >
	CSimpleStringT& operator+=( const StaticString< XCHAR, t_nSize >& strSrc )
	{
		Append( static_cast<const XCHAR *>(strSrc), strSrc.GetLength() );

		return( *this );
	}
	CSimpleStringT& operator+=( char ch )
	{
		AppendChar( XCHAR( ch ) );

		return( *this );
	}
	CSimpleStringT& operator+=( unsigned char ch )
	{
		AppendChar( XCHAR( ch ) );

		return( *this );
	}
	CSimpleStringT& operator+=( wchar_t ch )
	{
		AppendChar( XCHAR( ch ) );

		return( *this );
	}

	XCHAR operator[]( int iChar ) const
	{
		_ASSERT( (iChar >= 0) && (iChar <= GetLength()) );  // Indexing the '\0' is OK
		
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
	void Append( const CSimpleStringT& strSrc )
	{
		Append( strSrc.GetString(), strSrc.GetLength() );
	}
	template <bool bMFCDLL>
	void Append( const CSimpleStringT<BaseType, bMFCDLL>& strSrc )
	{
		Append( strSrc.GetString(), strSrc.GetLength() );
	}	
	void Empty() throw()
	{
		StringData* pOldData = GetData();
		StringMgr* pStringMgr = pOldData->pStringMgr;
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
			StringData* pNewData = pStringMgr->GetNilString();
			Attach( pNewData );
		}
	}
	void FreeExtra()
	{
		StringData* pOldData = GetData();
		int nLength = pOldData->nDataLength;
		StringMgr* pStringMgr = pOldData->pStringMgr;
		if( pOldData->nAllocLength == nLength )
		{
			return;
		}

		if( !pOldData->IsLocked() )  // Don't reallocate a locked buffer that's shrinking
		{
			StringData* pNewData = pStringMgr->Allocate( nLength, sizeof( XCHAR ) );
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
			
		return( m_pszData[iChar] );
	}
	PXSTR GetBuffer()
	{
		StringData* pData = GetData();
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
	StringMgr* GetManager() const throw()
	{
		StringMgr* pStringMgr = GetData()->pStringMgr;
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
		StringData* pData = GetData();
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
		StringData* pData = GetData();
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

		int nLength = GetLength();
		PXSTR pszBuffer = GetBuffer();
		pszBuffer[iChar] = ch;
		ReleaseBufferSetLength( nLength );
			
	}
	void SetManager( StringMgr* pStringMgr )
	{
		_ASSERT( IsEmpty() );

		StringData* pData = GetData();
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
				//AtlThrow(E_INVALIDARG);			
				return;
				
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
	friend CSimpleStringT operator+(
		const CSimpleStringT& str1,
		const CSimpleStringT& str2 )
	{
		CSimpleStringT s( str1.GetManager() );

		Concatenate( s, str1, str1.GetLength(), str2, str2.GetLength() );

		return( s );
	}

	friend CSimpleStringT operator+(
		const CSimpleStringT& str1,
		PCXSTR psz2 )
	{
		CSimpleStringT s( str1.GetManager() );

		Concatenate( s, str1, str1.GetLength(), psz2, StringLength( psz2 ) );

		return( s );
	}

	friend CSimpleStringT operator+(
		PCXSTR psz1,
		const CSimpleStringT& str2 )
	{
		CSimpleStringT s( str2.GetManager() );

		Concatenate( s, psz1, StringLength( psz1 ), str2, str2.GetLength() );

		return( s );
	}

	static void __cdecl CopyChars( XCHAR* pchDest, const XCHAR* pchSrc, int nChars ) throw()
	{
		#pragma warning (push)
		#pragma warning(disable : 4996)
		memcpy( pchDest, pchSrc, nChars*sizeof( XCHAR ) );
		#pragma warning (pop)
	}
	static void __cdecl CopyChars( XCHAR* pchDest, size_t nDestLen, const XCHAR* pchSrc, int nChars ) throw()
	{
		memcpy_s( pchDest, nDestLen*sizeof( XCHAR ), pchSrc, nChars*sizeof( XCHAR ) );
	}

	static void __cdecl CopyCharsOverlapped( XCHAR* pchDest, const XCHAR* pchSrc, int nChars ) throw()
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

	static int __cdecl StringLength( __in_z_opt const char* psz ) throw()
	{
		if( psz == NULL )
		{
			return( 0 );
		}
		return( int( strlen( psz ) ) );
	}
	static int __cdecl StringLength( __in_z_opt const wchar_t* psz ) throw()
	{
		if( psz == NULL )
		{
			return( 0 );
		}
		return( int( wcslen( psz ) ) );
	}

protected:
	static void __cdecl Concatenate( CSimpleStringT& strResult, PCXSTR psz1, int nLength1, PCXSTR psz2, int nLength2 )
	{
		int nNewLength = nLength1+nLength2;
		PXSTR pszBuffer = strResult.GetBuffer( nNewLength );
        CopyChars( pszBuffer, nLength1, psz1, nLength1 );
		CopyChars( pszBuffer+nLength1, nLength2, psz2, nLength2 );
		strResult.ReleaseBufferSetLength( nNewLength );
	}
	__declspec( noreturn ) static void __cdecl ThrowMemoryException()
	{
		//AtlThrow( E_OUTOFMEMORY );
		_ASSERT(0);
	}

// Implementation
private:
	void Attach( StringData* pData ) throw()
	{
		m_pszData = static_cast< PXSTR >( pData->data() );
	}
	void Fork( int nLength )
	{
		StringData* pOldData = GetData();
		int nOldLength = pOldData->nDataLength;
		StringData* pNewData = pOldData->pStringMgr->Clone()->Allocate( nLength, sizeof( XCHAR ) );
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
	StringData* GetData() const throw()
	{
		return( reinterpret_cast< StringData* >( m_pszData )-1 );
	}
	PXSTR PrepareWrite( int nLength )
	{
		StringData* pOldData = GetData();
		int nShared = 1-pOldData->nRefs;  // nShared < 0 means true, >= 0 means false
		int nTooShort = pOldData->nAllocLength-nLength;  // nTooShort < 0 means true, >= 0 means false
		if( (nShared|nTooShort) < 0 )  // If either sign bit is set (i.e. either is less than zero), we need to copy data
		{
			PrepareWrite2( nLength );
		}

		return( m_pszData );
	}
	void PrepareWrite2( int nLength )
	{
		StringData* pOldData = GetData();
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
	void Reallocate( int nLength )
	{
		StringData* pOldData = GetData();
		_ASSERT( pOldData->nAllocLength < nLength );
		StringMgr* pStringMgr = pOldData->pStringMgr;
                if ( pOldData->nAllocLength >= nLength || nLength <= 0)
                {
			ThrowMemoryException();
                        return;
                }
		StringData* pNewData = pStringMgr->Reallocate( pOldData, nLength, sizeof( XCHAR ) );
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

		GetData()->nDataLength = nLength;
		m_pszData[nLength] = 0;
	}

	static StringData* __cdecl CloneData( StringData* pData )
	{
		StringData* pNewData = NULL;

		StringMgr* pNewStringMgr = pData->pStringMgr->Clone();
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
	
	friend class CSimpleStringT<BaseType, !t_bMFCDLL>;
};

template< typename TCharType, bool t_bMFCDLL >
class StrBufT
{
public:
	typedef CSimpleStringT< TCharType, t_bMFCDLL> StringType;
	typedef typename StringType::XCHAR XCHAR;
	typedef typename StringType::PXSTR PXSTR;
	typedef typename StringType::PCXSTR PCXSTR;

	static const DWORD AUTO_LENGTH = 0x01;  // Automatically determine the new length of the string at release.  The string must be null-terminated.
	static const DWORD SET_LENGTH = 0x02;  // Set the length of the string object at GetBuffer time

public:
	explicit StrBufT( StringType& str ) throw( ... ) :
		m_str( str ),
		m_pszBuffer( NULL ),
#ifdef _DEBUG
		m_nBufferLength( str.GetLength() ),
#endif
		m_nLength( str.GetLength() )
	{
		m_pszBuffer = m_str.GetBuffer();
	}
	
	StrBufT( StringType& str, int nMinLength, DWORD dwFlags = AUTO_LENGTH ) throw( ... ) :
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

	void SetLength( int nLength )
	{
		_ASSERT( nLength >= 0 );		
		_ASSERT( nLength <= m_nBufferLength );

		if( nLength < 0 )
			_ASSERT(0);
			//AtlThrow(E_INVALIDARG);
		
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

typedef CSimpleStringT< TCHAR > SimpleString;
typedef CSimpleStringT< char > SimpleStringA;
typedef CSimpleStringT< wchar_t > SimpleStringW;
typedef StrBufT< TCHAR > CStrBuf;
typedef StrBufT< char > CStrBufA;
typedef StrBufT< wchar_t > CStrBufW;

}; // namespace Helix

#endif  // HSIMPSTR_H
