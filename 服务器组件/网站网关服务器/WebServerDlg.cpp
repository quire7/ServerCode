#include "stdafx.h"
#include "WebServer.h"
#include "WebServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define  IDI_RECONNECT_CENTER	10001

//�˵�����
#define IDM_CLEAR_ALL				(WM_USER+201)						//ɾ����Ϣ
#define IDM_SELECT_ALL				(WM_USER+202)						//ȫ��ѡ��
#define IDM_COPY_STRING				(WM_USER+203)						//�����ַ�
#define IDM_SAVE_STRING				(WM_USER+205)						//������Ϣ
#define IDM_DELETE_STRING			(WM_USER+204)						//ɾ���ַ�

CWebServerDlg::CWebServerDlg() : CDialog(IDD_SERV_DIALOG), m_nOpenConnections(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bRunning = FALSE;
	m_bInitialized = FALSE;
}

CWebServerDlg::~CWebServerDlg()
{
	//KillTimer(IDI_RECONNECT_CENTER);

	//�ر�����
	if (m_ClientSocket.GetInterface()!=NULL)
	{
		m_ClientSocket->CloseSocket();
	}
}

void CWebServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_SERVICE_EVENT, m_TraceServiceControl);
}

//�ӿڲ�ѯ
VOID * CWebServerDlg::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IClientSocketSink, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IClientSocketSink, Guid, dwQueryVer);
	return NULL;
}

BEGIN_MESSAGE_MAP(CWebServerDlg, CDialog)
	ON_COMMAND(ID_SERVER_START, OnServerStart)
	ON_COMMAND(ID_SERVER_STOP, OnServerStop)
	ON_COMMAND(ID_SERVER_EXIT, OnServerExit)
	ON_WM_CLOSE()

	ON_BN_CLICKED(IDC_START_SERVICE, OnStart)
	ON_BN_CLICKED(IDC_STOP_SERVICE, OnStop)
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL CWebServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//���ñ���
	SetWindowText(TEXT("��ҳ���ط����� -- [ ֹͣ ]"));

	// Set big icon
	SetIcon(m_hIcon, TRUE);			
	// Set small icon
	SetIcon(m_hIcon, FALSE);		
	
	//��ȡĿ¼
	TCHAR szPath[MAX_PATH]=TEXT("");
	GetCurrentDirectory(sizeof(szPath),szPath);

	//��ȡ����
	TCHAR szFileName[MAX_PATH] = {0};
	_tprintf_s(szFileName,sizeof(szFileName),TEXT("%s\\ServerParameter.ini"), szPath);

	CString strTag = TEXT("WebGate");
	//�Ƿ��Զ�����
	m_bAutoStart = (GetPrivateProfileInt(strTag, TEXT("AutoStart"), 0, szFileName)>0);
	//����˿�
	m_nPort = GetPrivateProfileInt(strTag, TEXT("ServicePort"), PORT_WEB_GATE, szFileName);
	//���ķ�������ַ
	TCHAR szAddr[128]={0};
	::GetPrivateProfileString(strTag, TEXT("CorrespondAddr"), TEXT("127.0.0.1"), szAddr, sizeof(szAddr), szFileName);

	m_strCenterAddr = szAddr;
	//���ķ������˿�
	m_nCenterPort = GetPrivateProfileInt(strTag, TEXT("CorrespondPort"), PORT_CENTER, szFileName);

	if ( !m_bAutoStart )
	{
		LPCTSTR pszCmdLine=AfxGetApp()->m_lpCmdLine;
		if (pszCmdLine[0]!=0)
		{
			m_bAutoStart=TRUE;
		}
	}

	if (m_bAutoStart)
	{
		PostMessage(WM_COMMAND, ID_SERVER_START);
	}

	//�����������
	if (!m_ClientSocket.CreateInstance() )
	{
		ASSERT(FALSE);
		AddTraceLine(m_ClientSocket.m_szDescribe);

		return false;
	}

	IUnknownEx * pIUnknow = QUERY_ME_INTERFACE(IUnknownEx);
	if (!m_ClientSocket->SetTCPSocketSink(pIUnknow)) return false;

	m_bInitialized = TRUE;

	return TRUE;
}

void CWebServerDlg::OnCancel()
{
	//ѯ���˳�
	if ( m_bRunning )
	{
		CString strMessage=TEXT("���������������У�ȷʵҪ�˳���");
		if (AfxMessageBox(strMessage,MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2)!=IDYES) return;
	}

	OnServerStop();

	__super::OnCancel();
}

BOOL CWebServerDlg::OnCommand( WPARAM wParam, LPARAM lParam )
{
	switch (LOWORD(wParam))
	{
	case IDC_SERVICE_EVENT:
		{
			if (lParam==WM_RBUTTONUP)
			{
				CMenu menu;
				menu.CreatePopupMenu();
				//�����˵�
				menu.AppendMenu(0, IDM_COPY_STRING,TEXT("����(&C)     Ctrl+C"));
				menu.AppendMenu(0, IDM_SELECT_ALL,TEXT("ȫѡ(&A)     Ctrl+A"));
				menu.AppendMenu(0, IDM_DELETE_STRING,TEXT("ɾ��(&D)"));
				menu.AppendMenu(0, IDM_CLEAR_ALL,TEXT("�����Ļ"));

				//��ȡλ��
				CPoint MousePoint;
				GetCursorPos(&MousePoint);
				
				menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON,MousePoint.x,MousePoint.y,this);
			}
			return TRUE;
		}
	}

	return __super::OnCommand(wParam,lParam);
}

