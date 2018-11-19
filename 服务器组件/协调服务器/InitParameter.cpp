#include "StdAfx.h"
#include "InitParameter.h"

//////////////////////////////////////////////////////////////////////////////////

#define REG_ITEM_NAME				TEXT("Information")					//ע�����

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CInitParameter::CInitParameter() 
{ 
	InitParameter();
}

//��������
CInitParameter::~CInitParameter()
{
}

//��ʼ��
VOID CInitParameter::InitParameter()
{
	//ϵͳ����
	m_wMaxConnect=MAX_CONTENT;
	m_wServicePort=PORT_CENTER;

	//������Ϣ
	ZeroMemory(&m_AccountsDBParameter, sizeof(m_AccountsDBParameter));
	ZeroMemory(&m_TreasureDBParameter, sizeof(m_TreasureDBParameter));
	ZeroMemory(&m_PlatformDBParameter, sizeof(m_PlatformDBParameter));

	ZeroMemory(&m_LoveLinessConfig, sizeof(m_LoveLinessConfig));
	ZeroMemory(&m_TreasureConfig, sizeof(m_TreasureConfig));

	return;
}

//��������
VOID CInitParameter::LoadInitParameter()
{
	//���ò���
	InitParameter();

	//��ȡ·��
	TCHAR szWorkDir[MAX_PATH]=TEXT("");
	CYSService::GetWorkDirectory(szWorkDir,CountArray(szWorkDir));

	//����·��
	TCHAR szIniFile[MAX_PATH]=TEXT("");
	_sntprintf_s(szIniFile,CountArray(szIniFile),TEXT("%s\\ServerParameter.ini"),szWorkDir);

	//��ȡ����
	CYSIniData IniData;
	IniData.SetIniFilePath(szIniFile);

	//��ȡ����
	m_wMaxConnect=IniData.ReadInt(TEXT("Correspond"),TEXT("ConnectMax"),m_wMaxConnect);
	m_wServicePort=IniData.ReadInt(TEXT("Correspond"),TEXT("ServicePort"),m_wServicePort);

	//������Ϣ
	m_AccountsDBParameter.wDataBasePort = (WORD)IniData.ReadInt(TEXT("AccountsDB"), TEXT("DBPort"), 3306);
	IniData.ReadString(TEXT("AccountsDB"), TEXT("DBAddr"), NULL, m_AccountsDBParameter.szDataBaseAddr, CountArray(m_AccountsDBParameter.szDataBaseAddr));
	IniData.ReadString(TEXT("AccountsDB"), TEXT("DBUser"), NULL, m_AccountsDBParameter.szDataBaseUser, CountArray(m_AccountsDBParameter.szDataBaseUser));
	IniData.ReadString(TEXT("AccountsDB"), TEXT("DBPass"), NULL, m_AccountsDBParameter.szDataBasePass, CountArray(m_AccountsDBParameter.szDataBasePass));
	IniData.ReadString(TEXT("AccountsDB"), TEXT("DBName"), szAccountsDB, m_AccountsDBParameter.szDataBaseName, CountArray(m_AccountsDBParameter.szDataBaseName));

	//������Ϣ
	m_TreasureDBParameter.wDataBasePort = (WORD)IniData.ReadInt(TEXT("TreasureDB"), TEXT("DBPort"), 3306);
	IniData.ReadString(TEXT("TreasureDB"), TEXT("DBAddr"), NULL, m_TreasureDBParameter.szDataBaseAddr, CountArray(m_TreasureDBParameter.szDataBaseAddr));
	IniData.ReadString(TEXT("TreasureDB"), TEXT("DBUser"), NULL, m_TreasureDBParameter.szDataBaseUser, CountArray(m_TreasureDBParameter.szDataBaseUser));
	IniData.ReadString(TEXT("TreasureDB"), TEXT("DBPass"), NULL, m_TreasureDBParameter.szDataBasePass, CountArray(m_TreasureDBParameter.szDataBasePass));
	IniData.ReadString(TEXT("TreasureDB"), TEXT("DBName"), szTreasureDB, m_TreasureDBParameter.szDataBaseName, CountArray(m_TreasureDBParameter.szDataBaseName));

	//������Ϣ
	m_PlatformDBParameter.wDataBasePort = (WORD)IniData.ReadInt(TEXT("PlatformDB"), TEXT("DBPort"), 3306);
	IniData.ReadString(TEXT("PlatformDB"), TEXT("DBAddr"), NULL, m_PlatformDBParameter.szDataBaseAddr, CountArray(m_PlatformDBParameter.szDataBaseAddr));
	IniData.ReadString(TEXT("PlatformDB"), TEXT("DBUser"), NULL, m_PlatformDBParameter.szDataBaseUser, CountArray(m_PlatformDBParameter.szDataBaseUser));
	IniData.ReadString(TEXT("PlatformDB"), TEXT("DBPass"), NULL, m_PlatformDBParameter.szDataBasePass, CountArray(m_PlatformDBParameter.szDataBasePass));
	IniData.ReadString(TEXT("PlatformDB"), TEXT("DBName"), szPlatformDB, m_PlatformDBParameter.szDataBaseName, CountArray(m_PlatformDBParameter.szDataBaseName));

	IniData.ReadString(TEXT("LoveLinessRanking"), TEXT("EmailTitle"), TEXT("�������а���"), m_LoveLinessConfig.szEmailTitle, CountArray(m_LoveLinessConfig.szEmailTitle));
	IniData.ReadString(TEXT("LoveLinessRanking"), TEXT("EmailMsg"), TEXT("��ϲ�����������а��л��%d���Σ����%s�������뾡����գ�"), m_LoveLinessConfig.szEmailMsg, CountArray(m_LoveLinessConfig.szEmailMsg));

	IniData.ReadString(TEXT("TreasureRanking"), TEXT("EmailTitle"), TEXT("�Ƹ����а���"), m_TreasureConfig.szEmailTitle, CountArray(m_TreasureConfig.szEmailTitle));
	IniData.ReadString(TEXT("TreasureRanking"), TEXT("EmailMsg"), TEXT("��ϲ���ڲƸ����а��л��%d���Σ����%s�������뾡����գ�"), m_TreasureConfig.szEmailMsg, CountArray(m_TreasureConfig.szEmailMsg));

	return;
}

//��������
void CInitParameter::SaveInitParameter()
{
	return;
}

//////////////////////////////////////////////////////////////////////////////////
