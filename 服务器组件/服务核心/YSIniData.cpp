#include "StdAfx.h"
#include "YSIniData.h"
#include "YSEncrypt.h"

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CYSIniData::CYSIniData()
{
	//设置变量
	ZeroMemory(m_szIniFile,sizeof(m_szIniFile));

	return;
}

//析构函数
CYSIniData::~CYSIniData()
{
}

//设置路径
VOID CYSIniData::SetIniFilePath(LPCTSTR pszIniFile)
{
	//设置变量
	lstrcpyn(m_szIniFile,pszIniFile,CountArray(m_szIniFile));

	return;
}

//读取数值
UINT CYSIniData::ReadInt(LPCTSTR pszItem, LPCTSTR pszSubItem, INT nDefault)
{
	//效验状态
	ASSERT(m_szIniFile[0]!=0);

	//读取数值
	UINT uReadData=GetPrivateProfileInt(pszItem,pszSubItem,nDefault,m_szIniFile);

	return uReadData;
}

//读取字符
LPCTSTR CYSIniData::ReadString(LPCTSTR pszItem, LPCTSTR pszSubItem, LPCTSTR pszDefault, LPTSTR pszString, WORD wMaxCount)
{
	//效验状态
	ASSERT(m_szIniFile[0]!=0);

	//读取字符
	GetPrivateProfileString(pszItem,pszSubItem,pszDefault,pszString,wMaxCount,m_szIniFile);

	return pszString;
}

//读取字符
LPCTSTR CYSIniData::ReadEncryptString(LPCTSTR pszItem, LPCTSTR pszSubItem, LPCTSTR pszDefault, LPTSTR pszString, WORD wMaxCount)
{
	//效验状态
	ASSERT(m_szIniFile[0]!=0);
	ASSERT(wMaxCount<=MAX_SOURCE_LEN);

	//设置结果
	if (wMaxCount>0) pszString[0]=0;

	//读取字符
	TCHAR szStringRead[MAX_ENCRYPT_LEN];
	DWORD dwReadCount=GetPrivateProfileString(pszItem,pszSubItem,NULL,szStringRead,MAX_ENCRYPT_LEN,m_szIniFile);

	//解密字符
	if ((dwReadCount>0)&&(dwReadCount<CountArray(szStringRead)))
	{
		CYSEncrypt YSEncrypt;
		YSEncrypt.XorCrevasse(szStringRead,pszString,wMaxCount);
	}

	//默认参数
	if ((wMaxCount>0)&&(pszString[0]==0)) lstrcpyn(pszString,pszDefault,wMaxCount);

	return pszString;
}

//读取矩形
bool CYSIniData::ReadRect(RECT & ValueRect, LPCTSTR pszItem, LPCTSTR pszSubItem)
{
	//效验状态
	ASSERT(m_szIniFile[0]!=0);

	//设置变量
	TCHAR szReadData[64]=TEXT("");
	ZeroMemory(&ValueRect,sizeof(ValueRect));

	//读取字符
	GetPrivateProfileString(pszItem,pszSubItem,TEXT(""),szReadData,CountArray(szReadData),m_szIniFile);

	//数据处理
	if (szReadData[0]!=0)
	{
		//读取变量
		LPCTSTR pszString=szReadData;
		ValueRect.left=SwitchStringToValue(pszString);
		ValueRect.top=SwitchStringToValue(pszString);
		ValueRect.right=SwitchStringToValue(pszString);
		ValueRect.bottom=SwitchStringToValue(pszString);

		return true;
	}

	return false;
}

//读取尺寸
bool CYSIniData::ReadSize(SIZE & ValueSize, LPCTSTR pszItem, LPCTSTR pszSubItem)
{
	//效验状态
	ASSERT(m_szIniFile[0]!=0);

	//设置变量
	TCHAR szReadData[64]=TEXT("");
	ZeroMemory(&ValueSize,sizeof(ValueSize));

	//读取字符
	GetPrivateProfileString(pszItem,pszSubItem,TEXT(""),szReadData,CountArray(szReadData),m_szIniFile);

	//数据处理
	if (szReadData[0]!=0)
	{
		//读取变量
		LPCTSTR pszString=szReadData;
		ValueSize.cx=SwitchStringToValue(pszString);
		ValueSize.cy=SwitchStringToValue(pszString);

		return true;
	}

	return false;
}

//读取坐标
bool CYSIniData::ReadPoint(POINT & ValuePoint, LPCTSTR pszItem, LPCTSTR pszSubItem)
{
	//效验状态
	ASSERT(m_szIniFile[0]!=0);

	//设置变量
	TCHAR szReadData[64]=TEXT("");
	ZeroMemory(&ValuePoint,sizeof(ValuePoint));

	//读取字符
	GetPrivateProfileString(pszItem,pszSubItem,TEXT(""),szReadData,CountArray(szReadData),m_szIniFile);

	//数据处理
	if (szReadData[0]!=0)
	{
		//读取变量
		LPCTSTR pszString=szReadData;
		ValuePoint.x=SwitchStringToValue(pszString);
		ValuePoint.y=SwitchStringToValue(pszString);

		return true;
	}

	return false;
}

//读取颜色
bool CYSIniData::ReadColor(COLORREF & ValueColor, LPCTSTR pszItem, LPCTSTR pszSubItem)
{
	//效验状态
	ASSERT(m_szIniFile[0]!=0);

	//设置变量
	TCHAR szReadData[64]=TEXT("");
	ZeroMemory(&ValueColor,sizeof(ValueColor));

	//读取字符
	GetPrivateProfileString(pszItem,pszSubItem,TEXT(""),szReadData,CountArray(szReadData),m_szIniFile);

	//数据处理
	if (szReadData[0]!=0)
	{
		//读取变量
		LPCTSTR pszString=szReadData;
		ValueColor=RGB(SwitchStringToValue(pszString),SwitchStringToValue(pszString),SwitchStringToValue(pszString));

		return true;
	}

	return false;
}

//转换数值
LONG CYSIniData::SwitchStringToValue(LPCTSTR & pszSring)
{
	//效验参数
	ASSERT((pszSring!=NULL)&&(pszSring[0]!=0));
	if ((pszSring==NULL)||(pszSring[0]==0)) return 0L;

	//寻找开始
	while (((pszSring[0]>0)&&(pszSring[0]<TEXT('0')))||(pszSring[0]>TEXT('9'))) pszSring++;

	//读取数值
	LONG lValue=0L;
	while ((pszSring[0]>=TEXT('0'))&&(pszSring[0]<=TEXT('9')))
	{
		lValue=lValue*10L+pszSring[0]-TEXT('0');
		++pszSring;
	}

	return lValue;
}

//////////////////////////////////////////////////////////////////////////////////
