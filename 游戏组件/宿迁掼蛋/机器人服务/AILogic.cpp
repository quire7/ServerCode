#include "StdAfx.h"
#include "AILogic.h"

//��������
const BYTE cbIndexCount=5;

//////////////////////////////////////////////////////////////////////////
//���캯��
CAILogic::CAILogic()
{
	//�߼�����
	m_cbMainValue = 2;
	m_wRoundOutTimes = 0x0700;
}

//��������
CAILogic::~CAILogic()
{

}

//��ȡ����
BYTE CAILogic::GetCardType(const BYTE cbCardData[], BYTE cbCardCount)
{
	ASSERT(cbCardCount<=28);
	if (cbCardCount>28) return AI_CT_ERROR;

	BYTE cbCardDataTemp[MAX_COUNT] = {0};
	CopyMemory(cbCardDataTemp, cbCardData, cbCardCount);
	//����
	SortCardList(cbCardDataTemp, cbCardCount, ST_ORDER);

	//������
	switch (cbCardCount)
	{
	case 0:	//����
		{
			return AI_CT_ERROR;
		}
	case 1: //����
		{
			return AI_CT_SINGLE;
		}
	case 2:	//����
		{
			return (GetCardValue(cbCardData[0])==GetCardValue(cbCardData[1])) ? AI_CT_DOUBLE : AI_CT_ERROR;
		}
	case 3:	//����
		{
			return (GetCardValue(cbCardData[0])==GetCardValue(cbCardData[2])) ? AI_CT_THREE : AI_CT_ERROR;
		}
	}

	//�����˿�
	tagAnalyseResult AnalyseResult;
	AnalysebCardData(cbCardData, cbCardCount, AnalyseResult);

	//ը������
	if (cbCardCount==4&&cbCardDataTemp[0]==0x4F&&cbCardDataTemp[3]==0x4E) return AI_CT_BOMB_TW;
	if (cbCardCount>=4&&cbCardCount<=10&&AnalyseResult.cbBlockCount[cbCardCount-1]==1) return AI_CT_BOMB;

	//˳������
	if (cbCardCount==5 && AnalyseResult.cbBlockCount[0]==cbCardCount)
	{
		//�˿�����
		BYTE cbSignedCount = AnalyseResult.cbBlockCount[0];
		BYTE cbCardColor = GetCardColor(AnalyseResult.cbCardData[0], cbSignedCount);
		bool bStructureLink = IsStructureLink(AnalyseResult.cbCardData[0], cbSignedCount, 1);

		//�����ж�
		if (bStructureLink&&cbCardColor==0xF0) return AI_CT_SINGLE_LINK;
		if (bStructureLink&&cbCardColor!=0xF0) return AI_CT_TONG_HUA_SHUN;
	}

	//��������
	if (cbCardCount==6 && AnalyseResult.cbBlockCount[1]*2==cbCardCount)
	{
		BYTE cbDoubleCount = AnalyseResult.cbBlockCount[1]*2;
		if (IsStructureLink(AnalyseResult.cbCardData[1], cbDoubleCount, 2)) return AI_CT_DOUBLE_LINK;
	}

	//��������
	if (cbCardCount==6 && AnalyseResult.cbBlockCount[2]*3==cbCardCount)
	{
		BYTE cbThreeCount = AnalyseResult.cbBlockCount[2]*3;
		if (IsStructureLink(AnalyseResult.cbCardData[2], cbThreeCount, 3)) return AI_CT_THREE_LINK;
	}

	//��������
	if (cbCardCount==5&&AnalyseResult.cbBlockCount[1]==1&&AnalyseResult.cbBlockCount[2]==1) return AI_CT_THREE_DOUBLE;

	return AI_CT_ERROR;
}

//��ȡ��ɫ
BYTE CAILogic::GetCardColor(const BYTE cbCardData[], BYTE cbCardCount)
{
	//Ч�����
	ASSERT(cbCardCount>0);
	if (cbCardCount==0) return 0xF0;

	//���ƻ�ɫ
	BYTE cbCardColor = GetCardColor(cbCardData[0]);
	//��ɫ�ж�
	for (BYTE i=0; i<cbCardCount; i++)
	{
		if (GetCardColor(cbCardData[i]) != cbCardColor)
		{
			return 0xF0;
		}
	}

	return cbCardColor;
}

//�����˿�
VOID CAILogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType)
{
	//�������
	if (cbCardCount==0) return;
	if (cbSortType==ST_CUSTOM) return;

	//ת����ֵ
	BYTE cbSortValue[MAX_COUNT] = {0};
	for (BYTE i=0; i<cbCardCount; i++)
	{
		switch (cbSortType)
		{
		case ST_COUNT:	//��Ŀ����
		case ST_ORDER:	//�ȼ�����
			{
				cbSortValue[i] = GetCardLogicValue(cbCardData[i]);
			}break;
		case ST_VALUE:	//��ֵ����
			{
				cbSortValue[i] = GetCardValue(cbCardData[i]);	
			}break;
		case ST_COLOR:	//��ɫ����
			{
				cbSortValue[i] = GetCardColor(cbCardData[i])+GetCardLogicValue(cbCardData[i]);
			}break;
		}
	}

	//�������
	bool bSorted = true;
	BYTE cbSwitchData=0, cbLast=cbCardCount-1;
	do
	{
		bSorted = true;
		for (BYTE i=0; i<cbLast; i++)
		{
			if (cbSortValue[i]<cbSortValue[i+1] || (cbSortValue[i]==cbSortValue[i+1]&&cbCardData[i]<cbCardData[i+1]))
			{
				//���ñ�־
				bSorted = false;

				//�˿�����
				cbSwitchData = cbCardData[i];
				cbCardData[i] = cbCardData[i+1];
				cbCardData[i+1] = cbSwitchData;

				//����Ȩλ
				cbSwitchData = cbSortValue[i];
				cbSortValue[i] = cbSortValue[i+1];
				cbSortValue[i+1] = cbSwitchData;
			}	
		}
		cbLast--;
	} while(bSorted==false);

	//��Ŀ����
	if (cbSortType == ST_COUNT)
	{
		//��������
		BYTE cbCardIndex = 0;

		//�����˿�
		tagAnalyseResult AnalyseResult;
		AnalysebCardData(cbCardData, cbCardCount, AnalyseResult);

		//��ȡ�˿�
		for (BYTE i=0; i<CountArray(AnalyseResult.cbBlockCount); i++)
		{
			//�����˿�
			BYTE cbIndex = CountArray(AnalyseResult.cbBlockCount)-i-1;
			CopyMemory(&cbCardData[cbCardIndex], AnalyseResult.cbCardData[cbIndex], AnalyseResult.cbBlockCount[cbIndex]*(cbIndex+1)*sizeof(BYTE));

			//��������
			cbCardIndex += AnalyseResult.cbBlockCount[cbIndex]*(cbIndex+1)*sizeof(BYTE);
		}
	}
	return;
}

//ɾ���˿�
bool CAILogic::RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount)
{
	//��������
	if (cbRemoveCount==0) return true;

	ASSERT(cbRemoveCount<=cbCardCount);
	if (cbRemoveCount>cbCardCount) return false;

	//�������
	BYTE cbDeleteCardCount=0, cbDeleteCardData[MAX_COUNT]={0};
	if (cbCardCount>CountArray(cbDeleteCardData)) return false;
	CopyMemory(cbDeleteCardData, cbCardData, cbCardCount*sizeof(cbCardData[0]));

	//�����˿�
	for (BYTE i=0; i<cbRemoveCount; i++)
	{
		for (BYTE j=0; j<cbCardCount; j++)
		{
			if (cbRemoveCard[i] == cbDeleteCardData[j])
			{
				cbDeleteCardCount++;
				cbDeleteCardData[j] = 0;
				break;
			}
		}
	}
	ASSERT(cbDeleteCardCount==cbRemoveCount);
	if (cbDeleteCardCount!=cbRemoveCount) return false;

	ZeroMemory(cbCardData, cbCardCount*sizeof(cbCardData[0]));
	//�����˿�
	for (BYTE i=0,cbCardPos=0; i<cbCardCount; i++)
	{
		if (cbDeleteCardData[i]>0x00 && cbDeleteCardData[i]<=0x4F) 
		{
			cbCardData[cbCardPos++]=cbDeleteCardData[i];
		}
	}

	return true;
}

//�߼���ֵ
BYTE CAILogic::GetCardLogicValue(BYTE cbCardData)
{
	//�˿�����
	BYTE cbCardValue = GetCardValue(cbCardData);
	//�����˿�
	if (cbCardValue==m_cbMainValue) return 15;

	//�߼���ֵ
	switch (cbCardValue)
	{
	case 0x01: { return 14; }
	case 0x0E: { return 16; }
	case 0x0F: { return 17; }
	}
	//ת����ֵ
	return cbCardValue;
}

//������Ʒ����˿�
VOID CAILogic::AnalyseMagicData(const BYTE cbHandCardData[], BYTE cbHandCardCount, tagMagicResult &MagicResult)
{
	ZeroMemory(&MagicResult, sizeof(MagicResult));
	//��֤
	if (cbHandCardCount<=0) return;

	for (BYTE i=0; i<cbHandCardCount; i++)
	{
		BYTE cbCardValue = GetCardValue(cbHandCardData[i]);
		BYTE cbCardColor = GetCardColor(cbHandCardData[i]);
		if (cbCardValue==m_cbMainValue && cbCardColor==0x20)
		{
			MagicResult.cbMagicCardData[MagicResult.cbMagicCardCount++] = cbHandCardData[i];
		}
		else
		{
			MagicResult.cbNormalCardData[MagicResult.cbNormalCardCount++] = cbHandCardData[i];
		}
	}
}

//�Ա��˿�
bool CAILogic::CompareCard(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount)
{
	//�����ж�
	BYTE cbNextType = GetCardType(cbNextCard, cbNextCount);
	BYTE cbFirstType = GetCardType(cbFirstCard, cbFirstCount);

	//ը���ж�
	if (cbFirstType>=AI_CT_TONG_HUA_SHUN || cbNextType>=AI_CT_TONG_HUA_SHUN)
	{
		//����ը��
		if (cbNextType==AI_CT_BOMB_TW) return true;
		if (cbFirstType==AI_CT_BOMB_TW) return false;

		//�ͼ�����
		if (cbNextType<AI_CT_TONG_HUA_SHUN) return false;
		if (cbFirstType<AI_CT_TONG_HUA_SHUN) return true;

		//��Ŀ�Ա�
		if (cbFirstCount==cbNextCount && cbFirstType==cbNextType)
		{
			if (cbFirstType==AI_CT_BOMB || cbFirstType==AI_CT_BOMB_TW)
			{
				return GetCardLogicValue(cbNextCard[0])>GetCardLogicValue(cbFirstCard[0]);
			}
			else
			{
				return CompareCardByValue(cbFirstCard, cbNextCard, cbFirstCount, cbNextCount);
			}
		}

		//��ͬ����
		if (cbFirstCount==cbNextCount && cbFirstType!=cbNextType)
		{
			if (cbNextType==AI_CT_TONG_HUA_SHUN) return true;
			if (cbFirstType==AI_CT_TONG_HUA_SHUN) return false;
			return false;
		}

		return cbNextCount>cbFirstCount;
	}

	//��ͬ����
	if (cbFirstType!=cbNextType) return false;

	//��ͬ����
	switch (cbFirstType)
	{
	case AI_CT_SINGLE:				//��������
	case AI_CT_DOUBLE:				//��������
	case AI_CT_THREE:				//��������
		{
			return GetCardLogicValue(cbNextCard[0])>GetCardLogicValue(cbFirstCard[0]);
		}
	case AI_CT_SINGLE_LINK:			//��������
	case AI_CT_DOUBLE_LINK:			//��������
	case AI_CT_THREE_LINK:			//��������
		{
			return CompareCardByValue(cbFirstCard, cbNextCard, cbFirstCount, cbNextCount);
		}
	case AI_CT_THREE_DOUBLE:		//��������
		{
			//�����˿�
			tagAnalyseResult AnalyseResultNext, AnalyseResultFirst;
			AnalysebCardData(cbNextCard, cbNextCount, AnalyseResultNext);
			AnalysebCardData(cbFirstCard, cbFirstCount, AnalyseResultFirst);
			return GetCardLogicValue(AnalyseResultNext.cbCardData[2][0])>GetCardLogicValue(AnalyseResultFirst.cbCardData[2][0]);
		}
	}

	//�������
	//ASSERT(FALSE);	
	return false;
}

//����˿�
bool CAILogic::MagicCardData(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbResultCard[MAX_COUNT])
{
	//��������
	BYTE cbNormalCardData[MAX_COUNT] = {0};
	BYTE cbNormalCardCount=0, cbMagicCardCount=0;

	//���׼��
	for (BYTE i=0; i<cbCardCount; i++)
	{
		//���ͳ��
		if (GetCardValue(cbCardData[i])==m_cbMainValue && GetCardColor(cbCardData[i])==0x20)
		{
			cbMagicCardCount++;
		}
		else 
		{
			cbNormalCardData[cbNormalCardCount++] = cbCardData[i];
		}
	}

	//ʧ���ж�
	if (cbMagicCardCount==0 || cbNormalCardCount==0)
	{
		CopyMemory(cbResultCard, cbCardData, cbCardCount*sizeof(BYTE));
		return false;
	}

	//��ȡ��ֵ
	BYTE cbCardLogicValue = GetCardLogicValue(cbNormalCardData[0]);
	//ͬ�Ʊ��
	if (cbCardLogicValue <= 15)
	{
		BYTE cbSameCount = 0;
		for (cbSameCount=1; cbSameCount<cbNormalCardCount; cbSameCount++)
		{
			if (GetCardLogicValue(cbNormalCardData[cbSameCount])!=cbCardLogicValue) break;
		}
		if (cbSameCount == cbNormalCardCount)
		{
			//���ý��
			for (BYTE j=0; j<cbMagicCardCount; j++)
			{
				cbResultCard[j] = cbNormalCardData[0];
			}
			for (BYTE j=0; j<cbNormalCardCount; j++)
			{
				cbResultCard[j+cbMagicCardCount] = cbNormalCardData[j];
			}
			return true;
		}
	}

	//�˿˷���
	BYTE cbMaxSameCount=1, cbSameCount=1;
	for (BYTE i=1; i<cbNormalCardCount; i++)
	{
		//��ȡ�˿�
		BYTE cbCardLogicValueTemp = GetCardLogicValue(cbNormalCardData[i]);
		if (cbCardLogicValueTemp==cbCardLogicValue) cbSameCount++;

		if (cbCardLogicValueTemp != cbCardLogicValue)
		{
			if (i == cbNormalCardCount-1)
			{
				cbMaxSameCount = __max(cbSameCount, cbMaxSameCount);
			}
			else
			{
				cbSameCount = 1;
				cbCardLogicValue = cbCardLogicValueTemp;
			}
		}
		else 
		{
			if (i == cbNormalCardCount-1)
			{
				cbMaxSameCount = __max(cbSameCount, cbMaxSameCount);
			}
		}
	}

	//��������
	if (cbCardCount==5 && cbMaxSameCount<=3)
	{
		//��������
		BYTE cbHeadCardCount=1, cbTailCardCount=1;
		//��������
		BYTE cbHeadCardValue = GetCardValue(cbNormalCardData[0]);
		BYTE cbTailCardValue = GetCardValue(cbNormalCardData[cbNormalCardCount-1]);
		//����ж�
		BYTE i = 0;
		for (i=1; i<cbNormalCardCount-1; i++)
		{
			//��ȡ��ֵ
			BYTE cbCardValue = GetCardValue(cbNormalCardData[i]);

			//�˿��ж�
			if (cbCardValue == cbHeadCardValue) 
			{
				cbHeadCardCount++;
				continue;
			}
			//�˿��ж�
			if (cbCardValue == cbTailCardValue) 
			{
				cbTailCardCount++;
				continue;
			}
			break;
		}

		//�ɹ��ж�
		if (i == cbNormalCardCount-1)
		{
			//�仯��ֵ
			BYTE cbMagicValue = 0;
			bool bSplitMagicCard = false;
			if (cbMagicCardCount == 1)
			{
				if (cbHeadCardCount==2 && cbTailCardCount==2)
				{
					if (cbHeadCardValue >= 0x0E)
					{
						cbMagicValue = cbTailCardValue;
					}
					else if (cbTailCardValue >= 0x0E)
					{
						cbMagicValue = cbHeadCardValue;
					}
					else
					{
						BYTE cbHeadLogicValue = GetCardLogicValue(cbNormalCardData[0]);
						BYTE cbTailLogicValue = GetCardLogicValue(cbNormalCardData[cbNormalCardCount - 1]);
						cbMagicValue = cbHeadLogicValue > cbTailLogicValue ? cbHeadCardValue : cbTailCardValue;
					}
				}
				else if (cbHeadCardCount==1 && cbTailCardCount==3)
				{
					cbMagicValue = cbHeadCardValue;
				}
				else if (cbHeadCardCount==3 && cbTailCardCount==1)
				{
					cbMagicValue = cbTailCardValue;
				}
			}
			else if (cbMagicCardCount == 2)
			{
				if (cbHeadCardCount==1 && cbTailCardCount==2)
				{
					if (cbTailCardValue >= 0x0E)
					{
						cbMagicValue = cbHeadCardValue;
					}
					else
					{
						BYTE cbHeadLogicValue = GetCardLogicValue(cbNormalCardData[0]);
						BYTE cbTailLogicValue = GetCardLogicValue(cbNormalCardData[cbNormalCardCount - 1]);
						if (cbHeadLogicValue > cbTailLogicValue)
						{
							cbMagicValue = cbHeadCardValue;
						}
						else
						{
							bSplitMagicCard = true;
						}
					}
				}
				else if (cbHeadCardCount==2 && cbTailCardCount==1)
				{
					if (cbHeadCardValue >= 0x0E)
					{
						cbMagicValue = cbTailCardValue;
					}
					else
					{
						BYTE cbHeadLogicValue = GetCardLogicValue(cbNormalCardData[0]);
						BYTE cbTailLogicValue = GetCardLogicValue(cbNormalCardData[cbNormalCardCount - 1]);
						if (cbHeadLogicValue > cbTailLogicValue)
						{
							bSplitMagicCard = true;
						}
						else
						{
							cbMagicValue = cbTailCardValue;
						}
					}
				}
			}

			CopyMemory(cbResultCard, cbNormalCardData, cbNormalCardCount*sizeof(BYTE));
			if (bSplitMagicCard)
			{
				cbResultCard[cbNormalCardCount++] = MakeCardData(cbHeadCardValue-1, 0);
				cbResultCard[cbNormalCardCount++] = MakeCardData(cbTailCardValue-1, 0);
			}
			else
			{				
				for (BYTE j=0; j<cbMagicCardCount; j++)
				{
					cbResultCard[cbNormalCardCount+j] = MakeCardData(cbMagicValue-1, 0);
				}
			}
			SortCardList(cbResultCard, cbCardCount, ST_COUNT);
			return true;
		}
	}

	//�����ж�
	bool bMagicLink = false;
	if (cbMaxSameCount==1 && cbCardCount==5)
	{
		bMagicLink = true;
	}
	if ((cbMaxSameCount==2||cbMaxSameCount==3) && cbCardCount==6) 
	{
		bMagicLink = true;
	}

	//���Ʊ��
	if (bMagicLink)
	{
		//�����ֲ�
		tagDistributing Distributing;
		AnalysebDistributing(cbNormalCardData, cbNormalCardCount, Distributing);

		//������
		for (BYTE cbTimes=0; cbTimes<2; cbTimes++)
		{
			//��������
			BYTE cbFillCount = 0;
			BYTE cbLeaveCount = cbNormalCardCount;
			BYTE cbUseableCount = cbMagicCardCount;

			//���״̬
			BYTE cbFillStatus[13] = {0};
			//����˿�
			for (BYTE i=0; i<14; i++)
			{
				//��������
				BYTE cbIndex = ((cbTimes==1)?(13-i)%13:i)%13;

				//����ж�
				if (cbLeaveCount!=cbNormalCardCount || Distributing.cbDistributing[cbIndex][cbIndexCount]!=0)
				{
					//ʧ���ж�
					if (Distributing.cbDistributing[cbIndex][cbIndexCount]+cbUseableCount < cbMaxSameCount) break;

					//��������
					BYTE cbTurnFillCount = 0;
					//�����˿�
					for (BYTE j=0; j<4; j++)
					{
						for (BYTE k=0; k<Distributing.cbDistributing[cbIndex][j]; k++)
						{
							cbLeaveCount--;
							cbTurnFillCount++;
							cbResultCard[cbFillCount++] = MakeCardData(cbIndex, j);
						}
					}

					//�����
					for (BYTE i=cbTurnFillCount; i<cbMaxSameCount; i++)
					{
						cbUseableCount--;
						cbResultCard[cbFillCount++] = MakeCardData(cbIndex, GetCardColor(cbNormalCardData[0])>>4);
					}

					//���ñ���
					cbFillStatus[cbIndex] = cbMaxSameCount;
				}
			}

			//�ɹ��ж�
			if (cbUseableCount==0 && cbLeaveCount==0)
			{
				SortCardList(cbResultCard, cbCardCount, ST_ORDER);
				return true;
			}
		}
	}

	//�����˿�
	CopyMemory(cbResultCard, cbCardData, cbCardCount*sizeof(BYTE));
	return false;
}

//�����˿�
VOID CAILogic::AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult &AnalyseResult)
{
	//���ý��
	ZeroMemory(&AnalyseResult, sizeof(AnalyseResult));

	//�˿˷���
	for (BYTE i=0; i<cbCardCount; i++)
	{
		//��������
		BYTE cbSameCount = 1;
		BYTE cbLogicValue = GetCardValue(cbCardData[i]);
		//����ͬ��
		for (BYTE j=i+1; j<cbCardCount; j++)
		{
			//��ȡ�˿�
			if (GetCardValue(cbCardData[j])!=cbLogicValue) break;

			//���ñ���
			cbSameCount++;
		}

		//���ý��
		BYTE cbIndex = AnalyseResult.cbBlockCount[cbSameCount-1]++;
		for (BYTE j=0; j<cbSameCount; j++)
		{
			AnalyseResult.cbCardData[cbSameCount-1][cbIndex*cbSameCount+j] = cbCardData[i+j];
		}

		//��������
		i += cbSameCount-1;
	}
}

//�����ֲ�
VOID CAILogic::AnalysebDistributing(const BYTE cbCardData[], BYTE cbCardCount, tagDistributing &Distributing)
{
	//���ñ���
	ZeroMemory(&Distributing, sizeof(Distributing));

	//���ñ���
	for (BYTE i=0; i<cbCardCount; i++)
	{
		if (cbCardData[i]==0) continue;

		//��ȡ����
		BYTE cbCardColor = GetCardColor(cbCardData[i]);
		BYTE cbCardValue = GetCardValue(cbCardData[i]);

		//�ֲ���Ϣ
		Distributing.cbCardCount++;
		Distributing.cbDistributing[cbCardValue-1][cbIndexCount]++;
		Distributing.cbDistributing[cbCardValue-1][cbCardColor>>4]++;
	}
}

//�����˿�
BYTE CAILogic::MakeCardData(BYTE cbValueIndex, BYTE cbColorIndex)
{
	return (cbColorIndex<<4)|(cbValueIndex+1);
}

//�Ƿ�����
bool CAILogic::IsStructureLink(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbCellCount)
{
	//��Ŀ�ж�
	ASSERT((cbCardCount%cbCellCount)==0);
	if ((cbCardCount%cbCellCount)!=0) return false;
	
	//�����˿�
	BYTE cbCardDataTemp[MAX_COUNT] = {0};
	CopyMemory(cbCardDataTemp, cbCardData, cbCardCount*sizeof(BYTE));
	//�˿�����
	SortCardList(cbCardDataTemp, cbCardCount, ST_VALUE);

	//��������
	BYTE cbBlockCount = cbCardCount/cbCellCount;
	BYTE cbFirstValue = GetCardValue(cbCardDataTemp[0]);

	//��Ч����
	if (cbFirstValue>=14) return false;

	//�˿�����
	for (BYTE i=1; i<cbBlockCount; i++)
	{
		//�˿���ֵ
		BYTE cbCardValue = GetCardValue(cbCardDataTemp[i*cbCellCount]);

		//�������
		if (cbCardValue==1&&cbFirstValue==13) continue;

		//�����ж�
		if (cbFirstValue!=cbCardValue+i) return false;
	}

	//����ж�
	return true;
}

//�Ա��˿�
bool CAILogic::CompareCardByValue(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount)
{
	//��������
	bool bHaveTwoNext = false;
	BYTE cbConsultNext[2] = {0x00,0x00};

	//�����˿�
	for (BYTE i=0; i<cbNextCount; i++)
	{
		//��ȡ��ֵ
		BYTE cbConsultValue = GetCardValue(cbNextCard[i]);

		//���ñ���
		if (!bHaveTwoNext&&(cbConsultValue==0x02)) bHaveTwoNext=true;

		//�����˿�
		if (cbConsultValue == 0x01)
		{
			if (14>cbConsultNext[0]) cbConsultNext[0] = 14;
			if (cbConsultValue>cbConsultNext[1]) cbConsultNext[1] = cbConsultValue;
		}
		else
		{
			if (cbConsultValue>cbConsultNext[0]) cbConsultNext[0] = cbConsultValue;
			if (cbConsultValue>cbConsultNext[1]) cbConsultNext[1] = cbConsultValue;
		}
	}

	//�������� 
	bool bHaveTwoFirst = false;
	BYTE cbConsultFirst[2] = {0x00,0x00};

	//�����˿�
	for (BYTE i=0; i<cbFirstCount; i++)
	{
		//��ȡ��ֵ
		BYTE cbConsultValue = GetCardValue(cbFirstCard[i]);

		//���ñ���
		if (!bHaveTwoFirst&&(cbConsultValue==0x02)) bHaveTwoFirst = true;

		//�����˿�
		if (cbConsultValue == 0x01)
		{
			if (14>cbConsultFirst[0]) cbConsultFirst[0] = 14;
			if (cbConsultValue>cbConsultFirst[1]) cbConsultFirst[1] = cbConsultValue;
		}
		else
		{
			if (cbConsultValue>cbConsultFirst[0]) cbConsultFirst[0] = cbConsultValue;
			if (cbConsultValue>cbConsultFirst[1]) cbConsultFirst[1] = cbConsultValue;
		}
	}

	//�Ա��˿�
	BYTE cbResultNext = (bHaveTwoNext==false)?cbConsultNext[0]:cbConsultNext[1];
	BYTE cbResultFirst = (bHaveTwoFirst==false)?cbConsultFirst[0]:cbConsultFirst[1];

	return cbResultNext>cbResultFirst;
}

