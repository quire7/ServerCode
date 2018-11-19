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

//�ؼ����໯
void CSystemOptionDlg::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
}

//��ʼ������
BOOL CSystemOptionDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//��������
	((CEdit *)GetDlgItem(IDC_LISTEN_PORT))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_MAX_CONNECT))->LimitText(3);

	//���ز���
	CInitParameter InitParamter;
	InitParamter.LoadInitParameter();

	//���ÿؼ�
	SetDlgItemInt(IDC_LISTEN_PORT, InitParamter.m_wServicePort, FALSE);
	SetDlgItemInt(IDC_MAX_CONNECT, InitParamter.m_wMaxConnect, FALSE);

	return TRUE;
}

//ȷ������
void CSystemOptionDlg::OnOK()
{
	//��ȡ����
	CInitParameter InitParamter;
	InitParamter.m_wServicePort = GetDlgItemInt(IDC_LISTEN_PORT);
	InitParamter.m_wMaxConnect = GetDlgItemInt(IDC_MAX_CONNECT);

	//��������
	InitParamter.SaveInitParameter();

	__super::OnOK();
}
