#include "StdAfx.h"
#include "GameLogic.h"

//索引变量
const BYTE cbIndexCount = 5;

//////////////////////////////////////////////////////////////////////////
//扑克数据
const BYTE	CGameLogic::m_cbCardData[FULL_COUNT] =
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	    //方块 A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	    //梅花 A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	    //红桃 A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	    //黑桃 A - K
	0x4e,0x4f,                                                              //小王，大王
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	    //方块 A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	    //梅花 A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	    //红桃 A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	    //黑桃 A - K
	0x4e,0x4f                                                               //小王，大王
};

//////////////////////////////////////////////////////////////////////////
//构造函数
CGameLogic::CGameLogic()
{
	//逻辑变量
	m_cbMainValue = 2;
}

//析构函数
CGameLogic::~CGameLogic()
{

}


//获取类型
BYTE CGameLogic::GetCardType(const BYTE cbCardData[], BYTE cbCardCount)
{
	ASSERT(cbCardCount <= 28);
	if (cbCardCount > 28) return CT_ERROR;

	BYTE cbCardDataTemp[MAX_COUNT] = { 0 };
	CopyMemory(cbCardDataTemp, cbCardData, cbCardCount);
	//排序
	SortCardList(cbCardDataTemp, cbCardCount, ST_ORDER);

	//简单牌型
	switch (cbCardCount)
	{
	case 0:	//空牌
	{
		return CT_ERROR;
	}
	case 1: //单牌
	{
		return CT_SINGLE;
	}
	case 2:	//对牌
	{
		return (GetCardValue(cbCardData[0]) == GetCardValue(cbCardData[1])) ? CT_DOUBLE : CT_ERROR;
	}
	case 3:	//三牌
	{
		return (GetCardValue(cbCardData[0]) == GetCardValue(cbCardData[2])) ? CT_THREE : CT_ERROR;
	}
	}

	//分析扑克
	tagAnalyseResult AnalyseResult;
	AnalysebCardData(cbCardData, cbCardCount, AnalyseResult);

	//炸弹类型
	if (cbCardCount == 4 && cbCardDataTemp[0] == 0x4F && cbCardDataTemp[3] == 0x4E) return CT_BOMB_TW;
	if (cbCardCount >= 4 && cbCardCount <= 10 && AnalyseResult.cbBlockCount[cbCardCount - 1] == 1) return CT_BOMB;

	//顺子类型
	if (cbCardCount == 5 && AnalyseResult.cbBlockCount[0] == cbCardCount)
	{
		//扑克属性
		BYTE cbSignedCount = AnalyseResult.cbBlockCount[0];
		BYTE cbCardColor = GetCardColor(AnalyseResult.cbCardData[0], cbSignedCount);
		bool bStructureLink = IsStructureLink(AnalyseResult.cbCardData[0], cbSignedCount, 1);

		//类型判断
		if (bStructureLink&&cbCardColor == 0xF0) return CT_SINGLE_LINK;
		if (bStructureLink&&cbCardColor != 0xF0) return CT_TONG_HUA_SHUN;
	}

	//对连类型
	if (cbCardCount == 6 && AnalyseResult.cbBlockCount[1] * 2 == cbCardCount)
	{
		BYTE cbDoubleCount = AnalyseResult.cbBlockCount[1] * 2;
		if (IsStructureLink(AnalyseResult.cbCardData[1], cbDoubleCount, 2)) return CT_DOUBLE_LINK;
	}

	//三连类型
	if (cbCardCount == 6 && AnalyseResult.cbBlockCount[2] * 3 == cbCardCount)
	{
		BYTE cbThreeCount = AnalyseResult.cbBlockCount[2] * 3;
		if (IsStructureLink(AnalyseResult.cbCardData[2], cbThreeCount, 3)) return CT_THREE_LINK;
	}

	//三带二型
	if (cbCardCount == 5 && AnalyseResult.cbBlockCount[1] == 1 && AnalyseResult.cbBlockCount[2] == 1) return CT_THREE_DOUBLE;

	return CT_ERROR;
}

//获取花色
BYTE CGameLogic::GetCardColor(const BYTE cbCardData[], BYTE cbCardCount)
{
	//效验参数
	ASSERT(cbCardCount > 0);
	if (cbCardCount == 0) return 0xF0;

	//首牌花色
	BYTE cbCardColor = GetCardColor(cbCardData[0]);
	//花色判断
	for (BYTE i = 0; i < cbCardCount; i++)
	{
		if (GetCardColor(cbCardData[i]) != cbCardColor)
		{
			return 0xF0;
		}
	}

	return cbCardColor;
}

