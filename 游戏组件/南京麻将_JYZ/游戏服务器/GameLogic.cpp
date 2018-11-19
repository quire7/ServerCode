#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//��̬����

//������
const BYTE CGameLogic::m_cbCardDataArray[MAX_REPERTORY]=
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,					//����
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,					//����
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,					//����
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,					//����
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,					//����
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,					//����
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,					//����
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,					//����
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,					//ͬ��
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,					//ͬ��
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,					//ͬ��
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,					//ͬ��
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,								//����
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,								//����
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,								//����
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,								//����
	0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48							//����(�����ġ������÷�������񡢾�)
};

//////////////////////////////////////////////////////////////////////////
//���캯��
CGameLogic::CGameLogic()
{
	ResetAllData();
}

//��������
CGameLogic::~CGameLogic()
{
}

//��������
void CGameLogic::ResetAllData()
{

}

//������
void CGameLogic::RandCardData( BYTE cbCardData[], BYTE cbMaxCount )
{
	//��ʼ������
	srand(GetTickCount()|(rand()<<8));

	//����׼��
	BYTE cbCardDataTemp[MAX_REPERTORY] = {0};
	CopyMemory(cbCardDataTemp, m_cbCardDataArray, sizeof(BYTE)*MAX_REPERTORY);

	//������
	BYTE cbRandCount=0, cbPosition=0;
	do
	{
		cbPosition = rand()%(cbMaxCount-cbRandCount);
		cbCardData[cbRandCount++] = cbCardDataTemp[cbPosition];
		cbCardDataTemp[cbPosition] = cbCardDataTemp[cbMaxCount-cbRandCount];
	} while (cbRandCount<cbMaxCount);
}

//ɾ����
bool CGameLogic::RemoveCard( BYTE cbCardIndex[MAX_INDEX], BYTE cbRemoveCard )
{
	//Ч����
	VERIFY(IsValidCard(cbRemoveCard));
	ASSERT(cbCardIndex[SwitchToCardIndex(cbRemoveCard)]>0);

	//ɾ����
	BYTE cbRemoveIndex = SwitchToCardIndex(cbRemoveCard);
	ASSERT(cbRemoveIndex < MAX_INDEX);
	if (cbCardIndex[cbRemoveIndex] > 0)
	{
		cbCardIndex[cbRemoveIndex]--;
		return true;
	}

	//ʧ��Ч��
	ASSERT(FALSE);
	return false;
}

//ɾ����
bool CGameLogic::RemoveCard( BYTE cbCardIndex[MAX_INDEX], BYTE cbRemoveCard[], BYTE cbRemoveCount )
{
	//ɾ����
	for (BYTE i=0; i<cbRemoveCount; i++)
	{
		//Ч����
		VERIFY(IsValidCard(cbRemoveCard[i]));
		BYTE cbRemoveIndex = SwitchToCardIndex(cbRemoveCard[i]);
		//У������
		ASSERT(cbRemoveIndex < MAX_INDEX);
		//У����Ŀ
		ASSERT(cbCardIndex[cbRemoveIndex]>0);
		//ɾ����
		if (cbCardIndex[cbRemoveIndex] == 0)
		{
			//�������
			ASSERT(FALSE);

			//��ԭɾ��
			for (BYTE j=0; j<i; j++) 
			{
				VERIFY(IsValidCard(cbRemoveCard[j]));
				cbCardIndex[SwitchToCardIndex(cbRemoveCard[j])]++;
			}

			return false;
		}
		else 
		{
			//ɾ����
			--cbCardIndex[cbRemoveIndex];
		}
	}

	return true;
}

//ɾ����
bool CGameLogic::RemoveCard( BYTE cbCardData[], BYTE cbCardCount, BYTE cbRemoveCard[], BYTE cbRemoveCount )
{
	//��������
	ASSERT(cbCardCount<=14);
	ASSERT(cbRemoveCount<=cbCardCount);

	//�������
	BYTE cbDeleteCount=0, cbTempCardData[14]={0};
	if (cbCardCount > CountArray(cbTempCardData))
	{
		return false;
	}
	CopyMemory(cbTempCardData, cbCardData, cbCardCount*sizeof(cbCardData[0]));

	//������
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

	//�ɹ��ж�
	if (cbDeleteCount != cbRemoveCount) 
	{
		ASSERT(FALSE);
		return false;
	}

	ZeroMemory(cbCardData, cbCardCount*sizeof(cbCardData[0]));
	//������
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

//������
void CGameLogic::SortCardList( BYTE cbCardData[], BYTE cbCardCount )const
{
	if (cbCardCount<2) return;
	//�������
	bool bSorted = true;
	BYTE cbTempData=0, cbLast=cbCardCount-1;
	do
	{
		bSorted = true;
		for (BYTE i=0; i<cbLast; i++)
		{
			if (cbCardData[i] > cbCardData[i+1])
			{
				//����λ��
				cbTempData = cbCardData[i];
				cbCardData[i] = cbCardData[i+1];
				cbCardData[i+1] = cbTempData;
				bSorted = false;
			}	
		}
		cbLast--;
	} while(!bSorted);
}

//��ȡӲ����
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

//��Ч�ж�
bool CGameLogic::IsValidCard( BYTE cbCardData )
{
	BYTE cbCardValue = GetCardValue(cbCardData);
	BYTE cbCardColor = GetCardColor(cbCardData);
	return (((cbCardValue>=1)&&(cbCardValue<=9)&&(cbCardColor<=2)) || \
			((cbCardValue>=1)&&(cbCardValue<=7)&&(cbCardColor==3)) || \
			((cbCardValue>=1)&&(cbCardValue<=8)&&(cbCardColor==4)));
}

//����Ŀ
BYTE CGameLogic::GetCardCount( const BYTE cbCardIndex[MAX_INDEX] )
{
	//��Ŀͳ��
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

//��ȡ���
BYTE CGameLogic::GetWeaveCard( DWORD dwWeaveKind, BYTE cbCenterCard, BYTE cbCardBuffer[MAX_WEAVE] )
{
	//�����
	switch (dwWeaveKind)
	{
	case WIK_OXX:		//��Բ���
		{
			//���ñ���
			cbCardBuffer[0] = cbCenterCard;
			cbCardBuffer[1] = cbCenterCard+1;
			cbCardBuffer[2] = cbCenterCard+2;
			return 3;
		}
	case WIK_XXO:		//�ҳԲ���
		{
			//���ñ���
			cbCardBuffer[0] = cbCenterCard;
			cbCardBuffer[1] = cbCenterCard-1;
			cbCardBuffer[2] = cbCenterCard-2;
			return 3;
		}
	case WIK_XOX:	//�гԲ���
		{
			//���ñ���
			cbCardBuffer[0] = cbCenterCard;
			cbCardBuffer[1] = cbCenterCard-1;
			cbCardBuffer[2] = cbCenterCard+1;
			return 3;
		}
	case WIK_PENG:		//���Ʋ���
		{
			//���ñ���
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
	case WIK_AGANG:		//���Ʋ���
		{
			//���ñ���
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

//������Ʒ���
DWORD CGameLogic::CalculateFanShu( DWORD dwHuPaiKind )
{
	DWORD dwHuPaiFanShu = 0;

	//ƽ��
	if ((dwHuPaiKind&HU_PINGHU) != 0)
	{
		dwHuPaiFanShu += 0;
	}
	//����
	if ((dwHuPaiKind&HU_MENQING) != 0)
	{
		dwHuPaiFanShu += 20;
	}
	//��һɫ
	if ((dwHuPaiKind&HU_HUNYISE) != 0)
	{
		dwHuPaiFanShu += 40;
	}
	//�ԶԺ�
	if ((dwHuPaiKind&HU_DUIDUIHU) != 0)
	{
		dwHuPaiFanShu += 40;
	}
	//��һɫ
	if ((dwHuPaiKind&HU_QINGYISE) != 0)
	{
		dwHuPaiFanShu += 60;
	}
	//ȫ�����
	if ((dwHuPaiKind&HU_QQDUDIAO) != 0)
	{
		dwHuPaiFanShu += 60;
	}
	//�߶�
	if ((dwHuPaiKind&HU_QIDUI) != 0)
	{
		dwHuPaiFanShu += 80;
	}
	//˫�߶�
	if ((dwHuPaiKind&HU_SQIDUI) != 0)
	{
		dwHuPaiFanShu += 160;
	}
	//�޻���
	if ((dwHuPaiKind&HU_WUHUAGUO) != 0)
	{
		dwHuPaiFanShu += 40;
	}
	//ѹ��
	if ((dwHuPaiKind&HU_YAJUE) != 0)
	{
		dwHuPaiFanShu += 40;
	}
	//С�ܿ���
	if ((dwHuPaiKind&HU_XGKAIHUA) != 0)
	{
		dwHuPaiFanShu += 20;
	}
	//��ܿ���
	if ((dwHuPaiKind&HU_DGKAIHUA) != 0)
	{
		dwHuPaiFanShu += 40;
	}
	//���
	if ((dwHuPaiKind&HU_TIANHU) != 0)
	{
		dwHuPaiFanShu += 400;
	}
	//�غ�
	if ((dwHuPaiKind&HU_DIHU) != 0)
	{
		dwHuPaiFanShu += 80;
	}

	return dwHuPaiFanShu;
}

//������Ƿ���������
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

//�����ȼ�
BYTE CGameLogic::GetUserActionRank( DWORD dwUserAction )
{
	//���Ƶȼ�
	if (dwUserAction&WIK_HU) 
	{
		return 5; 
	}
	//��ս�ȼ�
	if (dwUserAction&WIK_TIAOZHAN)
	{
		return 4;
	}
	//���Ƶȼ�
	if (dwUserAction&WIK_GANG)
	{
		return 3;
	}
	//���Ƶȼ�
	if (dwUserAction&WIK_PENG) 
	{
		return 2;
	}
	//���Ƶȼ�
	if (dwUserAction&WIK_CHI) 
	{
		return 1;
	}
	//

	return 0;
}

//���Ƶȼ�
WORD CGameLogic::GetHuPaiActionRank( tagHuPaiResult & HuPaiResult )
{
	return 0;
}

//���Ʒ���
DWORD CGameLogic::AnalyseChiPai( const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard )
{
	//����Ч��
	VERIFY(IsValidCard(cbCurrentCard));

	//�����ж�,��������
	if (cbCurrentCard >= 0x31) 
	{
		return WIK_NULL;
	}

	//��������
	BYTE cbExcursion[3] = {0, 1, 2};
	DWORD dwItemKind[3] = {WIK_OXX, WIK_XOX, WIK_XXO};

	//�����ж�
	DWORD dwEatKind = WIK_NULL; BYTE cbFirstIndex = 0;
	BYTE cbCurrentIndex = SwitchToCardIndex(cbCurrentCard);
	for (BYTE i=0; i<CountArray(dwItemKind); i++)
	{
		BYTE cbValueIndex = cbCurrentIndex%9;
		if ((cbValueIndex>=cbExcursion[i]) && ((cbValueIndex-cbExcursion[i])<=6))
		{
			//�����ж�
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

			//��������
			dwEatKind |= dwItemKind[i];
		}
	}

	return dwEatKind;
}

//���Ʒ���
DWORD CGameLogic::AnalysePengPai( const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard )
{
	//����Ч��
	VERIFY(IsValidCard(cbCurrentCard));

	//�����ж�
	return (cbCardIndex[SwitchToCardIndex(cbCurrentCard)]>=2)?WIK_PENG:WIK_NULL;
}

//���Ʒ���
DWORD CGameLogic::AnalyseGangPai( const BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, tagGangPaiResult &GangPaiResult, bool bZiMo )
{
	//���ñ���
	DWORD dwActionMask = WIK_NULL;
	ZeroMemory(&GangPaiResult, sizeof(GangPaiResult));

	//�������(����)
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
		//���ϸ���(����)
		for (BYTE i=0; i<MAX_NORMAL; i++)
		{
			if (cbCardIndex[i] == 4)
			{
				dwActionMask |= WIK_AGANG;
				GangPaiResult.dwGangType[GangPaiResult.cbCardCount] = WIK_AGANG;
				GangPaiResult.cbCardData[GangPaiResult.cbCardCount++] = SwitchToCardData(i);
			}
		}

		//��ϸ���(����)
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

//���Ʒ���
DWORD CGameLogic::AnalyseTingPai( const BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount, BYTE cbHuaCount, DWORD dwPreHuPaiKind, tagTingPaiResult &TingPaiResult, bool bCheckTianTing/*=false*/ )
{
	//���ñ���
	ZeroMemory(&TingPaiResult, sizeof(TingPaiResult));

	//��������
	DWORD dwTingPaiKind = WIK_NULL;
	BYTE cbCardIndexTemp[MAX_INDEX] = {0};
	CopyMemory(cbCardIndexTemp, cbCardIndex, sizeof(BYTE)*MAX_INDEX);
	//�������
	if (bCheckTianTing)
	{
 		tagTingPaiItem TingPaiItem;
 		ZeroMemory(&TingPaiItem, sizeof(TingPaiItem));
 		TingPaiItem.cbOutCardData = 0;
 
 		//�����ж�
 		for (BYTE i=0; i<MAX_NORMAL; i++)
 		{
 			//���Ʒ���
 			BYTE cbCurrentCard = SwitchToCardData(i); 

 			//��������
 			tagHuPaiResult HuPaiResult;
 			DWORD dwHuPaiKind = AnalyseHuPai(cbCardIndexTemp, WeaveItem, cbItemCount, cbCurrentCard, cbHuaCount, dwPreHuPaiKind, HuPaiResult, false);
 
 			//����ж�
  			if (dwHuPaiKind == WIK_HU) 
  			{
				TingPaiItem.dwHuCardFanShu[TingPaiItem.cbTingCardCount] = HuPaiResult.dwHuPaiFanShu;
  				TingPaiItem.cbTingCardData[TingPaiItem.cbTingCardCount++] = cbCurrentCard;
  				dwTingPaiKind = WIK_TING;
  			}
 		}
 		//�������ƽ��
  		if (TingPaiItem.cbTingCardCount > 0)
  		{
  			TingPaiResult.TingPaiItemArrary[TingPaiResult.cbTingPaiItemCount++] = TingPaiItem;
  		}
	}
	else
	{
		BYTE cbCardCount = GetCardCount(cbCardIndex);
		ASSERT(cbCardCount%3==2 && cbCardCount>0 && cbCardCount<=MAX_COUNT);
		//���Ʒ���
		for (BYTE i=0; i<MAX_NORMAL; i++)
		{
			//���ƹ���
			if (cbCardIndexTemp[i] == 0)
			{
				continue;
			}

			//���ƴ���
			cbCardIndexTemp[i]--;
			tagTingPaiItem TingPaiItem;
			ZeroMemory(&TingPaiItem, sizeof(TingPaiItem));
			TingPaiItem.cbOutCardData = SwitchToCardData(i);

			//�����ж�
			for (BYTE j=0; j<MAX_NORMAL; j++)
			{
				if (cbCardIndexTemp[j]==4) continue;
				//��������
				tagHuPaiResult HuPaiResult;
				ZeroMemory(&HuPaiResult, sizeof(HuPaiResult));

				//���Ʒ���
				BYTE cbCurrentCard = SwitchToCardData(j);
				DWORD dwHuPaiKind = AnalyseHuPai(cbCardIndexTemp, WeaveItem, cbItemCount, cbCurrentCard, cbHuaCount, dwPreHuPaiKind, HuPaiResult, false);

				//����ж�
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

			//��ԭ����
			cbCardIndexTemp[i]++;
		}
	}

	return dwTingPaiKind;
}

//���Ʒ���
DWORD CGameLogic::AnalyseHuPai( const BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, BYTE cbHuaCount, DWORD dwPreHuPaiKind, \
							   tagHuPaiResult &HuPaiResult, bool bZiMo, bool bContinue/*=true*/)
{	
	ZeroMemory(&HuPaiResult, sizeof(HuPaiResult));

	//��������
	DWORD dwHuPaiKind=0, dwHuPaiFanShu=0;;
	CAnalyseItemArray AnalyseItemArray;
	AnalyseItemArray.RemoveAll();

	//������
	BYTE cbCardIndexTemp[MAX_INDEX] = {0};
	CopyMemory(cbCardIndexTemp, cbCardIndex, sizeof(BYTE)*MAX_INDEX);

	//������
	if (!bZiMo && IsValidCard(cbCurrentCard))
	{
		BYTE cbCurrentCardIndex = SwitchToCardIndex(cbCurrentCard);
		ASSERT(cbCurrentCardIndex>=0 && cbCurrentCardIndex<MAX_NORMAL);
		cbCardIndexTemp[cbCurrentCardIndex]++;
	}

	BYTE cbCardCount = GetCardCount(cbCardIndexTemp);
	ASSERT(cbCardCount%3==2 && cbCardCount>=2 && cbCardCount<=MAX_COUNT);
	BYTE cbQiDuiType = IsQiDui(cbCardIndexTemp, WeaveItem, cbWeaveCount, cbCurrentCard);

	//�߶�����
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
		//��һɫ
		if (IsHunYiSe(cbCardIndexTemp, WeaveItem, cbWeaveCount))
		{
			dwHuPaiKind |= HU_HUNYISE;
		}
		//��һɫ
		if (IsQingYiSe(cbCardIndexTemp, WeaveItem, cbWeaveCount))
		{
			dwHuPaiKind |= HU_QINGYISE;
		}

		dwHuPaiKind |= dwPreHuPaiKind;
	}
	else
	{
		//������
		AnalyseCard(cbCardIndexTemp, WeaveItem, cbWeaveCount, AnalyseItemArray);

		//���Ʒ���
		if (AnalyseItemArray.GetCount() > 0)
		{
			if (!bContinue)
			{
				return WIK_HU;
			}

			//�ҳ��������
			DWORD dwHuPaiKindTemp=0, dwHuPaiFanShuTemp=0;
			
			//���ͷ���
			for (INT_PTR i=0; i<AnalyseItemArray.GetCount(); i++)
			{
				dwHuPaiKindTemp |= dwPreHuPaiKind;
				//��������
				tagAnalyseItem *pAnalyseItem = &AnalyseItemArray[i];
				//����
				if (IsMenQing(cbCardIndexTemp, WeaveItem, cbWeaveCount))
				{
					dwHuPaiKindTemp |= HU_MENQING;
				}
 				//ȫ�����
 				if (IsQuanQiuDuDiao(cbCardIndexTemp, pAnalyseItem, WeaveItem, cbWeaveCount, cbCurrentCard, cbHuaCount, dwPreHuPaiKind))
 				{
 					dwHuPaiKindTemp |= HU_QQDUDIAO;
 				}	
 				//�ԶԺ�
 				if (IsDuiDuiHu(pAnalyseItem))
 				{
 					dwHuPaiKindTemp |= HU_DUIDUIHU;
 				}
				//��һɫ
				if (IsHunYiSe(cbCardIndexTemp, WeaveItem, cbWeaveCount))
				{
					dwHuPaiKindTemp |= HU_HUNYISE;
				}
				//��һɫ
				if (IsQingYiSe(cbCardIndexTemp, WeaveItem, cbWeaveCount))
				{
					dwHuPaiKindTemp |= HU_QINGYISE;
				}
				//ֻ��һ��
				if (IsOnlyHuOneCard(cbCardIndexTemp, WeaveItem, cbWeaveCount, cbCurrentCard, cbHuaCount, dwPreHuPaiKind))
				{
					dwHuPaiKindTemp |= HU_ONLYONE;
				}
				else
				{
					//����ȥ��ѹ��
					dwHuPaiKindTemp &= ~HU_YAJUE;
				}

				//���Ǵ��������
				if (!IsBigHuPaiKind(dwHuPaiKindTemp) && (dwHuPaiKindTemp&HU_MENQING)==0)
				{
					//Ӳ�������ĸ����Ժ�ƽ��
					if (cbHuaCount >= 4)
					{
						dwHuPaiKindTemp |= HU_PINGHU;
					}
					else
					{
						dwHuPaiKindTemp = HU_NULL;
					}
				}

				//�ж����ʹ�С
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

	//����ж�
	if (dwHuPaiKind != HU_NULL)
	{
		////ȥ��ƽ��
		//if (IsBigHuPaiKind(dwHuPaiKind) || (dwHuPaiKind&HU_MENQING)!=0)
		//{
		//	dwHuPaiKind &= ~HU_PINGHU;
		//}

		DWORD dwNoPinghu = HU_MENQING | HU_WUHUAGUO | HU_YAJUE | HU_DGKAIHUA | HU_XGKAIHUA | HU_HUNYISE |
			HU_QINGYISE | HU_DUIDUIHU | HU_QQDUDIAO | HU_QIDUI | HU_SQIDUI | HU_TIANHU | HU_DIHU;

		//ȥ��ƽ��
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

//������
bool CGameLogic::AnalyseCard( const BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount, CAnalyseItemArray &AnalyseItemArray )
{
	//������Ŀ
	BYTE cbCardCount = GetCardCount(cbCardIndex);

	//Ч����Ŀ
	ASSERT((cbCardCount>=2)&&(cbCardCount<=MAX_COUNT)&&((cbCardCount-2)%3==0));
	if ((cbCardCount<2)||(cbCardCount>MAX_COUNT)||((cbCardCount-2)%3!=0)) return false;

	//��������
	BYTE cbKindItemCount = 0;
	tagKindItem KindItem[MAX_COUNT-2];
	ZeroMemory(KindItem, sizeof(KindItem));

	//�����ж�
	BYTE cbLessKindItem = (cbCardCount-2)/3;
	ASSERT((cbLessKindItem+cbWeaveCount)==4);

	//�����ж�
	if (cbLessKindItem == 0)
	{
		//Ч�����
		ASSERT((cbCardCount==2)&&(cbWeaveCount==4));

		//�����ж�
		for (BYTE i=0; i<MAX_NORMAL; i++)
		{
			if (cbCardIndex[i] == 2)
			{
				//��������
				tagAnalyseItem AnalyseItem;
				ZeroMemory(&AnalyseItem, sizeof(AnalyseItem));

				//���ý��
				for (BYTE j=0; j<cbWeaveCount; j++)
				{
					AnalyseItem.dwWeaveKind[j] = WeaveItem[j].dwWeaveKind;
					AnalyseItem.cbCenterCard[j] = WeaveItem[j].cbCenterCard;
				}
				AnalyseItem.cbCardEye = SwitchToCardData(i);

				//������
				AnalyseItemArray.Add(AnalyseItem);
				return true;
			}
		}
		return false;
	}

	//��ַ���
	if (cbCardCount >= 3)
	{
		for (BYTE i=0; i<MAX_NORMAL; i++)
		{
			//ͬ���ж�
			if (cbCardIndex[i] >= 3)
			{
				KindItem[cbKindItemCount].cbCardIndex[0] = i;
				KindItem[cbKindItemCount].cbCardIndex[1] = i;
				KindItem[cbKindItemCount].cbCardIndex[2] = i;
				KindItem[cbKindItemCount].dwWeaveKind = WIK_PENG;
				KindItem[cbKindItemCount++].cbCenterCard = SwitchToCardData(i);
			}

			//�����ж�
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

	//��Ϸ���
	if (cbKindItemCount >= cbLessKindItem)
	{
		//��������
		BYTE cbCardIndexTemp[MAX_INDEX] = {0};
		BYTE cbIndex[4] = {0, 1, 2, 3};
		tagKindItem *pKindItem[4];
		ZeroMemory(&pKindItem, sizeof(pKindItem));

		//��ʼ���
		do
		{
			//���ñ���
			CopyMemory(cbCardIndexTemp, cbCardIndex, sizeof(BYTE)*MAX_INDEX);
			for (BYTE i=0; i<cbLessKindItem; i++)
			{
				pKindItem[i] = &KindItem[cbIndex[i]];
			}

			//�����ж�
			bool bEnoughCard  = true;
			for (BYTE i=0; i<cbLessKindItem*3; i++)
			{
				//�����ж�
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

			//�����ж�
			if (bEnoughCard)
			{
				//�����ж�
				BYTE cbCardEye = 0;
				for (BYTE i=0; i<MAX_NORMAL; i++)
				{
					if (cbCardIndexTemp[i] == 2)
					{
						cbCardEye = SwitchToCardData(i);
						break;
					}
				}

				//�������
				if (cbCardEye != 0)
				{
					//��������
					tagAnalyseItem AnalyseItem;
					ZeroMemory(&AnalyseItem, sizeof(AnalyseItem));

					//�������
					for (BYTE i=0; i<cbWeaveCount; i++)
					{
						AnalyseItem.dwWeaveKind[i] = WeaveItem[i].dwWeaveKind;
						AnalyseItem.cbCenterCard[i] = WeaveItem[i].cbCenterCard;
					}

					//��������
					for (BYTE i=0; i<cbLessKindItem; i++) 
					{
						AnalyseItem.dwWeaveKind[i+cbWeaveCount] = pKindItem[i]->dwWeaveKind;
						AnalyseItem.cbCenterCard[i+cbWeaveCount] = pKindItem[i]->cbCenterCard;
					}

					//��������
					AnalyseItem.cbCardEye = cbCardEye;

					//������
					AnalyseItemArray.Add(AnalyseItem);
				}
			}

			//��������
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

//����
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

//��һɫ��
bool CGameLogic::IsHunYiSe( const BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount )
{
	bool bCardColor[4] = {false, false, false, false};
	//����
	for (BYTE i=0; i<MAX_NORMAL; i++)
	{
		if (cbCardIndex[i] > 0)
		{
			BYTE cbCardData = SwitchToCardData(i);
			bCardColor[GetCardColor(cbCardData)] = true;
		}
	}
	//�����
	for (BYTE i=0; i<cbWeaveCount; i++)
	{
		BYTE cbCardData = WeaveItem[i].cbCenterCard;
		bCardColor[GetCardColor(cbCardData)] = true;
	}
	//ͳ��
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

//��һɫ��
bool CGameLogic::IsQingYiSe( const BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount )
{
	bool bCardColor[3] = {false, false, false};
	//����
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
	//�����
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
	//ͳ��
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

//�ԶԺ�
bool CGameLogic::IsDuiDuiHu( tagAnalyseItem *pAnalyseItem )
{
	//���ͷ���
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

//ȫ�����
bool CGameLogic::IsQuanQiuDuDiao( const BYTE cbCardIndex[MAX_INDEX], tagAnalyseItem *pAnalyseItem, tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, BYTE cbHuaCount, DWORD dwPreHuPaiKind )
{
	// ���������С��
	if (cbWeaveCount < MAX_WEAVE)
	{
		return false;
	}

	//���ͷ���
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

//�߶�
BYTE CGameLogic::IsQiDui(const BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard)
{
	//4:˫�߶Լ��껪 3:����˫�߶� 2:˫�߶� 1:�߶� 0:�����߶�
	if (cbWeaveCount > 0 || GetCardCount(cbCardIndex) != MAX_COUNT)
	{
		return 0;
	}

	// ��ǰ����(˫�߶ԣ��Լ�������ͬ�����ƣ����߶�ʱ��������)
	BYTE cbCurrentCardIndex = SwitchToCardIndex(cbCurrentCard);
	BYTE cbFourCardCount = 0, cbTwoCardCount = 0;

	for (BYTE i = 0; i < MAX_NORMAL; i++)
	{
		// ����
		if (cbCardIndex[i] == 0)
		{
			continue;
		}
		else if (cbCardIndex[i] % 2 == 1)
		{//���Ż�����
			return 0;
		}
		else if (cbCardIndex[i] == 2)
		{//����
			cbTwoCardCount++;
		}
		else if (cbCardIndex[i] == 4)
		{
			//����(������������)
			if (i == cbCurrentCardIndex)
			{
				cbFourCardCount++;
			}
			else
			{
				// ���Ǹ�ͷ���߶�
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

//��������
bool CGameLogic::IsDanDiaoJiang( const BYTE cbCardIndex[MAX_INDEX], tagAnalyseItem *pAnalyseItem, tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, BYTE cbHuaCount, DWORD dwPreHuPaiKind )
{
	ASSERT(pAnalyseItem!=NULL);
	//����
	BYTE cbEyeCard = pAnalyseItem->cbCardEye;
	//�Ƿ�ֻ��һ��
	bool bHuOneCard = IsOnlyHuOneCard(cbCardIndex, WeaveItem, cbWeaveCount, cbCurrentCard, cbHuaCount, dwPreHuPaiKind);
	if( !bHuOneCard) 
	{
		return false;
	}
	//�����ƾ�������
	return (pAnalyseItem->cbCardEye==cbCurrentCard)?true:false;
}

//ֻ��һ����
bool CGameLogic::IsOnlyHuOneCard( const BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount, BYTE cbCurrentCard, BYTE cbHuaCount, DWORD dwPreHuPaiKind )
{
	//������
	BYTE cbCardIndexTemp[MAX_INDEX] = {0};
	CopyMemory(cbCardIndexTemp, cbCardIndex, sizeof(BYTE)*MAX_INDEX);

	BYTE cbCurrentCardIndex = SwitchToCardIndex(cbCurrentCard);
	cbCardIndexTemp[cbCurrentCardIndex]--;

	for (BYTE i=0; i<MAX_NORMAL; i++)
	{
		//���Ʒ���
		BYTE cbReplaceCard = SwitchToCardData(i);
		if( cbCurrentCard == cbReplaceCard ) continue;  //�ų���ǰ������

		tagHuPaiResult HuPaiResult;
		DWORD dwHuPaiKind = AnalyseHuPai(cbCardIndexTemp, WeaveItem, cbItemCount, cbReplaceCard, cbHuaCount, dwPreHuPaiKind, HuPaiResult, false, false);
		
		if (dwHuPaiKind == WIK_HU)
		{
			return false;
		}
	}
	return true;
}

//�Ƿ��Ǵ��
bool CGameLogic::IsBigHuPaiKind( DWORD dwHuPaiKind )
{
	DWORD dwBigHuPaiKind = (HU_HUNYISE | HU_QINGYISE | HU_DUIDUIHU | HU_QQDUDIAO | HU_QIDUI | HU_SQIDUI | HU_TIANHU | HU_DIHU);
	return ((dwHuPaiKind&dwBigHuPaiKind)!=0)?true:false;
}

//�Ƿ����߶Ժ�����
bool CGameLogic::IsHuPaiQiDuiKind( DWORD dwHuPaiKind )
{
	DWORD dwHuPaiQiDuiKind = (HU_QIDUI|HU_SQIDUI);
	return ((dwHuPaiKind&dwHuPaiQiDuiKind)!=0)?true:false;
}

//��ת��
BYTE CGameLogic::SwitchToCardData( BYTE cbCardIndex )
{
	ASSERT(cbCardIndex<MAX_INDEX);

	//������
	if (cbCardIndex<MAX_NORMAL+3)
	{
		return ((cbCardIndex/9)<<4)|(cbCardIndex%9+1);
	}
	//����
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

//��ת��
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

//��ת��
BYTE CGameLogic::SwitchToCardData( BYTE cbCardIndex[MAX_INDEX], BYTE cbCardData[MAX_COUNT] )
{
	//ת����
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

//��ת��
BYTE CGameLogic::SwitchToCardIndex( BYTE cbCardData[], BYTE cbCardCount, BYTE cbCardIndex[MAX_INDEX] )
{
	//���ñ���
	ZeroMemory(cbCardIndex,sizeof(BYTE)*MAX_INDEX);

	//ת����
	for (BYTE i=0; i<cbCardCount; i++)
	{
		VERIFY(IsValidCard(cbCardData[i]));
		cbCardIndex[SwitchToCardIndex(cbCardData[i])]++;
	}

	return cbCardCount;
}
//////////////////////////////////////////////////////////////////////////