#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
LONG			CGameLogic::m_lQuanYa[4][INT_ROBOT_QUANYA]={0};		//机器人全押率	
LONG			CGameLogic::m_lGiveUp[4][INT_ROBOT_GIVEUP]={0};		//机器人放弃率	
LONG			CGameLogic::m_lAdd[4][INT_ROBOT_ADD]={0};			//机器人加注率
const WORD		CGameLogic::m_wLogicCount=GAME_PLAYER;
CString 		CGameLogic::m_strFileName;
CString 		CGameLogic::m_strFileName2;

//扑克数据
BYTE CGameLogic::m_cbCardData[FULL_COUNT]={
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//方块 A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//梅花 A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//红桃 A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D	//黑桃 A - K
};

//构造函数
CGameLogic::CGameLogic()
{
}

//析构函数
CGameLogic::~CGameLogic()
{

}

//混乱扑克
VOID CGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//测试代码
	//CopyMemory(cbCardBuffer,m_cbCardData,cbBufferCount);
	//混乱准备
	BYTE bCardCount = CountArray(m_cbCardData);
	BYTE cbCardData[CountArray(m_cbCardData)];
	CopyMemory(cbCardData,m_cbCardData,sizeof(m_cbCardData));

	//混乱扑克
	BYTE cbRandCount=0,cbPosition=0;
	int buffer=bCardCount-cbRandCount;
	do
	{
		buffer=bCardCount-cbRandCount;
		if(buffer<=0)
		{
			buffer =1;
		}
		cbPosition=BYTE(rand()%buffer);
		cbCardBuffer[cbRandCount++]=cbCardData[cbPosition];
		cbCardData[cbPosition]=cbCardData[bCardCount-cbRandCount];
	} while (cbRandCount<cbBufferCount);

	return;
}

