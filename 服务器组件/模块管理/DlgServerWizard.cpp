#include "StdAfx.h"
#include "Resource.h"
#include "DlgServerWizard.h"
#include "ModuleDBParameter.h"
#include "Ws2tcpip.h"

//////////////////////////////////////////////////////////////////////////////////

//ҳ�涨��
#define ITEM_SERVER_OPTION_1		0									//����ѡ��
#define ITEM_SERVER_OPTION_2		1									//����ѡ��
#define ITEM_SERVER_OPTION_3		2									//����ѡ��
#define ITEM_SERVER_OPTION_4		3
#define ITEM_SERVER_OPTION_CUSTOM	4									//��������

//��������
#define SUB_GP_GET_PARAMETER		100									//��ȡ����
#define SUB_GP_ADD_PARAMETER		101									//��Ӳ���
#define SUB_GP_MODIFY_PARAMETER		102									//�޸Ĳ���
#define SUB_GP_DELETE_PARAMETER		103									//ɾ������

//��������
#define MIN_INOUTINTERVAL			30*60								//��С���
#define MAX_INOUTINTERVAL			24*3600								//�����

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgServerOptionItem, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgServerWizardItem, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgServerOptionItem1, CDlgServerOptionItem)
	ON_CBN_SELCHANGE(IDC_SERVER_TYPE, OnSelchangeServerType)
	ON_CBN_SELCHANGE(IDC_SERVER_KIND, OnSelchangeServerKind)
	ON_BN_CLICKED(IDC_REVENUE_RADIO, OnBnClickedRevenueRadio)
	ON_BN_CLICKED(IDC_SERVICE_RADIO, OnBnClickedServiceRadio)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgServerOptionItem2, CDlgServerOptionItem)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgServerOptionItem3, CDlgServerOptionItem)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgServerOptionItem4, CDlgServerOptionItem)
	ON_BN_CLICKED(IDC_BT_ADD, OnBnClickedBtAdd)
	ON_BN_CLICKED(IDC_BT_MODIFY, OnBnClickedBtModify)
	ON_BN_CLICKED(IDC_BT_DELETE, OnBnClickedBtDelete)

	ON_NOTIFY(LVN_DELETEITEM, IDC_LIST_PARAMETER, OnLvnListParameterDeleteitem)
	ON_NOTIFY(NM_CLICK, IDC_LIST_PARAMETER, OnHdnListParameterItemclick)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_PARAMETER, OnLvnKeydownListParameter)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgServerOptionItemCustom, CDlgServerOptionItem)
	ON_WM_SETFOCUS()
	ON_WM_NCDESTROY()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgServerWizardItem1, CDlgServerWizardItem)
	ON_NOTIFY(NM_DBLCLK, IDC_MODULE_LIST, OnNMDblclkModuleList)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgServerWizardItem2, CDlgServerWizardItem)
	ON_WM_SETFOCUS()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CTRL, OnTcnSelchangeTabCtrl)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgServerWizard, CDialog)
	ON_BN_CLICKED(IDC_LAST, OnBnClickedLast)
	ON_BN_CLICKED(IDC_NEXT, OnBnClickedNext)
	ON_BN_CLICKED(IDC_FINISH, OnBnClickedFinish)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDlgServerOptionItem::CDlgServerOptionItem(UINT nIDTemplate) : CDialog(nIDTemplate)
{
	//���ñ���
	m_pDlgServerWizard=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	return;
}

//��������
CDlgServerOptionItem::~CDlgServerOptionItem()
{
}

//��������
bool CDlgServerOptionItem::SaveItemData()
{
	//��������
	if ((m_hWnd!=NULL)&&(SaveInputInfo()==false))
	{
		return false;
	}

	return true;
}

//��ʾ����
bool CDlgServerOptionItem::ShowOptionItem(const CRect & rcRect, CWnd * pParentWnd)
{
	//��������
	if (m_hWnd==NULL) 
	{
		//������Դ
		AfxSetResourceHandle(GetModuleHandle(MODULE_MANAGER_DLL_NAME));

		//��������
		Create(m_lpszTemplateName,pParentWnd);
		SetWindowPos(NULL,rcRect.left,rcRect.top,rcRect.Width(),rcRect.Height(),SWP_NOZORDER|SWP_NOACTIVATE);

		//������Դ
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}

	//��ʾ����
	ShowWindow(SW_SHOW);

	return true;
}

//ȷ������
VOID CDlgServerOptionItem::OnOK() 
{ 
	//Ͷ����Ϣ
	m_pDlgServerWizard->PostMessage(WM_COMMAND,MAKELONG(IDOK,0),0);

	return;
}

//ȡ����Ϣ
VOID CDlgServerOptionItem::OnCancel() 
{ 
	//Ͷ����Ϣ
	m_pDlgServerWizard->PostMessage(WM_COMMAND,MAKELONG(IDCANCEL,0),0);

	return;
}

//λ����Ϣ
VOID CDlgServerOptionItem::OnSize(UINT nType, INT cx, INT cy)
{
	__super::OnSize(nType, cx, cy);

	//�����ؼ�
	RectifyControl(cx,cy);

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDlgServerOptionItem1::CDlgServerOptionItem1() : CDlgServerOptionItem(IDD_SERVER_OPTION_1)
{
}

//��������
CDlgServerOptionItem1::~CDlgServerOptionItem1()
{
}

//��ʼ������
BOOL CDlgServerOptionItem1::OnInitDialog()
{
	__super::OnInitDialog();

	//����ؼ�
	InitCtrlWindow();

	//��������
	CComboBox * pServerType=(CComboBox *)GetDlgItem(IDC_SERVER_TYPE);
	CComboBox * pServerKind=(CComboBox *)GetDlgItem(IDC_SERVER_KIND);

	//��������
	if (m_pGameServiceOption->szServerName[0]==0)
	{
		SetDlgItemText(IDC_SERVER_NAME,m_pGameServiceAttrib->szGameName);
	}
	else SetDlgItemText(IDC_SERVER_NAME,m_pGameServiceOption->szServerName);

	//������Ŀ
	if (m_pGameServiceOption->wTableCount!=0)
	{
		SetDlgItemInt(IDC_TABLE_COUNT,m_pGameServiceOption->wTableCount);
	}
	else if (m_pGameServiceAttrib->wChairCount >= MAX_CHAIR)
	{
		SetDlgItemInt(IDC_TABLE_COUNT,1);
	}
	else 
	{
		SetDlgItemInt(IDC_TABLE_COUNT,60);
	}

	//����˿�
	if (m_pGameServiceOption->wServerPort!=0)
	{
		SetDlgItemInt(IDC_SERVER_PORT,m_pGameServiceOption->wServerPort);
	}

	////���ݿ���
	//if (m_pGameServiceOption->szDataBaseName[0]!=0)
	//{
	//	SetDlgItemText(IDC_DATABASE_NAME,m_pGameServiceOption->szDataBaseName);
	//}
	//else SetDlgItemText(IDC_DATABASE_NAME,m_pGameServiceAttrib->szDataBaseName);

	////���ӵ�ַ
	//if (m_pGameServiceOption->szDataBaseAddr[0]!=0)
	//{
	//	//DWORD dwDataBaseAddr=inet_addr(CT2CA(m_pGameServiceOption->szDataBaseAddr));
	//	//((CIPAddressCtrl *)GetDlgItem(IDC_DATABASE_ADDR))->SetAddress(htonl(dwDataBaseAddr));

	//	struct in_addr addr = { 0 };
	//	inet_pton(AF_INET, CT2CA(m_pGameServiceOption->szDataBaseAddr), (void*)&addr);
	//	((CIPAddressCtrl *)GetDlgItem(IDC_DATABASE_ADDR))->SetAddress(htonl(addr.s_addr));
	//}

	//if (m_pGameServiceOption->szDataBaseAddr[0] != 0)
	//{
	//	//DWORD dwCacheServerAddr = inet_addr(CT2CA(m_pGameServiceOption->szDataBaseAddr));
	//	//((CIPAddressCtrl *)GetDlgItem(IDC_CACHE_SERVER_ADDR))->SetAddress(htonl(dwCacheServerAddr));

	//	struct in_addr addr = { 0 };
	//	inet_pton(AF_INET, CT2CA(m_pGameServiceOption->szDataBaseAddr), (void*)&addr);
	//	((CIPAddressCtrl *)GetDlgItem(IDC_CACHE_SERVER_ADDR))->SetAddress(htonl(addr.s_addr));
	//}

	//��Ϸ����
	for (INT i=0;i<pServerType->GetCount();i++)
	{
		if (pServerType->GetItemData(i)==m_pGameServiceOption->wServerType)
		{
			pServerType->SetCurSel(i);
		}
	}

	//��������
	if (m_pGameServiceOption->wServerKind&SERVER_GENRE_PASSWD)
	{
		GetDlgItem(IDC_SERVER_PASSWD)->EnableWindow(true);
		SetDlgItemText(IDC_SERVER_PASSWD,m_pGameServiceOption->szServerPasswd);
	}
	else
	{
		GetDlgItem(IDC_SERVER_PASSWD)->EnableWindow(false);
		SetDlgItemText(IDC_SERVER_PASSWD,TEXT(""));
	}

	//��������
	for (INT i=0;i<pServerKind->GetCount();i++)
	{
		if (pServerKind->GetItemData(i)==m_pGameServiceOption->wServerKind)
		{
			pServerKind->SetCurSel(i);
		}
	}

	//��������
	if (m_pGameServiceOption->lCellScore!=0) SetDlgItemInt(IDC_CELL_SCORE,(LONG)m_pGameServiceOption->lCellScore);
	if (m_pGameServiceOption->wMaxPlayer!=0) SetDlgItemInt(IDC_MAX_PLAYER,(LONG)m_pGameServiceOption->wMaxPlayer);
	if (m_pGameServiceOption->lRestrictScore!=0) SetDlgItemInt(IDC_RESTRICT_SCORE,(LONG)m_pGameServiceOption->lRestrictScore);
	if (m_pGameServiceOption->wRevenueRatio!=0)
	{
		OnBnClickedRevenueRadio();
		SetDlgItemInt(IDC_SERVICE_REVENUE,(LONG)m_pGameServiceOption->wRevenueRatio);
		((CButton*)GetDlgItem(IDC_REVENUE_RADIO))->SetCheck(BST_CHECKED);
		((CButton*)GetDlgItem(IDC_SERVICE_RADIO))->SetCheck(BST_UNCHECKED);
		
	}
	else if(m_pGameServiceOption->lServiceScore != 0)
	{
		OnBnClickedServiceRadio();
		SetDlgItemInt(IDC_SERVICE_REVENUE,(LONG)m_pGameServiceOption->lServiceScore);
		((CButton*)GetDlgItem(IDC_SERVICE_RADIO))->SetCheck(BST_CHECKED);
		((CButton*)GetDlgItem(IDC_REVENUE_RADIO))->SetCheck(BST_UNCHECKED);
	}

	if (m_pGameServiceOption->dwWinExperience != 0) SetDlgItemInt(IDC_WIN_EXP, (LONG)m_pGameServiceOption->dwWinExperience);
	if (m_pGameServiceOption->dwFailExperience != 0) SetDlgItemInt(IDC_FAIL_EXP, (LONG)m_pGameServiceOption->dwFailExperience);

	//������Ϸ�޵׷�����
	if(m_pGameServiceAttrib->wChairCount >= MAX_CHAIR)
	{
		GetDlgItem(IDC_CELL_SCORE)->EnableWindow(FALSE);
	}

	//�ҽ�����
	WORD wGameKindID=m_pGameServiceOption->wKindID;
	WORD wAttribKindID=m_pGameServiceAttrib->wKindID;
	if (m_pGameServiceOption->wSortID != 0)
	{
		SetDlgItemInt(IDC_SORT_ID, m_pGameServiceOption->wSortID);
	}

	//Ĭ��ѡ��
	if (pServerType->GetCurSel()==LB_ERR)
	{
		//��ȡ����
		TCHAR szDataBase[32]=TEXT("");
		GetDlgItemText(IDC_DATABASE_NAME,szDataBase,CountArray(szDataBase));

		//��������
		WORD wServerType=0;
		bool bGoldDataBase=(lstrcmpi(szDataBase,szTreasureDB)==0);
		bool bDefaultDataBase=(lstrcmpi(szDataBase,m_pGameServiceAttrib->szDataBaseName)==0);

		//���Ͷ���
		if ((wServerType==0)&&(bGoldDataBase==true)) wServerType=GAME_GENRE_GOLD;
		if ((wServerType==0)&&(bGoldDataBase==false)&&(bDefaultDataBase==true)) wServerType=GAME_GENRE_SCORE;

		//��������
		if ((m_pGameServiceAttrib->wSupporType&wServerType)!=0L)
		{
			for (INT i=0;i<pServerType->GetCount();i++)
			{
				if (pServerType->GetItemData(i)==wServerType)
				{
					pServerType->SetCurSel(i);
					break;
				}
			}
		}
	}

	return TRUE;
}

//��������
bool CDlgServerOptionItem1::SaveInputInfo()
{
	//��������
	CComboBox * pServerType=(CComboBox *)GetDlgItem(IDC_SERVER_TYPE);
	CComboBox * pServerKind=(CComboBox *)GetDlgItem(IDC_SERVER_KIND);
	CIPAddressCtrl * pIPAddressCtrl=(CIPAddressCtrl *)GetDlgItem(IDC_DATABASE_ADDR);

	//�ҽ�����
	m_pGameServiceOption->wSortID=GetDlgItemInt(IDC_SORT_ID);

	//��������
	m_pGameServiceOption->wMaxPlayer=GetDlgItemInt(IDC_MAX_PLAYER);
	m_pGameServiceOption->lCellScore=GetDlgItemInt(IDC_CELL_SCORE);
	m_pGameServiceOption->dwWinExperience = GetDlgItemInt(IDC_WIN_EXP);
	m_pGameServiceOption->dwFailExperience = GetDlgItemInt(IDC_FAIL_EXP);
	
	m_pGameServiceOption->lRestrictScore=GetDlgItemInt(IDC_RESTRICT_SCORE);
	if(((CButton*)GetDlgItem(IDC_REVENUE_RADIO))->GetCheck()==BST_CHECKED)
	{
		m_pGameServiceOption->wRevenueRatio=GetDlgItemInt(IDC_SERVICE_REVENUE);
		m_pGameServiceOption->lServiceScore=0L;
	}
	else
	{
		m_pGameServiceOption->lServiceScore=GetDlgItemInt(IDC_SERVICE_REVENUE);
		m_pGameServiceOption->wRevenueRatio=0L;
	}

	//ѡ��ؼ�
	m_pGameServiceOption->wServerType=(WORD)pServerType->GetItemData(pServerType->GetCurSel());
	m_pGameServiceOption->wServerKind=(WORD)pServerKind->GetItemData(pServerKind->GetCurSel());

	//��������
	m_pGameServiceOption->wTableCount=GetDlgItemInt(IDC_TABLE_COUNT);
	m_pGameServiceOption->wServerPort=GetDlgItemInt(IDC_SERVER_PORT);
	GetDlgItemText(IDC_SERVER_NAME,m_pGameServiceOption->szServerName,CountArray(m_pGameServiceOption->szServerName));
	//GetDlgItemText(IDC_DATABASE_NAME,m_pGameServiceOption->szDataBaseName,CountArray(m_pGameServiceOption->szDataBaseName));
	GetDlgItemText(IDC_SERVER_PASSWD,m_pGameServiceOption->szServerPasswd,CountArray(m_pGameServiceOption->szServerPasswd));

	////���ӵ�ַ
	//DWORD dwDataBaseAddr=INADDR_NONE;
	//pIPAddressCtrl->GetAddress(dwDataBaseAddr);
	//_sntprintf_s(m_pGameServiceOption->szDataBaseAddr, CountArray(m_pGameServiceOption->szDataBaseAddr), TEXT("%d.%d.%d.%d"),
	//	*(((BYTE *)&dwDataBaseAddr)+3),*(((BYTE *)&dwDataBaseAddr)+2),*(((BYTE *)&dwDataBaseAddr)+1),*((BYTE *)&dwDataBaseAddr));

	//��������
	if (m_pGameServiceOption->szServerName[0]==0)
	{
		AfxMessageBox(TEXT("��Ϸ�������ֲ���Ϊ�գ���������Ϸ��������"),MB_ICONERROR);
		return false;
	}

	//����ж�
	if((m_pGameServiceOption->wServerKind&SERVER_GENRE_PASSWD)!=0 && (m_pGameServiceOption->szServerPasswd[0]==0))
	{
		AfxMessageBox(TEXT("��Ϸ�������벻��Ϊ�գ���������Ϸ��������"),MB_ICONERROR);
		return false;
	}

	//�����ж�
	if (m_pGameServiceOption->wServerType==0)
	{
		AfxMessageBox(TEXT("��Ϸ�������Ͳ���Ϊ�գ�������ѡ����Ϸ��������"),MB_ICONERROR);
		return false;
	}

	////���ݿ���
	//if (m_pGameServiceOption->szDataBaseName[0]==0)
	//{
	//	AfxMessageBox(TEXT("��Ϸ���ݿ�������Ϊ�գ���������Ϸ���ݿ���"),MB_ICONERROR);
	//	return false;
	//}

	////���ӵ�ַ
	//if (INVALID_IP_ADDRESS(dwDataBaseAddr))
	//{
	//	AfxMessageBox(TEXT("��Ϸ���ݿ��ַ��ʽ����ȷ��������������Ϸ���ݿ��ַ"),MB_ICONERROR);
	//	return false;
	//}

	//������Ŀ
	if ((m_pGameServiceOption->wTableCount==0)||(m_pGameServiceOption->wTableCount>MAX_TABLE))
	{
		AfxMessageBox(TEXT("��Ϸ������Ŀ��Ч��������������Ϸ������Ŀ����Ч��ֵ��ΧΪ 1 - 512"),MB_ICONERROR);
		return false;
	}

	return true;
}

//�����ؼ�
VOID CDlgServerOptionItem1::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//����ؼ�
VOID CDlgServerOptionItem1::InitCtrlWindow()
{
	//�б�����
	((CEdit *)GetDlgItem(IDC_SORT_ID))->LimitText(5);

	//��������
	((CEdit *)GetDlgItem(IDC_MAX_PLAYER))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_CELL_SCORE))->LimitText(6);
	((CEdit *)GetDlgItem(IDC_SERVICE_REVENUE))->LimitText(3);
	((CButton*)GetDlgItem(IDC_REVENUE_RADIO))->SetCheck(BST_CHECKED);
	((CEdit *)GetDlgItem(IDC_WIN_EXP))->LimitText(6);
	((CEdit *)GetDlgItem(IDC_FAIL_EXP))->LimitText(6);

	//��������
	((CEdit *)GetDlgItem(IDC_TABLE_COUNT))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_SERVER_PORT))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_SERVER_NAME))->LimitText(31);
	((CEdit *)GetDlgItem(IDC_DATABASE_NAME))->LimitText(31);

	//��������
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_SERVER_TYPE);
	WORD wServerType[5]={GAME_GENRE_GOLD,GAME_GENRE_SCORE,GAME_GENRE_MATCH,GAME_GENRE_EDUCATE,GAME_GENRE_PRIVATE};
	LPCTSTR pszServerType[5] = { TEXT("�Ƹ�����"), TEXT("��ֵ����"), TEXT("��������"), TEXT("��ϰ����"), TEXT("˽�˳�����") };

	//��������
	for (INT i=0;i<CountArray(wServerType);i++)
	{
		if ((m_pGameServiceAttrib->wSupporType&wServerType[i])==0) continue;
		pComboBox->SetItemData(pComboBox->AddString(pszServerType[i]),wServerType[i]);
	}

	//��������
	pComboBox=(CComboBox *)GetDlgItem(IDC_SERVER_KIND);
	WORD wServerKind[2]={SERVER_GENRE_NORMAL,SERVER_GENRE_PASSWD};
	LPCTSTR pszServerKind[2]={TEXT("��ͨ����"),TEXT("���뷿��")};

	//��������
	for (INT i=0;i<CountArray(wServerKind);i++)
	{
		pComboBox->SetItemData(pComboBox->InsertString(i,pszServerKind[i]),wServerKind[i]);
	}
	if(pComboBox->GetCurSel()==LB_ERR) pComboBox->SetCurSel(0);

	return;
}

