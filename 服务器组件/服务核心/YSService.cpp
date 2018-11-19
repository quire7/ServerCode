#include "StdAfx.h"
#include "YSService.h"
#include "YSEncrypt.h"

#pragma comment(lib, "Version.lib")

//////////////////////////////////////////////////////////////////////////////////

//״̬��Ϣ
struct tagAstatInfo
{
	ADAPTER_STATUS					AdapterStatus;						//����״̬
	NAME_BUFFER						NameBuff[16];						//���ֻ���
};

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CYSService::CYSService()
{
}

//�����ַ�
bool CYSService::SetClipboardString(LPCTSTR pszString)
{
	//��������
	HANDLE hData=NULL;
	BOOL bOpenClopboard=FALSE;

	//ִ���߼�
	__try
	{
		//�򿪿���
		bOpenClopboard=OpenClipboard(AfxGetMainWnd()->m_hWnd);
		if (bOpenClopboard==FALSE) __leave;

		//��տ���
		if (EmptyClipboard()==FALSE) __leave;

		//�����ڴ�
		HANDLE hData=GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,CountStringBuffer(pszString));
		if (hData==NULL) __leave;

		//��������
		lstrcpy((LPTSTR)GlobalLock(hData),pszString);
		GlobalUnlock(hData);

		//��������
		#ifndef _UNICODE
			::SetClipboardData(CF_TEXT,hData);
		#else
			::SetClipboardData(CF_UNICODETEXT,hData);
		#endif
	}

	//��ֹ����
	__finally
	{
		//�ͷ��ڴ�
		if (hData!=NULL) GlobalUnlock(hData);

		//�رտ���
		if (bOpenClopboard==TRUE) CloseClipboard();

		//�����ж�
		if (AbnormalTermination()==TRUE)
		{
			ASSERT(FALSE);
		}
	}

	return true;
}

//������ʶ
bool CYSService::GetMachineID(TCHAR szMachineID[LEN_MACHINE_ID])
{
	//��������
	TCHAR szMACAddress[LEN_NETWORK_ID]=TEXT("");

	//������ʶ
	GetMACAddress(szMACAddress);

	//ת����Ϣ
	ASSERT(LEN_MACHINE_ID>=LEN_MD5);
	CYSEncrypt::MD5Encrypt(szMACAddress,szMachineID);

	return true;
}

//������ʶ
bool CYSService::GetMachineIDEx(TCHAR szMachineID[LEN_MACHINE_ID])
{
	//��������
	TCHAR szMACAddress[LEN_NETWORK_ID]=TEXT("");

	//������ʶ
	WORD wMacSize=GetMACAddress(szMACAddress);

	//Ӳ�̱�ʶ
	DWORD *pbuf=(DWORD*)szMACAddress;
	WORD wIndex=(wMacSize+sizeof(DWORD)-1)/sizeof(DWORD);
	LPCTSTR pszHardDisk[]={TEXT("C:\\"),TEXT("D:\\"),TEXT("E:\\")};
	for (WORD i=wIndex;i<CountArray(pszHardDisk);i++)
	{
		ASSERT(CountArray(pszHardDisk)>(i-wIndex));
		GetVolumeInformation(pszHardDisk[i-wIndex],NULL,0,pbuf+i,NULL,NULL,0,NULL);
	}

	//ת����Ϣ
	ASSERT(LEN_MACHINE_ID>=LEN_MD5);
	CYSEncrypt::MD5Encrypt(szMACAddress,szMachineID);

	return true;
}

