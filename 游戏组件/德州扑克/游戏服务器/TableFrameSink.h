#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include <list>
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//��Ϸ������
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//��ұ���
protected:
	WORD							m_wDUser;								//D���
	WORD							m_wCurrentUser;							//��ǰ���
	WORD                            m_wMUser[2];                            //äע���

	//���״̬
protected:
	BYTE							m_cbPlayStatus[GAME_PLAYER];			//��Ϸ״̬
	BYTE							m_cbGiveUpStatus[GAME_PLAYER];			//����״̬

	//��ע��Ϣ
protected:
	LONG							m_lCellScore;							//��Ԫ��ע
	LONG							m_lTurnLessScore;						//��С��ע
	LONG							m_lAddLessScore;						//����Сע
	LONG							m_lTurnMaxScore[GAME_PLAYER];			//�����ע
	LONG							m_lBalanceScore;						//ƽ����ע
	LONG                            m_lAllScore;							//����ע
	LONG							m_lTableScore[GAME_PLAYER];				//������ע
	LONG							m_lTotalScore[GAME_PLAYER];				//�ۼ���ע
	LONG							m_lUserMaxScore[GAME_PLAYER];			//�����ע  ����ܵĴ�����
	LONG                            m_TabelScoreBuffer[GAME_PLAYER];        //������ע
	LONG                            m_aryChipPool[GAME_PLAYER + 1];			//�߳س���
	LONG                            m_aryEndScore[GAME_PLAYER];				//��������
	//LONG                            m_OnPoolBuffer;							
	WORD							m_wOperaCount;							//��������	
	BYTE                            m_ChipPoolCount;                        //�߳�����  
	BYTE                            m_SuoHaCountBuffer[4];                  //ÿһ��������������
	BYTE							m_cbShowHand[GAME_PLAYER];				//����û�
	BYTE							m_cbBalanceCount;						//��Ϸ���ڼ���
	BYTE                            m_cbChipTypeID;							//��ע����
	BYTE							m_cbHandChair[GAME_PLAYER][GAME_PLAYER];//ÿ�������в�������
	BYTE							m_cbHandChairCount[GAME_PLAYER];		//ÿ�������в�����������

	//���ò���
	static LONG                     m_ISetDJPK[9];
	bool                            m_bFristPlayStatus[GAME_PLAYER];
	bool                            m_cbShowHandBuffer[GAME_PLAYER];
	bool                            m_GameOver;								//��Ϸ����
	bool                            m_SendCardEnd[GAME_PLAYER];             //���ƽ���	
	LONG                            m_UserAllScore;
	int                             m_UserWinOk;                            //�Ƿ��������ƻ���
	int                             m_nPassCardCount;                       //һȦ��ע������������

	//���Ӵ�����������
	//int                             m_lAddTakeMinTimes;    					//���Ӵ�������С�ޱ���
	//int                             m_lAddTakeMaxTimes;    					//���Ӵ���������ޱ���
	float                           m_fTableVipLimit;      					//ר������ 9.5��

	LONG                            m_lUserTakeGold[GAME_PLAYER];			//��Ҵ���������
	//int                             m_nDaShangCount[GAME_PLAYER];			//��Ҵ��ʹ���

	//��ע����
	WORD                            m_pHostChairID;
	bool                            m_bGameStart;                           //��Ϸ��ʼ��
	BYTE                            m_bAgreeBase[GAME_PLAYER];              //ͬ���������õĵ�ע
	LONG                            m_lBaseScore;                           //��Ϸ��ע
	typedef std::list<WORD>::iterator IteListPlayer;
	std::list<WORD>					m_listPlayers;                          //��ǰ�����ķ��Թ����
	std::list<WORD>					m_listConfirmBase;                       //��ǰ��������Ҫȷ�ϵ�ע�����

	CMD_S_Robot						m_GameRobot;

	//�˿���Ϣ
protected:
	BYTE							m_cbSendCardCount;						//������Ŀ
	BYTE							m_cbCenterCardData[MAX_CENTERCOUNT];	//�����˿�
	BYTE							m_cbHandCardData[GAME_PLAYER][MAX_COUNT];//�����˿�

	//�������
protected:
	CGameLogic						m_GameLogic;							//��Ϸ�߼�
	ITableFrame						* m_pITableFrame;						//��ܽӿ�
	const tagGameServiceOption		* m_pGameServiceOption;					//���ò���

	//���Ա���
