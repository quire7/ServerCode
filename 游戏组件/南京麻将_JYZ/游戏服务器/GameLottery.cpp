#include "StdAfx.h"
#include "GameLottery.h"
///////////////////////////////////////////////////////////////////////////////////
CGameLottery  *CGameLottery::m_spGameLottery = NULL;
//////////////////////////////////////////////////////////////////////////////////
//���캯��
CGameLottery::CGameLottery(void)
{
	m_lPrizePoolScore = 0;
	ZeroMemory(&m_stGameLottery, sizeof(m_stGameLottery));
}

//��������
CGameLottery::~CGameLottery(void)
{
}

//���ٳ齱ʵ��
void CGameLottery::Release()
{
	SafeDelete(m_spGameLottery);
}

//��ȡ�齱ʵ��
CGameLottery *CGameLottery::GetInstance()
{
	if (NULL == m_spGameLottery)
	{
		m_spGameLottery = new CGameLottery();
	}
	return m_spGameLottery;
}

//��ȡ�齱���ò���
bool CGameLottery::ReadGameLotteryConfig()
{
	////��ȡ�齱����
	//TCHAR szFileName[MAX_PATH] = TEXT("");
	//CSHService::GetWorkDirectory(szFileName, CountString(szFileName));	
	//StringCchCat(szFileName, CountString(szFileName), TEXT("\\NJMJLottery.xml"));
 //	if (!::PathFileExists(szFileName))
 //	{
 //		ASSERT(FALSE);
	//	return false;
 //	}

	//WHXmlDocument *pXmlDoc = NULL;
	//pXmlDoc = new WHXmlDocument();
	//if (!pXmlDoc->LoadFile(szFileName))
	//{
	//	ASSERT(FALSE);
	//	SafeDelete(pXmlDoc);
	//	return false;
	//}

	//WHXMLElement *pRootElement = pXmlDoc->FirstChildElement(_T("GameLottery"));
	//if (pRootElement == NULL)
	//{
	//	ASSERT(FALSE);
	//	SafeDelete(pXmlDoc);
	//	return false;
	//}

	////���ؽ���ռ�齱����
	//WHXMLElement *pPrizePool = pRootElement->FirstChildElement(_T("PrizePool"));
	//if (pPrizePool==NULL || pPrizePool->Attribute(_T("PoolRate"), &m_stGameLottery.fPoolRate)==NULL)
	//{
	//	ASSERT(FALSE);
	//	SafeDelete(pXmlDoc);
	//	return false;
	//}
	////���س齱����
	//WHXMLElement *pLotteryType = pRootElement->FirstChildElement(_T("LotteryType"));
	//if (pLotteryType == NULL)
	//{
	//	ASSERT(FALSE);
	//	SafeDelete(pXmlDoc);
	//	return false;
	//}
	//WHXMLElement *pKey = pLotteryType->FirstChildElement(_T("Key"));
	//while (pKey != NULL)
	//{
	//	int nID = 0;
	//	if (pKey->Attribute(_T("ID"), &nID) == NULL)
	//	{
	//		ASSERT(FALSE);
	//		SafeDelete(pXmlDoc);
	//		return false;
	//	}
	//	if (pKey->Attribute(_T("MinPoolScore"), &m_stGameLottery.stLotteryType[nID].nMinPoolScore) == NULL)
	//	{
	//		ASSERT(FALSE);
	//		SafeDelete(pXmlDoc);
	//		return false;
	//	}
	//	if (pKey->Attribute(_T("MaxPoolScore"), &m_stGameLottery.stLotteryType[nID].nMaxPoolScore) == NULL)
	//	{
	//		ASSERT(FALSE);
	//		SafeDelete(pXmlDoc);
	//		return false;
	//	}
	//	if (pKey->Attribute(_T("MinJinBi"), &m_stGameLottery.stLotteryType[nID].nGetMinJinBi) == NULL)
	//	{
	//		ASSERT(FALSE);
	//		SafeDelete(pXmlDoc);
	//		return false;
	//	}
	//	if (pKey->Attribute(_T("MaxJinBi"), &m_stGameLottery.stLotteryType[nID].nGetMaxJinBi) == NULL)
	//	{
	//		ASSERT(FALSE);
	//		SafeDelete(pXmlDoc);
	//		return false;
	//	}
	//	if (pKey->Attribute(_T("MinJiangQuan"), &m_stGameLottery.stLotteryType[nID].nGetMinJiangQuan) == NULL)
	//	{
	//		ASSERT(FALSE);
	//		SafeDelete(pXmlDoc);
	//		return false;
	//	}
	//	if (pKey->Attribute(_T("MaxJiangQuan"), &m_stGameLottery.stLotteryType[nID].nGetMaxJiangQuan) == NULL)
	//	{
	//		ASSERT(FALSE);
	//		SafeDelete(pXmlDoc);
	//		return false;
	//	}
	//	if (pKey->Attribute(_T("MinSaiPai"), &m_stGameLottery.stLotteryType[nID].nGetMinSaiPai) == NULL)
	//	{
	//		ASSERT(FALSE);
	//		SafeDelete(pXmlDoc);
	//		return false;
	//	}
	//	if (pKey->Attribute(_T("MaxSaiPai"), &m_stGameLottery.stLotteryType[nID].nGetMaxSaiPai) == NULL)
	//	{
	//		ASSERT(FALSE);
	//		SafeDelete(pXmlDoc);
	//		return false;
	//	}
	//	pKey = pKey->NextSiblingElement(_T("Key"));
	//}
	////���ؽ�Ʒ����
	//WHXMLElement *pPrizeRate = pRootElement->FirstChildElement(_T("PrizeRate"));
	//if (pPrizeRate == NULL)
	//{
	//	ASSERT(FALSE);
	//	SafeDelete(pXmlDoc);
	//	return false;
	//}
	//while (pPrizeRate != NULL)
	//{
	//	int nID = 0;
	//	if (pPrizeRate->Attribute(_T("ID"), &nID) == NULL)
	//	{
	//		ASSERT(FALSE);
	//		SafeDelete(pXmlDoc);
	//		return false;
	//	}
	//	if (pPrizeRate->Attribute(_T("TotalRate"), &m_stGameLottery.stPrizeRate[nID].fTotalRate) == NULL)
	//	{
	//		ASSERT(FALSE);
	//		SafeDelete(pXmlDoc);
	//		return false;
	//	}
	//	if (pPrizeRate->Attribute(_T("ExchageRate"), &m_stGameLottery.stPrizeRate[nID].nExchangeRate) == NULL)
	//	{
	//		ASSERT(FALSE);
	//		SafeDelete(pXmlDoc);
	//		return false;
	//	}
	//	pKey = pPrizeRate->FirstChildElement(_T("Key"));
	//	if (pKey == NULL)
	//	{
	//		ASSERT(FALSE);
	//		SafeDelete(pXmlDoc);
	//		return false;
	//	}
	//	while(pKey != NULL)
	//	{
	//		int nSubID = 0;
	//		if (pKey->Attribute(_T("ID"), &nSubID) == NULL)
	//		{
	//			ASSERT(FALSE);
	//			SafeDelete(pXmlDoc);
	//			return false;
	//		}
	//		if (pKey->Attribute(_T("MinPrize"), &m_stGameLottery.stPrizeRate[nID].stSubPrizeRate[nSubID].nGetMinPrize) == NULL)
	//		{
	//			ASSERT(FALSE);
	//			SafeDelete(pXmlDoc);
	//			return false;
	//		}
	//		if (pKey->Attribute(_T("MaxPrize"), &m_stGameLottery.stPrizeRate[nID].stSubPrizeRate[nSubID].nGetMaxPrize) == NULL)
	//		{
	//			ASSERT(FALSE);
	//			SafeDelete(pXmlDoc);
	//			return false;
	//		}
	//		if (pKey->Attribute(_T("Rate"), &m_stGameLottery.stPrizeRate[nID].stSubPrizeRate[nSubID].fGetPrizeRate) == NULL)
	//		{
	//			ASSERT(FALSE);
	//			SafeDelete(pXmlDoc);
	//			return false;
	//		}
	//		pKey = pKey->NextSiblingElement(_T("Key"));
	//	}
	//	pPrizeRate = pPrizeRate->NextSiblingElement(_T("PrizeRate"));
	//}
 //	//�������轱Ʒ
 //	WHXMLElement *pDummyPrize = pRootElement->FirstChildElement(_T("DummyPrize"));
 //	if (pDummyPrize==NULL || pDummyPrize->Attribute(_T("Rate"), &m_nDummyPrizeRate)==NULL)
 //	{
 //		ASSERT(FALSE);
 //		SafeDelete(pXmlDoc);
 //		return false;
 //	}
 //	pKey = pDummyPrize->FirstChildElement(_T("Key"));
 //	if (pKey == NULL)
 //	{
 //		ASSERT(FALSE);
 //		SafeDelete(pXmlDoc);
 //		return false;
 //	}
 //	while(pKey != NULL)
 //	{
 //		int nID=0, nCount=0;
 //		if (pKey->Attribute(_T("ID"), &nID) == NULL)
 //		{
 //			ASSERT(FALSE);
 //			SafeDelete(pXmlDoc);
 //			return false;
 //		}
 //		if (pKey->Attribute(_T("Count"), &nCount) == NULL)
 //		{
 //			ASSERT(FALSE);
 //			SafeDelete(pXmlDoc);
 //			return false;
 //		}
 //		for (BYTE i=0; i<nCount; i++)
 //		{
 //			CString strNum;
 //			strNum.Format(_T("Number_%d"), i);
 //			int nPrizeNumber = 0;
 //			if (pKey->Attribute(strNum, &nPrizeNumber) == NULL)
 //			{
 //				ASSERT(FALSE);
 //				SafeDelete(pXmlDoc);
 //				return false;
 //			}
 //			tagLotteryPrizeResult stPrizeResult;
 //			stPrizeResult.cbPrizeType = nID;
 //			stPrizeResult.lPrizeCount = nPrizeNumber;
 //			m_vecDummyLotteryPrize.push_back(stPrizeResult);
 //		}
 //		pKey = pKey->NextSiblingElement(_T("Key"));
 //	}
	//SafeDelete(pXmlDoc); 
	return true;
}