//��������
VOID CDlgServerOptionItem1::UpdateDataBaseName()
{
	//��������
	CComboBox * pServerType=(CComboBox *)GetDlgItem(IDC_SERVER_TYPE);

	//���ݵ���
	switch ((WORD)pServerType->GetItemData(pServerType->GetCurSel()))
	{
	case GAME_GENRE_GOLD:	//�Ƹ�����
		{
			SetDlgItemText(IDC_DATABASE_NAME,szTreasureDB);
			break;
		}
	case GAME_GENRE_SCORE:	//��ֵ����
		{
			SetDlgItemText(IDC_DATABASE_NAME,m_pGameServiceAttrib->szDataBaseName);
			break;
		}
	case GAME_GENRE_MATCH:	//��������
		{
			SetDlgItemText(IDC_DATABASE_NAME, szTreasureDB);
			break;
		}
	case GAME_GENRE_PRIVATE:	//��������
	{
		SetDlgItemText(IDC_DATABASE_NAME, szTreasureDB);
		break;
	}
	default:
		{
			//��ȡ����
			TCHAR szDataBase[32]=TEXT("");
			GetDlgItemText(IDC_DATABASE_NAME,szDataBase,CountArray(szDataBase));

			//�����ж�
			if (lstrcmpi(szDataBase,szTreasureDB)==0) SetDlgItemText(IDC_DATABASE_NAME,TEXT(""));
			if (lstrcmpi(szDataBase,m_pGameServiceAttrib->szDataBaseName)==0) SetDlgItemText(IDC_DATABASE_NAME,TEXT(""));

			break;
		}
	}

	return;
}

//ѡ��ı�
VOID CDlgServerOptionItem1::OnSelchangeServerType()
{
	//��������
	UpdateDataBaseName();

	return;
}

