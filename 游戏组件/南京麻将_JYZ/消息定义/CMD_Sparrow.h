#ifndef CMD_SPARROW_HEAD_FILE
#define CMD_SPARROW_HEAD_FILE

#pragma pack(push)  
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////////////////////
//公共宏定义
#define KIND_ID						302										//游戏 I D
#define GAME_PLAYER					4										//游戏人数
#define GAME_NAME					TEXT("南京麻将进园子")					//游戏名字

#define VERSION_SERVER				PROCESS_VERSION(6,0,3)					//程序版本
#define VERSION_CLIENT				PROCESS_VERSION(6,0,3)					//程序版本

//////////////////////////////////////////////////////////////////////////////////////////////////

//常量定义
#define MAX_WEAVE					4											//最大组合
#define MAX_INDEX					42											//所有牌最大索引
#define MAX_NORMAL					31											//非花牌最大索引
#define MAX_COUNT					14											//最大数目
#define MAX_HUA_COUNT				20											//花牌最大数目
#define MAX_REPERTORY				144											//最大库存

//最大听牌张数
#define MAX_TING_OUT				14											//最多可打出后听
#define MAX_TING_COUNT				9											//最多听9张牌

//发牌方位
#define SEND_CARD_FRONT				0											//从前面发牌
#define SEND_CARD_BACK				1											//从后面发牌

//////////////////////////////////////////////////////////////////////////////////////////////////
//游戏小结缘由
#define PAY_MONEY_MINGGANG			0											//明杠
#define PAY_MONEY_ANGANG			1											//暗牌
#define PAY_MONEY_HUAGANG			2											//花杠
#define	PAY_MONEY_SILIANFA			3											//四连发(四个人连出一样的牌)
#define PAY_MONEY_MEISIZHANG		4											//霉四张(一人出四张一样的牌)
#define PAY_MONEY_SHOUSIFENG		5											//首四风(前四张东、南、西、北或北、西、南、东)

//小结时，花基数
#define BASE_HUA_COUNT_MGANG		10											//明杠，花数
#define BASE_HUA_COUNT_AGANG		5											//暗杠，花数
#define BASE_HUA_COUNT_BGANG		10											//补杠，花数

#define BASE_HUA_COUNT_HGANG		10											//花杠，花数
#define BASE_HUA_COUNT_FAFEN		10											//罚分，花数

#define BASE_HUA_COUNT_AGANG_WJZ	5											//暗杠，花数
#define BASE_HUA_COUNT_JIANGFEN_WJZ	5											//奖分，花数
#define BASE_HUA_COUNT_JIANGFEN_BX	10											//奖分，花数

#define BASE_BAO_ZI_SCORE			100											//包子积分;
//////////////////////////////////////////////////////////////////////////////////////////////////
//游戏抽奖
#define MAX_LOTTERY_COUNT			5											//可选抽奖最大数量
//////////////////////////////////////////////////////////////////////////////////////////////////
//定时器定义
#define IDI_BASESCORE				1											//底注设置定时器					
#define IDI_SHOOT_DICE				2											//掷骰子定时器
#define IDI_DISPATCH_CARD			3											//发牌计时器
#define IDI_SEND_CARD				4											//摸牌计时器
#define IDI_SEND_BUHUA				5											//补花计时器
#define IDI_OUT_CARD				6											//出牌计时器
#define IDI_USER_OPERATE			7											//操作计时器
#define IDI_USER_OFFLINE			10											//断线延时托管计时器
//定时器时间
#define IDT_BASESCORE               25											//设置底注时间
#define IDT_SHOOT_DICE				1											//掷骰子时间
#define IDT_DISPATCH_CARD			10											//发牌时间
#define IDT_SEND_CARD				2											//摸牌时间
#define IDT_SEND_BUHUA				2											//补花时间
#define IDT_OUT_CARD				10											//出牌时间
#define IDT_USER_OPERATE			20											//操作时间
#define IDT_USER_TRUST				1											//托管时间
#define IDT_USER_TING				1											//听牌时间
#define IDT_USER_OFFLINE			15											//断线延时托管时间
//定时器类型
#define IDS_TIMER_NORMAL			0											//正常状态
#define	IDS_TIMER_TRUST				1											//托管状态
#define IDS_TIMER_TING				2											//听牌状态

