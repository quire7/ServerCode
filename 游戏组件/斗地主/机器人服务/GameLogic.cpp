#include "StdAfx.h"
#include "io.h"
#include <fstream>
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//静态变量
//扑克数据
const BYTE	CGameLogic::m_cbCardData[FULL_COUNT]=
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//方块 A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//梅花 A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//红桃 A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	//黑桃 A - K
	0x4E,0x4F,
};
//好牌数据
const BYTE	CGameLogic::m_cbGoodcardData[GOOD_CARD_COUNT]=
{
	0x01,0x02,0x11,0x12,0x21,0x22,0x31,0x32,0x4E,0x4F,
	0x07,0x08,0x09,0x17,0x18,0x19,0x27,0x28,0x29,0x37,0x38,0x39,
	0x0A,0x0B,0x0C,0x0D,0x1A,0x1B,0x1C,0x1D,0x2A,0x2B,0x2C,0x2D,0x3A,0x3B,0x3C,0x3D
};
//牌库数据
std::vector<LandCardList>         CGameLogic::m_vecLandCard;

//////////////////////////////////////////////////////////////////////////
//构造函数
CGameLogic::CGameLogic()
{
}

//析构函数
CGameLogic::~CGameLogic()
{
}

//加载牌库数据
void CGameLogic::LoadLandCard()
{	
	m_vecLandCard.clear();
	int ListCount = 0;
	BYTE cardbuffer[51] = {0};	
	if (m_vecLandCard.empty())
	{
		std::ostringstream ostr;
		ostr.str("");
		ostr << "randcard/1" << ".card";

		std::ifstream ifs(ostr.str().c_str());		
		if (!ifs.is_open())
		{
			//AfxMessageBox(TEXT("Small_Path  文件未找到"),MB_OK);
			return;
		}
		if (ifs.bad())
		{
			//AfxMessageBox(TEXT("Small_Path is  bad!  "),MB_OK);
			return;
		}		
		LandCardList LcardList;	
		BYTE Allcard[FULL_COUNT] = {0};
		int nX1[17] = {0};			
		CString strBuf;
		char szBuff[128] = {0};
		while( !ifs.eof() )
		{
			ZeroMemory(szBuff,sizeof(szBuff));
			ifs.getline(szBuff,sizeof(szBuff));
			if (strcmp(szBuff, "(end)")==0)
			{
				int count1 = (int)LcardList.vecLandListCard.size();
				memset(cardbuffer,0,sizeof(cardbuffer));
				for (int j=0;j<count1; j++)
				{
					cardbuffer[j] = LcardList.vecLandListCard[j];		
				}
				CopyMemory( Allcard, m_cbCardData, sizeof( m_cbCardData )  );
				if(!RemoveAllCard(cardbuffer,51,Allcard,54))
				{			
					strBuf.Format("第%d组牌不合格",ListCount);
					OutputDebugString(strBuf);
				}
				else
				{
					m_vecLandCard.push_back(LcardList);		
				}	
				ListCount++;
				LcardList.vecLandListCard.clear();
				memset(cardbuffer,0,sizeof(cardbuffer));				
			}
			else
			{
				ZeroMemory(nX1,sizeof(nX1));
				std::sscanf(szBuff, "%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X;",
					&nX1[0],&nX1[1],&nX1[2],&nX1[3],&nX1[4],&nX1[5],&nX1[6],&nX1[7],&nX1[8],&nX1[9],&nX1[10],&nX1[11],&nX1[12]
					,&nX1[13],&nX1[14],&nX1[15],&nX1[16]);	
					for(int i=0; i<17; i++)
					{
						LcardList.vecLandListCard.push_back((BYTE)nX1[i]);
					}	
			}		
		}
		strBuf.Format("合格组数;%d",m_vecLandCard.size());
		OutputDebugString(strBuf);
	}
}

//删除所有扑克
bool CGameLogic::RemoveAllCard( const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount )
{
	//检验数据
	ASSERT(cbRemoveCount<=cbCardCount);
	if (cbRemoveCount > cbCardCount) return false;

	//定义变量
	BYTE cbDeleteCount=0, cbDeleteCard[FULL_COUNT]={0};
	if (cbCardCount>CountArray(cbDeleteCard)) return false;
	CopyMemory(cbDeleteCard, cbCardData, cbCardCount*sizeof(cbCardData[0]));
	CString strErrorTip;
	//置零扑克
	for (BYTE i=0; i<cbRemoveCount; i++)
	{
		bool bIsTheCard = false;
		for (BYTE j=0; j<cbCardCount; j++)
		{
			if (cbRemoveCard[i] == cbDeleteCard[j])
			{
				bIsTheCard = true;
				cbDeleteCount++;
				cbDeleteCard[j] = 0;
				break;
			}
		}
		if (!bIsTheCard)
		{
			strErrorTip.Format("%d:%x无法删除", i, cbRemoveCard[i]);
			OutputDebugString(strErrorTip);
		}
	}
	VERIFY_RETURN_FALSE(cbDeleteCount==cbRemoveCount);

	//清理扑克
	ZeroMemory(cbCardData, cbCardCount);
	for (BYTE i=0,cbCardPos=0; i<cbCardCount; i++)
	{
		if (cbDeleteCard[i] != 0)
		{
			cbCardData[cbCardPos++] = cbDeleteCard[i];
		}
	}
	return true;
}

//获取底牌类型
BYTE CGameLogic::GetBackCardType( const BYTE cbBackCardData[], bool bLongTaskCheck/*=false*/ )
{
	//不是长线任务检查
	if (!bLongTaskCheck) 
	{
		return BCT_NORMAL;
	}

	//数据拷贝
	BYTE cbCardData[BACK_COUNT] = {0};
	CopyMemory(cbCardData, cbBackCardData, sizeof(BYTE)*BACK_COUNT);
	SortCardList(cbCardData, BACK_COUNT, ST_ORDER);

	//提取牌数据
	BYTE cbCardValue[BACK_COUNT]={0}, cbCardColor[BACK_COUNT]={0};
	for (int i=0; i<BACK_COUNT; ++i)
	{
		cbCardValue[i] = GetCardLogicValue(cbCardData[i]);
		cbCardColor[i] = GetCardColor(cbCardData[i]);
	}

	//双王
	if (cbCardData[0]==0x4F && cbCardData[1]==0x4E)
	{
		return BCT_DOUBLE_JOKER;
	}
	//三个二
	if (cbCardValue[0]==15 && cbCardValue[1]==15 &&	cbCardValue[2]==15)
	{
		return BCT_THREE_2;
	}
	//单王双2
	if (cbCardColor[0]==0x40 && cbCardValue[1]==15 && cbCardValue[2]==15)
	{
		return BCT_SINGLE_JOKER_DOUBLE_2;
	}
	//同花顺、顺子
	if (cbCardValue[0]==cbCardValue[1]+1 && cbCardValue[1]==cbCardValue[2]+1 && cbCardValue[0]<=14)
	{
		if (cbCardColor[0]==cbCardColor[1] && cbCardColor[1]==cbCardColor[2])
		{
			return BCT_TONGHUASHUN;
		}
		else
		{
			return BCT_SHUNZI;
		}
	}
	//单王
	if (cbCardColor[0] == 0x40)
	{
		return BCT_SINGLE_JOKER;
	}
	//双二
	if (cbCardValue[0]==15 && cbCardValue[1]==15)
	{
		return BCT_DOUBLE_2;
	}
	//全小
	if (cbCardValue[0] < 10)
	{
		return BCT_ALL_LITTLE;
	}

	return 0;
}

//获取底牌倍数
BYTE CGameLogic::GetBackCardMulitple( const BYTE cbBackCardType )
{
// 	if (cbBackCardType <= BCT_TONGHUASHUN)
// 	{
// 		return 3;
// 	}
// 	else if (cbBackCardType <= BCT_ALL_LITTLE)
// 	{
// 		return 2;
// 	}
	return 1;
}

