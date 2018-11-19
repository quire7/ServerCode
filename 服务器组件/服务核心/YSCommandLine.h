#ifndef YS_COMMAND_LINE_HEAD_FILE
#define YS_COMMAND_LINE_HEAD_FILE

#pragma once

#include "ServiceCoreHead.h"

//////////////////////////////////////////////////////////////////////////////////

//命令解释
class SERVICE_CORE_CLASS CYSCommandLine
{
	//函数定义
public:
	//构造函数
	CYSCommandLine();
	//析构函数
	virtual ~CYSCommandLine();

	//功能函数
public:
	//查询命令
	bool SearchCommandItem(LPCTSTR pszCommandLine, LPCTSTR pszCommand, TCHAR szParameter[], WORD wParameterLen);
};

//////////////////////////////////////////////////////////////////////////////////

#endif