//底注相关
#define STATUS_SBS_ERROR			0
#define STATUS_SBS_SET				1
#define STATUS_SBS_WAIT				2
#define STATUS_SBS_NOTIFY			3
#define ACTION_SBS_SET				4
#define ACTION_SBS_NOSET			5
#define ACTION_SBS_AGREE			6
#define ACTION_SBS_DISAGREE			7
//客户端服务器公用变量  底注和金币的比率
#define  GOLD_RATE                  400

//////////////////////////////////////////////////////////////////////////////////////////////////
//组合子项
struct tagWeaveItem
{
	DWORD							dwWeaveKind;								//组合类型
	BYTE							cbCenterCard;								//中心牌
	BYTE							cbPublicCard;								//公开标志
	WORD							wProvideUser;								//供应用户
};

struct tagTingPaiItemData;
//听牌子项
struct tagTingPaiItem
{
	BYTE							cbOutCardData;								//打出去的牌
	BYTE							cbTingCardCount;							//听牌数目
	BYTE							cbTingCardData[MAX_TING_COUNT];				//听牌数据
	BYTE							cbLeftCardCount[MAX_TING_COUNT];			//剩余数目
	DWORD							dwHuCardFanShu[MAX_TING_COUNT];				//听牌番数

	inline tagTingPaiItem& operator=(const tagTingPaiItemData& TingPaiItemData);
};
//杠牌结果
struct tagGangPaiResult
{
	BYTE							cbCardCount;								//牌数目
	BYTE							cbCardData[4];								//牌数据
	DWORD                           dwGangType[4];								//杠牌类型
};

struct tagTingPaiDataStream;
//听牌结果
struct tagTingPaiResult
{
	BYTE							cbTingPaiItemCount;							//可打牌数目
	tagTingPaiItem					TingPaiItemArrary[MAX_TING_OUT];			//可听牌数据

	tagTingPaiResult()
	{
		memset(this, 0, sizeof(tagTingPaiResult));
	}

	inline tagTingPaiResult(tagTingPaiDataStream& refTingPaiDataStream);
};

//听牌子项
struct tagTingPaiItemData
{
	BYTE							cbOutCardData;								//打出去的牌
	BYTE							cbTingCardCount;							//听牌数目
	std::vector<BYTE>				cbTingCardData;								//听牌数据;
	std::vector<BYTE>				cbLeftCardCount;							//剩余数目;
	std::vector<DWORD>				dwHuCardFanShu;								//听牌番数;

	tagTingPaiItemData()
	{
		cbOutCardData = 0;
		cbTingCardCount = 0;

		cbTingCardData.clear();
		cbLeftCardCount.clear();
		dwHuCardFanShu.clear();
	}

	tagTingPaiItemData(const tagTingPaiItemData& TingPaiItem)
	{
		cbOutCardData = TingPaiItem.cbOutCardData;
		cbTingCardCount = TingPaiItem.cbTingCardCount;

		cbTingCardData.assign(TingPaiItem.cbTingCardData.begin(), TingPaiItem.cbTingCardData.end());
		cbLeftCardCount.assign(TingPaiItem.cbLeftCardCount.begin(), TingPaiItem.cbLeftCardCount.end());
		dwHuCardFanShu.assign(TingPaiItem.dwHuCardFanShu.begin(), TingPaiItem.dwHuCardFanShu.end());
	}

	tagTingPaiItemData(const tagTingPaiItem& TingPaiItem)
	{
		cbOutCardData = TingPaiItem.cbOutCardData;
		cbTingCardCount = TingPaiItem.cbTingCardCount;

		cbTingCardData.assign(TingPaiItem.cbTingCardData, TingPaiItem.cbTingCardData + cbTingCardCount);
		cbLeftCardCount.assign(TingPaiItem.cbLeftCardCount, TingPaiItem.cbLeftCardCount + cbTingCardCount);
		dwHuCardFanShu.assign(TingPaiItem.dwHuCardFanShu, TingPaiItem.dwHuCardFanShu + cbTingCardCount);
	}

