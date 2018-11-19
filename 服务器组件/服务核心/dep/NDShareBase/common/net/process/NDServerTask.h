/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\net\process\NDServerTask.h
	file base:	NDServerTask
	purpose:	server task;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_SERVER_TASK_H__
#define __SHARE_BASE_ND_SERVER_TASK_H__


#include "memory/NDMessageQueue.h"

_NDSHAREBASE_BEGIN

class NDByteBuffer;
class NDProcess;
class NDServerTask
{
private:
	typedef	NDMessageQueue< NDByteBuffer* >		MsgBufQueue;

	NDProcess*									m_pProcess;
	MsgBufQueue									m_msgDeque;

	static	NDServerTask*						m_pNDServerTask;

public:
	static NDServerTask* getInstance();
	static void releaseInstance();

	~NDServerTask(void);

	NDBool		setProcessor( NDProcess* pProcess );

	void		putQueue( NDByteBuffer* pMsgBuf );
	void		getQueue( NDByteBuffer*& rpMsgBuf );
	
	void		taskProcess();

protected:
	NDServerTask(void);

	void		disposeMessage( NDByteBuffer& rMsgBuf );

private:
	NDServerTask(const NDServerTask& other);
	NDServerTask& operator = (const NDServerTask& other);

};

_NDSHAREBASE_END

#endif

