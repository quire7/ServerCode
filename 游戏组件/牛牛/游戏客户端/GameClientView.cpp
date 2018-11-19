#include "StdAfx.h"
#include "Resource.h"
#include "GameClient.h"
#include "GameClientEngine.h"
#include "GameClientView.h"


//////////////////////////////////////////////////////////////////////////
//�궨��

//��ť��ʶ
#define IDC_HINT_OX						130								//��ʾ��ť
#define IDC_START						100								//��ʼ��ť
#define IDC_MAX_SCORE					104								//���ť
#define IDC_MIN_SCORE					105								//���ٰ�ť
#define IDC_ADD_SCORE					110								//��ע��ť

#define IDC_READY						111								//̯�ư�ť
#define IDC_RESORT						112								//���Ű�ť
#define IDC_OX							113								//ţţ��ť
#define IDC_ONE_SCORE					117								//��ע��ť
#define IDC_TWO_SCORE					114								//��ע��ť
#define IDC_THREE_SCORE					115								//��ע��ť
#define IDC_FOUR_SCORE					116								//��ע��ť
#define IDC_SHORTCUT					118								//��ݼ���ť
#define IDC_IDLER						119								//�мҰ�ť
#define IDC_BANKER						120								//ׯ�Ұ�ť

//��ʱ����ʶ
#define IDI_SEND_CARD					98								//���ƶ�ʱ��
#define IDI_GOLD						97								//���ƶ�ʱ��
#define IDI_USER_ACTION					101								//������ʱ��

//��ʱ��ʱ��
#define TIME_USER_ACTION				4000							//������ʱ��

//�ƶ��ٶ�
#define	SEND_PELS						80								//�����ٶ�	
#define	TIME_SENDSPEED					50								//����ٶ�
//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameViewGDI)							  
	ON_WM_CREATE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameClientView::CGameClientView() 
{
	//��������
	for (WORD i=0;i<GAME_PLAYER;i++) 
	{
		m_bOxValue[i]=0xff;
		m_SendEndingPos[i].SetPoint(0,0);
	}
	m_wSendCount=0;
	m_wSendIndex=0;
	m_SendCardPos.SetPoint(0,0);
	m_SendCardItemArray.RemoveAll();

	//���ݱ���
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_bOpenCardUser,sizeof(m_bOpenCardUser));
	ZeroMemory(m_bShowSocre,sizeof(m_bShowSocre));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
	m_wBankerUser=INVALID_CHAIR;
	m_bInvest=false;
	m_bCallUser=0xff;
	m_bShow=true;
	m_bLookOnUser=false;
	m_bOpenCard=false;
	m_bUserAction=false;

	return;
}

//��������
CGameClientView::~CGameClientView()
{
	m_ViewFontEx.DeleteFont;
}

//��Ϣ����
BOOL CGameClientView::PreTranslateMessage(MSG * pMsg)
{
	m_ToolTipCtrl.RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

//������Ϣ
int CGameClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//����λͼ
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_ImageOxValue.LoadImage(this,hInstance,TEXT("COW_VALUE"));
	m_ImageOxValueZero.LoadFromResource(this,hInstance,IDB_COW_VALUE_ZERO);
	m_ImageDoulbeOx.LoadImage(this,hInstance,TEXT("DOUBLE_COW"));
	m_ImageNumber.LoadImage(this,hInstance,TEXT("PNG_NUMBER"));
	m_ImageScore.LoadFromResource(this,hInstance,IDB_SCORE);
	m_ImageJeton.LoadFromResource(this,hInstance,IDB__MONEY);
	m_ImageWaitCall.LoadImage(this,hInstance,TEXT("CALL_BANKER"));
	m_ImageCard.LoadFromResource(this,hInstance,IDB_CARD);
	m_ImageViewBack.LoadFromResource(this,hInstance,IDB_VIEW_BACK);
	m_ImageViewCenter.LoadFromResource(this,hInstance,IDB_VIEW_FILL);
	m_ImageBanker.LoadImage(this,hInstance,TEXT("BANKER_FLAG"));

	//�����ؼ�
	CRect rcCreate(0,0,0,0);

	//������ť
	m_btHintOx.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_HINT_OX);
	m_btStart.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_START);
	m_btReSort.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_RESORT);
	m_btOpenCard.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_READY);
	m_btOx.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_OX);
	m_btShortcut.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_SHORTCUT);
	m_btOneScore.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_ONE_SCORE);
	m_btTwoScore.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_TWO_SCORE);
	m_btThreeScore.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_THREE_SCORE);
	m_btFourScore.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_FOUR_SCORE);
	m_btBanker.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_BANKER);
	m_btIdler.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_IDLER);

	//����λͼ
	m_btHintOx.SetButtonImage(IDB_HINT_OX,hInstance,false,false);
	m_btStart.SetButtonImage(IDB_START,hInstance,false,false);
	m_btReSort.SetButtonImage(IDB_RESOCT,hInstance,false,false);
	m_btOpenCard.SetButtonImage(IDB_OPENCARD,hInstance,false,false);
	m_btOx.SetButtonImage(IDB_COW,hInstance,false,false);
	m_btShortcut.SetButtonImage(IDB_ShortCut,hInstance,false,false);
	m_btOneScore.SetButtonImage(IDB__MONEY,hInstance,false,false);
	m_btTwoScore.SetButtonImage(IDB__MONEY,hInstance,false,false);
	m_btThreeScore.SetButtonImage(IDB__MONEY,hInstance,false,false);
	m_btFourScore.SetButtonImage(IDB__MONEY,hInstance,false,false);
	m_btBanker.SetButtonImage(IDB_BANKER,hInstance,false,false);
	m_btIdler.SetButtonImage(IDB_IDLER,hInstance,false,false);

	//�����ؼ�
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_CardControl[i].Create(NULL,NULL,WS_VISIBLE|WS_CHILD,rcCreate,this,20+i);
		m_CardControlOx[i].Create(NULL,NULL,WS_VISIBLE|WS_CHILD,rcCreate,this,20+GAME_PLAYER+i);
		m_CardControlOx[i].SetDrawHeight(30);
	}

    //��ť��ʾ
	m_ToolTipCtrl.Create(this);
	m_ToolTipCtrl.Activate(TRUE);
	m_ToolTipCtrl.AddTool(&m_btShortcut,TEXT("ţ��ʾ��ݼ�"));
	m_ScoreView.Create( IDD_GAME_SCORE,this );

	//��������
	m_ViewFontEx.CreateFont(this,TEXT("����"),12,400);

	return 0;
}

