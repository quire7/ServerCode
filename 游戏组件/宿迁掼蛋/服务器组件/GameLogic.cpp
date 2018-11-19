#include "StdAfx.h"
#include "GameLogic.h"

//��������
const BYTE cbIndexCount = 5;

//////////////////////////////////////////////////////////////////////////
//�˿�����
const BYTE	CGameLogic::m_cbCardData[FULL_COUNT] =
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	    //���� A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	    //÷�� A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	    //���� A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	    //���� A - K
	0x4e,0x4f,                                                              //С��������
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	    //���� A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	    //÷�� A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	    //���� A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	    //���� A - K
	0x4e,0x4f                                                               //С��������
};

//////////////////////////////////////////////////////////////////////////
//���캯��
CGameLogic::CGameLogic()
{
	//�߼�����
	m_cbMainValue = 2;
}

//��������
CGameLogic::~CGameLogic()
{

}


//��ȡ����
BYTE CGameLogic::GetCardType(const BYTE cbCardData[], BYTE cbCardCount)
{
	ASSERT(cbCardCount <= 28);
	if (cbCardCount > 28) return CT_ERROR;

	BYTE cbCardDataTemp[MAX_COUNT] = { 0 };
	CopyMemory(cbCardDataTemp, cbCardData, cbCardCount);
	//����
	SortCardList(cbCardDataTemp, cbCardCount, ST_ORDER);

	//������
	switch (cbCardCount)
	{
	case 0:	//����
	{
		return CT_ERROR;
	}
	case 1: //����
	{
		return CT_SINGLE;
	}
	case 2:	//����
	{
		return (GetCardValue(cbCardData[0]) == GetCardValue(cbCardData[1])) ? CT_DOUBLE : CT_ERROR;
	}
	case 3:	//����
	{
		return (GetCardValue(cbCardData[0]) == GetCardValue(cbCardData[2])) ? CT_THREE : CT_ERROR;
	}
	}

	//�����˿�
	tagAnalyseResult AnalyseResult;
	AnalysebCardData(cbCardData, cbCardCount, AnalyseResult);

	//ը������
	if (cbCardCount == 4 && cbCardDataTemp[0] == 0x4F && cbCardDataTemp[3] == 0x4E) return CT_BOMB_TW;
	if (cbCardCount >= 4 && cbCardCount <= 10 && AnalyseResult.cbBlockCount[cbCardCount - 1] == 1) return CT_BOMB;

	//˳������
	if (cbCardCount == 5 && AnalyseResult.cbBlockCount[0] == cbCardCount)
	{
		//�˿�����
		BYTE cbSignedCount = AnalyseResult.cbBlockCount[0];
		BYTE cbCardColor = GetCardColor(AnalyseResult.cbCardData[0], cbSignedCount);
		bool bStructureLink = IsStructureLink(AnalyseResult.cbCardData[0], cbSignedCount, 1);

		//�����ж�
		if (bStructureLink&&cbCardColor == 0xF0) return CT_SINGLE_LINK;
		if (bStructureLink&&cbCardColor != 0xF0) return CT_TONG_HUA_SHUN;
	}

	//��������
	if (cbCardCount == 6 && AnalyseResult.cbBlockCount[1] * 2 == cbCardCount)
	{
		BYTE cbDoubleCount = AnalyseResult.cbBlockCount[1] * 2;
		if (IsStructureLink(AnalyseResult.cbCardData[1], cbDoubleCount, 2)) return CT_DOUBLE_LINK;
	}

	//��������
	if (cbCardCount == 6 && AnalyseResult.cbBlockCount[2] * 3 == cbCardCount)
	{
		BYTE cbThreeCount = AnalyseResult.cbBlockCount[2] * 3;
		if (IsStructureLink(AnalyseResult.cbCardData[2], cbThreeCount, 3)) return CT_THREE_LINK;
	}

	//��������
	if (cbCardCount == 5 && AnalyseResult.cbBlockCount[1] == 1 && AnalyseResult.cbBlockCount[2] == 1) return CT_THREE_DOUBLE;

	return CT_ERROR;
}

