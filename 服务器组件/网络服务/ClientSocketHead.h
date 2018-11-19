#ifndef WH_SOCKET_HEAD_FILE
#define WH_SOCKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////

//ƽ̨�ļ�
#include "..\..\ȫ�ֶ���\Platform.h"
#include "..\�������\ServiceCoreHead.h"

//////////////////////////////////////////////////////////////////////////////////
//��������

//��������
#ifndef YS_SOCKET_CLASS
	#ifdef  YS_SOCKET_DLL
		#define YS_SOCKET_CLASS _declspec(dllexport)
	#else
		#define YS_SOCKET_CLASS _declspec(dllimport)
	#endif
#endif

//ģ�鶨��
#ifndef _DEBUG
	#define YS_SOCKET_DLL_NAME		TEXT("ClientSocket.dll")				//�������
#else
	#define YS_SOCKET_DLL_NAME		TEXT("ClientSocketD.dll")				//�������
#endif

//////////////////////////////////////////////////////////////////////////////////
//��������

//�ر�ԭ��
#define SHUT_REASON_INSIDE			0									//�ڲ�ԭ��
#define SHUT_REASON_NORMAL			1									//�����ر�
#define SHUT_REASON_REMOTE			2									//Զ�̹ر�
#define SHUT_REASON_TIME_OUT		3									//���糬ʱ
#define SHUT_REASON_EXCEPTION		4									//�쳣�ر�

//////////////////////////////////////////////////////////////////////////////////
//��������

//����״̬
#define SOCKET_STATUS_IDLE			0									//����״̬
#define SOCKET_STATUS_WAIT			1									//�ȴ�״̬
#define SOCKET_STATUS_CONNECT		2									//����״̬

//���Ӵ���
#define CONNECT_SUCCESS				0									//���ӳɹ�
#define CONNECT_FAILURE				1									//����ʧ��
#define CONNECT_EXCEPTION			2									//�����쳣
#define CONNECT_PROXY_FAILURE		3									//���ӳɹ�
#define CONNECT_PROXY_USER_INVALID	4									//�û���Ч

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IClientSocket INTERFACE_VERSION(1,1)
	static const GUID IID_IClientSocket = { 0xa870d592, 0x447a, 0x40bd, 0x00ae, 0x4e, 0x5e, 0x64, 0xde, 0xce, 0x22, 0xed };
#else
	#define VER_IClientSocket INTERFACE_VERSION(1,1)
	static const GUID IID_IClientSocket={0x8dfd434a,0xfb21,0x4178,0x00b0,0x3b,0xe6,0x32,0x9f,0x0b,0x5b,0x2e};
#endif

//����ӿ�
interface IClientSocket : public IUnknownEx
{
	//���ýӿ�
public:
	//���ñ�ʶ
	virtual VOID SetSocketID(WORD wSocketID)=NULL;
	//���ýӿ�
	virtual bool SetTCPSocketSink(IUnknownEx * pIUnknownEx)=NULL;

	//��Ϣ�ӿ�
public:
	//����״̬
	virtual BYTE GetSocketStatus()=NULL;
	//���ͼ��
	virtual DWORD GetLastSendTick()=NULL;
	//���ռ��
	virtual DWORD GetLastRecvTick()=NULL;

	//�����ӿ�
public:
	//�ر�����
	virtual VOID CloseSocket()=NULL;
	//���Ӳ���
	virtual BYTE Connect(DWORD dwServerIP, WORD wPort)=NULL;
	//���Ӳ���
	virtual BYTE Connect(LPCTSTR pszServerIP, WORD wPort)=NULL;
	//���ͺ���
	virtual WORD SendData(WORD wMainCmdID, WORD wSubCmdID)=NULL;
	//���ͺ���
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

//����ӿ�
interface IClientSocketSink : public IUnknownEx
{
	//�����¼�
public:
	//�����¼�
	virtual bool OnEventClientSocketLink(WORD wSocketID, INT nErrorCode) = NULL;
	//�ر��¼�
	virtual bool OnEventClientSocketShut(WORD wSocketID, BYTE cbShutReason) = NULL;
	//��ȡ�¼�
	virtual bool OnEventClientSocketRead(WORD wSocketID, TCP_Command Command, VOID * pData, WORD wDataSize) = NULL;
};

//////////////////////////////////////////////////////////////////////////////////

DECLARE_MODULE_HELPER(ClientSocket, YS_SOCKET_DLL_NAME, "CreateClientSocket")

//////////////////////////////////////////////////////////////////////////////////

#endif