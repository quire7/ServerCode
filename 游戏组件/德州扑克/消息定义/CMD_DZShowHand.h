#ifndef CMD_DZSHOWHAND_HEAD_FILE
#define CMD_DZSHOWHAND_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//公共宏定义

#define KIND_ID							7									//游戏 I D
#define GAME_PLAYER						9									//游戏人数
#define GAME_NAME						TEXT("德州扑克")					//游戏名字

#define GOLD_RATE                       30                                 //倍数封顶

//版本信息
#define VERSION_SERVER					PROCESS_VERSION(0,0,1)				//程序版本
#define VERSION_CLIENT					PROCESS_VERSION(0,0,1)				//程序版本

//结束原因
#define GER_NO_PLAYER					0x10								//没有玩家

//数目定义
#define FULL_COUNT					    52									//全牌数目
#define MAX_COUNT						2									//最大数目
#define MAX_CENTERCOUNT					5									//最大数目

//数值掩码
#define	CARD_MASK_COLOR				0xF0									//花色掩码
#define	CARD_MASK_VALUE				0x0F									//数值掩码

#define  SMALL_CARD_WIDTH			23
#define  SMALL_CARD_HEIGHT			30

//打赏花费
#define DASHANG_COST    500

//机器人参数
#define INT_ROBOT_QUANYA				21									//全押或者开牌
#define INT_ROBOT_GIVEUP				42									//配置参数放弃率的数量
#define INT_ROBOT_ADD   				32									//配置参数跟加率的数量
#define INT_ROBOT_CARDLEVEL				5									//牌等级
#define INT_ROBOT_BEI   				5									//配置参数比赛配置的数量
#define INT_ROBOT_AI  			    	3									//配置参数AI等级配置
#define INT_ROBOT_FIRSTCARD				5									//牌等级
#define INT_ROBOT_CARDTYPE              5                                   //配置
//X 排列方式
enum enXCollocateMode 
{
	enXLeft,						//左对齐
	enXCenter,						//中对齐
	enXRight,						//右对齐
};

//Y 排列方式
enum enYCollocateMode 
{ 
	enYTop,							//上对齐
	enYCenter,						//中对齐
	enYBottom,						//下对齐
};

#define VALIDE_CHAIRID(wChairID) ((wChairID) >=0 && (wChairID) < GAME_PLAYER)
//扑克结构
struct tagCardItem
{
	bool							bEffect;							//特效标志
	bool							bMy;								//自己标志
	bool							bShoot;								//弹起标志
	BYTE							cbCardData;							//扑克数据
};



//属性定义
#define MAX_CARD_COUNT				5									//扑克数目
#define SPACE_CARD_DATA				255									//间距扑克


//间距定义
#define DEF_X_DISTANCE				19									//默认间距
#define DEF_Y_DISTANCE				17									//默认间距
#define DEF_SHOOT_DISTANCE			20									//默认间距

//下注类型
#define CHIP_TYPE_ADD     1    //加注
#define CHIP_TYPE_GEN     2    //跟
#define CHIP_TYPE_ALLIN   3    //全下
#define CHIP_TYPE_PASS    4    //让牌
#define CHIP_TYPE_GIVEUP  5    //放弃
#define CHIP_TYPE_OVER    6    //不能下注  游戏结束
//////////////////////////////////////////////////////////////////////////
//服务器命令结构
#define SUB_S_GAME_START				100							//游戏开始
#define SUB_S_ADD_SCORE					101							//加注结果
#define SUB_S_GIVE_UP					102							//放弃跟注
#define SUB_S_TURN_OVER					103							//一轮结束
#define SUB_S_SEND_CARD					104							//发牌消息
#define SUB_S_GAME_END					105							//游戏结束
#define SUB_S_SIT_DOWN					106							//用户坐下
#define SUB_S_OPEN_CARD					107							//用户开牌
#define SUB_S_LOOK_CARD					108							//用户亮牌
#define SUB_S_USER_ROBOT				111							//机器人
#define SUB_S_GAN_READY                 112                         //机器人准备

