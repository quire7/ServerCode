#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//静态变量

//牌数据
const BYTE CGameLogic::m_cbCardDataArray[MAX_REPERTORY]=
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,					//万子
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,					//万子
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,					//万子
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,					//万子
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,					//索子
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,					//索子
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,					//索子
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,					//索子
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,					//同子
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,					//同子
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,					//同子
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,					//同子
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,								//番子
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,								//番子
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,								//番子
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,								//番子
	0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48							//花牌(春、夏、秋、冬，梅、兰、竹、菊)
};

//////////////////////////////////////////////////////////////////////////
//构造函数
CGameLogic::CGameLogic()
{
	ResetAllData();
}

//析构函数
CGameLogic::~CGameLogic()
{
}

//重置数据
void CGameLogic::ResetAllData()
{

}

//混乱牌
void CGameLogic::RandCardData( BYTE cbCardData[], BYTE cbMaxCount )
{
	//初始化种子
	srand(GetTickCount()|(rand()<<8));

	//混乱准备
	BYTE cbCardDataTemp[MAX_REPERTORY] = {0};
	CopyMemory(cbCardDataTemp, m_cbCardDataArray, sizeof(BYTE)*MAX_REPERTORY);

	//混乱牌
	BYTE cbRandCount=0, cbPosition=0;
	do
	{
		cbPosition = rand()%(cbMaxCount-cbRandCount);
		cbCardData[cbRandCount++] = cbCardDataTemp[cbPosition];
		cbCardDataTemp[cbPosition] = cbCardDataTemp[cbMaxCount-cbRandCount];
	} while (cbRandCount<cbMaxCount);
}

//删除牌
bool CGameLogic::RemoveCard( BYTE cbCardIndex[MAX_INDEX], BYTE cbRemoveCard )
{
	//效验牌
	VERIFY(IsValidCard(cbRemoveCard));
	ASSERT(cbCardIndex[SwitchToCardIndex(cbRemoveCard)]>0);

	//删除牌
	BYTE cbRemoveIndex = SwitchToCardIndex(cbRemoveCard);
	ASSERT(cbRemoveIndex < MAX_INDEX);
	if (cbCardIndex[cbRemoveIndex] > 0)
	{
		cbCardIndex[cbRemoveIndex]--;
		return true;
	}

	//失败效验
	ASSERT(FALSE);
	return false;
}

//删除牌
bool CGameLogic::RemoveCard( BYTE cbCardIndex[MAX_INDEX], BYTE cbRemoveCard[], BYTE cbRemoveCount )
{
	//删除牌
	for (BYTE i=0; i<cbRemoveCount; i++)
	{
		//效验牌
		VERIFY(IsValidCard(cbRemoveCard[i]));
		BYTE cbRemoveIndex = SwitchToCardIndex(cbRemoveCard[i]);
		//校验索引
		ASSERT(cbRemoveIndex < MAX_INDEX);
		//校验数目
		ASSERT(cbCardIndex[cbRemoveIndex]>0);
		//删除牌
		if (cbCardIndex[cbRemoveIndex] == 0)
		{
			//错误断言
			ASSERT(FALSE);

			//还原删除
			for (BYTE j=0; j<i; j++) 
			{
				VERIFY(IsValidCard(cbRemoveCard[j]));
				cbCardIndex[SwitchToCardIndex(cbRemoveCard[j])]++;
			}

			return false;
		}
		else 
		{
			//删除牌
			--cbCardIndex[cbRemoveIndex];
		}
	}

	return true;
}

//删除牌
bool CGameLogic::RemoveCard( BYTE cbCardData[], BYTE cbCardCount, BYTE cbRemoveCard[], BYTE cbRemoveCount )
{
	//检验数据
	ASSERT(cbCardCount<=14);
	ASSERT(cbRemoveCount<=cbCardCount);

	//定义变量
	BYTE cbDeleteCount=0, cbTempCardData[14]={0};
	if (cbCardCount > CountArray(cbTempCardData))
	{
		return false;
	}
	CopyMemory(cbTempCardData, cbCardData, cbCardCount*sizeof(cbCardData[0]));

	//置零牌
	for (BYTE i=0; i<cbRemoveCount; i++)
	{
		for (BYTE j=0; j<cbCardCount; j++)
		{
			if (cbRemoveCard[i] == cbTempCardData[j])
			{
				cbDeleteCount++;
				cbTempCardData[j] = 0;
				break;
			}
		}
	}

	//成功判断
	if (cbDeleteCount != cbRemoveCount) 
	{
		ASSERT(FALSE);
		return false;
	}

	ZeroMemory(cbCardData, cbCardCount*sizeof(cbCardData[0]));
	//清理牌
	BYTE cbCardPos = 0;
	for (BYTE i=0; i<cbCardCount; i++)
	{
		if (cbTempCardData[i] != 0) 
		{
			cbCardData[cbCardPos++] = cbTempCardData[i];
		}
	}

	return true;
}

//排序牌
void CGameLogic::SortCardList( BYTE cbCardData[], BYTE cbCardCount )const
{
	if (cbCardCount<2) return;
	//排序操作
	bool bSorted = true;
	BYTE cbTempData=0, cbLast=cbCardCount-1;
	do
	{
		bSorted = true;
		for (BYTE i=0; i<cbLast; i++)
		{
			if (cbCardData[i] > cbCardData[i+1])
			{
				//交换位置
				cbTempData = cbCardData[i];
				cbCardData[i] = cbCardData[i+1];
				cbCardData[i+1] = cbTempData;
				bSorted = false;
			}	
		}
		cbLast--;
	} while(!bSorted);
}

