/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\net\socket\NDSelect.h
	file base:	NDSelect
	purpose:	select socket event;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_SELECT_H__
#define __SHARE_BASE_ND_SELECT_H__


#include "NDTypes.h"

#ifdef WIN32
#include <WinSock2.h>
#pragma warning(disable:4127)
#pragma warning(disable:4244)
#else
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>


#endif

_NDSHAREBASE_BEGIN

class NDSelect
{
private:
	enum
	{
		SELECT_BAK = 0,		//当前系统中拥有的完整句柄数据;
		SELECT_USE = 1,		//用于select调用的句柄数据;
		SELECT_MAX = 2,		//结构使用数量;
	};

	SOCKET				m_nMaxFD;
	timeval				m_timeout[SELECT_MAX];

	fd_set				m_readFDs[SELECT_MAX];
	fd_set				m_writeFDs[SELECT_MAX];

public:
	NDSelect();
	~NDSelect();

	NDBool				setMaxFD( SOCKET fd );

	NDBool				registerReadWriteEvent( SOCKET fd );
	NDBool				unregisterReadWriteEvent( SOCKET fd );	//调用过此函数，记得重新设置最大SOCKET句柄;

	//run函数实际执行SELECT操作;
	//run函数return true, refNum返回多少个SOCKET处于就绪状态;
	//run函数return false, refNum返回错误码;
	NDBool				run( NDUint32& refNum );

	NDBool				isReadEvent( SOCKET fd );
	NDBool				isWriteEvent( SOCKET fd );

	//优化READ_WRITE_EVENT,在需要发送的时候才注册WRITE_EVENT;
	NDBool				registerWriteEvent( SOCKET fd );
	NDBool				unregisterWriteEvent( SOCKET fd );
};



_NDSHAREBASE_END
#endif

