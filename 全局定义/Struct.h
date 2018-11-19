#ifndef STRUCT_HEAD_FILE
#define STRUCT_HEAD_FILE


#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//游戏列表

//游戏类型
struct tagGameType
{
	WORD							wJoinID;							//挂接索引
	WORD							wSortID;							//排序索引
	WORD							wTypeID;							//类型索引
	TCHAR							szTypeName[LEN_TYPE];				//种类名字
};

//游戏种类
struct tagGameKind
{
	WORD							wTypeID;							//类型索引
	WORD							wJoinID;							//挂接索引
	WORD							wSortID;							//排序索引
	WORD							wKindID;							//类型索引
	WORD							wGameID;							//模块索引
	DWORD							dwOnLineCount;						//在线人数
	DWORD							dwAndroidCount;						//在线人数
	DWORD							dwFullCount;						//满员人数
	TCHAR							szKindName[LEN_KIND];				//游戏名字
	TCHAR							szProcessName[LEN_PROCESS];			//进程名字
};

//游戏节点
struct tagGameNode
{
	WORD							wKindID;							//名称索引
	WORD							wJoinID;							//挂接索引
	WORD							wSortID;							//排序索引
	WORD							wNodeID;							//节点索引
	TCHAR							szNodeName[LEN_NODE];				//节点名称
};

//定制类型
struct tagGamePage
{
	WORD							wPageID;							//页面索引
	WORD							wKindID;							//名称索引
	WORD							wNodeID;							//节点索引
	WORD							wSortID;							//排序索引
	WORD							wOperateType;						//控制类型
	TCHAR							szDisplayName[LEN_PAGE];			//显示名称
};

//游戏房间
struct tagGameServer
{
	WORD							wKindID;							//名称索引
	WORD							wNodeID;							//节点索引
	WORD							wSortID;							//排序索引
	WORD							wServerID;							//房间索引
	WORD                            wServerKind;                        //房间类型
	WORD							wServerType;						//房间类型
	WORD							wServerPort;						//房间端口
	SCORE							lCellScore;							//单元积分
	SCORE							lEnterScore;						//进入积分
	DWORD							dwServerRule;						//房间规则
	DWORD							dwOnLineCount;						//在线人数
	DWORD							dwAndroidCount;						//机器人数
	DWORD							dwFullCount;						//满员人数
	TCHAR							szServerAddr[32];					//房间名称
	TCHAR							szServerName[LEN_SERVER];			//房间名称
};

//比赛报名
struct tagSignupMatchInfo
{
	WORD							wServerID;							//房间标识
	DWORD							dwMatchID;							//比赛标识
	DWORD							dwMatchNO;							//比赛场次
};

//比赛信息
struct tagGameMatch
{
	//基本信息
	WORD							wServerID;							//房间标识
	DWORD							dwMatchID;							//比赛标识
	DWORD							dwMatchNO;							//比赛场次	
	BYTE							cbMatchType;						//比赛类型
	TCHAR							szMatchName[32];					//比赛名称

	//比赛信息
	BYTE							cbMemberOrder;						//会员等级
	BYTE							cbMatchFeeType;						//扣费类型
	SCORE							lMatchFee;							//比赛费用	

	//比赛信息
	WORD							wStartUserCount;					//开赛人数
	WORD							wMatchPlayCount;					//比赛局数

	//比赛奖励
	WORD							wRewardCount;						//奖励人数

	//比赛时间
	SYSTEMTIME						MatchStartTime;						//开始时间
	SYSTEMTIME						MatchEndTime;						//结束时间	
};

//视频配置
struct tagAVServerOption
{
	WORD							wAVServerPort;						//视频端口
	DWORD							dwAVServerAddr;						//视频地址
};

//在线信息
struct tagOnLineInfoKind
{
	WORD							wKindID;							//类型标识
	DWORD							dwOnLineCount;						//在线人数
};

//在线统计
struct tagOnLineInfoKindEx
{
	WORD							wKindID;							//类型标识
	DWORD							dwOnLineCount;						//在线人数
	DWORD							dwAndroidCount;						//机器人数
};

//在线信息
struct tagOnLineInfoServer
{
	WORD							wServerID;							//房间标识
	DWORD							dwOnLineCount;						//在线人数
};

