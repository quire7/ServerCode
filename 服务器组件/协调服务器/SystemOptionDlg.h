#ifndef _SYSTEM_OPTION_DIALOG_H_
#define _SYSTEM_OPTION_DIALOG_H_

#pragma once

#include "Stdafx.h"
#include "resource.h"
#include "InitParameter.h"

//////////////////////////////////////////////////////////////////////////

//���öԻ���
class CSystemOptionDlg : public CDialog
{
public:
	CSystemOptionDlg();
	virtual ~CSystemOptionDlg();

	//���غ���
protected:
	//��ʼ������
	virtual BOOL OnInitDialog();
	//�ؼ����໯
	virtual void DoDataExchange(CDataExchange * pDX);
	//ȷ������
	virtual void OnOK();

	//���ܺ���
public:
	//���ò���
	void SetInitParameter(const CInitParameter & InitParameter);

	//��Ϣӳ��
	DECLARE_MESSAGE_MAP()
};


#endif