//提取硬花牌
BYTE CGameLogic::DistillHardHuaCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbHardHuaCardData[MAX_HUA_COUNT] )
{
	BYTE cbHardHuaCardCount = 0;
	for (BYTE i=31; i<MAX_INDEX; i++)
	{
		if (cbCardIndex[i]==0) continue;
		for (BYTE j=0; j<cbCardIndex[i]; j++)
		{
			cbHardHuaCardData[cbHardHuaCardCount++] = SwitchToCardData(i);
		}
	}
	ASSERT(cbHardHuaCardCount <= MAX_HUA_COUNT);
	return cbHardHuaCardCount;
}

//有效判断
bool CGameLogic::IsValidCard( BYTE cbCardData )
{
	BYTE cbCardValue = GetCardValue(cbCardData);
	BYTE cbCardColor = GetCardColor(cbCardData);
	return (((cbCardValue>=1)&&(cbCardValue<=9)&&(cbCardColor<=2)) || \
			((cbCardValue>=1)&&(cbCardValue<=7)&&(cbCardColor==3)) || \
			((cbCardValue>=1)&&(cbCardValue<=8)&&(cbCardColor==4)));
}

//牌数目
BYTE CGameLogic::GetCardCount( const BYTE cbCardIndex[MAX_INDEX] )
{
	//数目统计
	BYTE cbCardCount = 0;
	for (BYTE i=0; i<MAX_INDEX; i++) 
	{
		ASSERT(cbCardIndex[i]<=5);
		if (cbCardIndex[i]>0 && cbCardIndex[i]<=5)
		{
			cbCardCount += cbCardIndex[i];
		}
	}

	return cbCardCount;
}

//获取组合
BYTE CGameLogic::GetWeaveCard( DWORD dwWeaveKind, BYTE cbCenterCard, BYTE cbCardBuffer[MAX_WEAVE] )
{
	//组合牌
	switch (dwWeaveKind)
	{
	case WIK_OXX:		//左吃操作
		{
			//设置变量
			cbCardBuffer[0] = cbCenterCard;
			cbCardBuffer[1] = cbCenterCard+1;
			cbCardBuffer[2] = cbCenterCard+2;
			return 3;
		}
	case WIK_XXO:		//右吃操作
		{
			//设置变量
			cbCardBuffer[0] = cbCenterCard;
			cbCardBuffer[1] = cbCenterCard-1;
			cbCardBuffer[2] = cbCenterCard-2;
			return 3;
		}
	case WIK_XOX:	//中吃操作
		{
			//设置变量
			cbCardBuffer[0] = cbCenterCard;
			cbCardBuffer[1] = cbCenterCard-1;
			cbCardBuffer[2] = cbCenterCard+1;
			return 3;
		}
	case WIK_PENG:		//碰牌操作
		{
			//设置变量
			cbCardBuffer[0] = cbCenterCard;
			cbCardBuffer[1] = cbCenterCard;
			cbCardBuffer[2] = cbCenterCard;
			return 3;
		}
	case WIK_MGANG:
	case WIK_BGANG:
		{
			cbCardBuffer[0] = cbCenterCard;
			cbCardBuffer[1] = cbCenterCard;
			cbCardBuffer[2] = cbCenterCard;
			cbCardBuffer[3] = cbCenterCard;
			return 4;
		}
	case WIK_AGANG:		//杠牌操作
		{
			//设置变量
			cbCardBuffer[0] = 0;
			cbCardBuffer[1] = 0;
			cbCardBuffer[2] = 0;
			cbCardBuffer[3] = cbCenterCard;
			return 4;
		}
	default:
		{
			ASSERT(FALSE);
		}
	}

	return 0;
}

//计算胡牌番数
DWORD CGameLogic::CalculateFanShu( DWORD dwHuPaiKind )
{
	DWORD dwHuPaiFanShu = 0;

	//平胡
	if ((dwHuPaiKind&HU_PINGHU) != 0)
	{
		dwHuPaiFanShu += 0;
	}
	//门清
	if ((dwHuPaiKind&HU_MENQING) != 0)
	{
		dwHuPaiFanShu += 20;
	}
	//混一色
	if ((dwHuPaiKind&HU_HUNYISE) != 0)
	{
		dwHuPaiFanShu += 40;
	}
	//对对胡
	if ((dwHuPaiKind&HU_DUIDUIHU) != 0)
	{
		dwHuPaiFanShu += 40;
	}
	//清一色
	if ((dwHuPaiKind&HU_QINGYISE) != 0)
	{
		dwHuPaiFanShu += 60;
	}
	//全球独钓
	if ((dwHuPaiKind&HU_QQDUDIAO) != 0)
	{
		dwHuPaiFanShu += 60;
	}
	//七对
	if ((dwHuPaiKind&HU_QIDUI) != 0)
	{
		dwHuPaiFanShu += 80;
	}
	//双七对
	if ((dwHuPaiKind&HU_SQIDUI) != 0)
	{
		dwHuPaiFanShu += 160;
	}
	//无花果
	if ((dwHuPaiKind&HU_WUHUAGUO) != 0)
	{
		dwHuPaiFanShu += 40;
	}
	//压绝
	if ((dwHuPaiKind&HU_YAJUE) != 0)
	{
		dwHuPaiFanShu += 40;
	}
	//小杠开花
	if ((dwHuPaiKind&HU_XGKAIHUA) != 0)
	{
		dwHuPaiFanShu += 20;
	}
	//大杠开花
	if ((dwHuPaiKind&HU_DGKAIHUA) != 0)
	{
		dwHuPaiFanShu += 40;
	}
	//天胡
	if ((dwHuPaiKind&HU_TIANHU) != 0)
	{
		dwHuPaiFanShu += 400;
	}
	//地胡
	if ((dwHuPaiKind&HU_DIHU) != 0)
	{
		dwHuPaiFanShu += 80;
	}

	return dwHuPaiFanShu;
}