//获取类型
BYTE CGameLogic::GetCardType( const BYTE cbCardData[], const BYTE cbCardCount )
{
	ASSERT(cbCardCount!=0);
	if (cbCardCount == 0)
	{
		return CT_ERROR;
	}

	//简单牌型
	switch (cbCardCount)
	{
	case 1: //单牌
		{
			return CT_SINGLE;
		}
	case 2:	//对牌火箭
		{
			//牌型判断
			if (cbCardData[0]==0x4F && cbCardData[1]==0x4E)
			{
				return CT_MISSILE_CARD;
			}
			if (GetCardLogicValue(cbCardData[0]) == GetCardLogicValue(cbCardData[1]))
			{
				return CT_DOUBLE;
			}
			return CT_ERROR;
		}
	}

	//分析扑克
	tagAnalyseResult AnalyseResult;
	if (!AnalysebCardData(cbCardData, cbCardCount, AnalyseResult))
	{
		return CT_ERROR;
	}

	//四牌判断
	if (AnalyseResult.cbFourCount > 0)
	{
		//牌型判断
		if (AnalyseResult.cbFourCount==1 && cbCardCount==4)
		{
			return CT_BOMB_CARD;
		}
		if (AnalyseResult.cbFourCount==1 && AnalyseResult.cbSingleCount==2 && cbCardCount==6)
		{
			return CT_FOUR_TAKE_ONE;
		}
		if (AnalyseResult.cbFourCount==1 && AnalyseResult.cbDoubleCount==1 && cbCardCount==6)
		{
			return CT_FOUR_TAKE_ONE;
		}
		if (AnalyseResult.cbFourCount==1 && AnalyseResult.cbDoubleCount==2 && cbCardCount==8)
		{
			return CT_FOUR_TAKE_TWO;
		}

		//分析含有炸弹飞机
		if (AnalyseResult.cbThreeCount > 0)
		{
			if ((AnalyseResult.cbFourCount+AnalyseResult.cbThreeCount)*4 == cbCardCount)
			{
				BYTE cbAnalyCardData[MAX_COUNT]={0}, cbAnalyCardCount=0;
				//拷贝四张牌
				for (int i=0; i<AnalyseResult.cbFourCount; ++i)
				{
					CopyMemory(&cbAnalyCardData[cbAnalyCardCount], &AnalyseResult.cbFourCardData[i*4], 3);
					cbAnalyCardCount += 3;
				}
				//拷贝三张牌
				for (int i=0; i<AnalyseResult.cbThreeCount; ++i)
				{
					CopyMemory(&cbAnalyCardData[cbAnalyCardCount], &AnalyseResult.cbThreeCardData[i*3], 3);
					cbAnalyCardCount += 3;
				}
				SortCardList(cbAnalyCardData, cbAnalyCardCount, ST_ORDER);
				if (GetCardType(cbAnalyCardData, cbAnalyCardCount) == CT_THREE_LINE)
				{
					return CT_THREE_TAKE_ONE;
				}
			}
		}
		return CT_ERROR;
	}

	//三牌判断
	if (AnalyseResult.cbThreeCount > 0)
	{
		//三条类型
		if (AnalyseResult.cbThreeCount==1 && cbCardCount==3)
		{
			return CT_THREE;
		}
		//连牌判断
		if (AnalyseResult.cbThreeCount > 1)
		{
			//变量定义
			BYTE cbFirstCard = AnalyseResult.cbThreeCardData[0];
			BYTE cbFirstLogicValue = GetCardLogicValue(cbFirstCard);
			BYTE cbStartIndex = 1;
			if (cbFirstLogicValue == 15)
			{
				cbFirstLogicValue = GetCardLogicValue(AnalyseResult.cbThreeCardData[cbStartIndex*3]);
				cbStartIndex = 2;
			}

			//连牌判断
			BYTE cbThreeLength=1, cbMaxThreeLenght=1;
			for (; cbStartIndex<AnalyseResult.cbThreeCount; cbStartIndex++)
			{
				cbThreeLength++;
				BYTE cbNextCard = AnalyseResult.cbThreeCardData[cbStartIndex*3];
				if (cbFirstLogicValue != (GetCardLogicValue(cbNextCard)+cbMaxThreeLenght))
				{
					cbFirstLogicValue = GetCardLogicValue(cbNextCard);
					cbThreeLength = 1;
				}	
				if (cbThreeLength > cbMaxThreeLenght)
				{
					cbMaxThreeLenght = cbThreeLength;
				}
			}
			//特殊判断
			if (cbMaxThreeLenght != AnalyseResult.cbThreeCount)
			{
				if (cbMaxThreeLenght==AnalyseResult.cbThreeCount-1 && cbMaxThreeLenght*4==cbCardCount)
				{
					return CT_THREE_TAKE_ONE;
				}
				return CT_ERROR;
			}
		}

		//牌形判断
		if (AnalyseResult.cbThreeCount*3 == cbCardCount) 
		{
			return CT_THREE_LINE;
		}
		if (AnalyseResult.cbThreeCount*4 == cbCardCount)
		{
			return CT_THREE_TAKE_ONE;
		}
		if (AnalyseResult.cbThreeCount*5==cbCardCount && AnalyseResult.cbDoubleCount==AnalyseResult.cbThreeCount)
		{
			return CT_THREE_TAKE_TWO;
		}
		return CT_ERROR;
	}

	//两张类型
	if (AnalyseResult.cbDoubleCount >= 3)
	{
		//变量定义
		BYTE cbFirstCard = AnalyseResult.cbDoubleCardData[0];
		BYTE cbFirstLogicValue = GetCardLogicValue(cbFirstCard);
		//错误过虑
		if (cbFirstLogicValue >= 15)
		{
			return CT_ERROR;
		}
		//连牌判断
		for (BYTE i=1; i<AnalyseResult.cbDoubleCount; i++)
		{
			BYTE cbnextCard = AnalyseResult.cbDoubleCardData[i*2];
			if (cbFirstLogicValue != GetCardLogicValue(cbnextCard)+i)
			{
				return CT_ERROR;
			}
		}
		//二连判断
		if (AnalyseResult.cbDoubleCount*2 == cbCardCount) 
		{
			return CT_DOUBLE_LINE;
		}
		return CT_ERROR;
	}

	//单张判断
	if (AnalyseResult.cbSingleCount>=5 && AnalyseResult.cbSingleCount==cbCardCount)
	{
		//变量定义
		BYTE cbFirstCard = AnalyseResult.cbSingleCardData[0];
		BYTE cbFirstLogicValue = GetCardLogicValue(cbFirstCard);
		//错误过虑
		if (cbFirstLogicValue >= 15)
		{
			return CT_ERROR;
		}
		//连牌判断
		for (BYTE i=1; i<AnalyseResult.cbSingleCount; i++)
		{
			BYTE cbNextCard = AnalyseResult.cbSingleCardData[i];
			if (cbFirstLogicValue != GetCardLogicValue(cbNextCard)+i)
			{
				return CT_ERROR;
			}
		}
		return CT_SINGLE_LINE;
	}
	return CT_ERROR;
}

//混乱扑克
void CGameLogic::RandCardList( BYTE cbCardBuffer[], BYTE cbBufferCount )
{
	//混乱准备
	BYTE cbCardData[CountArray(m_cbCardData)] = {0};
	CopyMemory(cbCardData, m_cbCardData, sizeof(m_cbCardData));

	//混乱扑克
	BYTE cbRandCount=0, cbPosition=0;
	do
	{
		cbPosition = rand()%(cbBufferCount-cbRandCount);
		cbCardBuffer[cbRandCount++] = cbCardData[cbPosition];
		cbCardData[cbPosition] = cbCardData[cbBufferCount-cbRandCount];
	}while (cbRandCount<cbBufferCount);
}

//排列扑克
void CGameLogic::SortCardList( BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType )
{
	//数目过虑
	if (cbCardCount<2) return;

	//转换数值
	BYTE cbSortValue[MAX_COUNT] = {0};
	for (BYTE i=0; i<cbCardCount; i++)
	{
		cbSortValue[i] = GetCardLogicValue(cbCardData[i]);	
	}

	//排序操作
	bool bSorted = true;
	BYTE cbThreeCount=0, cbLast=cbCardCount-1;
	do
	{
		bSorted = true;
		for (BYTE i=0; i<cbLast; i++)
		{
			if (cbSortValue[i]<cbSortValue[i+1] || (cbSortValue[i]==cbSortValue[i+1]&&cbCardData[i]<cbCardData[i+1]))
			{
				//交换位置
				cbThreeCount = cbCardData[i];
				cbCardData[i] = cbCardData[i+1];
				cbCardData[i+1] = cbThreeCount;
				cbThreeCount = cbSortValue[i];
				cbSortValue[i] = cbSortValue[i+1];
				cbSortValue[i+1] = cbThreeCount;
				bSorted = false;
			}	
		}
		cbLast--;
	} while(bSorted==false);

	//数目排序
	if (cbSortType == ST_COUNT)
	{
		//分析扑克
		BYTE cbIndex = 0;
		tagAnalyseResult AnalyseResult;
		if (AnalysebCardData(cbCardData, cbCardCount, AnalyseResult))
		{
			return;
		}

		//拷贝四牌
		CopyMemory(&cbCardData[cbIndex], AnalyseResult.cbFourCardData, sizeof(BYTE)*AnalyseResult.cbFourCount*4);
		cbIndex += AnalyseResult.cbFourCount*4;

		//拷贝三牌
		CopyMemory(&cbCardData[cbIndex], AnalyseResult.cbThreeCardData, sizeof(BYTE)*AnalyseResult.cbThreeCount*3);
		cbIndex += AnalyseResult.cbThreeCount*3;

		//拷贝对牌
		CopyMemory(&cbCardData[cbIndex], AnalyseResult.cbDoubleCardData, sizeof(BYTE)*AnalyseResult.cbDoubleCount*2);
		cbIndex+=AnalyseResult.cbDoubleCount*2;

		//拷贝单牌
		CopyMemory(&cbCardData[cbIndex], AnalyseResult.cbSingleCardData, sizeof(BYTE)*AnalyseResult.cbSingleCount);
		cbIndex += AnalyseResult.cbSingleCount;
	}
}

