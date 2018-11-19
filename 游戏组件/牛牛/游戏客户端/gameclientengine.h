#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "GameClientView.h"


//////////////////////////////////////////////////////////////////////////

//��Ϸ�Ի���
class CGameClientEngine : public CGameFrameEngine
{
	//���ñ���
protected:
	DWORD							m_dwCardHSpace;							//�˿�����
	WORD							m_wViewChairID[GAME_PLAYER];			//��ͼλ��

	//��ע��Ϣ
protected:
	LONGLONG						m_lTurnMaxScore;						//�����ע
	LONGLONG						m_lTableScore[GAME_PLAYER];				//��ע��Ŀ

	//״̬����
protected:
	WORD							m_wBankerUser;							//ׯ���û�
	BYTE							m_cbPlayStatus[GAME_PLAYER];			//��Ϸ״̬
	TCHAR							m_szNickNames[GAME_PLAYER][LEN_NICKNAME];	//�������

	//�û��˿�
protected:
	BYTE							m_cbHandCardData[GAME_PLAYER][MAX_COUNT];//�û�����
	BYTE							m_bUserOxCard[GAME_PLAYER];				//ţţ����								
	//�ؼ�����
public:
	CGameLogic						m_GameLogic;							//��Ϸ�߼�
	CGameClientView					m_GameClientView;						//��Ϸ��ͼ
	CGlobalUnits *                  m_pGlobalUnits;                         //ȫ�ֵ�Ԫ

	//��������
public:
	//���캯��
	CGameClientEngine();
	//��������
	virtual ~CGameClientEngine();

	//����̳�
private:
	//��ʼ����
	virtual bool OnInitGameEngine();
	//���ÿ��
	virtual bool OnResetGameEngine();
	//��Ϸ����
	virtual void OnGameOptionSet();

    //ʱ���¼�
public:
	//ʱ��ɾ��
	virtual bool OnEventGameClockKill(WORD wChairID);
	//ʱ����Ϣ
	virtual bool OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD wClockID);

	//��Ϸ�¼�
public:
	//�Թ���Ϣ
	virtual bool OnEventLookonMode(VOID * pData, WORD wDataSize);
	//��Ϸ��Ϣ
	virtual bool OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//������Ϣ
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize);

	//�����ӿ�
public:
	//��������
	virtual bool AllowBackGroundSound(bool bAllowSound){ return true; }

	//��Ϣ����
protected:
	//�û���ׯ
	bool OnSubCallBanker(const void * pBuffer, WORD wDataSize);
	//��Ϸ��ʼ
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//�û���ע
	bool OnSubAddScore(const void * pBuffer, WORD wDataSize);
	//������Ϣ
	bool OnSubSendCard(const void * pBuffer, WORD wDataSize);
	//�û�̯��
	bool OnSubOpenCard(const void * pBuffer, WORD wDataSize);
	//�û�ǿ��
	bool OnSubPlayerExit(const void * pBuffer, WORD wDataSize);
	//��Ϸ����
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);

	//��������
protected:
	//���¿���
	void UpdateScoreControl(LONGLONG lScore[],BOOL bShow);
	//�Ƿ�ǰ�����û�
	bool IsCurrentUser(WORD wCurrentUser){return (!IsLookonMode() && wCurrentUser==GetMeChairID());}

	//��Ϣӳ��
protected:
	//��ʾ��ť
	LRESULT	OnHintOx(WPARAM wParam, LPARAM lParam);
	//��ʼ��ť
	LRESULT	OnStart(WPARAM wParam, LPARAM lParam);
	//��ע��ť 
	LRESULT	OnAddScore(WPARAM wParam, LPARAM lParam);
	//ţţ��ť
	LRESULT	OnOx(WPARAM wParam, LPARAM lParam);
	//��ݼ���ť 
	LRESULT OnShortcut(WPARAM wParam, LPARAM lParam);
	//̯�ư�ť
	LRESULT	OnOpenCard(WPARAM wParam, LPARAM lParam);
	//�������
	LRESULT OnSendCardFinish(WPARAM wParam, LPARAM lParam);
	//��ʾ����
	LRESULT OnSetUserOxValue(WPARAM wParam, LPARAM lParam);
	//��ʾ��Ϣ
	LRESULT OnShowInfo(WPARAM wParam, LPARAM lParam);
	//ţBTΪ��
	LRESULT OnOxEnable(WPARAM wParam, LPARAM lParam);
	//�˿˷���
	LRESULT OnSortCard(WPARAM wParam, LPARAM lParam);
	//�˿˷���
	LRESULT OnReSortCard(WPARAM wParam, LPARAM lParam);
	//��ׯ��Ϣ
	LRESULT OnBanker(WPARAM wParam, LPARAM lParam);
	//�����Ϣ
	LRESULT OnClickCard(WPARAM wParam, LPARAM lParam);

public:
	afx_msg void OnTimer(UINT nIDEvent);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
