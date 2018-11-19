#ifndef DATA_BASE_PACKET_HEAD_FILE
#define DATA_BASE_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//请求数据包

//用户事件
#define	DBR_GR_LOGON_USERID			100									//I D登录
#define	DBR_GR_LOGON_RELINE			101									//用户重连
#define	DBR_GR_LOGON_ACCOUNTS		102									//帐号登录
#define DBR_GR_LOGON_MATCH			103									//比赛登录
#define	DBR_GR_LOGON_MOBILE			104									//手机登录

//系统事件
#define DBR_GR_WRITE_GAME_SCORE		200									//游戏积分
#define DBR_GR_LEAVE_GAME_SERVER	201									//离开房间
#define DBR_GR_GAME_SCORE_RECORD	202									//积分记录
#define DBR_GR_MANAGE_USER_RIGHT	203									//权限管理
#define DBR_GR_LOAD_SYSTEM_MESSAGE	204									//系统消息
#define DBR_GR_LOAD_SENSITIVE_WORDS	205									//敏感词	
#define DBR_GR_SEND_EMAIL			206									//发送邮件

#define DBR_GR_UNLOCK_ANDROID_USER	207									//解锁机器人
#define DBR_GR_LOAD_CARD_LIBRARY	208									//加载牌库
#define DBR_GR_PRIVATE_GAME_RECORD	209									//私人场游戏记录
#define DBR_GR_SINGLE_PRIVATE_RECORD	210								//单场游戏记录;
#define DBR_GR_PRIVATE_GAME_END		211									//私人场结束;

//配置事件
#define DBR_GR_LOAD_PARAMETER		300									//加载参数
#define DBR_GR_LOAD_GAME_COLUMN		301									//加载列表
#define DBR_GR_LOAD_ANDROID_PARAMETER 302								//加载参数
#define DBR_GR_LOAD_ANDROID_USER	303									//加载机器
#define DBR_GR_LOAD_GAME_PROPERTY	304									//加载道具
#define DBR_GR_LOAD_USER_PROPERTY	305									//加载用户道具

//银行事件
#define DBR_GR_USER_ENABLE_INSURE	400									//开通银行
#define DBR_GR_USER_SAVE_SCORE		401									//存入游戏币
#define DBR_GR_USER_TAKE_SCORE		402									//提取游戏币
#define DBR_GR_USER_TRANSFER_SCORE	403									//转帐游戏币
#define DBR_GR_QUERY_INSURE_INFO	404									//查询银行
#define DBR_GR_QUERY_TRANSFER_USER_INFO	    405							//查询用户

//游戏事件
#define DBR_GR_PROPERTY_REQUEST		500									//赠送礼物
#define DBR_GR_GAME_FRAME_REQUEST	502									//游戏请求

//比赛事件
#define DBR_GR_MATCH_SIGNUP			600									//比赛费用
#define DBR_GR_MATCH_UNSIGNUP		601									//退出比赛
#define DBR_GR_MATCH_START			602									//比赛开始
#define DBR_GR_MATCH_OVER			603									//比赛结束	
#define DBR_GR_MATCH_REWARD			604									//比赛奖励
#define DBR_GR_MATCH_ENTER_FOR		605									//报名进入
#define DBR_GR_MATCH_ELIMINATE      606									//比赛淘汰

//私人场事件
#define DBR_GR_PRIVATE_INFO			620									//私人场信息
#define DBR_GR_CREATE_PRIVAT		621									//创建私人场
#define DBR_GR_CREATE_PRIVAT_COST	622									//返还私人场

//任务事件
#define DBR_GR_TASK_TAKE			700									//领取任务
#define DBR_GR_TASK_REWARD			701									//领取奖励
#define DBR_GR_TASK_LOAD_LIST		702									//加载任务
#define DBR_GR_TASK_QUERY_INFO		703									//查询任务
#define DBR_GR_TASK_PROGRESS		704									//任务跟进

//兑换事件
#define DBR_GR_LOAD_MEMBER_PARAMETER 800								//会员参数
#define DBR_GR_PURCHASE_MEMBER		801									//购买会员
#define DBR_GR_EXCHANGE_SCORE		802									//兑换游戏币
#define DBR_GR_REFRESH_ONLINE_USER	900									//刷新在线人数

//////////////////////////////////////////////////////////////////////////////////
//输出信息