//删除扑克
bool CGameLogic::RemoveCard( const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount )
{
	if (cbRemoveCount==0)
	{
		return true;
	}
	//检验数据
	ASSERT(cbRemoveCount<=cbCardCount);
	if (cbRemoveCount>cbCardCount)
	{
		ASSERT(0);
		return false;
	}
	//定义变量
	BYTE cbDeleteCount=0, cbDeleteCard[MAX_COUNT]={0};
	if (cbCardCount>CountArray(cbDeleteCard)) return false;
	CopyMemory(cbDeleteCard, cbCardData, cbCardCount*sizeof(BYTE));

	//置零扑克
	for (BYTE i=0; i<cbRemoveCount; i++)
	{
		for (BYTE j=0; j<cbCardCount; j++)
		{
			if (cbRemoveCard[i] == cbDeleteCard[j])
			{
				cbDeleteCount++;
				cbDeleteCard[j] = 0;
				break;
			}
		}
	}
	ASSERT(cbDeleteCount==cbRemoveCount);
	if (cbDeleteCount!=cbRemoveCount) return false;

	ZeroMemory(cbCardData, sizeof(BYTE)*cbCardCount);
	//清理扑克
	for (BYTE i=0,cbCardPos=0; i<cbCardCount; i++)
	{
		if (cbDeleteCard[i] != 0)
		{
			cbCardData[cbCardPos++] = cbDeleteCard[i];
		}
	}
	return true;
}

//得到好牌
void CGameLogic::GetGoodCardData( BYTE cbGoodCardData[NORMAL_COUNT] )
{
	//混乱准备
	BYTE cbCardData[CountArray(m_cbGoodcardData)] = {0};
	BYTE cbCardBuffer[CountArray(m_cbGoodcardData)] = {0};
	CopyMemory(cbCardData, m_cbGoodcardData, sizeof(m_cbGoodcardData));

	//混乱扑克
	BYTE cbRandCount=0, cbPosition=0;
	BYTE cbBufferCount = CountArray(m_cbGoodcardData);
	do
	{
		cbPosition = rand()%(cbBufferCount-cbRandCount);
		cbCardBuffer[cbRandCount++] = cbCardData[cbPosition];
		cbCardData[cbPosition] = cbCardData[cbBufferCount-cbRandCount];
	}while (cbRandCount<cbBufferCount);

	//复制好牌
	CopyMemory(cbGoodCardData, cbCardBuffer, NORMAL_COUNT);
}

//删除好牌
bool CGameLogic::RemoveGoodCardData( BYTE cbGoodcardData[NORMAL_COUNT], BYTE cbGoodCardCount, BYTE cbCardData[FULL_COUNT], BYTE cbCardCount ) 
{
	//检验数据
	ASSERT(cbGoodCardCount<=cbCardCount);
	if (cbGoodCardCount>cbCardCount) return false;

	//定义变量
	BYTE cbDeleteCount=0,cbDeletaCard[FULL_COUNT]={0};
	if (cbCardCount>CountArray(cbDeletaCard)) return false;
	CopyMemory(cbDeletaCard, cbCardData, cbCardCount*sizeof(cbCardData[0]));

	//置零扑克
	for (BYTE i=0; i<cbGoodCardCount; i++)
	{
		for (BYTE j=0; j<cbCardCount; j++)
		{
			if (cbGoodcardData[i] == cbDeletaCard[j])
			{
				cbDeleteCount++;
				cbDeletaCard[j] = 0;
				break;
			}
		}
	}
	ASSERT(cbDeleteCount==cbGoodCardCount);
	if (cbDeleteCount!=cbGoodCardCount) return false;

	//清理扑克
	for (BYTE i=0,cbCardPos=0;i<cbCardCount;i++)
	{
		if (cbDeletaCard[i] != 0) 
		{
			cbCardData[cbCardPos++] = cbDeletaCard[i];
		}
	}

	return true;
}

//混乱缓冲区
void CGameLogic::RandArrayBuffer( BYTE cbBufferData[], BYTE cbBufferCount )
{
	BYTE cbPosition=0, cbTemp=0;
	for (BYTE i=cbBufferCount; i>1; i--)
	{
		cbPosition=BYTE(rand()%i);
		cbTemp=cbBufferData[i-1];
		cbBufferData[i-1]=cbBufferData[cbPosition];
		cbBufferData[cbPosition]=cbTemp;
	}
	return;
}

//牌型缓冲区
void CGameLogic::SortArrayBuffer( BYTE cbBufferData[], BYTE cbBufferCount )
{
	bool bSorted = true;
	BYTE cbTemp=0, cbLast=cbBufferCount-1;
	do
	{
		bSorted = true;
		for (BYTE i=0; i<cbLast; i++)
		{
			if (cbBufferData[i]>cbBufferData[i+1])
			{
				cbTemp = cbBufferData[i];
				cbBufferData[i] = cbBufferData[i+1];
				cbBufferData[i+1] = cbTemp;
				bSorted = false;
			}	
		}
		cbLast--;
	} while(bSorted==false);
}

//有效判断
bool CGameLogic::IsValidCard( BYTE cbCardData )
{
	//获取属性
	BYTE cbCardColor = GetCardColor(cbCardData);
	BYTE cbCardValue = GetCardValue(cbCardData);

	//有效判断
	if (cbCardData==0x4E || cbCardData==0x4F)
	{
		return true;
	}
	if (cbCardColor<=0x30 && cbCardValue>=0x01 && cbCardValue<=0x0D)
	{
		return true;
	}

	return false;
}

//有效判断
bool CGameLogic::IsValidCount( BYTE cbCardCount )
{
	if (cbCardCount>0 && cbCardCount<=MAX_COUNT)
	{
		return true;
	}
	return false;
}

//有效判断
bool CGameLogic::IsValidUser( WORD wChairID )
{
	if (wChairID>=0 && wChairID<GAME_PLAYER)
	{
		return true;
	}
	return false;
}

//逻辑数值
BYTE CGameLogic::GetCardLogicValue( BYTE cbCardData )
{
	ASSERT(cbCardData!=0);
	if (cbCardData == 0)
	{
		return 0;
	}
	//扑克属性
	BYTE cbCardColor = GetCardColor(cbCardData);
	BYTE cbCardValue = GetCardValue(cbCardData);
	ASSERT(cbCardValue!=0);

	//转换数值
	if (cbCardColor == 0x40)
	{
		return cbCardValue+2;
	}
	return (cbCardValue<=2) ? (cbCardValue+13) : cbCardValue;
}

