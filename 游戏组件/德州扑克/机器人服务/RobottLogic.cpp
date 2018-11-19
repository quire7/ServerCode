#include "StdAfx.h"
#include "robottlogic.h"
#include "AndroidUserItemSink.h"
CRobottLogic::CRobottLogic(void)
{	
	OnClear();
}

CRobottLogic::~CRobottLogic(void)
{

}
//混乱扑克
void CRobottLogic::OnClear()
{
	memset(m_lBei,0,sizeof(m_lBei));
	memset(m_cbMyCardType,0,sizeof(m_cbMyCardType));
	ZeroMemory(m_cbEndCardData,sizeof(m_cbEndCardData));
	m_iBalanceCount = 0;	//平衡次数	
	m_iLastBalanceCnt = -1;	//上一轮的平衡次数
}
int CRobottLogic::SetRobotAlgorithm(CAndroidUserItemSink* _AndroidUserItemSink) 
{
	m_pAndroidUserItemSink = _AndroidUserItemSink;
	return 0;
}

int CRobottLogic::Robots(WORD wChairID, LONG lScore,BYTE ID)
{		
	if(m_iBalanceCount>m_iLastBalanceCnt || m_cbMyCardType[m_iBalanceCount]==0)//更换轮次
	{
		m_iLastBalanceCnt = m_iBalanceCount;
		m_cbRobotWin = RobotAlgorithm(wChairID,lScore,ID);
	}
	RobotsLostAlgorith(wChairID,lScore,m_cbRobotWin);
	return 0;
}
int CRobottLogic::RobotsLostAlgorith(WORD wChairID, LONG lScore,BYTE ID) 
{	
	srand(GetTickCount()+reinterpret_cast<UINT_PTR>(this));
	m_cbRobotsType =CHIP_TYPE_GIVEUP;
	m_cbRobotWin=ID;
	int iRobotsID =RobotsRate(m_cbMyCardType[m_iBalanceCount],0,wChairID);	
	int iRand=rand()%1000;
	if(iRand<iRobotsID)
	{
		m_cbRobotsType = CHIP_TYPE_GIVEUP;//放弃
		if(0==m_lTurnLessScore && m_lAddLessScore!=m_lBei[2]*m_lCellScore)
		{
			ASSERT((38 + m_iBalanceCount) < INT_ROBOT_GIVEUP);

			//让牌
			if (rand()%1000<m_lRobotGiveUp[38+m_iBalanceCount])
			{
				m_cbRobotsType=CHIP_TYPE_PASS;
			}
		}		
	
		m_lRobotsScore = 0;
	}
	else//不放弃
	{
		iRand =rand()%1000;
		iRobotsID =RobotsRate(m_cbMyCardType[m_iBalanceCount],2,wChairID);//算全押
		if(iRand<iRobotsID)//走全押
		{
			m_cbRobotsType = CHIP_TYPE_ALLIN;
			m_lRobotsScore = m_pAndroidUserItemSink->m_lTurnMaxScore[wChairID];
		}
		else
		{			
			iRand =rand()%1000;
			iRobotsID =RobotsRate(m_cbMyCardType[m_iBalanceCount],1,wChairID);//算加注率
			LONG lAddScore = m_lTurnLessScore>0?m_lTurnLessScore:m_lAddLessScore;
			LONG LXi = 0;
			if(iRand<iRobotsID)//走加注
			{
				if(m_cbRobotWin)//如果是5家最大牌减多少放弃率
				{
					LXi = rand()%m_lRobotAdd[26+(m_cbMyCardType[m_iBalanceCount]+2)/3];						
				}
				else//加多少筹码
				{				
					LXi = rand()%m_lRobotAdd[29+(m_cbMyCardType[m_iBalanceCount]+2)/3];
				}
				if(1==LXi)
				{
					lAddScore=lAddScore*3/2;
				}
				else if(2==LXi)
				{
					lAddScore*=3;
				}
				else if(3==LXi)
				{
					lAddScore*=4;
				}
				else
				{
					lAddScore*=2;
				}
				m_lRobotsScore = lAddScore;					
				m_cbRobotsType = CHIP_TYPE_ADD;
			}
			else//走跟让
			{				
				if(m_lTurnLessScore>0)//跟注
				{
					m_lRobotsScore =m_lTurnLessScore;
					m_cbRobotsType=CHIP_TYPE_GEN;
				}
				else //让牌
				{
					m_lRobotsScore = 0;
					m_cbRobotsType = CHIP_TYPE_PASS;
				}			
			}
		}
	}

	if(m_lRobotsScore>=m_pAndroidUserItemSink->m_lTurnMaxScore[wChairID] && m_cbRobotsType!=CHIP_TYPE_ALLIN)//如果超过自己的最大下注，则全压
	{
		m_cbRobotsType = CHIP_TYPE_ALLIN;
		m_lRobotsScore = m_pAndroidUserItemSink->m_lTurnMaxScore[wChairID];
	}

	int RobotsTimer=2;
	if ( CHIP_TYPE_GEN == m_cbRobotsType )
	{
		RobotsTimer = rand()%2 + 1;
	}
	else if ( CHIP_TYPE_ADD == m_cbRobotsType )
	{
		RobotsTimer = rand()%3 + 2;
	}
	else if ( CHIP_TYPE_ALLIN == m_cbRobotsType )
	{
		RobotsTimer=rand()%3+3;
	}
	else
	{
		RobotsTimer=rand()%3 + 1;
	}

	m_pAndroidUserItemSink->m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE, RobotsTimer);	
	return 0;
}
bool CRobottLogic::SetLevel(WORD wChairID) 
{
	BYTE cbMyCard[2] = {0};
	CopyMemory(cbMyCard, m_pAndroidUserItemSink->m_cbHandCardData[wChairID], sizeof(cbMyCard));
	const BYTE cbCardValue1=m_GameLogic.GetCardLogicValue(cbMyCard[0]);
	const BYTE cbCardValue2=m_GameLogic.GetCardLogicValue(cbMyCard[1]);
	if(m_iBalanceCount == 0)//第一轮
	{
		BYTE BcbCenterCardData[5] = {0};
		//CopyMemory(BcbCenterCardData,m_AndroidUserItemSink->m_cbCenterCardData,sizeof(BYTE)*3);
		BYTE BMaxData[7] = {0};
		BYTE CardTYPE = m_GameLogic.GetFiveFromSeven(m_pAndroidUserItemSink->m_cbHandCardData[wChairID],MAX_COUNT,BcbCenterCardData,MAX_CENTERCOUNT,BMaxData,CT_MAXTYPE);

		if(1==CardTYPE && cbCardValue1>=CARD_VALUE_Q)
		{
			m_cbMyCardType[m_iBalanceCount]=5;//AA-QQ,类型等级为5
		}
		else if(CARD_VALUE_A==cbCardValue1 && CARD_VALUE_K==cbCardValue2)
		{
			m_cbMyCardType[m_iBalanceCount]=5;//AK类型等级为5
		}
		else if(m_cbRobotCardType[wChairID][0]==1 && cbCardValue1>=CARD_VALUE_9)
		{
			m_cbMyCardType[m_iBalanceCount]=4;//QQ-99类型等级为4
		}
		else if(m_cbRobotCardType[wChairID][0]==2 && CARD_VALUE_A==cbCardValue1 && CARD_VALUE_Q==cbCardValue2)
		{
			m_cbMyCardType[m_iBalanceCount]=4;//同花色AQ类型等级为4
		}
		else if(CARD_VALUE_A==cbCardValue1 && cbCardValue2>=CARD_VALUE_J)
		{
			m_cbMyCardType[m_iBalanceCount]=4;//Aj类型等级为4
		}
		else if(CARD_VALUE_A==cbCardValue1 && cbCardValue2>=CARD_VALUE_10)	
		{
			m_cbMyCardType[m_iBalanceCount]=3;//A-10类型等级为3
		}
		else if(m_cbRobotCardType[wChairID][0]==2 && CARD_VALUE_K==cbCardValue1 && cbCardValue2>=CARD_VALUE_Q)	
		{
			m_cbMyCardType[m_iBalanceCount]=3;//同花KQ类型等级为3
		}
		else if(m_cbRobotCardType[wChairID][0]==2 && ((CARD_VALUE_K==cbCardValue1 && cbCardValue2>=CARD_VALUE_10)
			|| (CARD_VALUE_Q==cbCardValue1 && cbCardValue2>=CARD_VALUE_10)
			|| (CARD_VALUE_J==cbCardValue1 && cbCardValue2>=CARD_VALUE_10)
			|| (CARD_VALUE_10==cbCardValue1 && cbCardValue2>=CARD_VALUE_9)))
		{
			m_cbMyCardType[m_iBalanceCount]=2;//同花K-10,Q-10,J-10,10-9类型等级为2
		}
		else if(m_cbRobotCardType[wChairID][0]==1 && cbCardValue1>=2)
		{
			m_cbMyCardType[m_iBalanceCount]=2;//对子8-2类型等级为2
		}
		else if(m_cbRobotCardType[wChairID][0]==2 && ((CARD_VALUE_A==cbCardValue1 && cbCardValue2>=2)
			|| (CARD_VALUE_K==cbCardValue1 && cbCardValue2>=CARD_VALUE_9)
			|| (CARD_VALUE_Q==cbCardValue1 && cbCardValue2>=CARD_VALUE_J)
			|| (CARD_VALUE_J==cbCardValue1 && cbCardValue2>=CARD_VALUE_10)
			|| (CARD_VALUE_9==cbCardValue1 && cbCardValue2>=CARD_VALUE_8)
			|| (CARD_VALUE_8==cbCardValue1 && cbCardValue2>=CARD_VALUE_8)))	
		{
			m_cbMyCardType[m_iBalanceCount]=1;//同花色A-9,A-2,K-9,QJ,JQ,10J,J10,98,87类型等级为1
		}
		else if((CARD_VALUE_K==cbCardValue1 && cbCardValue2>=CARD_VALUE_10)
			|| (CARD_VALUE_Q==cbCardValue1 && cbCardValue2>=CARD_VALUE_10)
			|| (CARD_VALUE_J==cbCardValue1 && cbCardValue2>=CARD_VALUE_10)
			|| m_cbRobotCardType[wChairID][0]==2
			|| cbCardValue1>CARD_VALUE_Q)
		{
			m_cbMyCardType[m_iBalanceCount]=1;
		}
		else
		{
			m_cbMyCardType[m_iBalanceCount]=0;
		}
	}
	else if(m_iBalanceCount == 1)//第二轮
	{
		BYTE ThreeOk = 0;//判断手牌和中心牌是否组成三条
		if(m_GameLogic.GetCardType(m_pAndroidUserItemSink->m_cbCenterCardData,3)!=CT_THREE_TIAO)
		{
			ThreeOk = 1;
		}
		BYTE BShunZiData[7] = {0};
		BYTE BcbCenterCardData[5] = {0};
		CopyMemory(BcbCenterCardData,m_pAndroidUserItemSink->m_cbCenterCardData,sizeof(BYTE)*3);

		BYTE SHunZiCOunt = m_GameLogic.GetFiveFromSeven(m_pAndroidUserItemSink->m_cbHandCardData[wChairID],MAX_COUNT,BcbCenterCardData,MAX_CENTERCOUNT,BShunZiData,CT_SHUN_ZI);

		BYTE BTongHuaData[7] = {0};
		BYTE STongHuaCOunt = m_GameLogic.GetFiveFromSeven(m_pAndroidUserItemSink->m_cbHandCardData[wChairID],MAX_COUNT,BcbCenterCardData,MAX_CENTERCOUNT,BTongHuaData,CT_TONG_HUA);

		bool MyTongHua = false;
		for(int i=0; i<STongHuaCOunt; i++)
		{
			if(m_GameLogic.GetCardColor(m_pAndroidUserItemSink->m_cbHandCardData[wChairID][0]) == m_GameLogic.GetCardColor(BTongHuaData[i]) || m_GameLogic.GetCardColor(m_pAndroidUserItemSink->m_cbHandCardData[wChairID][1]) == m_GameLogic.GetCardColor(BTongHuaData[i]))
			{
				MyTongHua = true;
				break;
			}
		}

		BYTE BDuiZiData[7] = {0};
		bool bMyDuiZi = false;
		BYTE SDuiZiCOunt = m_GameLogic.GetFiveFromSeven(m_pAndroidUserItemSink->m_cbHandCardData[wChairID],MAX_COUNT,BcbCenterCardData,MAX_CENTERCOUNT,BDuiZiData,CT_ONE_LONG);

		for(int i=0; i<SDuiZiCOunt; i++)
		{
			if(m_pAndroidUserItemSink->m_cbHandCardData[wChairID][0] == BDuiZiData[i] || m_pAndroidUserItemSink->m_cbHandCardData[wChairID][1] == BDuiZiData[i])
			{
				bMyDuiZi = true;
				break;
			}
		}
		BYTE BMaxData[7] = {0};
		BYTE CardTYPE = m_GameLogic.GetFiveFromSeven(m_pAndroidUserItemSink->m_cbHandCardData[wChairID],MAX_COUNT,BcbCenterCardData,MAX_CENTERCOUNT,BMaxData,CT_MAXTYPE);

		if(CardTYPE > CT_THREE_TIAO)//组成三条以上类型等级为5
		{
			m_cbMyCardType[m_iBalanceCount]=5;
		}
		else if(CardTYPE == CT_THREE_TIAO && 1==ThreeOk)//组成三条类型等级为4
		{
			m_cbMyCardType[m_iBalanceCount]=4;
		}
		else if(CardTYPE==CT_TWO_LONG && m_cbRobotCardType[wChairID][0]!=1)//组成两对,必须手牌和中心牌各有一张类型等级为4
		{
			m_cbMyCardType[m_iBalanceCount]=4;
		}
		else if(4==STongHuaCOunt || CardTYPE==CT_TWO_LONG || (m_cbRobotCardType[wChairID][0]==1 && cbCardValue1>=CARD_VALUE_Q))//差一张同花，两对，手牌对子AA-QQ
		{
			m_cbMyCardType[m_iBalanceCount]=3;
		}
		else if(4==SHunZiCOunt || (m_cbRobotCardType[wChairID][0]==1 && cbCardValue1>=CARD_VALUE_9))//差一张顺子，手牌JJ-99
		{
			m_cbMyCardType[m_iBalanceCount]=2;
		}
		else if(bMyDuiZi || (3==SHunZiCOunt && MyTongHua) || (1==m_cbRobotCardType[wChairID][0])
			|| (CARD_VALUE_A==cbCardValue1 && CARD_VALUE_K==cbCardValue2) || (CARD_VALUE_K==cbCardValue1 && CARD_VALUE_Q==cbCardValue2))
		{
			m_cbMyCardType[m_iBalanceCount]=1;
		}
		else
		{
			m_cbMyCardType[m_iBalanceCount]=0;
		}
	}
	else if(m_iBalanceCount == 2)//第三轮
	{
		BYTE ThreeOk = 0;//判断手牌和中心牌是否组成三条
		BYTE BcbCenterCardData[5] = {0};
		CopyMemory(BcbCenterCardData,m_pAndroidUserItemSink->m_cbCenterCardData,sizeof(BYTE)*4);
		m_GameLogic.SortCardList(BcbCenterCardData,CountArray(BcbCenterCardData));

		if(m_GameLogic.GetCardType(BcbCenterCardData,4)!=CT_THREE_TIAO)
		{
			ThreeOk = 1;
		}


		BYTE BShunZiData[7] = {0};
		BYTE SHunZiCOunt = m_GameLogic.GetFiveFromSeven(m_pAndroidUserItemSink->m_cbHandCardData[wChairID],MAX_COUNT,BcbCenterCardData,MAX_CENTERCOUNT,BShunZiData,CT_SHUN_ZI);

		bool MySHunZi = false;
		for(int i=0; i<SHunZiCOunt; i++)
		{
			if(m_pAndroidUserItemSink->m_cbHandCardData[wChairID][0]== BShunZiData[i] || m_pAndroidUserItemSink->m_cbHandCardData[wChairID][1] == BShunZiData[i])
			{
				MySHunZi = true;
				break;
			}
		}

		BYTE BTongHuaData[7] = {0};
		BYTE STongHuaCOunt = m_GameLogic.GetFiveFromSeven(m_pAndroidUserItemSink->m_cbHandCardData[wChairID],MAX_COUNT,BcbCenterCardData,MAX_CENTERCOUNT,BTongHuaData,CT_TONG_HUA);

		bool MyTongHua = false;
		for(int i=0; i<STongHuaCOunt; i++)
		{
			if(m_GameLogic.GetCardColor(m_pAndroidUserItemSink->m_cbHandCardData[wChairID][0]) == m_GameLogic.GetCardColor(BTongHuaData[i]) || m_GameLogic.GetCardColor(m_pAndroidUserItemSink->m_cbHandCardData[wChairID][1]) == m_GameLogic.GetCardColor(BTongHuaData[i]))
			{
				MyTongHua = true;
				break;
			}
		}
		BYTE BDuiZiData[7] = {0};
		bool MyDuiZi = false;
		m_GameLogic.GetFiveFromSeven(m_pAndroidUserItemSink->m_cbHandCardData[wChairID],MAX_COUNT,BcbCenterCardData,MAX_CENTERCOUNT,BDuiZiData,CT_ONE_LONG);

		for(int i=0; i<STongHuaCOunt; i++)
		{
			if(m_pAndroidUserItemSink->m_cbHandCardData[wChairID][0]== BTongHuaData[i] || m_pAndroidUserItemSink->m_cbHandCardData[wChairID][1] == BTongHuaData[i])
			{
				MyDuiZi = true;
				break;
			}
		}
		BYTE BMaxData[7] = {0};
		BYTE CardTYPE = m_GameLogic.GetFiveFromSeven(m_pAndroidUserItemSink->m_cbHandCardData[wChairID],MAX_COUNT,BcbCenterCardData,MAX_CENTERCOUNT,BMaxData,CT_MAXTYPE);

		if(CardTYPE > CT_SHUN_ZI)//组成三条以上类型等级为5
		{
			m_cbMyCardType[m_iBalanceCount]=5;
		}
		else if(CardTYPE>=CT_THREE_TIAO)//三条以上
		{
			m_cbMyCardType[m_iBalanceCount]=4;
		}
		else if(CardTYPE==CT_TWO_LONG && m_cbRobotCardType[wChairID][0]==1 && m_GameLogic.GetCardType(m_pAndroidUserItemSink->m_cbCenterCardData,4)!=CT_TWO_LONG)//两对
		{
			m_cbMyCardType[m_iBalanceCount]=4;
		}//自己AA-QQ，组合成的两对（中心牌各有一张）
		else if(m_cbRobotCardType[wChairID][0]==1 && cbCardValue1>=CARD_VALUE_Q || m_cbRobotCardType[wChairID][0]!=1 && m_GameLogic.GetCardType(m_pAndroidUserItemSink->m_cbCenterCardData,3)!=CT_TWO_LONG)
		{
			m_cbMyCardType[m_iBalanceCount]=3;
		}//差一张成顺子（中心牌有两张），jj-99,(中心牌是三张)手中Ak,KQ,对2以上，
		else if(MySHunZi && SHunZiCOunt>=4 || m_cbRobotCardType[wChairID][0]==1 && cbCardValue1>=CARD_VALUE_9 || m_GameLogic.GetCardType(m_pAndroidUserItemSink->m_cbCenterCardData,4)==CT_THREE_TIAO
			 && cbCardValue1>=CARD_VALUE_K && cbCardValue1>=CARD_VALUE_Q)
		{
			m_cbMyCardType[m_iBalanceCount]=2;
		}//差一张成同花(中心牌小于等于两张同花)，差一张成三条一张成两对，88-22，手中Ak,KQ,对2以上
		else if(4==STongHuaCOunt && MyTongHua || m_cbRobotCardType[wChairID][0]==1 || CardTYPE ==CT_ONE_LONG && cbCardValue1>=CARD_VALUE_K || cbCardValue1>=CARD_VALUE_K && cbCardValue1>=CARD_VALUE_Q || MyDuiZi)
		{
			m_cbMyCardType[m_iBalanceCount]=1;
		}
		else
		{
			m_cbMyCardType[m_iBalanceCount]=0;
		}

	}
	else//第四轮
	{
		BYTE ThreeOk = 0;//判断手牌和中心牌是否组成三条
		if(m_GameLogic.GetCardType(m_pAndroidUserItemSink->m_cbCenterCardData,3)!=CT_THREE_TIAO)
		{
			ThreeOk = 1;
		}
		BYTE BShunZiData[7] = {0};
		BYTE SHunZiCOunt = m_GameLogic.GetFiveFromSeven(m_pAndroidUserItemSink->m_cbHandCardData[wChairID],MAX_COUNT,m_pAndroidUserItemSink->m_cbCenterCardData,MAX_CENTERCOUNT,BShunZiData,CT_SHUN_ZI);
		bool MySHunZi = false;
		for(int i=0; i<SHunZiCOunt; i++)
		{
			if(m_pAndroidUserItemSink->m_cbHandCardData[wChairID][0]== BShunZiData[i] || m_pAndroidUserItemSink->m_cbHandCardData[wChairID][1] == BShunZiData[i])
			{
				MySHunZi = true;
				break;
			}
		}

		BYTE BTongHuaData[7] = {0};
		BYTE STongHuaCOunt = m_GameLogic.GetFiveFromSeven(m_pAndroidUserItemSink->m_cbHandCardData[wChairID],MAX_COUNT,m_pAndroidUserItemSink->m_cbCenterCardData,MAX_CENTERCOUNT,BTongHuaData,CT_TONG_HUA);
		bool MyTongHua = false;
		for(int i=0; i<STongHuaCOunt; i++)
		{
			if(m_GameLogic.GetCardColor(m_pAndroidUserItemSink->m_cbHandCardData[wChairID][0]) == m_GameLogic.GetCardColor(BTongHuaData[i]) || m_GameLogic.GetCardColor(m_pAndroidUserItemSink->m_cbHandCardData[wChairID][1]) == m_GameLogic.GetCardColor(BTongHuaData[i]))
			{
				MyTongHua = true;
				break;
			}
		}

		BYTE BDuiZiData[7] = {0};
		bool bMyDuiZi = false;
		m_GameLogic.GetFiveFromSeven(m_pAndroidUserItemSink->m_cbHandCardData[wChairID],MAX_COUNT,m_pAndroidUserItemSink->m_cbCenterCardData,MAX_CENTERCOUNT,BDuiZiData,CT_ONE_LONG);
		for(int i=0; i<STongHuaCOunt; i++)
		{
			if(m_pAndroidUserItemSink->m_cbHandCardData[wChairID][0]== BTongHuaData[i] || m_pAndroidUserItemSink->m_cbHandCardData[wChairID][1] == BTongHuaData[i])
			{
				bMyDuiZi = true;
				break;
			}
		}
		BYTE cbSameCnt = 0;
		BYTE cbSameCard[2] = {0};
		for(int i=0; i<5; i++)
		{
			if(m_pAndroidUserItemSink->m_cbHandCardData[wChairID][0] == m_cbEndCardData[wChairID][i])
			{
				cbSameCnt++;
				cbSameCard[0] = m_cbEndCardData[wChairID][i];
			}
			if(m_pAndroidUserItemSink->m_cbHandCardData[wChairID][1] == m_cbEndCardData[wChairID][i])
			{
				cbSameCnt++;	
				cbSameCard[1] = m_cbEndCardData[wChairID][i];
			}
		}
		BYTE cbSameCardValue1=m_GameLogic.GetCardLogicValue(cbSameCard[0]);

		//中心牌的牌型
		//同花		
		BYTE CenterTongHuaCard[5] = {0};
		BYTE CenterTongHuaCount = m_GameLogic.GetFiveFromSeven(m_pAndroidUserItemSink->m_cbHandCardData[wChairID],0,m_pAndroidUserItemSink->m_cbCenterCardData,MAX_CENTERCOUNT,CenterTongHuaCard,CT_TONG_HUA);
		//顺子		
		BYTE CenterShunZiCount = m_GameLogic.GetFiveFromSeven(m_pAndroidUserItemSink->m_cbHandCardData[wChairID],0,m_pAndroidUserItemSink->m_cbCenterCardData,MAX_CENTERCOUNT,CenterTongHuaCard,CT_SHUN_ZI);

		if(m_cbRobotsWin[wChairID] >= CT_SHUN_ZI && 2==cbSameCnt)
		{
			m_cbMyCardType[m_iBalanceCount]=5;//组成三条以上类型等级为5
		}
		else if(m_cbRobotsWin[wChairID] == CT_THREE_TIAO && 2==cbSameCnt && m_cbRobotCardType[wChairID][0]==1)
		{
			m_cbMyCardType[m_iBalanceCount]=5;//组成三条，且手中占两张
		}
		else if((m_cbRobotsWin[wChairID] >= CT_THREE_TIAO && cbSameCnt>=1)
			|| (m_cbRobotsWin[wChairID] >= CT_TWO_LONG && 2==cbSameCnt && m_cbRobotCardType[wChairID][0]==1 && cbCardValue1>=CARD_VALUE_Q))
		{
			m_cbMyCardType[m_iBalanceCount]=4;//三条，且手中有一张三条的牌，两对且手中有一对且对子值大于等于Q，三条以上的牌，且手中有一张
		}
		else if((m_cbRobotsWin[wChairID] >= CT_TWO_LONG && 2==cbSameCnt && m_cbRobotCardType[wChairID][0]==1 && cbCardValue1>=CARD_VALUE_8)
			 || (m_cbRobotsWin[wChairID] >= CT_TWO_LONG && 1==cbSameCnt && m_cbRobotCardType[wChairID][0]!=1 && cbSameCardValue1>=CARD_VALUE_Q))
		{
			m_cbMyCardType[m_iBalanceCount]=3;//两对且手中有一对且对子值大于等于8.两对且手中有一张，且值大于等于8
		}
		else if(m_cbRobotsWin[wChairID] == CT_ONE_LONG && CenterTongHuaCount<=4 && CenterShunZiCount<=0 && 0==cbSameCnt && cbSameCardValue1>=CARD_VALUE_J
			 || m_cbRobotsWin[wChairID] >= CT_TWO_LONG && cbSameCnt>=1 && m_cbRobotCardType[wChairID][0]!=1 && cbSameCardValue1>=2)
		{
			m_cbMyCardType[m_iBalanceCount]=2;//手上有一对，手牌不是顺子和同花，对子值大于等于J,两对手牌占一张
		}
		else if(bMyDuiZi || (m_cbRobotsWin[wChairID]<=CT_ONE_LONG && 1==cbSameCnt)
			|| (m_cbRobotsWin[wChairID]>=CT_TWO_LONG && 0==cbSameCnt && cbSameCardValue1>=CARD_VALUE_K)
			|| (CenterTongHuaCount<=2 && CenterShunZiCount<=2))
		{
			m_cbMyCardType[m_iBalanceCount]=1;//对子类型，手牌占一张,中心牌类型是两对以上，手牌一张不占，但是手牌最大值是13
		}
		else
		{
			m_cbMyCardType[m_iBalanceCount]=0;
		}
	}
	return true;
}
BYTE CRobottLogic::RobotAlgorithm(WORD wChairID, LONG lScore,BYTE ID) 
{
	ZeroMemory(m_cbEndCardData,sizeof(m_cbEndCardData));
	UserWinList WinnerList={0};

	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		WinnerList.wWinerList[i] = 10;
		if (m_pAndroidUserItemSink->m_cbPlayStatus[i])
		{
			//最大牌型
			m_cbRobotsWin[i] = m_GameLogic.FiveFromSeven(m_pAndroidUserItemSink->m_cbHandCardData[i],MAX_COUNT,m_pAndroidUserItemSink->m_cbCenterCardData,MAX_CENTERCOUNT,m_cbEndCardData[i],MAX_CENTERCOUNT);
		}
	}	
	m_GameLogic.SortCardList(m_pAndroidUserItemSink->m_cbHandCardData[wChairID],2);
	m_GameLogic.GetCardLogicValue1(m_pAndroidUserItemSink->m_cbHandCardData[wChairID],m_cbRobotCardType[wChairID]);
	SetLevel(wChairID);//获得当前手牌的等级

	//查找最大用户
	if(!m_GameLogic.SelectMaxUser(m_cbEndCardData,WinnerList,m_pAndroidUserItemSink->m_cbHandCardData,0))
	{
		return 0;
	}

	for(int j=0;j<WinnerList.bSameCount;j++)
	{
		if(WinnerList.wWinerList[j]==wChairID)
		{
			return 1;
		}
	}
	return 0;
}
int CRobottLogic::RobotsRate( BYTE cbValue,BYTE ID, WORD wChairID ) 
{
	LONG lRateBuf=0;
	//放弃率
	if(ID == 0)
	{
		//第1手牌
		if (0==m_iBalanceCount)
		{
			lRateBuf = m_lRobotGiveUp[cbValue];
		}
		else
		{
			lRateBuf = m_lRobotGiveUp[m_iBalanceCount*6+14+cbValue];
		}
		if(m_cbRobotWin)//如果是5家最大牌减多少放弃率
		{
			lRateBuf-=m_lRobotGiveUp[6+(cbValue+2)/3];
		}
		else//加多少放弃率
		{
			lRateBuf+=m_lRobotGiveUp[9+(cbValue+2)/3];
		}		
		if((m_lTurnLessScore/m_pAndroidUserItemSink->m_lCellScore)>=m_lRobotGiveUp[12]
			 || 	m_pAndroidUserItemSink->m_lTableScore[wChairID]/m_pAndroidUserItemSink->m_lCellScore>=m_lRobotGiveUp[13])
		{
			
			if(m_cbRobotWin)
			{
				lRateBuf+=m_lRobotGiveUp[17+(cbValue+2)/3];//如果是5家最大牌减多少放弃率??????????????????????
			}
			else
			{
				lRateBuf+=m_lRobotGiveUp[14+(cbValue+2)/3];//加多少放弃率
			}
		}		
	}
	else if(ID == 1)//加注率
	{
		ASSERT(m_iBalanceCount<=3);
		lRateBuf = m_lRobotAdd[m_iBalanceCount*6+cbValue];
		
		if(m_cbRobotWin)
		{
			lRateBuf+=m_lRobotAdd[24];//如果是5家最大牌增加加注率
		}
		else
		{		
			lRateBuf-=m_lRobotAdd[25];//减少加注率
		}
	}
	else if(ID == 2)//全押率
	{
		ASSERT(m_iBalanceCount<=3);
		if (0==m_iBalanceCount)
		{
			lRateBuf=0;
		}
		else
		{
			lRateBuf = m_lRobotQuAnYa[(m_iBalanceCount-1)*6+cbValue];
		}
		if(m_cbRobotWin)
		{
			lRateBuf+=m_lRobotQuAnYa[18];//如果是5家最大牌增加加注率
		}
		else
		{
			lRateBuf-=m_lRobotQuAnYa[19];//减少加注率
		}
		if(m_iBalanceCount<=0 && 0==cbValue)
		{
			lRateBuf-=300;
		}
	}
	return lRateBuf;
}