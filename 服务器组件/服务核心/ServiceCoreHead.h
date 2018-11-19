#ifndef SERVICE_CORE_HEAD_FILE
#define SERVICE_CORE_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//包含文件

//MFC 文件
#include <Nb30.h>
#include <Afxmt.h>
#include <AtlBase.h>

//系统文件
#include "..\..\全局定义\Platform.h"

//////////////////////////////////////////////////////////////////////////////////
//公共定义

//导出定义
#ifndef SERVICE_CORE_CLASS
	#ifdef  SERVICE_CORE_DLL
		#define SERVICE_CORE_CLASS _declspec(dllexport)
	#else
		#define SERVICE_CORE_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifndef _DEBUG
	#define SERVICE_CORE_DLL_NAME	TEXT("ServiceCore.dll")			//组件名字
#else
	#define SERVICE_CORE_DLL_NAME	TEXT("ServiceCoreD.dll")		//组件名字
#endif

//////////////////////////////////////////////////////////////////////////////////
//导出文件

#ifndef SERVICE_CORE_DLL
	#include "YSThread.h"
	#include "YSRegKey.h"
	#include "YSEncrypt.h"
	#include "YSService.h"
	#include "YSIniData.h"
	#include "YSDataQueue.h"
	#include "YSDataLocker.h"
	#include "YSCommandLine.h"
	#include "YSCharUtils.h"
	#include "YSAutoBuffer.h"
	#include "RedisInstance.h"
	#include "json.h"
	#include "sha1.hpp"
	#include "base64.h"
	#include "YSAllocationID.h"
#endif



//////////////////////////////////////////////////////////////////////////////////

#endif