//������ַ
bool CYSService::GetMACAddress(TCHAR szMACAddress[LEN_NETWORK_ID])
{
	//��������
	HINSTANCE hInstance=NULL;

	//ִ���߼�
	__try
	{
		//���� DLL
		hInstance=LoadLibrary(TEXT("NetApi32.dll"));
		if (hInstance==NULL) __leave;

		//��ȡ����
		typedef BYTE __stdcall NetBiosProc(NCB * Ncb);
		NetBiosProc * pNetBiosProc=(NetBiosProc *)GetProcAddress(hInstance,"Netbios");
		if (pNetBiosProc==NULL) __leave;

		//��������
		NCB Ncb;
		LANA_ENUM LanaEnum;
		ZeroMemory(&Ncb,sizeof(Ncb));
		ZeroMemory(&LanaEnum,sizeof(LanaEnum));

		//ö������
		Ncb.ncb_command=NCBENUM;
		Ncb.ncb_length=sizeof(LanaEnum);
		Ncb.ncb_buffer=(BYTE *)&LanaEnum;
		if ((pNetBiosProc(&Ncb)!=NRC_GOODRET)||(LanaEnum.length==0)) __leave;

		//��ȡ��ַ
		if (LanaEnum.length>0)
		{
			//��������
			tagAstatInfo Adapter;
			ZeroMemory(&Adapter,sizeof(Adapter));

			//��������
			Ncb.ncb_command=NCBRESET;
			Ncb.ncb_lana_num=LanaEnum.lana[0];
			if (pNetBiosProc(&Ncb)!=NRC_GOODRET) __leave;

			//��ȡ״̬
			Ncb.ncb_command=NCBASTAT;
			Ncb.ncb_length=sizeof(Adapter);
			Ncb.ncb_buffer=(BYTE *)&Adapter;
			Ncb.ncb_lana_num=LanaEnum.lana[0];
			strcpy_s((char *)Ncb.ncb_callname, sizeof(Ncb.ncb_callname), "*");
			if (pNetBiosProc(&Ncb)!=NRC_GOODRET) __leave;

			//��ȡ��ַ
			for (INT i=0;i<6;i++)
			{
				ASSERT((i*2)<LEN_NETWORK_ID);
				_stprintf_s(&szMACAddress[i * 2], (LEN_NETWORK_ID-2*i), TEXT("%02X"), Adapter.AdapterStatus.adapter_address[i]);
			}
		}
	}

	//��������
	__finally
	{
		//�ͷ���Դ
		if (hInstance!=NULL)
		{
			FreeLibrary(hInstance);
			hInstance=NULL;
		}

		//�������
		if (AbnormalTermination()==TRUE)
		{
			ASSERT(FALSE);
		}
	}

	return true;
}

//ע���ȼ�
bool CYSService::UnRegisterHotKey(HWND hWnd, UINT uKeyID)
{
	//ע���ȼ�
	BOOL bSuccess=::UnregisterHotKey(hWnd,uKeyID);

	return (bSuccess==TRUE)?true:false;
}

//ע���ȼ�
bool CYSService::RegisterHotKey(HWND hWnd, UINT uKeyID, WORD wHotKey)
{
	//��������
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

	//ע���ȼ�
	BOOL bSuccess=::RegisterHotKey(hWnd,uKeyID,cbModifiers,LOBYTE(wHotKey));

	return (bSuccess==TRUE)?true:false;
}

//����Ŀ¼
bool CYSService::GetWorkDirectory(TCHAR szWorkDirectory[], WORD wBufferCount)
{
	//ģ��·��
	TCHAR szModulePath[MAX_PATH]=TEXT("");
	GetModuleFileName(AfxGetInstanceHandle(),szModulePath,CountArray(szModulePath));

	//�����ļ�
	for (INT i=lstrlen(szModulePath);i>=0;i--)
	{
		if (szModulePath[i]==TEXT('\\'))
		{
			szModulePath[i]=0;
			break;
		}
	}

	//���ý��
	ASSERT(szModulePath[0]!=0);
	lstrcpyn(szWorkDirectory,szModulePath,wBufferCount);

	return true;
}

//�ļ��汾
bool CYSService::GetModuleVersion(LPCTSTR pszModuleName, DWORD & dwVersionInfo)
{
	//���ý��
	dwVersionInfo=0L;

	//���ջ���
	BYTE cbInfoBuffer[1024];
	ZeroMemory(cbInfoBuffer,sizeof(cbInfoBuffer));

	//ģ����Ϣ
    DWORD dwFileHandle=NULL;
	if (GetFileVersionInfo(pszModuleName,dwFileHandle,sizeof(cbInfoBuffer),cbInfoBuffer)==FALSE) return false;

	//��ȡ��Ϣ
	UINT uQuerySize=0;
	VS_FIXEDFILEINFO * pFixedFileInfo=NULL;
    if (VerQueryValue(cbInfoBuffer,TEXT("\\"),(VOID * *)&pFixedFileInfo,&uQuerySize)==FALSE) return false;

	//���ý��
	if ((pFixedFileInfo!=NULL)&&(uQuerySize==sizeof(VS_FIXEDFILEINFO)))
	{
		//���ð汾
		WORD wVersion1=HIWORD(pFixedFileInfo->dwFileVersionMS);
		WORD wVersion2=LOWORD(pFixedFileInfo->dwFileVersionMS);
		WORD wVersion3=HIWORD(pFixedFileInfo->dwFileVersionLS);
		WORD wVersion4=LOWORD(pFixedFileInfo->dwFileVersionLS);
		dwVersionInfo=MAKELONG(MAKEWORD(wVersion4,wVersion3),MAKEWORD(wVersion2,wVersion1));

		return true;
	}

	return false;
}

//SYSTEMTIMEת�ַ���
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