//对比扑克
bool CGameLogic::CompareCard( const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount, BYTE cbRecordType )
{
	//获取类型
	BYTE cbNextType = GetCardType(cbNextCard, cbNextCount);
	BYTE cbFirstType = GetCardType(cbFirstCard, cbFirstCount);

	//特殊判断
	if (cbFirstCount==12 && cbNextCount==12 && cbRecordType==CT_THREE_TAKE_ONE)
	{
		if (cbNextType == CT_THREE_LINE)
		{
			cbNextType = CT_THREE_TAKE_ONE;
		}
		if (cbFirstType == CT_THREE_LINE)
		{
			cbFirstType = CT_THREE_TAKE_ONE;
		}
	}

	//类型判断
	if (cbNextType == CT_ERROR) return false;
	if (cbNextType == CT_MISSILE_CARD) return true;
	if (cbFirstType == CT_MISSILE_CARD) return false;

	//炸弹判断
	if (cbFirstType!=CT_BOMB_CARD && cbNextType==CT_BOMB_CARD) return true;
	if (cbFirstType==CT_BOMB_CARD && cbNextType!=CT_BOMB_CARD) return false;

	//规则判断
	if (cbFirstType!=cbNextType || cbFirstCount!=cbNextCount) return false;

	//开始对比
	switch (cbNextType)
	{
	case CT_SINGLE:
	case CT_DOUBLE:
	case CT_THREE:
	case CT_SINGLE_LINE:
	case CT_DOUBLE_LINE:
	case CT_THREE_LINE:
	case CT_BOMB_CARD:
		{
			//获取数值
			BYTE cbNextLogicValue = GetCardLogicValue(cbNextCard[0]);
			BYTE cbFirstLogicValue = GetCardLogicValue(cbFirstCard[0]);
			//对比扑克
			return (cbNextLogicValue>cbFirstLogicValue) ? true : false;
		}
	case CT_THREE_TAKE_ONE:
	case CT_THREE_TAKE_TWO:
		{
			//分析扑克
			tagAnalyseResult NextResult, FirstResult;
			VERIFY(AnalysebCardData(cbNextCard, cbNextCount, NextResult));
			VERIFY(AnalysebCardData(cbFirstCard, cbFirstCount, FirstResult));
			//获取数值
			BYTE cbFirstLogicValue = GetCardLogicValue(FirstResult.cbThreeCardData[0]);
			BYTE cbNextLogicValue = GetCardLogicValue(NextResult.cbThreeCardData[0]);

			if (NextResult.cbThreeCount>1 && GetCardLogicValue(NextResult.cbThreeCardData[0])!=GetCardLogicValue(NextResult.cbThreeCardData[3])+1)
			{
				cbNextLogicValue = GetCardLogicValue(NextResult.cbThreeCardData[3]);
			}			
			if (FirstResult.cbThreeCount>1 && GetCardLogicValue(FirstResult.cbThreeCardData[0])!=GetCardLogicValue(FirstResult.cbThreeCardData[3])+1)
			{
				cbFirstLogicValue = GetCardLogicValue(FirstResult.cbThreeCardData[3]);
			}			
			//对比扑克
			return (cbNextLogicValue>cbFirstLogicValue) ? true : false;
		}
	case CT_FOUR_TAKE_ONE:
	case CT_FOUR_TAKE_TWO:
		{
			//分析扑克
			tagAnalyseResult NextResult, FirstResult;
			VERIFY(AnalysebCardData(cbNextCard, cbNextCount, NextResult));
			VERIFY(AnalysebCardData(cbFirstCard, cbFirstCount, FirstResult));
			//获取数值
			BYTE cbNextLogicValue = GetCardLogicValue(NextResult.cbFourCardData[0]);
			BYTE cbFirstLogicValue = GetCardLogicValue(FirstResult.cbFourCardData[0]);
			//对比扑克
			return (cbNextLogicValue>cbFirstLogicValue) ? true : false;
		}
	}	
	return false;
}