//获取牌型
BYTE CGameLogic::GetMagicCardType(const BYTE cbCardData[], BYTE cbCardCount)
{
	BYTE cbMagicData[MAX_COUNT] = { 0 };
	MagicCardData(cbCardData, cbCardCount, cbMagicData);
	return GetCardType(cbMagicData, cbCardCount);
}

//混乱扑克
VOID CGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//混乱准备
	BYTE cbCardData[CountArray(m_cbCardData)];
	CopyMemory(cbCardData, m_cbCardData, sizeof(m_cbCardData));

	//混乱扑克
	BYTE cbRandCount = 0, cbPosition = 0;
	do
	{
		cbPosition = rand() % (cbBufferCount - cbRandCount);
		cbCardBuffer[cbRandCount++] = cbCardData[cbPosition];
		cbCardData[cbPosition] = cbCardData[cbBufferCount - cbRandCount];
	} while (cbRandCount < cbBufferCount);

	return;
}

//排列扑克
VOID CGameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType)
{
	//排序过虑
	if (cbCardCount == 0) return;
	if (cbSortType == ST_CUSTOM) return;

	//转换数值
	BYTE cbSortValue[MAX_COUNT] = { 0 };
	for (BYTE i = 0; i < cbCardCount; i++)
	{
		switch (cbSortType)
		{
		case ST_COUNT:	//数目排序
		case ST_ORDER:	//等级排序
		{
			cbSortValue[i] = GetCardLogicValue(cbCardData[i]);
		}break;
		case ST_VALUE:	//数值排序
		{
			cbSortValue[i] = GetCardValue(cbCardData[i]);
		}break;
		case ST_COLOR:	//花色排序
		{
			cbSortValue[i] = GetCardColor(cbCardData[i]) + GetCardLogicValue(cbCardData[i]);
		}break;
		}
	}

	//排序操作
	bool bSorted = true;
	BYTE cbSwitchData = 0, cbLast = cbCardCount - 1;
	do
	{
		bSorted = true;
		for (BYTE i = 0; i < cbLast; i++)
		{
			if (cbSortValue[i] < cbSortValue[i + 1] || (cbSortValue[i] == cbSortValue[i + 1] && cbCardData[i] < cbCardData[i + 1]))
			{
				//设置标志
				bSorted = false;

				//扑克数据
				cbSwitchData = cbCardData[i];
				cbCardData[i] = cbCardData[i + 1];
				cbCardData[i + 1] = cbSwitchData;

				//排序权位
				cbSwitchData = cbSortValue[i];
				cbSortValue[i] = cbSortValue[i + 1];
				cbSortValue[i + 1] = cbSwitchData;
			}
		}
		cbLast--;
	} while (bSorted == false);

	//数目排序
	if (cbSortType == ST_COUNT)
	{
		//变量定义
		BYTE cbCardIndex = 0;

		//分析扑克
		tagAnalyseResult AnalyseResult;
		AnalysebCardData(cbCardData, cbCardCount, AnalyseResult);

		//提取扑克
		for (BYTE i = 0; i < CountArray(AnalyseResult.cbBlockCount); i++)
		{
			//拷贝扑克
			BYTE cbIndex = CountArray(AnalyseResult.cbBlockCount) - i - 1;
			CopyMemory(&cbCardData[cbCardIndex], AnalyseResult.cbCardData[cbIndex], AnalyseResult.cbBlockCount[cbIndex] * (cbIndex + 1)*sizeof(BYTE));

			//设置索引
			cbCardIndex += AnalyseResult.cbBlockCount[cbIndex] * (cbIndex + 1)*sizeof(BYTE);
		}
	}
	return;
}

//删除扑克
bool CGameLogic::RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount)
{
	//检验数据
	if (cbRemoveCount == 0) return true;

	ASSERT(cbRemoveCount <= cbCardCount);
	if (cbRemoveCount > cbCardCount) return false;

	//定义变量
	BYTE cbDeleteCardCount = 0, cbDeleteCardData[MAX_COUNT] = { 0 };
	if (cbCardCount > CountArray(cbDeleteCardData)) return false;
	CopyMemory(cbDeleteCardData, cbCardData, cbCardCount*sizeof(cbCardData[0]));

	//置零扑克
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
	//清理扑克
	for (BYTE i = 0, cbCardPos = 0; i < cbCardCount; i++)
	{
		if (cbDeleteCardData[i] > 0x00 && cbDeleteCardData[i] <= 0x4F)
		{
			cbCardData[cbCardPos++] = cbDeleteCardData[i];
		}
	}

	return true;
}