//��ӽ�����Ŀ
void CGameLottery::AddScoreToPrizePool( LONG lScoreNumber )
{
	m_lPrizePoolScore += (LONG)(lScoreNumber*m_stGameLottery.fPoolRate);
}

//��ȡ��ҳ齱��Ʒ
void CGameLottery::OnUserLottery( BYTE &cbPrizeType, LONG &lPrizeCount )
{
	cbPrizeType = 0; 
	lPrizeCount = 0;
	ASSERT(m_lPrizePoolScore>=0);
	for (BYTE i=0; i<MAX_LOTTERY_TYPE; i++)
	{
		if (m_lPrizePoolScore>=m_stGameLottery.stLotteryType[i].nMinPoolScore && (m_lPrizePoolScore<=m_stGameLottery.stLotteryType[i].nMaxPoolScore||m_stGameLottery.stLotteryType[i].nMaxPoolScore==-1))
		{
			GetLotteryPrize(i, cbPrizeType, lPrizeCount);
			break;
		}
	}
	m_lPrizePoolScore -= m_stGameLottery.stPrizeRate[cbPrizeType].nExchangeRate*lPrizeCount;
	ASSERT(m_lPrizePoolScore>=0);
}

//��ȡ����Ľ���
void CGameLottery::OnGetDummyPrize( BYTE &cbPrizeType, LONG &lPrizeCount )
{
	int nRandIndex = rand()%RATE_DENOMINATOR;
	if (nRandIndex < m_nDummyPrizeRate)
	{
		size_t i = rand()%m_vecDummyLotteryPrize.size();
		cbPrizeType = m_vecDummyLotteryPrize[i].cbPrizeType;
		lPrizeCount = m_vecDummyLotteryPrize[i].lPrizeCount;
	}
	else
	{
		OnUserLottery(cbPrizeType, lPrizeCount);
	}
}