//出牌搜索
bool CGameLogic::SearchOutCard( const BYTE cbHandCardData[], const BYTE cbHandCardCount, const BYTE cbTurnCardData[], const BYTE cbTurnCardCount, tagOutCardResult &OutCardResult )
{
	//设置结果
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));
	//构造扑克
	BYTE cbTempCardData[MAX_COUNT] = {0};
	BYTE cbTempCardCount = cbHandCardCount;
	CopyMemory(cbTempCardData, cbHandCardData, sizeof(BYTE)*cbHandCardCount);
	//排列扑克
	SortCardList(cbTempCardData, cbTempCardCount, ST_ORDER);

	//获取类型
	BYTE cbTurnOutType = CT_ERROR; 
	if (cbTurnCardCount > 0)
	{		
		cbTurnOutType = GetCardType(cbTurnCardData, cbTurnCardCount);
	}

	//出牌分析
	switch (cbTurnOutType)
	{
	case CT_ERROR:					//错误类型
		{
			//获取数值
			BYTE cbLogicValue = GetCardLogicValue(cbTempCardData[cbTempCardCount-1]);

			//多牌判断
			BYTE cbSameCount = 1;
			for (BYTE i=1; i<cbTempCardCount; i++)
			{
				if (GetCardLogicValue(cbTempCardData[cbTempCardCount-i-1]) == cbLogicValue)
				{
					cbSameCount++;
				}
				else
				{
					break;
				}
			}

			//完成处理
			if (cbSameCount > 1)
			{
				OutCardResult.cbCardCount = cbSameCount;
				for (BYTE j=0; j<cbSameCount; j++) 
				{
					OutCardResult.cbCardData[j] = cbTempCardData[cbTempCardCount-1-j];
				}
				return true;
			}

			//单牌处理
			OutCardResult.cbCardCount = 1;
			OutCardResult.cbCardData[0] = cbTempCardData[cbTempCardCount-1];
			return true;
		}break;
	case CT_SINGLE:					//单牌类型
	case CT_DOUBLE:					//对牌类型
	case CT_THREE:					//三条类型
		{
			//分析扑克
			tagAnalyseResult AnalyseResult;
			VERIFY(AnalysebCardData(cbTempCardData,cbTempCardCount,AnalyseResult));

			//获取数值
			BYTE cbLogicValue = GetCardLogicValue(cbTurnCardData[0]);
			//寻找单牌
			if (cbTurnCardCount <= 1)
			{
				for (BYTE i=0; i<AnalyseResult.cbSingleCount; i++)
				{
					BYTE cbIndex = AnalyseResult.cbSingleCount-i-1;
					if (GetCardLogicValue(AnalyseResult.cbSingleCardData[cbIndex]) > cbLogicValue)
					{
						//设置结果
						OutCardResult.cbCardCount = cbTurnCardCount;
						CopyMemory(OutCardResult.cbCardData, &AnalyseResult.cbSingleCardData[cbIndex], sizeof(BYTE)*cbTurnCardCount);
						return true;
					}
				}
			}
			//寻找对牌
			if (cbTurnCardCount <= 2)
			{
				for (BYTE i=0; i<AnalyseResult.cbDoubleCount; i++)
				{
					BYTE cbIndex = (AnalyseResult.cbDoubleCount-i-1)*2;
					if (GetCardLogicValue(AnalyseResult.cbDoubleCardData[cbIndex]) > cbLogicValue)
					{
						//设置结果
						OutCardResult.cbCardCount = cbTurnCardCount;
						CopyMemory(OutCardResult.cbCardData, &AnalyseResult.cbDoubleCardData[cbIndex], sizeof(BYTE)*cbTurnCardCount);
						return true;
					}
				}
			}

			//寻找三牌
			if (cbTurnCardCount <= 3)
			{
				for (BYTE i=0; i<AnalyseResult.cbThreeCount; i++)
				{
					BYTE cbIndex = (AnalyseResult.cbThreeCount-i-1)*3;
					if (GetCardLogicValue(AnalyseResult.cbThreeCardData[cbIndex]) > cbLogicValue)
					{
						//设置结果
						OutCardResult.cbCardCount = cbTurnCardCount;
						CopyMemory(OutCardResult.cbCardData, &AnalyseResult.cbThreeCardData[cbIndex], sizeof(BYTE)*cbTurnCardCount);
						return true;
					}
				}
			}			
		}break;
	case CT_SINGLE_LINE:			//单连类型
		{
			//长度判断
			if (cbTempCardCount < cbTurnCardCount) break;

			//获取数值
			BYTE cbLogicValue = GetCardLogicValue(cbTurnCardData[0]);
			//搜索连牌
			for (BYTE i=(cbTurnCardCount-1); i<cbTempCardCount; i++)
			{
				//获取数值
				BYTE cbHandLogicValue = GetCardLogicValue(cbTempCardData[cbTempCardCount-i-1]);
				//构造判断
				if (cbHandLogicValue >= 15) break;
				if (cbHandLogicValue <= cbLogicValue) continue;

				//搜索连牌
				BYTE cbLineCount = 0;
				for (BYTE j=(cbTempCardCount-i-1); j<cbTempCardCount;j ++)
				{
					if ((GetCardLogicValue(cbTempCardData[j])+cbLineCount) == cbHandLogicValue) 
					{
						//增加连数
						OutCardResult.cbCardData[cbLineCount++] = cbTempCardData[j];
						//完成判断
						if (cbLineCount == cbTurnCardCount)
						{
							OutCardResult.cbCardCount = cbTurnCardCount;
							return true;
						}
					}
				}
			}		
		}break;
	case CT_DOUBLE_LINE:			//对连类型
		{
			//长度判断
			if (cbTempCardCount < cbTurnCardCount) break;

			//获取数值
			BYTE cbLogicValue = GetCardLogicValue(cbTurnCardData[0]);
			//搜索连牌
			for (BYTE i=(cbTurnCardCount-1); i<cbTempCardCount; i++)
			{
				//获取数值
				BYTE cbHandLogicValue = GetCardLogicValue(cbTempCardData[cbTempCardCount-i-1]);
				//构造判断
				if (cbHandLogicValue <= cbLogicValue) continue;
				if (cbTurnCardCount>1 && cbHandLogicValue>=15) break;

				//搜索连牌
				BYTE cbLineCount = 0;
				for (BYTE j=(cbTempCardCount-i-1); j<(cbTempCardCount-1); j++)
				{
					if (GetCardLogicValue(cbTempCardData[j])+cbLineCount==cbHandLogicValue && GetCardLogicValue(cbTempCardData[j+1])+cbLineCount==cbHandLogicValue)
					{
						//增加连数
						OutCardResult.cbCardData[cbLineCount*2] = cbTempCardData[j];
						OutCardResult.cbCardData[(cbLineCount++)*2+1] = cbTempCardData[j+1];
						//完成判断
						if (cbLineCount*2 == cbTurnCardCount)
						{
							OutCardResult.cbCardCount = cbTurnCardCount;
							return true;
						}
					}
				}
			}
		}break;
	case CT_THREE_LINE:				//三连类型
	case CT_THREE_TAKE_ONE:	//三带一单
	case CT_THREE_TAKE_TWO:	//三带一对
		{
			//长度判断
			if (cbTempCardCount < cbTurnCardCount) break;

			//获取数值
			BYTE cbLogicValue = 0;
			for (BYTE i=0; i<cbTurnCardCount-2; i++)
			{
				cbLogicValue = GetCardLogicValue(cbTurnCardData[i]);
				if (GetCardLogicValue(cbTurnCardData[i+1]) != cbLogicValue) continue;
				if (GetCardLogicValue(cbTurnCardData[i+2]) != cbLogicValue) continue;
				break;
			}

			//属性数值
			BYTE cbTurnLineCount = 0;
			if (cbTurnOutType == CT_THREE_TAKE_ONE) cbTurnLineCount = cbTurnCardCount/4;
			else if (cbTurnOutType == CT_THREE_TAKE_TWO) cbTurnLineCount = cbTurnCardCount/5;
			else cbTurnLineCount = cbTurnCardCount/3;
			//分析扑克
			tagAnalyseResult AnalyseResultBuffer;
			VERIFY(AnalysebCardData(cbTempCardData, cbTempCardCount, AnalyseResultBuffer));

			//搜索连牌
			for (BYTE i=cbTurnLineCount*3-1; i<cbTempCardCount; i++)
			{
				//获取数值
				BYTE cbHandLogicValue = GetCardLogicValue(cbTempCardData[cbTempCardCount-i-1]);

				//构造判断
				if (cbHandLogicValue <= cbLogicValue) continue;
				if (cbTurnLineCount>1 && cbHandLogicValue>=15) break;
				bool bfour = false;
				for (int b=0; b<AnalyseResultBuffer.cbFourCount; b++)
				{
					if (GetCardLogicValue(AnalyseResultBuffer.cbFourCardData[b*4]) == cbHandLogicValue)
					{
						bfour = true;
						break;
					}
				}
				if (bfour)
				{
					continue;
				}

				//搜索连牌
				BYTE cbLineCount = 0;
				for (BYTE j=(cbTempCardCount-i-1); j<(cbTempCardCount-2); j++)
				{
					//设置变量
					OutCardResult.cbCardCount = 0;
					//三牌判断
					if (GetCardLogicValue(cbTempCardData[j])+cbLineCount != cbHandLogicValue) continue;
					if (GetCardLogicValue(cbTempCardData[j+1])+cbLineCount != cbHandLogicValue) continue;
					if (GetCardLogicValue(cbTempCardData[j+2])+cbLineCount != cbHandLogicValue) continue;

					//增加连数
					OutCardResult.cbCardData[cbLineCount*3] = cbTempCardData[j];
					OutCardResult.cbCardData[cbLineCount*3+1] = cbTempCardData[j+1];
					OutCardResult.cbCardData[(cbLineCount++)*3+2] = cbTempCardData[j+2];

					//完成判断
					if (cbLineCount == cbTurnLineCount)
					{
						//连牌设置
						OutCardResult.cbCardCount = cbLineCount*3;

						//构造扑克
						BYTE cbLeftCardData[MAX_COUNT] = {0};
						BYTE cbLeftCount = cbTempCardCount-OutCardResult.cbCardCount;
						CopyMemory(cbLeftCardData, cbTempCardData, sizeof(BYTE)*cbTempCardCount);
						RemoveCard(OutCardResult.cbCardData, OutCardResult.cbCardCount, cbLeftCardData, cbTempCardCount);

						//分析扑克
						tagAnalyseResult AnalyseResultLeft;
						VERIFY(AnalysebCardData(cbLeftCardData,cbLeftCount,AnalyseResultLeft));

						//单牌处理
						if (cbTurnOutType == CT_THREE_TAKE_ONE)
						{
							//提取单牌
							for (BYTE k=0; k<AnalyseResultLeft.cbSingleCount; k++)
							{
								//中止判断
								if (OutCardResult.cbCardCount == cbTurnCardCount) break;	

								//设置扑克
								BYTE cbIndex = AnalyseResultLeft.cbSingleCount-k-1;
								if (cbHandLogicValue == GetCardLogicValue(AnalyseResultLeft.cbSingleCardData[cbIndex]))
								{
									continue;
								}
								BYTE cbSignedCard = AnalyseResultLeft.cbSingleCardData[cbIndex];
								OutCardResult.cbCardData[OutCardResult.cbCardCount++] = cbSignedCard;
							}

							//提取对牌
							for (BYTE k=0; k<AnalyseResultLeft.cbDoubleCount*2; k++)
							{
								//中止判断
								if (OutCardResult.cbCardCount == cbTurnCardCount) break;

								//设置扑克
								BYTE cbIndex = (AnalyseResultLeft.cbDoubleCount*2-k-1);
								BYTE cbSignedCard = AnalyseResultLeft.cbDoubleCardData[cbIndex];
								OutCardResult.cbCardData[OutCardResult.cbCardCount++] = cbSignedCard;
							}

							//提取三牌
							for (BYTE k=0; k<AnalyseResultLeft.cbThreeCount*3; k++)
							{
								//中止判断
								if (OutCardResult.cbCardCount == cbTurnCardCount) break;

								//设置扑克
								BYTE cbIndex = (AnalyseResultLeft.cbThreeCount*3-k-1);
								BYTE cbSignedCard = AnalyseResultLeft.cbThreeCardData[cbIndex];
								OutCardResult.cbCardData[OutCardResult.cbCardCount++] = cbSignedCard;
							}

							//提取四牌
							for (BYTE k=0;k<AnalyseResultLeft.cbFourCount*4;k++)
							{
								//中止判断
								if (OutCardResult.cbCardCount==cbTurnCardCount) break;

								//设置扑克
								BYTE cbIndex=(AnalyseResultLeft.cbFourCount*4-k-1);
								BYTE cbSignedCard=AnalyseResultLeft.cbFourCardData[cbIndex];
								OutCardResult.cbCardData[OutCardResult.cbCardCount++]=cbSignedCard;
							}
						}

						//对牌处理
						if (cbTurnOutType == CT_THREE_TAKE_TWO)
						{
							//提取对牌
							for (BYTE k=0; k<AnalyseResultLeft.cbDoubleCount; k++)
							{
								//中止判断
								if (OutCardResult.cbCardCount == cbTurnCardCount) break;

								//设置扑克
								BYTE cbIndex = (AnalyseResultLeft.cbDoubleCount-k-1)*2;
								BYTE cbCardData1 = AnalyseResultLeft.cbDoubleCardData[cbIndex];
								BYTE cbCardData2 = AnalyseResultLeft.cbDoubleCardData[cbIndex+1];
								OutCardResult.cbCardData[OutCardResult.cbCardCount++] = cbCardData1;
								OutCardResult.cbCardData[OutCardResult.cbCardCount++] = cbCardData2;
							}

							//提取三牌
							for (BYTE k=0; k<AnalyseResultLeft.cbThreeCount; k++)
							{
								//中止判断
								if (OutCardResult.cbCardCount == cbTurnCardCount) break;

								//设置扑克
								BYTE cbIndex = (AnalyseResultLeft.cbThreeCount-k-1)*3;
								BYTE cbCardData1 = AnalyseResultLeft.cbThreeCardData[cbIndex];
								BYTE cbCardData2 = AnalyseResultLeft.cbThreeCardData[cbIndex+1];
								OutCardResult.cbCardData[OutCardResult.cbCardCount++] = cbCardData1;
								OutCardResult.cbCardData[OutCardResult.cbCardCount++] = cbCardData2;
							}

							//提取四牌
							for (BYTE k=0; k<AnalyseResultLeft.cbFourCount; k++)
							{
								//中止判断
								if (OutCardResult.cbCardCount == cbTurnCardCount) break;

								//设置扑克
								BYTE cbIndex = (AnalyseResultLeft.cbFourCount-k-1)*4;
								BYTE cbCardData1 = AnalyseResultLeft.cbFourCardData[cbIndex];
								BYTE cbCardData2 = AnalyseResultLeft.cbFourCardData[cbIndex+1];
								OutCardResult.cbCardData[OutCardResult.cbCardCount++] = cbCardData1;
								OutCardResult.cbCardData[OutCardResult.cbCardCount++] = cbCardData2;
							}
						}

						//完成判断
						if (OutCardResult.cbCardCount == cbTurnCardCount)
						{
							return true;
						}
					}
				}
			}
		}break;
	}

	//搜索炸弹
	if (cbTempCardCount>=4 && cbTurnOutType!=CT_MISSILE_CARD)
	{
		//变量定义
		BYTE cbLogicValue = 0;
		if (cbTurnOutType == CT_BOMB_CARD)
		{
			cbLogicValue = GetCardLogicValue(cbTurnCardData[0]);
		}

		//搜索炸弹
		for (BYTE i=3; i<cbTempCardCount; i++)
		{
			//获取数值
			BYTE cbHandLogicValue = GetCardLogicValue(cbTempCardData[cbTempCardCount-i-1]);

			//构造判断
			if (cbHandLogicValue <= cbLogicValue) continue;

			//炸弹判断
			BYTE j = 1;
			BYTE cbTempLogicValue = GetCardLogicValue(cbTempCardData[cbTempCardCount-i-1]);
			for (; j<4; j++)
			{
				if (GetCardLogicValue(cbTempCardData[cbTempCardCount+j-i-1]) != cbTempLogicValue) 
				{
					break;
				}
			}
			if (j != 4)
			{
				continue;
			}

			//设置结果
			OutCardResult.cbCardCount = 4;
			OutCardResult.cbCardData[0] = cbTempCardData[cbTempCardCount-i-1];
			OutCardResult.cbCardData[1] = cbTempCardData[cbTempCardCount-i];
			OutCardResult.cbCardData[2] = cbTempCardData[cbTempCardCount-i+1];
			OutCardResult.cbCardData[3] = cbTempCardData[cbTempCardCount-i+2];
			return true;
		}
	}

	//搜索火箭
	if (cbTempCardCount>=2 && cbTempCardData[0]==0x4F && cbTempCardData[1]==0x4E)
	{
		if (cbTurnOutType == CT_MISSILE_CARD) return true;
		//设置结果
		OutCardResult.cbCardCount = 2;
		OutCardResult.cbCardData[0] = cbTempCardData[0];
		OutCardResult.cbCardData[1] = cbTempCardData[1];
		return true;
	}
	return false;
}