//逻辑事件
#define DBO_GR_LOGON_SUCCESS		100									//登录成功
#define DBO_GR_LOGON_FAILURE		101									//登录失败

//配置事件
#define DBO_GR_GAME_PARAMETER		200									//配置信息
#define DBO_GR_GAME_COLUMN_INFO		201									//列表信息
#define DBO_GR_GAME_ANDROID_PARAMETER 202								//参数信息
#define DBO_GR_GAME_ANDROID_INFO	203									//机器信息
#define DBO_GR_GAME_UNLOCK_ANDROID	204									//解锁机器
#define DBO_GR_GAME_PROPERTY_INFO	205									//道具信息
#define DBO_GR_GAME_MEMBER_PAREMETER 206								//会员参数
#define DBO_GR_USER_PROPERTY_INFO	207									//用户道具
//银行命令
#define DBO_GR_USER_INSURE_INFO		300									//银行资料
#define DBO_GR_USER_INSURE_SUCCESS	301									//银行成功
#define DBO_GR_USER_INSURE_FAILURE	302									//银行失败
#define DBO_GR_USER_INSURE_USER_INFO   303								//用户资料
#define DBO_GR_USER_INSURE_ENABLE_RESULT 304							//开通结果

//游戏事件
#define DBO_GR_PROPERTY_SUCCESS		400									//道具成功
#define DBO_GR_PROPERTY_FAILURE		401									//道具失败
#define DBO_GR_GAME_FRAME_RESULT	402									//游戏结果

//比赛事件
#define DBO_GR_MATCH_EVENT_START	500									//比赛标识
#define DBO_GR_MATCH_SIGNUP_RESULT	500									//报名结果
#define DBO_GR_MATCH_UNSIGNUP_RESULT 501								//退赛结果
#define DBO_GR_MATCH_RANK_LIST		502									//比赛排行
#define DBO_GR_MATCH_REWARD_RESULT  503									//奖励结果

//私人场事件
#define DBO_GR_PRIVATE_EVENT_START	550									//私人场开始标识
#define DBO_GR_PRIVATE_INFO			551									//私人场信息	
#define DBO_GR_CREATE_PRIVATE		552									//私人场信息
#define DBO_GR_SINGLE_RECORD		553									//单场游戏记录;
#define DBO_GR_PRIVATE_EVENT_END	559									//私人场结束标识

#define DBO_GR_MATCH_EVENT_END		599									//比赛标识

//系统事件
#define DBO_GR_SYSTEM_MESSAGE_RESULT	600								//消息结果
#define DBO_GR_SYSTEM_MESSAGE_FINISH	601								//消息完成	
#define DBO_GR_SENSITIVE_WORDS		    602								//敏感词
#define DBR_GR_SEND_EMAIL_SUCCESS		603								//发送邮件

//任务事件
#define DBO_GR_TASK_LIST			700									//任务列表
#define DBO_GR_TASK_LIST_END		701									//任务结束
#define DBO_GR_TASK_INFO			702									//任务信息
#define DBO_GR_TASK_RESULT			703									//任务结果

//兑换事件
#define DBO_GR_MEMBER_PARAMETER     800									//会员参数
#define DBO_GR_PURCHASE_RESULT		801									//购买结果
#define DBO_GR_EXCHANGE_RESULT		802									//兑换结果

//缓存事件
#define DBO_GR_REREG_SERVER_IN_REDIS	900								//重新在缓存中注册服务器
//////////////////////////////////////////////////////////////////////////////////

//ID 登录
struct DBR_GR_LogonUserID
{		
	//登录信息
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_MD5];				//登录密码

	//附加信息
	DWORD							dwMatchID;							//比赛标识
	DWORD							dwMatchNO;							//比赛编号
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识
};

//mobile 登录
struct DBR_GR_LogonMobile
{
	//登录信息
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_MD5];				//登录密码
	BYTE                            cbDeviceType;                       //设备类型
	WORD                            wBehaviorFlags;                     //行为标识
	WORD                            wPageTableCount;                    //分页桌数

	//附加信息
	DWORD							dwMatchID;							//比赛标识
	DWORD							dwMatchNO;							//比赛编号
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识
};

//用户重连
struct DBR_GR_RELINE_USERID
{
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_MD5];				//登录密码
};

