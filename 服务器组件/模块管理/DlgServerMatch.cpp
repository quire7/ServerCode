#include "Stdafx.h"
#include "Resource.h"
#include "DlgServerMatch.h"


//�б�����
#define LIST_STYTE LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_FLATSB

//////////////////////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDlgServerMatch, CDialog)

	//��ť��Ϣ
	ON_BN_CLICKED(IDC_LOAD_MATCH, OnBnClickedLoadMatch)

	//�ؼ���Ϣ
	ON_NOTIFY(NM_DBLCLK, IDC_MATCH_LIST, OnNMDblclkServerList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_MATCH_LIST, OnLvnItemChangedServerList)

END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////////////////////////////////
//���캯��
CDlgServerMatch::CDlgServerMatch() : CDialog(IDD_SERVER_MATCH)
{
	//��ʶ����
	m_wKindID=0;

	//ģ�����
	ZeroMemory(&m_GameMatchOption,sizeof(m_GameMatchOption));

	return;
}

//��������
CDlgServerMatch::~CDlgServerMatch()
{
}

//��ʼ����
BOOL CDlgServerMatch::OnInitDialog()
{
	__super::OnInitDialog();

	//���ر���
	LoadDBMatchItem();

	return TRUE;  
}

//���ݽ���
void CDlgServerMatch::DoDataExchange(CDataExchange* pDX)
{
	//�����б�
	__super::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_MATCH_LIST,m_MatchListControl);
}

//�򿪱���
bool CDlgServerMatch::OpenGameMatch(WORD wKindID)
{
	//���ñ���
	m_wKindID=wKindID;

	//������Դ
	AfxSetResourceHandle(GetModuleHandle(MODULE_MANAGER_DLL_NAME));

	//���÷���
	if (DoModal()==IDC_LOAD_SERVER)
	{
		return true;
	}

	return false;
}


//���ط���
bool CDlgServerMatch::LoadDBMatchItem()
{
	//�����б�
	m_MatchListControl.DeleteAllItems();

	//���ð�ť
	GetDlgItem(IDC_LOAD_MATCH)->EnableWindow(FALSE);

	//������Ϣ
	CServerInfoManager ServerInfoManager;
	if (ServerInfoManager.LoadGameMatchOption(m_wKindID,m_MatchOptionBuffer)==true)
	{
		//��������
		tagGameMatchOption * pGameMatchOption=NULL;

		for(INT_PTR nIndex=0;nIndex<m_MatchOptionBuffer.m_GameMatchOptionArray.GetCount();nIndex++)
		{
			//��ȡ����
			pGameMatchOption=m_MatchOptionBuffer.m_GameMatchOptionArray[nIndex];

			//�����б�
			ASSERT(pGameMatchOption!=NULL);
			if (pGameMatchOption!=NULL) m_MatchListControl.InsertMatchOption(pGameMatchOption);
		}
		
		return true;
	}

	return false;
}

//���ط���
VOID CDlgServerMatch::OnBnClickedLoadMatch()
{
	//��ȡѡ��
	ASSERT(m_MatchListControl.GetFirstSelectedItemPosition()!=NULL);
	POSITION Position=m_MatchListControl.GetFirstSelectedItemPosition();
	
	//��ȡ����
	INT nListItem=m_MatchListControl.GetNextSelectedItem(Position);
	tagGameMatchOption * pGameMatchOption=(tagGameMatchOption *)m_MatchListControl.GetItemData(nListItem);

	//���ؽ���
	CServerInfoManager ServerInfoManager;
	pGameMatchOption->wRewardCount=ServerInfoManager.LoadGameMatchReward(pGameMatchOption->dwMatchID,pGameMatchOption->dwMatchNO);

	//���ñ���
	CopyMemory(&m_GameMatchOption,pGameMatchOption,sizeof(m_GameMatchOption));

	//�����Ի���
	EndDialog(IDC_LOAD_SERVER);
}

//˫���б�
VOID CDlgServerMatch::OnNMDblclkServerList(NMHDR * pNMHDR, LRESULT * pResult)
{
	//��������
	if (((NMITEMACTIVATE *)pNMHDR)->iItem!=LB_ERR)
	{
		OnBnClickedLoadMatch();
	}

	return;
}

//ѡ��ı�
VOID CDlgServerMatch::OnLvnItemChangedServerList(NMHDR * pNMHDR, LRESULT * pResult)
{
	//��ȡѡ��
	POSITION Position=m_MatchListControl.GetFirstSelectedItemPosition();

	//�����б�
	if (Position!=NULL)
	{
		//���ð�ť
		GetDlgItem(IDC_LOAD_MATCH)->EnableWindow(TRUE);
	}
	else
	{
		//���ð�ť
		GetDlgItem(IDC_LOAD_MATCH)->EnableWindow(FALSE);
	}

	return;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
