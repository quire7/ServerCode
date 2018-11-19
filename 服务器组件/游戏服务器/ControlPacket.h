#ifndef CONTROL_PACKET_HEAD_FILE
#define CONTROL_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////

//事件结果;
#define ER_FAILURE					0									//失败标志;
#define ER_SUCCESS					1									//成功标志;

//////////////////////////////////////////////////////////////////////////////////
//界面数据包;

#define UI_CORRESPOND_RESULT		1									//协调结果;
#define UI_SERVICE_CONFIG_RESULT	2									//配置结果;
#define UI_SERVICE_CONTROL_REQUEST	3									//服务控制;

//////////////////////////////////////////////////////////////////////////////////
//控制数据包;

#define CT_CONNECT_CORRESPOND		100									//连接协调;
#define CT_LOAD_SERVICE_CONFIG		101									//加载配置;
#define CT_SERVICE_CONTROL			102									//服务控制;
//////////////////////////////////////////////////////////////////////////////////

//控制结果;
struct CP_ControlResult
{
	BYTE							cbSuccess;							//成功标志;
};

enum eServiceControlRequest
{
	eServiceControlRequest_Invalid,										//无效的;
	eServiceControlRequest_RunParameter,								//运行参数;
};

//服务控制的请求;
struct CP_ServiceControlRequest 
{
	BYTE							cbServiceControlRequestType;		//请求类型;
	DWORD							dwUserID;							//用户标识;
	SCORE							lParam1;							//参数1;
	SCORE							lParam2;							//参数2;
};
//////////////////////////////////////////////////////////////////////////////////

#endif