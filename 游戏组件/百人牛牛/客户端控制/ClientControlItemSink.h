#pragma once
#include "../�ͻ������/ClientControl.h"

#define CONTROL_AREA		4
// CClientControlItemSinkDlg �Ի���

class AFX_EXT_CLASS CClientControlItemSinkDlg : public IClientControlDlg
{
	DECLARE_DYNAMIC(CClientControlItemSinkDlg)

protected:
	bool m_bWinArea[CONTROL_AREA];
	BYTE m_cbControlStyle;
	BYTE m_cbExcuteTimes;

	//���¿��
	LONGLONG m_lStorage;
	LONGLONG m_lDeduct;

public:
	CClientControlItemSinkDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CClientControlItemSinkDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_ADMIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	//���¿���
	virtual void  OnAllowControl(bool bEnable);
	//������
	virtual bool  ReqResult(const void * pBuffer);

	//���¿��
	virtual bool  UpdateStorage(const void * pBuffer);
	//���¿ؼ�
	virtual void  UpdateControl();

public:
	//���ý���
	void ReSetAdminWnd();

public:
	//��ʾ״̬
	void PrintCurStatus();

public:
	afx_msg void  OnReSet();
	afx_msg void  OnRefresh();
	afx_msg void  OnExcute();
	afx_msg void  OnRadioClick();
protected:
	virtual void OnCancel();
	void RequestUpdateStorage();
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedBtnUpdateStorage();
};