//游戏积分
struct DBR_GR_WriteGameScore
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	DWORD							dwClientAddr;						//连接地址

	//用户信息
	DWORD							dwDBQuestID;						//请求标识
	DWORD							dwInoutIndex;						//进出索引

	//附件信息
	bool							bTaskForward;						//任务跟进

	//最大牌型
	BYTE							cbMaxCardType;
	BYTE							aryCardData[MAX_CARD_DATA_COUNT];

	//成绩变量
	tagVariationInfo				VariationInfo;						//提取信息

	//比赛信息
	DWORD							dwMatchID;							//比赛标识
	DWORD							dwMatchNO;							//比赛场次
};

//离开房间
struct DBR_GR_LeaveGameServer
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	DWORD							dwInoutIndex;						//记录索引
	DWORD							dwLeaveReason;						//离开原因
	DWORD							dwOnLineTimeCount;					//在线时长

	//最大牌型
	BYTE							cbMaxCardType;
	BYTE							aryCardData[MAX_CARD_DATA_COUNT];

	//成绩变量
	tagVariationInfo				RecordInfo;							//记录信息
	tagVariationInfo				VariationInfo;						//提取信息

	//系统信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识
};

//游戏记录
struct DBR_GR_GameScoreRecord
{
	//桌子信息
	WORD							wTableID;							//桌子号码
	WORD							wUserCount;							//用户数目
	WORD							wAndroidCount;						//机器数目

	//损耗税收
	SCORE							lWasteCount;						//损耗数目
	SCORE							lRevenueCount;						//税收数目

	//统计信息
	DWORD							dwUserMemal;						//奖牌数目
	DWORD							dwPlayTimeCount;					//游戏时间

	//时间信息
	SYSTEMTIME						SystemTimeStart;					//开始时间
	SYSTEMTIME						SystemTimeConclude;					//结束时间

	//积分记录
	WORD							wRecordCount;						//记录数目
	tagGameScoreRecord				GameScoreRecord[MAX_CHAIR];			//游戏记录
};

//开通银行
struct DBR_GR_UserEnableInsure
{
	BYTE                            cbActivityGame;                     //游戏动作
	DWORD							dwUserID;							//用户 I D	
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
	TCHAR							szInsurePass[LEN_PASSWORD];			//银行密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//存入游戏币
struct DBR_GR_UserSaveScore
{
	BYTE                            cbActivityGame;                     //游戏动作
	DWORD							dwUserID;							//用户 I D
	SCORE							lSaveScore;							//存入游戏币
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//取出游戏币
struct DBR_GR_UserTakeScore
{
	BYTE                            cbActivityGame;                     //游戏动作
	DWORD							dwUserID;							//用户 I D
	SCORE							lTakeScore;							//提取游戏币
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szPassword[LEN_PASSWORD];			//银行密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//转帐游戏币
struct DBR_GR_UserTransferScore
{
	BYTE                            cbActivityGame;                     //游戏动作
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientAddr;						//连接地址	
	SCORE							lTransferScore;						//转帐游戏币
	TCHAR							szAccounts[LEN_NICKNAME];			//目标用户
	TCHAR							szPassword[LEN_PASSWORD];			//银行密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
	TCHAR							szTransRemark[LEN_TRANS_REMARK];	//转帐备注
};

//加载机器
struct DBR_GR_LoadAndroidUser
{
	DWORD							dwBatchID;							//批次标识
	DWORD							dwAndroidCount;						//机器数目
};

//查询银行
struct DBR_GR_QueryInsureInfo
{
	BYTE                            cbActivityGame;                     //游戏动作
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szPassword[LEN_PASSWORD];			//银行密码
};

//查询用户
struct DBR_GR_QueryTransferUserInfo
{
	BYTE                            cbActivityGame;                     //游戏动作
	BYTE                            cbByNickName;                       //昵称赠送
	TCHAR							szAccounts[LEN_ACCOUNTS];			//目标用户
	DWORD							dwUserID;							//用户 I D
};

//道具请求
struct DBR_GR_PropertyRequest
{
	DWORD							dwDBID;								//道具数据库ID

	//购买信息
	WORD							wItemCount;							//购买数目
	WORD							wPropertyIndex;						//道具索引

	DWORD							dwSourceUserID;						//使用对象
	DWORD							dwTargetUserID;						//被使用对象