//ѡ��ı�
VOID CDlgServerOptionItem1::OnSelchangeServerKind()
{
	//��������
	CComboBox * pServerKind=(CComboBox *)GetDlgItem(IDC_SERVER_KIND);

	//���ݵ���
	int nCurSelIndex = pServerKind->GetCurSel();
	switch (pServerKind->GetItemData(nCurSelIndex))
	{
	case SERVER_GENRE_NORMAL:	//��ͨ����
		{
			GetDlgItem(IDC_SERVER_PASSWD)->EnableWindow(false);
			break;
		}
	case SERVER_GENRE_PASSWD:	//��������
		{
			GetDlgItem(IDC_SERVER_PASSWD)->EnableWindow(true);
			break;
		}
	}

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDlgServerOptionItem2::CDlgServerOptionItem2() : CDlgServerOptionItem(IDD_SERVER_OPTION_2)
{
}

//��������
CDlgServerOptionItem2::~CDlgServerOptionItem2()
{
}

//��ʼ������
BOOL CDlgServerOptionItem2::OnInitDialog()
{
	__super::OnInitDialog();

	//����ؼ�
	InitCtrlWindow();

	//��������
	CComboBox * pMinMember=(CComboBox *)GetDlgItem(IDC_MIN_ENTER_MEMBER);
	CComboBox * pMaxMember=(CComboBox *)GetDlgItem(IDC_MAX_ENTER_MEMBER);

	//��������
	if (m_pGameServiceOption->lMinEnterScore != 0)
	{
		SetDlgItemInt(IDC_MIN_ENTER_SCORE, (LONG)m_pGameServiceOption->lMinEnterScore);
	}

	if (m_pGameServiceOption->lMaxEnterScore != 0)
	{
		SetDlgItemInt(IDC_MAX_ENTER_SCORE, (LONG)m_pGameServiceOption->lMaxEnterScore);
	}

	if (m_pGameServiceOption->lMinTableScore != 0)
	{
		SetDlgItemInt(IDC_MIN_TABLE_SCORE, (LONG)m_pGameServiceOption->lMinTableScore);
	}
	
	// ��������
	if (m_pGameServiceOption->lDefPutScore != 0)
	{
		SetDlgItemInt(IDC_DEF_PUT_SCORE, (LONG)m_pGameServiceOption->lDefPutScore);
	}

	if (m_pGameServiceOption->lLowPutScore != 0)
	{
		SetDlgItemInt(IDC_LOW_PUT_SCORE, (LONG)m_pGameServiceOption->lLowPutScore);
	}

	if (m_pGameServiceOption->iExchangeRatio != 0)
	{
		SetDlgItemInt(IDC_EXCHANGE_RATIO, (int)m_pGameServiceOption->iExchangeRatio);
	}

	if (m_pGameServiceOption->lMagicExpScore != 0)
	{
		SetDlgItemInt(IDC_MAGIC_EXP, (LONG)m_pGameServiceOption->lMagicExpScore);
	}

	if (m_pGameServiceOption->lGratuityScore != 0)
	{
		SetDlgItemInt(IDC_GRATUITY, (LONG)m_pGameServiceOption->lGratuityScore);
	}

	//��Ա����
	//if (m_pGameServiceOption->cbMaxEnterMember!=0)
	{
		//��ͻ�Ա
		for (INT i=0;i<pMinMember->GetCount();i++)
		{
			if (pMinMember->GetItemData(i)==m_pGameServiceOption->cbMinEnterMember)
			{
				pMinMember->SetCurSel(i);
				break;
			}
		}

		//��߻�Ա
		for (INT i=0;i<pMaxMember->GetCount();i++)
		{
			if (pMaxMember->GetItemData(i)==m_pGameServiceOption->cbMaxEnterMember)
			{
				pMaxMember->SetCurSel(i);
				break;
			}
		}
	}

	//��ֹ����
	bool bForfendGameChat=CServerRule::IsForfendGameChat(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_FORFEND_GAME_CHAT))->SetCheck((bForfendGameChat==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//��ֹ˽��
	bool bForfendWisperOnGame=CServerRule::IsForfendWisperOnGame(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_FORFEND_WISPER_ON_GAME))->SetCheck((bForfendWisperOnGame==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//��ֹ����
	bool bForfendRoomEnter=CServerRule::IsForfendRoomEnter(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_FORFEND_ROOM_ENTER))->SetCheck((bForfendRoomEnter==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//��ֹ����
	bool bForfendGameEnter=CServerRule::IsForfendGameEnter(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_FORFEND_GAME_ENTER))->SetCheck((bForfendGameEnter==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//��ֹ�Թ�
	bool bForfendGameLookon=CServerRule::IsForfendGameLookon(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_FORFEND_GAME_LOOKON))->SetCheck((bForfendGameLookon==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//��ֹ����
	if(m_pGameServiceAttrib->wChairCount > MAX_CHAIR_GENERAL) CServerRule::SetForfendGameRule(m_pGameServiceOption->dwServerRule, true);
	if(m_pGameServiceAttrib->wChairCount > MAX_CHAIR_GENERAL) ((CButton *)GetDlgItem(IDC_FORFEND_GAME_RULE))->EnableWindow(FALSE);
	bool bForfendGameRule=CServerRule::IsForfendGameRule(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_FORFEND_GAME_RULE))->SetCheck((bForfendGameRule==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//��¼����
	bool bRecordGameScore=CServerRule::IsRecordGameScore(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_RECORD_GAME_SCORE))->SetCheck((bRecordGameScore==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//��¼����
	//bool bRecordGameTrack=CServerRule::IsRecordGameTrack(m_pGameServiceOption->dwServerRule);
	//((CButton *)GetDlgItem(IDC_RECORD_GAME_TRACK))->SetCheck((bRecordGameTrack==TRUE)?BST_CHECKED:BST_UNCHECKED);
	((CButton *)GetDlgItem(IDC_RECORD_GAME_TRACK))->SetCheck(BST_UNCHECKED);
	((CButton *)GetDlgItem(IDC_RECORD_GAME_TRACK))->EnableWindow(FALSE);

	////���вƸ���Ϸ��ÿ�ּ�ʱд�֣���¼ÿ�ֳɼ���Ĭ��Ϊֱ�ӹ�ѡ��ɫ״̬
	//if(m_pGameServiceOption->wServerType == GAME_GENRE_GOLD)
	//{
	//	CServerRule::SetImmediateWriteScore(m_pGameServiceOption->dwServerRule,true);
	//	GetDlgItem(IDC_IMMEDIATE_WRITE_SCORE)->EnableWindow(FALSE);
	//}

	//��ʱд��
	bool bImmediateWriteScore=CServerRule::IsImmediateWriteScore(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_IMMEDIATE_WRITE_SCORE))->SetCheck((bImmediateWriteScore==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//��̬�׷�
	//bool bDynamicCellScore=CServerRule::IsDynamicCellScore(m_pGameServiceOption->dwServerRule);
	//((CButton *)GetDlgItem(IDC_DYNAMIC_CELL_SCORE))->SetCheck((bDynamicCellScore==TRUE)?BST_CHECKED:BST_UNCHECKED);
	((CButton *)GetDlgItem(IDC_DYNAMIC_CELL_SCORE))->SetCheck(BST_UNCHECKED);
	((CButton *)GetDlgItem(IDC_DYNAMIC_CELL_SCORE))->EnableWindow(FALSE);

	//������Ϣ
	bool bAllowAvertCheatMode=CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_ALLOW_AVERT_CHEAT_MODE))->SetCheck((bAllowAvertCheatMode==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//����ռλ
	bool bAllowAndroidSimulate=CServerRule::IsAllowAndroidSimulate(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_ALLOW_ANDROID_SIMULATE))->SetCheck((bAllowAndroidSimulate==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//��̬����
	if (m_pGameServiceAttrib->cbDynamicJoin==TRUE)
	{
		bool bAllowDynamicJoin=CServerRule::IsAllowDynamicJoin(m_pGameServiceOption->dwServerRule);
		((CButton *)GetDlgItem(IDC_ALLOW_DYNAMIC_JOIN))->SetCheck((bAllowDynamicJoin==TRUE)?BST_CHECKED:BST_UNCHECKED);
	}

	//�������
	if (m_pGameServiceAttrib->cbAndroidUser==TRUE)
	{
		bool bAllowAndroidAttend=CServerRule::IsAllowAndroidAttend(m_pGameServiceOption->dwServerRule);
		((CButton *)GetDlgItem(IDC_ALLOW_ANDROID_ATTEND))->SetCheck((bAllowAndroidAttend==TRUE)?BST_CHECKED:BST_UNCHECKED);
	}

	//���ߴ���
	if (m_pGameServiceAttrib->cbOffLineTrustee==TRUE)
	{
		bool bAllowOffLineTrustee=CServerRule::IsAllowOffLineTrustee(m_pGameServiceOption->dwServerRule);
		if (m_pGameServiceOption->wServerType!=GAME_GENRE_MATCH)
			((CButton *)GetDlgItem(IDC_ALLOW_OFFLINE_TRUSTEE))->SetCheck((bAllowOffLineTrustee==TRUE)?BST_CHECKED:BST_UNCHECKED);
		else
			((CButton *)GetDlgItem(IDC_ALLOW_OFFLINE_TRUSTEE))->SetCheck(BST_CHECKED);
	}
	//((CButton *)GetDlgItem(IDC_ALLOW_OFFLINE_TRUSTEE))->SetCheck(BST_UNCHECKED);
	//((CButton *)GetDlgItem(IDC_ALLOW_OFFLINE_TRUSTEE))->EnableWindow(FALSE);

	return TRUE;
}

//��������
bool CDlgServerOptionItem2::SaveInputInfo()
{
	//��������
	CComboBox * pMinMember=(CComboBox *)GetDlgItem(IDC_MIN_ENTER_MEMBER);
	CComboBox * pMaxMember=(CComboBox *)GetDlgItem(IDC_MAX_ENTER_MEMBER);

	//��������
	m_pGameServiceOption->cbMinEnterMember=0;
	m_pGameServiceOption->cbMaxEnterMember=0;
	m_pGameServiceOption->lMinEnterScore=GetDlgItemInt(IDC_MIN_ENTER_SCORE);
	m_pGameServiceOption->lMaxEnterScore=GetDlgItemInt(IDC_MAX_ENTER_SCORE);
	m_pGameServiceOption->lMinTableScore=GetDlgItemInt(IDC_MIN_TABLE_SCORE);
	
	//��������
	m_pGameServiceOption->lDefPutScore = GetDlgItemInt(IDC_DEF_PUT_SCORE);
	m_pGameServiceOption->lLowPutScore = GetDlgItemInt(IDC_LOW_PUT_SCORE);
	m_pGameServiceOption->iExchangeRatio = GetDlgItemInt(IDC_EXCHANGE_RATIO);

	//��������
	m_pGameServiceOption->lMagicExpScore = GetDlgItemInt(IDC_MAGIC_EXP);
	m_pGameServiceOption->lGratuityScore = GetDlgItemInt(IDC_GRATUITY);

	//��Ա����
	if (pMinMember->GetCurSel()!=LB_ERR)
	{
		INT nCurSelect=pMinMember->GetCurSel();
		m_pGameServiceOption->cbMinEnterMember=(BYTE)pMinMember->GetItemData(nCurSelect);
	}

	//��߻�Ա
	if (pMaxMember->GetCurSel()!=LB_ERR)
	{
		INT nCurSelect=pMaxMember->GetCurSel();
		m_pGameServiceOption->cbMaxEnterMember=(BYTE)pMaxMember->GetItemData(nCurSelect);
	}

	//����Ч��
	if ((m_pGameServiceOption->lMaxEnterScore!=0L)&&(m_pGameServiceOption->lMaxEnterScore<m_pGameServiceOption->lMinEnterScore))
	{
		AfxMessageBox(TEXT("���뷿����ͳɼ�����߳ɼ����ߣ���������κ���Ҷ����ܽ���"),MB_ICONERROR);
		return false;
	}

	//����Ч��
	if ((m_pGameServiceOption->cbMaxEnterMember!=0L)&&(m_pGameServiceOption->cbMaxEnterMember<m_pGameServiceOption->cbMinEnterMember))
	{
		AfxMessageBox(TEXT("���뷿����ͻ�Ա�������߻�Ա���𻹸ߣ���������κ���Ҷ����ܽ���"),MB_ICONERROR);
		return false;
	}

	//��������
	if ((m_pGameServiceOption->lDefPutScore != 0L)&&(m_pGameServiceOption->lDefPutScore <= m_pGameServiceOption->lLowPutScore) )
	{
		AfxMessageBox(TEXT("Ĭ�ϴ������С�ڿ�ʼ����������ֵ�����������һ��ִ����쳣"), MB_ICONERROR);
		return false;
	}

	if (m_pGameServiceOption->lDefPutScore != 0L && m_pGameServiceOption->iExchangeRatio <= 0L)
	{
		AfxMessageBox(TEXT("������ֶһ������쳣�����������һ��ִ����쳣"), MB_ICONERROR);
		return false;
	}

	//��ֹ����
	CButton * pForfendGameChat=(CButton *)GetDlgItem(IDC_FORFEND_GAME_CHAT);
	CServerRule::SetForfendGameChat(m_pGameServiceOption->dwServerRule,(pForfendGameChat->GetCheck()==BST_CHECKED));

	//��ֹ˽��
	CButton * pForfendWisperOnGame=(CButton *)GetDlgItem(IDC_FORFEND_WISPER_ON_GAME);
	CServerRule::SetForfendWisperOnGame(m_pGameServiceOption->dwServerRule,(pForfendWisperOnGame->GetCheck()==BST_CHECKED));

	//��ֹ����
	CButton * pForfendRoomEnter=(CButton *)GetDlgItem(IDC_FORFEND_ROOM_ENTER);
	CServerRule::SetForfendRoomEnter(m_pGameServiceOption->dwServerRule,(pForfendRoomEnter->GetCheck()==BST_CHECKED));

	//��ֹ����
	CButton * pForfendGameEnter=(CButton *)GetDlgItem(IDC_FORFEND_GAME_ENTER);
	CServerRule::SetForfendGameEnter(m_pGameServiceOption->dwServerRule,(pForfendGameEnter->GetCheck()==BST_CHECKED));

	//��ֹ�Թ�
	CButton * pForfendGameLookon=(CButton *)GetDlgItem(IDC_FORFEND_GAME_LOOKON);
	CServerRule::SetForfendGameLookon(m_pGameServiceOption->dwServerRule,(pForfendGameLookon->GetCheck()==BST_CHECKED));

	//��ֹ����
	CButton * pForfendGameRule=(CButton *)GetDlgItem(IDC_FORFEND_GAME_RULE);
	CServerRule::SetForfendGameRule(m_pGameServiceOption->dwServerRule,(pForfendGameRule->GetCheck()==BST_CHECKED));

	//��¼����
	CButton * pRecordGameScore=(CButton *)GetDlgItem(IDC_RECORD_GAME_SCORE);
	CServerRule::SetRecordGameScore(m_pGameServiceOption->dwServerRule,(pRecordGameScore->GetCheck()==BST_CHECKED));

	//��¼����
	CButton * pRecordGameTrack=(CButton *)GetDlgItem(IDC_RECORD_GAME_TRACK);
	CServerRule::SetRecordGameTrack(m_pGameServiceOption->dwServerRule,(pRecordGameTrack->GetCheck()==BST_CHECKED));

	//��ʱд��
	CButton * pImmediateWriteScore=(CButton *)GetDlgItem(IDC_IMMEDIATE_WRITE_SCORE);
	CServerRule::SetImmediateWriteScore(m_pGameServiceOption->dwServerRule,(pImmediateWriteScore->GetCheck()==BST_CHECKED));

	//��̬�׷�
	CButton * pDynamicCellScore=(CButton *)GetDlgItem(IDC_DYNAMIC_CELL_SCORE);
	CServerRule::SetDynamicCellScore(m_pGameServiceOption->dwServerRule,(pDynamicCellScore->GetCheck()==BST_CHECKED));

	//������Ϣ
	CButton * pAvertCheatMode=(CButton *)GetDlgItem(IDC_ALLOW_AVERT_CHEAT_MODE);
	CServerRule::SetAllowAvertCheatMode(m_pGameServiceOption->dwServerRule,(pAvertCheatMode->GetCheck()==BST_CHECKED));

	//����ռλ
	CButton * pAllowAndroidSimulate=(CButton *)GetDlgItem(IDC_ALLOW_ANDROID_SIMULATE);
	CServerRule::SetAllowAndroidSimulate(m_pGameServiceOption->dwServerRule,(pAllowAndroidSimulate->GetCheck()==BST_CHECKED));

	//��̬����
	if (m_pGameServiceAttrib->cbDynamicJoin==TRUE)
	{
		CButton * pAllowDynamicJoin=(CButton *)GetDlgItem(IDC_ALLOW_DYNAMIC_JOIN);
		CServerRule::SetAllowDynamicJoin(m_pGameServiceOption->dwServerRule,(pAllowDynamicJoin->GetCheck()==BST_CHECKED));
	}

	//��������
	if (m_pGameServiceAttrib->cbAndroidUser==TRUE)
	{
		CButton * pAllowAndroidUser=(CButton *)GetDlgItem(IDC_ALLOW_ANDROID_ATTEND);
		CServerRule::SetAllowAndroidAttend(m_pGameServiceOption->dwServerRule,(pAllowAndroidUser->GetCheck()==BST_CHECKED));
	}

	//���ߴ���
	if (m_pGameServiceAttrib->cbOffLineTrustee==TRUE)
	{
		CButton * pAllowOffLineTrustee=(CButton *)GetDlgItem(IDC_ALLOW_OFFLINE_TRUSTEE);
		CServerRule::SetAllowOffLineTrustee(m_pGameServiceOption->dwServerRule,(pAllowOffLineTrustee->GetCheck()==BST_CHECKED));
	}

	return true;
}

//�����ؼ�
VOID CDlgServerOptionItem2::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//����ؼ�
VOID CDlgServerOptionItem2::InitCtrlWindow()
{
	//��������
	((CEdit *)GetDlgItem(IDC_MIN_ENTER_SCORE))->LimitText(18);
	((CEdit *)GetDlgItem(IDC_MAX_ENTER_SCORE))->LimitText(18);
	((CEdit *)GetDlgItem(IDC_MIN_TABLE_SCORE))->LimitText(18);

	//��Ա����
	CComboBox * pMinMember=(CComboBox *)GetDlgItem(IDC_MIN_ENTER_MEMBER);
	CComboBox * pMaxMember=(CComboBox *)GetDlgItem(IDC_MAX_ENTER_MEMBER);
	LPCTSTR pszMember[]={TEXT("û������"),TEXT("�»�Ա"),TEXT("���Ȼ�Ա"),TEXT("�����Ա")};

	//��Ա��Ϣ
	for (INT i=0;i<CountArray(pszMember);i++)
	{
		pMinMember->SetItemData(pMinMember->AddString(pszMember[i]),i);
		pMaxMember->SetItemData(pMaxMember->AddString(pszMember[i]),i);
	}

	//�ؼ�����
	GetDlgItem(IDC_ALLOW_DYNAMIC_JOIN)->EnableWindow((m_pGameServiceAttrib->cbDynamicJoin==TRUE)?TRUE:FALSE);
	GetDlgItem(IDC_ALLOW_ANDROID_ATTEND)->EnableWindow((m_pGameServiceAttrib->cbAndroidUser==TRUE)?TRUE:FALSE);
	GetDlgItem(IDC_ALLOW_OFFLINE_TRUSTEE)->EnableWindow((m_pGameServiceAttrib->cbOffLineTrustee==TRUE)?TRUE:FALSE);

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDlgServerOptionItem3::CDlgServerOptionItem3() : CDlgServerOptionItem(IDD_SERVER_OPTION_3)
{
}

//��������
CDlgServerOptionItem3::~CDlgServerOptionItem3()
{
}

//��ʼ������
BOOL CDlgServerOptionItem3::OnInitDialog()
{
	__super::OnInitDialog();

	//��������
	if (m_pGameServiceOption->wMinDistributeUser!=0)
	{
		SetDlgItemInt(IDC_MIN_DISTRIBUTE_USER,m_pGameServiceOption->wMinDistributeUser);
	}

	//������
	if (m_pGameServiceOption->wDistributeTimeSpace!=0)
	{
		SetDlgItemInt(IDC_DISTRIBUTE_TIME_SPACE,m_pGameServiceOption->wDistributeTimeSpace);
	}

	//�������
	if (m_pGameServiceOption->wDistributeDrawCount!=0)
	{
		SetDlgItemInt(IDC_DISTRIBUTE_DRAW_COUNT,m_pGameServiceOption->wDistributeDrawCount);
	}

	//��������
	if (m_pGameServiceOption->wMinPartakeGameUser!=0)
	{
		SetDlgItemInt(IDC_DISTRIBUTE_START_MIN_USER,m_pGameServiceOption->wMinPartakeGameUser);
	}

	//�������
	if (m_pGameServiceOption->wMaxPartakeGameUser!=0)
	{
		SetDlgItemInt(IDC_DISTRIBUTE_START_MAX_USER,m_pGameServiceOption->wMaxPartakeGameUser);
	}

	//�������
	bool bDistributeAllow=((m_pGameServiceOption->cbDistributeRule&DISTRIBUTE_ALLOW)!=0);
	((CButton *)GetDlgItem(IDC_DISTRIBUTE_ALLOW))->SetCheck((bDistributeAllow==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//����ѡ��
	bool bDistributeImmediate=((m_pGameServiceOption->cbDistributeRule&DISTRIBUTE_IMMEDIATE)!=0);
	((CButton *)GetDlgItem(IDC_DISTRIBUTE_IMMEDIATE))->SetCheck((bDistributeImmediate==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//ͬ��ѡ��
	bool bDistributeLastTable=((m_pGameServiceOption->cbDistributeRule&DISTRIBUTE_LAST_TABLE)!=0);
	((CButton *)GetDlgItem(IDC_DISTRIBUTE_LAST_TABLE))->SetCheck((bDistributeLastTable==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//��ַѡ��
	bool bDistributeSameAddress=((m_pGameServiceOption->cbDistributeRule&DISTRIBUTE_SAME_ADDRESS)!=0);
	((CButton *)GetDlgItem(IDC_DISTRIBUTE_SAME_ADDRESS))->SetCheck((bDistributeSameAddress==TRUE)?BST_CHECKED:BST_UNCHECKED);

	return TRUE;
}

//��������
bool CDlgServerOptionItem3::SaveInputInfo()
{
	//��������
	m_pGameServiceOption->wMinDistributeUser=GetDlgItemInt(IDC_MIN_DISTRIBUTE_USER);
	m_pGameServiceOption->wDistributeTimeSpace=GetDlgItemInt(IDC_DISTRIBUTE_TIME_SPACE);
	m_pGameServiceOption->wDistributeDrawCount=GetDlgItemInt(IDC_DISTRIBUTE_DRAW_COUNT);
	m_pGameServiceOption->wMinPartakeGameUser=GetDlgItemInt(IDC_DISTRIBUTE_START_MIN_USER);
	m_pGameServiceOption->wMaxPartakeGameUser=GetDlgItemInt(IDC_DISTRIBUTE_START_MAX_USER);

	//�������
	if (((CButton *)GetDlgItem(IDC_DISTRIBUTE_ALLOW))->GetCheck()==BST_CHECKED)
	{
		m_pGameServiceOption->cbDistributeRule|=DISTRIBUTE_ALLOW;
	}
	else
	{
		m_pGameServiceOption->cbDistributeRule&=~DISTRIBUTE_ALLOW;
	}

	//����ѡ��
	if (((CButton *)GetDlgItem(IDC_DISTRIBUTE_IMMEDIATE))->GetCheck()==BST_CHECKED)
	{
		m_pGameServiceOption->cbDistributeRule|=DISTRIBUTE_IMMEDIATE;
	}
	else
	{
		m_pGameServiceOption->cbDistributeRule&=~DISTRIBUTE_IMMEDIATE;
	}

	//ͬ��ѡ��
	if (((CButton *)GetDlgItem(IDC_DISTRIBUTE_LAST_TABLE))->GetCheck()==BST_CHECKED)
	{
		m_pGameServiceOption->cbDistributeRule|=DISTRIBUTE_LAST_TABLE;
	}
	else
	{
		m_pGameServiceOption->cbDistributeRule&=~DISTRIBUTE_LAST_TABLE;
	}

	//��ַѡ��
	if (((CButton *)GetDlgItem(IDC_DISTRIBUTE_SAME_ADDRESS))->GetCheck()==BST_CHECKED)
	{
		m_pGameServiceOption->cbDistributeRule|=DISTRIBUTE_SAME_ADDRESS;
	}
	else
	{
		m_pGameServiceOption->cbDistributeRule&=~DISTRIBUTE_SAME_ADDRESS;
	}

	return true;
}

//�����ؼ�
VOID CDlgServerOptionItem3::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDlgServerOptionItem4::CDlgServerOptionItem4() : CDlgServerOptionItem(IDD_SERVER_OPTION_4)
{
	//���ñ���
	m_nSelectItem = -1;
}

//��������
CDlgServerOptionItem4::~CDlgServerOptionItem4()
{
}

//��������
VOID CDlgServerOptionItem4::DoDataExchange(CDataExchange* pDX)
{
	//�󶨿ؼ�
	DDX_Control(pDX, IDC_COMBO_ENTER_HOUR, m_cbEnterHour);
	DDX_Control(pDX, IDC_COMBO_ENTER_MINUTE, m_cbEnterMinute);
	DDX_Control(pDX, IDC_COMBO_ENTER_SECOND, m_cbEnterSecond);
	DDX_Control(pDX, IDC_COMBO_LEAVE_HOUR, m_cbLeaveHour);
	DDX_Control(pDX, IDC_COMBO_LEAVE_MINUTE, m_cbLeaveMinute);
	DDX_Control(pDX, IDC_COMBO_LEAVE_SECOND, m_cbLeaveSecond);

	//�󶨿ؼ�
	DDX_Control(pDX, IDC_EDIT_ANDROID_COUNT, m_edtAndroidCount);
	DDX_Control(pDX, IDC_EDIT_TAKESCORE_MIN, m_edtTableScoreMin);
	DDX_Control(pDX, IDC_EDIT_TAKESCORE_MAX, m_edtTableScoreMax);
	DDX_Control(pDX, IDC_EDIT_ENTERINTERVAL_MIN, m_edtEnterIntervalMin);
	DDX_Control(pDX, IDC_EDIT_ENTERINTERVAL_MAX, m_edtEnterIntervalMax);
	DDX_Control(pDX, IDC_EDIT_LEAVEINTERVAL_MIN, m_edtLeaveIntervalMin);
	DDX_Control(pDX, IDC_EDIT_LEAVEINTERVAL_MAX, m_edtLeaveIntervalMax);
	DDX_Control(pDX, IDC_EDIT_SWITCHTABLE_MIN, m_edtSwitchTableMin);
	DDX_Control(pDX, IDC_EDIT_SWITCHTABLE_MAX, m_edtSwitchTableMax);

	//�󶨿ؼ�
	DDX_Control(pDX, IDC_BT_ADD, m_btAddItem);
	DDX_Control(pDX, IDC_BT_MODIFY, m_btModifyItem);
	DDX_Control(pDX, IDC_BT_DELETE, m_btDeleteItem);

	//�󶨿ؼ�
	DDX_Control(pDX, IDC_LIST_PARAMETER, m_ParemeterList);

	__super::DoDataExchange(pDX);
}

//��ʼ������
BOOL CDlgServerOptionItem4::OnInitDialog()
{
	__super::OnInitDialog();

	//��������
	m_edtAndroidCount.LimitText(3);
	m_edtTableScoreMin.LimitText(16);
	m_edtTableScoreMax.LimitText(16);
	m_edtSwitchTableMin.LimitText(4);
	m_edtSwitchTableMax.LimitText(4);
	m_edtEnterIntervalMin.LimitText(4);
	m_edtEnterIntervalMax.LimitText(4);
	m_edtLeaveIntervalMin.LimitText(4);
	m_edtLeaveIntervalMax.LimitText(4);

	//��������
	CString strItemText;

	//�����ؼ�
	for (BYTE cbIndex = 0; cbIndex < 24; cbIndex++)
	{
		strItemText.Format(TEXT("%d"), cbIndex);
		m_cbEnterHour.SetItemData(m_cbEnterHour.InsertString(cbIndex, strItemText), cbIndex);
		m_cbLeaveHour.SetItemData(m_cbLeaveHour.InsertString(cbIndex, strItemText), cbIndex);
	}

	//�����ؼ�
	for (BYTE cbIndex = 0; cbIndex < 60; cbIndex++)
	{
		strItemText.Format(TEXT("%d"), cbIndex);
		m_cbEnterMinute.SetItemData(m_cbEnterMinute.InsertString(cbIndex, strItemText), cbIndex);
		m_cbEnterSecond.SetItemData(m_cbEnterSecond.InsertString(cbIndex, strItemText), cbIndex);
		m_cbLeaveMinute.SetItemData(m_cbLeaveMinute.InsertString(cbIndex, strItemText), cbIndex);
		m_cbLeaveSecond.SetItemData(m_cbLeaveSecond.InsertString(cbIndex, strItemText), cbIndex);
	}


	//���ÿؼ�
	DWORD dwExstyle = m_ParemeterList.GetExtendedStyle();
	m_ParemeterList.SetExtendedStyle(dwExstyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | WS_EX_STATICEDGE);

	//�б�ؼ�
	m_ParemeterList.InsertColumn(0, TEXT("���α�ʶ"), LVCFMT_CENTER, 60);
	m_ParemeterList.InsertColumn(1, TEXT("������Ŀ"), LVCFMT_CENTER, 60);
	m_ParemeterList.InsertColumn(2, TEXT("����ģʽ"), LVCFMT_CENTER, 60);
	m_ParemeterList.InsertColumn(3, TEXT("����ʱ��"), LVCFMT_CENTER, 60);
	m_ParemeterList.InsertColumn(4, TEXT("�뿪ʱ��"), LVCFMT_CENTER, 60);
	m_ParemeterList.InsertColumn(5, TEXT("Я�����ٷ���"), LVCFMT_CENTER, 90);
	m_ParemeterList.InsertColumn(6, TEXT("Я��������"), LVCFMT_CENTER, 90);
	m_ParemeterList.InsertColumn(7, TEXT("������С���"), LVCFMT_CENTER, 90);
	m_ParemeterList.InsertColumn(8, TEXT("���������"), LVCFMT_CENTER, 90);
	m_ParemeterList.InsertColumn(9, TEXT("�뿪��С���"), LVCFMT_CENTER, 90);
	m_ParemeterList.InsertColumn(10, TEXT("�뿪�����"), LVCFMT_CENTER, 90);
	m_ParemeterList.InsertColumn(11, TEXT("������С����"), LVCFMT_CENTER, 90);
	m_ParemeterList.InsertColumn(12, TEXT("����������"), LVCFMT_CENTER, 90);

	//��ȡ����
	CYSArray<tagAndroidParameter> aryAndParameter;
	CServerInfoManager ServerInfoManager;
	ServerInfoManager.GetAndroidParameter(m_pGameServiceOption->wServerID, aryAndParameter);

	OnEventAndroidParenter(SUB_GP_GET_PARAMETER, aryAndParameter.GetCount(), aryAndParameter.GetData());

	return TRUE;
}

//��������
bool CDlgServerOptionItem4::SaveInputInfo()
{
	return true;
}

//�����ؼ�
VOID CDlgServerOptionItem4::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}


//����޸�
VOID CDlgServerOptionItem4::AddModifyParameter(bool bModify)
{
	//��������
	TCHAR szMessage[128] = TEXT("");
	tagAndroidParameter AndroidParameter;
	ZeroMemory(&AndroidParameter, sizeof(AndroidParameter));

	//ѡ���ж�
	if (bModify == true && m_nSelectItem == -1) return;

	//������Ŀ
	AndroidParameter.dwAndroidCount = GetDlgItemInt(IDC_EDIT_ANDROID_COUNT);

	//ͳ�ƻ�����
	DWORD dwAndroidCount = 0;
	tagAndroidParameter * pItemData = NULL;
	for (int nIndex = 0; nIndex < m_ParemeterList.GetItemCount(); nIndex++)
	{
		pItemData = (tagAndroidParameter *)m_ParemeterList.GetItemData(nIndex);
		if (pItemData != NULL)
		{
			if (bModify == false || nIndex != m_nSelectItem)
			{
				dwAndroidCount += pItemData->dwAndroidCount;
			}
		}
	}

	if (AndroidParameter.dwAndroidCount <= 0 || AndroidParameter.dwAndroidCount + dwAndroidCount > MAX_ANDROID)
	{
		_sntprintf_s(szMessage, CountArray(szMessage), TEXT("��Ǹ,ÿ������Ļ�������Ŀ���Ϊ%d��,���������룡"), MAX_ANDROID);

		//��Ϣ��ʾ
		MessageBox(szMessage, TEXT("��ʾ"));

		//���ý���
		m_edtAndroidCount.SetFocus();

		return;
	}

	//����ģʽ
	AndroidParameter.dwServiceMode |= ((CButton *)GetDlgItem(IDC_CHECK_SERVICE_MODE1))->GetCheck() == BST_CHECKED ? ANDROID_SIMULATE : 0;
	AndroidParameter.dwServiceMode |= ((CButton *)GetDlgItem(IDC_CHECK_SERVICE_MODE2))->GetCheck() == BST_CHECKED ? ANDROID_INITIATIVE : 0;
	AndroidParameter.dwServiceMode |= ((CButton *)GetDlgItem(IDC_CHECK_SERVICE_MODE3))->GetCheck() == BST_CHECKED ? ANDROID_PASSIVITY : 0;
	if (AndroidParameter.dwServiceMode == 0)
	{
		//��Ϣ��ʾ
		MessageBox(TEXT("�������ٹ�ѡһ�ַ���ģʽ��"), TEXT("��ʾ"));

		return;
	}

	//����ʱ��
	AndroidParameter.dwEnterTime = GetDlgItemInt(IDC_COMBO_ENTER_HOUR) * 3600 + GetDlgItemInt(IDC_COMBO_ENTER_MINUTE) * 60 + GetDlgItemInt(IDC_COMBO_ENTER_SECOND);
	AndroidParameter.dwLeaveTime = GetDlgItemInt(IDC_COMBO_LEAVE_HOUR) * 3600 + GetDlgItemInt(IDC_COMBO_LEAVE_MINUTE) * 60 + GetDlgItemInt(IDC_COMBO_LEAVE_SECOND);

	//������
	DWORD dwIntervalTime = 0;
	if (AndroidParameter.dwLeaveTime >= AndroidParameter.dwEnterTime)
	{
		dwIntervalTime = AndroidParameter.dwLeaveTime - AndroidParameter.dwEnterTime;
	}
	else
	{
		dwIntervalTime = AndroidParameter.dwLeaveTime + 24 * 3600 - AndroidParameter.dwEnterTime;
	}

	//���̫��
	if (dwIntervalTime < MIN_INOUTINTERVAL)
	{
		_sntprintf_s(szMessage, CountArray(szMessage), TEXT("��Ǹ,�������뿪��ʱ������Ҫ�Ƚ����ʱ����%d���ӣ�"), MIN_INOUTINTERVAL / 60);

		//��Ϣ��ʾ
		MessageBox(szMessage, TEXT("��ʾ"));

		return;
	}

	//���̫��
	if (dwIntervalTime > MAX_INOUTINTERVAL)
	{
		_sntprintf_s(szMessage, CountArray(szMessage), TEXT("��Ǹ,�������뿪�ͽ����ʱ�������%dСʱ֮�ڣ�"), MAX_INOUTINTERVAL / 3600);

		//��Ϣ��ʾ
		MessageBox(szMessage, TEXT("��ʾ"));

		return;
	}

	//��������
	TCHAR szValue[32] = TEXT("");

	//��С����
	GetDlgItemText(IDC_EDIT_TAKESCORE_MIN, szValue, CountArray(szValue));
	AndroidParameter.lTakeMinScore = _tcstoi64(szValue, NULL, 10);

	//������
	GetDlgItemText(IDC_EDIT_TAKESCORE_MAX, szValue, CountArray(szValue));
	AndroidParameter.lTakeMaxScore = _tcstoi64(szValue, NULL, 10);

	//����У��
	if (AndroidParameter.lTakeMaxScore - AndroidParameter.lTakeMinScore < 0)
	{
		//��Ϣ��ʾ
		MessageBox(TEXT("��Ǹ,������Я�������������ܵ�����С������"), TEXT("��ʾ"));

		return;
	}

	//������
	AndroidParameter.dwEnterMinInterval = GetDlgItemInt(IDC_EDIT_ENTERINTERVAL_MIN);
	AndroidParameter.dwEnterMaxInterval = GetDlgItemInt(IDC_EDIT_ENTERINTERVAL_MAX);

	//���У��
	if (AndroidParameter.dwEnterMaxInterval < AndroidParameter.dwEnterMinInterval)
	{
		//��Ϣ��ʾ
		MessageBox(TEXT("��Ǹ,�����˽��뷿�����������ܵ�����С�����"), TEXT("��ʾ"));

		return;
	}

	//�뿪���
	AndroidParameter.dwLeaveMinInterval = GetDlgItemInt(IDC_EDIT_LEAVEINTERVAL_MIN);
	AndroidParameter.dwLeaveMaxInterval = GetDlgItemInt(IDC_EDIT_LEAVEINTERVAL_MAX);

	//���У��
	if (AndroidParameter.dwLeaveMaxInterval < AndroidParameter.dwLeaveMinInterval)
	{
		//��Ϣ��ʾ
		MessageBox(TEXT("��Ǹ,�������뿪�������������ܵ�����С�����"), TEXT("��ʾ"));

		return;
	}

	//�뿪���
	AndroidParameter.dwSwitchMinInnings = GetDlgItemInt(IDC_EDIT_SWITCHTABLE_MIN);
	AndroidParameter.dwSwitchMaxInnings = GetDlgItemInt(IDC_EDIT_SWITCHTABLE_MAX);

	//����У��
	if (AndroidParameter.dwSwitchMaxInnings < AndroidParameter.dwSwitchMinInnings)
	{
		//��Ϣ��ʾ
		MessageBox(TEXT("��Ǹ,�����˻���������������С����С������"), TEXT("��ʾ"));

		return;
	}

	if (m_pGameServiceOption == NULL || m_pGameServiceOption->wServerID == 0)
	{
		MessageBox(TEXT("�봴������ɹ��������û����ˣ�"));
		return;
	}

	if (bModify == false)
	{
		//��Ŀ�ж�
		if (m_ParemeterList.GetItemCount() >= MAX_BATCH)
		{
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("��Ǹ,ÿ���������ֻ�����%d�������˲�����"), MAX_BATCH);

			MessageBox(szMessage, TEXT("��ʾ"));

			return;
		}

		//���ӻ�����
		CServerInfoManager ServerInfoManager;
		ServerInfoManager.AddAndroidParameter(m_pGameServiceOption->wServerID, &AndroidParameter);

		OnEventAndroidParenter(SUB_GP_ADD_PARAMETER, 1, &AndroidParameter);
	}
	else
	{
		if (m_nSelectItem == -1) return;
		tagAndroidParameter * pItemData = (tagAndroidParameter *)m_ParemeterList.GetItemData(m_nSelectItem);
		if (pItemData == NULL) return;

		//��ȡ����
		CServerInfoManager ServerInfoManager;
		AndroidParameter.dwBatchID = pItemData->dwBatchID;
		ServerInfoManager.ModifyAndroidParameter(m_pGameServiceOption->wServerID, &AndroidParameter);

		OnEventAndroidParenter(SUB_GP_MODIFY_PARAMETER, 1, &AndroidParameter);
	}

	return;
}

//ѡ������
VOID CDlgServerOptionItem4::SelectItem(int nIndex)
{
	//���ñ���
	m_nSelectItem = nIndex;
	if (m_nSelectItem == -1) return;

	//��ȡ����
	tagAndroidParameter * pItemData = (tagAndroidParameter *)m_ParemeterList.GetItemData(m_nSelectItem);
	if (pItemData == NULL) return;

	//������Ŀ
	SetDlgItemInt(IDC_EDIT_ANDROID_COUNT, pItemData->dwAndroidCount);

	//����ģʽ
	((CButton *)GetDlgItem(IDC_CHECK_SERVICE_MODE1))->SetCheck((pItemData->dwServiceMode&ANDROID_SIMULATE) ? BST_CHECKED : BST_UNCHECKED);
	((CButton *)GetDlgItem(IDC_CHECK_SERVICE_MODE2))->SetCheck((pItemData->dwServiceMode&ANDROID_INITIATIVE) ? BST_CHECKED : BST_UNCHECKED);
	((CButton *)GetDlgItem(IDC_CHECK_SERVICE_MODE3))->SetCheck((pItemData->dwServiceMode&ANDROID_PASSIVITY) ? BST_CHECKED : BST_UNCHECKED);

	//����Сʱ
	int nHourIndex = pItemData->dwEnterTime / 3600;
	m_cbEnterHour.SetCurSel(nHourIndex);

	//�������
	int nMinuteIndex = (pItemData->dwEnterTime - nHourIndex * 3600) / 60;
	m_cbEnterMinute.SetCurSel(nMinuteIndex);

	//��������
	int nSecondIndex = pItemData->dwEnterTime - nHourIndex * 3600 - nMinuteIndex * 60;
	m_cbEnterSecond.SetCurSel(nSecondIndex);

	//�뿪Сʱ
	nHourIndex = pItemData->dwLeaveTime / 3600;
	m_cbLeaveHour.SetCurSel(nHourIndex);

	//�뿪����
	nMinuteIndex = (pItemData->dwLeaveTime - nHourIndex * 3600) / 60;
	m_cbLeaveMinute.SetCurSel(nMinuteIndex);

	//�뿪����
	nSecondIndex = pItemData->dwLeaveTime - nHourIndex * 3600 - nMinuteIndex * 60;
	m_cbLeaveSecond.SetCurSel(nSecondIndex);

	//Я������
	TCHAR szTakeScore[32] = TEXT("");
	_sntprintf_s(szTakeScore, CountArray(szTakeScore), TEXT("%I64d"), pItemData->lTakeMinScore);
	m_edtTableScoreMin.SetWindowText(szTakeScore);
	_sntprintf_s(szTakeScore, CountArray(szTakeScore), TEXT("%I64d"), pItemData->lTakeMaxScore);
	m_edtTableScoreMax.SetWindowText(szTakeScore);

	//������
	SetDlgItemInt(IDC_EDIT_ENTERINTERVAL_MIN, pItemData->dwEnterMinInterval);
	SetDlgItemInt(IDC_EDIT_ENTERINTERVAL_MAX, pItemData->dwEnterMaxInterval);

	//�뿪���
	SetDlgItemInt(IDC_EDIT_LEAVEINTERVAL_MIN, pItemData->dwLeaveMinInterval);
	SetDlgItemInt(IDC_EDIT_LEAVEINTERVAL_MAX, pItemData->dwLeaveMaxInterval);

	//��������
	SetDlgItemInt(IDC_EDIT_SWITCHTABLE_MIN, pItemData->dwSwitchMinInnings);
	SetDlgItemInt(IDC_EDIT_SWITCHTABLE_MAX, pItemData->dwSwitchMaxInnings);
}

//��������
VOID CDlgServerOptionItem4::SetItemToParameterList(int nItemIndex, tagAndroidParameter * pAndroidParameter)
{
	//����У��
	ASSERT(nItemIndex != -1 && pAndroidParameter != NULL);
	if (nItemIndex == -1 || pAndroidParameter == NULL) return;

	//��������
	TCHAR szItemValue[32] = TEXT("");

	//��������
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pAndroidParameter->dwBatchID);
	m_ParemeterList.SetItemText(nItemIndex, 0, szItemValue);

	//��������
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pAndroidParameter->dwAndroidCount);
	m_ParemeterList.SetItemText(nItemIndex, 1, szItemValue);

	//��������
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pAndroidParameter->dwServiceMode);
	m_ParemeterList.SetItemText(nItemIndex, 2, szItemValue);

	//��������
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pAndroidParameter->dwEnterTime);
	m_ParemeterList.SetItemText(nItemIndex, 3, szItemValue);

	//��������
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pAndroidParameter->dwLeaveTime);
	m_ParemeterList.SetItemText(nItemIndex, 4, szItemValue);

	//��������
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%I64d"), pAndroidParameter->lTakeMinScore);
	m_ParemeterList.SetItemText(nItemIndex, 5, szItemValue);

	//��������
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%I64d"), pAndroidParameter->lTakeMaxScore);
	m_ParemeterList.SetItemText(nItemIndex, 6, szItemValue);

	//��������
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pAndroidParameter->dwEnterMinInterval);
	m_ParemeterList.SetItemText(nItemIndex, 7, szItemValue);

	//��������
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pAndroidParameter->dwEnterMaxInterval);
	m_ParemeterList.SetItemText(nItemIndex, 8, szItemValue);

	//��������
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pAndroidParameter->dwLeaveMinInterval);
	m_ParemeterList.SetItemText(nItemIndex, 9, szItemValue);

	//��������
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pAndroidParameter->dwLeaveMaxInterval);
	m_ParemeterList.SetItemText(nItemIndex, 10, szItemValue);

	//��������
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pAndroidParameter->dwSwitchMinInnings);
	m_ParemeterList.SetItemText(nItemIndex, 11, szItemValue);

	//��������
	_sntprintf_s(szItemValue, CountArray(szItemValue), TEXT("%d"), pAndroidParameter->dwSwitchMaxInnings);
	m_ParemeterList.SetItemText(nItemIndex, 12, szItemValue);
}

//��������
VOID CDlgServerOptionItem4::OnEventAndroidParenter(WORD wSubCommdID, WORD wParameterCount, tagAndroidParameter * pAndroidParameter)
{
	//��ȡ����
	if (wSubCommdID == SUB_GP_GET_PARAMETER)
	{
		//��������
		int nItemIndex = -1;
		tagAndroidParameter * pTempParameter = NULL;
		tagAndroidParameter * pItemData = NULL;

		//�������
		m_ParemeterList.DeleteAllItems();

		//��������
		LVITEM lVItem;
		lVItem.mask = LVIF_TEXT;
		lVItem.iSubItem = 0;
		lVItem.pszText = TEXT("");

		//ѭ������
		for (WORD wIndex = 0; wIndex < wParameterCount; wIndex++)
		{
			//��ȡ����
			pTempParameter = &pAndroidParameter[wIndex];
			if (pTempParameter == NULL) continue;

			//��������.
			lVItem.iItem = wIndex;
			nItemIndex = m_ParemeterList.InsertItem(&lVItem);
			if (nItemIndex == -1) continue;

			//��������
			SetItemToParameterList(nItemIndex, pTempParameter);

			//������Դ
			pItemData = new tagAndroidParameter;
			if (pItemData != NULL)
			{
				//��������
				CopyMemory(pItemData, pTempParameter, sizeof(tagAndroidParameter));

				//��������
				m_ParemeterList.SetItemData(nItemIndex, (DWORD)pItemData);
			}
		}

		return;
	}

	//��Ӳ���
	if (wSubCommdID == SUB_GP_ADD_PARAMETER)
	{
		//��������
		int nItemIndex = -1;

		//��������
		LVITEM lVItem;
		lVItem.mask = LVIF_TEXT;
		lVItem.iSubItem = 0;
		lVItem.pszText = TEXT("");

		//��������
		lVItem.iItem = m_ParemeterList.GetItemCount();
		nItemIndex = m_ParemeterList.InsertItem(&lVItem);

		//��������
		SetItemToParameterList(nItemIndex, pAndroidParameter);

		//������Դ
		tagAndroidParameter * pItemData = new tagAndroidParameter;
		if (pItemData != NULL)
		{
			//��������
			CopyMemory(pItemData, pAndroidParameter, sizeof(tagAndroidParameter));

			//��������
			m_ParemeterList.SetItemData(nItemIndex, (DWORD)pItemData);
		}

		return;
	}

	//�޸Ĳ���
	if (wSubCommdID == SUB_GP_MODIFY_PARAMETER)
	{
		//��������
		int nItemIndex = -1;
		tagAndroidParameter * pItemData = NULL;

		//���Ҷ���
		for (int nIndex = 0; nIndex < m_ParemeterList.GetItemCount(); nIndex++)
		{
			//��ȡ����
			tagAndroidParameter * pItemData = (tagAndroidParameter *)m_ParemeterList.GetItemData(nIndex);
			if (pItemData == NULL) continue;
			if (pAndroidParameter->dwBatchID == pItemData->dwBatchID)
			{
				//��������
				SetItemToParameterList(nIndex, pAndroidParameter);

				//��������
				CopyMemory(pItemData, pAndroidParameter, sizeof(tagAndroidParameter));

				break;
			}
		}

		return;
	}

	//ɾ������
	if (wSubCommdID == SUB_GP_DELETE_PARAMETER)
	{

		//���Ҷ���
		for (int nIndex = 0; nIndex < m_ParemeterList.GetItemCount(); nIndex++)
		{
			tagAndroidParameter * pItemData = (tagAndroidParameter *)m_ParemeterList.GetItemData(nIndex);
			if (pItemData == NULL) continue;
			if (pAndroidParameter->dwBatchID == pItemData->dwBatchID)
			{
				//��������
				m_ParemeterList.DeleteItem(nIndex);

				break;
			}
		}

		return;
	}

	return;
}

//��������
VOID CDlgServerOptionItem4::OnHdnListParameterItemclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

	//ѡ������
	SelectItem(phdr->iItem);

	*pResult = 0;
}

//ɾ������
VOID CDlgServerOptionItem4::OnLvnListParameterDeleteitem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	//��ȡ����
	tagAndroidParameter * pItemData = (tagAndroidParameter *)m_ParemeterList.GetItemData(pNMLV->iItem);
	if (pItemData != NULL)
	{
		SafeDelete(pItemData);
	}

	*pResult = 0;
}

//���̰���
VOID CDlgServerOptionItem4::OnLvnKeydownListParameter(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);

	//��������
	int nItem = -1;
	POSITION pos;

	switch (pLVKeyDow->wVKey)
	{
	case VK_DOWN:
	{
		pos = m_ParemeterList.GetFirstSelectedItemPosition();
		while (pos)
		{
			//��ȡ����
			nItem = m_ParemeterList.GetNextSelectedItem(pos);
			if (nItem == -1) break;

			//��������
			nItem = __min(nItem + 1, m_ParemeterList.GetItemCount() - 1);
			SelectItem(nItem);
			break;
		}

		break;
	}
	case VK_UP:
	{
		pos = m_ParemeterList.GetFirstSelectedItemPosition();
		while (pos)
		{
			//��ȡ����
			nItem = m_ParemeterList.GetNextSelectedItem(pos);
			if (nItem == -1) break;

			//��������
			nItem = __max(nItem - 1, 0);
			SelectItem(nItem);
			break;
		}
		break;
	}
	}

	*pResult = 0;
}

//�������
VOID CDlgServerOptionItem4::OnBnClickedBtAdd()
{
	//�������
	AddModifyParameter(false);
}

//�༭����
VOID CDlgServerOptionItem4::OnBnClickedBtModify()
{
	//�������
	AddModifyParameter(true);
}

//ɾ������
VOID CDlgServerOptionItem4::OnBnClickedBtDelete()
{
	//��ȡ����
	if (m_nSelectItem == -1) return;
	if (m_ParemeterList.GetItemCount() <= m_nSelectItem)
	{
		m_nSelectItem = -1;
		return;
	}

	//��ȡ����
	tagAndroidParameter * pItemData = (tagAndroidParameter *)m_ParemeterList.GetItemData(m_nSelectItem);
	if (pItemData == NULL) return;

	//��ȡʱ��
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);
	DWORD dwTodayTickCount = SystemTime.wHour * 3600 + SystemTime.wMinute * 60 + SystemTime.wSecond;

	//ʱ���ж�
	if (dwTodayTickCount >= pItemData->dwEnterTime && dwTodayTickCount <= pItemData->dwLeaveTime)
	{
		//ѡ����ʾ
		INT nRetCode = MessageBox(TEXT("���������������ڷ���״̬,��ȷ��Ҫɾ����"), TEXT("����"), MB_YESNO);

		//��ʱ���˳�
		if (nRetCode == IDNO) return;
	}

	if (m_pGameServiceOption==NULL||m_pGameServiceOption->wServerID == 0)
	{
		MessageBox(TEXT("�봴������ɹ��������û����ˣ�"));
		return;
	}

	//ɾ������
	CServerInfoManager ServerInfoManager;
	ServerInfoManager.DeleteAndroidParameter(m_pGameServiceOption->wServerID, pItemData->dwBatchID);
	OnEventAndroidParenter(SUB_GP_DELETE_PARAMETER, 1, pItemData);

	//���ñ���
	m_nSelectItem = -1;

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDlgServerOptionItemCustom::CDlgServerOptionItemCustom() : CDlgServerOptionItem(IDD_SERVER_OPTION_6)
{
	//���ñ���
	m_hCustomRule=NULL;
	m_pIGameServiceCustomRule=NULL;

	return;
}

//��������
CDlgServerOptionItemCustom::~CDlgServerOptionItemCustom()
{
}

//��ʼ������
BOOL CDlgServerOptionItemCustom::OnInitDialog()
{
	__super::OnInitDialog();

	//��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//���ñ���
	m_pIGameServiceCustomRule=m_pDlgServerWizard->m_pIGameServiceCustomRule;

	//��������
	WORD wCustonSize=sizeof(m_pGameServiceOption->cbCustomRule);
	m_hCustomRule=m_pIGameServiceCustomRule->CreateCustomRule(this,rcClient,m_pGameServiceOption->cbCustomRule,wCustonSize);

	return TRUE;
}

//��������
bool CDlgServerOptionItemCustom::SaveInputInfo()
{
	//��������
	if (m_hCustomRule!=NULL)
	{
		//��������
		WORD wCustonSize=sizeof(m_pGameServiceOption->cbCustomRule);
		bool bSuccess=m_pIGameServiceCustomRule->SaveCustomRule(m_pGameServiceOption->cbCustomRule,wCustonSize);

		return bSuccess;
	}

	return true;
}

//�����ؼ�
VOID CDlgServerOptionItemCustom::RectifyControl(INT nWidth, INT nHeight)
{
	//����λ��
	if (m_hCustomRule!=NULL)
	{
		::SetWindowPos(m_hCustomRule,NULL,0,0,nWidth,nHeight,SWP_NOMOVE|SWP_NOZORDER);
	}

	return;
}

//������Ϣ
VOID CDlgServerOptionItemCustom::OnNcDestroy()
{
	//�رմ���
	if (m_hCustomRule!=NULL)
	{
		::DestroyWindow(m_hCustomRule);
	}

	//���ñ���
	m_hCustomRule=NULL;
	m_pIGameServiceCustomRule=NULL;

	__super::OnNcDestroy();
}

//������Ϣ
VOID CDlgServerOptionItemCustom::OnSetFocus(CWnd * pNewWnd)
{
	__super::OnSetFocus(pNewWnd);

	//���ý���
	if (m_hCustomRule!=NULL)
	{
		::SetFocus(m_hCustomRule);
	}

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDlgServerWizardItem::CDlgServerWizardItem(UINT nIDTemplate) : CDialog(nIDTemplate)
{
	//���ñ���
	m_pDlgServerWizard=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	return;
}

//��������
CDlgServerWizardItem::~CDlgServerWizardItem()
{
}

//��������
bool CDlgServerWizardItem::SaveItemData()
{
	//��������
	if ((m_hWnd!=NULL)&&(SaveInputInfo()==false))
	{
		return false;
	}

	return true;
}

//������
bool CDlgServerWizardItem::ShowWizardItem(const CRect & rcRect, CWnd * pParentWnd)
{
	//��������
	if (m_hWnd==NULL) 
	{
		//������Դ
		AfxSetResourceHandle(GetModuleHandle(MODULE_MANAGER_DLL_NAME));

		//��������
		Create(m_lpszTemplateName,pParentWnd);
		SetWindowPos(NULL,rcRect.left,rcRect.top,rcRect.Width(),rcRect.Height(),SWP_NOZORDER|SWP_NOACTIVATE);

		//������Դ
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}

	//��ʾ����
	ShowWindow(SW_SHOW);

	return true;
}

//ȷ������
VOID CDlgServerWizardItem::OnOK()
{ 
	//Ͷ����Ϣ
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDOK,0),0);

	return;
}

//ȡ����Ϣ
VOID CDlgServerWizardItem::OnCancel()
{ 
	//Ͷ����Ϣ
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDCANCEL,0),0);

	return;
}