//�����
BOOL CGameClientView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case IDC_HINT_OX:			//��ʾ��ť
		{
			//������Ϣ
			SendEngineMessage(IDM_HINT_OX,0,0);
			return TRUE;
		}
	case IDC_START:				//��ʼ��ť
		{
			//������Ϣ
			SendEngineMessage(IDM_START,0,0);
			return TRUE;
		}
	case IDC_READY:				//̯�ư�ť
		{
			//������Ϣ
			SendEngineMessage(IDM_READY,0,0);
			return TRUE;
		}
	case IDC_OX:				//ţţ��ť
		{
			//������Ϣ
			SendEngineMessage(IDM_OX,0,0);
			return TRUE;
		}
	case IDC_RESORT:			//���Ű�ť
		{
			//������Ϣ
			SendEngineMessage(IDM_RESORT,0,0);
			return TRUE;
		}
	case IDC_ONE_SCORE:			//��ע��ť
		{
			//������Ϣ
			SendEngineMessage(IDM_ADD_SCORE,4,1);
			return TRUE;
		}
	case IDC_TWO_SCORE:			//��ע��ť
		{
			//������Ϣ
			SendEngineMessage(IDM_ADD_SCORE,3,2);
			return TRUE;
		}
	case IDC_THREE_SCORE:		//��ע��ť
		{
			//������Ϣ
			SendEngineMessage(IDM_ADD_SCORE,2,3);
			return TRUE;
		}
	case IDC_FOUR_SCORE:		//��ע��ť
		{
			//������Ϣ
			SendEngineMessage(IDM_ADD_SCORE,1,4);
			return TRUE;
		}
	case IDC_SHORTCUT:			//��ݼ���ť
		{
			//������Ϣ
			SendEngineMessage(IDM_SHORTCUT_KEY,0,0);
			return TRUE;
		}
	case IDC_BANKER:			//ׯ�Ұ�ť
		{
			//������Ϣ
			SendEngineMessage(IDM_BANKER,1,1);
			return TRUE;
		}
	case IDC_IDLER:				//�мҰ�ť
		{
			//������Ϣ
			SendEngineMessage(IDM_BANKER,0,0);
			return TRUE;
		}
	}

	return __super::OnCommand(wParam, lParam);
}

//���ý���
VOID CGameClientView::ResetGameView()
{
	//��������
	for (WORD i=0;i<GAME_PLAYER;i++) 
	{
		m_bOxValue[i]=0xff;
	}
	m_wSendCount=0;
	m_wSendIndex=0;
	m_SendCardPos.SetPoint(0,0);
	m_SendCardItemArray.RemoveAll();

	//���ݱ���
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_bOpenCardUser,sizeof(m_bOpenCardUser));
	ZeroMemory(m_bShowSocre,sizeof(m_bShowSocre));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
	m_wBankerUser=INVALID_CHAIR;				
	m_bInvest=false;
	m_bCallUser=0xff;
	m_bShow=true;
	m_bLookOnUser=false;
	m_bOpenCard=false;
	m_bUserAction=false;

	//ɾ����ʱ��
	KillTimer(IDI_SEND_CARD);

	//���ذ�ť
	m_btHintOx.ShowWindow(SW_HIDE);
	m_btStart.ShowWindow(SW_HIDE);
	m_btReSort.ShowWindow(SW_HIDE);
	m_btOpenCard.ShowWindow(SW_HIDE);
	m_btOx.ShowWindow(SW_HIDE);
	m_btOneScore.ShowWindow(SW_HIDE);
	m_btTwoScore.ShowWindow(SW_HIDE);
	m_btThreeScore.ShowWindow(SW_HIDE);
	m_btFourScore.ShowWindow(SW_HIDE);
	m_btShortcut.ShowWindow(SW_HIDE);
	m_btBanker.ShowWindow(SW_HIDE);
	m_btIdler.ShowWindow(SW_HIDE);

	//���ؿؼ�
	m_ScoreView.ShowWindow(SW_HIDE);
	for (WORD i=0;i<GAME_PLAYER;i++) 
	{
		m_CardControl[i].SetDisplayFlag(false);
		//m_CardControlOx[i].SetDisplayFlag(false);
		m_CardControl[i].SetCardData(NULL,0);
		m_CardControlOx[i].SetCardData(NULL,0);
	}

	return;
}