//检测牌是否在数组中
bool CGameLogic::IsCardInArrary( const BYTE cbCardArrary[], const BYTE cbArraryCount, const BYTE cbCardData )
{
	ASSERT(cbArraryCount>0);
	if (cbArraryCount==0) return false;

	for (BYTE i=0; i<cbArraryCount; i++)
	{
		if (cbCardArrary[i] == cbCardData)
		{
			return true;
		}
	}
	return false;
}

//动作等级
BYTE CGameLogic::GetUserActionRank( DWORD dwUserAction )
{
	//胡牌等级
	if (dwUserAction&WIK_HU) 
	{
		return 5; 
	}
	//挑战等级
	if (dwUserAction&WIK_TIAOZHAN)
	{
		return 4;
	}
	//杠牌等级
	if (dwUserAction&WIK_GANG)
	{
		return 3;
	}
	//碰牌等级
	if (dwUserAction&WIK_PENG) 
	{
		return 2;
	}
	//吃牌等级
	if (dwUserAction&WIK_CHI) 
	{
		return 1;
	}
	//

	return 0;
}

//胡牌等级
WORD CGameLogic::GetHuPaiActionRank( tagHuPaiResult & HuPaiResult )
{
	return 0;
}

//吃牌分析
DWORD CGameLogic::AnalyseChiPai( const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard )
{
	//参数效验
	VERIFY(IsValidCard(cbCurrentCard));

	//过滤判断,番子无连
	if (cbCurrentCard >= 0x31) 
	{
		return WIK_NULL;
	}

	//变量定义
	BYTE cbExcursion[3] = {0, 1, 2};
	DWORD dwItemKind[3] = {WIK_OXX, WIK_XOX, WIK_XXO};

	//吃牌判断
	DWORD dwEatKind = WIK_NULL; BYTE cbFirstIndex = 0;
	BYTE cbCurrentIndex = SwitchToCardIndex(cbCurrentCard);
	for (BYTE i=0; i<CountArray(dwItemKind); i++)
	{
		BYTE cbValueIndex = cbCurrentIndex%9;
		if ((cbValueIndex>=cbExcursion[i]) && ((cbValueIndex-cbExcursion[i])<=6))
		{
			//吃牌判断
			cbFirstIndex = cbCurrentIndex-cbExcursion[i];
			if ((cbCurrentIndex!=cbFirstIndex) && (cbCardIndex[cbFirstIndex]==0))
			{
				continue;
			}
			if ((cbCurrentIndex!=(cbFirstIndex+1)) && (cbCardIndex[cbFirstIndex+1]==0))
			{
				continue;
			}
			if ((cbCurrentIndex!=(cbFirstIndex+2)) && (cbCardIndex[cbFirstIndex+2]==0))
			{
				continue;
			}

			//设置类型
			dwEatKind |= dwItemKind[i];
		}
	}

	return dwEatKind;
}

//碰牌分析
DWORD CGameLogic::AnalysePengPai( const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard )
{
	//参数效验
	VERIFY(IsValidCard(cbCurrentCard));

	//碰牌判断
	return (cbCardIndex[SwitchToCardIndex(cbCurrentCard)]>=2)?WIK_PENG:WIK_NULL;
}

//杠牌分析
DWORD CGameLogic::AnalyseGangPai( const BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, tagGangPaiResult &GangPaiResult, bool bZiMo )
{
	//设置变量
	DWORD dwActionMask = WIK_NULL;
	ZeroMemory(&GangPaiResult, sizeof(GangPaiResult));

	//打出的牌(明杠)
	if (!bZiMo && IsValidCard(cbCurrentCard))
	{
		BYTE cbCurrentIndex = SwitchToCardIndex(cbCurrentCard);
		if(cbCardIndex[cbCurrentIndex] == 3)
		{
			dwActionMask |= WIK_MGANG;
			GangPaiResult.dwGangType[GangPaiResult.cbCardCount] = WIK_MGANG;
			GangPaiResult.cbCardData[GangPaiResult.cbCardCount++] = cbCurrentCard;
		}
	}
	else
	{
		//手上杠牌(暗杠)
		for (BYTE i=0; i<MAX_NORMAL; i++)
		{
			if (cbCardIndex[i] == 4)
			{
				dwActionMask |= WIK_AGANG;
				GangPaiResult.dwGangType[GangPaiResult.cbCardCount] = WIK_AGANG;
				GangPaiResult.cbCardData[GangPaiResult.cbCardCount++] = SwitchToCardData(i);
			}
		}

		//组合杠牌(补杠)
		for (BYTE i=0; i<cbWeaveCount; i++)
		{
			if (WeaveItem[i].dwWeaveKind == WIK_PENG)
			{
				BYTE cbCenterCardIndex = SwitchToCardIndex(WeaveItem[i].cbCenterCard);
				ASSERT(cbCenterCardIndex>=0 && cbCenterCardIndex<MAX_NORMAL);
				if (cbCardIndex[cbCenterCardIndex] == 1)
				{
					dwActionMask |= WIK_BGANG;
					GangPaiResult.dwGangType[GangPaiResult.cbCardCount] = WIK_BGANG;
					GangPaiResult.cbCardData[GangPaiResult.cbCardCount++] = WeaveItem[i].cbCenterCard;
				}
			}
		}
	}

	return dwActionMask;
}