//��ȡ��ɫ
BYTE CGameLogic::GetCardColor(const BYTE cbCardData[], BYTE cbCardCount)
{
	//Ч�����
	ASSERT(cbCardCount > 0);
	if (cbCardCount == 0) return 0xF0;

	//���ƻ�ɫ
	BYTE cbCardColor = GetCardColor(cbCardData[0]);
	//��ɫ�ж�
	for (BYTE i = 0; i < cbCardCount; i++)
	{
		if (GetCardColor(cbCardData[i]) != cbCardColor)
		{
			return 0xF0;
		}
	}

	return cbCardColor;
}

//��ȡ����
BYTE CGameLogic::GetMagicCardType(const BYTE cbCardData[], BYTE cbCardCount)
{
	BYTE cbMagicData[MAX_COUNT] = { 0 };
	MagicCardData(cbCardData, cbCardCount, cbMagicData);
	return GetCardType(cbMagicData, cbCardCount);
}

//�����˿�
VOID CGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//����׼��
	BYTE cbCardData[CountArray(m_cbCardData)];
	CopyMemory(cbCardData, m_cbCardData, sizeof(m_cbCardData));

	//�����˿�
	BYTE cbRandCount = 0, cbPosition = 0;
	do
	{
		cbPosition = rand() % (cbBufferCount - cbRandCount);
		cbCardBuffer[cbRandCount++] = cbCardData[cbPosition];
		cbCardData[cbPosition] = cbCardData[cbBufferCount - cbRandCount];
	} while (cbRandCount < cbBufferCount);

	return;
}

//�����˿�
VOID CGameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType)
{
	//�������
	if (cbCardCount == 0) return;
	if (cbSortType == ST_CUSTOM) return;

	//ת����ֵ
	BYTE cbSortValue[MAX_COUNT] = { 0 };
	for (BYTE i = 0; i < cbCardCount; i++)
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
			cbSortValue[i] = GetCardColor(cbCardData[i]) + GetCardLogicValue(cbCardData[i]);
		}break;
		}
	}

	//�������
	bool bSorted = true;
	BYTE cbSwitchData = 0, cbLast = cbCardCount - 1;
	do
	{
		bSorted = true;
		for (BYTE i = 0; i < cbLast; i++)
		{
			if (cbSortValue[i] < cbSortValue[i + 1] || (cbSortValue[i] == cbSortValue[i + 1] && cbCardData[i] < cbCardData[i + 1]))
			{
				//���ñ�־
				bSorted = false;

				//�˿�����
				cbSwitchData = cbCardData[i];
				cbCardData[i] = cbCardData[i + 1];
				cbCardData[i + 1] = cbSwitchData;

				//����Ȩλ
				cbSwitchData = cbSortValue[i];
				cbSortValue[i] = cbSortValue[i + 1];
				cbSortValue[i + 1] = cbSwitchData;
			}
		}
		cbLast--;
	} while (bSorted == false);

	//��Ŀ����
	if (cbSortType == ST_COUNT)
	{
		//��������
		BYTE cbCardIndex = 0;

		//�����˿�
		tagAnalyseResult AnalyseResult;
		AnalysebCardData(cbCardData, cbCardCount, AnalyseResult);

		//��ȡ�˿�
		for (BYTE i = 0; i < CountArray(AnalyseResult.cbBlockCount); i++)
		{
			//�����˿�
			BYTE cbIndex = CountArray(AnalyseResult.cbBlockCount) - i - 1;
			CopyMemory(&cbCardData[cbCardIndex], AnalyseResult.cbCardData[cbIndex], AnalyseResult.cbBlockCount[cbIndex] * (cbIndex + 1)*sizeof(BYTE));

			//��������
			cbCardIndex += AnalyseResult.cbBlockCount[cbIndex] * (cbIndex + 1)*sizeof(BYTE);
		}
	}
	return;
}

//ɾ���˿�
bool CGameLogic::RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount)
{
	//��������
	if (cbRemoveCount == 0) return true;

	ASSERT(cbRemoveCount <= cbCardCount);
	if (cbRemoveCount > cbCardCount) return false;

	//�������
	BYTE cbDeleteCardCount = 0, cbDeleteCardData[MAX_COUNT] = { 0 };
	if (cbCardCount > CountArray(cbDeleteCardData)) return false;
	CopyMemory(cbDeleteCardData, cbCardData, cbCardCount*sizeof(cbCardData[0]));

	//�����˿�
	for (BYTE i = 0; i < cbRemoveCount; i++)
	{
		for (BYTE j = 0; j < cbCardCount; j++)
		{
			if (cbRemoveCard[i] == cbDeleteCardData[j])
			{
				cbDeleteCardCount++;
				cbDeleteCardData[j] = 0;
				break;
			}
		}
	}
	ASSERT(cbDeleteCardCount == cbRemoveCount);
	if (cbDeleteCardCount != cbRemoveCount) return false;

	ZeroMemory(cbCardData, cbCardCount*sizeof(cbCardData[0]));
	//�����˿�
	for (BYTE i = 0, cbCardPos = 0; i < cbCardCount; i++)
	{
		if (cbDeleteCardData[i] > 0x00 && cbDeleteCardData[i] <= 0x4F)
		{
			cbCardData[cbCardPos++] = cbDeleteCardData[i];
		}
	}

	return true;
}

//�߼���ֵ
BYTE CGameLogic::GetCardLogicValue(BYTE cbCardData)
{
	//�˿�����
	BYTE cbCardValue = GetCardValue(cbCardData);
	//�����˿�
	if (cbCardValue == m_cbMainValue) return 15;

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

//�Ա��˿�
bool CGameLogic::CompareCard(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount)
{
	//�����ж�
	BYTE cbNextType = GetCardType(cbNextCard, cbNextCount);
	BYTE cbFirstType = GetCardType(cbFirstCard, cbFirstCount);

	//ը���ж�
	if (cbFirstType >= CT_TONG_HUA_SHUN || cbNextType >= CT_TONG_HUA_SHUN)
	{
		//����ը��
		if (cbNextType == CT_BOMB_TW) return true;
		if (cbFirstType == CT_BOMB_TW) return false;

		//�ͼ�����
		if (cbNextType < CT_TONG_HUA_SHUN) return false;
		if (cbFirstType<CT_TONG_HUA_SHUN) return true;

		//��Ŀ�Ա�
		if (cbFirstCount == cbNextCount && cbFirstType == cbNextType)
		{
			if (cbFirstType == CT_BOMB || cbFirstType == CT_BOMB_TW)
			{
				return GetCardLogicValue(cbNextCard[0])>GetCardLogicValue(cbFirstCard[0]);
			}
			else
			{
				return CompareCardByValue(cbFirstCard, cbNextCard, cbFirstCount, cbNextCount);
			}
		}

		//��ͬ����
		if (cbFirstCount == cbNextCount && cbFirstType != cbNextType)
		{
			if (cbNextType == CT_TONG_HUA_SHUN) return true;
			if (cbFirstType == CT_TONG_HUA_SHUN) return false;
			return false;
		}

		return cbNextCount > cbFirstCount;
	}

	//��ͬ����
	if (cbFirstType != cbNextType) return false;

	//��ͬ����
	switch (cbFirstType)
	{
	case CT_SINGLE:				//��������
	case CT_DOUBLE:				//��������
	case CT_THREE:				//��������
	{
		return GetCardLogicValue(cbNextCard[0]) > GetCardLogicValue(cbFirstCard[0]);
	}
	case CT_SINGLE_LINK:			//��������
	case CT_DOUBLE_LINK:			//��������
	case CT_THREE_LINK:			//��������
	{
		return CompareCardByValue(cbFirstCard, cbNextCard, cbFirstCount, cbNextCount);
	}
	case CT_THREE_DOUBLE:		//��������
	{
		//�����˿�
		tagAnalyseResult AnalyseResultNext, AnalyseResultFirst;
		AnalysebCardData(cbNextCard, cbNextCount, AnalyseResultNext);
		AnalysebCardData(cbFirstCard, cbFirstCount, AnalyseResultFirst);
		return GetCardLogicValue(AnalyseResultNext.cbCardData[2][0]) > GetCardLogicValue(AnalyseResultFirst.cbCardData[2][0]);
	}
	}

	//�������
	//ASSERT(FALSE);	
	return false;
}

//����˿�
bool CGameLogic::MagicCardData(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbResultCard[MAX_COUNT])
{
	//��������
	BYTE cbNormalCardData[MAX_COUNT] = { 0 };
	BYTE cbNormalCardCount = 0, cbMagicCardCount = 0;

	//���׼��
	for (BYTE i = 0; i < cbCardCount; i++)
	{
		//���ͳ��
		if (GetCardValue(cbCardData[i]) == m_cbMainValue && GetCardColor(cbCardData[i]) == 0x20)
		{
			cbMagicCardCount++;
		}
		else
		{
			cbNormalCardData[cbNormalCardCount++] = cbCardData[i];
		}
	}

	//ʧ���ж�
	if (cbMagicCardCount == 0 || cbNormalCardCount == 0)
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
		for (cbSameCount = 1; cbSameCount < cbNormalCardCount; cbSameCount++)
		{
			if (GetCardLogicValue(cbNormalCardData[cbSameCount]) != cbCardLogicValue) break;
		}
		if (cbSameCount == cbNormalCardCount)
		{
			//���ý��
			for (BYTE j = 0; j < cbMagicCardCount; j++)
			{
				cbResultCard[j] = cbNormalCardData[0];
			}
			for (BYTE j = 0; j < cbNormalCardCount; j++)
			{
				cbResultCard[j + cbMagicCardCount] = cbNormalCardData[j];
			}
			return true;
		}
	}

	//�˿˷���
	BYTE cbMaxSameCount = 1, cbSameCount = 1;
	for (BYTE i = 1; i < cbNormalCardCount; i++)
	{
		//��ȡ�˿�
		BYTE cbCardLogicValueTemp = GetCardLogicValue(cbNormalCardData[i]);
		if (cbCardLogicValueTemp == cbCardLogicValue) cbSameCount++;

		if (cbCardLogicValueTemp != cbCardLogicValue)
		{
			if (i == cbNormalCardCount - 1)
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
			if (i == cbNormalCardCount - 1)
			{
				cbMaxSameCount = __max(cbSameCount, cbMaxSameCount);
			}
		}
	}

	//��������
	if (cbCardCount == 5 && cbMaxSameCount <= 3)
	{
		//��������
		BYTE cbHeadCardCount = 1, cbTailCardCount = 1;
		//��������
		BYTE cbHeadCardValue = GetCardValue(cbNormalCardData[0]);
		BYTE cbTailCardValue = GetCardValue(cbNormalCardData[cbNormalCardCount - 1]);
		//����ж�
		BYTE i = 0;
		for (i = 1; i < cbNormalCardCount - 1; i++)
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
		if (i == cbNormalCardCount - 1)
		{
			//�仯��ֵ
			BYTE cbMagicValue = 0;
			bool bSplitMagicCard = false;
			if (cbMagicCardCount == 1)
			{
				if (cbHeadCardCount == 2 && cbTailCardCount == 2)
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
				else if (cbHeadCardCount == 1 && cbTailCardCount == 3)
				{
					cbMagicValue = cbHeadCardValue;
				}
				else if (cbHeadCardCount == 3 && cbTailCardCount == 1)
				{
					cbMagicValue = cbTailCardValue;
				}
			}
			else if (cbMagicCardCount == 2)
			{
				if (cbHeadCardCount == 1 && cbTailCardCount == 2)
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
				else if (cbHeadCardCount == 2 && cbTailCardCount == 1)
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
				cbResultCard[cbNormalCardCount++] = MakeCardData(cbHeadCardValue - 1, 0);
				cbResultCard[cbNormalCardCount++] = MakeCardData(cbTailCardValue - 1, 0);
			}
			else
			{
				for (BYTE j = 0; j < cbMagicCardCount; j++)
				{
					cbResultCard[cbNormalCardCount + j] = MakeCardData(cbMagicValue - 1, 0);
				}
			}
			SortCardList(cbResultCard, cbCardCount, ST_COUNT);
			return true;
		}
	}

	//�����ж�
	bool bMagicLink = false;
	if (cbMaxSameCount == 1 && cbCardCount == 5)
	{
		bMagicLink = true;
	}
	if ((cbMaxSameCount == 2 || cbMaxSameCount == 3) && cbCardCount == 6)
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
		for (BYTE cbTimes = 0; cbTimes < 2; cbTimes++)
		{
			//��������
			BYTE cbFillCount = 0;
			BYTE cbLeaveCount = cbNormalCardCount;
			BYTE cbUseableCount = cbMagicCardCount;

			//���״̬
			BYTE cbFillStatus[13] = { 0 };
			//����˿�
			for (BYTE i = 0; i < 14; i++)
			{
				//��������
				BYTE cbIndex = ((cbTimes == 1) ? (13 - i) % 13 : i) % 13;

				//����ж�
				if (cbLeaveCount != cbNormalCardCount || Distributing.cbDistributing[cbIndex][cbIndexCount] != 0)
				{
					//ʧ���ж�
					if (Distributing.cbDistributing[cbIndex][cbIndexCount] + cbUseableCount < cbMaxSameCount) break;

					//��������
					BYTE cbTurnFillCount = 0;
					//�����˿�
					for (BYTE j = 0; j < 4; j++)
					{
						for (BYTE k = 0; k < Distributing.cbDistributing[cbIndex][j]; k++)
						{
							cbLeaveCount--;
							cbTurnFillCount++;
							cbResultCard[cbFillCount++] = MakeCardData(cbIndex, j);
						}
					}

					//�����
					for (BYTE i = cbTurnFillCount; i < cbMaxSameCount; i++)
					{
						cbUseableCount--;
						cbResultCard[cbFillCount++] = MakeCardData(cbIndex, GetCardColor(cbNormalCardData[0]) >> 4);
					}

					//���ñ���
					cbFillStatus[cbIndex] = cbMaxSameCount;
				}
			}

			//�ɹ��ж�
			if (cbUseableCount == 0 && cbLeaveCount == 0)
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
VOID CGameLogic::AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult &AnalyseResult)
{
	//���ý��
	ZeroMemory(&AnalyseResult, sizeof(AnalyseResult));

	//�˿˷���
	for (BYTE i = 0; i < cbCardCount; i++)
	{
		//��������
		BYTE cbSameCount = 1;
		BYTE cbLogicValue = GetCardValue(cbCardData[i]);
		//����ͬ��
		for (BYTE j = i + 1; j < cbCardCount; j++)
		{
			//��ȡ�˿�
			if (GetCardValue(cbCardData[j]) != cbLogicValue) break;

			//���ñ���
			cbSameCount++;
		}

		//���ý��
		BYTE cbIndex = AnalyseResult.cbBlockCount[cbSameCount - 1]++;
		for (BYTE j = 0; j < cbSameCount; j++)
		{
			AnalyseResult.cbCardData[cbSameCount - 1][cbIndex*cbSameCount + j] = cbCardData[i + j];
		}

		//��������
		i += cbSameCount - 1;
	}
}

//�����ֲ�
VOID CGameLogic::AnalysebDistributing(const BYTE cbCardData[], BYTE cbCardCount, tagDistributing &Distributing)
{
	//���ñ���
	ZeroMemory(&Distributing, sizeof(Distributing));

	//���ñ���
	for (BYTE i = 0; i < cbCardCount; i++)
	{
		if (cbCardData[i] == 0) continue;

		//��ȡ����
		BYTE cbCardColor = GetCardColor(cbCardData[i]);
		BYTE cbCardValue = GetCardValue(cbCardData[i]);

		//�ֲ���Ϣ
		Distributing.cbCardCount++;
		Distributing.cbDistributing[cbCardValue - 1][cbIndexCount]++;
		Distributing.cbDistributing[cbCardValue - 1][cbCardColor >> 4]++;
	}
}

//�����˿�
BYTE CGameLogic::MakeCardData(BYTE cbValueIndex, BYTE cbColorIndex)
{
	return (cbColorIndex << 4) | (cbValueIndex + 1);
}

//�Ƿ�����
bool CGameLogic::IsStructureLink(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbCellCount)
{
	//��Ŀ�ж�
	ASSERT((cbCardCount%cbCellCount) == 0);
	if ((cbCardCount%cbCellCount) != 0) return false;

	//�����˿�
	BYTE cbCardDataTemp[MAX_COUNT] = { 0 };
	CopyMemory(cbCardDataTemp, cbCardData, cbCardCount*sizeof(BYTE));
	//�˿�����
	SortCardList(cbCardDataTemp, cbCardCount, ST_VALUE);

	//��������
	BYTE cbBlockCount = cbCardCount / cbCellCount;
	BYTE cbFirstValue = GetCardValue(cbCardDataTemp[0]);

	//��Ч����
	if (cbFirstValue >= 14) return false;

	//�˿�����
	for (BYTE i = 1; i < cbBlockCount; i++)
	{
		//�˿���ֵ
		BYTE cbCardValue = GetCardValue(cbCardDataTemp[i*cbCellCount]);

		//�������
		if (cbCardValue == 1 && cbFirstValue == 13) continue;

		//�����ж�
		if (cbFirstValue != cbCardValue + i) return false;
	}

	//����ж�
	return true;
}

//�Ա��˿�
bool CGameLogic::CompareCardByValue(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount)
{
	//��������
	bool bHaveTwoNext = false;
	BYTE cbConsultNext[2] = { 0x00, 0x00 };

	//�����˿�
	for (BYTE i = 0; i < cbNextCount; i++)
	{
		//��ȡ��ֵ
		BYTE cbConsultValue = GetCardValue(cbNextCard[i]);

		//���ñ���
		if (!bHaveTwoNext && (cbConsultValue == 0x02)) bHaveTwoNext = true;

		//�����˿�
		if (cbConsultValue == 0x01)
		{
			if (14 > cbConsultNext[0]) cbConsultNext[0] = 14;
			if (cbConsultValue > cbConsultNext[1]) cbConsultNext[1] = cbConsultValue;
		}
		else
		{
			if (cbConsultValue > cbConsultNext[0]) cbConsultNext[0] = cbConsultValue;
			if (cbConsultValue > cbConsultNext[1]) cbConsultNext[1] = cbConsultValue;
		}
	}

	//�������� 
	bool bHaveTwoFirst = false;
	BYTE cbConsultFirst[2] = { 0x00, 0x00 };

	//�����˿�
	for (BYTE i = 0; i < cbFirstCount; i++)
	{
		//��ȡ��ֵ
		BYTE cbConsultValue = GetCardValue(cbFirstCard[i]);

		//���ñ���
		if (!bHaveTwoFirst && (cbConsultValue == 0x02)) bHaveTwoFirst = true;

		//�����˿�
		if (cbConsultValue == 0x01)
		{
			if (14 > cbConsultFirst[0]) cbConsultFirst[0] = 14;
			if (cbConsultValue > cbConsultFirst[1]) cbConsultFirst[1] = cbConsultValue;
		}
		else
		{
			if (cbConsultValue > cbConsultFirst[0]) cbConsultFirst[0] = cbConsultValue;
			if (cbConsultValue > cbConsultFirst[1]) cbConsultFirst[1] = cbConsultValue;
		}
	}

	//�Ա��˿�
	BYTE cbResultNext = (bHaveTwoNext == false) ? cbConsultNext[0] : cbConsultNext[1];
	BYTE cbResultFirst = (bHaveTwoFirst == false) ? cbConsultFirst[0] : cbConsultFirst[1];

	return cbResultNext > cbResultFirst;
}

//����˿�
bool CGameLogic::Add(BYTE bAddCard, BYTE  cbCardData[],BYTE cbCardCount/*=27*/)
{
	cbCardData[cbCardCount]=bAddCard;
	SortCardList(cbCardData, cbCardCount + 1, ST_ORDER);
	return true;
}

//��ȡ������
BYTE CGameLogic::GetJingongCard(const BYTE cbCardData[])
{
	BYTE cbPos = 0;
	while(cbPos<3)
	{
		if (GetCardValue(cbCardData[cbPos]) != m_cbMainValue)
		{
			return cbCardData[cbPos];
		}

		++cbPos;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