	SCORE							lRecvLoveLiness;					//接受魅力

	////消费模式
	//BYTE                            cbRequestArea;			            //使用范围 
	//BYTE							cbConsumeScore;						//积分消费
	//SCORE							lFrozenedScore;						//冻结积分

	////用户权限
	//DWORD                           dwUserRight;						//会员权限

	//系统信息
	WORD							wTableID;							//桌子号码
	DWORD							dwInoutIndex;						//进入标识
	DWORD							dwClientAddr;						//连接地址
	SYSTEMTIME						UsedTime;							//使用时间
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};


//查询任务
struct DBR_GR_TaskQueryInfo
{
	//用户信息
	DWORD							dwUserID;							//用户标识	
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码
};

//领取任务
struct DBR_GR_TaskTake
{
	//用户信息
	WORD							wTaskID;							//任务标识
	DWORD							dwUserID;							//用户标识	
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//领取奖励
struct DBR_GR_TaskReward
{
	//用户信息
	WORD							wTaskID;							//任务标识
	DWORD							dwUserID;							//用户标识	
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//任务跟进
struct DBR_GR_TaskProgress
{
	//用户信息
	DWORD							dwUserID;							//用户标识

	//任务信息
	DWORD							dwID;								//任务序列
	WORD							wTaskProgress;						//任务进度
	BYTE							cbTaskStatus;						//任务状态
};

//兑换游戏币
struct DBR_GR_ExchangeScore
{
	DWORD							dwUserID;							//用户标识
	SCORE							lExchangeIngot;						//兑换元宝

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//////////////////////////////////////////////////////////////////////////////////

//登录成功
struct DBO_GR_LogonSuccess
{
	//属性资料
	WORD							wFaceID;							//头像标识
	DWORD							dwUserID;							//用户 ID
	DWORD							dwCustomID;							//自定头像
	DWORD							dwCustomFaceVer;					//自定头像版本
	TCHAR							szNickName[LEN_NICKNAME];			//帐号昵称

	//用户资料
	BYTE							cbGender;							//用户性别
	BYTE							cbMemberOrder;						//会员等级
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//个性签名

	//积分信息
	SCORE							lGold;								//用户金币
	SCORE							lDiamond;							//用户钻石
	SCORE							lInsure;							//用户银行

	//游戏信息
	DWORD							dwWinCount;							//胜利盘数
	DWORD							dwLostCount;						//失败盘数
	DWORD							dwDrawCount;						//和局盘数
	DWORD							dwFleeCount;						//逃跑盘数
	DWORD							dwExperience;						//用户经验
	LONG							lLoveLiness;						//用户魅力

	//最大牌型
	BYTE							cbMaxCardType;
	BYTE							aryCardData[MAX_CARD_DATA_COUNT];

	//用户权限
	DWORD							dwUserRight;						//用户权限

	//索引变量
	DWORD							dwInoutIndex;						//记录索引
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识

	//任务变量
	//WORD							wTaskID;							//任务标识
	//BYTE							cbTaskStatus;						//任务状态
	//DWORD							dwTaskProgress;						//任务进度	
	//SYSTEMTIME						TaskTakeTime;						//领取时间

	//救济金
	BYTE							cbBenefitTimes;						//已领次数
	SYSTEMTIME						LastBenefitTime;					//上次领取时间

	//手机定义
	BYTE                            cbDeviceType;                       //设备类型

	//辅助信息
	TCHAR							szPassword[LEN_MD5];				//登录密码
	TCHAR							szDescribeString[128];				//错误消息

