/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\database\NDMysqlConst.h
	file base:	NDMysqlConst
	purpose:	mysql param;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SERVERSHARE_ND_MYSQL_CONST_H__
#define __SERVERSHARE_ND_MYSQL_CONST_H__


#include "NDTypes.h"

_NDSHAREBASE_BEGIN

struct NDMysqlConnParam
{
private:
	NDUint32						m_nPort;									//端口;
	string							m_strHost;									//主机名或IP;
	string							m_strUser;									//用户名;
	string							m_strPassWord;								//密码;
	string							m_strDBName;								//数据库名;

public:
	NDMysqlConnParam() : m_nPort(0) {}
	NDMysqlConnParam( NDUint32 nPort, const string& strHost, const string& strUser, const string& strPassWord, const string& strDBName ):
	m_nPort(nPort), m_strHost(strHost), m_strUser(strUser), m_strPassWord(strPassWord), m_strDBName(strDBName) {}
	NDMysqlConnParam( const NDMysqlConnParam& other ) : m_nPort(other.m_nPort), m_strHost(other.m_strHost), m_strUser(other.m_strUser),
													m_strPassWord(other.m_strPassWord), m_strDBName(other.m_strDBName) {}
	NDMysqlConnParam& operator = ( const NDMysqlConnParam& other )
	{
		if ( this == &other )	return *this;
		
		m_nPort			= other.m_nPort;
		m_strHost		= other.m_strHost;
		m_strUser		= other.m_strUser;
		m_strPassWord	= other.m_strPassWord;
		m_strDBName		= other.m_strDBName;

		return *this;
	}

	void			setPort(NDUint32 nPort)				{ m_nPort = nPort; }
	void			setHostName(const string& strHost)	{ m_strHost = strHost; }
	void			setUserName(const string& strUser)	{ m_strUser = strUser; }
	void			setPassWord(const string& passWord)	{ m_strPassWord = passWord; }
	void			setDBName(const string& strDBName)	{ m_strDBName = strDBName; }
	
	NDUint32		getPort() const { return m_nPort; }
	const string&	getHostName()	{ return m_strHost; }
	const string&	getUserName()	{ return m_strUser; }
	const string&	getPassWord()	{ return m_strPassWord; }
	const string&	getDBName()		{ return m_strDBName; }
};

_NDSHAREBASE_END

#endif