//////////////////////////////////////////////////////////////////////////////////
//机器参数
struct tagAndroidParameter
{
	//属性变量
	DWORD							dwBatchID;							//批次标识
	DWORD							dwServiceMode;						//服务模式
	DWORD							dwAndroidCount;						//机器数目
	DWORD							dwEnterTime;						//进入时间
	DWORD							dwLeaveTime;						//离开时间
	SCORE							lTakeMinScore;						//携带分数
	SCORE							lTakeMaxScore;						//携带分数
	DWORD							dwEnterMinInterval;					//进入间隔
	DWORD							dwEnterMaxInterval;					//进入间隔
	DWORD							dwLeaveMinInterval;					//离开间隔
	DWORD							dwLeaveMaxInterval;					//离开间隔	
	DWORD							dwSwitchMinInnings;					//换桌局数
	DWORD							dwSwitchMaxInnings;					//换桌局数	
};

//////////////////////////////////////////////////////////////////////////////////
//用户信息

//桌子状态
struct tagTableStatus
{
	BYTE							cbTableLock;						//锁定标志
	BYTE							cbPlayStatus;						//游戏标志
	LONG							lCellScore;							//单元积分
};

//用户状态
struct tagUserStatus
{
	WORD							wTableID;							//桌子索引
	WORD							wChairID;							//椅子位置
	BYTE							cbUserStatus;						//用户状态
};

//用户属性
struct tagUserAttrib
{
	BYTE							cbCompanion;						//用户关系
};

//用户积分
struct tagUserScore
{
	//积分信息
	SCORE							lScore;								//用户带入分数
	SCORE							lHoldScore;							//身上积分

	//输赢信息
	DWORD							dwWinCount;							//胜利盘数
	DWORD							dwLostCount;						//失败盘数
	DWORD							dwDrawCount;						//和局盘数
	DWORD							dwFleeCount;						//逃跑盘数

	//全局信息
	DWORD							dwExperience;						//用户经验
};


//道具使用
//struct tagUsePropertyInfo
//{
//	WORD                            wPropertyCount;                     //道具数目
//	WORD                            dwValidNum;						    //有效数字
//	DWORD                           dwEffectTime;                       //生效时间
//};


//用户道具
struct tagUserProperty
{
	DWORD							dwDBID;								//道具数据库ID
	WORD                            wPropertyUseMark;                   //道具标示
	WORD                            wPropertyCount;                     //道具数目
	//WORD                            dwValidNum;						    //有效数字
	//SYSTEMTIME                      EffectTime;                         //生效时间
	SYSTEMTIME						OverTime;							//过期时间

	//tagUsePropertyInfo              PropertyInfo[MAX_PT_MARK];			//使用信息   
};

//道具包裹
struct tagPropertyPackage
{
	WORD                            wTrumpetCount;                     //小喇叭数
	WORD                            wTyphonCount;                      //大喇叭数
};

//时间信息
struct tagTimeInfo
{
	DWORD						dwEnterTableTimer;						//进出桌子时间
	DWORD						dwLeaveTableTimer;						//离开桌子时间
	DWORD						dwStartGameTimer;						//开始游戏时间
	DWORD						dwEndGameTimer;							//离开游戏时间
};

//用户信息
struct tagUserInfo
{
	//基本属性
	DWORD							dwUserID;							//用户 I D
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//个性签名

	//头像信息
	WORD							wFaceID;							//头像索引
	//DWORD							dwCustomID;							//自定标识
	DWORD							dwCustomFaceVer;					//自定义头像版本

	//用户资料
	BYTE							cbGender;							//用户性别
	BYTE							cbMemberOrder;						//会员等级
	BYTE							cbMasterOrder;						//管理等级

	//用户状态
	WORD							wTableID;							//桌子索引
	WORD							wLastTableID;					    //游戏桌子
	WORD							wChairID;							//椅子索引
	BYTE							cbUserStatus;						//用户状态

	//积分信息
	SCORE							lGold;								//用户金币
	SCORE							lScore;								//用户分数
	SCORE							lDiamond;							//用户钻石
	SCORE							lInsure;							//用户银行

