#include "Stdafx.h"
#include "GameServer.h"
#include "GameServerDlg.h"

//////////////////////////////////////////////////////////////////////////////////

//�������
CGameServerApp theApp;

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameServerApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////
inline void EnableMemLeakCheck()
{
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
}

#ifdef _DEBUG
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


//���캯��
CGameServerApp::CGameServerApp()
{
	//EnableMemLeakCheck();
	//_CrtSetBreakAlloc(1014);
}

//��������
BOOL CGameServerApp::InitInstance()
{
	__super::InitInstance();

	//�������
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();

	//����ע���
	SetRegistryKey(szProduct);

	//��ʾ����
	CGameServerDlg GameServerDlg;
	m_pMainWnd=&GameServerDlg;
	GameServerDlg.DoModal();

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////