//λ����Ϣ
VOID CDlgServerWizardItem::OnSize(UINT nType, INT cx, INT cy)
{
	__super::OnSize(nType, cx, cy);

	//�����ؼ�
	RectifyControl(cx,cy);

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDlgServerWizardItem1::CDlgServerWizardItem1() : CDlgServerWizardItem(IDD_SERVER_WIZARD_1)
{
}

//��������
CDlgServerWizardItem1::~CDlgServerWizardItem1()
{
}

//�ؼ���
VOID CDlgServerWizardItem1::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROMPT, m_StaticPrompt);
	DDX_Control(pDX, IDC_MODULE_LIST, m_ModuleListControl);
}

//��ʼ������
BOOL CDlgServerWizardItem1::OnInitDialog()
{
	__super::OnInitDialog();

	//�����б�
	LoadDBModuleItem();

	return TRUE;
}

//��������
bool CDlgServerWizardItem1::SaveInputInfo()
{
	//��������
	tagGameModuleInfo * pGameModuleInfo=NULL;
	POSITION Position=m_ModuleListControl.GetFirstSelectedItemPosition();
	
	//��ȡѡ��
	if (Position!=NULL)
	{
		INT nListItem=m_ModuleListControl.GetNextSelectedItem(Position);
		pGameModuleInfo=(tagGameModuleInfo *)m_ModuleListControl.GetItemData(nListItem);
	}

	//ѡ���ж�
	if (pGameModuleInfo==NULL)
	{
		AfxMessageBox(TEXT("�����ȴ���Ϸ�б���ѡ����Ϸ���"),MB_ICONERROR);
		return false;
	}

	//��Ϸģ��
	m_GameServiceManager.CloseInstance();
	m_GameServiceManager.SetModuleCreateInfo(pGameModuleInfo->szServerDLLName,GAME_SERVICE_CREATE_NAME);

	//�����ж�
	if (pGameModuleInfo->dwNativeVersion==0L)
	{
		//������ʾ
		TCHAR szString[512]=TEXT("");
		_sntprintf_s(szString, CountArray(szString), TEXT("[ %s ] ���������û�а�װ�����Ȱ�װ�������"), pGameModuleInfo->szGameName);

		//��ʾ��Ϣ
		AfxMessageBox(szString,MB_ICONERROR);

		return false;
	}

	//�����ж�
	if (pGameModuleInfo->dwNativeVersion!=pGameModuleInfo->dwServerVersion)
	{
		//������ʾ
		TCHAR szString[512]=TEXT("");
		_sntprintf_s(szString, CountArray(szString), TEXT("[ %s ] ������������ˣ��Ƿ񻹼�������������"), pGameModuleInfo->szGameName);

		//��ʾ��Ϣ
		if (AfxMessageBox(szString,MB_ICONWARNING|MB_YESNO|MB_DEFBUTTON2)!=IDYES) return false;
	}

	//����ģ��
	if (m_GameServiceManager.CreateInstance()==false)
	{
		//������ʾ
		TCHAR szString[512]=TEXT("");
		_sntprintf_s(szString, CountArray(szString), TEXT("[ %s ] �����������ʧ�ܣ�������Ϸ����ʧ��"), pGameModuleInfo->szGameName);

		//��ʾ��Ϣ
		AfxMessageBox(szString,MB_ICONERROR);

		return false;
	}

	//����ģ��
	m_pDlgServerWizard->SetWizardParameter(m_GameServiceManager.GetInterface(),NULL);

	////Ĭ������
	//lstrcpyn(m_pGameServiceOption->szDataBaseName,pGameModuleInfo->szDataBaseName,CountArray(m_pGameServiceOption->szDataBaseName));
	//lstrcpyn(m_pGameServiceOption->szDataBaseAddr,pGameModuleInfo->szDataBaseAddr,CountArray(m_pGameServiceOption->szDataBaseAddr));

	return true;
}

