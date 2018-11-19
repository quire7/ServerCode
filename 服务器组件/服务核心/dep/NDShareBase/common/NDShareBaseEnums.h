#ifndef __SHARE_BASE_ND_SHARE_BASE_ENUMS_H__
#define __SHARE_BASE_ND_SHARE_BASE_ENUMS_H__


//�߳�״̬ö��;
enum EThreadStatus
{
	EThreadStatus_invalid,		//��Ч;
	EThreadStatus_ready,		//��ǰ�̴߳���׼��״̬;
	EThreadStatus_running,		//��ǰ�̴߳�������״̬;
	EThreadStatus_exiting,		//��ǰ�̴߳��������˳�״̬;
	EThreadStatus_exit,			//��ǰ�̴߳����˳�״̬;
};

//IO�¼�ö��;
enum EIOEventType
{
	EIOEventType_Invalid	= 0x00,		//��Ч;
	EIOEventType_Read		= 0x01,		//���¼�;
	EIOEventType_Write		= 0x02,		//д�¼�;
	EIOEventType_Close		= 0x04,		//�ر��¼�;
};

enum E_NDSlotFunctorBase
{
	E_NDSlotFunctorBase_RUN		= 0x01,
	E_NDSlotFunctorBase_SUSPEND	= 0x02,
	E_NDSlotFunctorBase_DELETE	= 0x04,
};

//Session(socket)���ӶϿ�����ö��;
enum ESessionDisconnectionType
{
	ESessionDisconnectionType_NULL						= 0x00,
	ESessionDisconnectionType_PACKET_ERROR_ACTIVE		= 0x01,		//����������,�����Ͽ�;
	ESessionDisconnectionType_PING_BEYOND_TIME_ACTIVE	= 0x02,		//PING����ʱ,�����Ͽ�;
	ESessionDisconnectionType_READ_PASSIVE				= 0x04,		//IO���¼�ʱ����,�����Ͽ�;
	ESessionDisconnectionType_SEND_PASSIVE				= 0x08,		//IO���¼�ʱ����,�����Ͽ�;
};

//���ݿ��������ö��;
enum EDataSourceOperType
{
	EDataSourceOperType_NULL,
	EDataSourceOperType_SELECT,		
	EDataSourceOperType_INSERT,
	EDataSourceOperType_UPDATE,
	EDataSourceOperType_DELETE,
	EDataSourceOperType_MAX,
};


#endif