//听牌分析
DWORD CGameLogic::AnalyseTingPai( const BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount, BYTE cbHuaCount, DWORD dwPreHuPaiKind, tagTingPaiResult &TingPaiResult, bool bCheckTianTing/*=false*/ )
{
	//重置变量
	ZeroMemory(&TingPaiResult, sizeof(TingPaiResult));

	//变量定义
	DWORD dwTingPaiKind = WIK_NULL;
	BYTE cbCardIndexTemp[MAX_INDEX] = {0};
	CopyMemory(cbCardIndexTemp, cbCardIndex, sizeof(BYTE)*MAX_INDEX);
	//检测天听
	if (bCheckTianTing)
	{
 		tagTingPaiItem TingPaiItem;
 		ZeroMemory(&TingPaiItem, sizeof(TingPaiItem));
 		TingPaiItem.cbOutCardData = 0;
 
 		//听牌判断
 		for (BYTE i=0; i<MAX_NORMAL; i++)
 		{
 			//胡牌分析
 			BYTE cbCurrentCard = SwitchToCardData(i); 

 			//变量定义
 			tagHuPaiResult HuPaiResult;
 			DWORD dwHuPaiKind = AnalyseHuPai(cbCardIndexTemp, WeaveItem, cbItemCount, cbCurrentCard, cbHuaCount, dwPreHuPaiKind, HuPaiResult, false);
 
 			//结果判断
  			if (dwHuPaiKind == WIK_HU) 
  			{
				TingPaiItem.dwHuCardFanShu[TingPaiItem.cbTingCardCount] = HuPaiResult.dwHuPaiFanShu;
  				TingPaiItem.cbTingCardData[TingPaiItem.cbTingCardCount++] = cbCurrentCard;
  				dwTingPaiKind = WIK_TING;
  			}
 		}
 		//加入听牌结果
  		if (TingPaiItem.cbTingCardCount > 0)
  		{
  			TingPaiResult.TingPaiItemArrary[TingPaiResult.cbTingPaiItemCount++] = TingPaiItem;
  		}
	}
	else
	{
		BYTE cbCardCount = GetCardCount(cbCardIndex);
		ASSERT(cbCardCount%3==2 && cbCardCount>0 && cbCardCount<=MAX_COUNT);
		//听牌分析
		for (BYTE i=0; i<MAX_NORMAL; i++)
		{
			//空牌过滤
			if (cbCardIndexTemp[i] == 0)
			{
				continue;
			}

			//听牌处理
			cbCardIndexTemp[i]--;
			tagTingPaiItem TingPaiItem;
			ZeroMemory(&TingPaiItem, sizeof(TingPaiItem));
			TingPaiItem.cbOutCardData = SwitchToCardData(i);

			//听牌判断
			for (BYTE j=0; j<MAX_NORMAL; j++)
			{
				if (cbCardIndexTemp[j]==4) continue;
				//变量定义
				tagHuPaiResult HuPaiResult;
				ZeroMemory(&HuPaiResult, sizeof(HuPaiResult));

				//胡牌分析
				BYTE cbCurrentCard = SwitchToCardData(j);
				DWORD dwHuPaiKind = AnalyseHuPai(cbCardIndexTemp, WeaveItem, cbItemCount, cbCurrentCard, cbHuaCount, dwPreHuPaiKind, HuPaiResult, false);

				//结果判断
				if (dwHuPaiKind == WIK_HU) 
				{
					TingPaiItem.dwHuCardFanShu[TingPaiItem.cbTingCardCount] = HuPaiResult.dwHuPaiFanShu;
					TingPaiItem.cbTingCardData[TingPaiItem.cbTingCardCount++] = cbCurrentCard;
					dwTingPaiKind = WIK_TING;
				}
			}

			if (TingPaiItem.cbTingCardCount > 0)
			{
				TingPaiResult.TingPaiItemArrary[TingPaiResult.cbTingPaiItemCount++] = TingPaiItem;
			}

			//还原处理
			cbCardIndexTemp[i]++;
		}
	}

	return dwTingPaiKind;
}