//�����ؼ�
VOID CGameClientView::RectifyControl(INT nWidth, INT nHeight)
{
	//����λ��
	m_SendStartPos.SetPoint(nWidth/2,nHeight/2-30);

	INT nXFace=50;
	INT nYFace=50;
	INT nXTimer=60;
	INT nYTimer=60;
	
	m_ptAvatar[0].x=nWidth/2-nXFace/2-10;
	m_ptAvatar[0].y=nHeight/2-351;
	m_ptAvatar[2].x=nWidth/2-nXFace/2;
	m_ptAvatar[2].y=nHeight/2+352-nYFace;
	m_ptAvatar[1].x=nWidth/2+335;
	m_ptAvatar[1].y=nHeight/2-nYFace;
	m_ptAvatar[3].x=nWidth/2-373;
	m_ptAvatar[3].y=nHeight/2-nYFace;

	m_ptNickName[0].x=nWidth/2+21;
	m_ptNickName[0].y=nHeight/2-345;
	m_ptNickName[2].x=nWidth/2+30;
	m_ptNickName[2].y=nHeight/2+325;
	m_ptNickName[1].x=nWidth/2+183+120;
	m_ptNickName[1].y=nHeight/2+5;
	m_ptNickName[3].x=nWidth/2-183-190;
	m_ptNickName[3].y=nHeight/2+5;

	m_ptClock[0].x=m_ptAvatar[0].x-nXTimer/2-5;
	m_ptClock[0].y=nHeight/2-325;
	m_ptClock[2].x=m_ptAvatar[2].x-nXTimer/2-5;
	m_ptClock[2].y=nHeight/2+350-nYTimer/2;
	m_ptClock[1].x=nWidth/2+188+164;
	m_ptClock[1].y=nHeight/2-nYFace-nYTimer+20;
	m_ptClock[3].x=nWidth/2-220-131;
	m_ptClock[3].y=nHeight/2-nYFace+20-nYTimer;

	m_ptReady[0].x=nWidth/2;
	m_ptReady[0].y=nHeight/2-250;
	m_ptReady[2].x=nWidth/2;
	m_ptReady[2].y=nHeight/2+250;
	m_ptReady[1].x=nWidth/2+250;
	m_ptReady[1].y=nHeight/2;
	m_ptReady[3].x=nWidth/2-250;
	m_ptReady[3].y=nHeight/2;

	//��������
	for(BYTE i=0;i<GAME_PLAYER;i++)
	{
		m_PointBanker[i].x=m_ptClock[i].x-nXTimer/2;
		m_PointBanker[i].y=m_ptClock[i].y-nYTimer/2;
	}
	m_PointBanker[1].x+=6;
	m_PointBanker[3].x+=2;
	m_PointBanker[2].y-=6;
	m_PointBanker[0].x	+=200;
	m_PointBanker[0].y	+=10;
	m_PointBanker[2].x	+=160;
	m_PointBanker[1].y	+=170;
	m_PointBanker[3].y	+=170;

	m_ptJeton[0].x=nWidth/2-32;
	m_ptJeton[0].y=nHeight/2-153;
	m_ptJeton[2].x=nWidth/2-32;
	m_ptJeton[2].y=nHeight/2+74;
	m_ptJeton[1].x=nWidth/2+52;
	m_ptJeton[1].y=nHeight/2-49;
	m_ptJeton[3].x=nWidth/2-116;
	m_ptJeton[3].y=nHeight/2-49;

	//�û��˿�;
	m_CardControl[0].SetBenchmarkPos(nWidth/2,nHeight/2-290,enXCenter,enYTop);
	m_CardControlOx[0].SetBenchmarkPos(nWidth/2,nHeight/2-320,enXCenter,enYTop);
	m_CardControl[2].SetBenchmarkPos(nWidth/2,nHeight/2+270,enXCenter,enYBottom);
	m_CardControlOx[2].SetBenchmarkPos(nWidth/2,nHeight/2+240,enXCenter,enYBottom);
	m_CardControl[1].SetBenchmarkPos(nWidth/2+293,nHeight/2-30,enXRight,enYCenter);
	m_CardControlOx[1].SetBenchmarkPos(nWidth/2+283,nHeight/2-60,enXRight,enYCenter);
	m_CardControl[3].SetBenchmarkPos(nWidth/2-293,nHeight/2-30,enXLeft,enYCenter);
	m_CardControlOx[3].SetBenchmarkPos(nWidth/2-283,nHeight/2-60,enXLeft,enYCenter);

	//����λ��
	m_SendEndingPos[0].x=m_CardControl[0].GetCardPos().x;
	m_SendEndingPos[0].y=m_CardControl[0].GetCardPos().y;
	m_SendEndingPos[1].x=m_CardControl[1].GetCardPos().x;
	m_SendEndingPos[1].y=m_CardControl[1].GetCardPos().y;
	m_SendEndingPos[2].x=m_CardControl[2].GetCardPos().x;
	m_SendEndingPos[2].y=m_CardControl[2].GetCardPos().y;
	m_SendEndingPos[3].x=m_CardControl[3].GetCardPos().x;
	m_SendEndingPos[3].y=m_CardControl[3].GetCardPos().y;

	//ţֵλ��
	m_ptValue[0].x=nWidth/2-138;
	m_ptValue[0].y=nHeight/2-250;
	m_ptValue[1].x=nWidth/2+200;
	m_ptValue[1].y=nHeight/2+30;
	m_ptValue[2].x=nWidth/2-157;
	m_ptValue[2].y=nWidth/2+158;
	m_ptValue[3].x=nWidth/2-280;
	m_ptValue[3].y=nHeight/2+30;

	//��ť�ؼ�
	CRect rcButton;
	HDWP hDwp=BeginDeferWindowPos(32);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;

	//��ʼ��ť
	m_btStart.GetWindowRect(&rcButton);
	DeferWindowPos(hDwp,m_btStart,NULL,nWidth/2-32,nHeight/2+112,0,0,uFlags);

	//���ư�ť
	m_btReSort.GetWindowRect(&rcButton);
	DeferWindowPos(hDwp,m_btReSort,NULL,nWidth/2-32,nHeight/2+112,0,0,uFlags);
	DeferWindowPos(hDwp,m_btOpenCard,NULL,nWidth/2+25,nHeight/2+112,0,0,uFlags);
	DeferWindowPos(hDwp,m_btOx,NULL,nWidth/2-118,nHeight/2+112,0,0,uFlags);
	DeferWindowPos(hDwp,m_btHintOx,NULL,nWidth/2-88,nHeight/2+112,0,0,uFlags);

	//������ť
	DeferWindowPos(hDwp,m_btShortcut,NULL,nWidth/2+330,nHeight/2+320,0,0,uFlags);

	//���밴ť
	DeferWindowPos(hDwp,m_btOneScore,NULL,nWidth/2-151,nHeight/2+163,0,0,uFlags);
	DeferWindowPos(hDwp,m_btTwoScore,NULL,nWidth/2-67,nHeight/2+163,0,0,uFlags);
	DeferWindowPos(hDwp,m_btThreeScore,NULL,nWidth/2+15,nHeight/2+163,0,0,uFlags);
	DeferWindowPos(hDwp,m_btFourScore,NULL,nWidth/2+98,nHeight/2+163,0,0,uFlags);

	//ѡׯ��ť
	DeferWindowPos(hDwp,m_btBanker,NULL,nWidth/2-76,nHeight/2+151,0,0,uFlags);
	DeferWindowPos(hDwp,m_btIdler,NULL,nWidth/2+14,nHeight/2+151,0,0,uFlags);

	//�����ƶ�
	EndDeferWindowPos(hDwp);

	//��ע����

	//������ͼ
	CRect rcControl;
	m_ScoreView.GetWindowRect(&rcControl);
	CPoint ptPos( nWidth/2-183,nHeight/2-153 );
	ClientToScreen( &ptPos );
	m_ScoreView.SetWindowPos( NULL,ptPos.x,ptPos.y,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOCOPYBITS );
	
	return;
}

