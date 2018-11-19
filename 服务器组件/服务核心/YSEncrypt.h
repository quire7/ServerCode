#ifndef YS_ENCRYPT_HEAD_FILE
#define YS_ENCRYPT_HEAD_FILE

#pragma once

#include "ServiceCoreHead.h"

//////////////////////////////////////////////////////////////////////////////////

//宏定义
#define XOR_TIMES					8									//加密倍数
#define MAX_SOURCE_LEN				64									//最大长度
#define MAX_ENCRYPT_LEN				(MAX_SOURCE_LEN*XOR_TIMES)			//最大长度

//////////////////////////////////////////////////////////////////////////////////

//加密组件
class SERVICE_CORE_CLASS CYSEncrypt
{
	//函数定义
public:
	//构造函数
	CYSEncrypt();
	//析构函数
	virtual ~CYSEncrypt();

	//加密函数
public:
	//生成密文
	static bool MD5Encrypt(LPCTSTR pszSourceData, TCHAR szMD5Result[LEN_MD5]);

	//加密函数
public:
	//生成密文
	static bool XorEncrypt(LPCTSTR pszSourceData, LPTSTR pszEncrypData, WORD wMaxCount);
	//解开密文
	static bool XorCrevasse(LPCTSTR pszEncrypData, LPTSTR pszSourceData, WORD wMaxCount);

	//加密函数
public:
	//生成密文
	static bool MapEncrypt(LPCTSTR pszSourceData, LPTSTR pszEncrypData, WORD wMaxCount);
	//解开密文
	static bool MapCrevasse(LPCTSTR pszEncrypData, LPTSTR pszSourceData, WORD wMaxCount);

	//网络数据加密
public:
	//加密数据
	static WORD EncryptBuffer(BYTE pcbDataBuffer[], WORD wDataSize, WORD wBufferSize, bool bClient=true);
	//解密数据
	static WORD CrevasseBuffer(BYTE pcbDataBuffer[], WORD wDataSize);
};

//////////////////////////////////////////////////////////////////////////////////

#endif