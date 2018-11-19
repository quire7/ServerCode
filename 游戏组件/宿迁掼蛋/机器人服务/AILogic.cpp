#include "StdAfx.h"
#include "AILogic.h"

//索引变量
const BYTE cbIndexCount=5;

//////////////////////////////////////////////////////////////////////////
//构造函数
CAILogic::CAILogic()
{
	//逻辑变量
	m_cbMainValue = 2;
	m_wRoundOutTimes = 0x0700;
}

//析构函数
CAILogic::~CAILogic()
{

}

//获取类型
BYTE CAILogic::GetCardType(const BYTE cbCardData[], BYTE cbCardCount)
{
	ASSERT(cbCardCount<=28);
	if (cbCardCount>28) return AI_CT_ERROR;

	BYTE cbCardDataTemp[MAX_COUNT] = {0};
	CopyMemory(cbCardDataTemp, cbCardData, cbCardCount);
	//排序
	SortCardList(cbCardDataTemp, cbCardCount, ST_ORDER);

	//简单牌型
	switch (cbCardCount)
	{
	case 0:	//空牌
		{
			return AI_CT_ERROR;
		}
	case 1: //单牌
		{
			return AI_CT_SINGLE;
		}
	case 2:	//对牌
		{
			return (GetCardValue(cbCardData[0])==GetCardValue(cbCardData[1])) ? AI_CT_DOUBLE : AI_CT_ERROR;
		}
	case 3:	//三牌
		{
			return (GetCardValue(cbCardData[0])==GetCardValue(cbCardData[2])) ? AI_CT_THREE : AI_CT_ERROR;
		}
	}

	//分析扑克
	tagAnalyseResult AnalyseResult;
	AnalysebCardData(cbCardData, cbCardCount, AnalyseResult);

	//炸弹类型
	if (cbCardCount==4&&cbCardDataTemp[0]==0x4F&&cbCardDataTemp[3]==0x4E) return AI_CT_BOMB_TW;
	if (cbCardCount>=4&&cbCardCount<=10&&AnalyseResult.cbBlockCount[cbCardCount-1]==1) return AI_CT_BOMB;

	//顺子类型
	if (cbCardCount==5 && AnalyseResult.cbBlockCount[0]==cbCardCount)
	{
		//扑克属性
		BYTE cbSignedCount = AnalyseResult.cbBlockCount[0];
		BYTE cbCardColor = GetCardColor(AnalyseResult.cbCardData[0], cbSignedCount);
		bool bStructureLink = IsStructureLink(AnalyseResult.cbCardData[0], cbSignedCount, 1);

		//类型判断
		if (bStructureLink&&cbCardColor==0xF0) return AI_CT_SINGLE_LINK;
		if (bStructureLink&&cbCardColor!=0xF0) return AI_CT_TONG_HUA_SHUN;
	}

	//对连类型
	if (cbCardCount==6 && AnalyseResult.cbBlockCount[1]*2==cbCardCount)
	{
		BYTE cbDoubleCount = AnalyseResult.cbBlockCount[1]*2;
		if (IsStructureLink(AnalyseResult.cbCardData[1], cbDoubleCount, 2)) return AI_CT_DOUBLE_LINK;
	}

	//三连类型
	if (cbCardCount==6 && AnalyseResult.cbBlockCount[2]*3==cbCardCount)
	{
		BYTE cbThreeCount = AnalyseResult.cbBlockCount[2]*3;
		if (IsStructureLink(AnalyseResult.cbCardData[2], cbThreeCount, 3)) return AI_CT_THREE_LINK;
	}

	//三带二型
	if (cbCardCount==5&&AnalyseResult.cbBlockCount[1]==1&&AnalyseResult.cbBlockCount[2]==1) return AI_CT_THREE_DOUBLE;

	return AI_CT_ERROR;
}

//获取花色
BYTE CAILogic::GetCardColor(const BYTE cbCardData[], BYTE cbCardCount)
{
	//效验参数
	ASSERT(cbCardCount>0);
	if (cbCardCount==0) return 0xF0;

	//首牌花色
	BYTE cbCardColor = GetCardColor(cbCardData[0]);
	//花色判断
	for (BYTE i=0; i<cbCardCount; i++)
	{
		if (GetCardColor(cbCardData[i]) != cbCardColor)
		{
			return 0xF0;
		}
	}

	return cbCardColor;
}

//排列扑克
VOID CAILogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType)
{
	//排序过虑
	if (cbCardCount==0) return;
	if (cbSortType==ST_CUSTOM) return;

	//转换数值
	BYTE cbSortValue[MAX_COUNT] = {0};
	for (BYTE i=0; i<cbCardCount; i++)
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
				cbSortValue[i] = GetCardColor(cbCardData[i])+GetCardLogicValue(cbCardData[i]);
			}break;
		}
	}

	//排序操作
	bool bSorted = true;
	BYTE cbSwitchData=0, cbLast=cbCardCount-1;
	do
	{
		bSorted = true;
		for (BYTE i=0; i<cbLast; i++)
		{
			if (cbSortValue[i]<cbSortValue[i+1] || (cbSortValue[i]==cbSortValue[i+1]&&cbCardData[i]<cbCardData[i+1]))
			{
				//设置标志
				bSorted = false;

				//扑克数据
				cbSwitchData = cbCardData[i];
				cbCardData[i] = cbCardData[i+1];
				cbCardData[i+1] = cbSwitchData;

				//排序权位
				cbSwitchData = cbSortValue[i];
				cbSortValue[i] = cbSortValue[i+1];
				cbSortValue[i+1] = cbSwitchData;
			}	
		}
		cbLast--;
	} while(bSorted==false);

	//数目排序
	if (cbSortType == ST_COUNT)
	{
		//变量定义
		BYTE cbCardIndex = 0;

		//分析扑克
		tagAnalyseResult AnalyseResult;
		AnalysebCardData(cbCardData, cbCardCount, AnalyseResult);

		//提取扑克
		for (BYTE i=0; i<CountArray(AnalyseResult.cbBlockCount); i++)
		{
			//拷贝扑克
			BYTE cbIndex = CountArray(AnalyseResult.cbBlockCount)-i-1;
			CopyMemory(&cbCardData[cbCardIndex], AnalyseResult.cbCardData[cbIndex], AnalyseResult.cbBlockCount[cbIndex]*(cbIndex+1)*sizeof(BYTE));

			//设置索引
			cbCardIndex += AnalyseResult.cbBlockCount[cbIndex]*(cbIndex+1)*sizeof(BYTE);
		}
	}
	return;
}

//删除扑克
bool CAILogic::RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount)
{
	//检验数据
	if (cbRemoveCount==0) return true;

	ASSERT(cbRemoveCount<=cbCardCount);
	if (cbRemoveCount>cbCardCount) return false;

	//定义变量
	BYTE cbDeleteCardCount=0, cbDeleteCardData[MAX_COUNT]={0};
	if (cbCardCount>CountArray(cbDeleteCardData)) return false;
	CopyMemory(cbDeleteCardData, cbCardData, cbCardCount*sizeof(cbCardData[0]));

	//置零扑克
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
	//清理扑克
	for (BYTE i=0,cbCardPos=0; i<cbCardCount; i++)
	{
		if (cbDeleteCardData[i]>0x00 && cbDeleteCardData[i]<=0x4F) 
		{
			cbCardData[cbCardPos++]=cbDeleteCardData[i];
		}
	}

	return true;
}