	tagTingPaiItemData& operator=(const tagTingPaiItemData& refInfo)
	{
		if (this == &refInfo)
		{
			return *this;
		}

		cbOutCardData = refInfo.cbOutCardData;
		cbTingCardCount = refInfo.cbTingCardCount;

		cbTingCardData.assign(refInfo.cbTingCardData.begin(), refInfo.cbTingCardData.end());
		cbLeftCardCount.assign(refInfo.cbLeftCardCount.begin(), refInfo.cbLeftCardCount.end());
		dwHuCardFanShu.assign(refInfo.dwHuCardFanShu.begin(), refInfo.dwHuCardFanShu.end());

		return *this;
	}

	bool StreamValue(DataStream& kData, bool bSend)
	{
		try
		{
			Stream_VALUE(cbOutCardData);
			Stream_VALUE(cbTingCardCount);

			StructVecotr(BYTE, cbTingCardData);
			StructVecotr(BYTE, cbLeftCardCount);
			StructVecotr(DWORD, dwHuCardFanShu);

			return true;
		}
		catch (...)
		{
			return false;
		}
	}
};

// 听牌结果;
struct tagTingPaiDataStream
{
	BYTE							cbTingPaiItemCount;							//可打牌数目
	DataStream						kdataStream;								//数据流;

	tagTingPaiDataStream()
	{
		cbTingPaiItemCount = 0;
		kdataStream.clear();
	}

	tagTingPaiDataStream(const tagTingPaiDataStream& TingPaiResult)
	{
		cbTingPaiItemCount = TingPaiResult.cbTingPaiItemCount;
		kdataStream.assign(TingPaiResult.kdataStream.begin(), TingPaiResult.kdataStream.end());
	}

	tagTingPaiDataStream(const tagTingPaiResult& TingPaiResult)
	{
		cbTingPaiItemCount = TingPaiResult.cbTingPaiItemCount;

		for (size_t i = 0; i < cbTingPaiItemCount; i++)
		{
			tagTingPaiItemData tmpItem(TingPaiResult.TingPaiItemArrary[i]);
			tmpItem.StreamValue(kdataStream, true);
		}
	}

	tagTingPaiDataStream& operator=(const tagTingPaiDataStream& refInfo)
	{
		if (this == &refInfo)
		{
			return *this;
		}

		cbTingPaiItemCount = refInfo.cbTingPaiItemCount;
		kdataStream.assign(refInfo.kdataStream.begin(), refInfo.kdataStream.end());

		return *this;
	}

	bool StreamValue(DataStream& kData, bool bSend)
	{
		try
		{
			Stream_VALUE(cbTingPaiItemCount);
			Stream_VALUE(kdataStream);

			return true;
		}
		catch (...)
		{
			return false;
		}
	}
};

inline tagTingPaiItem& tagTingPaiItem::operator = (const tagTingPaiItemData& TingPaiItemData)
{
	cbOutCardData = TingPaiItemData.cbOutCardData;
	cbTingCardCount = TingPaiItemData.cbTingCardCount;


	if (cbTingCardCount > 0)
	{
		memcpy(cbTingCardData, &TingPaiItemData.cbTingCardData[0], cbTingCardCount*sizeof(BYTE));
		memcpy(cbLeftCardCount, &TingPaiItemData.cbLeftCardCount[0], cbTingCardCount*sizeof(BYTE));
		memcpy(dwHuCardFanShu, &TingPaiItemData.dwHuCardFanShu[0], cbTingCardCount*sizeof(DWORD));
	}
	return *this;
}

inline tagTingPaiResult::tagTingPaiResult(tagTingPaiDataStream& refTingPaiDataStream)
{
	cbTingPaiItemCount = refTingPaiDataStream.cbTingPaiItemCount;
	for (BYTE i = 0; i < cbTingPaiItemCount; ++i)
	{
		tagTingPaiItemData	tempTingPaiItemData;
		tempTingPaiItemData.StreamValue(refTingPaiDataStream.kdataStream, false);
		TingPaiItemArrary[i] = tempTingPaiItemData;
	}
}

//胡牌结果
struct tagHuPaiResult
{
	DWORD							dwHuPaiKind;								//胡牌类型
	DWORD							dwHuPaiFanShu;								//番数数目
};
//抽奖结构数据
struct tagLotteryPrizeResult
{
	BYTE							cbPrizeType;								//奖品类型
	LONG							lPrizeCount;								//奖品数量
};

//////////////////////////////////////////////////////////////////////////////////////////////////
//游戏状态命令
#define GS_MJ_FREE					GAME_STATUS_FREE							//空闲状态
#define GS_MJ_DICE					(GAME_STATUS_PLAY+1)						//掷骰子状态
#define GS_MJ_PLAY					(GAME_STATUS_PLAY+2)						//游戏状态