protected:
	static const WORD				m_wPlayerCount;							//��Ϸ����

	//��������
public:
	//���캯��
	CTableFrameSink();
	//��������
	virtual ~CTableFrameSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//����ӿ�
public:
	//��ʼ��
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//��λ����
	virtual void RepositionSink();

	//��ѯ�ӿ�
public:
	//��ѯ�޶�
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem) { return 0; }
	//���ٻ���
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem) { return 0; }
	//��ѯ�Ƿ�۷����
	virtual bool QueryBuckleServiceCharge(WORD wChairID) { return true; }
	//�����¼�
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize){ return false; }
	//�����¼�
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason){ return false; }
	//���û���
	virtual void SetGameBaseScore(LONG lBaseScore) { }

	//��Ϣ�ӿ�
public:
	//��Ϸ״̬
	virtual bool IsUserPlaying(WORD wChairID);
	//������Ϸ���������
	virtual SCORE GetGameLockGold(IServerUserItem * pIServerUserItem);
	//�����¼� 
public:
	//�û�����
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem);
	//�û�����
	virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//�û�����
	virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�����
	virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�ͬ��
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);
	//��Ϸ�¼�
public:
	//��Ϸ��ʼ
	virtual bool OnEventGameStart();
	//��Ϸ����
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//���ͳ���
	virtual bool OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	//��Ϣ�¼�
protected:
	//�����¼�
	bool OnUserGiveUp(WORD wChairID);
	//��ע�¼�
	bool OnUserAddScore(WORD wChairID, LONG lScore, bool bGiveUp);
	//��������
	bool OnPlayEnd(WORD wChairID, BYTE PlayId);
	//������Ӵ�����
	bool OnUserAddTakeGold(WORD wChairID, LONG lAddTake);
	//�¼��ӿ�
public:
	//��ʱ���¼�
	virtual bool OnTimerMessage(DWORD dwTimerID, WPARAM wBindParam);
	//��Ϸ��Ϣ����
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//�����Ϣ����
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//��������
public:
	//��ȡ�����ļ�
	BOOL SetDzpkInit(BYTE ID = 0);
	//������
	bool CardExchange(BYTE ID);
	//ɱ�����ж�ʱ��
	void KillTimerAll();
	//һ����ע��ɺ����
	bool OnChipRendEnd();
	//д����ʵ���������
	bool WriteAllScore(LONG UserScore[], LONG Tax[]);
	//������ҵ������ע
	void CalUserMaxChipGold(WORD wChairID);
	//��������ô�������С�������
	void CalRangeTakeGold(WORD wChairID, LONG & lMin, LONG & lMax);

	//��ע���
	bool IsGenreGold(void);																	//��ҳ����õ�ע ���Ҳ��ǹ̶���ע
	void DoUserSitDown(IServerUserItem * pIServerUser);										//����������� ����ҷ���ȷ�ϵ�ע
	bool SendSetBaseScore(IServerUserItem * pIServerUser, BYTE bSetBaseScore = 1);			// 1 ���� 0 �ȴ� 2 ȷ��
	bool CheckAgreeBase();																	//��û��ȷ�ϵ�ע����� ����ȷ����Ϣ
	bool CheckMoneyLimit(LONG lScoreLimit);													//�����ҵ�Ǯ�Ƿ���С��Ϸ������
	void SendKickOutMsg(WORD ChairID);
	bool OnUserSetBase(CMD_C_SetBaseScore* pBaseScore, IServerUserItem * pIServerUserItem);	//���õ�ע �ͻ�����Ӧ
	bool OnUserDaShang(WORD wChairID);

	bool OnTimerKickOut();

	//˽�˳��ӿ�;
public:
	//����˽�˳���Ϣ;
	virtual void SetPrivateInfo(BYTE bGameTypeIdex, DWORD	bGameRuleIdex) {};
	//����˽�˳�������Ϣ;
	virtual void ResetPrivateEndInfo() {};
	//���˽�˳�������Ϣ��;
	virtual void GetPrivateEndInfo(DataStream &kDataStream, bool bSend) {};
	//�ж�˽�˳��Ƿ����;
	virtual	bool IsPrivateEnd() { return false; };

};

//////////////////////////////////////////////////////////////////////////

#endif