//提示出牌
bool CGameLogic::SearchOutCard( const BYTE cbHandCardData[], const BYTE cbHandCardCount, const BYTE cbTurnCardData[], const BYTE cbTurnCardCount, tagPromptCardResult &OutCardResult )
{
	//设置结果
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));

	//拷贝出牌
	BYTE cbOutCardData[MAX_COUNT] = {0};
	BYTE cbOutCardCount = cbTurnCardCount;
	CopyMemory(cbOutCardData, cbTurnCardData, cbTurnCardCount);

	bool bContinue = true;
	tagOutCardResult TempOutCardResult;
	while (bContinue)
	{
		ZeroMemory(&TempOutCardResult, sizeof(TempOutCardResult));
		SearchOutCard(cbHandCardData, cbHandCardCount, cbOutCardData, cbOutCardCount, TempOutCardResult);
		if (TempOutCardResult.cbCardCount > 0)
		{
			BYTE cbPos = OutCardResult.cbResultCount;
			OutCardResult.cbCardCount[cbPos] = TempOutCardResult.cbCardCount;
			CopyMemory(OutCardResult.cbCardData[cbPos], TempOutCardResult.cbCardData, sizeof(BYTE)*TempOutCardResult.cbCardCount);
			OutCardResult.cbResultCount++;

			cbOutCardCount = TempOutCardResult.cbCardCount;
			CopyMemory(cbOutCardData, TempOutCardResult.cbCardData, sizeof(BYTE)*TempOutCardResult.cbCardCount);
		}
		else
		{
			bContinue = false;
		}
	}
	return false;
}

//分析扑克
bool CGameLogic::AnalysebCardData( const BYTE cbCardData[], const BYTE cbCardCount, tagAnalyseResult &AnalyseResult )
{
	//设置结果
	ZeroMemory(&AnalyseResult, sizeof(AnalyseResult));
	if (cbCardCount == 0) return false;

	//扑克分析
	for (BYTE i=0; i<cbCardCount; i++)
	{
		//变量定义
		BYTE cbSameCount = 1;
		BYTE cbFirstLogicValue = GetCardLogicValue(cbCardData[i]);
		if (cbFirstLogicValue == 0) return false;

		//搜索同牌
		for (BYTE j=i+1; j<cbCardCount; j++)
		{
			BYTE cbNextLogicValue = GetCardLogicValue(cbCardData[j]);
			if (cbNextLogicValue == 0) return false;
			//获取扑克
			if (cbNextLogicValue != cbFirstLogicValue) break;
			//设置变量
			cbSameCount++;
		}

		//设置结果
		switch (cbSameCount)
		{
		case 1:		//单张
			{
				BYTE cbIndex = AnalyseResult.cbSingleCount++;
				AnalyseResult.cbSingleCardData[cbIndex*cbSameCount] = cbCardData[i];
			}break;
		case 2:		//两张
			{
				BYTE cbIndex = AnalyseResult.cbDoubleCount++;
				AnalyseResult.cbDoubleCardData[cbIndex*cbSameCount] = cbCardData[i];
				AnalyseResult.cbDoubleCardData[cbIndex*cbSameCount+1] = cbCardData[i+1];
			}break;
		case 3:		//三张
			{
				BYTE cbIndex = AnalyseResult.cbThreeCount++;
				AnalyseResult.cbThreeCardData[cbIndex*cbSameCount] = cbCardData[i];
				AnalyseResult.cbThreeCardData[cbIndex*cbSameCount+1] = cbCardData[i+1];
				AnalyseResult.cbThreeCardData[cbIndex*cbSameCount+2] = cbCardData[i+2];
			}break;
		case 4:		//四张
			{
				BYTE cbIndex = AnalyseResult.cbFourCount++;
				AnalyseResult.cbFourCardData[cbIndex*cbSameCount] = cbCardData[i];
				AnalyseResult.cbFourCardData[cbIndex*cbSameCount+1] = cbCardData[i+1];
				AnalyseResult.cbFourCardData[cbIndex*cbSameCount+2] = cbCardData[i+2];
				AnalyseResult.cbFourCardData[cbIndex*cbSameCount+3] = cbCardData[i+3];
			}break;
		default:
			{
				ASSERT(FALSE);
			}break;
		}

		//设置索引
		i += cbSameCount-1;
	}
	return true;
}

//分析炸弹
void CGameLogic::GetAllBombCard( const BYTE cbHandCardData[], const BYTE cbHandCardCount, BYTE cbBomCardData[], BYTE &cbBomCardCount )
{
	cbBomCardCount = 0;
	//数据校验
	if (cbHandCardCount < 2) return;

	BYTE cbTempCardData[MAX_COUNT] = {0};
	CopyMemory(cbTempCardData, cbHandCardData, cbHandCardCount);
	//大小排序
	SortCardList(cbTempCardData, cbHandCardCount, ST_ORDER);

	//双王炸弹
	if (0x4F==cbTempCardData[0] && 0x4E==cbTempCardData[1])
	{
		cbBomCardData[cbBomCardCount++] = cbTempCardData[0];
		cbBomCardData[cbBomCardCount++] = cbTempCardData[1];
	}

	//扑克分析
	for (BYTE i=0; i<cbHandCardCount; i++)
	{
		//变量定义
		BYTE cbSameCount = 1;
		BYTE cbLogicValue = GetCardLogicValue(cbTempCardData[i]);

		//搜索同牌
		for (BYTE j=i+1; j<cbHandCardCount; j++)
		{
			//获取扑克
			if (GetCardLogicValue(cbTempCardData[j])!=cbLogicValue) break;

			//设置变量
			cbSameCount++;
		}
		//判断、加入
		if (4 == cbSameCount)
		{
			cbBomCardData[cbBomCardCount++] = cbTempCardData[i];
			cbBomCardData[cbBomCardCount++] = cbTempCardData[i+1];
			cbBomCardData[cbBomCardCount++] = cbTempCardData[i+2];
			cbBomCardData[cbBomCardCount++] = cbTempCardData[i+3];
		}
		//设置索引
		i += cbSameCount-1;
	}
}

//分析三条
void CGameLogic::GetAllThreeCard( const BYTE cbHandCardData[], const BYTE cbHandCardCount, BYTE cbThreeCardData[], BYTE &cbThreeCardCount )
{
	cbThreeCardCount = 0;
	if (cbHandCardCount < 3) return;

	BYTE cbTempCardData[MAX_COUNT] = {0};
	CopyMemory(cbTempCardData, cbHandCardData, cbHandCardCount);
	//大小排序
	SortCardList(cbTempCardData, cbHandCardCount, ST_ORDER);

	//扑克分析
	for (BYTE i=0;i<cbHandCardCount;i++)
	{
		//变量定义
		BYTE cbSameCount = 1;
		BYTE cbLogicValue = GetCardLogicValue(cbTempCardData[i]);

		//搜索同牌
		for (BYTE j=i+1; j<cbHandCardCount; j++)
		{
			//获取扑克
			if (GetCardLogicValue(cbTempCardData[j]) != cbLogicValue) break;
			//设置变量
			cbSameCount++;
		}
		//判断、加入
		if (cbSameCount >= 3)
		{
			cbThreeCardData[cbThreeCardCount++] = cbTempCardData[i];
			cbThreeCardData[cbThreeCardCount++] = cbTempCardData[i+1];
			cbThreeCardData[cbThreeCardCount++] = cbTempCardData[i+2];	
		}
		//设置索引
		i += cbSameCount-1;
	}
}

