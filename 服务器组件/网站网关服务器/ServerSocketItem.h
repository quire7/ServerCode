#pragma once

#include "..\�ں�����\KernelEngineHead.h"
#include "..\�������\ClientSocketHead.h"
#include "ParseData.h"

#define VERIFY_KEY "123456"

enum {
	eWebNull,			//��Ч
	eWebBuyProp,		//�������
	eWebPropUpdate,		//���߳ɹ�
	eWebUserInfoChange,	//�û���Ϣ�ı�
	eWebSomeAttrChange,	//���ĳ�����Ա仯

	eWebSysteMessage = 100,	//ϵͳ��Ϣ
};


#define BUY_PROP_VERIFY_ERROR	10	//�������ʧ��
#define BUY_PROP_SUCCESSED		11

class CIOCPServer;

class CServerSocketItem : public CTCPSocket
{
public:
	CServerSocketItem(WORD id, WORD socket, DWORD dwClientAddr, CIOCPServer* pIOCPServer, IClientSocket* pClientSocket);
	virtual ~CServerSocketItem();

public:
	virtual bool processRecvData(void* data, DWORD len);

	bool Close();

	bool HttpResponse(LPCTSTR lpszMessage);
	//���ͽ��
	bool SendResult(LPCTSTR pstrFormat, ...);
	//ִ������
	bool ExecuteRequest();
	//�����������
	bool WebBuyPropRequest(json::Value& val);


protected:
	CParseData	m_ParseData;
	IClientSocket* m_pClientSocket;
};