//逻辑数值
BYTE CGameLogic::GetCardLogicValue(BYTE cbCardData)
{
	//扑克属性
	BYTE cbCardValue = GetCardValue(cbCardData);
	//主牌扑克
	if (cbCardValue == m_cbMainValue) return 15;

	//逻辑数值
	switch (cbCardValue)
	{
	case 0x01: { return 14; }
	case 0x0E: { return 16; }
	case 0x0F: { return 17; }
	}
	//转换数值
	return cbCardValue;
}

//对比扑克
bool CGameLogic::CompareCard(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount)
{
	//类型判断
	BYTE cbNextType = GetCardType(cbNextCard, cbNextCount);
	BYTE cbFirstType = GetCardType(cbFirstCard, cbFirstCount);

	//炸弹判断
	if (cbFirstType >= CT_TONG_HUA_SHUN || cbNextType >= CT_TONG_HUA_SHUN)
	{
		//天王炸弹
		if (cbNextType == CT_BOMB_TW) return true;
		if (cbFirstType == CT_BOMB_TW) return false;

		//低级类型
		if (cbNextType < CT_TONG_HUA_SHUN) return false;
		if (cbFirstType<CT_TONG_HUA_SHUN) return true;

		//数目对比
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

		//不同类型
		if (cbFirstCount == cbNextCount && cbFirstType != cbNextType)
		{
			if (cbNextType == CT_TONG_HUA_SHUN) return true;
			if (cbFirstType == CT_TONG_HUA_SHUN) return false;
			return false;
		}

		return cbNextCount > cbFirstCount;
	}

	//不同类型
	if (cbFirstType != cbNextType) return false;

	//相同类型
	switch (cbFirstType)
	{
	case CT_SINGLE:				//单牌类型
	case CT_DOUBLE:				//对子类型
	case CT_THREE:				//三条类型
	{
		return GetCardLogicValue(cbNextCard[0]) > GetCardLogicValue(cbFirstCard[0]);
	}
	case CT_SINGLE_LINK:			//单连类型
	case CT_DOUBLE_LINK:			//对连类型
	case CT_THREE_LINK:			//三连类型
	{
		return CompareCardByValue(cbFirstCard, cbNextCard, cbFirstCount, cbNextCount);
	}
	case CT_THREE_DOUBLE:		//三带对型
	{
		//分析扑克
		tagAnalyseResult AnalyseResultNext, AnalyseResultFirst;
		AnalysebCardData(cbNextCard, cbNextCount, AnalyseResultNext);
		AnalysebCardData(cbFirstCard, cbFirstCount, AnalyseResultFirst);
		return GetCardLogicValue(AnalyseResultNext.cbCardData[2][0]) > GetCardLogicValue(AnalyseResultFirst.cbCardData[2][0]);
	}
	}

	//错误断言
	//ASSERT(FALSE);	
	return false;
}

