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
	NDUint32			m_nDBThreadID;			//DBThread下标;
	NDMysqlBaseOp*		m_pNDMysqlBaseOp;		//数据库操作对象;
	NDTime				m_nSleepMSTime;			//线程SLEEP毫秒数;

public:
	NDDBThread();
	virtual ~NDDBThread();

	void				run();
	void				stop();

	NDBool				init( NDUint32 nDBThreadID, NDTime nSleepMSTime, const NDMysqlConnParam& refMysqlConnParam );
	void				release();

	virtual NDBool		disposeOperateData() = 0;					//获得或者同步数据到数据库中的数据;
	virtual NDBool		isOperateDataEmpty() = 0;					//检查是否还有需要同步到数据库中的数据;
	virtual NDBool		isQuitDBThread() = 0;						//是否退出DBThread;
};

typedef vector<NDDBThread*>				NDDBThreadPtrVec;
typedef NDDBThreadPtrVec::iterator		NDDBThreadPtrVecIter;

_NDSHAREBASE_END
#endif

