#include "StdAfx.h"
#include "InitParameter.h"

//////////////////////////////////////////////////////////////////////////////////

//ʱ�䶨��
#define TIME_CONNECT				30									//����ʱ��
#define TIME_COLLECT				30									//ͳ��ʱ��

//�ͻ�ʱ��
#define TIME_INTERMIT				0									//�ж�ʱ��
#define TIME_ONLINE_COUNT			600									//����ʱ��

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
	//ʱ�䶨��
	m_wConnectTime=TIME_CONNECT;
	m_wCollectTime=TIME_COLLECT;

	//Э����Ϣ
	m_wCorrespondPort=PORT_CENTER;
	ZeroMemory(&m_CorrespondAddress,sizeof(m_CorrespondAddress));

	//������Ϣ
	ZeroMemory(&m_ServiceAddress,sizeof(m_ServiceAddress));
	ZeroMemory(&m_TreasureDBParameter,sizeof(m_TreasureDBParameter));
	ZeroMemory(&m_PlatformDBParameter,sizeof(m_PlatformDBParameter));
	ZeroMemory(&m_BenefitInfo, sizeof(m_BenefitInfo));

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
	IniData.ReadString(TEXT("ServerInfo"),TEXT("ServiceAddr"),NULL,m_ServiceAddress.szAddress,CountArray(m_ServiceAddress.szAddress));

	//Э����Ϣ
	m_wCorrespondPort=IniData.ReadInt(TEXT("Correspond"),TEXT("ServicePort"),m_wCorrespondPort);
	IniData.ReadString(TEXT("ServerInfo"),TEXT("CorrespondAddr"),NULL,m_CorrespondAddress.szAddress,CountArray(m_CorrespondAddress.szAddress));

	//ˢ����������ʱ��
	m_dwRefrashOnlineUserTime = IniData.ReadInt(TEXT("ServerInfo"), TEXT("RefreshOnlineUserTime"), m_dwRefrashOnlineUserTime);
	m_dwRefrashOnlineUserTime = __max(m_dwRefrashOnlineUserTime, 10);

	//������Ϣ
	m_TreasureDBParameter.wDataBasePort=(WORD)IniData.ReadInt(TEXT("TreasureDB"),TEXT("DBPort"),3306);
	IniData.ReadString(TEXT("TreasureDB"),TEXT("DBAddr"),NULL,m_TreasureDBParameter.szDataBaseAddr,CountArray(m_TreasureDBParameter.szDataBaseAddr));
	IniData.ReadString(TEXT("TreasureDB"),TEXT("DBUser"),NULL,m_TreasureDBParameter.szDataBaseUser,CountArray(m_TreasureDBParameter.szDataBaseUser));
	IniData.ReadString(TEXT("TreasureDB"),TEXT("DBPass"),NULL,m_TreasureDBParameter.szDataBasePass,CountArray(m_TreasureDBParameter.szDataBasePass));
	IniData.ReadString(TEXT("TreasureDB"),TEXT("DBName"),szTreasureDB,m_TreasureDBParameter.szDataBaseName,CountArray(m_TreasureDBParameter.szDataBaseName));

	//������Ϣ
	m_PlatformDBParameter.wDataBasePort = (WORD)IniData.ReadInt(TEXT("PlatformDB"), TEXT("DBPort"), 3306);
	IniData.ReadString(TEXT("PlatformDB"),TEXT("DBAddr"),NULL,m_PlatformDBParameter.szDataBaseAddr,CountArray(m_PlatformDBParameter.szDataBaseAddr));
	IniData.ReadString(TEXT("PlatformDB"),TEXT("DBUser"),NULL,m_PlatformDBParameter.szDataBaseUser,CountArray(m_PlatformDBParameter.szDataBaseUser));
	IniData.ReadString(TEXT("PlatformDB"),TEXT("DBPass"),NULL,m_PlatformDBParameter.szDataBasePass,CountArray(m_PlatformDBParameter.szDataBasePass));
	IniData.ReadString(TEXT("PlatformDB"),TEXT("DBName"),szPlatformDB,m_PlatformDBParameter.szDataBaseName,CountArray(m_PlatformDBParameter.szDataBaseName));

	//������Ϣ
	m_AccountsDBParameter.wDataBasePort = (WORD)IniData.ReadInt(TEXT("AccountsDB"), TEXT("DBPort"), 3306);
	IniData.ReadString(TEXT("AccountsDB"), TEXT("DBAddr"), NULL, m_AccountsDBParameter.szDataBaseAddr, CountArray(m_AccountsDBParameter.szDataBaseAddr));
	IniData.ReadString(TEXT("AccountsDB"), TEXT("DBUser"), NULL, m_AccountsDBParameter.szDataBaseUser, CountArray(m_AccountsDBParameter.szDataBaseUser));
	IniData.ReadString(TEXT("AccountsDB"), TEXT("DBPass"), NULL, m_AccountsDBParameter.szDataBasePass, CountArray(m_AccountsDBParameter.szDataBasePass));
	IniData.ReadString(TEXT("AccountsDB"), TEXT("DBName"), szAccountsDB, m_AccountsDBParameter.szDataBaseName, CountArray(m_AccountsDBParameter.szDataBaseName));

	//�ȼý�
	m_BenefitInfo.lGoldLimit = (SCORE)IniData.ReadInt(TEXT("Benefit"), TEXT("GoldLimit"), 0);
	m_BenefitInfo.cbReceiveTimes = (BYTE)IniData.ReadInt(TEXT("Benefit"), TEXT("ReceiveTimes"), 0);
	m_BenefitInfo.wTimeIntervals = (WORD)IniData.ReadInt(TEXT("Benefit"), TEXT("TimeIntervals"), 0);
	//m_BenefitInfo.RewardPropID = (WORD)IniData.ReadInt(TEXT("Benefit"), TEXT("RewardPropID"), 0);
	IniData.ReadString(TEXT("Benefit"), TEXT("RewardProp"), TEXT("1*10000"), m_BenefitInfo.szRewardProp, CountArray(m_BenefitInfo.szRewardProp));
	IniData.ReadString(TEXT("Benefit"), TEXT("PromptMsg"), TEXT("�ȼý��ѷ��������䣬����ǰȥ��ȡ��"), m_BenefitInfo.szPromptMsg, CountArray(m_BenefitInfo.szPromptMsg));
	IniData.ReadString(TEXT("Benefit"), TEXT("EmailTitle"), TEXT("�ȼý�ϵͳ"), m_BenefitInfo.szEmailTitle, CountArray(m_BenefitInfo.szEmailTitle));
	IniData.ReadString(TEXT("Benefit"), TEXT("EmailMsg"), TEXT("����%s����ȼý�ϵͳ��ϵͳͨ���ʼ����������;ȼý𣬽��Ϊ%d��ң��뾡�����!"), m_BenefitInfo.szEmailMsg, CountArray(m_BenefitInfo.szEmailMsg));

	return;
}

//////////////////////////////////////////////////////////////////////////////////