//��ȡ��ҳ齱��Ʒ
void CGameLottery::GetLotteryPrize( const BYTE cbIndex, BYTE &cbPrizeType, LONG &lPrizeCount )
{
	//����С��200ʱ
	if (cbIndex == 0)
	{
		cbPrizeType = LOTTERY_PRIZE_JINBI;
		lPrizeCount = 1;
	}
	else
	{
		//��Ʒ��������
		double fRandIndex = (rand()%RATE_DENOMINATOR)/double(RATE_DENOMINATOR);
		ASSERT(1.0==m_stGameLottery.stPrizeRate[LOTTERY_PRIZE_JINBI].fTotalRate+ m_stGameLottery.stPrizeRate[LOTTERY_PRIZE_JIANGQUAN].fTotalRate+ m_stGameLottery.stPrizeRate[LOTTERY_PRIZE_SAIPAI].fTotalRate);
		//��ȡ��ҽ���
		if (fRandIndex < m_stGameLottery.stPrizeRate[LOTTERY_PRIZE_JINBI].fTotalRate)
		{
			cbPrizeType = LOTTERY_PRIZE_JINBI;
		}
		//��ȡ��ȯ
		else if (fRandIndex < m_stGameLottery.stPrizeRate[LOTTERY_PRIZE_JINBI].fTotalRate+m_stGameLottery.stPrizeRate[LOTTERY_PRIZE_JIANGQUAN].fTotalRate)
		{
			cbPrizeType = LOTTERY_PRIZE_JIANGQUAN;
		}
		//��ȡ����
		else if (fRandIndex < m_stGameLottery.stPrizeRate[LOTTERY_PRIZE_JINBI].fTotalRate+m_stGameLottery.stPrizeRate[LOTTERY_PRIZE_JIANGQUAN].fTotalRate+m_stGameLottery.stPrizeRate[LOTTERY_PRIZE_SAIPAI].fTotalRate)
		{
			cbPrizeType = LOTTERY_PRIZE_SAIPAI;
		}
		else
		{
			ASSERT(FALSE);
		}

		switch (cbPrizeType)
		{
		case LOTTERY_PRIZE_JINBI:
			{
				lPrizeCount = GetJinBiPrizeCount();
				if (lPrizeCount<m_stGameLottery.stLotteryType[cbIndex].nGetMinJinBi || lPrizeCount>m_stGameLottery.stLotteryType[cbIndex].nGetMaxJinBi)
				{
					lPrizeCount = m_stGameLottery.stLotteryType[cbIndex].nGetMinJinBi+(rand()%(m_stGameLottery.stLotteryType[cbIndex].nGetMaxJinBi-m_stGameLottery.stLotteryType[cbIndex].nGetMinJinBi));
				}
			}break;
		case LOTTERY_PRIZE_JIANGQUAN:
			{
				lPrizeCount = GetJiangQuanPrizeCount();
				if (lPrizeCount<m_stGameLottery.stLotteryType[cbIndex].nGetMinJiangQuan || lPrizeCount>m_stGameLottery.stLotteryType[cbIndex].nGetMaxJiangQuan)
				{
					lPrizeCount = m_stGameLottery.stLotteryType[cbIndex].nGetMinJiangQuan+(rand()%(m_stGameLottery.stLotteryType[cbIndex].nGetMaxJiangQuan-m_stGameLottery.stLotteryType[cbIndex].nGetMinJiangQuan));
				}
			}break;
		case LOTTERY_PRIZE_SAIPAI:
			{
				lPrizeCount = GetSaiPaiPrizeCount();
				if (lPrizeCount<m_stGameLottery.stLotteryType[cbIndex].nGetMinSaiPai || lPrizeCount>m_stGameLottery.stLotteryType[cbIndex].nGetMaxSaiPai)
				{
					lPrizeCount = m_stGameLottery.stLotteryType[cbIndex].nGetMinSaiPai+(rand()%(m_stGameLottery.stLotteryType[cbIndex].nGetMaxSaiPai-m_stGameLottery.stLotteryType[cbIndex].nGetMinSaiPai));
				}
			}break;
		default:
			{
				lPrizeCount = 0;
				ASSERT(FALSE);
			}break;
		}
	}
}

