#ifndef MATCH_SERVICE_HEAD_HEAD_FILE
#define MATCH_SERVICE_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////

//平台文件;
#include "..\..\服务器组件\游戏服务\MatchServiceInterface.h"


//////////////////////////////////////////////////////////////////////////////////
//导出定义;

//导出定义;
#ifndef MATCH_SERVICE_CLASS
	#ifdef  MATCH_SERVICE_DLL
		#define MATCH_SERVICE_CLASS _declspec(dllexport)
	#else
		#define MATCH_SERVICE_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义;
#ifndef _DEBUG
	#define MATCH_SERVICE_DLL_NAME	TEXT("MatchService.dll")			//组件名字
#else
	#define MATCH_SERVICE_DLL_NAME	TEXT("MatchServiceD.dll")			//组件名字
#endif

//////////////////////////////////////////////////////////////////////////////////
//类型声明;
class CLockTimeMatch;

//创建函数;
#define MATCH_SERVICE_CREATE_NAME	"CreateMatchServiceManager"			//创建函数;

//////////////////////////////////////////////////////////////////////////////////
//游戏服务
DECLARE_MODULE_HELPER(MatchServiceManager, MATCH_SERVICE_DLL_NAME, MATCH_SERVICE_CREATE_NAME)

//////////////////////////////////////////////////////////////////////////////////

#endif