//���������û�������Ϣ
void CAILogic::OnHandCardChange(const BYTE cbAllUserHandCardData[][MAX_COUNT], const BYTE cbAllUserHandCardCount[])
{
	ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));
	CopyMemory(m_cbHandCardCount, cbAllUserHandCardCount, sizeof(m_cbHandCardCount));
	CopyMemory(m_cbHandCardData[0], cbAllUserHandCardData[0], m_cbHandCardCount[0]);
	CopyMemory(m_cbHandCardData[1], cbAllUserHandCardData[1], m_cbHandCardCount[1]);
	CopyMemory(m_cbHandCardData[2], cbAllUserHandCardData[2], m_cbHandCardCount[2]);
	CopyMemory(m_cbHandCardData[3], cbAllUserHandCardData[3], m_cbHandCardCount[3]);
}

//�����û�����
void CAILogic::SetUserHandCard(WORD wUserID, const BYTE cbUserCardData[], BYTE cbUserCardCount)
{
	ZeroMemory(m_cbHandCardData[wUserID], MAX_COUNT);
	CopyMemory(m_cbHandCardData[wUserID], cbUserCardData, cbUserCardCount);
	m_cbHandCardCount[wUserID] = cbUserCardCount;
}

//�ж�һ���û��Ƿ���Σ���û�
bool CAILogic::IsDangerous(WORD wUserChairID)
{
	if (m_cbHandCardCount[wUserChairID] == 0)
	{
		return false;
	}
	if (m_cbHandCardCount[wUserChairID] <= 6)
	{
		return true;
	}
	tagOutCardTypeResult OutCardTypeResult[TYPE_COUNT];
	ZeroMemory(OutCardTypeResult, sizeof(OutCardTypeResult));
	AnalyseOutCardType(m_cbHandCardData[wUserChairID], m_cbHandCardCount[wUserChairID], OutCardTypeResult);

	BYTE cbNormalCardCount = 0;
	BYTE cbBobmCardCount=0, cbBobmCardType=AI_CT_ERROR;
	for (int i=1; i<TYPE_COUNT; i++)
	{
		if (i > 7)
		{
			if (OutCardTypeResult[i].cbCardTypeCount > 0)
			{
				cbBobmCardCount += OutCardTypeResult[i].cbCardTypeCount;
				cbBobmCardType = i;
			}
		}
		else
		{
			if (OutCardTypeResult[i].cbCardTypeCount > 0)
			{
				cbNormalCardCount += OutCardTypeResult[i].cbCardTypeCount;
			}
		}
	}
	//��ʣһ��ը����һ����
	if (cbBobmCardCount==1 && cbNormalCardCount>0)
	{
		BYTE cbHandCardDataElemData[MAX_COUNT] = {0};
		BYTE cbHandCatdDataElemCount = m_cbHandCardCount[wUserChairID];
		CopyMemory(cbHandCardDataElemData, m_cbHandCardData[wUserChairID], m_cbHandCardCount[wUserChairID]);
		RemoveCard(OutCardTypeResult[cbBobmCardType].cbEachHandCardData[0], OutCardTypeResult[cbBobmCardType].cbEachHandCardCount[0], cbHandCardDataElemData, cbHandCatdDataElemCount);
		if (GetMagicCardType(cbHandCardDataElemData,cbHandCatdDataElemCount-OutCardTypeResult[cbBobmCardType].cbEachHandCardCount[0]) != AI_CT_ERROR)
		{
			return true;
		}
	}
	return false;
}

//�ų������е�ը�������Ƶ�����
BYTE CAILogic::GetAfterRemoveBombsCardType(const BYTE cbHandCardData[], const BYTE cbHandCardCount)
{
	BYTE cbCopyCardData[MAX_COUNT]={0}, cbCopyCardCount=cbHandCardCount;
	CopyMemory(cbCopyCardData, cbHandCardData, cbHandCardCount);

	tagOutCardTypeResult CardTypeResult[TYPE_COUNT];
	ZeroMemory(&CardTypeResult, sizeof(CardTypeResult));
	AnalyseOutCardType(cbHandCardData, cbHandCardCount, CardTypeResult);

	BYTE cbBobmCardCount = 0;
	for (int i=1; i<TYPE_COUNT; i++)
	{
		if (i>=AI_CT_TONG_HUA_SHUN && CardTypeResult[i].cbCardTypeCount>0)
		{
			for (int j=0; j<CardTypeResult[i].cbCardTypeCount; j++)
			{
				RemoveCard(CardTypeResult[i].cbEachHandCardData[j], CardTypeResult[i].cbEachHandCardCount[j], cbCopyCardData, cbCopyCardCount);
				cbCopyCardCount -= CardTypeResult[i].cbEachHandCardCount[j];
			}
		}	
	}

	if (cbCopyCardCount > 0)
	{
		return GetMagicCardType(cbCopyCardData, cbCopyCardCount);
	}
	return AI_CT_BOMB;
}

//����ͬ��˳
bool CAILogic::SearchTongHuaShun(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, tagOutCardResult &OutCardResult)
{
	//A�ⶥ,ֱ�ӷ���
	if (GetCardValue(cbReferCard)==1) return false;
	if (cbHandCardCount<5) return false;

	//�����˿�
	tagDistributing Distribute;
	AnalysebDistributing(cbHandCardData, cbHandCardCount, Distribute);

	//ȷ��������ʼλ��
	BYTE cbReferIndex = (cbReferCard!=0) ? GetCardValue(cbReferCard)-4 : 0;
	BYTE cbColorCount[4] = {0,0,0,0};
	//��������ͬ��˳
	for (BYTE i=cbReferIndex; i<13; i++)
	{
		if (Distribute.cbDistributing[i][cbIndexCount] == 0) 
		{
			ZeroMemory(cbColorCount, sizeof(cbColorCount));
			continue;
		}

		for (BYTE j=0; j<4; j++)
		{
			if (Distribute.cbDistributing[i][j] == 0)
			{
				cbColorCount[j] = 0;
				continue;
			}
			//�ж��Ƿ������㹻
			if (++cbColorCount[j] == 5)
			{
				OutCardResult.cbCardCount = 0;
				for (int k=i; k>=i-4; k--)
				{
					OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = MakeCardData(k,j);
				}
				SortCardList(OutCardResult.cbResultCard,OutCardResult.cbCardCount,ST_VALUE);
				if (GetCardValue(OutCardResult.cbResultCard[0]) == 12)
				{
					SortCardList(OutCardResult.cbResultCard, OutCardResult.cbCardCount, ST_ORDER);
				}
				return true;
			}
		}
	}
	//����10��A��ͬ��˳
	for (BYTE i=0; i<4; i++)
	{
		//�����˳
		if (cbColorCount[i]==4 && Distribute.cbDistributing[0][i]>0)
		{
			OutCardResult.cbCardCount = 1;
			OutCardResult.cbResultCard[0] = MakeCardData(0, i);
			for (BYTE j=12; j>=9; j--)
			{
				OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = MakeCardData(j,i);
			}
			return true;
		}
	}
	return false;
}

//�������ͬ��˳
bool CAILogic::SearchMagicTongHuaShun( const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, tagOutCardResult &OutCardResult )
{
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));

	//A�ⶥ,ֱ�ӷ���
	if (GetCardValue(cbReferCard)==1) return false;
	if (cbHandCardCount<5) return false;

	//�����˿�
	tagMagicResult MagicResult;
	AnalyseMagicData(cbHandCardData, cbHandCardCount, MagicResult);
	if (MagicResult.cbMagicCardCount==0) return false;

	//�����˿�
	tagDistributing Distribute;
	AnalysebDistributing(MagicResult.cbNormalCardData, MagicResult.cbNormalCardCount, Distribute);

	//ȷ����ʼ����λ��
	BYTE cbReferIndex = (cbReferCard!=0) ? GetCardValue(cbReferCard)-4 : 0;
	BYTE cbStartPsition=0, cbStartColor=0;
	bool bFind = false;
	for (BYTE cbMagicCount=1; cbMagicCount<=MagicResult.cbMagicCardCount; cbMagicCount++)
	{
		for (BYTE i=0; i<4; i++)//��ɫ
		{
			for (int j=cbReferIndex; j<10; j++)//
			{
				BYTE cbInvalidCount = 0;	//ȱλ��
				BYTE cbLianCount = 0;		//������
				for (int k=0; k<5; k++)
				{
					if (Distribute.cbDistributing[j+k][cbIndexCount]==0 || Distribute.cbDistributing[j+k][i]==0)
					{
						if (k==0 && j!= 9) break; //�����һ��(����jQKA�����)���ǿյľ�����
						cbInvalidCount++;
					}
					else 
					{
						cbLianCount++;
					}
					if (cbInvalidCount>cbMagicCount) break;
				}
				if (cbLianCount == 5-cbMagicCount)
				{
					cbStartPsition = j;
					cbStartColor = i;
					bFind = true;
					break;
				}
			}
		}

		if (bFind)  break;
	}

	if (bFind)
	{
		BYTE k = cbStartPsition;
		BYTE cbCount = 0;
		BYTE cbMagicIndex = 0;
		do
		{
			if (Distribute.cbDistributing[k][cbIndexCount]>0 && Distribute.cbDistributing[k][cbStartColor]>0)
			{
				OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = MakeCardData(k, cbStartColor);
			}
			else
			{
				OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = MagicResult.cbMagicCardData[cbMagicIndex++];
			}
			k = (k+1)%13;
		}while(++cbCount < 5);
		//����
		return true;
	}
	return false;
}

//���ڵĻ�ɫ����
bool CAILogic::NearbyColorCount(BYTE cbHandCardData[MAX_COUNT], BYTE cbHandCardCount, BYTE cbCardData, BYTE cbColorCount[4])
{
	//���������
	if (cbCardData==0x4E || cbCardData==0x4F) return false;

	//��ʼ������
	ZeroMemory(cbColorCount, sizeof(BYTE)*4);

	BYTE cbCardDataTemp[MAX_COUNT] = {0};
	BYTE cbCardCountTemp = cbHandCardCount;
	CopyMemory(cbCardDataTemp, cbHandCardData, cbCardCountTemp);
	SortCardList(cbCardDataTemp,cbCardCountTemp,ST_VALUE);

	BYTE cbCardValue = GetCardValue(cbCardData);
	BYTE cbMinCardValue = (cbCardValue-5<1) ? 1 : (cbCardValue-5);
	BYTE cbMaxCardValue = (cbCardValue+5>14) ? 14 : (cbCardValue+5);

	for (BYTE i=0; i<cbHandCardCount; i++)
	{
		if (cbCardDataTemp[i]==0x4E||cbCardDataTemp[i]==0x4F) continue;

		BYTE cbTempColor = GetCardColor(cbCardDataTemp[i]);
		BYTE cbTempValue = GetCardValue(cbCardDataTemp[i]);

		if (cbTempValue>cbMinCardValue && cbTempValue<cbMaxCardValue && cbTempValue!=cbCardValue)
		{
			cbColorCount[cbTempColor>>4]++;
		}
	}

	//�����A�д�˳
	if (cbCardValue == 1)
	{
		cbCardValue = 14;
		BYTE cbMinCardValue = 9;
		BYTE cbMaxCardValue = 14;
		for (BYTE i=0; i<cbHandCardCount; i++)
		{
			BYTE cbTempColor = GetCardColor(cbCardDataTemp[i]);
			BYTE cbTempValue = GetCardValue(cbCardDataTemp[i]);

			if (cbTempValue>=cbMinCardValue && cbTempValue<=cbMaxCardValue && cbTempValue!=cbCardValue)
			{
				cbColorCount[cbTempColor>>4]++;
			}
		}
	}
	return true;
}

//��������
void CAILogic::AnalyseOutCardType(BYTE const cbHandCardData[], BYTE const cbHandCardCount, tagOutCardTypeResult CardTypeResult[TYPE_COUNT])
{
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult[0])*TYPE_COUNT);

	BYTE cbCardDataTemp[MAX_COUNT] = {0};
	BYTE cbCardCountTemp = cbHandCardCount;
	CopyMemory(cbCardDataTemp, cbHandCardData, cbHandCardCount);
	SortCardList(cbCardDataTemp, cbHandCardCount, ST_ORDER);

	//ͬ��˳����
	tagOutCardTypeResult CardTongHuaShunResult;
	ZeroMemory(&CardTongHuaShunResult, sizeof(CardTongHuaShunResult));
	if (AnalyseTongHuaShun(cbCardDataTemp, cbCardCountTemp, CardTongHuaShunResult))
	{
		ASSERT(CardTongHuaShunResult.cbCardTypeCount > 0);
		for (BYTE i=0; i<CardTongHuaShunResult.cbCardTypeCount; i++)
		{
			RemoveCard(CardTongHuaShunResult.cbEachHandCardData[i], CardTongHuaShunResult.cbEachHandCardCount[i], cbCardDataTemp, cbCardCountTemp);
			cbCardCountTemp -= CardTongHuaShunResult.cbEachHandCardCount[i];
		}

 		CopyMemory(&CardTypeResult[AI_CT_TONG_HUA_SHUN], &CardTongHuaShunResult, sizeof(CardTongHuaShunResult));
	}
	
	//��ͨ���ͷ���
	tagOutCardTypeResult CardTypeResultNomal[TYPE_COUNT];
	AnalyseOutCardTypeNomal(cbCardDataTemp, cbCardCountTemp, CardTypeResultNomal);

	//����ͬ��˳
	if (CardTypeResultNomal[AI_CT_TONG_HUA_SHUN].cbCardTypeCount > 0)
	{
		for (BYTE i=0; i<CardTypeResultNomal[AI_CT_TONG_HUA_SHUN].cbCardTypeCount; i++)
		{
			VERIFY(GetMagicCardType(CardTypeResultNomal[AI_CT_TONG_HUA_SHUN].cbEachHandCardData[i],CardTypeResultNomal[AI_CT_TONG_HUA_SHUN].cbEachHandCardCount[i]) == AI_CT_TONG_HUA_SHUN);
			BYTE cbIndex = CardTypeResult[AI_CT_TONG_HUA_SHUN].cbCardTypeCount;
			CopyMemory(CardTypeResult[AI_CT_TONG_HUA_SHUN].cbEachHandCardData[cbIndex],CardTypeResultNomal[AI_CT_TONG_HUA_SHUN].cbEachHandCardData[i],CardTypeResultNomal[AI_CT_TONG_HUA_SHUN].cbEachHandCardCount[i]);			
		    CardTypeResult[AI_CT_TONG_HUA_SHUN].cbEachHandCardCount[cbIndex] = CardTypeResultNomal[AI_CT_TONG_HUA_SHUN].cbEachHandCardCount[i];
			CardTypeResult[AI_CT_TONG_HUA_SHUN].cbCardTypeCount++;
		}
	}
	for (BYTE i=AI_CT_SINGLE; i<=AI_CT_BOMB_TW; i++)
	{
		if (i == AI_CT_TONG_HUA_SHUN) continue;
		CopyMemory(&CardTypeResult[i], &CardTypeResultNomal[i], sizeof(CardTypeResultNomal[i]));
	}

	//��֤���
	for (BYTE cbType=AI_CT_SINGLE; cbType<=AI_CT_BOMB_TW; cbType++)
	{
		if (CardTypeResult[cbType].cbCardTypeCount > 0)
		{
			for (BYTE i=0; i<CardTypeResult[cbType].cbCardTypeCount; i++)
			{
				//��֤
				CheckOutCardRightType(CardTypeResult[cbType].cbEachHandCardData[i], CardTypeResult[cbType].cbEachHandCardCount[i], cbType);
			}
		}
	}
}

//���ó�ʼֵ
void CAILogic::SetInitData()
{
	//ʣ�����
	m_iOutCardCount[0] = 2;
	for (BYTE i=1; i<=MAX_LOGIC_VALUE; i++)
	{
		if (i == m_cbMainValue)
		{
			m_iOutCardCount[i] = 0;
		}
		else if (i == 15)
		{
			m_iOutCardCount[i] = 6;
		}
		else if (i==18 || i==17)
		{
			m_iOutCardCount[i] = 2;
		}
		else
		{
			m_iOutCardCount[i] = 8;
		}
	}

	//��С��������
	memset(m_cbKindCount, 0, sizeof(m_cbKindCount));
	//�������ͼ�¼
	memset(m_PlayerTypeImfo, 0, sizeof(m_PlayerTypeImfo));
	//���Ƽ�¼��Ϣ
	m_ForOutImfo.cbOutCardCount = 0;
	ZeroMemory(m_ForOutImfo.cbOutCardData, sizeof(m_ForOutImfo.cbOutCardData));
	m_ForOutImfo.wOutChairID = INVALID_CHAIR;
	m_CurPlayerOutTimes.cbCurPlaOutCount = 0;
	m_CurPlayerOutTimes.cbEnemyOutCount = 0;
	m_CurPlayerOutTimes.wCurrentChairID = INVALID_CHAIR;
	m_CurPlayerOutTimes.cbEnemyFirstCount = 0;
	//�״γ�����Ҽ�¼
	m_bFirstOutCard = false;

	//�����������
	memset(&m_PlayerBestOutType, 0, sizeof(m_PlayerBestOutType));
	//ʣ���˿���Ŀ
	memset(m_cbCardNum, (MAX_COUNT-1), sizeof(m_cbCardNum));
}

//��¼ʣ�����
void CAILogic::RecordOutCard(BYTE cbOutCard[], BYTE cbOutCount)
{
	for (BYTE i=0; i<cbOutCount; i++)
	{
		BYTE cbLogicCardValue = GetCardLogicValue(cbOutCard[i]);
		BYTE cbCardValue = GetCardValue(cbOutCard[i]);
		if (cbCardValue==m_cbMainValue && GetCardColor(cbOutCard[i])==0x20)
		{
			m_iOutCardCount[0]--;
		}
		else
		{
			m_iOutCardCount[cbLogicCardValue]--;
		}

		ASSERT(m_iOutCardCount[cbLogicCardValue]>=0 && m_iOutCardCount[0]>=0);
	}
}

//��¼��С��(������Ҳ�Ҫ��)
void CAILogic::RecordKindCard(WORD wMeChairID, BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wPassaChairID, WORD wOutChairID, BYTE cbTurnCardData[], BYTE cbTurnCardCount, PlayerOutImfomation ForOutImfo)
{
	//���Ƽ�¼
	//�Ѿ�û�д�С��
	if (m_iOutCardCount[17]==0 || m_cbKindCount[wMeChairID][1]==m_iOutCardCount[17])
	{
		for (BYTE i=0; i<GAME_PLAYER; i++)
		{
			if (i==wMeChairID) continue;
			m_cbKindCount[i][0] = 0;
		}
	}
	else if (m_iOutCardCount[18]==0 || m_cbKindCount[wMeChairID][2]==m_iOutCardCount[18])
	{
		for (BYTE i=0; i<GAME_PLAYER; i++)
		{
			if (i==wMeChairID) continue;
			m_cbKindCount[i][1] = 0;
		}
	}
	//ȥ����С��
	if (wPassaChairID == INVALID_CHAIR)
	{
		ASSERT(wOutChairID != INVALID_CHAIR);
		BYTE cbCardData[MAX_COUNT]={0};
		CopyMemory(cbCardData, cbTurnCardData, cbTurnCardCount);
		SortCardList(cbCardData, cbTurnCardCount, ST_ORDER);
		for (BYTE i=0; i<cbTurnCardCount; i++)
		{
			BYTE cbLogicValue = GetCardLogicValue(cbCardData[i]);
			//�ų����Ǵ�С������
			if (cbLogicValue<17) break;
				
			if (m_cbKindCount[wOutChairID][cbLogicValue-17] > 0)
			{
				m_cbKindCount[wOutChairID][cbLogicValue-17] --;
			}
		}
	}
	//���Ž��д�С�����ж�
	BYTE cbType = (wPassaChairID==INVALID_CHAIR) ? GetCardType(cbTurnCardData,cbTurnCardCount) : GetCardType(m_ForOutImfo.cbOutCardData,m_ForOutImfo.cbOutCardCount);
	if (cbType != AI_CT_SINGLE)  return;

	BYTE cbOutLogicValue = GetCardLogicValue(ForOutImfo.cbOutCardData[0]);	
	if (cbOutLogicValue==15 || cbOutLogicValue==17)
	{
		//����
		if (wPassaChairID == INVALID_CHAIR)
		{	
			int iCount = (int)(m_iOutCardCount[17] - m_cbKindCount[wMeChairID][0]);
			if (cbOutLogicValue==15 && iCount==1 && cbTurnCardData[0]==0x4F)
			{
				//�����ƣ��Ӵ�����û��С�����ڣ�С��һ�ţ�����		
				m_cbKindCount[wOutChairID][0] = 0;

				if (ForOutImfo.wOutChairID==(wMeChairID+2)%GAME_PLAYER || ForOutImfo.wOutChairID==wMeChairID)
				{
					//���������һ�����ˣ�С���ڶԼ����У�
					if (wOutChairID == (ForOutImfo.wOutChairID+3)%GAME_PLAYER)
					{
						m_cbKindCount[(wMeChairID+2)%GAME_PLAYER][0] = 1;
					}
				}					
			}
		}
		else //������
		{
			WORD wTempOutChairID = ForOutImfo.wOutChairID;
			BYTE cbIndex = (cbOutLogicValue==15) ? 17 : 18;
			int iCount = (int)(m_iOutCardCount[cbIndex] - m_cbKindCount[wMeChairID][cbIndex-17]);
			if (iCount > 0)
			{
				//�����ִ����Ѿ�ȷ��������Ҫ����
				if (cbOutLogicValue == 17)	return;

				// b�����ҳ����ƻ�����С������Ҫ�ĶԼ�û�д�/С���� 
				if (wTempOutChairID==(wMeChairID+2)%GAME_PLAYER || wTempOutChairID==wMeChairID)
				{ 
					//���˶���Ҫ����С���ڶԼ����У�
					if (wPassaChairID == (wTempOutChairID+3)%GAME_PLAYER)
					{
						m_cbKindCount[wPassaChairID][cbIndex] = 0;
						m_cbKindCount[(wMeChairID+2)%GAME_PLAYER][cbIndex] = (BYTE)iCount;

						//����ǳ�С��������Ҳû��
						if (cbIndex==17 && m_iOutCardCount[18]-m_cbKindCount[wMeChairID][1]>0)
						{
							m_cbKindCount[wPassaChairID][1] = 0;
						}
					}
				}
				else if (wPassaChairID == (wMeChairID+2)%GAME_PLAYER)
				{
				    //���˳������ƻ�����С������Ҫ�ı���û�д�/С��
					m_cbKindCount[wPassaChairID][cbIndex] = 0;
				}
			}
		}	
	}	
}

//��¼���Ƶ�����
void CAILogic::RecordOutCardType(WORD wOutChairID, BYTE cbTurnCardData[], BYTE cbTurnCardCount, bool bFirst, WORD wPassChairID, BYTE cbHandCardData[], BYTE cbHandCardCount, PlayerOutImfomation ForOutImfo)
{
	//ASSERT(bFirst && cbTurnCount != 0);
	BYTE cbTurnType = GetCardType(cbTurnCardData,cbTurnCardCount);
	BYTE cbForType = GetCardType(ForOutImfo.cbOutCardData,ForOutImfo.cbOutCardCount);

	//�����ը�����ý��м���
	if (cbForType==AI_CT_BOMB || cbForType==AI_CT_BOMB_TW || cbForType==AI_CT_TONG_HUA_SHUN)
	{
		return;
	}
	
	//�׳���
	if (bFirst)
	{
		//�����ը�����ý��м���
		if (cbTurnType==AI_CT_BOMB || cbTurnType==AI_CT_BOMB_TW || cbTurnType==AI_CT_TONG_HUA_SHUN)
		{
			return;
		}

		//���������Ǳ���ϲ��������
		if (!SearchType(cbTurnType,true,wOutChairID,false))
		{
			m_PlayerTypeImfo[wOutChairID].cbLikeCardType[m_PlayerTypeImfo[wOutChairID].cbLikeCardTypeCount] = cbTurnType;
			m_PlayerTypeImfo[wOutChairID].cbLikeCardTypeCount++;

			//�Ƴ���ϲ������
			SearchType(cbTurnType,false,wOutChairID,true);
		}
	}
	else if (wPassChairID!=INVALID_CHAIR || cbTurnType==AI_CT_BOMB || cbTurnType==AI_CT_BOMB_TW || cbTurnType==AI_CT_TONG_HUA_SHUN)
	{
		BYTE cbForTurnData[MAX_COUNT] = {0};
		BYTE cbForTurnCount = ForOutImfo.cbOutCardCount;
		CopyMemory(cbForTurnData, ForOutImfo.cbOutCardData, ForOutImfo.cbOutCardCount);
		WORD wForChairID = ForOutImfo.wOutChairID;
		WORD wTempOutChairID = wPassChairID !=INVALID_CHAIR ? wPassChairID : wOutChairID;

		//�ǳ����ߵĵ���
		if (wForChairID != (wTempOutChairID+2)%GAME_PLAYER)
		{
			if (cbForType==AI_CT_SINGLE || cbForType==AI_CT_DOUBLE || cbForType==AI_CT_THREE || cbForType==AI_CT_THREE_DOUBLE)
			{
				//������ͬһ���������Ļ������������ϣ�
				BYTE cbValue = GetTypeCardValue(cbForTurnData,cbForTurnCount);
				BYTE cbMaxValue = SearchMaxLogicValue(cbForType,cbHandCardData,cbHandCardCount);
			
				if (cbForType!=AI_CT_SINGLE && 15<cbMaxValue)
				{
					cbMaxValue = 15;
				}
				if (cbValue<cbMaxValue && cbMaxValue!=0)
				{
					//�Ƴ�ϲ������
					SearchType(cbForType,true,wTempOutChairID,true);	
					//��ӵ���ϲ������
					if (!SearchType(cbForType,false,wTempOutChairID,false))
					{
						m_PlayerTypeImfo[wTempOutChairID].cbUnLikeCardType[m_PlayerTypeImfo[wTempOutChairID].cbUnLikeCardTypeCount] = cbForType;
						m_PlayerTypeImfo[wTempOutChairID].cbUnLikeCardTypeCount ++;
					}
				}
			}
		}
	}
	else if (wPassChairID == INVALID_CHAIR)
	{
		//����(����ϲ��������ɾ��)
		//�Ƴ���ϲ������
		SearchType(cbTurnType,false,wOutChairID,true);
	}
}

