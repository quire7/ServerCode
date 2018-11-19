#pragma once

#include "resource.h"

class CWebServerApp : public CWinApp
{
public:
	CWebServerApp();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	afx_msg void OnAppExit();

	DECLARE_MESSAGE_MAP()
};

