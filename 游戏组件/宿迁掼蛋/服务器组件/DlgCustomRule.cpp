#include "Stdafx.h"
#include "Resource.h"
#include "DlgCustomRule.h"

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgCustomRule, CDialog)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数;
CDlgCustomRule::CDlgCustomRule() : CDialog(IDD_CUSTOM_RULE)
{
	//设置变量;
	ZeroMemory(&m_CustomRule,sizeof(m_CustomRule));

	m_CustomRule.cbTimeOutCard = 15;
	m_CustomRule.cbTimeStartGame = 5;
	m_CustomRule.cbTimeHeadOutCard = 20;

	m_CustomRule.cbTimeTrustee = 1;
	m_CustomRule.cbTimePayTribute = 10;
	m_CustomRule.cbTimeRetTribute = 15;
    
	return;
}

//析构函数;
CDlgCustomRule::~CDlgCustomRule()
{
}

//配置函数;
BOOL CDlgCustomRule::OnInitDialog()
{
	__super::OnInitDialog();

	//设置控件;
	((CEdit *)GetDlgItem(IDC_TIME_OUT_CARD))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_TIME_START_GAME))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_TIME_HEAD_OUT_CARD))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_TIME_TRUSTEE))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_TIME_PAY_TRIBUTE))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_TIME_RETURN_TRIBUTE))->LimitText(2);

	//更新参数;
	FillDataToControl();

	return FALSE;
}

//确定函数;
VOID CDlgCustomRule::OnOK() 
{ 
	//投递消息;
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDOK,0),0);

	return;
}

//取消消息;
VOID CDlgCustomRule::OnCancel() 
{ 
	//投递消息;
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDCANCEL,0),0);

	return;
}

//更新控件;
bool CDlgCustomRule::FillDataToControl()
{
	//设置数据;
	SetDlgItemInt(IDC_TIME_OUT_CARD, m_CustomRule.cbTimeOutCard);
	SetDlgItemInt(IDC_TIME_START_GAME, m_CustomRule.cbTimeStartGame);
	SetDlgItemInt(IDC_TIME_HEAD_OUT_CARD, m_CustomRule.cbTimeHeadOutCard);

	SetDlgItemInt(IDC_TIME_TRUSTEE, m_CustomRule.cbTimeTrustee);
	SetDlgItemInt(IDC_TIME_PAY_TRIBUTE, m_CustomRule.cbTimePayTribute);
	SetDlgItemInt(IDC_TIME_RETURN_TRIBUTE, m_CustomRule.cbTimeRetTribute);

	return true;
}

//更新数据;
bool CDlgCustomRule::FillControlToData()
{
	//设置数据;
	m_CustomRule.cbTimeOutCard = (BYTE)GetDlgItemInt(IDC_TIME_OUT_CARD);
	m_CustomRule.cbTimeStartGame = (BYTE)GetDlgItemInt(IDC_TIME_START_GAME);
	m_CustomRule.cbTimeHeadOutCard = (BYTE)GetDlgItemInt(IDC_TIME_HEAD_OUT_CARD);
	m_CustomRule.cbTimeTrustee = (BYTE)GetDlgItemInt(IDC_TIME_TRUSTEE);
	m_CustomRule.cbTimePayTribute = (BYTE)GetDlgItemInt(IDC_TIME_PAY_TRIBUTE);
	m_CustomRule.cbTimeRetTribute = (BYTE)GetDlgItemInt(IDC_TIME_RETURN_TRIBUTE);

	//开始时间;
	if ((m_CustomRule.cbTimeStartGame<2)||(m_CustomRule.cbTimeStartGame>30))
	{
		AfxMessageBox(TEXT("开始时间设置范围错误，请重新设置！"),MB_ICONSTOP);
		return false;
	}

	//进贡时间;
	if ((m_CustomRule.cbTimePayTribute<5) || (m_CustomRule.cbTimePayTribute>60))
	{
		AfxMessageBox(TEXT("进贡时间设置范围错误，请重新设置！"),MB_ICONSTOP);
		return false;
	}

	//回贡时间;
	if ((m_CustomRule.cbTimeRetTribute < 5) || (m_CustomRule.cbTimeRetTribute>60))
	{
		AfxMessageBox(TEXT("回贡时间设置范围错误，请重新设置！"), MB_ICONSTOP);
		return false;
	}

	//首出时间;
	if ((m_CustomRule.cbTimeHeadOutCard<5)||(m_CustomRule.cbTimeHeadOutCard>60))
	{
		AfxMessageBox(TEXT("首出时间设置范围错误，请重新设置！"),MB_ICONSTOP);
		return false;
	}

	//出牌时间;
	if ((m_CustomRule.cbTimeOutCard<5)||(m_CustomRule.cbTimeOutCard>60))
	{
		AfxMessageBox(TEXT("出牌时间设置范围错误，请重新设置！"),MB_ICONSTOP);
		return false;
	}

	//托管时间;
	if ((m_CustomRule.cbTimeTrustee<1) || (m_CustomRule.cbTimeTrustee>20))
	{
		AfxMessageBox(TEXT("托管时间设置范围错误，请重新设置！"), MB_ICONSTOP);
		return false;
	}

	return true;
}

//读取配置;
bool CDlgCustomRule::GetCustomRule(tagCustomRule & CustomRule)
{
	//读取参数;
	if (FillControlToData()==true)
	{
		CustomRule=m_CustomRule;
		return true;
	}

	return false;
}

//设置配置;
bool CDlgCustomRule::SetCustomRule(tagCustomRule & CustomRule)
{
	//设置变量;
	m_CustomRule=CustomRule;

	//更新参数;
	if (m_hWnd!=NULL) FillDataToControl();

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