//��������
bool CAILogic::SearchType(BYTE cbType, bool bLike, WORD wChairID, bool bMove)
{
    //������������
	if (cbType==AI_CT_ERROR) return true;
	if (m_cbCardNum[wChairID]==0) return false;

	if (bLike)
	{
		if (m_PlayerTypeImfo[wChairID].cbLikeCardTypeCount > 0)
		{
			for (BYTE i=0; i<m_PlayerTypeImfo[wChairID].cbLikeCardTypeCount; i++)
			{
				if (m_PlayerTypeImfo[wChairID].cbLikeCardType[i] == cbType)
				{
					//�Ƴ�����
					if (bMove)
					{
						for (BYTE j=i; j<m_PlayerTypeImfo[wChairID].cbLikeCardTypeCount-1; j++)
						{
							m_PlayerTypeImfo[wChairID].cbLikeCardType[j] = m_PlayerTypeImfo[wChairID].cbLikeCardType[j+1];
						}
						m_PlayerTypeImfo[wChairID].cbLikeCardTypeCount --;
					}
					return true;
				}
			}
		}
	}
	else
	{
		if (m_PlayerTypeImfo[wChairID].cbUnLikeCardTypeCount > 0)
		{
			for (BYTE i=0; i<m_PlayerTypeImfo[wChairID].cbUnLikeCardTypeCount; i++)
			{
				if (m_PlayerTypeImfo[wChairID].cbUnLikeCardType[i] == cbType)
				{
					//�Ƴ�����
					if (bMove)
					{
						for (BYTE j=i; j<m_PlayerTypeImfo[wChairID].cbUnLikeCardTypeCount-1; j++)
						{
							m_PlayerTypeImfo[wChairID].cbUnLikeCardType[j] = m_PlayerTypeImfo[wChairID].cbUnLikeCardType[j+1];

						}
						m_PlayerTypeImfo[wChairID].cbUnLikeCardTypeCount --;
					}
					return true;
				}
			}
		}
	}

	return false;
}

//���Ƽ�¼�ĺ����ܻ�
void CAILogic::SetOutCardImfomation(WORD wMeChairID, WORD wOutChairID, WORD wCurrentID, BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbTurnCardData[], BYTE cbTurnCardCount, bool bOutCard, bool bFirstOutCard)
{
	WORD wPassID = (!bOutCard) ? wOutChairID : INVALID_CHAIR;	
	//��¼�Ƿ����״γ�����	
	m_bFirstOutCard = bFirstOutCard;

	//��¼��
	if (bOutCard)
	{
		RecordOutCard(cbTurnCardData, cbTurnCardCount);
	}
	//���Ƽ�¼
	RecordKindCard(wMeChairID, cbHandCardData, cbHandCardCount, wPassID, wOutChairID, cbTurnCardData, cbTurnCardCount, m_ForOutImfo);
	//��¼����
	RecordOutCardType(wOutChairID, cbTurnCardData, cbTurnCardCount, m_bFirstOutCard, wPassID, cbHandCardData, cbHandCardCount, m_ForOutImfo);
	//��ȡ������
	GetBestCardType(wOutChairID, cbTurnCardData, cbTurnCardCount, wPassID);	

	//�������
	if (bOutCard)
	{	
		//ǰһ������Ҽ�¼
		m_ForOutImfo.wOutChairID = wOutChairID;
		m_ForOutImfo.cbOutCardCount = cbTurnCardCount;
		CopyMemory(m_ForOutImfo.cbOutCardData,cbTurnCardData,cbTurnCardCount);
	}
	if (m_bFirstOutCard)
	{
		//��ǰ����������ƴ���
		if (m_CurPlayerOutTimes.wCurrentChairID == wOutChairID)
		{
			m_CurPlayerOutTimes.cbCurPlaOutCount++;
		}
		else
		{
			m_CurPlayerOutTimes.cbCurPlaOutCount = 1;
		}
		//�����״��������ƵĴ���
		if (m_CurPlayerOutTimes.wCurrentChairID==wOutChairID || m_CurPlayerOutTimes.wCurrentChairID==(wOutChairID+2)%GAME_PLAYER)
		{
			m_CurPlayerOutTimes.cbEnemyFirstCount++;
		}
		else
		{
			m_CurPlayerOutTimes.cbEnemyFirstCount = 1;
		}
		m_CurPlayerOutTimes.wCurrentChairID = wOutChairID;

		//�������Ѿ��������������
		if (m_cbCardNum[wOutChairID] == 0)
		{
			m_CurPlayerOutTimes.cbCurPlaOutCount = 0;
			m_CurPlayerOutTimes.cbEnemyOutCount = 0;
			m_CurPlayerOutTimes.wCurrentChairID = INVALID_CHAIR;
			m_CurPlayerOutTimes.cbEnemyFirstCount = 0;
		}
	}

	//���˳��ƵĴ���
	if (bOutCard)
	{
		if (wOutChairID==(wMeChairID+1)%GAME_PLAYER || wOutChairID==(wMeChairID+3)%GAME_PLAYER)
		{
			m_CurPlayerOutTimes.cbEnemyOutCount++;
		}
		else
		{
			m_CurPlayerOutTimes.cbEnemyOutCount = 0;
		}
	}
}

//��ȡ������
void CAILogic::GetBestCardType(WORD wOutChairID, BYTE cbTurnCard[], BYTE cbTurnCount, WORD wPassID)
{
	//���Ƽ���ʣ���Ƶ�����
	if (wPassID == INVALID_CHAIR)
	{
		m_cbCardNum[wOutChairID] -= cbTurnCount;

		//�Ƿ�ӵ��������
		BYTE cbBestType = 0;
		if (m_cbCardNum[wOutChairID] == 1)
		{
			cbBestType = AI_CT_SINGLE;
		}
		else if (m_cbCardNum[wOutChairID] == 2)
		{
			cbBestType = AI_CT_DOUBLE;
		}
		else if (m_cbCardNum[wOutChairID] == 3)
		{
			cbBestType = AI_CT_THREE;
		}
		else if (m_cbCardNum[wOutChairID] == 5)
		{
			cbBestType = AI_CT_THREE_DOUBLE;
		}

		m_PlayerBestOutType.cbCardType[wOutChairID] = cbBestType;
		m_PlayerBestOutType.cbCardSafeValue[wOutChairID] = 0;
	}
	else
	{
		//�������޸�
		BYTE cbType = GetCardType(cbTurnCard,cbTurnCount);
		if (cbType==AI_CT_SINGLE || cbType==AI_CT_DOUBLE || cbType==AI_CT_THREE || cbType==AI_CT_THREE_DOUBLE)
		{
			if (m_PlayerBestOutType.cbCardType[wPassID] == cbType)
			{
				BYTE cbCardData[5] = {0};
				CopyMemory(cbCardData, cbTurnCard, cbTurnCount);
				SortCardList(cbCardData, cbTurnCount, ST_ORDER);
				BYTE cbValue = GetTypeCardValue(cbTurnCard,cbTurnCount);
				//�޸�
				if (m_PlayerBestOutType.cbCardSafeValue[0]==0 || cbValue<m_PlayerBestOutType.cbCardSafeValue[0])
				{
					m_PlayerBestOutType.cbCardSafeValue[0] = cbValue;
				}
			}
		}
	}	
}

//�ƶ�������ʣ��������߼�ֵ
BYTE CAILogic::SearchMaxLogicValue(BYTE cbType, BYTE cbHandCardData[], BYTE cbHandCardCount)
{
	BYTE cbCardCount = 0;
	switch (cbType)
	{
	case AI_CT_SINGLE:
		{
			cbCardCount = 1;
		}break;
	case AI_CT_DOUBLE:
		{
			cbCardCount = 2;
		}break;
	case AI_CT_THREE:
	case AI_CT_THREE_DOUBLE:
		{
			cbCardCount = 3;
		}break;
	}

	if (cbCardCount > 0)
	{
		//�����е��Ƶĸ���
		BYTE cbCount[MAX_LOGIC_VALUE+1] = {0};
		for (BYTE i=0; i<cbHandCardCount; i++)
		{
			BYTE cbLogicCardValue = GetCardLogicValue(cbHandCardData[i]);
			BYTE cbCardValue = GetCardValue(cbHandCardData[i]);
			if (cbCardValue==m_cbMainValue && GetCardColor(cbHandCardData[i])==0x20)
			{
				cbCount[0]++;
			}
			cbCount[cbLogicCardValue]++;
		}

		for (int i=MAX_LOGIC_VALUE; i>=MIN_LOGIC_VALUE; i--)
		{
			if (i==m_cbMainValue) continue;
			BYTE cbTemp = (BYTE)m_iOutCardCount[i];
			//�޳������Լ�ӵ�е���
			cbTemp -= cbCount[i];
			if (cbTemp >= cbCardCount)
			{
				return i;
			}
		}
	}

	return 0;
}

//��ȡ�����Ƶ��߼�ֵ
BYTE CAILogic::GetTypeCardValue(BYTE cbCardData[], BYTE cbCardCount)
{
	BYTE cbMagicData[MAX_COUNT] = {0};
	MagicCardData(cbCardData, cbCardCount, cbMagicData);

	BYTE cbCardType = GetCardType(cbMagicData, cbCardCount);
	if (cbCardType!=AI_CT_SINGLE && cbCardType!=AI_CT_DOUBLE && cbCardType!=AI_CT_THREE_DOUBLE && cbCardType!=AI_CT_THREE)
	{
		return 0;
	}

	BYTE cbValue = 0;
	if (cbCardType == AI_CT_THREE_DOUBLE)
	{
		//�Է����������������
		tagAnalyseResult AnalyseResult;
		AnalysebCardData(cbMagicData, cbCardCount, AnalyseResult);
		cbValue = GetCardLogicValue(AnalyseResult.cbCardData[2][0]);
	}
	else
	{
		cbValue = GetCardLogicValue(cbMagicData[0]);
	}

	return cbValue;
}

//��ȡ����
BYTE CAILogic::GetMagicCardType(const BYTE cbCardData[], BYTE cbCardCount)
{
	BYTE cbMagicData[MAX_COUNT] = {0};
	MagicCardData(cbCardData, cbCardCount, cbMagicData);
	return GetCardType(cbMagicData, cbCardCount);
}

//������ͱȽ�
bool CAILogic::CompareMagicCard(BYTE cbFirstCard[], BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount)
{
	BYTE cbFirstMagic[MAX_COUNT] = {0};
	BYTE cbNextMagic[MAX_COUNT] = {0};
	MagicCardData(cbFirstCard, cbFirstCount, cbFirstMagic);
	MagicCardData(cbNextCard, cbNextCount, cbNextMagic);

	return CompareCard(cbFirstMagic, cbNextMagic, cbFirstCount, cbNextCount);
}