//获取类型
BYTE CGameLogic::GetCardType(BYTE cbCardData[], BYTE cbCardCount)
{
	//数据校验
	//ASSERT(cbCardCount == MAX_CENTERCOUNT);
	//if(cbCardCount !=MAX_CENTERCOUNT) return 0;

	//变量定义
	bool cbSameColor=true;
	bool bLineCard=true;
	BYTE cbFirstColor=GetCardColor(cbCardData[0]);
	BYTE cbFirstValue=GetCardLogicValue(cbCardData[0]);

	//牌形分析
	for (BYTE i=1;i<cbCardCount;i++)
	{
		//数据分析
		if (GetCardColor(cbCardData[i])!=cbFirstColor) cbSameColor=false;
		if (cbFirstValue!=(GetCardLogicValue(cbCardData[i])+i))
		{
			bLineCard=false;
		}

		//结束判断
		if ((cbSameColor==false) && (bLineCard==false))
		{
			break;
		}
	}

	//最小同花顺
	if((bLineCard == false) && (cbFirstValue == CARD_VALUE_A))
	{
		BYTE i=1;
		for (i=1;i<cbCardCount;i++)
		{
			BYTE cbLogicValue=GetCardLogicValue(cbCardData[i]);
			if ((cbFirstValue!=(cbLogicValue+i+8)))
			{
				break;
			}
		}
		if( i == cbCardCount)
		{
			bLineCard =true;
		}
	}

	//皇家同花顺
	if ((cbSameColor==true) && (bLineCard==true) && (GetCardLogicValue(cbCardData[1]) ==CARD_VALUE_K )) 
	{
		return CT_KING_TONG_HUA_SHUN;
	}

	//顺子类型
	if ((cbSameColor==false) && (bLineCard==true)) 
	{
		return CT_SHUN_ZI;
	}

	//同花类型
	if ((cbSameColor==true) && (bLineCard==false)) 
	{
		return CT_TONG_HUA;
	}

	//同花顺类型
	if ((cbSameColor==true) && (bLineCard==true))
	{
		return CT_TONG_HUA_SHUN;
	}

	//扑克分析
	tagAnalyseResult AnalyseResult;
	AnalysebCardData(cbCardData,cbCardCount,AnalyseResult);

	//类型判断
	if (1==AnalyseResult.cbFourCount) 
	{
		return CT_TIE_ZHI;
	}
	if (2==AnalyseResult.cbLONGCount) 
	{
		return CT_TWO_LONG;
	}
	if ((1==AnalyseResult.cbLONGCount) && (1==AnalyseResult.cbThreeCount))
	{
		return CT_HU_LU;
	}
	if ((1==AnalyseResult.cbThreeCount) && (0==AnalyseResult.cbLONGCount))
	{
		return CT_THREE_TIAO;
	}
	if ((1==AnalyseResult.cbLONGCount) && (3==AnalyseResult.cbSignedCount)) 
	{
		return CT_ONE_LONG;
	}

	return CT_SINGLE;
}
//整理最大牌型
BYTE CGameLogic::FinishCard(BYTE cbEndCardData[],BYTE cbEndCardData1[],BYTE cbHandCardCount1,BYTE cbHandData[],BYTE Type)
{
	//扑克分析
	tagAnalyseResult AnalyseResult;
	AnalysebCardData(cbEndCardData,cbHandCardCount1,AnalyseResult);

	switch(Type)
	{		
	case CT_SINGLE://单张
		{				
			cbEndCardData1[0] = cbHandData[0]>cbHandData[1]?cbHandData[0] : cbHandData[1];
			break;
		}
	case CT_ONE_LONG://1对
		{				
			cbEndCardData1[0] = AnalyseResult.cbLONGLogicVolue[0];
			if(GetCardValue(cbEndCardData1[0])!=GetCardValue(cbHandData[0]) && GetCardValue(cbEndCardData1[0])!=GetCardValue(cbHandData[1]))
			{
				cbEndCardData1[1] = cbHandData[0]>cbHandData[1]?cbHandData[0] : cbHandData[1];
			}
			break;
		}
	case CT_TWO_LONG://2对
		{				
			cbEndCardData1[0] = AnalyseResult.cbLONGLogicVolue[0];
			cbEndCardData1[1] = AnalyseResult.cbLONGLogicVolue[1];
			if(GetCardValue(cbEndCardData1[0])!=GetCardValue(cbHandData[0]) && GetCardValue(cbEndCardData1[0])!=GetCardValue(cbHandData[1]))
			{
				if(GetCardValue(cbEndCardData1[1])!=GetCardValue(cbHandData[0]) && GetCardValue(cbEndCardData1[1])!=GetCardValue(cbHandData[1]))
				{
					cbEndCardData1[2] = cbHandData[0]>cbHandData[1]?cbHandData[0] : cbHandData[1];
				}
			}
			break;
		}
	case CT_THREE_TIAO://3条
		{				
			cbEndCardData1[0] = AnalyseResult.cbThreeLogicVolue[0];		
			if(GetCardValue(cbEndCardData1[0])!=GetCardValue(cbHandData[0]) && GetCardValue(cbEndCardData1[0])!=GetCardValue(cbHandData[1]))
			{
				cbEndCardData1[1] = cbHandData[0]>cbHandData[1]?cbHandData[0] : cbHandData[1];
			}
			break;
		}
	case CT_HU_LU: //葫芦
		{				
			cbEndCardData1[0] = AnalyseResult.cbThreeLogicVolue[0];
			cbEndCardData1[1] = AnalyseResult.cbLONGLogicVolue[0];
			if(GetCardValue(cbEndCardData1[0])!=GetCardValue(cbHandData[0]) && GetCardValue(cbEndCardData1[0])!=GetCardValue(cbHandData[1]))
			{
				if(GetCardValue(cbEndCardData1[1])!=GetCardValue(cbHandData[0]) && GetCardValue(cbEndCardData1[1])!=GetCardValue(cbHandData[1]))
				{
					cbEndCardData1[2] = cbHandData[0]>cbHandData[1]?cbHandData[0] : cbHandData[1];
				}
			}
			break;
		}
	case CT_TIE_ZHI://铁枝
		{
			cbEndCardData1[0] = AnalyseResult.cbFourLogicVolue[0];	
			if(GetCardValue(cbEndCardData1[0])!=GetCardValue(cbHandData[0]) && GetCardValue(cbEndCardData1[0])!=GetCardValue(cbHandData[1]))
			{
				cbEndCardData1[1] = cbHandData[0]>cbHandData[1]?cbHandData[0] : cbHandData[1];
			}
			break;
		}		
	}
	return 0;
};

//最大牌型
BYTE CGameLogic::FiveFromSeven(BYTE cbHandCardData[],BYTE cbHandCardCount,BYTE cbCenterCardData[],BYTE cbCenterCardCount,BYTE cbLastCardData[],BYTE cbLastCardCount)
{
	//临时变量
	BYTE cbTempCardData[MAX_COUNT+MAX_CENTERCOUNT]={0};
	BYTE cbTempLastCardData[5]={0};

	//拷贝数据
	CopyMemory(cbTempCardData,cbHandCardData,sizeof(BYTE)*MAX_COUNT);
	CopyMemory(&cbTempCardData[2],cbCenterCardData,sizeof(BYTE)*MAX_CENTERCOUNT);

	//排列扑克
	SortCardList(cbTempCardData,CountArray(cbTempCardData));
	CopyMemory(cbLastCardData,cbTempCardData,sizeof(BYTE)*MAX_CENTERCOUNT);
	BYTE cbCardKind = GetCardType(cbLastCardData,sizeof(BYTE)*MAX_CENTERCOUNT);
	BYTE cbTempCardKind = 0;
	//组合算法
	for (int i=0;i<3;i++)
	{
		for (int j= i+1;j<4;j++)
		{
			for (int k = j+1;k<5;k++)
			{
				for (int m =k+1;m<6;m++)
				{
					for (int n=m+1;n<7;n++)
					{
						//获取数据
						cbTempLastCardData[0]=cbTempCardData[i];
						cbTempLastCardData[1]=cbTempCardData[j];
						cbTempLastCardData[2]=cbTempCardData[k];
						cbTempLastCardData[3]=cbTempCardData[m];
						cbTempLastCardData[4]=cbTempCardData[n];
						//获取牌型
						cbTempCardKind = GetCardType(cbTempLastCardData,CountArray(cbTempLastCardData));
						//牌型大小
						if(CompareCard(cbTempLastCardData,cbLastCardData,CountArray(cbTempLastCardData))==2)
						{
							cbCardKind = cbTempCardKind;
							CopyMemory(cbLastCardData,cbTempLastCardData,sizeof(BYTE)*CountArray(cbTempLastCardData));
						}
					}
				}
			}
		}
	}
	return cbCardKind;
}

