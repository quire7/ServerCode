#ifndef CMD_SHOWHAND_HEAD_FILE
#define CMD_SHOWHAND_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//公共宏定义


#define KIND_ID						26									//游戏 I D;
//游戏房间信息
#define GAME_NAME					TEXT("掼蛋")							//游戏名字;

//组件属性
#define VERSION_SERVER				PROCESS_VERSION(6,0,3)				//程序版本;
#define VERSION_CLIENT				PROCESS_VERSION(6,0,3)				//程序版本;

//游戏信息
#define GAME_PLAYER					4									//游戏人数;
#define MAX_COUNT					28									//最大牌数;
#define HALF_COUNT					MAX_COUNT/2							//一半数量;
//游戏升级信息
#define SHUANG_XIA					10									//游戏双下;
#define DAN_XIA						11									//游戏单下;
#define OTHER						12									//对门末游;
//场景状态
#define GS_WK_FREE					GAME_STATUS_FREE					//等待开始;
#define GS_WK_SEND_CARD				GAME_STATUS_PLAY					//发牌状态;
#define GS_WK_TRIBUTE			    (GAME_STATUS_PLAY+1)				//进贡状态;
#define GS_WK_PLAYING				(GAME_STATUS_PLAY+2)				//进行状态;
#define GS_WK_WAIT_CONTINUE			(GAME_STATUS_PLAY+3)				//继续游戏等待状态;

//最大输赢倍数
#define MAX_RATE					3

//请求次数
#define	REQUEST_QUIT_COUNT			3

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//进贡阶段
enum enPayStatus
{
	enNone,																//无;
	enJin_Gong,															//进贡;
	enHui_Gong,															//回贡;
	enGet_Hui_Gong,														//获取回贡;
	enKang_Gong															//抗贡;
};

