#include "StdAfx.h"
#include "Resource.h"
#include "GameClient.h"
#include "GameClientEngine.h"
#include "GameClientView.h"


//////////////////////////////////////////////////////////////////////////
//宏定义

//按钮标识
#define IDC_HINT_OX						130								//提示按钮
#define IDC_START						100								//开始按钮
#define IDC_MAX_SCORE					104								//最大按钮
#define IDC_MIN_SCORE					105								//最少按钮
#define IDC_ADD_SCORE					110								//加注按钮

#define IDC_READY						111								//摊牌按钮
#define IDC_RESORT						112								//重排按钮
#define IDC_OX							113								//牛牛按钮
#define IDC_ONE_SCORE					117								//加注按钮
#define IDC_TWO_SCORE					114								//加注按钮
#define IDC_THREE_SCORE					115								//加注按钮
#define IDC_FOUR_SCORE					116								//加注按钮
#define IDC_SHORTCUT					118								//快捷键按钮
#define IDC_IDLER						119								//闲家按钮
#define IDC_BANKER						120								//庄家按钮

//定时器标识
#define IDI_SEND_CARD					98								//发牌定时器
#define IDI_GOLD						97								//发牌定时器
#define IDI_USER_ACTION					101								//动作定时器

//定时器时间
#define TIME_USER_ACTION				4000							//动作定时器

//移动速度
#define	SEND_PELS						80								//发牌速度	
#define	TIME_SENDSPEED					50								//间隔速度
//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameViewGDI)							  
	ON_WM_CREATE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView() 
{
	//动画变量
	for (WORD i=0;i<GAME_PLAYER;i++) 
	{
		m_bOxValue[i]=0xff;
		m_SendEndingPos[i].SetPoint(0,0);
	}
	m_wSendCount=0;
	m_wSendIndex=0;
	m_SendCardPos.SetPoint(0,0);
	m_SendCardItemArray.RemoveAll();

	//数据变量
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

//析构函数
CGameClientView::~CGameClientView()
{
	m_ViewFontEx.DeleteFont;
}

//消息解释
BOOL CGameClientView::PreTranslateMessage(MSG * pMsg)
{
	m_ToolTipCtrl.RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

//建立消息
int CGameClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//加载位图
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

	//创建控件
	CRect rcCreate(0,0,0,0);

	//创建按钮
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

	//加载位图
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

	//创建控件
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_CardControl[i].Create(NULL,NULL,WS_VISIBLE|WS_CHILD,rcCreate,this,20+i);
		m_CardControlOx[i].Create(NULL,NULL,WS_VISIBLE|WS_CHILD,rcCreate,this,20+GAME_PLAYER+i);
		m_CardControlOx[i].SetDrawHeight(30);
	}

    //按钮提示
	m_ToolTipCtrl.Create(this);
	m_ToolTipCtrl.Activate(TRUE);
	m_ToolTipCtrl.AddTool(&m_btShortcut,TEXT("牛提示快捷键"));
	m_ScoreView.Create( IDD_GAME_SCORE,this );

	//创建字体
	m_ViewFontEx.CreateFont(this,TEXT("宋体"),12,400);

	return 0;
}