//����
void CAILogic::SearchNextOutCard(WORD wMeChairID, WORD wOutChairID, const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult &OutCardResult)
{
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));

	//�������
	const WORD wUnderChairID = (wMeChairID+1)%GAME_PLAYER;
	const WORD wUpsideChairID = (wMeChairID+3)%GAME_PLAYER;
	const WORD wFriendChairID = (wMeChairID+2)%GAME_PLAYER;

	//��¼����
	if ((m_wRoundOutTimes>>8) == wOutChairID)
	{
		++m_wRoundOutTimes;
	}
	else
	{
		m_wRoundOutTimes = (wOutChairID<<8);
	}

	BYTE cbHandData[MAX_COUNT]={0}, cbTurnData[MAX_COUNT]={0};
	CopyMemory(cbHandData, cbHandCardData, cbHandCardCount);	
	CopyMemory(cbTurnData, cbTurnCardData, cbTurnCardCount);
	BYTE cbTurnType = GetMagicCardType(cbTurnData, cbTurnCardCount);
	ASSERT(cbTurnType!=AI_CT_ERROR);

	//����Σ�յȼ�
	bool bUnderDanger = IsDangerous((wMeChairID+1)%GAME_PLAYER);
	bool bUpsideDanger = IsDangerous((wMeChairID+3)%GAME_PLAYER);

	//1��һ���ܳ������
	BYTE cbHandCardType = GetMagicCardType(cbHandData,cbHandCardCount);
	if (cbHandCardType != AI_CT_ERROR)
	{
		if (CompareMagicCard(cbTurnData,cbHandData,cbTurnCardCount,cbHandCardCount))
		{
			bool bOutCard = false;
			//����ը�����ƣ�
			if (cbHandCardType < AI_CT_TONG_HUA_SHUN)
			{
				bOutCard = true;
			}
			//��ը��
			else
			{
				//���ѷ�������
				if (wOutChairID == wFriendChairID)
				{
					//�¼�Σ�գ���ը�����򲻳�
					if (bUnderDanger)
					{
						bOutCard = true;
					}
				}
				//���˳����ƣ�ը��Ѿ�� 
				else  
				{
					bOutCard = true;
				}
			}

			if (bOutCard)
			{
				OutCardResult.cbCardCount = cbHandCardCount;
				CopyMemory(OutCardResult.cbResultCard, cbHandData, cbHandCardCount);
				VERIFY(CheckWillOutCard(wOutChairID,wMeChairID,OutCardResult.cbResultCard,OutCardResult.cbCardCount,cbTurnData,cbTurnCardCount));
				return;
			}
		}
	}
	
	//����ѷ��ɽ������֣������ѷ�������ը�ӵģ�����
	if (m_cbHandCardCount[wUnderChairID]==0 && CompareMagicCard(cbTurnData,m_cbHandCardData[wFriendChairID],cbTurnCardCount,m_cbHandCardCount[wFriendChairID]))
	{
		BYTE cbFriendType = GetMagicCardType(m_cbHandCardData[wFriendChairID],m_cbHandCardCount[wFriendChairID]);
		if (cbFriendType<AI_CT_TONG_HUA_SHUN && cbFriendType!=AI_CT_ERROR)
		{
			return;
		}
	}

	//�������ϵ���
	tagOutCardTypeResult CardTypeResult[TYPE_COUNT];
	ZeroMemory(&CardTypeResult, sizeof(CardTypeResult));
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));
	AnalyseOutCardType(cbHandData, cbHandCardCount, CardTypeResult);

	BYTE cbMagicTurnData[10] = {0};
	MagicCardData(cbTurnData, cbTurnCardCount, cbMagicTurnData);

	//�������Ƴ�
	BYTE cbOutType = AI_CT_ERROR;
	if (CardTypeResult[cbTurnType].cbCardTypeCount > 0)
	{
		for (BYTE i=0; i<CardTypeResult[cbTurnType].cbCardTypeCount; i++)
		{
			if (CompareMagicCard(cbTurnData,CardTypeResult[cbTurnType].cbEachHandCardData[i],cbTurnCardCount,CardTypeResult[cbTurnType].cbEachHandCardCount[i]))
			{
				cbOutType = cbTurnType;
				OutCardResult.cbCardCount = CardTypeResult[cbTurnType].cbEachHandCardCount[i];
				CopyMemory(OutCardResult.cbResultCard,CardTypeResult[cbTurnType].cbEachHandCardData[i],OutCardResult.cbCardCount);
				
				break;
			}
		}
	}

	//��ը��
	if (OutCardResult.cbCardCount == 0)
	{
		BYTE cbTemp[3] = {AI_CT_BOMB,AI_CT_TONG_HUA_SHUN,AI_CT_BOMB_TW};
		for (BYTE i=0; i<3; i++)
		{
			BYTE cbType = cbTemp[i];
			if (CardTypeResult[cbType].cbCardTypeCount > 0)
			{
				for (BYTE j=0; j<CardTypeResult[cbType].cbCardTypeCount; j++)
				{
					if (CompareMagicCard(cbTurnData,CardTypeResult[cbType].cbEachHandCardData[j],cbTurnCardCount,CardTypeResult[cbType].cbEachHandCardCount[j]))
					{
						cbOutType = cbType;
						OutCardResult.cbCardCount = CardTypeResult[cbType].cbEachHandCardCount[j];
						CopyMemory(OutCardResult.cbResultCard,CardTypeResult[cbType].cbEachHandCardData[j],OutCardResult.cbCardCount);
						break;
					}
				}
			}
			if (OutCardResult.cbCardCount>0) break;
		}
	}

	//���ʣһ���ƺ�ը��
	if (wOutChairID!=wFriendChairID && OutCardResult.cbCardCount>0)
	{
		BYTE cbType = GetAfterRemoveBombsCardType(cbHandCardData, cbHandCardCount);
		//�¼�Σ��
		if (bUnderDanger)
		{
			BYTE cbHCType = GetMagicCardType(m_cbHandCardData[(wMeChairID+1)%GAME_PLAYER], m_cbHandCardCount[(wMeChairID+1)%GAME_PLAYER]);
			if (cbHCType >= AI_CT_TONG_HUA_SHUN) 
			{
				if (m_cbHandCardCount[(wMeChairID+3)%GAME_PLAYER] == 0)
				{
					return;
				}
				else
				{
					if (rand()%10 < 3)
					{
						return;
					}
				}
			}
			else if (cbHCType != AI_CT_ERROR)
			{
				return;
			}
		}
		//�ϼ�Σ��
		if (bUpsideDanger)
		{
			BYTE cbHCType = GetMagicCardType(m_cbHandCardData[wUpsideChairID], m_cbHandCardCount[wUpsideChairID]);
			if (cbHCType >= AI_CT_TONG_HUA_SHUN) 
			{
				if (m_cbHandCardCount[wUnderChairID] == 0)
				{
					return;
				}
				else
				{
					if (rand()%10 < 3)
					{
						return;
					}
				}
				
			}
			else if (cbHCType != AI_CT_ERROR)
			{
				return;
			}
		}

		//�Լ����ƿɳ�
		if (cbType != AI_CT_ERROR)
		{
			if (rand()%10 < 5)
			{
				return;
			}
		}
	}


	//����ǵ��źͶ���(����)
	if (cbTurnType==AI_CT_SINGLE || cbTurnType==AI_CT_DOUBLE)
	{
		//�Ƿ�ʣ��һ���ƺ�һ��ը����һ��������������
		bool bCaiCard = true;
		BYTE cbTempCard[MAX_COUNT] = {0};
		BYTE cbCount = cbHandCardCount;
		CopyMemory(cbTempCard, cbHandData, cbHandCardCount);
		for (BYTE i=AI_CT_TONG_HUA_SHUN; i<=AI_CT_BOMB_TW; i++)
		{
			if (CardTypeResult[i].cbCardTypeCount > 0)
			{
				for (BYTE j=0; j<CardTypeResult[i].cbCardTypeCount; j++)
				{
					RemoveCard(CardTypeResult[i].cbEachHandCardData[j], CardTypeResult[i].cbEachHandCardCount[j], cbTempCard, cbCount);
					cbCount -= CardTypeResult[i].cbEachHandCardCount[j];
				}
			}

			if (cbCount!=cbHandCardCount && GetMagicCardType(cbTempCard,cbCount)==AI_CT_THREE_DOUBLE)
			{
				bCaiCard = false;
			}
		}

		if ((OutCardResult.cbCardCount==0 || cbOutType==AI_CT_BOMB || cbOutType==AI_CT_BOMB_TW || cbOutType==AI_CT_TONG_HUA_SHUN) && bCaiCard)
		{
			//������Ż����Ƕ��ӳ����Ǵ����������ϵ���
			BYTE cbValue = GetTypeCardValue(cbTurnData, cbTurnCardCount);
			
			//������С�������ȳ�С��һ��
			if (cbTurnType == AI_CT_SINGLE)
			{
				if (cbValue==15 && CardTypeResult[AI_CT_DOUBLE].cbCardTypeCount>0)
				{
					if (CardTypeResult[AI_CT_DOUBLE].cbEachHandCardData[CardTypeResult[AI_CT_DOUBLE].cbCardTypeCount-1][0] == 0x4E)
					{
						cbOutType = cbTurnType;
						OutCardResult.cbCardCount = 1;
						OutCardResult.cbResultCard[0] = CardTypeResult[AI_CT_DOUBLE].cbEachHandCardData[CardTypeResult[AI_CT_DOUBLE].cbCardTypeCount-1][0];
					}
				}
				else if (cbValue==17 && CardTypeResult[AI_CT_DOUBLE].cbCardTypeCount>0)
				{
					if (CardTypeResult[AI_CT_DOUBLE].cbEachHandCardData[CardTypeResult[AI_CT_DOUBLE].cbCardTypeCount-1][0] == 0x4F)
					{
						cbOutType = cbTurnType;
						OutCardResult.cbCardCount = 1;
						OutCardResult.cbResultCard[0] = CardTypeResult[AI_CT_DOUBLE].cbEachHandCardData[CardTypeResult[AI_CT_DOUBLE].cbCardTypeCount-1][0];
					}
				}

				//����з���γ�����,һ������ѹ��
				if (OutCardResult.cbCardCount>0 && (m_wRoundOutTimes>>8)!=(wMeChairID+2)%GAME_PLAYER)
				{
					if ((m_wRoundOutTimes&0x00ff) == 2)
					{
						if (rand()%10 < 3)
						{
							return;
						}
					}
					if ((m_wRoundOutTimes&0x00ff) > 2)
					{
						if (rand()%10 < 4)
						{
							return;
						}
					}
				}
			}

			if (OutCardResult.cbCardCount==0 || cbOutType==AI_CT_BOMB || cbOutType==AI_CT_BOMB_TW || cbOutType==AI_CT_TONG_HUA_SHUN)
			{
				//�ǶԼҲ�Ҫ����
				if (m_PlayerTypeImfo[(wMeChairID+2)%GAME_PLAYER].cbUnLikeCardTypeCount > 0)
				{
					for (BYTE i=0; i<m_PlayerTypeImfo[wFriendChairID].cbUnLikeCardTypeCount; i++)
					{
						if (m_PlayerTypeImfo[wFriendChairID].cbUnLikeCardType[i]==cbTurnType || m_cbCardNum[wFriendChairID]==0)
						{
							//�˿˷���
							tagOutCardResult OutCardResultTemp;
							ZeroMemory(&OutCardResultTemp, sizeof(OutCardResultTemp));
							AnalyseCardPass(cbHandData, cbHandCardCount, cbTurnData,cbTurnType, OutCardResultTemp);
							if (OutCardResultTemp.cbCardCount > 0)
							{
								cbOutType = cbTurnType;
								VERIFY(GetMagicCardType(OutCardResultTemp.cbResultCard,OutCardResultTemp.cbCardCount) == cbTurnType);
								CopyMemory(&OutCardResult, &OutCardResultTemp, sizeof(OutCardResultTemp));
							}
							break;
						}
					}
				}
			}
		}
	}

	if (OutCardResult.cbCardCount == 0)
	{
		return;
	}
	ASSERT(cbOutType!=AI_CT_ERROR);
	//��������
	if (cbOutType!=AI_CT_BOMB && cbOutType!=AI_CT_TONG_HUA_SHUN && cbOutType!=AI_CT_BOMB_TW)
	{
		bool bUnAvailableType = false;
		if (cbOutType==m_PlayerBestOutType.cbCardType[wUnderChairID] || cbOutType==m_PlayerBestOutType.cbCardType[wUpsideChairID])
		{
			bUnAvailableType = true;
		}

		//Σ��ϵ����1�� 
		BYTE bDanger = 0;
		WORD wDangerChairID = INVALID_CHAIR;
		if (bUnAvailableType)
		{
			for (BYTE i=1; i<GAME_PLAYER; i++)
			{
				WORD wChairID = (wMeChairID+i)%GAME_PLAYER;
				if (m_cbCardNum[wChairID]==0) continue;
				//���������Ϊֹ
				if (wChairID == wOutChairID) break;

				if (wChairID == wFriendChairID)
				{
					WORD cbNextChairID = (wChairID+1)%GAME_PLAYER;
					if (m_cbCardNum[cbNextChairID]!=0 && m_PlayerBestOutType.cbCardType[cbNextChairID]==cbTurnType)
					{
						bDanger = 2;
						wDangerChairID = cbNextChairID;
					}
					break;
				}
				else
				{
					if (m_PlayerBestOutType.cbCardType[wChairID]==cbTurnType && bDanger==0)
					{
						bDanger = 1;
						wDangerChairID = wChairID;
						break;
					}
				}	
			}

			if (bDanger == 0)
			{
				bUnAvailableType = false;
			}
		}

		//2���ɳ�������
		if (!bUnAvailableType)
		{
			//1����������ǵ���
			if (wOutChairID==wUnderChairID || wOutChairID==wUpsideChairID)
			{
				//1�������ƣ����ƣ�(���ԣ�˳�ӣ�����)
				if (cbOutType==AI_CT_DOUBLE_LINK || cbOutType==AI_CT_SINGLE_LINK || cbOutType==AI_CT_THREE_LINK)
				{
					CheckOutCardRightType(OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbOutType);
					VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData, cbTurnCardCount));
					return;
				}
				else
				{
					// 2���������ƣ�
					//����������ϼ�
					if (wOutChairID == wUpsideChairID)
					{
						//Ҫ������� || ��С�� || ����
						BYTE cbMaxValue = SearchMaxLogicValue(cbOutType, cbHandData, cbHandCardCount);
						BYTE cbValue = GetTypeCardValue(OutCardResult.cbResultCard, OutCardResult.cbCardCount);
						if (cbMaxValue > 0)
						{
							bool bOut = false;
							if (cbValue>=cbMaxValue || cbValue>=15)
							{
								//a)������ʣ��3�������ƿɳ�������
								if (cbValue+3 >= cbMaxValue)
								{
									bOut = true;
								}
								//c) �ԼҲ�Ҫ�����ͣ�����
								if (m_PlayerTypeImfo[wFriendChairID].cbUnLikeCardTypeCount>0 && m_cbCardNum[wFriendChairID]!=0)
								{
									for (BYTE i=0;i<m_PlayerTypeImfo[wFriendChairID].cbUnLikeCardTypeCount; i++)
									{
										if (cbTurnType == m_PlayerTypeImfo[wFriendChairID].cbUnLikeCardType[i])
										{
											bOut = true;
											break;
										}
									}
								}	
								//�Լ��Ѿ�������
								if (m_cbCardNum[wFriendChairID] == 0)
								{
									bOut = true;
								}
								//ͳ�ƽ��
								if (bOut)
								{
									VERIFY(GetMagicCardType(OutCardResult.cbResultCard, OutCardResult.cbCardCount)==cbOutType);
									VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData,cbTurnCardCount));
									return;
								}
								else
								{
									if (rand()%100 <= 98)
									{
										ZeroMemory(&OutCardResult, sizeof(OutCardResult));
									}
									VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData,cbTurnCardCount));
									return;
								}
							}
							else
							{
								//��������
								VERIFY(GetMagicCardType(OutCardResult.cbResultCard, OutCardResult.cbCardCount) == cbOutType);
								VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData, cbTurnCardCount));
								return;
							}
						}
					}
					else//b) ������Ҳ����ϼң�˵�����ҶԼҽӲ����ƣ�
					{
						//��������
						VERIFY(GetMagicCardType(OutCardResult.cbResultCard, OutCardResult.cbCardCount) == cbOutType);
						VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData, cbTurnCardCount));
						return;
					}
				}
			}
			else
			{
				//��������Ǳ��ҶԼ�
				//1�������ƣ�100%�����ƣ�
				//1�������ƣ������ƣ�(���ԣ�˳�ӣ�����)
				if (cbOutType==AI_CT_DOUBLE_LINK || cbOutType==AI_CT_SINGLE_LINK || cbOutType==AI_CT_THREE_LINK)
				{
					ZeroMemory(&OutCardResult, sizeof(OutCardResult));
					VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData, cbTurnCardCount));
					return;
				}
				else
				{
					//a) С��2�����ڵ��ƣ�98%�����ƣ�
					BYTE cbMaxCardValue = SearchMaxLogicValue(cbTurnType, cbHandData, cbHandCardCount);
					BYTE cbCardValue = GetTypeCardValue(OutCardResult.cbResultCard, OutCardResult.cbCardCount);
					bool bOut = true;
					if (cbMaxCardValue > 0)
					{	
						if (cbCardValue+2 >= cbMaxCardValue)
						{
							bOut = false;
						}
					}

					//b) ���˲�Ҫ�����ͣ�98%�����ƣ�
					BYTE cbTemp = 0;
					for (BYTE i=1; i<=3; i+=2)
					{
						WORD wChairID = (wMeChairID+i)%GAME_PLAYER;
						if (m_cbCardNum[wChairID]==0) continue;
						if (m_PlayerTypeImfo[wChairID].cbUnLikeCardTypeCount > 0)
						{
							for (BYTE j=0; j<m_PlayerTypeImfo[wChairID].cbUnLikeCardTypeCount; j++)
							{
								if (m_PlayerTypeImfo[wChairID].cbUnLikeCardType[j] == cbTurnType)
								{
									cbTemp++;
									break;
								}
							}
						}
					}

					if (cbTemp == 2)
					{
						bOut = false;
					}

					if (bOut)
					{
						//��������
						VERIFY(GetMagicCardType(OutCardResult.cbResultCard, OutCardResult.cbCardCount) == cbOutType);
						VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData, cbTurnCardCount));
						return;
					}
					else
					{
						//�Ƚ�С���ƽ���
						if ((cbTurnType==AI_CT_SINGLE&&cbCardValue<=14) || cbCardValue<=10)
						{
							VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData, cbTurnCardCount));
							//�������� 
							return;
						}
						if (rand()%100 <= 98)
						{
							ZeroMemory(&OutCardResult, sizeof(OutCardResult));	
						}
						VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData, cbTurnCardCount));
						return;
					}
				}
			}
		}
		else 
		{
			ASSERT(bDanger!=0);
			//���ɳ�����
			//1��Σ��ϵ����1
			if (bDanger == 1)
			{
				//1�������������Ÿ���n��
				if (cbTurnType==AI_CT_SINGLE || cbTurnType==AI_CT_DOUBLE || cbTurnType==AI_CT_THREE || cbTurnType==AI_CT_THREE_DOUBLE)
				{
					//1��һ��ը����һ��(�����Ƕ��ӣ�������������)��������󣬳�ը����	
					if (CardTypeResult[cbTurnType].cbCardTypeCount <= 1)
					{
						BYTE cbTempData[MAX_COUNT] = {0};
						BYTE cbTempCount = cbHandCardCount;
						CopyMemory(cbTempData, cbHandData, cbTempCount);
						RemoveCard(CardTypeResult[cbTurnType].cbEachHandCardData[0], CardTypeResult[cbTurnType].cbEachHandCardCount[0], cbTempData, cbTempCount);
						cbTempCount -= CardTypeResult[cbTurnType].cbEachHandCardCount[0];
						BYTE cbType = GetMagicCardType(cbTempData,cbTempCount);
						if (cbType==AI_CT_BOMB || cbType==AI_CT_TONG_HUA_SHUN || cbType==AI_CT_BOMB_TW)
						{
							BYTE cbMinValue = m_PlayerBestOutType.cbCardSafeValue[wDangerChairID]==0 ? SearchMaxLogicValue(cbTurnType,cbHandData,cbHandCardCount):(m_PlayerBestOutType.cbCardSafeValue[wDangerChairID]+1);
							if (GetTypeCardValue(CardTypeResult[cbTurnType].cbEachHandCardData[0], CardTypeResult[cbTurnType].cbEachHandCardCount[0]) >= cbMinValue
								&& CompareMagicCard(cbTurnData, CardTypeResult[cbTurnType].cbEachHandCardData[0], cbTurnCardCount, CardTypeResult[cbTurnType].cbEachHandCardCount[0]))
							{
								OutCardResult.cbCardCount = 0;
								OutCardResult.cbResultCard[0] = CardTypeResult[cbTurnType].cbEachHandCardData[0][0];
								VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData, cbTurnCardCount));
								return;
							}
							else
							{
								OutCardResult.cbCardCount = cbTempCount;
								CopyMemory(OutCardResult.cbResultCard, cbTempData,cbTempCount);
								VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData,cbTurnCardCount));
								return;
							}
						}
					}
					else
					{
						//2��>= 2 ������������������ƣ�
						ASSERT(CardTypeResult[cbTurnType].cbCardTypeCount > 0);
						BYTE cbMaxValue = SearchMaxLogicValue(cbTurnType, cbHandData, cbHandCardCount);
						BYTE cbMinVaue = GetTypeCardValue(CardTypeResult[cbTurnType].cbEachHandCardData[0], CardTypeResult[cbTurnType].cbEachHandCardCount[0]);
						for (int i=CardTypeResult[cbTurnType].cbCardTypeCount-1; i>=0; i--)
						{
							BYTE cbValue = GetTypeCardValue(CardTypeResult[cbTurnType].cbEachHandCardData[i], CardTypeResult[cbTurnType].cbEachHandCardCount[i]);
							if (!CompareMagicCard(cbTurnData,CardTypeResult[cbTurnType].cbEachHandCardData[i],cbTurnCardCount,CardTypeResult[cbTurnType].cbEachHandCardCount[i])) break;

							if (cbValue>=cbMaxValue) continue;
							if (cbValue == cbMinVaue)
							{
								OutCardResult.cbCardCount = CardTypeResult[cbTurnType].cbEachHandCardCount[CardTypeResult[cbTurnType].cbCardTypeCount-1];
								CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbTurnType].cbEachHandCardData[CardTypeResult[cbTurnType].cbCardTypeCount-1], OutCardResult.cbCardCount);
								VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData, cbTurnCardCount));
								return;
							}

							OutCardResult.cbCardCount = CardTypeResult[cbTurnType].cbEachHandCardCount[i];
							CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbTurnType].cbEachHandCardData[i], OutCardResult.cbCardCount);
							VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData, cbTurnCardCount));
							return;
						}
						VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData, cbTurnCardCount));
						return;
					}
				}	
			}
			else
			{
				//2��Σ��ϵ������1
				//1��һ��ը����һ��(�����Ƕ��ӣ�������������)���������ģ���ը����
				//2��������ƣ�
				if (CardTypeResult[cbTurnType].cbCardTypeCount == cbTurnCardCount)
				{
					BYTE cbTempData[MAX_COUNT] = {0};
					BYTE cbTempCount = cbHandCardCount;
					CopyMemory(cbTempData, cbHandData, cbTempCount);
					RemoveCard(OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTempData, cbTempCount);
					cbTempCount -= OutCardResult.cbCardCount;
					BYTE cbType = GetMagicCardType(cbTempData, cbTempCount);
					if (cbType==AI_CT_BOMB || cbType==AI_CT_TONG_HUA_SHUN || cbType==AI_CT_BOMB_TW)
					{
						BYTE cbMinValue = m_PlayerBestOutType.cbCardSafeValue[wDangerChairID]==0 ? SearchMaxLogicValue(cbTurnType,cbHandData,cbHandCardCount):(m_PlayerBestOutType.cbCardSafeValue[wDangerChairID]+1);
						if (GetTypeCardValue(CardTypeResult[cbTurnType].cbEachHandCardData[0],CardTypeResult[cbTurnType].cbEachHandCardCount[0]) >= cbMinValue)
						{
							OutCardResult.cbCardCount = 0;
							OutCardResult.cbResultCard[0] = CardTypeResult[cbTurnType].cbEachHandCardData[0][0];
							VERIFY(CheckWillOutCard(wOutChairID,wMeChairID,OutCardResult.cbResultCard,OutCardResult.cbCardCount,cbTurnData,cbTurnCardCount));
							return;
						}
						else
						{
							OutCardResult.cbCardCount = cbTempCount;
							CopyMemory(OutCardResult.cbResultCard,cbTempData,cbTempCount);
							VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData, cbTurnCardCount));
							return;
						}
					}
				}
				else
				{
					// 2��������ƣ�
					if (CardTypeResult[cbTurnType].cbCardTypeCount > 0)
					{
						if (CompareMagicCard(OutCardResult.cbResultCard, CardTypeResult[cbTurnType].cbEachHandCardData[CardTypeResult[cbTurnType].cbCardTypeCount-1], OutCardResult.cbCardCount, CardTypeResult[cbTurnType].cbEachHandCardCount[CardTypeResult[cbTurnType].cbCardTypeCount-1]))
						{
							OutCardResult.cbCardCount = CardTypeResult[cbTurnType].cbEachHandCardCount[CardTypeResult[cbTurnType].cbCardTypeCount-1];
							CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbTurnType].cbEachHandCardData[CardTypeResult[cbTurnType].cbCardTypeCount-1], OutCardResult.cbCardCount);
							VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData, cbTurnCardCount));
							return;
						}
					}
					else
					{
						//����ը����������
						tagOutCardResult OutCardResultTemp;
						ZeroMemory(&OutCardResultTemp, sizeof(OutCardResultTemp));
						AnalyseCardPass(cbHandData, cbHandCardCount, cbTurnData, cbTurnType, OutCardResultTemp);
						if (OutCardResultTemp.cbCardCount > 0)
						{
							CopyMemory(&OutCardResult, &OutCardResultTemp, sizeof(OutCardResultTemp));
						}
					}
					VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData, cbTurnCardCount));
					return;
				}
			}
		}
	}
	else //ը��
	{
		
		//�����ƴ���rate�� 
		//�������û�����ˣ�
		//�����������ƻ����Ƿ��������е�����ƣ����߳���ը�������߳����ǶԼҲ�Ҫ������ 
		//�ԼҲ�Ҫ������
		bool bLikeType = true;
		if (m_cbCardNum[wFriendChairID] > 0)
		{
			if (m_PlayerTypeImfo[wFriendChairID].cbUnLikeCardTypeCount > 0)
			{
				for (BYTE i=0; i<m_PlayerTypeImfo[wFriendChairID].cbUnLikeCardTypeCount; i++)
				{
					if (m_PlayerTypeImfo[wFriendChairID].cbUnLikeCardType[i] == cbTurnType)
					{
						bLikeType = false;
						break;
					}
				}
			}
		}

		//ը��������
		BYTE cbBombCount = 0;
		BYTE cbCardCount = m_cbCardNum[(wOutChairID+2)%GAME_PLAYER];
		for (BYTE i=AI_CT_TONG_HUA_SHUN; i<=AI_CT_BOMB_TW; i++)
		{
			cbBombCount += CardTypeResult[i].cbCardTypeCount;
		}
		BYTE cbHandOutCount =0;
		// AI_CT_THREE_DOUBLE �ڶ��Ӻ�������
		for (BYTE i=AI_CT_SINGLE; i<AI_CT_THREE_DOUBLE; i++)
		{
			cbHandOutCount  += CardTypeResult[i].cbCardTypeCount;
		}
		//����һ���ظ��ģ�����һ�ζ��Ӻ�һ��3��
		cbHandOutCount -= CardTypeResult[AI_CT_THREE_DOUBLE].cbCardTypeCount;

		//ը���������ϵĳ���
		if (cbBombCount > cbHandOutCount)
		{
			if (wOutChairID != wFriendChairID)
			{
				BYTE cbMaxValue = SearchMaxLogicValue(cbTurnType, cbHandData, cbHandCardCount);
				if ((cbOutType>=AI_CT_SINGLE && cbOutType<=AI_CT_THREE) || cbOutType==AI_CT_THREE_DOUBLE)
				{
					if (cbMaxValue!=0 && GetTypeCardValue(cbTurnData,cbTurnCardCount)>=cbMaxValue)
					{
						return;
					}
				}
				else
				{
					return;
				}
			}
		}

		//����������е����Ѿ�û��
		if ((wOutChairID==wUnderChairID||wOutChairID==wUnderChairID) && m_cbCardNum[wOutChairID]==0)
		{
			BYTE cbMaxValue  = SearchMaxLogicValue(cbTurnType,cbHandData,cbHandCardCount);
			if ((cbTurnType==AI_CT_BOMB || cbTurnType==AI_CT_BOMB_TW || cbTurnType==AI_CT_TONG_HUA_SHUN || cbTurnType==AI_CT_SINGLE_LINK || cbTurnType==AI_CT_DOUBLE_LINK || cbTurnType==AI_CT_THREE_LINK) 
				|| ((cbTurnType==AI_CT_SINGLE || cbTurnType==AI_CT_DOUBLE || cbTurnType==AI_CT_THREE || cbTurnType==AI_CT_THREE_DOUBLE)	&& GetTypeCardValue(cbTurnData,cbTurnCardCount) >= cbMaxValue && cbMaxValue >0) 
				|| m_cbCardNum[wFriendChairID]==0 || !bLikeType)
			{
				if (m_cbCardNum[(wOutChairID+2)%GAME_PLAYER] <= 10)
				{
					BYTE cbBombRate = (11-m_cbCardNum[(wOutChairID+2)%GAME_PLAYER])*10*cbBombCount;
					if (rand()%100 > cbBombRate)
					{
						ZeroMemory(&OutCardResult, sizeof(OutCardResult));
					}
					VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData, cbTurnCardCount));
					return;
				}
				else if (m_cbCardNum[(wOutChairID+2)%GAME_PLAYER] <= 15 && cbBombCount >= 2)
				{
					if (rand()%100 > 50)
					{
						ZeroMemory(&OutCardResult,sizeof(OutCardResult));
					}
					VERIFY(CheckWillOutCard(wOutChairID,wMeChairID,OutCardResult.cbResultCard,OutCardResult.cbCardCount,cbTurnData,cbTurnCardCount));
					return;
				}
				else if (cbBombCount == 1)
				{
					ZeroMemory(&OutCardResult,sizeof(OutCardResult));
					VERIFY(CheckWillOutCard(wOutChairID,wMeChairID,OutCardResult.cbResultCard,OutCardResult.cbCardCount,cbTurnData,cbTurnCardCount));
					return;
				}
			}
		}


		//���ʼ���
		if (m_cbCardNum[(wOutChairID+2)%GAME_PLAYER]<5 && cbBombCount<2)
		{
			cbBombCount = 2;
		}
	
		BYTE cbTempTurnData[MAX_COUNT] = {0};
		CopyMemory(cbTempTurnData, cbTurnCardData, cbTurnCardCount);
		BYTE cbMaxValue = SearchMaxLogicValue(cbTurnType,cbHandData,cbHandCardCount);
		if (wOutChairID!=wFriendChairID && m_cbCardNum[wOutChairID]==0 && m_cbCardNum[(wOutChairID+2)%GAME_PLAYER]<10)
		{
			if ((cbMaxValue>0&&GetTypeCardValue(cbTempTurnData,cbTurnCardCount)>=cbMaxValue) 
				|| cbTurnType==AI_CT_DOUBLE_LINK || cbTurnType==AI_CT_SINGLE_LINK || cbTurnType==AI_CT_THREE_LINK
				|| cbTurnType==AI_CT_BOMB||cbTurnType==AI_CT_TONG_HUA_SHUN || !bLikeType)
			{
				if (rand()%100 <= (11-cbCardCount)*10*cbBombCount)
				{
					ASSERT(OutCardResult.cbCardCount > 0);
					VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData, cbTurnCardCount));
					return;
				}	
			}
		}
		
		//1���ɳ�����(ʣ��ĵ��˵ĸ���
		BYTE cbEnemy = 0;
		for( BYTE i=1; i<=3; i+=2)
		{
			WORD wChairID = (wMeChairID+i)%GAME_PLAYER;
			if (m_cbCardNum[wChairID] > 0)
			{
				cbEnemy ++;
			}
		}

		bool bUnAvailableType = false;
		if (cbTurnType==m_PlayerBestOutType.cbCardType[wUnderChairID] || cbTurnType==m_PlayerBestOutType.cbCardType[wUpsideChairID])
		{
			bUnAvailableType = true;
		}

		//Σ��ϵ����1�� 
		BYTE cbDanger = 0;
		WORD wDangerChairID = INVALID_CHAIR;
		if (bUnAvailableType)
		{
			for (BYTE i=1; i<GAME_PLAYER; i++)
			{
				WORD wChairID = (wMeChairID+i)%GAME_PLAYER;
				if (m_cbCardNum[wChairID]==0) continue;
				//���������Ϊֹ
				if (wChairID==wOutChairID) break;

				if (wChairID == wFriendChairID)
				{
					WORD cbNextChairID = (wChairID+1)%GAME_PLAYER;
					if (m_cbCardNum[cbNextChairID]!=0 && m_PlayerBestOutType.cbCardType[cbNextChairID]==cbTurnType)
					{
						cbDanger = 2;
						wDangerChairID = cbNextChairID;
					}
					break;
				}
				else
				{
					if (m_PlayerBestOutType.cbCardType[wChairID]==cbTurnType && cbDanger==0)
					{
						cbDanger = 1;
						wDangerChairID = wChairID;
						break;
					}
				}	
			}

			if (cbDanger == 0)
			{
				bUnAvailableType = false;
			}
		}		

		if (!bUnAvailableType)
		{
			BYTE cbBombRate = 0;
			//	��������Ǳ��ң�������
			if (wOutChairID == wFriendChairID)
			{
				ZeroMemory(&OutCardResult, sizeof(OutCardResult));
				VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData, cbTurnCardCount));
				return;
			}
			else
			{
				//�������ֻʣ��һ��ը������ը��,���߶���ը��
				BYTE cbCount = 0;
				for (BYTE i=AI_CT_SINGLE; i<AI_CT_THREE_DOUBLE; i++)
				{
					cbCount += CardTypeResult[i].cbCardTypeCount;
				}
				if (cbCount == 0)
				{
					VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData, cbTurnCardCount));
					return;
				}
				//����������е��Ƶĸ���С��10
				if (m_cbCardNum[wOutChairID]<=10 && m_cbCardNum[wOutChairID]>0)
				{
					cbBombRate = (11-m_cbCardNum[wOutChairID])*5*(cbEnemy==2?1:2);
					if (!bLikeType)
					{
						cbBombRate += 10;
					}
				}			

				if (m_CurPlayerOutTimes.wCurrentChairID==wUnderChairID || m_CurPlayerOutTimes.wCurrentChairID==wUpsideChairID)
				{
					// �Լ��������ƵĴ��� > 2�Σ�2�غϣ� || �Լ��״γ��ƴ��� >= 3 (n)
					BYTE cbTempCount = cbBombCount>=2 ? 2:3;
					if (m_CurPlayerOutTimes.cbCurPlaOutCount>=cbTempCount || m_CurPlayerOutTimes.cbEnemyOutCount>=4 || m_CurPlayerOutTimes.cbEnemyFirstCount>=(cbTempCount+1-((cbBombCount>2)?1:0)))
					{
						BYTE cbTempRate = (cbBombCount*10);
						BYTE cbTemp = __max(m_CurPlayerOutTimes.cbCurPlaOutCount, m_CurPlayerOutTimes.cbEnemyOutCount);
						cbBombRate += cbTemp*cbTempRate+abs(2-cbEnemy)*cbTempRate*2;
						if (cbBombCount-2 > 0)
						{
							cbBombRate += (cbBombCount-1)*cbTempRate;
						}

						if (m_cbCardNum[wOutChairID]>5 && cbEnemy==2 && cbBombCount>=2)
						{
							cbBombRate += cbTemp*cbTempRate;
						}
						if (!bLikeType)
						{
							cbBombRate += cbBombCount*cbTempRate;
						}
					}
				}
				
				//��������ǵ��ˣ����������ƻ����ǶԼҵĲ�Ҫ�����ͣ����������Ʋ�ը
				if (cbTurnType==AI_CT_SINGLE || cbTurnType==AI_CT_DOUBLE || cbTurnType==AI_CT_THREE || cbTurnType==AI_CT_THREE_DOUBLE)
				{
					BYTE cbTempData[MAX_COUNT] = {0};
					CopyMemory(cbTempData, cbHandCardData, cbHandCardCount);
			
					BYTE cbMaxValue = SearchMaxLogicValue(cbTurnType, cbTempData, cbHandCardCount);
					BYTE cbValue = GetTypeCardValue(cbTempTurnData, cbTurnCardCount);
					ASSERT(cbMaxValue>0);
					if (bLikeType && cbValue<cbMaxValue && m_cbCardNum[wFriendChairID]!=0)
					{
						//������
						ZeroMemory(&OutCardResult, sizeof(OutCardResult));
						VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData, cbTurnCardCount));
						return;
					}
				}
				else 
				{
					if (cbBombCount > 2)  //������ը�����ʱ���
					{
						cbBombRate += cbBombCount*10;	
					}
					if (m_CurPlayerOutTimes.wCurrentChairID!=m_ForOutImfo.wOutChairID && m_CurPlayerOutTimes.wCurrentChairID==wOutChairID
						&& m_CurPlayerOutTimes.wCurrentChairID!=INVALID_CHAIR && m_ForOutImfo.wOutChairID!=INVALID_CHAIR)
					{
						cbBombRate += cbBombCount*30*m_CurPlayerOutTimes.cbCurPlaOutCount;	
					}
					else 
					{
						BYTE cbTempCount = cbBombCount>=2 ? 2:3;
						cbBombRate += cbBombCount*10*((m_CurPlayerOutTimes.cbCurPlaOutCount-cbTempCount) >0 ? (m_CurPlayerOutTimes.cbCurPlaOutCount-cbTempCount):0);
					}
				}
				
				if (rand()%100 < cbBombRate)
				{
					VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData, cbTurnCardCount));
					return;
				}
				else
				{
					//������
					ZeroMemory(&OutCardResult, sizeof(OutCardResult));
					VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData, cbTurnCardCount));
					return;
				}
				//�Ƶĸ��� <= 4�������׼ҳ��ƴ��� >= 3 && num == 2; �����Ż����Ը�ʣ��ĵ��ˣ� (��ֻʣ��ը��~)   
				//Rate = 10;
				//��������Ƶĸ��� > 10 (�����Լ��öԼ����ߣ��׳����ȳ��Լ�ϲ������)
				//Rate += n/2 * 10;   
			}
		}
		else
		{
			// 2�����ɳ�����
			ASSERT(cbDanger!=0);
			if (cbDanger == 1)
			{
				//1��ʣ��ը��������һ���ƣ�
				BYTE cbTempCount = 0;
				for (BYTE i=AI_CT_SINGLE; i<=AI_CT_THREE_DOUBLE; i++)
				{
					cbTempCount += CardTypeResult[i].cbCardTypeCount;
				}
				if (cbTempCount==0 || cbTempCount==1)
				{
					//���ҳ��Ƴ�������Сֵ�ģ�������
					BYTE cbMinValue = m_PlayerBestOutType.cbCardSafeValue[wDangerChairID]==0 ? SearchMaxLogicValue(cbTurnType,cbHandData,cbHandCardCount):(m_PlayerBestOutType.cbCardSafeValue[wDangerChairID]+1);
					if (wOutChairID==wDangerChairID && GetTypeCardValue(cbTempTurnData,cbTurnCardCount)>=cbMinValue)
					{
						ZeroMemory(&OutCardResult, sizeof(OutCardResult));
					}	
				}
				//����ǵ��ţ�����������2�����ϵ���
				if (cbTurnType==AI_CT_SINGLE && CardTypeResult[AI_CT_SINGLE].cbCardTypeCount>=2 && m_cbCardNum[wFriendChairID]!=0)
				{
					ZeroMemory(&OutCardResult, sizeof(OutCardResult));
				}
				//�������
				VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData, cbTurnCardCount));
				return;
			}//Σ��ϵ����2
			else if (cbDanger == 2)
			{
				//���ҳ��ƣ�����ը��
				if (wOutChairID == wFriendChairID)
				{
					ZeroMemory(&OutCardResult, sizeof(OutCardResult));
				}
				else 
				{
					//1��ʣ��ը��������һ���ƣ�
					BYTE cbBombRate = 0;
					BYTE cbTempCount = 0;
					for (BYTE i=AI_CT_SINGLE; i<=AI_CT_THREE_DOUBLE; i++)
					{
						cbTempCount += CardTypeResult[i].cbCardTypeCount;
					}
					if (cbTempCount==0 || cbTempCount==1)
					{
						VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData, cbTurnCardCount));
						return;
					}
					else
					{
						WORD wChairIDXia = m_cbCardNum[wUpsideChairID]==0 ? INVALID_CHAIR:wUnderChairID;
						ASSERT(m_cbCardNum[wFriendChairID]>0 && wDangerChairID==wUpsideChairID);

						//�Լһ��ڣ��ǶԼҲ��ɳ������ͣ�
						if (m_PlayerTypeImfo[wFriendChairID].cbUnLikeCardTypeCount > 0)
						{
							for (BYTE i=0; i<m_PlayerTypeImfo[wFriendChairID].cbUnLikeCardTypeCount; i++)
							{
								if (m_PlayerTypeImfo[wFriendChairID].cbUnLikeCardType[i] == cbTurnType)
								{
									VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData, cbTurnCardCount));
									//�ǶԼҲ�Ҫ������
									return;
								}
							}
						}

						BYTE cbMinValue = m_PlayerBestOutType.cbCardSafeValue[wDangerChairID]==0 ? SearchMaxLogicValue(cbTurnType,cbHandData,cbHandCardCount):(m_PlayerBestOutType.cbCardSafeValue[wDangerChairID]+1);
						//ʣ��һ���Լң����ƶԼ����ϼң�ʣ���������2�����ϣ����ţ���������
						if (wChairIDXia == INVALID_CHAIR)
						{
							if (GetTypeCardValue(cbTempTurnData,cbTurnCardCount)<cbMinValue || m_cbCardNum[wFriendChairID]<cbTurnCardCount)
							{
								VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData, cbTurnCardCount));
								return;
							}

							//����ǵ��ţ�ֻʣ�ڴ��ڵ�������������һ�ŵ��ţ���ը��
							if (cbTurnType == AI_CT_SINGLE)
							{
								if (CardTypeResult[AI_CT_SINGLE].cbCardTypeCount > 0)
								{
									BYTE cbCount = 0;
									for (BYTE i=0; i<CardTypeResult[AI_CT_SINGLE].cbCardTypeCount; i++)
									{
										if (GetCardLogicValue(CardTypeResult[AI_CT_SINGLE].cbEachHandCardData[i][0]) >= cbMinValue)
										{
											cbCount ++;
										}
									}

									if ((CardTypeResult[AI_CT_SINGLE].cbCardTypeCount-cbCount) <= 1)
									{
										VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData, cbTurnCardCount));
										return;
									}
								}
								else
								{
									VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData, cbTurnCardCount));
									return;
								}
							}
						}	
					}
				}
			}
		}
	}	
}