//�滭����
VOID CGameClientView::DrawGameView(CDC * pDC, INT nWidth, INT nHeight)
{
	//�滭����
	DrawViewImage(pDC,m_ImageViewCenter,DRAW_MODE_SPREAD);
	DrawViewImage(pDC,m_ImageViewBack,DRAW_MODE_CENTENT);	

	//�滭�û�
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//��������
		WORD wUserTimer=GetUserClock(i);
		IClientUserItem * pClientUserItem=GetClientUserItem(i);

		//�滭�û�
		if (pClientUserItem!=NULL)
		{
			//�û�����
			pDC->SetTextAlign((i==1)?TA_RIGHT:TA_LEFT);
			pDC->SetTextColor((wUserTimer>0)?RGB(250,250,250):RGB(220,220,220));

			//�û����
			TCHAR szBuffer[64]=TEXT("");
			LONGLONG lLeaveScore=pClientUserItem->GetUserScore()-m_lTableScore[i];
			_sntprintf(szBuffer,sizeof(szBuffer),TEXT("��")SCORE_STRING,lLeaveScore);

			//�û�����
			//DrawTextString(pDC,pClientUserItem->GetNickName(),RGB(255,255,255),RGB(0,0,0),m_ptNickName[i].x+TempX,m_ptNickName[i].y+TempY);
			CRect rcNickName(m_ptNickName[i].x,m_ptNickName[i].y,m_ptNickName[i].x+100,m_ptNickName[i].y+18);
			m_ViewFontEx.DrawText(pDC, pClientUserItem->GetNickName(),&rcNickName,RGB(255,255,255),DT_LEFT|DT_TOP|DT_SINGLELINE|DT_END_ELLIPSIS);

			//�û����
			//DrawTextString(pDC,szBuffer,RGB(255,255,255),RGB(0,0,0),m_ptNickName[i].x+TempX,m_ptNickName[i].y+16+TempY);
			CRect rcUserGold(m_ptNickName[i].x,m_ptNickName[i].y+16,m_ptNickName[i].x+100,m_ptNickName[i].y+34);
			m_ViewFontEx.DrawText(pDC, szBuffer,&rcUserGold,RGB(255,255,255),DT_LEFT|DT_TOP|DT_SINGLELINE|DT_END_ELLIPSIS);

			//������Ϣ
			if (wUserTimer!=0)
			{
				if (m_bCallUser==0xff && (pClientUserItem->GetUserStatus()==US_PLAYING|| m_cbPlayStatus[2]==USEX_DYNAMIC))
					DrawUserClock(pDC,m_ptClock[i].x,m_ptClock[i].y,wUserTimer);
				else if (pClientUserItem->GetUserStatus()==US_PLAYING)DrawUserClock(pDC,nWidth/2,nHeight/2-25,wUserTimer);
				else DrawUserClock(pDC,m_ptClock[i].x,m_ptClock[i].y,wUserTimer);
			}
			if (pClientUserItem->GetUserStatus()==US_READY) DrawUserReady(pDC,m_ptReady[i].x,m_ptReady[i].y);
			DrawUserAvatar(pDC,m_ptAvatar[i].x,m_ptAvatar[i].y,pClientUserItem);
		}

	}

	//�ȴ���ע
	if (m_bInvest==true)
	{
		//������Դ
		CFont InfoFont;
		InfoFont.CreateFont(-24,0,0,0,700,0,0,0,134,3,2,1,1,TEXT("����_GB2312"));

		//���� DC
		pDC->SetTextAlign(TA_CENTER|TA_TOP);
		CFont * pOldFont=pDC->SelectObject(&InfoFont);

		//�滭��Ϣ
		TCHAR zsInfo[128]=TEXT("");
		_sntprintf(zsInfo,sizeof(zsInfo),TEXT("�ȴ��м���ע"));
		//DrawTextString(pDC,zsInfo,RGB(250,200,40),RGB(0,0,0),nWidth/2,nHeight/2);
		CDFontEx InfoFontEx;
		InfoFontEx.CreateFont(this,TEXT("����_GB2312"),24,700);
		InfoFontEx.DrawText(pDC,zsInfo,nWidth/2,nHeight/2,RGB(250,200,40),DT_CENTER|DT_TOP);
		InfoFontEx.DeleteFont();

		//������Դ
		pDC->SelectObject(pOldFont);
		InfoFont.DeleteObject();
	}

	//��������
	if (m_bUserAction==true)
	{
		//������Դ
		CFont InfoFont;
		InfoFont.CreateFont(-16,0,0,0,700,0,0,0,134,3,2,1,1,TEXT("����_GB2312"));

		//���� DC
		pDC->SetTextColor(RGB(250,200,40));
		pDC->SetTextAlign(TA_CENTER|TA_TOP);
		CFont * pOldFont=pDC->SelectObject(&InfoFont);

		//�滭��Ϣ
		//pDC->TextOut(nWidth/2,nHeight/2+150,TEXT("��������,ϵͳ�Զ�Ϊ������ ^-^"));
		CDFontEx InfoFontEx;
		InfoFontEx.CreateFont(this,TEXT("����_GB2312"),16,700);
		InfoFontEx.DrawText(pDC,TEXT("��������,ϵͳ�Զ�Ϊ������ ^-^"),nWidth/2,nHeight/2,RGB(250,200,40),DT_CENTER|DT_TOP);
		InfoFontEx.DeleteFont();

		//������Դ
		pDC->SelectObject(pOldFont);
		InfoFont.DeleteObject();
	}

	//�ȴ�̯��
	if (m_bOpenCard==true)
	{
		//������Դ
		CFont InfoFont;
		InfoFont.CreateFont(-24,0,0,0,700,0,0,0,134,3,2,1,1,TEXT("����_GB2312"));

		//���� DC
		pDC->SetTextColor(RGB(250,200,40));
		pDC->SetTextAlign(TA_CENTER|TA_TOP);
		CFont * pOldFont=pDC->SelectObject(&InfoFont);

		//�滭��Ϣ
		TCHAR zsInfo[128]=TEXT("");
		_sntprintf(zsInfo,sizeof(zsInfo),TEXT("�ȴ����̯��"));
		//DrawTextString(pDC,zsInfo,RGB(250,200,40),RGB(0,0,0),nWidth/2,nHeight/2);
		CDFontEx InfoFontEx;
		InfoFontEx.CreateFont(this,TEXT("����_GB2312"),24,700);
		InfoFontEx.DrawText(pDC,zsInfo,nWidth/2,nHeight/2,RGB(250,200,40),DT_CENTER|DT_TOP);
		InfoFontEx.DeleteFont();

		//������Դ
		pDC->SelectObject(pOldFont);
		InfoFont.DeleteObject();
	}

	//�ȴ���ׯ
	if(m_bCallUser!=0xff && (m_bCallUser!=2 || m_bLookOnUser))
	{
		int y=m_SendEndingPos[m_bCallUser].y+10;
		int x=nWidth/2 - m_ImageWaitCall.GetWidth()/2;
		if(m_bCallUser==1)x=nWidth/2+273-120;
		else if(m_bCallUser==3)x=nWidth/2-273-30;
		else if(m_bCallUser==0)y+=30;
		m_ImageWaitCall.AlphaDrawImage(pDC,x,y,m_ImageWaitCall.GetWidth(),m_ImageWaitCall.GetHeight(),0,0,255);
	}

	//ׯ����Ϣ
	if (m_wBankerUser!=INVALID_CHAIR)
	{
		IClientUserItem * pClientUserItem=GetClientUserItem(m_wBankerUser);
		if (pClientUserItem!=NULL)
		{
			//ׯ�ұ�־
			m_ImageBanker.AlphaDrawImage(pDC,m_PointBanker[m_wBankerUser].x,m_PointBanker[m_wBankerUser].y,m_ImageBanker.GetWidth(), m_ImageBanker.GetHeight(),0,0,255);
		}
	}

	//�滭����
	for (BYTE i=0;i<GAME_PLAYER;i++)
	{
		if(m_cbPlayStatus[i]!=USEX_PLAYING) continue;
		IClientUserItem * pClientUserItem=GetClientUserItem(i);
		if (pClientUserItem!=NULL &&(m_lTableScore[i]!=0L || m_bShowSocre[i]))
		{
			int iPos=1;
			if(m_bShowSocre[i])
			{
				if(m_lTableScore[i]>0L)iPos=0;
				else if(m_lTableScore[i]<=0L)iPos=2;
			}
			m_ImageScore.TransDrawImage(pDC,m_ptJeton[i].x,m_ptJeton[i].y,m_ImageScore.GetWidth()/3,
				m_ImageScore.GetHeight(),m_ImageScore.GetWidth()/3*iPos,0,RGB(255,0,255));

			//��������
			CFont ViewFont;
			ViewFont.CreateFont(-12,0,0,0,700,0,0,0,134,3,2,1,1,TEXT("����"));

			//���� DC
			pDC->SetTextAlign(TA_CENTER);
			CFont * pOldFont=pDC->SelectObject(&ViewFont);

			//�滭��Ϣ
			TCHAR tc[LEN_ACCOUNTS]=TEXT("");
			_sntprintf(tc,sizeof(tc),SCORE_STRING,m_lTableScore[i]);
			//DrawTextString(pDC,tc,RGB(250,250,250),RGB(0,0,0),m_ptJeton[i].x+32,m_ptJeton[i].y+8);
			CDFontEx::DrawText(this,pDC,12,700,tc,m_ptJeton[i].x+32,m_ptJeton[i].y+8,RGB(250,250,2500),DT_CENTER|DT_TOP);

			//������Դ
			pDC->SelectObject(pOldFont);
			ViewFont.DeleteObject();
		}
	}

	//��ʾ̯�Ʊ�־
	for (BYTE i=0;i<GAME_PLAYER;i++)
	{
		if (m_bOpenCardUser[i]==TRUE)
		{
			//��������
			CFont ViewFont;
			ViewFont.CreateFont(-20,0,0,0,700,0,0,0,134,3,2,1,1,TEXT("���Ĳ���"));

			//���� DC
			pDC->SetTextAlign(TA_LEFT);
			CFont * pOldFont=pDC->SelectObject(&ViewFont);

			//�滭��Ϣ
			TCHAR tc[LEN_ACCOUNTS]=TEXT("");
			_sntprintf(tc,sizeof(tc),TEXT("̯"));
			INT x,y;
			if(i==0)
			{
				x = m_ptValue[i].x+35;
				y = m_ptValue[i].y+40;
			}
			else if(i==1)
			{
				x = m_ptValue[i].x+97;
				y = m_ptValue[i].y-55;
			}
			else if(i==2)
			{
				x = m_ptValue[i].x+35;
				y = m_ptValue[i].y+40;
			}
			else 
			{
				x = m_ptValue[i].x-37;
				y = m_ptValue[i].y-55;
			}
			//DrawTextString(pDC,tc,RGB(250,128,56),RGB(0,0,0),x,y);
			CDFontEx ViewFontEx;
			ViewFontEx.CreateFont(this,TEXT("���Ĳ���"),20,700);
			ViewFontEx.DrawText(pDC,tc,x,y,RGB(250,128,56),DT_LEFT);
			ViewFontEx.DeleteFont();

			//������Դ
			pDC->SelectObject(pOldFont);
			ViewFont.DeleteObject();
		}
	}

	//�滭�˿�
	if (m_SendCardItemArray.GetCount()>0)
	{
		//��������
		tagSendCardItem * pSendCardItem=&m_SendCardItemArray[0];

		//��ȡ��С
		int nItemWidth=m_ImageCard.GetWidth()/13;
		int nItemHeight=m_ImageCard.GetHeight()/5;

		//�滭�˿�
		int nXPos=m_SendCardPos.x-nItemWidth/2;
		int nYPos=m_SendCardPos.y-nItemHeight/2+10;
		m_ImageCard.TransDrawImage(pDC,nXPos,nYPos,nItemWidth,nItemHeight,nItemWidth*2,nItemHeight*4,RGB(255,0,255));
	}

	//��ʾ����
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if(m_cbPlayStatus[i]!=USEX_PLAYING) continue;
		if(m_bOxValue[i]!=0xff)
		{
			if(m_bShow || i==2)
			{
				if(m_bOxValue[i]!=0)
				{
					INT nXpt = (i%2!=0)?(m_ptValue[i].x):(m_CardControl[i].GetCardPos().x-2-m_ImageOxValue.GetWidth());
					INT nypt = m_ptValue[i].y;
					if(i==2) nXpt-=35;
					m_ImageOxValue.AlphaDrawImage(pDC,nXpt,nypt,m_ImageOxValue.GetWidth(),m_ImageOxValue.GetHeight(),0,0,255);
					if(m_bOxValue[i]>=OX_THREE_SAME)
					{
						pDC->SetTextColor(RGB(0,0,0));
						pDC->SetTextAlign(TA_LEFT);
						TCHAR tCh[64]= TEXT("��ţ");
						if(m_bOxValue[i]==OX_THREE_SAME)_sntprintf(tCh,CountArray(tCh),TEXT("��«"));
						else if(m_bOxValue[i]==OX_FOUR_SAME)_sntprintf(tCh,CountArray(tCh),TEXT("ը��"));
						//pDC->TextOut(nXpt+33,nypt+10,tCh,sizeof(tCh));
						CDFontEx::DrawText(this,pDC,-20,700,tCh,nXpt+33,nypt+10,RGB(0,0,0),TA_LEFT);
					}	
					else if(m_bOxValue[i]==10)
					{
						m_ImageDoulbeOx.AlphaDrawImage(pDC,nXpt+33,nypt+6,m_ImageDoulbeOx.GetWidth(),m_ImageDoulbeOx.GetHeight(),0,0,255);
					}	
					else if(m_bOxValue[i]<10)
					{
						m_ImageNumber.AlphaDrawImage(pDC,nXpt+35,nypt+7,
							m_ImageNumber.GetWidth()/9,m_ImageNumber.GetHeight(),
							m_ImageNumber.GetWidth()/9*((int)m_bOxValue[i]-1),0,255);
					}
				}
				else if(m_bOxValue[i]==0)
				{				
					INT nXpt = m_ptValue[i].x;
					INT nypt = m_ptValue[i].y;
					m_ImageOxValueZero.TransDrawImage(pDC,nXpt+((i!=1)?31:5),nypt+1,
						m_ImageOxValueZero.GetWidth(),m_ImageOxValueZero.GetHeight(),0,0,RGB(255,0,255));
				}
			}
		}
	}
	return;
}


