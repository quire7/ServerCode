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

//Sharememory�ڴ�����ͷ�Ľṹ;
struct NDSMHead
{
	NDUint16		m_nOwnType;			//����ӵ���߱��;
	NDSM_KEY		m_nKey;				//��ֵ;
	NDUint32		m_nSize;			//�ܴ�С;
	NDUint32		m_nHeadVer;			//�����̰汾;
	NDUint32		m_nUnitPosition;	//��Ԫʹ����Ŀ(������Ӧ�ô�������,Ϊ��ʹ�÷���);
	NDUint32		m_nUnitCapacity;	//��Ԫ�����Ŀ(������Ӧ�ô�������,Ϊ��ʹ�÷���);
};

//Sharememory��Ԫӵ���ߵ�����;
enum eNDSMU_OWN_TYPE
{
	eNDSMU_OWN_TYPE_FREE,				//Sharememory��Ԫ����;
	eNDSMU_OWN_TYPE_SELF,				//Sharememory��Ԫ�������Լ�;
};

//Sharememory��Ԫʹ��״̬;
enum eNDSMU_USE_STATE
{
	eNDSMU_USE_STATE_FREE,				//����״̬;
	eNDSMU_USE_STATE_HOLDDATA,			//��ռ��״̬;
	eNDSMU_USE_STATE_READFREE,			//׼���ͷ�״̬;
	eNDSMU_USE_STATE_FREEED,			//�ͷ�״̬��;
};

//Sharememory��Ԫͷ�Ľṹ;
struct NDSMUHead
{
	NDUint8			m_nUseStatus;		//�õ�Ԫ��ǰʹ��״̬;
	NDUint16		m_nOwnType;			//����ӵ���߱��;
	NDUint32		m_nPoolID;			//��Ԫ���е�ID;
	NDTime			m_nSaveTime;		//�洢ʱ��(����ʱ��(UTC),��ȷ����);
};


enum eNDSMKEY
{
	eNDSMKEY_LOG = 1,
};


#endif 


