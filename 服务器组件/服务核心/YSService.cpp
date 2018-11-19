#include "StdAfx.h"
#include "YSService.h"
#include "YSEncrypt.h"

#pragma comment(lib, "Version.lib")

//////////////////////////////////////////////////////////////////////////////////

//状态信息
struct tagAstatInfo
{
	ADAPTER_STATUS					AdapterStatus;						//网卡状态
	NAME_BUFFER						NameBuff[16];						//名字缓冲
};

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CYSService::CYSService()
{
}

//拷贝字符
bool CYSService::SetClipboardString(LPCTSTR pszString)
{
	//变量定义
	HANDLE hData=NULL;
	BOOL bOpenClopboard=FALSE;

	//执行逻辑
	__try
	{
		//打开拷贝
		bOpenClopboard=OpenClipboard(AfxGetMainWnd()->m_hWnd);
		if (bOpenClopboard==FALSE) __leave;

		//清空拷贝
		if (EmptyClipboard()==FALSE) __leave;

		//申请内存
		HANDLE hData=GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,CountStringBuffer(pszString));
		if (hData==NULL) __leave;

		//复制数据
		lstrcpy((LPTSTR)GlobalLock(hData),pszString);
		GlobalUnlock(hData);

		//设置数据
		#ifndef _UNICODE
			::SetClipboardData(CF_TEXT,hData);
		#else
			::SetClipboardData(CF_UNICODETEXT,hData);
		#endif
	}

	//终止程序
	__finally
	{
		//释放内存
		if (hData!=NULL) GlobalUnlock(hData);

		//关闭拷贝
		if (bOpenClopboard==TRUE) CloseClipboard();

		//错误判断
		if (AbnormalTermination()==TRUE)
		{
			ASSERT(FALSE);
		}
	}

	return true;
}

//机器标识
bool CYSService::GetMachineID(TCHAR szMachineID[LEN_MACHINE_ID])
{
	//变量定义
	TCHAR szMACAddress[LEN_NETWORK_ID]=TEXT("");

	//网卡标识
	GetMACAddress(szMACAddress);

	//转换信息
	ASSERT(LEN_MACHINE_ID>=LEN_MD5);
	CYSEncrypt::MD5Encrypt(szMACAddress,szMachineID);

	return true;
}

//机器标识
bool CYSService::GetMachineIDEx(TCHAR szMachineID[LEN_MACHINE_ID])
{
	//变量定义
	TCHAR szMACAddress[LEN_NETWORK_ID]=TEXT("");

	//网卡标识
	WORD wMacSize=GetMACAddress(szMACAddress);

	//硬盘标识
	DWORD *pbuf=(DWORD*)szMACAddress;
	WORD wIndex=(wMacSize+sizeof(DWORD)-1)/sizeof(DWORD);
	LPCTSTR pszHardDisk[]={TEXT("C:\\"),TEXT("D:\\"),TEXT("E:\\")};
	for (WORD i=wIndex;i<CountArray(pszHardDisk);i++)
	{
		ASSERT(CountArray(pszHardDisk)>(i-wIndex));
		GetVolumeInformation(pszHardDisk[i-wIndex],NULL,0,pbuf+i,NULL,NULL,0,NULL);
	}

	//转换信息
	ASSERT(LEN_MACHINE_ID>=LEN_MD5);
	CYSEncrypt::MD5Encrypt(szMACAddress,szMachineID);

	return true;
}