	//游戏信息
	DWORD							dwWinCount;							//胜利盘数
	DWORD							dwLostCount;						//失败盘数
	DWORD							dwDrawCount;						//和局盘数
	DWORD							dwFleeCount;						//逃跑盘数	
	DWORD							dwExperience;						//用户经验
	DWORD							dwUserMedal;						//奖牌奖励
	LONG							lLoveLiness;						//用户魅力

	//救济金
	BYTE							cbBenefitTimes;						//已领次数
	SYSTEMTIME						LastBenefitTime;					//上次领取时间

	//时间信息
	tagTimeInfo						TimerInfo;							//时间信息	
};

//用户信息
struct tagUserInfoHead
{
	//用户属性
	DWORD							dwUserID;							//用户 I D

	//头像信息
	WORD							wFaceID;							//头像索引
	DWORD							dwCustomFaceVer;					//自定标识版本

	//用户属性
	BYTE							cbGender;							//用户性别
	BYTE							cbMemberOrder;						//会员等级

	//用户状态
	WORD							wTableID;							//桌子索引
	WORD							wChairID;							//椅子索引
	BYTE							cbUserStatus;						//用户状态

	//积分信息
	SCORE							lScore;								//用户分数
	SCORE							lHoldScore;							//身上积分
	//SCORE							lDiamond;							//用户钻石
	//SCORE							lInsure;							//用户银行

	////游戏信息
	//DWORD							dwWinCount;							//胜利盘数
	//DWORD							dwLostCount;						//失败盘数
	//DWORD							dwDrawCount;						//和局盘数
	//DWORD							dwFleeCount;						//逃跑盘数
	//DWORD							dwExperience;						//用户经验
	//LONG							lLoveLiness;						//用户魅力
};

//头像信息
struct tagCustomFaceInfo
{
	DWORD							dwDataSize;							//数据大小
	DWORD							dwCustomFace[FACE_CX*FACE_CY];		//图片信息
};

//用户信息
struct tagUserRemoteInfo
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	DWORD							dwGameID;							//游戏标识
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称

	//等级信息
	BYTE							cbGender;							//用户性别
	BYTE							cbMemberOrder;						//会员等级
	BYTE							cbMasterOrder;						//管理等级

	//位置信息
	WORD							wKindID;							//类型标识
	WORD							wServerID;							//房间标识
	TCHAR							szGameServer[LEN_SERVER];			//房间位置
};

//////////////////////////////////////////////////////////////////////////////////
//任务参数
struct tagTaskParameter
{
	//基本信息
	WORD							wTaskID;							//任务标识
	WORD							wTaskType;							//任务类型
	WORD							wTaskObject;						//任务目标
	BYTE							cbPlayerType;						//玩家类型
	WORD							wKindID;							//类型标识
	DWORD							dwTimeLimit;						//时间限制

	//奖励信息
	SCORE							lStandardAwardGold;					//奖励金币
	SCORE							lStandardAwardMedal;				//奖励奖牌
	SCORE							lMemberAwardGold;					//奖励金币
	SCORE							lMemberAwardMedal;					//奖励奖牌

	//描述信息
	//TCHAR							szTaskName[LEN_TASK_NAME];			//任务名称
	//TCHAR							szTaskDescribe[320];				//任务描述
};

//任务状态
struct tagTaskStatus
{
	WORD							wTaskID;							//任务标识
	WORD							wTaskProgress;						//任务进度
	BYTE							cbTaskStatus;						//任务状态
};

//低保参数
struct tagBaseEnsureParameter
{
	SCORE							lScoreCondition;					//游戏币条件
	SCORE							lScoreAmount;						//游戏币数量
	BYTE							cbTakeTimes;						//领取次数	
};

//推广信息
struct tagUserSpreadInfo
{
	DWORD							dwSpreadCount;						//推广人数
	SCORE							lSpreadReward;						//推广奖励
};

//等级配置
struct tagGrowLevelConfig
{
	WORD							wLevelID;							//等级 I D
	DWORD							dwExperience;						//相应经验
};

//等级参数
struct tagGrowLevelParameter
{
	WORD							wCurrLevelID;						//当前等级
	DWORD							dwExperience;						//当前经验
	DWORD							dwUpgradeExperience;				//下级经验
	SCORE							lUpgradeRewardGold;					//升级奖励
	SCORE							lUpgradeRewardIngot;				//升级奖励
};

