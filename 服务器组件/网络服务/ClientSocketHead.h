#ifndef WH_SOCKET_HEAD_FILE
#define WH_SOCKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////

//平台文件
#include "..\..\全局定义\Platform.h"
#include "..\服务核心\ServiceCoreHead.h"

//////////////////////////////////////////////////////////////////////////////////
//公共定义

//导出定义
#ifndef YS_SOCKET_CLASS
	#ifdef  YS_SOCKET_DLL
		#define YS_SOCKET_CLASS _declspec(dllexport)
	#else
		#define YS_SOCKET_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifndef _DEBUG
	#define YS_SOCKET_DLL_NAME		TEXT("ClientSocket.dll")				//组件名字
#else
	#define YS_SOCKET_DLL_NAME		TEXT("ClientSocketD.dll")				//组件名字
#endif

//////////////////////////////////////////////////////////////////////////////////
//公共定义

//关闭原因
#define SHUT_REASON_INSIDE			0									//内部原因
#define SHUT_REASON_NORMAL			1									//正常关闭
#define SHUT_REASON_REMOTE			2									//远程关闭
#define SHUT_REASON_TIME_OUT		3									//网络超时
#define SHUT_REASON_EXCEPTION		4									//异常关闭

//////////////////////////////////////////////////////////////////////////////////
//公共定义

//网络状态
#define SOCKET_STATUS_IDLE			0									//空闲状态
#define SOCKET_STATUS_WAIT			1									//等待状态
#define SOCKET_STATUS_CONNECT		2									//连接状态

//连接错误
#define CONNECT_SUCCESS				0									//连接成功
#define CONNECT_FAILURE				1									//连接失败
#define CONNECT_EXCEPTION			2									//参数异常
#define CONNECT_PROXY_FAILURE		3									//连接成功
#define CONNECT_PROXY_USER_INVALID	4									//用户无效

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IClientSocket INTERFACE_VERSION(1,1)
	static const GUID IID_IClientSocket = { 0xa870d592, 0x447a, 0x40bd, 0x00ae, 0x4e, 0x5e, 0x64, 0xde, 0xce, 0x22, 0xed };
#else
	#define VER_IClientSocket INTERFACE_VERSION(1,1)
	static const GUID IID_IClientSocket={0x8dfd434a,0xfb21,0x4178,0x00b0,0x3b,0xe6,0x32,0x9f,0x0b,0x5b,0x2e};
#endif

//网络接口
interface IClientSocket : public IUnknownEx
{
	//配置接口
public:
	//设置标识
	virtual VOID SetSocketID(WORD wSocketID)=NULL;
	//设置接口
	virtual bool SetTCPSocketSink(IUnknownEx * pIUnknownEx)=NULL;

	//信息接口
public:
	//网络状态
	virtual BYTE GetSocketStatus()=NULL;
	//发送间隔
	virtual DWORD GetLastSendTick()=NULL;
	//接收间隔
	virtual DWORD GetLastRecvTick()=NULL;

	//操作接口
public:
	//关闭连接
	virtual VOID CloseSocket()=NULL;
	//连接操作
	virtual BYTE Connect(DWORD dwServerIP, WORD wPort)=NULL;
	//连接操作
	virtual BYTE Connect(LPCTSTR pszServerIP, WORD wPort)=NULL;
	//发送函数
	virtual WORD SendData(WORD wMainCmdID, WORD wSubCmdID)=NULL;
	//发送函数
	virtual WORD SendData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IClientSocketSink INTERFACE_VERSION(1,1)
	static const GUID IID_IClientSocketSink = { 0xdb0f09dd, 0x65e4, 0x441f, 0x0094, 0xb1, 0xfe, 0x80, 0x8c, 0x1d, 0xf2, 0x65 };
#else
	#define VER_IClientSocketSink INTERFACE_VERSION(1,1)
	static const GUID IID_IClientSocketSink={0x231416e8,0x74b0,0x472d,0x009e,0xc3,0x4a,0x22,0xe5,0x21,0x09,0x37};
#endif

//网络接口
interface IClientSocketSink : public IUnknownEx
{
	//网络事件
public:
	//连接事件
	virtual bool OnEventClientSocketLink(WORD wSocketID, INT nErrorCode) = NULL;
	//关闭事件
	virtual bool OnEventClientSocketShut(WORD wSocketID, BYTE cbShutReason) = NULL;
	//读取事件
	virtual bool OnEventClientSocketRead(WORD wSocketID, TCP_Command Command, VOID * pData, WORD wDataSize) = NULL;
};

//////////////////////////////////////////////////////////////////////////////////

DECLARE_MODULE_HELPER(ClientSocket, YS_SOCKET_DLL_NAME, "CreateClientSocket")

//////////////////////////////////////////////////////////////////////////////////

#endif