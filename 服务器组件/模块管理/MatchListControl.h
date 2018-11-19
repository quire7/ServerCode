#ifndef MATCH_LIST_CONTROL_HEAD_FILE
#define MATCH_LIST_CONTROL_HEAD_FILE

#pragma once

#include "ListControl.h"
#include "ModuleManagerHead.h"

//////////////////////////////////////////////////////////////////////////////////

//�����б�
class MODULE_MANAGER_CLASS CMatchListControl : public CListControl
{
	//��������
public:
	//���캯��
	CMatchListControl();
	//��������
	virtual ~CMatchListControl();

	//���غ���
public:
	//�����б�
	virtual VOID InitListControl();
	//��������
	virtual INT SortListItemData(LPARAM lParam1, LPARAM lParam2, WORD wColumnIndex);
	//��ȡ��ɫ
	virtual VOID GetListItemColor(LPARAM lItemParam, UINT uItemStatus, tagListItemColor & ListItemColor);

	//���ܺ���
public:
	//�����б�
	bool InsertMatchOption(tagGameMatchOption * pGameMatchOption);

	//��������
private:
	//��������
	WORD GetInsertIndex(tagGameMatchOption * pGameMatchOption);
	//�����ַ�
	LPCTSTR GetDescribeString(tagGameMatchOption * pGameMatchOption, WORD wColumnIndex);
};

//////////////////////////////////////////////////////////////////////////////////

#endif