//命令函数
BOOL CGameClientView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case IDC_HINT_OX:			//提示按钮
		{
			//发送消息
			SendEngineMessage(IDM_HINT_OX,0,0);
			return TRUE;
		}
	case IDC_START:				//开始按钮
		{
			//发送消息
			SendEngineMessage(IDM_START,0,0);
			return TRUE;
		}
	case IDC_READY:				//摊牌按钮
		{
			//发送消息
			SendEngineMessage(IDM_READY,0,0);
			return TRUE;
		}
	case IDC_OX:				//牛牛按钮
		{
			//发送消息
			SendEngineMessage(IDM_OX,0,0);
			return TRUE;
		}
	case IDC_RESORT:			//重排按钮
		{
			//发送消息
			SendEngineMessage(IDM_RESORT,0,0);
			return TRUE;
		}
	case IDC_ONE_SCORE:			//加注按钮
		{
			//发送消息
			SendEngineMessage(IDM_ADD_SCORE,4,1);
			return TRUE;
		}
	case IDC_TWO_SCORE:			//加注按钮
		{
			//发送消息
			SendEngineMessage(IDM_ADD_SCORE,3,2);
			return TRUE;
		}
	case IDC_THREE_SCORE:		//加注按钮
		{
			//发送消息
			SendEngineMessage(IDM_ADD_SCORE,2,3);
			return TRUE;
		}
	case IDC_FOUR_SCORE:		//加注按钮
		{
			//发送消息
			SendEngineMessage(IDM_ADD_SCORE,1,4);
			return TRUE;
		}
	case IDC_SHORTCUT:			//快捷键按钮
		{
			//发送消息
			SendEngineMessage(IDM_SHORTCUT_KEY,0,0);
			return TRUE;
		}
	case IDC_BANKER:			//庄家按钮
		{
			//发送消息
			SendEngineMessage(IDM_BANKER,1,1);
			return TRUE;
		}
	case IDC_IDLER:				//闲家按钮
		{
			//发送消息
			SendEngineMessage(IDM_BANKER,0,0);
			return TRUE;
		}
	}

	return __super::OnCommand(wParam, lParam);
}