#define SUB_S_SET_BASESCORE				113							//设置底注
#define SUB_S_KICK_OUT					114							//踢出玩家
#define SUB_S_ADD_TAKEGOLD				115							//增加带入金币
#define SUB_S_DASHANG					116							//打赏

//游戏状态
struct CMD_S_StatusFree
{
	LONG							lBaseScore;							//基础积分
	LONG                            lAddTakeGoldMin;                    //增加带入金币最小值
	LONG                            lAddTakeGoldMax;                    //增加带入金币最大值
	LONG                            lUserTakeGold[GAME_PLAYER];         //玩家带入金币
};

//游戏状态
struct CMD_S_StatusPlay
{
	//标志变量	
	//加注信息
	LONG								lCellScore;							//单元下注
	LONG								lTurnMaxScore[GAME_PLAYER];			//最大下注
	LONG								lTurnLessScore;						//最小下注
	LONG								lCellMaxScore;						//最大下注
	LONG								lAddLessScore;						//加最小注
	LONG                                lAllChipScore;                      //当前总下注
	LONG								lTableScore[GAME_PLAYER];			//下注数目
	LONG								lTotalScore[GAME_PLAYER];			//累计下注
	LONG                                lTableBuffer[GAME_PLAYER];          //本局下注
	LONG                                lUserMaxScore[GAME_PLAYER];         //最大携带
	LONG                                aryChipPool[GAME_PLAYER+1];         //底池筹码
	LONG                                aryEndScore[GAME_PLAYER];  

	//状态信息
	WORD								wDUser;								//D玩家
	WORD								wDMUser[2];							//盲注玩家

	WORD				 				wCurrentUser;						//当前玩家
	BYTE								cbPlayStatus[GAME_PLAYER];			//游戏状态
	BYTE								cbGiveUpStatus[GAME_PLAYER];		//游戏状态
	BYTE							    cbBalanceCount;						//平衡次数
	BYTE                                cbChipPoolCount;                    //边池数量
	BYTE								bGameId;
	//扑克信息
	BYTE								cbCenterCardData[MAX_CENTERCOUNT];	//扑克数目
	BYTE								cbCenterCount;	                     //中心扑克数目
	BYTE								cbHandCardData[MAX_COUNT];			//桌面扑克
	LONG                                lAddTakeGoldMin;                    //增加带入金币最小值
	LONG                                lAddTakeGoldMax;                    //增加带入金币最大值
	LONG                                lUserTakeGold[GAME_PLAYER];         //玩家带入金币
};

//机器人信息
struct CMD_S_Robot
{
	BYTE								BRobot;                             //是否置换
	BYTE                                BAi;                                //机器人AI智能
	BYTE								cbCardData[GAME_PLAYER][MAX_COUNT];			//手牌	
	BYTE								cbCenterCardData[MAX_CENTERCOUNT];	//中心扑克
	LONG                                clLbei[INT_ROBOT_BEI];              //倍数配置	
	LONG							    lRobotAdd[INT_ROBOT_ADD];			//机器人加注率
	LONG							    lRobotQuanYa[INT_ROBOT_QUANYA];		//机器人全押率
	LONG								lRobotGiveUp[INT_ROBOT_GIVEUP];		//机器人放弃率	
};

