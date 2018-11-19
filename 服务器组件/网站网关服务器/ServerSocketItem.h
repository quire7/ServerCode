#pragma once

#include "..\内核引擎\KernelEngineHead.h"
#include "..\网络服务\ClientSocketHead.h"
#include "ParseData.h"

#define VERIFY_KEY "123456"

enum {
	eWebNull,			//无效
	eWebBuyProp,		//购买道具
	eWebPropUpdate,		//道具成功
	eWebUserInfoChange,	//用户信息改变
	eWebSomeAttrChange,	//玩家某个属性变化

	eWebSysteMessage = 100,	//系统消息
};


#define BUY_PROP_VERIFY_ERROR	10	//购买检验失败
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
	//发送结果
	bool SendResult(LPCTSTR pstrFormat, ...);
	//执行请求
	bool ExecuteRequest();
	//购买道具请求
	bool WebBuyPropRequest(json::Value& val);


protected:
	CParseData	m_ParseData;
	IClientSocket* m_pClientSocket;
};
