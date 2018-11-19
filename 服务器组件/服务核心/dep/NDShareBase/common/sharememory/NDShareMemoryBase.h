#ifndef __SHARE_BASE_ND_SHARE_MEMORY_BASE_H__
#define __SHARE_BASE_ND_SHARE_MEMORY_BASE_H__


#if defined(WIN32)
typedef	void*					NDSMHandle;
#define	INVALID_SM_HANDLE		((void*)0)
#else
typedef	int						NDSMHandle;
#define INVALID_SM_HANDLE		(-1)
#endif

#ifndef NDSM_KEY
typedef unsigned long	NDSM_KEY;
#endif

//Sharememory内存区域头的结构;
struct NDSMHead
{
	NDUint16		m_nOwnType;			//进程拥有者标记;
	NDSM_KEY		m_nKey;				//键值;
	NDUint32		m_nSize;			//总大小;
	NDUint32		m_nHeadVer;			//最后存盘版本;
	NDUint32		m_nUnitPosition;	//单元使用数目(本来不应该存在这里,为了使用方面);
	NDUint32		m_nUnitCapacity;	//单元最大数目(本来不应该存在这里,为了使用方面);
};

//Sharememory单元拥有者的类型;
enum eNDSMU_OWN_TYPE
{
	eNDSMU_OWN_TYPE_FREE,				//Sharememory单元空闲;
	eNDSMU_OWN_TYPE_SELF,				//Sharememory单元创建者自己;
};

//Sharememory单元使用状态;
enum eNDSMU_USE_STATE
{
	eNDSMU_USE_STATE_FREE,				//空闲状态;
	eNDSMU_USE_STATE_HOLDDATA,			//被占用状态;
	eNDSMU_USE_STATE_READFREE,			//准备释放状态;
	eNDSMU_USE_STATE_FREEED,			//释放状态中;
};

//Sharememory单元头的结构;
struct NDSMUHead
{
	NDUint8			m_nUseStatus;		//该单元当前使用状态;
	NDUint16		m_nOwnType;			//进程拥有者标记;
	NDUint32		m_nPoolID;			//单元池中的ID;
	NDTime			m_nSaveTime;		//存储时间(绝对时间(UTC),精确到秒);
};


enum eNDSMKEY
{
	eNDSMKEY_LOG = 1,
};


#endif 