//��ʾ��Ϣ
void CGameClientView::SetUserAction(bool bWait)
{
	if(m_bUserAction!=bWait)
	{
		m_bUserAction=bWait;

		//���½���
		RefreshGameView();
	}

	//����ʱ��
	if(bWait)
	{
		SetTimer(IDI_USER_ACTION,TIME_USER_ACTION,NULL);
	}
	else 
	{
		KillTimer(IDI_USER_ACTION);
	}

	return;
}

//�����ж�
bool CGameClientView::IsDispatchCard()
{
	return m_SendCardItemArray.GetCount()>0;
}

//��ɷ���
void CGameClientView::FinishDispatchCard()
{
	//����ж�
	if (m_SendCardItemArray.GetCount()==0) return;

	//��ɶ���
	for (INT_PTR i=0;i<m_SendCardItemArray.GetCount();i++)
	{
		//��ȡ����
		tagSendCardItem * pSendCardItem=&m_SendCardItemArray[i];

		//��ȡ�˿�
		BYTE cbCardData[MAX_COUNT];
		WORD wChairID=pSendCardItem->wChairID;
		BYTE cbCardCount=(BYTE)m_CardControl[wChairID].GetCardData(cbCardData,CountArray(cbCardData));

		//�����˿�
		if (cbCardCount<MAX_COUNT)
		{
			cbCardData[cbCardCount++]=pSendCardItem->cbCardData;
			m_CardControl[wChairID].SetCardData(cbCardData,cbCardCount);
		}
	}

	SendEngineMessage(IDM_SEND_CARD_FINISH,0,0);

	//ɾ������
	KillTimer(IDI_SEND_CARD);
	m_SendCardItemArray.RemoveAll();

	//���½���
	RefreshGameView();

	return;
}

