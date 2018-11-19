#include "StdAfx.h"
#include "YSRegKey.h"
#include "YSEncrypt.h"

//////////////////////////////////////////////////////////////////////////////////
//��̬����

TCHAR		CYSRegKey::m_szMainKeyName[64]=TEXT("");					//��������

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CYSRegKey::CYSRegKey()
{
	//�������
	m_hRegKey=NULL;

	//��������
	if (m_szMainKeyName[0]==0)
	{
		CWinApp * pWinApp=AfxGetApp();
		lstrcpyn(m_szMainKeyName,pWinApp->m_pszAppName,CountArray(m_szMainKeyName));
	}

	return;
}

//��������
CYSRegKey::~CYSRegKey()
{
	//�رռ���
	if (m_hRegKey!=NULL) CloseRegKey();

	return;
}

//��������
VOID CYSRegKey::SetMainKeyName(LPCTSTR pszMainKeyName)
{
	//���ñ���
	ASSERT(pszMainKeyName!=NULL);
	lstrcpyn(m_szMainKeyName,pszMainKeyName,CountArray(m_szMainKeyName));

	return;
}

//�Ӵ���
HKEY CYSRegKey::Detach()
{
	//�������
	HKEY hRegKey=m_hRegKey;

	//���ñ���
	m_hRegKey=NULL;

	return hRegKey;
}

//�󶨼���
HKEY CYSRegKey::Attach(HKEY hRegKey)
{
	//Ч�����
	ASSERT(hRegKey!=NULL);

	//���ñ���
	m_hRegKey=hRegKey;

	return m_hRegKey;
}

//�رռ���
bool CYSRegKey::CloseRegKey()
{
	//�رռ���
	RegCloseKey(m_hRegKey);

	//���ñ���
	m_hRegKey=NULL;

	return true;
}

//�򿪼���
bool CYSRegKey::OpenRegKey(LPCTSTR pszKeyName, bool bCreate)
{
	//Ч�����
	ASSERT(pszKeyName!=NULL);

	//�رռ���
	if (m_hRegKey!=NULL) RegCloseKey(m_hRegKey);

	//���ɼ���
	TCHAR szFullKeyName[256]=TEXT("");
	_sntprintf_s(szFullKeyName, CountArray(szFullKeyName), TEXT("Software\\%s\\%s\\%s"), szProductKey, m_szMainKeyName, pszKeyName);

	//�򿪼���
	m_hRegKey=NULL;
	LONG lResult=RegOpenKey(HKEY_CURRENT_USER,szFullKeyName,&m_hRegKey);
	if ((lResult!=ERROR_SUCCESS)&&(bCreate==true)) lResult=RegCreateKey(HKEY_CURRENT_USER,szFullKeyName,&m_hRegKey);

	return (lResult==ERROR_SUCCESS);
}

//�򿪼���
bool CYSRegKey::OpenRegKey(LPCTSTR pszMainKeyName, LPCTSTR pszKeyName, bool bCreate)
{
	//Ч�����
	ASSERT(pszKeyName!=NULL);
	ASSERT(pszMainKeyName!=NULL);

	//�رռ���
	if (m_hRegKey!=NULL) RegCloseKey(m_hRegKey);

	//���ɼ���
	TCHAR szFullKeyName[256]=TEXT("");
	_sntprintf_s(szFullKeyName,CountArray(szFullKeyName),TEXT("Software\\%s\\%s\\%s"),szProductKey,pszMainKeyName,pszKeyName);

	//�򿪼���
	m_hRegKey=NULL;
	LONG lResult=RegOpenKey(HKEY_CURRENT_USER,szFullKeyName,&m_hRegKey);
	if ((lResult!=ERROR_SUCCESS)&&(bCreate==true)) lResult=RegCreateKey(HKEY_CURRENT_USER,szFullKeyName,&m_hRegKey);

	return (lResult==ERROR_SUCCESS);
}

//�򿪼���
HKEY CYSRegKey::OpenItemKey(LPCTSTR pszSubKeyName)
{
	//Ч�����
	ASSERT(m_hRegKey!=NULL);
	ASSERT(pszSubKeyName!=NULL);

	//�򿪼���
	HKEY hRegKey=NULL;
	RegOpenKey(m_hRegKey,pszSubKeyName,&hRegKey);

	return hRegKey;
}

//��������
HKEY CYSRegKey::CreateItemKey(LPCTSTR pszSubKeyName)
{
	//Ч�����
	ASSERT(m_hRegKey!=NULL);
	ASSERT(pszSubKeyName!=NULL);

	//��������
	HKEY hRegKey=NULL;
	RegCreateKey(m_hRegKey,pszSubKeyName,&hRegKey);

	return hRegKey;
}

//ö�ټ���
bool CYSRegKey::EnumItemString(DWORD dwIndex, LPTSTR pszString, DWORD dwSize)
{
	//Ч�����
	ASSERT(m_hRegKey!=NULL);

	//���ñ���
	pszString[0]=0;

	//ö�ټ���
	LONG lResult=RegEnumKey(m_hRegKey,dwIndex,pszString,dwSize);

	return (lResult==ERROR_SUCCESS);
}

//д����ֵ
bool CYSRegKey::WriteValue(LPCTSTR pszName, DWORD dwValue)
{
	//Ч�����
	ASSERT(m_hRegKey!=NULL);

	//д����ֵ
	LONG lResult=RegSetValueEx(m_hRegKey,pszName,0,REG_DWORD,(LPBYTE)(&dwValue),sizeof(DWORD));

	return (lResult==ERROR_SUCCESS);
}

