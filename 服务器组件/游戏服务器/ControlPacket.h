#ifndef CONTROL_PACKET_HEAD_FILE
#define CONTROL_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////

//�¼����;
#define ER_FAILURE					0									//ʧ�ܱ�־;
#define ER_SUCCESS					1									//�ɹ���־;

//////////////////////////////////////////////////////////////////////////////////
//�������ݰ�;

#define UI_CORRESPOND_RESULT		1									//Э�����;
#define UI_SERVICE_CONFIG_RESULT	2									//���ý��;
#define UI_SERVICE_CONTROL_REQUEST	3									//�������;

//////////////////////////////////////////////////////////////////////////////////
//�������ݰ�;

#define CT_CONNECT_CORRESPOND		100									//����Э��;
#define CT_LOAD_SERVICE_CONFIG		101									//��������;
#define CT_SERVICE_CONTROL			102									//�������;
//////////////////////////////////////////////////////////////////////////////////

//���ƽ��;
struct CP_ControlResult
{
	BYTE							cbSuccess;							//�ɹ���־;
};

enum eServiceControlRequest
{
	eServiceControlRequest_Invalid,										//��Ч��;
	eServiceControlRequest_RunParameter,								//���в���;
};

//������Ƶ�����;
struct CP_ServiceControlRequest 
{
	BYTE							cbServiceControlRequestType;		//��������;
	DWORD							dwUserID;							//�û���ʶ;
	SCORE							lParam1;							//����1;
	SCORE							lParam2;							//����2;
};
//////////////////////////////////////////////////////////////////////////////////

#endif