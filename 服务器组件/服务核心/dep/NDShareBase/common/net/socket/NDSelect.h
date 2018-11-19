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
		SELECT_BAK = 0,		//��ǰϵͳ��ӵ�е������������;
		SELECT_USE = 1,		//����select���õľ������;
		SELECT_MAX = 2,		//�ṹʹ������;
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
	NDBool				unregisterReadWriteEvent( SOCKET fd );	//���ù��˺������ǵ������������SOCKET���;

	//run����ʵ��ִ��SELECT����;
	//run����return true, refNum���ض��ٸ�SOCKET���ھ���״̬;
	//run����return false, refNum���ش�����;
	NDBool				run( NDUint32& refNum );

	NDBool				isReadEvent( SOCKET fd );
	NDBool				isWriteEvent( SOCKET fd );

	//�Ż�READ_WRITE_EVENT,����Ҫ���͵�ʱ���ע��WRITE_EVENT;
	NDBool				registerWriteEvent( SOCKET fd );
	NDBool				unregisterWriteEvent( SOCKET fd );
};



_NDSHAREBASE_END
#endif