//��ȡ��ҽ�������
LONG CGameLottery::GetJinBiPrizeCount()
{
	double fRandIndex = (rand()%RATE_DENOMINATOR)/double(RATE_DENOMINATOR);
	double fAllSubRate[MAX_SUB_PRIZE_RATE] = {0};
	for (BYTE i=0; i<MAX_SUB_PRIZE_RATE; i++)
	{
		for (BYTE j=0; j<i; j++)
		{
			fAllSubRate[i] = m_stGameLottery.stPrizeRate[LOTTERY_PRIZE_JINBI].stSubPrizeRate[i].fGetPrizeRate+m_stGameLottery.stPrizeRate[LOTTERY_PRIZE_JINBI].stSubPrizeRate[j].fGetPrizeRate;
		}
	}

	for (BYTE i=0; i<MAX_SUB_PRIZE_RATE; i++)
	{
		if (fRandIndex < fAllSubRate[i])
		{
			return m_stGameLottery.stPrizeRate[LOTTERY_PRIZE_JINBI].stSubPrizeRate[i].nGetMinPrize+(rand()%(m_stGameLottery.stPrizeRate[LOTTERY_PRIZE_JINBI].stSubPrizeRate[i].nGetMaxPrize-m_stGameLottery.stPrizeRate[LOTTERY_PRIZE_JINBI].stSubPrizeRate[i].nGetMinPrize));
		}
	}
	return 0;
}