//根据牌型获得能够达到这种牌型需要多少张数
BYTE CGameLogic::GetFiveFromSeven(BYTE cbHandCardData[],BYTE cbHandCardCount,BYTE cbCenterCardData[],BYTE cbCenterCardCount,BYTE cbLastCardData[],BYTE CardType)
{
	//临时变量
	BYTE cbTempCardData[MAX_COUNT+MAX_CENTERCOUNT]={0};
	BYTE cbTempLastCardData[5]={0};

	//拷贝数据
	if(cbHandCardCount>0)
	{
		CopyMemory(cbTempCardData,cbHandCardData,sizeof(BYTE)*MAX_COUNT);
		CopyMemory(&cbTempCardData[2],cbCenterCardData,sizeof(BYTE)*MAX_CENTERCOUNT);
	}
	else
	{
		CopyMemory(cbTempCardData, cbCenterCardData, sizeof(BYTE)*MAX_CENTERCOUNT);
	}
	//排列扑克
	SortCardList(cbTempCardData,CountArray(cbTempCardData));

	//CopyMemory(cbLastCardData,cbTempCardData,sizeof(BYTE)*MAX_CENTERCOUNT);
	BYTE MaxCount = 0;	
	BYTE cbTempCardKind = 0;
	//组合算法
	for (int i=0;i<3;i++)
	{		
		for (int j= i+1;j<4;j++)
		{
			for (int k = j+1;k<5;k++)
			{
				for (int m =k+1;m<6;m++)
				{					
					for (int n=m+1;n<7;n++)
					{
						//获取数据
						cbTempLastCardData[0]=cbTempCardData[i];
						cbTempLastCardData[1]=cbTempCardData[j];
						cbTempLastCardData[2]=cbTempCardData[k];
						cbTempLastCardData[3]=cbTempCardData[m];
						cbTempLastCardData[4]=cbTempCardData[n];
						bool bZero = false;
						for(int b=0;b<5;b++)
						{
							if(cbTempLastCardData[b] == 0)
							{
								bZero = true;
								break;
							}
						}
						if(bZero)
						{
							break;
						}
						//获取牌型
						cbTempCardKind = GetCardType(cbTempLastCardData,CountArray(cbTempLastCardData));
						BYTE buffCount = 0;
						BYTE LianCard[7] = {0};
						if(CT_MAXTYPE == CardType)
						{
							if(MaxCount<cbTempCardKind)
							{
								MaxCount = cbTempCardKind;
								CopyMemory(cbLastCardData,cbTempLastCardData,sizeof(BYTE)*5);
							}
						}
						else if(cbTempCardKind != CardType || CardType == CT_ONE_LONG && (cbTempCardKind==CT_ONE_LONG))
						{
							if(CardType == CT_TONG_HUA) //同花
							{								
								for(int b=0; b<5; b++)
								{
									memset(LianCard,0,sizeof(LianCard));
									BYTE buffCount2 = 0;
									if(cbTempLastCardData[b]==0)continue;
									BYTE cbFirstColor = GetCardColor(cbTempLastCardData[b]);
									for(int f=0; f<5; f++)
									{									
										BYTE cbSecColor = GetCardColor(cbTempLastCardData[f]);
										if(cbSecColor == cbFirstColor)//如果是同花
										{
											LianCard[buffCount2++]=cbTempLastCardData[f];											
										}
									}
									if(buffCount2>buffCount && buffCount2>1) 
									{	
										buffCount = buffCount2;		
										MaxCount = buffCount;
										CopyMemory(cbLastCardData,LianCard,sizeof(BYTE)*buffCount);
									}										
								}								
							}
							else if(CardType == CT_SHUN_ZI) //顺子
							{
								for(int b=0; b<5; b++)
								{
									memset(LianCard,0,sizeof(LianCard));
									BYTE buffCount2 = 0;
									BYTE cbFirstValue = GetCardLogicValue(cbTempLastCardData[b]);
									BYTE Count =0;
									int f = 0;
									for(f=b+1; f<5; f++)
									{									
										BYTE cbSecValue = GetCardLogicValue(cbTempLastCardData[f]);
										if(cbSecValue == cbFirstValue) continue;
										Count++;
										if(cbSecValue+1 == cbFirstValue)//如果是顺子
										{
											cbFirstValue = cbSecValue;
											LianCard[buffCount2++]=cbTempLastCardData[f];	
										}
										else
										{
											break;
										}
									}	
									if(buffCount2+1>=2 && buffCount2+1!=5)
									{
										if(b>0)
										{
											cbFirstValue = GetCardLogicValue(cbTempLastCardData[b]);
											for(int G=0; G<b; G++)
											{
												BYTE cbSecValue = GetCardLogicValue(cbTempLastCardData[G]);
												if(cbFirstValue+2==cbSecValue)
												{
													LianCard[buffCount2++]=cbTempLastCardData[G];	
												}
											}
										}
										if(buffCount2<5)
										{
											if(f<4)
											{
												cbFirstValue = GetCardLogicValue(LianCard[buffCount2]);
												for(int G=f; G<5; G++)
												{
													BYTE cbSecValue = GetCardLogicValue(cbTempLastCardData[G]);
													if(cbFirstValue-2==cbSecValue)
													{
														LianCard[buffCount2++]=cbTempLastCardData[G];	
													}
												}
											}
										}
									}
									if(buffCount2+1>buffCount)
									{
										LianCard[buffCount2++]=cbTempLastCardData[b];	

										buffCount = buffCount2;
										if(MaxCount<buffCount)
										{
											MaxCount = buffCount;
											CopyMemory(cbLastCardData,LianCard,sizeof(BYTE)*buffCount);												
										}											
									}
									if(CARD_VALUE_A==cbFirstValue)//如果是A
									{
										buffCount2 = 0;
										Count = 0;
										for(int f=1; f<5; f++)
										{									
											BYTE cbSecValue = GetCardLogicValue(cbTempLastCardData[5-f]);
											if(cbSecValue-f == 1)//如果是顺子
											{
												LianCard[buffCount2++]=cbTempLastCardData[f];	
											}
										}
										if(buffCount2+1>buffCount)
										{
											LianCard[buffCount2++]=cbTempLastCardData[b];	
											buffCount = buffCount2;
											if(MaxCount<buffCount)
											{
												MaxCount = buffCount;
												CopyMemory(cbLastCardData,LianCard,sizeof(BYTE)*buffCount);
											}										
										}
									}
								}	
							}
							else if(CardType == CT_ONE_LONG && (cbTempCardKind==CT_ONE_LONG))//对子
							{									
								for(int b=0; b<5; b++)
								{
									memset(LianCard,0,sizeof(LianCard));
									BYTE cbFirstValue = GetCardLogicValue(cbTempLastCardData[b]);
									if(cbFirstValue == 0)
									{
										break;
									}
									for(int f=b+1; f<5; f++)
									{		
										if(cbTempLastCardData[f]==0)
										{
											break;
										}
										BYTE cbSecValue = GetCardLogicValue(cbTempLastCardData[f]);
										if(cbSecValue == cbFirstValue)//如果是对子
										{												
											MaxCount = 2;
											cbLastCardData[0] = cbTempLastCardData[b];
											cbLastCardData[1] = cbTempLastCardData[f];	
											return 2;												
										}
									}										
								}
							}
						}
						else
						{	
							if(CardType != CT_ONE_LONG)
							{							
								MaxCount = 5;
								CopyMemory(cbLastCardData,cbTempLastCardData,sizeof(BYTE)*5);
								return MaxCount;
							}							
						}
					}
				}
			}
		}
	}
	return MaxCount;
}

//牌型大小排序
bool CGameLogic::SelectCardPaiXu(BYTE bCardData[GAME_PLAYER][MAX_CENTERCOUNT],BYTE wchair[GAME_PLAYER][MAX_CENTERCOUNT],BYTE wCount[])
{
	//清理数据
	ZeroMemory(wchair,sizeof(wchair));
	CString BB1;
	//First数据
	WORD wWinnerID=INVALID_CHAIR;
	BYTE i=0;
	for (;i<m_wLogicCount;i++)
	{
		if(bCardData[i][0]!=0)
		{						
			wWinnerID=i;
			break;
		}
	}

	//过滤全零
	if(i==GAME_PLAYER)return false;

	//查找最大用户
	WORD wTemp = wWinnerID;
	for(WORD i=1;i<GAME_PLAYER;i++)
	{
		WORD j=(i+wTemp)%GAME_PLAYER;
		if(bCardData[j][0]==0)continue;		
		if(CompareCard(bCardData[j],bCardData[wWinnerID],MAX_CENTERCOUNT)>1)
		{			
			wWinnerID=j;

		}
	}	
	return true;
}
//比较两牌大小
BYTE CGameLogic::SelectTwoCard(BYTE Vale1[],BYTE Vale2[])
{
	BYTE Max1[2]={0,0},Max2[2]={0,0};

	if(GetCardLogicValue(Vale1[0])>GetCardLogicValue(Vale1[1]))
	{	
		Max1[0] =GetCardLogicValue(Vale1[0]);
		Max1[1] =GetCardLogicValue(Vale1[1]);

	}
	else
	{
		Max1[0] =GetCardLogicValue(Vale1[1]);
		Max1[1] =GetCardLogicValue(Vale1[0]);
	}
	if(GetCardLogicValue(Vale2[0])>GetCardLogicValue(Vale2[1]))
	{	
		Max2[0] =GetCardLogicValue(Vale2[0]);
		Max2[1] =GetCardLogicValue(Vale2[1]);
	}
	else
	{
		Max2[0] =GetCardLogicValue(Vale2[1]);
		Max2[1] =GetCardLogicValue(Vale2[0]);
	}
	if(Max1[0]>Max2[0])
	{
		return 2;
	}
	else if(Max1[0]<Max2[0])
	{
		return 1;
	}
	else
	{
		if(Max1[1]>Max2[1])
		{
			return 2;
		}
		else if(Max1[1]<Max2[1])
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
}

//查找最大
bool CGameLogic::SelectMaxUser(BYTE bCardData[GAME_PLAYER][MAX_CENTERCOUNT],UserWinList &EndResult,BYTE bHandData[GAME_PLAYER][2],const LONG lm_AddScore[])
{
	//清理数据
	ZeroMemory(&EndResult,sizeof(EndResult));

	//First数据
	WORD wWinnerID=INVALID_CHAIR;
	BYTE i=0;
	for (;i<m_wLogicCount;i++)
	{
		if(bCardData[i][0]!=0)
		{						
			wWinnerID=i;
			break;
		}
	}
	//过滤全零
	if(i==GAME_PLAYER)return false;

	BYTE MaxHandCard[GAME_PLAYER];
	memset(MaxHandCard,0,sizeof(MaxHandCard));
	/*for(WORD i=0;i<GAME_PLAYER;i++)
	{
	if(SelectTwoCard(bHandData[i][0],bHandData[i][1]))
	{
	MaxHandCard[i] = bHandData[i][0];
	}else
	{
	MaxHandCard[i] = bHandData[i][1];
	}
	}*/
	//查找最大用户
	WORD wTemp = wWinnerID;
	for(WORD i=1;i<GAME_PLAYER;i++)
	{
		WORD j=(i+wTemp)%GAME_PLAYER;
		if(bCardData[j][0]==0)continue;		
		if(CompareCard(bCardData[j],bCardData[wWinnerID],MAX_CENTERCOUNT)>1)
		{			
			wWinnerID=j;
		}else if(CompareCard(bCardData[j],bCardData[wWinnerID],MAX_CENTERCOUNT)<=0)
		{
			if(SelectTwoCard(bHandData[j],bHandData[wWinnerID])>1)
			{
				wWinnerID=j;
			}
		}
	}	
	//查找相同数据
	EndResult.wWinerList[EndResult.bSameCount++] = wWinnerID;
	for(WORD i=0;i<m_wLogicCount;i++)
	{
		if(i==wWinnerID || bCardData[i][0]==0)continue;
		if(CompareCard(bCardData[i],bCardData[wWinnerID],MAX_CENTERCOUNT)==FALSE)
		{
			/*if(SelectTwoCard(bHandData[i],bHandData[wWinnerID])==0)
			{*/
			EndResult.wWinerList[EndResult.bSameCount++] = i;
			//}			
		}
	}

	//从小到大
	if(EndResult.bSameCount>1 && lm_AddScore!=NULL)
	{
		int iSameCount=(int)EndResult.bSameCount;
		while((iSameCount--)>0)
		{
			LONG lTempSocre = lm_AddScore[EndResult.wWinerList[iSameCount]];
			for(int i=iSameCount-1;i>=0;i--)
			{
				ASSERT(lm_AddScore[EndResult.wWinerList[i]]>0);
				if(lTempSocre < lm_AddScore[EndResult.wWinerList[i]])
				{
					lTempSocre = lm_AddScore[EndResult.wWinerList[i]];
					WORD wTemp = EndResult.wWinerList[iSameCount];
					EndResult.wWinerList[iSameCount] = EndResult.wWinerList[i];
					EndResult.wWinerList[i] = wTemp;
				}
			}
		}
	}
	return true;
}

//排序
void CGameLogic::SortValueList(LONG cbValue[], BYTE ID[], BYTE cbCardCount)
{	
	bool bSorted=true;
	LONG cbTempData = 0,bLast=cbCardCount-1;
	do
	{
		bSorted=true;
		for (BYTE i=0;i<bLast;i++)
		{
			if (cbValue[i]>=cbValue[i+1])
			{
				//交换位置
				cbTempData=cbValue[i];
				cbValue[i]=cbValue[i+1];
				cbValue[i+1]=cbTempData;
				BYTE Idbuffer = ID[i] ;
				ID[i]         = ID[i+1];
				ID[i+1]       = Idbuffer;
				bSorted=false;
			}	
		}
		bLast--;
	} while(bSorted==false);
}
//排列扑克
void CGameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount)
{
	//转换数值
	BYTE cbLogicValue[FULL_COUNT];
	for (BYTE i=0;i<cbCardCount;i++) 
		cbLogicValue[i]=GetCardLogicValue(cbCardData[i]);	

	//排序操作
	bool bSorted=true;
	BYTE cbTempData,bLast=cbCardCount-1;
	do
	{
		bSorted=true;
		for (BYTE i=0;i<bLast;i++)
		{
			if ((cbLogicValue[i]<cbLogicValue[i+1]) || 
				((cbLogicValue[i]==cbLogicValue[i+1]) && (cbCardData[i]<cbCardData[i+1])))
			{
				//交换位置
				cbTempData=cbCardData[i];
				cbCardData[i]=cbCardData[i+1];
				cbCardData[i+1]=cbTempData;
				cbTempData=cbLogicValue[i];
				cbLogicValue[i]=cbLogicValue[i+1];
				cbLogicValue[i+1]=cbTempData;
				bSorted=false;
			}	
		}
		bLast--;
	} while(bSorted==false);

	return;
}
//逻辑数值
BYTE CGameLogic::GetCardLogicValue1(BYTE cbCardData[],BYTE CardType[])
{
	BYTE bCardType=0;
	if(GetCardColor(cbCardData[0])==GetCardColor(cbCardData[1]))
		CardType[0]=2;
	BYTE FirstCard=GetCardValue(cbCardData[0]),NextCard=GetCardValue(cbCardData[1]);

	if(FirstCard==NextCard)
	{
		CardType[0]=1;
		CardType[1]=GetCardValue(cbCardData[0]);
	}else
	{
		if(CardType[0]!=2) CardType[0]=0;		
		CardType[1]=FirstCard>NextCard?FirstCard:NextCard;
	}
	return bCardType;
}


//逻辑数值
BYTE CGameLogic::GetCardLogicValue(BYTE cbCardData)
{
	//扑克属性
	BYTE bCardValue=GetCardValue(cbCardData);

	//转换数值
	return (1==bCardValue)?(bCardValue+13):bCardValue;
}

//对比扑克
BYTE CGameLogic::CompareCard(BYTE cbFirstData[], BYTE cbNextData[], BYTE cbCardCount)
{
	//获取类型
	BYTE cbNextType=GetCardType(cbNextData,cbCardCount);
	BYTE cbFirstType=GetCardType(cbFirstData,cbCardCount);

	//类型判断
	//大
	if(cbFirstType>cbNextType)
		return 2;

	//小
	if(cbFirstType<cbNextType)
		return 1;

	//简单类型
	switch(cbFirstType)
	{
	case CT_SINGLE:			//单牌
		{
			//对比数值
			BYTE i=0;
			for (i=0;i<cbCardCount;i++)
			{
				BYTE cbNextValue=GetCardLogicValue(cbNextData[i]);
				BYTE cbFirstValue=GetCardLogicValue(cbFirstData[i]);

				//大
				if(cbFirstValue > cbNextValue)
					return 2;
				//小
				else if(cbFirstValue <cbNextValue)
					return 1;
				//等
				else
					continue;
			}
			//平
			if( i == cbCardCount)
				return 0;
			ASSERT(FALSE);
		}
	case CT_ONE_LONG:		//对子
	case CT_TWO_LONG:		//两对
	case CT_THREE_TIAO:		//三条
	case CT_TIE_ZHI:		//铁支
	case CT_HU_LU:			//葫芦
		{
			//分析扑克
			tagAnalyseResult AnalyseResultNext;
			tagAnalyseResult AnalyseResultFirst;
			AnalysebCardData(cbNextData,cbCardCount,AnalyseResultNext);
			AnalysebCardData(cbFirstData,cbCardCount,AnalyseResultFirst);

			//四条数值
			if (AnalyseResultFirst.cbFourCount>0)
			{
				BYTE cbNextValue=AnalyseResultNext.cbFourLogicVolue[0];
				BYTE cbFirstValue=AnalyseResultFirst.cbFourLogicVolue[0];

				//比较四条
				if(cbFirstValue != cbNextValue)return (cbFirstValue > cbNextValue)?2:1;

				//比较单牌
				ASSERT(1==AnalyseResultFirst.cbSignedCount && 1==AnalyseResultNext.cbSignedCount);
				cbFirstValue = AnalyseResultFirst.cbSignedLogicVolue[0];
				cbNextValue = AnalyseResultNext.cbSignedLogicVolue[0];
				if(cbFirstValue != cbNextValue)return (cbFirstValue > cbNextValue)?2:1;
				else return 0;
			}

			//三条数值
			if (AnalyseResultFirst.cbThreeCount>0)
			{
				BYTE cbNextValue=AnalyseResultNext.cbThreeLogicVolue[0];
				BYTE cbFirstValue=AnalyseResultFirst.cbThreeLogicVolue[0];

				//比较三条
				if(cbFirstValue != cbNextValue)return (cbFirstValue > cbNextValue)?2:1;

				//葫芦牌型
				if(CT_HU_LU == cbFirstType)
				{
					//比较对牌
					ASSERT(1==AnalyseResultFirst.cbLONGCount && 1==AnalyseResultNext.cbLONGCount);
					cbFirstValue = AnalyseResultFirst.cbLONGLogicVolue[0];
					cbNextValue = AnalyseResultNext.cbLONGLogicVolue[0];
					if(cbFirstValue != cbNextValue)return (cbFirstValue > cbNextValue)?2:1;
					else return 0;
				}
				else //三条带单
				{
					//比较单牌
					ASSERT(2==AnalyseResultFirst.cbSignedCount && 2==AnalyseResultNext.cbSignedCount);

					//散牌数值
					BYTE i=0;
					for (i=0;i<AnalyseResultFirst.cbSignedCount;i++)
					{
						BYTE cbNextValue=AnalyseResultNext.cbSignedLogicVolue[i];
						BYTE cbFirstValue=AnalyseResultFirst.cbSignedLogicVolue[i];
						//大
						if(cbFirstValue > cbNextValue)
							return 2;
						//小
						else if(cbFirstValue <cbNextValue)
							return 1;
						//等
						else continue;
					}
					if( i == AnalyseResultFirst.cbSignedCount)
						return 0;
					ASSERT(FALSE);
				}
			}

			//对子数值
			BYTE i=0;
			for ( i=0;i<AnalyseResultFirst.cbLONGCount;i++)
			{
				BYTE cbNextValue=AnalyseResultNext.cbLONGLogicVolue[i];
				BYTE cbFirstValue=AnalyseResultFirst.cbLONGLogicVolue[i];
				//大
				if(cbFirstValue > cbNextValue)
					return 2;
				//小
				else if(cbFirstValue <cbNextValue)
					return 1;
				//平
				else
					continue;
			}

			//比较单牌
			ASSERT( i == AnalyseResultFirst.cbLONGCount);
			{
				ASSERT(AnalyseResultFirst.cbSignedCount==AnalyseResultNext.cbSignedCount
					&& AnalyseResultNext.cbSignedCount>0);
				//散牌数值
				for (i=0;i<AnalyseResultFirst.cbSignedCount;i++)
				{
					BYTE cbNextValue=AnalyseResultNext.cbSignedLogicVolue[i];
					BYTE cbFirstValue=AnalyseResultFirst.cbSignedLogicVolue[i];
					//大
					if(cbFirstValue > cbNextValue)
						return 2;
					//小
					else if(cbFirstValue <cbNextValue)
						return 1;
					//等
					else continue;
				}
				//平
				if( i == AnalyseResultFirst.cbSignedCount)
					return 0;
			}
			break;
		}
	case CT_SHUN_ZI:		//顺子
	case CT_TONG_HUA_SHUN:	//同花顺
		{
			//数值判断
			BYTE cbNextValue=GetCardLogicValue(cbNextData[0]);
			BYTE cbFirstValue=GetCardLogicValue(cbFirstData[0]);

			bool bFirstmin= (cbFirstValue ==(GetCardLogicValue(cbFirstData[1])+9));
			bool bNextmin= (cbNextValue ==(GetCardLogicValue(cbNextData[1])+9));

			//大小顺子
			if ((bFirstmin==true) && (bNextmin == false))
			{
				return 1;
			}
			//大小顺子
			else if ((bFirstmin==false) && (bNextmin == true))
			{
				return 2;
			}
			//等同顺子
			else
			{
				//平
				if(cbFirstValue == cbNextValue)return 0;		
				return (cbFirstValue > cbNextValue)?2:1;		
			}
		}
	case CT_TONG_HUA:		//同花
		{	
			//散牌数值
			BYTE i=0;
			for (;i<cbCardCount;i++)
			{
				BYTE cbNextValue=GetCardLogicValue(cbNextData[i]);
				BYTE cbFirstValue=GetCardLogicValue(cbFirstData[i]);

				if(cbFirstValue == cbNextValue)continue;
				return (cbFirstValue > cbNextValue)?2:1;
			}
			//平
			if( i == cbCardCount) return 0;
		}
	}

	return  0;
}

//分析扑克
void CGameLogic::AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult & AnalyseResult)
{
	//设置结果
	ZeroMemory(&AnalyseResult,sizeof(AnalyseResult));

	//扑克分析
	for (BYTE i=0;i<cbCardCount;i++)
	{
		//变量定义
		BYTE cbSameCount=1;
		BYTE cbSameCardData[4]={cbCardData[i],0,0,0};
		BYTE cbLogicValue=GetCardLogicValue(cbCardData[i]);

		//获取同牌
		for (int j=i+1;j<cbCardCount;j++)
		{
			//逻辑对比
			if (GetCardLogicValue(cbCardData[j])!=cbLogicValue) break;

			//设置扑克
			cbSameCardData[cbSameCount++]=cbCardData[j];
		}

		//保存结果
		switch (cbSameCount)
		{
		case 1:		//单张
			{
				AnalyseResult.cbSignedLogicVolue[AnalyseResult.cbSignedCount]=cbLogicValue;
				CopyMemory(&AnalyseResult.cbSignedCardData[(AnalyseResult.cbSignedCount++)*cbSameCount],cbSameCardData,cbSameCount);
				break;
			}
		case 2:		//两张
			{
				AnalyseResult.cbLONGLogicVolue[AnalyseResult.cbLONGCount]=cbLogicValue;
				CopyMemory(&AnalyseResult.cbLONGCardData[(AnalyseResult.cbLONGCount++)*cbSameCount],cbSameCardData,cbSameCount);
				break;
			}
		case 3:		//三张
			{
				AnalyseResult.cbThreeLogicVolue[AnalyseResult.cbThreeCount]=cbLogicValue;
				CopyMemory(&AnalyseResult.cbThreeCardData[(AnalyseResult.cbThreeCount++)*cbSameCount],cbSameCardData,cbSameCount);
				break;
			}
		case 4:		//四张
			{
				AnalyseResult.cbFourLogicVolue[AnalyseResult.cbFourCount]=cbLogicValue;
				CopyMemory(&AnalyseResult.cbFourCardData[(AnalyseResult.cbFourCount++)*cbSameCount],cbSameCardData,cbSameCount);
				break;
			}
		}

		//设置递增
		i+=cbSameCount-1;
	}

	return;
}


//读取配置文件
BOOL CGameLogic::SetZJHInit(void)
{
	CString strWorkDir;
	//模块路径
	LPTSTR pszBuf=strWorkDir.GetBuffer(MAX_PATH);
	GetModuleFileName(NULL,pszBuf,MAX_PATH);

	//移除文件名
	PathRemoveFileSpec(pszBuf);

	strWorkDir.ReleaseBuffer();

	m_strFileName = strWorkDir + TEXT("\\DZShowHandServer.ini");
	m_strFileName2 = strWorkDir + TEXT("%s\\DZPKSet.ini");

	//设置文件名
	ZeroMemory(m_lGiveUp, sizeof(m_lGiveUp));
	ZeroMemory(m_lAdd, sizeof(m_lAdd));
	ZeroMemory(m_lQuanYa, sizeof(m_lQuanYa));
	CString strKey;
	CString strApp;
	for ( int ID=0; ID<4; ID++ )
	{
		strApp.Format(TEXT("DZPK%d"),ID);	
		//放弃的配置
		for(int i=0; i<INT_ROBOT_GIVEUP; i++)
		{	
			strKey.Format(TEXT("Giveup%d"), i);
			m_lGiveUp[ID][i] = GetPrivateProfileInt(strApp, strKey, 0, m_strFileName);					
		}			
		//加注的配置
		for(int i=0; i<INT_ROBOT_ADD; i++)
		{
			strKey.Format(TEXT("Add%d"), i);
			m_lAdd[ID][i] = GetPrivateProfileInt(strApp, strKey, 0, m_strFileName);		

		}
		//全压和开牌的配置
		for(int i=0; i<INT_ROBOT_QUANYA; i++)
		{
			strKey.Format(TEXT("QUANYA%d"), i);
			m_lQuanYa[ID][i] = GetPrivateProfileInt(strApp, strKey, 0, m_strFileName);				
		}
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