//网卡地址
bool CYSService::GetMACAddress(TCHAR szMACAddress[LEN_NETWORK_ID])
{
	//变量定义
	HINSTANCE hInstance=NULL;

	//执行逻辑
	__try
	{
		//加载 DLL
		hInstance=LoadLibrary(TEXT("NetApi32.dll"));
		if (hInstance==NULL) __leave;

		//获取函数
		typedef BYTE __stdcall NetBiosProc(NCB * Ncb);
		NetBiosProc * pNetBiosProc=(NetBiosProc *)GetProcAddress(hInstance,"Netbios");
		if (pNetBiosProc==NULL) __leave;

		//变量定义
		NCB Ncb;
		LANA_ENUM LanaEnum;
		ZeroMemory(&Ncb,sizeof(Ncb));
		ZeroMemory(&LanaEnum,sizeof(LanaEnum));

		//枚举网卡
		Ncb.ncb_command=NCBENUM;
		Ncb.ncb_length=sizeof(LanaEnum);
		Ncb.ncb_buffer=(BYTE *)&LanaEnum;
		if ((pNetBiosProc(&Ncb)!=NRC_GOODRET)||(LanaEnum.length==0)) __leave;

		//获取地址
		if (LanaEnum.length>0)
		{
			//变量定义
			tagAstatInfo Adapter;
			ZeroMemory(&Adapter,sizeof(Adapter));

			//重置网卡
			Ncb.ncb_command=NCBRESET;
			Ncb.ncb_lana_num=LanaEnum.lana[0];
			if (pNetBiosProc(&Ncb)!=NRC_GOODRET) __leave;

			//获取状态
			Ncb.ncb_command=NCBASTAT;
			Ncb.ncb_length=sizeof(Adapter);
			Ncb.ncb_buffer=(BYTE *)&Adapter;
			Ncb.ncb_lana_num=LanaEnum.lana[0];
			strcpy_s((char *)Ncb.ncb_callname, sizeof(Ncb.ncb_callname), "*");
			if (pNetBiosProc(&Ncb)!=NRC_GOODRET) __leave;

			//获取地址
			for (INT i=0;i<6;i++)
			{
				ASSERT((i*2)<LEN_NETWORK_ID);
				_stprintf_s(&szMACAddress[i * 2], (LEN_NETWORK_ID-2*i), TEXT("%02X"), Adapter.AdapterStatus.adapter_address[i]);
			}
		}
	}

	//结束清理
	__finally
	{
		//释放资源
		if (hInstance!=NULL)
		{
			FreeLibrary(hInstance);
			hInstance=NULL;
		}

		//错误断言
		if (AbnormalTermination()==TRUE)
		{
			ASSERT(FALSE);
		}
	}

	return true;
}

//注销热键
bool CYSService::UnRegisterHotKey(HWND hWnd, UINT uKeyID)
{
	//注销热键
	BOOL bSuccess=::UnregisterHotKey(hWnd,uKeyID);

	return (bSuccess==TRUE)?true:false;
}

//注册热键
bool CYSService::RegisterHotKey(HWND hWnd, UINT uKeyID, WORD wHotKey)
{
	//变量定义
	BYTE cbModifiers=0;
	if (HIBYTE(wHotKey)&HOTKEYF_ALT)
	{
		cbModifiers |= MOD_ALT;
	}

	if (HIBYTE(wHotKey)&HOTKEYF_SHIFT)
	{
		cbModifiers |= MOD_SHIFT;
	}

	if (HIBYTE(wHotKey)&HOTKEYF_CONTROL)
	{
		cbModifiers |= MOD_CONTROL;
	}

	//注册热键
	BOOL bSuccess=::RegisterHotKey(hWnd,uKeyID,cbModifiers,LOBYTE(wHotKey));

	return (bSuccess==TRUE)?true:false;
}

//进程目录
bool CYSService::GetWorkDirectory(TCHAR szWorkDirectory[], WORD wBufferCount)
{
	//模块路径
	TCHAR szModulePath[MAX_PATH]=TEXT("");
	GetModuleFileName(AfxGetInstanceHandle(),szModulePath,CountArray(szModulePath));

	//分析文件
	for (INT i=lstrlen(szModulePath);i>=0;i--)
	{
		if (szModulePath[i]==TEXT('\\'))
		{
			szModulePath[i]=0;
			break;
		}
	}

	//设置结果
	ASSERT(szModulePath[0]!=0);
	lstrcpyn(szWorkDirectory,szModulePath,wBufferCount);

	return true;
}

