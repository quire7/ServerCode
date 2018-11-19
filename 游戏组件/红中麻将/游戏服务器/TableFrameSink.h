#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//ö�ٶ���

//Ч������
enum enEstimatKind
{
	EstimatKind_OutCard,			//����Ч��;
	EstimatKind_GangCard,			//����Ч��;
};

//���Ƶ÷�
struct tagGangScore
{
	BYTE		cbGangCount;							//�ܸ���;
	LONGLONG	lScore[MAX_WEAVE][GAME_PLAYER];			//ÿ���ܵ÷�;
};

//////////////////////////////////////////////////////////////////////////

//��Ϸ������
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//��Ϸ����
protected:
	WORD							m_wBankerUser;							//ׯ���û�;
	WORD							m_wEastUser;							//�״ζ���;
	LONGLONG						m_lGameScore[GAME_PLAYER];				//��Ϸ�÷�;
	BYTE							m_cbCardIndex[GAME_PLAYER][MAX_INDEX];	//�û��˿�;
	bool							m_bTrustee[GAME_PLAYER];				//�Ƿ��й�;
	tagGangScore					m_GangScore[GAME_PLAYER];				//

	WORD							m_wLostFanShu[GAME_PLAYER][GAME_PLAYER];//


	BYTE							m_cbCardDataNiao[MAX_NIAO_CARD];		// ����;
	BYTE							m_cbNiaoCount;							//���Ƹ���;
	BYTE							m_cbNiaoPick;							//�������;
	BYTE							m_cbOneShootScore;						//һ�����ط���;

	bool							m_bCanTrustee;							//�Ƿ��й�;

	//������Ϣ
protected:
	WORD							m_wOutCardUser;							//�����û�;
	BYTE							m_cbOutCardData;						//�����˿�;
	BYTE							m_cbOutCardCount;						//������Ŀ;
	BYTE							m_cbDiscardCount[GAME_PLAYER];			//������Ŀ;
	BYTE							m_cbDiscardCard[GAME_PLAYER][55];		//������¼;

	//������Ϣ
protected:
	BYTE							m_cbSendCardData;						//�����˿�;
	BYTE							m_cbSendCardCount;						//������Ŀ;
	BYTE							m_cbLeftCardCount;						//ʣ����Ŀ;
	BYTE							m_cbRepertoryCard[MAX_REPERTORY];		//����˿�;
	BYTE							m_cbRepertoryCard_HZ[MAX_REPERTORY_HZ];	//����˿�;

	BYTE                            m_cbMasterCheckCard[GAME_PLAYER];		//���Ҫ��;

	//���б���
protected:
	WORD							m_wResumeUser;							//��ԭ�û�;
	WORD							m_wCurrentUser;							//��ǰ�û�;
	WORD							m_wProvideUser;							//��Ӧ�û�;
	BYTE							m_cbProvideCard;						//��Ӧ�˿�;

	//״̬����
protected:
	bool							m_bSendStatus;							//����״̬;
	bool							m_bGangStatus;							//����״̬;
	bool							m_bGangOutStatus;						//

	//�û�״̬
public:
	bool							m_bResponse[GAME_PLAYER];				//��Ӧ��־;
	DWORD							m_cbUserAction[GAME_PLAYER];			//�û�����;
	BYTE							m_cbOperateCard[GAME_PLAYER];			//�����˿�;
	DWORD							m_cbPerformAction[GAME_PLAYER];			//ִ�ж���;

	LONGLONG						m_lStartHuScore[GAME_PLAYER];			//���ֺ�����;
	CChiHuRight						m_StartHuRight[GAME_PLAYER];			//���ֺ�Ȩλ;

	tagTingPaiResult				m_TingPaiResult[GAME_PLAYER];			//���ƽ��;

	//����˿�
protected:
	BYTE							m_cbWeaveItemCount[GAME_PLAYER];		//�����Ŀ;
	tagWeaveItem					m_WeaveItemArray[GAME_PLAYER][MAX_WEAVE];//����˿�;

	//������Ϣ
protected:
	BYTE							m_cbChiHuCard;							//�Ժ��˿�;
	DWORD							m_dwChiHuKind[GAME_PLAYER];				//�Ժ����;
	CChiHuRight						m_ChiHuRight[GAME_PLAYER];				//
	WORD							m_wProvider[GAME_PLAYER];				//

	//�������
protected:
	CGameLogic						m_GameLogic;							//��Ϸ�߼�;
	ITableFrame						* m_pITableFrame;						//��ܽӿ�;
	const tagGameServiceOption		* m_pGameServiceOption;					//���ò���;

	BYTE							m_cbGameTypeIdex;						//��Ϸ����;
	DWORD							m_dwGameRuleIdex;						//��Ϸ����;
	BYTE							m_cbMagicCardIndex;
	BYTE							m_cbMagicCardData;

	//��������;
