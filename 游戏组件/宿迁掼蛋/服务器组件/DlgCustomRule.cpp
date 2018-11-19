#include "Stdafx.h"
#include "Resource.h"
#include "DlgCustomRule.h"

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgCustomRule, CDialog)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��;
CDlgCustomRule::CDlgCustomRule() : CDialog(IDD_CUSTOM_RULE)
{
	//���ñ���;
	ZeroMemory(&m_CustomRule,sizeof(m_CustomRule));

	m_CustomRule.cbTimeOutCard = 15;
	m_CustomRule.cbTimeStartGame = 5;
	m_CustomRule.cbTimeHeadOutCard = 20;

	m_CustomRule.cbTimeTrustee = 1;
	m_CustomRule.cbTimePayTribute = 10;
	m_CustomRule.cbTimeRetTribute = 15;
    
	return;
}

//��������;
CDlgCustomRule::~CDlgCustomRule()
{
}

//���ú���;
BOOL CDlgCustomRule::OnInitDialog()
{
	__super::OnInitDialog();

	//���ÿؼ�;
	((CEdit *)GetDlgItem(IDC_TIME_OUT_CARD))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_TIME_START_GAME))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_TIME_HEAD_OUT_CARD))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_TIME_TRUSTEE))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_TIME_PAY_TRIBUTE))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_TIME_RETURN_TRIBUTE))->LimitText(2);

	//���²���;
	FillDataToControl();

	return FALSE;
}

//ȷ������;
VOID CDlgCustomRule::OnOK() 
{ 
	//Ͷ����Ϣ;
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDOK,0),0);

	return;
}

//ȡ����Ϣ;
VOID CDlgCustomRule::OnCancel() 
{ 
	//Ͷ����Ϣ;
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDCANCEL,0),0);

	return;
}

//���¿ؼ�;
bool CDlgCustomRule::FillDataToControl()
{
	//��������;
	SetDlgItemInt(IDC_TIME_OUT_CARD, m_CustomRule.cbTimeOutCard);
	SetDlgItemInt(IDC_TIME_START_GAME, m_CustomRule.cbTimeStartGame);
	SetDlgItemInt(IDC_TIME_HEAD_OUT_CARD, m_CustomRule.cbTimeHeadOutCard);

	SetDlgItemInt(IDC_TIME_TRUSTEE, m_CustomRule.cbTimeTrustee);
	SetDlgItemInt(IDC_TIME_PAY_TRIBUTE, m_CustomRule.cbTimePayTribute);
	SetDlgItemInt(IDC_TIME_RETURN_TRIBUTE, m_CustomRule.cbTimeRetTribute);

	return true;
}

//��������;
bool CDlgCustomRule::FillControlToData()
{
	//��������;
	m_CustomRule.cbTimeOutCard = (BYTE)GetDlgItemInt(IDC_TIME_OUT_CARD);
	m_CustomRule.cbTimeStartGame = (BYTE)GetDlgItemInt(IDC_TIME_START_GAME);
	m_CustomRule.cbTimeHeadOutCard = (BYTE)GetDlgItemInt(IDC_TIME_HEAD_OUT_CARD);
	m_CustomRule.cbTimeTrustee = (BYTE)GetDlgItemInt(IDC_TIME_TRUSTEE);
	m_CustomRule.cbTimePayTribute = (BYTE)GetDlgItemInt(IDC_TIME_PAY_TRIBUTE);
	m_CustomRule.cbTimeRetTribute = (BYTE)GetDlgItemInt(IDC_TIME_RETURN_TRIBUTE);

	//��ʼʱ��;
	if ((m_CustomRule.cbTimeStartGame<2)||(m_CustomRule.cbTimeStartGame>30))
	{
		AfxMessageBox(TEXT("��ʼʱ�����÷�Χ�������������ã�"),MB_ICONSTOP);
		return false;
	}

	//����ʱ��;
	if ((m_CustomRule.cbTimePayTribute<5) || (m_CustomRule.cbTimePayTribute>60))
	{
		AfxMessageBox(TEXT("����ʱ�����÷�Χ�������������ã�"),MB_ICONSTOP);
		return false;
	}

	//�ع�ʱ��;
	if ((m_CustomRule.cbTimeRetTribute < 5) || (m_CustomRule.cbTimeRetTribute>60))
	{
		AfxMessageBox(TEXT("�ع�ʱ�����÷�Χ�������������ã�"), MB_ICONSTOP);
		return false;
	}

	//�׳�ʱ��;
	if ((m_CustomRule.cbTimeHeadOutCard<5)||(m_CustomRule.cbTimeHeadOutCard>60))
	{
		AfxMessageBox(TEXT("�׳�ʱ�����÷�Χ�������������ã�"),MB_ICONSTOP);
		return false;
	}

	//����ʱ��;
	if ((m_CustomRule.cbTimeOutCard<5)||(m_CustomRule.cbTimeOutCard>60))
	{
		AfxMessageBox(TEXT("����ʱ�����÷�Χ�������������ã�"),MB_ICONSTOP);
		return false;
	}

	//�й�ʱ��;
	if ((m_CustomRule.cbTimeTrustee<1) || (m_CustomRule.cbTimeTrustee>20))
	{
		AfxMessageBox(TEXT("�й�ʱ�����÷�Χ�������������ã�"), MB_ICONSTOP);
		return false;
	}

	return true;
}

//��ȡ����;
bool CDlgCustomRule::GetCustomRule(tagCustomRule & CustomRule)
{
	//��ȡ����;
	if (FillControlToData()==true)
	{
		CustomRule=m_CustomRule;
		return true;
	}

	return false;
}

//��������;
bool CDlgCustomRule::SetCustomRule(tagCustomRule & CustomRule)
{
	//���ñ���;
	m_CustomRule=CustomRule;

	//���²���;
	if (m_hWnd!=NULL) FillDataToControl();

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