//文件版本
bool CYSService::GetModuleVersion(LPCTSTR pszModuleName, DWORD & dwVersionInfo)
{
	//设置结果
	dwVersionInfo=0L;

	//接收缓冲
	BYTE cbInfoBuffer[1024];
	ZeroMemory(cbInfoBuffer,sizeof(cbInfoBuffer));

	//模块信息
    DWORD dwFileHandle=NULL;
	if (GetFileVersionInfo(pszModuleName,dwFileHandle,sizeof(cbInfoBuffer),cbInfoBuffer)==FALSE) return false;

	//获取信息
	UINT uQuerySize=0;
	VS_FIXEDFILEINFO * pFixedFileInfo=NULL;
    if (VerQueryValue(cbInfoBuffer,TEXT("\\"),(VOID * *)&pFixedFileInfo,&uQuerySize)==FALSE) return false;

	//设置结果
	if ((pFixedFileInfo!=NULL)&&(uQuerySize==sizeof(VS_FIXEDFILEINFO)))
	{
		//设置版本
		WORD wVersion1=HIWORD(pFixedFileInfo->dwFileVersionMS);
		WORD wVersion2=LOWORD(pFixedFileInfo->dwFileVersionMS);
		WORD wVersion3=HIWORD(pFixedFileInfo->dwFileVersionLS);
		WORD wVersion4=LOWORD(pFixedFileInfo->dwFileVersionLS);
		dwVersionInfo=MAKELONG(MAKEWORD(wVersion4,wVersion3),MAKEWORD(wVersion2,wVersion1));

		return true;
	}

	return false;
}

//SYSTEMTIME转字符串
std::string CYSService::SystemTimeToChar(SYSTEMTIME& st)
{
	char szBuffer[64] = "";
	_snprintf_s(szBuffer, sizeof(szBuffer), "%d-%d-%d %d:%d:%d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	return std::string(szBuffer);
}

bool CYSService::StringToSystemTime(LPCTSTR lpszValue, SYSTEMTIME &time)
{
	if (!lpszValue)
	{
		return false;
	}
	int nYear = 0;
	int nMonth = 0;
	int nDay = 0;
	int nHour = 0;
	int nSecond = 0;
	int nMinute = 0;

	CString  str = lpszValue;
	_stscanf_s(str, _T("%d-%d-%d %d:%d:%d"), &nYear, &nMonth, &nDay, &nHour, &nMinute, &nSecond); 
	
	if (nMonth == 0 || nDay == 0)
	{
		_stscanf_s(str, _T("%d/%d/%d %d:%d:%d"), &nYear, &nMonth, &nDay, &nHour, &nMinute, &nSecond);
	}
	
	time.wYear = nYear;
	time.wMonth = nMonth;
	time.wDay = nDay;
	time.wHour = nHour;
	time.wSecond = nSecond;
	time.wMinute = nMinute;

	return true;
}



std::string CYSService::ToString(bool v)
{
	return v ? "true" : "false";
}

std::string CYSService::ToString(BYTE v)
{
	char buff[4] = { 0 };
	sprintf_s(buff, sizeof(buff), "%d", v);
	return buff;
}

std::string CYSService::ToString(WORD v)
{
	char buff[8] = { 0 };
	sprintf_s(buff, sizeof(buff), "%d", v);
	return buff;
}

std::string CYSService::ToString(DWORD v)
{
	char buff[16] = { 0 };
	sprintf_s(buff, sizeof(buff), "%u", v);
	return buff;
}

std::string CYSService::ToString(LONG v)
{
	char buff[32] = { 0 };
	sprintf_s(buff, sizeof(buff), "%ld", v);
	return buff;
}

std::string CYSService::ToString(SCORE v)
{
	char buff[32] = { 0 };
	sprintf_s(buff, sizeof(buff), "%I64d", v);
	return buff;
}

bool CYSService::ToBool(std::string v)
{
	if (v.compare("true") == 0)
		return true;

	return false;
}

BYTE CYSService::ToByte(std::string v)
{
	return atoi(v.c_str());
}

WORD CYSService::ToWord(std::string v)
{
	return atoi(v.c_str());
}

DWORD CYSService::ToDword(std::string v)
{
	return atoi(v.c_str());
}

LONG CYSService::ToLong(std::string v)
{
	return atol(v.c_str());
}

SCORE CYSService::ToScore(std::string v)
{
	return atoll(v.c_str());
}

//////////////////////////////////////////////////////////////////////////////////
