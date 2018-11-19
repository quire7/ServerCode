#ifndef YS_INI_DATA_HEAD_FILE
#define YS_INI_DATA_HEAD_FILE

#pragma once

#include "ServiceCoreHead.h"

//////////////////////////////////////////////////////////////////////////////////

//��������
class SERVICE_CORE_CLASS CYSIniData
{
	//��������
protected:
	TCHAR							m_szIniFile[MAX_PATH];				//�ļ�·��

	//��������
public:
	//���캯��
	CYSIniData();
	//��������
	virtual ~CYSIniData();

	//·������
public:
	//����·��
	VOID SetIniFilePath(LPCTSTR pszIniFile);
	//��ȡ·��
	LPCTSTR GetIniFilePath() { return m_szIniFile; }

	//���ݶ�ȡ
public:
	//��ȡ����
	bool ReadRect(RECT & ValueRect, LPCTSTR pszItem, LPCTSTR pszSubItem);
	//��ȡ�ߴ�
	bool ReadSize(SIZE & ValueSize, LPCTSTR pszItem, LPCTSTR pszSubItem);
	//��ȡ����
	bool ReadPoint(POINT & ValuePoint, LPCTSTR pszItem, LPCTSTR pszSubItem);
	//��ȡ��ɫ
	bool ReadColor(COLORREF & ValueColor, LPCTSTR pszItem, LPCTSTR pszSubItem);

	//�����ȡ
public:
	//��ȡ��ֵ
	UINT ReadInt(LPCTSTR pszItem, LPCTSTR pszSubItem, INT nDefault);
	//��ȡ�ַ�
	LPCTSTR ReadString(LPCTSTR pszItem, LPCTSTR pszSubItem, LPCTSTR pszDefault, LPTSTR pszString, WORD wMaxCount);
	//��ȡ�ַ�
	LPCTSTR ReadEncryptString(LPCTSTR pszItem, LPCTSTR pszSubItem, LPCTSTR pszDefault, LPTSTR pszString, WORD wMaxCount);

	//�ڲ�����
protected:
	//ת����ֵ
	LONG SwitchStringToValue(LPCTSTR & pszSring);
};

//////////////////////////////////////////////////////////////////////////////////

#endif