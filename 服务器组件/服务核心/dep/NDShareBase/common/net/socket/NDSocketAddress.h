/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\net\socket\NDSocketAddress.h
	file base:	NDSocketAddress
	purpose:	socket address;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_SOCKET_ADDRESS_H__
#define __SHARE_BASE_ND_SOCKET_ADDRESS_H__

#include "NDTypes.h"
#include "NDShareBaseMacros.h"


_NDSHAREBASE_BEGIN

struct NDSocketAddress
{
private:
	NDUint16	m_nPort;
	char		m_szIP[ND_IP_LENGTH];

public:
	NDSocketAddress()
	{ clear(); }

	NDSocketAddress( const NDSocketAddress& other ) : m_nPort( other.m_nPort )
	{
		memcpy( m_szIP, other.m_szIP, ND_IP_LENGTH );
		m_szIP[ND_IP_LENGTH - 1] = '\0';
	}

	NDSocketAddress( const char* pStrIP, NDUint16 nPort ) : m_nPort( nPort )
	{
		setIP( pStrIP );
	}

	NDSocketAddress& operator = (const NDSocketAddress& other)
	{
		if ( this == &other ) return *this;

		m_nPort	= other.m_nPort;

		memcpy( m_szIP, other.m_szIP, ND_IP_LENGTH );
		m_szIP[ND_IP_LENGTH - 1] = '\0';

		return *this;
	}

	NDBool	equal( const NDSocketAddress& refNetAddr ) const
	{
		NDBool bRet = NDFalse;

		if( m_nPort == refNetAddr.m_nPort )
		{
			if ( 0 == strncmp( m_szIP, refNetAddr.m_szIP, ND_IP_LENGTH ) )
			{
				bRet = NDTrue;
			}
		}
		
		return bRet;
	}

	void	setPort( NDUint16 nPort ) { m_nPort = nPort; }
	NDBool	setIP( const char* pStrIP )	  
	{ 
		if ( NULL == pStrIP || '\0' == pStrIP[0] ) return NDFalse;

		NDUint32 nLen = (NDUint32)strlen( pStrIP );
		if ( nLen >= ND_IP_LENGTH ) 
		{
			nLen = ( ND_IP_LENGTH - 1 );
		}

		memcpy( m_szIP, pStrIP, nLen );
		m_szIP[nLen] = '\0';

		return NDTrue;
	}

	NDUint16		getPort() const { return m_nPort; }
	const char*		getIP()   const { return m_szIP; }

	NDBool			isValidAddr() const
	{
		return ( ( NULL != m_szIP ) && ( '\0' != m_szIP[0] ) && ( 0 != m_nPort ) );
	}
	void clear()
	{
		m_nPort		= 0;
		memset( m_szIP, 0, ND_IP_LENGTH );
	}

};

_NDSHAREBASE_END
#endif