	//任务信息
	WORD							wTaskCount;							//任务数目
	tagUserTaskInfo					UserTaskInfo[TASK_MAX_COUNT];		//任务信息
};

//登录失败
struct DBO_GR_LogonFailure
{
	LONG							lResultCode;						//错误代码
	TCHAR							szDescribeString[128];				//错误消息
};

//机器信息
struct DBO_GR_GameAndroidParameter
{
	LONG							lResultCode;						//结果代码
	WORD							wParameterCount;					//参数数目
	tagAndroidParameter				AndroidParameter[MAX_BATCH];		//机器信息
};

//机器信息
struct DBO_GR_GameAndroidInfo
{
	LONG							lResultCode;						//结果代码
	DWORD							dwBatchID;							//批次标识
	WORD							wAndroidCount;						//用户数目
	tagAndroidAccountsInfo			AndroidAccountsInfo[MAX_ANDROID];	//机器帐号
};

//道具信息
struct DBO_GR_GamePropertyInfo
{
	LONG							lResultCode;						//结果代码
	BYTE							cbPropertyCount;					//道具数目
	tagPropertyInfo					PropertyInfo[MAX_PROPERTY];			//道具信息
};

//银行资料
struct DBO_GR_UserInsureInfo
{
	BYTE                            cbActivityGame;                     //游戏动作
	BYTE							cbEnjoinTransfer;					//转帐开关
	WORD							wRevenueTake;						//税收比例
	WORD							wRevenueTransfer;					//税收比例
	WORD							wRevenueTransferMember;				//税收比例
	WORD							wServerID;							//房间标识
	SCORE							lUserScore;							//用户游戏币
	SCORE							lUserInsure;						//银行游戏币
	SCORE							lTransferPrerequisite;				//转帐条件
};

//银行成功
struct DBO_GR_UserInsureSuccess
{
	BYTE                            cbActivityGame;                     //游戏动作
	DWORD							dwUserID;							//用户 I D
	SCORE							lSourceScore;						//原来游戏币
	SCORE							lSourceInsure;						//原来游戏币
	SCORE							lInsureRevenue;						//银行税收
	SCORE							lFrozenedScore;						//冻结积分
	SCORE							lVariationScore;					//游戏币变化
	SCORE							lVariationInsure;					//银行变化
	TCHAR							szDescribeString[128];				//描述消息
};

//银行失败
struct DBO_GR_UserInsureFailure
{
	BYTE                            cbActivityGame;                     //游戏动作
	LONG							lResultCode;						//操作代码
	SCORE							lFrozenedScore;						//冻结积分
	TCHAR							szDescribeString[128];				//描述消息
};

//用户资料
struct DBO_GR_UserTransferUserInfo
{
	BYTE                            cbActivityGame;                     //游戏动作
	DWORD							dwGameID;							//用户 I D
	TCHAR							szAccounts[LEN_ACCOUNTS];			//帐号昵称
};

//开通结果
struct DBO_GR_UserInsureEnableResult
{
	BYTE                            cbActivityGame;                     //游戏动作
	BYTE							cbInsureEnabled;					//使能标识
	TCHAR							szDescribeString[128];				//描述消息
};

//道具成功
struct DBO_GR_S_PropertySuccess
{
	//道具信息
	WORD							wItemCount;							//购买数目
	WORD							wPropertyIndex;						//道具索引
	DWORD							dwSourceUserID;						//购买对象
	DWORD							dwTargetUserID;						//使用对象

	//消费模式
	BYTE                            cbRequestArea;						//请求范围
	BYTE							cbConsumeScore;						//积分消费
	SCORE							lFrozenedScore;						//冻结积分

	//会员权限
	DWORD                           dwUserRight;						//会员权限

	//结果信息
	SCORE							lConsumeGold;						//消费游戏币
	LONG							lSendLoveLiness;					//赠送魅力
	LONG							lRecvLoveLiness;					//接受魅力

	//会员信息
	BYTE							cbMemberOrder;						//会员等级
};

//任务列表
struct DBO_GR_TaskListInfo
{
	WORD							wTaskCount;							//任务数目
};

//任务信息
struct DBO_GR_TaskInfo
{
	WORD							wTaskCount;							//任务数量
	tagTaskStatus					TaskStatus[TASK_MAX_COUNT];			//任务状态
};

//签到结果
struct DBO_GR_TaskResult
{
	//结果信息
	bool							bSuccessed;							//成功标识
	WORD							wCommandID;							//命令标识
	WORD							wCurrTaskID;						//当前任务

	//财富信息
	SCORE							lCurrScore;							//当前金币
	SCORE							lCurrIngot;							//当前元宝
	
