#ifndef YS_COMMAND_LINE_HEAD_FILE
#define YS_COMMAND_LINE_HEAD_FILE

#pragma once

#include "ServiceCoreHead.h"

//////////////////////////////////////////////////////////////////////////////////

//�������
class SERVICE_CORE_CLASS CYSCommandLine
{
	//��������
public:
	//���캯��
	CYSCommandLine();
	//��������
	virtual ~CYSCommandLine();

	//���ܺ���
public:
	//��ѯ����
	bool SearchCommandItem(LPCTSTR pszCommandLine, LPCTSTR pszCommand, TCHAR szParameter[], WORD wParameterLen);
};

//////////////////////////////////////////////////////////////////////////////////

#endif