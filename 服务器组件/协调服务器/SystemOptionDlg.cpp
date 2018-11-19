#include "Stdafx.h"
#include "SystemOptionDlg.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CSystemOptionDlg, CDialog)
END_MESSAGE_MAP()

CSystemOptionDlg::CSystemOptionDlg() : CDialog(IDD_SYSTEM_OPTION)
{
}

CSystemOptionDlg::~CSystemOptionDlg()
{
}

//控件子类化
void CSystemOptionDlg::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
}

//初始化函数
BOOL CSystemOptionDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//限制输入
	((CEdit *)GetDlgItem(IDC_LISTEN_PORT))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_MAX_CONNECT))->LimitText(3);

	//加载参数
	CInitParameter InitParamter;
	InitParamter.LoadInitParameter();

	//设置控件
	SetDlgItemInt(IDC_LISTEN_PORT, InitParamter.m_wServicePort, FALSE);
	SetDlgItemInt(IDC_MAX_CONNECT, InitParamter.m_wMaxConnect, FALSE);

	return TRUE;
}

//确定函数
void CSystemOptionDlg::OnOK()
{
	//获取输入
	CInitParameter InitParamter;
	InitParamter.m_wServicePort = GetDlgItemInt(IDC_LISTEN_PORT);
	InitParamter.m_wMaxConnect = GetDlgItemInt(IDC_MAX_CONNECT);

	//保存设置
	InitParamter.SaveInitParameter();

	__super::OnOK();
}
