/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\function\NDRefCounted.h
	file base:	NDRefCounted
	purpose:	reference count;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_REF_COUNTED_H__
#define __SHARE_BASE_ND_REF_COUNTED_H__

#include "NDTypes.h"
#include "NDShareBaseMacros.h"

_NDSHAREBASE_BEGIN

/* NDRefCounted template isn't thread safe */
template<typename T>
class NDRefCounted
{
public:
	NDRefCounted() : m_pObject( NULL ), m_pCount( 0 ) {}
	NDRefCounted( T* pObject ) : m_pObject( pObject ), m_pCount( (NULL !=pObject) ? (new NDUint32(1)) : 0  ) {}
	NDRefCounted( const NDRefCounted<T>& other ) 
		: m_pObject( other.m_pObject ), m_pCount( other.m_pCount )
	{
		if ( NULL != m_pCount )
			AddRef();
	}

	~NDRefCounted()
	{
		if ( NULL != m_pObject )
			releaseRef();
	}

	NDRefCounted<T>& operator = ( const NDRefCounted<T>& other )
	{
		if (this != &other)
		{
			if ( NULL != m_pObject)
				releaseRef();

			m_pObject = other.m_pObject;
			m_pCount  = (NULL != m_pObject) ? other.m_pCount : 0;

			if ( NULL != m_pCount )
				AddRef();
		}

		return *this;
	}

	bool operator == ( const NDRefCounted<T>& other ) const
	{	
		return (m_pObject == other.m_pObject);
	}

	bool operator != ( const NDRefCounted<T>& other ) const
	{	
		return !(*this == other);
	}

	bool isvaild() const
	{
		return ( NULL != m_pObject );
	}

	const T& operator * () const
	{
		return (*m_pObject);
	}

	T& operator * ()
	{
		return (*m_pObject);
	}

	const T* operator -> () const
	{
		return m_pObject;
	}

	T* operator -> ()
	{
		return m_pObject;
	}

private:
	void AddRef()
	{
		++(*m_pCount);
	}
	
	void releaseRef()
	{
		if ( (--(*m_pCount)) == 0 )
		{
			SAFE_DELETE( m_pObject );
			SAFE_DELETE( m_pCount );
		}
	}

private:
	T*				m_pObject;
	NDUint32*		m_pCount;
};	

_NDSHAREBASE_END
#endif