//游戏级数
struct Series
{
	BYTE                            cbOurSeries;                        //我方级数;
	BYTE                            cbOtherSeries;                      //对方级数;
	BYTE                            cbCurrentSeries;                    //本局级数;
};
//进贡信息
struct TributeInfo
{
	WORD							wFrom;						//进(回)贡玩家;
	WORD							wTo;						//进(回)贡给谁;
	BYTE							cbCard;						//进(回)贡的牌;
	TributeInfo()
	{
		wFrom = INVALID_CHAIR;
		wTo = INVALID_CHAIR;
		cbCard = 0;
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//空闲状态
struct CMD_S_StatusFree
{
	BYTE							cbTimePayTribute;					//进贡时间;
	BYTE							cbTimeRetTribute;					//回贡时间;
	BYTE							cbTimeHeadOutCard;					//首出时间;
	BYTE							cbTimeOutCard;						//出牌时间;
	BYTE							cbTimeTrustee;						//托管时间;

	BYTE							cbCurSeries;						//当前级数;
	LONG							lBaseScore;							//房间底注;
};

//发牌场景
struct CDM_S_StatusSendCard
{
	BYTE							cbTimePayTribute;					//进贡时间;
	BYTE							cbTimeRetTribute;					//回贡时间;
	BYTE							cbTimeHeadOutCard;					//首出时间;
	BYTE							cbTimeOutCard;						//出牌时间;
	BYTE							cbTimeTrustee;						//托管时间;

	LONG							lBaseScore;							//房间底注;
	Series							stSeries;							//当前等级情况;
	bool							bWin;								//偶数家上局是否赢;
	BYTE							cbHandCardCount[GAME_PLAYER];		//玩家手牌数量;
	BYTE							cbHandCardData[27];					//自己手牌数据;
};

//进贡时场景
struct CMD_S_StatusTribute
{
	BYTE							cbTimePayTribute;					//进贡时间;
	BYTE							cbTimeRetTribute;					//回贡时间;
	BYTE							cbTimeHeadOutCard;					//首出时间;
	BYTE							cbTimeOutCard;						//出牌时间;
	BYTE							cbTimeTrustee;						//托管时间;

	LONG							lBaseScore;							//房间底注;
	BYTE							cbCardData[MAX_COUNT];				//手牌数据;
	BYTE							cbCardCount[GAME_PLAYER];			//手牌数目;
	Series                          stSeries;                           //游戏级数;
	bool							bLastRoundWin;						//偶数家上局是否赢;
	BYTE							cbUserTrustee[4];					//玩家托管状态;
};

//玩时场景
struct CMD_S_StatusPlay
{
	BYTE							cbTimePayTribute;					//进贡时间;
	BYTE							cbTimeRetTribute;					//回贡时间;
	BYTE							cbTimeHeadOutCard;					//首出时间;
	BYTE							cbTimeOutCard;						//出牌时间;
	BYTE							cbTimeTrustee;						//托管时间;

	LONG							lBaseScore;							//房间底注;
	WORD							wLastOutUser;						//出牌的人;
	WORD				 			wCurrentUser;						//当前玩家;
	BYTE							cbCardData[27];						//手上扑克;
	BYTE							cbCardCount[4];						//扑克数目;
	BYTE							cbTurnCardData[10];					//出牌列表;
	BYTE							cbTurnCardCount;					//基础出牌;
	Series                          stSeries;                           //游戏级数;
	bool							bWin;								//偶数家上局是否赢;
	BYTE							cbUserTrustee[4];					//玩家托管状态;
	BYTE							cbRanking[4];						//排名;
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////
//服务器命令结构
#define SUB_S_START_OUT				101									//进贡完成后开始首家出牌;
#define SUB_S_OUT_CARD				102									//用户出牌;
#define SUB_S_PASS_CARD				103									//放弃出牌;
#define SUB_S_PAY_TRIBUTE_NOTIFY	104                                 //通知用户进贡;
#define SUB_S_PAY_TRIBUTE_CARD		105									//进贡的牌;
#define SUB_S_PAY_TRIBUTE_RESULT	106                                 //进贡结果;
#define SUB_S_RETURN_TRIBUTE		107									//回贡扑克;
#define SUB_S_GAME_END				108									//游戏结束;
#define SUB_S_REQUEST_QUIT			109									//请求退出;
#define SUB_S_RESPONSE_QUIT         110									//回应退出;
#define SUB_S_OPPOSITE_CARD			111									//对家的牌;

#define SUB_S_ALL_CARD				112									//所有扑克;
#define SUB_S_USER_OUT_OVER			113									//用户完牌;
#define SUB_S_SEND_CARD				121									//发牌消息;
#define SUB_S_USER_TRUST			122									//用户托管;
#define SUB_S_FORCE_QUIT			124									//逃跑;


//用户完牌消息;
struct CMD_S_User_Out_Over
{
	WORD							wChairID;							//完牌用户;
	BYTE							cbRanking;							//名次;
};

//所有扑克;
struct CMD_S_AllCard
{
	BYTE cbHandCardCount[GAME_PLAYER];
	BYTE cbHandCardData[GAME_PLAYER][MAX_COUNT];
};

//发送扑克;
struct CMD_S_Send_Card
{
	Series							stSeries;							//新一局等级;
	BYTE							cbCardData[27];						//牌数据;
};

//开始出牌;
struct CMD_S_StartOut
{
	WORD				 			wCurrentUser;						//当前玩家;
	WORD							wWinSideUser;						//台上玩家;
	BYTE							cbHandCardCount;					//扑克数量;
	BYTE							cbHandCardData[27];					//扑克数据;
};

//对家的牌;
struct CMD_S_Opposite_Card
{
	WORD							wFromUser;							//对家ID;
	BYTE							cbCardCount;						//扑克大小;
	BYTE							cbCardData[27];						//扑克数据;
};
//用户出牌;
struct CMD_S_OutCard
{
	WORD				 			wCurrentUser;						//当前玩家;
	WORD							wOutCardUser;						//出牌玩家;
	WORD							wLastOutUser;						//上家出牌玩家;
	BYTE							cbCardType;							//出牌类型;
	BYTE							cbCardCount;						//扑克数目;
	BYTE							cbCardData[10];						//扑克列表;
};
//放弃出牌;
struct CMD_S_PassCard
{
	BYTE							bNewTurn;							//一轮开始;
	WORD				 			wPassUser;							//放弃玩家;
	WORD				 			wCurrentUser;						//当前玩家;
	WORD							wJiefengUser;						//接风用户;
};

//通知进贡;
struct CMD_S_PayTributeNotify
{	
	BYTE							cbPayStatus;						//进贡阶段;
};

//进贡扑克;
struct CMD_S_PayTributeCard
{
	WORD							wChairID;							//座位号;
	BYTE							cbCardData;							//牌;
};

//进贡结果;
struct CMD_S_PayTributeResult
{
	TributeInfo						stTribute[2];						//进(回)贡信息;
};

//用户托管;
struct CMD_S_User_Trust
{
	WORD							wChairID;							//座位号;
	BYTE							cbUserTrustee;						//托管状态;
};
//游戏结束;
struct CMD_S_GameEnd
{
	BYTE							cbGameEndType;						//结束类型;
	LONG							lGameScore[4];						//游戏积分;
	BYTE							cbCardCount[4];						//扑克数目;
	BYTE							cbCardData[108];					//扑克列表;
	BYTE							cbRanking[4];						//游戏名次;
	bool							bTongGuan;							//是否通关;
	Series                          stSeries;                           //游戏级数;
};

//请求数据包;
struct CMD_S_RequestQuit
{
	WORD							wChairID;							//座位号;
};
//答复消息;
struct CMD_S_ResponseQuit
{
	WORD							wChairID;							//座位号;
	bool                            bResponse;							//回应答复;
};
//逃跑;
struct CMD_S_ForceQuit
{
	WORD							wChairID;
};


//////////////////////////////////////////////////////////////////////////
//客户端命令结构;
#define SUB_C_OUT_CARD				2									//用户出牌;
#define SUB_C_PASS_CARD				3									//放弃出牌;
#define SUB_C_PAY_TRIBUTE           4                                   //玩家进贡;
#define SUB_C_RETURN_TRIBUTE        5                                   //玩家回贡;
#define SUB_C_USER_TRUST			6									//用户托管;
#define SUB_C_HALFWAY_QUIT			7									//中途退出;
#define SUB_C_REQUEST_QUIT			8									//强求退出;
#define SUB_C_RESPONSE_QUIT         9									//回应答复;

//设置桌子参数;
struct CMD_C_SetTableInfo
{
	BYTE							cbSetSeries;						//必须打到几;
	LONG							lBaseScore;							//桌子底注;
	BYTE							cbSetTableAction;					//是否设置或同意;
};

//用户托管;
struct CMD_C_UserTrust
{
	BYTE							cbUserTrustee;						//托管状态;
};
//出牌数据包;
struct CMD_C_OutCard
{
	BYTE							cbCardCount;						//出牌数目;
	BYTE							cbCardData[27];						//扑克列表;
};
//请求数据包;
struct CMD_C_RequestQuit
{
	WORD							wChairID;							//座位号;
	BYTE							cbRequestQuitType;					//0:请求退出 1:强退;
};
//答复消息;
struct CMD_C_ResposeQuit
{
	WORD							wChairID;							//座位号;
	BYTE                            cbResponseQuitType;					//回应答复;
};

//用户进贡;
struct CMD_C_PayTribute
{
	BYTE							cbCard;								//进(回)贡的牌;
};

// 游戏解散结构;
struct CMD_S_Private_End_Info
{
	WORD		wMainChairID;				// 房主;
	BYTE		cbFinishCout;				// 完成局数;
	SCORE	    lAllScore[GAME_PLAYER];		// 总成绩;
	SCORE	    lDetails[GAME_PLAYER][16];	// 成绩明细(目前开房最大16局);

	void StreamValue(DataStream& kData, bool bSend)
	{
		if (bSend)
		{
			kData.pushValue(this, sizeof(CMD_S_Private_End_Info));
		}
		else
		{
			kData.popValue(this, sizeof(CMD_S_Private_End_Info));
		}
	}
};

#pragma pack()

#endif