//空闲状态
struct CMD_S_StatusFree
{
	bool							bCurBiXiaHu;								//当局比下胡
	LONG							lBaseScore;									//房间底注
	LONG                            lCellScore;									//房间最小底注限制
	LONG                            lMaxCellScore;								//房间的最大底注限制
	BYTE                            cbSBSStatus;								//0 未设置 1让wPlayerID设置 2让wPlayerID同意    

};
//掷骰子状态
struct CMD_S_StatusDice
{
	LONG							lBaseScore;									//房间底注
	bool							bCurBiXiaHu;								//当局比下胡
	WORD							wBankerUser;								//庄家用户
	WORD							wEastUser;									//东家用户
};
//游戏状态
struct CMD_S_StatusPlay
{
	//游戏变量
	LONG							lBaseScore;									//房间底注
	bool							bCurBiXiaHu;								//当局比下胡
	WORD							wSiceCount;									//骰子点数
	WORD							wBankerUser;								//庄家用户
	WORD							wCurrentUser;								//当前用户
	WORD							wEastUser;									//东家用户

	//状态变量
	BYTE							cbLeftCardCount;							//剩余数目
	bool							bUserTrust[GAME_PLAYER];					//是否托管
	bool							bUserTing[GAME_PLAYER];						//是否听牌
	LONG							lTiaoZhanScore;								//挑战得分

	//出牌信息
	WORD							wOutCardUser;								//出牌用户
	BYTE							cbOutCardData;								//出牌牌
	BYTE							cbDiscardCount[GAME_PLAYER];				//丢弃数目
	BYTE							cbDiscardCard[GAME_PLAYER][72];				//丢弃记录

	//牌数据
	BYTE							cbCardCount[GAME_PLAYER];					//牌数目
	BYTE							cbCardData[MAX_COUNT];						//牌列表
	BYTE							cbSendCardData;								//发送牌
	BYTE							cbHuaPaiCount[GAME_PLAYER];					//花牌数目
	BYTE							cbHuaPaiData[GAME_PLAYER][MAX_HUA_COUNT];	//花牌数据

	//组合牌
	BYTE							cbWeaveCount[GAME_PLAYER];					//组合数目
	tagWeaveItem					WeaveItemArray[GAME_PLAYER][MAX_WEAVE];		//组合牌

	LONG							lOutCardScore[GAME_PLAYER][2];				//玩家出牌得分 0:奖分 1:罚分
	LONG							lGangScore[GAME_PLAYER];					//玩家杠分
};

//////////////////////////////////////////////////////////////////////////////////////////////////
//服务器命令结构
#define SUB_S_SET_BASESCORE			100											//设置底注
#define SUB_S_KICK_OUT				101											//踢出玩家
#define SUB_S_SHOOT_DICE			102											//掷骰子
#define SUB_S_GAME_START			103											//游戏开始
#define SUB_S_SEND_BUHUA			104											//玩家补花
#define SUB_S_SEND_CARD				105											//发送牌
#define SUB_S_OUT_CARD				106											//出牌命令
#define SUB_S_OPERATE_NOTIFY		107											//操作提示
#define SUB_S_OPERATE_RESULT		108											//操作命令
#define SUB_S_PAY_MONEY				109											//游戏过程中的结算
#define SUB_S_GAME_END				110											//游戏结束
#define SUB_S_GAME_LOTTERY			111											//游戏抽奖
#define SUB_S_USER_TRUST			112											//用户托管
#define SUB_S_SPECIAL_LIGHT			113											//特殊点亮;
#define SUB_S_MASTER_LEFTCARD		114											//剩余牌堆

//设置底注
struct CMD_S_SetBaseScore
{
	WORD							wChairID;									//玩家设置
	LONG                            lBaseScore;									//底注
	LONG                            lCellScore;									//房间最小底注限制
	LONG                            lMaxCellScore;								//房间的最大底注限制
	BYTE                            cbSBSStatus;								//0 未设置 1让wPlayerID设置 2让wPlayerID同意       
};
//踢出用户
struct CMD_S_KickOut
{
	WORD                            wChairID;									//被踢玩家ID
	BYTE                            cbReasion;									//被踢原因
};

