#ifndef YS_SERVICE_HEAD_FILE
#define YS_SERVICE_HEAD_FILE

#pragma once

#include "ServiceCoreHead.h"
#include <string>

//////////////////////////////////////////////////////////////////////////////////

//系统服务
class SERVICE_CORE_CLASS CYSService
{
	//函数定义
private:
	//构造函数
	CYSService();

	//系统功能
public:
	//拷贝字符
	static bool SetClipboardString(LPCTSTR pszString);

	//机器标识
public:
	//机器标识
	static bool GetMachineID(TCHAR szMachineID[LEN_MACHINE_ID]);
	//机器标识
	static bool GetMachineIDEx(TCHAR szMachineID[LEN_MACHINE_ID]);
	//网卡地址
	static bool GetMACAddress(TCHAR szMACAddress[LEN_NETWORK_ID]);

	//系统热键
public:
	//注销热键
	static bool UnRegisterHotKey(HWND hWnd, UINT uKeyID);
	//注册热键
	static bool RegisterHotKey(HWND hWnd, UINT uKeyID, WORD wHotKey);

	//系统文件
public:
	//工作目录
	static bool GetWorkDirectory(TCHAR szWorkDirectory[], WORD wBufferCount);
	//文件版本
	static bool GetModuleVersion(LPCTSTR pszModuleName, DWORD & dwVersionInfo);

	//其它功能
public:
	//SYSTEMTIME转字符串
	static std::string SystemTimeToChar(SYSTEMTIME& st);
	//字符串转SYSTEMTIME
	static bool StringToSystemTime(LPCTSTR lpszValue, SYSTEMTIME &time);

	static std::string ToString(bool v);
	static std::string ToString(BYTE v);
	static std::string ToString(WORD v);
	static std::string ToString(DWORD v);
	static std::string ToString(LONG v);
	static std::string ToString(SCORE v);
	static bool ToBool(std::string v);
	static BYTE ToByte(std::string v);
	static WORD ToWord(std::string v);
	static DWORD ToDword(std::string v);
	static LONG ToLong(std::string v);
	static SCORE ToScore(std::string v);
};

//////////////////////////////////////////////////////////////////////////////////

#endif