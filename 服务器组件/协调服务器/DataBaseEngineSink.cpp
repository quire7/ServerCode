#include "StdAfx.h"
#include "ServiceUnits.h"
#include "DataBaseEngineSink.h"
#include <sstream>

#define TYPE_Loveliness_Ranking		0
#define TYPE_Treasure_Ranking		1

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDataBaseEngineSink::CDataBaseEngineSink()
{
	//�������
	m_pInitParameter=NULL;
	m_pIDataBaseEngineEvent=NULL;

	return;
}

//��������
CDataBaseEngineSink::~CDataBaseEngineSink()
{
}

//�ӿڲ�ѯ
VOID * CDataBaseEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBaseEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseEngineSink,Guid,dwQueryVer);
	return NULL;
}

//�����¼�
bool CDataBaseEngineSink::OnDataBaseEngineStart(IUnknownEx * pIUnknownEx)
{
	//��������
	if ((m_AccountsDBModule.GetInterface()==NULL)&&(m_AccountsDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	if ((m_TreasureDBModule.GetInterface()==NULL)&&(m_TreasureDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	if ((m_PlatformDBModule.GetInterface()==NULL)&&(m_PlatformDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	try
	{
		//������Ϣ
		tagDataBaseParameter * pAccountsDBParameter=&m_pInitParameter->m_AccountsDBParameter;
		tagDataBaseParameter * pTreasureDBParameter=&m_pInitParameter->m_TreasureDBParameter;
		tagDataBaseParameter * pPlatformDBParameter=&m_pInitParameter->m_PlatformDBParameter;

		//��������
		m_AccountsDBModule->SetDBInfo(pAccountsDBParameter->szDataBaseAddr, pAccountsDBParameter->wDataBasePort,
			pAccountsDBParameter->szDataBaseName,pAccountsDBParameter->szDataBaseUser,pAccountsDBParameter->szDataBasePass);
		m_TreasureDBModule->SetDBInfo(pTreasureDBParameter->szDataBaseAddr, pTreasureDBParameter->wDataBasePort,
			pTreasureDBParameter->szDataBaseName,pTreasureDBParameter->szDataBaseUser,pTreasureDBParameter->szDataBasePass);
		m_PlatformDBModule->SetDBInfo(pPlatformDBParameter->szDataBaseAddr, pPlatformDBParameter->wDataBasePort,
			pPlatformDBParameter->szDataBaseName,pPlatformDBParameter->szDataBaseUser,pPlatformDBParameter->szDataBasePass);

		//��������
		m_AccountsDBModule->Open();

		//��������
		m_TreasureDBModule->Open();

		//��������
		m_PlatformDBModule->Open();

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		return false;
	}

	return true;
}

//ֹͣ�¼�
bool CDataBaseEngineSink::OnDataBaseEngineConclude(IUnknownEx * pIUnknownEx)
{
	//�ر�����
	if (m_AccountsDBModule.GetInterface()!=NULL)
	{
		m_AccountsDBModule->Close();
		m_AccountsDBModule.CloseInstance();
	}

	//�ر�����
	if (m_TreasureDBModule.GetInterface()!=NULL)
	{
		m_TreasureDBModule->Close();
		m_TreasureDBModule.CloseInstance();
	}

	//�ر�����
	if (m_PlatformDBModule.GetInterface()!=NULL)
	{
		m_PlatformDBModule->Close();
		m_PlatformDBModule.CloseInstance();
	}

	//�������
	m_pInitParameter=NULL;
	m_pIDataBaseEngineEvent=NULL;

	return true;
}

//ʱ���¼�
bool CDataBaseEngineSink::OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	return false;
}

//�����¼�
bool CDataBaseEngineSink::OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize)
{
	return false;
}

//�����¼�
bool CDataBaseEngineSink::OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
	case DBR_GP_LoveLinessRanking:
	{
		return OnRequestLoveLinessRanking(dwContextID, pData, wDataSize);
	}
	case DBR_GP_TreasureRanking:
	{
		return OnRequestTreasureRanking(dwContextID, pData, wDataSize);
	}
	}

	return false;
}

//�������а佱
bool CDataBaseEngineSink::OnRequestLoveLinessRanking(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	return RankingReward(TYPE_Loveliness_Ranking);
}

//�Ƹ����а佱
bool CDataBaseEngineSink::OnRequestTreasureRanking(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	return RankingReward(TYPE_Treasure_Ranking);
}

bool CDataBaseEngineSink::RankingReward(BYTE cbType)
{
	//��ȡ���н������ò�����MAP
	stringstream ss;
	//�˴�cbType����תΪint����
	ss << "select RankingID,PropList,PropListDesc from rankingrewardconfig where TypeID=" << (int)cbType;
	BOOL bRet = m_AccountsDBModule->ExecuteSQL(ss.str());
	ASSERT(bRet);
	SQLRecordset rs(m_AccountsDBModule);

	map<WORD, tagRankingRewardConfig>	mapRewardConfig;
	if (rs.GetCount() > 0)
	{
		//��ȡ��¼
		do
		{
			WORD wRankingID = rs.GetValue_WORD("RankingID");
			tagRankingRewardConfig config;
			ZeroMemory(&config, sizeof(tagRankingRewardConfig));
			config.wRankingID = wRankingID;
			rs.GetValue_String("PropList", config.szPropList, sizeof(config.szPropList));
			rs.GetValue_String("PropListDesc", config.szPropListDesc, sizeof(config.szPropListDesc));
			//mapRewardConfig[wID] = config;
			mapRewardConfig.insert(pair<WORD, tagRankingRewardConfig>(wRankingID, config));
		} while (rs.Fetch());
	}
	else
	{
		ASSERT(FALSE);
		return false;
	}

	//��ȡ�����Ƿ�䷢����Ʒ
	ss.str("");
	ss << "select count(*) as cnt from rankingreward where TypeID=" << (int)cbType << " and CreateDate=DATE_SUB(curdate(), INTERVAL 1 DAY)";
	bRet = m_AccountsDBModule->ExecuteSQL(ss.str());
	ASSERT(bRet);
	rs.Initial(m_AccountsDBModule->GetMYSQL());
	rs.Fetch();

	if (rs.GetCount() > 0 && rs.GetValue_INT("cnt")>0 )
	{
		//ASSERT(FALSE);
		return false;
	}

	//��ȡ�������ID������MAP
	ss.str("");
	ss << "select UserID from accountsinfo order by ";
	if (cbType == TYPE_Loveliness_Ranking)
	{
		ss << "LoveLiness";
	}
	else if (cbType == TYPE_Treasure_Ranking)
	{
		ss << "Gold";
	}
	ss << " DESC limit " << mapRewardConfig.size();

	bRet = m_AccountsDBModule->ExecuteSQL(ss.str());
	ASSERT(bRet);
	rs.Initial(m_AccountsDBModule->GetMYSQL());

	map<int, DWORD> mapUser;
	if (rs.GetCount() > 0)
	{
		int i = 1;
		rs.Fetch();
		//��ȡ��¼
		do
		{
			mapUser[i++] = rs.GetValue_DWORD("UserID");
		} while (rs.Fetch());
	}

	//�������ͻ�ȡ�ʼ�����
	tagRankingRewardEmailConfig* pEmailConfig = NULL;
	if (cbType == TYPE_Loveliness_Ranking)
	{
		pEmailConfig = &m_pInitParameter->m_LoveLinessConfig;
	}
	else if (cbType == TYPE_Treasure_Ranking)
	{
		pEmailConfig = &m_pInitParameter->m_TreasureConfig;
	}

#if STOR_UTF8
	CStringA strEmailMsg = pEmailConfig->szEmailMsg;
	CStringA strEmailTitle = CYSCharUtils::ConvertToUTF8(pEmailConfig->szEmailTitle);
#else
	CStringA strEmailMsg = CYSCharUtils::ConvertToANSI(pEmailConfig->szEmailMsg);
	CStringA strEmailTitle = CYSCharUtils::ConvertToANSI(pEmailConfig->szEmailTitle);
#endif
	char* pStrEmailMsg = strEmailMsg.GetBuffer();
	char* pStrEmailTitle = strEmailTitle.GetBuffer();

	//�����ʼ��Ͱ佱��¼
	map<int, DWORD>::iterator it;
	for (it = mapUser.begin(); it != mapUser.end(); ++it)
	{
		//"key: " << it->first, " value: " << it->second
		ss.str("");

		int nRankingID = it->first;
		DWORD dwUserID = it->second;
		tagRankingRewardConfig* pRewardConfig = &mapRewardConfig[nRankingID];

		char szMsg[256] = { 0 };
		_snprintf_s(szMsg, sizeof(szMsg), pStrEmailMsg, nRankingID, pRewardConfig->szPropListDesc);

		//�����ʼ�;
#if STOR_UTF8
		ss << "INSERT INTO notices(toUserID, title, content, attachment, type, created_at) VALUES(";
		ss << dwUserID << ",\"" << pStrEmailTitle << "\",\"" << CYSCharUtils::ConvertToUTF8(szMsg) << "\",\"";
		ss << CYSCharUtils::ConvertToUTF8(pRewardConfig->szPropList) << "\"," << EN_EMAIL_SYSTIME << "," << "NOW())";
#else
		ss << "INSERT INTO notices(toUserID, title, content, attachment, type, created_at) VALUES(";
		ss << dwUserID << ",\"" << pStrEmailTitle << "\",\"" << szMsg << "\",\"";
		ss << pRewardConfig->szPropList << "\"," << EN_EMAIL_SYSTIME << "," << "NOW())";
#endif

		bRet = m_AccountsDBModule->ExecuteSQL(ss.str());
		ASSERT(bRet);

		ss.str("");
		ss << "insert into rankingreward(UserID,Ranking,CreateDate,TypeID) values (";
		ss << dwUserID << "," << nRankingID << "," << "DATE_SUB(curdate(), INTERVAL 1 DAY)," << (int)cbType << ")";

		bRet = m_AccountsDBModule->ExecuteSQL(ss.str());
		ASSERT(bRet);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
