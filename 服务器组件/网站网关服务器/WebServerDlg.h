#pragma once

#include "IDAllocator.h"
#include "ByteBuffer.h"
#include "IOCPServer.h"
#include "ServerSocketItem.h"

#include <afxtempl.h>
#include "..\网络服务\ClientSocketHead.h"


class CWebServerDlg : public CDialog, public IClientSocketSink, public CIOCPServer
{
public:
	CWebServerDlg();
	~CWebServerDlg();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID * QueryInterface(const IID & Guid, DWORD dwQueryVer);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnInitDialog();
	//取消函数
	virtual void OnCancel();
	//命令处理
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	virtual bool OnEventClientSocketLink(WORD wServiceID, INT nErrorCode);
	//网络读取消息
	virtual bool OnEventClientSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize);
	//网络关闭消息
	virtual bool OnEventClientSocketShut(WORD wServiceID, BYTE cbShutReason);

	virtual CTCPSocket* onCreateTCPSocket(WORD nClient, WORD socket, DWORD dwClientAddr)
	{
		return new CServerSocketItem(nClient, socket, dwClientAddr, this, m_ClientSocket.GetInterface());
	}

	void AddTraceLine(LPCTSTR pstrFormat, ...);

public:
	CString		m_strCenterAddr;
	int			m_nCenterPort;
	int			m_nOpenConnections;

protected:
	int			m_nPort;
	BOOL		m_bAutoStart;
	BOOL		m_bInitialized;

	BOOL		m_bRunning;
	HICON		m_hIcon;

	CClientSocketHelper			m_ClientSocket;						//网络连接
	CTraceServiceControl		m_TraceServiceControl;				//追踪窗口

	afx_msg void OnServerStart();
	afx_msg void OnServerStop();
	afx_msg void OnServerExit();

	afx_msg void OnClose();
	afx_msg void OnStart();
	afx_msg void OnStop();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);

	DECLARE_MESSAGE_MAP()
};