	//提示信息
	TCHAR							szNotifyContent[128];				//提示内容
};

//会员参数
struct DBO_GR_MemberParameter
{
	WORD							wMemberCount;						//会员数目
	tagMemberParameter				MemberParameter[64];				//会员参数
};

//购买结果
struct DBO_GR_PurchaseResult
{
	bool							bSuccessed;							//成功标识
	BYTE							cbMemberOrder;						//会员系列
	DWORD							dwUserRight;						//用户权限
	SCORE							lCurrScore;							//当前游戏币
	TCHAR							szNotifyContent[128];				//提示内容
};
 
//兑换结果
struct DBO_GR_ExchangeResult
{
	bool							bSuccessed;							//成功标识
	SCORE							lCurrScore;							//当前游戏币
	SCORE							lCurrIngot;							//当前元宝
	TCHAR							szNotifyContent[128];				//提示内容
};

//道具失败
struct DBO_GR_PropertyFailure
{
	BYTE                            cbRequestArea;						//请求范围
	LONG							lResultCode;						//操作代码
	SCORE							lFrozenedScore;						//冻结积分
	TCHAR							szDescribeString[128];				//描述消息
};

////比赛名次
//struct DBO_GR_MatchRank
//{
//	BYTE							cbRank;								//比赛名次
//	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称
//	LONG							lMatchScore;						//用户得分
//};

//报名结果
struct DBO_GR_MatchSingupResult
{
	bool							bResultCode;						//结果代码
	DWORD							dwUserID;							//用户标识
	SCORE							lCurrGold;							//当前金币
	SCORE							lCurrIngot;							//当前元宝
	TCHAR							szDescribeString[128];				//描述消息
};

//比赛排行
struct DBO_GR_MatchRankList
{
	WORD							wUserCount;							//用户数目
	DWORD							dwMatchID;							//比赛标识
	DWORD							dwMatchNO;							//比赛场次
	tagMatchRankInfo				MatchRankInfo[128];					//奖励信息
};

//权限管理
struct DBR_GR_ManageUserRight
{
	DWORD							dwUserID;							//目标用户
	DWORD							dwAddRight;							//添加权限
	DWORD							dwRemoveRight;						//删除权限
	bool							bGameRight;							//游戏权限
};

//权限管理
struct DBR_GR_ManageMatchRight
{	
	DWORD							dwUserID;							//目标用户
	DWORD							dwAddRight;							//添加权限
	DWORD							dwRemoveRight;						//删除权限	
	DWORD							dwMatchID;							//比赛标识
	DWORD							dwMatchNO;							//比赛场次
};

//比赛报名
struct DBR_GR_MatchSignup
{
	//报名费用
	DWORD							dwUserID;							//用户ID
	SCORE							lMatchFee;							//报名费用

	//系统信息
	DWORD							dwInoutIndex;						//进入标识
	DWORD							dwClientAddr;						//连接地址
	DWORD							dwMatchID;							//比赛ID
	DWORD							dwMatchNO;							//比赛场次
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//比赛开始
struct DBR_GR_MatchStart
{
	WORD							wServerID;							//房间标识
	BYTE							cbMatchType;						//比赛类型
	DWORD							dwMatchID;							//比赛标识
	DWORD							dwMatchNO;							//比赛场次	
};

//比赛结束
struct DBR_GR_MatchOver
{
	WORD							wServerID;							//房间标识
	BYTE							cbMatchType;						//比赛类型
	DWORD							dwMatchID;							//比赛标识
	DWORD							dwMatchNO;							//比赛场次	
	SYSTEMTIME						MatchStartTime;						//开始时间
	SYSTEMTIME						MatchEndTime;						//结束时间
};

//比赛淘汰
struct DBR_GR_MatchEliminate
{
	DWORD							dwUserID;							//用户标识
	WORD							wServerID;							//房间标识
	BYTE							cbMatchType;						//比赛类型
	DWORD							dwMatchID;							//比赛标识
	DWORD							dwMatchNO;							//比赛场次	
};


//比赛奖励
struct DBR_GR_MatchReward
{
	DWORD							dwUserID;							//用户 I D
	SCORE							lRewardGold;						//奖励金币
	DWORD							dwRewardIngot;						//奖励元宝
    DWORD							dwRewardExperience;					//用户经验	
	DWORD							dwClientAddr;						//连接地址
};

//奖励结果
struct DBR_GR_MatchRewardResult
{
	bool							bResultCode;						//结果代码
	DWORD							dwUserID;							//用户标识
	SCORE							lCurrGold;							//当前金币
	SCORE							lCurrIngot;							//当前元宝
	DWORD							dwCurrExperience;					//当前经验
};

//系统消息
struct DBR_GR_SystemMessage
{
	DWORD                           dwMessageID;                        //消息ID
	BYTE							cbMessageType;						//消息类型
	BYTE                            cbAllRoom;                          //全体房间
	DWORD							dwTimeRate;						    //时间频率
	__time64_t						tStartTime;							//开始时间
	__time64_t                      tConcludeTime;                      //结束时间
	TCHAR							szSystemMessage[LEN_USER_CHAT];		//系统消息
};

//邮件结构
struct DBR_GR_EMAIL
{
	DWORD							dwRecUserID;						//接收用户
	BYTE							cbType;								//邮件类型
	SYSTEMTIME						createTime;							//创建时间

