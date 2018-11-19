#include "stdafx.h"
#include "WebServer.h"
#include "WebServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CWebServerApp, CWinApp)
	ON_COMMAND(ID_APP_EXIT, OnAppExit)

	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CWebServerApp::CWebServerApp()
{
}

CWebServerApp theApp;

BOOL CWebServerApp::InitInstance()
{
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	//初始化组件
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();

	CWebServerDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}

	return FALSE;
}

int CWebServerApp::ExitInstance() 
{
	return CWinApp::ExitInstance();
}

void CWebServerApp::OnAppExit() 
{
	m_pMainWnd->DestroyWindow();
}