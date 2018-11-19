#ifndef _SYSTEM_OPTION_DIALOG_H_
#define _SYSTEM_OPTION_DIALOG_H_

#pragma once

#include "Stdafx.h"
#include "resource.h"
#include "InitParameter.h"

//////////////////////////////////////////////////////////////////////////

//配置对话框
class CSystemOptionDlg : public CDialog
{
public:
	CSystemOptionDlg();
	virtual ~CSystemOptionDlg();

	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();
	//控件子类化
	virtual void DoDataExchange(CDataExchange * pDX);
	//确定函数
	virtual void OnOK();

	//功能函数
public:
	//配置参数
	void SetInitParameter(const CInitParameter & InitParameter);

	//消息映射
	DECLARE_MESSAGE_MAP()
};


#endif