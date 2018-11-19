#pragma once

#include "IDAllocator.h"
#include "ByteBuffer.h"
#include "IOCPServer.h"
#include "ServerSocketItem.h"

#include <afxtempl.h>
#include "..\�������\ClientSocketHead.h"


class CWebServerDlg : public CDialog, public IClientSocketSink, public CIOCPServer
{
public:
	CWebServerDlg();
	~CWebServerDlg();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(const IID & Guid, DWORD dwQueryVer);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnInitDialog();
	//ȡ������
	virtual void OnCancel();
	//�����
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	virtual bool OnEventClientSocketLink(WORD wServiceID, INT nErrorCode);
	//�����ȡ��Ϣ
	virtual bool OnEventClientSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize);
	//����ر���Ϣ
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

	CClientSocketHelper			m_ClientSocket;						//��������
	CTraceServiceControl		m_TraceServiceControl;				//׷�ٴ���

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