//变幻扑克
bool CGameLogic::MagicCardData(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbResultCard[MAX_COUNT])
{
	//变量定义
	BYTE cbNormalCardData[MAX_COUNT] = { 0 };
	BYTE cbNormalCardCount = 0, cbMagicCardCount = 0;

	//变幻准备
	for (BYTE i = 0; i < cbCardCount; i++)
	{
		//变幻统计
		if (GetCardValue(cbCardData[i]) == m_cbMainValue && GetCardColor(cbCardData[i]) == 0x20)
		{
			cbMagicCardCount++;
		}
		else
		{
			cbNormalCardData[cbNormalCardCount++] = cbCardData[i];
		}
	}

	//失败判断
	if (cbMagicCardCount == 0 || cbNormalCardCount == 0)
	{
		CopyMemory(cbResultCard, cbCardData, cbCardCount*sizeof(BYTE));
		return false;
	}

	//获取数值
	BYTE cbCardLogicValue = GetCardLogicValue(cbNormalCardData[0]);
	//同牌变幻
	if (cbCardLogicValue <= 15)
	{
		BYTE cbSameCount = 0;
		for (cbSameCount = 1; cbSameCount < cbNormalCardCount; cbSameCount++)
		{
			if (GetCardLogicValue(cbNormalCardData[cbSameCount]) != cbCardLogicValue) break;
		}
		if (cbSameCount == cbNormalCardCount)
		{
			//设置结果
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

	//扑克分析
	BYTE cbMaxSameCount = 1, cbSameCount = 1;
	for (BYTE i = 1; i < cbNormalCardCount; i++)
	{
		//获取扑克
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

	//三带二型
	if (cbCardCount == 5 && cbMaxSameCount <= 3)
	{
		//变量定义
		BYTE cbHeadCardCount = 1, cbTailCardCount = 1;
		//变量定义
		BYTE cbHeadCardValue = GetCardValue(cbNormalCardData[0]);
		BYTE cbTailCardValue = GetCardValue(cbNormalCardData[cbNormalCardCount - 1]);
		//变幻判断
		BYTE i = 0;
		for (i = 1; i < cbNormalCardCount - 1; i++)
		{
			//获取数值
			BYTE cbCardValue = GetCardValue(cbNormalCardData[i]);

			//扑克判断
			if (cbCardValue == cbHeadCardValue)
			{
				cbHeadCardCount++;
				continue;
			}
			//扑克判断
			if (cbCardValue == cbTailCardValue)
			{
				cbTailCardCount++;
				continue;
			}
			break;
		}

		//成功判断
		if (i == cbNormalCardCount - 1)
		{
			//变化数值
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

	//连牌判断
	bool bMagicLink = false;
	if (cbMaxSameCount == 1 && cbCardCount == 5)
	{
		bMagicLink = true;
	}
	if ((cbMaxSameCount == 2 || cbMaxSameCount == 3) && cbCardCount == 6)
	{
		bMagicLink = true;
	}

	//连牌变幻
	if (bMagicLink)
	{
		//分析分布
		tagDistributing Distributing;
		AnalysebDistributing(cbNormalCardData, cbNormalCardCount, Distributing);

		//填充分析
		for (BYTE cbTimes = 0; cbTimes < 2; cbTimes++)
		{
			//变量定义
			BYTE cbFillCount = 0;
			BYTE cbLeaveCount = cbNormalCardCount;
			BYTE cbUseableCount = cbMagicCardCount;

			//填充状态
			BYTE cbFillStatus[13] = { 0 };
			//填充扑克
			for (BYTE i = 0; i < 14; i++)
			{
				//变量定义
				BYTE cbIndex = ((cbTimes == 1) ? (13 - i) % 13 : i) % 13;

				//填充判断
				if (cbLeaveCount != cbNormalCardCount || Distributing.cbDistributing[cbIndex][cbIndexCount] != 0)
				{
					//失败判断
					if (Distributing.cbDistributing[cbIndex][cbIndexCount] + cbUseableCount < cbMaxSameCount) break;

					//变量定义
					BYTE cbTurnFillCount = 0;
					//常规扑克
					for (BYTE j = 0; j < 4; j++)
					{
						for (BYTE k = 0; k < Distributing.cbDistributing[cbIndex][j]; k++)
						{
							cbLeaveCount--;
							cbTurnFillCount++;
							cbResultCard[cbFillCount++] = MakeCardData(cbIndex, j);
						}
					}

					//填充变幻
					for (BYTE i = cbTurnFillCount; i < cbMaxSameCount; i++)
					{
						cbUseableCount--;
						cbResultCard[cbFillCount++] = MakeCardData(cbIndex, GetCardColor(cbNormalCardData[0]) >> 4);
					}

					//设置变量
					cbFillStatus[cbIndex] = cbMaxSameCount;
				}
			}

			//成功判断
			if (cbUseableCount == 0 && cbLeaveCount == 0)
			{
				SortCardList(cbResultCard, cbCardCount, ST_ORDER);
				return true;
			}
		}
	}

	//设置扑克
	CopyMemory(cbResultCard, cbCardData, cbCardCount*sizeof(BYTE));
	return false;
}

//分析扑克
VOID CGameLogic::AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult &AnalyseResult)
{
	//设置结果
	ZeroMemory(&AnalyseResult, sizeof(AnalyseResult));

	//扑克分析
	for (BYTE i = 0; i < cbCardCount; i++)
	{
		//变量定义
		BYTE cbSameCount = 1;
		BYTE cbLogicValue = GetCardValue(cbCardData[i]);
		//搜索同牌
		for (BYTE j = i + 1; j < cbCardCount; j++)
		{
			//获取扑克
			if (GetCardValue(cbCardData[j]) != cbLogicValue) break;

			//设置变量
			cbSameCount++;
		}

		//设置结果
		BYTE cbIndex = AnalyseResult.cbBlockCount[cbSameCount - 1]++;
		for (BYTE j = 0; j < cbSameCount; j++)
		{
			AnalyseResult.cbCardData[cbSameCount - 1][cbIndex*cbSameCount + j] = cbCardData[i + j];
		}

		//设置索引
		i += cbSameCount - 1;
	}
}

//分析分布
VOID CGameLogic::AnalysebDistributing(const BYTE cbCardData[], BYTE cbCardCount, tagDistributing &Distributing)
{
	//设置变量
	ZeroMemory(&Distributing, sizeof(Distributing));

	//设置变量
	for (BYTE i = 0; i < cbCardCount; i++)
	{
		if (cbCardData[i] == 0) continue;

		//获取属性
		BYTE cbCardColor = GetCardColor(cbCardData[i]);
		BYTE cbCardValue = GetCardValue(cbCardData[i]);

		//分布信息
		Distributing.cbCardCount++;
		Distributing.cbDistributing[cbCardValue - 1][cbIndexCount]++;
		Distributing.cbDistributing[cbCardValue - 1][cbCardColor >> 4]++;
	}
}

//构造扑克
BYTE CGameLogic::MakeCardData(BYTE cbValueIndex, BYTE cbColorIndex)
{
	return (cbColorIndex << 4) | (cbValueIndex + 1);
}

//是否连牌
bool CGameLogic::IsStructureLink(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbCellCount)
{
	//数目判断
	ASSERT((cbCardCount%cbCellCount) == 0);
	if ((cbCardCount%cbCellCount) != 0) return false;

	//构造扑克
	BYTE cbCardDataTemp[MAX_COUNT] = { 0 };
	CopyMemory(cbCardDataTemp, cbCardData, cbCardCount*sizeof(BYTE));
	//扑克排序
	SortCardList(cbCardDataTemp, cbCardCount, ST_VALUE);

	//变量定义
	BYTE cbBlockCount = cbCardCount / cbCellCount;
	BYTE cbFirstValue = GetCardValue(cbCardDataTemp[0]);

	//无效过虑
	if (cbFirstValue >= 14) return false;

	//扑克搜索
	for (BYTE i = 1; i < cbBlockCount; i++)
	{
		//扑克数值
		BYTE cbCardValue = GetCardValue(cbCardDataTemp[i*cbCellCount]);

		//特殊过虑
		if (cbCardValue == 1 && cbFirstValue == 13) continue;

		//连牌判断
		if (cbFirstValue != cbCardValue + i) return false;
	}

	//结果判断
	return true;
}

//对比扑克
bool CGameLogic::CompareCardByValue(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount)
{
	//变量定义
	bool bHaveTwoNext = false;
	BYTE cbConsultNext[2] = { 0x00, 0x00 };

	//参照扑克
	for (BYTE i = 0; i < cbNextCount; i++)
	{
		//获取数值
		BYTE cbConsultValue = GetCardValue(cbNextCard[i]);

		//设置变量
		if (!bHaveTwoNext && (cbConsultValue == 0x02)) bHaveTwoNext = true;

		//设置扑克
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

	//变量定义 
	bool bHaveTwoFirst = false;
	BYTE cbConsultFirst[2] = { 0x00, 0x00 };

	//参照扑克
	for (BYTE i = 0; i < cbFirstCount; i++)
	{
		//获取数值
		BYTE cbConsultValue = GetCardValue(cbFirstCard[i]);

		//设置变量
		if (!bHaveTwoFirst && (cbConsultValue == 0x02)) bHaveTwoFirst = true;

		//设置扑克
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

	//对比扑克
	BYTE cbResultNext = (bHaveTwoNext == false) ? cbConsultNext[0] : cbConsultNext[1];
	BYTE cbResultFirst = (bHaveTwoFirst == false) ? cbConsultFirst[0] : cbConsultFirst[1];

	return cbResultNext > cbResultFirst;
}

//添加扑克
bool CGameLogic::Add(BYTE bAddCard, BYTE  cbCardData[],BYTE cbCardCount/*=27*/)
{
	cbCardData[cbCardCount]=bAddCard;
	SortCardList(cbCardData, cbCardCount + 1, ST_ORDER);
	return true;
}

//获取进贡牌
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