//д���ַ�
bool CYSRegKey::WriteString(LPCTSTR pszName, LPCTSTR pszString)
{
	//Ч�����
	ASSERT(m_hRegKey!=NULL);

	//д����ֵ
	DWORD dwDataSize=CountStringBuffer(pszString);
	LONG lResult=RegSetValueEx(m_hRegKey,pszName,0,REG_SZ,(LPBYTE)pszString,dwDataSize);

	return (lResult==ERROR_SUCCESS);
}

//д���ַ�
bool CYSRegKey::WriteEncryptString(LPCTSTR pszName, LPCTSTR pszString)
{
	//��������
	TCHAR szRegString[MAX_ENCRYPT_LEN+1]=TEXT("");
	CYSEncrypt::XorEncrypt(pszString,szRegString,CountArray(szRegString));

	//д���ַ�
	return WriteString(pszName,szRegString);
}

//д������
bool CYSRegKey::WriteBinary(LPCTSTR pszName, const VOID * pData, DWORD dwSize)
{
	//Ч�����
	ASSERT(m_hRegKey!=NULL);

	//д����ֵ
	LONG lResult=RegSetValueEx(m_hRegKey,pszName,0,REG_BINARY,(LPBYTE)(pData),dwSize);

	return (lResult==ERROR_SUCCESS);
}

//��ȡ��ֵ
DWORD CYSRegKey::GetValue(LPCTSTR pszName, DWORD dwDefValue)
{
	//Ч�����
	ASSERT(m_hRegKey!=NULL);

	//��ȡ��ֵ
	DWORD dwType=REG_NONE,dwResultSize=sizeof(DWORD),dwValue=0;
	LONG lResult=RegQueryValueEx(m_hRegKey,pszName,0,&dwType,(BYTE *)(&dwValue),&dwResultSize);
	if ((lResult!=ERROR_SUCCESS)||(dwType!=REG_DWORD)) return dwDefValue;

	return dwValue;
}

//��ȡ����
DWORD CYSRegKey::GetBinary(LPCTSTR pszName, VOID * pData, DWORD dwBufferSize)
{
	//Ч�����
	ASSERT(pData!=NULL);
	ASSERT(m_hRegKey!=NULL);

	//��ȡ��ֵ
	DWORD dwType=REG_NONE,dwResultSize=dwBufferSize;
	LONG lResult=RegQueryValueEx(m_hRegKey,pszName,0,&dwType,(BYTE *)(pData),&dwResultSize);
	if ((lResult!=ERROR_SUCCESS)||(dwType!=REG_BINARY)) return 0;

	return dwResultSize;
}

//��ȡ�ַ�
DWORD CYSRegKey::GetString(LPCTSTR pszName, LPTSTR pszString, UINT uMaxCount)
{
	//Ч�����
	ASSERT(pszString!=NULL);
	ASSERT(m_hRegKey!=NULL);

	//��ȡ��ֵ
	DWORD dwType=REG_NONE,dwResultSize=uMaxCount*sizeof(TCHAR);
	LONG lResult=RegQueryValueEx(m_hRegKey,pszName,0,&dwType,(BYTE *)(pszString),&dwResultSize);
	if ((lResult!=ERROR_SUCCESS)||(dwType!=REG_SZ)) return 0;

	return dwResultSize/sizeof(TCHAR);
}

//��ȡ�ַ�
DWORD CYSRegKey::GetEncryptString(LPCTSTR pszName, LPTSTR pszString, UINT uMaxCount)
{
	//Ч�����
	ASSERT(pszString!=NULL);
	ASSERT(m_hRegKey!=NULL);

	//��ȡ�ַ�
	TCHAR szRegString[MAX_ENCRYPT_LEN+1]=TEXT("");
	GetString(pszName,szRegString,CountArray(szRegString));

	//��������
	CYSEncrypt::XorCrevasse(szRegString,pszString,uMaxCount);

	return lstrlen(pszString);
}

//ɾ����ֵ
bool CYSRegKey::DeleteValue(LPCTSTR pszName)
{
	//Ч�����
	ASSERT(m_hRegKey!=NULL);

	//ɾ����ֵ
	LONG lResult=RegDeleteValue(m_hRegKey,pszName);

	return (lResult==ERROR_SUCCESS);
}

//ɾ������
bool CYSRegKey::RecurseDeleteKey(LPCTSTR pszSubKeyName)
{
	//Ч�����
	ASSERT(m_hRegKey!=NULL);
	ASSERT(pszSubKeyName!=NULL);

	//ɾ������
	LONG lResult=RecurseDeleteKey(m_hRegKey,pszSubKeyName);

	return (lResult==ERROR_SUCCESS);
}

//ɾ������
LONG CYSRegKey::RecurseDeleteKey(HKEY hParentKey, LPCTSTR pszSubKeyName)
{
	//Ч�����
	ASSERT(hParentKey!=NULL);
	ASSERT(pszSubKeyName!=NULL);

	//�򿪼���
	HKEY hSubKey=NULL;
	LONG lResult=RegOpenKey(hParentKey,pszSubKeyName,&hSubKey);
	if (lResult!=ERROR_SUCCESS) return lResult;

	//�ݹ�ɾ��
	TCHAR szKeyName[256];
	while (RegEnumKey(hSubKey,0,szKeyName,sizeof(szKeyName))==ERROR_SUCCESS)
	{
		//ɾ������
		lResult=RecurseDeleteKey(hSubKey,szKeyName);
		if (lResult!=ERROR_SUCCESS)
		{
			RegCloseKey(hSubKey);
			return lResult;
		}
	}

	//�رռ���
	RegCloseKey(hSubKey);

	//ɾ������
	lResult=RegDeleteKey(hParentKey,pszSubKeyName);

	return lResult;
}

//////////////////////////////////////////////////////////////////////////////////