//胡牌分析
DWORD CGameLogic::AnalyseHuPai( const BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, BYTE cbHuaCount, DWORD dwPreHuPaiKind, \
							   tagHuPaiResult &HuPaiResult, bool bZiMo, bool bContinue/*=true*/)
{	
	ZeroMemory(&HuPaiResult, sizeof(HuPaiResult));

	//变量定义
	DWORD dwHuPaiKind=0, dwHuPaiFanShu=0;;
	CAnalyseItemArray AnalyseItemArray;
	AnalyseItemArray.RemoveAll();

	//构造牌
	BYTE cbCardIndexTemp[MAX_INDEX] = {0};
	CopyMemory(cbCardIndexTemp, cbCardIndex, sizeof(BYTE)*MAX_INDEX);

	//插入牌
	if (!bZiMo && IsValidCard(cbCurrentCard))
	{
		BYTE cbCurrentCardIndex = SwitchToCardIndex(cbCurrentCard);
		ASSERT(cbCurrentCardIndex>=0 && cbCurrentCardIndex<MAX_NORMAL);
		cbCardIndexTemp[cbCurrentCardIndex]++;
	}

	BYTE cbCardCount = GetCardCount(cbCardIndexTemp);
	ASSERT(cbCardCount%3==2 && cbCardCount>=2 && cbCardCount<=MAX_COUNT);
	BYTE cbQiDuiType = IsQiDui(cbCardIndexTemp, WeaveItem, cbWeaveCount, cbCurrentCard);

	//七对牌型
	if (cbQiDuiType > 0)
	{
		switch (cbQiDuiType)
		{
		case 1:
		{
			dwHuPaiKind |= HU_QIDUI;
		}break;
		case 2:
		case 3:
		case 4:
		{
			dwHuPaiKind |= HU_SQIDUI;
		}break;
		}
		//混一色
		if (IsHunYiSe(cbCardIndexTemp, WeaveItem, cbWeaveCount))
		{
			dwHuPaiKind |= HU_HUNYISE;
		}
		//清一色
		if (IsQingYiSe(cbCardIndexTemp, WeaveItem, cbWeaveCount))
		{
			dwHuPaiKind |= HU_QINGYISE;
		}

		dwHuPaiKind |= dwPreHuPaiKind;
	}
	else
	{
		//分析牌
		AnalyseCard(cbCardIndexTemp, WeaveItem, cbWeaveCount, AnalyseItemArray);

		//胡牌分析
		if (AnalyseItemArray.GetCount() > 0)
		{
			if (!bContinue)
			{
				return WIK_HU;
			}

			//找出最大牌型
			DWORD dwHuPaiKindTemp=0, dwHuPaiFanShuTemp=0;
			
			//牌型分析
			for (INT_PTR i=0; i<AnalyseItemArray.GetCount(); i++)
			{
				dwHuPaiKindTemp |= dwPreHuPaiKind;
				//变量定义
				tagAnalyseItem *pAnalyseItem = &AnalyseItemArray[i];
				//门清
				if (IsMenQing(cbCardIndexTemp, WeaveItem, cbWeaveCount))
				{
					dwHuPaiKindTemp |= HU_MENQING;
				}
 				//全球独钓
 				if (IsQuanQiuDuDiao(cbCardIndexTemp, pAnalyseItem, WeaveItem, cbWeaveCount, cbCurrentCard, cbHuaCount, dwPreHuPaiKind))
 				{
 					dwHuPaiKindTemp |= HU_QQDUDIAO;
 				}	
 				//对对胡
 				if (IsDuiDuiHu(pAnalyseItem))
 				{
 					dwHuPaiKindTemp |= HU_DUIDUIHU;
 				}
				//混一色
				if (IsHunYiSe(cbCardIndexTemp, WeaveItem, cbWeaveCount))
				{
					dwHuPaiKindTemp |= HU_HUNYISE;
				}
				//清一色
				if (IsQingYiSe(cbCardIndexTemp, WeaveItem, cbWeaveCount))
				{
					dwHuPaiKindTemp |= HU_QINGYISE;
				}
				//只胡一张
				if (IsOnlyHuOneCard(cbCardIndexTemp, WeaveItem, cbWeaveCount, cbCurrentCard, cbHuaCount, dwPreHuPaiKind))
				{
					dwHuPaiKindTemp |= HU_ONLYONE;
				}
				else
				{
					//否则去掉压绝
					dwHuPaiKindTemp &= ~HU_YAJUE;
				}

				//不是大胡和门清
				if (!IsBigHuPaiKind(dwHuPaiKindTemp) && (dwHuPaiKindTemp&HU_MENQING)==0)
				{
					//硬花大于四个可以胡平胡
					if (cbHuaCount >= 4)
					{
						dwHuPaiKindTemp |= HU_PINGHU;
					}
					else
					{
						dwHuPaiKindTemp = HU_NULL;
					}
				}

				//判断牌型大小
				if (dwHuPaiKindTemp != HU_NULL)
				{
					dwHuPaiFanShuTemp = CalculateFanShu(dwHuPaiKindTemp);
					if ((dwHuPaiFanShuTemp > dwHuPaiFanShu) || (dwHuPaiKindTemp&HU_PINGHU)!= 0)
					{
						dwHuPaiKind = dwHuPaiKindTemp;
						dwHuPaiFanShu = dwHuPaiFanShuTemp;
					}
				}
			}
		}
	}

	//结果判断
	if (dwHuPaiKind != HU_NULL)
	{
		////去除平胡
		//if (IsBigHuPaiKind(dwHuPaiKind) || (dwHuPaiKind&HU_MENQING)!=0)
		//{
		//	dwHuPaiKind &= ~HU_PINGHU;
		//}

		DWORD dwNoPinghu = HU_MENQING | HU_WUHUAGUO | HU_YAJUE | HU_DGKAIHUA | HU_XGKAIHUA | HU_HUNYISE |
			HU_QINGYISE | HU_DUIDUIHU | HU_QQDUDIAO | HU_QIDUI | HU_SQIDUI | HU_TIANHU | HU_DIHU;

		//去除平胡
		if ((dwHuPaiKind&dwNoPinghu) != 0)
		{
			dwHuPaiKind &= ~HU_PINGHU;
		}

		dwPreHuPaiKind = dwHuPaiKind;
		HuPaiResult.dwHuPaiKind = dwHuPaiKind;
		HuPaiResult.dwHuPaiFanShu = CalculateFanShu(dwHuPaiKind);
		return WIK_HU;
	}

	return WIK_NULL;
}

//分析牌
bool CGameLogic::AnalyseCard( const BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount, CAnalyseItemArray &AnalyseItemArray )
{
	//计算数目
	BYTE cbCardCount = GetCardCount(cbCardIndex);

	//效验数目
	ASSERT((cbCardCount>=2)&&(cbCardCount<=MAX_COUNT)&&((cbCardCount-2)%3==0));
	if ((cbCardCount<2)||(cbCardCount>MAX_COUNT)||((cbCardCount-2)%3!=0)) return false;

	//变量定义
	BYTE cbKindItemCount = 0;
	tagKindItem KindItem[MAX_COUNT-2];
	ZeroMemory(KindItem, sizeof(KindItem));

	//需求判断
	BYTE cbLessKindItem = (cbCardCount-2)/3;
	ASSERT((cbLessKindItem+cbWeaveCount)==4);

	//单钓判断
	if (cbLessKindItem == 0)
	{
		//效验参数
		ASSERT((cbCardCount==2)&&(cbWeaveCount==4));

		//牌眼判断
		for (BYTE i=0; i<MAX_NORMAL; i++)
		{
			if (cbCardIndex[i] == 2)
			{
				//变量定义
				tagAnalyseItem AnalyseItem;
				ZeroMemory(&AnalyseItem, sizeof(AnalyseItem));

				//设置结果
				for (BYTE j=0; j<cbWeaveCount; j++)
				{
					AnalyseItem.dwWeaveKind[j] = WeaveItem[j].dwWeaveKind;
					AnalyseItem.cbCenterCard[j] = WeaveItem[j].cbCenterCard;
				}
				AnalyseItem.cbCardEye = SwitchToCardData(i);

				//插入结果
				AnalyseItemArray.Add(AnalyseItem);
				return true;
			}
		}
		return false;
	}

	//拆分分析
	if (cbCardCount >= 3)
	{
		for (BYTE i=0; i<MAX_NORMAL; i++)
		{
			//同牌判断
			if (cbCardIndex[i] >= 3)
			{
				KindItem[cbKindItemCount].cbCardIndex[0] = i;
				KindItem[cbKindItemCount].cbCardIndex[1] = i;
				KindItem[cbKindItemCount].cbCardIndex[2] = i;
				KindItem[cbKindItemCount].dwWeaveKind = WIK_PENG;
				KindItem[cbKindItemCount++].cbCenterCard = SwitchToCardData(i);
			}

			//连牌判断
			if ((i<(MAX_NORMAL-6)) && (cbCardIndex[i]>0) && ((i%9)<7))
			{
				for (BYTE j=1; j<=cbCardIndex[i]; j++)
				{
					if ((cbCardIndex[i+1]>=j) && (cbCardIndex[i+2]>=j))
					{
						KindItem[cbKindItemCount].cbCardIndex[0] = i;
						KindItem[cbKindItemCount].cbCardIndex[1] = i+1;
						KindItem[cbKindItemCount].cbCardIndex[2] = i+2;
						KindItem[cbKindItemCount].dwWeaveKind = WIK_OXX;
						KindItem[cbKindItemCount++].cbCenterCard = SwitchToCardData(i);
					}
				}
			}
		}
	}

	//组合分析
	if (cbKindItemCount >= cbLessKindItem)
	{
		//变量定义
		BYTE cbCardIndexTemp[MAX_INDEX] = {0};
		BYTE cbIndex[4] = {0, 1, 2, 3};
		tagKindItem *pKindItem[4];
		ZeroMemory(&pKindItem, sizeof(pKindItem));

		//开始组合
		do
		{
			//设置变量
			CopyMemory(cbCardIndexTemp, cbCardIndex, sizeof(BYTE)*MAX_INDEX);
			for (BYTE i=0; i<cbLessKindItem; i++)
			{
				pKindItem[i] = &KindItem[cbIndex[i]];
			}

			//数量判断
			bool bEnoughCard  = true;
			for (BYTE i=0; i<cbLessKindItem*3; i++)
			{
				//存在判断
				BYTE cbCardIndex = pKindItem[i/3]->cbCardIndex[i%3]; 
				if (cbCardIndexTemp[cbCardIndex] == 0)
				{
					bEnoughCard = false;
					break;
				}
				else 
				{
					cbCardIndexTemp[cbCardIndex]--;
				}
			}

			//胡牌判断
			if (bEnoughCard)
			{
				//牌眼判断
				BYTE cbCardEye = 0;
				for (BYTE i=0; i<MAX_NORMAL; i++)
				{
					if (cbCardIndexTemp[i] == 2)
					{
						cbCardEye = SwitchToCardData(i);
						break;
					}
				}

				//组合类型
				if (cbCardEye != 0)
				{
					//变量定义
					tagAnalyseItem AnalyseItem;
					ZeroMemory(&AnalyseItem, sizeof(AnalyseItem));

					//设置组合
					for (BYTE i=0; i<cbWeaveCount; i++)
					{
						AnalyseItem.dwWeaveKind[i] = WeaveItem[i].dwWeaveKind;
						AnalyseItem.cbCenterCard[i] = WeaveItem[i].cbCenterCard;
					}

					//设置牌型
					for (BYTE i=0; i<cbLessKindItem; i++) 
					{
						AnalyseItem.dwWeaveKind[i+cbWeaveCount] = pKindItem[i]->dwWeaveKind;
						AnalyseItem.cbCenterCard[i+cbWeaveCount] = pKindItem[i]->cbCenterCard;
					}

					//设置牌眼
					AnalyseItem.cbCardEye = cbCardEye;

					//插入结果
					AnalyseItemArray.Add(AnalyseItem);
				}
			}

			//设置索引
			if (cbIndex[cbLessKindItem-1] == (cbKindItemCount-1))
			{
				BYTE i = cbLessKindItem - 1;
				for (; i>0; i--)
				{
					if ((cbIndex[i-1]+1) != cbIndex[i])
					{
						BYTE cbNewIndex = cbIndex[i-1];
						for (BYTE j=(i-1); j<cbLessKindItem; j++) 
						{
							cbIndex[j] = cbNewIndex+j-i+2;
						}
						break;
					}
				}
				if (i==0)
				{
					break;
				}
			}
			else
			{
				cbIndex[cbLessKindItem-1]++;
			}

		} while (true);
	}

	return (AnalyseItemArray.GetCount()>0);
}

//门清
bool CGameLogic::IsMenQing( const BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount )
{
	for (BYTE i=0; i<cbWeaveCount; i++)
	{
		tagWeaveItem *pWeaveItem = &WeaveItem[i];
		if ((pWeaveItem->dwWeaveKind&WIK_CHI)!=0 || (pWeaveItem->dwWeaveKind&WIK_PENG)!=0)
		{
			return false;			
		}
	}
	return true;
}

//混一色牌
bool CGameLogic::IsHunYiSe( const BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount )
{
	bool bCardColor[4] = {false, false, false, false};
	//手牌
	for (BYTE i=0; i<MAX_NORMAL; i++)
	{
		if (cbCardIndex[i] > 0)
		{
			BYTE cbCardData = SwitchToCardData(i);
			bCardColor[GetCardColor(cbCardData)] = true;
		}
	}
	//组合牌
	for (BYTE i=0; i<cbWeaveCount; i++)
	{
		BYTE cbCardData = WeaveItem[i].cbCenterCard;
		bCardColor[GetCardColor(cbCardData)] = true;
	}
	//统计
	BYTE cbColorCount = 0;
	for (BYTE i=0; i<3; i++)
	{
		if (bCardColor[i])
		{
			cbColorCount++;
		}
	}

	return (cbColorCount==1&&bCardColor[3])?true:false;
}

//清一色牌
bool CGameLogic::IsQingYiSe( const BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount )
{
	bool bCardColor[3] = {false, false, false};
	//手牌
	for (BYTE i=0; i<MAX_NORMAL; i++)
	{
		if (cbCardIndex[i] > 0)
		{
			BYTE cbCardData = SwitchToCardData(i);
			if (IsZiCard(cbCardData))
			{
				return false;
			}
			bCardColor[GetCardColor(cbCardData)] = true;
		}
	}
	//组合牌
	for (BYTE i=0; i<cbWeaveCount; i++)
	{
		BYTE cbCardData = WeaveItem[i].cbCenterCard;
		if (IsZiCard(cbCardData))
		{
			return false;
		}
		else
		{
			bCardColor[GetCardColor(cbCardData)] = true;
		}
	}
	//统计
	BYTE cbColorCount = 0;
	for (BYTE i=0; i<3; i++)
	{
		if (bCardColor[i])
		{
			cbColorCount++;
		}
	}

	return (cbColorCount==1)?true:false;
}

//对对胡
bool CGameLogic::IsDuiDuiHu( tagAnalyseItem *pAnalyseItem )
{
	//牌型分析
	for (BYTE i=0; i<CountArray(pAnalyseItem->dwWeaveKind); i++)
	{
		DWORD dwWeaveKind = pAnalyseItem->dwWeaveKind[i];
		if ((dwWeaveKind&WIK_CHI) != 0) 
		{
			return false;
		}
	}
	return true;
}

//全球独钓
bool CGameLogic::IsQuanQiuDuDiao( const BYTE cbCardIndex[MAX_INDEX], tagAnalyseItem *pAnalyseItem, tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, BYTE cbHuaCount, DWORD dwPreHuPaiKind )
{
	// 组合牌数量小于
	if (cbWeaveCount < MAX_WEAVE)
	{
		return false;
	}

	//牌型分析
	for (BYTE i=0; i<CountArray(pAnalyseItem->dwWeaveKind); i++)
	{
		DWORD dwWeaveKind = pAnalyseItem->dwWeaveKind[i];
		if ((dwWeaveKind&WIK_CHI)!=0 || (dwWeaveKind&WIK_AGANG)!=0) 
		{
			return false;
		}
	}
	return IsDanDiaoJiang(cbCardIndex, pAnalyseItem, WeaveItem, cbWeaveCount, cbCurrentCard, cbHuaCount, dwPreHuPaiKind);
}

//七对
BYTE CGameLogic::IsQiDui(const BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard)
{
	//4:双七对嘉年华 3:豪华双七对 2:双七对 1:七对 0:不是七对
	if (cbWeaveCount > 0 || GetCardCount(cbCardIndex) != MAX_COUNT)
	{
		return 0;
	}

	// 当前胡牌(双七对：自己有三张同样的牌，听七对时单听绝张)
	BYTE cbCurrentCardIndex = SwitchToCardIndex(cbCurrentCard);
	BYTE cbFourCardCount = 0, cbTwoCardCount = 0;

	for (BYTE i = 0; i < MAX_NORMAL; i++)
	{
		// 空牌
		if (cbCardIndex[i] == 0)
		{
			continue;
		}
		else if (cbCardIndex[i] % 2 == 1)
		{//单张或三张
			return 0;
		}
		else if (cbCardIndex[i] == 2)
		{//对子
			cbTwoCardCount++;
		}
		else if (cbCardIndex[i] == 4)
		{
			//四张(必须是听得牌)
			if (i == cbCurrentCardIndex)
			{
				cbFourCardCount++;
			}
			else
			{
				// 不是杠头算七对
				cbTwoCardCount += 2;
			}
		}
	}

	if (cbFourCardCount * 4 + cbTwoCardCount * 2 != MAX_COUNT)
	{
		return 0;
	}
	else
	{
		return cbFourCardCount + 1;
	}

	return 0;
}

//单钓将牌
bool CGameLogic::IsDanDiaoJiang( const BYTE cbCardIndex[MAX_INDEX], tagAnalyseItem *pAnalyseItem, tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, BYTE cbHuaCount, DWORD dwPreHuPaiKind )
{
	ASSERT(pAnalyseItem!=NULL);
	//眼牌
	BYTE cbEyeCard = pAnalyseItem->cbCardEye;
	//是否只胡一张
	bool bHuOneCard = IsOnlyHuOneCard(cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard, cbHuaCount, dwPreHuPaiKind);
	if( !bHuOneCard) 
	{
		return false;
	}
	//胡的牌就是眼牌
	return (pAnalyseItem->cbCardEye==cbCurrentCard)?true:false;
}

//只胡一张牌
bool CGameLogic::IsOnlyHuOneCard( const BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount, BYTE cbCurrentCard, BYTE cbHuaCount, DWORD dwPreHuPaiKind )
{
	//构造牌
	BYTE cbCardIndexTemp[MAX_INDEX] = {0};
	CopyMemory(cbCardIndexTemp, cbCardIndex, sizeof(BYTE)*MAX_INDEX);

	BYTE cbCurrentCardIndex = SwitchToCardIndex(cbCurrentCard);
	cbCardIndexTemp[cbCurrentCardIndex]--;

	for (BYTE i=0; i<MAX_NORMAL; i++)
	{
		//胡牌分析
		BYTE cbReplaceCard = SwitchToCardData(i);
		if( cbCurrentCard == cbReplaceCard ) continue;  //排除当前胡的牌

		tagHuPaiResult HuPaiResult;
		DWORD dwHuPaiKind = AnalyseHuPai(cbCardIndexTemp, WeaveItem, cbItemCount, cbReplaceCard, cbHuaCount, dwPreHuPaiKind, HuPaiResult, false, false);
		
		if (dwHuPaiKind == WIK_HU)
		{
			return false;
		}
	}
	return true;
}

//是否是大胡
bool CGameLogic::IsBigHuPaiKind( DWORD dwHuPaiKind )
{
	DWORD dwBigHuPaiKind = (HU_HUNYISE | HU_QINGYISE | HU_DUIDUIHU | HU_QQDUDIAO | HU_QIDUI | HU_SQIDUI | HU_TIANHU | HU_DIHU);
	return ((dwHuPaiKind&dwBigHuPaiKind)!=0)?true:false;
}

//是否是七对胡牌型
bool CGameLogic::IsHuPaiQiDuiKind( DWORD dwHuPaiKind )
{
	DWORD dwHuPaiQiDuiKind = (HU_QIDUI|HU_SQIDUI);
	return ((dwHuPaiKind&dwHuPaiQiDuiKind)!=0)?true:false;
}

//牌转换
BYTE CGameLogic::SwitchToCardData( BYTE cbCardIndex )
{
	ASSERT(cbCardIndex<MAX_INDEX);

	//正常牌
	if (cbCardIndex<MAX_NORMAL+3)
	{
		return ((cbCardIndex/9)<<4)|(cbCardIndex%9+1);
	}
	//花牌
	else if (cbCardIndex<MAX_INDEX)
	{
		return ((4<<4)|(cbCardIndex-MAX_NORMAL-3+1));
	}
	else 
	{
		ASSERT(false);
		return 0;
	}
}

//牌转换
BYTE CGameLogic::SwitchToCardIndex( BYTE cbCardData )
{
	VERIFY(IsValidCard(cbCardData));
	BYTE cbCardColor = GetCardColor(cbCardData);
	BYTE cbCardValue = GetCardValue(cbCardData);
	BYTE cbIndex = MAX_INDEX;
	if (cbCardColor < 4)
	{
		cbIndex = cbCardColor*9+cbCardValue-1;	
		//return cbCardColor*9+cbCardValue-1;	
	}
	else
	{
		cbIndex = MAX_NORMAL+3+cbCardValue-1;
		//return MAX_NORMAL+3+cbCardValue-1;
	}

	ASSERT(cbIndex < MAX_INDEX);
	return cbIndex;
}

//牌转换
BYTE CGameLogic::SwitchToCardData( BYTE cbCardIndex[MAX_INDEX], BYTE cbCardData[MAX_COUNT] )
{
	//转换牌
	BYTE cbPosition = 0;
	for (BYTE i=0; i<MAX_INDEX; i++)
	{
		ASSERT(cbCardIndex[i]>=0&&cbCardIndex[i]<=4);
		if (cbCardIndex[i]>0 && cbCardIndex[i]<=4)
		{
			for (BYTE j=0; j<cbCardIndex[i]; j++)
			{
				ASSERT(cbPosition<MAX_COUNT);
				cbCardData[cbPosition++] = SwitchToCardData(i);
			}
		}
	}

	return cbPosition;
}

//牌转换
BYTE CGameLogic::SwitchToCardIndex( BYTE cbCardData[], BYTE cbCardCount, BYTE cbCardIndex[MAX_INDEX] )
{
	//设置变量
	ZeroMemory(cbCardIndex,sizeof(BYTE)*MAX_INDEX);

	//转换牌
	for (BYTE i=0; i<cbCardCount; i++)
	{
		VERIFY(IsValidCard(cbCardData[i]));
		cbCardIndex[SwitchToCardIndex(cbCardData[i])]++;
	}

	return cbCardCount;
}
//////////////////////////////////////////////////////////////////////////