//逻辑数值
BYTE CAILogic::GetCardLogicValue(BYTE cbCardData)
{
	//扑克属性
	BYTE cbCardValue = GetCardValue(cbCardData);
	//主牌扑克
	if (cbCardValue==m_cbMainValue) return 15;

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

//按变幻牌分析扑克
VOID CAILogic::AnalyseMagicData(const BYTE cbHandCardData[], BYTE cbHandCardCount, tagMagicResult &MagicResult)
{
	ZeroMemory(&MagicResult, sizeof(MagicResult));
	//验证
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

//对比扑克
bool CAILogic::CompareCard(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount)
{
	//类型判断
	BYTE cbNextType = GetCardType(cbNextCard, cbNextCount);
	BYTE cbFirstType = GetCardType(cbFirstCard, cbFirstCount);

	//炸弹判断
	if (cbFirstType>=AI_CT_TONG_HUA_SHUN || cbNextType>=AI_CT_TONG_HUA_SHUN)
	{
		//天王炸弹
		if (cbNextType==AI_CT_BOMB_TW) return true;
		if (cbFirstType==AI_CT_BOMB_TW) return false;

		//低级类型
		if (cbNextType<AI_CT_TONG_HUA_SHUN) return false;
		if (cbFirstType<AI_CT_TONG_HUA_SHUN) return true;

		//数目对比
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

		//不同类型
		if (cbFirstCount==cbNextCount && cbFirstType!=cbNextType)
		{
			if (cbNextType==AI_CT_TONG_HUA_SHUN) return true;
			if (cbFirstType==AI_CT_TONG_HUA_SHUN) return false;
			return false;
		}

		return cbNextCount>cbFirstCount;
	}

	//不同类型
	if (cbFirstType!=cbNextType) return false;

	//相同类型
	switch (cbFirstType)
	{
	case AI_CT_SINGLE:				//单牌类型
	case AI_CT_DOUBLE:				//对子类型
	case AI_CT_THREE:				//三条类型
		{
			return GetCardLogicValue(cbNextCard[0])>GetCardLogicValue(cbFirstCard[0]);
		}
	case AI_CT_SINGLE_LINK:			//单连类型
	case AI_CT_DOUBLE_LINK:			//对连类型
	case AI_CT_THREE_LINK:			//三连类型
		{
			return CompareCardByValue(cbFirstCard, cbNextCard, cbFirstCount, cbNextCount);
		}
	case AI_CT_THREE_DOUBLE:		//三带对型
		{
			//分析扑克
			tagAnalyseResult AnalyseResultNext, AnalyseResultFirst;
			AnalysebCardData(cbNextCard, cbNextCount, AnalyseResultNext);
			AnalysebCardData(cbFirstCard, cbFirstCount, AnalyseResultFirst);
			return GetCardLogicValue(AnalyseResultNext.cbCardData[2][0])>GetCardLogicValue(AnalyseResultFirst.cbCardData[2][0]);
		}
	}

	//错误断言
	//ASSERT(FALSE);	
	return false;
}

//变幻扑克
bool CAILogic::MagicCardData(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbResultCard[MAX_COUNT])
{
	//变量定义
	BYTE cbNormalCardData[MAX_COUNT] = {0};
	BYTE cbNormalCardCount=0, cbMagicCardCount=0;

	//变幻准备
	for (BYTE i=0; i<cbCardCount; i++)
	{
		//变幻统计
		if (GetCardValue(cbCardData[i])==m_cbMainValue && GetCardColor(cbCardData[i])==0x20)
		{
			cbMagicCardCount++;
		}
		else 
		{
			cbNormalCardData[cbNormalCardCount++] = cbCardData[i];
		}
	}

	//失败判断
	if (cbMagicCardCount==0 || cbNormalCardCount==0)
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
		for (cbSameCount=1; cbSameCount<cbNormalCardCount; cbSameCount++)
		{
			if (GetCardLogicValue(cbNormalCardData[cbSameCount])!=cbCardLogicValue) break;
		}
		if (cbSameCount == cbNormalCardCount)
		{
			//设置结果
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

	//扑克分析
	BYTE cbMaxSameCount=1, cbSameCount=1;
	for (BYTE i=1; i<cbNormalCardCount; i++)
	{
		//获取扑克
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

	//三带二型
	if (cbCardCount==5 && cbMaxSameCount<=3)
	{
		//变量定义
		BYTE cbHeadCardCount=1, cbTailCardCount=1;
		//变量定义
		BYTE cbHeadCardValue = GetCardValue(cbNormalCardData[0]);
		BYTE cbTailCardValue = GetCardValue(cbNormalCardData[cbNormalCardCount-1]);
		//变幻判断
		BYTE i = 0;
		for (i=1; i<cbNormalCardCount-1; i++)
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
		if (i == cbNormalCardCount-1)
		{
			//变化数值
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

	//连牌判断
	bool bMagicLink = false;
	if (cbMaxSameCount==1 && cbCardCount==5)
	{
		bMagicLink = true;
	}
	if ((cbMaxSameCount==2||cbMaxSameCount==3) && cbCardCount==6) 
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
		for (BYTE cbTimes=0; cbTimes<2; cbTimes++)
		{
			//变量定义
			BYTE cbFillCount = 0;
			BYTE cbLeaveCount = cbNormalCardCount;
			BYTE cbUseableCount = cbMagicCardCount;

			//填充状态
			BYTE cbFillStatus[13] = {0};
			//填充扑克
			for (BYTE i=0; i<14; i++)
			{
				//变量定义
				BYTE cbIndex = ((cbTimes==1)?(13-i)%13:i)%13;

				//填充判断
				if (cbLeaveCount!=cbNormalCardCount || Distributing.cbDistributing[cbIndex][cbIndexCount]!=0)
				{
					//失败判断
					if (Distributing.cbDistributing[cbIndex][cbIndexCount]+cbUseableCount < cbMaxSameCount) break;

					//变量定义
					BYTE cbTurnFillCount = 0;
					//常规扑克
					for (BYTE j=0; j<4; j++)
					{
						for (BYTE k=0; k<Distributing.cbDistributing[cbIndex][j]; k++)
						{
							cbLeaveCount--;
							cbTurnFillCount++;
							cbResultCard[cbFillCount++] = MakeCardData(cbIndex, j);
						}
					}

					//填充变幻
					for (BYTE i=cbTurnFillCount; i<cbMaxSameCount; i++)
					{
						cbUseableCount--;
						cbResultCard[cbFillCount++] = MakeCardData(cbIndex, GetCardColor(cbNormalCardData[0])>>4);
					}

					//设置变量
					cbFillStatus[cbIndex] = cbMaxSameCount;
				}
			}

			//成功判断
			if (cbUseableCount==0 && cbLeaveCount==0)
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
VOID CAILogic::AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult &AnalyseResult)
{
	//设置结果
	ZeroMemory(&AnalyseResult, sizeof(AnalyseResult));

	//扑克分析
	for (BYTE i=0; i<cbCardCount; i++)
	{
		//变量定义
		BYTE cbSameCount = 1;
		BYTE cbLogicValue = GetCardValue(cbCardData[i]);
		//搜索同牌
		for (BYTE j=i+1; j<cbCardCount; j++)
		{
			//获取扑克
			if (GetCardValue(cbCardData[j])!=cbLogicValue) break;

			//设置变量
			cbSameCount++;
		}

		//设置结果
		BYTE cbIndex = AnalyseResult.cbBlockCount[cbSameCount-1]++;
		for (BYTE j=0; j<cbSameCount; j++)
		{
			AnalyseResult.cbCardData[cbSameCount-1][cbIndex*cbSameCount+j] = cbCardData[i+j];
		}

		//设置索引
		i += cbSameCount-1;
	}
}

//分析分布
VOID CAILogic::AnalysebDistributing(const BYTE cbCardData[], BYTE cbCardCount, tagDistributing &Distributing)
{
	//设置变量
	ZeroMemory(&Distributing, sizeof(Distributing));

	//设置变量
	for (BYTE i=0; i<cbCardCount; i++)
	{
		if (cbCardData[i]==0) continue;

		//获取属性
		BYTE cbCardColor = GetCardColor(cbCardData[i]);
		BYTE cbCardValue = GetCardValue(cbCardData[i]);

		//分布信息
		Distributing.cbCardCount++;
		Distributing.cbDistributing[cbCardValue-1][cbIndexCount]++;
		Distributing.cbDistributing[cbCardValue-1][cbCardColor>>4]++;
	}
}

//构造扑克
BYTE CAILogic::MakeCardData(BYTE cbValueIndex, BYTE cbColorIndex)
{
	return (cbColorIndex<<4)|(cbValueIndex+1);
}

//是否连牌
bool CAILogic::IsStructureLink(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbCellCount)
{
	//数目判断
	ASSERT((cbCardCount%cbCellCount)==0);
	if ((cbCardCount%cbCellCount)!=0) return false;
	
	//构造扑克
	BYTE cbCardDataTemp[MAX_COUNT] = {0};
	CopyMemory(cbCardDataTemp, cbCardData, cbCardCount*sizeof(BYTE));
	//扑克排序
	SortCardList(cbCardDataTemp, cbCardCount, ST_VALUE);

	//变量定义
	BYTE cbBlockCount = cbCardCount/cbCellCount;
	BYTE cbFirstValue = GetCardValue(cbCardDataTemp[0]);

	//无效过虑
	if (cbFirstValue>=14) return false;

	//扑克搜索
	for (BYTE i=1; i<cbBlockCount; i++)
	{
		//扑克数值
		BYTE cbCardValue = GetCardValue(cbCardDataTemp[i*cbCellCount]);

		//特殊过虑
		if (cbCardValue==1&&cbFirstValue==13) continue;

		//连牌判断
		if (cbFirstValue!=cbCardValue+i) return false;
	}

	//结果判断
	return true;
}

//对比扑克
bool CAILogic::CompareCardByValue(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount)
{
	//变量定义
	bool bHaveTwoNext = false;
	BYTE cbConsultNext[2] = {0x00,0x00};

	//参照扑克
	for (BYTE i=0; i<cbNextCount; i++)
	{
		//获取数值
		BYTE cbConsultValue = GetCardValue(cbNextCard[i]);

		//设置变量
		if (!bHaveTwoNext&&(cbConsultValue==0x02)) bHaveTwoNext=true;

		//设置扑克
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

	//变量定义 
	bool bHaveTwoFirst = false;
	BYTE cbConsultFirst[2] = {0x00,0x00};

	//参照扑克
	for (BYTE i=0; i<cbFirstCount; i++)
	{
		//获取数值
		BYTE cbConsultValue = GetCardValue(cbFirstCard[i]);

		//设置变量
		if (!bHaveTwoFirst&&(cbConsultValue==0x02)) bHaveTwoFirst = true;

		//设置扑克
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

	//对比扑克
	BYTE cbResultNext = (bHaveTwoNext==false)?cbConsultNext[0]:cbConsultNext[1];
	BYTE cbResultFirst = (bHaveTwoFirst==false)?cbConsultFirst[0]:cbConsultFirst[1];

	return cbResultNext>cbResultFirst;
}

//拷贝所有用户手牌信息
void CAILogic::OnHandCardChange(const BYTE cbAllUserHandCardData[][MAX_COUNT], const BYTE cbAllUserHandCardCount[])
{
	ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));
	CopyMemory(m_cbHandCardCount, cbAllUserHandCardCount, sizeof(m_cbHandCardCount));
	CopyMemory(m_cbHandCardData[0], cbAllUserHandCardData[0], m_cbHandCardCount[0]);
	CopyMemory(m_cbHandCardData[1], cbAllUserHandCardData[1], m_cbHandCardCount[1]);
	CopyMemory(m_cbHandCardData[2], cbAllUserHandCardData[2], m_cbHandCardCount[2]);
	CopyMemory(m_cbHandCardData[3], cbAllUserHandCardData[3], m_cbHandCardCount[3]);
}

//设置用户手牌
void CAILogic::SetUserHandCard(WORD wUserID, const BYTE cbUserCardData[], BYTE cbUserCardCount)
{
	ZeroMemory(m_cbHandCardData[wUserID], MAX_COUNT);
	CopyMemory(m_cbHandCardData[wUserID], cbUserCardData, cbUserCardCount);
	m_cbHandCardCount[wUserID] = cbUserCardCount;
}

//判断一个用户是否是危险用户
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
	//就剩一个炸弹和一手牌
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

//排除手牌中的炸弹后，手牌的牌型
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

//搜索同花顺
bool CAILogic::SearchTongHuaShun(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, tagOutCardResult &OutCardResult)
{
	//A封顶,直接返回
	if (GetCardValue(cbReferCard)==1) return false;
	if (cbHandCardCount<5) return false;

	//分析扑克
	tagDistributing Distribute;
	AnalysebDistributing(cbHandCardData, cbHandCardCount, Distribute);

	//确定搜索开始位置
	BYTE cbReferIndex = (cbReferCard!=0) ? GetCardValue(cbReferCard)-4 : 0;
	BYTE cbColorCount[4] = {0,0,0,0};
	//搜索常规同花顺
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
			//判断是否搜索足够
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
	//搜索10到A的同花顺
	for (BYTE i=0; i<4; i++)
	{
		//如果有顺
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

//变幻搜索同花顺
bool CAILogic::SearchMagicTongHuaShun( const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, tagOutCardResult &OutCardResult )
{
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));

	//A封顶,直接返回
	if (GetCardValue(cbReferCard)==1) return false;
	if (cbHandCardCount<5) return false;

	//分析扑克
	tagMagicResult MagicResult;
	AnalyseMagicData(cbHandCardData, cbHandCardCount, MagicResult);
	if (MagicResult.cbMagicCardCount==0) return false;

	//分析扑克
	tagDistributing Distribute;
	AnalysebDistributing(MagicResult.cbNormalCardData, MagicResult.cbNormalCardCount, Distribute);

	//确定开始搜索位置
	BYTE cbReferIndex = (cbReferCard!=0) ? GetCardValue(cbReferCard)-4 : 0;
	BYTE cbStartPsition=0, cbStartColor=0;
	bool bFind = false;
	for (BYTE cbMagicCount=1; cbMagicCount<=MagicResult.cbMagicCardCount; cbMagicCount++)
	{
		for (BYTE i=0; i<4; i++)//花色
		{
			for (int j=cbReferIndex; j<10; j++)//
			{
				BYTE cbInvalidCount = 0;	//缺位数
				BYTE cbLianCount = 0;		//相连数
				for (int k=0; k<5; k++)
				{
					if (Distribute.cbDistributing[j+k][cbIndexCount]==0 || Distribute.cbDistributing[j+k][i]==0)
					{
						if (k==0 && j!= 9) break; //如果第一个(除了jQKA的情况)就是空的就跳出
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
		//排序
		return true;
	}
	return false;
}

//相邻的花色数量
bool CAILogic::NearbyColorCount(BYTE cbHandCardData[MAX_COUNT], BYTE cbHandCardCount, BYTE cbCardData, BYTE cbColorCount[4])
{
	//如果是王牌
	if (cbCardData==0x4E || cbCardData==0x4F) return false;

	//初始化变量
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

	//如果是A有大顺
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

//分析牌型
void CAILogic::AnalyseOutCardType(BYTE const cbHandCardData[], BYTE const cbHandCardCount, tagOutCardTypeResult CardTypeResult[TYPE_COUNT])
{
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult[0])*TYPE_COUNT);

	BYTE cbCardDataTemp[MAX_COUNT] = {0};
	BYTE cbCardCountTemp = cbHandCardCount;
	CopyMemory(cbCardDataTemp, cbHandCardData, cbHandCardCount);
	SortCardList(cbCardDataTemp, cbHandCardCount, ST_ORDER);

	//同花顺分析
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
	
	//普通牌型分析
	tagOutCardTypeResult CardTypeResultNomal[TYPE_COUNT];
	AnalyseOutCardTypeNomal(cbCardDataTemp, cbCardCountTemp, CardTypeResultNomal);

	//复制同花顺
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

	//验证结果
	for (BYTE cbType=AI_CT_SINGLE; cbType<=AI_CT_BOMB_TW; cbType++)
	{
		if (CardTypeResult[cbType].cbCardTypeCount > 0)
		{
			for (BYTE i=0; i<CardTypeResult[cbType].cbCardTypeCount; i++)
			{
				//验证
				CheckOutCardRightType(CardTypeResult[cbType].cbEachHandCardData[i], CardTypeResult[cbType].cbEachHandCardCount[i], cbType);
			}
		}
	}
}

//设置初始值
void CAILogic::SetInitData()
{
	//剩余的牌
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

	//大小王的数量
	memset(m_cbKindCount, 0, sizeof(m_cbKindCount));
	//出牌类型记录
	memset(m_PlayerTypeImfo, 0, sizeof(m_PlayerTypeImfo));
	//出牌记录信息
	m_ForOutImfo.cbOutCardCount = 0;
	ZeroMemory(m_ForOutImfo.cbOutCardData, sizeof(m_ForOutImfo.cbOutCardData));
	m_ForOutImfo.wOutChairID = INVALID_CHAIR;
	m_CurPlayerOutTimes.cbCurPlaOutCount = 0;
	m_CurPlayerOutTimes.cbEnemyOutCount = 0;
	m_CurPlayerOutTimes.wCurrentChairID = INVALID_CHAIR;
	m_CurPlayerOutTimes.cbEnemyFirstCount = 0;
	//首次出牌玩家记录
	m_bFirstOutCard = false;

	//各玩家最优牌
	memset(&m_PlayerBestOutType, 0, sizeof(m_PlayerBestOutType));
	//剩余扑克数目
	memset(m_cbCardNum, (MAX_COUNT-1), sizeof(m_cbCardNum));
}

//记录剩余的牌
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

//记录大小王(出牌玩家不要牌)
void CAILogic::RecordKindCard(WORD wMeChairID, BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wPassaChairID, WORD wOutChairID, BYTE cbTurnCardData[], BYTE cbTurnCardCount, PlayerOutImfomation ForOutImfo)
{
	//出牌记录
	//已经没有大小王
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
	//去除大小王
	if (wPassaChairID == INVALID_CHAIR)
	{
		ASSERT(wOutChairID != INVALID_CHAIR);
		BYTE cbCardData[MAX_COUNT]={0};
		CopyMemory(cbCardData, cbTurnCardData, cbTurnCardCount);
		SortCardList(cbCardData, cbTurnCardCount, ST_ORDER);
		for (BYTE i=0; i<cbTurnCardCount; i++)
		{
			BYTE cbLogicValue = GetCardLogicValue(cbCardData[i]);
			//排除不是大小王的牌
			if (cbLogicValue<17) break;
				
			if (m_cbKindCount[wOutChairID][cbLogicValue-17] > 0)
			{
				m_cbKindCount[wOutChairID][cbLogicValue-17] --;
			}
		}
	}
	//单张进行大小王的判断
	BYTE cbType = (wPassaChairID==INVALID_CHAIR) ? GetCardType(cbTurnCardData,cbTurnCardCount) : GetCardType(m_ForOutImfo.cbOutCardData,m_ForOutImfo.cbOutCardCount);
	if (cbType != AI_CT_SINGLE)  return;

	BYTE cbOutLogicValue = GetCardLogicValue(ForOutImfo.cbOutCardData[0]);	
	if (cbOutLogicValue==15 || cbOutLogicValue==17)
	{
		//出牌
		if (wPassaChairID == INVALID_CHAIR)
		{	
			int iCount = (int)(m_iOutCardCount[17] - m_cbKindCount[wMeChairID][0]);
			if (cbOutLogicValue==15 && iCount==1 && cbTurnCardData[0]==0x4F)
			{
				//出主牌，接大王（没有小王存在（小王一张））。		
				m_cbKindCount[wOutChairID][0] = 0;

				if (ForOutImfo.wOutChairID==(wMeChairID+2)%GAME_PLAYER || ForOutImfo.wOutChairID==wMeChairID)
				{
					//出牌是最后一个敌人，小王在对家手中；
					if (wOutChairID == (ForOutImfo.wOutChairID+3)%GAME_PLAYER)
					{
						m_cbKindCount[(wMeChairID+2)%GAME_PLAYER][0] = 1;
					}
				}					
			}
		}
		else //不出牌
		{
			WORD wTempOutChairID = ForOutImfo.wOutChairID;
			BYTE cbIndex = (cbOutLogicValue==15) ? 17 : 18;
			int iCount = (int)(m_iOutCardCount[cbIndex] - m_cbKindCount[wMeChairID][cbIndex-17]);
			if (iCount > 0)
			{
				//抗贡局大王已经确定，不需要计算
				if (cbOutLogicValue == 17)	return;

				// b）本家出主牌或者是小王：不要的对家没有大/小王； 
				if (wTempOutChairID==(wMeChairID+2)%GAME_PLAYER || wTempOutChairID==wMeChairID)
				{ 
					//敌人都不要，大小王在对家手中；
					if (wPassaChairID == (wTempOutChairID+3)%GAME_PLAYER)
					{
						m_cbKindCount[wPassaChairID][cbIndex] = 0;
						m_cbKindCount[(wMeChairID+2)%GAME_PLAYER][cbIndex] = (BYTE)iCount;

						//如果是出小王，大王也没出
						if (cbIndex==17 && m_iOutCardCount[18]-m_cbKindCount[wMeChairID][1]>0)
						{
							m_cbKindCount[wPassaChairID][1] = 0;
						}
					}
				}
				else if (wPassaChairID == (wMeChairID+2)%GAME_PLAYER)
				{
				    //敌人出的主牌或者是小王：不要的本家没有大/小王
					m_cbKindCount[wPassaChairID][cbIndex] = 0;
				}
			}
		}	
	}	
}

//记录出牌的牌型
void CAILogic::RecordOutCardType(WORD wOutChairID, BYTE cbTurnCardData[], BYTE cbTurnCardCount, bool bFirst, WORD wPassChairID, BYTE cbHandCardData[], BYTE cbHandCardCount, PlayerOutImfomation ForOutImfo)
{
	//ASSERT(bFirst && cbTurnCount != 0);
	BYTE cbTurnType = GetCardType(cbTurnCardData,cbTurnCardCount);
	BYTE cbForType = GetCardType(ForOutImfo.cbOutCardData,ForOutImfo.cbOutCardCount);

	//如果是炸弹不用进行计算
	if (cbForType==AI_CT_BOMB || cbForType==AI_CT_BOMB_TW || cbForType==AI_CT_TONG_HUA_SHUN)
	{
		return;
	}
	
	//首出牌
	if (bFirst)
	{
		//如果是炸弹不用进行计算
		if (cbTurnType==AI_CT_BOMB || cbTurnType==AI_CT_BOMB_TW || cbTurnType==AI_CT_TONG_HUA_SHUN)
		{
			return;
		}

		//出的牌型是本家喜欢的牌型
		if (!SearchType(cbTurnType,true,wOutChairID,false))
		{
			m_PlayerTypeImfo[wOutChairID].cbLikeCardType[m_PlayerTypeImfo[wOutChairID].cbLikeCardTypeCount] = cbTurnType;
			m_PlayerTypeImfo[wOutChairID].cbLikeCardTypeCount++;

			//移除不喜欢类型
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

		//是出牌者的敌人
		if (wForChairID != (wTempOutChairID+2)%GAME_PLAYER)
		{
			if (cbForType==AI_CT_SINGLE || cbForType==AI_CT_DOUBLE || cbForType==AI_CT_THREE || cbForType==AI_CT_THREE_DOUBLE)
			{
				//（不是同一类型中最大的或者是主牌以上）
				BYTE cbValue = GetTypeCardValue(cbForTurnData,cbForTurnCount);
				BYTE cbMaxValue = SearchMaxLogicValue(cbForType,cbHandCardData,cbHandCardCount);
			
				if (cbForType!=AI_CT_SINGLE && 15<cbMaxValue)
				{
					cbMaxValue = 15;
				}
				if (cbValue<cbMaxValue && cbMaxValue!=0)
				{
					//移除喜欢类型
					SearchType(cbForType,true,wTempOutChairID,true);	
					//添加到不喜欢类型
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
		//出牌(讲不喜欢的类型删除)
		//移除不喜欢类型
		SearchType(cbTurnType,false,wOutChairID,true);
	}
}

//查找牌型
bool CAILogic::SearchType(BYTE cbType, bool bLike, WORD wChairID, bool bMove)
{
    //避免无类型牌
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
					//移除数据
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
					//移除数据
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

//出牌记录的函数总汇
void CAILogic::SetOutCardImfomation(WORD wMeChairID, WORD wOutChairID, WORD wCurrentID, BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbTurnCardData[], BYTE cbTurnCardCount, bool bOutCard, bool bFirstOutCard)
{
	WORD wPassID = (!bOutCard) ? wOutChairID : INVALID_CHAIR;	
	//记录是否是首次出的牌	
	m_bFirstOutCard = bFirstOutCard;

	//记录牌
	if (bOutCard)
	{
		RecordOutCard(cbTurnCardData, cbTurnCardCount);
	}
	//王牌记录
	RecordKindCard(wMeChairID, cbHandCardData, cbHandCardCount, wPassID, wOutChairID, cbTurnCardData, cbTurnCardCount, m_ForOutImfo);
	//记录牌型
	RecordOutCardType(wOutChairID, cbTurnCardData, cbTurnCardCount, m_bFirstOutCard, wPassID, cbHandCardData, cbHandCardCount, m_ForOutImfo);
	//获取最优牌
	GetBestCardType(wOutChairID, cbTurnCardData, cbTurnCardCount, wPassID);	

	//放在最后
	if (bOutCard)
	{	
		//前一出牌玩家记录
		m_ForOutImfo.wOutChairID = wOutChairID;
		m_ForOutImfo.cbOutCardCount = cbTurnCardCount;
		CopyMemory(m_ForOutImfo.cbOutCardData,cbTurnCardData,cbTurnCardCount);
	}
	if (m_bFirstOutCard)
	{
		//当前晚间连续出牌次数
		if (m_CurPlayerOutTimes.wCurrentChairID == wOutChairID)
		{
			m_CurPlayerOutTimes.cbCurPlaOutCount++;
		}
		else
		{
			m_CurPlayerOutTimes.cbCurPlaOutCount = 1;
		}
		//敌人首次连续出牌的次数
		if (m_CurPlayerOutTimes.wCurrentChairID==wOutChairID || m_CurPlayerOutTimes.wCurrentChairID==(wOutChairID+2)%GAME_PLAYER)
		{
			m_CurPlayerOutTimes.cbEnemyFirstCount++;
		}
		else
		{
			m_CurPlayerOutTimes.cbEnemyFirstCount = 1;
		}
		m_CurPlayerOutTimes.wCurrentChairID = wOutChairID;

		//如果玩家已经出完牌数据清空
		if (m_cbCardNum[wOutChairID] == 0)
		{
			m_CurPlayerOutTimes.cbCurPlaOutCount = 0;
			m_CurPlayerOutTimes.cbEnemyOutCount = 0;
			m_CurPlayerOutTimes.wCurrentChairID = INVALID_CHAIR;
			m_CurPlayerOutTimes.cbEnemyFirstCount = 0;
		}
	}

	//敌人出牌的次数
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

//获取最优牌
void CAILogic::GetBestCardType(WORD wOutChairID, BYTE cbTurnCard[], BYTE cbTurnCount, WORD wPassID)
{
	//出牌计算剩余牌的张数
	if (wPassID == INVALID_CHAIR)
	{
		m_cbCardNum[wOutChairID] -= cbTurnCount;

		//是否拥有最优牌
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
		//最优牌修改
		BYTE cbType = GetCardType(cbTurnCard,cbTurnCount);
		if (cbType==AI_CT_SINGLE || cbType==AI_CT_DOUBLE || cbType==AI_CT_THREE || cbType==AI_CT_THREE_DOUBLE)
		{
			if (m_PlayerBestOutType.cbCardType[wPassID] == cbType)
			{
				BYTE cbCardData[5] = {0};
				CopyMemory(cbCardData, cbTurnCard, cbTurnCount);
				SortCardList(cbCardData, cbTurnCount, ST_ORDER);
				BYTE cbValue = GetTypeCardValue(cbTurnCard,cbTurnCount);
				//修改
				if (m_PlayerBestOutType.cbCardSafeValue[0]==0 || cbValue<m_PlayerBestOutType.cbCardSafeValue[0])
				{
					m_PlayerBestOutType.cbCardSafeValue[0] = cbValue;
				}
			}
		}
	}	
}

//制定类型中剩余的最大的逻辑值
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
		//我手中的牌的个数
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
			//剔除本人自己拥有的牌
			cbTemp -= cbCount[i];
			if (cbTemp >= cbCardCount)
			{
				return i;
			}
		}
	}

	return 0;
}

//获取牌型牌的逻辑值
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
		//以防配牌配出来的牌型
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

//获取牌型
BYTE CAILogic::GetMagicCardType(const BYTE cbCardData[], BYTE cbCardCount)
{
	BYTE cbMagicData[MAX_COUNT] = {0};
	MagicCardData(cbCardData, cbCardCount, cbMagicData);
	return GetCardType(cbMagicData, cbCardCount);
}

//变幻牌型比较
bool CAILogic::CompareMagicCard(BYTE cbFirstCard[], BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount)
{
	BYTE cbFirstMagic[MAX_COUNT] = {0};
	BYTE cbNextMagic[MAX_COUNT] = {0};
	MagicCardData(cbFirstCard, cbFirstCount, cbFirstMagic);
	MagicCardData(cbNextCard, cbNextCount, cbNextMagic);

	return CompareCard(cbFirstMagic, cbNextMagic, cbFirstCount, cbNextCount);
}

//接牌
void CAILogic::SearchNextOutCard(WORD wMeChairID, WORD wOutChairID, const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult &OutCardResult)
{
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));

	//定义玩家
	const WORD wUnderChairID = (wMeChairID+1)%GAME_PLAYER;
	const WORD wUpsideChairID = (wMeChairID+3)%GAME_PLAYER;
	const WORD wFriendChairID = (wMeChairID+2)%GAME_PLAYER;

	//记录出牌
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

	//敌人危险等级
	bool bUnderDanger = IsDangerous((wMeChairID+1)%GAME_PLAYER);
	bool bUpsideDanger = IsDangerous((wMeChairID+3)%GAME_PLAYER);

	//1）一首能出完出牌
	BYTE cbHandCardType = GetMagicCardType(cbHandData,cbHandCardCount);
	if (cbHandCardType != AI_CT_ERROR)
	{
		if (CompareMagicCard(cbTurnData,cbHandData,cbTurnCardCount,cbHandCardCount))
		{
			bool bOutCard = false;
			//不是炸弹接牌；
			if (cbHandCardType < AI_CT_TONG_HUA_SHUN)
			{
				bOutCard = true;
			}
			//是炸弹
			else
			{
				//是友方出的牌
				if (wOutChairID == wFriendChairID)
				{
					//下家危险，出炸，否则不出
					if (bUnderDanger)
					{
						bOutCard = true;
					}
				}
				//敌人出的牌，炸他丫的 
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
	
	//如果友方可接牌清手，并且友方不是用炸接的，不出
	if (m_cbHandCardCount[wUnderChairID]==0 && CompareMagicCard(cbTurnData,m_cbHandCardData[wFriendChairID],cbTurnCardCount,m_cbHandCardCount[wFriendChairID]))
	{
		BYTE cbFriendType = GetMagicCardType(m_cbHandCardData[wFriendChairID],m_cbHandCardCount[wFriendChairID]);
		if (cbFriendType<AI_CT_TONG_HUA_SHUN && cbFriendType!=AI_CT_ERROR)
		{
			return;
		}
	}

	//分析手上的牌
	tagOutCardTypeResult CardTypeResult[TYPE_COUNT];
	ZeroMemory(&CardTypeResult, sizeof(CardTypeResult));
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));
	AnalyseOutCardType(cbHandData, cbHandCardCount, CardTypeResult);

	BYTE cbMagicTurnData[10] = {0};
	MagicCardData(cbTurnData, cbTurnCardCount, cbMagicTurnData);

	//找牌型牌出
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

	//出炸弹
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

	//如果剩一手牌和炸弹
	if (wOutChairID!=wFriendChairID && OutCardResult.cbCardCount>0)
	{
		BYTE cbType = GetAfterRemoveBombsCardType(cbHandCardData, cbHandCardCount);
		//下家危险
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
		//上家危险
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

		//自己手牌可出
		if (cbType != AI_CT_ERROR)
		{
			if (rand()%10 < 5)
			{
				return;
			}
		}
	}


	//如果是单张和对子(拆牌)
	if (cbTurnType==AI_CT_SINGLE || cbTurnType==AI_CT_DOUBLE)
	{
		//是否剩余一手牌和一个炸弹（一首牌是三带二）
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
			//如果单张或者是对子出的是大于主牌以上的牌
			BYTE cbValue = GetTypeCardValue(cbTurnData, cbTurnCardCount);
			
			//单张有小王对子先出小王一个
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

				//如果敌方多次出此牌,一定概率压他
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
				//是对家不要的牌
				if (m_PlayerTypeImfo[(wMeChairID+2)%GAME_PLAYER].cbUnLikeCardTypeCount > 0)
				{
					for (BYTE i=0; i<m_PlayerTypeImfo[wFriendChairID].cbUnLikeCardTypeCount; i++)
					{
						if (m_PlayerTypeImfo[wFriendChairID].cbUnLikeCardType[i]==cbTurnType || m_cbCardNum[wFriendChairID]==0)
						{
							//扑克分析
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
	//正常接牌
	if (cbOutType!=AI_CT_BOMB && cbOutType!=AI_CT_TONG_HUA_SHUN && cbOutType!=AI_CT_BOMB_TW)
	{
		bool bUnAvailableType = false;
		if (cbOutType==m_PlayerBestOutType.cbCardType[wUnderChairID] || cbOutType==m_PlayerBestOutType.cbCardType[wUpsideChairID])
		{
			bUnAvailableType = true;
		}

		//危险系数是1： 
		BYTE bDanger = 0;
		WORD wDangerChairID = INVALID_CHAIR;
		if (bUnAvailableType)
		{
			for (BYTE i=1; i<GAME_PLAYER; i++)
			{
				WORD wChairID = (wMeChairID+i)%GAME_PLAYER;
				if (m_cbCardNum[wChairID]==0) continue;
				//到出牌玩家为止
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

		//2）可出的牌型
		if (!bUnAvailableType)
		{
			//1）出牌玩家是敌人
			if (wOutChairID==wUnderChairID || wOutChairID==wUpsideChairID)
			{
				//1）牌型牌：接牌；(连对，顺子，三连)
				if (cbOutType==AI_CT_DOUBLE_LINK || cbOutType==AI_CT_SINGLE_LINK || cbOutType==AI_CT_THREE_LINK)
				{
					CheckOutCardRightType(OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbOutType);
					VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData, cbTurnCardCount));
					return;
				}
				else
				{
					// 2）非牌型牌：
					//出牌玩家是上家
					if (wOutChairID == wUpsideChairID)
					{
						//要出最大牌 || 大小王 || 主牌
						BYTE cbMaxValue = SearchMaxLogicValue(cbOutType, cbHandData, cbHandCardCount);
						BYTE cbValue = GetTypeCardValue(OutCardResult.cbResultCard, OutCardResult.cbCardCount);
						if (cbMaxValue > 0)
						{
							bool bOut = false;
							if (cbValue>=cbMaxValue || cbValue>=15)
							{
								//a)出的牌剩余3个以内牌可出：出牌
								if (cbValue+3 >= cbMaxValue)
								{
									bOut = true;
								}
								//c) 对家不要的牌型：出牌
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
								//对家已经出玩牌
								if (m_cbCardNum[wFriendChairID] == 0)
								{
									bOut = true;
								}
								//统计结果
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
								//正常出牌
								VERIFY(GetMagicCardType(OutCardResult.cbResultCard, OutCardResult.cbCardCount) == cbOutType);
								VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData, cbTurnCardCount));
								return;
							}
						}
					}
					else//b) 出牌玩家不是上家（说明本家对家接不出牌）
					{
						//正常出牌
						VERIFY(GetMagicCardType(OutCardResult.cbResultCard, OutCardResult.cbCardCount) == cbOutType);
						VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData, cbTurnCardCount));
						return;
					}
				}
			}
			else
			{
				//出牌玩家是本家对家
				//1）牌型牌：100%不接牌；
				//1）牌型牌：不接牌；(连对，顺子，三连)
				if (cbOutType==AI_CT_DOUBLE_LINK || cbOutType==AI_CT_SINGLE_LINK || cbOutType==AI_CT_THREE_LINK)
				{
					ZeroMemory(&OutCardResult, sizeof(OutCardResult));
					VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData, cbTurnCardCount));
					return;
				}
				else
				{
					//a) 小于2个以内的牌，98%不出牌；
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

					//b) 敌人不要的牌型，98%不出牌；
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
						//正常出牌
						VERIFY(GetMagicCardType(OutCardResult.cbResultCard, OutCardResult.cbCardCount) == cbOutType);
						VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData, cbTurnCardCount));
						return;
					}
					else
					{
						//比较小的牌接牌
						if ((cbTurnType==AI_CT_SINGLE&&cbCardValue<=14) || cbCardValue<=10)
						{
							VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData, cbTurnCardCount));
							//正常出牌 
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
			//不可出牌型
			//1）危险系数是1
			if (bDanger == 1)
			{
				//1）单个：（单张个数n）
				if (cbTurnType==AI_CT_SINGLE || cbTurnType==AI_CT_DOUBLE || cbTurnType==AI_CT_THREE || cbTurnType==AI_CT_THREE_DOUBLE)
				{
					//1）一个炸弹和一张(或者是对子，三条和三带二)，不是最大，出炸弹；	
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
						//2）>= 2 出除最大单张下面的最大牌；
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
				//2）危险系数不是1
				//1）一个炸弹和一张(或者是对子，三条和三带二)，不是最大的，出炸弹；
				//2）出最大牌；
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
					// 2）出最大牌；
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
						//出除炸弹外最大的牌
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
	else //炸弹
	{
		
		//特殊牌处理（rate） 
		//出牌玩家没有牌了，
		//出的是牌型牌或者是非牌型牌中的最大牌，或者出的炸弹，或者出的是对家不要的牌型 
		//对家不要的牌型
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

		//炸弹的数量
		BYTE cbBombCount = 0;
		BYTE cbCardCount = m_cbCardNum[(wOutChairID+2)%GAME_PLAYER];
		for (BYTE i=AI_CT_TONG_HUA_SHUN; i<=AI_CT_BOMB_TW; i++)
		{
			cbBombCount += CardTypeResult[i].cbCardTypeCount;
		}
		BYTE cbHandOutCount =0;
		// AI_CT_THREE_DOUBLE 在对子和三条中
		for (BYTE i=AI_CT_SINGLE; i<AI_CT_THREE_DOUBLE; i++)
		{
			cbHandOutCount  += CardTypeResult[i].cbCardTypeCount;
		}
		//剪到一个重复的，加了一次对子和一次3条
		cbHandOutCount -= CardTypeResult[AI_CT_THREE_DOUBLE].cbCardTypeCount;

		//炸弹多与手上的出牌
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

		//出牌玩家手中的牌已经没有
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


		//概率计算
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
		
		//1）可出牌型(剩余的敌人的个数
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

		//危险系数是1： 
		BYTE cbDanger = 0;
		WORD wDangerChairID = INVALID_CHAIR;
		if (bUnAvailableType)
		{
			for (BYTE i=1; i<GAME_PLAYER; i++)
			{
				WORD wChairID = (wMeChairID+i)%GAME_PLAYER;
				if (m_cbCardNum[wChairID]==0) continue;
				//到出牌玩家为止
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
			//	出牌玩家是本家：不出牌
			if (wOutChairID == wFriendChairID)
			{
				ZeroMemory(&OutCardResult, sizeof(OutCardResult));
				VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData, cbTurnCardCount));
				return;
			}
			else
			{
				//如果手上只剩余一个炸弹，出炸弹,或者都是炸弹
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
				//出牌玩家手中的牌的个数小于10
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
					// 对家连续出牌的次数 > 2次（2回合） || 对家首次出牌次数 >= 3 (n)
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
				
				//出牌玩家是敌人，不是最大的牌或者是对家的不要的牌型，不是牌型牌不炸
				if (cbTurnType==AI_CT_SINGLE || cbTurnType==AI_CT_DOUBLE || cbTurnType==AI_CT_THREE || cbTurnType==AI_CT_THREE_DOUBLE)
				{
					BYTE cbTempData[MAX_COUNT] = {0};
					CopyMemory(cbTempData, cbHandCardData, cbHandCardCount);
			
					BYTE cbMaxValue = SearchMaxLogicValue(cbTurnType, cbTempData, cbHandCardCount);
					BYTE cbValue = GetTypeCardValue(cbTempTurnData, cbTurnCardCount);
					ASSERT(cbMaxValue>0);
					if (bLikeType && cbValue<cbMaxValue && m_cbCardNum[wFriendChairID]!=0)
					{
						//不出牌
						ZeroMemory(&OutCardResult, sizeof(OutCardResult));
						VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData, cbTurnCardCount));
						return;
					}
				}
				else 
				{
					if (cbBombCount > 2)  //牌型牌炸弹多的时候出
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
					//不出牌
					ZeroMemory(&OutCardResult, sizeof(OutCardResult));
					VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData, cbTurnCardCount));
					return;
				}
				//牌的个数 <= 4，并且首家出牌次数 >= 3 && num == 2; （留着火力对付剩余的敌人） (若只剩余炸弹~)   
				//Rate = 10;
				//出牌玩家牌的个数 > 10 (牺牲自己让对家先走，首出牌先出对家喜欢的牌)
				//Rate += n/2 * 10;   
			}
		}
		else
		{
			// 2）不可出牌型
			ASSERT(cbDanger!=0);
			if (cbDanger == 1)
			{
				//1）剩余炸弹或者是一首牌：
				BYTE cbTempCount = 0;
				for (BYTE i=AI_CT_SINGLE; i<=AI_CT_THREE_DOUBLE; i++)
				{
					cbTempCount += CardTypeResult[i].cbCardTypeCount;
				}
				if (cbTempCount==0 || cbTempCount==1)
				{
					//本家出牌出大与最小值的，不出；
					BYTE cbMinValue = m_PlayerBestOutType.cbCardSafeValue[wDangerChairID]==0 ? SearchMaxLogicValue(cbTurnType,cbHandData,cbHandCardCount):(m_PlayerBestOutType.cbCardSafeValue[wDangerChairID]+1);
					if (wOutChairID==wDangerChairID && GetTypeCardValue(cbTempTurnData,cbTurnCardCount)>=cbMinValue)
					{
						ZeroMemory(&OutCardResult, sizeof(OutCardResult));
					}	
				}
				//如果是单张，手上有至少2个以上单张
				if (cbTurnType==AI_CT_SINGLE && CardTypeResult[AI_CT_SINGLE].cbCardTypeCount>=2 && m_cbCardNum[wFriendChairID]!=0)
				{
					ZeroMemory(&OutCardResult, sizeof(OutCardResult));
				}
				//其余出：
				VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData, cbTurnCardCount));
				return;
			}//危险系数是2
			else if (cbDanger == 2)
			{
				//本家出牌，不出炸弹
				if (wOutChairID == wFriendChairID)
				{
					ZeroMemory(&OutCardResult, sizeof(OutCardResult));
				}
				else 
				{
					//1）剩余炸弹或者是一首牌：
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

						//对家还在（是对家不可出牌类型）
						if (m_PlayerTypeImfo[wFriendChairID].cbUnLikeCardTypeCount > 0)
						{
							for (BYTE i=0; i<m_PlayerTypeImfo[wFriendChairID].cbUnLikeCardTypeCount; i++)
							{
								if (m_PlayerTypeImfo[wFriendChairID].cbUnLikeCardType[i] == cbTurnType)
								{
									VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData, cbTurnCardCount));
									//是对家不要的牌型
									return;
								}
							}
						}

						BYTE cbMinValue = m_PlayerBestOutType.cbCardSafeValue[wDangerChairID]==0 ? SearchMaxLogicValue(cbTurnType,cbHandData,cbHandCardCount):(m_PlayerBestOutType.cbCardSafeValue[wDangerChairID]+1);
						//剩余一个对家，出牌对家是上家，剩余的牌中有2个以上（单张）该中牌型
						if (wChairIDXia == INVALID_CHAIR)
						{
							if (GetTypeCardValue(cbTempTurnData,cbTurnCardCount)<cbMinValue || m_cbCardNum[wFriendChairID]<cbTurnCardCount)
							{
								VERIFY(CheckWillOutCard(wOutChairID, wMeChairID, OutCardResult.cbResultCard, OutCardResult.cbCardCount, cbTurnData, cbTurnCardCount));
								return;
							}

							//如果是单张，只剩于大于等于他的最大的外一张单张，出炸弹
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

//查找扑克
bool CAILogic::SearchOutCardAI(WORD wMeChairID, WORD wOutChairID, const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult &OutCardResult)
{
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));

	bool bFirstOut = cbTurnCardCount==0 ? true:false;
	//首次出牌
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

//验证牌型
bool CAILogic::CheckOutCardRightType(BYTE cbCardData[], BYTE cbCardCount, BYTE cbType)
{
	//变幻牌型
	BYTE cbMagicData[MAX_COUNT] = {0};
	MagicCardData(cbCardData, cbCardCount, cbMagicData);
 	VERIFY(GetCardType(cbMagicData,cbCardCount)==cbType);
	return true;
}

//是否拆牌
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
	//判断可理由的牌型
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
	//没有可以拆的牌型/最大牌小与出的牌
	if (cbType==0 || cbTempValue<=cbOutValue)
	{
		//如果拥有天王炸单张多于3张以上
		if (tagResult[AI_CT_BOMB_TW].cbCardTypeCount > 0)
		{
			//小单张大于3
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
		//统计结果
		OutCardResult.cbCardCount = (cbCardType==AI_CT_SINGLE?1:2);
		CopyMemory(OutCardResult.cbResultCard, tagResult[cbType].cbEachHandCardData[tagResult[cbType].cbCardTypeCount-1], OutCardResult.cbCardCount);
		return true;
	}	
	return false;
}

//分析同花顺
bool CAILogic::AnalyseTongHuaShun(BYTE const cbHandCardData[], BYTE cbHandCardCount, tagOutCardTypeResult &CardTypeResult)
{
	ZeroMemory(&CardTypeResult, sizeof(CardTypeResult));
	
	//最多是5个同花顺	
	tagOutCardTypeResult OutCardTypeResultTemp[MAX_COUNT], AnalyseOutCardTypeResult[TYPE_COUNT];
	ZeroMemory(&OutCardTypeResultTemp, sizeof(OutCardTypeResultTemp));
	ZeroMemory(&AnalyseOutCardTypeResult, sizeof(AnalyseOutCardTypeResult));

	BYTE cbHandData[MAX_COUNT]={0}, cbHandCount=cbHandCardCount;
	BYTE cbRemoveData[MAX_COUNT]={0}, cbRemoveCount=cbHandCount;
	CopyMemory(cbHandData, cbHandCardData, cbHandCount);
	CopyMemory(cbRemoveData, cbHandCardData, cbHandCount);

	//炸弹数和单张个数
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

			//是否有相同的同花顺
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
		//存储数据		
		while(bTemp && cbRemoveCount > 0)
		{
			OutCardTypeResultTemp[cbCount].cbEachHandCardCount[OutCardTypeResultTemp[cbCount].cbCardTypeCount] = OutCardResult.cbCardCount;
			CopyMemory(OutCardTypeResultTemp[cbCount].cbEachHandCardData[OutCardTypeResultTemp[cbCount].cbCardTypeCount], OutCardResult.cbResultCard, OutCardResult.cbCardCount);
			OutCardTypeResultTemp[cbCount].cbCardTypeCount++;
			//移除已有的同花顺

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
		
		//统计炸弹数和单张个数
		//分析手上的牌	
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

	//统计结果
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
			//剩余的单张的个数<= 3 或者< 原来的个数；
			//炸弹个数 + 同花顺个数>=原来的；	
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

			//验证结果
			for (BYTE i=0; i<CardTypeResult.cbCardTypeCount; i++)
			{
				CheckOutCardRightType(CardTypeResult.cbEachHandCardData[i], CardTypeResult.cbEachHandCardCount[i], AI_CT_TONG_HUA_SHUN);
			}
			return true;
		}
	}

	return false;
}

//牌型搜索(除同花顺)
void CAILogic::AnalyseOutCardTypeNomal(BYTE const cbHandCardData[], BYTE const cbHandCardCount, tagOutCardTypeResult CardTypeResult[TYPE_COUNT/*+1*/])
{
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult[0])*TYPE_COUNT);

	BYTE cbCardDataTemp[MAX_COUNT] = {0};
	BYTE cbCardCountTemp = cbHandCardCount;
	//保留扑克，防止分析时改变扑克
	CopyMemory(cbCardDataTemp, cbHandCardData, cbHandCardCount);
	SortCardList(cbCardDataTemp, cbHandCardCount, ST_ORDER);

	//分析扑克
	BYTE cbMagicCard = MakeCardData(m_cbMainValue-1, 2);
	BYTE cbRemoveData[MAX_COUNT] = {0};
	BYTE cbRemoveCount = 0;
	tagAnalyseResult AnalyseResult;
	//分析扑克
	AnalysebCardData(cbCardDataTemp, cbCardCountTemp, AnalyseResult);

	//配牌
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

		//如果原本就是炸弹就不需要去拆火了
		if (cbMainCount==4 && iMagicCount==1)
		{
			iMagicCount = 0;
		}
		else if (cbMainCount-4>0 && iMagicCount>0)
		{
			iMagicCount = __min((cbMainCount-4), iMagicCount); 
		}
		//移除变化牌
		if (iMagicCount > 0)
		{	
			for (int i=0; i<iMagicCount; i++)
			{
				RemoveCard(&cbMagicCard, 1, cbCardDataTemp, cbCardCountTemp);
				cbCardCountTemp--;
			}
		}
		
		//补缺
		// 1）顺子/同花顺（3张以上的单张顺子）；
		{
			do 
			{
				tagOutCardResult SingleLinkOutSesult;
				int iFind = SerarchSingleLink(cbCardDataTemp, cbCardCountTemp, iMagicCount, SingleLinkOutSesult);
				if (iFind==0) break;

				//统计数据
				BYTE cbResultType = GetMagicCardType(SingleLinkOutSesult.cbResultCard, SingleLinkOutSesult.cbCardCount);
				ASSERT(cbResultType==AI_CT_SINGLE_LINK || cbResultType==AI_CT_TONG_HUA_SHUN);

				//统计结果			
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

				//移除数据
				RemoveCard(SingleLinkOutSesult.cbResultCard, SingleLinkOutSesult.cbCardCount, cbCardDataTemp, cbCardCountTemp);
				cbCardCountTemp -= SingleLinkOutSesult.cbCardCount;
			} while (true);	
		}

		//分析扑克(从大到小排列)
		AnalysebCardData(cbCardDataTemp, cbCardCountTemp, AnalyseResult);

		//有变化的牌：配火
		if (iMagicCount > 0)
		{		
			// 1）3条配成炸弹（2个3条以上选最小的3条）
			BYTE cbTempCount = AnalyseResult.cbBlockCount[2];
			while(iMagicCount>0 && cbTempCount>0)
			{
				CardTypeResult[AI_CT_BOMB].cbCardType = AI_CT_BOMB;
				CopyMemory(CardTypeResult[AI_CT_BOMB].cbEachHandCardData[CardTypeResult[AI_CT_BOMB].cbCardTypeCount],&AnalyseResult.cbCardData[2][(cbTempCount-1)*3],3);
				CardTypeResult[AI_CT_BOMB].cbEachHandCardData[CardTypeResult[AI_CT_BOMB].cbCardTypeCount][3] = cbMagicCard;
				CardTypeResult[AI_CT_BOMB].cbEachHandCardCount[CardTypeResult[AI_CT_BOMB].cbCardTypeCount] = 4;
				CardTypeResult[AI_CT_BOMB].cbCardTypeCount++;

				//验证
				CheckOutCardRightType(CardTypeResult[AI_CT_BOMB].cbEachHandCardData[CardTypeResult[AI_CT_BOMB].cbCardTypeCount-1], 4, AI_CT_BOMB);

				//移除的扑克
				CopyMemory(&cbRemoveData[cbRemoveCount], &AnalyseResult.cbCardData[2][(cbTempCount-1)*3], 3);
				cbRemoveCount += 3;
				cbTempCount --;
				iMagicCount --;
			}

			//或者剩余的牌
			if (cbRemoveCount > 0)
			{
				RemoveCard(cbRemoveData, cbRemoveCount, cbCardDataTemp, cbCardCountTemp);
				cbCardCountTemp -= cbRemoveCount;
			}
		}	

		//补缺
		//3连对缺一张的；（大于10以上的算为变化的牌中）(主花色的对子不算在里面)
		//(把原来有的3连对剔除掉)
		AnalysebCardData(cbCardDataTemp, cbCardCountTemp, AnalyseResult);
		if (AnalyseResult.cbBlockCount[1] >= 3)
		{
			BYTE cbLink = 0;
			BYTE cbResultData[MAX_COUNT] = {0};
			BYTE cbResultCount = 0;
			BYTE cbTempData[MAX_COUNT] = {0};
			BYTE cbTempCount = AnalyseResult.cbBlockCount[1]*2;
			CopyMemory(cbTempData, AnalyseResult.cbCardData[1], cbTempCount);
			//排列顺序
			SortCardList(cbTempData, cbTempCount, ST_VALUE);

			//如果主为2，223344 和334455 同时存在，选择组合334455
			bool bMainLink = false;
			BYTE cbMainData[6] = {0};
			BYTE cbMainCount = 0;
			for (int i=(AnalyseResult.cbBlockCount[1]-1); i>0; i--)
			{
				BYTE cbFirstValue = GetCardValue(cbTempData[i*2]);
				//(主花色的对子不算在里面)排除大小王的对子
				if (m_cbMainValue==2 && cbFirstValue==1) continue;
				if (cbTempData[i*2]==0x4E || cbTempData[i*2]==0x4F) break;

				CopyMemory(&cbResultData[0], &cbTempData[i*2], 2);
				cbResultCount = 2;
				cbLink = 1;

				for (int j=i-1; j>=0; j--)
				{
					//(主花色的对子不算在里面)
					if (cbTempData[j*2]==0x4E || cbTempData[j*2]==0x4F) break;
					if (cbFirstValue+cbLink != GetCardValue(cbTempData[j*2])) break;

					CopyMemory(&cbResultData[cbResultCount], &cbTempData[j*2], 2);
					cbResultCount += 2;
					cbLink ++;

					if (cbLink == 3) break;
				}

				//统计结果
				if (cbLink == 3)
				{
					//验证
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
							//移除数据
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
					//在10以下的连对固定其余的对子可以机动
					if (GetCardValue(cbResultData[cbResultCount-1])<10 && !bMainLink)
					{
						//移除数据
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

			//补缺连队
			if (iMagicCount > 0)
			{
				AnalysebCardData(cbCardDataTemp, cbCardCountTemp, AnalyseResult);
				if (AnalyseResult.cbBlockCount[1]>=2 && iMagicCount>0 && AnalyseResult.cbBlockCount[0]>=0)
				{
					BYTE cbLink = 0;
					BYTE cbResultData[MAX_COUNT] = {0};
					BYTE cbResultCount = 0;
					BYTE cbNeed = 0;
					BYTE cbCount1= 0;  //拷贝的是对子还是单张
					BYTE cbCount2= 0;  //拷贝的是对子还是单张

					////对子和单张
					cbTempCount = AnalyseResult.cbBlockCount[1]*2;
					CopyMemory(cbTempData, AnalyseResult.cbCardData[1], cbTempCount);
					CopyMemory(&cbTempData[cbTempCount], &AnalyseResult.cbCardData[0], AnalyseResult.cbBlockCount[0]);
					cbTempCount += AnalyseResult.cbBlockCount[0];
					SortCardList(cbTempData,cbTempCount,ST_VALUE);

					for (int i=cbTempCount-1; i>0; i--)
					{
						BYTE cbFirstValue = GetCardValue(cbTempData[i]);
						//(主花色的对子不算在里面)排除大小王的对子
						if ((m_cbMainValue==2&&cbFirstValue==1) || (cbTempData[i]==0x4E||cbTempData[i]==0x4F))
						{
							continue;
						}

						//避免相同的牌
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

							//避免相同的牌
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

							//不重复处理数据
							if (cbCount2 == 2) j--;
						}

						//统计结果
						if (cbLink == 3)
						{
							//最大对子小于10 的固定
							if (GetCardValue(cbResultData[cbResultCount-1]) < 10)
							{
								RemoveCard(cbResultData, cbResultCount, cbCardDataTemp, cbCardCountTemp);
								cbCardCountTemp -= cbResultCount;
								RemoveCard(cbResultData, cbResultCount, cbTempData, cbTempCount);
								cbTempCount -= cbResultCount;
								SortCardList(cbTempData, cbTempCount, ST_VALUE);
								i = cbTempCount;
							}

							//补缺
							if (cbNeed == 1)
							{
								cbResultData[cbResultCount] = cbMagicCard;
								cbResultCount++;
								iMagicCount--;
							}				

							//验证
							CheckOutCardRightType(cbResultData, cbResultCount, AI_CT_DOUBLE_LINK);

							CardTypeResult[AI_CT_DOUBLE_LINK].cbCardType = AI_CT_DOUBLE_LINK;
							CardTypeResult[AI_CT_DOUBLE_LINK].cbEachHandCardCount[CardTypeResult[AI_CT_DOUBLE_LINK].cbCardTypeCount] = 6;
							CopyMemory(CardTypeResult[AI_CT_DOUBLE_LINK].cbEachHandCardData[CardTypeResult[AI_CT_DOUBLE_LINK].cbCardTypeCount], cbResultData, cbResultCount);
							CardTypeResult[AI_CT_DOUBLE_LINK].cbCardTypeCount ++;

							ASSERT(iMagicCount>=0);
							if (iMagicCount==0 || cbTempCount<5) break;
						}

						//不重复处理数据
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

							//验证
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
		//配火牌没有用途
		if (iMagicCount > 0)
		{
			for (BYTE i=0; i<iMagicCount; i++)
			{
				cbCardDataTemp[cbCardCountTemp] = cbMagicCard;
				cbCardCountTemp ++;
			}
		}
	}

	//天王炸
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

	//除顺子，同花顺和连对外的牌型，(从大到小)
	SortCardList(cbCardDataTemp, cbCardCountTemp, ST_ORDER);
	AnalysebCardData(cbCardDataTemp, cbCardCountTemp, AnalyseResult);
	//3连
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

			//如果主为2，222333 和333444 同时存在，选择组合333444
			bool bMainLink = false;
			BYTE cbMainCount = 0;
			for (int i=AnalyseResult.cbBlockCount[2]-1; i>0; i--)
			{
				BYTE cbFirstValue = GetCardValue(cbTempData[i*3]);
				//aaa222不提倡出牌
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

				//存储数据
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
					//验证
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
				//验证
				CheckOutCardRightType(cbResultData, cbResultCount, AI_CT_THREE_LINK);

				CardTypeResult[AI_CT_THREE_LINK].cbCardType = AI_CT_THREE_LINK;
				CardTypeResult[AI_CT_THREE_LINK].cbEachHandCardCount[CardTypeResult[AI_CT_THREE_LINK].cbCardTypeCount] = 6;
				CopyMemory(CardTypeResult[AI_CT_THREE_LINK].cbEachHandCardData[CardTypeResult[AI_CT_THREE_LINK].cbCardTypeCount], cbResultData, cbResultCount);
				CardTypeResult[AI_CT_THREE_LINK].cbCardTypeCount++;
			}
		}
	}

	//3带2
	{
		//从小到大
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
	//三条
	{
		//从小到大
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
	//对子
	{
		//从小到大
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
	//单个
	{
		//从小到大
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

	//炸弹
	{
		//从小到大
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

	//验证结果
	for (BYTE cbType=AI_CT_SINGLE; cbType<=AI_CT_BOMB_TW; cbType++)
	{
		if (CardTypeResult[cbType].cbCardTypeCount > 0)
		{
			for (BYTE i=0; i<CardTypeResult[cbType].cbCardTypeCount; i++)
			{
				//验证
				CheckOutCardRightType(CardTypeResult[cbType].cbEachHandCardData[i], CardTypeResult[cbType].cbEachHandCardCount[i], cbType);
			}
		}
	}
}

//测试炸自己人的清理
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

//查找顺子
int CAILogic::SerarchSingleLink(BYTE cbCardData[], BYTE cbCardCount, int iMagicCount, tagOutCardResult &OutCardResult)
{	
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));

	int iFind = 0;
	BYTE cbMagicData = MakeCardData(m_cbMainValue-1, 2);
	SortCardList(cbCardData, cbCardCount, ST_VALUE);

	//补缺
	// 1）顺子/同花顺（4张以上的单张顺子）；
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
			//去除大小王
			if (cbTempData[i]==0x4E || cbTempData[i]==0x4F) break;

			BYTE cbFirstValue = GetCardValue(cbTempData[i]);
			cbLink = 1;
			cbNeedCount = 0;
			//统计数据
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

				//统计数据
				cbResultData[cbResultCount] = cbTempData[j];
				cbResultCount++;
				cbLink++;

				if (cbLink==5 || cbLink+cbNeed==5) 
				{
					break;
				}
			}
			SortCardList(cbResultData, cbResultCount, ST_VALUE);
			//中间缺少必须要添加的
			cbNecessaryCount = cbNeedCount;
			ASSERT(cbNecessaryCount == cbNeed);
			//10~A
			if (cbLink>=3 && cbLink<5 && cbFirstValue>=10 && GetCardValue(cbTempData[cbTempCount-1])==1)
			{
				//统计数据
				cbResultData[cbResultCount] = cbTempData[cbTempCount-1];
				cbResultCount++;
				cbLink++;
				SortCardList(cbResultData, cbResultCount, ST_ORDER);
				cbNeed = 5-cbLink;
			}

			//两边缺少连的情况
			if (cbLink>=3 && cbLink<5 && cbNeed+cbLink<5)
			{
				//先找大的
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
				//选择小的
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
			//先找对子或者是3条中的一个,5个以上的炸弹
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

				//分析扑克
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

				
				//可否是同花顺
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
				//统计数据
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

				//验证
				BYTE cbResultType = GetMagicCardType(cbResultData, cbResultCount);			
				if (cbResultType!=AI_CT_SINGLE_LINK && cbResultType!=AI_CT_TONG_HUA_SHUN) continue;
				if (iFind == 1 || iFind == 2)
				{
					iMagicCount -= iFind;
					ASSERT(iMagicCount >= 0);
				}

				ASSERT(cbResultType==AI_CT_SINGLE_LINK || cbResultType==AI_CT_TONG_HUA_SHUN);

				//统计结果		
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

//验证是否是自己手中的牌
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
	
	//分析手上的牌
	tagOutCardTypeResult CardTypeResult[TYPE_COUNT];
	ZeroMemory(&CardTypeResult, sizeof(CardTypeResult));
	AnalyseOutCardType(cbHandData, cbHandCardCount, CardTypeResult);

	//1）一首能出完；
	//变幻扑克
	if (GetMagicCardType(cbHandData,cbHandCardCount) != AI_CT_ERROR)
	{
		OutCardResult.cbCardCount = cbHandCardCount;
		CopyMemory(OutCardResult.cbResultCard, cbHandData, OutCardResult.cbCardCount);

		CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
		return;
	}
	//如果自己手上有天王炸和其他一手牌
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

	//自己手上有一炸一手牌，如果敌家手上只剩一个炸，比较自己的炸和敌人家的炸。如果我大，先出炸，否则交给下面代码处理
	{
		BYTE cbUnderCardType = GetMagicCardType(m_cbHandCardData[wUnderChairID], m_cbHandCardCount[wUnderChairID]);
		BYTE cbUpsideCardType = GetMagicCardType(m_cbHandCardData[wUpsideChairID], m_cbHandCardCount[wUpsideChairID]);
		
		//如果敌家有人手上只剩一个炸弹,看自己的牌型是不是一炸一手牌
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
			if (bIsConsiderOutBomb)   //所有条件都满足了，比较炸的大小
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

				if (IsOutBomb)  //自己的炸，比他们的大，先出炸
				{
					CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbBombCardType].cbEachHandCardData[0], CardTypeResult[cbBombCardType].cbEachHandCardCount[0]);
					OutCardResult.cbCardCount = CardTypeResult[cbBombCardType].cbEachHandCardCount[0];
					return;
				}
			}

		}
	}
	//下家就剩单张和同花顺
	if (m_cbHandCardCount[wUnderChairID] == 6)
	{
		tagOutCardTypeResult UnderOutCardTypeResult[TYPE_COUNT];
		AnalyseOutCardType(m_cbHandCardData[wUnderChairID], m_cbHandCardCount[wUnderChairID], UnderOutCardTypeResult);
		bool bUnderOneAndTongHuaShun = UnderOutCardTypeResult[AI_CT_TONG_HUA_SHUN].cbCardTypeCount==1&&UnderOutCardTypeResult[AI_CT_SINGLE].cbCardTypeCount==1;
		bool bUnderDuiziAndZhaDan = UnderOutCardTypeResult[AI_CT_BOMB].cbCardTypeCount==1&&UnderOutCardTypeResult[AI_CT_DOUBLE].cbCardTypeCount==1;
		//如果敌方只剩下单张和同花顺,不出单张
		if (bUnderOneAndTongHuaShun)
		{
			//出非单张
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
			//出单张
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
			//出非对子
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
			//出三张
			if (CardTypeResult[AI_CT_THREE].cbCardTypeCount > 0)
			{
				OutCardResult.cbCardCount = CardTypeResult[AI_CT_THREE].cbEachHandCardCount[0];
				CopyMemory(OutCardResult.cbResultCard, CardTypeResult[AI_CT_THREE].cbEachHandCardData[0], sizeof(BYTE)*OutCardResult.cbCardCount);
				CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
				return;
			}
		}
	}
	//友方就剩6张并且只剩单张和同花顺
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
	//上家就剩单张和同花顺
	if (m_cbHandCardCount[wUpsideChairID] == 6)
	{
		tagOutCardTypeResult UpsideOutCardTypeResult[TYPE_COUNT];
		AnalyseOutCardType(m_cbHandCardData[wUpsideChairID], m_cbHandCardCount[wUpsideChairID], UpsideOutCardTypeResult);
		bool bUpsideOneAndTongHuaShun = UpsideOutCardTypeResult[AI_CT_TONG_HUA_SHUN].cbCardTypeCount==1&&UpsideOutCardTypeResult[AI_CT_SINGLE].cbCardTypeCount==1;
		bool bUpsideDuiziAndZhaDan = UpsideOutCardTypeResult[AI_CT_BOMB].cbCardTypeCount==1&&UpsideOutCardTypeResult[AI_CT_DOUBLE].cbCardTypeCount==1;
		//如果敌方只剩下单张和同花顺,不出单张
		if (bUpsideOneAndTongHuaShun)
		{
			//除非单张
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
			//出单张
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
			//出非对子
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
			//出三张
			if (CardTypeResult[AI_CT_THREE].cbCardTypeCount > 0)
			{
				OutCardResult.cbCardCount = CardTypeResult[AI_CT_THREE].cbEachHandCardCount[0];
				CopyMemory(OutCardResult.cbResultCard, CardTypeResult[AI_CT_THREE].cbEachHandCardData[0], sizeof(BYTE)*OutCardResult.cbCardCount);
				CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
				return;
			}
		}
	}
	//敌方有人牌少于三张
	if (m_cbHandCardCount[wUnderChairID]<=3 || m_cbHandCardCount[wUpsideChairID])
	{
		//出非对子
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
	//一手牌一个炸弹（出一手牌）
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
				//如果牌型不相同
				if (cbCardCountTemp!=m_cbHandCardCount[(wMeChairID+1)%GAME_PLAYER] && cbCardCountTemp!=m_cbHandCardCount[(wMeChairID+3)%GAME_PLAYER]) 
				{	
					return;
				}
				//如果牌型相同,先出炸弹
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
	
	//2）可以放牌类型；（不是本家不要的牌型）
	if (m_PlayerBestOutType.cbCardType[wFriendChairID] != 0)
	{
		//1）是不可出的牌，但是不是下家
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
			//单个:1）疑似最大的大王或者是小王在本家手中，或者是下家已经没有牌，出最小单张；
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

						//统计结果
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

						//统计结果
						OutCardResult.cbCardCount = CardTypeResult[cbType].cbEachHandCardCount[i];
					    CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbType].cbEachHandCardData[i], OutCardResult.cbCardCount);			
						CheckOutCardInHand(cbHandCardData,cbHandCardCount,OutCardResult.cbResultCard,OutCardResult.cbCardCount);
						return;
					}
				}
			}
		}
	}

	//统计手中的牌型
	BYTE cbUnAvailableCount=0, cbUnAvailableType[TYPE_COUNT]={0};
	// 3）可出的牌型
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
		//1）牌型牌（顺子，和连对）
		BYTE cbRand = rand()%cbFirstCount;
		BYTE cbType = cbFirstType[cbRand];

		OutCardResult.cbCardCount = CardTypeResult[cbType].cbEachHandCardCount[0];
		CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbType].cbEachHandCardData[0], OutCardResult.cbCardCount);
		CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
		return;
	}
	else if (cbSecondCount > 0)
	{
		//2）若本家牌比较差，出对家需要，敌人不要的牌；或者是对家需要的牌；
		//3）单个：有大小王/最大牌（单个个数除去大小王和主牌外 > 1）
		//4）对子：有大小王或者是主牌对子 出对子（对子除大小王和主牌外 > 1）
		
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

				//统计结果
				if ((CardTypeResult[cbType].cbCardTypeCount-cbCount)>=1 && cbCount!=0)
				{
					OutCardResult.cbCardCount = CardTypeResult[cbType].cbEachHandCardCount[0];
					CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbType].cbEachHandCardData[0], OutCardResult.cbCardCount);
					CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
					return;
				}
			}	
		}	
		
		 //5）出对家喜欢的牌型  
		if (m_PlayerTypeImfo[wFriendChairID].cbLikeCardTypeCount>0 && m_cbCardNum[wFriendChairID]!=0)
		{
			for (BYTE i=0; i<m_PlayerTypeImfo[wFriendChairID].cbLikeCardTypeCount; i++)
			{
				BYTE cbFirstType = m_PlayerTypeImfo[wFriendChairID].cbLikeCardType[i];
				for(BYTE j=0; j<cbSecondCount; j++)
				{
					if (cbFirstType == cbSecondType[j])
					{
						//统计结果
						OutCardResult.cbCardCount = CardTypeResult[cbFirstType].cbEachHandCardCount[0];
						CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbFirstType].cbEachHandCardData[0], OutCardResult.cbCardCount);						
						CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
						return;
					}
				}
			}
		}

		//6）出三连
		for (BYTE i=0; i<cbSecondCount; i++)
		{
			if (cbSecondType[i] == AI_CT_THREE_LINK)
			{
				//统计结果
				OutCardResult.cbCardCount = CardTypeResult[AI_CT_THREE_LINK].cbEachHandCardCount[0];
				CopyMemory(OutCardResult.cbResultCard, CardTypeResult[AI_CT_THREE_LINK].cbEachHandCardData[0], OutCardResult.cbCardCount);
				CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
				return;
			}
		}

		//5）对子（没有大小王 && 对家有大小王对 && 本家有主牌 && 单张个数比较多）出对子
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

		//6）剩余牌型中的随机牌型（哪个个数比较多就先出哪一个，一样的话先出个数多的再出个数少的）
		//三带二：出对子或者是3条，出的个数大于相应的否则出三带二
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
		//统计结果
		OutCardResult.cbCardCount = CardTypeResult[cbType].cbEachHandCardCount[0];
		CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbType].cbEachHandCardData[0], OutCardResult.cbCardCount);
		CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);		
		return;

	}
	else if (cbUnAvailableCount > 0)
	{
		//不可出牌型
		//特殊处理
	    //一个炸弹和一首牌(单张)（对家手中的牌 < 4，并且有2个不可出牌型(有一个1)） 先出炸弹后出牌
		//单张
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

			//a) 危险玩家不是下家，出最小单张；
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
				//b) 否则出1）一个炸弹和一张，不是最大的单张，出炸弹；
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
						//否则出最大单张
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
					//2）>= 2 出除最大单张下面的最大牌；
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
			//2）对子：a) 出最大对子的单张；
			ASSERT(CardTypeResult[AI_CT_DOUBLE].cbCardTypeCount > 0);
			OutCardResult.cbCardCount = 1;
			OutCardResult.cbResultCard[0] = CardTypeResult[AI_CT_DOUBLE].cbEachHandCardData[CardTypeResult[AI_CT_DOUBLE].cbCardTypeCount-1][0];

			CheckOutCardInHand(cbHandCardData, cbHandCardCount, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
			ASSERT(OutCardResult.cbCardCount != 0);
			return;
		}
		else if (cbUnAvailableType[0] == AI_CT_THREE)
		{
			//3）三条：b）出最大对子；
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
		//炸弹
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

//获取贡牌
BYTE CAILogic::GetTributeCard(WORD wMeChariID, BYTE cbHandCardData[], BYTE cbHandCardCount)
{
	//状态判断
	ASSERT(cbHandCardCount == 27);
	BYTE cbHandCardDataTemp[MAX_COUNT] = {0};
	ZeroMemory(cbHandCardDataTemp, sizeof(BYTE)*MAX_COUNT);
	CopyMemory(cbHandCardDataTemp, cbHandCardData, cbHandCardCount);
	//变量定义
	BYTE cbThrowCard = 0;
	//寻找扑克
	if (cbThrowCard == 0)
	{
		//变量定义
		BYTE cbCardLogicValue = 0;

		//寻找扑克
		for (BYTE i=0; i<cbHandCardCount; i++)
		{
			//扑克过虑
			if (cbHandCardData[i]==(m_cbMainValue|0x20)) continue;

			//扑克判断
			if (GetCardLogicValue(cbHandCardData[i]) > cbCardLogicValue)
			{
				cbThrowCard = cbHandCardData[i];
				cbCardLogicValue = GetCardLogicValue(cbHandCardData[i]);
			}
		}
	}

	//添加进贡设置	非王牌—进贡左右各5个逻辑差之间计算四种花色的个数，（除红桃外）花色最多的进贡；
	if (cbThrowCard!=0x4E && cbThrowCard!=0x4F)
	{
		BYTE cbThrowValue = GetCardValue(cbThrowCard);
		BYTE cbTempCount=0, cbTempData[8]={0};
		//寻找扑克
		for (BYTE i=0; i<cbHandCardCount; i++)
		{
			//扑克过虑
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

//获取回贡牌
BYTE CAILogic::GetBackTributeCard(WORD wMeChariID, BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wThrowUserNum, bool bThrowToFriend)
{
	ASSERT(cbHandCardCount == 28);
	BYTE cbHandCardDataTemp[MAX_COUNT] = {0};
	ZeroMemory(cbHandCardDataTemp, sizeof(BYTE)*MAX_COUNT);
	CopyMemory(cbHandCardDataTemp, cbHandCardData, cbHandCardCount);
	//变量定义
	BYTE cbIncepCard = 0;
	//寻找扑克
	if (cbIncepCard == 0)
	{
		//变量定义
		BYTE cbCardLogicValue = 0xFF;
		//寻找扑克
		for (BYTE i=0; i<cbHandCardCount; i++)
		{
			if (GetCardLogicValue(cbHandCardData[i]) < cbCardLogicValue)
			{
				cbIncepCard = cbHandCardData[i];
				cbCardLogicValue = GetCardLogicValue(cbHandCardData[i]);
			}
		}
	}

	//添加回贡
	tagOutCardTypeResult CardTypeResult[TYPE_COUNT];
	AnalyseOutCardType(cbHandCardData, cbHandCardCount, CardTypeResult);
	//(整理后)单张中最小的牌（小与10）；
	//进贡对象是敌人

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
	//进贡对象是本家对家
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