//�����ؼ�
VOID CDlgServerWizardItem1::RectifyControl(INT nWidth, INT nHeight)
{
	//������ʾ
	if (m_StaticPrompt.m_hWnd!=NULL)
	{
		m_StaticPrompt.SetWindowPos(NULL,5,8,nWidth-10,12,SWP_NOZORDER|SWP_NOCOPYBITS);
	}

	//�����б�
	if (m_ModuleListControl.m_hWnd!=NULL)
	{
		m_ModuleListControl.SetWindowPos(NULL,5,28,nWidth-10,nHeight-28,SWP_NOZORDER|SWP_NOCOPYBITS);
	}

	return;
}

//�����б�
bool CDlgServerWizardItem1::LoadDBModuleItem()
{
	//������Ϣ
	if (m_ModuleInfoManager.LoadGameModuleInfo(m_ModuleInfoBuffer)==true)
	{
		//�����б�
		m_ModuleListControl.DeleteAllItems();

		//��������
		POSITION Position=m_ModuleInfoBuffer.m_GameModuleInfoMap.GetStartPosition();

		//ö��ģ��
		while (Position!=NULL)
		{
			//��ȡ����
			WORD wModuleID=0L;
			tagGameModuleInfo * pGameModuleInfo=NULL;
			m_ModuleInfoBuffer.m_GameModuleInfoMap.GetNextAssoc(Position,wModuleID,pGameModuleInfo);

			//�����б�
			ASSERT(pGameModuleInfo!=NULL);
			if (pGameModuleInfo!=NULL) m_ModuleListControl.InsertModuleInfo(pGameModuleInfo);
		}
	}

	return false;
}

//˫���б�
VOID CDlgServerWizardItem1::OnNMDblclkModuleList(NMHDR * pNMHDR, LRESULT * pResult)
{
	//��������
	if (((NMITEMACTIVATE *)pNMHDR)->iItem!=LB_ERR)
	{
		//��������
		INT nListItem=((NMITEMACTIVATE *)pNMHDR)->iItem;
		tagGameModuleInfo * pGameModuleInfo=(tagGameModuleInfo *)m_ModuleListControl.GetItemData(nListItem);

		//Ͷ����Ϣ
		if (pGameModuleInfo->dwNativeVersion!=0L)
		{
			GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDC_NEXT,0),0);
		}

		return;
	}

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDlgServerWizardItem2::CDlgServerWizardItem2() : CDlgServerWizardItem(IDD_SERVER_WIZARD_2)
{
	//���ñ���
	m_wItemCount=0;
	m_wActiveIndex=INVALID_WORD;
	ZeroMemory(m_pOptionItem,sizeof(m_pOptionItem));

	return;
}

//��������
CDlgServerWizardItem2::~CDlgServerWizardItem2()
{
}

//�ؼ���
VOID CDlgServerWizardItem2::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_CTRL, m_TabCtrl);
}

//��ʼ������
BOOL CDlgServerWizardItem2::OnInitDialog()
{
	__super::OnInitDialog();

	//���ñ���
	m_wItemCount=0;
	m_wActiveIndex=INVALID_WORD;

	//��������
	m_pOptionItem[m_wItemCount++]=&m_ServerOptionItem1;
	m_TabCtrl.InsertItem(ITEM_SERVER_OPTION_1,TEXT("��������"));

	//����ѡ��
	m_pOptionItem[m_wItemCount++]=&m_ServerOptionItem2;
	m_TabCtrl.InsertItem(ITEM_SERVER_OPTION_2,TEXT("����ѡ��"));

	//����Ȩ��
	m_pOptionItem[m_wItemCount++]=&m_ServerOptionItem3;
	m_TabCtrl.InsertItem(ITEM_SERVER_OPTION_3,TEXT("����ѡ��"));

	//����������
	m_pOptionItem[m_wItemCount++] = &m_ServerOptionItem4;
	m_TabCtrl.InsertItem(ITEM_SERVER_OPTION_4, TEXT("����������"));

	//������
	if (m_pDlgServerWizard->m_pIGameServiceCustomRule!=NULL)
	{
		m_pOptionItem[m_wItemCount++]=&m_ServerOptionItemCustom;
		m_TabCtrl.InsertItem(ITEM_SERVER_OPTION_CUSTOM,TEXT("������"));
	}

	//��������
	ActiveOptionItem(0);

	return TRUE;
}