//发送扑克
struct CMD_S_GameStart
{
	WORD				 				wCurrentUser;						//当前玩家
	WORD								wDUser;								//D玩家
	WORD								wSmallChipInUser;					//小盲注玩家	
	WORD								wMaxChipInUser;						//大盲注玩家	
	LONG								lCellScore;							//单元下注
	LONG								lTurnMaxScore[GAME_PLAYER];			//最大下注
	LONG								lUserMaxScore[GAME_PLAYER];			//最大注
	LONG								lTurnLessScore;						//最小下注
	LONG								lAddLessScore;						//加最小注
	LONG								lAddSmall;						    //加小盲注系数
	LONG								lAddMax;						    //加大盲注系数
	LONG                                lTableScore[GAME_PLAYER];
	BYTE                                UserCount;                          //玩家人数
	BYTE								cbCardData[MAX_COUNT];				//用户扑克
	//BYTE								cbCardData[GAME_PLAYER][MAX_COUNT];	//用户扑克
	//BYTE								cbAllData[GAME_PLAYER][MAX_COUNT];	//用户扑克
	BYTE                                cbPlayStatus[GAME_PLAYER];
	BOOL                                cbShowHand[GAME_PLAYER];            //开局梭哈
};


//设置底注
struct CMD_S_Set_BaseScore
{
	BYTE                                bSetOrConfirm;                         //  0 未设置 1让wPlayerID设置 2让wPlayerID同意       
	WORD								wChairID;							//玩家设置
	LONG                                lBaseScore;                         //底注
	LONG                                lCellScore;                       //房间最小底注限制
	LONG                                lMaxCellScore;                    //房间的最大底注限制
};
struct CMD_S_KickOut
{
	BYTE                                bReasion;                     //踢人原因
	WORD                                wChairID;                     //被踢玩家ID
};

//盲注玩家
struct CMD_S_MangZhu
{
	BYTE                                 BPlayEndType;                 
};

//用户下注
struct CMD_S_AddScore
{
	WORD								wCurrentUser;						//当前用户
	WORD								wAddScoreUser;						//加注用户
	LONG								lAddScoreCount;						//加注数目
	LONG								lTurnLessScore;						//最少下注
	LONG								lTurnMaxScore[GAME_PLAYER];			//最大下注
	LONG								lUserMaxScore[GAME_PLAYER];			//最大注
	LONG                                lTableScore[GAME_PLAYER];	        //当前下注
	LONG                                lTotalScore[GAME_PLAYER];	        //当前下注
	LONG								lAddLessScore;						//加最小注	
	BYTE                                cbChipTypeID;
	LONG                                lAllScore;
	BYTE                                cbBalanceCount;
};

//一轮结束
struct CMD_S_TurnOver
{
	WORD								wAddScoreUser;						//加注用户
	LONG								lAddScoreCount;						//加注数目
	LONG								lTurnLessScore;						//最少下注
	LONG								lTurnMaxScore;						//最大下注(身上剩余积分)
	LONG								lAddLessScore;						//加最小注
	BYTE                                cbChipTypeID;
	LONG                                lAllScore;
	LONG                                lTableScore[GAME_PLAYER];	        //本轮下注
	LONG                                lTotalScore[GAME_PLAYER];	        //累计下注
	BYTE                                cbChipPoolCount;					//奖池数量
	LONG                                aryChipPool[GAME_PLAYER + 1];		//底池筹码
	BYTE                                cbBalanceCount;
};

//用户放弃
struct CMD_S_GiveUp
{
	WORD								wGiveUpUser;						//放弃用户
	LONG								lLost;								//输掉金币	                           
	WORD                                wID;						        //是否看牌  
	WORD                                wLookUser;						    //看牌用户 
	LONG                                LAllScore;                          //底池筹码
};

//发牌数据包
struct CMD_S_SendCard
{
	BYTE								cbPublic;							//是否公牌
	WORD								wCurrentUser;						//当前用户
	BYTE								cbSendCardCount;					//发牌数目
	BYTE								cbCenterCardData[MAX_CENTERCOUNT];	//中心扑克
	LONG                                LAllScore;
	LONG                                ChipPool[GAME_PLAYER+1];
	LONG                                lTableScore[GAME_PLAYER];
	LONG                                lTurnMaxScore;                      //当前可下注最大
};