public:
	//���캯��;
	CTableFrameSink();
	//��������;
	virtual ~CTableFrameSink();

	//�����ӿ�;
public:
	//�ͷŶ���;
	virtual VOID Release() { }
	//�ӿڲ�ѯ;
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//����ӿ�;
public:
	//��ʼ��;
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//��λ����;
	virtual VOID RepositionSink();

	//��ѯ�ӿ�;
public:
	//��ѯ�޶�;
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem){  return 0; };
	//���ٻ���;
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem){ return 0; };
	//��ѯ�Ƿ�۷����;
	virtual bool QueryBuckleServiceCharge(WORD wChairID){return true;}

	//�����ӿ�;
public:
	//���û���;
	virtual void SetGameBaseScore(LONG lBaseScore){};

	//��Ϸ�¼�;
public:
	//��Ϸ��ʼ;
	virtual bool OnEventGameStart();
	//��Ϸ����;
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//���ͳ���;
	virtual bool OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	void Shuffle(BYTE* RepertoryCard,int nCardCount); //ϴ��;

	//�¼��ӿ�;
public:
	//��ʱ���¼�;
	virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
	//�����¼�;
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize) { return false; }
	//�����¼�;
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason) { return false; }

	//����ӿ�;
public:
	//��Ϸ��Ϣ����;
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//�����Ϣ����;
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//�û��¼�;
public:
	//�û�����;
	virtual bool OnActionUserOffLine(WORD wChairID,IServerUserItem * pIServerUserItem) { return true; }
	//�û�����;
	virtual bool OnActionUserConnect(WORD wChairID,IServerUserItem * pIServerUserItem) { return true; }
	//�û�����;
	virtual bool OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�����;
	virtual bool OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�ͬ��;
	virtual bool OnActionUserOnReady(WORD wChairID,IServerUserItem * pIServerUserItem, void * pData, WORD wDataSize) { return true; }

	//��Ϸ�¼�;
protected:
	//�û�����;
	bool OnUserOutCard(WORD wChairID, BYTE cbCardData);
	//�û�����;
	bool OnUserOperateCard(WORD wChairID, BYTE cbOperateCode, BYTE cbOperateCard, bool bCheckQiangGangHu = true);
	//�û��й�;
	bool OnUserTrustee(WORD wChairID, bool bTrustee);
	
public:
	bool hasRule(DWORD cbRule);

	BYTE AnalyseChiHuCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, CChiHuRight &ChiHuRight);

	//�жϺʹ���4�����е����;
	BYTE AnalyseChiHuCardZZ_XIAOHU(const BYTE cbCardIndex[MAX_INDEX], CChiHuRight &ChiHuRight);

	BYTE AnalyseChiHuCardCS_XIAOHU(const BYTE cbCardIndex[MAX_INDEX], CChiHuRight &ChiHuRight);

	bool DisposeZZXiaohu(const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//˽�˳��ӿ�;
public:
	//����˽�˳���Ϣ;
	virtual void SetPrivateInfo(BYTE bGameTypeIdex, DWORD bGameRuleIdex);
	//����˽�˳�������Ϣ;
	virtual void ResetPrivateEndInfo();
	//���˽�˳�������Ϣ��;
	virtual void GetPrivateEndInfo(DataStream &kDataStream, bool bSend);
	//�ж�˽�˳��Ƿ����;
	virtual	bool IsPrivateEnd();

	//��������;
protected:
	//���Ͳ���;
	bool SendOperateNotify();
	//�ɷ��˿�;
	bool DispatchCardData(WORD wCurrentUser,bool bTail=false);
	//��Ӧ�ж�;
	bool EstimateUserRespond(WORD wCenterUser, BYTE cbCenterCard, enEstimatKind EstimatKind);

	//
	void ProcessChiHuUser( WORD wChairId, bool bGiveUp );
	//
	void FiltrateRight( WORD wChairId,CChiHuRight &chr );

	//���������(������);
	BYTE GetNiaoCardNum(WORD wHuChairID );
	void setNiaoCard( WORD wHuChairID );

	//�Ƿ�����к�����;
	bool HasHongZhongCard( WORD wChairID );
	//����һ�����صĵ���;
	BYTE CalcOneShootScore( BYTE cbCardData );

	//�Ƿ�����,���ƺ󼰳�����,�����;
	bool IsTingPai(WORD wChairID);

	// ��ȡҪ��;
	BYTE GetSendCardData(WORD dwSendUser);
};

//////////////////////////////////////////////////////////////////////////

#endif