//掷骰子
struct CMD_S_ShootDice
{
	bool							bCurBiXiaHu;								//当局比下胡
	WORD							wBankerUser;								//庄家用户
	WORD							wEastUser;									//东家用户
};
//游戏开始
struct CMD_S_GameStart
{
	WORD							wSiceCount;									//骰子点数
	BYTE							cbCardData[MAX_COUNT];						//牌列表
};
//发送牌
struct CMD_S_SendCard
{
	WORD							wSendCardUser;								//发牌用户
	BYTE							cbCardData;									//牌数据
	BYTE							cbDirection;								//从前(后)摸
};
//玩家补花
struct CMD_S_SendBuHua
{
	WORD							wBuHuaUser;									//补花用户
	bool							bFirstBuHua;								//首轮补花
	BYTE							cbBuHuaCount;								//补花数目
	BYTE							cbBuHuaData[MAX_COUNT];						//补花数据
	BYTE							cbReplaceCardData[MAX_COUNT];				//替换的牌
};
//出牌命令
struct CMD_S_OutCard
{
	WORD							wOutCardUser;								//出牌用户
	BYTE							cbOutCardData;								//出牌牌
};
//操作提示
struct CMD_S_OperateNotify_Send
{
	WORD							wOperateUser;								//操作玩家
	WORD							wCurrentUser;								//当前用户
	DWORD							dwActionMask;								//动作掩码
	BYTE							cbActionCard;								//动作牌
	tagGangPaiResult                GangPaiResult;								//杠牌操作

	void StreamValue(DataStream& kData, bool bSend)
	{
		if (bSend)
		{
			kData.pushValue(this, sizeof(CMD_S_OperateNotify_Send));
		}
		else
		{
			kData.popValue(this, sizeof(CMD_S_OperateNotify_Send));
		}
	}
};

//操作命令
struct CMD_S_OperateResult
{
	WORD							wOperateUser;								//操作用户
	WORD							wProvideUser;								//供应用户
	DWORD							dwOperateCode;								//操作代码
	BYTE							cbOperateCard;								//操作牌
	LONG							lTiaoZhanScore;								//挑战得分
};
//游戏小结
struct CMD_S_PayMoney
{
	BYTE							cbPayReason;								//支付缘由
	WORD							wPayToUser;									//得钱玩家
	LONG							lPayToNumber;								//得钱数目
	WORD							wPayFromUser[GAME_PLAYER-1];				//付钱玩家
	LONG							lPayFromNumber[GAME_PLAYER-1];				//支付数额
};
//游戏结束
struct CMD_S_GameEnd
{
	//结束信息
	BYTE							cbGameEndType;								//游戏结束类型 0:正常,1:荒庄,2:逃跑,3:解散
	WORD							wBankerUser;								//庄家用户
	WORD							wEscapeUser;								//逃跑玩家
	WORD							wProvideUser;								//供应用户
	BYTE							cbProvideCard;								//供应牌
	bool							bHaveBiXiaHu;								//是否比下胡
	LONG							lCellScore;									//房间底注

	//积分信息
	LONG							lGameScore[GAME_PLAYER];					//游戏积分
	LONG							lHuScore[GAME_PLAYER];						//胡牌分;
	LONG							lGangScore[GAME_PLAYER];					//杠分;	
	LONG							lOutScore[GAME_PLAYER];						//出牌分;
	LONG							lBaoZiScore[GAME_PLAYER];					//出牌分;

	LONG							lTiaoZhanScore[GAME_PLAYER];				//挑战得分
	LONG							lTingPaiScore[GAME_PLAYER];					//报听分数
	BYTE							cbHuaPaiCount[GAME_PLAYER][2];				//花牌数量
	BYTE							cbHuCardData;								//胡的牌

	//牌信息
	BYTE							cbCardCount[GAME_PLAYER];					//牌数目
	BYTE							cbCardData[GAME_PLAYER][MAX_COUNT];			//牌数据

	//组合牌
	BYTE							cbWeaveItemCount[GAME_PLAYER];				//组合数目
	tagWeaveItem					WeaveItemArray[GAME_PLAYER][MAX_WEAVE];		//组合牌
	tagHuPaiResult					stHuPaiResult[GAME_PLAYER];					//胡牌结果