//游戏结束
struct CMD_S_GameEnd
{
	BYTE								cbTotalEnd;							//强退标志
	LONG								lGameTax[GAME_PLAYER];				//游戏服务费
	LONG								lGameScore[GAME_PLAYER];			//游戏得分
	LONG								lChipScore[GAME_PLAYER];			//剩余筹码
	LONG								lTotalScore[GAME_PLAYER];			//累计下注
	LONG								lWinChipScore[GAME_PLAYER];			//赢得的筹码
	LONG								lChipPool[GAME_PLAYER+1];           //边池筹码
	BYTE								cbCardData[GAME_PLAYER][MAX_COUNT];	//用户扑克	
	BYTE								cbCenterCardData[MAX_CENTERCOUNT];	//中心扑克
	BYTE								cbLastCenterCardData[GAME_PLAYER][MAX_CENTERCOUNT];//最后扑克
	BYTE                                cbCardType[GAME_PLAYER];				//玩家牌型
	BYTE                                cbWinUserId[GAME_PLAYER][GAME_PLAYER];	//每一波筹码将被谁获得
	LONG								lChipPoolScore[GAME_PLAYER][GAME_PLAYER];//每个奖池中筹码分配
	BYTE                                cbWinCount[GAME_PLAYER];			//每一波筹码被几个人获得
	BYTE                                cbChipPoolCount;                    //边池数量
	BYTE                                cbPlayStatus[GAME_PLAYER];          //游戏状态
	BYTE                                cbPlayGiveUp[GAME_PLAYER];          //放弃状态
	LONG                                lAllScore;
};

//用户亮牌
struct CMD_S_LiangCard
{
	BYTE                                BLiangId;                       //是否亮牌
	WORD								wLiangUser;						//亮牌用户
	BYTE								cbCardData[MAX_COUNT];							//亮牌扑克	

};

//游戏结束
struct CMD_S_OpenCard
{
	BYTE                                wID;                              
	WORD								wWinUser;							//用户开牌
};

//增加带入
struct CMD_S_Add_TakeGold
{
	WORD             wChairID;                //玩家
	LONG             lAddTakeGold;            //增加的带入金币数
	LONG             lAllTakeGold;            //玩家所有带入金币
};
//打赏
struct CMD_S_DaShang
{
	WORD	wChairID;
	//LONG       lUserTakeGold[GAME_PLAYER];         //玩家带入金币变化
	LONG	lUserScore;
};
////用户坐下
//struct CMD_S_SitDown
//{
//	WORD								wChairID;							//用户坐号
//};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构
#define SUB_C_ADD_SCORE					1									//用户加注
#define SUB_C_GIVE_UP					2									//放弃消息
#define SUB_C_NOLOOK_CARD				6									//不亮消息
#define SUB_C_LOOK_CARD					16									//亮牌消息
#define SUB_C_LOOKNO_CARD				17									//不亮牌消息
#define SUB_C_GAMEOVER				   12									//是否结束游戏
#define SUB_C_START_ROB				   13									//是否开始加注

#define SUB_C_PLAY_END				   14									//动画结束
#define SUB_C_SET_BASESCORE			   15									//用户设置底注
#define SUB_C_ADD_TAKEGOLD             18                                   //玩家增加带入
#define SUB_C_DASHANG                  19                                   //打赏

#define SETBASE_WAIT                0
#define SETBASE_SET                 1
#define SETBASE_CONFIRM             2
#define SETBASE_OVER                3
#define SETBASE_NOTAGREE            4
//用户加注
struct CMD_C_AddScore
{
	LONG								lScore;								//加注数目
	BYTE                                ID;                                 
};

//用户加注
struct CMD_C_PlatEnd
{
	BYTE                                ID;                                 
};
//用户设置底注
struct CMD_C_SetBaseScore
{
	BYTE                                bSetOrConfirm;                           //1 设置 0 确认
	WORD								bUserChairID;								//
	LONG                                lBaseScore;
};
//增加带入金币
struct CMD_C_Add_TakeGold
{
	LONG       lAddTakeGold;
};
//打赏
struct CMD_C_DaShang
{
	WORD    wChairID;
};
//////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif