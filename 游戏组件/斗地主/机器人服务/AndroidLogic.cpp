#include "StdAfx.h"
#include "Androidlogic.h"

//���캯��
CAndroidLogic::CAndroidLogic()
{
	ResetAllData(true);
}

//��������
CAndroidLogic::~CAndroidLogic()
{

}

//���ñ���
void CAndroidLogic::ResetAllData( bool bConstruct/*=false*/ )
{
	m_wBankerUser = INVALID_CHAIR;
	m_lBankerOutCardCount = 0;
	ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount, sizeof(m_cbHandCardCount));
	ZeroMemory(m_cbBackCardData, sizeof(m_cbBackCardData));
	ZeroMemory(m_cbAllCardOutCount, sizeof(m_cbAllCardOutCount));
}

//����ׯ��
void CAndroidLogic::SetBankerUser( const WORD wBankerUser ) 
{
	m_wBankerUser = wBankerUser;
}

//�����˿�
void CAndroidLogic::SetUserCard( const WORD wChairID, const BYTE cbCardData[], const BYTE cbCardCount )
{
	CopyMemory(m_cbHandCardData[wChairID], cbCardData, cbCardCount*sizeof(BYTE));
	m_cbHandCardCount[wChairID] = cbCardCount;

	//�����˿�
	SortCardList(m_cbHandCardData[wChairID], cbCardCount, ST_ORDER);
}

//���õ���
void CAndroidLogic::SetBackCard( const BYTE cbCardData[], const BYTE cbCardCount )
{
	ZeroMemory(m_cbBackCardData, sizeof(BYTE)*BACK_COUNT);
	CopyMemory(m_cbBackCardData, cbCardData, sizeof(BYTE)*BACK_COUNT);
}

//���õ���
void CAndroidLogic::SetBackCard( const WORD wChairID, const BYTE cbCardData[], const BYTE cbCardCount  )
{
	BYTE cbTempCount = m_cbHandCardCount[wChairID];
	CopyMemory(m_cbHandCardData[wChairID]+cbTempCount, cbCardData, cbCardCount*sizeof(BYTE));
	m_cbHandCardCount[wChairID] += cbCardCount;

	//�����˿�
	SortCardList(m_cbHandCardData[wChairID], m_cbHandCardCount[wChairID], ST_ORDER);
}

//ɾ���˿�
void CAndroidLogic::RemoveUserCard( const WORD wChairID, const BYTE cbRemoveCardData[], const BYTE cbRemoveCardCount ) 
{
	bool bSuccess = RemoveCard(cbRemoveCardData, cbRemoveCardCount, m_cbHandCardData[wChairID], m_cbHandCardCount[wChairID]);
	ASSERT(bSuccess);
	m_cbHandCardCount[wChairID] -= cbRemoveCardCount;

	//��¼�ѳ�������
	for (BYTE i=0; i<cbRemoveCardCount; i++)
	{
		BYTE cbLogicCardValue = GetCardLogicValue(cbRemoveCardData[i]);
		ASSERT(cbLogicCardValue>0 && cbLogicCardValue<18);
		if (cbLogicCardValue>=3 && cbLogicCardValue<18)
		{
			m_cbAllCardOutCount[cbLogicCardValue]++;
		}
	}
	//�������ƴ���
	if (wChairID==m_wBankerUser && cbRemoveCardCount>0)
	{
		++m_lBankerOutCardCount;
	}
}

//�з��ж�
BYTE CAndroidLogic::LandScore( const WORD wMeChairID, const BYTE cbCurrentLandScore)
{
	//������Ŀ
	BYTE cbMeLargeCardCount = 0, cbMeWangCount = 0;
	BYTE cbMeLandScoreCardData[MAX_COUNT] = { 0 };
	CopyMemory(cbMeLandScoreCardData, m_cbHandCardData[wMeChairID], sizeof(BYTE)*m_cbHandCardCount[wMeChairID]);
	CopyMemory(&cbMeLandScoreCardData[m_cbHandCardCount[wMeChairID]], m_cbBackCardData, sizeof(BYTE)*BACK_COUNT);
	for (int i = 0; i < MAX_COUNT; i++)
	{
		if (cbMeLandScoreCardData[i] >= 0x4E) cbMeWangCount++;
		if (GetCardLogicValue(cbMeLandScoreCardData[i]) >= 15) cbMeLargeCardCount++;
	}

	//���Ƹ���
	BYTE cbSingleCardCount = AnalyseSinleCardCount(cbMeLandScoreCardData, sizeof(cbMeLandScoreCardData), NULL, 0);
	BYTE cbBombCardCount = 0, cbBombCardData[MAX_COUNT] = { 0 };
	GetAllBombCard(cbMeLandScoreCardData, MAX_COUNT, cbBombCardData, cbBombCardCount);
	//�����֣�����1������2,������ը��
	if (cbMeLargeCardCount >= 3 && cbMeWangCount > 0 || cbBombCardCount >= 4)
	{
		return cbCurrentLandScore < 3 ? 3 : 255;
	}
	//�ж��֣�2���ϵ���4�ţ�������С��4��
	if (cbMeLargeCardCount >= 4 && cbSingleCardCount <= 4)
	{
		return cbCurrentLandScore < 2 ? 2 : 255;
	}
	//�����з�
	if (cbMeLargeCardCount <= 2 || cbCurrentLandScore == 1)
	{
		return 255;
	}

	//��������
	BYTE cbMinSingleCardCount = MAX_COUNT;
	for (WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID)
	{
		BYTE cbTempSingleCardCount = AnalyseSinleCardCount(m_cbHandCardData[wChairID], NORMAL_COUNT, NULL, 0);
		if (wChairID != wMeChairID && cbTempSingleCardCount < cbMinSingleCardCount)
		{
			cbTempSingleCardCount = cbMinSingleCardCount;
		}
	}
	//��һ��
	if (cbMeLargeCardCount >= 3 && cbSingleCardCount < cbMinSingleCardCount - 3)
	{
		return cbCurrentLandScore == 0 ? 1 : 255;
	}
	//�����з�
	return 255;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/********************************************************************
��������Combination
������
cbCombineCardData���洢��������Ͻ��
cbResComLen���ѵõ�����ϳ��ȣ���ʼ����ʱ�˲���Ϊ0
cbResultCardData���洢���е���Ͻ��
cbResCardLen��cbResultCardData�ĵ�һ�±�ĳ��ȣ���Ͻ���ĸ���
cbSrcCardData����Ҫ����ϵ�����
cbSrcLen��cbSrcCardData��������Ŀ
cbCombineLen2��cbCombineLen1����ϵĳ��ȣ���ʼ����ʱ������ȡ�
*********************************************************************/
//����㷨
void CAndroidLogic::Combination( BYTE cbCombineCardData[], BYTE cbResComLen, BYTE cbResultCardData[100][5], BYTE &cbResCardLen, BYTE cbSrcCardData[], BYTE cbCombineLen1, BYTE cbSrcLen, const BYTE cbCombineLen2 )
{
	if (cbResComLen == cbCombineLen2)
	{
		CopyMemory(&cbResultCardData[cbResCardLen], cbCombineCardData, cbResComLen);
		++cbResCardLen;
		ASSERT(cbResCardLen<255);
	}
	else
	{ 
		if (cbCombineLen1>=1 && cbSrcLen>0 && cbSrcLen+1>=0)
		{
			cbCombineCardData[cbCombineLen2-cbCombineLen1] = cbSrcCardData[0];
			++cbResComLen;
			Combination(cbCombineCardData, cbResComLen, cbResultCardData, cbResCardLen, cbSrcCardData+1, cbCombineLen1-1, cbSrcLen-1, cbCombineLen2);

			--cbResComLen;
			Combination(cbCombineCardData, cbResComLen, cbResultCardData, cbResCardLen, cbSrcCardData+1, cbCombineLen1, cbSrcLen-1, cbCombineLen2);
		}
	}
}

//�����㷨
void CAndroidLogic::Permutation( BYTE *list, int m, int n, BYTE result[][4], BYTE &len )
{ 
	int j=0, temp=0; 
	if (m == n)
	{ 
		for (j=0; j<n; j++) 
		{
			result[len][j] = list[j]; 
		}
		len++;
	} 
	else
	{ 
		for (j=m; j<n; j++)
		{ 
			temp = list[m];
			list[m] = list[j];
			list[j] = temp;
			Permutation(list, m+1, n, result, len); 
			temp = list[m];
			list[m] = list[j];
			list[j] = temp;
		} 
	} 
}

//��������ж�
bool CAndroidLogic::IsLargestCard( const WORD wChairID, const BYTE cbWantOutCardData[], const BYTE cbWantOutCardCount )
{
	VERIFY_RETURN_FALSE(IsValidUser(wChairID));

	tagPromptCardResult MePromptCardResult;
	CGameLogic::SearchOutCard(m_cbHandCardData[wChairID], m_cbHandCardCount[wChairID], cbWantOutCardData, cbWantOutCardCount, MePromptCardResult);

	if (MePromptCardResult.cbResultCount > 0)
	{
		return false;
	}
	return true;
}

//�ų�ը����һ�����������
bool CAndroidLogic::IsLargestCardWithOutBomb( const WORD wMeChairID, const BYTE cbWantOutCardData[], const BYTE cbWantOutCardCount )
{
	VERIFY_RETURN_FALSE(IsValidUser(wMeChairID));

	BYTE cbWantOutCardType = GetCardType(cbWantOutCardData, cbWantOutCardCount);
	VERIFY_RETURN_FALSE(cbWantOutCardType!=CT_ERROR);

	//���Ƽ�¼�м����Լ�������
	BYTE cbAllCardOutCount[18] = {0};
	CopyMemory(cbAllCardOutCount, m_cbAllCardOutCount, sizeof(m_cbAllCardOutCount));
	for (BYTE i=0; i<m_cbHandCardCount[wMeChairID]; i++)
	{
		BYTE cbCardLogicValue = GetCardLogicValue(m_cbHandCardData[wMeChairID][i]);
		ASSERT(cbCardLogicValue>=3 && cbCardLogicValue<18);
		if (cbCardLogicValue>=3 && cbCardLogicValue<18)
		{
			cbAllCardOutCount[cbCardLogicValue]++;
		}
	}

	//��������
	if (cbWantOutCardType == CT_SINGLE)
	{
		BYTE cbCardLogicValue = GetCardLogicValue(cbWantOutCardData[0]);
		ASSERT(cbCardLogicValue>0 && cbCardLogicValue<18);
		//����ֱ�ӷ���
		if (cbCardLogicValue == 17) return true;
		bool bFindLargerCard = false;
		for (BYTE i=cbCardLogicValue+1; i<18; i++)
		{
			if (i==16 || i==17)
			{
				if (cbAllCardOutCount[i] == 0) 
				{
					bFindLargerCard = true;
					break;
				}
			}
			else
			{
				if (cbAllCardOutCount[i] < 4)
				{
					bFindLargerCard = true;
					break;
				}
			}
		}
		return !bFindLargerCard;
	}
	//��������
	else if (cbWantOutCardType == CT_DOUBLE)
	{
		BYTE cbCardLogicValue = GetCardLogicValue(cbWantOutCardData[0]);
		ASSERT(cbCardLogicValue>0 && cbCardLogicValue<18);
		//��2ֱ�ӷ���
		if (cbCardLogicValue == 15) return true;
		bool bFindLargerCard = false;
		for (BYTE i=cbCardLogicValue+1; i<16; i++)
		{
			if (cbAllCardOutCount[i] <= 2)
			{
				bFindLargerCard = true;
				break;
			}
		}
		return !bFindLargerCard;
	}
	//��������
	else if (cbWantOutCardType == CT_THREE)
	{
		BYTE cbCardLogicValue = GetCardLogicValue(cbWantOutCardData[0]);
		ASSERT(cbCardLogicValue>0 && cbCardLogicValue<18);
		//��2ֱ�ӷ���
		if (cbCardLogicValue == 15) return true;
		bool bFindLargerCard = false;
		for (BYTE i=cbCardLogicValue+1; i<16; i++)
		{
			if (cbAllCardOutCount[i] <= 1)
			{
				bFindLargerCard = true;
				break;
			}
		}
		return !bFindLargerCard;
	}
	//˳������
	else if (cbWantOutCardType == CT_SINGLE_LINE)
	{
		BYTE cbTempCardData[MAX_COUNT] = {0};
		CopyMemory(cbTempCardData, cbWantOutCardData, cbWantOutCardCount);
		SortCardList(cbTempCardData, cbWantOutCardCount, ST_ORDER);
		BYTE cbCardLogicValue = GetCardLogicValue(cbTempCardData[0]);
		ASSERT(cbCardLogicValue>0 && cbCardLogicValue<18);
		//A��ͷֱ�ӷ���
		if (cbCardLogicValue == 14) return true;
		for (WORD i=0; i<GAME_PLAYER; i++)
		{
			if (i == wMeChairID) continue;
			if (!IsLargestCard(i, cbWantOutCardData, cbWantOutCardCount))
			{
				return false;
			}
		}
		return true;
	}
	//˫˳����
	else if (cbWantOutCardType == CT_DOUBLE_LINE)
	{
		BYTE cbTempCardData[MAX_COUNT] = {0};
		CopyMemory(cbTempCardData, cbWantOutCardData, cbWantOutCardCount);
		SortCardList(cbTempCardData, cbWantOutCardCount, ST_ORDER);
		BYTE cbCardLogicValue = GetCardLogicValue(cbTempCardData[0]);
		ASSERT(cbCardLogicValue>0 && cbCardLogicValue<18);
		//A��ͷֱ�ӷ���
		if (cbCardLogicValue == 14) return true;
		for (WORD i=0; i<GAME_PLAYER; i++)
		{
			if (i == wMeChairID) continue;
			if (!IsLargestCard(i, cbWantOutCardData, cbWantOutCardCount))
			{
				return false;
			}
		}
		return true;
	}
	//��˳����
	else if (cbWantOutCardType == CT_THREE_LINE)
	{
		BYTE cbTempCardData[MAX_COUNT] = {0};
		CopyMemory(cbTempCardData, cbWantOutCardData, cbWantOutCardCount);
		SortCardList(cbTempCardData, cbWantOutCardCount, ST_ORDER);
		BYTE cbCardLogicValue = GetCardLogicValue(cbTempCardData[0]);
		ASSERT(cbCardLogicValue>0 && cbCardLogicValue<18);
		//A��ͷֱ�ӷ���
		if (cbCardLogicValue == 14) return true;
		for (WORD i=0; i<GAME_PLAYER; i++)
		{
			if (i == wMeChairID) continue;
			if (!IsLargestCard(i, cbWantOutCardData, cbWantOutCardCount))
			{
				return false;
			}
		}
		return true;
	}
	//
	else if (cbWantOutCardType == CT_THREE_TAKE_ONE)
	{
		//����һ��
		if (cbWantOutCardCount == 4)
		{
			BYTE cbThreeCardData[MAX_COUNT]={0}, cbThreeCardCount=0;
			GetAllThreeCard(cbWantOutCardData, cbWantOutCardCount, cbThreeCardData, cbThreeCardCount);
			ASSERT(cbThreeCardCount == 3);
			bool bFindLargerCard = false;
			if (cbThreeCardCount == 3)
			{
				BYTE cbCardLogicValue = GetCardLogicValue(cbThreeCardData[0]);
				if (cbCardLogicValue == 15) return true;
				for (BYTE i=cbCardLogicValue+1; i<16; i++)
				{
					if (cbAllCardOutCount[i] <= 1)
					{
						bFindLargerCard = true;
						break;
					}
				}
			}
			return !bFindLargerCard;
		}
		//�ɻ������
		else
		{
			for (WORD i=0; i<GAME_PLAYER; i++)
			{
				if (i == wMeChairID) continue;
				if (!IsLargestCard(i, cbWantOutCardData, cbWantOutCardCount))
				{
					return false;
				}
			}
			return true;
		}
	}
	//
	else if (cbWantOutCardType == CT_THREE_TAKE_TWO)
	{
		//����һ��
		if (cbWantOutCardCount == 5)
		{
			BYTE cbThreeCardData[MAX_COUNT]={0}, cbThreeCardCount=0;
			GetAllThreeCard(cbWantOutCardData, cbWantOutCardCount, cbThreeCardData, cbThreeCardCount);
			ASSERT(cbThreeCardCount == 3);
			bool bFindLargerCard = false;
			if (cbThreeCardCount == 3)
			{
				BYTE cbCardLogicValue = GetCardLogicValue(cbThreeCardData[0]);
				if (cbCardLogicValue == 15) return true;
				for (BYTE i=cbCardLogicValue+1; i<16; i++)
				{
					if (cbAllCardOutCount[i] <= 1)
					{
						bFindLargerCard = true;
						break;
					}
				}
			}
			return !bFindLargerCard;
		}
		//�ɻ������
		else
		{
			for (WORD i=0; i<GAME_PLAYER; i++)
			{
				if (i == wMeChairID) continue;
				if (!IsLargestCard(i, cbWantOutCardData, cbWantOutCardCount))
				{
					return false;
				}
			}
			return true;
		}
	}
	return true;
}

//�����Ĵ�������
void CAndroidLogic::Analyse4D2CardType( const WORD wMeChairID, const BYTE cbHandCardData[], const BYTE cbHandCardCount, const BYTE cbHandCardType, tagOutCardResult &OutCardResult )
{
	//�������
	const WORD wUndersideUser = (m_wBankerUser+1)%GAME_PLAYER;	 //�����¼�
	const WORD wUpsideUser = (wUndersideUser+1)%GAME_PLAYER;	 //�����ϼ�
	//��������ǵ���
	if (wMeChairID == m_wBankerUser)
	{
		//������������
		tagAnalyseResult MeAnalyseResult;
		VERIFY(AnalysebCardData(cbHandCardData, cbHandCardCount, MeAnalyseResult));
		ASSERT(MeAnalyseResult.cbFourCount==1);
		//�Ĵ�������
		if (cbHandCardType == CT_FOUR_TAKE_ONE)
		{
			//�Ĵ������ж�
			if (cbHandCardData[0]==0x4F && cbHandCardData[1]==0x4E)
			{
				OutCardResult.cbCardCount = 2;
				OutCardResult.cbCardData[0] = 0x4F;
				OutCardResult.cbCardData[1] = 0x4E;
				return;
			}
			else
			{
				if (MeAnalyseResult.cbDoubleCount == 1)
				{
					if ((m_cbHandCardCount[wUndersideUser]==2&&GetCardValue(m_cbHandCardData[wUndersideUser][0])==GetCardValue(m_cbHandCardData[wUndersideUser][1])&&GetCardLogicValue(m_cbHandCardData[wUndersideUser][0])>GetCardLogicValue(MeAnalyseResult.cbDoubleCardData[0]))
						|| (m_cbHandCardCount[wUpsideUser]==2&&GetCardValue(m_cbHandCardData[wUpsideUser][0])==GetCardValue(m_cbHandCardData[wUpsideUser][1])&&GetCardLogicValue(m_cbHandCardData[wUpsideUser][0])>GetCardLogicValue(MeAnalyseResult.cbDoubleCardData[0])))
					{
						OutCardResult.cbCardCount = cbHandCardCount;
						CopyMemory(&OutCardResult.cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
						return;
					}
					else
					{
						OutCardResult.cbCardCount = 2;
						CopyMemory(&OutCardResult.cbCardData, &MeAnalyseResult.cbDoubleCardData[0], sizeof(BYTE)*2);
						return;
					}
				}
				else if (MeAnalyseResult.cbSingleCount == 2)
				{
					//����Լ��ĵ��ƴ���ũ��������
					if (GetCardLogicValue(MeAnalyseResult.cbSingleCardData[0])>=m_cbHandCardData[wUndersideUser][0] && GetCardLogicValue(MeAnalyseResult.cbSingleCardData[0])>=m_cbHandCardData[wUpsideUser][0])
					{
						OutCardResult.cbCardCount = 1;
						OutCardResult.cbCardData[0] = MeAnalyseResult.cbSingleCardData[0];
						return;
					}
					else
					{
						if ((m_cbHandCardCount[wUndersideUser]==1&&GetCardLogicValue(m_cbHandCardData[wUndersideUser][0])>GetCardLogicValue(MeAnalyseResult.cbSingleCardData[0]))
							|| (m_cbHandCardCount[wUpsideUser]==1&&GetCardLogicValue(m_cbHandCardData[wUpsideUser][0])>GetCardLogicValue(MeAnalyseResult.cbSingleCardData[0])))
						{
							OutCardResult.cbCardCount = cbHandCardCount;
							CopyMemory(&OutCardResult.cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
							return;
						}
						else
						{
							OutCardResult.cbCardCount = 1;
							OutCardResult.cbCardData[0] = MeAnalyseResult.cbSingleCardData[1];
							return;
						}
					}
				}
			}
		}
		else if (cbHandCardType == CT_FOUR_TAKE_TWO)
		{
			if ((m_cbHandCardCount[wUndersideUser]==2&&GetCardValue(m_cbHandCardData[wUndersideUser][0])==GetCardValue(m_cbHandCardData[wUndersideUser][1])&&GetCardLogicValue(m_cbHandCardData[wUndersideUser][0])>GetCardLogicValue(MeAnalyseResult.cbDoubleCardData[0]))
				|| (m_cbHandCardCount[wUpsideUser]==2&&GetCardValue(m_cbHandCardData[wUpsideUser][0])==GetCardValue(m_cbHandCardData[wUpsideUser][1])&&GetCardLogicValue(m_cbHandCardData[wUpsideUser][0])>GetCardLogicValue(MeAnalyseResult.cbDoubleCardData[0])))
			{
				//���Ĵ���
				OutCardResult.cbCardCount = cbHandCardCount;
				CopyMemory(&OutCardResult.cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
				return;
			}
			else
			{
				OutCardResult.cbCardCount = 2;
				CopyMemory(&OutCardResult.cbCardData, &MeAnalyseResult.cbDoubleCardData[MeAnalyseResult.cbDoubleCount*2-2], sizeof(BYTE)*2);
				return;
			}
		}
	}
	else
	{
		//�������
		const WORD wFriendUser = (wMeChairID==wUndersideUser) ? wUpsideUser : wUndersideUser;  //����
		//������������
		tagAnalyseResult MeAnalyseResult, BankerAnalyseResult, FriendAnalyseResult;
		VERIFY(AnalysebCardData(cbHandCardData, cbHandCardCount, MeAnalyseResult));
		VERIFY(AnalysebCardData(m_cbHandCardData[m_wBankerUser], m_cbHandCardCount[m_wBankerUser], BankerAnalyseResult));
		VERIFY(AnalysebCardData(m_cbHandCardData[wFriendUser], m_cbHandCardCount[wFriendUser], FriendAnalyseResult));
		ASSERT(MeAnalyseResult.cbFourCount==1);
		if (cbHandCardType == CT_FOUR_TAKE_ONE)
		{
			//�Ĵ������ж�
			if (cbHandCardData[0]==0x4F && cbHandCardData[1]==0x4E)
			{
				OutCardResult.cbCardCount = 2;
				OutCardResult.cbCardData[0] = 0x4F;
				OutCardResult.cbCardData[1] = 0x4E;
				return;
			}
			else
			{
				if (MeAnalyseResult.cbDoubleCount == 1)
				{
					if (m_cbHandCardCount[m_wBankerUser]==2 && BankerAnalyseResult.cbDoubleCount==1 && GetCardLogicValue(BankerAnalyseResult.cbDoubleCardData[0])>GetCardLogicValue(MeAnalyseResult.cbDoubleCardData[0]))
					{
						//��������ǵ����¼�
						if (wMeChairID == wUndersideUser)
						{
							//�жϵ����ϼ��Ƿ��бȵ�����Ķ���
							BYTE cbDoubleCardData[MAX_COUNT]={0}, cbDoubleCardCount=0;
							GetAllDoubleCard(m_cbHandCardData[wFriendUser], m_cbHandCardCount[m_wBankerUser], cbDoubleCardData, cbDoubleCardCount);
							bool bFindLargerDoubleCard = false;
							for (BYTE i=0; i<cbDoubleCardCount; i+=2)
							{
								if (GetCardLogicValue(cbDoubleCardData[i]) >= GetCardLogicValue(BankerAnalyseResult.cbDoubleCardData[0]))
								{
									bFindLargerDoubleCard = true;
									break;
								}
							}
							//�����ӣ������ϼ��ô��ƽӹ�ȥ
							if (bFindLargerDoubleCard)
							{
								OutCardResult.cbCardCount = 2;
								CopyMemory(&OutCardResult.cbCardData, &MeAnalyseResult.cbDoubleCardData[0], sizeof(BYTE)*2);
								return;
							}
							else
							{
								OutCardResult.cbCardCount = cbHandCardCount;
								CopyMemory(&OutCardResult.cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
								return;
							}
						}
						else
						{
							OutCardResult.cbCardCount = cbHandCardCount;
							CopyMemory(&OutCardResult.cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
							return;
						}
					}
					else
					{
						OutCardResult.cbCardCount = 2;
						CopyMemory(&OutCardResult.cbCardData, &MeAnalyseResult.cbDoubleCardData[0], sizeof(BYTE)*2);
						return;
					}
				}
				else if (MeAnalyseResult.cbSingleCount == 2)
				{
					ASSERT(MeAnalyseResult.cbSingleCount==2);
					//�Լ����ϵ���С���Ŵ��ڵ������ϵ���
					if (GetCardLogicValue(MeAnalyseResult.cbSingleCardData[1]) >= GetCardLogicValue(m_cbHandCardData[m_wBankerUser][0]))
					{
						OutCardResult.cbCardCount = 1;
						OutCardResult.cbCardData[0] = MeAnalyseResult.cbSingleCardData[1];
						return;
					}
					else
					{
						if (GetCardLogicValue(MeAnalyseResult.cbSingleCardData[0]) >= GetCardLogicValue(m_cbHandCardData[m_wBankerUser][0]))
						{
							//���ϳ�ը���������ĵ���,�����Լ��ǵ������ϼ�
							if (IsLargestCardWithOutBomb(wMeChairID,MeAnalyseResult.cbSingleCardData,1) || wMeChairID==wUpsideUser)
							{
								OutCardResult.cbCardCount = 1;
								OutCardResult.cbCardData[0] = MeAnalyseResult.cbSingleCardData[0];
								return;
							}
							else 
							{
								OutCardResult.cbCardCount = 1;
								OutCardResult.cbCardData[0] = MeAnalyseResult.cbSingleCardData[1];
								return;
							}
						}
						else
						{
							//�����ϼ����Լ�
							if (wMeChairID == wUpsideUser)
							{
								if (m_cbHandCardCount[m_wBankerUser] == 1)
								{
									OutCardResult.cbCardCount = cbHandCardCount;
									CopyMemory(OutCardResult.cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
									return;
								}
								else
								{
									OutCardResult.cbCardCount = 1;
									OutCardResult.cbCardData[0] = MeAnalyseResult.cbSingleCardData[1];
									return;
								}
							}
							//�����¼����Լ�
							else if (wMeChairID == wUndersideUser)
							{
								if (m_cbHandCardCount[m_wBankerUser] == 1)
								{
									//���Ѵ��ƽӹ�ȥ
									if (GetCardLogicValue(m_cbHandCardData[wFriendUser][0]) >= GetCardLogicValue(m_cbHandCardData[m_wBankerUser][0]))
									{
										OutCardResult.cbCardCount = 1;
										OutCardResult.cbCardData[0] = MeAnalyseResult.cbSingleCardData[1];
										return;
									}
									else
									{
										OutCardResult.cbCardCount = cbHandCardCount;
										CopyMemory(OutCardResult.cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
										return;
									}
								}
								else
								{
									OutCardResult.cbCardCount = 1;
									OutCardResult.cbCardData[0] = MeAnalyseResult.cbSingleCardData[1];
									return;
								}
							}
						}
					}
				}
			}
		}
		else if (cbHandCardType == CT_FOUR_TAKE_TWO)
		{
			ASSERT(MeAnalyseResult.cbDoubleCount==2);
			//����ֻʣһ��
			if (m_cbHandCardCount[m_wBankerUser]==2 && GetCardValue(m_cbHandCardData[m_wBankerUser][0])==GetCardValue(m_cbHandCardData[m_wBankerUser][1]))
			{
				//��С�Ķ��ӱȵ����Ļ���
				if (GetCardLogicValue(MeAnalyseResult.cbDoubleCardData[2]) >= GetCardLogicValue(m_cbHandCardData[m_wBankerUser][0]))
				{
					OutCardResult.cbCardCount = 2;
					CopyMemory(OutCardResult.cbCardData, &MeAnalyseResult.cbDoubleCardData[2], sizeof(BYTE)*2);
					return;
				}
				else
				{
					if (GetCardLogicValue(MeAnalyseResult.cbDoubleCardData[0]) >= GetCardLogicValue(m_cbHandCardData[m_wBankerUser][0]))
					{
						if (IsLargestCardWithOutBomb(wMeChairID,MeAnalyseResult.cbDoubleCardData,2) || wMeChairID==wUpsideUser)
						{
							OutCardResult.cbCardCount = 2;
							CopyMemory(OutCardResult.cbCardData, &MeAnalyseResult.cbDoubleCardData[0], sizeof(BYTE)*2);
							return;
						}
						else
						{
							OutCardResult.cbCardCount = 2;
							CopyMemory(OutCardResult.cbCardData, &MeAnalyseResult.cbDoubleCardData[2], sizeof(BYTE)*2);
							return;
						}
					}
				}
			}
			else 
			{
				OutCardResult.cbCardCount = 2;
				CopyMemory(OutCardResult.cbCardData, &MeAnalyseResult.cbDoubleCardData[2], sizeof(BYTE)*2);
				return;
			}
		}		
	}
}

//����ֻʣһ����ʱ�ĳ��Ʋ���
void CAndroidLogic::AnalyseEnemyLastOneCard( const WORD wMeChairID, const BYTE cbHandCardData[], const BYTE cbHandCardCount, const tagOutCardTypeResult CardTypeResult[13], tagOutCardResult &OutCardResult )
{
	ZeroMemory(&OutCardResult, sizeof(tagOutCardResult));
	
	//�������
	const WORD wUndersideUser = (m_wBankerUser+1)%GAME_PLAYER;	 //�����¼�
	const WORD wUpsideUser = (wUndersideUser+1)%GAME_PLAYER;	 //�����ϼ�

	bool bEnemyOnlyHaveOneCard = false;
	if (wMeChairID == m_wBankerUser)
	{
		if (m_cbHandCardCount[wUndersideUser]==1 || m_cbHandCardCount[wUpsideUser]==1)
		{
			bEnemyOnlyHaveOneCard = true;
		}
		//����ũ��ȫ��ֻʣһ��
		if (m_cbHandCardCount[wUndersideUser]==1 && m_cbHandCardCount[wUpsideUser]==1)
		{
			BYTE cbTempCardData[MAX_COUNT]={0}, cbTempCardCount=0;
			BYTE cbFirstCardIndex = 0;
			if (cbHandCardCount>=2 && cbHandCardData[0]==0x4F && cbHandCardData[1]==0x4E)
			{
				cbFirstCardIndex = 2;
			}
			if (cbHandCardCount-cbFirstCardIndex > 0)
			{
				cbTempCardCount = cbHandCardCount-cbFirstCardIndex;
				CopyMemory(cbTempCardData, &cbHandCardData[cbFirstCardIndex], sizeof(BYTE)*cbTempCardCount);
				tagAnalyseResult MeAnalyseResult;
				AnalysebCardData(cbTempCardData, cbTempCardCount, MeAnalyseResult);
				if ((MeAnalyseResult.cbSingleCount<=1) || (MeAnalyseResult.cbSingleCount==2&&GetCardLogicValue(MeAnalyseResult.cbSingleCardData[0])>=GetCardLogicValue(m_cbHandCardData[wUndersideUser][0])&&GetCardLogicValue(MeAnalyseResult.cbSingleCardData[0])>=GetCardLogicValue(m_cbHandCardData[wUpsideUser][0])))
				{
					if (MeAnalyseResult.cbDoubleCount > 0)
					{
						OutCardResult.cbCardCount = 2;
						CopyMemory(OutCardResult.cbCardData, MeAnalyseResult.cbDoubleCardData, sizeof(BYTE)*2);
						return;
					}
					if (MeAnalyseResult.cbThreeCount > 0)
					{
						OutCardResult.cbCardCount = 3;
						CopyMemory(OutCardResult.cbCardData, MeAnalyseResult.cbThreeCardData, sizeof(BYTE)*3);
						return;
					}
					if (MeAnalyseResult.cbFourCount > 0)
					{
						OutCardResult.cbCardCount = 4;
						CopyMemory(OutCardResult.cbCardData, MeAnalyseResult.cbFourCardData, sizeof(BYTE)*4);
						return;
					}
					if (MeAnalyseResult.cbSingleCount > 0)
					{
						OutCardResult.cbCardCount = 1;
						OutCardResult.cbCardData[0] = MeAnalyseResult.cbSingleCardData[0];
						return;
					}
				}
			}
		}
	}
	else
	{
		if (m_cbHandCardCount[m_wBankerUser] == 1)
		{
			bEnemyOnlyHaveOneCard = true;

			BYTE cbTempCardData[MAX_COUNT]={0}, cbTempCardCount=0;
			BYTE cbFirstCardIndex = 0;
			if (cbHandCardCount>=2 && cbHandCardData[0]==0x4F && cbHandCardData[1]==0x4E)
			{
				cbFirstCardIndex = 2;
			}
			if (cbHandCardCount-cbFirstCardIndex > 0)
			{
				cbTempCardCount = cbHandCardCount-cbFirstCardIndex;
				CopyMemory(cbTempCardData, &cbHandCardData[cbFirstCardIndex], sizeof(BYTE)*cbTempCardCount);
				tagAnalyseResult MeAnalyseResult;
				AnalysebCardData(cbTempCardData, cbTempCardCount, MeAnalyseResult);
				if ((MeAnalyseResult.cbSingleCount<=1) || (MeAnalyseResult.cbSingleCount==2&&GetCardLogicValue(MeAnalyseResult.cbSingleCardData[0])>=GetCardLogicValue(m_cbHandCardData[m_wBankerUser][0])))
				{
					if (MeAnalyseResult.cbDoubleCount > 0)
					{
						OutCardResult.cbCardCount = 2;
						CopyMemory(OutCardResult.cbCardData, MeAnalyseResult.cbDoubleCardData, sizeof(BYTE)*2);
						return;
					}
					if (MeAnalyseResult.cbThreeCount > 0)
					{
						OutCardResult.cbCardCount = 3;
						CopyMemory(OutCardResult.cbCardData, MeAnalyseResult.cbThreeCardData, sizeof(BYTE)*3);
						return;
					}
					if (MeAnalyseResult.cbFourCount > 0)
					{
						OutCardResult.cbCardCount = 4;
						CopyMemory(OutCardResult.cbCardData, MeAnalyseResult.cbFourCardData, sizeof(BYTE)*4);
						return;
					}
					if (MeAnalyseResult.cbSingleCount > 0)
					{
						OutCardResult.cbCardCount = 1;
						OutCardResult.cbCardData[0] = MeAnalyseResult.cbSingleCardData[0];
						return;
					}
				}
			}
		}
	}

	if (bEnemyOnlyHaveOneCard)
	{
		//���㵥��
		BYTE cbSingleCardCount[4] = {MAX_COUNT,MAX_COUNT,MAX_COUNT,MAX_COUNT};
		BYTE cbOutCardIndex[4] = {0};
		BYTE cbOutCardType[4] = {CT_ERROR};
		BYTE cbMinSingleCardCount = MAX_COUNT;
		BYTE cbMinCardType = CT_ERROR;
		BYTE cbMinCardIndex = 0;
		//�����ź�ը����
		for(BYTE cbCardType=CT_DOUBLE; cbCardType<CT_FOUR_TAKE_ONE; ++cbCardType)
		{
			const tagOutCardTypeResult &tempOutCardTypeResult = CardTypeResult[cbCardType];
			//��ͬ���ͣ���ͬ���ȣ���������������ͬ���Ϳ��ܳ��Ȳ�һ��
			for (BYTE i=0; i<tempOutCardTypeResult.cbCardTypeCount; ++i)
			{
				//��ֹ��������ը���ж�
				if (cbCardType == CT_DOUBLE)
				{
					tagAnalyseResult AnalyseResult;
					AnalysebCardData(cbHandCardData, cbHandCardCount, AnalyseResult);
					if (AnalyseResult.cbSingleCount+AnalyseResult.cbThreeCount*3 == cbHandCardCount)
					{
						bool bContinue = false;
						for (BYTE cbThreeIndex=0; cbThreeIndex<AnalyseResult.cbThreeCount; ++cbThreeIndex)
						{
							if (GetCardValue(tempOutCardTypeResult.cbEachHandCardData[i][0]) == GetCardValue(AnalyseResult.cbThreeCardData[3*cbThreeIndex]))
							{
								bContinue = true;
								break;
							}
						}
						if (bContinue) continue;
					}
					if (AnalyseResult.cbFourCount > 0)
					{
						bool bContinue = false;
						for (BYTE cbFourIndex=0; cbFourIndex<AnalyseResult.cbFourCount; cbFourIndex++)
						{
							if (GetCardValue(tempOutCardTypeResult.cbEachHandCardData[i][0]) == GetCardValue(AnalyseResult.cbFourCardData[4*cbFourIndex]))
							{
								bContinue = true;
								break;
							}
						}
						if (bContinue) continue;
					}
				}

				BYTE cbTempCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, tempOutCardTypeResult.cbEachHandCardData[i], tempOutCardTypeResult.cbEachHandCardCount[i]);
				//����cbMinSingleCardCount[4]�����ֵ
				for (BYTE j=0; j<4; ++j)
				{
					if (cbSingleCardCount[j] >= cbTempCount)
					{
						cbSingleCardCount[j] = cbTempCount;
						cbOutCardIndex[j] = i;
						cbOutCardType[j] = cbCardType;
						break;
					}
				}
				//������Сֵ
				if (cbMinSingleCardCount >= cbTempCount)
				{
					//��С����
					cbMinCardType = cbCardType;
					//��С�����е���С����
					cbMinSingleCardCount = cbTempCount;						
					//��С�����е���С��
					cbMinCardIndex = i;
				}
			}
		}
		if (cbMinSingleCardCount != MAX_COUNT)
		{
			BYTE cbCurIndex = cbMinCardIndex;
			//ѡ����С��
			for (BYTE i=0; i<4; ++i)
			{
				if (cbOutCardType[i]==cbMinCardType && cbSingleCardCount[i]<=cbMinSingleCardCount && GetCardLogicValue(CardTypeResult[cbMinCardType].cbEachHandCardData[cbOutCardIndex[i]][0])<GetCardLogicValue(CardTypeResult[cbMinCardType].cbEachHandCardData[cbCurIndex][0]))
				{
					cbCurIndex = cbOutCardIndex[i];
				}
			}
			//���ñ���
			OutCardResult.cbCardCount = CardTypeResult[cbMinCardType].cbEachHandCardCount[cbCurIndex];
			CopyMemory(OutCardResult.cbCardData, CardTypeResult[cbMinCardType].cbEachHandCardData[cbCurIndex], CardTypeResult[cbMinCardType].cbEachHandCardCount[cbCurIndex]*sizeof(BYTE));
			return;
		}

		//ʣ���������
		if (cbHandCardCount==4 && cbHandCardData[0]==0x4F && cbHandCardData[1]==0x4E)
		{
			//�����ĵ���
			OutCardResult.cbCardCount = 1;
			OutCardResult.cbCardData[0] = cbHandCardData[2];
			return;
		}
		BYTE cbTempCardData[MAX_COUNT]={0}, cbTempCardCount=0;
		BYTE cbFirstCardIndex = 0;
		if (cbHandCardCount>=2 && cbHandCardData[0]==0x4F && cbHandCardData[1]==0x4E)
		{
			cbFirstCardIndex = 2;
		}
		cbTempCardCount = cbHandCardCount-cbFirstCardIndex;
		CopyMemory(cbTempCardData, &cbHandCardData[cbFirstCardIndex], sizeof(BYTE)*cbTempCardCount);
		//ը���͵���
		BYTE cbBombCardData[MAX_COUNT]={0}, cbBombCardCount=0;
		BYTE cbOneCardData[MAX_COUNT]={0}, cbOneCardCount=0;
		GetAllBombCard(cbTempCardData, cbTempCardCount, cbBombCardData, cbBombCardCount);
		GetAllSingleCard(cbTempCardData, cbTempCardCount, cbOneCardData, cbOneCardCount, true);
		SortCardList(cbBombCardData, cbBombCardCount, ST_ORDER);
		SortCardList(cbOneCardData, cbOneCardCount, ST_ORDER);
		ASSERT(cbOneCardCount+cbBombCardCount==cbTempCardCount);
		//�����Դ�ĵ���
		if (IsLargestCardWithOutBomb(wMeChairID, cbOneCardData, 1))
		{
			OutCardResult.cbCardCount = 1;
			OutCardResult.cbCardData[0] = cbOneCardData[0];
			return;
		}
		//ը����С�ڵ��ŵĸ���+1
		if (cbBombCardCount > 0)
		{
			if (cbBombCardCount/2+1 < cbOneCardCount)
			{
				OutCardResult.cbCardCount = 6;
				CopyMemory(OutCardResult.cbCardData, &cbBombCardData[cbBombCardCount-4], sizeof(BYTE)*4);
				CopyMemory(&OutCardResult.cbCardData[4], &cbOneCardData[cbOneCardCount-2], sizeof(BYTE)*2);
				return;
			}
			else
			{
				if (cbOneCardCount >= 3)
				{
					OutCardResult.cbCardCount = 6;
					CopyMemory(OutCardResult.cbCardData, &cbBombCardData[cbBombCardCount-4], sizeof(BYTE)*4);
					CopyMemory(&OutCardResult.cbCardData[4], &cbOneCardData[cbOneCardCount-2], sizeof(BYTE)*2);
					return;
				}
				else
				{
					OutCardResult.cbCardCount = 4;
					CopyMemory(OutCardResult.cbCardData, &cbBombCardData[cbBombCardCount-4], sizeof(BYTE)*4);
					return;
				}
			}
		}
		//������
		for (BYTE i=0; i<cbOneCardCount; i++)
		{
			OutCardResult.cbCardCount = 1;
			OutCardResult.cbCardData[0] = cbOneCardData[0];
			return;
		}
	}
}

//����ֻʣ������ʱ�ĳ��Ʋ���
void CAndroidLogic::AnalyseEnemyLastTwoCard( const WORD wMeChairID, const BYTE cbHandCardData[], const BYTE cbHandCardCount, const tagOutCardTypeResult CardTypeResult[13], tagOutCardResult &OutCardResult )
{
	ZeroMemory(&OutCardResult, sizeof(tagOutCardResult));
	
	//�������
	const WORD wUndersideUser = (m_wBankerUser+1)%GAME_PLAYER;	 //�����¼�
	const WORD wUpsideUser = (wUndersideUser+1)%GAME_PLAYER;	 //�����ϼ�

	bool bEnemyOnlyHaveTwoCard = false;
	if (wMeChairID == m_wBankerUser)
	{
		if (m_cbHandCardCount[wUndersideUser]==2 )
		{
			bEnemyOnlyHaveTwoCard = true;
		}
		if (m_cbHandCardCount[wUpsideUser] == 2)
		{
			bEnemyOnlyHaveTwoCard = true;
		}
	}
	else
	{
		if (m_cbHandCardCount[m_wBankerUser] == 2)
		{
			bEnemyOnlyHaveTwoCard = true;
		}
	}

	if (bEnemyOnlyHaveTwoCard)
	{
		//���㵥��
		BYTE cbSingleCardCount[4] = {MAX_COUNT,MAX_COUNT,MAX_COUNT,MAX_COUNT};
		BYTE cbOutCardIndex[4] = {0};
		BYTE cbOutCardType[4] = {CT_ERROR};
		BYTE cbMinSingleCardCount = MAX_COUNT;
		BYTE cbMinCardType = CT_ERROR;
		BYTE cbMinCardIndex = 0;
		//�����ź�ը����
		for(BYTE cbCardType=CT_THREE; cbCardType<CT_FOUR_TAKE_ONE; ++cbCardType)
		{
			const tagOutCardTypeResult &tempOutCardTypeResult = CardTypeResult[cbCardType];
			//��ͬ���ͣ���ͬ���ȣ���������������ͬ���Ϳ��ܳ��Ȳ�һ��
			for (BYTE i=0; i<tempOutCardTypeResult.cbCardTypeCount; ++i)
			{
				BYTE cbTempCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, tempOutCardTypeResult.cbEachHandCardData[i], tempOutCardTypeResult.cbEachHandCardCount[i]);
				//����cbMinSingleCardCount[4]�����ֵ
				for (BYTE j=0; j<4; ++j)
				{
					if (cbSingleCardCount[j] >= cbTempCount)
					{
						cbSingleCardCount[j] = cbTempCount;
						cbOutCardIndex[j] = i;
						cbOutCardType[j] = cbCardType;
						break;
					}
				}
				//������Сֵ
				if (cbMinSingleCardCount >= cbTempCount)
				{
					//��С����
					cbMinCardType = cbCardType;
					//��С�����е���С����
					cbMinSingleCardCount = cbTempCount;						
					//��С�����е���С��
					cbMinCardIndex = i;
				}
			}
		}
		if (cbMinSingleCardCount != MAX_COUNT)
		{
			BYTE cbCurIndex = cbMinCardIndex;
			//ѡ����С��
			for (BYTE i=0; i<4; ++i)
			{
				if (cbOutCardType[i]==cbMinCardType && cbSingleCardCount[i]<=cbMinSingleCardCount && GetCardLogicValue(CardTypeResult[cbMinCardType].cbEachHandCardData[cbOutCardIndex[i]][0])<GetCardLogicValue(CardTypeResult[cbMinCardType].cbEachHandCardData[cbCurIndex][0]))
				{
					cbCurIndex = cbOutCardIndex[i];
				}
			}
			//���ñ���
			OutCardResult.cbCardCount = CardTypeResult[cbMinCardType].cbEachHandCardCount[cbCurIndex];
			CopyMemory(OutCardResult.cbCardData, CardTypeResult[cbMinCardType].cbEachHandCardData[cbCurIndex], CardTypeResult[cbMinCardType].cbEachHandCardCount[cbCurIndex]*sizeof(BYTE));
			return;
		}

		//�����Ʋ���˵Ķ���
		BYTE cbTempCardData[MAX_COUNT]={0}, cbTempCardCount=0;
		BYTE cbFirstCardIndex = 0;
		if (cbHandCardCount>=2 && cbHandCardData[0]==0x4F && cbHandCardData[1]==0x4E)
		{
			cbFirstCardIndex = 2;
		}
		cbTempCardCount = cbHandCardCount-cbFirstCardIndex;
		CopyMemory(cbTempCardData, &cbHandCardData[cbFirstCardIndex], sizeof(BYTE)*cbTempCardCount);
		SortCardList(cbTempCardData, cbTempCardCount, ST_ORDER);
		tagAnalyseResult MeAnalyseResult;
		AnalysebCardData(cbTempCardData, cbTempCardCount, MeAnalyseResult);
		for (int i=MeAnalyseResult.cbSingleCount-1; i>=0; i--)
		{
			//�����Ǿ��Դ�ĵ���
			if (!IsLargestCardWithOutBomb(wMeChairID,&MeAnalyseResult.cbSingleCardData[i],1))
			{
				OutCardResult.cbCardCount = 1;
				OutCardResult.cbCardData[0] = MeAnalyseResult.cbSingleCardData[i];
				return;
			}
		}
		//����ӳ�
		for (int i=MeAnalyseResult.cbDoubleCount-1; i>=0; i--)
		{
			//���Ǿ��Դ�Ķ���
			if (!IsLargestCardWithOutBomb(wMeChairID,&MeAnalyseResult.cbDoubleCardData[i*2],2))
			{
				OutCardResult.cbCardCount = 1;
				OutCardResult.cbCardData[0] = MeAnalyseResult.cbDoubleCardData[i*2];
				return;
			}
		}
		//ʣ�µ�ȫ�Ǵ��ƿ��Գ�����
		for (int i=MeAnalyseResult.cbSingleCount-1; i>=0; i--)
		{
			OutCardResult.cbCardCount = 1;
			OutCardResult.cbCardData[0] = MeAnalyseResult.cbSingleCardData[i];
			return;
		}
		for (int i=MeAnalyseResult.cbDoubleCount-1; i>=0; i--)
		{
			OutCardResult.cbCardCount = 2;
			CopyMemory(OutCardResult.cbCardData, &MeAnalyseResult.cbDoubleCardData[i*2], sizeof(BYTE)*2);
			return;
		}
		for (int i=MeAnalyseResult.cbFourCount-1; i>=0; i--)
		{
			OutCardResult.cbCardCount = 4;
			CopyMemory(OutCardResult.cbCardData, &MeAnalyseResult.cbFourCardData[i*4], sizeof(BYTE)*4);
			return;
		}
		if (cbHandCardCount==2 && cbHandCardData[0]==0x4F && cbHandCardData[1]==0x4E)
		{
			OutCardResult.cbCardCount = 2;
			OutCardResult.cbCardData[0] = 0x4F;
			OutCardResult.cbCardData[1] = 0x4E;
			return;
		}
	}
}

//��������
void CAndroidLogic::AnalyseOutCardType( const BYTE cbHandCardData[], const BYTE cbHandCardCount, tagOutCardTypeResult OutCardTypeResult[12+1] )
{
	ZeroMemory(OutCardTypeResult, sizeof(tagOutCardTypeResult)*13);
	if (cbHandCardCount==0 || cbHandCardCount>MAX_COUNT) return;

	//�����˿ˣ���ֹ����ʱ�ı��˿�
	BYTE cbReserveCardData[MAX_COUNT] = {0};
	CopyMemory(cbReserveCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);	
	SortCardList(cbReserveCardData, cbHandCardCount, ST_ORDER);

	//��������
	BYTE cbTempCardData[MAX_COUNT] = {0};
	CopyMemory(cbTempCardData, cbReserveCardData, sizeof(BYTE)*cbHandCardCount);
	//��������
	if (cbHandCardCount >= 1)
	{
		BYTE cbSingleCardCount=0, cbSingleCardData[MAX_COUNT]={0};
		GetAllSingleCard(cbTempCardData, cbHandCardCount, cbSingleCardData, cbSingleCardCount, false);
		for (BYTE i=0; i<cbSingleCardCount; i++)
		{
			BYTE cbIndex = OutCardTypeResult[CT_SINGLE].cbCardTypeCount;
			OutCardTypeResult[CT_SINGLE].cbCardType = CT_SINGLE;
			OutCardTypeResult[CT_SINGLE].cbEachHandCardCount[cbIndex] = 1;
			OutCardTypeResult[CT_SINGLE].cbEachHandCardData[cbIndex][0] = cbSingleCardData[i];
			OutCardTypeResult[CT_SINGLE].cbCardTypeCount++;

			ASSERT(OutCardTypeResult[CT_SINGLE].cbCardTypeCount<MAX_TYPE_COUNT);
		}
	}
	//��������
	if (cbHandCardCount >= 2)
	{
		BYTE cbDoubleCardCount=0, cbDoubleCardData[MAX_COUNT]={0};
		GetAllDoubleCard(cbTempCardData, cbHandCardCount, cbDoubleCardData, cbDoubleCardCount);
		for (BYTE i=0; i<cbDoubleCardCount; i+=2)
		{
			BYTE cbIndex = OutCardTypeResult[CT_DOUBLE].cbCardTypeCount;
			OutCardTypeResult[CT_DOUBLE].cbCardType = CT_DOUBLE;
			OutCardTypeResult[CT_DOUBLE].cbEachHandCardCount[cbIndex] = 2;
			OutCardTypeResult[CT_DOUBLE].cbEachHandCardData[cbIndex][0] = cbDoubleCardData[i];
			OutCardTypeResult[CT_DOUBLE].cbEachHandCardData[cbIndex][1] = cbDoubleCardData[i+1];
			OutCardTypeResult[CT_DOUBLE].cbCardTypeCount++;	

			ASSERT(OutCardTypeResult[CT_DOUBLE].cbCardTypeCount<MAX_TYPE_COUNT);
		}
	}
	//��������
	if (cbHandCardCount >= 3)
	{
		BYTE cbThreeCardCount=0, cbThreeCardData[MAX_COUNT]={0};
		GetAllThreeCard(cbTempCardData, cbHandCardCount, cbThreeCardData, cbThreeCardCount);
		for (BYTE i=0; i<cbThreeCardCount; i+=3)
		{
			BYTE cbIndex = OutCardTypeResult[CT_THREE].cbCardTypeCount;
			OutCardTypeResult[CT_THREE].cbCardType = CT_THREE;
			OutCardTypeResult[CT_THREE].cbEachHandCardCount[cbIndex] = 3;
			OutCardTypeResult[CT_THREE].cbEachHandCardData[cbIndex][0] = cbThreeCardData[i];
			OutCardTypeResult[CT_THREE].cbEachHandCardData[cbIndex][1] = cbThreeCardData[i+1];
			OutCardTypeResult[CT_THREE].cbEachHandCardData[cbIndex][2] = cbThreeCardData[i+2];
			OutCardTypeResult[CT_THREE].cbCardTypeCount++;	

			ASSERT(OutCardTypeResult[CT_THREE].cbCardTypeCount<MAX_TYPE_COUNT);
		}
	}
	//ը������
	if (cbHandCardCount >= 2)
	{
		BYTE cbFourCardCount=0, cbFourCardData[MAX_COUNT]={0};
		if (cbHandCardCount>=2 && cbTempCardData[0]==0x4F && cbTempCardData[1]==0x4E)
		{
			BYTE cbIndex = OutCardTypeResult[CT_BOMB_CARD].cbCardTypeCount;
			OutCardTypeResult[CT_BOMB_CARD].cbCardType = CT_BOMB_CARD;
			OutCardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[cbIndex] = 2;
			OutCardTypeResult[CT_BOMB_CARD].cbEachHandCardData[cbIndex][0] = cbTempCardData[0];
			OutCardTypeResult[CT_BOMB_CARD].cbEachHandCardData[cbIndex][1] = cbTempCardData[1];
			OutCardTypeResult[CT_BOMB_CARD].cbCardTypeCount++;	
			GetAllBombCard(cbTempCardData+2, cbHandCardCount-2, cbFourCardData, cbFourCardCount);
		}
		else 
		{
			GetAllBombCard(cbTempCardData, cbHandCardCount, cbFourCardData, cbFourCardCount);
		}
		for (BYTE i=0; i<cbFourCardCount; i+=4)
		{
			BYTE cbIndex = OutCardTypeResult[CT_BOMB_CARD].cbCardTypeCount;
			OutCardTypeResult[CT_BOMB_CARD].cbCardType = CT_BOMB_CARD;
			OutCardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[cbIndex] = 4;
			OutCardTypeResult[CT_BOMB_CARD].cbEachHandCardData[cbIndex][0] = cbFourCardData[i];
			OutCardTypeResult[CT_BOMB_CARD].cbEachHandCardData[cbIndex][1] = cbFourCardData[i+1];
			OutCardTypeResult[CT_BOMB_CARD].cbEachHandCardData[cbIndex][2] = cbFourCardData[i+2];
			OutCardTypeResult[CT_BOMB_CARD].cbEachHandCardData[cbIndex][3] = cbFourCardData[i+3];
			OutCardTypeResult[CT_BOMB_CARD].cbCardTypeCount++;	

			ASSERT(OutCardTypeResult[CT_BOMB_CARD].cbCardTypeCount<MAX_TYPE_COUNT);
		}
	}

	//˳������
	if (cbHandCardCount >= 5)
	{
		//�ָ��˿ˣ���ֹ����ʱ�ı��˿�
		ZeroMemory(cbTempCardData, sizeof(cbTempCardData));
		BYTE cbLeftCardCount = 0;
		//ȥ��2����
		for (BYTE i=0; i<cbHandCardCount; ++i)
		{
			if (GetCardLogicValue(cbReserveCardData[i]) < 15)
			{
				cbTempCardData[cbLeftCardCount++] = cbReserveCardData[i];
			}
		}
		SortCardList(cbTempCardData, cbLeftCardCount, ST_ORDER);

		bool bFindSingleLine = true;
		BYTE cbSingleLineCount=1, cbSingleLineCard[12]={0};
		//�����ж�
		while (cbLeftCardCount>=5 && bFindSingleLine)
		{
			bFindSingleLine = false;
			cbSingleLineCount = 1;
			cbSingleLineCard[cbSingleLineCount-1] = cbTempCardData[0];
			BYTE cbLastCard = cbTempCardData[0];
			for (BYTE i=1; i<cbLeftCardCount; i++)
			{
				BYTE cbCardData = cbTempCardData[i];
				//�����ж�
				if ((GetCardLogicValue(cbLastCard)-GetCardLogicValue(cbCardData))!=1 && GetCardValue(cbLastCard)!=GetCardValue(cbCardData)) 
				{
					cbLastCard = cbTempCardData[i];
					//�Ƿ�Ϸ�
					if (cbSingleLineCount < 5) 
					{
						cbSingleLineCount = 1;
						cbSingleLineCard[cbSingleLineCount-1] = cbTempCardData[i];
						continue;
					}
					else
					{
						break;
					}
				}
				//ͬ���ж�
				else if (GetCardValue(cbLastCard) != GetCardValue(cbCardData))
				{
					cbLastCard = cbCardData;
					cbSingleLineCard[cbSingleLineCount] = cbCardData;
					++cbSingleLineCount;
				}
			}

			//��������
			if (cbSingleLineCount >= 5)
			{
				for (BYTE i=5; i<=cbSingleLineCount; i++)
				{
					for (BYTE j=0; j<=cbSingleLineCount-i; j++)
					{
						BYTE cbIndex = OutCardTypeResult[CT_SINGLE_LINE].cbCardTypeCount;
						OutCardTypeResult[CT_SINGLE_LINE].cbCardType = CT_SINGLE_LINE;
						OutCardTypeResult[CT_SINGLE_LINE].cbEachHandCardCount[cbIndex] = i;
						CopyMemory(OutCardTypeResult[CT_SINGLE_LINE].cbEachHandCardData[cbIndex], &cbSingleLineCard[j], sizeof(BYTE)*i);
						OutCardTypeResult[CT_SINGLE_LINE].cbCardTypeCount++;

						ASSERT(OutCardTypeResult[CT_SINGLE_LINE].cbCardTypeCount<MAX_TYPE_COUNT);
					}
				}
				//ɾ�����ҳ���˳��
				bFindSingleLine = true;
				RemoveCard(cbSingleLineCard, cbSingleLineCount, cbTempCardData, cbLeftCardCount);
				cbLeftCardCount -= cbSingleLineCount;
			}
		}
	}

	//˫˳����
	if (cbHandCardCount >= 6)
	{
		ZeroMemory(cbTempCardData, sizeof(cbTempCardData));
		//�ָ��˿ˣ���ֹ����ʱ�ı��˿�
		BYTE cbLeftCardCount = 0;
		//ȥ��2����
		for (BYTE i=0; i<cbHandCardCount; ++i)
		{
			if (GetCardLogicValue(cbReserveCardData[i]) < 15)	
			{
				cbTempCardData[cbLeftCardCount++] = cbReserveCardData[i]; 
			}
		}
		SortCardList(cbTempCardData, cbLeftCardCount, ST_ORDER);

		bool bFindDoubleLine = true;
		BYTE cbDoubleLineCount=0, cbDoubleLineCard[MAX_COUNT]={0};
		//��ʼ�ж�
		while (cbLeftCardCount>=6 && bFindDoubleLine)
		{
			BYTE cbLastCard = cbTempCardData[0];
			BYTE cbSameCount = 1;
			cbDoubleLineCount = 0;
			bFindDoubleLine = false;
			for (BYTE i=1; i<cbLeftCardCount; ++i)
			{
				//����ͬ��
				while (i<cbLeftCardCount && GetCardLogicValue(cbLastCard)==GetCardLogicValue(cbTempCardData[i]))
				{
					++cbSameCount;
					++i;
				}

				BYTE cbLastDoubleCardValue = (cbDoubleLineCount>0) ? GetCardLogicValue(cbDoubleLineCard[cbDoubleLineCount-1]) : 0;
				//���¿�ʼ
				if ((cbSameCount<2||(cbDoubleLineCount>0&&(cbLastDoubleCardValue-GetCardLogicValue(cbLastCard))!=1)) && i<=cbLeftCardCount)
				{
					if (cbDoubleLineCount >= 6) break;
					//����
					if (cbSameCount >= 2) i -= cbSameCount;
					cbLastCard = cbTempCardData[i];
					cbDoubleLineCount = 0;
				}
				//��������
				else if (cbSameCount >= 2)
				{
					cbDoubleLineCard[cbDoubleLineCount] = cbTempCardData[i-cbSameCount];
					cbDoubleLineCard[cbDoubleLineCount+1] = cbTempCardData[i-cbSameCount+1];
					cbDoubleLineCount += 2;
					//��β�ж�
					if (i == cbLeftCardCount-2)
					{
						if (GetCardLogicValue(cbLastCard)-GetCardLogicValue(cbTempCardData[i])==1 && GetCardLogicValue(cbTempCardData[i])==GetCardLogicValue(cbTempCardData[i+1]))
						{
							cbDoubleLineCard[cbDoubleLineCount] = cbTempCardData[i];
							cbDoubleLineCard[cbDoubleLineCount+1] = cbTempCardData[i+1];
							cbDoubleLineCount += 2;
							break;
						}
					}
				}
				cbLastCard = cbTempCardData[i];
				cbSameCount = 1;
			}

			//��������
			if (cbDoubleLineCount >= 6)
			{
				for (BYTE i=6; i<=cbDoubleLineCount; i+=2)
				{
					for (BYTE j=0; j<=cbDoubleLineCount-i; j+=2)
					{
						BYTE cbIndex = OutCardTypeResult[CT_DOUBLE_LINE].cbCardTypeCount;
						OutCardTypeResult[CT_DOUBLE_LINE].cbCardType = CT_DOUBLE_LINE;
						OutCardTypeResult[CT_DOUBLE_LINE].cbEachHandCardCount[cbIndex] = i;
						CopyMemory(OutCardTypeResult[CT_DOUBLE_LINE].cbEachHandCardData[cbIndex], &cbDoubleLineCard[j], sizeof(BYTE)*i);
						OutCardTypeResult[CT_DOUBLE_LINE].cbCardTypeCount++;

						ASSERT(OutCardTypeResult[CT_DOUBLE_LINE].cbCardTypeCount<MAX_TYPE_COUNT);
					}
				}
				//ɾ�����ҵ���˫˳
				bFindDoubleLine = true;
				RemoveCard(cbDoubleLineCard, cbDoubleLineCount, cbTempCardData, cbLeftCardCount);				
				cbLeftCardCount -= cbDoubleLineCount;
			}
		}
	}

	//�ɻ�����
	if (cbHandCardCount >= 6)
	{
		ZeroMemory(cbTempCardData, sizeof(cbTempCardData));
		//�ָ��˿ˣ���ֹ����ʱ�ı��˿�
		BYTE cbLeftCardCount = 0;
		//ȥ��2����
		for (BYTE i=0; i<cbHandCardCount; ++i)	
		{
			if (GetCardLogicValue(cbReserveCardData[i]) <15 )
			{
				cbTempCardData[cbLeftCardCount++] = cbReserveCardData[i];
			}
		}
		SortCardList(cbTempCardData, cbLeftCardCount, ST_ORDER);

		bool bFindThreeLine = true;
		BYTE cbThreeLineCount=0, cbThreeLineCard[MAX_COUNT]={0};
		//��ʼ�ж�
		while (cbLeftCardCount>=6 && bFindThreeLine)
		{
			BYTE cbLastCard = cbTempCardData[0];
			BYTE cbSameCount = 1;
			cbThreeLineCount = 0;
			bFindThreeLine = false;
			for (BYTE i=1; i<cbLeftCardCount; ++i)
			{
				//����ͬ��
				while (i<cbLeftCardCount && GetCardLogicValue(cbLastCard)==GetCardLogicValue(cbTempCardData[i]))
				{
					++cbSameCount;
					++i;
				}

				BYTE cbLastThreeCardValue = (cbThreeLineCount>0) ? GetCardLogicValue(cbThreeLineCard[cbThreeLineCount-1]) : 0;
				//���¿�ʼ
				if ((cbSameCount<3||(cbThreeLineCount>0&&(cbLastThreeCardValue-GetCardLogicValue(cbLastCard))!=1)) && i<=cbLeftCardCount)
				{
					if (cbThreeLineCount>=6) break;
					if (cbSameCount>=3) i -= cbSameCount;
					cbLastCard = cbTempCardData[i];
					cbThreeLineCount = 0;
				}
				//��������
				else if (cbSameCount >= 3)
				{
					cbThreeLineCard[cbThreeLineCount] = cbTempCardData[i-cbSameCount];
					cbThreeLineCard[cbThreeLineCount+1] = cbTempCardData[i-cbSameCount+1];
					cbThreeLineCard[cbThreeLineCount+2] = cbTempCardData[i-cbSameCount+2];
					cbThreeLineCount += 3;
					//��β�ж�
					if (i == cbLeftCardCount-3)
					{
						if (GetCardLogicValue(cbLastCard)-GetCardLogicValue(cbTempCardData[i])==1 && GetCardLogicValue(cbTempCardData[i])==GetCardLogicValue(cbTempCardData[i+1]) && GetCardLogicValue(cbTempCardData[i])==GetCardLogicValue(cbTempCardData[i+2]))
						{
							cbThreeLineCard[cbThreeLineCount] = cbTempCardData[i];
							cbThreeLineCard[cbThreeLineCount+1] = cbTempCardData[i+1];
							cbThreeLineCard[cbThreeLineCount+2] = cbTempCardData[i+2];
							cbThreeLineCount += 3;
							break;
						}
					}
				}

				cbLastCard = cbTempCardData[i];
				cbSameCount = 1;
			}

			//��������
			if (cbThreeLineCount >= 6)
			{
				for (BYTE i=6; i<=cbThreeLineCount; i+=3)
				{
					for (BYTE j=0; j<=cbThreeLineCount-i; j+=3)
					{
						BYTE cbIndex = OutCardTypeResult[CT_THREE_LINE].cbCardTypeCount;
						OutCardTypeResult[CT_THREE_LINE].cbCardType = CT_THREE_LINE;
						OutCardTypeResult[CT_THREE_LINE].cbEachHandCardCount[cbIndex] = i;
						CopyMemory(OutCardTypeResult[CT_THREE_LINE].cbEachHandCardData[cbIndex], &cbThreeLineCard[j], sizeof(BYTE)*i);
						OutCardTypeResult[CT_THREE_LINE].cbCardTypeCount++;

						ASSERT(OutCardTypeResult[CT_THREE_LINE].cbCardTypeCount<MAX_TYPE_COUNT);
					}
				}
				//ɾ�����ҵ�����˳
				bFindThreeLine = true;
				RemoveCard(cbThreeLineCard, cbThreeLineCount, cbTempCardData, cbLeftCardCount);
				cbLeftCardCount -= cbThreeLineCount;
			}
		}
	}

	//����һ������������
	{
		//�ָ��˿ˣ���ֹ����ʱ�ı��˿�
		CopyMemory(cbTempCardData, cbReserveCardData, cbHandCardCount);

		BYTE cbBombCardCount=0, cbBombCardData[MAX_COUNT]={0};
		GetAllBombCard(cbTempCardData, cbHandCardCount, cbBombCardData, cbBombCardCount);
		RemoveCard(cbBombCardData, cbBombCardCount, cbTempCardData, cbHandCardCount);
		BYTE cbThreeCardCount=0, cbThreeCardData[MAX_COUNT]={0};
		GetAllThreeCard(cbTempCardData, cbHandCardCount-cbBombCardCount, cbThreeCardData, cbThreeCardCount);

		//����һ��
		{
			//ȥ������
			BYTE cbRemainCardData[MAX_COUNT]={0}, cbRemainCardCount=cbHandCardCount-cbBombCardCount;
			CopyMemory(cbRemainCardData, cbTempCardData, sizeof(BYTE)*cbRemainCardCount);
			RemoveCard(cbThreeCardData, cbThreeCardCount, cbRemainCardData, cbRemainCardCount);
			cbRemainCardCount -= cbThreeCardCount;
			//��ȡ���еĵ���
			BYTE cbSingleCardCount=0, cbSingleCardData[MAX_COUNT]={0};
			GetAllSingleCard(cbRemainCardData, cbRemainCardCount, cbSingleCardData, cbSingleCardCount, false);
			//������һ��
			for (BYTE i=0; i<cbThreeCardCount; i+=3)
			{
				//������һ��
				for (BYTE j=0; j<cbSingleCardCount; ++j)
				{
					BYTE cbIndex = OutCardTypeResult[CT_THREE_TAKE_ONE].cbCardTypeCount;
					OutCardTypeResult[CT_THREE_TAKE_ONE].cbCardType = CT_THREE_TAKE_ONE;
					OutCardTypeResult[CT_THREE_TAKE_ONE].cbEachHandCardCount[cbIndex] = 4;
					OutCardTypeResult[CT_THREE_TAKE_ONE].cbEachHandCardData[cbIndex][0] = cbThreeCardData[i];
					OutCardTypeResult[CT_THREE_TAKE_ONE].cbEachHandCardData[cbIndex][1] = cbThreeCardData[i+1];
					OutCardTypeResult[CT_THREE_TAKE_ONE].cbEachHandCardData[cbIndex][2] = cbThreeCardData[i+2];
					OutCardTypeResult[CT_THREE_TAKE_ONE].cbEachHandCardData[cbIndex][3] = cbSingleCardData[j];
					OutCardTypeResult[CT_THREE_TAKE_ONE].cbCardTypeCount++;
				}			
			}
		}

		//��������
		{
			bool bFindThreeLine = true;
			BYTE cbLeftThreeCardCount = cbThreeCardCount;
			BYTE cbLastCardIndex = (cbThreeCardCount>0&&GetCardLogicValue(cbThreeCardData[0])==15) ? 3 : 0;
			while (cbLeftThreeCardCount>=6 && bFindThreeLine)
			{
				bFindThreeLine = false;
				BYTE cbLastCardLogicValue = GetCardLogicValue(cbThreeCardData[cbLastCardIndex]);
				BYTE cbThreeLineCardCount=3, cbThreeLineCardData[MAX_COUNT]={0};
				cbThreeLineCardData[0] = cbThreeCardData[cbLastCardIndex];
				cbThreeLineCardData[1] = cbThreeCardData[cbLastCardIndex+1];
				cbThreeLineCardData[2] = cbThreeCardData[cbLastCardIndex+2];
				for (BYTE j=3+cbLastCardIndex; j<cbLeftThreeCardCount; j+=3)
				{
					//�����ж�
					if (cbLastCardLogicValue-GetCardLogicValue(cbThreeCardData[j]) != 1)
					{
						cbLastCardIndex = j;
						if (cbLeftThreeCardCount-j >= 6)
						{
							bFindThreeLine = true;
						}
						break;
					}

					cbLastCardLogicValue = GetCardLogicValue(cbThreeCardData[j]);
					cbThreeLineCardData[cbThreeLineCardCount] = cbThreeCardData[j];
					cbThreeLineCardData[cbThreeLineCardCount+1] = cbThreeCardData[j+1];
					cbThreeLineCardData[cbThreeLineCardCount+2] = cbThreeCardData[j+2];
					cbThreeLineCardCount += 3;
				}
				if (cbThreeLineCardCount > 3)
				{
					BYTE cbRemainCardData[MAX_COUNT]={0}, cbRemainCardCount=cbHandCardCount-cbBombCardCount;
					CopyMemory(cbRemainCardData, cbTempCardData, sizeof(BYTE)*cbRemainCardCount);
					RemoveCard(cbThreeCardData, cbThreeCardCount, cbRemainCardData, cbRemainCardCount);
					cbRemainCardCount -= cbThreeCardCount;

					for (BYTE start=0; start<cbThreeLineCardCount-3; start+=3)
					{
						//��˳��Ŀ
						BYTE cbThisTreeLineCardCount = cbThreeLineCardCount-start;
						//���Ƹ���
						BYTE cbSingleCardCount = (cbThisTreeLineCardCount)/3;
						//���Ʋ���
						if (cbRemainCardCount < cbSingleCardCount) continue;

						//�������
						BYTE cbComCard[5] = {0};
						BYTE cbComResCard[254][5] = {0};
						BYTE cbComResLen = 0;
						Combination(cbComCard, 0, cbComResCard, cbComResLen, cbRemainCardData, cbSingleCardCount, cbRemainCardCount, cbSingleCardCount);
						for (BYTE i=0; i<cbComResLen; ++i)
						{
							BYTE cbIndex = OutCardTypeResult[CT_THREE_TAKE_ONE].cbCardTypeCount;
							OutCardTypeResult[CT_THREE_TAKE_ONE].cbCardType = CT_THREE_TAKE_ONE;
							OutCardTypeResult[CT_THREE_TAKE_ONE].cbEachHandCardCount[cbIndex] = cbThisTreeLineCardCount+cbSingleCardCount;
							CopyMemory(OutCardTypeResult[CT_THREE_TAKE_ONE].cbEachHandCardData[cbIndex], cbThreeLineCardData+start, sizeof(BYTE)*cbThisTreeLineCardCount);
							CopyMemory(OutCardTypeResult[CT_THREE_TAKE_ONE].cbEachHandCardData[cbIndex]+cbThisTreeLineCardCount, cbComResCard[i], cbSingleCardCount);
							OutCardTypeResult[CT_THREE_TAKE_ONE].cbCardTypeCount++;

							ASSERT(OutCardTypeResult[CT_THREE_TAKE_ONE].cbCardTypeCount<MAX_TYPE_COUNT);
						}
					}

					//�Ƴ�����
					bFindThreeLine = true;
					RemoveCard(cbThreeLineCardData, cbThreeLineCardCount, cbThreeCardData, cbLeftThreeCardCount);
					cbLeftThreeCardCount -= cbThreeLineCardCount;
					cbLastCardIndex = (cbLeftThreeCardCount>0&&GetCardLogicValue(cbThreeCardData[0])==15) ? 3 : 0;
				}
			}
		}
	}

	//����һ�Ի���������
	{
		//�ָ��˿ˣ���ֹ����ʱ�ı��˿�
		CopyMemory(cbTempCardData, cbReserveCardData, cbHandCardCount);

		//��ȡը��
		BYTE cbBombCardCount=0, cbBombCardData[MAX_COUNT]={0};
		GetAllBombCard(cbTempCardData, cbHandCardCount, cbBombCardData, cbBombCardCount);
		RemoveCard(cbBombCardData, cbBombCardCount, cbTempCardData, cbHandCardCount);
		//��ȡ����
		BYTE cbThreeCardCount=0, cbThreeCardData[MAX_COUNT]={0};
		GetAllThreeCard(cbTempCardData, cbHandCardCount-cbBombCardCount, cbThreeCardData, cbThreeCardCount);
		RemoveCard(cbThreeCardData, cbThreeCardCount, cbTempCardData, cbHandCardCount-cbBombCardCount);
		//��ȡʣ�����
		BYTE cbRemainCardCount=0, cbRemainCarData[MAX_COUNT]={0};
		CopyMemory(cbRemainCarData, cbTempCardData, cbHandCardCount-cbBombCardCount-cbThreeCardCount);
		cbRemainCardCount = cbHandCardCount-cbBombCardCount-cbThreeCardCount;
		//��ȡ����
		BYTE cbDoubleCardCount=0, cbDoubleCardData[MAX_COUNT]={0};
		GetAllDoubleCard(cbRemainCarData, cbRemainCardCount, cbDoubleCardData, cbDoubleCardCount);

		//������һ��
		for (BYTE i=0; i<cbThreeCardCount; i+=3)
		{
			for (BYTE j=0; j<cbDoubleCardCount; j+=2)
			{
				BYTE cbIndex = OutCardTypeResult[CT_THREE_TAKE_TWO].cbCardTypeCount;
				OutCardTypeResult[CT_THREE_TAKE_TWO].cbCardType = CT_THREE_TAKE_TWO;
				OutCardTypeResult[CT_THREE_TAKE_TWO].cbEachHandCardCount[cbIndex] = 5;
				OutCardTypeResult[CT_THREE_TAKE_TWO].cbEachHandCardData[cbIndex][0] = cbThreeCardData[i];
				OutCardTypeResult[CT_THREE_TAKE_TWO].cbEachHandCardData[cbIndex][1] = cbThreeCardData[i+1];
				OutCardTypeResult[CT_THREE_TAKE_TWO].cbEachHandCardData[cbIndex][2] = cbThreeCardData[i+2];
				OutCardTypeResult[CT_THREE_TAKE_TWO].cbEachHandCardData[cbIndex][3] = cbDoubleCardData[j];
				OutCardTypeResult[CT_THREE_TAKE_TWO].cbEachHandCardData[cbIndex][4] = cbDoubleCardData[j+1];
				OutCardTypeResult[CT_THREE_TAKE_TWO].cbCardTypeCount++;
			}	
		}
		//��������	
		{
			bool bFindThreeLine = true;
			BYTE cbLeftThreeCardCount = cbThreeCardCount;
			BYTE cbLastCardIndex = (cbThreeCardCount>0&&GetCardLogicValue(cbThreeCardData[0])==15) ? 3 : 0;
			while (cbLeftThreeCardCount>=6 && bFindThreeLine)
			{
				BYTE cbLastCardLogicValue = GetCardLogicValue(cbThreeCardData[cbLastCardIndex]);
				BYTE cbThreeLineCardCount=3, cbThreeLineCard[MAX_COUNT]={0};
				cbThreeLineCard[0] = cbThreeCardData[cbLastCardIndex];
				cbThreeLineCard[1] = cbThreeCardData[cbLastCardIndex+1];
				cbThreeLineCard[2] = cbThreeCardData[cbLastCardIndex+2];
				bFindThreeLine = false;
				for (BYTE j=3+cbLastCardIndex; j<cbLeftThreeCardCount; j+=3)
				{
					//�����ж�
					if (1 != cbLastCardLogicValue-GetCardLogicValue(cbThreeCardData[j]))
					{
						cbLastCardIndex = j;
						if (cbLeftThreeCardCount-j>=6) bFindThreeLine = true;
						break;
					}

					cbLastCardLogicValue = GetCardLogicValue(cbThreeCardData[j]);
					cbThreeLineCard[cbThreeLineCardCount] = cbThreeCardData[j];
					cbThreeLineCard[cbThreeLineCardCount+1] = cbThreeCardData[j+1];
					cbThreeLineCard[cbThreeLineCardCount+2] = cbThreeCardData[j+2];
					cbThreeLineCardCount += 3;
				}
				if (cbThreeLineCardCount > 3)
				{
					for (BYTE start=0; start<cbThreeLineCardCount-3; start+=3)
					{
						//��˳��Ŀ
						BYTE cbThisTreeLineCardCount = cbThreeLineCardCount-start;
						//��������
						BYTE cbDoubleCardCount = ((cbThisTreeLineCardCount)/3);
						//���Ʋ���
						if (cbRemainCardCount < cbDoubleCardCount) continue;

						BYTE cbDoubleCardIndex[10] = {0}; //�����±�
						for (BYTE i=0,j=0; i<cbDoubleCardCount; i+=2, ++j)
						{
							cbDoubleCardIndex[j] = i;
						}

						//�������
						BYTE cbComCard[5] = {0};
						BYTE cbComResCard[254][5] = {0};
						BYTE cbComResLen = 0;
						//���ö��Ƶ��±�����ϣ��ٸ����±���ȡ������
						Combination(cbComCard, 0, cbComResCard, cbComResLen, cbDoubleCardIndex, cbDoubleCardCount, cbDoubleCardCount/2, cbDoubleCardCount);
						ASSERT(cbComResLen<=254);
						for (BYTE i=0; i<cbComResLen; ++i)
						{
							BYTE cbIndex = OutCardTypeResult[CT_THREE_TAKE_TWO].cbCardTypeCount;
							OutCardTypeResult[CT_THREE_TAKE_TWO].cbCardType = CT_THREE_TAKE_TWO;
							OutCardTypeResult[CT_THREE_TAKE_TWO].cbEachHandCardCount[cbIndex] = cbThisTreeLineCardCount+2*cbDoubleCardCount;
							//��������
							CopyMemory(OutCardTypeResult[CT_THREE_TAKE_TWO].cbEachHandCardData[cbIndex], cbThreeLineCard+start, sizeof(BYTE)*cbThisTreeLineCardCount);
							//�������
							for (BYTE j=0,k=0; j<cbDoubleCardCount; ++j,k+=2)
							{
								OutCardTypeResult[CT_THREE_TAKE_TWO].cbEachHandCardData[cbIndex][cbThisTreeLineCardCount+k] = cbDoubleCardData[cbComResCard[i][j]];
								OutCardTypeResult[CT_THREE_TAKE_TWO].cbEachHandCardData[cbIndex][cbThisTreeLineCardCount+k+1] = cbDoubleCardData[cbComResCard[i][j]+1];
							}
							OutCardTypeResult[CT_THREE_TAKE_TWO].cbCardTypeCount++;

							ASSERT(OutCardTypeResult[CT_THREE_TAKE_TWO].cbCardTypeCount<MAX_TYPE_COUNT);
						}
					}
					//�Ƴ�����	
					bFindThreeLine = true;	
					RemoveCard(cbThreeLineCard, cbThreeLineCardCount, cbThreeCardData, cbLeftThreeCardCount);
					cbLeftThreeCardCount -= cbThreeLineCardCount;
					cbLastCardIndex = (cbLeftThreeCardCount>0&&GetCardLogicValue(cbThreeCardData[0])==15) ? 3 : 0;
				}
			}
		}
	}
}

//��������
void CAndroidLogic::AnalyseOutCardType( const BYTE cbHandCardData[], const BYTE cbHandCardCount, const BYTE cbTurnCardData[], const BYTE cbTurnCardCount, tagOutCardTypeResult OutCardTypeResult[12+1] )
{
	ZeroMemory(OutCardTypeResult, sizeof(tagOutCardTypeResult)*13);
	
	BYTE cbTurnCardType = GetCardType(cbTurnCardData, cbTurnCardCount);
	ASSERT(cbTurnCardType!=CT_ERROR);
	if (cbTurnCardType==CT_ERROR) return;

	BYTE cbTempCardData[MAX_COUNT] = {0};
	CopyMemory(cbTempCardData, cbHandCardData, cbHandCardCount);
	SortCardList(cbTempCardData, cbHandCardCount, ST_ORDER);

	//��ը������ʱ����Ѱ��ը��
	if (cbTurnCardType!=CT_MISSILE_CARD && cbTurnCardType!=CT_BOMB_CARD)
	{
		BYTE cbBombCardCount=0, cbBombCardData[MAX_COUNT]={0};
		//˫��ը��
		if (cbHandCardCount>=2 && cbTempCardData[0]==0x4F && cbTempCardData[1]==0x4E)
		{
			BYTE cbIndex = OutCardTypeResult[CT_BOMB_CARD].cbCardTypeCount;
			OutCardTypeResult[CT_BOMB_CARD].cbCardType = CT_BOMB_CARD;
			OutCardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[cbIndex] = 2;
			OutCardTypeResult[CT_BOMB_CARD].cbEachHandCardData[cbIndex][0] = cbTempCardData[0];
			OutCardTypeResult[CT_BOMB_CARD].cbEachHandCardData[cbIndex][1] = cbTempCardData[1];
			OutCardTypeResult[CT_BOMB_CARD].cbCardTypeCount++;
			GetAllBombCard(cbTempCardData+2, cbHandCardCount-2, cbBombCardData, cbBombCardCount);
		}
		//ը������
		else
		{
			GetAllBombCard(cbTempCardData, cbHandCardCount, cbBombCardData, cbBombCardCount);
		}
		//����ը��
		for (BYTE i=0; i<cbBombCardCount/4; ++i)
		{
			BYTE cbIndex = OutCardTypeResult[CT_BOMB_CARD].cbCardTypeCount;
			OutCardTypeResult[CT_BOMB_CARD].cbCardType = CT_BOMB_CARD;
			OutCardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[cbIndex] = 4;
			CopyMemory(OutCardTypeResult[CT_BOMB_CARD].cbEachHandCardData[cbIndex], cbBombCardData+4*i, 4);
			OutCardTypeResult[CT_BOMB_CARD].cbCardTypeCount++;

			ASSERT(OutCardTypeResult[CT_BOMB_CARD].cbCardTypeCount<=MAX_TYPE_COUNT);
		}
	}

	switch(cbTurnCardType)
	{
	case CT_SINGLE:				//��������
		{
			BYTE cbSingleCardCount=0, cbSingleCardData[MAX_COUNT]={0};
			GetAllSingleCard(cbTempCardData, cbHandCardCount, cbSingleCardData, cbSingleCardCount, false);
			for (BYTE i=0; i<cbSingleCardCount; ++i) 
			{
				if (GetCardLogicValue(cbSingleCardData[i]) > GetCardLogicValue(cbTurnCardData[0])) 
				{
					BYTE cbIndex = OutCardTypeResult[CT_SINGLE].cbCardTypeCount;
					OutCardTypeResult[CT_SINGLE].cbCardType = CT_SINGLE;
					OutCardTypeResult[CT_SINGLE].cbEachHandCardCount[cbIndex] = 1;
					OutCardTypeResult[CT_SINGLE].cbEachHandCardData[cbIndex][0] = cbSingleCardData[i];
					OutCardTypeResult[CT_SINGLE].cbCardTypeCount++;

					ASSERT(OutCardTypeResult[CT_SINGLE].cbCardTypeCount<=MAX_TYPE_COUNT);
				}
			}
		}break;
	case CT_DOUBLE:				//��������
		{
			BYTE cbDoubleCardCount=0, cbDoubleCardData[MAX_COUNT]={0};
			GetAllDoubleCard(cbHandCardData, cbHandCardCount, cbDoubleCardData, cbDoubleCardCount);
			for (BYTE i=0; i<cbDoubleCardCount; i+=2)
			{
				if (GetCardLogicValue(cbDoubleCardData[i]) > GetCardLogicValue(cbTurnCardData[0]))
				{
					BYTE cbIndex = OutCardTypeResult[CT_DOUBLE].cbCardTypeCount;
					OutCardTypeResult[CT_DOUBLE].cbCardType = CT_DOUBLE;
					OutCardTypeResult[CT_DOUBLE].cbEachHandCardCount[cbIndex] = 2;
					OutCardTypeResult[CT_DOUBLE].cbEachHandCardData[cbIndex][0] = cbDoubleCardData[i];
					OutCardTypeResult[CT_DOUBLE].cbEachHandCardData[cbIndex][1] = cbDoubleCardData[i+1];
					OutCardTypeResult[CT_DOUBLE].cbCardTypeCount++;

					ASSERT(OutCardTypeResult[CT_DOUBLE].cbCardTypeCount<=MAX_TYPE_COUNT);
				}
			}
		}break;
	case CT_THREE:				//��������
		{
			BYTE cbThreeCardCount=0, cbThreeCardData[MAX_COUNT]={0};
			GetAllThreeCard(cbHandCardData, cbHandCardCount, cbThreeCardData, cbThreeCardCount);
			for (BYTE i=0; i<cbThreeCardCount; i+=3)
			{
				if (GetCardLogicValue(cbThreeCardData[i]) > GetCardLogicValue(cbTurnCardData[0]))
				{
					BYTE Index = OutCardTypeResult[CT_THREE].cbCardTypeCount;
					OutCardTypeResult[CT_THREE].cbCardType = CT_THREE;
					OutCardTypeResult[CT_THREE].cbEachHandCardCount[Index] = 3;
					OutCardTypeResult[CT_THREE].cbEachHandCardData[Index][0] = cbThreeCardData[i];
					OutCardTypeResult[CT_THREE].cbEachHandCardData[Index][1] = cbThreeCardData[i+1];
					OutCardTypeResult[CT_THREE].cbEachHandCardData[Index][2] = cbThreeCardData[i+2];
					OutCardTypeResult[CT_THREE].cbCardTypeCount++;

					ASSERT(OutCardTypeResult[CT_THREE].cbCardTypeCount<=MAX_TYPE_COUNT);
				}
			}
		}break;
	case CT_SINGLE_LINE:		//��������
		{
			ZeroMemory(cbTempCardData, sizeof(cbTempCardData));
			BYTE cbLeftCardCount = 0;
			//ȥ��2����
			for (BYTE i=0; i<cbHandCardCount; ++i)
			{
				if (GetCardLogicValue(cbHandCardData[i]) < 15)
				{
					cbTempCardData[cbLeftCardCount++] = cbHandCardData[i];
				}
			}
			SortCardList(cbTempCardData, cbLeftCardCount, ST_ORDER);

			bool bFindSingleLine = true;
			BYTE cbSingleLineCount=1, cbSingleLineCard[12]={0};
			//�����ж�
			while (cbLeftCardCount>=cbTurnCardCount && bFindSingleLine)
			{	
				bFindSingleLine = false;
				cbSingleLineCount = 1;
				cbSingleLineCard[cbSingleLineCount-1] = cbTempCardData[0];
				BYTE cbLastCard = cbTempCardData[0];
				for (BYTE i=1; i<cbLeftCardCount; i++)
				{
					BYTE cbCardData = cbTempCardData[i];
					//�����ж�
					if (GetCardLogicValue(cbLastCard)-GetCardLogicValue(cbCardData)!=1 && GetCardValue(cbLastCard)!=GetCardValue(cbCardData)) 
					{
						cbLastCard = cbTempCardData[i];
						//�Ƿ�Ϸ�
						if (cbSingleLineCount < cbTurnCardCount) 
						{
							cbSingleLineCount = 1;
							cbSingleLineCard[cbSingleLineCount-1] = cbTempCardData[i];
							continue;
						}
						else
						{
							break;
						}
					}
					//ͬ���ж�
					else if (GetCardValue(cbLastCard) != GetCardValue(cbCardData))
					{
						cbLastCard = cbCardData;
						cbSingleLineCard[cbSingleLineCount] = cbCardData;
						++cbSingleLineCount;
					}
				}

				//��������
				if (cbSingleLineCount>=cbTurnCardCount && GetCardLogicValue(cbSingleLineCard[0])>GetCardLogicValue(cbTurnCardData[0]))
				{
					BYTE cbStart = 0;
					//��������
					while (GetCardLogicValue(cbSingleLineCard[cbStart])>GetCardLogicValue(cbTurnCardData[0]) && ((cbSingleLineCount-cbStart)>=cbTurnCardCount))
					{
						BYTE cbIndex = OutCardTypeResult[CT_SINGLE_LINE].cbCardTypeCount;
						OutCardTypeResult[CT_SINGLE_LINE].cbCardType = CT_SINGLE_LINE;
						OutCardTypeResult[CT_SINGLE_LINE].cbEachHandCardCount[cbIndex] = cbTurnCardCount;
						CopyMemory(OutCardTypeResult[CT_SINGLE_LINE].cbEachHandCardData[cbIndex], cbSingleLineCard+cbStart, sizeof(BYTE)*cbTurnCardCount);
						OutCardTypeResult[CT_SINGLE_LINE].cbCardTypeCount++;
						cbStart++;

						ASSERT(OutCardTypeResult[CT_SINGLE_LINE].cbCardTypeCount<=MAX_TYPE_COUNT);
					}
					//ɾ�����ҵ���˳��
					bFindSingleLine = true;
					RemoveCard(cbSingleLineCard, cbSingleLineCount, cbTempCardData, cbLeftCardCount);
					cbLeftCardCount -= cbSingleLineCount;
				}
			}
		}break;
	case CT_DOUBLE_LINE:		//��������
		{
			ZeroMemory(cbTempCardData, sizeof(cbTempCardData));
			BYTE cbLeftCardCount = 0;
			//ȥ��2����
			for (BYTE i=0; i<cbHandCardCount; ++i)	
			{
				if (GetCardLogicValue(cbHandCardData[i]) < 15)
				{
					cbTempCardData[cbLeftCardCount++] = cbHandCardData[i]; 
				}
			}
			SortCardList(cbTempCardData, cbLeftCardCount, ST_ORDER);

			bool bFindDoubleLine = true;
			BYTE cbDoubleLineCount=0, cbDoubleLineCard[MAX_COUNT]={0};
			//��ʼ�ж�
			while (cbLeftCardCount>=cbTurnCardCount && bFindDoubleLine)
			{
				BYTE cbLastCard = cbTempCardData[0];
				BYTE cbSameCount = 1;
				cbDoubleLineCount = 0;
				bFindDoubleLine = false;
				for (BYTE i=1; i<cbLeftCardCount; ++i)
				{
					//����ͬ��
					while (GetCardValue(cbLastCard)==GetCardValue(cbTempCardData[i]) && i<cbLeftCardCount)
					{
						++cbSameCount;
						++i;
					}

					BYTE cbLastDoubleCardValue = (cbDoubleLineCount>0) ? GetCardLogicValue(cbDoubleLineCard[cbDoubleLineCount-1]) : 0;
					//���¿�ʼ
					if ((cbSameCount<2||(cbDoubleLineCount>0&&(cbLastDoubleCardValue-GetCardLogicValue(cbLastCard))!=1)) && i<=cbLeftCardCount)
					{
						if (cbDoubleLineCount>=cbTurnCardCount) break;
						if (cbSameCount>=2) i -= cbSameCount;

						cbLastCard = cbTempCardData[i];
						cbDoubleLineCount = 0;
					}
					//��������
					else if (cbSameCount >= 2)
					{
						cbDoubleLineCard[cbDoubleLineCount] = cbTempCardData[i-cbSameCount];
						cbDoubleLineCard[cbDoubleLineCount+1] = cbTempCardData[i-cbSameCount+1];
						cbDoubleLineCount += 2;

						//��β�ж�
						if (i == cbLeftCardCount-2)
						{
							if (GetCardLogicValue(cbLastCard)-GetCardLogicValue(cbTempCardData[i])==1 && GetCardLogicValue(cbTempCardData[i])==GetCardLogicValue(cbTempCardData[i+1]))
							{
								cbDoubleLineCard[cbDoubleLineCount] = cbTempCardData[i];
								cbDoubleLineCard[cbDoubleLineCount+1] = cbTempCardData[i+1];
								cbDoubleLineCount += 2;
								break;
							}
						}
					}

					cbLastCard = cbTempCardData[i];
					cbSameCount = 1;
				}

				//��������
				if (cbDoubleLineCount >= cbTurnCardCount)
				{
					BYTE cbStart = 0;
					//��������
					while (GetCardLogicValue(cbDoubleLineCard[cbStart])>GetCardLogicValue(cbTurnCardData[0]) && cbDoubleLineCount-cbStart>=cbTurnCardCount)
					{
						BYTE cbIndex = OutCardTypeResult[CT_DOUBLE_LINE].cbCardTypeCount;
						OutCardTypeResult[CT_DOUBLE_LINE].cbCardType = CT_DOUBLE_LINE;
						OutCardTypeResult[CT_DOUBLE_LINE].cbEachHandCardCount[cbIndex] = cbTurnCardCount;
						CopyMemory(OutCardTypeResult[CT_DOUBLE_LINE].cbEachHandCardData[cbIndex], cbDoubleLineCard+cbStart, sizeof(BYTE)*cbTurnCardCount);
						OutCardTypeResult[CT_DOUBLE_LINE].cbCardTypeCount++;
						cbStart += 2;

						ASSERT(OutCardTypeResult[CT_DOUBLE_LINE].cbCardTypeCount<=MAX_TYPE_COUNT);
					}
					//ɾ�����ҵ�������
					bFindDoubleLine = true;
					RemoveCard(cbDoubleLineCard, cbDoubleLineCount, cbTempCardData, cbLeftCardCount);				
					cbLeftCardCount -= cbDoubleLineCount;
				}
			}			
		}break;
	case CT_THREE_LINE:			//��������
		{
			ZeroMemory(cbTempCardData, sizeof(cbTempCardData));
			BYTE cbLeftCardCount = 0;
			//ȥ��2����
			for (BYTE i=0; i<cbHandCardCount; ++i)
			{
				if (GetCardLogicValue(cbHandCardData[i]) < 15)
				{
					cbTempCardData[cbLeftCardCount++] = cbHandCardData[i];
				}
			}
			SortCardList(cbTempCardData, cbLeftCardCount, ST_ORDER);

			bool bFindThreeLine = true;
			BYTE cbThreeLineCount=0, cbThreeLineCard[MAX_COUNT]={0};
			//��ʼ�ж�
			while (cbLeftCardCount>=cbTurnCardCount && bFindThreeLine)
			{
				BYTE cbLastCard = cbTempCardData[0];
				BYTE cbSameCount = 1;
				cbThreeLineCount = 0;
				bFindThreeLine = false;
				for (BYTE i=1; i<cbLeftCardCount; ++i)
				{
					//����ͬ��
					while (GetCardValue(cbLastCard)==GetCardValue(cbTempCardData[i]) && i<cbLeftCardCount)
					{
						++cbSameCount;
						++i;
					}

					BYTE cbLastThreeCardValue = (cbThreeLineCount>0) ? GetCardLogicValue(cbThreeLineCard[cbThreeLineCount-1]) : 0;
					//���¿�ʼ
					if ((cbSameCount<3||(cbThreeLineCount>0&&(cbLastThreeCardValue-GetCardLogicValue(cbLastCard))!=1)) && i<=cbLeftCardCount)
					{
						if (cbThreeLineCount>=cbTurnCardCount) break;
						if (cbSameCount>=3) i -= 3;

						cbLastCard = cbTempCardData[i];
						cbThreeLineCount = 0;
					}
					//��������
					else if (cbSameCount >= 3)
					{
						cbThreeLineCard[cbThreeLineCount] = cbTempCardData[i-cbSameCount];
						cbThreeLineCard[cbThreeLineCount+1] = cbTempCardData[i-cbSameCount+1];
						cbThreeLineCard[cbThreeLineCount+2] = cbTempCardData[i-cbSameCount+2];
						cbThreeLineCount += 3;

						//��β�ж�
						if (i == cbLeftCardCount-3)
						{
							if (GetCardLogicValue(cbLastCard)-GetCardLogicValue(cbTempCardData[i])==1 && GetCardLogicValue(cbTempCardData[i])==GetCardLogicValue(cbTempCardData[i+1]) && GetCardLogicValue(cbTempCardData[i])==GetCardLogicValue(cbTempCardData[i+2]))
							{
								cbThreeLineCard[cbThreeLineCount] = cbTempCardData[i];
								cbThreeLineCard[cbThreeLineCount+1] = cbTempCardData[i+1];
								cbThreeLineCard[cbThreeLineCount+2] = cbTempCardData[i+2];
								cbThreeLineCount += 3;
								break;
							}
						}
					}

					cbLastCard = cbTempCardData[i];
					cbSameCount = 1;
				}

				//��������
				if (cbThreeLineCount >= cbTurnCardCount)
				{
					BYTE cbStart = 0;
					//��������
					while (GetCardLogicValue(cbThreeLineCard[cbStart])>GetCardLogicValue(cbTurnCardData[0]) && ((cbThreeLineCount-cbStart)>=cbTurnCardCount))
					{
						BYTE cbIndex = OutCardTypeResult[CT_THREE_LINE].cbCardTypeCount;
						OutCardTypeResult[CT_THREE_LINE].cbCardType = CT_THREE_LINE;
						OutCardTypeResult[CT_THREE_LINE].cbEachHandCardCount[cbIndex] = cbTurnCardCount;
						CopyMemory(OutCardTypeResult[CT_THREE_LINE].cbEachHandCardData[cbIndex], cbThreeLineCard+cbStart, sizeof(BYTE)*cbTurnCardCount);
						OutCardTypeResult[CT_THREE_LINE].cbCardTypeCount++;
						cbStart += 3;

						ASSERT(OutCardTypeResult[CT_THREE_LINE].cbCardTypeCount<=MAX_TYPE_COUNT);
					}
					//ɾ�����ҵ�������
					bFindThreeLine = true;
					RemoveCard(cbThreeLineCard, cbThreeLineCount, cbTempCardData, cbLeftCardCount);
					cbLeftCardCount -= cbThreeLineCount;
				}
			}
		}break;
	case CT_THREE_TAKE_ONE://����һ��
		{
			BYTE cbSingleCardCount = cbTurnCardCount/4;
			//�����˿�
			BYTE cbTurnThreeCardCount=0, cbTurnThreeCardData[MAX_COUNT]={0};
			GetAllThreeCard(cbTurnCardData, cbTurnCardCount, cbTurnThreeCardData, cbTurnThreeCardCount);
			//�Ƴ�ը��
			BYTE cbHandBombCardCount=0, cbHandBombCardData[MAX_COUNT]={0};
			GetAllBombCard(cbTempCardData, cbHandCardCount, cbHandBombCardData, cbHandBombCardCount);
			RemoveCard(cbHandBombCardData, cbHandBombCardCount, cbTempCardData, cbHandCardCount);
			BYTE cbFirstCard = 0;
			//ȥ��2����
			if (cbTurnThreeCardCount > 3)
			{
				for (BYTE i=0; i<cbHandCardCount-cbHandBombCardCount; ++i)	
				{
					if (GetCardLogicValue(cbTempCardData[i]) < 15)	
					{
						cbFirstCard = i; 
						break;
					}
				}
			}

			BYTE cbHandThreeCardCount=0, cbHandThreeCardData[MAX_COUNT]={0};
			GetAllThreeCard(cbTempCardData+cbFirstCard, cbHandCardCount-cbHandBombCardCount-cbFirstCard, cbHandThreeCardData, cbHandThreeCardCount);
			if (cbHandThreeCardCount<cbTurnThreeCardCount || (cbHandThreeCardCount>0&&GetCardLogicValue(cbHandThreeCardData[0])<=GetCardLogicValue(cbTurnThreeCardData[0]))) 
			{
				return;
			}

			for (BYTE i=0; i<cbHandThreeCardCount; i+=3)
			{
				BYTE cbLastLogicCard = GetCardLogicValue(cbHandThreeCardData[i]);
				BYTE cbThreeLineCard[MAX_COUNT] = {0};
				BYTE cbThreeLineCardCount = 3;
				cbThreeLineCard[0] = cbHandThreeCardData[i];
				cbThreeLineCard[1] = cbHandThreeCardData[i+1];
				cbThreeLineCard[2] = cbHandThreeCardData[i+2];
				for (BYTE j=i+3; j<cbHandThreeCardCount; j+=3)
				{
					//�����ж�
					if (1!=cbLastLogicCard-GetCardLogicValue(cbHandThreeCardData[j]) || cbThreeLineCardCount==cbTurnThreeCardCount) break;

					cbLastLogicCard = GetCardLogicValue(cbHandThreeCardData[j]);
					cbThreeLineCard[cbThreeLineCardCount] = cbHandThreeCardData[j];
					cbThreeLineCard[cbThreeLineCardCount+1] = cbHandThreeCardData[j+1];
					cbThreeLineCard[cbThreeLineCardCount+2] = cbHandThreeCardData[j+2];
					cbThreeLineCardCount += 3;
				}

				if (cbThreeLineCardCount==cbTurnThreeCardCount && GetCardLogicValue(cbThreeLineCard[0])>GetCardLogicValue(cbTurnThreeCardData[0]))
				{
					BYTE cbRemainCardData[MAX_COUNT] = {0};
					CopyMemory(cbRemainCardData, cbTempCardData, sizeof(BYTE)*(cbHandCardCount-cbHandBombCardCount));
					RemoveCard(cbThreeLineCard, cbTurnThreeCardCount, cbRemainCardData, (cbHandCardCount-cbHandBombCardCount));

					//�������
					BYTE cbComCard[5] = {0};
					BYTE cbComResCard[254][5] = {0};
					BYTE cbComResLen = 0;
					Combination(cbComCard, 0, cbComResCard, cbComResLen, cbRemainCardData, cbSingleCardCount, (cbHandCardCount-cbHandBombCardCount)-cbTurnThreeCardCount, cbSingleCardCount);
					for (BYTE i=0; i<cbComResLen; ++i)
					{
						BYTE cbIndex = OutCardTypeResult[CT_THREE_TAKE_ONE].cbCardTypeCount;
						OutCardTypeResult[CT_THREE_TAKE_ONE].cbCardType = CT_THREE_TAKE_ONE;
						OutCardTypeResult[CT_THREE_TAKE_ONE].cbEachHandCardCount[cbIndex] = cbTurnCardCount;
						//��������
						CopyMemory(OutCardTypeResult[CT_THREE_TAKE_ONE].cbEachHandCardData[cbIndex], cbThreeLineCard, sizeof(BYTE)*cbTurnThreeCardCount);
						//���浥��
						CopyMemory(OutCardTypeResult[CT_THREE_TAKE_ONE].cbEachHandCardData[cbIndex]+cbTurnThreeCardCount, cbComResCard[i], cbSingleCardCount);
						ASSERT(cbTurnThreeCardCount+cbSingleCardCount==cbTurnCardCount);

						OutCardTypeResult[CT_THREE_TAKE_ONE].cbCardTypeCount++;

						ASSERT(OutCardTypeResult[CT_THREE_TAKE_ONE].cbCardTypeCount<=MAX_TYPE_COUNT);
					}
				}
			}					
		}break;
	case CT_THREE_TAKE_TWO://����һ��
		{
			BYTE cbDoubleCardCount = cbTurnCardCount/5;
			//�����˿�
			BYTE cbTurnThreeCardCount=0, cbTurnThreeCardData[MAX_COUNT]={0};
			GetAllThreeCard(cbTurnCardData, cbTurnCardCount, cbTurnThreeCardData, cbTurnThreeCardCount);

			BYTE cbFirstCard = 0;
			//ȥ��2����
			if (cbTurnThreeCardCount > 3)
			{
				for (BYTE i=0; i<cbHandCardCount; ++i)
				{
					if (GetCardLogicValue(cbTempCardData[i]) < 15)	
					{
						cbFirstCard = i;
						break;
					}
				}
			}

			BYTE cbHandThreeCardCount=0, cbHandThreeCardData[MAX_COUNT]={0};
			GetAllThreeCard(cbTempCardData+cbFirstCard, cbHandCardCount-cbFirstCard, cbHandThreeCardData, cbHandThreeCardCount);
			if (cbHandThreeCardCount<cbTurnThreeCardCount || (cbHandThreeCardCount>0&&GetCardLogicValue(cbHandThreeCardData[0])<GetCardLogicValue(cbTurnThreeCardData[0])))
			{
				return;
			}

			for (BYTE i=0; i<cbHandThreeCardCount; i+=3)
			{
				BYTE cbLastLogicCardValue = GetCardLogicValue(cbHandThreeCardData[i]);
				BYTE cbThreeLineCardCount=3, cbThreeLineCardData[MAX_COUNT]={0};
				cbThreeLineCardData[0] = cbHandThreeCardData[i];
				cbThreeLineCardData[1] = cbHandThreeCardData[i+1];
				cbThreeLineCardData[2] = cbHandThreeCardData[i+2];
				for (BYTE j=i+3; j<cbHandThreeCardCount; j+=3)
				{
					//�����ж�
					if (cbLastLogicCardValue-GetCardLogicValue(cbHandThreeCardData[j])!=1 || cbThreeLineCardCount==cbTurnThreeCardCount) break;

					cbLastLogicCardValue = GetCardLogicValue(cbHandThreeCardData[j]);
					cbThreeLineCardData[cbThreeLineCardCount] = cbHandThreeCardData[j];
					cbThreeLineCardData[cbThreeLineCardCount+1] = cbHandThreeCardData[j+1];
					cbThreeLineCardData[cbThreeLineCardCount+2] = cbHandThreeCardData[j+2];
					cbThreeLineCardCount += 3;
				}

				if (cbThreeLineCardCount==cbTurnThreeCardCount && GetCardLogicValue(cbThreeLineCardData[0])>GetCardLogicValue(cbTurnThreeCardData[0]))
				{
					BYTE cbRemainCardData[MAX_COUNT] = {0};
					CopyMemory(cbRemainCardData, cbTempCardData, sizeof(BYTE)*cbHandCardCount);
					RemoveCard(cbThreeLineCardData, cbTurnThreeCardCount, cbRemainCardData, cbHandCardCount);

					BYTE cbAllDoubleCardCount=0, cbAllDoubleCardData[MAX_COUNT]={0};
					GetAllDoubleCard(cbRemainCardData, cbHandCardCount-cbTurnThreeCardCount, cbAllDoubleCardData, cbAllDoubleCardCount);

					BYTE cbDoubleCardIndex[10] = {0}; //�����±�
					for (BYTE i=0,j=0; i<cbAllDoubleCardCount; i+=2,++j)
					{
						cbDoubleCardIndex[j] = i;
					}

					//�������
					BYTE cbComCard[5] = {0};
					BYTE cbComResCard[254][5] = {0};
					BYTE cbComResLen = 0;
					//���ö��Ƶ��±�����ϣ��ٸ����±���ȡ������
					Combination(cbComCard, 0, cbComResCard, cbComResLen, cbDoubleCardIndex, cbDoubleCardCount, cbAllDoubleCardCount/2, cbDoubleCardCount);
					for (BYTE i=0; i<cbComResLen; ++i)
					{
						BYTE cbIndex = OutCardTypeResult[CT_THREE_TAKE_TWO].cbCardTypeCount;
						OutCardTypeResult[CT_THREE_TAKE_TWO].cbCardType = CT_THREE_TAKE_TWO;
						OutCardTypeResult[CT_THREE_TAKE_TWO].cbEachHandCardCount[cbIndex] = cbTurnCardCount;
						//��������
						CopyMemory(OutCardTypeResult[CT_THREE_TAKE_TWO].cbEachHandCardData[cbIndex], cbThreeLineCardData, sizeof(BYTE)*cbTurnThreeCardCount);
						//�������
						for (BYTE j=0,k=0; j<cbDoubleCardCount; ++j,k+=2)
						{
							OutCardTypeResult[CT_THREE_TAKE_TWO].cbEachHandCardData[cbIndex][cbTurnThreeCardCount+k] = cbAllDoubleCardData[cbComResCard[i][j]];
							OutCardTypeResult[CT_THREE_TAKE_TWO].cbEachHandCardData[cbIndex][cbTurnThreeCardCount+k+1] = cbAllDoubleCardData[cbComResCard[i][j]+1];
						}
						ASSERT(cbTurnThreeCardCount+cbDoubleCardCount*2==cbTurnCardCount);
						OutCardTypeResult[CT_THREE_TAKE_TWO].cbCardTypeCount++;

						ASSERT(OutCardTypeResult[CT_THREE_TAKE_TWO].cbCardTypeCount<=MAX_TYPE_COUNT);
					}
				}
			}	
		}break;
	case CT_FOUR_TAKE_ONE: //�Ĵ�����
		{
			BYTE cbFirstCard = 0;
			//ȥ������
			for (BYTE i=0; i<cbHandCardCount; ++i)
			{
				if (GetCardColor(cbTempCardData[i]) != 0x40)	
				{
					cbFirstCard = i;
					break;
				}
			}

			BYTE cbHandFourCardCount=0, cbHandFourCardData[MAX_COUNT]={0};
			BYTE cbTurnFourCardCount=0, cbTurnFourCardData[MAX_COUNT]={0};
			//��ȡ����
			GetAllBombCard(cbTempCardData+cbFirstCard, cbHandCardCount-cbFirstCard, cbHandFourCardData, cbHandFourCardCount);
			GetAllBombCard(cbTurnCardData, cbTurnCardCount, cbTurnFourCardData, cbTurnFourCardCount);
			if ((cbHandFourCardCount==0) || (cbHandFourCardCount>0&&GetCardLogicValue(cbHandFourCardData[0])<GetCardLogicValue(cbTurnFourCardData[0])))
			{
				return;
			}

			BYTE cbCanOutFourCardCount=0, cbCanOutFourCardData[MAX_COUNT]={0};
			//�ɳ�����
			for (BYTE i=0; i<cbHandFourCardCount; i+=4)
			{
				if (GetCardLogicValue(cbHandFourCardData[i]) > GetCardLogicValue(cbTurnFourCardData[0]))
				{
					cbCanOutFourCardData[cbCanOutFourCardCount] = cbHandFourCardData[i];
					cbCanOutFourCardData[cbCanOutFourCardCount+1] = cbHandFourCardData[i+1];
					cbCanOutFourCardData[cbCanOutFourCardCount+2] = cbHandFourCardData[i+2];
					cbCanOutFourCardData[cbCanOutFourCardCount+3] = cbHandFourCardData[i+3];
					cbCanOutFourCardCount += 4;
				}
			}

			if (cbHandCardCount-cbCanOutFourCardCount < cbTurnCardCount-cbTurnFourCardCount)
			{
				return;
			}

			BYTE cbRemainCardData[MAX_COUNT] = {0};
			CopyMemory(cbRemainCardData, cbTempCardData, cbHandCardCount*sizeof(BYTE));
			RemoveCard(cbCanOutFourCardData, cbCanOutFourCardCount, cbRemainCardData, cbHandCardCount);
			for (BYTE cbStart=0; cbStart<cbCanOutFourCardCount; cbStart+=4)
			{
				//�������
				BYTE cbComCard[5] = {0};
				BYTE cbComResCard[254][5] = {0};
				BYTE cbComResLen = 0;
				//�������
				Combination(cbComCard, 0, cbComResCard, cbComResLen, cbRemainCardData, 2, cbHandCardCount-cbCanOutFourCardCount, 2);
				for (BYTE i=0; i<cbComResLen; ++i)
				{
					//���ܴ���
					if (GetCardValue(cbComResCard[i][0]) == GetCardValue(cbComResCard[i][1])) continue;

					BYTE cbIndex = OutCardTypeResult[CT_FOUR_TAKE_ONE].cbCardTypeCount;
					OutCardTypeResult[CT_FOUR_TAKE_ONE].cbCardType = CT_FOUR_TAKE_ONE;
					OutCardTypeResult[CT_FOUR_TAKE_ONE].cbEachHandCardCount[cbIndex] = 6;
					CopyMemory(OutCardTypeResult[CT_FOUR_TAKE_ONE].cbEachHandCardData[cbIndex], cbCanOutFourCardData+cbStart, 4);
					OutCardTypeResult[CT_FOUR_TAKE_ONE].cbEachHandCardData[cbIndex][4] = cbComResCard[i][0];
					OutCardTypeResult[CT_FOUR_TAKE_ONE].cbEachHandCardData[cbIndex][4+1] = cbComResCard[i][1];
					OutCardTypeResult[CT_FOUR_TAKE_ONE].cbCardTypeCount++;

					ASSERT(OutCardTypeResult[CT_FOUR_TAKE_ONE].cbCardTypeCount<=MAX_TYPE_COUNT);
				}
			}
		}break;
	case CT_FOUR_TAKE_TWO: //�Ĵ�����
		{
			BYTE cbFirstCard = 0;
			//ȥ������
			for (BYTE i=0; i<cbHandCardCount; ++i)
			{
				if (GetCardColor(cbTempCardData[i]) != 0x40)
				{
					cbFirstCard = i; 
					break;
				}
			}

			BYTE cbHandFourCardCount=0, cbHandFourCardData[MAX_COUNT]={0};
			BYTE cbTurnFourCardCount=0, cbTurnFourCardData[MAX_COUNT]={0};
			//��ȡ����
			GetAllBombCard(cbTempCardData+cbFirstCard, cbHandCardCount-cbFirstCard, cbHandFourCardData, cbHandFourCardCount);
			GetAllBombCard(cbTurnCardData, cbTurnCardCount, cbTurnFourCardData, cbTurnFourCardCount);
			if ((cbHandFourCardCount==0) || (cbHandFourCardCount>0&&GetCardLogicValue(cbHandFourCardData[0])<GetCardLogicValue(cbTurnFourCardData[0])))
			{
				return;
			}

			BYTE cbCanOutFourCardCount=0, cbCanOutFourCardData[MAX_COUNT]={0};
			//�ɳ�����
			for (BYTE i=0; i<cbHandFourCardCount; i+=4)
			{
				if (GetCardLogicValue(cbHandFourCardData[i]) > GetCardLogicValue(cbTurnFourCardData[0]))
				{
					cbCanOutFourCardData[cbCanOutFourCardCount] = cbHandFourCardData[i];
					cbCanOutFourCardData[cbCanOutFourCardCount+1] = cbHandFourCardData[i+1];
					cbCanOutFourCardData[cbCanOutFourCardCount+2] = cbHandFourCardData[i+2];
					cbCanOutFourCardData[cbCanOutFourCardCount+3] = cbHandFourCardData[i+3];
					cbCanOutFourCardCount += 4;
				}
			}

			if (cbHandCardCount-cbCanOutFourCardCount < cbTurnCardCount-cbTurnFourCardCount) 
			{
				return;
			}

			BYTE cbRemainCardData[MAX_COUNT] = {0};
			CopyMemory(cbRemainCardData, cbTempCardData, cbHandCardCount*sizeof(BYTE));
			RemoveCard(cbCanOutFourCardData, cbCanOutFourCardCount, cbRemainCardData, cbHandCardCount);
			for (BYTE cbStart=0; cbStart<cbCanOutFourCardCount; cbStart+=4)
			{
				BYTE cbDoubleCardCount=0, cbDoubleCardData[MAX_COUNT]={0};
				GetAllDoubleCard(cbRemainCardData, cbHandCardCount-cbCanOutFourCardCount, cbDoubleCardData, cbDoubleCardCount);

				BYTE cbDoubleCardIndex[10] = {0}; //�����±�
				for (BYTE i=0, j=0; i<cbDoubleCardCount; i+=2, ++j)
				{
					cbDoubleCardIndex[j] = i;
				}

				//�������
				BYTE cbComCard[5] = {0};
				BYTE cbComResCard[254][5] = {0};
				BYTE cbComResLen = 0;
				//���ö��Ƶ��±�����ϣ��ٸ����±���ȡ������
				Combination(cbComCard, 0, cbComResCard, cbComResLen, cbDoubleCardIndex, 2, cbDoubleCardCount/2, 2);
				for (BYTE i=0; i<cbComResLen; ++i)
				{
					BYTE cbIndex = OutCardTypeResult[CT_FOUR_TAKE_TWO].cbCardTypeCount;
					OutCardTypeResult[CT_FOUR_TAKE_TWO].cbCardType = CT_FOUR_TAKE_TWO;
					OutCardTypeResult[CT_FOUR_TAKE_TWO].cbEachHandCardCount[cbIndex] = 8;
					CopyMemory(OutCardTypeResult[CT_FOUR_TAKE_TWO].cbEachHandCardData[cbIndex], cbCanOutFourCardData+cbStart, 4);
					//�������
					for(BYTE j=0,k=0; j<4; ++j,k+=2)
					{
						OutCardTypeResult[CT_FOUR_TAKE_TWO].cbEachHandCardData[cbIndex][4+k] = cbDoubleCardData[cbComResCard[i][j]];
						OutCardTypeResult[CT_FOUR_TAKE_TWO].cbEachHandCardData[cbIndex][4+k+1] = cbDoubleCardData[cbComResCard[i][j]+1];
					}
					OutCardTypeResult[CT_FOUR_TAKE_TWO].cbCardTypeCount++;

					ASSERT(OutCardTypeResult[CT_FOUR_TAKE_TWO].cbCardTypeCount<=MAX_TYPE_COUNT);
				}
			}
		}break;
	case CT_BOMB_CARD:			//ը������
		{
			BYTE cbBombCardCount=0, cbBombCardData[MAX_COUNT]={0};
			GetAllBombCard(cbTempCardData, cbHandCardCount, cbBombCardData, cbBombCardCount);
			BYTE cbFirstBomb = 0;
			if (cbBombCardCount>0 && cbBombCardData[0]==0x4F)
			{
				BYTE cbIndex = OutCardTypeResult[CT_BOMB_CARD].cbCardTypeCount;
				OutCardTypeResult[CT_BOMB_CARD].cbCardType = CT_BOMB_CARD;
				OutCardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[cbIndex] = 2;
				OutCardTypeResult[CT_BOMB_CARD].cbEachHandCardData[cbIndex][0] = 0x4F;
				OutCardTypeResult[CT_BOMB_CARD].cbEachHandCardData[cbIndex][1] = 0x4E;
				OutCardTypeResult[CT_BOMB_CARD].cbCardTypeCount++;

				ASSERT(OutCardTypeResult[CT_BOMB_CARD].cbCardTypeCount<=MAX_TYPE_COUNT);
				cbFirstBomb = 2;
			}
			for (BYTE i=cbFirstBomb; i<cbBombCardCount; i+=4)
			{
				if (GetCardLogicValue(cbBombCardData[i]) > GetCardLogicValue(cbTurnCardData[0]))
				{
					BYTE cbIndex = OutCardTypeResult[CT_BOMB_CARD].cbCardTypeCount;
					OutCardTypeResult[CT_BOMB_CARD].cbCardType = CT_BOMB_CARD;
					OutCardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[cbIndex] = 4;
					CopyMemory(OutCardTypeResult[CT_BOMB_CARD].cbEachHandCardData[cbIndex], cbBombCardData+i, 4);
					OutCardTypeResult[CT_BOMB_CARD].cbCardTypeCount++;

					ASSERT(OutCardTypeResult[CT_BOMB_CARD].cbCardTypeCount<=MAX_TYPE_COUNT);
				}
			}		
		}break;
	case CT_MISSILE_CARD:		//�������
		{
			//û�бȻ�����������
			break;
		}
	default:
		ASSERT(false);
		break;
	}
}

//���Ƹ���
BYTE CAndroidLogic::AnalyseSinleCardCount( const BYTE cbHandCardData[], const BYTE cbHandCardCount, const BYTE cbWantOutCardData[], const BYTE cbWantOutCardCount, BYTE cbSingleCardData[] )
{
	//�����ж�
	ASSERT(cbHandCardCount>0);
	if (cbHandCardCount <= 0) return MAX_COUNT+5;

	BYTE cbRemainCardCount=cbHandCardCount, cbRemainCardData[MAX_COUNT]={0};
	CopyMemory(cbRemainCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
	SortCardList(cbRemainCardData, cbHandCardCount, ST_ORDER);

	//�Ƴ��˿�
	if (cbWantOutCardCount != 0)
	{
		RemoveCard(cbWantOutCardData, cbWantOutCardCount, cbRemainCardData, cbHandCardCount);
		cbRemainCardCount = cbHandCardCount-cbWantOutCardCount;
	}

	//����ָ��
	typedef void (CAndroidLogic::*pGetAllCardFun)(BYTE const [], BYTE const , BYTE[], BYTE &); 
	//ָ������
	pGetAllCardFun GetAllCardFunArray[4] = {&CGameLogic::GetAllBombCard,&CGameLogic::GetAllLineCard,&CGameLogic::GetAllThreeCard,&CGameLogic::GetAllDoubleCard};

	//ָ�������±�
	BYTE cbIndexArray[4] = {0,1,2,3};
	//���н��
	BYTE cbPermutationRes[24][4] = {0};
	BYTE cbLen = 0;
	//��������
	Permutation(cbIndexArray, 0, 4, cbPermutationRes, cbLen);
	ASSERT(cbLen==24);
	if (cbLen != 24) return MAX_COUNT+5;

	//������Ŀ
	BYTE cbMinSingleCardCount = MAX_COUNT+5;
	//������Сֵ
	for (BYTE i=0; i<24; ++i)
	{
		//��������
		BYTE cbTempCardData[MAX_COUNT]={0}, cbTempCardCount=cbRemainCardCount;
		CopyMemory(cbTempCardData, cbRemainCardData, cbRemainCardCount);
		for (BYTE j=0; j<4; ++j)
		{
			BYTE cbIndex = cbPermutationRes[i][j];
			//У���±�
			ASSERT(cbIndex>=0 && cbIndex<4);
			if (cbIndex<0 || cbIndex>=4) return MAX_COUNT+5;

			pGetAllCardFun pTempGetAllCardFun = GetAllCardFunArray[cbIndex];
			//��ȡ�˿�
			BYTE cbGetCardCount=0, cbGetCardData[MAX_COUNT]={0};
			//��Ա����
			((*this).*pTempGetAllCardFun)(cbTempCardData, cbTempCardCount, cbGetCardData, cbGetCardCount);

			//ɾ���˿�
			if (cbGetCardCount != 0) 
			{
				RemoveCard(cbGetCardData, cbGetCardCount, cbTempCardData, cbTempCardCount);
				cbTempCardCount -= cbGetCardCount;
			}
		}

		//���㵥��
		BYTE cbSingleCardCount=0, cbSingleCard[MAX_COUNT]={0};
		GetAllSingleCard(cbTempCardData, cbTempCardCount, cbSingleCard, cbSingleCardCount, true);
		if (cbMinSingleCardCount> cbSingleCardCount)
		{
			cbMinSingleCardCount = cbSingleCardCount;
			//���浥��
			if (cbSingleCardData != NULL)
			{
				CopyMemory(cbSingleCardData, cbSingleCard, cbSingleCardCount);
			}
		}
	}

	//�������ж�
	if (cbWantOutCardCount > 0)
	{
		//��������
		BYTE cbCardType = GetCardType(cbWantOutCardData, cbWantOutCardCount);
		if (cbCardType==CT_THREE_TAKE_ONE || cbCardType==CT_THREE_TAKE_TWO)
		{
			for (BYTE i=3; i<cbWantOutCardCount; ++i)
			{
				if (GetCardLogicValue(cbWantOutCardData[i]) >= 14)
				{
					cbMinSingleCardCount += 3;
				}
			}
		}
	}

	//�������ж�
	if (cbWantOutCardCount>0 && GetCardType(cbWantOutCardData, cbWantOutCardCount)==CT_DOUBLE)
	{
		BYTE cbAllThreeCount=0, cbAllThreeCardData[MAX_COUNT]={0};
		BYTE cbAllLineCount=0, cbAllLineCardData[MAX_COUNT]={0};
		GetAllThreeCard(cbHandCardData, cbHandCardCount, cbAllThreeCardData, cbAllThreeCount);
		GetAllLineCard(cbHandCardData, cbHandCardCount, cbAllLineCardData, cbAllLineCount);

		BYTE cbThreeCardValue = 0;
		for (BYTE i=0; i<cbAllThreeCount; ++i)
		{
			for (BYTE j=0; j<cbWantOutCardCount; ++j)
			{
				if (GetCardLogicValue(cbWantOutCardData[j]) == GetCardLogicValue(cbAllThreeCardData[i]))
				{
					cbThreeCardValue = GetCardValue( cbWantOutCardData[j] );
					break;
				}
			}
		}

		//�Ƿ�������
		bool bInLineCard = false;
		for (BYTE cbLineCardIndex=0; cbLineCardIndex<cbAllLineCount; ++cbLineCardIndex)
		{
			if (GetCardValue(cbAllLineCardData[cbLineCardIndex]) == cbThreeCardValue)
			{
				bInLineCard = true;
				break;
			}
		}
		if (!bInLineCard && cbThreeCardValue!=0) 
		{
			cbMinSingleCardCount += 2;
		}
	}

	//��ը�ж�
	if (cbWantOutCardCount != 0)
	{
		//ը���˿�
		BYTE cbBombCardCount=0, cbBombCardData[20]={0};
		GetAllBombCard(cbHandCardData, cbHandCardCount, cbBombCardData, cbBombCardCount);

		//��������
		BYTE cbCardType = GetCardType(cbWantOutCardData, cbWantOutCardCount);
		if (cbBombCardCount>0 && cbCardType<CT_BOMB_CARD)
		{
			//Ѱ����ͬ
			for (BYTE i=GetCardColor(cbBombCardData[0])==4?2:0; i<cbBombCardCount; i+=4)
			{
				for (BYTE j=0; j<cbWantOutCardCount; ++j)
				{
					if (GetCardValue(cbBombCardData[i])==GetCardValue(cbWantOutCardData[j]) && GetCardLogicValue(cbWantOutCardData[j])<15 && cbCardType!=CT_SINGLE_LINE && cbCardType!=CT_DOUBLE_LINE) 
					{
						return MAX_COUNT+5; //����ը��
					}
					else if (GetCardValue(cbBombCardData[i])==GetCardValue(cbWantOutCardData[j]) && GetCardLogicValue(cbWantOutCardData[j])<15 && (cbCardType==CT_SINGLE_LINE||cbCardType==CT_DOUBLE_LINE)) 
					{
						cbMinSingleCardCount += 2; //����ը��
					}
				}
			}

			//���ը���ж�
			if (cbCardType == CT_SINGLE_LINE)
			{
				BYTE cbBombCardCount = 0;
				for (BYTE i=GetCardColor(cbBombCardData[0])==4?2:0; i<cbBombCardCount; i+=4)
				{
					for (BYTE j=0; j<cbWantOutCardCount; ++j)
					{
						if (GetCardValue(cbBombCardData[i]) == GetCardValue(cbWantOutCardData[j])) 
						{
							++cbBombCardCount;
						}
					}
				}
				if (cbBombCardCount >= 2) return MAX_COUNT; //����ը��

				//��������
				tagAnalyseResult AnalyseResult;
				AnalysebCardData(cbHandCardData, cbHandCardCount,AnalyseResult);
                BYTE cbThreeCount = 0;
				for (BYTE i=0; i<AnalyseResult.cbThreeCount; ++i)
				{
					for (BYTE j=0; j<cbWantOutCardCount; ++j)
					{
						if (GetCardValue(cbWantOutCardData[j]) == GetCardValue(AnalyseResult.cbThreeCardData[3*i])) 
						{
							++cbThreeCount;
						}
					}
				}
				if (cbThreeCount+cbBombCardCount >= 2) 
				{
					return MAX_COUNT+5;
				}
			}
		}
	}

	return cbMinSingleCardCount;
}

//��������
bool CAndroidLogic::SearchOutCard( const BYTE cbHandCardData[], const BYTE cbHandCardCount, const BYTE cbTurnCardData[], const BYTE cbTurnCardCount, const WORD wMeChairID, const WORD wOutCardUser, tagOutCardResult &OutCardResult )
{
	//��ʼ����
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));

	//�������
	const WORD wUndersideOfBanker = (m_wBankerUser+1)%GAME_PLAYER;		//�����¼�
	const WORD wUpsideOfBanker = (wUndersideOfBanker+1)%GAME_PLAYER;	//�����ϼ�

	//��������
	BYTE cbTempHandCardCount=0, cbTempHandCardData[MAX_COUNT]={0};
	cbTempHandCardCount = cbHandCardCount;
	CopyMemory(cbTempHandCardData, cbHandCardData, sizeof(BYTE)*cbTempHandCardCount);
	SortCardList(cbTempHandCardData, cbTempHandCardCount, ST_ORDER);
	//�ȳ���
	if (cbTurnCardCount == 0)
	{
		//��������
		if (wMeChairID == m_wBankerUser) 
		{
			BankerOutCard(cbTempHandCardData, cbTempHandCardCount, wMeChairID, OutCardResult);
		}
		//�����¼�
		else if (wMeChairID == wUndersideOfBanker) 
		{
			UndersideOfBankerOutCard(cbTempHandCardData, cbTempHandCardCount, wMeChairID, OutCardResult);
		}
		//�����ϼ�
		else if (wMeChairID == wUpsideOfBanker)
		{
			UpsideOfBankerOutCard(cbTempHandCardData, cbTempHandCardCount, wMeChairID, OutCardResult);
		}
		//���� ID
		else
		{
			ASSERT(false);
		}
	}
	//ѹ��
	else
	{	
		//��������
		if (wMeChairID == m_wBankerUser)
		{
			BankerOutCard(cbHandCardData, cbHandCardCount, cbTurnCardData, cbTurnCardCount, wMeChairID, wOutCardUser, OutCardResult);
		}
		//�����¼�
		else if (wMeChairID == wUndersideOfBanker)
		{
			UndersideOfBankerOutCard(cbHandCardData, cbHandCardCount, cbTurnCardData, cbTurnCardCount, wMeChairID, wOutCardUser, OutCardResult);
		}
		//�����ϼ�
		else if (wMeChairID == wUpsideOfBanker)
		{
			UpsideOfBankerOutCard(cbHandCardData, cbHandCardCount, cbTurnCardData, cbTurnCardCount, wMeChairID, wOutCardUser, OutCardResult);
		}
		//���� ID
		else
		{
			ASSERT(false);
		}

		//�����ж�
		if (GetCardType(m_cbHandCardData[m_wBankerUser],m_cbHandCardCount[m_wBankerUser])!=CT_ERROR && m_cbHandCardCount[m_wBankerUser]<=5
			&& (OutCardResult.cbCardCount<=0||!CompareCard(cbTurnCardData,OutCardResult.cbCardData,cbTurnCardCount,OutCardResult.cbCardCount)) 
			&& m_cbHandCardCount[wUndersideOfBanker]==NORMAL_COUNT && m_cbHandCardCount[wUpsideOfBanker]==NORMAL_COUNT)
		{
			//���±�û��
			tagOutCardTypeResult CardTypeResult[12+1];
			//��������
			AnalyseOutCardType(cbHandCardData, cbHandCardCount, cbTurnCardData, cbTurnCardCount, CardTypeResult);

			BYTE cbOutCardType = GetCardType(cbTurnCardData, cbTurnCardCount);
			ASSERT(cbOutCardType!=CT_ERROR);
			//��ͬ����
			if (CardTypeResult[cbOutCardType].cbCardTypeCount > 0)
			{
				BYTE cbIndex = CardTypeResult[cbOutCardType].cbCardTypeCount - 1;
				BYTE cbCardCount = CardTypeResult[cbOutCardType].cbEachHandCardCount[cbIndex];
				CopyMemory(OutCardResult.cbCardData, CardTypeResult[cbOutCardType].cbEachHandCardData[cbIndex], cbCardCount);
				OutCardResult.cbCardCount = cbCardCount;
			}
			//��ը��
			else if (CardTypeResult[CT_BOMB_CARD].cbCardTypeCount > 0)
			{
				BYTE cbIndex = CardTypeResult[CT_BOMB_CARD].cbCardTypeCount - 1;
				BYTE cbCardCount = CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[cbIndex];
				CopyMemory(OutCardResult.cbCardData, CardTypeResult[CT_BOMB_CARD].cbEachHandCardData[cbIndex], cbCardCount);
				OutCardResult.cbCardCount = cbCardCount;
			}
			//�����
			else if (CardTypeResult[CT_MISSILE_CARD].cbCardTypeCount > 0)
			{
				OutCardResult.cbCardCount = 2;
				OutCardResult.cbCardData[0] = 0x4F;
				OutCardResult.cbCardData[1] = 0x4E;
			}
		}
		//�������ж�
		if (GetCardType(m_cbHandCardData[wOutCardUser],m_cbHandCardCount[wOutCardUser])!=CT_ERROR && m_cbHandCardCount[wOutCardUser]<=5 && m_lBankerOutCardCount==1 
			&& (OutCardResult.cbCardCount<=0||!CompareCard(cbTurnCardData,OutCardResult.cbCardData ,cbTurnCardCount,OutCardResult.cbCardCount)))
		{
			//���±�û��
			tagOutCardTypeResult CardTypeResult[12+1];
			//��������
			AnalyseOutCardType(cbHandCardData, cbHandCardCount, cbTurnCardData, cbTurnCardCount, CardTypeResult);

			BYTE cbOutCardType = GetCardType(cbTurnCardData, cbTurnCardCount);
			ASSERT(cbOutCardType!=CT_ERROR);
			//��ͬ����
			if (CardTypeResult[cbOutCardType].cbCardTypeCount > 0)
			{
				BYTE cbIndex = CardTypeResult[cbOutCardType].cbCardTypeCount - 1;
				BYTE cbCardCount = CardTypeResult[cbOutCardType].cbEachHandCardCount[cbIndex];
				CopyMemory(OutCardResult.cbCardData, CardTypeResult[cbOutCardType].cbEachHandCardData[cbIndex], cbCardCount);
				OutCardResult.cbCardCount = cbCardCount;
			}
			//��ը��
			else if (CardTypeResult[CT_BOMB_CARD].cbCardTypeCount > 0)
			{
				BYTE cbIndex = CardTypeResult[CT_BOMB_CARD].cbCardTypeCount - 1;
				BYTE cbCardCount = CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[cbIndex];
				CopyMemory(OutCardResult.cbCardData, CardTypeResult[CT_BOMB_CARD].cbEachHandCardData[cbIndex], cbCardCount);
				OutCardResult.cbCardCount = cbCardCount;
			}
			//�����
			else if (CardTypeResult[CT_MISSILE_CARD].cbCardTypeCount > 0)
			{
				OutCardResult.cbCardCount = 2;
				OutCardResult.cbCardData[0] = 0x4F;
				OutCardResult.cbCardData[1] = 0x4E;
			}
		}
	}
	return true;
}

//�������ƣ��ȳ��ƣ�
void CAndroidLogic::BankerOutCard( const BYTE cbHandCardData[], const BYTE cbHandCardCount, const WORD wMeChairID, tagOutCardResult &OutCardResult ) 
{
	//��ʼ����
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));
	if (cbHandCardCount == 0) return;

	//�������
	const WORD wUndersideUser = (m_wBankerUser+1)%GAME_PLAYER;		//�����¼�
	const WORD wUpsideUser = (wUndersideUser+1)%GAME_PLAYER;		//�����ϼ�

	//�Լ���������
	BYTE cbHandCardType = IsValidCount(cbHandCardCount) ? GetCardType(cbHandCardData,cbHandCardCount) : CT_ERROR;
	//�ж�ׯ��һ�����ܷ����
	if (cbHandCardType != CT_ERROR)
	{
		//�����Ĵ�����ֱ�ӳ���
		if (cbHandCardType!=CT_FOUR_TAKE_ONE && cbHandCardType!=CT_FOUR_TAKE_TWO)
		{
			OutCardResult.cbCardCount = cbHandCardCount;
			CopyMemory(&OutCardResult.cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
			return;
		}
		else
		{
			//�����Ĵ�������
			Analyse4D2CardType(wMeChairID, cbHandCardData, cbHandCardCount, cbHandCardType, OutCardResult);
			if (OutCardResult.cbCardCount>0 && GetCardType(OutCardResult.cbCardData, OutCardResult.cbCardCount)!=CT_ERROR)
			{
				return;
			}
		}
	}
	//�ж��Ƿ��ǻ����һ��
	if (cbHandCardCount>2 && cbHandCardData[0]==0x4F && cbHandCardData[1]==0x4E)
	{
		cbHandCardType = GetCardType(&cbHandCardData[2], cbHandCardCount-2);
		if (cbHandCardType != CT_ERROR)
		{
			//�����ʣ�µĲ����Ĵ�����ֱ�ӳ�ը��
			if (cbHandCardType!=CT_FOUR_TAKE_ONE && cbHandCardType!=CT_FOUR_TAKE_TWO)
			{
				OutCardResult.cbCardCount = 2;
				OutCardResult.cbCardData[0] = 0x4F;
				OutCardResult.cbCardData[1] = 0x4E;
				return;
			}
			else
			{
				//�����Ĵ�������
				Analyse4D2CardType(wMeChairID, cbHandCardData, cbHandCardCount, cbHandCardType, OutCardResult);
				if (OutCardResult.cbCardCount>0 && GetCardType(OutCardResult.cbCardData, OutCardResult.cbCardCount)!=CT_ERROR)
				{
					return;
				}
			}
		}
	}
	
	BYTE cbFirstCardIndex = 0;
	if (cbHandCardCount>=2 && cbHandCardData[0]==0x4F && cbHandCardData[1]==0x4E)
	{
		cbFirstCardIndex = 2;
	}
	//�Լ�ֻʣ���ź�����һ����,���ҵ��Ż�һ��û��ѹ
	BYTE cbTempCardData[MAX_COUNT]={0}, cbTempCardCount=cbHandCardCount-cbFirstCardIndex;
	for (BYTE i=0; i<cbHandCardCount-cbFirstCardIndex; i++)
	{
		ZeroMemory(cbTempCardData, sizeof(cbTempCardData));
		cbTempCardCount = cbHandCardCount-cbFirstCardIndex;
		CopyMemory(cbTempCardData, &cbHandCardData[cbFirstCardIndex], sizeof(BYTE)*cbTempCardCount);
		BYTE cbSingleCard = cbTempCardData[i];
		RemoveCard(&cbSingleCard, 1, cbTempCardData, cbTempCardCount);
		cbTempCardCount -= 1;
		BYTE cbLastCardType = GetCardType(cbTempCardData, cbTempCardCount);
		if (cbLastCardType==CT_ERROR || cbLastCardType==CT_FOUR_TAKE_ONE || cbLastCardType==CT_FOUR_TAKE_TWO)
		{
			continue;
		}
		if (IsLargestCard(wUndersideUser,cbTempCardData,cbTempCardCount) && IsLargestCard(wUpsideUser,cbTempCardData,cbTempCardCount))
		{
			OutCardResult.cbCardCount = cbTempCardCount;
			CopyMemory(OutCardResult.cbCardData, cbTempCardData, sizeof(BYTE)*cbTempCardCount);
			return;
		}
		if (IsLargestCard(wUndersideUser,&cbSingleCard,1) && IsLargestCard(wUpsideUser,&cbSingleCard,1))
		{
			OutCardResult.cbCardCount = 1;
			OutCardResult.cbCardData[0] = cbSingleCard;
			return;
		}
	}
	//���ż�һ��,��һ��������
	for (BYTE i=0; i<cbHandCardCount-cbFirstCardIndex; i++)
	{
		ZeroMemory(cbTempCardData, sizeof(cbTempCardData));
		cbTempCardCount = cbHandCardCount-cbFirstCardIndex;
		CopyMemory(cbTempCardData, &cbHandCardData[cbFirstCardIndex], sizeof(BYTE)*cbTempCardCount);
		BYTE cbSingleCard = cbTempCardData[i];
		RemoveCard(&cbSingleCard, 1, cbTempCardData, cbTempCardCount);
		cbTempCardCount -= 1;
		BYTE cbLastCardType = GetCardType(cbTempCardData, cbTempCardCount);
		if (cbLastCardType==CT_ERROR || cbLastCardType==CT_FOUR_TAKE_ONE || cbLastCardType==CT_FOUR_TAKE_TWO || cbLastCardType==CT_DOUBLE || (cbLastCardType==CT_BOMB_CARD&&m_cbHandCardCount[wUpsideUser]>1&&m_cbHandCardCount[wUndersideUser]>1))
		{
			continue;
		}
		tagAnalyseResult TempAnalyseResult;
		AnalysebCardData(cbTempCardData, cbTempCardCount, TempAnalyseResult);
		if (TempAnalyseResult.cbFourCount > 0) continue;
		OutCardResult.cbCardCount = cbTempCardCount;
		CopyMemory(OutCardResult.cbCardData, cbTempCardData, sizeof(BYTE)*cbTempCardCount);
		return;
	}
	//����������
	if (cbHandCardCount==4 && cbHandCardData[0]==0x4F && cbHandCardData[1]==0x4E && m_cbHandCardCount[wUndersideUser]==1 && GetCardLogicValue(cbHandCardData[2])<GetCardLogicValue(m_cbHandCardData[wUndersideUser][0]))
	{
		OutCardResult.cbCardCount = 1;
		OutCardResult.cbCardData[0] = cbHandCardData[2];
		return;
	}
	//����������
	if (cbHandCardCount==4 && cbHandCardData[0]==0x4F && cbHandCardData[1]==0x4E && m_cbHandCardCount[wUpsideUser]==1 && GetCardLogicValue(cbHandCardData[2])<GetCardLogicValue(m_cbHandCardData[wUpsideUser][0]))
	{
		OutCardResult.cbCardCount = 1;
		OutCardResult.cbCardData[0] = cbHandCardData[2];
		return;
	}
	
	//���±�û��
	tagOutCardTypeResult MeOutCardTypeResult[12+1];
	AnalyseOutCardType(cbHandCardData, cbHandCardCount, MeOutCardTypeResult);
	//ũ��ֻʣһ��
	AnalyseEnemyLastOneCard(wMeChairID, cbHandCardData, cbHandCardCount, MeOutCardTypeResult, OutCardResult);
	if (OutCardResult.cbCardCount>0 && GetCardType(OutCardResult.cbCardData,OutCardResult.cbCardCount)!=CT_ERROR) return;
	//ũ��ֻʣ����
	AnalyseEnemyLastTwoCard(wMeChairID, cbHandCardData, cbHandCardCount, MeOutCardTypeResult, OutCardResult);
	if (OutCardResult.cbCardCount>0 && GetCardType(OutCardResult.cbCardData,OutCardResult.cbCardCount)!=CT_ERROR) return;


	//������������
	tagAnalyseResult MeAnalyseResult, UndersideAnalyseResult, UpsideAnalyseResult;
	VERIFY(AnalysebCardData(cbHandCardData, cbHandCardCount, MeAnalyseResult));
	VERIFY(AnalysebCardData(m_cbHandCardData[wUndersideUser], m_cbHandCardCount[wUndersideUser], UndersideAnalyseResult));
	VERIFY(AnalysebCardData(m_cbHandCardData[wUpsideUser], m_cbHandCardCount[wUpsideUser], UpsideAnalyseResult));
	//��һ�ַ���/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		//���㵥��
		BYTE cbSingleCardCount[4] = {MAX_COUNT,MAX_COUNT,MAX_COUNT,MAX_COUNT};
		BYTE cbOutCardIndex[4] = {0};
		BYTE cbOutCardType[4] = {CT_ERROR};
		BYTE cbMinSingleCardCount = MAX_COUNT;
		BYTE cbMinCardType = CT_ERROR;
		BYTE cbMinCardIndex = 0;

		//��ը�������
		for(BYTE cbCardType=CT_DOUBLE; cbCardType<CT_BOMB_CARD; ++cbCardType)
		{
			const tagOutCardTypeResult &tempOutCardTypeResult = MeOutCardTypeResult[cbCardType];
			//��ͬ���ͣ���ͬ���ȣ���������������ͬ���Ϳ��ܳ��Ȳ�һ��
			BYTE cbThisHandCardCount = MAX_COUNT;

			//ũ��������
			tagOutCardTypeResult UpsideOutCardTypeResult[13], UndersideOutCardTypeResult[13];
			for (BYTE i=0; i<tempOutCardTypeResult.cbCardTypeCount; ++i)
			{
				//��ֹ��������ը���ж�
				if (cbCardType == CT_DOUBLE)
				{
					tagAnalyseResult AnalyseResult;
					AnalysebCardData(cbHandCardData, cbHandCardCount, AnalyseResult);
					if (AnalyseResult.cbSingleCount+AnalyseResult.cbThreeCount*3 == cbHandCardCount)
					{
						bool bContinue = false;
						for (BYTE cbThreeIndex=0; cbThreeIndex<AnalyseResult.cbThreeCount; ++cbThreeIndex)
						{
							if (GetCardValue(tempOutCardTypeResult.cbEachHandCardData[i][0]) == GetCardValue(AnalyseResult.cbThreeCardData[3*cbThreeIndex]))
							{
								bContinue = true;
								break;
							}
						}
						if (bContinue) continue;
					}
					if (AnalyseResult.cbFourCount > 0)
					{
						bool bContinue = false;
						for (BYTE cbFourIndex=0; cbFourIndex<AnalyseResult.cbFourCount; cbFourIndex++)
						{
							if (GetCardValue(tempOutCardTypeResult.cbEachHandCardData[i][0]) == GetCardValue(AnalyseResult.cbFourCardData[4*cbFourIndex]))
							{
								bContinue = true;
								break;
							}
						}
						if (bContinue) continue;
					}
				}

				BYTE cbTempCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, tempOutCardTypeResult.cbEachHandCardData[i], tempOutCardTypeResult.cbEachHandCardCount[i]);
				//���·���
				if (tempOutCardTypeResult.cbEachHandCardCount[i] != cbThisHandCardCount)
				{
					cbThisHandCardCount = tempOutCardTypeResult.cbEachHandCardCount[i];
					AnalyseOutCardType(m_cbHandCardData[wUpsideUser], m_cbHandCardCount[wUpsideUser], tempOutCardTypeResult.cbEachHandCardData[i], tempOutCardTypeResult.cbEachHandCardCount[i], UpsideOutCardTypeResult);
					AnalyseOutCardType(m_cbHandCardData[wUndersideUser], m_cbHandCardCount[wUndersideUser], tempOutCardTypeResult.cbEachHandCardData[i], tempOutCardTypeResult.cbEachHandCardCount[i], UndersideOutCardTypeResult);
				}

				//�з�����ѹס��
				if (UpsideOutCardTypeResult[cbCardType].cbCardTypeCount>0 || UndersideOutCardTypeResult[cbCardType].cbCardTypeCount>0)
				{
					//�ϼ��ܵ�
					if (UpsideOutCardTypeResult[cbCardType].cbEachHandCardCount[0]>0 && m_cbHandCardCount[wUpsideUser]<=UpsideOutCardTypeResult[cbCardType].cbEachHandCardCount[0]+1)
					{
						continue;
					}
					//�¼��ܵ�
					if (UndersideOutCardTypeResult[cbCardType].cbEachHandCardCount[0]>0 && m_cbHandCardCount[wUndersideUser]<=UndersideOutCardTypeResult[cbCardType].cbEachHandCardCount[0]+1)
					{
						continue;
					}
				}

				//���˳�ӣ������Ĵ���
				BYTE cbCurrentCardType = GetCardType(tempOutCardTypeResult.cbEachHandCardData[i], cbThisHandCardCount);
				if (cbThisHandCardCount!=cbHandCardCount && cbCurrentCardType>=CT_SINGLE_LINE && cbCurrentCardType<=CT_THREE_TAKE_TWO &&
					(GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][cbThisHandCardCount-1])>GetCardLogicValue(cbHandCardData[cbHandCardCount-2])||GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][0])>=11))
				{
					BYTE cbRemainCardCount=0, cbRemainCardData[MAX_COUNT]={0};
					CopyMemory(cbRemainCardData, cbHandCardData, cbHandCardCount);
					cbRemainCardCount = cbHandCardCount;
					//�Ƴ��˿�
					RemoveCard(tempOutCardTypeResult.cbEachHandCardData[i], cbThisHandCardCount, cbRemainCardData, cbRemainCardCount);
					cbRemainCardCount -= cbThisHandCardCount;

					//����˿�
					BYTE cbCurrentLargestLogicCard = GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][0]);
					if (GetCardType(cbRemainCardData,cbRemainCardCount)==CT_ERROR && ((cbCurrentCardType>=CT_THREE_TAKE_ONE&&cbCurrentCardType<=CT_THREE_TAKE_TWO&&cbCurrentLargestLogicCard>=11&&cbThisHandCardCount<=5) 
						|| (cbCurrentCardType==CT_SINGLE_LINE&&cbThisHandCardCount<=6&&cbCurrentLargestLogicCard>=12) 
						|| (cbCurrentCardType>=CT_DOUBLE_LINE&&cbCurrentCardType<=CT_THREE_LINE&&cbCurrentLargestLogicCard>=12&&cbThisHandCardCount<=8)))
					{
						//��ʱ����
						if (cbCurrentCardType>=CT_SINGLE_LINE && cbCurrentCardType<=CT_THREE_LINE && GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][cbThisHandCardCount- 1])>GetCardLogicValue(cbHandCardData[cbHandCardCount-3]))
						{
							continue;
						}

						if (cbCurrentCardType>=CT_THREE_TAKE_ONE && cbCurrentCardType<=CT_THREE_TAKE_TWO && GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][0])>GetCardLogicValue(cbHandCardData[cbHandCardCount-3]))
						{
							continue;
						}
					}
				}

				//��Դ�ԣ������ȳ���
				if (cbCardType==CT_DOUBLE && GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][0])>=11)
				{
					BYTE cbAllSingleCount=0, cbAllSingleCardData[MAX_COUNT]={0};
					cbAllSingleCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, NULL, 0, cbAllSingleCardData);
					if (cbAllSingleCount>=2 && GetCardLogicValue(cbAllSingleCardData[cbAllSingleCount-2])<10) 
					{
						continue;
					}
				}

				//�Ƿ��д��� 
				if (tempOutCardTypeResult.cbEachHandCardCount[i] != cbHandCardCount)
				{
					bool bHaveLargeCard = false;
					for (BYTE j=0; j<tempOutCardTypeResult.cbEachHandCardCount[i]; ++j)
					{
						if (GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][j]) >= 15) 
						{
							bHaveLargeCard = true;
						}
						if (cbCardType!=CT_SINGLE_LINE && cbCardType!=CT_DOUBLE_LINE && GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][0])==14) 
						{
							bHaveLargeCard = true; 
						}
					}
					if (bHaveLargeCard)	continue;
				}

				//����cbMinSingleCardCount[4]�����ֵ
				for (BYTE j=0; j<4; ++j)
				{
					if (cbSingleCardCount[j] >= cbTempCount)
					{
						cbSingleCardCount[j] = cbTempCount;
						cbOutCardIndex[j] = i;
						cbOutCardType[j] = cbCardType;
						break;
					}
				}

				//������Сֵ
				if (cbMinSingleCardCount >= cbTempCount)
				{
					//��С����
					cbMinCardType = cbCardType;
					//��С�����е���С����
					cbMinSingleCardCount = cbTempCount;						
					//��С�����е���С��
					cbMinCardIndex = i;
				}
			}
		}
		if (cbMinSingleCardCount>=AnalyseSinleCardCount(cbHandCardData,cbHandCardCount,NULL,0)+3 && m_cbHandCardCount[wUndersideUser]>=4 && m_cbHandCardCount[wUpsideUser]>=4)
		{
			cbMinSingleCardCount = MAX_COUNT;
		}
		if (cbMinSingleCardCount != MAX_COUNT)
		{
			BYTE cbCurIndex = cbMinCardIndex;
			//ѡ����С��
			for (BYTE i=0; i<4; ++i)
			{
				if (cbOutCardType[i]==cbMinCardType && cbSingleCardCount[i]<=cbMinSingleCardCount && GetCardLogicValue(MeOutCardTypeResult[cbMinCardType].cbEachHandCardData[cbOutCardIndex[i]][0])<GetCardLogicValue(MeOutCardTypeResult[cbMinCardType].cbEachHandCardData[cbCurIndex][0]))
				{
					cbCurIndex = cbOutCardIndex[i];
				}
			}
			//���ñ���
			OutCardResult.cbCardCount = MeOutCardTypeResult[cbMinCardType].cbEachHandCardCount[cbCurIndex];
			CopyMemory(OutCardResult.cbCardData, MeOutCardTypeResult[cbMinCardType].cbEachHandCardData[cbCurIndex], MeOutCardTypeResult[cbMinCardType].cbEachHandCardCount[cbCurIndex]*sizeof(BYTE));
			return;
		}
	}

	//��һ�ַ���/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		BYTE cbFirstCardIndex = 0;
		//��������2
		for (BYTE i=0; i<cbHandCardCount; ++i) 
		{
			if (GetCardLogicValue(cbHandCardData[i]) < 15)
			{
				cbFirstCardIndex = i;
				break;
			}
		}

		if (cbFirstCardIndex < cbHandCardCount-1)
		{
			AnalyseOutCardType(cbHandCardData+cbFirstCardIndex, cbHandCardCount-cbFirstCardIndex, MeOutCardTypeResult);
		}
		else
		{
			AnalyseOutCardType(cbHandCardData, cbHandCardCount, MeOutCardTypeResult);
		}

		//���㵥��
		BYTE cbSingleCardCount[4] = {MAX_COUNT,MAX_COUNT,MAX_COUNT,MAX_COUNT};
		BYTE cbOutCardIndex[4] = {0};
		BYTE cbOutcardType[4] = {CT_ERROR};
		BYTE cbMinSingleCardCount = MAX_COUNT;
		BYTE cbMinCardType = CT_ERROR;
		BYTE cbMinCardIndex = 0;

		//��ը�������
		for (BYTE cbCardType=CT_SINGLE; cbCardType<CT_BOMB_CARD; ++cbCardType)
		{
			const tagOutCardTypeResult &tempOutCardTypeResult = MeOutCardTypeResult[cbCardType];
			for (BYTE i=0; i<tempOutCardTypeResult.cbCardTypeCount; ++i)
			{
				//�мҿ����ߵ�
				if (CompareCard(tempOutCardTypeResult.cbEachHandCardData[i],m_cbHandCardData[wUndersideUser],tempOutCardTypeResult.cbEachHandCardCount[i],m_cbHandCardCount[wUndersideUser])&&GetCardType(m_cbHandCardData[wUndersideUser],m_cbHandCardCount[wUndersideUser])<CT_BOMB_CARD
					|| CompareCard(tempOutCardTypeResult.cbEachHandCardData[i],m_cbHandCardData[wUpsideUser],tempOutCardTypeResult.cbEachHandCardCount[i],m_cbHandCardCount[wUpsideUser])&&GetCardType(m_cbHandCardData[wUpsideUser],m_cbHandCardCount[wUpsideUser])<CT_BOMB_CARD)
				{
					continue;
				}

				//��ֹ��������ը���ж�
				if (cbCardType == CT_DOUBLE)
				{
					tagAnalyseResult AnalyseResult;
					AnalysebCardData(cbHandCardData, cbHandCardCount, AnalyseResult);
					if (AnalyseResult.cbSingleCount+AnalyseResult.cbThreeCount*3 == cbHandCardCount)
					{
						bool bContinue = false;
						for (BYTE cbThreeIndex=0; cbThreeIndex<AnalyseResult.cbThreeCount; ++cbThreeIndex)
						{
							if (GetCardValue(tempOutCardTypeResult.cbEachHandCardData[i][0]) == GetCardValue(AnalyseResult.cbThreeCardData[3*cbThreeIndex]))
							{
								bContinue = true;
								break;
							}
						}
						if (bContinue) continue;
					}
					if (AnalyseResult.cbFourCount > 0)
					{
						bool bContinue = false;
						for (BYTE cbFourIndex=0; cbFourIndex<AnalyseResult.cbFourCount; cbFourIndex++)
						{
							if (GetCardValue(tempOutCardTypeResult.cbEachHandCardData[i][0]) == GetCardValue(AnalyseResult.cbFourCardData[4*cbFourIndex]))
							{
								bContinue = true;
								break;
							}
						}
						if (bContinue) continue;
					}
				}

				//���˳�ӣ������Ĵ���
				if (tempOutCardTypeResult.cbEachHandCardCount[i]!=cbHandCardCount && cbCardType>=CT_SINGLE_LINE && cbCardType<=CT_THREE_TAKE_TWO &&
					(GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][tempOutCardTypeResult.cbEachHandCardCount[i]-1])>GetCardLogicValue(cbHandCardData[cbHandCardCount-2])||GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][0])>=11))
				{
					BYTE cbRemainCardCount=0, cbRemainCardData[MAX_COUNT]={0};
					CopyMemory(cbRemainCardData, cbHandCardData, cbHandCardCount);
					cbRemainCardCount = cbHandCardCount;
					//�Ƴ��˿�
					RemoveCard(tempOutCardTypeResult.cbEachHandCardData[i], tempOutCardTypeResult.cbEachHandCardCount[i], cbRemainCardData, cbRemainCardCount);
					cbRemainCardCount -= tempOutCardTypeResult.cbEachHandCardCount[i];

					//����˿�
					BYTE cbCurrentLargestLogicCard = GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][0]);
					if (GetCardType(cbRemainCardData,cbRemainCardCount)==CT_ERROR && ((cbCardType>=CT_THREE_TAKE_ONE&&cbCardType<=CT_THREE_TAKE_TWO&&cbCurrentLargestLogicCard>=11&&tempOutCardTypeResult.cbEachHandCardCount[i]<=5)
						|| (cbCardType==CT_SINGLE_LINE&&tempOutCardTypeResult.cbEachHandCardCount[i]<=6&&cbCurrentLargestLogicCard>=12) 
						|| (cbCardType>=CT_DOUBLE_LINE&&cbCardType<=CT_THREE_LINE&&cbCurrentLargestLogicCard>=12&&tempOutCardTypeResult.cbEachHandCardCount[i]<=8)))
					{
						//��ʱ����
						if (cbCardType>=CT_SINGLE_LINE && cbCardType<=CT_THREE_LINE && GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][tempOutCardTypeResult.cbEachHandCardCount[i]-1])>GetCardLogicValue(cbHandCardData[cbHandCardCount-3]))
						{
							continue;
						}

						if (cbCardType>=CT_THREE_TAKE_ONE && cbCardType<=CT_THREE_TAKE_TWO &&	GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][0])>GetCardLogicValue(cbHandCardData[cbHandCardCount-3]))
						{
							continue;
						}
					}
				}

				BYTE cbTempCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, tempOutCardTypeResult.cbEachHandCardData[i], tempOutCardTypeResult.cbEachHandCardCount[i]);
				//����cbMinSingleCardCount[4]�����ֵ
				for (BYTE j=0; j<4; ++j)
				{
					if (cbSingleCardCount[j] >= cbTempCount)
					{
						cbSingleCardCount[j] = cbTempCount;
						cbOutCardIndex[j] = i;
						cbOutcardType[j] = cbCardType;
						break;
					}
				}

				//������Сֵ
				if (cbMinSingleCardCount >= cbTempCount)
				{
					//��С����
					cbMinCardType = cbCardType;
					//��С�����е���С����
					cbMinSingleCardCount = cbTempCount;						
					//��С�����е���С��
					cbMinCardIndex = i;
				}
			}
		}

		if (cbMinSingleCardCount != MAX_COUNT)
		{
			BYTE cbCurIndex = cbMinCardIndex;
			//ѡ����С��
			for (BYTE i=0; i<4; ++i)
			{
				if (cbOutcardType[i]==cbMinCardType && cbSingleCardCount[i]<=cbMinSingleCardCount && GetCardLogicValue(MeOutCardTypeResult[cbMinCardType].cbEachHandCardData[cbOutCardIndex[i]][0])<GetCardLogicValue(MeOutCardTypeResult[cbMinCardType].cbEachHandCardData[cbCurIndex][0]))
				{
					cbCurIndex = cbOutCardIndex[i];
				}
			}
			//���ñ���
			OutCardResult.cbCardCount = MeOutCardTypeResult[cbMinCardType].cbEachHandCardCount[cbCurIndex];
			CopyMemory(OutCardResult.cbCardData, MeOutCardTypeResult[cbMinCardType].cbEachHandCardData[cbCurIndex], MeOutCardTypeResult[cbMinCardType].cbEachHandCardCount[cbCurIndex]*sizeof(BYTE));
			return;
		}
	}

	//�������Ͳ��ҷ���ʧ��
	//ֻʣը���ͻ��
	ZeroMemory(cbTempCardData, sizeof(cbTempCardData));
	cbTempCardCount = cbHandCardCount-cbFirstCardIndex;
	CopyMemory(cbTempCardData, &cbHandCardData[cbFirstCardIndex], sizeof(BYTE)*cbTempCardCount);
	AnalysebCardData(cbTempCardData, cbTempCardCount, MeAnalyseResult);
	if (MeAnalyseResult.cbFourCount>0 && MeAnalyseResult.cbFourCount*4==cbTempCardCount)
	{
		for (int i=MeAnalyseResult.cbFourCount-1; i>=0; i--)
		{
			OutCardResult.cbCardCount = 4;
			CopyMemory(OutCardResult.cbCardData, &MeAnalyseResult.cbFourCardData[i*4], sizeof(BYTE)*4);
			return;
		}
	}
	BYTE cbAllSingleCardCount=0, cbAllSingleCardData[MAX_COUNT]={0};
	cbAllSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, NULL, 0, cbAllSingleCardData);
	if (cbAllSingleCardCount > 0)
	{
		//�����û���������ͳ���С��һ��
		if ((m_cbHandCardCount[wUndersideUser]==1||m_cbHandCardCount[wUpsideUser]==1) && cbAllSingleCardCount>=2) 
		{
			OutCardResult.cbCardCount = 1;
			OutCardResult.cbCardData[0] = cbAllSingleCardData[cbAllSingleCardCount-2];
		}
		else 
		{
			OutCardResult.cbCardCount = 1;
			OutCardResult.cbCardData[0] = cbAllSingleCardData[cbAllSingleCardCount-1];
		}
		return;
	}

	//�����û���������ͳ���С��һ��
	OutCardResult.cbCardCount = 1;
	OutCardResult.cbCardData[0] = cbHandCardData[cbHandCardCount-1];
	return;
}

//�������ƣ�����ƣ�
void CAndroidLogic::BankerOutCard( const BYTE cbHandCardData[], const BYTE cbHandCardCount, const BYTE cbTurnCardData[], const BYTE cbTurnCardCount, const WORD wMeChairID, const WORD wOutCardUser, tagOutCardResult &OutCardResult ) 
{
	//��ʼ����
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));
	//��������
	BYTE cbOutCardType = GetCardType(cbTurnCardData,cbTurnCardCount);
	if (cbOutCardType == CT_MISSILE_CARD) return;

	//�������
	const WORD wUndersideUser = (m_wBankerUser+1)%GAME_PLAYER;	//�����¼�
	const WORD wUpsideUser = (wUndersideUser+1)%GAME_PLAYER;	//�����ϼ�

	//�Լ�ֻʣ�����һ��
	BYTE cbFirstCardIndex = 0;
	BYTE cbTempCardData[MAX_COUNT]={0}, cbTempCardCount=0;
	if (cbHandCardCount>=2 && cbHandCardData[0]==0x4F && cbHandCardData[1]==0x4E)
	{
		cbFirstCardIndex = 2;
		if (cbHandCardCount==2 || GetCardType(&cbHandCardData[2],cbHandCardCount-2)!=CT_ERROR)
		{
			OutCardResult.cbCardCount = 2;
			OutCardResult.cbCardData[0] = 0x4F;
			OutCardResult.cbCardData[1] = 0x4E;
			return;
		}
	}

	//�Լ�ʣ����������ܴ��������,����һ�����в�����ը��(ը�����ͳ���)
	BYTE cbBombCardCount=0, cbBombCardData[MAX_COUNT]={0};
	GetAllBombCard(cbHandCardData, cbHandCardCount, cbBombCardData, cbBombCardCount);
	if (CompareCard(cbTurnCardData,cbHandCardData,cbTurnCardCount,cbHandCardCount) && (GetCardType(cbHandCardData,cbHandCardCount)>=CT_BOMB_CARD||cbBombCardCount==0))
	{
		OutCardResult.cbCardCount = cbHandCardCount;
		CopyMemory(OutCardResult.cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
		return;
	}

	//�Լ�ֻʣը���ͻ��
	cbTempCardCount = cbHandCardCount-cbFirstCardIndex;
	CopyMemory(cbTempCardData, &cbHandCardData[cbFirstCardIndex], sizeof(BYTE)*cbTempCardCount);
	tagAnalyseResult MeAnalyseResult;
	AnalysebCardData(cbTempCardData, cbTempCardCount, MeAnalyseResult);
	//�ж��Ƿ���Գ�ը��
	bool bCanOutBombCard = false;
	BYTE cbCard2 = 0x02;
	//�����ѳ�
	if (IsLargestCardWithOutBomb(wMeChairID,&cbCard2,1))
	{
		bCanOutBombCard = true;
	}
	//�������ũ�����п��Գ�
	if ((m_cbHandCardCount[wUndersideUser]>=2&&m_cbHandCardData[wUndersideUser][1]!=0x4E) && (m_cbHandCardCount[wUpsideUser]>=2&&m_cbHandCardData[wUpsideUser][0]!=0x4E))
	{
		bCanOutBombCard = true;
	}
	//ȥ������������ƻ���������
	if (m_cbHandCardCount[wUndersideUser]>=5 && m_cbHandCardCount[wUpsideUser]>=5)
	{
		bCanOutBombCard = true;
	}
	if (MeAnalyseResult.cbFourCount>0 && MeAnalyseResult.cbFourCount*4==cbTempCardCount)
	{
		for (int i=MeAnalyseResult.cbFourCount-1; i>=0; i--)
		{
			if (bCanOutBombCard && CompareCard(cbTurnCardData,&MeAnalyseResult.cbFourCardData[i*4],cbTurnCardCount,4))
			{
				OutCardResult.cbCardCount = 4;
				CopyMemory(OutCardResult.cbCardData, &MeAnalyseResult.cbFourCardData[i*4], sizeof(BYTE)*4);
				return;
			}
			else if (cbHandCardCount>=2 && cbHandCardData[0]==0x4F && cbHandCardData[1]==0x4E)
			{
				OutCardResult.cbCardCount = 2;
				OutCardResult.cbCardData[0] = 0x4F;
				OutCardResult.cbCardData[1] = 0x4E;
				return;
			}
		}
	}

	//ը����һ��
	if (cbBombCardCount > 0)
	{
		BYTE cbRemainCardCount=0, cbRemainCardData[MAX_COUNT]={0};
		//ʣ���˿�
		CopyMemory(cbRemainCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
		cbRemainCardCount = cbHandCardCount;
		RemoveCard(cbBombCardData, cbBombCardCount, cbRemainCardData, cbRemainCardCount);
		cbRemainCardCount -= cbBombCardCount;

		//ʣ�µ���һ���ƣ�������������һ�����
		BYTE cbRemainMaxValue = cbRemainCardCount>0 ? GetCardLogicValue(cbRemainCardData[0]) : 0;
		BYTE cbUpMaxValue = GetCardLogicValue(m_cbHandCardData[wUpsideUser][0]);
		BYTE cbUnderMaxValue = GetCardLogicValue(m_cbHandCardData[wUndersideUser][0]);
		if ((cbRemainCardCount>0&&GetCardType(cbRemainCardData,cbRemainCardCount)!=CT_ERROR) || (cbRemainCardCount==2&&cbRemainMaxValue>=cbUnderMaxValue&&cbRemainMaxValue>=cbUpMaxValue))
		{
			//���
			if (GetCardColor(cbBombCardData[0]) == 0x40)
			{
				OutCardResult.cbCardCount = 2;
				OutCardResult.cbCardData[0] = 0x4F;
				OutCardResult.cbCardData[1] = 0x4E;
				return;
			}
			else
			{
				//ը����ѹ������
				if (CompareCard(cbTurnCardData,cbBombCardData,cbTurnCardCount,4))
				{
					tagPromptCardResult UpPromptCardResult, UnderPrompCardResult;
					CGameLogic::SearchOutCard(m_cbHandCardData[wUpsideUser], m_cbHandCardCount[wUpsideUser], cbBombCardData, 4, UpPromptCardResult);
					CGameLogic::SearchOutCard(m_cbHandCardData[wUndersideUser], m_cbHandCardCount[wUndersideUser], cbBombCardData, 4, UnderPrompCardResult);
					//�س�ը��
					if (UpPromptCardResult.cbResultCount==0 && UnderPrompCardResult.cbResultCount==0)
					{
						OutCardResult.cbCardCount = 4;
						CopyMemory(OutCardResult.cbCardData, cbBombCardData, sizeof(BYTE)*4);
						return;
					}
					else
					{
						if (CompareCard(cbTurnCardData,cbRemainCardData,cbTurnCardCount,cbRemainCardCount))
						{
							OutCardResult.cbCardCount = cbRemainCardCount;
							CopyMemory(OutCardResult.cbCardData, cbRemainCardData, sizeof(BYTE)*cbRemainCardCount);
							return;
						}
					}
				}
			}
		}
	}

	//�����Լ���������
	tagOutCardTypeResult MeCardTypeResult[12+1];
	AnalyseOutCardType(cbHandCardData, cbHandCardCount, cbTurnCardData, cbTurnCardCount, MeCardTypeResult);
	//ȡ���ĸ���С����
	BYTE cbSingleCardCount[4] = {MAX_COUNT,MAX_COUNT,MAX_COUNT,MAX_COUNT};
	BYTE cbOutCardIndex[4] = {0};	
	BYTE cbMinSingleCardCount = MAX_COUNT;
	//�ɳ��˿ˣ������Ѿ����˵�ը���ˣ�
	const tagOutCardTypeResult &CanOutCardType = MeCardTypeResult[cbOutCardType];
	for (BYTE i=0; i<CanOutCardType.cbCardTypeCount; ++i)
	{
		//��С����
		BYTE cbTempCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, CanOutCardType.cbEachHandCardData[i], CanOutCardType.cbEachHandCardCount[i]); 
		//����cbMinSingleCardCount[4]�����ֵ
		for (BYTE j=0; j<4; ++j)
		{
			if (cbSingleCardCount[j] >= cbTempCount)
			{
				cbSingleCardCount[j] = cbTempCount;
				cbOutCardIndex[j] = i;
				break;
			}
		}
	}
	for (BYTE i=0; i<4; ++i)
	{
		if (cbMinSingleCardCount > cbSingleCardCount[i])
		{
			cbMinSingleCardCount = cbSingleCardCount[i];
		}
	}

	//ԭʼ������
	BYTE cbOriginSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, NULL, 0);
	if (CanOutCardType.cbCardTypeCount>0 && cbMinSingleCardCount<MAX_COUNT)
	{
		BYTE cbMinLogicCardValue = GetCardLogicValue(0x4F)+1;
		bool bFindCard = false;
		BYTE cbCanOutIndex = 0;
		for (BYTE i=0; i<4; ++i)
		{
			BYTE cbCurIndex = cbOutCardIndex[i];
			if ((cbSingleCardCount[i]<cbOriginSingleCardCount+3) && cbMinLogicCardValue>GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbCurIndex][0]) && 
				((cbSingleCardCount[i]<=cbMinSingleCardCount)||(cbSingleCardCount[i]<=cbMinSingleCardCount+1&&CanOutCardType.cbCardType>=CT_THREE_TAKE_ONE&&CanOutCardType.cbCardType<=CT_THREE_TAKE_TWO)))
			{
				//��Դ���
				bool bNoLargeCard = true;
				//���������������������4�����ҳ�����С��K���ƶ��Ҳ��ǳ������ϵ������ʱ�����ܳ�2ȥ��
				if (m_cbHandCardCount[wOutCardUser]>=4 && cbHandCardCount>=5 && CanOutCardType.cbEachHandCardCount[cbCurIndex]>=2 && 
					GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbCurIndex][0])>=15 && GetCardLogicValue(cbTurnCardData[0])<13 &&
					(wOutCardUser==wUndersideUser&&GetCardLogicValue(cbTurnCardData[0])<GetCardLogicValue(m_cbHandCardData[wUndersideUser][0]) || wOutCardUser==wUpsideUser&&GetCardLogicValue(cbTurnCardData[0])<GetCardLogicValue(m_cbHandCardData[wUpsideUser][0])) && 
					CanOutCardType.cbEachHandCardCount[cbCurIndex]!=cbHandCardCount)
				{
					bNoLargeCard = false;
				}

				//������û�д��ƣ���Էɻ���������Ĵ��ƣ�
				for (BYTE k=3; k<CanOutCardType.cbEachHandCardCount[cbCurIndex]; ++k)
				{
					if (GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbCurIndex][k])>=15 && CanOutCardType.cbEachHandCardCount[cbCurIndex]!=cbHandCardCount)
					{
						bNoLargeCard = false;
					}
				}
				if (bNoLargeCard)
				{
					bFindCard = true;
					cbCanOutIndex = cbCurIndex; 
					cbMinLogicCardValue = GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbCurIndex][0]);
				}
			}
		}

		if (bFindCard)
		{
			//�����
			BYTE cbLargestLogicCard = 0;
			if (wOutCardUser == wUndersideUser)
			{
				cbLargestLogicCard = GetCardLogicValue(m_cbHandCardData[wUndersideUser][0]);
			}
			else if (wOutCardUser == wUpsideUser)
			{
				cbLargestLogicCard = GetCardLogicValue(m_cbHandCardData[wUpsideUser][0]);
			}
			bool bCanOut = true;

			//��ֻѹ2
			if (GetCardLogicValue(cbTurnCardData[0]) < cbLargestLogicCard)
			{
				if (GetCardColor(CanOutCardType.cbEachHandCardData[cbCanOutIndex][0])==0x40 && GetCardLogicValue(cbTurnCardData[0])<=14 && cbHandCardCount>5) 								
				{
					bCanOut = false;
				}
			}

			//˫���ж�
			if (GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbCanOutIndex][0])>=16 && cbHandCardCount>=2 && cbHandCardData[0]==0x4F && cbHandCardData[1]==0x4E)
			{
				bool bOutMissileCard = false;
				//һ���ƺ�һ��ը��
				BYTE cbRemainCardData[MAX_COUNT]={0}, cbRemainCardCount=cbHandCardCount;
				CopyMemory(cbRemainCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
				RemoveCard(cbRemainCardData, 2, cbRemainCardData, cbRemainCardCount);
				cbRemainCardCount -= 2;
				if (CT_ERROR != GetCardType(cbRemainCardData,cbRemainCardCount)) 
				{
					bOutMissileCard = true;
				}

				//ֻʣ������
				if (cbRemainCardCount<5 && cbRemainCardCount>0 && GetCardLogicValue(cbRemainCardData[0])>=14)
				{
					bOutMissileCard = true;
				}

				//ը������
				BYTE cbSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, CanOutCardType.cbEachHandCardData[cbCanOutIndex], CanOutCardType.cbEachHandCardCount[cbCanOutIndex]);
				if (cbSingleCardCount<=1 && GetCardLogicValue(cbRemainCardData[0])>=11) 
				{
					bOutMissileCard = true;
				}

				//����С��
				if (GetCardLogicValue(cbRemainCardData[0])<=10 && CT_ERROR==GetCardType(cbRemainCardData,cbRemainCardCount) && (GetCardLogicValue(m_cbHandCardData[wUndersideUser][0])>10||GetCardLogicValue(m_cbHandCardData[wUpsideUser][0])>10))
				{
					bOutMissileCard = false;
				}

				//����˿�
				if (bOutMissileCard)
				{
					//��������ը��
					BYTE cbIndex = MeCardTypeResult[CT_BOMB_CARD].cbCardTypeCount - 1;
					OutCardResult.cbCardCount = MeCardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[cbIndex];
					CopyMemory(OutCardResult.cbCardData, MeCardTypeResult[CT_BOMB_CARD].cbEachHandCardData[cbIndex], OutCardResult.cbCardCount);
					return;
				}
			}

			if (bCanOut)
			{
				//���ñ���
				OutCardResult.cbCardCount = CanOutCardType.cbEachHandCardCount[cbCanOutIndex];
				CopyMemory(OutCardResult.cbCardData, CanOutCardType.cbEachHandCardData[cbCanOutIndex], CanOutCardType.cbEachHandCardCount[cbCanOutIndex]*sizeof(BYTE));
				return;
			}
		}

		if (cbOutCardType == CT_SINGLE)
		{
			//�мҵ������
			BYTE cbLargestLogicCard = 0;
			if (wOutCardUser == wUndersideUser) 
			{
				cbLargestLogicCard = GetCardLogicValue(m_cbHandCardData[wUndersideUser][0]);
			}
			else if (wOutCardUser == wUpsideUser)
			{
				cbLargestLogicCard = GetCardLogicValue(m_cbHandCardData[wUpsideUser][0]);
			}

			if (GetCardLogicValue(cbTurnCardData[0])==14 || GetCardLogicValue(cbTurnCardData[0])>=cbLargestLogicCard || (GetCardLogicValue(cbTurnCardData[0])<cbLargestLogicCard-1) ||
				((wOutCardUser==wUndersideUser&&m_cbHandCardCount[wUndersideUser]<=5)||(wOutCardUser==wUpsideUser&&m_cbHandCardCount[wUpsideUser]<=5)))
			{
				//ȡһ�Ŵ��ڵ���2����Ҫ���мҳ����ƴ���ƣ�
				BYTE cbIndex = MAX_COUNT;
				for (BYTE i=0; i<cbHandCardCount; ++i)
				{
					if (GetCardLogicValue(cbHandCardData[i])>GetCardLogicValue(cbTurnCardData[0]) && GetCardLogicValue(cbHandCardData[i])>=15)
					{
						cbIndex = i;
					}
				}
				if (cbIndex != MAX_COUNT)
				{
					//���ñ���
					OutCardResult.cbCardCount = 1;
					OutCardResult.cbCardData[0] = cbHandCardData[cbIndex];
					return;
				}
			}
		}

		BYTE cbCurIndex=0, cbMinSingleCount=MAX_COUNT;
		for (BYTE i=0; i<MeCardTypeResult[cbOutCardType].cbCardTypeCount; ++i)
		{
			BYTE cbTempCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, MeCardTypeResult[cbOutCardType].cbEachHandCardData[i], MeCardTypeResult[cbOutCardType].cbEachHandCardCount[i]);
			if (cbMinSingleCount >= cbTempCount)
			{
				cbMinSingleCount = cbTempCount;
				cbCurIndex = i;
			}
		}
		//���ñ���
		OutCardResult.cbCardCount = MeCardTypeResult[cbOutCardType].cbEachHandCardCount[cbCurIndex];
		CopyMemory(OutCardResult.cbCardData, MeCardTypeResult[cbOutCardType].cbEachHandCardData[cbCurIndex], OutCardResult.cbCardCount);
		return;
	}

	//��Ҫ����ը��
	if (MeCardTypeResult[CT_BOMB_CARD].cbCardTypeCount > 0)
	{
		const tagOutCardTypeResult &BombCardType = MeCardTypeResult[CT_BOMB_CARD];
		BYTE cbMinLogicValue = GetCardLogicValue(BombCardType.cbEachHandCardData[0][0]);
		BYTE cbCurIndex = 0;
		for (BYTE i=0; i<BombCardType.cbCardTypeCount; ++i)
		{
			if (cbMinLogicValue > GetCardLogicValue(BombCardType.cbEachHandCardData[i][0]))
			{
				cbMinLogicValue = GetCardLogicValue(BombCardType.cbEachHandCardData[i][0]);
				cbCurIndex = i;
			}
		}

		bool bOutBomb = false;
		//һ���ƺ�һ��ը��
		BYTE cbRemainCardData[20]={0}, cbRemainCardCount=cbHandCardCount;
		CopyMemory(cbRemainCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
		RemoveCard(BombCardType.cbEachHandCardData[cbCurIndex], BombCardType.cbEachHandCardCount[cbCurIndex], cbRemainCardData, cbRemainCardCount);
		cbRemainCardCount -= BombCardType.cbEachHandCardCount[cbCurIndex];
		if (CT_ERROR != GetCardType(cbRemainCardData,cbRemainCardCount))
		{
			bOutBomb = true;
		}

		//ը������
		BYTE cbSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, BombCardType.cbEachHandCardData[cbCurIndex], BombCardType.cbEachHandCardCount[cbCurIndex]);
		if (cbSingleCardCount==0 && GetCardLogicValue(cbRemainCardData[0])>GetCardLogicValue(m_cbHandCardData[wUpsideUser==wOutCardUser?wUndersideUser:wUpsideUser][0]))
		{
			bOutBomb = true;
		}
		//ֻʣһ��
		BYTE cbRemainCardType = GetCardType(m_cbHandCardData[wOutCardUser], m_cbHandCardCount[wOutCardUser]);
		if (cbRemainCardType>CT_ERROR && cbRemainCardType<CT_FOUR_TAKE_ONE && GetCardLogicValue(m_cbHandCardData[wOutCardUser][0])<15 && 
			cbSingleCardCount<2 && (GetCardLogicValue(cbRemainCardData[0])>=GetCardLogicValue(m_cbHandCardData[wUndersideUser][0]) && 
			GetCardLogicValue(cbRemainCardData[0])>=GetCardLogicValue(m_cbHandCardData[wUpsideUser][0]))) 
		{
			bOutBomb = true;
		}

		//������
		if (cbRemainCardType!=CT_ERROR && m_lBankerOutCardCount==1)
		{
			bOutBomb = true;
		}

		//ֻʣ������
		BYTE cbRemainLargestCard = GetCardLogicValue(cbRemainCardData[0]);
		if (cbRemainCardCount<5 && cbRemainCardCount>0 && (cbRemainLargestCard!=GetCardLogicValue(BombCardType.cbEachHandCardData[cbCurIndex][0])) && 
			cbRemainLargestCard>GetCardLogicValue(m_cbHandCardData[wOutCardUser][0]) && cbRemainLargestCard>14)
		{
			bOutBomb = true;
		}

		//�����˿�
		tagAnalyseResult AnalyseResult;
		AnalysebCardData(cbRemainCardData, cbRemainCardCount, AnalyseResult);
		if (m_cbHandCardCount[wOutCardUser]==1 && (AnalyseResult.cbDoubleCount*2+AnalyseResult.cbThreeCount*3+AnalyseResult.cbFourCount*4+1>=cbRemainCardCount)) 
		{
			bOutBomb = true;
		}
		//���ñ���
		if (bOutBomb)
		{
			OutCardResult.cbCardCount = BombCardType.cbEachHandCardCount[cbCurIndex];
			CopyMemory(OutCardResult.cbCardData, BombCardType.cbEachHandCardData[cbCurIndex], BombCardType.cbEachHandCardCount[cbCurIndex]*sizeof(BYTE));
		}
		return;
	}
	//�������ֻʣһ����
	if (m_cbHandCardCount[wOutCardUser] == 1)
	{
		tagPromptCardResult MePromptCardResult;
		CGameLogic::SearchOutCard(cbHandCardData, cbHandCardCount, cbTurnCardData, cbTurnCardCount, MePromptCardResult);
		bool bCanOutBomb = false;
		//�������ж�
		if (m_lBankerOutCardCount == 1)
		{
			bCanOutBomb = true;
		}
		else
		{
			if (m_cbHandCardCount[wUndersideUser]==1 && m_cbHandCardCount[wUpsideUser]==1)
			{
				for (BYTE i=0; i<MePromptCardResult.cbResultCount; i++)
				{
					if (GetCardType(MePromptCardResult.cbCardData[i],MePromptCardResult.cbCardCount[i]) >= CT_BOMB_CARD)
					{
						BYTE cbWantOutCardData[MAX_COUNT]={0}, cbWantOutCardCount=0;
						cbWantOutCardCount = MePromptCardResult.cbCardCount[i];
						CopyMemory(cbWantOutCardData, &MePromptCardResult.cbCardData[i], sizeof(BYTE)*cbWantOutCardCount);
						BYTE cbTempCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, cbWantOutCardData, cbWantOutCardCount);
						if (cbTempCount <= 1)
						{
							bCanOutBomb = true;
							break;
						}
					}
				}
			}
		}
		//
		for (BYTE i=0; i<MePromptCardResult.cbResultCount; i++)
		{
			if (GetCardType(MePromptCardResult.cbCardData[i],MePromptCardResult.cbCardCount[i]) < CT_BOMB_CARD)
			{
				OutCardResult.cbCardCount = MePromptCardResult.cbCardCount[i];
				CopyMemory(OutCardResult.cbCardData, &MePromptCardResult.cbCardData[i], sizeof(BYTE)*OutCardResult.cbCardCount);
				return;
			}
		}
		for (BYTE i=0; i<MePromptCardResult.cbResultCount; i++)
		{
			if (GetCardType(MePromptCardResult.cbCardData[i],MePromptCardResult.cbCardCount[i])>=CT_BOMB_CARD && bCanOutBomb)
			{
				OutCardResult.cbCardCount = MePromptCardResult.cbCardCount[i];
				CopyMemory(OutCardResult.cbCardData, &MePromptCardResult.cbCardData[i], sizeof(BYTE)*OutCardResult.cbCardCount);
				return;
			}
		}
	}
 	//����¼�ֻʣһ����
 	if (m_cbHandCardCount[wUndersideUser]==1 && wOutCardUser==wUpsideUser && cbOutCardType==CT_SINGLE)
 	{
		if (GetCardLogicValue(cbHandCardData[0]) > GetCardLogicValue(cbTempCardData[0]))
		{
			OutCardResult.cbCardCount = 1;
			OutCardResult.cbCardData[0] = cbHandCardData[0];
			return;
		}
 	}
}

//�����ϼң��ȳ��ƣ�
void CAndroidLogic::UpsideOfBankerOutCard( const BYTE cbHandCardData[], const BYTE cbHandCardCount, const WORD wMeChairID, tagOutCardResult &OutCardResult )
{
	//��ʼ����
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));
	if (cbHandCardCount == 0) return;

	//�������
	const WORD wFriendUser = (m_wBankerUser+1)%GAME_PLAYER; //����,�����¼�

	//������������
	tagAnalyseResult MeAnalyseResult, BankerAnalyseResult;
	VERIFY(AnalysebCardData(cbHandCardData, cbHandCardCount, MeAnalyseResult));
	VERIFY(AnalysebCardData(m_cbHandCardData[m_wBankerUser], m_cbHandCardCount[m_wBankerUser], BankerAnalyseResult));
	//�Լ���������
	BYTE cbHandCardType = IsValidCount(cbHandCardCount) ? GetCardType(cbHandCardData,cbHandCardCount) : CT_ERROR;
	//�ж�һ�����ܷ����
	if (cbHandCardType != CT_ERROR)
	{
		//�����Ĵ�����ֱ�ӳ���
		if (cbHandCardType!=CT_FOUR_TAKE_ONE && cbHandCardType!=CT_FOUR_TAKE_TWO)
		{
			OutCardResult.cbCardCount = cbHandCardCount;
			CopyMemory(&OutCardResult.cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
			return;
		}
		else
		{
			//�����Ĵ�������
			Analyse4D2CardType(wMeChairID, cbHandCardData, cbHandCardCount, cbHandCardType, OutCardResult);
			if (OutCardResult.cbCardCount>0 && GetCardType(OutCardResult.cbCardData, OutCardResult.cbCardCount)!=CT_ERROR)
			{
				return;
			}
		}
	}
	//�ж��Ƿ��ǻ����һ��
	if (cbHandCardCount>2 && cbHandCardData[0]==0x4F && cbHandCardData[1]==0x4E)
	{
		cbHandCardType = GetCardType(&cbHandCardData[2], cbHandCardCount-2);
		if (cbHandCardType != CT_ERROR)
		{
			//�����ʣ�µĲ����Ĵ�����ֱ�ӳ�ը��
			if (cbHandCardType!=CT_FOUR_TAKE_ONE && cbHandCardType!=CT_FOUR_TAKE_TWO)
			{
				OutCardResult.cbCardCount = 2;
				OutCardResult.cbCardData[0] = 0x4F;
				OutCardResult.cbCardData[1] = 0x4E;
				return;
			}
			else
			{
				//�����Ĵ�������
				Analyse4D2CardType(wMeChairID, cbHandCardData, cbHandCardCount, cbHandCardType, OutCardResult);
				if (OutCardResult.cbCardCount>0 && GetCardType(OutCardResult.cbCardData, OutCardResult.cbCardCount)!=CT_ERROR)
				{
					return;
				}
			}
		}
	}


	BYTE cbFirstCardIndex = 0;
	if (cbHandCardCount>=2 && cbHandCardData[0]==0x4F && cbHandCardData[1]==0x4E)
	{
		cbFirstCardIndex = 2;
	}
	//�Լ�ֻʣ���ź�����һ����,���ҵ��Ż�һ�ֵ�����ѹ
	BYTE cbTempCardData[MAX_COUNT]={0}, cbTempCardCount=cbHandCardCount-cbFirstCardIndex;
	for (BYTE i=0; i<cbHandCardCount-cbFirstCardIndex; i++)
	{
		ZeroMemory(cbTempCardData, sizeof(cbTempCardData));
		cbTempCardCount = cbHandCardCount-cbFirstCardIndex;
		CopyMemory(cbTempCardData, &cbHandCardData[cbFirstCardIndex], sizeof(BYTE)*cbTempCardCount);
		BYTE cbSingleCard = cbTempCardData[i];
		RemoveCard(&cbSingleCard, 1, cbTempCardData, cbTempCardCount);
		cbTempCardCount -= 1;
		BYTE cbLastCardType = GetCardType(cbTempCardData, cbTempCardCount);
		if (cbLastCardType==CT_ERROR || cbLastCardType==CT_FOUR_TAKE_ONE || cbLastCardType==CT_FOUR_TAKE_TWO)
		{
			continue;
		}
		if (IsLargestCard(m_wBankerUser,cbTempCardData,cbTempCardCount))
		{
			OutCardResult.cbCardCount = cbTempCardCount;
			CopyMemory(OutCardResult.cbCardData, cbTempCardData, sizeof(BYTE)*cbTempCardCount);
			return;
		}
		if (IsLargestCard(m_wBankerUser,&cbSingleCard,1))
		{
			OutCardResult.cbCardCount = 1;
			OutCardResult.cbCardData[0] = cbSingleCard;
			return;
		}
	}
	//���ż�һ��,��һ��������
	for (BYTE i=0; i<cbHandCardCount-cbFirstCardIndex; i++)
	{
		ZeroMemory(cbTempCardData, sizeof(cbTempCardData));
		cbTempCardCount = cbHandCardCount-cbFirstCardIndex;
		CopyMemory(cbTempCardData, &cbHandCardData[cbFirstCardIndex], sizeof(BYTE)*cbTempCardCount);
		BYTE cbSingleCard = cbTempCardData[i];
		RemoveCard(&cbSingleCard, 1, cbTempCardData, cbTempCardCount);
		cbTempCardCount -= 1;
		BYTE cbLastCardType = GetCardType(cbTempCardData, cbTempCardCount);
		if (cbLastCardType==CT_ERROR || cbLastCardType==CT_FOUR_TAKE_ONE || cbLastCardType==CT_FOUR_TAKE_TWO || cbLastCardType==CT_DOUBLE || (cbLastCardType==CT_BOMB_CARD&&m_cbHandCardCount[m_wBankerUser]>1))
		{
			continue;
		}
		tagAnalyseResult TempAnalyseResult;
		AnalysebCardData(cbTempCardData, cbTempCardCount, TempAnalyseResult);
		if (TempAnalyseResult.cbFourCount > 0) continue;
		OutCardResult.cbCardCount = cbTempCardCount;
		CopyMemory(OutCardResult.cbCardData, cbTempCardData, sizeof(BYTE)*cbTempCardCount);
		return;
	}
	//����������
	if (cbHandCardCount==4 && cbHandCardData[0]==0x4F && cbHandCardData[1]==0x4E && m_cbHandCardCount[m_wBankerUser]==1)
	{
		OutCardResult.cbCardCount = 1;
		OutCardResult.cbCardData[0] = cbHandCardData[2];
		return;
	}

	//���ƽ����ж�
	if (m_cbHandCardCount[m_wBankerUser]==1 && cbHandCardCount>=2 && m_cbHandCardCount[wFriendUser]>=2)
	{
		tagAnalyseResult MeAnalyseResult, FriendAnalyseResult;
		//�����˿�
		AnalysebCardData(cbHandCardData, cbHandCardCount, MeAnalyseResult);
		AnalysebCardData(m_cbHandCardData[wFriendUser], m_cbHandCardCount[wFriendUser], FriendAnalyseResult);

		//�����ж�
		if ((m_cbHandCardCount[wFriendUser]==FriendAnalyseResult.cbDoubleCount*2+FriendAnalyseResult.cbThreeCount*3+FriendAnalyseResult.cbFourCount*4||m_cbHandCardCount[wFriendUser]==FriendAnalyseResult.cbDoubleCount*2+FriendAnalyseResult.cbThreeCount*3+FriendAnalyseResult.cbFourCount*4+1) 
			&& MeAnalyseResult.cbDoubleCount>0 && FriendAnalyseResult.cbDoubleCount>0)
		{
			//��С����
			BYTE cbMeMinDoubleCardValue = GetCardLogicValue(MeAnalyseResult.cbDoubleCardData[MeAnalyseResult.cbDoubleCount*2-2]);
			//������
			BYTE cbFriendMaxDoublecardValue = GetCardLogicValue(FriendAnalyseResult.cbDoubleCardData[0]);
			//�����ж�
			if (cbMeMinDoubleCardValue<14 && cbMeMinDoubleCardValue<cbFriendMaxDoublecardValue)
			{
				OutCardResult.cbCardCount = 2;
				OutCardResult.cbCardData[0] = MeAnalyseResult.cbDoubleCardData[MeAnalyseResult.cbDoubleCount*2-2];
				OutCardResult.cbCardData[1] = MeAnalyseResult.cbDoubleCardData[MeAnalyseResult.cbDoubleCount*2-1];
				return;
			}          			
		}
	}

	//���±�û��
	tagOutCardTypeResult MeOutCardTypeResult[12+1];
	AnalyseOutCardType(cbHandCardData, cbHandCardCount, MeOutCardTypeResult);
	//����ֻʣһ��
	AnalyseEnemyLastOneCard(wMeChairID, cbHandCardData, cbHandCardCount, MeOutCardTypeResult, OutCardResult);
	if (OutCardResult.cbCardCount>0 && GetCardType(OutCardResult.cbCardData,OutCardResult.cbCardCount)!=CT_ERROR) return;
	//����ֻʣ����
	AnalyseEnemyLastTwoCard(wMeChairID, cbHandCardData, cbHandCardCount, MeOutCardTypeResult, OutCardResult);
	if (OutCardResult.cbCardCount>0 && GetCardType(OutCardResult.cbCardData,OutCardResult.cbCardCount)!=CT_ERROR) return;

	//��һ�ַ���/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		//���㵥��
		BYTE cbSingleCardCount[4] = {MAX_COUNT,MAX_COUNT,MAX_COUNT,MAX_COUNT};
		BYTE cbOutCardIndex[4] = {0};
		BYTE cbOutCardType[4] = {CT_ERROR};
		BYTE cbMinSingleCardCount = MAX_COUNT;
		BYTE cbMinCardType = CT_ERROR;
		BYTE cbMinCardIndex = 0;
		//������������
		BYTE cbBankerDoubleCardCount=0, cbBankerDoubleCardData[MAX_COUNT]={0};
		GetAllDoubleCard(m_cbHandCardData[m_wBankerUser], m_cbHandCardCount[m_wBankerUser], cbBankerDoubleCardData, cbBankerDoubleCardCount);
		//��ը�������
		for (BYTE cbCardType=CT_DOUBLE; cbCardType<CT_BOMB_CARD; ++cbCardType)
		{
			const tagOutCardTypeResult &tempOutCardTypeResult = MeOutCardTypeResult[cbCardType];
			//��ͬ���ͣ���ͬ���ȣ���������������ͬ���Ϳ��ܳ��Ȳ�һ��
			BYTE cbThisHandCardCount = MAX_COUNT;
			//�����˿�
			tagOutCardTypeResult BankerCanOutCardResult[13], FriendOutCardTypeResult[13];
			for (BYTE i=0; i<tempOutCardTypeResult.cbCardTypeCount; ++i)
			{
				//��ֹ��������ը���ж�
				if (cbCardType == CT_DOUBLE)
				{
					tagAnalyseResult AnalyseResult;
					AnalysebCardData(cbHandCardData, cbHandCardCount, AnalyseResult);
					if (AnalyseResult.cbSingleCount+AnalyseResult.cbThreeCount*3 == cbHandCardCount)
					{
						bool bContinue = false;
						for (BYTE cbThreeIndex=0; cbThreeIndex<AnalyseResult.cbThreeCount; ++cbThreeIndex)
						{
							if (GetCardValue(tempOutCardTypeResult.cbEachHandCardData[i][0]) == GetCardValue(AnalyseResult.cbThreeCardData[3*cbThreeIndex]))
							{
								bContinue = true;
								break;
							}
						}
						if (bContinue) continue;
					}
					if (AnalyseResult.cbFourCount > 0)
					{
						bool bContinue = false;
						for (BYTE cbFourIndex=0; cbFourIndex<AnalyseResult.cbFourCount; cbFourIndex++)
						{
							if (GetCardValue(tempOutCardTypeResult.cbEachHandCardData[i][0]) == GetCardValue(AnalyseResult.cbFourCardData[4*cbFourIndex]))
							{
								bContinue = true;
								break;
							}
						}
						if (bContinue) continue;
					}
				}

				BYTE cbTempCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, tempOutCardTypeResult.cbEachHandCardData[i], tempOutCardTypeResult.cbEachHandCardCount[i]);
				//���ض���
				if (cbCardType==CT_DOUBLE && cbBankerDoubleCardCount>0 && GetCardLogicValue(cbBankerDoubleCardData[cbBankerDoubleCardCount-1])<10 &&
					GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][0])<GetCardLogicValue(cbBankerDoubleCardData[cbBankerDoubleCardCount-1]) &&
					GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[0][0])>=10 && GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[0][0])<14)
				{
					continue;
				}
                //���·���
				if (tempOutCardTypeResult.cbEachHandCardCount[i] != cbThisHandCardCount)
				{
					cbThisHandCardCount = tempOutCardTypeResult.cbEachHandCardCount[i];
					AnalyseOutCardType(m_cbHandCardData[m_wBankerUser], m_cbHandCardCount[m_wBankerUser], tempOutCardTypeResult.cbEachHandCardData[i], tempOutCardTypeResult.cbEachHandCardCount[i], BankerCanOutCardResult);
					AnalyseOutCardType(m_cbHandCardData[wFriendUser], m_cbHandCardCount[wFriendUser], tempOutCardTypeResult.cbEachHandCardData[i], tempOutCardTypeResult.cbEachHandCardCount[i], FriendOutCardTypeResult);
				}

 				//���˳�ӣ������Ĵ���
 				BYTE cbCurrentCardType = GetCardType(tempOutCardTypeResult.cbEachHandCardData[i], cbThisHandCardCount);
 				if (cbThisHandCardCount!=cbHandCardCount && cbCurrentCardType>=CT_SINGLE_LINE && cbCurrentCardType<=CT_THREE_TAKE_TWO &&
 				    (GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][cbThisHandCardCount-1])>GetCardLogicValue(cbHandCardData[cbHandCardCount-2])||GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][0])>=11))
 				{
 					BYTE cbRemainCardCount=0, cbRemainCardData[MAX_COUNT]={0};
 					CopyMemory(cbRemainCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
 					cbRemainCardCount = cbHandCardCount;
 					RemoveCard(tempOutCardTypeResult.cbEachHandCardData[i], cbThisHandCardCount, cbRemainCardData, cbRemainCardCount);
 					cbRemainCardCount -= cbThisHandCardCount;
 
 					//����˿�
 					BYTE cbCurrentLargestLogicCard = GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][0]);
 					if (GetCardType(cbRemainCardData,cbRemainCardCount)==CT_ERROR && (cbCurrentCardType>=CT_THREE_TAKE_ONE && 
 						cbCurrentCardType<=CT_THREE_TAKE_TWO && cbCurrentLargestLogicCard>=11 && cbThisHandCardCount<=5 || 
 						cbCurrentCardType==CT_SINGLE_LINE && cbThisHandCardCount<=6 && cbCurrentLargestLogicCard>=12 || 
 						cbCurrentCardType>=CT_DOUBLE_LINE && cbCurrentCardType<=CT_THREE_LINE && cbCurrentLargestLogicCard>=12 && cbThisHandCardCount<=8))
 					{
 						//��ʱ����
 						if (cbCurrentCardType>=CT_SINGLE_LINE && cbCurrentCardType<=CT_THREE_LINE && GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][cbThisHandCardCount-1])>GetCardLogicValue(cbHandCardData[cbHandCardCount-3]))
 						{
 							continue;
 						}
 
 						if (cbCurrentCardType>=CT_THREE_TAKE_ONE && cbCurrentCardType<=CT_THREE_TAKE_TWO && GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][0])>GetCardLogicValue(cbHandCardData[cbHandCardCount-3]))
 						{
 							continue;
 						}
 					}
 				}

				//��������ѹ�ƣ����Ҷ��Ѳ�����ѹ����
				if ((BankerCanOutCardResult[cbCardType].cbCardTypeCount>0&&FriendOutCardTypeResult[cbCardType].cbCardTypeCount==0) ||
					(BankerCanOutCardResult[cbCardType].cbCardTypeCount>0&&FriendOutCardTypeResult[cbCardType].cbCardTypeCount>0&&GetCardLogicValue(FriendOutCardTypeResult[cbCardType].cbEachHandCardData[0][0])<=GetCardLogicValue(BankerCanOutCardResult[cbCardType].cbEachHandCardData[0][0])))
				{
					//�����ܵ�
					if (BankerCanOutCardResult[cbCardType].cbEachHandCardCount[0]>0 && m_cbHandCardCount[m_wBankerUser]==BankerCanOutCardResult[cbCardType].cbEachHandCardCount[0]+1)
					{
						continue;
					}

					BYTE cbRemainCardCount=0, cbRemainCardData[MAX_COUNT]={0};
					CopyMemory(cbRemainCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
					cbRemainCardCount = cbHandCardCount;
					RemoveCard(tempOutCardTypeResult.cbEachHandCardData[i], tempOutCardTypeResult.cbEachHandCardCount[i], cbRemainCardData, cbRemainCardCount);
					cbRemainCardCount -= tempOutCardTypeResult.cbEachHandCardCount[i];
					tagOutCardTypeResult RemainCardTypeResult[12+1];
					AnalyseOutCardType(cbRemainCardData, cbRemainCardCount, tempOutCardTypeResult.cbEachHandCardData[i], tempOutCardTypeResult.cbEachHandCardCount[i], RemainCardTypeResult);
					//�Լ����������û���Ȩ
					if (RemainCardTypeResult[cbCardType].cbCardTypeCount==0 || GetCardLogicValue(RemainCardTypeResult[cbCardType].cbEachHandCardData[0][0]) <= GetCardLogicValue(BankerCanOutCardResult[cbCardType].cbEachHandCardData[0][0])) 
					{
						continue;
					}
				}

				//�Ƿ��д���
				if (tempOutCardTypeResult.cbEachHandCardCount[i] != cbHandCardCount)
				{
					bool bHaveLargeCard = false;
					for (BYTE j=0; j<tempOutCardTypeResult.cbEachHandCardCount[i]; ++j)
					{
						if (GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][j]) >= 15)
						{
							bHaveLargeCard = true;
						}
					}
					if (cbCardType!=CT_SINGLE_LINE && cbCardType!=CT_DOUBLE_LINE && GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][0])==14)
					{
						bHaveLargeCard = true; 
					}
					if (bHaveLargeCard) continue;
				}

				//�����Ƿ�����ߵ������ﶼû�п���ը��
				if (tempOutCardTypeResult.cbEachHandCardCount[i]==m_cbHandCardCount[m_wBankerUser] && cbCardType==GetCardType(m_cbHandCardData[m_wBankerUser],m_cbHandCardCount[m_wBankerUser])
					&& GetCardLogicValue(m_cbHandCardData[m_wBankerUser][0])>GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][0]))
				{
					continue;
				}

				//����cbMinSingleCardCount[4]�����ֵ
				for (BYTE j=0; j<4; ++j)
				{
					if (cbSingleCardCount[j] >= cbTempCount)
					{
						cbSingleCardCount[j] = cbTempCount;
						cbOutCardIndex[j] = i;
						cbOutCardType[j] = cbCardType;
						break;
					}
				}

				//������Сֵ
				if (cbMinSingleCardCount >= cbTempCount)
				{
					//��С����
					cbMinCardType = cbCardType;
					//��С�����е���С����
					cbMinSingleCardCount = cbTempCount;						
					//��С�����е���С��
					cbMinCardIndex = i;
				}
			}
		}

		if (cbMinSingleCardCount>=AnalyseSinleCardCount(cbHandCardData,cbHandCardCount,NULL,0)+3 && m_cbHandCardCount[m_wBankerUser]>4)
		{
			cbMinSingleCardCount = MAX_COUNT;
		}

		if (cbMinSingleCardCount != MAX_COUNT)
		{
			BYTE cbCurIndex = cbMinCardIndex;
			//ѡ����С��
			for (BYTE i=0; i<4; ++i)
			{
				if (cbOutCardType[i]==cbMinCardType && cbSingleCardCount[i]<=cbMinSingleCardCount && GetCardLogicValue(MeOutCardTypeResult[cbMinCardType].cbEachHandCardData[cbOutCardIndex[i]][0])<GetCardLogicValue(MeOutCardTypeResult[cbMinCardType].cbEachHandCardData[cbCurIndex][0]))
				{
					cbCurIndex = cbOutCardIndex[i];
				}
			}
			//���ñ���
			OutCardResult.cbCardCount = MeOutCardTypeResult[cbMinCardType].cbEachHandCardCount[cbCurIndex];
			CopyMemory(OutCardResult.cbCardData, MeOutCardTypeResult[cbMinCardType].cbEachHandCardData[cbCurIndex], MeOutCardTypeResult[cbMinCardType].cbEachHandCardCount[cbCurIndex]*sizeof(BYTE));
			return;
		}
	}

	//��һ�ַ���/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		BYTE cbFirstCardIndex = 0;
		//��������2
		for (BYTE i=0; i<cbHandCardCount; ++i) 
		{
			if (GetCardLogicValue(cbHandCardData[i]) < 15)
			{
				cbFirstCardIndex = i;
				break;
			}
		}

		if (cbFirstCardIndex < cbHandCardCount-1)
		{
			AnalyseOutCardType(cbHandCardData+cbFirstCardIndex, cbHandCardCount-cbFirstCardIndex, MeOutCardTypeResult);
		}
		else
		{
			AnalyseOutCardType(cbHandCardData, cbHandCardCount, MeOutCardTypeResult);
		}

		//���㵥��
		BYTE cbSingleCardCount[4] = {MAX_COUNT,MAX_COUNT,MAX_COUNT,MAX_COUNT};
		BYTE cbOutCardIndex[4] = {0};
		BYTE cbOutcardType[4] = {CT_ERROR};
		BYTE cbMinSingleCardCount = MAX_COUNT;
		BYTE cbMinCardType = CT_ERROR;
		BYTE cbMinCardIndex = 0;

		//������������
		BYTE cbBankerSingleCardData[MAX_COUNT] = {0};
		BYTE cbBankerSingleCardCount = AnalyseSinleCardCount(m_cbHandCardData[m_wBankerUser], m_cbHandCardCount[m_wBankerUser], NULL, 0, cbBankerSingleCardData);
		BYTE cbBankerSingleCardLogicValue = 0;
		if (cbBankerSingleCardCount>=2 && GetCardLogicValue(cbBankerSingleCardData[cbBankerSingleCardCount-2])<=10)
		{
			cbBankerSingleCardLogicValue = GetCardLogicValue(cbBankerSingleCardData[cbBankerSingleCardCount-2]);
		}
		else if (cbBankerSingleCardCount>=2 && GetCardLogicValue(cbBankerSingleCardData[cbBankerSingleCardCount-1])<=10) 
		{
			cbBankerSingleCardLogicValue = GetCardLogicValue(cbBankerSingleCardData[cbBankerSingleCardCount-1]);
		}
		else if (cbBankerSingleCardCount>0 && GetCardLogicValue(cbBankerSingleCardData[0])<=10)
		{
			cbBankerSingleCardLogicValue = GetCardLogicValue(cbBankerSingleCardData[0]);
		}
		//������������
		BYTE cbBankerDoubleCardCount=0, cbBankerDoubleCardData[MAX_COUNT]={0};
		GetAllDoubleCard(m_cbHandCardData[m_wBankerUser], m_cbHandCardCount[m_wBankerUser], cbBankerDoubleCardData, cbBankerDoubleCardCount);
		//��ը�������
		for (BYTE cbCardType=CT_SINGLE; cbCardType<CT_BOMB_CARD; ++cbCardType)
		{
			const tagOutCardTypeResult &tempOutCardTypeResult = MeOutCardTypeResult[cbCardType];
			for (BYTE i=0; i<tempOutCardTypeResult.cbCardTypeCount; ++i)
			{
				//��ֹ��������ը���ж�
				if (cbCardType == CT_DOUBLE)
				{
					tagAnalyseResult AnalyseResult;
					AnalysebCardData(cbHandCardData, cbHandCardCount, AnalyseResult);
					if (AnalyseResult.cbSingleCount+AnalyseResult.cbThreeCount*3 == cbHandCardCount)
					{
						bool bContinue = false;
						for (BYTE cbThreeIndex=0; cbThreeIndex<AnalyseResult.cbThreeCount; ++cbThreeIndex)
						{
							if (GetCardValue(tempOutCardTypeResult.cbEachHandCardData[i][0]) == GetCardValue(AnalyseResult.cbThreeCardData[3*cbThreeIndex]))
							{
								bContinue = true;
								break;
							}
						}
						if (bContinue) continue;
					}
					if (AnalyseResult.cbFourCount > 0)
					{
						bool bContinue = false;
						for (BYTE cbFourIndex=0; cbFourIndex<AnalyseResult.cbFourCount; cbFourIndex++)
						{
							if (GetCardValue(tempOutCardTypeResult.cbEachHandCardData[i][0]) == GetCardValue(AnalyseResult.cbFourCardData[4*cbFourIndex]))
							{
								bContinue = true;
								break;
							}
						}
						if (bContinue) continue;
					}
				}
				if (cbCardType==CT_SINGLE && cbBankerSingleCardCount>0 && GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][0])<cbBankerSingleCardLogicValue)
				{
					continue;
				}

				//���ض���
				if (cbCardType==CT_DOUBLE && cbBankerDoubleCardCount>0 && GetCardLogicValue(cbBankerDoubleCardData[cbBankerDoubleCardCount-1])<10 &&
					GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][0])<GetCardLogicValue(cbBankerDoubleCardData[cbBankerDoubleCardCount-1]) &&
					GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[0][0])>=10 && GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[0][0])<14)
				{
					continue;
				}
 				//���˳�ӣ������Ĵ���
 				if (tempOutCardTypeResult.cbEachHandCardCount[i]!=cbHandCardCount && cbCardType>=CT_SINGLE_LINE && cbCardType<=CT_THREE_TAKE_TWO &&
 					(GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][tempOutCardTypeResult.cbEachHandCardCount[i]-1])>GetCardLogicValue(cbHandCardData[cbHandCardCount-2])||GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][0])>=11))
 				{
 					BYTE cbRemainCardCount=0, cbRemainCardData[MAX_COUNT]={0};
 					CopyMemory(cbRemainCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
 					cbRemainCardCount = cbHandCardCount;
 					RemoveCard(tempOutCardTypeResult.cbEachHandCardData[i], tempOutCardTypeResult.cbEachHandCardCount[i], cbRemainCardData, cbRemainCardCount);
 					cbRemainCardCount -= tempOutCardTypeResult.cbEachHandCardCount[i];
 
 					//����˿�
 					BYTE cbCurrentLargestLogicCard = GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][0]);
 
 					if (GetCardType(cbRemainCardData,cbRemainCardCount)==CT_ERROR && (cbCardType>=CT_THREE_TAKE_ONE && 
 						cbCardType<=CT_THREE_TAKE_TWO && cbCurrentLargestLogicCard>=11 && tempOutCardTypeResult.cbEachHandCardCount[i]<=5 || 
 						cbCardType==CT_SINGLE_LINE && tempOutCardTypeResult.cbEachHandCardCount[i]<=6 && cbCurrentLargestLogicCard>=12 || 
 						cbCardType>=CT_DOUBLE_LINE && cbCardType<=CT_THREE_LINE && cbCurrentLargestLogicCard>=12 && tempOutCardTypeResult.cbEachHandCardCount[i]<=8))
 					{
 						//��ʱ����
 						if (cbCardType>=CT_SINGLE_LINE && cbCardType<=CT_THREE_LINE && GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][tempOutCardTypeResult.cbEachHandCardCount[i]-1])>GetCardLogicValue(cbHandCardData[cbHandCardCount-3]))
 						{
 							continue;
 						}
 						if (cbCardType>=CT_THREE_TAKE_ONE && cbCardType<=CT_THREE_TAKE_TWO &&	GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][0])>GetCardLogicValue(cbHandCardData[cbHandCardCount-3]))
 						{
 							continue;
 						}
 					}
 				}

				BYTE cbTempCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, tempOutCardTypeResult.cbEachHandCardData[i], tempOutCardTypeResult.cbEachHandCardCount[i]);
				//����cbMinSingleCardCount[4]�����ֵ
				for (BYTE j=0; j<4; ++j)
				{
					if (cbSingleCardCount[j] >= cbTempCount)
					{
						cbSingleCardCount[j] = cbTempCount;
						cbOutCardIndex[j] = i;
						cbOutcardType[j] = cbCardType;
						break;
					}
				}

				//������Сֵ
				if (cbMinSingleCardCount >= cbTempCount)
				{
					//��С����
					cbMinCardType = cbCardType;
					//��С�����е���С����
					cbMinSingleCardCount = cbTempCount;						
					//��С�����е���С��
					cbMinCardIndex = i;
				}
			}
		}

		if (cbMinSingleCardCount != MAX_COUNT)
		{
			BYTE cbCurIndex = cbMinCardIndex;
			//ѡ����С��
			for (BYTE i=0; i<4; ++i)
			{
				if (cbOutcardType[i]==cbMinCardType && cbSingleCardCount[i]<=cbMinSingleCardCount && GetCardLogicValue(MeOutCardTypeResult[cbMinCardType].cbEachHandCardData[cbOutCardIndex[i]][0])<GetCardLogicValue(MeOutCardTypeResult[cbMinCardType].cbEachHandCardData[cbCurIndex][0]))
				{
					cbCurIndex = cbOutCardIndex[i];
				}
			}
			//���ñ���
			OutCardResult.cbCardCount = MeOutCardTypeResult[cbMinCardType].cbEachHandCardCount[cbCurIndex];
			CopyMemory(OutCardResult.cbCardData, MeOutCardTypeResult[cbMinCardType].cbEachHandCardData[cbCurIndex], MeOutCardTypeResult[cbMinCardType].cbEachHandCardCount[cbCurIndex]*sizeof(BYTE));
			return;
		}
	}

	//�������Ͳ��ҷ���ʧ��
	//ֻʣը���ͻ��
	ZeroMemory(cbTempCardData, sizeof(cbTempCardData));
	cbTempCardCount = cbHandCardCount-cbFirstCardIndex;
	CopyMemory(cbTempCardData, &cbHandCardData[cbFirstCardIndex], sizeof(BYTE)*cbTempCardCount);
	AnalysebCardData(cbTempCardData, cbTempCardCount, MeAnalyseResult);
	if (MeAnalyseResult.cbFourCount>0 && MeAnalyseResult.cbFourCount*4==cbTempCardCount)
	{
		for (int i=MeAnalyseResult.cbFourCount-1; i>=0; i--)
		{
			OutCardResult.cbCardCount = 4;
			CopyMemory(OutCardResult.cbCardData, &MeAnalyseResult.cbFourCardData[i*4], sizeof(BYTE)*4);
			return;
		}
	}
	BYTE cbAllSingleCardCount=0, cbAllSingleCardData[MAX_COUNT]={0};
	cbAllSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, NULL, 0, cbAllSingleCardData);
	if (cbAllSingleCardCount > 0)
	{
		//�����û���������ͳ���С��һ��
		if (1 == m_cbHandCardCount[m_wBankerUser]) 
		{
			OutCardResult.cbCardCount = 1;
			OutCardResult.cbCardData[0] = cbAllSingleCardData[0];
		}
		else 
		{
			OutCardResult.cbCardCount = 1;
			OutCardResult.cbCardData[0] = cbAllSingleCardData[cbAllSingleCardCount-1];
		}
		return;
	}

	//�����û���������ͳ���С��һ��
	OutCardResult.cbCardCount = 1;
	OutCardResult.cbCardData[0] = cbHandCardData[cbHandCardCount-1];
	return;
}

//�����ϼң�����ƣ�
void CAndroidLogic::UpsideOfBankerOutCard( const BYTE cbHandCardData[], const BYTE cbHandCardCount, const BYTE cbTurnCardData[], const BYTE cbTurnCardCount, const WORD wMeChairID, const WORD wOutCardUser, tagOutCardResult &OutCardResult ) 
{	
	//��ʼ����
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));
	//��������
	BYTE cbOutCardType = GetCardType(cbTurnCardData, cbTurnCardCount);
	if (cbOutCardType == CT_MISSILE_CARD) return;

	//�������
	const WORD wFriendUser = (m_wBankerUser+1)%GAME_PLAYER;	//����,�����¼�

	//�Լ�ֻʣ�����һ��
	BYTE cbFirstCardIndex = 0;
	BYTE cbTempCardData[MAX_COUNT]={0}, cbTempCardCount=0;
	if (cbHandCardCount>=2 && cbHandCardData[0]==0x4F && cbHandCardData[1]==0x4E)
	{
		cbFirstCardIndex = 2;
		if (cbHandCardCount==2 || GetCardType(&cbHandCardData[2],cbHandCardCount-2)!=CT_ERROR)
		{
			OutCardResult.cbCardCount = 2;
			OutCardResult.cbCardData[0] = 0x4F;
			OutCardResult.cbCardData[1] = 0x4E;
			return;
		}
	}

	//�Լ�ʣ����������ܴ��������,����һ�����в�����ը��(ը�����ͳ���)
	BYTE cbBombCardCount=0, cbBombCardData[MAX_COUNT]={0};
	GetAllBombCard(cbHandCardData, cbHandCardCount, cbBombCardData, cbBombCardCount);
	if (CompareCard(cbTurnCardData,cbHandCardData,cbTurnCardCount,cbHandCardCount) && (GetCardType(cbHandCardData,cbHandCardCount)>=CT_BOMB_CARD||cbBombCardCount==0))
	{
		OutCardResult.cbCardCount = cbHandCardCount;
		CopyMemory(OutCardResult.cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
		return;
	}

	//�Լ�ֻʣը���ͻ��
	cbTempCardCount = cbHandCardCount-cbFirstCardIndex;
	CopyMemory(cbTempCardData, &cbHandCardData[cbFirstCardIndex], sizeof(BYTE)*cbTempCardCount);
	tagAnalyseResult MeAnalyseResult;
	AnalysebCardData(cbTempCardData, cbTempCardCount, MeAnalyseResult);
	//�ж��Ƿ���Գ�ը��
	bool bCanOutBombCard = false;
	BYTE cbCard2 = 0x02;
	//�����ѳ�
	if (IsLargestCardWithOutBomb(wMeChairID,&cbCard2,1))
	{
		bCanOutBombCard = true;
	}
	//������ڵ������п��Գ�
	if (m_cbHandCardCount[m_wBankerUser]>=2 && m_cbHandCardData[m_wBankerUser][1]!=0x4E)
	{
		bCanOutBombCard = true;
	}
	//ȥ������������ƻ���������
	if (m_cbHandCardCount[m_wBankerUser] >= 5)
	{
		bCanOutBombCard = true;
	}
	if (MeAnalyseResult.cbFourCount>0 && MeAnalyseResult.cbFourCount*4==cbTempCardCount)
	{
		for (int i=MeAnalyseResult.cbFourCount-1; i>=0; i--)
		{
			if (bCanOutBombCard && CompareCard(cbTurnCardData,&MeAnalyseResult.cbFourCardData[i*4],cbTurnCardCount,4))
			{
				OutCardResult.cbCardCount = 4;
				CopyMemory(OutCardResult.cbCardData, &MeAnalyseResult.cbFourCardData[i*4], sizeof(BYTE)*4);
				return;
			}
			else if (cbHandCardCount>=2 && cbHandCardData[0]==0x4F && cbHandCardData[1]==0x4E)
			{
				OutCardResult.cbCardCount = 2;
				OutCardResult.cbCardData[0] = 0x4F;
				OutCardResult.cbCardData[1] = 0x4E;
				return;
			}
		}
	}

	//ը����һ��
	if (cbBombCardCount>0 && m_wBankerUser==wOutCardUser)
	{
		BYTE cbRemainCardData[MAX_COUNT]={0}, cbRemainCardCount=cbHandCardCount;
		CopyMemory(cbRemainCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
		RemoveCard(cbBombCardData, cbBombCardCount, cbRemainCardData, cbRemainCardCount);
		cbRemainCardCount -= cbBombCardCount;
		BYTE cbRemainMaxValue = cbRemainCardCount>0 ? GetCardLogicValue(cbRemainCardData[0]) : 0;
		BYTE cbBankerMaxValue = GetCardLogicValue(m_cbHandCardData[m_wBankerUser][0]);
		if ((cbRemainCardCount>0&&GetCardType(cbRemainCardData,cbRemainCardCount)!=CT_ERROR) || (cbRemainCardCount==2&&cbRemainMaxValue>cbBankerMaxValue))
		{
			//˫��ը��
			if (GetCardColor(cbBombCardData[0]) == 0x40)
			{
				OutCardResult.cbCardCount = 2;
				OutCardResult.cbCardData[0] = 0x4F;
				OutCardResult.cbCardData[1] = 0x4E;
				return;
			}
			else
			{
				//ը����ѹ������
				if (CompareCard(cbTurnCardData,cbBombCardData,cbTurnCardCount,4))
				{
					tagPromptCardResult BankerPromptCardResult;
					CGameLogic::SearchOutCard(m_cbHandCardData[m_wBankerUser], m_cbHandCardCount[m_wBankerUser], cbBombCardData, 4, BankerPromptCardResult);
					//�س�ը��
					if (BankerPromptCardResult.cbResultCount == 0)
					{
						OutCardResult.cbCardCount = 4;
						CopyMemory(OutCardResult.cbCardData, cbBombCardData, sizeof(BYTE)*4);
						return;
					}
					else
					{
						if (CompareCard(cbTurnCardData,cbRemainCardData,cbTurnCardCount,cbRemainCardCount))
						{
							OutCardResult.cbCardCount = cbRemainCardCount;
							CopyMemory(OutCardResult.cbCardData, cbRemainCardData, sizeof(BYTE)*cbRemainCardCount);
							return;
						}
					}
				}
			}
		}
	}

	//�����ɳ���
	tagOutCardTypeResult MeOutCardTypeResult[12+1], BankerOutCardTypeResult[13];
	AnalyseOutCardType(cbHandCardData, cbHandCardCount, cbTurnCardData, cbTurnCardCount, MeOutCardTypeResult);
	AnalyseOutCardType(m_cbHandCardData[m_wBankerUser], m_cbHandCardCount[m_wBankerUser], BankerOutCardTypeResult);
	//������һ�ŵ���
	if (m_cbHandCardCount[m_wBankerUser] == 1)
	{
		if (cbOutCardType == CT_SINGLE)
		{
			if (m_wBankerUser == wOutCardUser)
			{
				//�жϳ���ը����ʣ��ֻ��һ������
				if (cbBombCardCount > 0)
				{
					BYTE cbRemainCardData[MAX_COUNT]={0}, cbRemainCardCount=cbHandCardCount;
					CopyMemory(cbRemainCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
					RemoveCard(cbBombCardData, cbBombCardCount, cbRemainCardData, cbRemainCardCount);
					cbRemainCardCount -= cbBombCardCount;
					tagAnalyseResult RemainAnalyseResult;
					AnalysebCardData(cbRemainCardData, cbRemainCardCount, RemainAnalyseResult);
					//��ը
					if (RemainAnalyseResult.cbThreeCount+1 >= RemainAnalyseResult.cbSingleCount)
					{
						//˫��ը��
						if (GetCardColor(cbBombCardData[0]) == 0x40)
						{
							OutCardResult.cbCardCount = 2;
							OutCardResult.cbCardData[0] = 0x4F;
							OutCardResult.cbCardData[1] = 0x4E;
							return;
						}
						else
						{
							OutCardResult.cbCardCount = 4;
							CopyMemory(OutCardResult.cbCardData, cbBombCardData, sizeof(BYTE)*4);
							return;
						}	
					}
					else
					{
						if (GetCardLogicValue(cbHandCardData[0]) > GetCardLogicValue(cbTurnCardData[0]))
						{
							OutCardResult.cbCardCount = 1;
							OutCardResult.cbCardData[0] = cbHandCardData[0];
							return;
						}
					}					
				}
				else
				{
					if (GetCardLogicValue(cbHandCardData[0]) > GetCardLogicValue(cbTurnCardData[0]))
					{
						OutCardResult.cbCardCount = 1;
						OutCardResult.cbCardData[0] = cbHandCardData[0];
						return;
					}
				}
			}
			else if (wFriendUser == wOutCardUser)
			{
				//�жϳ���ը����ʣ��ֻ��һ������
				if (cbBombCardCount > 0)
				{
					BYTE cbRemainCardCount=0, cbRemainCardData[MAX_COUNT]={0};
					CopyMemory(cbRemainCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
					cbRemainCardCount = cbHandCardCount;
					RemoveCard(cbBombCardData, cbBombCardCount, cbRemainCardData, cbRemainCardCount);
					cbRemainCardCount -= cbBombCardCount;
					tagAnalyseResult RemainAnalyseResult;
					AnalysebCardData(cbRemainCardData, cbRemainCardCount, RemainAnalyseResult);
					//��ը
					if (RemainAnalyseResult.cbThreeCount+1 >= RemainAnalyseResult.cbSingleCount)
					{
						//˫��ը��
						if (GetCardColor(cbBombCardData[0]) == 0x40)
						{
							OutCardResult.cbCardCount = 2;
							OutCardResult.cbCardData[0] = 0x4F;
							OutCardResult.cbCardData[1] = 0x4E;
							return;
						}
						else
						{
							OutCardResult.cbCardCount = 4;
							CopyMemory(OutCardResult.cbCardData, cbBombCardData, sizeof(BYTE)*4);
							return;
						}	
					}
					else
					{
						if (!IsLargestCardWithOutBomb(wMeChairID, cbTurnCardData, cbTurnCardCount))
						{
							if (GetCardLogicValue(cbHandCardData[0]) > GetCardLogicValue(cbTurnCardData[0]))
							{
								OutCardResult.cbCardCount = 1;
								OutCardResult.cbCardData[0] = cbHandCardData[0];
								return;
							}
						}
					}					
				}
				else
				{
					if (!IsLargestCardWithOutBomb(wMeChairID, cbTurnCardData, cbTurnCardCount))
					{
						if (GetCardLogicValue(cbHandCardData[0]) > GetCardLogicValue(cbTurnCardData[0]))
						{
							OutCardResult.cbCardCount = 1;
							OutCardResult.cbCardData[0] = cbHandCardData[0];
							return;
						}
					}
				}
			}
		}
		else
		{
			if (m_wBankerUser == wOutCardUser)
			{
				//�жϳ���ը����ʣ��ֻ��һ������
				if (cbBombCardCount > 0)
				{
					BYTE cbRemainCardCount=0, cbRemainCardData[MAX_COUNT]={0};
					CopyMemory(cbRemainCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
					cbRemainCardCount = cbHandCardCount;
					RemoveCard(cbBombCardData, cbBombCardCount, cbRemainCardData, cbRemainCardCount);
					cbRemainCardCount -= cbBombCardCount;
					tagAnalyseResult RemainAnalyseResult;
					AnalysebCardData(cbRemainCardData, cbRemainCardCount, RemainAnalyseResult);
					//��ը
					if (RemainAnalyseResult.cbThreeCount+1 >= RemainAnalyseResult.cbSingleCount)
					{
						//˫��ը��
						if (GetCardColor(cbBombCardData[0]) == 0x40)
						{
							OutCardResult.cbCardCount = 2;
							OutCardResult.cbCardData[0] = 0x4F;
							OutCardResult.cbCardData[1] = 0x4E;
							return;
						}
						else
						{
							if (CompareCard(cbTurnCardData,cbBombCardData,cbTurnCardCount,4))
							{
								OutCardResult.cbCardCount = 4;
								CopyMemory(OutCardResult.cbCardData, cbBombCardData, sizeof(BYTE)*4);
								return;
							}
						}	
					}
					else
					{
						if (MeOutCardTypeResult[cbOutCardType].cbCardTypeCount > 0)
						{
							OutCardResult.cbCardCount = MeOutCardTypeResult[cbOutCardType].cbEachHandCardCount[0];
							CopyMemory(OutCardResult.cbCardData, &MeOutCardTypeResult[cbOutCardType].cbEachHandCardData[0], sizeof(BYTE)*OutCardResult.cbCardCount);
							return;
						}
					}
				}
				else
				{
					if (MeOutCardTypeResult[cbOutCardType].cbCardTypeCount > 0)
					{
						OutCardResult.cbCardCount = MeOutCardTypeResult[cbOutCardType].cbEachHandCardCount[0];
						CopyMemory(OutCardResult.cbCardData, &MeOutCardTypeResult[cbOutCardType].cbEachHandCardData[0], sizeof(BYTE)*OutCardResult.cbCardCount);
						return;
					}
				}
			}
			else if (wFriendUser == wOutCardUser)
			{
				//�жϳ���ը����ʣ��ֻ��һ������
				if (cbBombCardCount > 0)
				{
					BYTE cbRemainCardCount=0, cbRemainCardData[MAX_COUNT]={0};
					CopyMemory(cbRemainCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
					cbRemainCardCount = cbHandCardCount;
					RemoveCard(cbBombCardData, cbBombCardCount, cbRemainCardData, cbRemainCardCount);
					cbRemainCardCount -= cbBombCardCount;
					tagAnalyseResult RemainAnalyseResult;
					AnalysebCardData(cbRemainCardData, cbRemainCardCount, RemainAnalyseResult);
					//��ը
					if (RemainAnalyseResult.cbThreeCount+1 >= RemainAnalyseResult.cbSingleCount)
					{
						//˫��ը��
						if (GetCardColor(cbBombCardData[0]) == 0x40)
						{
							OutCardResult.cbCardCount = 2;
							OutCardResult.cbCardData[0] = 0x4F;
							OutCardResult.cbCardData[1] = 0x4E;
							return;
						}
						else
						{
							if (CompareCard(cbTurnCardData,cbBombCardData,cbTurnCardCount,4))
							{
								OutCardResult.cbCardCount = 4;
								CopyMemory(OutCardResult.cbCardData, cbBombCardData, sizeof(BYTE)*4);
								return;
							}
						}	
					}
					else
					{
						//���ж϶����ܷ�ȫ�����꣬���ж��Լ��ܷ�ȫ������
						tagAnalyseResult FriendAnalyseResult;
						AnalysebCardData(m_cbHandCardData[wFriendUser], m_cbHandCardCount[wFriendUser], FriendAnalyseResult);
						if (FriendAnalyseResult.cbSingleCount==0 || FriendAnalyseResult.cbThreeCount+1>=FriendAnalyseResult.cbSingleCount
							|| (FriendAnalyseResult.cbThreeCount+2==FriendAnalyseResult.cbSingleCount && GetCardLogicValue(FriendAnalyseResult.cbSingleCardData[0])>=GetCardLogicValue(m_cbHandCardData[m_wBankerUser][0])))
						{
							return;
						}
					}
				}
				else
				{
					//���ж϶����ܷ�ȫ�����꣬���ж��Լ��ܷ�ȫ������
					tagAnalyseResult FriendAnalyseResult;
					AnalysebCardData(m_cbHandCardData[wFriendUser], m_cbHandCardCount[wFriendUser], FriendAnalyseResult);
					if (FriendAnalyseResult.cbSingleCount==0 || FriendAnalyseResult.cbThreeCount+1>=FriendAnalyseResult.cbSingleCount
						|| (FriendAnalyseResult.cbThreeCount+2==FriendAnalyseResult.cbSingleCount && GetCardLogicValue(FriendAnalyseResult.cbSingleCardData[0])>=GetCardLogicValue(m_cbHandCardData[m_wBankerUser][0])))
					{
						return;
					}
					//���±�û��
					tagOutCardTypeResult FriendCardTypeResult[12+1];
					AnalyseOutCardType(m_cbHandCardData[wFriendUser], m_cbHandCardCount[wFriendUser], FriendCardTypeResult);
					for (BYTE cbCurIndex=0; cbCurIndex<FriendCardTypeResult[CT_SINGLE_LINE].cbCardTypeCount; ++cbCurIndex)
					{
						//ʣ���˿�
						BYTE cbRemainCardData[MAX_COUNT]={0}, cbRemainCardCount=m_cbHandCardCount[wFriendUser];
						CopyMemory(cbRemainCardData, m_cbHandCardData[wFriendUser], sizeof(BYTE)*cbRemainCardCount);
						RemoveCard(FriendCardTypeResult[CT_SINGLE_LINE].cbEachHandCardData[cbCurIndex], FriendCardTypeResult[CT_SINGLE_LINE].cbEachHandCardCount[cbCurIndex], cbRemainCardData, cbRemainCardCount);
						cbRemainCardCount -= FriendCardTypeResult[CT_SINGLE_LINE].cbEachHandCardCount[cbCurIndex];
						//�����˿�
						AnalysebCardData(cbRemainCardData, cbRemainCardCount, FriendAnalyseResult);
						//�����ж�
						if (FriendAnalyseResult.cbSingleCount==0 || FriendAnalyseResult.cbThreeCount+1>=FriendAnalyseResult.cbSingleCount
							|| (FriendAnalyseResult.cbThreeCount+2==FriendAnalyseResult.cbSingleCount && GetCardLogicValue(FriendAnalyseResult.cbSingleCardData[0])>=GetCardLogicValue(m_cbHandCardData[m_wBankerUser][0])))
						{
							return;
						}
					}
					for (BYTE i=0; i<MeOutCardTypeResult[cbOutCardType].cbCardTypeCount; i++)
					{
						BYTE cbRemainCardData[MAX_COUNT]={0}, cbRemainCardCount=cbHandCardCount;
						CopyMemory(cbRemainCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
						RemoveCard(MeOutCardTypeResult[cbOutCardType].cbEachHandCardData[i], MeOutCardTypeResult[cbOutCardType].cbEachHandCardCount[i], cbRemainCardData, cbRemainCardCount);
						cbRemainCardCount -= MeOutCardTypeResult[cbOutCardType].cbEachHandCardCount[i];
						tagAnalyseResult RemainCardResult;
						AnalysebCardData(cbRemainCardData, cbRemainCardCount, RemainCardResult);
						if (RemainCardResult.cbSingleCount==0 || RemainCardResult.cbThreeCount+1>=FriendAnalyseResult.cbSingleCount
							|| (RemainCardResult.cbThreeCount+2==RemainCardResult.cbSingleCount && GetCardLogicValue(RemainCardResult.cbSingleCardData[0])>=GetCardLogicValue(m_cbHandCardData[m_wBankerUser][0])))
						{
							OutCardResult.cbCardCount = MeOutCardTypeResult[cbOutCardType].cbEachHandCardCount[i];
							CopyMemory(OutCardResult.cbCardData, &MeOutCardTypeResult[cbOutCardType].cbEachHandCardData[i], sizeof(BYTE)*OutCardResult.cbCardCount);
							return;
						}
					}
				}
			}	
		}
	}
	//������ʣһ��
	if (m_cbHandCardCount[m_wBankerUser]==2 && GetCardValue(m_cbHandCardData[m_wBankerUser][0])==GetCardValue(m_cbHandCardData[m_wBankerUser][1]))
	{
		//�ж϶���ֻʣը����һ��,��ǰ����ѳ����Ĵ������
		if (cbOutCardType==CT_DOUBLE && wOutCardUser==wFriendUser)
		{
			BYTE cbFriendBombCardData[MAX_COUNT]={0}, cbFriendBombCardCount=0;
			GetAllBombCard(m_cbHandCardData[wFriendUser], m_cbHandCardCount[wFriendUser], cbFriendBombCardData, cbFriendBombCardCount);
			BYTE cbFriendRemainCardData[MAX_COUNT]={0}, cbFriendRemainCardCount=m_cbHandCardCount[wFriendUser];
			CopyMemory(cbFriendRemainCardData, m_cbHandCardData[wFriendUser], sizeof(BYTE)*m_cbHandCardCount[wFriendUser]);
			RemoveCard(cbFriendBombCardData, cbFriendBombCardCount, cbFriendRemainCardData, cbFriendRemainCardCount);
			cbFriendRemainCardCount -= cbFriendBombCardCount;
			if (cbFriendBombCardCount>0 && cbFriendRemainCardCount==2 && GetCardValue(cbFriendRemainCardData[0])==GetCardValue(cbFriendRemainCardData[1]))
			{
				BYTE cbAllDoubleCardData[MAX_COUNT]={0}, cbAllDoubleCardCount=0;
				GetAllDoubleCard(cbHandCardData, cbHandCardCount, cbAllDoubleCardData, cbAllDoubleCardCount);
				if (cbAllDoubleCardCount>0 && GetCardLogicValue(cbAllDoubleCardData[0])>GetCardLogicValue(cbTurnCardData[0]))
				{
					OutCardResult.cbCardCount = 2;
					CopyMemory(OutCardResult.cbCardData, cbAllDoubleCardData, sizeof(BYTE)*2);
					return;
				}
			}
		}
	}

	//���߶Լ�
	if (CompareCard(cbTurnCardData,m_cbHandCardData[wFriendUser],cbTurnCardCount,m_cbHandCardCount[wFriendUser]) &&
		!CompareCard(cbTurnCardData,m_cbHandCardData[m_wBankerUser],cbTurnCardCount,m_cbHandCardCount[m_wBankerUser]))
	{
		return;
	}

	//���ׯ��û�д���������ѹ�Լ���
	if (wOutCardUser==wFriendUser && m_cbHandCardCount[m_wBankerUser]<=5 && 
		(BankerOutCardTypeResult[cbOutCardType].cbCardTypeCount==0||GetCardLogicValue(BankerOutCardTypeResult[cbOutCardType].cbEachHandCardData[0][0])<=GetCardLogicValue(cbTurnCardData[0])) &&
		MeOutCardTypeResult[cbOutCardType].cbEachHandCardCount[0]!=cbHandCardCount)//����һ�γ���
	{
		//��������
		return;
	}

 	//���ƶ���
 	if (cbOutCardType==CT_SINGLE && MeOutCardTypeResult[cbOutCardType].cbCardTypeCount>0)
 	{
 		BYTE cbMeSingleCardCount=0, cbMeSingleCardData[MAX_COUNT]={0};
 		BYTE cbBankerSingleCardCount=0, cbBankerSingleCardData[MAX_COUNT]={0};
 		//��ȡ����
 		cbMeSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, NULL, 0, cbMeSingleCardData);
 		cbBankerSingleCardCount = AnalyseSinleCardCount(m_cbHandCardData[m_wBankerUser], m_cbHandCardCount[m_wBankerUser], NULL, 0, cbBankerSingleCardData);
 		//��������С��
 		if (cbBankerSingleCardCount>0 && cbMeSingleCardCount>0 && GetCardLogicValue(cbBankerSingleCardData[cbBankerSingleCardCount-1])<=10)
 		{	
 			SortCardList(cbMeSingleCardData, cbMeSingleCardCount, ST_ORDER);
 			//��������
 			if (cbBankerSingleCardCount>=2 && GetCardValue(cbBankerSingleCardData[cbBankerSingleCardCount-2])<=10)
 			{
 				for (int nMeIndex=cbMeSingleCardCount-1; nMeIndex>=0; --nMeIndex)
 				{
 					if (GetCardLogicValue(cbMeSingleCardData[nMeIndex])>GetCardLogicValue(cbTurnCardData[0]) && 
 						GetCardLogicValue(cbMeSingleCardData[nMeIndex])>=GetCardLogicValue(cbBankerSingleCardData[cbBankerSingleCardCount-2]) &&
 						GetCardLogicValue(cbMeSingleCardData[nMeIndex])<=15)
 					{
 						OutCardResult.cbCardCount = 1;
 						OutCardResult.cbCardData[0] = cbMeSingleCardData[nMeIndex];
 						return;
 					}
 				}
 			}	
 
 			//����һ��
 			for (int nMeIndex=cbMeSingleCardCount-1; nMeIndex>=0; --nMeIndex)
 			{
 				if (GetCardLogicValue(cbMeSingleCardData[nMeIndex])>GetCardLogicValue(cbTurnCardData[0]) && 
 					GetCardLogicValue(cbMeSingleCardData[nMeIndex])>=GetCardLogicValue(cbBankerSingleCardData[cbBankerSingleCardCount-1]) &&
 					GetCardLogicValue(cbMeSingleCardData[nMeIndex])<=15)
 				{
 					OutCardResult.cbCardCount = 1;
 					OutCardResult.cbCardData[0] = cbMeSingleCardData[nMeIndex];
 					return;
 				}
 			}
 		}
 	}

	//�¼�Ϊ���������ҵ����˿�����5��
	if (m_cbHandCardCount[m_wBankerUser]<=5 && MeOutCardTypeResult[cbOutCardType].cbCardTypeCount>0 && cbOutCardType!=CT_BOMB_CARD && 
		((wOutCardUser==wFriendUser&&GetCardLogicValue(cbTurnCardData[0])<12&&BankerOutCardTypeResult[cbOutCardType].cbCardTypeCount>0)||(wOutCardUser==m_wBankerUser)))
	{
		//��ֹ�����ȴ����Ƴ�ȥ
		BYTE cbCurIndex = cbOutCardType==CT_SINGLE ? 0 : MeOutCardTypeResult[cbOutCardType].cbCardTypeCount-1;
		//Ѱ�ҿ���ѹס��������Сһ����
		BYTE cbThisOutTypeMinSingleCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, MeOutCardTypeResult[cbOutCardType].cbEachHandCardData[0], MeOutCardTypeResult[cbOutCardType].cbEachHandCardCount[0]);
		BYTE cbBestIndex = 255;
		for (BYTE i=0; i<MeOutCardTypeResult[cbOutCardType].cbCardTypeCount; ++i)
		{
			BYTE cbTempSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, MeOutCardTypeResult[cbOutCardType].cbEachHandCardData[i], MeOutCardTypeResult[cbOutCardType].cbEachHandCardCount[i]);
			if ((BankerOutCardTypeResult[cbOutCardType].cbCardTypeCount==0 || BankerOutCardTypeResult[cbOutCardType].cbCardTypeCount>0&&GetCardLogicValue(MeOutCardTypeResult[cbOutCardType].cbEachHandCardData[i][0])>=GetCardLogicValue(BankerOutCardTypeResult[cbOutCardType].cbEachHandCardData[0][0]))
				&& cbTempSingleCardCount<=cbThisOutTypeMinSingleCount)
			{
				cbBestIndex = i;
				cbThisOutTypeMinSingleCount = cbTempSingleCardCount;
			}

			if ((BankerOutCardTypeResult[cbOutCardType].cbCardTypeCount==0) || (BankerOutCardTypeResult[cbOutCardType].cbCardTypeCount>0&&GetCardLogicValue(MeOutCardTypeResult[cbOutCardType].cbEachHandCardData[i][0])>=GetCardLogicValue(BankerOutCardTypeResult[cbOutCardType].cbEachHandCardData[0][0])))
			{
				cbCurIndex = i;
			}
			else 
			{
				break;
			}
		}

		if (cbBestIndex != 255)
		{
			OutCardResult.cbCardCount = MeOutCardTypeResult[cbOutCardType].cbEachHandCardCount[cbBestIndex];
			CopyMemory(OutCardResult.cbCardData, MeOutCardTypeResult[cbOutCardType].cbEachHandCardData[cbBestIndex], OutCardResult.cbCardCount);
		}
		else
		{
			OutCardResult.cbCardCount = MeOutCardTypeResult[cbOutCardType].cbEachHandCardCount[cbCurIndex];
			CopyMemory(OutCardResult.cbCardData, MeOutCardTypeResult[cbOutCardType].cbEachHandCardData[cbCurIndex], OutCardResult.cbCardCount);
		}
		return;
	}

	//ȡ���ĸ���С����
	BYTE cbSingleCardCount[4] = {MAX_COUNT,MAX_COUNT,MAX_COUNT,MAX_COUNT};
	BYTE cbOutCardIndex[4] = {0};	
	BYTE cbMinSingleCount = MAX_COUNT;
	//�ɳ��˿ˣ������Ѿ����˵�ը���ˣ�
	const tagOutCardTypeResult &CanOutCardType = MeOutCardTypeResult[cbOutCardType];
	for (BYTE i=0; i<CanOutCardType.cbCardTypeCount; ++i)
	{
		//��С����
		BYTE cbTempCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount,CanOutCardType.cbEachHandCardData[i], CanOutCardType.cbEachHandCardCount[i]); 
		//����cbMinSingleCardCount[4]�����ֵ
		for (BYTE j=0; j<4; ++j)
		{
			if (cbSingleCardCount[j] >= cbTempCount)
			{
				cbSingleCardCount[j] = cbTempCount;
				cbOutCardIndex[j] = i;
				break;
			}
		}
	}
	for (BYTE i=0; i<4; ++i)
	{
		if (cbMinSingleCount > cbSingleCardCount[i])
		{
			cbMinSingleCount = cbSingleCardCount[i];
		}
	}
	//ԭʼ������
	BYTE cbOriginSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, NULL, 0);
	//������������
	BYTE cbBankerDoubleCardCount=0, cbBankerDoubleCardData[MAX_COUNT]={0};
	GetAllDoubleCard(m_cbHandCardData[m_wBankerUser], m_cbHandCardCount[m_wBankerUser], cbBankerDoubleCardData, cbBankerDoubleCardCount);
	//���ѳ���
	if (m_wBankerUser != wOutCardUser)
	{
		//�������������һ����
		if (GetCardType(m_cbHandCardData[wFriendUser],m_cbHandCardCount[wFriendUser]) != CT_ERROR) 
		{
			return;
		}

		//�������TestOutAllCard���ѶԿɳ�ը�����������
		if (CanOutCardType.cbCardTypeCount>0 && CanOutCardType.cbCardType<CT_BOMB_CARD)
		{
			//������������
			BYTE cbBankerSingleCardData[MAX_COUNT] = {0};
			BYTE cbBankerSingleCardCount = AnalyseSinleCardCount(m_cbHandCardData[m_wBankerUser], m_cbHandCardCount[m_wBankerUser], NULL, 0, cbBankerSingleCardData);
			BYTE cbBankerSingleCardLogicValue = 0;
			if (cbBankerSingleCardCount>=2 && GetCardLogicValue(cbBankerSingleCardData[cbBankerSingleCardCount-2])<=10) 
			{
				cbBankerSingleCardLogicValue = GetCardLogicValue(cbBankerSingleCardData[cbBankerSingleCardCount-2]);
			}
			else if (cbBankerSingleCardCount>=2 && GetCardLogicValue(cbBankerSingleCardData[cbBankerSingleCardCount-1])<=10)
			{
				cbBankerSingleCardLogicValue = GetCardLogicValue(cbBankerSingleCardData[cbBankerSingleCardCount-1]);
			}
			else if (cbBankerSingleCardCount>0 && GetCardLogicValue(cbBankerSingleCardData[0])<=10) 
			{
				cbBankerSingleCardLogicValue = GetCardLogicValue(cbBankerSingleCardData[0]);
			}

			BYTE cbMinLogicCardValue = GetCardLogicValue(0x4F)+1;
			bool bFindCard = false;
			BYTE cbCanOutIndex = 0;
			for (BYTE i=0; i<4; ++i)
			{
				BYTE cbCurIndex = cbOutCardIndex[i];
				//�����������Ʋ��ӶԼ���
				if (CanOutCardType.cbCardType>=CT_THREE && CanOutCardType.cbCardType<=CT_MISSILE_CARD && GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbCurIndex][0])>=7 && CanOutCardType.cbEachHandCardCount[cbCurIndex]<=5)
				{
					continue;
				}
				//��������
				bool bCanOut = false;
				if (cbOutCardType==CT_SINGLE && cbBankerSingleCardCount>0 && GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbCurIndex][0])>=cbBankerSingleCardLogicValue &&
					GetCardLogicValue(cbTurnCardData[0])<14 && cbSingleCardCount[i]<cbOriginSingleCardCount && GetCardLogicValue(m_cbHandCardData[m_wBankerUser][0])>GetCardLogicValue(cbTurnCardData[0]))
				{
					bCanOut = true;
				}
				//���ض���
				if (cbOutCardType==CT_DOUBLE && cbBankerDoubleCardCount>0 && GetCardLogicValue(cbBankerDoubleCardData[cbBankerDoubleCardCount-1])<10 &&
					GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbCurIndex][0])<GetCardLogicValue(cbBankerDoubleCardData[cbBankerDoubleCardCount-1]) &&
					GetCardLogicValue(CanOutCardType.cbEachHandCardData[0][0])>=10 && GetCardLogicValue(CanOutCardType.cbEachHandCardData[0][0])<14)
				{
					continue;
				}

				//С��J���ƣ�����С��K������ɢ��
				if (bCanOut ||
					(cbMinLogicCardValue>GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbCurIndex][0]) && cbHandCardCount>5 && 
					 (cbSingleCardCount[i]<cbOriginSingleCardCount+3 && (cbSingleCardCount[i]<=cbMinSingleCount || cbSingleCardCount[i]<=cbMinSingleCount+1&&CanOutCardType.cbCardType>=CT_THREE_TAKE_ONE&&CanOutCardType.cbCardType<=CT_THREE_TAKE_TWO) && 
					  (GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbCurIndex][0])<=11 || (cbSingleCardCount[i]<cbOriginSingleCardCount)&&GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbCurIndex][0])<=13)))
				   )
				{
					//������û�д��ƣ���Էɻ���������Ĵ��ƣ�
					bool bNoLargeCard = true;
					for (BYTE k=3; k<CanOutCardType.cbEachHandCardCount[cbCurIndex]; ++k)
					{
						//�д��ƶ��Ҳ���һ�γ���
						if (GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbCurIndex][k])>=15 && CanOutCardType.cbEachHandCardCount[cbCurIndex]!=cbHandCardCount)
						{
							bNoLargeCard = false;
						}
					}
					if (bNoLargeCard)
					{
						bFindCard = true;
						cbCanOutIndex = cbCurIndex; 
						cbMinLogicCardValue = GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbCurIndex][0]);
					}
				}
				else if (cbHandCardCount<5 && cbSingleCardCount[i]<cbOriginSingleCardCount+4 && cbSingleCardCount[i]<=cbMinSingleCount && cbMinLogicCardValue>GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbCurIndex][0]))
				{
					//�ܳ������Լҵ�2
					if (GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbCurIndex][0])>=16 && GetCardLogicValue(cbTurnCardData[0])>=15)
					{
						continue;
					}
					bFindCard = true;
					cbCanOutIndex = cbCurIndex; 
					cbMinLogicCardValue = GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbCurIndex][0]);
				}
			}

			if (bFindCard)
			{
				//���ñ���
				OutCardResult.cbCardCount = CanOutCardType.cbEachHandCardCount[cbCanOutIndex];
				CopyMemory(OutCardResult.cbCardData, CanOutCardType.cbEachHandCardData[cbCanOutIndex], CanOutCardType.cbEachHandCardCount[cbCanOutIndex]*sizeof(BYTE));
				return;
			}
			//��������������
			else if (cbHandCardCount <= 5)
			{
				BYTE cbMinLogicCard = GetCardLogicValue(0x4F)+1;
				BYTE cbCanOutIndex = 0;
				for (BYTE i=0; i<4; ++i)
				{
					if (cbSingleCardCount[i]<MAX_COUNT && cbSingleCardCount[i]<=cbMinSingleCount && cbMinLogicCard>GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbOutCardIndex[i]][0]) && GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbOutCardIndex[i]][0])<=14)
					{
						cbMinLogicCard = GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbOutCardIndex[i]][0]);
						cbCanOutIndex = cbOutCardIndex[i];
					}
				}
				if (cbMinLogicCard != (GetCardLogicValue(0x4F)+1))
				{
					//���ñ���
					OutCardResult.cbCardCount=CanOutCardType.cbEachHandCardCount[cbCanOutIndex];
					CopyMemory(OutCardResult.cbCardData,CanOutCardType.cbEachHandCardData[cbCanOutIndex],CanOutCardType.cbEachHandCardCount[cbCanOutIndex]*sizeof(BYTE));
					return;
				}
			}
			return;
		}
		else
		{
			return;
		}

	}
	//��������
	else
	{
		if (CanOutCardType.cbCardTypeCount > 0)
		{
			BYTE cbMinLogicCardValue = GetCardLogicValue(0x4F)+1;
			bool bFindCard = false;
			BYTE cbCanOutIndex = 0;
			for (BYTE i=0; i<4; ++i)
			{
				BYTE cbCurIndex = cbOutCardIndex[i];
				if ((cbSingleCardCount[i]<cbOriginSingleCardCount+4) && cbMinLogicCardValue>GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbCurIndex][0]) && 
					(cbSingleCardCount[i]<=cbMinSingleCount || cbSingleCardCount[i]<=cbMinSingleCount+1&&CanOutCardType.cbCardType>=CT_THREE_TAKE_ONE&&CanOutCardType.cbCardType<=CT_THREE_TAKE_TWO))
				{
					//��Դ���
					bool bNoLargeCard = true;
					//������������������4�����ҵ���������С��K���ƶ��Ҳ��ǵ������ϵ������ʱ�����ܳ�2ȥ��
					if (m_cbHandCardCount[m_wBankerUser]>=4 && cbHandCardCount>=5 && CanOutCardType.cbEachHandCardCount[cbCurIndex]>=2 && 
						GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbCurIndex][0])>=15 && GetCardLogicValue(cbTurnCardData[0])<13 &&
						GetCardLogicValue(cbTurnCardData[0])<GetCardLogicValue(m_cbHandCardData[m_wBankerUser][0]) && 
						CanOutCardType.cbEachHandCardCount[cbCurIndex]!=cbHandCardCount)
					{
						bNoLargeCard = false;
					}

					//������û�д��ƣ���Էɻ���������Ĵ��ƣ�
					for (BYTE k=3; k<CanOutCardType.cbEachHandCardCount[cbCurIndex]; ++k)
					{
						if (GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbCurIndex][k])>=15 && CanOutCardType.cbEachHandCardCount[cbCurIndex]!=cbHandCardCount)
						{
							bNoLargeCard = false;
						}
					}
					if (bNoLargeCard)
					{
						bFindCard = true;
						cbCanOutIndex = cbCurIndex; 
						cbMinLogicCardValue = GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbCurIndex][0]);
					}
				}
			}

			if (bFindCard)
			{
				//�����������
				BYTE cbLargestLogicCard = GetCardLogicValue(m_cbHandCardData[m_wBankerUser][0]);
				bool bCanOut = true;
				//��ֻѹ2
				if (GetCardLogicValue(cbTurnCardData[0]) < cbLargestLogicCard)
				{
					if (GetCardColor(CanOutCardType.cbEachHandCardData[cbCanOutIndex][0])==0x40 && GetCardLogicValue(cbTurnCardData[0])<=14 && cbHandCardCount>5) 								
					{
						bCanOut = false;
					}
				}
				 //˫���ж�
				if (GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbCanOutIndex][0])>=16 && cbHandCardCount>=2 && cbHandCardData[0]==0x4F && cbHandCardData[1]==0x4E)
				{
					bool bOutMissileCard = false;
					//һ���ƺ�һ��ը��
					BYTE cbRemainCardData[20]={0}, cbRemainCardCount=cbHandCardCount;
					CopyMemory(cbRemainCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
					RemoveCard(cbRemainCardData, 2, cbRemainCardData, cbRemainCardCount);
					cbRemainCardCount -= 2;
					if (CT_ERROR != GetCardType(cbRemainCardData, cbRemainCardCount)) 
					{
						bOutMissileCard = true;
					}
					//ֻʣ������
					if (cbRemainCardCount<5 && cbRemainCardCount>0 && GetCardLogicValue(cbRemainCardData[0])>=14) 
					{
						bOutMissileCard = true;
					}

					//ը������
					BYTE cbSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, CanOutCardType.cbEachHandCardData[cbCanOutIndex], CanOutCardType.cbEachHandCardCount[cbCanOutIndex]);
					if (cbSingleCardCount<=1 && GetCardLogicValue(cbRemainCardData[0])>=11) 
					{
						bOutMissileCard = true;
					}
					//����С��
					if (GetCardLogicValue(cbRemainCardData[0])<=10 && CT_ERROR==GetCardType(cbRemainCardData,cbRemainCardCount) && GetCardLogicValue(m_cbHandCardData[m_wBankerUser][0])>10)
					{
						bOutMissileCard = false;
					}
					//����˿�
					if (bOutMissileCard)
					{
						//��������ը��
						BYTE cbIndex = MeOutCardTypeResult[CT_BOMB_CARD].cbCardTypeCount-1;
						OutCardResult.cbCardCount = MeOutCardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[cbIndex];
						CopyMemory(OutCardResult.cbCardData, MeOutCardTypeResult[CT_BOMB_CARD].cbEachHandCardData[cbIndex], OutCardResult.cbCardCount);
						return;
					}
				}
				if (bCanOut)
				{
					//���ñ���
					OutCardResult.cbCardCount = CanOutCardType.cbEachHandCardCount[cbCanOutIndex];
					CopyMemory(OutCardResult.cbCardData, CanOutCardType.cbEachHandCardData[cbCanOutIndex], CanOutCardType.cbEachHandCardCount[cbCanOutIndex]*sizeof(BYTE));
					return;
				}
			}
			if (cbOutCardType == CT_SINGLE)
			{
				//�����������
				BYTE cbLargestLogicCard = GetCardLogicValue(m_cbHandCardData[m_wBankerUser][0]);
				if (GetCardLogicValue(cbTurnCardData[0])==14 || GetCardLogicValue(cbTurnCardData[0])>=cbLargestLogicCard || 
					(GetCardLogicValue(cbTurnCardData[0])<cbLargestLogicCard-1) || m_cbHandCardCount[m_wBankerUser]<=5)
				{
					//ȡһ�Ŵ��ڵ���2����Ҫ�ȵ��������ƴ���ƣ�
					BYTE cbIndex = MAX_COUNT;
					for (BYTE i=0; i<cbHandCardCount; ++i)
					{
						if (GetCardLogicValue(cbHandCardData[i])>GetCardLogicValue(cbTurnCardData[0]) && GetCardLogicValue(cbHandCardData[i])>=15)
						{
							cbIndex = i;
						}
					}
					if (cbIndex != MAX_COUNT)
					{
						//���ñ���
						OutCardResult.cbCardCount = 1;
						OutCardResult.cbCardData[0] = cbHandCardData[cbIndex];
						return;
					}
				}
			}
		}

		//��Ҫ����ը��
		if (MeOutCardTypeResult[CT_BOMB_CARD].cbCardTypeCount>0 && NORMAL_COUNT==cbHandCardCount && NORMAL_COUNT==m_cbHandCardCount[wFriendUser])
		{
			const tagOutCardTypeResult &BomCardType = MeOutCardTypeResult[CT_BOMB_CARD];
			BYTE cbMinLogicValue = GetCardLogicValue(BomCardType.cbEachHandCardData[0][0]);
			BYTE cbCurIndex = 0;
			for (BYTE i=0; i<BomCardType.cbCardTypeCount; ++i)
			{
				if (cbMinLogicValue > GetCardLogicValue(BomCardType.cbEachHandCardData[i][0]))
				{
					cbMinLogicValue = GetCardLogicValue(BomCardType.cbEachHandCardData[i][0]);
					cbCurIndex = i;
				}
			}
			bool bOutBomb = false;
			//�����ж�
			if (NORMAL_COUNT==cbHandCardCount && NORMAL_COUNT==m_cbHandCardCount[wFriendUser] && CT_ERROR!=GetCardType(m_cbHandCardData[m_wBankerUser],m_cbHandCardCount[m_wBankerUser]))
			{
				bOutBomb = true;
			}
			//һ���ƺ�һ��ը��
			BYTE cbRemainCardData[MAX_COUNT]={0}, cbRemainCardCount=cbHandCardCount;
			CopyMemory(cbRemainCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
			RemoveCard(BomCardType.cbEachHandCardData[cbCurIndex], BomCardType.cbEachHandCardCount[cbCurIndex], cbRemainCardData, cbRemainCardCount);
			cbRemainCardCount -= BomCardType.cbEachHandCardCount[cbCurIndex];
			if (CT_ERROR != GetCardType(cbRemainCardData, cbRemainCardCount)) 
			{
				bOutBomb = true;
			}
			//ը������
			BYTE cbSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, BomCardType.cbEachHandCardData[cbCurIndex],BomCardType.cbEachHandCardCount[cbCurIndex]);
			if (cbSingleCardCount==0 && GetCardLogicValue(cbRemainCardData[0])>GetCardLogicValue(m_cbHandCardData[m_wBankerUser][0])) 
			{
				bOutBomb = true;
			}
			//ֻʣһ��
			BYTE cbRemainCardType = GetCardType(m_cbHandCardData[wOutCardUser], m_cbHandCardCount[wOutCardUser]);
			if (cbRemainCardType>CT_ERROR && cbRemainCardType<CT_FOUR_TAKE_ONE && GetCardLogicValue(m_cbHandCardData[wOutCardUser][0])<15 && 
				cbSingleCardCount<2 && (GetCardLogicValue(cbRemainCardData[0])>=GetCardLogicValue(m_cbHandCardData[m_wBankerUser][0])))
			{
				bOutBomb = true;
			}
			//ֻʣ������
			BYTE cbRemainLargestCard = GetCardLogicValue(cbRemainCardData[0]);
			if (cbRemainCardCount<5 && cbRemainCardCount>0 && (cbRemainLargestCard!=GetCardLogicValue(BomCardType.cbEachHandCardData[cbCurIndex][0])) && 
				cbRemainLargestCard>GetCardLogicValue(m_cbHandCardData[wOutCardUser][0]) && cbRemainLargestCard>14)
			{
				bOutBomb = true;
			}
			//�����˿�
			tagAnalyseResult AnalyseResult;
			AnalysebCardData(cbRemainCardData, cbRemainCardCount, AnalyseResult);
			if (m_cbHandCardCount[m_wBankerUser]==1 && (AnalyseResult.cbDoubleCount*2+AnalyseResult.cbThreeCount*3+AnalyseResult.cbFourCount*4+1 >= cbRemainCardCount))
			{
				bOutBomb = true;
			}
			//���ñ���
			if (bOutBomb)
			{
				OutCardResult.cbCardCount = BomCardType.cbEachHandCardCount[cbCurIndex];
				CopyMemory(OutCardResult.cbCardData, BomCardType.cbEachHandCardData[cbCurIndex], BomCardType.cbEachHandCardCount[cbCurIndex]*sizeof(BYTE));
			}
			return;
		}
		return;
	}
}

//�����¼ң��ȳ��ƣ�
void CAndroidLogic::UndersideOfBankerOutCard( const BYTE cbHandCardData[], const BYTE cbHandCardCount, const WORD wMeChairID, tagOutCardResult &OutCardResult ) 
{
	//��ʼ����
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));
	if (cbHandCardCount == 0) return;

	//�������
	const WORD wFriendUser = (wMeChairID+1)%GAME_PLAYER; //���ѣ������ϼ�

	//������������
	tagAnalyseResult MeAnalyseResult, BankerAnalyseResult;
	VERIFY(AnalysebCardData(cbHandCardData, cbHandCardCount, MeAnalyseResult));
	VERIFY(AnalysebCardData(m_cbHandCardData[m_wBankerUser], m_cbHandCardCount[m_wBankerUser], BankerAnalyseResult));
	//�Լ���������
	BYTE cbHandCardType = IsValidCount(cbHandCardCount) ? GetCardType(cbHandCardData,cbHandCardCount) : CT_ERROR;
	//�ж�һ�����ܷ����
	if (cbHandCardType != CT_ERROR)
	{
		//�����Ĵ�����ֱ�ӳ���
		if (cbHandCardType!=CT_FOUR_TAKE_ONE && cbHandCardType!=CT_FOUR_TAKE_TWO)
		{
			OutCardResult.cbCardCount = cbHandCardCount;
			CopyMemory(&OutCardResult.cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
			return;
		}
		else
		{
			//�����Ĵ�������
			Analyse4D2CardType(wMeChairID, cbHandCardData, cbHandCardCount, cbHandCardType, OutCardResult);
			if (OutCardResult.cbCardCount>0 && GetCardType(OutCardResult.cbCardData, OutCardResult.cbCardCount)!=CT_ERROR)
			{
				return;
			}
		}
	}
	//�ж��Ƿ��ǻ����һ��
	if (cbHandCardCount>2 && cbHandCardData[0]==0x4F && cbHandCardData[1]==0x4E)
	{
		cbHandCardType = GetCardType(&cbHandCardData[2], cbHandCardCount-2);
		if (cbHandCardType != CT_ERROR)
		{
			//�����ʣ�µĲ����Ĵ�����ֱ�ӳ�ը��
			if (cbHandCardType!=CT_FOUR_TAKE_ONE && cbHandCardType!=CT_FOUR_TAKE_TWO)
			{
				OutCardResult.cbCardCount = 2;
				OutCardResult.cbCardData[0] = 0x4F;
				OutCardResult.cbCardData[1] = 0x4E;
				return;
			}
			else
			{
				//�����Ĵ�������
				Analyse4D2CardType(wMeChairID, cbHandCardData, cbHandCardCount, cbHandCardType, OutCardResult);
				if (OutCardResult.cbCardCount>0 && GetCardType(OutCardResult.cbCardData, OutCardResult.cbCardCount)!=CT_ERROR)
				{
					return;
				}
			}
		}
	}

	BYTE cbFirstCardIndex = 0;
	if (cbHandCardCount>=2 && cbHandCardData[0]==0x4F && cbHandCardData[1]==0x4E)
	{
		cbFirstCardIndex = 2;
	}
	//�ж϶���ֻʣһ����
	if (m_cbHandCardCount[wFriendUser] == 1)
	{
		//�Լ��л����һ��С�ڶ��ѵ���
		if (cbHandCardCount>2 && cbHandCardData[0]==0x4F && cbHandCardData[1]==0x4E)
		{
			if (GetCardLogicValue(cbHandCardData[cbHandCardCount-1]) < GetCardLogicValue(m_cbHandCardData[wFriendUser][0]))
			{
				OutCardResult.cbCardCount = 2;
				OutCardResult.cbCardData[0] = 0x4F;
				OutCardResult.cbCardData[1] = 0x4E;
				return;
			}
		}
		//�Լ���ը����һ��С�ڶ��ѵ���
		BYTE cbBombCardData[MAX_COUNT]={0}, cbBombCardCount=0;
		GetAllBombCard(cbHandCardData, cbHandCardCount, cbBombCardData, cbBombCardCount);
		if (cbBombCardCount > 0)
		{
			BYTE cbRemainCardData[MAX_COUNT]={0}, cbRemainCardCount=cbHandCardCount;
			CopyMemory(cbRemainCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
			RemoveCard(cbBombCardData, cbBombCardCount, cbRemainCardData, cbRemainCardCount);
			cbRemainCardCount -= cbBombCardCount;
			if (cbRemainCardCount>0 && GetCardLogicValue(cbRemainCardData[cbRemainCardCount-1])<GetCardLogicValue(m_cbHandCardData[wFriendUser][0]))
			{
				if (m_cbHandCardCount[m_wBankerUser] < 4)
				{
					OutCardResult.cbCardCount = 4;
					CopyMemory(OutCardResult.cbCardData, cbBombCardData, sizeof(BYTE)*4);
					ASSERT(GetCardType(OutCardResult.cbCardData, OutCardResult.cbCardCount)==CT_BOMB_CARD);
					return;
				}								
			}
		}
	}
	//�Լ�ֻʣ���ź�����һ����,���ҵ��Ż�һ��û��ѹ
	BYTE cbTempCardData[MAX_COUNT]={0}, cbTempCardCount=cbHandCardCount-cbFirstCardIndex;
	for (BYTE i=0; i<cbHandCardCount-cbFirstCardIndex; i++)
	{
		ZeroMemory(cbTempCardData, sizeof(cbTempCardData));
		cbTempCardCount = cbHandCardCount-cbFirstCardIndex;
		CopyMemory(cbTempCardData, &cbHandCardData[cbFirstCardIndex], sizeof(BYTE)*cbTempCardCount);
		BYTE cbSingleCard = cbTempCardData[i];
		RemoveCard(&cbSingleCard, 1, cbTempCardData, cbTempCardCount);
		cbTempCardCount -= 1;
		BYTE cbLastCardType = GetCardType(cbTempCardData, cbTempCardCount);
		if (cbLastCardType==CT_ERROR || cbLastCardType==CT_FOUR_TAKE_ONE || cbLastCardType==CT_FOUR_TAKE_TWO)
		{
			continue;
		}
		if (IsLargestCard(m_wBankerUser,cbTempCardData,cbTempCardCount))
		{
			OutCardResult.cbCardCount = cbTempCardCount;
			CopyMemory(OutCardResult.cbCardData, cbTempCardData, sizeof(BYTE)*cbTempCardCount);
			return;
		}
		if (IsLargestCard(m_wBankerUser,&cbSingleCard,1))
		{
			OutCardResult.cbCardCount = 1;
			OutCardResult.cbCardData[0] = cbSingleCard;
			return;
		}
	}
	//����ֻʣ���ţ������Լ���һ��С�ڶ��ѵ���
	if (m_cbHandCardCount[wFriendUser]==1 && GetCardLogicValue(cbHandCardData[cbHandCardCount-1])<GetCardLogicValue(m_cbHandCardData[wFriendUser][0]))
	{
		OutCardResult.cbCardCount = 1;
		OutCardResult.cbCardData[0] = cbHandCardData[cbHandCardCount-1];
		return;
	}
	//���߶���
	BYTE cbDoubleCardCount=0, cbDoubleCardData[MAX_COUNT]={0};
	GetAllDoubleCard(cbHandCardData, cbHandCardCount, cbDoubleCardData, cbDoubleCardCount);
 	if (GetCardType(m_cbHandCardData[wFriendUser], m_cbHandCardCount[wFriendUser])==CT_DOUBLE && cbDoubleCardCount>=2)
 	{
 		if (GetCardLogicValue(cbDoubleCardData[cbDoubleCardCount-1]) < GetCardLogicValue(m_cbHandCardData[wFriendUser][0]))
 		{
 			OutCardResult.cbCardCount = 2;
 			OutCardResult.cbCardData[0] = cbDoubleCardData[cbDoubleCardCount-2];
 			OutCardResult.cbCardData[1] = cbDoubleCardData[cbDoubleCardCount-1];
 			return;
 		}
 	}

	//���ż�һ��,��һ��������
	for (BYTE i=0; i<cbHandCardCount-cbFirstCardIndex; i++)
	{
		ZeroMemory(cbTempCardData, sizeof(cbTempCardData));
		cbTempCardCount = cbHandCardCount-cbFirstCardIndex;
		CopyMemory(cbTempCardData, &cbHandCardData[cbFirstCardIndex], sizeof(BYTE)*cbTempCardCount);
		BYTE cbSingleCard = cbTempCardData[i];
		RemoveCard(&cbSingleCard, 1, cbTempCardData, cbTempCardCount);
		cbTempCardCount -= 1;
		BYTE cbLastCardType = GetCardType(cbTempCardData, cbTempCardCount);
		if (cbLastCardType==CT_ERROR || cbLastCardType==CT_FOUR_TAKE_ONE || cbLastCardType==CT_FOUR_TAKE_TWO || cbLastCardType==CT_DOUBLE || (cbLastCardType==CT_BOMB_CARD&&m_cbHandCardCount[m_wBankerUser]>1))
		{
			continue;
		}
		tagAnalyseResult TempAnalyseResult;
		AnalysebCardData(cbTempCardData, cbTempCardCount, TempAnalyseResult);
		if (TempAnalyseResult.cbFourCount > 0) continue;
		OutCardResult.cbCardCount = cbTempCardCount;
		CopyMemory(OutCardResult.cbCardData, cbTempCardData, sizeof(BYTE)*cbTempCardCount);
		return;
	}
	//����������
	if (cbHandCardCount==4 && cbHandCardData[0]==0x4F && cbHandCardData[1]==0x4E && m_cbHandCardCount[m_wBankerUser]==1)
	{
		OutCardResult.cbCardCount = 1;
		OutCardResult.cbCardData[0] = cbHandCardData[2];
		return;
	}

	//���ƽ����ж�
	if (m_cbHandCardCount[m_wBankerUser]==1 && cbHandCardCount>=2 && m_cbHandCardCount[wFriendUser]>=2)
	{
		tagAnalyseResult MeAnalyseResult, FriendAnalyseResult;
		//�����˿�
		AnalysebCardData(cbHandCardData, cbHandCardCount, MeAnalyseResult);
		AnalysebCardData(m_cbHandCardData[wFriendUser], m_cbHandCardCount[wFriendUser], FriendAnalyseResult);

		//�����ж�
		if ((m_cbHandCardCount[wFriendUser]==(FriendAnalyseResult.cbDoubleCount*2+FriendAnalyseResult.cbThreeCount*3+FriendAnalyseResult.cbFourCount*4) || 
			m_cbHandCardCount[wFriendUser]==(FriendAnalyseResult.cbDoubleCount*2+FriendAnalyseResult.cbThreeCount*3+FriendAnalyseResult.cbFourCount*4+1)) &&
			MeAnalyseResult.cbDoubleCount>0 && FriendAnalyseResult.cbDoubleCount>0)
		{
			//��С����
			BYTE cbMeLeastDoubleCardLogicValue = GetCardLogicValue(MeAnalyseResult.cbDoubleCardData[MeAnalyseResult.cbDoubleCount*2-2]);
			//������
			BYTE cbFriendLargestDoublecardLogicValue = GetCardLogicValue(FriendAnalyseResult.cbDoubleCardData[0]);
			//�����ж�
			if (cbMeLeastDoubleCardLogicValue<14 && cbMeLeastDoubleCardLogicValue<cbFriendLargestDoublecardLogicValue)
			{
				OutCardResult.cbCardCount = 2;
				OutCardResult.cbCardData[0] = MeAnalyseResult.cbDoubleCardData[MeAnalyseResult.cbDoubleCount*2-2];
				OutCardResult.cbCardData[1] = MeAnalyseResult.cbDoubleCardData[MeAnalyseResult.cbDoubleCount*2-1];
				return;
			}          			
		}
	}

	//���±�û��
	tagOutCardTypeResult MeOutCardTypeResult[12+1];
	AnalyseOutCardType(cbHandCardData, cbHandCardCount, MeOutCardTypeResult);
	//����ֻʣһ��
	AnalyseEnemyLastOneCard(wMeChairID, cbHandCardData, cbHandCardCount, MeOutCardTypeResult, OutCardResult);
	if (OutCardResult.cbCardCount>0 && GetCardType(OutCardResult.cbCardData,OutCardResult.cbCardCount)!=CT_ERROR) return;

	//��һ�ַ���/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		//���㵥��
		BYTE cbSingleCardCount[4] = {MAX_COUNT,MAX_COUNT,MAX_COUNT,MAX_COUNT};
		BYTE cbOutCardIndex[4] = {0};
		BYTE cbOutCardType[4] = {CT_ERROR};
		BYTE cbMinSingleCardCount = MAX_COUNT;
		BYTE cbMinCardType = CT_ERROR;
		BYTE cbMinCardIndex = 0;
		//��ը�������
		for (BYTE cbCardType=CT_DOUBLE; cbCardType<CT_BOMB_CARD; ++cbCardType)
		{
			const tagOutCardTypeResult &tempOutCardTypeResult = MeOutCardTypeResult[cbCardType];
			//��ͬ���ͣ���ͬ���ȣ���������������ͬ���Ϳ��ܳ��Ȳ�һ��
			BYTE cbThisHandCardCount = MAX_COUNT;
			//�����˿�
			tagOutCardTypeResult BankerCanOutCard[13], FriendOutCardTypeResult[13];
			for (BYTE i=0; i<tempOutCardTypeResult.cbCardTypeCount; ++i)
			{
				//��ֹ��������ը���ж�
				if (cbCardType == CT_DOUBLE)
				{
					tagAnalyseResult AnalyseResult;
					AnalysebCardData(cbHandCardData, cbHandCardCount, AnalyseResult);
					if (AnalyseResult.cbSingleCount+AnalyseResult.cbThreeCount*3 == cbHandCardCount)
					{
						bool bContinue = false;
						for (BYTE cbThreeIndex=0; cbThreeIndex<AnalyseResult.cbThreeCount; ++cbThreeIndex)
						{
							if (GetCardValue(tempOutCardTypeResult.cbEachHandCardData[i][0]) == GetCardValue(AnalyseResult.cbThreeCardData[3*cbThreeIndex]))
							{
								bContinue = true;
								break;
							}
						}
						if (bContinue) continue;
					}
					if (AnalyseResult.cbFourCount > 0)
					{
						bool bContinue = false;
						for (BYTE cbFourIndex=0; cbFourIndex<AnalyseResult.cbFourCount; cbFourIndex++)
						{
							if (GetCardValue(tempOutCardTypeResult.cbEachHandCardData[i][0]) == GetCardValue(AnalyseResult.cbFourCardData[4*cbFourIndex]))
							{
								bContinue = true;
								break;
							}
						}
						if (bContinue) continue;
					}
				}

				BYTE cbTempCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, tempOutCardTypeResult.cbEachHandCardData[i], tempOutCardTypeResult.cbEachHandCardCount[i]);
				//���·���
				if (tempOutCardTypeResult.cbEachHandCardCount[i] != cbThisHandCardCount)
				{
					cbThisHandCardCount = tempOutCardTypeResult.cbEachHandCardCount[i];
					AnalyseOutCardType(m_cbHandCardData[m_wBankerUser], m_cbHandCardCount[m_wBankerUser], tempOutCardTypeResult.cbEachHandCardData[i], tempOutCardTypeResult.cbEachHandCardCount[i], BankerCanOutCard);
					AnalyseOutCardType(m_cbHandCardData[wFriendUser], m_cbHandCardCount[wFriendUser], tempOutCardTypeResult.cbEachHandCardData[i], tempOutCardTypeResult.cbEachHandCardCount[i], FriendOutCardTypeResult);
				}

 				//���˳�ӣ������Ĵ���
 				BYTE cbCurrentCardType = GetCardType(tempOutCardTypeResult.cbEachHandCardData[i], cbThisHandCardCount);
 				if (cbThisHandCardCount!=cbHandCardCount && cbCurrentCardType>=CT_SINGLE_LINE && cbCurrentCardType<=CT_THREE_TAKE_TWO &&
 					(GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][cbThisHandCardCount-1])>GetCardLogicValue(cbHandCardData[cbHandCardCount-2])||GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][0])>=11))
 				{
 					BYTE cbRemainCardCount=0, cbRemainCardData[MAX_COUNT]={0};
 					CopyMemory(cbRemainCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
 					cbRemainCardCount = cbHandCardCount;
 					RemoveCard(tempOutCardTypeResult.cbEachHandCardData[i], cbThisHandCardCount, cbRemainCardData, cbRemainCardCount);
 					cbRemainCardCount -= cbThisHandCardCount;
 					//����˿�
 					BYTE cbCurrentLargestLogicCard = GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][0]);
 					if (GetCardType(cbRemainCardData,cbRemainCardCount)==CT_ERROR && (cbCurrentCardType>=CT_THREE_TAKE_ONE && 
 						cbCurrentCardType<=CT_THREE_TAKE_TWO && cbCurrentLargestLogicCard>=11 && cbThisHandCardCount<=5 || 
 						cbCurrentCardType==CT_SINGLE_LINE && cbThisHandCardCount<=6 && cbCurrentLargestLogicCard>=12 || 
 						cbCurrentCardType>=CT_DOUBLE_LINE && cbCurrentCardType<=CT_THREE_LINE && cbCurrentLargestLogicCard>=12 && cbThisHandCardCount<=8))
 					{
 						//��ʱ����
 						if (cbCurrentCardType>=CT_SINGLE_LINE && cbCurrentCardType<=CT_THREE_LINE && GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][cbThisHandCardCount-1])>GetCardLogicValue(cbHandCardData[cbHandCardCount-3]))
 						{
 							continue;
 						}
 
 						if (cbCurrentCardType>=CT_THREE_TAKE_ONE && cbCurrentCardType<=CT_THREE_TAKE_TWO && GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][0])>GetCardLogicValue(cbHandCardData[cbHandCardCount-3]))
 						{
 							continue;
 						}
 					}
 				}

				//��Դ�ԣ������ȳ���
				if (cbCardType==CT_DOUBLE && GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][0])>=11)
				{
					BYTE cbAllSingleCount=0, cbAllSingleCardData[MAX_COUNT]={0};
					cbAllSingleCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, NULL, 0, cbAllSingleCardData);
					if (cbAllSingleCount>=2 && GetCardLogicValue(cbAllSingleCardData[cbAllSingleCount-2])<10) 
					{
						continue;
					}
				}

				//��������ѹ�ƣ����Ҷ��Ѳ�����ѹ����
				if ((BankerCanOutCard[cbCardType].cbCardTypeCount>0&&FriendOutCardTypeResult[cbCardType].cbCardTypeCount==0) || (BankerCanOutCard[cbCardType].cbCardTypeCount>0 && FriendOutCardTypeResult[cbCardType].cbCardTypeCount>0 &&
					GetCardLogicValue(FriendOutCardTypeResult[cbCardType].cbEachHandCardData[0][0])<=GetCardLogicValue(BankerCanOutCard[cbCardType].cbEachHandCardData[0][0])))
				{
					//�����ܵ�
					if (BankerCanOutCard[cbCardType].cbEachHandCardCount[0]>0 && m_cbHandCardCount[m_wBankerUser]<=BankerCanOutCard[cbCardType].cbEachHandCardCount[0]+1)
					{
						continue;
					}
				}
				//�Ƿ��д���
				if (tempOutCardTypeResult.cbEachHandCardCount[i] != cbHandCardCount)
				{
					bool bHaveLargeCard = false;
					for (BYTE j=0; j<tempOutCardTypeResult.cbEachHandCardCount[i]; ++j)
					{
						if (GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][j]) >= 15)
						{
							bHaveLargeCard = true;
						}
					}

					if (cbCardType!=CT_SINGLE_LINE && cbCardType!=CT_DOUBLE_LINE && GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][0])==14) 
					{
						bHaveLargeCard = true; 
					}
					if (bHaveLargeCard) continue;
				}

				//�����Ƿ�����ߵ������ﶼû�п���ը��
				if (tempOutCardTypeResult.cbEachHandCardCount[i]==m_cbHandCardCount[m_wBankerUser] && cbCardType==GetCardType(m_cbHandCardData[m_wBankerUser],m_cbHandCardCount[m_wBankerUser]) &&
					GetCardLogicValue(m_cbHandCardData[m_wBankerUser][0])>GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][0]))
				{
					continue;
				}
				//����cbMinSingleCardCount[4]�����ֵ
				for (BYTE j=0; j<4; ++j)
				{
					if (cbSingleCardCount[j] >= cbTempCount)
					{
						cbSingleCardCount[j] = cbTempCount;
						cbOutCardIndex[j] = i;
						cbOutCardType[j] = cbCardType;
						break;
					}
				}

				//������Сֵ
				if (cbMinSingleCardCount >= cbTempCount)
				{
					//��С����
					cbMinCardType = cbCardType;
					//��С�����е���С����
					cbMinSingleCardCount = cbTempCount;						
					//��С�����е���С��
					cbMinCardIndex = i;
				}
			}
		}
		if (cbMinSingleCardCount >= AnalyseSinleCardCount(cbHandCardData,cbHandCardCount,NULL,0)+3 && m_cbHandCardCount[m_wBankerUser]>4)
		{
			cbMinSingleCardCount = MAX_COUNT;
		}

		if (cbMinSingleCardCount != MAX_COUNT)
		{
			BYTE cbCurIndex = cbMinCardIndex;
			//ѡ����С��
			for (BYTE i=0; i<4; ++i)
			{
				if (cbOutCardType[i]==cbMinCardType && cbSingleCardCount[i]<=cbMinSingleCardCount && GetCardLogicValue(MeOutCardTypeResult[cbMinCardType].cbEachHandCardData[cbOutCardIndex[i]][0])<GetCardLogicValue(MeOutCardTypeResult[cbMinCardType].cbEachHandCardData[cbCurIndex][0]))
				{
					cbCurIndex = cbOutCardIndex[i];
				}
			}
			//���ñ���
			OutCardResult.cbCardCount = MeOutCardTypeResult[cbMinCardType].cbEachHandCardCount[cbCurIndex];
			CopyMemory(OutCardResult.cbCardData, MeOutCardTypeResult[cbMinCardType].cbEachHandCardData[cbCurIndex], MeOutCardTypeResult[cbMinCardType].cbEachHandCardCount[cbCurIndex]*sizeof(BYTE));
			return;
		}

		//��������˿�����5����û���ҵ��ʺϵ�����Ӵ����С
		if (OutCardResult.cbCardCount<=0 && m_cbHandCardCount[m_wBankerUser]<=5)
		{
			//ֻ��һ����ʱ���ܷŵ�����
			if (m_cbHandCardCount[m_wBankerUser]==1 && MeOutCardTypeResult[CT_SINGLE].cbCardTypeCount>0)
			{
				//��Сһ��
				BYTE cbCurIndex = MAX_COUNT;
				for (BYTE i=0; i<MeOutCardTypeResult[CT_SINGLE].cbCardTypeCount; ++i)
				{
					if (GetCardLogicValue(MeOutCardTypeResult[CT_SINGLE].cbEachHandCardData[i][0]) >= GetCardLogicValue(m_cbHandCardData[m_wBankerUser][0]))
					{
						cbCurIndex = i;
					}
					else
					{
						break;
					}
				}
				if (MAX_COUNT != cbCurIndex)
				{
					OutCardResult.cbCardCount = MeOutCardTypeResult[CT_SINGLE].cbEachHandCardCount[cbCurIndex];
					CopyMemory(OutCardResult.cbCardData, MeOutCardTypeResult[CT_SINGLE].cbEachHandCardData[cbCurIndex], OutCardResult.cbCardCount);
					return;
				}
			}
		}
	}

	//��һ�ַ���/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		BYTE cbFirstCardIndex = 0;
		//��������2
		for (BYTE i=0; i<cbHandCardCount; ++i) 
		{
			if (GetCardLogicValue(cbHandCardData[i]) < 15)
			{
				cbFirstCardIndex = i;
				break;
			}
		}

		if (cbFirstCardIndex < cbHandCardCount-1)
		{
			AnalyseOutCardType(cbHandCardData+cbFirstCardIndex, cbHandCardCount-cbFirstCardIndex, MeOutCardTypeResult);
		}
		else
		{
			AnalyseOutCardType(cbHandCardData, cbHandCardCount, MeOutCardTypeResult);
		}

		//���㵥��
		BYTE cbSingleCardCount[4] = {MAX_COUNT,MAX_COUNT,MAX_COUNT,MAX_COUNT};
		BYTE cbOutCardIndex[4] = {0};
		BYTE cbOutCardType[4] = {CT_ERROR};
		BYTE cbMinSingleCardCount = MAX_COUNT;
		BYTE cbMinCardType = CT_ERROR;
		BYTE cbMinCardIndex = 0;
		//��ը�������
		for (BYTE cbCardType=CT_SINGLE; cbCardType<CT_BOMB_CARD; ++cbCardType)
		{
			const tagOutCardTypeResult &tempOutCardTypeResult = MeOutCardTypeResult[cbCardType];
			for (BYTE i=0; i<tempOutCardTypeResult.cbCardTypeCount; ++i)
			{
				//ׯ�ҿ����ߵ�
				if (CompareCard(tempOutCardTypeResult.cbEachHandCardData[i],m_cbHandCardData[m_wBankerUser],tempOutCardTypeResult.cbEachHandCardCount[i],m_cbHandCardCount[m_wBankerUser]))
				{
					continue;
				}
				//��ֹ��������ը���ж�
				if (cbCardType == CT_DOUBLE)
				{
					tagAnalyseResult AnalyseResult;
					AnalysebCardData(cbHandCardData, cbHandCardCount, AnalyseResult);
					if (AnalyseResult.cbSingleCount+AnalyseResult.cbThreeCount*3 == cbHandCardCount)
					{
						bool bContinue = false;
						for (BYTE cbThreeIndex=0; cbThreeIndex<AnalyseResult.cbThreeCount; ++cbThreeIndex)
						{
							if (GetCardValue(tempOutCardTypeResult.cbEachHandCardData[i][0]) == GetCardValue(AnalyseResult.cbThreeCardData[3*cbThreeIndex]))
							{
								bContinue = true;
								break;
							}
						}
						if (bContinue) continue;
					}
					if (AnalyseResult.cbFourCount > 0)
					{
						bool bContinue = false;
						for (BYTE cbFourIndex=0; cbFourIndex<AnalyseResult.cbFourCount; cbFourIndex++)
						{
							if (GetCardValue(tempOutCardTypeResult.cbEachHandCardData[i][0]) == GetCardValue(AnalyseResult.cbFourCardData[4*cbFourIndex]))
							{
								bContinue = true;
								break;
							}
						}
						if (bContinue) continue;
					}
				}

 				//���˳�ӣ������Ĵ���
 				if (tempOutCardTypeResult.cbEachHandCardCount[i]!=cbHandCardCount && cbCardType>=CT_SINGLE_LINE && cbCardType<=CT_THREE_TAKE_TWO &&
 					(GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][tempOutCardTypeResult.cbEachHandCardCount[i]-1])>GetCardLogicValue(cbHandCardData[cbHandCardCount-2])||GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][0])>=11))
 				{
 					BYTE cbRemainCardCount=0, cbRemainCardData[MAX_COUNT]={0};
 					CopyMemory(cbRemainCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
 					cbRemainCardCount = cbHandCardCount;
 					RemoveCard(tempOutCardTypeResult.cbEachHandCardData[i], tempOutCardTypeResult.cbEachHandCardCount[i], cbRemainCardData, cbRemainCardCount);
 					cbRemainCardCount -= tempOutCardTypeResult.cbEachHandCardCount[i];
 
 					//����˿�
 					BYTE cbCurrentLargestLogicCard = GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][0]);
 					if (GetCardType(cbRemainCardData, cbRemainCardCount)==CT_ERROR && (cbCardType>=CT_THREE_TAKE_ONE && 
 						cbCardType<=CT_THREE_TAKE_TWO && cbCurrentLargestLogicCard>=11 && tempOutCardTypeResult.cbEachHandCardCount[i]<=5 || 
 						cbCardType==CT_SINGLE_LINE && tempOutCardTypeResult.cbEachHandCardCount[i]<=6 && cbCurrentLargestLogicCard>=12 || 
 						cbCardType>=CT_DOUBLE_LINE && cbCardType<=CT_THREE_LINE && cbCurrentLargestLogicCard>=12 && tempOutCardTypeResult.cbEachHandCardCount[i]<=8))
 					{
 						//��ʱ����
 						if (cbCardType>=CT_SINGLE_LINE && cbCardType<=CT_THREE_LINE && GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][tempOutCardTypeResult.cbEachHandCardCount[i]-1])>GetCardLogicValue(cbHandCardData[cbHandCardCount-3]))
 						{
 							continue;
 						}
 
 						if (cbCardType>=CT_THREE_TAKE_ONE && cbCardType<=CT_THREE_TAKE_TWO &&	GetCardLogicValue(tempOutCardTypeResult.cbEachHandCardData[i][0])>GetCardLogicValue(cbHandCardData[cbHandCardCount-3]))
 						{
 							continue;
 						}
 					}
 				}

				BYTE cbTempCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, tempOutCardTypeResult.cbEachHandCardData[i], tempOutCardTypeResult.cbEachHandCardCount[i]);
				//����cbMinSingleCardCount[4]�����ֵ
				for (BYTE j=0; j<4; ++j)
				{
					if (cbSingleCardCount[j] >= cbTempCount)
					{
						cbSingleCardCount[j] = cbTempCount;
						cbOutCardIndex[j] = i;
						cbOutCardType[j] = cbCardType;
						break;
					}
				}

				//������Сֵ
				if (cbMinSingleCardCount >= cbTempCount)
				{
					//��С����
					cbMinCardType = cbCardType;
					//��С�����е���С����
					cbMinSingleCardCount = cbTempCount;						
					//��С�����е���С��
					cbMinCardIndex = i;
				}
			}
		}

		if (cbMinSingleCardCount != MAX_COUNT)
		{
			BYTE cbCurIndex = cbMinCardIndex;
			//ѡ����С��
			for(BYTE i=0; i<4; ++i)
			{
				if (cbOutCardType[i]==cbMinCardType && cbSingleCardCount[i]<=cbMinSingleCardCount && GetCardLogicValue(MeOutCardTypeResult[cbMinCardType].cbEachHandCardData[cbOutCardIndex[i]][0])<GetCardLogicValue(MeOutCardTypeResult[cbMinCardType].cbEachHandCardData[cbCurIndex][0]))
				{
					cbCurIndex = cbOutCardIndex[i];
				}
			}
			//���ñ���
			OutCardResult.cbCardCount = MeOutCardTypeResult[cbMinCardType].cbEachHandCardCount[cbCurIndex];
			CopyMemory(OutCardResult.cbCardData, MeOutCardTypeResult[cbMinCardType].cbEachHandCardData[cbCurIndex], MeOutCardTypeResult[cbMinCardType].cbEachHandCardCount[cbCurIndex]*sizeof(BYTE));
			return;
		}
	}

	//�������Ͳ��ҷ���ʧ��
	//ֻʣը���ͻ��
	ZeroMemory(cbTempCardData, sizeof(cbTempCardData));
	cbTempCardCount = cbHandCardCount-cbFirstCardIndex;
	CopyMemory(cbTempCardData, &cbHandCardData[cbFirstCardIndex], sizeof(BYTE)*cbTempCardCount);
	AnalysebCardData(cbTempCardData, cbTempCardCount, MeAnalyseResult);
	if (MeAnalyseResult.cbFourCount>0 && MeAnalyseResult.cbFourCount*4==cbTempCardCount)
	{
		for (int i=MeAnalyseResult.cbFourCount-1; i>=0; i--)
		{
			OutCardResult.cbCardCount = 4;
			CopyMemory(OutCardResult.cbCardData, &MeAnalyseResult.cbFourCardData[i*4], sizeof(BYTE)*4);
			return;
		}
	}
	BYTE cbAllSingleCardCount=0, cbAllSingleCardData[MAX_COUNT]={0};
	cbAllSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, NULL, 0, cbAllSingleCardData);
	if (cbAllSingleCardCount > 0)
	{
		//�����û���������ͳ���С��һ��
		OutCardResult.cbCardCount = 1;
		OutCardResult.cbCardData[0] = cbAllSingleCardData[cbAllSingleCardCount-1];
		return;
	}

	//�����û���������ͳ���С��һ��
	OutCardResult.cbCardCount = 1;
	OutCardResult.cbCardData[0] = cbHandCardData[cbHandCardCount-1];
	return;
}

//�����¼ң�����ƣ�
void CAndroidLogic::UndersideOfBankerOutCard( const BYTE cbHandCardData[], const BYTE cbHandCardCount, const BYTE cbTurnCardData[], const BYTE cbTurnCardCount, const WORD wMeChairID, const WORD wOutCardUser, tagOutCardResult &OutCardResult )
{
	//��ʼ����
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));
	//��������
	BYTE cbOutCardType = GetCardType(cbTurnCardData,cbTurnCardCount);
	if (cbOutCardType == CT_MISSILE_CARD) return;

	//�������
	const WORD wFriendUser = (wMeChairID+1)%GAME_PLAYER;	//����,�����ϼ�

	//�Լ�ֻʣ�����һ��
	BYTE cbFirstCardIndex = 0;
	BYTE cbTempCardData[MAX_COUNT]={0}, cbTempCardCount=0;
	if (cbHandCardCount>=2 && cbHandCardData[0]==0x4F && cbHandCardData[1]==0x4E)
	{
		cbFirstCardIndex = 2;
		if (cbHandCardCount==2 || GetCardType(&cbHandCardData[2],cbHandCardCount-2)!=CT_ERROR)
		{
			OutCardResult.cbCardCount = 2;
			OutCardResult.cbCardData[0] = 0x4F;
			OutCardResult.cbCardData[1] = 0x4E;
			return;
		}
	}

	//�Լ�ʣ����������ܴ��������,����һ�����в�����ը��(ը�����ͳ���)
	BYTE cbBombCardCount=0, cbBombCardData[MAX_COUNT]={0};
	GetAllBombCard(cbHandCardData, cbHandCardCount, cbBombCardData, cbBombCardCount);
	if (CompareCard(cbTurnCardData,cbHandCardData,cbTurnCardCount,cbHandCardCount) && (GetCardType(cbHandCardData,cbHandCardCount)>=CT_BOMB_CARD||cbBombCardCount==0))
	{
		OutCardResult.cbCardCount = cbHandCardCount;
		CopyMemory(OutCardResult.cbCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
		return;
	}

	//�Լ�ֻʣը���ͻ��
	cbTempCardCount = cbHandCardCount-cbFirstCardIndex;
	CopyMemory(cbTempCardData, &cbHandCardData[cbFirstCardIndex], sizeof(BYTE)*cbTempCardCount);
	tagAnalyseResult MeAnalyseResult;
	AnalysebCardData(cbTempCardData, cbTempCardCount, MeAnalyseResult);
	//�ж��Ƿ���Գ�ը��
	bool bCanOutBombCard = false;
	BYTE cbCard2 = 0x02;
	//�����ѳ�
	if (IsLargestCardWithOutBomb(wMeChairID,&cbCard2,1))
	{
		bCanOutBombCard = true;
	}
	//������ڵ������п��Գ�
	if (m_cbHandCardCount[m_wBankerUser]>=2 && m_cbHandCardData[m_wBankerUser][1]!=0x4E)
	{
		bCanOutBombCard = true;
	}
	//ȥ������������ƻ���������
	if (m_cbHandCardCount[m_wBankerUser] >= 5)
	{
		bCanOutBombCard = true;
	}
	if (MeAnalyseResult.cbFourCount>0 && MeAnalyseResult.cbFourCount*4==cbTempCardCount)
	{
		for (int i=MeAnalyseResult.cbFourCount-1; i>=0; i--)
		{
			if (bCanOutBombCard && CompareCard(cbTurnCardData,&MeAnalyseResult.cbFourCardData[i*4],cbTurnCardCount,4))
			{
				OutCardResult.cbCardCount = 4;
				CopyMemory(OutCardResult.cbCardData, &MeAnalyseResult.cbFourCardData[i*4], sizeof(BYTE)*4);
				return;
			}
			else if (cbHandCardCount>=2 && cbHandCardData[0]==0x4F && cbHandCardData[1]==0x4E)
			{
				OutCardResult.cbCardCount = 2;
				OutCardResult.cbCardData[0] = 0x4F;
				OutCardResult.cbCardData[1] = 0x4E;
				return;
			}
		}
	}

	//ը����һ��
	if (cbBombCardCount>0 && wOutCardUser==m_wBankerUser)
	{
		//ʣ���˿�
		BYTE cbRemainCardCount=0, cbRemainCardData[MAX_COUNT]={0};
		CopyMemory(cbRemainCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
		cbRemainCardCount = cbHandCardCount;
		RemoveCard(cbBombCardData, cbBombCardCount, cbRemainCardData, cbRemainCardCount);
		cbRemainCardCount -= cbBombCardCount;
		BYTE cbRemainMaxValue = cbRemainCardCount>0 ? GetCardLogicValue(cbRemainCardData[0]) : 0;
		BYTE cbBankerMaxValue = GetCardLogicValue(m_cbHandCardData[m_wBankerUser][0]);
		if ((cbRemainCardCount>0&&GetCardType(cbRemainCardData,cbRemainCardCount)!=CT_ERROR) || (cbRemainCardCount==2&&cbRemainMaxValue>cbBankerMaxValue))
		{
			//˫��ը��
			if (GetCardColor(cbBombCardData[0]) == 0x40)
			{
				OutCardResult.cbCardCount = 2;
				OutCardResult.cbCardData[0] = 0x4F;
				OutCardResult.cbCardData[1] = 0x4E;
				return;
			}
			else
			{
				//ը����ѹ������
				if (CompareCard(cbTurnCardData,cbBombCardData,cbTurnCardCount,4))
				{
					tagPromptCardResult BankerPromptCardResult;
					CGameLogic::SearchOutCard(m_cbHandCardData[m_wBankerUser], m_cbHandCardCount[m_wBankerUser], cbBombCardData, 4, BankerPromptCardResult);
					//�س�ը��
					if (BankerPromptCardResult.cbResultCount == 0)
					{
						OutCardResult.cbCardCount = 4;
						CopyMemory(OutCardResult.cbCardData, cbBombCardData, sizeof(BYTE)*4);
						return;
					}
					else
					{
						if (CompareCard(cbTurnCardData,cbRemainCardData,cbTurnCardCount,cbRemainCardCount))
						{
							OutCardResult.cbCardCount = cbRemainCardCount;
							CopyMemory(OutCardResult.cbCardData, cbRemainCardData, sizeof(BYTE)*cbRemainCardCount);
							return;
						}
					}
				}
			}
		}
	}

	//��������û��ը��
	if (m_cbHandCardCount[m_wBankerUser]==1 || (m_cbHandCardCount[m_wBankerUser]<4&&IsLargestCardWithOutBomb(wMeChairID,&cbCard2,1)))
	{
		cbBombCardCount = 0;
		ZeroMemory(cbBombCardData, sizeof(cbBombCardData));
		GetAllBombCard(cbHandCardData, cbHandCardCount, cbBombCardData, cbBombCardCount);
		BYTE cbRemainCardCount=0, cbRemainCardData[MAX_COUNT]={0};
		CopyMemory(cbRemainCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
		cbRemainCardCount = cbHandCardCount;
		RemoveCard(cbBombCardData, cbBombCardCount, cbRemainCardData, cbRemainCardCount);
		cbRemainCardCount -= cbBombCardCount;
		if (cbBombCardCount > 0)
		{
			//��������ֻ��һ��
			if (m_cbHandCardCount[wFriendUser] == 1)
			{
				if (cbRemainCardCount>0 && GetCardLogicValue(cbRemainCardData[cbRemainCardCount-1])<GetCardLogicValue(m_cbHandCardData[wFriendUser][0]))
				{
					if (GetCardColor(cbBombCardData[0]) == 0x40)
					{
						OutCardResult.cbCardCount = 2;
						OutCardResult.cbCardData[0] = 0x4F;
						OutCardResult.cbCardData[1] = 0x4E;
						return;
					}
					else
					{
						if (CompareCard(cbTurnCardData,cbBombCardData,cbTurnCardCount,4))
						{
							OutCardResult.cbCardCount = 4;
							CopyMemory(OutCardResult.cbCardData, cbBombCardData, sizeof(BYTE)*4);
							return;
						}
					}
				}
			}
			//�������о�ʣһ��
			else if (GetCardType(m_cbHandCardData[wFriendUser],m_cbHandCardCount[wFriendUser]) == CT_DOUBLE)
			{
				BYTE cbDoubleCardCount=0, cbDoubleCardData[MAX_COUNT]={0};
				GetAllDoubleCard(cbRemainCardData, cbRemainCardCount, cbDoubleCardData, cbDoubleCardCount);
				if (cbDoubleCardCount>=2 && GetCardLogicValue(cbDoubleCardData[cbDoubleCardCount-1])<GetCardLogicValue(m_cbHandCardData[wFriendUser][0]))
				{
					if (GetCardColor(cbBombCardData[0]) == 0x40)
					{
						OutCardResult.cbCardCount = 2;
						OutCardResult.cbCardData[0] = 0x4F;
						OutCardResult.cbCardData[1] = 0x4E;
						return;
					}
					else
					{
						if (CompareCard(cbTurnCardData,cbBombCardData,cbTurnCardCount,4))
						{
							OutCardResult.cbCardCount = 4;
							CopyMemory(OutCardResult.cbCardData, cbBombCardData, sizeof(BYTE)*4);
							return;
						}
					}
				}
			}
		}
	}

	//���±�û��
	tagOutCardTypeResult MeOutCardTypeResult[12+1], BankerOutCardTypeResult[13];
	AnalyseOutCardType(cbHandCardData,cbHandCardCount,cbTurnCardData,cbTurnCardCount, MeOutCardTypeResult);
	AnalyseOutCardType(m_cbHandCardData[m_wBankerUser], m_cbHandCardCount[m_wBankerUser], BankerOutCardTypeResult);
	//������ʣһ��
	if (m_cbHandCardCount[m_wBankerUser] == 1)
	{
		//��������ǵ���
		if (wOutCardUser == m_wBankerUser)
		{
			//�жϳ���ը����ʣ��ֻ��һ������
			if (cbBombCardCount > 0)
			{
				BYTE cbRemainCardData[MAX_COUNT]={0}, cbRemainCardCount=cbHandCardCount;
				CopyMemory(cbRemainCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
				RemoveCard(cbBombCardData, cbBombCardCount, cbRemainCardData, cbRemainCardCount);
				cbRemainCardCount -= cbBombCardCount;
				tagAnalyseResult RemainAnalyseResult;
				AnalysebCardData(cbRemainCardData, cbRemainCardCount, RemainAnalyseResult);
				//��ը
				if (RemainAnalyseResult.cbThreeCount+1 >= RemainAnalyseResult.cbSingleCount)
				{
					//˫��ը��
					if (GetCardColor(cbBombCardData[0]) == 0x40)
					{
						OutCardResult.cbCardCount = 2;
						OutCardResult.cbCardData[0] = 0x4F;
						OutCardResult.cbCardData[1] = 0x4E;
						return;
					}
					else
					{
						if (CompareCard(cbTurnCardData,cbBombCardData,cbTurnCardCount,4))
						{
							OutCardResult.cbCardCount = 4;
							CopyMemory(OutCardResult.cbCardData, cbBombCardData, sizeof(BYTE)*4);
							return;
						}
					}	
				}
				else
				{
					if (MeOutCardTypeResult[cbOutCardType].cbCardTypeCount > 0)
					{
						OutCardResult.cbCardCount = MeOutCardTypeResult[cbOutCardType].cbEachHandCardCount[0];
						CopyMemory(OutCardResult.cbCardData, &MeOutCardTypeResult[cbOutCardType].cbEachHandCardData[0], sizeof(BYTE)*OutCardResult.cbCardCount);
						return;
					}
				}
			}
			else
			{
				if (MeOutCardTypeResult[cbOutCardType].cbCardTypeCount > 0)
				{
					OutCardResult.cbCardCount = MeOutCardTypeResult[cbOutCardType].cbEachHandCardCount[0];
					CopyMemory(OutCardResult.cbCardData, &MeOutCardTypeResult[cbOutCardType].cbEachHandCardData[0], sizeof(BYTE)*OutCardResult.cbCardCount);
					return;
				}
			}
		}
		//��������Ƕ���
		else if (wOutCardUser == wFriendUser)
		{
			//�жϳ���ը����ʣ��ֻ��һ������
			if (cbBombCardCount > 0)
			{
				BYTE cbRemainCardData[MAX_COUNT]={0}, cbRemainCardCount=cbHandCardCount;
				CopyMemory(cbRemainCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
				RemoveCard(cbBombCardData, cbBombCardCount, cbRemainCardData, cbRemainCardCount);
				cbRemainCardCount -= cbBombCardCount;
				tagAnalyseResult RemainAnalyseResult;
				AnalysebCardData(cbRemainCardData, cbRemainCardCount, RemainAnalyseResult);
				//��ը
				if (RemainAnalyseResult.cbThreeCount+1 >= RemainAnalyseResult.cbSingleCount)
				{
					//˫��ը��
					if (GetCardColor(cbBombCardData[0]) == 0x40)
					{
						OutCardResult.cbCardCount = 2;
						OutCardResult.cbCardData[0] = 0x4F;
						OutCardResult.cbCardData[1] = 0x4E;
						return;
					}
					else
					{
						if (CompareCard(cbTurnCardData,cbBombCardData,cbTurnCardCount,4))
						{
							OutCardResult.cbCardCount = 4;
							CopyMemory(OutCardResult.cbCardData, cbBombCardData, sizeof(BYTE)*4);
							return;
						}
					}	
				}
				else
				{
					//���ж϶����ܷ�ȫ�����꣬���ж��Լ��ܷ�ȫ������
					tagAnalyseResult FriendAnalyseResult;
					AnalysebCardData(m_cbHandCardData[wFriendUser], m_cbHandCardCount[wFriendUser], FriendAnalyseResult);
					if (FriendAnalyseResult.cbSingleCount==0 || FriendAnalyseResult.cbThreeCount+1>=FriendAnalyseResult.cbSingleCount
						|| (FriendAnalyseResult.cbThreeCount+2==FriendAnalyseResult.cbSingleCount && GetCardLogicValue(FriendAnalyseResult.cbSingleCardData[0])>=GetCardLogicValue(m_cbHandCardData[m_wBankerUser][0])))
					{
						return;
					}
				}
			}
			else
			{
				//���ж϶����ܷ�ȫ�����꣬���ж��Լ��ܷ�ȫ������
				tagAnalyseResult FriendAnalyseResult;
				AnalysebCardData(m_cbHandCardData[wFriendUser], m_cbHandCardCount[wFriendUser], FriendAnalyseResult);
				if (FriendAnalyseResult.cbSingleCount==0 || FriendAnalyseResult.cbThreeCount+1>=FriendAnalyseResult.cbSingleCount
					|| (FriendAnalyseResult.cbThreeCount+2==FriendAnalyseResult.cbSingleCount && GetCardLogicValue(FriendAnalyseResult.cbSingleCardData[0])>=GetCardLogicValue(m_cbHandCardData[m_wBankerUser][0])))
				{
					return;
				}
				//���±�û��
				tagOutCardTypeResult FriendCardTypeResult[12+1];
				AnalyseOutCardType(m_cbHandCardData[wFriendUser], m_cbHandCardCount[wFriendUser], FriendCardTypeResult);
				for (BYTE cbCurIndex=0; cbCurIndex<FriendCardTypeResult[CT_SINGLE_LINE].cbCardTypeCount; ++cbCurIndex)
				{
					//ʣ���˿�
					BYTE cbRemainCardData[MAX_COUNT]={0}, cbRemainCardCount=m_cbHandCardCount[wFriendUser];
					CopyMemory(cbRemainCardData, m_cbHandCardData[wOutCardUser], sizeof(BYTE)*cbRemainCardCount);
					RemoveCard(FriendCardTypeResult[CT_SINGLE_LINE].cbEachHandCardData[cbCurIndex], FriendCardTypeResult[CT_SINGLE_LINE].cbEachHandCardCount[cbCurIndex], cbRemainCardData, cbRemainCardCount);
					cbRemainCardCount -= FriendCardTypeResult[CT_SINGLE_LINE].cbEachHandCardCount[cbCurIndex];
					//�����˿�
					AnalysebCardData(cbRemainCardData, cbRemainCardCount, FriendAnalyseResult);
					//�����ж�
					if (FriendAnalyseResult.cbSingleCount==0 || FriendAnalyseResult.cbThreeCount+1>=FriendAnalyseResult.cbSingleCount
						|| (FriendAnalyseResult.cbThreeCount+2==FriendAnalyseResult.cbSingleCount && GetCardLogicValue(FriendAnalyseResult.cbSingleCardData[0])>=GetCardLogicValue(m_cbHandCardData[m_wBankerUser][0])))
					{
						return;
					}
				}
				for (BYTE i=0; i<MeOutCardTypeResult[cbOutCardType].cbCardTypeCount; i++)
				{
					BYTE cbRemainCardData[MAX_COUNT]={0}, cbRemainCardCount=cbHandCardCount;
					CopyMemory(cbRemainCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
					RemoveCard(MeOutCardTypeResult[cbOutCardType].cbEachHandCardData[i], MeOutCardTypeResult[cbOutCardType].cbEachHandCardCount[i], cbRemainCardData, cbRemainCardCount);
					cbRemainCardCount -= MeOutCardTypeResult[cbOutCardType].cbEachHandCardCount[i];
					tagAnalyseResult RemainCardResult;
					AnalysebCardData(cbRemainCardData, cbRemainCardCount, RemainCardResult);
					if (RemainCardResult.cbSingleCount==0 || RemainCardResult.cbThreeCount+1>=FriendAnalyseResult.cbSingleCount
						|| (RemainCardResult.cbThreeCount+2==RemainCardResult.cbSingleCount && GetCardLogicValue(RemainCardResult.cbSingleCardData[0])>=GetCardLogicValue(m_cbHandCardData[m_wBankerUser][0])))
					{
						OutCardResult.cbCardCount = MeOutCardTypeResult[cbOutCardType].cbEachHandCardCount[i];
						CopyMemory(OutCardResult.cbCardData, &MeOutCardTypeResult[cbOutCardType].cbEachHandCardData[i], sizeof(BYTE)*OutCardResult.cbCardCount);
						return;
					}
				}
			}
		}
	}
	//���߶Լ�
	if (CompareCard(cbTurnCardData,m_cbHandCardData[wFriendUser],cbTurnCardCount,m_cbHandCardCount[wFriendUser]))
	{
		return;
	}

	//ȡ���ĸ���С����
	BYTE cbSingleCardCount[4] = {MAX_COUNT,MAX_COUNT,MAX_COUNT,MAX_COUNT};
	BYTE cbOutCardIndex[4] = {0};	
	BYTE cbMinSingleCardCount = MAX_COUNT;
	//�ɳ��˿ˣ������Ѿ����˵�ը���ˣ�
	const tagOutCardTypeResult &CanOutCardType = MeOutCardTypeResult[cbOutCardType];
	for (BYTE i=0; i<CanOutCardType.cbCardTypeCount; ++i)
	{
		//��С����
		BYTE cbTempCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount,CanOutCardType.cbEachHandCardData[i], CanOutCardType.cbEachHandCardCount[i]); 
		//����cbMinSingleCardCount[4]�����ֵ
		for (BYTE j=0; j<4; ++j)
		{
			if (cbSingleCardCount[j] >= cbTempCount)
			{
				cbSingleCardCount[j] = cbTempCount;
				cbOutCardIndex[j] = i;
				break;
			}
		}
	}
	for (BYTE i=0; i<4; ++i)
	{
		if (cbMinSingleCardCount > cbSingleCardCount[i])
		{
			cbMinSingleCardCount = cbSingleCardCount[i];
		}
	}
	//ԭʼ������
	BYTE cbOriginSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount,NULL,0);
	//���ѳ���
	if (wOutCardUser == wFriendUser)
	{
		//�������������һ����
		if (GetCardType(m_cbHandCardData[wFriendUser],m_cbHandCardCount[wFriendUser]) != CT_ERROR)
		{
			return;
		}

		//�������TestOutAllCard���ѶԿɳ�ը�����������
		if (CanOutCardType.cbCardTypeCount>0 && CanOutCardType.cbCardType<CT_BOMB_CARD && cbMinSingleCardCount<MAX_COUNT)
		{
			BYTE cbMinLogicCardValue = GetCardLogicValue(0x4F)+1;
			bool bFindCard = false;
			BYTE cbCanOutIndex = 0;
			for (BYTE i=0; i<4; ++i)
			{
				BYTE cbCurIndex = cbOutCardIndex[i];				
				//�����������Ʋ��ӶԼ���
				if (CanOutCardType.cbCardType>=CT_THREE && CanOutCardType.cbCardType<=CT_MISSILE_CARD && GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbCurIndex][0])>=7 && CanOutCardType.cbEachHandCardCount[cbCurIndex]<=5)
				{
					continue;
				}				
				//С��J���ƣ�����С��K������ɢ��
				if ((cbSingleCardCount[i]<cbOriginSingleCardCount+3 && (cbSingleCardCount[i]<=cbMinSingleCardCount||cbSingleCardCount[i]<=cbMinSingleCardCount+1 && 
					CanOutCardType.cbCardType>=CT_THREE_TAKE_ONE && CanOutCardType.cbCardType<=CT_THREE_TAKE_TWO) && (GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbCurIndex][0])<=11 || (cbSingleCardCount[i]<cbOriginSingleCardCount)&&GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbCurIndex][0])<=13)) &&
					cbMinLogicCardValue>GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbCurIndex][0]) && cbHandCardCount>5)
				{
					//������û�д��ƣ���Էɻ���������Ĵ��ƣ�
					bool bNoLargeCard = true;
					for (BYTE k=3; k<CanOutCardType.cbEachHandCardCount[cbCurIndex]; ++k)
					{
						//�д��ƶ��Ҳ���һ�γ���
						if (GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbCurIndex][k])>=15 && CanOutCardType.cbEachHandCardCount[cbCurIndex]!=cbHandCardCount) 
						{
							bNoLargeCard = false;
						}
					}
					if (bNoLargeCard)
					{
						bFindCard = true;
						cbCanOutIndex = cbCurIndex; 
						cbMinLogicCardValue = GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbCurIndex][0]);
					}
				}
				else if (cbHandCardCount<5 && cbSingleCardCount[i]<cbOriginSingleCardCount+4 && cbSingleCardCount[i]<=cbMinSingleCardCount && cbMinLogicCardValue>GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbCurIndex][0]))
				{
					//�ܳ������Լҵ�2
					if (GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbCurIndex][0])>=16 && GetCardLogicValue(cbTurnCardData[0])>=15)
					{
						continue;
					}
					bFindCard = true;
					cbCanOutIndex = cbCurIndex; 
					cbMinLogicCardValue = GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbCurIndex][0]);
				}
			}

			if (bFindCard)
			{
				//���ñ���
				OutCardResult.cbCardCount = CanOutCardType.cbEachHandCardCount[cbCanOutIndex];
				CopyMemory(OutCardResult.cbCardData, CanOutCardType.cbEachHandCardData[cbCanOutIndex], CanOutCardType.cbEachHandCardCount[cbCanOutIndex]*sizeof(BYTE));
				return;
			}
			//��������������
			else if (cbHandCardCount <= 5)
			{	
				BYTE cbCanOutIndex = 0;
				BYTE cbMinLogicCard = GetCardLogicValue(0x4F)+1;
				for (BYTE i=0; i<4; ++i)
				{
					if (cbSingleCardCount[i]<MAX_COUNT && cbSingleCardCount[i]<=cbMinSingleCardCount && cbMinLogicCard>GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbOutCardIndex[i]][0]) && GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbOutCardIndex[i]][0])<=14)
					{
						cbMinLogicCard = GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbOutCardIndex[i]][0]);
						cbCanOutIndex = cbOutCardIndex[i];
					}
				}
				if (cbMinLogicCard != (GetCardLogicValue(0x4F)+1))
				{
					//���ñ���
					OutCardResult.cbCardCount = CanOutCardType.cbEachHandCardCount[cbCanOutIndex];
					CopyMemory(OutCardResult.cbCardData, CanOutCardType.cbEachHandCardData[cbCanOutIndex], CanOutCardType.cbEachHandCardCount[cbCanOutIndex]*sizeof(BYTE));
					return;
				}
			}
			return;
		}
		else
		{
			return;
		}
	}
	//��������
	else
	{
		if (CanOutCardType.cbCardTypeCount>0 && cbMinSingleCardCount<MAX_COUNT)
		{
			BYTE cbMinLogicCardValue = GetCardLogicValue(0x4F)+1;
			bool bFindCard = false;
			BYTE cbCanOutIndex = 0;
			for (BYTE i=0; i<4; ++i)
			{
				BYTE cbCurIndex = cbOutCardIndex[i];
				if ((cbSingleCardCount[i]<cbOriginSingleCardCount+4) && (cbSingleCardCount[i]<=cbMinSingleCardCount || cbSingleCardCount[i]<=cbMinSingleCardCount+1&&CanOutCardType.cbCardType>=CT_THREE_TAKE_ONE&&CanOutCardType.cbCardType<=CT_THREE_TAKE_TWO) 
					&& cbMinLogicCardValue>GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbCurIndex][0]))
				{
					//��Դ���
					bool bNoLargeCard = true;
					//������������������4�����ҵ���������С��K���ƶ��Ҳ��ǵ������ϵ������ʱ�����ܳ�2ȥ��
					if (m_cbHandCardCount[m_wBankerUser]>=4 && cbHandCardCount>=5 && CanOutCardType.cbEachHandCardCount[cbCurIndex]>=2 && 
						GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbCurIndex][0])>=15 && GetCardLogicValue(cbTurnCardData[0])<13 &&
						GetCardLogicValue(cbTurnCardData[0])<GetCardLogicValue(m_cbHandCardData[m_wBankerUser][0]) && CanOutCardType.cbEachHandCardCount[cbCurIndex]!=cbHandCardCount)
					{
						bNoLargeCard = false;
					}

					//������û�д��ƣ���Էɻ���������Ĵ��ƣ�
					for (BYTE k=3; k<CanOutCardType.cbEachHandCardCount[cbCurIndex]; ++k)
					{
						if (GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbCurIndex][k])>=15 && CanOutCardType.cbEachHandCardCount[cbCurIndex]!=cbHandCardCount)
						{
							bNoLargeCard = false;
						}
					}
					if (bNoLargeCard)
					{
						bFindCard = true;
						cbCanOutIndex = cbCurIndex; 
						cbMinLogicCardValue = GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbCurIndex][0]);
					}
				}
			}

			if (bFindCard)
			{
				//�����������
				BYTE cbLargestLogicCard = GetCardLogicValue(m_cbHandCardData[m_wBankerUser][0]);
				bool bCanOut = true;
				//��ֻѹ2
				if (GetCardLogicValue(cbTurnCardData[0]) < cbLargestLogicCard)
				{
					if (GetCardColor(CanOutCardType.cbEachHandCardData[cbCanOutIndex][0])==0x40 && GetCardLogicValue(cbTurnCardData[0])<=14 && cbHandCardCount>5) 								
					{
						bCanOut = false;
					}
				}

                //˫���ж�
				if (GetCardLogicValue(CanOutCardType.cbEachHandCardData[cbCanOutIndex][0])>=16 && cbHandCardCount>=2 && cbHandCardData[0]==0x4F && cbHandCardData[1]==0x4E)
				{
					bool bOutMissileCard = false;
					//һ���ƺ�һ��ը��
					BYTE cbRemainCardData[20]={0}, cbRemainCardCount=cbHandCardCount;
					CopyMemory(cbRemainCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
					RemoveCard(cbRemainCardData, 2, cbRemainCardData, cbRemainCardCount);
					cbRemainCardCount -= 2;
					if (CT_ERROR != GetCardType(cbRemainCardData,cbRemainCardCount))
					{
						bOutMissileCard = true;
					}
					//ֻʣ������
					if (cbRemainCardCount<5 && cbRemainCardCount>0 && GetCardLogicValue(cbRemainCardData[0])>=14) 
					{
						bOutMissileCard = true;
					}

					//ը������
					BYTE cbSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, CanOutCardType.cbEachHandCardData[cbCanOutIndex], CanOutCardType.cbEachHandCardCount[cbCanOutIndex]);
					if (cbSingleCardCount<=1 && GetCardLogicValue(cbRemainCardData[0])>=11) 
					{
						bOutMissileCard = true;
					}

					//����С��
					if (GetCardLogicValue(cbRemainCardData[0])<=10 && CT_ERROR==GetCardType(cbRemainCardData,cbRemainCardCount) && GetCardLogicValue(m_cbHandCardData[m_wBankerUser][0])>10)
					{
						bOutMissileCard = false;
					}
					//����˿�
					if (bOutMissileCard)
					{
						//��������ը��
						BYTE cbIndex = MeOutCardTypeResult[CT_BOMB_CARD].cbCardTypeCount - 1;
						OutCardResult.cbCardCount = MeOutCardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[cbIndex];
						CopyMemory(OutCardResult.cbCardData, MeOutCardTypeResult[CT_BOMB_CARD].cbEachHandCardData[cbIndex], OutCardResult.cbCardCount);
						return;
					}
				}
				if (bCanOut)
				{
					//���ñ���
					OutCardResult.cbCardCount = CanOutCardType.cbEachHandCardCount[cbCanOutIndex];
					CopyMemory(OutCardResult.cbCardData, CanOutCardType.cbEachHandCardData[cbCanOutIndex], CanOutCardType.cbEachHandCardCount[cbCanOutIndex]*sizeof(BYTE));
					return;
				}
			}

			if (cbOutCardType == CT_SINGLE)
			{
				//�����������
				BYTE cbLargestLogicCard = GetCardLogicValue(m_cbHandCardData[m_wBankerUser][0]);
				if (GetCardLogicValue(cbTurnCardData[0])==14 || GetCardLogicValue(cbTurnCardData[0])>=cbLargestLogicCard || GetCardLogicValue(cbTurnCardData[0])<cbLargestLogicCard-1 || m_cbHandCardCount[m_wBankerUser]<=5)
				{
					//ȡһ�Ŵ��ڵ���2����Ҫ�ȵ��������ƴ���ƣ�
					BYTE cbCurcbIndex = MAX_COUNT;
					for (BYTE i=0; i<cbHandCardCount; ++i)
					{
						if (GetCardLogicValue(cbHandCardData[i])>GetCardLogicValue(cbTurnCardData[0]) && GetCardLogicValue(cbHandCardData[i])>=15)
						{
							cbCurcbIndex = i;
						}
					}
					if (cbCurcbIndex != MAX_COUNT)
					{
						//���ñ���
						OutCardResult.cbCardCount = 1;
						OutCardResult.cbCardData[0] = cbHandCardData[cbCurcbIndex];
						return;
					}
				}
			}

			//�����Ѳ������ص���ʱ
			tagOutCardTypeResult FriendCardTypeResult[13];
			AnalyseOutCardType(m_cbHandCardData[wFriendUser], m_cbHandCardCount[wFriendUser], cbTurnCardData, cbTurnCardCount, FriendCardTypeResult);
			//�����Ѳ������ص���ʱ
			if (m_cbHandCardCount[m_wBankerUser]<=4 && FriendCardTypeResult[cbOutCardType].cbCardTypeCount==0 && MeOutCardTypeResult[cbOutCardType].cbCardTypeCount>0)
			{
				BYTE cbMinSingleCount = MAX_COUNT;
				BYTE cbCurIndex = 0;
				for (BYTE i=0; i<MeOutCardTypeResult[cbOutCardType].cbCardTypeCount; ++i)
				{
					BYTE cbTempCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, MeOutCardTypeResult[cbOutCardType].cbEachHandCardData[i], MeOutCardTypeResult[cbOutCardType].cbEachHandCardCount[i]);
					if (cbMinSingleCount >= cbTempCount)
					{
						cbMinSingleCount = cbTempCount;
						cbCurIndex = i;
					}
				}
				//���ñ���
				OutCardResult.cbCardCount = MeOutCardTypeResult[cbOutCardType].cbEachHandCardCount[cbCurIndex];
				CopyMemory(OutCardResult.cbCardData, MeOutCardTypeResult[cbOutCardType].cbEachHandCardData[cbCurIndex], OutCardResult.cbCardCount);
				return;
			}
		}

		//��Ҫ����ը��
		if (MeOutCardTypeResult[CT_BOMB_CARD].cbCardTypeCount>0 && NORMAL_COUNT==cbHandCardCount && NORMAL_COUNT==m_cbHandCardCount[(m_wBankerUser+2)%GAME_PLAYER])
		{
			const tagOutCardTypeResult &BomCardType = MeOutCardTypeResult[CT_BOMB_CARD];
			BYTE cbMinLogicValue = GetCardLogicValue(BomCardType.cbEachHandCardData[0][0]);
			BYTE cbCurIndex = 0;
			for (BYTE i=0; i<BomCardType.cbCardTypeCount; ++i)
			{
				if (cbMinLogicValue > GetCardLogicValue(BomCardType.cbEachHandCardData[i][0]))
				{
					cbMinLogicValue = GetCardLogicValue(BomCardType.cbEachHandCardData[i][0]);
					cbCurIndex = i;
				}
			}

			bool bOutBomb = false;
			//�����ж�
			if (NORMAL_COUNT==cbHandCardCount && NORMAL_COUNT==m_cbHandCardCount[(m_wBankerUser+2)%GAME_PLAYER] && CT_ERROR!=GetCardType(m_cbHandCardData[m_wBankerUser],m_cbHandCardCount[m_wBankerUser]))
			{
				bOutBomb = true;
			}

			//һ���ƺ�һ��ը��
			BYTE cbRemainCardData[20]={0}, cbRemainCardCount=cbHandCardCount;
			CopyMemory(cbRemainCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
			RemoveCard(BomCardType.cbEachHandCardData[cbCurIndex], BomCardType.cbEachHandCardCount[cbCurIndex], cbRemainCardData, cbRemainCardCount);
			cbRemainCardCount -= BomCardType.cbEachHandCardCount[cbCurIndex];
			if (CT_ERROR != GetCardType(cbRemainCardData,cbRemainCardCount))
			{
				bOutBomb = true;
			}

			//ը������
			BYTE cbSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, BomCardType.cbEachHandCardData[cbCurIndex],BomCardType.cbEachHandCardCount[cbCurIndex]);
			if (cbSingleCardCount==0 && GetCardLogicValue(cbRemainCardData[0])>GetCardLogicValue(m_cbHandCardData[m_wBankerUser][0])) 
			{
				bOutBomb = true;
			}

			//ֻʣһ��
			BYTE cbRemainCardType = GetCardType(m_cbHandCardData[wOutCardUser],m_cbHandCardCount[wOutCardUser]);
			if (cbRemainCardType>CT_ERROR && cbRemainCardType<CT_FOUR_TAKE_ONE && GetCardLogicValue(m_cbHandCardData[wOutCardUser][0])<15 && 
				cbSingleCardCount<2 && (GetCardLogicValue(cbRemainCardData[0])>=GetCardLogicValue(m_cbHandCardData[m_wBankerUser][0])))
			{
				bOutBomb = true;
			}

			//ֻʣ������
			BYTE cbRemainLargestCard = GetCardLogicValue(cbRemainCardData[0]);
			if (cbRemainCardCount<5 && cbRemainCardCount>0 && (cbRemainLargestCard!=GetCardLogicValue(BomCardType.cbEachHandCardData[cbCurIndex][0])) && 
				cbRemainLargestCard>GetCardLogicValue(m_cbHandCardData[wOutCardUser][0]) && cbRemainLargestCard>14)
			{
				bOutBomb = true;
			}

			//�����˿�
			tagAnalyseResult AnalyseResult;
			AnalysebCardData(cbRemainCardData, cbRemainCardCount, AnalyseResult);
			if (m_cbHandCardCount[m_wBankerUser]==1 && AnalyseResult.cbDoubleCount*2+AnalyseResult.cbThreeCount*3+AnalyseResult.cbFourCount*4+1>=cbRemainCardCount) 
			{
				bOutBomb = true;
			}
			//���ñ���
			if (bOutBomb)
			{
				OutCardResult.cbCardCount = BomCardType.cbEachHandCardCount[cbCurIndex];
				CopyMemory(OutCardResult.cbCardData, BomCardType.cbEachHandCardData[cbCurIndex], BomCardType.cbEachHandCardCount[cbCurIndex]*sizeof(BYTE));
			}
			return;
		}
		return;
	}
	return;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////