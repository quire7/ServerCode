#ifndef YS_SERVICE_HEAD_FILE
#define YS_SERVICE_HEAD_FILE

#pragma once

#include "ServiceCoreHead.h"
#include <string>

//////////////////////////////////////////////////////////////////////////////////

//ϵͳ����
class SERVICE_CORE_CLASS CYSService
{
	//��������
private:
	//���캯��
	CYSService();

	//ϵͳ����
public:
	//�����ַ�
	static bool SetClipboardString(LPCTSTR pszString);

	//������ʶ
public:
	//������ʶ
	static bool GetMachineID(TCHAR szMachineID[LEN_MACHINE_ID]);
	//������ʶ
	static bool GetMachineIDEx(TCHAR szMachineID[LEN_MACHINE_ID]);
	//������ַ
	static bool GetMACAddress(TCHAR szMACAddress[LEN_NETWORK_ID]);

	//ϵͳ�ȼ�
public:
	//ע���ȼ�
	static bool UnRegisterHotKey(HWND hWnd, UINT uKeyID);
	//ע���ȼ�
	static bool RegisterHotKey(HWND hWnd, UINT uKeyID, WORD wHotKey);

	//ϵͳ�ļ�
public:
	//����Ŀ¼
	static bool GetWorkDirectory(TCHAR szWorkDirectory[], WORD wBufferCount);
	//�ļ��汾
	static bool GetModuleVersion(LPCTSTR pszModuleName, DWORD & dwVersionInfo);

	//��������
public:
	//SYSTEMTIMEת�ַ���
	static std::string SystemTimeToChar(SYSTEMTIME& st);
	//�ַ���תSYSTEMTIME
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