//�����˿�
void CGameClientView::DispatchUserCard(WORD wChairID, BYTE cbCardData)
{
	//Ч�����
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return;

	//���ñ���
	tagSendCardItem SendCardItem;
	SendCardItem.cbCardData=cbCardData;
	SendCardItem.wChairID=wChairID;

	//��������
	m_SendCardItemArray.Add(SendCardItem);

	//��������
	if (m_SendCardItemArray.GetCount()==1) 
	{
		//��ȡλ��
		CRect rcClient;
		GetClientRect(&rcClient);

		//����λ��
		m_SendCardPos.x=m_SendStartPos.x;
		m_SendCardPos.y=m_SendStartPos.y;

		//λ�Ƽ���
		int nXCount=abs(m_SendEndingPos[SendCardItem.wChairID].x-m_SendStartPos.x)/SEND_PELS;
		int nYCount=abs(m_SendEndingPos[SendCardItem.wChairID].y-m_SendStartPos.y)/SEND_PELS;

		//���ô���
		m_wSendIndex=0;
		m_wSendCount=__max(1,__max(nXCount,nYCount));

		//���ö�ʱ��
		SetTimer(IDI_SEND_CARD,TIME_SENDSPEED,NULL);

		//��������
		CGameClientEngine * pGameClientDlg=CONTAINING_RECORD(this,CGameClientEngine,m_GameClientView);
		pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("SEND_CARD"));
	}

	return;
}