//�����˿�
bool CAILogic::SearchOutCardAI(WORD wMeChairID, WORD wOutChairID, const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult &OutCardResult)
{
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));

	bool bFirstOut = cbTurnCardCount==0 ? true:false;
	//�״γ���
	if (bFirstOut)
	{
		SearchFirstOutCard(wMeChairID, cbHandCardData, cbHandCardCount, OutCardResult);
		ASSERT(OutCardResult.cbCardCount!=0);		
	}
	else
	{
		SearchNextOutCard(wMeChairID, wOutChairID, cbHandCardData, cbHandCardCount, cbTurnCardData, cbTurnCardCount, OutCardResult);
		if (OutCardResult.cbCardCount != 0)
		{
			BYTE cbTurnData[MAX_COUNT] = {0};
			CopyMemory(cbTurnData, cbTurnCardData, cbTurnCardCount);
			VERIFY(CompareMagicCard(cbTurnData, OutCardResult.cbResultCard, cbTurnCardCount, OutCardResult.cbCardCount));
		}
	}
	return true;
}

//��֤����
bool CAILogic::CheckOutCardRightType(BYTE cbCardData[], BYTE cbCardCount, BYTE cbType)
{
	//�������
	BYTE cbMagicData[MAX_COUNT] = {0};
	MagicCardData(cbCardData, cbCardCount, cbMagicData);
 	VERIFY(GetCardType(cbMagicData,cbCardCount)==cbType);
	return true;
}

//�Ƿ����
bool CAILogic::AnalyseCardPass(BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbOutCard[], BYTE cbCardType, tagOutCardResult &OutCardResult)
{
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));
	if (cbCardType!=AI_CT_SINGLE && cbCardType!=AI_CT_DOUBLE) return false;

	BYTE cbOutValue = GetCardLogicValue(cbOutCard[0]);
	BYTE cbMaxValue = SearchMaxLogicValue(cbCardType, cbHandCardData, cbHandCardCount);
	if (cbOutValue>=cbMaxValue) return false;

	tagOutCardTypeResult tagResult[TYPE_COUNT];
	AnalyseOutCardType(cbHandCardData, cbHandCardCount, tagResult);

	BYTE cbType = 0;
	BYTE cbTempValue = 0;
	//�жϿ����ɵ�����
	for (BYTE i=cbCardType+1; i<=AI_CT_THREE; i++)
	{
		if (tagResult[i].cbCardTypeCount > 0)
		{
			BYTE cbVaue = GetCardLogicValue(tagResult[i].cbEachHandCardData[(tagResult[i].cbCardTypeCount-1)][0]);
			if (cbVaue > cbTempValue)
			{
				cbType = i;
				cbTempValue = cbVaue;
			}
		}
	}
	//û�п��Բ������/�����С�������
	if (cbType==0 || cbTempValue<=cbOutValue)
	{
		//���ӵ������ը���Ŷ���3������
		if (tagResult[AI_CT_BOMB_TW].cbCardTypeCount > 0)
		{
			//С���Ŵ���3
			BYTE cbCount = 0;
			if (tagResult[AI_CT_SINGLE].cbCardTypeCount > 0)
			{
				for (BYTE i=0; i<tagResult[AI_CT_SINGLE].cbCardTypeCount; i++)
				{
					if (GetCardLogicValue(tagResult[AI_CT_SINGLE].cbEachHandCardData[i][0] < 15))
					{
						cbCount++;
					}
				}
				if (cbCount >= 3)
				{
					BYTE cbTempData[4] = {0};
					CopyMemory(cbTempData, &tagResult[AI_CT_BOMB_TW].cbEachHandCardData[0], 4);
					SortCardList(cbTempData, 4, ST_ORDER);

					OutCardResult.cbCardCount = (cbCardType==AI_CT_SINGLE?1:2);
					CopyMemory(OutCardResult.cbResultCard, &cbTempData[2], OutCardResult.cbCardCount);
					return true;
				}
			}
		}
		return false;
	}
	else
	{
		//ͳ�ƽ��
		OutCardResult.cbCardCount = (cbCardType==AI_CT_SINGLE?1:2);
		CopyMemory(OutCardResult.cbResultCard, tagResult[cbType].cbEachHandCardData[tagResult[cbType].cbCardTypeCount-1], OutCardResult.cbCardCount);
		return true;
	}	
	return false;
}

//����ͬ��˳
bool CAILogic::AnalyseTongHuaShun(BYTE const cbHandCardData[], BYTE cbHandCardCount, tagOutCardTypeResult &CardTypeResult)
{
	ZeroMemory(&CardTypeResult, sizeof(CardTypeResult));
	
	//�����5��ͬ��˳	
	tagOutCardTypeResult OutCardTypeResultTemp[MAX_COUNT], AnalyseOutCardTypeResult[TYPE_COUNT];
	ZeroMemory(&OutCardTypeResultTemp, sizeof(OutCardTypeResultTemp));
	ZeroMemory(&AnalyseOutCardTypeResult, sizeof(AnalyseOutCardTypeResult));

	BYTE cbHandData[MAX_COUNT]={0}, cbHandCount=cbHandCardCount;
	BYTE cbRemoveData[MAX_COUNT]={0}, cbRemoveCount=cbHandCount;
	CopyMemory(cbHandData, cbHandCardData, cbHandCount);
	CopyMemory(cbRemoveData, cbHandCardData, cbHandCount);

	//ը�����͵��Ÿ���
	BYTE cbBombCount[MAX_COUNT] = {0};
	BYTE cbSingleCount[MAX_COUNT] = {0};

	BYTE cbCount = 0;
	BYTE cbIndexValue = 0x14;
	tagOutCardResult OutCardResult;
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));

	BYTE cbMaxData[MAX_COUNT] = {0};
	cbMaxData[cbCount]= cbIndexValue;
	bool bTongHuaCard = true;
 	while (bTongHuaCard)
	{
		bTongHuaCard = SearchTongHuaShun(cbHandData, cbHandCount, cbMaxData[cbCount], OutCardResult);
		if (!bTongHuaCard)
		{
			bTongHuaCard = SearchMagicTongHuaShun(cbHandData, cbHandCount, cbMaxData[cbCount], OutCardResult);
		}

		if (bTongHuaCard)
		{
			BYTE cbMagicData[5] = {0};
			MagicCardData(OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbMagicData);
			cbMaxData[cbCount+1] = cbMagicData[0];
			if (GetCardValue(cbMaxData[cbCount+1]) != 1)
			{
				SortCardList(cbMagicData, OutCardResult.cbCardCount, ST_VALUE);
				cbMaxData[cbCount+1] = cbMagicData[0];
			}

			//�Ƿ�����ͬ��ͬ��˳
			for (BYTE i=0; i<=cbCount; i++)
			{
				if (cbMaxData[cbCount+1] == cbMaxData[i])
				{
					bTongHuaCard = false;
					break;
				}
			}
		}
		
		if (!bTongHuaCard)
		{
			break;
		}

		bool bTemp = bTongHuaCard;
		cbRemoveCount = cbHandCount;
		CopyMemory(cbRemoveData, cbHandData, cbHandCount);
		//�洢����		
		while(bTemp && cbRemoveCount > 0)
		{
			OutCardTypeResultTemp[cbCount].cbEachHandCardCount[OutCardTypeResultTemp[cbCount].cbCardTypeCount] = OutCardResult.cbCardCount;
			CopyMemory(OutCardTypeResultTemp[cbCount].cbEachHandCardData[OutCardTypeResultTemp[cbCount].cbCardTypeCount], OutCardResult.cbResultCard, OutCardResult.cbCardCount);
			OutCardTypeResultTemp[cbCount].cbCardTypeCount++;
			//�Ƴ����е�ͬ��˳

			RemoveCard(OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbRemoveData, cbRemoveCount);
			cbRemoveCount -= OutCardResult.cbCardCount;

			bTemp = SearchTongHuaShun(cbRemoveData, cbRemoveCount, cbIndexValue, OutCardResult);
			if (!bTemp)
			{
				bTemp = SearchMagicTongHuaShun(cbRemoveData, cbRemoveCount, cbIndexValue, OutCardResult);
			}

			if (!bTemp || cbRemoveCount<=0)
			{
				break;
			}
		}
		
		//ͳ��ը�����͵��Ÿ���
		//�������ϵ���	
		if (bTongHuaCard)
		{
			if (cbRemoveCount > 0)
			{
				ZeroMemory(&AnalyseOutCardTypeResult, sizeof(AnalyseOutCardTypeResult));
				AnalyseOutCardTypeNomal(cbRemoveData, cbRemoveCount, AnalyseOutCardTypeResult);
				cbSingleCount[cbCount] = AnalyseOutCardTypeResult[AI_CT_SINGLE].cbCardTypeCount;
				cbBombCount[cbCount] = AnalyseOutCardTypeResult[AI_CT_BOMB_TW].cbCardTypeCount+AnalyseOutCardTypeResult[AI_CT_BOMB].cbCardTypeCount+AnalyseOutCardTypeResult[AI_CT_TONG_HUA_SHUN].cbCardTypeCount;
			}
			cbBombCount[cbCount] += OutCardTypeResultTemp[cbCount].cbCardTypeCount;	
			cbCount++;
		}
	}

	//ͳ�ƽ��
	if (cbCount > 0)
	{
		ZeroMemory(&AnalyseOutCardTypeResult, sizeof(AnalyseOutCardTypeResult));
		AnalyseOutCardTypeNomal(cbHandData, cbHandCount, AnalyseOutCardTypeResult);
		BYTE cbHandBoobCount = AnalyseOutCardTypeResult[AI_CT_BOMB_TW].cbCardTypeCount+AnalyseOutCardTypeResult[AI_CT_BOMB].cbCardTypeCount+AnalyseOutCardTypeResult[AI_CT_TONG_HUA_SHUN].cbCardTypeCount;
		BYTE cbMaxTongHuaCount = 0;
		BYTE cbMaxSingleCount = MAX_COUNT;
		BYTE cbIndex = MAX_COUNT;
		for (BYTE i=0; i<cbCount; i++)
		{
			//ʣ��ĵ��ŵĸ���<= 3 ����< ԭ���ĸ�����
			//ը������ + ͬ��˳����>=ԭ���ģ�	
			if ((AnalyseOutCardTypeResult[AI_CT_SINGLE].cbCardTypeCount>=cbSingleCount[i]||cbSingleCount[i]<=3) && cbHandBoobCount<cbBombCount[i])
			{
				if (OutCardTypeResultTemp[i].cbCardTypeCount>cbMaxTongHuaCount || (OutCardTypeResultTemp[i].cbCardTypeCount==cbMaxTongHuaCount&&cbSingleCount[i]<cbMaxSingleCount))
				{
					cbIndex  = i;
					cbMaxSingleCount = cbSingleCount[i];
					cbMaxTongHuaCount = OutCardTypeResultTemp[i].cbCardTypeCount;
				}
			}
		}
		
		if (cbIndex != MAX_COUNT)
		{
			CopyMemory(&CardTypeResult, &OutCardTypeResultTemp[cbIndex], sizeof(OutCardTypeResultTemp[cbIndex]));

			//��֤���
			for (BYTE i=0; i<CardTypeResult.cbCardTypeCount; i++)
			{
				CheckOutCardRightType(CardTypeResult.cbEachHandCardData[i], CardTypeResult.cbEachHandCardCount[i], AI_CT_TONG_HUA_SHUN);
			}
			return true;
		}
	}

	return false;
}

