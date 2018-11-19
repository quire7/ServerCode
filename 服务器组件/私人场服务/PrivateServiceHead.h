#ifndef PRIVATE_SERVICE_HEAD_HEAD_FILE
#define PRIVATE_SERVICE_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////


//平台文件;
#include "..\..\服务器组件\游戏服务\MatchServiceInterface.h"


//////////////////////////////////////////////////////////////////////////////////
//导出定义

//导出定义
#ifndef PRIVATE_SERVICE_CLASS
	#ifdef  PRIVATE_SERVICE_DLL
		#define PRIVATE_SERVICE_CLASS _declspec(dllexport)
	#else
		#define PRIVATE_SERVICE_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifndef _DEBUG
	#define PRIVATE_SERVICE_DLL_NAME	TEXT("PrivateService.dll")			//组件名字
#else
	#define PRIVATE_SERVICE_DLL_NAME	TEXT("PrivateServiceD.dll")			//组件名字
#endif

//////////////////////////////////////////////////////////////////////////////////
//类型声明
class PriaveteGame;

//创建函数;
#define PRIVATE_SERVICE_CREATE_NAME	"CreatePrivateServiceManager"			//创建函数;

//游戏服务
//DECLARE_MODULE_HELPER(MatchServiceManager, PRIVATE_SERVICE_DLL_NAME, PRIVATE_SERVICE_CREATE_NAME)

//////////////////////////////////////////////////////////////////////////////////

#endif