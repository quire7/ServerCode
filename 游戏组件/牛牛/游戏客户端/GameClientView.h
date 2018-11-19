#pragma once

#include "Stdafx.h"
#include "ScoreView.h"
#include "CardControl.h"
#include "GoldControl.h"

//////////////////////////////////////////////////////////////////////////

//消息定义
#define IDM_HINT_OX					WM_USER+99							//提示消息
#define IDM_START					WM_USER+100							//开始消息
#define IDM_READY					WM_USER+101							//摊牌消息
#define IDM_OX						WM_USER+102							//牛牛消息
#define IDM_MAX_SCORE				WM_USER+103							//最大加注
#define IDM_SEND_CARD_FINISH		WM_USER+104							//发牌完成
#define IDM_SHOW_VALUE				WM_USER+105							//点数信息
#define IDM_SHOW_INFO				WM_USER+106							//提示信息
#define IDM_OX_ENABLE				WM_USER+107							//牛牛按钮
#define IDM_SORT					WM_USER+108							//扑克分类
#define IDM_RESORT					WM_USER+109							//重排消息
#define IDM_ADD_SCORE				WM_USER+110							//加注消息
#define IDM_SHORTCUT_KEY			WM_USER+111							//快捷键
#define IDM_BANKER					WM_USER+112							//做庄消息
#define IDM_CLICK_CARD				WM_USER+114							//点击消息


#define MY_VIEW_CHAIRID				2									//椅子位置

//////////////////////////////////////////////////////////////////////////
//结构定义

//发牌子项
struct tagSendCardItem
{
	WORD							wChairID;							//发牌用户
	BYTE							cbCardData;							//发牌数据
};

//数组说明
typedef CWHArray<tagSendCardItem,tagSendCardItem &> CSendCardItemArray;

//////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameViewGDI
{
	//数据变量
protected:
	WORD							m_wBankerUser;						//庄家用户
	LONGLONG						m_lTableScore[GAME_PLAYER];		//下注数目
	BYTE							m_bOxValue[GAME_PLAYER];			//牛牛数据
	BYTE							m_bOpenCardUser[GAME_PLAYER];		//摊牌用户
	BYTE							m_bCallUser;						//等待叫庄
	BYTE							m_cbPlayStatus[GAME_PLAYER];		//游戏状态
	bool							m_bInvest;							//等待下注
	bool							m_bShowSocre[GAME_PLAYER];			//显示积分
	bool							m_bUserAction;						//等待标志

	//牌型数据
public:
	bool							m_bShow;							//显示牌型
	bool							m_bLookOnUser;						//旁观标志
	bool							m_bOpenCard;						//等待摊牌

	//动画变量
protected:
	CPoint							m_SendCardPos;						//发牌位置
	WORD							m_wSendCount;						//移动次数
	WORD							m_wSendIndex;						//移动次数
	CPoint							m_SendEndingPos[GAME_PLAYER];		//桌牌位置
	CPoint							m_SendStartPos;						//始发位置
	CSendCardItemArray				m_SendCardItemArray;				//发牌数组

	//按钮控件
public:
	CSkinButton						m_btHintOx;							//提示按钮
	CSkinButton						m_btStart;							//开始按钮
	CSkinButton						m_btReSort;							//重排按钮
	CSkinButton						m_btOpenCard;						//摊牌按钮
	CSkinButton						m_btOx;								//牛牛按钮
	CSkinButton						m_btShortcut;						//快捷键按钮
	CSkinButton						m_btOneScore;						//一分按钮
	CSkinButton						m_btTwoScore;						//二分按钮
	CSkinButton						m_btThreeScore;						//三分按钮
	CSkinButton						m_btFourScore;						//四分按钮
	CSkinButton						m_btBanker;							//庄家按钮
	CSkinButton						m_btIdler;							//闲家按钮

	//控件变量
public:
	CScoreView						m_ScoreView;						//成绩窗口
	CCardControl					m_CardControl[GAME_PLAYER];			//用户扑克
	CCardControl					m_CardControlOx[GAME_PLAYER];		//用户扑克

	//位置信息
protected:
	CPoint							m_PointBanker[GAME_PLAYER];			//庄家位置
	CPoint							m_ptJeton[GAME_PLAYER];				//金币位置
	CPoint							m_ptValue[GAME_PLAYER];				//点数位置

	//界面变量
protected:
	CPngImageEx						m_ImageBanker;						//庄家资源
	CPngImageEx						m_ImageOxValue;						//牛值资源
	CPngImageEx						m_ImageDoulbeOx;					//牛牛资源
	CPngImageEx						m_ImageWaitCall;					//等待资源
	CPngImageEx						m_ImageNumber;						//数字资源

	CBitImageEx						m_ImageCard;						//扑克资源
	CBitImageEx						m_ImageViewBack;					//背景资源
	CBitImageEx						m_ImageViewCenter;					//背景资源
	CBitImageEx						m_ImageJeton;						//筹码资源
	CBitImageEx						m_ImageOxValueZero;					//牛值资源
	CBitImageEx						m_ImageOpenCard;					//摊牌资源
	CBitImageEx						m_ImageScore;						//筹码资源

	//提示变量
protected:
	CToolTipCtrl					m_ToolTipCtrl;						//提示控件
	CDFontEx                        m_ViewFontEx;                       //视图字体

	//函数定义
public:
	//构造函数
	CGameClientView();
	//析构函数
	virtual ~CGameClientView();
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//重载函数
protected:
	//命令函数
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//继承函数
private:
	//重置界面
	virtual VOID ResetGameView();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);
	//绘画界面
	virtual VOID DrawGameView(CDC * pDC, INT nWidth, INT nHeight);
	//WIN7支持
	virtual bool RealizeWIN7() { return true; }

	//功能函数
public:
	//提示信息
	void SetUserAction(bool bWait);
	//动画判断
	bool IsDispatchCard();
	//完成发牌
	void FinishDispatchCard();
	//发送扑克
	void DispatchUserCard(WORD wChairID, BYTE cbCardData);
	//设置下注
	void SetUserTableScore(WORD wChairID, LONGLONG lTableScore);
	//庄家标志
	void SetBankerUser(WORD wBankerUser);
	//等待叫庄
	void SetWaitCall(BYTE bCallUser);
	//等待标志
	void SetWaitInvest(bool Invest);
	//显示牌型
	void ShowType(bool bShow);
	//设置点数
	void SetUserOxValue(WORD wChiarID,BYTE bValue);
	//获取点数
	BYTE GetValue(WORD wChiarID){return m_bOxValue[wChiarID];}
	//显示摊牌
	void ShowOpenCard(WORD wChiarID,BOOL bShow=TRUE);
	//显示积分
	void ShowScore(WORD wChiarID,bool bShow);
	//更新视图
	void RefreshGameView();
	//用户状态
	void SetUserPlayingStatus(WORD wChairID, BYTE cbPlayingStatus);

	//消息映射
protected:
	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//定时器消息
	afx_msg void OnTimer(UINT nIDEvent);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
