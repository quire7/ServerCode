#include "stdafx.h"
#include "WebServer.h"
#include "WebServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define  IDI_RECONNECT_CENTER	10001

//菜单命令
#define IDM_CLEAR_ALL				(WM_USER+201)						//删除信息
#define IDM_SELECT_ALL				(WM_USER+202)						//全部选择
#define IDM_COPY_STRING				(WM_USER+203)						//拷贝字符
#define IDM_SAVE_STRING				(WM_USER+205)						//保存信息
#define IDM_DELETE_STRING			(WM_USER+204)						//删除字符

CWebServerDlg::CWebServerDlg() : CDialog(IDD_SERV_DIALOG), m_nOpenConnections(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bRunning = FALSE;
	m_bInitialized = FALSE;
}

CWebServerDlg::~CWebServerDlg()
{
	//KillTimer(IDI_RECONNECT_CENTER);

	//关闭连接
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

//接口查询
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

	//设置标题
	SetWindowText(TEXT("网页网关服务器 -- [ 停止 ]"));

	// Set big icon
	SetIcon(m_hIcon, TRUE);			
	// Set small icon
	SetIcon(m_hIcon, FALSE);		
	
	//获取目录
	TCHAR szPath[MAX_PATH]=TEXT("");
	GetCurrentDirectory(sizeof(szPath),szPath);

	//读取配置
	TCHAR szFileName[MAX_PATH] = {0};
	_tprintf_s(szFileName,sizeof(szFileName),TEXT("%s\\ServerParameter.ini"), szPath);

	CString strTag = TEXT("WebGate");
	//是否自动启动
	m_bAutoStart = (GetPrivateProfileInt(strTag, TEXT("AutoStart"), 0, szFileName)>0);
	//服务端口
	m_nPort = GetPrivateProfileInt(strTag, TEXT("ServicePort"), PORT_WEB_GATE, szFileName);
	//中心服务器地址
	TCHAR szAddr[128]={0};
	::GetPrivateProfileString(strTag, TEXT("CorrespondAddr"), TEXT("127.0.0.1"), szAddr, sizeof(szAddr), szFileName);

	m_strCenterAddr = szAddr;
	//中心服务器端口
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

	//创建网络组件
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
	//询问退出
	if ( m_bRunning )
	{
		CString strMessage=TEXT("服务器正在运行中，确实要退出吗？");
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
				//建立菜单
				menu.AppendMenu(0, IDM_COPY_STRING,TEXT("复制(&C)     Ctrl+C"));
				menu.AppendMenu(0, IDM_SELECT_ALL,TEXT("全选(&A)     Ctrl+A"));
				menu.AppendMenu(0, IDM_DELETE_STRING,TEXT("删除(&D)"));
				menu.AppendMenu(0, IDM_CLEAR_ALL,TEXT("清除屏幕"));

				//获取位置
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

	//设置标题
	SetWindowText(TEXT("网页网关服务器 -- [ 初始化 ]"));

	if ( Start(1000, static_cast<WORD>(m_nPort)) )
	{
		m_bRunning = TRUE;
		
		//设置标题
		SetWindowText(TEXT("网页网关服务器 -- [ 运行 ]"));

		AddTraceLine(_T("网站网关服务启动成功，监听端口号为 %d."), m_nPort);

		m_ClientSocket->Connect(m_strCenterAddr, static_cast<WORD>(m_nCenterPort));

		//设置界面
		GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
		GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);

		return;
	}

	AddTraceLine(_T("服务启动失败，监听端口号为 %d."), m_nPort);

	//设置标题
	SetWindowText(TEXT("网页网关服务器 -- [ 停止 ]"));
}

void CWebServerDlg::OnServerStop() 
{
	if (!m_bRunning)
		return;

	m_bRunning = FALSE;	
	m_ClientSocket->CloseSocket();

	Stop();

	AddTraceLine(_T("服务停止."));

	//设置标题
	SetWindowText(TEXT("网页网关服务器 -- [ 停止 ]"));

	//设置界面
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
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if ( nIDEvent == IDI_RECONNECT_CENTER )
	{
		KillTimer(IDI_RECONNECT_CENTER);

		//重新连接
		if ( m_bRunning && m_ClientSocket->GetSocketStatus()==SOCKET_STATUS_IDLE )
		{
			m_ClientSocket->Connect(m_strCenterAddr, static_cast<WORD>(m_nCenterPort));
		}
	}

	__super::OnTimer(nIDEvent);
}

//网络关闭消息
bool CWebServerDlg::OnEventClientSocketShut(WORD wServiceID, BYTE cbShutReason)
{
	if (cbShutReason!=SHUT_REASON_NORMAL)
	{
		if ((cbShutReason==SHUT_REASON_REMOTE)||(cbShutReason==SHUT_REASON_TIME_OUT))
		{
			AddTraceLine(_T("由于网络问题，您已经与服务器断开连接，请重新连接"));
		}
		else
		{
			AddTraceLine(_T("由于网络数据包处理失败，网络中断了"));
		}

		//设置定时器重新连接
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
		//设置定时器重新连接
		if ( m_bRunning )
		{
			SetTimer(IDI_RECONNECT_CENTER, 1000, NULL);
		}
	}

	if ( m_ClientSocket->GetSocketStatus()==SOCKET_STATUS_CONNECT )
	{
		AddTraceLine(_T("与中心服务器连接成功!"));

		//CMD_KN_DetectSocket DetectSocket;
		//m_ClientSocket->SendData(MDM_KN_COMMAND,SUB_KN_DETECT_SOCKET,&DetectSocket,sizeof(DetectSocket));

		CMD_CS_C_RegisterWebGate mReg;
		ZeroMemory(&mReg, sizeof(CMD_CS_C_RegisterWebGate));

		m_ClientSocket->SendData(MDM_CS_REGISTER, SUB_CS_C_REGISTER_WEBGATE, &mReg, sizeof(mReg));

		return true;
	}

	return true;
}