//分析对子
void CGameLogic::GetAllDoubleCard( const BYTE cbHandCardData[], const BYTE cbHandCardCount, BYTE cbDoubleCardData[], BYTE &cbDoubleCardCount )
{
	cbDoubleCardCount = 0;
	if (cbHandCardCount < 2) return;

	BYTE cbTempCardData[MAX_COUNT] = {0};
	CopyMemory(cbTempCardData, cbHandCardData, cbHandCardCount);
	//大小排序
	SortCardList(cbTempCardData, cbHandCardCount, ST_ORDER);

	//扑克分析
	for (BYTE i=0; i<cbHandCardCount; i++)
	{
		//变量定义
		BYTE cbSameCount = 1;
		BYTE cbLogicValue = GetCardLogicValue(cbTempCardData[i]);

		//搜索同牌
		for (BYTE j=i+1; j<cbHandCardCount; j++)
		{
			//获取扑克
			if (GetCardLogicValue(cbTempCardData[j]) != cbLogicValue) break;
			//设置变量
			cbSameCount++;
		}
		//判断、加入
		if (cbSameCount >= 2)
		{
			cbDoubleCardData[cbDoubleCardCount++] = cbTempCardData[i];
			cbDoubleCardData[cbDoubleCardCount++] = cbTempCardData[i+1];
		}
		//设置索引
		i += cbSameCount-1;
	}
}

//分析单牌
void CGameLogic::GetAllSingleCard( const BYTE cbHandCardData[], const BYTE cbHandCardCount, BYTE cbSingleCardData[], BYTE &cbSingleCardCount, bool bOnlySingle )
{
	cbSingleCardCount = 0;
	if (cbHandCardCount == 0) return;

	BYTE cbTempCardData[MAX_COUNT] = {0};
	CopyMemory(cbTempCardData, cbHandCardData, cbHandCardCount);
	//大小排序
	SortCardList(cbTempCardData, cbHandCardCount, ST_ORDER);

	//扑克分析
	for (BYTE i=0; i<cbHandCardCount; i++)
	{
		//变量定义
		BYTE cbSameCount = 1;
		BYTE cbLogicValue = GetCardLogicValue(cbTempCardData[i]);

		//搜索同牌
		for (BYTE j=i+1; j<cbHandCardCount; j++)
		{
			//获取扑克
			if (GetCardLogicValue(cbTempCardData[j]) != cbLogicValue) break;
			//设置变量
			cbSameCount++;
		}
		//判断、加入
		if (bOnlySingle)
		{
			if (cbSameCount == 1)
			{
				cbSingleCardData[cbSingleCardCount++] = cbTempCardData[i];
			}
		}
		else
		{
			if (cbSameCount >= 1)
			{
				cbSingleCardData[cbSingleCardCount++] = cbTempCardData[i];
			}
		}
		//设置索引
		i += cbSameCount-1;
	}
}