//��������(��ͬ��˳)
void CAILogic::AnalyseOutCardTypeNomal(BYTE const cbHandCardData[], BYTE const cbHandCardCount, tagOutCardTypeResult CardTypeResult[TYPE_COUNT/*+1*/])
{
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult[0])*TYPE_COUNT);

	BYTE cbCardDataTemp[MAX_COUNT] = {0};
	BYTE cbCardCountTemp = cbHandCardCount;
	//�����˿ˣ���ֹ����ʱ�ı��˿�
	CopyMemory(cbCardDataTemp, cbHandCardData, cbHandCardCount);
	SortCardList(cbCardDataTemp, cbHandCardCount, ST_ORDER);

	//�����˿�
	BYTE cbMagicCard = MakeCardData(m_cbMainValue-1, 2);
	BYTE cbRemoveData[MAX_COUNT] = {0};
	BYTE cbRemoveCount = 0;
	tagAnalyseResult AnalyseResult;
	//�����˿�
	AnalysebCardData(cbCardDataTemp, cbCardCountTemp, AnalyseResult);

	//����
	{
		int iMagicCount = 0;
		BYTE cbMainCount = 0;
		for (BYTE i=0; i<cbCardCountTemp; i++)
		{
			BYTE cbValue = GetCardValue(cbCardDataTemp[i]);
			BYTE cbColor = GetCardColor(cbCardDataTemp[i]);
			if (cbValue == m_cbMainValue)
			{
				cbMainCount++;
				if (cbColor == 0x20)
				{
					iMagicCount++;
				}	
			}
		}

		//���ԭ������ը���Ͳ���Ҫȥ�����
		if (cbMainCount==4 && iMagicCount==1)
		{
			iMagicCount = 0;
		}
		else if (cbMainCount-4>0 && iMagicCount>0)
		{
			iMagicCount = __min((cbMainCount-4), iMagicCount); 
		}
		//�Ƴ��仯��
		if (iMagicCount > 0)
		{	
			for (int i=0; i<iMagicCount; i++)
			{
				RemoveCard(&cbMagicCard, 1, cbCardDataTemp, cbCardCountTemp);
				cbCardCountTemp--;
			}
		}
		
		//��ȱ
		// 1��˳��/ͬ��˳��3�����ϵĵ���˳�ӣ���
		{
			do 
			{
				tagOutCardResult SingleLinkOutSesult;
				int iFind = SerarchSingleLink(cbCardDataTemp, cbCardCountTemp, iMagicCount, SingleLinkOutSesult);
				if (iFind==0) break;

				//ͳ������
				BYTE cbResultType = GetMagicCardType(SingleLinkOutSesult.cbResultCard, SingleLinkOutSesult.cbCardCount);
				ASSERT(cbResultType==AI_CT_SINGLE_LINK || cbResultType==AI_CT_TONG_HUA_SHUN);

				//ͳ�ƽ��			
				CardTypeResult[cbResultType].cbCardType = cbResultType;
				CopyMemory(CardTypeResult[cbResultType].cbEachHandCardData[CardTypeResult[cbResultType].cbCardTypeCount], SingleLinkOutSesult.cbResultCard, SingleLinkOutSesult.cbCardCount);
				CardTypeResult[cbResultType].cbEachHandCardCount[CardTypeResult[cbResultType].cbCardTypeCount] = 5;
				CardTypeResult[cbResultType].cbCardTypeCount++;
				CheckOutCardInHand(cbHandCardData, cbHandCardCount, SingleLinkOutSesult.cbResultCard, SingleLinkOutSesult.cbCardCount);

				if (iFind != 3)
				{
					ASSERT(iMagicCount >= iFind);
					iMagicCount -= (int)iFind;

					for (BYTE i=0; i<iFind; i++)
					{
						RemoveCard(&cbMagicCard, 1, SingleLinkOutSesult.cbResultCard, SingleLinkOutSesult.cbCardCount);
						SingleLinkOutSesult.cbCardCount --;
					}
				}

				//�Ƴ�����
				RemoveCard(SingleLinkOutSesult.cbResultCard, SingleLinkOutSesult.cbCardCount, cbCardDataTemp, cbCardCountTemp);
				cbCardCountTemp -= SingleLinkOutSesult.cbCardCount;
			} while (true);	
		}

		//�����˿�(�Ӵ�С����)
		AnalysebCardData(cbCardDataTemp, cbCardCountTemp, AnalyseResult);

		//�б仯���ƣ����
		if (iMagicCount > 0)
		{		
			// 1��3�����ը����2��3������ѡ��С��3����
			BYTE cbTempCount = AnalyseResult.cbBlockCount[2];
			while(iMagicCount>0 && cbTempCount>0)
			{
				CardTypeResult[AI_CT_BOMB].cbCardType = AI_CT_BOMB;
				CopyMemory(CardTypeResult[AI_CT_BOMB].cbEachHandCardData[CardTypeResult[AI_CT_BOMB].cbCardTypeCount],&AnalyseResult.cbCardData[2][(cbTempCount-1)*3],3);
				CardTypeResult[AI_CT_BOMB].cbEachHandCardData[CardTypeResult[AI_CT_BOMB].cbCardTypeCount][3] = cbMagicCard;
				CardTypeResult[AI_CT_BOMB].cbEachHandCardCount[CardTypeResult[AI_CT_BOMB].cbCardTypeCount] = 4;
				CardTypeResult[AI_CT_BOMB].cbCardTypeCount++;

				//��֤
				CheckOutCardRightType(CardTypeResult[AI_CT_BOMB].cbEachHandCardData[CardTypeResult[AI_CT_BOMB].cbCardTypeCount-1], 4, AI_CT_BOMB);

				//�Ƴ����˿�
				CopyMemory(&cbRemoveData[cbRemoveCount], &AnalyseResult.cbCardData[2][(cbTempCount-1)*3], 3);
				cbRemoveCount += 3;
				cbTempCount --;
				iMagicCount --;
			}

			//����ʣ�����
			if (cbRemoveCount > 0)
			{
				RemoveCard(cbRemoveData, cbRemoveCount, cbCardDataTemp, cbCardCountTemp);
				cbCardCountTemp -= cbRemoveCount;
			}
		}	

		//��ȱ
		//3����ȱһ�ŵģ�������10���ϵ���Ϊ�仯�����У�(����ɫ�Ķ��Ӳ���������)
		//(��ԭ���е�3�����޳���)
		AnalysebCardData(cbCardDataTemp, cbCardCountTemp, AnalyseResult);
		if (AnalyseResult.cbBlockCount[1] >= 3)
		{
			BYTE cbLink = 0;
			BYTE cbResultData[MAX_COUNT] = {0};
			BYTE cbResultCount = 0;
			BYTE cbTempData[MAX_COUNT] = {0};
			BYTE cbTempCount = AnalyseResult.cbBlockCount[1]*2;
			CopyMemory(cbTempData, AnalyseResult.cbCardData[1], cbTempCount);
			//����˳��
			SortCardList(cbTempData, cbTempCount, ST_VALUE);

			//�����Ϊ2��223344 ��334455 ͬʱ���ڣ�ѡ�����334455
			bool bMainLink = false;
			BYTE cbMainData[6] = {0};
			BYTE cbMainCount = 0;
			for (int i=(AnalyseResult.cbBlockCount[1]-1); i>0; i--)
			{
				BYTE cbFirstValue = GetCardValue(cbTempData[i*2]);
				//(����ɫ�Ķ��Ӳ���������)�ų���С���Ķ���
				if (m_cbMainValue==2 && cbFirstValue==1) continue;
				if (cbTempData[i*2]==0x4E || cbTempData[i*2]==0x4F) break;

				CopyMemory(&cbResultData[0], &cbTempData[i*2], 2);
				cbResultCount = 2;
				cbLink = 1;

				for (int j=i-1; j>=0; j--)
				{
					//(����ɫ�Ķ��Ӳ���������)
					if (cbTempData[j*2]==0x4E || cbTempData[j*2]==0x4F) break;
					if (cbFirstValue+cbLink != GetCardValue(cbTempData[j*2])) break;

					CopyMemory(&cbResultData[cbResultCount], &cbTempData[j*2], 2);
					cbResultCount += 2;
					cbLink ++;

					if (cbLink == 3) break;
				}

				//ͳ�ƽ��
				if (cbLink == 3)
				{
					//��֤
					CheckOutCardRightType(cbResultData, cbResultCount, AI_CT_DOUBLE_LINK);

					if (bMainLink)
					{
						if (m_cbMainValue+1 == cbFirstValue)
						{
							ASSERT(CardTypeResult[AI_CT_DOUBLE_LINK].cbCardTypeCount>0);
							CardTypeResult[AI_CT_DOUBLE_LINK].cbCardTypeCount--;
						}
						else
						{
							cbMainCount = 0;
							//�Ƴ�����
							RemoveCard(cbMainData, cbMainCount, cbCardDataTemp, cbCardCountTemp);
							cbCardCountTemp -= cbMainCount;
						}

						bMainLink = false;
					}
					CardTypeResult[AI_CT_DOUBLE_LINK].cbCardType = AI_CT_DOUBLE_LINK;
					CardTypeResult[AI_CT_DOUBLE_LINK].cbEachHandCardCount[CardTypeResult[AI_CT_DOUBLE_LINK].cbCardTypeCount] = 6;
					CopyMemory(CardTypeResult[AI_CT_DOUBLE_LINK].cbEachHandCardData[CardTypeResult[AI_CT_DOUBLE_LINK].cbCardTypeCount], cbResultData, cbResultCount);
					CardTypeResult[AI_CT_DOUBLE_LINK].cbCardTypeCount++;

					if (cbFirstValue == m_cbMainValue)
					{
						bMainLink = true;
						cbMainCount = cbResultCount;
						CopyMemory(cbMainData, cbResultData, cbResultCount);
					}
					//��10���µ����Թ̶�����Ķ��ӿ��Ի���
					if (GetCardValue(cbResultData[cbResultCount-1])<10 && !bMainLink)
					{
						//�Ƴ�����
						RemoveCard(cbResultData, cbResultCount, cbCardDataTemp, cbCardCountTemp);
						cbCardCountTemp -= cbResultCount;

						AnalysebCardData(cbCardDataTemp, cbCardCountTemp, AnalyseResult);
						cbTempCount = AnalyseResult.cbBlockCount[1];
						CopyMemory(cbTempData, AnalyseResult.cbCardData[1], cbTempCount*2);
						i = cbTempCount;
					}
				}
			}
			//}

			//��ȱ����
			if (iMagicCount > 0)
			{
				AnalysebCardData(cbCardDataTemp, cbCardCountTemp, AnalyseResult);
				if (AnalyseResult.cbBlockCount[1]>=2 && iMagicCount>0 && AnalyseResult.cbBlockCount[0]>=0)
				{
					BYTE cbLink = 0;
					BYTE cbResultData[MAX_COUNT] = {0};
					BYTE cbResultCount = 0;
					BYTE cbNeed = 0;
					BYTE cbCount1= 0;  //�������Ƕ��ӻ��ǵ���
					BYTE cbCount2= 0;  //�������Ƕ��ӻ��ǵ���

					////���Ӻ͵���
					cbTempCount = AnalyseResult.cbBlockCount[1]*2;
					CopyMemory(cbTempData, AnalyseResult.cbCardData[1], cbTempCount);
					CopyMemory(&cbTempData[cbTempCount], &AnalyseResult.cbCardData[0], AnalyseResult.cbBlockCount[0]);
					cbTempCount += AnalyseResult.cbBlockCount[0];
					SortCardList(cbTempData,cbTempCount,ST_VALUE);

					for (int i=cbTempCount-1; i>0; i--)
					{
						BYTE cbFirstValue = GetCardValue(cbTempData[i]);
						//(����ɫ�Ķ��Ӳ���������)�ų���С���Ķ���
						if ((m_cbMainValue==2&&cbFirstValue==1) || (cbTempData[i]==0x4E||cbTempData[i]==0x4F))
						{
							continue;
						}

						//������ͬ����
						cbCount1 = 1;
						if ((i-1) >= 0)
						{
							if (GetCardValue(cbTempData[i]) == GetCardValue(cbTempData[i-1]))
							{
								cbCount1 = 2;
							}
							else
							{
								cbCount1 = 1;
							}
						}
						cbLink = 1;
						CopyMemory(&cbResultData[0], &cbTempData[i-(cbCount1-1)], cbCount1);
						cbResultCount = cbCount1;
						cbNeed = cbCount1==2 ? 0:1;

						for (int j=i-cbCount1; j>=0; j--)
						{
							BYTE cbSecondValue = GetCardValue(cbTempData[j]);
							if (cbTempData[j]==0x4E ||cbTempData[j]==0x4F)
							{
								continue;
							}

							//������ͬ����
							cbCount2 = 1;
							if ((j-1) >= 0)
							{
								if (GetCardValue(cbTempData[j]) == GetCardValue(cbTempData[j-1]))
								{
									cbCount2 = 2;
								}
								else
								{
									cbCount2 = 1;
								}
							}

							if ((cbFirstValue+cbLink!=cbSecondValue) || (cbCount2==1&&cbNeed==1)) break;

							if (cbCount2 == 1)
							{
								cbNeed ++;
							}

							CopyMemory(&cbResultData[cbResultCount], &cbTempData[j-(cbCount2-1)], cbCount2);
							cbResultCount += cbCount2;
							cbLink++;

							if (cbLink == 3)
							{
								break;
							}

							//���ظ���������
							if (cbCount2 == 2) j--;
						}

						//ͳ�ƽ��
						if (cbLink == 3)
						{
							//������С��10 �Ĺ̶�
							if (GetCardValue(cbResultData[cbResultCount-1]) < 10)
							{
								RemoveCard(cbResultData, cbResultCount, cbCardDataTemp, cbCardCountTemp);
								cbCardCountTemp -= cbResultCount;
								RemoveCard(cbResultData, cbResultCount, cbTempData, cbTempCount);
								cbTempCount -= cbResultCount;
								SortCardList(cbTempData, cbTempCount, ST_VALUE);
								i = cbTempCount;
							}

							//��ȱ
							if (cbNeed == 1)
							{
								cbResultData[cbResultCount] = cbMagicCard;
								cbResultCount++;
								iMagicCount--;
							}				

							//��֤
							CheckOutCardRightType(cbResultData, cbResultCount, AI_CT_DOUBLE_LINK);

							CardTypeResult[AI_CT_DOUBLE_LINK].cbCardType = AI_CT_DOUBLE_LINK;
							CardTypeResult[AI_CT_DOUBLE_LINK].cbEachHandCardCount[CardTypeResult[AI_CT_DOUBLE_LINK].cbCardTypeCount] = 6;
							CopyMemory(CardTypeResult[AI_CT_DOUBLE_LINK].cbEachHandCardData[CardTypeResult[AI_CT_DOUBLE_LINK].cbCardTypeCount], cbResultData, cbResultCount);
							CardTypeResult[AI_CT_DOUBLE_LINK].cbCardTypeCount ++;

							ASSERT(iMagicCount>=0);
							if (iMagicCount==0 || cbTempCount<5) break;
						}

						//���ظ���������
						if (cbCount1 == 2) i--;
					}
				}

				//QKA
				if (cbTempCount>5 && iMagicCount>0)
				{
					cbResultCount = 0;
					for (BYTE i=0; i<cbTempCount; i++)
					{
						BYTE cbValue = GetCardValue(cbTempData[i]);
						if (cbValue==1 || cbValue==12 || cbValue==13)
						{
							cbResultData[cbResultCount] = cbTempData[i];
							cbResultCount ++;
						}

						if (cbResultCount == 5)
						{
							cbResultData[cbResultCount] = cbMagicCard;
							cbResultCount++;
							iMagicCount--;
							ASSERT(iMagicCount >= 0);

							//��֤
							CheckOutCardRightType(cbResultData, cbResultCount, AI_CT_DOUBLE_LINK);

							CardTypeResult[AI_CT_DOUBLE_LINK].cbCardType = AI_CT_DOUBLE_LINK;
							CardTypeResult[AI_CT_DOUBLE_LINK].cbEachHandCardCount[CardTypeResult[AI_CT_DOUBLE_LINK].cbCardTypeCount] = 6;
							CopyMemory(CardTypeResult[AI_CT_DOUBLE_LINK].cbEachHandCardData[CardTypeResult[AI_CT_DOUBLE_LINK].cbCardTypeCount], cbResultData, cbResultCount);
							CardTypeResult[AI_CT_DOUBLE_LINK].cbCardTypeCount++;
						}
					}
				}
			}

		}
		//�����û����;
		if (iMagicCount > 0)
		{
			for (BYTE i=0; i<iMagicCount; i++)
			{
				cbCardDataTemp[cbCardCountTemp] = cbMagicCard;
				cbCardCountTemp ++;
			}
		}
	}

	//����ը
	{
		BYTE cbResultData[4] = {0};
		BYTE cbResultCount = 0;
		for (BYTE i=0; i<cbCardCountTemp; i++)
		{
			if (cbCardDataTemp[i]==0x4E || cbCardDataTemp[i]==0x4F)
			{
				cbResultData[cbResultCount] = cbCardDataTemp[i];
				cbResultCount ++;
			}
			else
			{
				break;
			}
		}

		if (cbResultCount >= 4)
		{
			CardTypeResult[AI_CT_BOMB_TW].cbCardType = AI_CT_BOMB_TW;
			CardTypeResult[AI_CT_BOMB_TW].cbEachHandCardCount[CardTypeResult[AI_CT_BOMB_TW].cbCardTypeCount] = cbResultCount;
			CopyMemory(CardTypeResult[AI_CT_BOMB_TW].cbEachHandCardData[CardTypeResult[AI_CT_BOMB_TW].cbCardTypeCount], &cbResultData, cbResultCount);
			CardTypeResult[AI_CT_BOMB_TW].cbCardTypeCount++;

			VERIFY(GetCardType(CardTypeResult[AI_CT_BOMB_TW].cbEachHandCardData[CardTypeResult[AI_CT_BOMB_TW].cbCardTypeCount-1],cbResultCount)==AI_CT_BOMB_TW);
			RemoveCard(cbResultData, cbResultCount, cbCardDataTemp, cbCardCountTemp);
			cbCardCountTemp -= cbResultCount;
		}
	}

	//��˳�ӣ�ͬ��˳������������ͣ�(�Ӵ�С)
	SortCardList(cbCardDataTemp, cbCardCountTemp, ST_ORDER);
	AnalysebCardData(cbCardDataTemp, cbCardCountTemp, AnalyseResult);
	//3��
	{
		if (AnalyseResult.cbBlockCount[2] > 1)
		{
			BYTE cbLink = 0;
			BYTE cbResultData[MAX_COUNT]={0};
			BYTE cbResultCount = 0;
			BYTE cbTempData[MAX_COUNT]={0};
			BYTE cbTempCount = AnalyseResult.cbBlockCount[2]*3;
			CopyMemory(cbTempData, AnalyseResult.cbCardData[2], cbTempCount);
			SortCardList(cbTempData, cbTempCount, ST_VALUE);

			//�����Ϊ2��222333 ��333444 ͬʱ���ڣ�ѡ�����333444
			bool bMainLink = false;
			BYTE cbMainCount = 0;
			for (int i=AnalyseResult.cbBlockCount[2]-1; i>0; i--)
			{
				BYTE cbFirstValue = GetCardValue(cbTempData[i*3]);
				//aaa222���ᳫ����
				if (m_cbMainValue==2 && cbFirstValue==1) continue;
				if (cbTempData[i*3]==0x4E || cbTempData[i*3]==0x4F) break;
				CopyMemory(&cbResultData, &cbTempData[i*3], 3);
				cbResultCount = 3;
				cbLink = 1;
				for (int j=i-1; j>=0; j--)
				{
					if (cbTempData[j*3]==0x4E || cbTempData[j*3]==0x4F) break;
					BYTE cbSecondValue = GetCardValue(cbTempData[j*3]); 
					if (cbFirstValue+cbLink!=cbSecondValue) break;

					CopyMemory(&cbResultData[cbResultCount], &cbTempData[j*3], 3);
					cbResultCount += 3;
					cbLink ++;
					break;
				}

				//�洢����
				if (cbLink == 2)
				{
					if (bMainLink)
					{
						if (m_cbMainValue+1 == cbFirstValue)
						{
							ASSERT(CardTypeResult[AI_CT_THREE_LINK].cbCardTypeCount > 0);
							CardTypeResult[AI_CT_THREE_LINK].cbCardTypeCount--;
						}
						bMainLink = false;
					}

					ASSERT(cbResultCount==6);
					//��֤
					CheckOutCardRightType(cbResultData, cbResultCount, AI_CT_THREE_LINK);

					CardTypeResult[AI_CT_THREE_LINK].cbCardType = AI_CT_THREE_LINK;
					CardTypeResult[AI_CT_THREE_LINK].cbEachHandCardCount[CardTypeResult[AI_CT_THREE_LINK].cbCardTypeCount] = 6;
					CopyMemory(CardTypeResult[AI_CT_THREE_LINK].cbEachHandCardData[CardTypeResult[AI_CT_THREE_LINK].cbCardTypeCount], cbResultData, cbResultCount);
					CardTypeResult[AI_CT_THREE_LINK].cbCardTypeCount++;

					if (cbFirstValue == m_cbMainValue)
					{
						bMainLink = true;
					}
				}
			}

			//KA
			ZeroMemory(cbResultData, sizeof(cbResultData));
			cbResultCount = 0;
			for (BYTE i=0; i<AnalyseResult.cbBlockCount[2]; i++)
			{
				BYTE cbValue = GetCardValue(AnalyseResult.cbCardData[2][i*3]);
				if (cbValue==1 || cbValue==13)
				{
					CopyMemory(&cbResultData[cbResultCount], &AnalyseResult.cbCardData[2][i*3], 3);
					cbResultCount += 3;
				}
			}
			if (cbResultCount == 6)
			{
				//��֤
				CheckOutCardRightType(cbResultData, cbResultCount, AI_CT_THREE_LINK);

				CardTypeResult[AI_CT_THREE_LINK].cbCardType = AI_CT_THREE_LINK;
				CardTypeResult[AI_CT_THREE_LINK].cbEachHandCardCount[CardTypeResult[AI_CT_THREE_LINK].cbCardTypeCount] = 6;
				CopyMemory(CardTypeResult[AI_CT_THREE_LINK].cbEachHandCardData[CardTypeResult[AI_CT_THREE_LINK].cbCardTypeCount], cbResultData, cbResultCount);
				CardTypeResult[AI_CT_THREE_LINK].cbCardTypeCount++;
			}
		}
	}

	//3��2
	{
		//��С����
		if (AnalyseResult.cbBlockCount[1]>0 && AnalyseResult.cbBlockCount[2]>0)
		{
			for (int i=AnalyseResult.cbBlockCount[2]-1; i>=0; i--)
			{
				CardTypeResult[AI_CT_THREE_DOUBLE].cbCardType = AI_CT_THREE_DOUBLE;
				CardTypeResult[AI_CT_THREE_DOUBLE].cbEachHandCardCount[CardTypeResult[AI_CT_THREE_DOUBLE].cbCardTypeCount] = 5;
				CopyMemory(&CardTypeResult[AI_CT_THREE_DOUBLE].cbEachHandCardData[CardTypeResult[AI_CT_THREE_DOUBLE].cbCardTypeCount][0], &AnalyseResult.cbCardData[2][i*3], 3);
				CopyMemory(&CardTypeResult[AI_CT_THREE_DOUBLE].cbEachHandCardData[CardTypeResult[AI_CT_THREE_DOUBLE].cbCardTypeCount][3], &AnalyseResult.cbCardData[1][(AnalyseResult.cbBlockCount[1]-1)*2], 2);		
				CardTypeResult[AI_CT_THREE_DOUBLE].cbCardTypeCount ++;

				VERIFY(GetCardType(CardTypeResult[AI_CT_THREE_DOUBLE].cbEachHandCardData[CardTypeResult[AI_CT_THREE_DOUBLE].cbCardTypeCount-1],5)==AI_CT_THREE_DOUBLE);
			}
		}
	}
	//����
	{
		//��С����
		if (AnalyseResult.cbBlockCount[2] > 0)
		{
			for (int i=AnalyseResult.cbBlockCount[2]-1; i>=0; i--)
			{
				CardTypeResult[AI_CT_THREE].cbCardType = AI_CT_THREE;
				CardTypeResult[AI_CT_THREE].cbEachHandCardCount[CardTypeResult[AI_CT_THREE].cbCardTypeCount] = 3;
				CopyMemory(CardTypeResult[AI_CT_THREE].cbEachHandCardData[CardTypeResult[AI_CT_THREE].cbCardTypeCount], &AnalyseResult.cbCardData[2][i*3], 3);
				CardTypeResult[AI_CT_THREE].cbCardTypeCount++;

				VERIFY(GetCardType(CardTypeResult[AI_CT_THREE].cbEachHandCardData[CardTypeResult[AI_CT_THREE].cbCardTypeCount-1],3)==AI_CT_THREE);
			}
		}
	}
	//����
	{
		//��С����
		if (AnalyseResult.cbBlockCount[1] > 0)
		{
			for (int i=AnalyseResult.cbBlockCount[1]-1; i>=0; i--)
			{
				CardTypeResult[AI_CT_DOUBLE].cbCardType = AI_CT_DOUBLE;
				CardTypeResult[AI_CT_DOUBLE].cbEachHandCardCount[CardTypeResult[AI_CT_DOUBLE].cbCardTypeCount] = 2;
				CopyMemory(CardTypeResult[AI_CT_DOUBLE].cbEachHandCardData[CardTypeResult[AI_CT_DOUBLE].cbCardTypeCount], &AnalyseResult.cbCardData[1][i*2], 2);
				CardTypeResult[AI_CT_DOUBLE].cbCardTypeCount++;

				VERIFY(GetCardType(CardTypeResult[AI_CT_DOUBLE].cbEachHandCardData[CardTypeResult[AI_CT_DOUBLE].cbCardTypeCount-1],2)==AI_CT_DOUBLE);
			}
		}
	}
	//����
	{
		//��С����
		if (AnalyseResult.cbBlockCount[0] > 0)
		{
			for (int i=AnalyseResult.cbBlockCount[0]-1; i>=0; i--)
			{
				CardTypeResult[AI_CT_SINGLE].cbCardType = AI_CT_SINGLE;
				CardTypeResult[AI_CT_SINGLE].cbEachHandCardCount[CardTypeResult[AI_CT_SINGLE].cbCardTypeCount]=1;
				CopyMemory(CardTypeResult[AI_CT_SINGLE].cbEachHandCardData[CardTypeResult[AI_CT_SINGLE].cbCardTypeCount], &AnalyseResult.cbCardData[0][i], 1);
				CardTypeResult[AI_CT_SINGLE].cbCardTypeCount++;

				VERIFY(GetCardType(CardTypeResult[AI_CT_SINGLE].cbEachHandCardData[CardTypeResult[AI_CT_SINGLE].cbCardTypeCount-1],1)==AI_CT_SINGLE);
			}
		}
	}

	//ը��
	{
		//��С����
		for (BYTE cbType=3; cbType<=7; cbType++)
		{
			BYTE cbCount = cbType+1;
			if (AnalyseResult.cbBlockCount[cbType] > 0)
			{
				for (int i=AnalyseResult.cbBlockCount[cbType]-1; i>=0; i--)
				{
					CardTypeResult[AI_CT_BOMB].cbCardType = AI_CT_BOMB;
					CardTypeResult[AI_CT_BOMB].cbEachHandCardCount[CardTypeResult[AI_CT_BOMB].cbCardTypeCount] = cbCount;
					CopyMemory(CardTypeResult[AI_CT_BOMB].cbEachHandCardData[CardTypeResult[AI_CT_BOMB].cbCardTypeCount], &AnalyseResult.cbCardData[cbType][i*cbCount], cbCount);
					CardTypeResult[AI_CT_BOMB].cbCardTypeCount++;

					VERIFY(GetCardType(CardTypeResult[AI_CT_BOMB].cbEachHandCardData[CardTypeResult[AI_CT_BOMB].cbCardTypeCount-1],cbCount)==AI_CT_BOMB);
				}
			}
		}	
	}

	//��֤���
	for (BYTE cbType=AI_CT_SINGLE; cbType<=AI_CT_BOMB_TW; cbType++)
	{
		if (CardTypeResult[cbType].cbCardTypeCount > 0)
		{
			for (BYTE i=0; i<CardTypeResult[cbType].cbCardTypeCount; i++)
			{
				//��֤
				CheckOutCardRightType(CardTypeResult[cbType].cbEachHandCardData[i], CardTypeResult[cbType].cbEachHandCardCount[i], cbType);
			}
		}
	}
}

//����ը�Լ��˵�����
bool CAILogic::CheckWillOutCard(WORD wOutID, WORD wMeID, BYTE cbCard[], BYTE cbCount, BYTE cbTurnCard[], BYTE cbTurnCount)
{
	if (cbCount==0) return true;	

	BYTE cbMagic[MAX_COUNT]={0}, cbMagicTurn[MAX_COUNT]={0};
	MagicCardData(cbTurnCard, cbTurnCount, cbMagicTurn);
	MagicCardData(cbCard, cbCount, cbMagic);

	if (!CompareCard(cbMagicTurn,cbMagic,cbTurnCount,cbCount) && cbTurnCount!=0)
	{
		ASSERT(FALSE);
	}

	BYTE cbType = GetCardType(cbMagic,cbCount);
	if (cbType==AI_CT_BOMB || cbType==AI_CT_BOMB_TW || cbType==AI_CT_TONG_HUA_SHUN)
	{
		return true;
	}
	return true;
}

//����˳��
int CAILogic::SerarchSingleLink(BYTE cbCardData[], BYTE cbCardCount, int iMagicCount, tagOutCardResult &OutCardResult)
{	
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));

	int iFind = 0;
	BYTE cbMagicData = MakeCardData(m_cbMainValue-1, 2);
	SortCardList(cbCardData, cbCardCount, ST_VALUE);

	//��ȱ
	// 1��˳��/ͬ��˳��4�����ϵĵ���˳�ӣ���
	tagAnalyseResult AnalyseResult;
	AnalysebCardData(cbCardData, cbCardCount, AnalyseResult);
	if (AnalyseResult.cbBlockCount[0] >= 3)
	{
		BYTE cbNeedValue[10] = {0};
		BYTE cbNeedCount = 0;
		BYTE cbNecessaryCount =0;
		BYTE cbNeedColor = 0;
		BYTE cbResultData[5] = {0};
		BYTE cbResultCount = 0;
		BYTE cbTempData[MAX_COUNT] = {0};
		BYTE cbTempCount = AnalyseResult.cbBlockCount[0];
		CopyMemory(cbTempData, AnalyseResult.cbCardData[0], cbTempCount);
		SortCardList(cbTempData, cbTempCount, ST_VALUE);

		BYTE cbLink = 0;
		BYTE cbNeed = 0;
		for (int i=(cbTempCount-1); i>0; i--)
		{
			//ȥ����С��
			if (cbTempData[i]==0x4E || cbTempData[i]==0x4F) break;

			BYTE cbFirstValue = GetCardValue(cbTempData[i]);
			cbLink = 1;
			cbNeedCount = 0;
			//ͳ������
			cbResultData[0] = cbTempData[i];
			cbResultCount = 1;
			cbNeed = 0;
			iFind = 0;

			for (int j=i-1; j>=0; j--)
			{
				if (cbTempData[j]==0x4E || cbTempData[j]==0x4F) break;
				BYTE cbSecondValue = GetCardValue(cbTempData[j]);
				if (cbFirstValue+cbLink+cbNeed != cbSecondValue)
				{
					//if(cbNeed >= iMagicCount) break;
					BYTE cbTempNeed = (cbSecondValue - GetCardValue(cbTempData[j+1])-1);
					//if (cbNeed > iMagicCount) break;
					if (cbNeed>2 || (cbLink+cbNeed+cbTempNeed+1)>5) break;
					cbNeed += cbTempNeed;
					for (int i=0; i<cbTempNeed; i++)
					{
						cbNeedValue[cbNeedCount] = cbFirstValue+cbLink+(cbNeed -cbTempNeed)+i;
						cbNeedCount++;
					}
				}

				//ͳ������
				cbResultData[cbResultCount] = cbTempData[j];
				cbResultCount++;
				cbLink++;

				if (cbLink==5 || cbLink+cbNeed==5) 
				{
					break;
				}
			}
			SortCardList(cbResultData, cbResultCount, ST_VALUE);
			//�м�ȱ�ٱ���Ҫ��ӵ�
			cbNecessaryCount = cbNeedCount;
			ASSERT(cbNecessaryCount == cbNeed);
			//10~A
			if (cbLink>=3 && cbLink<5 && cbFirstValue>=10 && GetCardValue(cbTempData[cbTempCount-1])==1)
			{
				//ͳ������
				cbResultData[cbResultCount] = cbTempData[cbTempCount-1];
				cbResultCount++;
				cbLink++;
				SortCardList(cbResultData, cbResultCount, ST_ORDER);
				cbNeed = 5-cbLink;
			}

			//����ȱ���������
			if (cbLink>=3 && cbLink<5 && cbNeed+cbLink<5)
			{
				//���Ҵ��
				BYTE cbMaxValue = GetCardValue(cbResultData[0]);
				for (BYTE k=1; k<=5-(cbNeed+cbLink); k++)
				{
					if (cbMaxValue+k <= 14)
					{
						if (cbMaxValue+k == 14)
						{
							cbNeedValue[cbNeedCount] = 1;
							cbNeedCount++;
						}
						else
						{
							cbNeedValue[cbNeedCount] = (cbMaxValue+k);
							cbNeedCount++;
						}
					}
				}
				//ѡ��С��
				BYTE cbMinValue = GetCardValue(cbResultData[cbResultCount-1]);
				for (BYTE k=1; k<=5-(cbNeed+cbLink); k++)
				{
					if (cbMinValue-k >= 1)
					{
						cbNeedValue[cbNeedCount] = (cbMinValue -k);
						cbNeedCount ++;
					}
				}
				cbNeed = 5 - cbLink;
			}
			//���Ҷ��ӻ�����3���е�һ��,5�����ϵ�ը��
			if (cbLink>=3 && cbLink!=5 && cbLink+cbNeed==5)
			{
				BYTE cbTemp[MAX_COUNT] = {0};
				BYTE cbCount = 0;
				for (BYTE i=AI_CT_DOUBLE-1; i<AI_CT_THREE; i++)
				{
					if (AnalyseResult.cbBlockCount[i] > 0)
					{
					   CopyMemory(&cbTemp[cbCount], &AnalyseResult.cbCardData[i], AnalyseResult.cbBlockCount[i]*(i+1));
					   cbCount += AnalyseResult.cbBlockCount[i]*(i+1);
					}
				}

				for (BYTE cbNum=4; cbNum<8; cbNum++)
				{
					if (AnalyseResult.cbBlockCount[cbNum] > 0)
					{
						CopyMemory(&cbTemp[cbCount], &AnalyseResult.cbCardData[cbNum], AnalyseResult.cbBlockCount[cbNum]*(cbNum+1));
						cbCount += AnalyseResult.cbBlockCount[cbNum]*(cbNum+1);
					}
				}
				if (cbCount > 0)
				{
					SortCardList(cbTemp, cbCount, ST_VALUE);
				}

				//�����˿�
				tagDistributing Distribute;
				AnalysebDistributing(cbTemp ,cbCount, Distribute);
				tagOutCardResult tagResult;
				CopyMemory(tagResult.cbResultCard, cbResultData, cbResultCount);
				tagResult.cbCardCount = cbResultCount;

				bool bSameColor = true;
				cbNeedColor = GetCardColor(cbResultData[0])>>4;
				for (BYTE i=1; i<cbResultCount; i++)
				{
					if (GetCardColor(cbResultData[i]) != GetCardColor(cbResultData[i-1]))
					{
						bSameColor = false;
						break;
					}
				}
				BYTE cbFindCount = 0;
				BYTE cbUnSameColor[5] = {0};
				BYTE cbUnSameColorCount = 0;
				for (BYTE i=0; i<cbNeedCount; i++)
				{
					if (Distribute.cbDistributing[cbNeedValue[i]-1][cbIndexCount] > 0)
					{
						if (bSameColor && Distribute.cbDistributing[cbNeedValue[i]-1][cbNeedColor]>0)
						{
							tagResult.cbResultCard[tagResult.cbCardCount] = MakeCardData(cbNeedValue[i]-1, cbNeedColor);
							tagResult.cbCardCount++;
						}
						else
						{
							for (BYTE k=0; k<4; k++)
							{
								if (Distribute.cbDistributing[cbNeedValue[i]-1][k] > 0)
								{
									tagResult.cbResultCard[tagResult.cbCardCount] = MakeCardData(cbNeedValue[i]-1,k);
									tagResult.cbCardCount ++;

									if (bSameColor)
									{
										cbUnSameColor[cbUnSameColorCount] = MakeCardData(cbNeedValue[i]-1,k);
										cbUnSameColorCount ++;
									}
									break;
								}
							}	
						}
						cbFindCount ++;
						if (tagResult.cbCardCount==5) break;
					}
					else if (i < cbNecessaryCount)
					{
						break;
					}
				}

				
				//�ɷ���ͬ��˳
				if (cbFindCount > 0)
				{
					if (bSameColor && cbLink+(cbFindCount-cbUnSameColorCount)+iMagicCount>=5 && GetCardType(tagResult.cbResultCard,tagResult.cbCardCount)!=AI_CT_TONG_HUA_SHUN)
					{
						if (cbUnSameColorCount != cbFindCount)
						{
							ASSERT(cbFindCount > cbUnSameColorCount);
							RemoveCard(cbUnSameColor, cbUnSameColorCount, tagResult.cbResultCard, tagResult.cbCardCount);
							tagResult.cbCardCount -= cbUnSameColorCount;

							CopyMemory(cbResultData, tagResult.cbResultCard, tagResult.cbCardCount);
							ASSERT(tagResult.cbCardCount>cbResultCount && cbNeed>=(cbFindCount-cbUnSameColorCount));
							cbResultCount = tagResult.cbCardCount;
							cbNeed -= (cbFindCount-cbUnSameColorCount);
							cbLink += (cbFindCount-cbUnSameColorCount);
						}
					}
					else
					{
						CopyMemory(cbResultData, tagResult.cbResultCard, tagResult.cbCardCount);
						ASSERT(tagResult.cbCardCount>cbResultCount && cbNeed>=cbFindCount);
						cbResultCount = tagResult.cbCardCount;
						cbNeed -= cbFindCount;
						cbLink += cbFindCount;
					}
				}	
			}
			if (cbLink==5 || (cbLink+cbNeed==5&&(int)cbNeed<=iMagicCount&&iMagicCount>0)) 
			{
				//ͳ������
				if (cbNeed > 0)
				{
					for (BYTE i=0; i<cbNeed; i++)
					{
						cbResultData[cbResultCount] = cbMagicData;
						cbResultCount++;
					}
					iFind = (int)cbNeed;
				}
				else
				{
					iFind = 3;
				}

				//��֤
				BYTE cbResultType = GetMagicCardType(cbResultData, cbResultCount);			
				if (cbResultType!=AI_CT_SINGLE_LINK && cbResultType!=AI_CT_TONG_HUA_SHUN) continue;
				if (iFind == 1 || iFind == 2)
				{
					iMagicCount -= iFind;
					ASSERT(iMagicCount >= 0);
				}

				ASSERT(cbResultType==AI_CT_SINGLE_LINK || cbResultType==AI_CT_TONG_HUA_SHUN);

				//ͳ�ƽ��		
				if (cbResultType==AI_CT_TONG_HUA_SHUN || (cbResultType==AI_CT_SINGLE_LINK&&cbNeed<=1))
				{
					OutCardResult.cbCardCount = cbResultCount;
					CopyMemory(OutCardResult.cbResultCard, cbResultData, cbResultCount);
				}
				else
				{
					iFind = 0;
				}
			}

			if (OutCardResult.cbCardCount > 0)
			{
				if (GetCardValue(OutCardResult.cbResultCard[0]) >= 10)
				{
					SortCardList(OutCardResult.cbResultCard, OutCardResult.cbCardCount, ST_ORDER);
				}
				else
				{
					SortCardList(OutCardResult.cbResultCard, OutCardResult.cbCardCount, ST_VALUE);
				}
				break;	
			}
		}
	}

	return iFind;
}