//������ע
void CGameClientView::SetUserTableScore(WORD wChairID, LONGLONG lTableScore)
{
	//��������
	if (wChairID!=INVALID_CHAIR) m_lTableScore[wChairID]=lTableScore;
	else ZeroMemory(m_lTableScore,sizeof(m_lTableScore));

	//���½���
	RefreshGameView();
	
	return;
}

//ׯ�ұ�־
void CGameClientView::SetBankerUser(WORD wBankerUser)
{
	m_wBankerUser=wBankerUser;

	//���½���
	RefreshGameView();
	return;
}

//�ȴ���ׯ
void CGameClientView::SetWaitCall(BYTE bCallUser)
{
	m_bCallUser=bCallUser;

	//���½���
	RefreshGameView();

	return ;
}

//�ȴ���־
void CGameClientView::SetWaitInvest(bool bInvest)
{
	if(m_bInvest!=bInvest)
	{
		m_bInvest=bInvest;

		//���½���
		RefreshGameView();
	}
	return;
}

//��ʾ����
void CGameClientView::ShowType(bool bShow)
{
	if(m_bShow!=bShow)
	{
		m_bShow=bShow;

		//���½���
		RefreshGameView();
	}

	return;
}

//��ʾ����
void CGameClientView::SetUserOxValue(WORD wChiarID,BYTE bValue)
{
	//ţţ����
	m_bOxValue[wChiarID]=bValue;

	//���½���
	RefreshGameView();

	return;
}