//分析顺子
void CGameLogic::GetAllLineCard( const BYTE cbHandCardData[], const BYTE cbHandCardCount, BYTE cbLineCardData[], BYTE &cbLineCardCount )
{
	cbLineCardCount = 0;
	//数据校验
	if (cbHandCardCount < 5) return;

	BYTE cbTempCardData[MAX_COUNT] = {0};
	CopyMemory(cbTempCardData, cbHandCardData, cbHandCardCount);
	//大小排序
	SortCardList(cbTempCardData, cbHandCardCount, ST_ORDER);

	BYTE cbFirstCard = 0;
	//去除2和王
	for (BYTE i=0; i<cbHandCardCount; ++i)	
	{
		if (GetCardLogicValue(cbTempCardData[i]) < 15)
		{
			cbFirstCard = i;
			break;
		}
	}
	//数据校验
	if (cbHandCardCount-cbFirstCard<5) return;

	BYTE cbSingleLineCount=0, cbSingleLineCard[12]={0};
	BYTE cbLeftCardCount = cbHandCardCount-cbFirstCard;
	bool bFindSingleLine = true;
	//连牌判断
	while (cbLeftCardCount>=5 && bFindSingleLine)
	{
		cbSingleLineCount = 1;
		bFindSingleLine = false;
		BYTE cbLastCard = cbTempCardData[cbFirstCard];
		cbSingleLineCard[cbSingleLineCount-1] = cbTempCardData[cbFirstCard];
		for (BYTE i=cbFirstCard+1; i<cbLeftCardCount; i++)
		{
			BYTE cbCardData = cbTempCardData[i];
			//连续判断
			if (GetCardLogicValue(cbLastCard)-GetCardLogicValue(cbCardData)!=1 && GetCardValue(cbLastCard)!=GetCardValue(cbCardData)) 
			{
				cbLastCard = cbTempCardData[i];
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
			//同牌判断
			else if (GetCardValue(cbLastCard) != GetCardValue(cbCardData))
			{
				cbLastCard = cbCardData;
				cbSingleLineCard[cbSingleLineCount] = cbCardData;
				++cbSingleLineCount;
			}					
		}

		//保存数据
		if (cbSingleLineCount >= 5)
		{
			RemoveCard(cbSingleLineCard, cbSingleLineCount, cbTempCardData, cbLeftCardCount);
			memcpy(cbLineCardData+cbLineCardCount, cbSingleLineCard, sizeof(BYTE)*cbSingleLineCount);
			cbLineCardCount += cbSingleLineCount;
			cbLeftCardCount -= cbSingleLineCount;
			bFindSingleLine = true;
		}
	}
}

//分析顺子
void CGameLogic::GetAllLineCard( const BYTE cbHandCardData[], const BYTE cbHandCardCount, tagPromptCardInfo &PromptCardInfo )
{	
	//数据校验
	if (cbHandCardCount < 5) return;

	BYTE cbTempCardData[MAX_COUNT] = {0};
	CopyMemory(cbTempCardData, cbHandCardData, cbHandCardCount);
	//大小排序
	SortCardList(cbTempCardData, cbHandCardCount, ST_ORDER);

	BYTE cbFirstCard = 0;
	//去除2和王
	for (BYTE i=0; i<cbHandCardCount; ++i)
	{
		if (GetCardLogicValue(cbTempCardData[i]) < 15)
		{
			cbFirstCard = i;
			break;
		}
	}


	BYTE cbLinkResult = 0;
	BYTE cbSingleLineCard[12] = {0};
	BYTE cbSingleLineCount = 0;
	BYTE cbLeftCardCount = cbHandCardCount - cbFirstCard; 
	bool bFindSingleLine = true;
	//连牌判断
	while (cbLeftCardCount>=5 && bFindSingleLine)
	{
		cbSingleLineCount = 1;
		bFindSingleLine = false;
		BYTE cbLastCard = cbTempCardData[cbFirstCard];
		cbSingleLineCard[cbSingleLineCount-1] = cbTempCardData[cbFirstCard];
		for (BYTE i=cbFirstCard+1; i<cbLeftCardCount; i++)
		{
			BYTE cbCardData=cbTempCardData[i];

			//连续判断
			if (GetCardLogicValue(cbLastCard)-GetCardLogicValue(cbCardData)!=1 && GetCardValue(cbLastCard)!=GetCardValue(cbCardData)) 
			{
				cbLastCard = cbTempCardData[i];
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
			//同牌判断
			else if (GetCardValue(cbLastCard) != GetCardValue(cbCardData))
			{
				cbLastCard = cbCardData;
				cbSingleLineCard[cbSingleLineCount] = cbCardData;
				++cbSingleLineCount;
			}					
		}

		//保存数据
		if (cbSingleLineCount >= 5)
		{
			RemoveCard(cbSingleLineCard, cbSingleLineCount, cbTempCardData, cbLeftCardCount);		
			CopyMemory(PromptCardInfo.stSingleLineResult.cbCardData[cbLinkResult], cbSingleLineCard, sizeof(BYTE)*cbSingleLineCount);
			PromptCardInfo.stSingleLineResult.cbCardCount[cbLinkResult] = cbSingleLineCount;
			++PromptCardInfo.stSingleLineResult.cbResultCount;
			++cbLinkResult;
			cbLeftCardCount -= cbSingleLineCount;
			bFindSingleLine = true;
		}
	}
}

//提取信息
void CGameLogic::PromptHandCardEachType( const BYTE cbHandCardData[], const BYTE cbHandCardCount, tagPromptCardInfo &PromptCardInfo )
{	
	ZeroMemory(&PromptCardInfo, sizeof(tagPromptCardInfo));
	ASSERT(cbHandCardCount>=0 && cbHandCardCount<=MAX_COUNT);
	if (cbHandCardCount==0 || cbHandCardCount>MAX_COUNT) return;

	//拷贝数据
	BYTE cbTempCardData[MAX_COUNT] = {0};
	BYTE cbTempCardCount = cbHandCardCount;
	CopyMemory(cbTempCardData, cbHandCardData, cbHandCardCount);
	//排序扑克
	SortCardList(cbTempCardData, cbTempCardCount, ST_ORDER);

	//分析扑克
	tagAnalyseResult AnalyseResult;
	VERIFY(AnalysebCardData(cbTempCardData,cbTempCardCount,AnalyseResult));

	//火箭
	if (AnalyseResult.cbSingleCount>=2 && AnalyseResult.cbSingleCardData[0]==0x4F && AnalyseResult.cbSingleCardData[1]==0x4E)
	{
		PromptCardInfo.stRocketResult.cbResultCount = 1;
		PromptCardInfo.stRocketResult.cbCardCount[0] = 2;
		PromptCardInfo.stRocketResult.cbCardData[0][0] = 0x4F;
		PromptCardInfo.stRocketResult.cbCardData[0][1] = 0x4E;
	}

	//炸弹
	if (AnalyseResult.cbFourCount > 0)
	{
		PromptCardInfo.stBombResult.cbResultCount = AnalyseResult.cbFourCount;
		for (int i=AnalyseResult.cbFourCount,k=0; i>0; --i,++k)
		{
			PromptCardInfo.stBombResult.cbCardCount[k] = 4;
			CopyMemory(PromptCardInfo.stBombResult.cbCardData[k], &AnalyseResult.cbFourCardData[(i-1)*4], sizeof(BYTE)*4);
		}
	}

	//四带二
	if (AnalyseResult.cbFourCount>0 && cbHandCardCount>=6)
	{
		int iResultCount = 0;
		for (int i=AnalyseResult.cbFourCount,k=0; i>0; --i,++k)
		{
			//四带二单
			if (AnalyseResult.cbSingleCount >= 2)
			{
				PromptCardInfo.stFourTwoResult.cbCardCount[iResultCount] = 6;
				CopyMemory(PromptCardInfo.stFourTwoResult.cbCardData[iResultCount], &AnalyseResult.cbFourCardData[(i-1)*4], sizeof(BYTE)*4);
				CopyMemory(&PromptCardInfo.stFourTwoResult.cbCardData[iResultCount][4], &AnalyseResult.cbSingleCardData[AnalyseResult.cbSingleCount-2], sizeof(BYTE)*2);
				SortCardList(PromptCardInfo.stFourTwoResult.cbCardData[iResultCount], 6, ST_ORDER);
				++iResultCount;
			}
			else
			{
				BYTE cbElemCardData[MAX_COUNT] = {0};
				CopyMemory(cbElemCardData, cbHandCardData, cbHandCardCount);
				RemoveCard(&AnalyseResult.cbFourCardData[(i-1)*4], 4, cbElemCardData, cbHandCardCount);
				PromptCardInfo.stFourTwoResult.cbCardCount[iResultCount] = 6;
				CopyMemory(PromptCardInfo.stFourTwoResult.cbCardData[iResultCount], &AnalyseResult.cbFourCardData[(i-1)*4], sizeof(BYTE)*4);
				CopyMemory(&PromptCardInfo.stFourTwoResult.cbCardData[iResultCount][4], &cbElemCardData[cbHandCardCount-6], sizeof(BYTE)*2);
				SortCardList(PromptCardInfo.stFourTwoResult.cbCardData[iResultCount], 6, ST_ORDER);
				++iResultCount;
			}
			//四带二双
			if (cbHandCardCount >= 8)
			{
				if (AnalyseResult.cbDoubleCount >= 2)
				{
					PromptCardInfo.stFourTwoResult.cbCardCount[iResultCount] = 8;
					CopyMemory(PromptCardInfo.stFourTwoResult.cbCardData[iResultCount], &AnalyseResult.cbFourCardData[(i-1)*4], sizeof(BYTE)*4);
					CopyMemory(&PromptCardInfo.stFourTwoResult.cbCardData[iResultCount][4], &AnalyseResult.cbDoubleCardData[AnalyseResult.cbDoubleCount*2-4], sizeof(BYTE)*4);
					SortCardList(PromptCardInfo.stFourTwoResult.cbCardData[iResultCount], 8, ST_ORDER);
					++iResultCount;
				}
				else
				{
					BYTE cbElemCardData[MAX_COUNT] = {0};
					CopyMemory(cbElemCardData, cbHandCardData, cbHandCardCount);
					RemoveCard(&AnalyseResult.cbFourCardData[(i-1)*4], 4, cbElemCardData, cbHandCardCount);
					//提取对牌
					BYTE cbDoubleCard[MAX_COUNT]={0}, cbDoubleCount=0;
					GetAllDoubleCard(cbElemCardData, cbHandCardCount-4, cbDoubleCard, cbDoubleCount);
					if (cbDoubleCount >= 4)
					{
						PromptCardInfo.stFourTwoResult.cbCardCount[iResultCount] = 8;
						CopyMemory(PromptCardInfo.stFourTwoResult.cbCardData[iResultCount], &AnalyseResult.cbFourCardData[(i-1)*4], sizeof(BYTE)*4);
						CopyMemory(&PromptCardInfo.stFourTwoResult.cbCardData[iResultCount][4], &cbDoubleCard[cbDoubleCount-4], sizeof(BYTE)*4);
						SortCardList(PromptCardInfo.stFourTwoResult.cbCardData[iResultCount], 8, ST_ORDER);
						++iResultCount;
					}
				}
			}
		}
		PromptCardInfo.stFourTwoResult.cbResultCount = iResultCount;
	}

	//飞机
	BYTE cbThreeCardData[MAX_COUNT]={0}, cbThreeCardCount=0;
	GetAllThreeCard(cbHandCardData, cbHandCardCount, cbThreeCardData, cbThreeCardCount);
	if (cbThreeCardCount >= 6)
	{
		for (int i=0; i<cbThreeCardCount/3-1; ++i)
		{
			if (GetCardLogicValue(cbThreeCardData[i*3])<=14 && GetCardLogicValue(cbThreeCardData[i*3])==GetCardLogicValue(cbThreeCardData[(i+1)*3])+1)
			{
				PromptCardInfo.cbPlaneResult.cbResultCount = 1;
				CopyMemory(PromptCardInfo.cbPlaneResult.cbCardData[0], &cbThreeCardData[i*3], sizeof(BYTE)*6);
				PromptCardInfo.cbPlaneResult.cbCardCount[0] = 6;
				break;
			}
		}
	}

	//姐妹对
	BYTE cbElemCardData[MAX_COUNT]={0}, cbElemCardCount=cbHandCardCount;
	CopyMemory(cbElemCardData, cbHandCardData, cbHandCardCount);
	BYTE cbDoubleCard[MAX_COUNT]={0}, cbDoubleCount=0;
	BYTE cbDoubleLineCount = 0;
	while(true)
	{
		ZeroMemory(cbDoubleCard, cbDoubleCount);
		GetAllDoubleCard(cbElemCardData, cbElemCardCount, cbDoubleCard, cbDoubleCount);
		if (cbDoubleCount >= 6)
		{
			BYTE cbSameCount=1, cbPos=0;
			for (int i=0,n=cbDoubleCount/2; i<n; ++i)
			{
				for (int k=i; k<n-1; ++k)
				{
					if (GetCardLogicValue(cbDoubleCard[k*2])<=14 &&	GetCardLogicValue(cbDoubleCard[k*2])==GetCardLogicValue(cbDoubleCard[(k+1)*2])+1)
					{
						++cbSameCount;
					}
					else
					{
						break;
					}
				}
				if (cbSameCount >  2) 
				{
					cbPos = i*2;
					break;
				}
				else
				{
					cbSameCount = 1;
				}
			}

			if (cbSameCount > 2)
			{
				++PromptCardInfo.stDoubleLineResult.cbResultCount;
				PromptCardInfo.stDoubleLineResult.cbCardCount[cbDoubleLineCount] = cbSameCount*2;
				CopyMemory(PromptCardInfo.stDoubleLineResult.cbCardData[cbDoubleLineCount], &cbDoubleCard[cbPos], sizeof(BYTE)*cbSameCount*2);
				RemoveCard(PromptCardInfo.stDoubleLineResult.cbCardData[cbDoubleLineCount], cbSameCount*2, cbElemCardData, cbElemCardCount);
				cbElemCardCount -= cbSameCount*2;
				++cbDoubleLineCount;
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}

	//顺子
	GetAllLineCard(cbHandCardData, cbHandCardCount, PromptCardInfo);
}



//////////////////////////////////////////////////////////////////////////