//��֤�Ƿ����Լ����е���
bool CAILogic::CheckOutCardInHand(const BYTE cbHandData[], const BYTE cbHandCount, BYTE cbOutCard[], BYTE cbOutCount)
{
	BYTE cbTempCard[MAX_COUNT]={0};
	BYTE cbTempCount = cbHandCount;
	CopyMemory(cbTempCard, cbHandData, cbHandCount);

	if (!RemoveCard(cbOutCard, cbOutCount, cbTempCard, cbTempCount))
	{
		ASSERT(false);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
void CAILogic::SearchFirstOutCard(WORD wMeChairID, const BYTE cbHandCardData[], BYTE cbHandCardCount, tagOutCardResult &OutCardResult)
{
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));

	const WORD wUpsideChairID = (wMeChairID+3)%GAME_PLAYER;
	const WORD wUnderChairID = (wMeChairID+1)%GAME_PLAYER;
	const WORD wFriendChairID = (wMeChairID+2)%GAME_PLAYER;

	BYTE cbHandData[MAX_COUNT] = {0};
	CopyMemory(cbHandData, cbHandCardData, cbHandCardCount);
	SortCardList(cbHandData, cbHandCardCount, ST_ORDER);
	
	//�������ϵ���
	tagOutCardTypeResult CardTypeResult[TYPE_COUNT];
	ZeroMemory(&CardTypeResult, sizeof(CardTypeResult));
	AnalyseOutCardType(cbHandData, cbHandCardCount, CardTypeResult);

	//1��һ���ܳ��ꣻ
	//����˿�
	if (GetMagicCardType(cbHandData,cbHandCardCount) != AI_CT_ERROR)
	{
		OutCardResult.cbCardCount = cbHandCardCount;
		CopyMemory(OutCardResult.cbResultCard, cbHandData, OutCardResult.cbCardCount);

		CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
		return;
	}
	//����Լ�����������ը������һ����
	{
		if (CardTypeResult[AI_CT_BOMB_TW].cbCardTypeCount>0)
		{
			BYTE cbHandCardDataTemp[MAX_COUNT]={0}, cbHandCardCountTemp=cbHandCardCount;
			CopyMemory(cbHandCardDataTemp, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
			RemoveCard(CardTypeResult[AI_CT_BOMB_TW].cbEachHandCardData[0], CardTypeResult[AI_CT_BOMB_TW].cbEachHandCardCount[0], cbHandCardDataTemp, cbHandCardCountTemp);
			cbHandCardCountTemp -= CardTypeResult[AI_CT_BOMB_TW].cbEachHandCardCount[0];
			if (GetMagicCardType(cbHandCardDataTemp, cbHandCardCountTemp) != AI_CT_ERROR)
			{
				OutCardResult.cbCardCount = CardTypeResult[AI_CT_BOMB_TW].cbEachHandCardCount[0];
				CopyMemory(OutCardResult.cbResultCard, CardTypeResult[AI_CT_BOMB_TW].cbEachHandCardData[0], sizeof(BYTE)*OutCardResult.cbCardCount);

				CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
				return;
			}
		}
	}

	//�Լ�������һըһ���ƣ�����м�����ֻʣһ��ը���Ƚ��Լ���ը�͵��˼ҵ�ը������Ҵ��ȳ�ը�����򽻸�������봦��
	{
		BYTE cbUnderCardType = GetMagicCardType(m_cbHandCardData[wUnderChairID], m_cbHandCardCount[wUnderChairID]);
		BYTE cbUpsideCardType = GetMagicCardType(m_cbHandCardData[wUpsideChairID], m_cbHandCardCount[wUpsideChairID]);
		
		//����м���������ֻʣһ��ը��,���Լ��������ǲ���һըһ����
		if (cbUnderCardType>=AI_CT_TONG_HUA_SHUN || cbUpsideCardType>=AI_CT_TONG_HUA_SHUN)		
		{
			BYTE cbBombCardType = AI_CT_ERROR;
			BYTE cbBombCardCount = 0;
			bool bIsConsiderOutBomb = false;
			for (BYTE i=AI_CT_TONG_HUA_SHUN; i<TYPE_COUNT; i++)
			{		
				if (CardTypeResult[i].cbCardTypeCount > 0)
				{
					cbBombCardCount += CardTypeResult[i].cbCardTypeCount;
					cbBombCardType = i;
				}
			}
			if (cbBombCardCount == 1)
			{
				BYTE cbHandCardDataTemp[MAX_COUNT]={0}, cbHandCardCountTemp=m_cbHandCardCount[wMeChairID];
				CopyMemory(cbHandCardDataTemp, m_cbHandCardData[wMeChairID], m_cbHandCardCount[wMeChairID]);
				RemoveCard(CardTypeResult[cbBombCardType].cbEachHandCardData[0], CardTypeResult[cbBombCardType].cbEachHandCardCount[0], cbHandCardDataTemp, cbHandCardCountTemp);
				cbHandCardCountTemp -= CardTypeResult[cbBombCardType].cbEachHandCardCount[0];
				if (GetMagicCardType(cbHandCardDataTemp, cbHandCardCountTemp) != AI_CT_ERROR)
				{
					bIsConsiderOutBomb = true;
				}
			}
			if (bIsConsiderOutBomb)   //���������������ˣ��Ƚ�ը�Ĵ�С
			{
				bool IsOutBomb = false;
				if (cbUnderCardType >= AI_CT_TONG_HUA_SHUN)
				{
					if (CompareMagicCard(m_cbHandCardData[wUnderChairID], CardTypeResult[cbBombCardType].cbEachHandCardData[0], m_cbHandCardCount[wUnderChairID], CardTypeResult[cbBombCardType].cbEachHandCardCount[0]))
					{
						IsOutBomb = true;
					}
					else
					{
						IsOutBomb = false;
					}
				}
				if (cbUpsideCardType >= AI_CT_TONG_HUA_SHUN)
				{
					if (CompareMagicCard(m_cbHandCardData[wUpsideChairID], CardTypeResult[cbBombCardType].cbEachHandCardData[0], m_cbHandCardCount[wUpsideChairID], CardTypeResult[cbBombCardType].cbEachHandCardCount[0]))
					{
						IsOutBomb = true;
					}
					else
					{
						IsOutBomb = false;
					}
				}

				if (IsOutBomb)  //�Լ���ը�������ǵĴ��ȳ�ը
				{
					CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbBombCardType].cbEachHandCardData[0], CardTypeResult[cbBombCardType].cbEachHandCardCount[0]);
					OutCardResult.cbCardCount = CardTypeResult[cbBombCardType].cbEachHandCardCount[0];
					return;
				}
			}

		}
	}
	//�¼Ҿ�ʣ���ź�ͬ��˳
	if (m_cbHandCardCount[wUnderChairID] == 6)
	{
		tagOutCardTypeResult UnderOutCardTypeResult[TYPE_COUNT];
		AnalyseOutCardType(m_cbHandCardData[wUnderChairID], m_cbHandCardCount[wUnderChairID], UnderOutCardTypeResult);
		bool bUnderOneAndTongHuaShun = UnderOutCardTypeResult[AI_CT_TONG_HUA_SHUN].cbCardTypeCount==1&&UnderOutCardTypeResult[AI_CT_SINGLE].cbCardTypeCount==1;
		bool bUnderDuiziAndZhaDan = UnderOutCardTypeResult[AI_CT_BOMB].cbCardTypeCount==1&&UnderOutCardTypeResult[AI_CT_DOUBLE].cbCardTypeCount==1;
		//����з�ֻʣ�µ��ź�ͬ��˳,��������
		if (bUnderOneAndTongHuaShun)
		{
			//���ǵ���
			for (BYTE i=AI_CT_DOUBLE; i<=AI_CT_THREE_DOUBLE; i++)
			{
				if (CardTypeResult[i].cbCardTypeCount > 0)
				{
					OutCardResult.cbCardCount = CardTypeResult[i].cbEachHandCardCount[0];
					CopyMemory(OutCardResult.cbResultCard, CardTypeResult[i].cbEachHandCardData[0], sizeof(BYTE)*OutCardResult.cbCardCount);
					CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
					return;
				}
			}
			//������
			for (int i=CardTypeResult[AI_CT_SINGLE].cbCardTypeCount-1; i>=0; i--)
			{
				OutCardResult.cbCardCount = CardTypeResult[AI_CT_SINGLE].cbEachHandCardCount[i];
				CopyMemory(OutCardResult.cbResultCard, CardTypeResult[AI_CT_SINGLE].cbEachHandCardData[i], sizeof(BYTE)*OutCardResult.cbCardCount);
				CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
				return;
			}
		}
		if (bUnderDuiziAndZhaDan)
		{
			//���Ƕ���
			for (BYTE i=AI_CT_SINGLE_LINK; i<=AI_CT_THREE_DOUBLE; i++)
			{
				if (CardTypeResult[i].cbCardTypeCount > 0)
				{
					OutCardResult.cbCardCount = CardTypeResult[i].cbEachHandCardCount[0];
					CopyMemory(OutCardResult.cbResultCard, CardTypeResult[i].cbEachHandCardData[0], sizeof(BYTE)*OutCardResult.cbCardCount);
					CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
					return;
				}
			}
			//������
			if (CardTypeResult[AI_CT_THREE].cbCardTypeCount > 0)
			{
				OutCardResult.cbCardCount = CardTypeResult[AI_CT_THREE].cbEachHandCardCount[0];
				CopyMemory(OutCardResult.cbResultCard, CardTypeResult[AI_CT_THREE].cbEachHandCardData[0], sizeof(BYTE)*OutCardResult.cbCardCount);
				CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
				return;
			}
		}
	}
	//�ѷ���ʣ6�Ų���ֻʣ���ź�ͬ��˳
	if (m_cbHandCardCount[wFriendChairID] == 6)
	{
		tagOutCardTypeResult FriendCardTypeResult[TYPE_COUNT];
		AnalyseOutCardType(m_cbHandCardData[wFriendChairID], m_cbHandCardCount[wFriendChairID], FriendCardTypeResult);
		if (FriendCardTypeResult[AI_CT_SINGLE].cbCardTypeCount==1 && FriendCardTypeResult[AI_CT_TONG_HUA_SHUN].cbCardTypeCount==1)
		{
			BYTE cbSingleCardData[MAX_COUNT]={0}, cbSingleCardCount=0;
			for (BYTE i=0; i<CardTypeResult[AI_CT_SINGLE].cbCardTypeCount; i++)
			{
				cbSingleCardData[cbSingleCardCount++] = CardTypeResult[AI_CT_SINGLE].cbEachHandCardData[i][0];
			}
			SortCardList(cbSingleCardData, cbSingleCardCount, ST_ORDER);
			for (int i=cbSingleCardCount-1; i>=0; i--)
			{
				if (GetCardLogicValue(cbSingleCardData[i]) < GetCardLogicValue(FriendCardTypeResult[AI_CT_SINGLE].cbEachHandCardData[0][0]))
				{
					OutCardResult.cbCardCount = 1;
					OutCardResult.cbResultCard[0] = cbSingleCardData[i];
					CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
					return;
				}
			}
		}
		if (FriendCardTypeResult[AI_CT_BOMB].cbCardTypeCount==1 && FriendCardTypeResult[AI_CT_DOUBLE].cbCardTypeCount==1)
		{
			for (BYTE i=0; i<CardTypeResult[AI_CT_DOUBLE].cbCardTypeCount; i++)
			{
				if (GetCardLogicValue(CardTypeResult[AI_CT_DOUBLE].cbEachHandCardData[i][0]) < GetCardLogicValue(FriendCardTypeResult[AI_CT_DOUBLE].cbEachHandCardData[0][0]))
				{
					OutCardResult.cbCardCount = CardTypeResult[AI_CT_DOUBLE].cbEachHandCardCount[i];
					CopyMemory(OutCardResult.cbResultCard, CardTypeResult[AI_CT_DOUBLE].cbEachHandCardData[i], sizeof(BYTE)*OutCardResult.cbCardCount);
					CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
					return;
				}
			}
		}
	}
	//�ϼҾ�ʣ���ź�ͬ��˳
	if (m_cbHandCardCount[wUpsideChairID] == 6)
	{
		tagOutCardTypeResult UpsideOutCardTypeResult[TYPE_COUNT];
		AnalyseOutCardType(m_cbHandCardData[wUpsideChairID], m_cbHandCardCount[wUpsideChairID], UpsideOutCardTypeResult);
		bool bUpsideOneAndTongHuaShun = UpsideOutCardTypeResult[AI_CT_TONG_HUA_SHUN].cbCardTypeCount==1&&UpsideOutCardTypeResult[AI_CT_SINGLE].cbCardTypeCount==1;
		bool bUpsideDuiziAndZhaDan = UpsideOutCardTypeResult[AI_CT_BOMB].cbCardTypeCount==1&&UpsideOutCardTypeResult[AI_CT_DOUBLE].cbCardTypeCount==1;
		//����з�ֻʣ�µ��ź�ͬ��˳,��������
		if (bUpsideOneAndTongHuaShun)
		{
			//���ǵ���
			for (BYTE i=AI_CT_DOUBLE; i<=AI_CT_THREE_DOUBLE; i++)
			{
				if (CardTypeResult[i].cbCardTypeCount > 0)
				{
					OutCardResult.cbCardCount = CardTypeResult[i].cbEachHandCardCount[0];
					CopyMemory(OutCardResult.cbResultCard, CardTypeResult[i].cbEachHandCardData[0], sizeof(BYTE)*OutCardResult.cbCardCount);
					CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
					return;
				}
			}
			//������
			for (int i=CardTypeResult[AI_CT_SINGLE].cbCardTypeCount-1; i>=0; i--)
			{
				OutCardResult.cbCardCount = CardTypeResult[AI_CT_SINGLE].cbEachHandCardCount[i];
				CopyMemory(OutCardResult.cbResultCard, CardTypeResult[AI_CT_SINGLE].cbEachHandCardData[i], sizeof(BYTE)*OutCardResult.cbCardCount);
				CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
				return;
			}
		}
		if (bUpsideDuiziAndZhaDan)
		{
			//���Ƕ���
			for (BYTE i=AI_CT_SINGLE_LINK; i<=AI_CT_THREE_DOUBLE; i++)
			{
				if (CardTypeResult[i].cbCardTypeCount > 0)
				{
					OutCardResult.cbCardCount = CardTypeResult[i].cbEachHandCardCount[0];
					CopyMemory(OutCardResult.cbResultCard, CardTypeResult[i].cbEachHandCardData[0], sizeof(BYTE)*OutCardResult.cbCardCount);
					CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
					return;
				}
			}
			//������
			if (CardTypeResult[AI_CT_THREE].cbCardTypeCount > 0)
			{
				OutCardResult.cbCardCount = CardTypeResult[AI_CT_THREE].cbEachHandCardCount[0];
				CopyMemory(OutCardResult.cbResultCard, CardTypeResult[AI_CT_THREE].cbEachHandCardData[0], sizeof(BYTE)*OutCardResult.cbCardCount);
				CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
				return;
			}
		}
	}
	//�з���������������
	if (m_cbHandCardCount[wUnderChairID]<=3 || m_cbHandCardCount[wUpsideChairID])
	{
		//���Ƕ���
		for (BYTE i=AI_CT_SINGLE_LINK; i<=AI_CT_THREE_DOUBLE; i++)
		{
			if (CardTypeResult[i].cbCardTypeCount > 0)
			{
				OutCardResult.cbCardCount = CardTypeResult[i].cbEachHandCardCount[0];
				CopyMemory(OutCardResult.cbResultCard, CardTypeResult[i].cbEachHandCardData[0], sizeof(BYTE)*OutCardResult.cbCardCount);
				CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
				return;
			}
		}
	}
	//һ����һ��ը������һ���ƣ�
	{
		BYTE cbCount = 0;
		BYTE cbCardCountTemp=0, cbCardDataTemp[MAX_COUNT]={0};
		for (BYTE i=AI_CT_SINGLE; i<=AI_CT_THREE_DOUBLE; i++)
		{
			cbCount += CardTypeResult[i].cbCardTypeCount;
			if (CardTypeResult[i].cbCardTypeCount==1 && cbCardCountTemp==0)
			{
				if (i!=m_PlayerBestOutType.cbCardType[wUnderChairID] && i!=m_PlayerBestOutType.cbCardType[wUpsideChairID])
				{
					cbCardCountTemp = CardTypeResult[i].cbEachHandCardCount[0];
					CopyMemory(cbCardDataTemp, CardTypeResult[i].cbEachHandCardData[0], cbCardCountTemp);
				}
				else
				{
					BYTE cbMinValue = 0;
					for (BYTE j=1; j<=3; j+=2)
					{
						if (i == m_PlayerBestOutType.cbCardType[(wMeChairID+j)%GAME_PLAYER])
						{
							cbMinValue = __max(cbMinValue, m_PlayerBestOutType.cbCardSafeValue[(wMeChairID+j)%GAME_PLAYER]);
						}
					}
					cbMinValue = cbMinValue>0 ? cbMinValue:SearchMaxLogicValue(i,cbHandData,cbHandCardCount);
					if (GetTypeCardValue(cbCardDataTemp,cbCardCountTemp) >= cbMinValue)
					{
						cbCardCountTemp = CardTypeResult[i].cbEachHandCardCount[0];
						CopyMemory(cbCardDataTemp, CardTypeResult[i].cbEachHandCardData[0], cbCardCountTemp);
					}		
				}					
			}
		}

		if (cbCount==1 && cbCardCountTemp>0)
		{
			OutCardResult.cbCardCount = cbCardCountTemp;
			CopyMemory(OutCardResult.cbResultCard, cbCardDataTemp, OutCardResult.cbCardCount);
			CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
				
			if (cbCardCountTemp <= 3)
			{
				//������Ͳ���ͬ
				if (cbCardCountTemp!=m_cbHandCardCount[(wMeChairID+1)%GAME_PLAYER] && cbCardCountTemp!=m_cbHandCardCount[(wMeChairID+3)%GAME_PLAYER]) 
				{	
					return;
				}
				//���������ͬ,�ȳ�ը��
				else
				{
					for (BYTE i=AI_CT_TONG_HUA_SHUN; i<=AI_CT_BOMB_TW; ++i)
					{
						if (CardTypeResult[i].cbCardTypeCount > 0)
						{
							ZeroMemory(&OutCardResult, sizeof(OutCardResult));
							CopyMemory(OutCardResult.cbResultCard, CardTypeResult[i].cbEachHandCardData[0], CardTypeResult[i].cbEachHandCardCount[0]);
							OutCardResult.cbCardCount = CardTypeResult[i].cbEachHandCardCount[0];
							return;
						}
					}
				}
			}
			else
			{
				return;
			}			
		}
	}
	
	//2�����Է������ͣ������Ǳ��Ҳ�Ҫ�����ͣ�
	if (m_PlayerBestOutType.cbCardType[wFriendChairID] != 0)
	{
		//1���ǲ��ɳ����ƣ����ǲ����¼�
		BYTE cbMinValue = 0;
		BYTE cbMaxValue = SearchMaxLogicValue(m_PlayerBestOutType.cbCardType[wFriendChairID], cbHandData, cbHandCardCount);
		BYTE cbFriendCardType = m_PlayerBestOutType.cbCardType[wFriendChairID];
		BYTE cbFriendCardValue = (BYTE)m_PlayerBestOutType.cbCardSafeValue[wFriendChairID];
		BYTE cbUnderCardType = m_PlayerBestOutType.cbCardType[wUnderChairID];
		BYTE cbUnderCardValue = (BYTE)m_PlayerBestOutType.cbCardSafeValue[wUnderChairID];
		cbMaxValue = cbFriendCardValue!=0 ? cbFriendCardValue : cbMaxValue;

		bool bFangCard = true;
		for (WORD i=0; i<GAME_PLAYER; i++)
		{
			if (i==wMeChairID || i==wFriendChairID) continue;
			if (cbFriendCardType == m_PlayerBestOutType.cbCardType[i])
			{
				if (m_PlayerBestOutType.cbCardSafeValue[i]==0 || m_PlayerBestOutType.cbCardSafeValue[i]>cbFriendCardValue)
				{
					bFangCard = false;
					break;
				}
				else
				{
					cbMinValue = __max(m_PlayerBestOutType.cbCardSafeValue[i], 0);
				}
			}
		}

		if (bFangCard)
		{
			if (cbFriendCardType==cbUnderCardType && cbFriendCardValue>cbUnderCardValue)
			{
				cbMinValue = cbUnderCardValue;
			}
			//����:1���������Ĵ���������С���ڱ������У��������¼��Ѿ�û���ƣ�����С���ţ�
			BYTE cbFriendType = m_PlayerBestOutType.cbCardType[wFriendChairID];
			if (cbFriendType == AI_CT_SINGLE)
			{
				BYTE cbMaxSingleValue = SearchMaxLogicValue(AI_CT_SINGLE, cbHandData, cbHandCardCount);
				bool bOut  = false;
				if (cbMaxSingleValue >= 17)
				{
					if (m_cbKindCount[wFriendChairID][cbMaxSingleValue-17] > 0)
					{
						bOut = true;
					}
				}

				if (bOut || m_cbCardNum[wUnderChairID]==0)
				{
					BYTE cbTempCount=0, cbTempData[MAX_COUNT]={0};
					if (CardTypeResult[AI_CT_SINGLE].cbCardTypeCount > 0)
					{
						for (BYTE i=0; i<CardTypeResult[AI_CT_SINGLE].cbCardTypeCount; i++)
						{
							BYTE cbValue = GetCardLogicValue(CardTypeResult[AI_CT_SINGLE].cbEachHandCardData[i][0]);
							if (cbValue>5 || cbValue>cbMaxValue) break;
							if (cbValue<cbMinValue) continue;

							OutCardResult.cbCardCount = 1;
							OutCardResult.cbResultCard[0] = CardTypeResult[AI_CT_SINGLE].cbEachHandCardData[i][0];
							CheckOutCardInHand(cbHandCardData,cbHandCardCount,OutCardResult.cbResultCard,OutCardResult.cbCardCount);
							return;			
						}
					}

					for (BYTE cbType=AI_CT_SINGLE; cbType<=AI_CT_THREE; cbType++)
					{
						if (CardTypeResult[cbType].cbCardTypeCount > 0)
						{
							for (BYTE i=0; i<CardTypeResult[cbType].cbCardTypeCount; i++)
							{
								CopyMemory(&cbTempData[cbTempCount], CardTypeResult[cbType].cbEachHandCardData[i], CardTypeResult[cbType].cbEachHandCardCount[i]);
								cbTempCount += CardTypeResult[cbType].cbEachHandCardCount[i];
							}
						}
					}

					SortCardList(cbTempData, cbTempCount, ST_ORDER);
					for (int i=(cbTempCount-1); i>=0; i--)
					{
						BYTE cbValue = GetCardLogicValue(cbTempData[i]);
						if (cbValue<cbMinValue) continue;
						if (cbValue>=cbMaxValue) break;

						//ͳ�ƽ��
						OutCardResult.cbCardCount = 1;
						OutCardResult.cbResultCard[0] = cbTempData[i];
						CheckOutCardInHand(cbHandCardData,cbHandCardCount,OutCardResult.cbResultCard,OutCardResult.cbCardCount);
						return;
					}					
				}
			}
			else 
			{
				BYTE cbType = m_PlayerBestOutType.cbCardType[wFriendChairID];
				if (CardTypeResult[cbType].cbCardTypeCount > 0)
				{
					for (BYTE i=0; i<CardTypeResult[cbType].cbCardTypeCount; i++)
					{
						BYTE cbValue = GetCardLogicValue(CardTypeResult[cbType].cbEachHandCardData[i][0]);
						if (cbValue>cbMaxValue) break;
						if (cbValue<cbMinValue) continue;

						//ͳ�ƽ��
						OutCardResult.cbCardCount = CardTypeResult[cbType].cbEachHandCardCount[i];
					    CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbType].cbEachHandCardData[i], OutCardResult.cbCardCount);			
						CheckOutCardInHand(cbHandCardData,cbHandCardCount,OutCardResult.cbResultCard,OutCardResult.cbCardCount);
						return;
					}
				}
			}
		}
	}

	//ͳ�����е�����
	BYTE cbUnAvailableCount=0, cbUnAvailableType[TYPE_COUNT]={0};
	// 3���ɳ�������
	BYTE cbFirstCount=0, cbFirstType[TYPE_COUNT]={0};
	BYTE cbSecondCount=0, cbSecondType[TYPE_COUNT]={0};

	for (BYTE i=AI_CT_SINGLE; i<=AI_CT_THREE_DOUBLE; i++)
	{
		if (CardTypeResult[i].cbCardTypeCount > 0)
		{
			if (i==m_PlayerBestOutType.cbCardType[wUnderChairID] || i==m_PlayerBestOutType.cbCardType[wUpsideChairID])
			{
				cbUnAvailableType[cbUnAvailableCount] = i;
				cbUnAvailableCount++;
			}
			else
			{
				if (i==AI_CT_DOUBLE_LINK || i==AI_CT_SINGLE_LINK)
				{
					cbFirstType[cbFirstCount] = i;
					cbFirstCount++;
				}
				else
				{
					cbSecondType[cbSecondCount] = i;
					cbSecondCount++;
				}
			}
		}
	}

	if (cbFirstCount > 0)
	{
		//1�������ƣ�˳�ӣ������ԣ�
		BYTE cbRand = rand()%cbFirstCount;
		BYTE cbType = cbFirstType[cbRand];

		OutCardResult.cbCardCount = CardTypeResult[cbType].cbEachHandCardCount[0];
		CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbType].cbEachHandCardData[0], OutCardResult.cbCardCount);
		CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
		return;
	}
	else if (cbSecondCount > 0)
	{
		//2���������ƱȽϲ���Լ���Ҫ�����˲�Ҫ���ƣ������ǶԼ���Ҫ���ƣ�
		//3���������д�С��/����ƣ�����������ȥ��С���������� > 1��
		//4�����ӣ��д�С�����������ƶ��� �����ӣ����ӳ���С���������� > 1��
		
		BYTE cbCount = 0;
		for (BYTE i=0; i<2; i++)
		{
			if (cbSecondType[i]==AI_CT_SINGLE || cbSecondType[i]==AI_CT_DOUBLE)
			{
				BYTE cbType = cbSecondType[i];
				for (int i=CardTypeResult[cbType].cbCardTypeCount-1; i>=0; i--)
				{
					BYTE cbValue = GetCardLogicValue(CardTypeResult[cbType].cbEachHandCardData[i][0]);
					if (cbValue>=15 || (cbValue>=SearchMaxLogicValue(cbType,cbHandData,cbHandCardCount)&&SearchMaxLogicValue(cbType,cbHandData,cbHandCardCount)!=0))
					{
						cbCount ++;
					}
				}

				//ͳ�ƽ��
				if ((CardTypeResult[cbType].cbCardTypeCount-cbCount)>=1 && cbCount!=0)
				{
					OutCardResult.cbCardCount = CardTypeResult[cbType].cbEachHandCardCount[0];
					CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbType].cbEachHandCardData[0], OutCardResult.cbCardCount);
					CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
					return;
				}
			}	
		}	
		
		 //5�����Լ�ϲ��������  
		if (m_PlayerTypeImfo[wFriendChairID].cbLikeCardTypeCount>0 && m_cbCardNum[wFriendChairID]!=0)
		{
			for (BYTE i=0; i<m_PlayerTypeImfo[wFriendChairID].cbLikeCardTypeCount; i++)
			{
				BYTE cbFirstType = m_PlayerTypeImfo[wFriendChairID].cbLikeCardType[i];
				for(BYTE j=0; j<cbSecondCount; j++)
				{
					if (cbFirstType == cbSecondType[j])
					{
						//ͳ�ƽ��
						OutCardResult.cbCardCount = CardTypeResult[cbFirstType].cbEachHandCardCount[0];
						CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbFirstType].cbEachHandCardData[0], OutCardResult.cbCardCount);						
						CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
						return;
					}
				}
			}
		}

		//6��������
		for (BYTE i=0; i<cbSecondCount; i++)
		{
			if (cbSecondType[i] == AI_CT_THREE_LINK)
			{
				//ͳ�ƽ��
				OutCardResult.cbCardCount = CardTypeResult[AI_CT_THREE_LINK].cbEachHandCardCount[0];
				CopyMemory(OutCardResult.cbResultCard, CardTypeResult[AI_CT_THREE_LINK].cbEachHandCardData[0], OutCardResult.cbCardCount);
				CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
				return;
			}
		}

		//5�����ӣ�û�д�С�� && �Լ��д�С���� && ���������� && ���Ÿ����Ƚ϶ࣩ������
		if (m_iOutCardCount[18]==2 || m_iOutCardCount[17]==2)
		{
			bool bHaveKind = false;
			bool bHaveMain = false;
			for (BYTE i=0; i<cbHandCardCount; i++)
			{
				BYTE cbValue = GetCardLogicValue(cbHandData[i]);
				if (cbValue==18 || cbValue==17)
				{
					bHaveKind = true;
				}
				else if (cbValue == 15)
				{
					bHaveMain = true;
				}
			}

			if (!bHaveKind && bHaveMain && CardTypeResult[AI_CT_SINGLE].cbCardTypeCount>=3)
			{
				for (BYTE i=0; i<cbSecondCount; i++)
				{
					if (cbSecondType[i] == AI_CT_DOUBLE)
					{
						OutCardResult.cbCardCount = 2;
						CopyMemory(OutCardResult.cbResultCard, CardTypeResult[AI_CT_DOUBLE].cbEachHandCardData[0], OutCardResult.cbCardCount);
						CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
						return;
					}
				}
			}
		}

		//6��ʣ�������е�������ͣ��ĸ������Ƚ϶���ȳ���һ����һ���Ļ��ȳ���������ٳ������ٵģ�
		//�������������ӻ�����3�������ĸ���������Ӧ�ķ����������
		BYTE cbType = 0;
		if (m_cbCardNum[wFriendChairID] == 0)
		{
			cbType = cbSecondType[cbSecondCount-1];
			BYTE cbTempCount = CardTypeResult[cbType].cbCardTypeCount;
			for (int i=(cbSecondCount-1); i>=0; i--)
			{
				if (CardTypeResult[cbSecondType[i]].cbCardTypeCount > cbTempCount)
				{
					cbTempCount = CardTypeResult[cbSecondType[i]].cbCardTypeCount;
					cbType = cbSecondType[i];
				}
			}
		}
		else
		{
			cbType = cbSecondType[0];
			BYTE cbTempCount = CardTypeResult[cbType].cbCardTypeCount;
			for (int i=0; i<cbSecondCount; i++)
			{
				if (CardTypeResult[cbSecondType[i]].cbCardTypeCount > cbTempCount)
				{
					cbTempCount = CardTypeResult[cbSecondType[i]].cbCardTypeCount;
					cbType = cbSecondType[i];
				}
			}
		}

		if (CardTypeResult[AI_CT_DOUBLE].cbCardTypeCount>0 && CardTypeResult[AI_CT_THREE].cbCardTypeCount>0 && CardTypeResult[AI_CT_THREE_DOUBLE].cbCardTypeCount>0)
		{
			bool bCanOut = true;
			if (cbUnAvailableCount > 0)
			{
				for (BYTE i=0; i<cbUnAvailableCount; i++)
				{
					if (cbUnAvailableType[i] == AI_CT_THREE_DOUBLE)
					{
						bCanOut = false;
					}
				}
			}
			if ((cbType==AI_CT_THREE||cbType==AI_CT_DOUBLE) && bCanOut)
			{
				BYTE cbTempType = cbType==AI_CT_THREE ? AI_CT_DOUBLE:AI_CT_THREE;
				if (CardTypeResult[cbType].cbCardTypeCount<=(CardTypeResult[cbTempType].cbCardTypeCount+1) && CardTypeResult[cbType].cbCardTypeCount>=(CardTypeResult[cbTempType].cbCardTypeCount-1))
				{
					cbType = AI_CT_THREE_DOUBLE;
				}
			}
		}
		//ͳ�ƽ��
		OutCardResult.cbCardCount = CardTypeResult[cbType].cbEachHandCardCount[0];
		CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbType].cbEachHandCardData[0], OutCardResult.cbCardCount);
		CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);		
		return;

	}
	else if (cbUnAvailableCount > 0)
	{
		//���ɳ�����
		//���⴦��
	    //һ��ը����һ����(����)���Լ����е��� < 4��������2�����ɳ�����(��һ��1)�� �ȳ�ը�������
		//����
		if (cbUnAvailableType[0] == AI_CT_SINGLE)
		{
			if (CardTypeResult[AI_CT_SINGLE].cbCardTypeCount == 1)
			{
				BYTE cbTempData[MAX_COUNT]={0};
				BYTE cbTempCount = cbHandCardCount;
				CopyMemory(cbTempData, cbHandData, cbTempCount);
				RemoveCard(CardTypeResult[AI_CT_SINGLE].cbEachHandCardData[0], 1, cbTempData, cbTempCount);
				cbTempCount -= CardTypeResult[AI_CT_SINGLE].cbEachHandCardCount[0];
				BYTE cbType = GetCardType(cbTempData,cbTempCount);
				if ((cbType==AI_CT_BOMB||cbType==AI_CT_TONG_HUA_SHUN||cbType==AI_CT_BOMB_TW) && m_cbCardNum[wUnderChairID]<4 && m_cbCardNum[wUpsideChairID]<4)
				{
					OutCardResult.cbCardCount = cbTempCount;
					CopyMemory(OutCardResult.cbResultCard, cbTempData, cbTempCount);

					CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
					ASSERT(OutCardResult.cbCardCount != 0);
					return;
				}
			}

			//a) Σ����Ҳ����¼ң�����С���ţ�
			if (m_cbCardNum[wUnderChairID] != 1)
			{
				ASSERT(CardTypeResult[AI_CT_SINGLE].cbCardTypeCount > 0);
				OutCardResult.cbCardCount = 1;
				OutCardResult.cbResultCard[0] = CardTypeResult[AI_CT_SINGLE].cbEachHandCardData[0][0];

				CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
				ASSERT(OutCardResult.cbCardCount != 0);
				return;
			}
			else
			{
				//b) �����1��һ��ը����һ�ţ��������ĵ��ţ���ը����
				if (CardTypeResult[AI_CT_SINGLE].cbCardTypeCount == 1)
				{
					BYTE cbTempData[MAX_COUNT]={0};
					BYTE cbTempCount = cbHandCardCount;
					CopyMemory(cbTempData, cbHandData, cbTempCount);
					RemoveCard(CardTypeResult[AI_CT_SINGLE].cbEachHandCardData[0], CardTypeResult[AI_CT_SINGLE].cbEachHandCardCount[0], cbTempData, cbTempCount);
					cbTempCount -= CardTypeResult[AI_CT_SINGLE].cbEachHandCardCount[0];
					BYTE cbType = GetCardType(cbTempData, cbTempCount);
					if ((cbType==AI_CT_BOMB || cbType==AI_CT_TONG_HUA_SHUN || cbType==AI_CT_BOMB_TW))
					{
						if (GetCardLogicValue(CardTypeResult[AI_CT_SINGLE].cbEachHandCardData[0][0]) >= SearchMaxLogicValue(AI_CT_SINGLE,cbHandData,cbHandCardCount))
						{
							OutCardResult.cbCardCount = 1;
							OutCardResult.cbResultCard[0] = CardTypeResult[AI_CT_SINGLE].cbEachHandCardData[0][0];

							CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
							ASSERT(OutCardResult.cbCardCount != 0);
							return;
						}
						else
						{
							OutCardResult.cbCardCount = cbTempCount;
							CopyMemory(OutCardResult.cbResultCard, cbTempData, cbTempCount);

							CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
							ASSERT(OutCardResult.cbCardCount != 0);
							return;
						}
					}
					else
					{
						//����������
						BYTE cbTempCount = cbHandCardCount;
						CopyMemory(cbTempData, cbHandData, cbTempCount);
						SortCardList(cbTempData, cbTempCount, ST_ORDER);

						OutCardResult.cbCardCount = 1;
						OutCardResult.cbResultCard[0] = cbTempData[0];

						CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
						ASSERT(OutCardResult.cbCardCount != 0);
						return;
					}			
				}
				else
				{
					//2��>= 2 ������������������ƣ�
					ASSERT(CardTypeResult[AI_CT_SINGLE].cbCardTypeCount > 0);
					BYTE cbMaxValue = SearchMaxLogicValue(AI_CT_SINGLE, cbHandData, cbHandCardCount);
					BYTE cbMinVaue = GetCardLogicValue(CardTypeResult[AI_CT_SINGLE].cbEachHandCardData[0][0]);
					for (int i=CardTypeResult[AI_CT_SINGLE].cbCardTypeCount-1; i>=0; i--)
					{
						BYTE cbValue = GetCardLogicValue(CardTypeResult[AI_CT_SINGLE].cbEachHandCardData[i][0]);
						if (cbValue>=cbMaxValue && cbValue!=cbMinVaue) continue;
						if (cbValue == cbMinVaue)
						{
							OutCardResult.cbCardCount = 1;
							OutCardResult.cbResultCard[0] = CardTypeResult[AI_CT_SINGLE].cbEachHandCardData[CardTypeResult[AI_CT_SINGLE].cbCardTypeCount-1][0];
							
							CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
							ASSERT(OutCardResult.cbCardCount != 0);
							return;
						}

						OutCardResult.cbCardCount = 1;
						OutCardResult.cbResultCard[0] = CardTypeResult[AI_CT_SINGLE].cbEachHandCardData[i][0];

						CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
						ASSERT(OutCardResult.cbCardCount != 0);
						return;
						
					}
					ASSERT(OutCardResult.cbCardCount != 0);
				}
				ASSERT(OutCardResult.cbCardCount != 0);
			}
			ASSERT(OutCardResult.cbCardCount != 0);
		}
		else if (cbUnAvailableType[0] == AI_CT_DOUBLE)
		{
			//2�����ӣ�a) �������ӵĵ��ţ�
			ASSERT(CardTypeResult[AI_CT_DOUBLE].cbCardTypeCount > 0);
			OutCardResult.cbCardCount = 1;
			OutCardResult.cbResultCard[0] = CardTypeResult[AI_CT_DOUBLE].cbEachHandCardData[CardTypeResult[AI_CT_DOUBLE].cbCardTypeCount-1][0];

			CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
			ASSERT(OutCardResult.cbCardCount != 0);
			return;
		}
		else if (cbUnAvailableType[0] == AI_CT_THREE)
		{
			//3��������b���������ӣ�
			ASSERT(CardTypeResult[AI_CT_THREE].cbCardTypeCount > 0);
			OutCardResult.cbCardCount = 2;
			CopyMemory(OutCardResult.cbResultCard, &CardTypeResult[AI_CT_THREE].cbEachHandCardData[CardTypeResult[AI_CT_THREE].cbCardTypeCount-1], OutCardResult.cbCardCount);
			CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
			ASSERT(OutCardResult.cbCardCount != 0);
			return;
		}
		ASSERT(OutCardResult.cbCardCount != 0);
	}
	else 
	{
		BYTE cbCount = 0;
		//ը��
		for (BYTE i=AI_CT_SINGLE; i<=AI_CT_THREE_DOUBLE; i++)
		{
			if (CardTypeResult[i].cbCardTypeCount > 0)
			{
				cbCount ++;
			}
		}

		ASSERT(cbCount == 0);
		BYTE cbBombType[3] = {AI_CT_BOMB,AI_CT_TONG_HUA_SHUN,AI_CT_BOMB_TW};
		for (BYTE i=0; i<3; i++)
		{
			BYTE cbType = cbBombType[i];
			if (CardTypeResult[cbType].cbCardTypeCount > 0)
			{
				OutCardResult.cbCardCount = CardTypeResult[cbType].cbEachHandCardCount[0];
				CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbType].cbEachHandCardData[0], OutCardResult.cbCardCount);
				CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
				return;
			}
		}
		ASSERT(OutCardResult.cbCardCount > 0);	
	}

	CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
	ASSERT(OutCardResult.cbCardCount > 0);	
}

