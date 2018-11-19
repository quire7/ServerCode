#pragma once

//////////////////////////////////////////////////////////////////////////////////

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#ifndef WINVER
#define WINVER 0x0501
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0410
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0400
#endif

#define _ATL_ATTRIBUTES
#define _AFX_ALL_WARNINGS
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS

//////////////////////////////////////////////////////////////////////////////////

//MFC 文件
#include <AfxWin.h>
#include <AfxExt.h>
#include <AfxCmn.h>
#include <AfxDisp.h>

//组件头文件
#include "..\消息定义\CMD_Ox.h"

//////////////////////////////////////////////////////////////////////////////////

//编译环境
#include "..\..\..\全局定义\SDKCondition.h"

//////////////////////////////////////////////////////////////////////////////////


#ifdef SDK_CONDITION
#include "..\..\..\开发库\Include\GameServiceHead.h"
#else
#include "..\..\..\服务器组件\游戏服务\GameServiceHead.h"
#endif

//////////////////////////////////////////////////////////////////////////