//��ʾ̯��
void CGameClientView::ShowOpenCard(WORD wChiarID,BOOL bShow)
{
	//̯���û�
	m_bOpenCardUser[wChiarID]=bShow;

	//���½���
	RefreshGameView();
	return;
}

//��ʾ����
void CGameClientView::ShowScore(WORD wChairID,bool bShow)
{
	//��ʾ��־
	m_bShowSocre[wChairID]=bShow;

	//���½���
	RefreshGameView();

	return;
}

//������ͼ
void CGameClientView::RefreshGameView()
{
	CRect rect;
	GetClientRect(&rect);
	InvalidGameView(rect.left,rect.top,rect.Width(),rect.Height());
	return;
}

//�û�״̬
void CGameClientView::SetUserPlayingStatus(WORD wChairID, BYTE cbPlayingStatus)
{
	if(wChairID<GAME_PLAYER)
	{
		m_cbPlayStatus[wChairID]=cbPlayingStatus;
	}
	else
	{
		ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
	}
}

//��ʱ����Ϣ
void CGameClientView::OnTimer(UINT nIDEvent)
{
	//���ʹ���
	switch(nIDEvent)
	{
	case IDI_SEND_CARD:		//�ƶ��˿�
		{
			//��������
			m_wSendIndex++;

			//����λ��
			tagSendCardItem * pSendCardItem=&m_SendCardItemArray[0];
			if(pSendCardItem->wChairID%2!=0)
				m_SendCardPos.x=m_SendStartPos.x+(m_SendEndingPos[pSendCardItem->wChairID].x-m_SendStartPos.x)*m_wSendIndex/                              m_wSendCount;
			else
				m_SendCardPos.y=m_SendStartPos.y+(m_SendEndingPos[pSendCardItem->wChairID].y-m_SendStartPos.y)*m_wSendIndex/                              m_wSendCount;

			//ֹͣ�ж�
			if (m_wSendIndex>=m_wSendCount)
			{
				//��ȡ�˿�
				BYTE cbCardData[MAX_COUNT];
				WORD wChairID=pSendCardItem->wChairID;
				BYTE cbCardCount=(BYTE)m_CardControl[wChairID].GetCardData(cbCardData,CountArray(cbCardData));

				//�����˿�
				cbCardData[cbCardCount++]=pSendCardItem->cbCardData;
				m_CardControl[wChairID].SetCardData(cbCardData,cbCardCount);

				//ɾ������
				m_SendCardItemArray.RemoveAt(0);

				//��������
				if (m_SendCardItemArray.GetCount()>0)
				{
					//��������
					tagSendCardItem * pSendCardItem=&m_SendCardItemArray[0];

					//����λ��
					m_SendCardPos.x=m_SendStartPos.x;
					m_SendCardPos.y=m_SendStartPos.y;

					//λ�Ƽ���
					int nXCount=abs(m_SendEndingPos[pSendCardItem->wChairID].x-m_SendStartPos.x)/SEND_PELS;
					int nYCount=abs(m_SendEndingPos[pSendCardItem->wChairID].y-m_SendStartPos.y)/SEND_PELS;

					//���ô���
					m_wSendIndex=0;
					m_wSendCount=__max(1,__max(nXCount,nYCount));

					//��������
					CGameClientEngine * pGameClientDlg=CONTAINING_RECORD(this,CGameClientEngine,m_GameClientView);
					pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("SEND_CARD"));
				}
				else 
				{
					//��ɴ���
					KillTimer(IDI_SEND_CARD);
					SendEngineMessage(IDM_SEND_CARD_FINISH,0,0);
				}
			}

			//���½���
			RefreshGameView();

			return ;
		}
	case IDI_USER_ACTION: 	//�û�����
		{
			//��������
			m_bUserAction = false;
			KillTimer(IDI_USER_ACTION);

			//���½���
			RefreshGameView();

			return;
		}
	}

	__super::OnTimer(nIDEvent);
}

//////////////////////////////////////////////////////////////////////////

