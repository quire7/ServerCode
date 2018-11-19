#ifndef MODULE_MANAGER_HEAD_HEAD_FILE
#define MODULE_MANAGER_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////

//平台定义
#include "..\..\全局定义\Platform.h"

//组件定义
#include "..\..\服务器组件\服务核心\ServiceCoreHead.h"
#include "..\..\服务器组件\游戏服务\GameServiceHead.h"
#include "..\..\服务器组件\内核引擎\KernelEngineHead.h"

//////////////////////////////////////////////////////////////////////////////////
//导出定义

//导出定义
#ifndef MODULE_MANAGER_CLASS
	#ifdef  MODULE_MANAGER_DLL
		#define MODULE_MANAGER_CLASS _declspec(dllexport)
	#else
		#define MODULE_MANAGER_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifndef _DEBUG
	#define MODULE_MANAGER_DLL_NAME	TEXT("ModuleManager.dll")			//组件 DLL 名字
#else
	#define MODULE_MANAGER_DLL_NAME	TEXT("ModuleManagerD.dll")			//组件 DLL 名字
#endif

//////////////////////////////////////////////////////////////////////////////////
//结构定义

//模块信息
struct tagGameModuleInfo
{
	//版本信息
	DWORD							dwClientVersion;					//游戏版本
	DWORD							dwServerVersion;					//服务版本
	DWORD							dwNativeVersion;					//本地版本

	//数据属性
	WORD							wGameID;							//模块标识
	TCHAR							szDataBaseAddr[15];					//数据库名
	TCHAR							szDataBaseName[32];					//数据库名

	//模块属性
	TCHAR							szGameName[LEN_KIND];				//游戏名字
	TCHAR							szServerDLLName[LEN_PROCESS];		//进程名字
};

//房间信息
struct tagGameServerInfo
{
	//索引变量
	WORD							wGameID;							//模块标识
	WORD							wServerID;							//房间号码

	//挂接属性
	WORD							wKindID;							//挂接类型
	//WORD							wNodeID;							//挂接节点
	WORD							wSortID;							//排列标识

	//版本信息
	DWORD							dwClientVersion;					//游戏版本
	DWORD							dwServerVersion;					//服务版本
	DWORD							dwNativeVersion;					//本地版本

	//模块属性
	TCHAR							szGameName[LEN_KIND];				//游戏名字
	TCHAR							szServerDLLName[LEN_PROCESS];		//进程名字

	//税收配置
	LONG							lCellScore;							//单位积分
	WORD							wRevenueRatio;						//税收比例
	SCORE							lServiceScore;						//服务费用

	//房间配置
	SCORE							lRestrictScore;						//限制积分
	SCORE							lMinTableScore;						//最低积分
	SCORE							lMinEnterScore;						//最低积分
	SCORE							lMaxEnterScore;						//最高积分

	//带入设置
	SCORE							lDefPutScore;						//默认一次带入多少积分
	SCORE							lLowPutScore;						//低于多少带入积分
	int								iExchangeRatio;						//兑换比例

	//其他配置
	SCORE							lMagicExpScore;						//魔法表情积分
	SCORE							lGratuityScore;						//打赏积分
	DWORD							dwWinExperience;					//胜利经验
	DWORD							dwFailExperience;					//失败经验

	//会员限制
	BYTE							cbMinEnterMember;					//最低会员
	BYTE							cbMaxEnterMember;					//最高会员

	//房间配置
	DWORD							dwServerRule;						//房间规则
	DWORD							dwAttachUserRight;					//附加权限

	//房间属性
	WORD							wMaxPlayer;							//最大数目
	WORD							wTableCount;						//桌子数目
	WORD							wServerPort;						//服务端口
	WORD                            wServerKind;                        //房间类别 
	WORD							wServerType;						//房间类型
	TCHAR							szServerName[LEN_SERVER];			//房间名称
	TCHAR                           szServerPasswd[LEN_PASSWORD];       //房间密码 

	//分组设置
	BYTE							cbDistributeRule;					//分组规则
	WORD							wMinDistributeUser;					//最少人数
	WORD							wDistributeTimeSpace;				//分组间隔
	WORD							wDistributeDrawCount;				//分组局数
	WORD							wMinPartakeGameUser;				//最少人数
	WORD							wMaxPartakeGameUser;				//最多人数