//��������
bool CDlgServerWizardItem2::SaveInputInfo()
{
	//��������
	for (INT i=0;i<m_wItemCount;i++)
	{
		if (m_pOptionItem[i]->SaveItemData()==false)
		{
			ActiveOptionItem(i);
			return false;
		}
	}

	return true;
}

//�����ؼ�
VOID CDlgServerWizardItem2::RectifyControl(INT nWidth, INT nHeight)
{
	//����ѡ��
	if (m_TabCtrl.m_hWnd!=NULL)
	{
		m_TabCtrl.SetWindowPos(NULL,5,5,nWidth-10,nHeight-5,SWP_NOZORDER|SWP_NOCOPYBITS);
	}

	//����ѡ��
	if ((m_TabCtrl.m_hWnd!=NULL)&&(m_wActiveIndex!=INVALID_WORD))
	{
		//��ȡλ��
		CRect rcItemRect;
		m_TabCtrl.GetWindowRect(&rcItemRect);

		//����λ��
		m_TabCtrl.ScreenToClient(&rcItemRect);
		m_TabCtrl.AdjustRect(FALSE,&rcItemRect);

		//�ƶ�λ��
		m_pOptionItem[m_wActiveIndex]->MoveWindow(&rcItemRect);
	}

	return;
}

//������
bool CDlgServerWizardItem2::ActiveOptionItem(WORD wIndex)
{
	//�ж�״̬
	if (m_wActiveIndex==wIndex)
	{
		m_pOptionItem[m_wActiveIndex]->SetFocus();
		return true;
	}

	//�������
	CDlgServerOptionItem * pItemOption=NULL;
	if (m_wActiveIndex<m_wItemCount) pItemOption=m_pOptionItem[m_wActiveIndex];

	//��ȡλ��
	CRect rcItemRect;
	m_TabCtrl.GetWindowRect(&rcItemRect);

	//����λ��
	m_TabCtrl.ScreenToClient(&rcItemRect);
	m_TabCtrl.AdjustRect(FALSE,&rcItemRect);

	//��������
	m_pOptionItem[wIndex]->m_pDlgServerWizard=m_pDlgServerWizard;
	m_pOptionItem[wIndex]->m_pGameServiceAttrib=m_pGameServiceAttrib;
	m_pOptionItem[wIndex]->m_pGameServiceOption=m_pGameServiceOption;

	//��������
	m_wActiveIndex=wIndex;
	m_pOptionItem[m_wActiveIndex]->ShowOptionItem(rcItemRect,&m_TabCtrl);

	//��������
	m_pOptionItem[m_wActiveIndex]->SetFocus();
	if (pItemOption!=NULL) pItemOption->ShowWindow(SW_HIDE);

	//����ѡ��
	if (m_TabCtrl.GetCurSel()!=wIndex) m_TabCtrl.SetCurSel(wIndex);

	return true;
}

//������Ϣ
VOID CDlgServerWizardItem2::OnSetFocus(CWnd * pNewWnd)
{
	__super::OnSetFocus(pNewWnd);

	//���ý���
	if ((m_wActiveIndex!=INVALID_WORD)&&(m_pOptionItem[m_wActiveIndex]->m_hWnd!=NULL))
	{
		m_pOptionItem[m_wActiveIndex]->SetFocus();
	}

	return;
}