	TCHAR							szRewardProp[256];					//奖励道具ID
	TCHAR							szEmailTitle[100];					//邮件标题
	TCHAR							szEmailMsg[320];					//邮件内容
};

//刷新在线玩家数
struct DBR_GR_RefreshOnlineUser
{
	char							szKey[32];
	char							szUserCount[8];
};

//加载道具
struct DBR_GR_LOADPROPERTY
{
	DWORD							dwUserID;
};

//任务列表
struct DBO_GR_PropertyListInfo
{
	WORD							wPropertyCount;						//数目
};

//////////////////////////////////////////////////////////////////////////////////

//私人场信息
struct DBR_GR_Private_Info
{
	WORD							wKindID;
};


//创建私人场
struct DBR_GR_Create_Private
{
	WORD							wKindID;
	DWORD							dwUserID;
	DWORD							dwCost;
	DWORD							dwCostType;
	BYTE							cbRoomType;
	DWORD							dwAgaginTable;
	BYTE							bPlayCoutIdex;							//游戏局数
	BYTE							bGameTypeIdex;							//游戏类型
	DWORD							bGameRuleIdex;							//游戏规则

	BYTE							cbEnterRoomScoreType;					//进房间时的分数类型;
	SCORE							lInitEnterRoomScore;					//进入房间时的分数;
};

//创建私人场
struct DBR_GR_Create_Private_Cost
{
	DWORD							dwUserID;
	DWORD							dwCost;
	DWORD							dwCostType;
};


//私人场信息
struct DBO_GR_Private_Info
{
	BYTE							cbDBExecSuccess;						//数据执行是否成功;
	WORD							wKindID;
	SCORE							lCostGold;
	BYTE							bPlayCout[4];							//玩家局数
	SCORE							lPlayCost[4];							//消耗点数
};

//比赛排行
struct DBO_GR_CreatePrivateResult
{
	BOOL							bSucess;
	BYTE							cbRoomType;
	TCHAR							szDescribeString[128];					//错误信息
	SCORE							lCurSocre;								//当前剩余
	WORD							wAgaginTable;							//重新加入桌子
	BYTE							bPlayCoutIdex;							//玩家局数
	BYTE							bGameTypeIdex;							//游戏类型
	DWORD							bGameRuleIdex;							//游戏规则

	BYTE							cbEnterRoomScoreType;					//进房间时的分数类型;
	SCORE							lInitEnterRoomScore;					//进入房间时的分数;

	DWORD							dwRecordID;								//录像记录总表的索引ID;
};

//解锁机器人
struct DBR_GR_UnlockAndroidUser
{
	WORD							wServerID;							//房间ID
	WORD							wBatchID;							//批次ID
};

//单一游戏记录返回recordID;
struct DBO_GR_SingleGameRecord
{
	DWORD							dwCreaterUserID;					//创建者;
	DWORD							dwRecordID;							//游戏记录主表ID;
};

//私人场结束
struct DBO_GR_PrivateGameEnd
{
	DWORD							dwUserID;
	SCORE							lScore;
	WORD							wPlayCount;
	DWORD							dwCardLibIndex;
	WORD							wKindID;
	WORD							wServerID;
};


//牌库;
struct DBO_GR_GameCardLibrary
{
	WORD							wKindID;
	DWORD							dwLibID;
	DWORD							dwCustomID;
	BYTE							cbCardCount;
	TCHAR							szCardData[128];
};

//牌库配置;
struct DBO_GR_GameCardLibraryCfg
{
	BYTE							cbEnable;			//牌库是否有效;
	DWORD							dwCount;			//牌库数量;
	DWORD							dwStartDateTime;	//开始时间;
	DWORD							dwEndDateTime;		//结算时间;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif