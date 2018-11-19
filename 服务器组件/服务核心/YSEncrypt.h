#ifndef YS_ENCRYPT_HEAD_FILE
#define YS_ENCRYPT_HEAD_FILE

#pragma once

#include "ServiceCoreHead.h"

//////////////////////////////////////////////////////////////////////////////////

//�궨��
#define XOR_TIMES					8									//���ܱ���
#define MAX_SOURCE_LEN				64									//��󳤶�
#define MAX_ENCRYPT_LEN				(MAX_SOURCE_LEN*XOR_TIMES)			//��󳤶�

//////////////////////////////////////////////////////////////////////////////////

//�������
class SERVICE_CORE_CLASS CYSEncrypt
{
	//��������
public:
	//���캯��
	CYSEncrypt();
	//��������
	virtual ~CYSEncrypt();

	//���ܺ���
public:
	//��������
	static bool MD5Encrypt(LPCTSTR pszSourceData, TCHAR szMD5Result[LEN_MD5]);

	//���ܺ���
public:
	//��������
	static bool XorEncrypt(LPCTSTR pszSourceData, LPTSTR pszEncrypData, WORD wMaxCount);
	//�⿪����
	static bool XorCrevasse(LPCTSTR pszEncrypData, LPTSTR pszSourceData, WORD wMaxCount);

	//���ܺ���
public:
	//��������
	static bool MapEncrypt(LPCTSTR pszSourceData, LPTSTR pszEncrypData, WORD wMaxCount);
	//�⿪����
	static bool MapCrevasse(LPCTSTR pszEncrypData, LPTSTR pszSourceData, WORD wMaxCount);

	//�������ݼ���
public:
	//��������
	static WORD EncryptBuffer(BYTE pcbDataBuffer[], WORD wDataSize, WORD wBufferSize, bool bClient=true);
	//��������
	static WORD CrevasseBuffer(BYTE pcbDataBuffer[], WORD wDataSize);
};

//////////////////////////////////////////////////////////////////////////////////

#endif