#ifndef PLATFORM_HEAD_FILE
#define PLATFORM_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//包含文件

//定义文件
#include "Macro.h"
#include "Define.h"

//结构文件
#include "Struct.h"
#include "Packet.h"
#include "Property.h"
#include "DataStream.h"

//模板文件
#include "Array.h"
#include "Module.h"
#include "PacketAide.h"
#include "ServerRule.h"
#include "RightDefine.h"

//////////////////////////////////////////////////////////////////////////////////

//程序版本
#define VERSION_PLAZA				PROCESS_VERSION(0,0,1)				//大厅版本
#define VERSION_MOBILE_ANDROID		PROCESS_VERSION(0,0,1)				//手机版本
#define VERSION_MOBILE_IOS			PROCESS_VERSION(0,0,1)				//手机版本

//版本定义
#define VERSION_EFFICACY			0									//效验版本

//////////////////////////////////////////////////////////////////////////////////
//平台常量
const TCHAR szProduct[]=TEXT("宿迁英硕棋牌平台");							//产品名字
const TCHAR szProductKey[]=TEXT("YSQPGamePlatform");					//产品主键

//数据库名
const TCHAR szPlatformDB[]=TEXT("fkplatformdb");						//平台数据库
const TCHAR szAccountsDB[]=TEXT("fkaccountsdb");						//用户数据库
const TCHAR	szTreasureDB[]=TEXT("fktreasuredb");						//财富数据库
const TCHAR	szExerciseDB[]=TEXT("fkexercisedb");						//练习数据库


//////////////////////////////////////////////////////////////////////////////////


#endif