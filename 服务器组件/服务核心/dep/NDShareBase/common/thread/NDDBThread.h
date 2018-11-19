#ifndef __SHARE_BASE_ND_DBTHREAD_H__
#define __SHARE_BASE_ND_DBTHREAD_H__

#include <vector>
using std::vector;

#include "database/NDMysqlConst.h"
#include "NDThread.h"

_NDSHAREBASE_BEGIN

class NDMysqlBaseOp;
class NDDBThread : public NDThread
{
protected:
	NDUint32			m_nDBThreadID;			//DBThread�±�;
	NDMysqlBaseOp*		m_pNDMysqlBaseOp;		//���ݿ��������;
	NDTime				m_nSleepMSTime;			//�߳�SLEEP������;

public:
	NDDBThread();
	virtual ~NDDBThread();

	void				run();
	void				stop();

	NDBool				init( NDUint32 nDBThreadID, NDTime nSleepMSTime, const NDMysqlConnParam& refMysqlConnParam );
	void				release();

	virtual NDBool		disposeOperateData() = 0;					//��û���ͬ�����ݵ����ݿ��е�����;
	virtual NDBool		isOperateDataEmpty() = 0;					//����Ƿ�����Ҫͬ�������ݿ��е�����;
	virtual NDBool		isQuitDBThread() = 0;						//�Ƿ��˳�DBThread;
};

typedef vector<NDDBThread*>				NDDBThreadPtrVec;
typedef NDDBThreadPtrVec::iterator		NDDBThreadPtrVecIter;

_NDSHAREBASE_END
#endif