	void StreamValue(DataStream& kData, bool bSend)
	{
		if (bSend)
		{
			kData.pushValue(this, sizeof(CMD_S_GameEnd));
		}
		else
		{
			kData.popValue(this, sizeof(CMD_S_GameEnd));
		}
	}
};

//用户托管
struct CMD_S_UserTrust
{
	bool							bTrust;										//是否托管
	WORD							wChairID;									//托管用户
};

//////////////////////////////////////////////////////////////////////////////////////////////////
//客户端命令结构
#define SUB_C_SET_BASESCORE			1											//用户设置底注
#define	SUB_C_SHOOT_DICE			2											//掷骰子
#define SUB_C_DISPATCH_OVER			3											//发牌完成
#define SUB_C_SENDCARD_OVER			4											//摸牌动画完成
#define SUB_C_BUHUA_OVER			5											//补花动画完成
#define SUB_C_OUT_CARD				6											//出牌命令
#define SUB_C_OPERATE_CARD			7											//操作牌
#define SUB_C_GAME_LOTTERY			8											//游戏抽奖
#define SUB_C_USER_TRUST			9											//用户托管
#define SUB_C_MASTER_CHEAKCARD		10											//选择的牌
#define SUB_C_MASTER_LEFTCARD		11											//剩余牌堆

//用户设置底注
struct CMD_C_SetBaseScore
{
	LONG                            lBaseScore;									//底注
	BYTE							cbSBSAction;								//1设置 0确认
};
//出牌命令
struct CMD_C_OutCard
{
	BYTE							cbCardData;									//牌数据
};
//操作命令
struct CMD_C_OperateCard
{
	DWORD							dwOperateCode;								//操作代码
	BYTE							cbOperateCard;								//操作牌
};
//游戏抽奖
struct CMD_C_GameLottery
{
	BYTE							cbLotteryIndex;								//抽奖时，玩家选择卡片ID
};
//用户托管
struct CMD_C_UserTrust
{
	bool							bTrust;										//是否托管	
};

struct CMD_C_MaterCheckCard
{
	BYTE							cbCheakCard;						//操作代码
};

struct MasterLeftCard
{
	BYTE      kMasterCheakCard;										// 当前选择;
	BYTE      kMasterLeftIndex[MAX_INDEX];							// 排堆;
};

//////////////////////////////////////////////////////////////////////////
// 私人场记录类型(最大不能超过MAX_PRIVATE_ACTION)
enum PRIVATE_Action
{
	PA_ZIMO = 0,		//自摸次数
	PA_ZHUOPAO,			//接炮次数
	PA_DIANPAO,			//点炮次数
	PA_ANGANG,			//暗杠次数
	PA_MINGGANG,		//明杠次数
	PA_WIN,				//胜利
	PA_LOSE,			//失败
	PA_ALLSCORE,		//总成绩
};

// 游戏解散结构
struct CMD_S_Private_End_Info
{
	WORD		wMainChairID;				// 房主
	BYTE		cbZiMoCout[GAME_PLAYER];	// 自摸次数
	BYTE		cbZhuoPaoCout[GAME_PLAYER];	// 接炮次数
	BYTE		cbDianPaoCout[GAME_PLAYER];	// 点炮次数
	BYTE		cbAnGang[GAME_PLAYER];		// 暗杠次数
	BYTE		cbMingGang[GAME_PLAYER];	// 明杠次数
	BYTE		cbWinCout[GAME_PLAYER];		// 胜利
	BYTE		cbLoseCout[GAME_PLAYER];	// 失败
	SCORE		lBaoZiScore[GAME_PLAYER];	// 外包积分;
	SCORE	    lAllScore[GAME_PLAYER];		// 总成绩
	SCORE		lMaxScore[GAME_PLAYER];		// 最高得分

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

// 游戏规则(1<<0 上次房卡AA使用，子游戏请勿使用);
enum eNJMJRuleEnum
{
	eNJMJRuleEnum_AA = 1,						// 房卡AA;
	eNJMJRuleEnum_JYZ = (1 << 1),				// 0 进园子 1 敞开头;
	eNJMJRuleEnum_BAO = (1 << 2),				// 0 有外包 1 无外包;
};

#pragma pack(pop)

//////////////////////////////////////////////////////////////////////////////////////////////////
extern void MyLog( TCHAR *szLog, ... );
#endif