//��ȡ��ȯ��������
LONG CGameLottery::GetJiangQuanPrizeCount()
{
	double fRandIndex = (rand()%RATE_DENOMINATOR)/double(RATE_DENOMINATOR);
	double fAllSubRate[MAX_SUB_PRIZE_RATE] = {0};
	for (BYTE i=0; i<MAX_SUB_PRIZE_RATE; i++)
	{
		for (BYTE j=0; j<i; j++)
		{
			fAllSubRate[i] = m_stGameLottery.stPrizeRate[LOTTERY_PRIZE_JIANGQUAN].stSubPrizeRate[i].fGetPrizeRate+m_stGameLottery.stPrizeRate[LOTTERY_PRIZE_JIANGQUAN].stSubPrizeRate[j].fGetPrizeRate;
		}
	}

	for (BYTE i=0; i<MAX_SUB_PRIZE_RATE; i++)
	{
		if (fRandIndex < fAllSubRate[i])
		{
			return m_stGameLottery.stPrizeRate[LOTTERY_PRIZE_JIANGQUAN].stSubPrizeRate[i].nGetMinPrize+(rand()%(m_stGameLottery.stPrizeRate[LOTTERY_PRIZE_JIANGQUAN].stSubPrizeRate[i].nGetMaxPrize-m_stGameLottery.stPrizeRate[LOTTERY_PRIZE_JIANGQUAN].stSubPrizeRate[i].nGetMinPrize));
		}
	}
	return 0;
}

//��ȡ���ƽ�������
LONG CGameLottery::GetSaiPaiPrizeCount()
{
	double fRandIndex = (rand()%RATE_DENOMINATOR)/double(RATE_DENOMINATOR);
	double fAllSubRate[MAX_SUB_PRIZE_RATE] = {0};
	for (BYTE i=0; i<MAX_SUB_PRIZE_RATE; i++)
	{
		for (BYTE j=0; j<i; j++)
		{
			fAllSubRate[i] = m_stGameLottery.stPrizeRate[LOTTERY_PRIZE_SAIPAI].stSubPrizeRate[i].fGetPrizeRate+m_stGameLottery.stPrizeRate[LOTTERY_PRIZE_SAIPAI].stSubPrizeRate[j].fGetPrizeRate;
		}
	}

	for (BYTE i=0; i<MAX_SUB_PRIZE_RATE; i++)
	{
		if (fRandIndex < fAllSubRate[i])
		{
			return m_stGameLottery.stPrizeRate[LOTTERY_PRIZE_SAIPAI].stSubPrizeRate[i].nGetMinPrize+(rand()%(m_stGameLottery.stPrizeRate[LOTTERY_PRIZE_SAIPAI].stSubPrizeRate[i].nGetMaxPrize-m_stGameLottery.stPrizeRate[LOTTERY_PRIZE_SAIPAI].stSubPrizeRate[i].nGetMinPrize));
		}
	}
	return 0;
}