void CWebServerDlg::OnServerStart() 
{
	if (m_bRunning)
		return;

	//���ñ���
	SetWindowText(TEXT("��ҳ���ط����� -- [ ��ʼ�� ]"));

	if ( Start(1000, static_cast<WORD>(m_nPort)) )
	{
		m_bRunning = TRUE;
		
		//���ñ���
		SetWindowText(TEXT("��ҳ���ط����� -- [ ���� ]"));

		AddTraceLine(_T("��վ���ط��������ɹ��������˿ں�Ϊ %d."), m_nPort);

		m_ClientSocket->Connect(m_strCenterAddr, static_cast<WORD>(m_nCenterPort));

		//���ý���
		GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
		GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);

		return;
	}

	AddTraceLine(_T("��������ʧ�ܣ������˿ں�Ϊ %d."), m_nPort);

	//���ñ���
	SetWindowText(TEXT("��ҳ���ط����� -- [ ֹͣ ]"));
}

void CWebServerDlg::OnServerStop() 
{
	if (!m_bRunning)
		return;

	m_bRunning = FALSE;	
	m_ClientSocket->CloseSocket();

	Stop();

	AddTraceLine(_T("����ֹͣ."));

	//���ñ���
	SetWindowText(TEXT("��ҳ���ط����� -- [ ֹͣ ]"));

	//���ý���
	GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(FALSE);
	GetDlgItem(IDC_START_SERVICE)->EnableWindow(TRUE);
}

void CWebServerDlg::OnServerExit() 
{
	CDialog::OnOK();
}

void CWebServerDlg::AddTraceLine(LPCTSTR pstrFormat, ...)
{
	try
	{
		CString str;

		// format and write the data we were given
		va_list args;
		va_start(args, pstrFormat);
		str.FormatV(pstrFormat, args);
		va_end(args);

		CTraceService::TraceString(str, TraceLevel_Normal);
	}
	catch(CMemoryException *e)
	{
		e->Delete();
	}
	catch(...)
	{
		// catch formating errors ...
	}
}

void CWebServerDlg::OnClose() 
{
	OnServerStop();
	
	CDialog::OnCancel();
}

void CWebServerDlg::OnStart() 
{
	OnServerStart();
}

void CWebServerDlg::OnStop() 
{
	OnServerStop();
}

void CWebServerDlg::OnTimer(UINT nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if ( nIDEvent == IDI_RECONNECT_CENTER )
	{
		KillTimer(IDI_RECONNECT_CENTER);

		//��������
		if ( m_bRunning && m_ClientSocket->GetSocketStatus()==SOCKET_STATUS_IDLE )
		{
			m_ClientSocket->Connect(m_strCenterAddr, static_cast<WORD>(m_nCenterPort));
		}
	}

	__super::OnTimer(nIDEvent);
}

//����ر���Ϣ
bool CWebServerDlg::OnEventClientSocketShut(WORD wServiceID, BYTE cbShutReason)
{
	if (cbShutReason!=SHUT_REASON_NORMAL)
	{
		if ((cbShutReason==SHUT_REASON_REMOTE)||(cbShutReason==SHUT_REASON_TIME_OUT))
		{
			AddTraceLine(_T("�����������⣬���Ѿ���������Ͽ����ӣ�����������"));
		}
		else
		{
			AddTraceLine(_T("�����������ݰ�����ʧ�ܣ������ж���"));
		}

		//���ö�ʱ����������
		if ( m_bRunning )
		{
			SetTimer(IDI_RECONNECT_CENTER, 1000, NULL);
		}
	}

	return true;
}

bool CWebServerDlg::OnEventClientSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize)
{
	return true;
}

bool CWebServerDlg::OnEventClientSocketLink(WORD wServiceID, INT nErrorCode)
{
	if ( nErrorCode != 0)
	{
		//���ö�ʱ����������
		if ( m_bRunning )
		{
			SetTimer(IDI_RECONNECT_CENTER, 1000, NULL);
		}
	}

	if ( m_ClientSocket->GetSocketStatus()==SOCKET_STATUS_CONNECT )
	{
		AddTraceLine(_T("�����ķ��������ӳɹ�!"));

		//CMD_KN_DetectSocket DetectSocket;
		//m_ClientSocket->SendData(MDM_KN_COMMAND,SUB_KN_DETECT_SOCKET,&DetectSocket,sizeof(DetectSocket));

		CMD_CS_C_RegisterWebGate mReg;
		ZeroMemory(&mReg, sizeof(CMD_CS_C_RegisterWebGate));

		m_ClientSocket->SendData(MDM_CS_REGISTER, SUB_CS_C_REGISTER_WEBGATE, &mReg, sizeof(mReg));

		return true;
	}

	return true;
}