	////数据属性
	//TCHAR							szDataBaseName[32];					//数据库名
	//TCHAR							szDataBaseAddr[32];					//地址信息

	////缓存服务器
	//TCHAR							szCacheServerAddr[16];				//缓存服务器地址
	//WORD							wCacheServerPort;					//缓存服务器端口

	//数据设置
	BYTE							cbCustomRule[1024];					//自定规则
	TCHAR							szServiceMachine[LEN_MACHINE_ID];	//机器序列
};

//房间信息
struct tagGameServerCreate
{
	//索引变量
	WORD							wGameID;							//模块标识
	WORD							wServerID;							//房间号码

	//挂接属性
	WORD							wKindID;							//挂接类型
	//WORD							wNodeID;							//挂接节点
	WORD							wSortID;							//排列标识

	//税收配置
	LONG							lCellScore;							//单位积分
	WORD							wRevenueRatio;						//税收比例
	SCORE							lServiceScore;						//服务费用

	//限制配置
	SCORE							lRestrictScore;						//限制积分
	SCORE							lMinTableScore;						//最低积分
	SCORE							lMinEnterScore;						//最低积分
	SCORE							lMaxEnterScore;						//最高积分
	
	//带入设置
	SCORE							lDefPutScore;						//默认一次带入多少积分
	SCORE							lLowPutScore;						//低于多少带入积分
	int								iExchangeRatio;						//兑换比例

	//其他配置
	SCORE							lMagicExpScore;						//魔法表情积分
	SCORE							lGratuityScore;						//打赏积分
	DWORD							dwWinExperience;					//胜利经验
	DWORD							dwFailExperience;					//失败经验

	//会员限制
	BYTE							cbMinEnterMember;					//最低会员
	BYTE							cbMaxEnterMember;					//最高会员

	//房间配置
	DWORD							dwServerRule;						//房间规则
	DWORD							dwAttachUserRight;					//附加权限

	//房间属性
	WORD							wMaxPlayer;							//最大数目
	WORD							wTableCount;						//桌子数目
	WORD							wServerPort;						//服务端口
	WORD                            wServerKind;                        //房间类别 
	WORD							wServerType;						//房间类型
	TCHAR							szServerName[LEN_SERVER];			//房间名称
	TCHAR                           szServerPasswd[LEN_PASSWORD];       //房间密码 

	//分组设置
	BYTE							cbDistributeRule;					//分组规则
	WORD							wMinDistributeUser;					//最少人数	
	WORD							wDistributeTimeSpace;				//分组间隔
	WORD							wDistributeDrawCount;				//分组局数
	WORD							wMinPartakeGameUser;				//最少人数
	WORD							wMaxPartakeGameUser;				//最多人数

	////数据属性
	//TCHAR							szDataBaseName[32];					//数据库名
	//TCHAR							szDataBaseAddr[32];					//地址信息

	////缓存服务器
	//TCHAR							szCacheServerAddr[16];				//缓存服务器地址
	//WORD							wCacheServerPort;					//缓存服务器端口

	//数据设置
	BYTE							cbCustomRule[64];					//自定规则
	TCHAR							szServiceMachine[LEN_MACHINE_ID];	//机器序列
};

//////////////////////////////////////////////////////////////////////////////////

//配置参数
struct tagModuleInitParameter
{
	tagGameServiceAttrib			GameServiceAttrib;					//服务属性
	tagGameServiceOption			GameServiceOption;					//服务配置
	tagGameMatchOption				GameMatchOption;					//比赛信息
};

//////////////////////////////////////////////////////////////////////////////////

//导出文件
#ifndef MODULE_MANAGER_DLL 
	
	#include "ListControl.h"
	#include "DlgServerItem.h"
	#include "DlgServerMatch.h"
	#include "DlgServerWizard.h"
	
	#include "ModuleListControl.h"
	#include "ModuleDBParameter.h"
	#include "ModuleInfoManager.h"

	#include "ServerCustomRule.h"
	#include "ServerListControl.h"
	#include "ServerInfoManager.h"

#endif

//////////////////////////////////////////////////////////////////////////////////

#endif