//��ȡ����
BYTE CAILogic::GetTributeCard(WORD wMeChariID, BYTE cbHandCardData[], BYTE cbHandCardCount)
{
	//״̬�ж�
	ASSERT(cbHandCardCount == 27);
	BYTE cbHandCardDataTemp[MAX_COUNT] = {0};
	ZeroMemory(cbHandCardDataTemp, sizeof(BYTE)*MAX_COUNT);
	CopyMemory(cbHandCardDataTemp, cbHandCardData, cbHandCardCount);
	//��������
	BYTE cbThrowCard = 0;
	//Ѱ���˿�
	if (cbThrowCard == 0)
	{
		//��������
		BYTE cbCardLogicValue = 0;

		//Ѱ���˿�
		for (BYTE i=0; i<cbHandCardCount; i++)
		{
			//�˿˹���
			if (cbHandCardData[i]==(m_cbMainValue|0x20)) continue;

			//�˿��ж�
			if (GetCardLogicValue(cbHandCardData[i]) > cbCardLogicValue)
			{
				cbThrowCard = cbHandCardData[i];
				cbCardLogicValue = GetCardLogicValue(cbHandCardData[i]);
			}
		}
	}

	//��ӽ�������	�����ơ��������Ҹ�5���߼���֮��������ֻ�ɫ�ĸ��������������⣩��ɫ���Ľ�����
	if (cbThrowCard!=0x4E && cbThrowCard!=0x4F)
	{
		BYTE cbThrowValue = GetCardValue(cbThrowCard);
		BYTE cbTempCount=0, cbTempData[8]={0};
		//Ѱ���˿�
		for (BYTE i=0; i<cbHandCardCount; i++)
		{
			//�˿˹���
			if (cbThrowValue!=GetCardValue(cbHandCardData[i]) || cbHandCardData[i]==(m_cbMainValue|0x20)) continue;

			cbTempData[cbTempCount++] = cbHandCardData[i];
		}

		if (cbTempCount > 1)
		{
			BYTE cbColorCount[4] = {0,0,0,0};
			BYTE cbMeHandCardData[MAX_COUNT] = {0};
			ZeroMemory(cbMeHandCardData, sizeof(cbMeHandCardData));
			BYTE cbMeHandCardCount = cbHandCardCount;
			CopyMemory(cbMeHandCardData, cbHandCardData, cbMeHandCardCount);

			NearbyColorCount(cbMeHandCardData, cbMeHandCardCount, cbThrowCard, cbColorCount);

			BYTE cbData = cbThrowCard;
			for (BYTE i=0; i<cbTempCount; i++)
			{
				BYTE cbIndexFirstColor = (GetCardColor(cbData))>>4;
				BYTE cbIndexSecondColr = (GetCardColor(cbTempData[i]))>>4;
				if (cbIndexSecondColr == cbIndexFirstColor) continue;
				if (cbColorCount[cbIndexSecondColr] > cbColorCount[cbIndexFirstColor])
				{
					cbData = cbTempData[i];
				}
			}

			cbThrowCard = cbData;
		}
	}

	return cbThrowCard;
}

//��ȡ�ع���
BYTE CAILogic::GetBackTributeCard(WORD wMeChariID, BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wThrowUserNum, bool bThrowToFriend)
{
	ASSERT(cbHandCardCount == 28);
	BYTE cbHandCardDataTemp[MAX_COUNT] = {0};
	ZeroMemory(cbHandCardDataTemp, sizeof(BYTE)*MAX_COUNT);
	CopyMemory(cbHandCardDataTemp, cbHandCardData, cbHandCardCount);
	//��������
	BYTE cbIncepCard = 0;
	//Ѱ���˿�
	if (cbIncepCard == 0)
	{
		//��������
		BYTE cbCardLogicValue = 0xFF;
		//Ѱ���˿�
		for (BYTE i=0; i<cbHandCardCount; i++)
		{
			if (GetCardLogicValue(cbHandCardData[i]) < cbCardLogicValue)
			{
				cbIncepCard = cbHandCardData[i];
				cbCardLogicValue = GetCardLogicValue(cbHandCardData[i]);
			}
		}
	}

	//��ӻع�
	tagOutCardTypeResult CardTypeResult[TYPE_COUNT];
	AnalyseOutCardType(cbHandCardData, cbHandCardCount, CardTypeResult);
	//(�����)��������С���ƣ�С��10����
	//���������ǵ���

	BYTE cbTempType = 0;
	bool bFind = false;
	ASSERT(wThrowUserNum!=0);
	if (!bThrowToFriend)
	{
		if (CardTypeResult[AI_CT_SINGLE].cbCardTypeCount > 0)
		{
			if (GetCardLogicValue(CardTypeResult[AI_CT_SINGLE].cbEachHandCardData[0][0]) < 10)
			{
				cbIncepCard = CardTypeResult[AI_CT_SINGLE].cbEachHandCardData[0][0];
				bFind = true;
			}
		}
		if ((CardTypeResult[AI_CT_DOUBLE].cbCardTypeCount>0||CardTypeResult[AI_CT_THREE].cbCardTypeCount>0) && !bFind)
		{
			if (CardTypeResult[AI_CT_DOUBLE].cbCardTypeCount > 0)
			{
				cbTempType = AI_CT_DOUBLE;
			}
			if (CardTypeResult[AI_CT_THREE].cbCardTypeCount > 0)
			{
				if (cbTempType != 0)
				{
					if (GetCardLogicValue(CardTypeResult[AI_CT_THREE].cbEachHandCardData[0][0]) < GetCardLogicValue(CardTypeResult[AI_CT_DOUBLE].cbEachHandCardData[0][0]))
					{
						cbTempType = AI_CT_THREE;
					}
				}
				else
				{
					cbTempType = AI_CT_THREE;
				}
			}

			if (cbTempType!=0 && GetCardLogicValue(CardTypeResult[cbTempType].cbEachHandCardData[0][0])<10)
			{
				BYTE cbColorCount[4] = {0,0,0,0};
				BYTE cbMeHandCardData[MAX_COUNT] = {0};
				BYTE cbMeHandCardCount = cbHandCardCount;
				CopyMemory(cbMeHandCardData, cbHandCardData, cbMeHandCardCount);

				NearbyColorCount(cbMeHandCardData, cbMeHandCardCount, CardTypeResult[cbTempType].cbEachHandCardData[0][0], cbColorCount);

				BYTE cbData = CardTypeResult[cbTempType].cbEachHandCardData[0][0];
				for (BYTE i=0; i<CardTypeResult[cbTempType].cbEachHandCardCount[0]; i++)
				{
					BYTE cbIndexFirstColor = (GetCardColor(cbData))>>4;
					BYTE cbIndexSecondColr = (GetCardColor(CardTypeResult[cbTempType].cbEachHandCardData[0][i]))>>4;
					if (cbIndexFirstColor == cbIndexFirstColor) continue;
					ASSERT(cbIndexSecondColr<4 && cbIndexFirstColor<4);
					if (cbColorCount[cbIndexSecondColr] > cbColorCount[cbIndexFirstColor])
					{
						cbData = CardTypeResult[cbTempType].cbEachHandCardData[0][i];
					}
				}
				cbIncepCard = cbData;
			}
		}
	}
	else
	//���������Ǳ��ҶԼ�
	{
		bool bFind = false;
		if (CardTypeResult[AI_CT_SINGLE].cbCardTypeCount > 0)
		{
			for (BYTE i=0; i<CardTypeResult[AI_CT_SINGLE].cbCardTypeCount; i++)
			{
				if (GetCardLogicValue(CardTypeResult[AI_CT_SINGLE].cbEachHandCardData[i][0]) >= 10)
				{
					break;
				}

				cbIncepCard = CardTypeResult[AI_CT_SINGLE].cbEachHandCardData[i][0];
				bFind = true;
			}
		}

		if ((CardTypeResult[AI_CT_DOUBLE].cbCardTypeCount>0||CardTypeResult[AI_CT_THREE].cbCardTypeCount>0) && !bFind)
		{
			BYTE cbTempType = 0;
			if (CardTypeResult[AI_CT_DOUBLE].cbCardTypeCount > 0)
			{
				cbTempType = AI_CT_DOUBLE;
			}
			if (CardTypeResult[AI_CT_THREE].cbCardTypeCount > 0)
			{
				if (cbTempType != 0)
				{
					if (GetCardLogicValue(CardTypeResult[AI_CT_THREE].cbEachHandCardData[0][0]) > GetCardLogicValue(CardTypeResult[AI_CT_DOUBLE].cbEachHandCardData[0][0]))
					{
						cbTempType = AI_CT_THREE;
					}
				}
				else
				{
					cbTempType = AI_CT_THREE;
				}
			}

			if (cbTempType!=0 && GetCardLogicValue(CardTypeResult[cbTempType].cbEachHandCardData[0][0])<10)
			{
				BYTE cbColorCount[4] = {0,0,0,0};
				BYTE cbMeHandCardData[MAX_COUNT] = {0};
				BYTE cbMeHandCardCount = cbHandCardCount;
				CopyMemory(cbMeHandCardData, cbHandCardData, cbMeHandCardCount);

				NearbyColorCount(cbMeHandCardData, cbMeHandCardCount, CardTypeResult[cbTempType].cbEachHandCardData[0][0],cbColorCount);

				BYTE cbData = CardTypeResult[cbTempType].cbEachHandCardData[0][0];
				for (BYTE i=0; i<CardTypeResult[cbTempType].cbEachHandCardCount[0]; i++)
				{
					BYTE cbIndexFirstColor = (GetCardColor(cbData))>>4;
					BYTE cbIndexSecondColr = (GetCardColor(CardTypeResult[cbTempType].cbEachHandCardData[0][i]))>>4;
					if (cbIndexSecondColr == cbIndexFirstColor) continue;
					if (cbColorCount[cbIndexSecondColr] < cbColorCount[cbIndexFirstColor])
					{
						cbData = CardTypeResult[cbTempType].cbEachHandCardData[0][i];
					}
				}
				cbIncepCard = cbData;
			}
		}
	}

	return cbIncepCard;
}