//ѡ��ı�
VOID CDlgServerWizardItem2::OnTcnSelchangeTabCtrl(NMHDR * pNMHDR, LRESULT * pResult)
{
	//����ҳ��
	switch (m_TabCtrl.GetCurSel())
	{
	case ITEM_SERVER_OPTION_1:{ ActiveOptionItem(0); break; }
	case ITEM_SERVER_OPTION_2:{ ActiveOptionItem(1); break; }
	case ITEM_SERVER_OPTION_3:{ ActiveOptionItem(2); break; }
	case ITEM_SERVER_OPTION_4:{ ActiveOptionItem(3); break; }
	case ITEM_SERVER_OPTION_CUSTOM:{ ActiveOptionItem(4); break; }
	}

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDlgServerWizard::CDlgServerWizard() : CDialog(IDD_SERVER_WIZARD_MAIN)
{
	//���ñ���
	m_wActiveIndex=INVALID_WORD;
	m_pWizardItem[0]=&m_ServerWizardItem1;
	m_pWizardItem[1]=&m_ServerWizardItem2;

	//�ӿڱ���
	m_pIGameServiceManager=NULL;
	m_pIGameServiceCustomRule=NULL;

	//������Ϣ
	ZeroMemory(&m_ModuleInitParameter,sizeof(m_ModuleInitParameter));

	return;
}

//��������
CDlgServerWizard::~CDlgServerWizard()
{
}

//��ʼ������
BOOL CDlgServerWizard::OnInitDialog()
{
	__super::OnInitDialog();

	//������Դ
	AfxSetResourceHandle(GetModuleHandle(NULL));

	//������
	ActiveWizardItem((m_pIGameServiceManager==NULL)?0:1);

	//�������
	if (m_pIGameServiceManager!=NULL)
	{
		SetDlgItemInt(IDC_GAME_ID,m_ModuleInitParameter.GameServiceAttrib.wKindID);
		SetDlgItemText(IDC_GAME_NAME,m_ModuleInitParameter.GameServiceAttrib.szGameName);
		SetDlgItemInt(IDC_CHAIR_COUNT,m_ModuleInitParameter.GameServiceAttrib.wChairCount);
		SetDlgItemText(IDC_DATABASE_NAME,m_ModuleInitParameter.GameServiceAttrib.szDataBaseName);
		SetDlgItemText(IDC_CLIENT_EXE_NAME,m_ModuleInitParameter.GameServiceAttrib.szClientEXEName);
		SetDlgItemText(IDC_SERVICE_DLL_NAME,m_ModuleInitParameter.GameServiceAttrib.szServerDLLName);
	}

	return FALSE;
}

//ȷ������
VOID CDlgServerWizard::OnOK()
{
	if ((m_wActiveIndex+1)<CountArray(m_pWizardItem))
	{
		OnBnClickedNext();
	}
	else
	{
		OnBnClickedFinish();
	}

	return;
}

//��������
bool CDlgServerWizard::CreateGameServer()
{
	//������Դ
	AfxSetResourceHandle(GetModuleHandle(MODULE_MANAGER_DLL_NAME));

	//���÷���
	if (DoModal()==IDOK)
	{
		return true;
	}

	return false;
}

//���ýӿ�
VOID CDlgServerWizard::SetWizardParameter(IGameServiceManager * pIGameServiceManager, tagGameServiceOption * pGameServiceOption)
{
	//��������
	if (pGameServiceOption==NULL)
	{
		for (WORD i=1;i<CountArray(m_pWizardItem);i++)
		{
			if ((m_pWizardItem[i]!=NULL)&&(m_pWizardItem[i]->m_hWnd!=NULL))
			{
				m_pWizardItem[i]->DestroyWindow();
			}
		}
	}

	//���ñ���
	m_pIGameServiceCustomRule=NULL;
	ZeroMemory(&m_ModuleInitParameter,sizeof(m_ModuleInitParameter));

	//��ȡ����
	ASSERT(pIGameServiceManager!=NULL);
	pIGameServiceManager->GetServiceAttrib(m_ModuleInitParameter.GameServiceAttrib);

	//���ýӿ�
	m_pIGameServiceManager=pIGameServiceManager;
	m_pIGameServiceCustomRule=QUERY_OBJECT_PTR_INTERFACE(m_pIGameServiceManager,IGameServiceCustomRule);

	//�������
	if (m_hWnd!=NULL)
	{
		SetDlgItemInt(IDC_GAME_ID,m_ModuleInitParameter.GameServiceAttrib.wKindID);
		SetDlgItemText(IDC_GAME_NAME,m_ModuleInitParameter.GameServiceAttrib.szGameName);
		SetDlgItemInt(IDC_CHAIR_COUNT,m_ModuleInitParameter.GameServiceAttrib.wChairCount);
		SetDlgItemText(IDC_DATABASE_NAME,m_ModuleInitParameter.GameServiceAttrib.szDataBaseName);
		SetDlgItemText(IDC_CLIENT_EXE_NAME,m_ModuleInitParameter.GameServiceAttrib.szClientEXEName);
		SetDlgItemText(IDC_SERVICE_DLL_NAME,m_ModuleInitParameter.GameServiceAttrib.szServerDLLName);
	}

	//���ù���
	if (pGameServiceOption==NULL)
	{
		//�Զ�����
		if (m_pIGameServiceCustomRule!=NULL)
		{
			WORD wCustomSize=sizeof(m_ModuleInitParameter.GameServiceOption.cbCustomRule);
			m_pIGameServiceCustomRule->DefaultCustomRule(m_ModuleInitParameter.GameServiceOption.cbCustomRule,wCustomSize);
		}

		//��������
		ASSERT(m_pIGameServiceManager!=NULL);
		m_pIGameServiceManager->RectifyParameter(m_ModuleInitParameter.GameServiceOption);
	}
	else
	{
		//��������
		CopyMemory(&m_ModuleInitParameter.GameServiceOption,pGameServiceOption,sizeof(tagGameServiceOption));
	}

	return;
}

//������
bool CDlgServerWizard::ActiveWizardItem(WORD wIndex)
{
	//�ж�״̬
	if (m_wActiveIndex==wIndex)
	{
		m_pWizardItem[m_wActiveIndex]->SetFocus();
		return true;
	}

	//�������
	CDlgServerWizardItem * pItemWizard=NULL;
	if (m_wActiveIndex<CountArray(m_pWizardItem))
	{
		//���ñ���
		pItemWizard=m_pWizardItem[m_wActiveIndex];

		//��������
		if ((wIndex>m_wActiveIndex)&&(m_pWizardItem[m_wActiveIndex]->SaveItemData()==false)) return false;
	}

	//��ȡλ��
	CRect rcItemRect;
	GetDlgItem(IDC_ITEM_FRAME)->GetWindowRect(rcItemRect);
	ScreenToClient(&rcItemRect);

	//��������
	m_pWizardItem[wIndex]->m_pDlgServerWizard=this;
	m_pWizardItem[wIndex]->m_pGameServiceAttrib=&m_ModuleInitParameter.GameServiceAttrib;
	m_pWizardItem[wIndex]->m_pGameServiceOption=&m_ModuleInitParameter.GameServiceOption;

	//��������
	m_wActiveIndex=wIndex;
	m_pWizardItem[m_wActiveIndex]->ShowWizardItem(rcItemRect,this);

	//��������
	m_pWizardItem[m_wActiveIndex]->SetFocus();
	if (pItemWizard!=NULL) pItemWizard->ShowWindow(SW_HIDE);

	//�������
	CButton * pButtonLast=(CButton * )GetDlgItem(IDC_LAST);
	CButton * pButtonNext=(CButton * )GetDlgItem(IDC_NEXT);
	CButton * pButtonFinish=(CButton * )GetDlgItem(IDC_FINISH);

	//���Ƚ���
	pButtonNext->EnableWindow(((m_wActiveIndex+1)<CountArray(m_pWizardItem))?TRUE:FALSE);
	pButtonFinish->EnableWindow(((m_wActiveIndex+1)==CountArray(m_pWizardItem))?TRUE:FALSE);
	pButtonLast->EnableWindow(((m_wActiveIndex>1)||((m_ModuleInitParameter.GameServiceOption.wServerID==0)&&(m_wActiveIndex>0)))?TRUE:FALSE);

	//�������
	TCHAR szTitle[128]=TEXT("");
	_sntprintf_s(szTitle, CountArray(szTitle), TEXT("���������� --- [ ���� %d ]"), m_wActiveIndex + 1);
	
	//���ñ���
	SetWindowText(szTitle);

	return true;
}

//��һ��
VOID CDlgServerWizard::OnBnClickedLast()
{
	//Ч�����
	ASSERT(m_wActiveIndex>0);
	ASSERT(m_wActiveIndex<CountArray(m_pWizardItem));

	//�л�ҳ��
	ActiveWizardItem(m_wActiveIndex-1);

	return;
}

//��һ��
VOID CDlgServerWizard::OnBnClickedNext()
{
	//Ч�����
	ASSERT((m_wActiveIndex+1)<CountArray(m_pWizardItem));

	//����ҳ��
	ActiveWizardItem(m_wActiveIndex+1);

	return;
}

//��ɰ�ť
VOID CDlgServerWizard::OnBnClickedFinish()
{
	//��������
	ASSERT(m_wActiveIndex>=1);
	if (m_pWizardItem[m_wActiveIndex]->SaveItemData()==false) return;

	//������Ϣ
	tagGameServerCreate GameServerCreate;
	ZeroMemory(&GameServerCreate,sizeof(GameServerCreate));

	//��������
	ASSERT(m_pIGameServiceManager!=NULL);
	m_pIGameServiceManager->RectifyParameter(m_ModuleInitParameter.GameServiceOption);

	//��������
	GameServerCreate.wGameID=m_ModuleInitParameter.GameServiceAttrib.wKindID;
	GameServerCreate.wServerID=m_ModuleInitParameter.GameServiceOption.wServerID;

	//�ҽ�����
	GameServerCreate.wKindID=m_ModuleInitParameter.GameServiceOption.wKindID;
	//GameServerCreate.wNodeID=m_ModuleInitParameter.GameServiceOption.wNodeID;
	GameServerCreate.wSortID=m_ModuleInitParameter.GameServiceOption.wSortID;

	//˰������
	GameServerCreate.lCellScore=m_ModuleInitParameter.GameServiceOption.lCellScore;
	GameServerCreate.wRevenueRatio=m_ModuleInitParameter.GameServiceOption.wRevenueRatio;
	GameServerCreate.lServiceScore=m_ModuleInitParameter.GameServiceOption.lServiceScore;

	//��������
	GameServerCreate.lRestrictScore=m_ModuleInitParameter.GameServiceOption.lRestrictScore;
	GameServerCreate.lMinTableScore=m_ModuleInitParameter.GameServiceOption.lMinTableScore;
	GameServerCreate.lMinEnterScore=m_ModuleInitParameter.GameServiceOption.lMinEnterScore;
	GameServerCreate.lMaxEnterScore=m_ModuleInitParameter.GameServiceOption.lMaxEnterScore;

	//����������
	GameServerCreate.lDefPutScore = m_ModuleInitParameter.GameServiceOption.lDefPutScore;
	GameServerCreate.lLowPutScore = m_ModuleInitParameter.GameServiceOption.lLowPutScore;
	GameServerCreate.iExchangeRatio = m_ModuleInitParameter.GameServiceOption.iExchangeRatio;

	//��������
	GameServerCreate.lMagicExpScore = m_ModuleInitParameter.GameServiceOption.lMagicExpScore;
	GameServerCreate.lGratuityScore = m_ModuleInitParameter.GameServiceOption.lGratuityScore;
	GameServerCreate.dwWinExperience = m_ModuleInitParameter.GameServiceOption.dwWinExperience;
	GameServerCreate.dwFailExperience = m_ModuleInitParameter.GameServiceOption.dwFailExperience;

	//��Ա����
	GameServerCreate.cbMaxEnterMember=m_ModuleInitParameter.GameServiceOption.cbMaxEnterMember;
	GameServerCreate.cbMaxEnterMember=m_ModuleInitParameter.GameServiceOption.cbMaxEnterMember;

	//��������
	GameServerCreate.dwServerRule=m_ModuleInitParameter.GameServiceOption.dwServerRule;
	GameServerCreate.dwAttachUserRight=m_ModuleInitParameter.GameServiceOption.dwAttachUserRight;

	//��������
	GameServerCreate.wMaxPlayer=m_ModuleInitParameter.GameServiceOption.wMaxPlayer;
	GameServerCreate.wTableCount=m_ModuleInitParameter.GameServiceOption.wTableCount;
	GameServerCreate.wServerType=m_ModuleInitParameter.GameServiceOption.wServerType;
	GameServerCreate.wServerKind=m_ModuleInitParameter.GameServiceOption.wServerKind;
	GameServerCreate.wServerPort=m_ModuleInitParameter.GameServiceOption.wServerPort;
	lstrcpyn(GameServerCreate.szServerName,m_ModuleInitParameter.GameServiceOption.szServerName,CountArray(GameServerCreate.szServerName));
	lstrcpyn(GameServerCreate.szServerPasswd,m_ModuleInitParameter.GameServiceOption.szServerPasswd,CountArray(GameServerCreate.szServerPasswd));

	//��������
	GameServerCreate.cbDistributeRule=m_ModuleInitParameter.GameServiceOption.cbDistributeRule;
	GameServerCreate.wMinDistributeUser=m_ModuleInitParameter.GameServiceOption.wMinDistributeUser;
	GameServerCreate.wDistributeTimeSpace=m_ModuleInitParameter.GameServiceOption.wDistributeTimeSpace;
	GameServerCreate.wDistributeDrawCount=m_ModuleInitParameter.GameServiceOption.wDistributeDrawCount;
	GameServerCreate.wMinPartakeGameUser=m_ModuleInitParameter.GameServiceOption.wMinPartakeGameUser;
	GameServerCreate.wMaxPartakeGameUser=m_ModuleInitParameter.GameServiceOption.wMaxPartakeGameUser;

	////������Ϣ
	//lstrcpyn(GameServerCreate.szDataBaseName,m_ModuleInitParameter.GameServiceOption.szDataBaseName,CountArray(GameServerCreate.szDataBaseName));
	//lstrcpyn(GameServerCreate.szDataBaseAddr,m_ModuleInitParameter.GameServiceOption.szDataBaseAddr,CountArray(GameServerCreate.szDataBaseAddr));

	////���������
	//lstrcpyn(GameServerCreate.szCacheServerAddr, m_ModuleInitParameter.GameServiceOption.szCacheServerAddr, CountArray(GameServerCreate.szCacheServerAddr));
	//GameServerCreate.wCacheServerPort = m_ModuleInitParameter.GameServiceOption.wCacheServerPort;

	//��չ����
	CYSService::GetMachineID(GameServerCreate.szServiceMachine);
	CopyMemory(GameServerCreate.cbCustomRule,m_ModuleInitParameter.GameServiceOption.cbCustomRule,sizeof(GameServerCreate.cbCustomRule));

	//��������
	tagGameServerInfo GameServerResult;
	CServerInfoManager ServerInfoManager;
	
	//���뷿��
	if (GameServerCreate.wServerID==0)
	{
		if (ServerInfoManager.InsertGameServer(&GameServerCreate,GameServerResult)==false) return;

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("��Ϸ���䴴���ɹ�"),TraceLevel_Normal);
	}

	//�޸ķ���
	if (GameServerCreate.wServerID!=0)
	{
		//�޸ķ���
		if (ServerInfoManager.ModifyGameServer(&GameServerCreate,GameServerResult)==false) return;

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("��Ϸ�������óɹ�"),TraceLevel_Normal);
	}

	//��������
	m_ModuleInitParameter.GameServiceOption.wServerID=GameServerResult.wServerID;

	//�ҽ�����
	m_ModuleInitParameter.GameServiceOption.wKindID=GameServerResult.wKindID;
	//m_ModuleInitParameter.GameServiceOption.wNodeID=GameServerResult.wNodeID;
	m_ModuleInitParameter.GameServiceOption.wSortID=GameServerResult.wSortID;

	//˰������
	m_ModuleInitParameter.GameServiceOption.lCellScore=GameServerResult.lCellScore;
	m_ModuleInitParameter.GameServiceOption.wRevenueRatio=m_ModuleInitParameter.GameServiceOption.wRevenueRatio;
	m_ModuleInitParameter.GameServiceOption.lServiceScore=m_ModuleInitParameter.GameServiceOption.lServiceScore;

	//��������
	m_ModuleInitParameter.GameServiceOption.lRestrictScore=GameServerResult.lRestrictScore;
	m_ModuleInitParameter.GameServiceOption.lMinTableScore=GameServerResult.lMinTableScore;
	m_ModuleInitParameter.GameServiceOption.lMinEnterScore=GameServerResult.lMinEnterScore;
	m_ModuleInitParameter.GameServiceOption.lMaxEnterScore=GameServerResult.lMaxEnterScore;

	//�����������
	m_ModuleInitParameter.GameServiceOption.lDefPutScore = GameServerResult.lDefPutScore;
	m_ModuleInitParameter.GameServiceOption.lLowPutScore = GameServerResult.lLowPutScore;
	m_ModuleInitParameter.GameServiceOption.iExchangeRatio = GameServerResult.iExchangeRatio;

	//��������
	m_ModuleInitParameter.GameServiceOption.lMagicExpScore = GameServerResult.lMagicExpScore;
	m_ModuleInitParameter.GameServiceOption.lGratuityScore = GameServerResult.lGratuityScore;
	m_ModuleInitParameter.GameServiceOption.dwWinExperience = GameServerResult.dwWinExperience;
	m_ModuleInitParameter.GameServiceOption.dwFailExperience = GameServerResult.dwFailExperience;

	//��Ա����
	m_ModuleInitParameter.GameServiceOption.cbMaxEnterMember=GameServerResult.cbMaxEnterMember;
	m_ModuleInitParameter.GameServiceOption.cbMaxEnterMember=GameServerResult.cbMaxEnterMember;

	//��������
	m_ModuleInitParameter.GameServiceOption.dwServerRule=GameServerResult.dwServerRule;
	m_ModuleInitParameter.GameServiceOption.dwAttachUserRight=GameServerResult.dwAttachUserRight;

	//��������
	m_ModuleInitParameter.GameServiceOption.wMaxPlayer=GameServerResult.wMaxPlayer;
	m_ModuleInitParameter.GameServiceOption.wTableCount=GameServerResult.wTableCount;
	m_ModuleInitParameter.GameServiceOption.wServerType=GameServerResult.wServerType;
	m_ModuleInitParameter.GameServiceOption.wServerPort=GameServerResult.wServerPort;
	lstrcpyn(m_ModuleInitParameter.GameServiceOption.szServerName,GameServerResult.szServerName,LEN_SERVER);

	//��������
	m_ModuleInitParameter.GameServiceOption.cbDistributeRule=GameServerResult.cbDistributeRule;
	m_ModuleInitParameter.GameServiceOption.wMinDistributeUser=GameServerResult.wMinDistributeUser;
	m_ModuleInitParameter.GameServiceOption.wDistributeTimeSpace=GameServerResult.wDistributeTimeSpace;
	m_ModuleInitParameter.GameServiceOption.wDistributeDrawCount=GameServerResult.wDistributeDrawCount;
	m_ModuleInitParameter.GameServiceOption.wMinPartakeGameUser=GameServerResult.wMinPartakeGameUser;
	m_ModuleInitParameter.GameServiceOption.wMaxPartakeGameUser=GameServerResult.wMaxPartakeGameUser;

	//��չ����
	UINT uCustomRuleSize=sizeof(m_ModuleInitParameter.GameServiceOption.cbCustomRule);
	CopyMemory(m_ModuleInitParameter.GameServiceOption.cbCustomRule,GameServerResult.cbCustomRule,uCustomRuleSize);

	//�رմ���
	EndDialog(IDOK);

	return;
}

//////////////////////////////////////////////////////////////////////////////////

void CDlgServerOptionItem1::OnBnClickedRevenueRadio()
{
	//
	GetDlgItem(IDC_STATIC_REVENUE_SERVICE)->SetWindowText(TEXT("˰�ձ�����"));
	((CEdit *)GetDlgItem(IDC_SERVICE_REVENUE))->SetWindowText(TEXT(""));
	((CEdit *)GetDlgItem(IDC_SERVICE_REVENUE))->LimitText(3);
}

void CDlgServerOptionItem1::OnBnClickedServiceRadio()
{
	//
	GetDlgItem(IDC_STATIC_REVENUE_SERVICE)->SetWindowText(TEXT("���������"));
	((CEdit *)GetDlgItem(IDC_SERVICE_REVENUE))->SetWindowText(TEXT(""));
	((CEdit *)GetDlgItem(IDC_SERVICE_REVENUE))->LimitText(9);
}