//会员参数
struct tagMemberParameter
{
	BYTE							cbMemberOrder;						//会员标识
	TCHAR							szMemberName[16];					//会员名称
	SCORE							lMemberPrice;						//会员价格
	SCORE							lPresentScore;						//赠送游戏币
};

//////////////////////////////////////////////////////////////////////////////////

//广场子项
struct tagGamePlaza
{
	WORD							wPlazaID;							//广场标识
	TCHAR							szServerAddr[32];					//服务地址
	TCHAR							szServerName[32];					//服务器名
};

//级别子项
struct tagLevelItem
{
	LONG							lLevelScore;						//级别积分
	TCHAR							szLevelName[16];					//级别描述
};

//会员子项
struct tagMemberItem
{
	BYTE							cbMemberOrder;						//等级标识
	TCHAR							szMemberName[16];					//等级名字
};

//管理子项
struct tagMasterItem
{
	BYTE							cbMasterOrder;						//等级标识
	TCHAR							szMasterName[16];					//等级名字
};

//列表子项
struct tagColumnItem
{
	BYTE							cbColumnWidth;						//列表宽度
	BYTE							cbDataDescribe;						//字段类型
	TCHAR							szColumnName[16];					//列表名字
};

//地址信息
struct tagAddressInfo
{
	TCHAR							szAddress[32];						//服务地址
};

//数据信息
struct tagDataBaseParameter
{
	WORD							wDataBasePort;						//数据库端口
	TCHAR							szDataBaseAddr[32];					//数据库地址
	TCHAR							szDataBaseUser[32];					//数据库用户
	TCHAR							szDataBasePass[32];					//数据库密码
	TCHAR							szDataBaseName[32];					//数据库名字
};

//缓存服务器
struct tagCacheServerParameter
{
	WORD							wServerPort;						//缓存服务器端口
	TCHAR							szServerAddr[64];					//缓存服务器地址
	TCHAR							szServerPass[33];					//缓存服务器密码
};

//房间配置
struct tagServerOptionInfo
{
	//挂接属性
	WORD							wKindID;							//挂接类型
	WORD							wNodeID;							//挂接节点
	WORD							wSortID;							//排列标识

	//税收配置
	WORD							wRevenueRatio;						//税收比例
	SCORE							lServiceScore;						//服务费用

	//房间配置
	SCORE							lRestrictScore;						//限制积分
	SCORE							lMinTableScore;						//最低积分
	SCORE							lMinEnterScore;						//最低积分
	SCORE							lMaxEnterScore;						//最高积分

	//会员限制
	BYTE							cbMinEnterMember;					//最低会员
	BYTE							cbMaxEnterMember;					//最高会员

	//房间属性
	DWORD							dwServerRule;						//房间规则
	TCHAR							szServerName[LEN_SERVER];			//房间名称
};

//救济金配置
struct tagBenefitInfo
{
	SCORE							lGoldLimit;							//获取资格			
	BYTE							cbReceiveTimes;						//每日可完成次数
	WORD							wTimeIntervals;						//两次发放时间间隔
	//WORD							RewardPropID;						//奖励道具ID
	TCHAR							szRewardProp[256];					//奖励道具
	TCHAR							szPromptMsg[128];					//提示信息
	TCHAR							szEmailTitle[128];					//邮件标题
	TCHAR							szEmailMsg[320];					//邮件内容
};

//排行榜奖励邮件配置
struct tagRankingRewardEmailConfig
{
	TCHAR							szEmailTitle[128];					//邮件标题
	TCHAR							szEmailMsg[320];					//邮件内容
};

//排行榜奖励配置
struct tagRankingRewardConfig
{
	WORD							wRankingID;							//排名ID
	char							szPropList[128];					//道具列表
	char							szPropListDesc[128];				//道具描述
};

//////////////////////////////////////////////////////////////////////////////////

//更新信息
struct tagUpdateInfo
{
	WORD							wStructSize;						//结构大小
	WORD							wUpdateCount;						//更新数目
	char							szDownLoadAddress[MAX_PATH];		//下载地址
};

//更新子项
struct tagUpdateItem
{
	WORD							wStructSize;						//结构大小
	char							szFileCheckSum[33];					//文件校验和
	char							szFileName[128];					//文件名称
};

//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif