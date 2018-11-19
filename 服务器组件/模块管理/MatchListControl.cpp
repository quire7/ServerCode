#include "StdAfx.h"
#include "MatchListControl.h"

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CMatchListControl::CMatchListControl()
{
}

//��������
CMatchListControl::~CMatchListControl()
{
}

//�����б�
VOID CMatchListControl::InitListControl()
{
	//��������
	INT nColIndex=0;

	//�����б�
	InsertColumn(nColIndex++,TEXT("��������"),LVCFMT_LEFT,90);
	InsertColumn(nColIndex++,TEXT("��������"),LVCFMT_LEFT,90);
	InsertColumn(nColIndex++,TEXT("�۷�����"),LVCFMT_CENTER,80);
	InsertColumn(nColIndex++,TEXT("��������"),LVCFMT_CENTER,60);
	InsertColumn(nColIndex++,TEXT("��������"),LVCFMT_CENTER,60);

	return;
}

//��������
INT CMatchListControl::SortListItemData(LPARAM lParam1, LPARAM lParam2, WORD wColumnIndex)
{
	//��������
	tagGameMatchOption * pGameMatchOption1=(tagGameMatchOption *)lParam1;
	tagGameMatchOption * pGameMatchOption2=(tagGameMatchOption *)lParam2;

	//��������
	switch (wColumnIndex)
	{
	case 0:		//��������
		{
			return lstrcmp(pGameMatchOption1->szMatchName,pGameMatchOption2->szMatchName);
		}
	case 1:		//��������
		{
			return pGameMatchOption1->cbMatchType>pGameMatchOption2->cbMatchType?SORT_POSITION_AFTER:SORT_POSITION_FRONT;			
		}
	//case 1:		//��ʼ����
	//	{
	//		//����ʱ��
	//		CTime StartTime1(pGameMatchOption1->MatchStartTime);
	//		CTime StartTime2(pGameMatchOption2->MatchStartTime);
	//		return StartTime1>StartTime2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;;
	//	}
	//case 2:		//��������
	//	{
	//		//����ʱ��
	//		CTime EndTime1(pGameMatchOption1->MatchEndTime);
	//		CTime EndTime2(pGameMatchOption2->MatchEndTime);
	//		return EndTime1>EndTime2?SORT_POSITION_AFTER:SORT_POSITION_FRONT;;
	//	}
	case 2:		//�۷�����
		{
			return pGameMatchOption1->cbMatchFeeType<pGameMatchOption2->cbMatchFeeType?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
		}
	case 3:		//��������
		{
			return pGameMatchOption1->lMatchFee>pGameMatchOption2->lMatchFee?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
		}
	case 4:		//��������
		{
			return pGameMatchOption1->cbMemberOrder>pGameMatchOption2->cbMemberOrder?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
		}
	//case 6:		//��̭����
	//	{
	//		return pGameMatchOption1->lMatchCullScore>pGameMatchOption2->lMatchCullScore?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
	//	}
	//case 7:		//��Ϸ����
	//	{
	//		return pGameMatchOption1->wMatchPlayCount>pGameMatchOption2->wMatchPlayCount?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
	//	}
	//case 8:		//��������
	//	{
	//		return lstrcmp(pGameMatchOption1->szMatchAwardContent,pGameMatchOption2->szMatchAwardContent);
	//	}
	//case 9:		//��������
	//	{
	//		return lstrcmp(pGameMatchOption1->szMatchContent,pGameMatchOption2->szMatchContent);
	//	}
	}

	return 0;
}

//��ȡ��ɫ
VOID CMatchListControl::GetListItemColor(LPARAM lItemParam, UINT uItemStatus, tagListItemColor & ListItemColor)
{
	//��������
	ASSERT(lItemParam!=NULL);

	//������ɫ
	ListItemColor.rcBackColor=(uItemStatus&ODS_SELECTED)?RGB(0,0,128):CR_NORMAL_BK;
	ListItemColor.rcTextColor=(uItemStatus&ODS_SELECTED)?RGB(255,255,255):RGB(0,0,0);

	return;
}

//�����б�
bool CMatchListControl::InsertMatchOption(tagGameMatchOption * pGameMatchOption)
{
	//��������
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//���ñ���
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pGameMatchOption;

	//�����ж�
	INT nInsertItem=FindItem(&FindInfo);
	if (nInsertItem!=LB_ERR) return true;

	//�����б�
	for (WORD i=0;i<m_ListHeaderCtrl.GetItemCount();i++)
	{
		if (i==0)
		{
			//��������
			INT nIndex=GetInsertIndex(pGameMatchOption);
			LPCTSTR pszDescribe=GetDescribeString(pGameMatchOption,i);
			nInsertItem=InsertItem(LVIF_TEXT|LVIF_PARAM,nIndex,pszDescribe,0,0,0,(LPARAM)pGameMatchOption);
		}
		else
		{
			//�ַ�����
			SetItem(nInsertItem,i,LVIF_TEXT,GetDescribeString(pGameMatchOption,i),0,0,0,0);
		}
	}

	return true;
}

//��������
WORD CMatchListControl::GetInsertIndex(tagGameMatchOption * pGameMatchOption)
{
	//��������
	INT nItemCount=GetItemCount();
	tagGameMatchOption * pGameMatchTemp=NULL;

	//��ȡλ��
	for (INT i=0;i<nItemCount;i++)
	{
		//��ȡ����
		pGameMatchTemp=(tagGameMatchOption *)GetItemData(i);

		//��������
		if (lstrcmp(pGameMatchOption->szMatchName,pGameMatchTemp->szMatchName)<0)
		{
			return i;
		}
	}

	return nItemCount;
}

//�����ַ�
LPCTSTR CMatchListControl::GetDescribeString(tagGameMatchOption * pGameMatchOption, WORD wColumnIndex)
{
	//��������
	static TCHAR szDescribe[128]=TEXT("");

	//�����ַ�
	switch (wColumnIndex)
	{
	case 0:		//��������
		{
			return pGameMatchOption->szMatchName;
		}
	case 1:		//��������
		{
			//��������
			LPCTSTR pszMatchType[] = {TEXT("��ʱ��"),TEXT("��ʱ��")};

			//�ж�����
			if(pGameMatchOption->cbMatchType<CountArray(pszMatchType))
			{
				_sntprintf(szDescribe,CountArray(szDescribe),TEXT("%s"),pszMatchType[pGameMatchOption->cbMatchType]);
			}

			return szDescribe;
		}
	case 2:		//��������
		{
			if(pGameMatchOption->cbMatchFeeType==MATCH_FEE_TYPE_GOLD)
			{
				_sntprintf(szDescribe,CountArray(szDescribe),TEXT("���"));
			}
			else if(pGameMatchOption->cbMatchFeeType==MATCH_FEE_TYPE_MEDAL)
			{
				_sntprintf(szDescribe,CountArray(szDescribe),TEXT("����"));
			}

			return szDescribe;
		}
	case 3:		//��������
		{
			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("%d"),pGameMatchOption->lMatchFee);
			return szDescribe;
		}
	case 4:		//��������
		{
			//��������
			LPCTSTR pszMemberOrder[] = { TEXT("�������"), TEXT("�»�Ա"), TEXT("���Ȼ�Ա"), TEXT("�����Ա") };

			//�ж�����
			if(pGameMatchOption->cbMemberOrder<CountArray(pszMemberOrder))
			{
				_sntprintf(szDescribe,CountArray(szDescribe),TEXT("%s"),pszMemberOrder[pGameMatchOption->cbMemberOrder]);		
			}

			return szDescribe;
		}
	//case 1:		//��ʼ����
	//	{
	//		CTime StartTime= CTime(pGameMatchOption->MatchStartTime);
	//		lstrcpyn(szDescribe,StartTime.Format(TEXT("%Y-%m-%d %H:%M:%S")),CountArray(szDescribe));
	//		return szDescribe;
	//	}
	//case 2:		//��������
	//	{
	//		CTime EndTime= CTime(pGameMatchOption->MatchEndTime);
	//		lstrcpyn(szDescribe,EndTime.Format(TEXT("%Y-%m-%d  %H:%M:%S")),CountArray(szDescribe));
	//		return szDescribe;
	//	}
	//case 3:		//��������
	//	{
	//		if(pGameMatchOption->cbMatchFeeType==MATCH_FEE_TYPE_GOLD)
	//		{
	//			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("���"));
	//		}
	//		else if(pGameMatchOption->cbMatchFeeType==MATCH_FEE_TYPE_MEDAL)
	//		{
	//			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("����"));
	//		}
	//		return szDescribe;
	//	}
	//case 4:		//��������
	//	{
	//		_sntprintf(szDescribe,CountArray(szDescribe),TEXT("%d"),pGameMatchOption->lMatchFee);
	//		return szDescribe;
	//	}
	//case 5:		//��ʼ����
	//	{
	//		_sntprintf(szDescribe,CountArray(szDescribe),TEXT("%d"),pGameMatchOption->lMatchInitScore);
	//		return szDescribe;
	//	}
	//case 6:		//��̭����
	//	{
	//		_sntprintf(szDescribe,CountArray(szDescribe),TEXT("%d"),pGameMatchOption->lMatchCullScore);
	//		return szDescribe;
	//	}
	//case 7:		//��Ϸ����
	//	{
	//		_sntprintf(szDescribe,CountArray(szDescribe),TEXT("%d"),pGameMatchOption->wMatchPlayCount);
	//		return szDescribe;
	//	}
	//case 8:		//��������
	//	{
	//		return pGameMatchOption->szMatchAwardContent;
	//	}
	//case 9:		//��������
	//	{
	//		return pGameMatchOption->szMatchContent;
	//	}
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////////////