//重置界面
VOID CGameClientView::ResetGameView()
{
	//动画变量
	for (WORD i=0;i<GAME_PLAYER;i++) 
	{
		m_bOxValue[i]=0xff;
	}
	m_wSendCount=0;
	m_wSendIndex=0;
	m_SendCardPos.SetPoint(0,0);
	m_SendCardItemArray.RemoveAll();

	//数据变量
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

	//删除定时器
	KillTimer(IDI_SEND_CARD);

	//隐藏按钮
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

	//隐藏控件
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


//调整控件
VOID CGameClientView::RectifyControl(INT nWidth, INT nHeight)
{
	//发牌位置
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

	//调整坐标
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

	//用户扑克;
	m_CardControl[0].SetBenchmarkPos(nWidth/2,nHeight/2-290,enXCenter,enYTop);
	m_CardControlOx[0].SetBenchmarkPos(nWidth/2,nHeight/2-320,enXCenter,enYTop);
	m_CardControl[2].SetBenchmarkPos(nWidth/2,nHeight/2+270,enXCenter,enYBottom);
	m_CardControlOx[2].SetBenchmarkPos(nWidth/2,nHeight/2+240,enXCenter,enYBottom);
	m_CardControl[1].SetBenchmarkPos(nWidth/2+293,nHeight/2-30,enXRight,enYCenter);
	m_CardControlOx[1].SetBenchmarkPos(nWidth/2+283,nHeight/2-60,enXRight,enYCenter);
	m_CardControl[3].SetBenchmarkPos(nWidth/2-293,nHeight/2-30,enXLeft,enYCenter);
	m_CardControlOx[3].SetBenchmarkPos(nWidth/2-283,nHeight/2-60,enXLeft,enYCenter);

	//结束位置
	m_SendEndingPos[0].x=m_CardControl[0].GetCardPos().x;
	m_SendEndingPos[0].y=m_CardControl[0].GetCardPos().y;
	m_SendEndingPos[1].x=m_CardControl[1].GetCardPos().x;
	m_SendEndingPos[1].y=m_CardControl[1].GetCardPos().y;
	m_SendEndingPos[2].x=m_CardControl[2].GetCardPos().x;
	m_SendEndingPos[2].y=m_CardControl[2].GetCardPos().y;
	m_SendEndingPos[3].x=m_CardControl[3].GetCardPos().x;
	m_SendEndingPos[3].y=m_CardControl[3].GetCardPos().y;

	//牛值位置
	m_ptValue[0].x=nWidth/2-138;
	m_ptValue[0].y=nHeight/2-250;
	m_ptValue[1].x=nWidth/2+200;
	m_ptValue[1].y=nHeight/2+30;
	m_ptValue[2].x=nWidth/2-157;
	m_ptValue[2].y=nWidth/2+158;
	m_ptValue[3].x=nWidth/2-280;
	m_ptValue[3].y=nHeight/2+30;

	//按钮控件
	CRect rcButton;
	HDWP hDwp=BeginDeferWindowPos(32);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;

	//开始按钮
	m_btStart.GetWindowRect(&rcButton);
	DeferWindowPos(hDwp,m_btStart,NULL,nWidth/2-32,nHeight/2+112,0,0,uFlags);

	//控制按钮
	m_btReSort.GetWindowRect(&rcButton);
	DeferWindowPos(hDwp,m_btReSort,NULL,nWidth/2-32,nHeight/2+112,0,0,uFlags);
	DeferWindowPos(hDwp,m_btOpenCard,NULL,nWidth/2+25,nHeight/2+112,0,0,uFlags);
	DeferWindowPos(hDwp,m_btOx,NULL,nWidth/2-118,nHeight/2+112,0,0,uFlags);
	DeferWindowPos(hDwp,m_btHintOx,NULL,nWidth/2-88,nHeight/2+112,0,0,uFlags);

	//辅助按钮
	DeferWindowPos(hDwp,m_btShortcut,NULL,nWidth/2+330,nHeight/2+320,0,0,uFlags);

	//筹码按钮
	DeferWindowPos(hDwp,m_btOneScore,NULL,nWidth/2-151,nHeight/2+163,0,0,uFlags);
	DeferWindowPos(hDwp,m_btTwoScore,NULL,nWidth/2-67,nHeight/2+163,0,0,uFlags);
	DeferWindowPos(hDwp,m_btThreeScore,NULL,nWidth/2+15,nHeight/2+163,0,0,uFlags);
	DeferWindowPos(hDwp,m_btFourScore,NULL,nWidth/2+98,nHeight/2+163,0,0,uFlags);

	//选庄按钮
	DeferWindowPos(hDwp,m_btBanker,NULL,nWidth/2-76,nHeight/2+151,0,0,uFlags);
	DeferWindowPos(hDwp,m_btIdler,NULL,nWidth/2+14,nHeight/2+151,0,0,uFlags);

	//结束移动
	EndDeferWindowPos(hDwp);

	//加注窗口

	//积分视图
	CRect rcControl;
	m_ScoreView.GetWindowRect(&rcControl);
	CPoint ptPos( nWidth/2-183,nHeight/2-153 );
	ClientToScreen( &ptPos );
	m_ScoreView.SetWindowPos( NULL,ptPos.x,ptPos.y,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOCOPYBITS );
	
	return;
}

//绘画界面
VOID CGameClientView::DrawGameView(CDC * pDC, INT nWidth, INT nHeight)
{
	//绘画背景
	DrawViewImage(pDC,m_ImageViewCenter,DRAW_MODE_SPREAD);
	DrawViewImage(pDC,m_ImageViewBack,DRAW_MODE_CENTENT);	

	//绘画用户
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		WORD wUserTimer=GetUserClock(i);
		IClientUserItem * pClientUserItem=GetClientUserItem(i);

		//绘画用户
		if (pClientUserItem!=NULL)
		{
			//用户名字
			pDC->SetTextAlign((i==1)?TA_RIGHT:TA_LEFT);
			pDC->SetTextColor((wUserTimer>0)?RGB(250,250,250):RGB(220,220,220));

			//用户金币
			TCHAR szBuffer[64]=TEXT("");
			LONGLONG lLeaveScore=pClientUserItem->GetUserScore()-m_lTableScore[i];
			_sntprintf(szBuffer,sizeof(szBuffer),TEXT("￥")SCORE_STRING,lLeaveScore);

			//用户名字
			//DrawTextString(pDC,pClientUserItem->GetNickName(),RGB(255,255,255),RGB(0,0,0),m_ptNickName[i].x+TempX,m_ptNickName[i].y+TempY);
			CRect rcNickName(m_ptNickName[i].x,m_ptNickName[i].y,m_ptNickName[i].x+100,m_ptNickName[i].y+18);
			m_ViewFontEx.DrawText(pDC, pClientUserItem->GetNickName(),&rcNickName,RGB(255,255,255),DT_LEFT|DT_TOP|DT_SINGLELINE|DT_END_ELLIPSIS);

			//用户金币
			//DrawTextString(pDC,szBuffer,RGB(255,255,255),RGB(0,0,0),m_ptNickName[i].x+TempX,m_ptNickName[i].y+16+TempY);
			CRect rcUserGold(m_ptNickName[i].x,m_ptNickName[i].y+16,m_ptNickName[i].x+100,m_ptNickName[i].y+34);
			m_ViewFontEx.DrawText(pDC, szBuffer,&rcUserGold,RGB(255,255,255),DT_LEFT|DT_TOP|DT_SINGLELINE|DT_END_ELLIPSIS);

			//其他信息
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

	//等待下注
	if (m_bInvest==true)
	{
		//创建资源
		CFont InfoFont;
		InfoFont.CreateFont(-24,0,0,0,700,0,0,0,134,3,2,1,1,TEXT("楷体_GB2312"));

		//设置 DC
		pDC->SetTextAlign(TA_CENTER|TA_TOP);
		CFont * pOldFont=pDC->SelectObject(&InfoFont);

		//绘画信息
		TCHAR zsInfo[128]=TEXT("");
		_sntprintf(zsInfo,sizeof(zsInfo),TEXT("等待闲家下注"));
		//DrawTextString(pDC,zsInfo,RGB(250,200,40),RGB(0,0,0),nWidth/2,nHeight/2);
		CDFontEx InfoFontEx;
		InfoFontEx.CreateFont(this,TEXT("楷体_GB2312"),24,700);
		InfoFontEx.DrawText(pDC,zsInfo,nWidth/2,nHeight/2,RGB(250,200,40),DT_CENTER|DT_TOP);
		InfoFontEx.DeleteFont();

		//清理资源
		pDC->SelectObject(pOldFont);
		InfoFont.DeleteObject();
	}

	//特殊牌型
	if (m_bUserAction==true)
	{
		//创建资源
		CFont InfoFont;
		InfoFont.CreateFont(-16,0,0,0,700,0,0,0,134,3,2,1,1,TEXT("楷体_GB2312"));

		//设置 DC
		pDC->SetTextColor(RGB(250,200,40));
		pDC->SetTextAlign(TA_CENTER|TA_TOP);
		CFont * pOldFont=pDC->SelectObject(&InfoFont);

		//绘画信息
		//pDC->TextOut(nWidth/2,nHeight/2+150,TEXT("特殊牌型,系统自动为你配牌 ^-^"));
		CDFontEx InfoFontEx;
		InfoFontEx.CreateFont(this,TEXT("楷体_GB2312"),16,700);
		InfoFontEx.DrawText(pDC,TEXT("特殊牌型,系统自动为你配牌 ^-^"),nWidth/2,nHeight/2,RGB(250,200,40),DT_CENTER|DT_TOP);
		InfoFontEx.DeleteFont();

		//清理资源
		pDC->SelectObject(pOldFont);
		InfoFont.DeleteObject();
	}

	//等待摊牌
	if (m_bOpenCard==true)
	{
		//创建资源
		CFont InfoFont;
		InfoFont.CreateFont(-24,0,0,0,700,0,0,0,134,3,2,1,1,TEXT("楷体_GB2312"));

		//设置 DC
		pDC->SetTextColor(RGB(250,200,40));
		pDC->SetTextAlign(TA_CENTER|TA_TOP);
		CFont * pOldFont=pDC->SelectObject(&InfoFont);

		//绘画信息
		TCHAR zsInfo[128]=TEXT("");
		_sntprintf(zsInfo,sizeof(zsInfo),TEXT("等待玩家摊牌"));
		//DrawTextString(pDC,zsInfo,RGB(250,200,40),RGB(0,0,0),nWidth/2,nHeight/2);
		CDFontEx InfoFontEx;
		InfoFontEx.CreateFont(this,TEXT("楷体_GB2312"),24,700);
		InfoFontEx.DrawText(pDC,zsInfo,nWidth/2,nHeight/2,RGB(250,200,40),DT_CENTER|DT_TOP);
		InfoFontEx.DeleteFont();

		//清理资源
		pDC->SelectObject(pOldFont);
		InfoFont.DeleteObject();
	}

	//等待叫庄
	if(m_bCallUser!=0xff && (m_bCallUser!=2 || m_bLookOnUser))
	{
		int y=m_SendEndingPos[m_bCallUser].y+10;
		int x=nWidth/2 - m_ImageWaitCall.GetWidth()/2;
		if(m_bCallUser==1)x=nWidth/2+273-120;
		else if(m_bCallUser==3)x=nWidth/2-273-30;
		else if(m_bCallUser==0)y+=30;
		m_ImageWaitCall.AlphaDrawImage(pDC,x,y,m_ImageWaitCall.GetWidth(),m_ImageWaitCall.GetHeight(),0,0,255);
	}

	//庄家信息
	if (m_wBankerUser!=INVALID_CHAIR)
	{
		IClientUserItem * pClientUserItem=GetClientUserItem(m_wBankerUser);
		if (pClientUserItem!=NULL)
		{
			//庄家标志
			m_ImageBanker.AlphaDrawImage(pDC,m_PointBanker[m_wBankerUser].x,m_PointBanker[m_wBankerUser].y,m_ImageBanker.GetWidth(), m_ImageBanker.GetHeight(),0,0,255);
		}
	}

	//绘画积分
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

			//创建字体
			CFont ViewFont;
			ViewFont.CreateFont(-12,0,0,0,700,0,0,0,134,3,2,1,1,TEXT("宋体"));

			//设置 DC
			pDC->SetTextAlign(TA_CENTER);
			CFont * pOldFont=pDC->SelectObject(&ViewFont);

			//绘画信息
			TCHAR tc[LEN_ACCOUNTS]=TEXT("");
			_sntprintf(tc,sizeof(tc),SCORE_STRING,m_lTableScore[i]);
			//DrawTextString(pDC,tc,RGB(250,250,250),RGB(0,0,0),m_ptJeton[i].x+32,m_ptJeton[i].y+8);
			CDFontEx::DrawText(this,pDC,12,700,tc,m_ptJeton[i].x+32,m_ptJeton[i].y+8,RGB(250,250,2500),DT_CENTER|DT_TOP);

			//清理资源
			pDC->SelectObject(pOldFont);
			ViewFont.DeleteObject();
		}
	}

	//显示摊牌标志
	for (BYTE i=0;i<GAME_PLAYER;i++)
	{
		if (m_bOpenCardUser[i]==TRUE)
		{
			//创建字体
			CFont ViewFont;
			ViewFont.CreateFont(-20,0,0,0,700,0,0,0,134,3,2,1,1,TEXT("华文彩云"));

			//设置 DC
			pDC->SetTextAlign(TA_LEFT);
			CFont * pOldFont=pDC->SelectObject(&ViewFont);

			//绘画信息
			TCHAR tc[LEN_ACCOUNTS]=TEXT("");
			_sntprintf(tc,sizeof(tc),TEXT("摊"));
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
			ViewFontEx.CreateFont(this,TEXT("华文彩云"),20,700);
			ViewFontEx.DrawText(pDC,tc,x,y,RGB(250,128,56),DT_LEFT);
			ViewFontEx.DeleteFont();

			//清理资源
			pDC->SelectObject(pOldFont);
			ViewFont.DeleteObject();
		}
	}

	//绘画扑克
	if (m_SendCardItemArray.GetCount()>0)
	{
		//变量定义
		tagSendCardItem * pSendCardItem=&m_SendCardItemArray[0];

		//获取大小
		int nItemWidth=m_ImageCard.GetWidth()/13;
		int nItemHeight=m_ImageCard.GetHeight()/5;

		//绘画扑克
		int nXPos=m_SendCardPos.x-nItemWidth/2;
		int nYPos=m_SendCardPos.y-nItemHeight/2+10;
		m_ImageCard.TransDrawImage(pDC,nXPos,nYPos,nItemWidth,nItemHeight,nItemWidth*2,nItemHeight*4,RGB(255,0,255));
	}

	//显示点数
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
						TCHAR tCh[64]= TEXT("花牛");
						if(m_bOxValue[i]==OX_THREE_SAME)_sntprintf(tCh,CountArray(tCh),TEXT("葫芦"));
						else if(m_bOxValue[i]==OX_FOUR_SAME)_sntprintf(tCh,CountArray(tCh),TEXT("炸弹"));
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


//提示信息
void CGameClientView::SetUserAction(bool bWait)
{
	if(m_bUserAction!=bWait)
	{
		m_bUserAction=bWait;

		//更新界面
		RefreshGameView();
	}

	//设置时间
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

//动画判断
bool CGameClientView::IsDispatchCard()
{
	return m_SendCardItemArray.GetCount()>0;
}

//完成发牌
void CGameClientView::FinishDispatchCard()
{
	//完成判断
	if (m_SendCardItemArray.GetCount()==0) return;

	//完成动画
	for (INT_PTR i=0;i<m_SendCardItemArray.GetCount();i++)
	{
		//获取数据
		tagSendCardItem * pSendCardItem=&m_SendCardItemArray[i];

		//获取扑克
		BYTE cbCardData[MAX_COUNT];
		WORD wChairID=pSendCardItem->wChairID;
		BYTE cbCardCount=(BYTE)m_CardControl[wChairID].GetCardData(cbCardData,CountArray(cbCardData));

		//设置扑克
		if (cbCardCount<MAX_COUNT)
		{
			cbCardData[cbCardCount++]=pSendCardItem->cbCardData;
			m_CardControl[wChairID].SetCardData(cbCardData,cbCardCount);
		}
	}

	SendEngineMessage(IDM_SEND_CARD_FINISH,0,0);

	//删除处理
	KillTimer(IDI_SEND_CARD);
	m_SendCardItemArray.RemoveAll();

	//更新界面
	RefreshGameView();

	return;
}

//发送扑克
void CGameClientView::DispatchUserCard(WORD wChairID, BYTE cbCardData)
{
	//效验参数
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return;

	//设置变量
	tagSendCardItem SendCardItem;
	SendCardItem.cbCardData=cbCardData;
	SendCardItem.wChairID=wChairID;

	//插入数组
	m_SendCardItemArray.Add(SendCardItem);

	//启动动画
	if (m_SendCardItemArray.GetCount()==1) 
	{
		//获取位置
		CRect rcClient;
		GetClientRect(&rcClient);

		//设置位置
		m_SendCardPos.x=m_SendStartPos.x;
		m_SendCardPos.y=m_SendStartPos.y;

		//位移计算
		int nXCount=abs(m_SendEndingPos[SendCardItem.wChairID].x-m_SendStartPos.x)/SEND_PELS;
		int nYCount=abs(m_SendEndingPos[SendCardItem.wChairID].y-m_SendStartPos.y)/SEND_PELS;

		//设置次数
		m_wSendIndex=0;
		m_wSendCount=__max(1,__max(nXCount,nYCount));

		//设置定时器
		SetTimer(IDI_SEND_CARD,TIME_SENDSPEED,NULL);

		//播放声音
		CGameClientEngine * pGameClientDlg=CONTAINING_RECORD(this,CGameClientEngine,m_GameClientView);
		pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("SEND_CARD"));
	}

	return;
}

//设置下注
void CGameClientView::SetUserTableScore(WORD wChairID, LONGLONG lTableScore)
{
	//设置数据
	if (wChairID!=INVALID_CHAIR) m_lTableScore[wChairID]=lTableScore;
	else ZeroMemory(m_lTableScore,sizeof(m_lTableScore));

	//更新界面
	RefreshGameView();
	
	return;
}

//庄家标志
void CGameClientView::SetBankerUser(WORD wBankerUser)
{
	m_wBankerUser=wBankerUser;

	//更新界面
	RefreshGameView();
	return;
}

//等待叫庄
void CGameClientView::SetWaitCall(BYTE bCallUser)
{
	m_bCallUser=bCallUser;

	//更新界面
	RefreshGameView();

	return ;
}

//等待标志
void CGameClientView::SetWaitInvest(bool bInvest)
{
	if(m_bInvest!=bInvest)
	{
		m_bInvest=bInvest;

		//更新界面
		RefreshGameView();
	}
	return;
}

//显示牌型
void CGameClientView::ShowType(bool bShow)
{
	if(m_bShow!=bShow)
	{
		m_bShow=bShow;

		//更新界面
		RefreshGameView();
	}

	return;
}

//显示点数
void CGameClientView::SetUserOxValue(WORD wChiarID,BYTE bValue)
{
	//牛牛数据
	m_bOxValue[wChiarID]=bValue;

	//更新界面
	RefreshGameView();

	return;
}

//显示摊牌
void CGameClientView::ShowOpenCard(WORD wChiarID,BOOL bShow)
{
	//摊牌用户
	m_bOpenCardUser[wChiarID]=bShow;

	//更新界面
	RefreshGameView();
	return;
}

//显示积分
void CGameClientView::ShowScore(WORD wChairID,bool bShow)
{
	//显示标志
	m_bShowSocre[wChairID]=bShow;

	//更新界面
	RefreshGameView();

	return;
}

//更新视图
void CGameClientView::RefreshGameView()
{
	CRect rect;
	GetClientRect(&rect);
	InvalidGameView(rect.left,rect.top,rect.Width(),rect.Height());
	return;
}

//用户状态
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

//定时器消息
void CGameClientView::OnTimer(UINT nIDEvent)
{
	//发送处理
	switch(nIDEvent)
	{
	case IDI_SEND_CARD:		//移动扑克
		{
			//设置索引
			m_wSendIndex++;

			//设置位置
			tagSendCardItem * pSendCardItem=&m_SendCardItemArray[0];
			if(pSendCardItem->wChairID%2!=0)
				m_SendCardPos.x=m_SendStartPos.x+(m_SendEndingPos[pSendCardItem->wChairID].x-m_SendStartPos.x)*m_wSendIndex/                              m_wSendCount;
			else
				m_SendCardPos.y=m_SendStartPos.y+(m_SendEndingPos[pSendCardItem->wChairID].y-m_SendStartPos.y)*m_wSendIndex/                              m_wSendCount;

			//停止判断
			if (m_wSendIndex>=m_wSendCount)
			{
				//获取扑克
				BYTE cbCardData[MAX_COUNT];
				WORD wChairID=pSendCardItem->wChairID;
				BYTE cbCardCount=(BYTE)m_CardControl[wChairID].GetCardData(cbCardData,CountArray(cbCardData));

				//设置扑克
				cbCardData[cbCardCount++]=pSendCardItem->cbCardData;
				m_CardControl[wChairID].SetCardData(cbCardData,cbCardCount);

				//删除动画
				m_SendCardItemArray.RemoveAt(0);

				//继续动画
				if (m_SendCardItemArray.GetCount()>0)
				{
					//变量定义
					tagSendCardItem * pSendCardItem=&m_SendCardItemArray[0];

					//设置位置
					m_SendCardPos.x=m_SendStartPos.x;
					m_SendCardPos.y=m_SendStartPos.y;

					//位移计算
					int nXCount=abs(m_SendEndingPos[pSendCardItem->wChairID].x-m_SendStartPos.x)/SEND_PELS;
					int nYCount=abs(m_SendEndingPos[pSendCardItem->wChairID].y-m_SendStartPos.y)/SEND_PELS;

					//设置次数
					m_wSendIndex=0;
					m_wSendCount=__max(1,__max(nXCount,nYCount));

					//播放声音
					CGameClientEngine * pGameClientDlg=CONTAINING_RECORD(this,CGameClientEngine,m_GameClientView);
					pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("SEND_CARD"));
				}
				else 
				{
					//完成处理
					KillTimer(IDI_SEND_CARD);
					SendEngineMessage(IDM_SEND_CARD_FINISH,0,0);
				}
			}

			//更新界面
			RefreshGameView();

			return ;
		}
	case IDI_USER_ACTION: 	//用户动作
		{
			//清理数据
			m_bUserAction = false;
			KillTimer(IDI_USER_ACTION);

			//更新界面
			RefreshGameView();

			return;
		}
	}

	__super::OnTimer(nIDEvent);
}

//////////////////////////////////////////////////////////////////////////

