#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
LONG			CGameLogic::m_lQuanYa[4][INT_ROBOT_QUANYA]={0};		//������ȫѺ��	
LONG			CGameLogic::m_lGiveUp[4][INT_ROBOT_GIVEUP]={0};		//�����˷�����	
LONG			CGameLogic::m_lAdd[4][INT_ROBOT_ADD]={0};			//�����˼�ע��
const WORD		CGameLogic::m_wLogicCount=GAME_PLAYER;
CString 		CGameLogic::m_strFileName;
CString 		CGameLogic::m_strFileName2;

//�˿�����
BYTE CGameLogic::m_cbCardData[FULL_COUNT]={
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//���� A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//÷�� A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//���� A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D	//���� A - K
};

//���캯��
CGameLogic::CGameLogic()
{
}

//��������
CGameLogic::~CGameLogic()
{

}

//�����˿�
VOID CGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//���Դ���
	//CopyMemory(cbCardBuffer,m_cbCardData,cbBufferCount);
	//����׼��
	BYTE bCardCount = CountArray(m_cbCardData);
	BYTE cbCardData[CountArray(m_cbCardData)];
	CopyMemory(cbCardData,m_cbCardData,sizeof(m_cbCardData));

	//�����˿�
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

//��ȡ����
BYTE CGameLogic::GetCardType(BYTE cbCardData[], BYTE cbCardCount)
{
	//����У��
	//ASSERT(cbCardCount == MAX_CENTERCOUNT);
	//if(cbCardCount !=MAX_CENTERCOUNT) return 0;

	//��������
	bool cbSameColor=true;
	bool bLineCard=true;
	BYTE cbFirstColor=GetCardColor(cbCardData[0]);
	BYTE cbFirstValue=GetCardLogicValue(cbCardData[0]);

	//���η���
	for (BYTE i=1;i<cbCardCount;i++)
	{
		//���ݷ���
		if (GetCardColor(cbCardData[i])!=cbFirstColor) cbSameColor=false;
		if (cbFirstValue!=(GetCardLogicValue(cbCardData[i])+i))
		{
			bLineCard=false;
		}

		//�����ж�
		if ((cbSameColor==false) && (bLineCard==false))
		{
			break;
		}
	}

	//��Сͬ��˳
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

	//�ʼ�ͬ��˳
	if ((cbSameColor==true) && (bLineCard==true) && (GetCardLogicValue(cbCardData[1]) ==CARD_VALUE_K )) 
	{
		return CT_KING_TONG_HUA_SHUN;
	}

	//˳������
	if ((cbSameColor==false) && (bLineCard==true)) 
	{
		return CT_SHUN_ZI;
	}

	//ͬ������
	if ((cbSameColor==true) && (bLineCard==false)) 
	{
		return CT_TONG_HUA;
	}

	//ͬ��˳����
	if ((cbSameColor==true) && (bLineCard==true))
	{
		return CT_TONG_HUA_SHUN;
	}

	//�˿˷���
	tagAnalyseResult AnalyseResult;
	AnalysebCardData(cbCardData,cbCardCount,AnalyseResult);

	//�����ж�
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
//�����������
BYTE CGameLogic::FinishCard(BYTE cbEndCardData[],BYTE cbEndCardData1[],BYTE cbHandCardCount1,BYTE cbHandData[],BYTE Type)
{
	//�˿˷���
	tagAnalyseResult AnalyseResult;
	AnalysebCardData(cbEndCardData,cbHandCardCount1,AnalyseResult);

	switch(Type)
	{		
	case CT_SINGLE://����
		{				
			cbEndCardData1[0] = cbHandData[0]>cbHandData[1]?cbHandData[0] : cbHandData[1];
			break;
		}
	case CT_ONE_LONG://1��
		{				
			cbEndCardData1[0] = AnalyseResult.cbLONGLogicVolue[0];
			if(GetCardValue(cbEndCardData1[0])!=GetCardValue(cbHandData[0]) && GetCardValue(cbEndCardData1[0])!=GetCardValue(cbHandData[1]))
			{
				cbEndCardData1[1] = cbHandData[0]>cbHandData[1]?cbHandData[0] : cbHandData[1];
			}
			break;
		}
	case CT_TWO_LONG://2��
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
	case CT_THREE_TIAO://3��
		{				
			cbEndCardData1[0] = AnalyseResult.cbThreeLogicVolue[0];		
			if(GetCardValue(cbEndCardData1[0])!=GetCardValue(cbHandData[0]) && GetCardValue(cbEndCardData1[0])!=GetCardValue(cbHandData[1]))
			{
				cbEndCardData1[1] = cbHandData[0]>cbHandData[1]?cbHandData[0] : cbHandData[1];
			}
			break;
		}
	case CT_HU_LU: //��«
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
	case CT_TIE_ZHI://��֦
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

//�������
BYTE CGameLogic::FiveFromSeven(BYTE cbHandCardData[],BYTE cbHandCardCount,BYTE cbCenterCardData[],BYTE cbCenterCardCount,BYTE cbLastCardData[],BYTE cbLastCardCount)
{
	//��ʱ����
	BYTE cbTempCardData[MAX_COUNT+MAX_CENTERCOUNT]={0};
	BYTE cbTempLastCardData[5]={0};

	//��������
	CopyMemory(cbTempCardData,cbHandCardData,sizeof(BYTE)*MAX_COUNT);
	CopyMemory(&cbTempCardData[2],cbCenterCardData,sizeof(BYTE)*MAX_CENTERCOUNT);

	//�����˿�
	SortCardList(cbTempCardData,CountArray(cbTempCardData));
	CopyMemory(cbLastCardData,cbTempCardData,sizeof(BYTE)*MAX_CENTERCOUNT);
	BYTE cbCardKind = GetCardType(cbLastCardData,sizeof(BYTE)*MAX_CENTERCOUNT);
	BYTE cbTempCardKind = 0;
	//����㷨
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
						//��ȡ����
						cbTempLastCardData[0]=cbTempCardData[i];
						cbTempLastCardData[1]=cbTempCardData[j];
						cbTempLastCardData[2]=cbTempCardData[k];
						cbTempLastCardData[3]=cbTempCardData[m];
						cbTempLastCardData[4]=cbTempCardData[n];
						//��ȡ����
						cbTempCardKind = GetCardType(cbTempLastCardData,CountArray(cbTempLastCardData));
						//���ʹ�С
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

//�������ͻ���ܹ��ﵽ����������Ҫ��������
BYTE CGameLogic::GetFiveFromSeven(BYTE cbHandCardData[],BYTE cbHandCardCount,BYTE cbCenterCardData[],BYTE cbCenterCardCount,BYTE cbLastCardData[],BYTE CardType)
{
	//��ʱ����
	BYTE cbTempCardData[MAX_COUNT+MAX_CENTERCOUNT]={0};
	BYTE cbTempLastCardData[5]={0};

	//��������
	if(cbHandCardCount>0)
	{
		CopyMemory(cbTempCardData,cbHandCardData,sizeof(BYTE)*MAX_COUNT);
		CopyMemory(&cbTempCardData[2],cbCenterCardData,sizeof(BYTE)*MAX_CENTERCOUNT);
	}
	else
	{
		CopyMemory(cbTempCardData, cbCenterCardData, sizeof(BYTE)*MAX_CENTERCOUNT);
	}
	//�����˿�
	SortCardList(cbTempCardData,CountArray(cbTempCardData));

	//CopyMemory(cbLastCardData,cbTempCardData,sizeof(BYTE)*MAX_CENTERCOUNT);
	BYTE MaxCount = 0;	
	BYTE cbTempCardKind = 0;
	//����㷨
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
						//��ȡ����
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
						//��ȡ����
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
							if(CardType == CT_TONG_HUA) //ͬ��
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
										if(cbSecColor == cbFirstColor)//�����ͬ��
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
							else if(CardType == CT_SHUN_ZI) //˳��
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
										if(cbSecValue+1 == cbFirstValue)//�����˳��
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
									if(CARD_VALUE_A==cbFirstValue)//�����A
									{
										buffCount2 = 0;
										Count = 0;
										for(int f=1; f<5; f++)
										{									
											BYTE cbSecValue = GetCardLogicValue(cbTempLastCardData[5-f]);
											if(cbSecValue-f == 1)//�����˳��
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
							else if(CardType == CT_ONE_LONG && (cbTempCardKind==CT_ONE_LONG))//����
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
										if(cbSecValue == cbFirstValue)//����Ƕ���
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

//���ʹ�С����
bool CGameLogic::SelectCardPaiXu(BYTE bCardData[GAME_PLAYER][MAX_CENTERCOUNT],BYTE wchair[GAME_PLAYER][MAX_CENTERCOUNT],BYTE wCount[])
{
	//��������
	ZeroMemory(wchair,sizeof(wchair));
	CString BB1;
	//First����
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

	//����ȫ��
	if(i==GAME_PLAYER)return false;

	//��������û�
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
//�Ƚ����ƴ�С
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

//�������
bool CGameLogic::SelectMaxUser(BYTE bCardData[GAME_PLAYER][MAX_CENTERCOUNT],UserWinList &EndResult,BYTE bHandData[GAME_PLAYER][2],const LONG lm_AddScore[])
{
	//��������
	ZeroMemory(&EndResult,sizeof(EndResult));

	//First����
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
	//����ȫ��
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
	//��������û�
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
	//������ͬ����
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

	//��С����
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

//����
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
				//����λ��
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
//�����˿�
void CGameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount)
{
	//ת����ֵ
	BYTE cbLogicValue[FULL_COUNT];
	for (BYTE i=0;i<cbCardCount;i++) 
		cbLogicValue[i]=GetCardLogicValue(cbCardData[i]);	

	//�������
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
				//����λ��
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
//�߼���ֵ
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


//�߼���ֵ
BYTE CGameLogic::GetCardLogicValue(BYTE cbCardData)
{
	//�˿�����
	BYTE bCardValue=GetCardValue(cbCardData);

	//ת����ֵ
	return (1==bCardValue)?(bCardValue+13):bCardValue;
}

//�Ա��˿�
BYTE CGameLogic::CompareCard(BYTE cbFirstData[], BYTE cbNextData[], BYTE cbCardCount)
{
	//��ȡ����
	BYTE cbNextType=GetCardType(cbNextData,cbCardCount);
	BYTE cbFirstType=GetCardType(cbFirstData,cbCardCount);

	//�����ж�
	//��
	if(cbFirstType>cbNextType)
		return 2;

	//С
	if(cbFirstType<cbNextType)
		return 1;

	//������
	switch(cbFirstType)
	{
	case CT_SINGLE:			//����
		{
			//�Ա���ֵ
			BYTE i=0;
			for (i=0;i<cbCardCount;i++)
			{
				BYTE cbNextValue=GetCardLogicValue(cbNextData[i]);
				BYTE cbFirstValue=GetCardLogicValue(cbFirstData[i]);

				//��
				if(cbFirstValue > cbNextValue)
					return 2;
				//С
				else if(cbFirstValue <cbNextValue)
					return 1;
				//��
				else
					continue;
			}
			//ƽ
			if( i == cbCardCount)
				return 0;
			ASSERT(FALSE);
		}
	case CT_ONE_LONG:		//����
	case CT_TWO_LONG:		//����
	case CT_THREE_TIAO:		//����
	case CT_TIE_ZHI:		//��֧
	case CT_HU_LU:			//��«
		{
			//�����˿�
			tagAnalyseResult AnalyseResultNext;
			tagAnalyseResult AnalyseResultFirst;
			AnalysebCardData(cbNextData,cbCardCount,AnalyseResultNext);
			AnalysebCardData(cbFirstData,cbCardCount,AnalyseResultFirst);

			//������ֵ
			if (AnalyseResultFirst.cbFourCount>0)
			{
				BYTE cbNextValue=AnalyseResultNext.cbFourLogicVolue[0];
				BYTE cbFirstValue=AnalyseResultFirst.cbFourLogicVolue[0];

				//�Ƚ�����
				if(cbFirstValue != cbNextValue)return (cbFirstValue > cbNextValue)?2:1;

				//�Ƚϵ���
				ASSERT(1==AnalyseResultFirst.cbSignedCount && 1==AnalyseResultNext.cbSignedCount);
				cbFirstValue = AnalyseResultFirst.cbSignedLogicVolue[0];
				cbNextValue = AnalyseResultNext.cbSignedLogicVolue[0];
				if(cbFirstValue != cbNextValue)return (cbFirstValue > cbNextValue)?2:1;
				else return 0;
			}

			//������ֵ
			if (AnalyseResultFirst.cbThreeCount>0)
			{
				BYTE cbNextValue=AnalyseResultNext.cbThreeLogicVolue[0];
				BYTE cbFirstValue=AnalyseResultFirst.cbThreeLogicVolue[0];

				//�Ƚ�����
				if(cbFirstValue != cbNextValue)return (cbFirstValue > cbNextValue)?2:1;

				//��«����
				if(CT_HU_LU == cbFirstType)
				{
					//�Ƚ϶���
					ASSERT(1==AnalyseResultFirst.cbLONGCount && 1==AnalyseResultNext.cbLONGCount);
					cbFirstValue = AnalyseResultFirst.cbLONGLogicVolue[0];
					cbNextValue = AnalyseResultNext.cbLONGLogicVolue[0];
					if(cbFirstValue != cbNextValue)return (cbFirstValue > cbNextValue)?2:1;
					else return 0;
				}
				else //��������
				{
					//�Ƚϵ���
					ASSERT(2==AnalyseResultFirst.cbSignedCount && 2==AnalyseResultNext.cbSignedCount);

					//ɢ����ֵ
					BYTE i=0;
					for (i=0;i<AnalyseResultFirst.cbSignedCount;i++)
					{
						BYTE cbNextValue=AnalyseResultNext.cbSignedLogicVolue[i];
						BYTE cbFirstValue=AnalyseResultFirst.cbSignedLogicVolue[i];
						//��
						if(cbFirstValue > cbNextValue)
							return 2;
						//С
						else if(cbFirstValue <cbNextValue)
							return 1;
						//��
						else continue;
					}
					if( i == AnalyseResultFirst.cbSignedCount)
						return 0;
					ASSERT(FALSE);
				}
			}

			//������ֵ
			BYTE i=0;
			for ( i=0;i<AnalyseResultFirst.cbLONGCount;i++)
			{
				BYTE cbNextValue=AnalyseResultNext.cbLONGLogicVolue[i];
				BYTE cbFirstValue=AnalyseResultFirst.cbLONGLogicVolue[i];
				//��
				if(cbFirstValue > cbNextValue)
					return 2;
				//С
				else if(cbFirstValue <cbNextValue)
					return 1;
				//ƽ
				else
					continue;
			}

			//�Ƚϵ���
			ASSERT( i == AnalyseResultFirst.cbLONGCount);
			{
				ASSERT(AnalyseResultFirst.cbSignedCount==AnalyseResultNext.cbSignedCount
					&& AnalyseResultNext.cbSignedCount>0);
				//ɢ����ֵ
				for (i=0;i<AnalyseResultFirst.cbSignedCount;i++)
				{
					BYTE cbNextValue=AnalyseResultNext.cbSignedLogicVolue[i];
					BYTE cbFirstValue=AnalyseResultFirst.cbSignedLogicVolue[i];
					//��
					if(cbFirstValue > cbNextValue)
						return 2;
					//С
					else if(cbFirstValue <cbNextValue)
						return 1;
					//��
					else continue;
				}
				//ƽ
				if( i == AnalyseResultFirst.cbSignedCount)
					return 0;
			}
			break;
		}
	case CT_SHUN_ZI:		//˳��
	case CT_TONG_HUA_SHUN:	//ͬ��˳
		{
			//��ֵ�ж�
			BYTE cbNextValue=GetCardLogicValue(cbNextData[0]);
			BYTE cbFirstValue=GetCardLogicValue(cbFirstData[0]);

			bool bFirstmin= (cbFirstValue ==(GetCardLogicValue(cbFirstData[1])+9));
			bool bNextmin= (cbNextValue ==(GetCardLogicValue(cbNextData[1])+9));

			//��С˳��
			if ((bFirstmin==true) && (bNextmin == false))
			{
				return 1;
			}
			//��С˳��
			else if ((bFirstmin==false) && (bNextmin == true))
			{
				return 2;
			}
			//��ͬ˳��
			else
			{
				//ƽ
				if(cbFirstValue == cbNextValue)return 0;		
				return (cbFirstValue > cbNextValue)?2:1;		
			}
		}
	case CT_TONG_HUA:		//ͬ��
		{	
			//ɢ����ֵ
			BYTE i=0;
			for (;i<cbCardCount;i++)
			{
				BYTE cbNextValue=GetCardLogicValue(cbNextData[i]);
				BYTE cbFirstValue=GetCardLogicValue(cbFirstData[i]);

				if(cbFirstValue == cbNextValue)continue;
				return (cbFirstValue > cbNextValue)?2:1;
			}
			//ƽ
			if( i == cbCardCount) return 0;
		}
	}

	return  0;
}

//�����˿�
void CGameLogic::AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult & AnalyseResult)
{
	//���ý��
	ZeroMemory(&AnalyseResult,sizeof(AnalyseResult));

	//�˿˷���
	for (BYTE i=0;i<cbCardCount;i++)
	{
		//��������
		BYTE cbSameCount=1;
		BYTE cbSameCardData[4]={cbCardData[i],0,0,0};
		BYTE cbLogicValue=GetCardLogicValue(cbCardData[i]);

		//��ȡͬ��
		for (int j=i+1;j<cbCardCount;j++)
		{
			//�߼��Ա�
			if (GetCardLogicValue(cbCardData[j])!=cbLogicValue) break;

			//�����˿�
			cbSameCardData[cbSameCount++]=cbCardData[j];
		}

		//������
		switch (cbSameCount)
		{
		case 1:		//����
			{
				AnalyseResult.cbSignedLogicVolue[AnalyseResult.cbSignedCount]=cbLogicValue;
				CopyMemory(&AnalyseResult.cbSignedCardData[(AnalyseResult.cbSignedCount++)*cbSameCount],cbSameCardData,cbSameCount);
				break;
			}
		case 2:		//����
			{
				AnalyseResult.cbLONGLogicVolue[AnalyseResult.cbLONGCount]=cbLogicValue;
				CopyMemory(&AnalyseResult.cbLONGCardData[(AnalyseResult.cbLONGCount++)*cbSameCount],cbSameCardData,cbSameCount);
				break;
			}
		case 3:		//����
			{
				AnalyseResult.cbThreeLogicVolue[AnalyseResult.cbThreeCount]=cbLogicValue;
				CopyMemory(&AnalyseResult.cbThreeCardData[(AnalyseResult.cbThreeCount++)*cbSameCount],cbSameCardData,cbSameCount);
				break;
			}
		case 4:		//����
			{
				AnalyseResult.cbFourLogicVolue[AnalyseResult.cbFourCount]=cbLogicValue;
				CopyMemory(&AnalyseResult.cbFourCardData[(AnalyseResult.cbFourCount++)*cbSameCount],cbSameCardData,cbSameCount);
				break;
			}
		}

		//���õ���
		i+=cbSameCount-1;
	}

	return;
}


//��ȡ�����ļ�
BOOL CGameLogic::SetZJHInit(void)
{
	CString strWorkDir;
	//ģ��·��
	LPTSTR pszBuf=strWorkDir.GetBuffer(MAX_PATH);
	GetModuleFileName(NULL,pszBuf,MAX_PATH);

	//�Ƴ��ļ���
	PathRemoveFileSpec(pszBuf);

	strWorkDir.ReleaseBuffer();

	m_strFileName = strWorkDir + TEXT("\\DZShowHandServer.ini");
	m_strFileName2 = strWorkDir + TEXT("%s\\DZPKSet.ini");

	//�����ļ���
	ZeroMemory(m_lGiveUp, sizeof(m_lGiveUp));
	ZeroMemory(m_lAdd, sizeof(m_lAdd));
	ZeroMemory(m_lQuanYa, sizeof(m_lQuanYa));
	CString strKey;
	CString strApp;
	for ( int ID=0; ID<4; ID++ )
	{
		strApp.Format(TEXT("DZPK%d"),ID);	
		//����������
		for(int i=0; i<INT_ROBOT_GIVEUP; i++)
		{	
			strKey.Format(TEXT("Giveup%d"), i);
			m_lGiveUp[ID][i] = GetPrivateProfileInt(strApp, strKey, 0, m_strFileName);					
		}			
		//��ע������
		for(int i=0; i<INT_ROBOT_ADD; i++)
		{
			strKey.Format(TEXT("Add%d"), i);
			m_lAdd[ID][i] = GetPrivateProfileInt(strApp, strKey, 0, m_strFileName);		

		}
		//ȫѹ�Ϳ��Ƶ�����
		for(int i=0; i<INT_ROBOT_QUANYA; i++)
		{
			strKey.Format(TEXT("QUANYA%d"), i);
			m_lQuanYa[ID][i] = GetPrivateProfileInt(strApp, strKey, 0, m_strFileName);				
		}
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
