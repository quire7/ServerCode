#ifndef __SHARE_BASE_ND_SHARE_BASE_ENUMS_H__
#define __SHARE_BASE_ND_SHARE_BASE_ENUMS_H__


//线程状态枚举;
enum EThreadStatus
{
	EThreadStatus_invalid,		//无效;
	EThreadStatus_ready,		//当前线程处于准备状态;
	EThreadStatus_running,		//当前线程处于运行状态;
	EThreadStatus_exiting,		//当前线程处于正在退出状态;
	EThreadStatus_exit,			//当前线程处于退出状态;
};

//IO事件枚举;
enum EIOEventType
{
	EIOEventType_Invalid	= 0x00,		//无效;
	EIOEventType_Read		= 0x01,		//读事件;
	EIOEventType_Write		= 0x02,		//写事件;
	EIOEventType_Close		= 0x04,		//关闭事件;
};

enum E_NDSlotFunctorBase
{
	E_NDSlotFunctorBase_RUN		= 0x01,
	E_NDSlotFunctorBase_SUSPEND	= 0x02,
	E_NDSlotFunctorBase_DELETE	= 0x04,
};

//Session(socket)连接断开类型枚举;
enum ESessionDisconnectionType
{
	ESessionDisconnectionType_NULL						= 0x00,
	ESessionDisconnectionType_PACKET_ERROR_ACTIVE		= 0x01,		//解析包错误,主动断开;
	ESessionDisconnectionType_PING_BEYOND_TIME_ACTIVE	= 0x02,		//PING包超时,主动断开;
	ESessionDisconnectionType_READ_PASSIVE				= 0x04,		//IO读事件时出错,被动断开;
	ESessionDisconnectionType_SEND_PASSIVE				= 0x08,		//IO读事件时出错,被动断开;
};

//数据库基本操作枚举;
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

