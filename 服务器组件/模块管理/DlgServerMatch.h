#ifndef DLG_MATCH_HEAD_FILE
#define DLG_MATCH_HEAD_FILE

#pragma once

//�����ļ�
#include "ModuleManagerHead.h"
#include "ServerInfoManager.h"
#include "MatchListControl.h"

//////////////////////////////////////////////////////////////////////////////////////////////////

//���Ի���
class MODULE_MANAGER_CLASS CDlgServerMatch : public CDialog
{
	//��ʶ����
protected:
	WORD							m_wKindID;								//���ͱ�ʶ

	//�б����
protected:
	CMatchOptionBuffer				m_MatchOptionBuffer;					//ģ����Ϣ
	CMatchListControl				m_MatchListControl;						//�����б�

	//���ñ���
public:
	tagGameMatchOption				m_GameMatchOption;						//��������

	//��������
public:
	//���캯��
	CDlgServerMatch();
	//��������
	virtual ~CDlgServerMatch();

	//�̳к���
protected:
	//��ʼ����
	virtual BOOL OnInitDialog();
	//���ݽ���
	virtual void DoDataExchange(CDataExchange* pDX);

	//���ú���
public:
	//�򿪱���
	bool OpenGameMatch(WORD wKindID);

	//���ܺ���
protected:
	//���ط���
	bool LoadDBMatchItem();

	//��ť��Ϣ
protected:
	//���ط���
	VOID OnBnClickedLoadMatch();

	//�ؼ���Ϣ
protected:
	//˫���б�
	VOID OnNMDblclkServerList(NMHDR * pNMHDR, LRESULT * pResult);
	//ѡ��ı�
	VOID OnLvnItemChangedServerList(NMHDR * pNMHDR, LRESULT * pResult);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////////////////////
#endif