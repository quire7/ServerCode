#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"

#include <math.h>
#include <fstream>
#include <list>
#include <algorithm>
using namespace std;
//////////////////////////////////////////////////////////////////////////
//ö�ٶ���
//Ч������
enum enEstimatKind
{
	EstimatKind_SendCard,			//����Ч��
	EstimatKind_OutCard,			//����Ч��
	EstimatKind_ChiPengCard,		//������Ч��
	EstimatKind_GangCard,			//����Ч��
	EstimatKind_BuGangCard,			//������Ч��
	EstimatKind_FirstSend,			//���ַ���
};

enum enGANG_STATUS
{
	GS_NULL = 0,					//��ʼ;
	GS_XIAOGANGKAIHUA = 1,			//С�ܿ���;
	GS_DAGANGKAIHUA = 2,			//��ܿ���;
};

//���ܺ����
struct tagQiangGangHu
{
	WORD		wGangPaiUser;		//�������
	BYTE		cbGangPaiCard;		//����ʱ����
	WORD		wQiangGangHuUser[3];//���ܺ����
	void		ReInit()
	{
		wGangPaiUser = INVALID_CHAIR;
		cbGangPaiCard = 0;
		memset(wQiangGangHuUser, INVALID_CHAIR, sizeof(wQiangGangHuUser));
	}
};

////���ƽ��
//struct tagBaoPaiResult
//{
//	bool	    bBaoPaiStatus;		//����״̬
//	BYTE		cbCurUserIndex;		//����ʱ����ǰ����
//	WORD		wTargetUser[3];		//����ʱ���������ID
//	BYTE		cbBaoPaiType[3];	//����ʱ����������
//};

//////////////////////////////////////////////////////////////////////////
//��Ϸ������
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
private:
	static BYTE						m_cbOpenGameLottery;					//�Ƿ����齱
	static LONG						m_lMaxPlazaExp;							//���ƽ̨����
	static CString					m_strLucyDrawCfg;						//�齱����

	//��ע����
protected:
	typedef std::list<WORD>::iterator IteListPlayer;
	bool							m_bGameStart;                           //��Ϸ��ʼ��
	LONG							m_lBaseScore;							//��Ϸ��ע
	WORD							m_wHostChairID;							//����ID
	bool							m_bAgreeBaseScore[GAME_PLAYER];         //ͬ���������õĵ�ע
	std::list<WORD>					m_listPlayers;                          //��ǰ�����ķ��Թ����
	std::list<WORD>					m_listAgreeBaseScore;                   //��ǰ��������Ҫȷ�ϵ�ע�����

	//��Ϸ����
protected:
	WORD							m_wEastUser;							//�״ζ���
	WORD							m_wSiceCount;							//���ӵ���
	WORD							m_wBankerUser;							//ׯ���û�
	LONG							m_lOutCardScore[GAME_PLAYER][2];		//��ҳ��Ƶ÷� 0:���� 1:����
	LONG							m_lGangScore[GAME_PLAYER];				//��Ҹܷ�
	BYTE							m_cbCardIndex[GAME_PLAYER][MAX_INDEX];	//�û���
	BYTE							m_cbHuaPaiCount[GAME_PLAYER][2];		//�������� 0:Ӳ��, 1:��
	BYTE							m_cbHuaPaiData[GAME_PLAYER][MAX_HUA_COUNT];//Ӳ��������
	bool							m_bUserTrust[GAME_PLAYER];				//�Ƿ��й�
	bool							m_bUserOffline[GAME_PLAYER];			//�û�����
	bool							m_bLastRoundBiXiaHu;					//��һ���Ƿ���º�
	bool							m_bChangeBanker;						//�Ƿ�ׯ
	bool							m_bHaveJiangFen;						//�Ƿ񽱷�
	bool							m_bHaveFaFen;							//�Ƿ񷣷�
	bool							m_bHaveBigHu;							//�Ƿ���
	bool							m_bHuaGang;								//�Ƿ񻨸�
	bool							m_bHaveBaozi;							//�Ƿ�˰�

	//������Ϣ
protected:
	WORD							m_wOutCardUser;							//�����û�
	BYTE							m_cbOutCardCount;						//������Ŀ
	BYTE							m_cbOutCardData;						//��������
	BYTE							m_cbDiscardCount[GAME_PLAYER];			//������Ŀ
	BYTE							m_cbDiscardCard[GAME_PLAYER][72];		//������¼
	BYTE							m_cbDiscardRecordCount[GAME_PLAYER];	//������Ŀ��¼
	BYTE							m_cbDiscardRecordCard[GAME_PLAYER][72];	//�������ݼ�¼
	BYTE							m_cbDiscardIndexCount[GAME_PLAYER][MAX_INDEX]; //����������¼
	//BYTE							m_cbBaoZiCardData[GAME_PLAYER][2];		//������ 0������ƣ�1������������

	//������Ϣ
protected:
	bool							m_bSendCardStatus;						//����״̬
	WORD							m_wSendCardUser;						//�������
	WORD							m_wBuHuaUser;							//��ǰ�����û�
	BYTE							m_cbSendCardData;						//������
	BYTE							m_cbSendCardCount;						//������Ŀ
	BYTE							m_cbLeftCardCount;						//ʣ����Ŀ
	BYTE							m_cbRepertoryCard[MAX_REPERTORY];		//�����
	BYTE                            m_cbMasterCheckCard[GAME_PLAYER];		//���Ҫ��

	//���б���
protected:
	WORD							m_wCheatChairID;						//���������λ�ú�
	byte							m_cbLeftCheatCardCount;					//ʣ�ಹ���滻�Ƶ�����
	BYTE							m_cbCheatCard[MAX_COUNT];				//����Ҫ������

	//���б���
protected:
	WORD							m_wResumeUser;							//��ԭ�û�
	WORD							m_wCurrentUser;							//��ǰ�û�
	WORD							m_wProvideUser;							//��Ӧ�û�
	BYTE							m_cbProvideCard;						//��Ӧ��
	WORD							m_wGangPaiProvideUser;					//�����ṩ�û�

	//
protected:
	WORD							m_wFirstOutUserForFaFen;				//�׸��������
	BYTE							m_cbLastOutCardData;					//��һ�ֳ�����
	BYTE							m_cbOutSameCardCount;					//һȦ�ڳ���ͬ�Ƶ���Ŀ
	BYTE							m_cbGangStatus;							//����״̬
	bool							m_bHuaGangStatus[GAME_PLAYER][5];		//���ܵ÷�״̬
	bool							m_bAwardStatus[GAME_PLAYER];			//���ƽ���״̬
	bool							m_bPenaltyStatus[GAME_PLAYER][MAX_NORMAL];//��������ͬ�Ʒ���״̬
	tagQiangGangHu					m_QiangGangHuInfo;						//���ܺ���Ϣ

	//�û�״̬
public:
	bool							m_bResponse[GAME_PLAYER];				//��Ӧ��־
	BYTE							m_cbOperateCard[GAME_PLAYER];			//������
	DWORD							m_dwUserAction[GAME_PLAYER];			//�û�����
	DWORD							m_dwPerformAction[GAME_PLAYER];			//ִ�ж���
	bool							m_bDispatchCardOver[GAME_PLAYER];		//�������
	bool							m_bSendCardOver[GAME_PLAYER];			//���ƶ������
	bool							m_bBuHuaOver[GAME_PLAYER];				//�����������
	bool							m_bFirstBuHuaOver[GAME_PLAYER];			//���ֲ������
	bool							m_bUserTianTing[GAME_PLAYER];			//�������
	bool							m_bUserTing[GAME_PLAYER];				//�������״̬
	bool							m_bInvalidHuStates[GAME_PLAYER];		//�Ƿ�����Ч�ĺ���״̬;
	BYTE							m_cbInvalidOperteStates[GAME_PLAYER];	//�Ƿ�����Ч������״̬;
	//bool							m_bFirstSpecialLights[GAME_PLAYER];		//�Ƿ����״�;
	//tagSpecialLight					m_SpecialLight[GAME_PLAYER];			//���������ʾ;
	//tagBaoPaiResult					m_BaoPaiResult[GAME_PLAYER];			//��Ұ��ƽ��

	//�����
protected:
	BYTE							m_cbWeaveItemCount[GAME_PLAYER];		//�����Ŀ
	tagWeaveItem					m_WeaveItemArray[GAME_PLAYER][MAX_WEAVE];//�����

	//������Ϣ
protected:
	BYTE							m_cbHuPaiCard;							//������
	tagHuPaiResult					m_HuPaiResult[GAME_PLAYER];				//���ƽ��
	tagTingPaiResult				m_TingPaiResult[GAME_PLAYER];			//���ƽ��
	tagGangPaiResult				m_GangPaiResult[GAME_PLAYER];			//���ƽ��
	tagTingPaiItem					m_HaveTingPaiResult[GAME_PLAYER];		//����������ƽ��

	//¼��
public:
	tagGameRecord					m_kGameRecord;

	//˽�˳�������Ϣ
public:
	CMD_S_Private_End_Info			m_PrivateEndInfo;

	//�������
protected:
	CGameLogic						m_GameLogic;							//��Ϸ�߼�
	ITableFrame						*m_pITableFrame;						//��ܽӿ�

	//���Ա���
protected:
	static const WORD				m_wPlayerCount;							//��Ϸ����
	static const tagGameServiceOption *m_pGameServiceOption;				//���ò���

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
	virtual void * QueryInterface(const IID &Guid, DWORD dwQueryVer);

	//����ӿ�
public:
	//��ʼ��
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//��λ����
	virtual void RepositionSink();
	//��Ϸ״̬
	virtual bool IsUserPlaying(WORD wChairID);

	//��Ϸ�¼�
public:
	//��Ϸ��ʼ
	virtual bool OnEventGameStart();
	//��Ϸ����
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//���ͳ���
	virtual bool OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	//�¼��ӿ�
public:
	//��ʱ���¼�
	virtual bool OnTimerMessage(DWORD dwTimerID, WPARAM wBindParam);
	//��Ϸ��Ϣ����
	virtual bool OnGameMessage(WORD wSubCmdID, void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//�����Ϣ����
	virtual bool OnFrameMessage(WORD wSubCmdID, void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//�����¼�
	virtual bool OnGameDataBase(WORD wRequestID, VOID * pData, WORD wDataSize){ return true; }

	//��ѯ�ӿ�
public:
	//��ѯ�޶�
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem) { return 0L; }
	//���ٻ���
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem){ return 0L; }
	//��ѯ�����
	virtual bool QueryBuckleServiceCharge(WORD wChairID) { return false; }
	//�����¼�
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize){ return false; }
	//�����¼�
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason){ return false; }
	//���û���
	virtual void SetGameBaseScore(LONG lBaseScore){ return; }

	//����ӿ�
public:
	//�����Ƿ����
	virtual bool IsTaskCompleted( WORD wChairID, DWORD dwTaskID, IN OUT DWORD &dwRecord );

	//�û��¼�
public:
	//�û�����
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem);
	//�û�����
	virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem);
	//�û�����
	virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�����
	virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�ͬ��
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize) { return true; }
	//���������ж�
	virtual bool OnActionUserFangKaCheck(WORD wChairID, IServerUserItem * pIServerUserItem, SCORE lUserInsure)  { return true; }

	//˽�˳�
public:
	virtual void SetPrivateInfo(BYTE bGameTypeIdex, DWORD bGameRuleIdex) { }
	//����˽�˳�������Ϣ
	virtual void ResetPrivateEndInfo();
	//���˽�˳�������Ϣ��
	virtual void GetPrivateEndInfo(DataStream &kDataStream, bool bSend);
	//�ж�˽�˳��Ƿ����
	virtual	bool IsPrivateEnd();

	// ¼���¼�
public:
	// ��Ϸ��ʼ
	void starGameRecord();
	// ��Ҳ�����ʾ;
	void addGameRecordAction(WORD wSubCmdID, void* pSubMessage, int nMessageSize);

	//��Ϸ�¼�
protected:
	//������õ�ע
	bool OnSubSetBaseScore(WORD wChairID, LONG lBaseScore, BYTE cbActionType);
	//��ʼ����
	bool OnSubShootDice(WORD wChairID);
	//�������
	bool OnSubDispatchOver(WORD wChairID);
	//�������
	bool OnSubSendCardOver(WORD wChairID);
	//���Ҫ��
	BYTE getSendCardData(WORD dwSendUser);
	//�����Ŷ�;
	void sendMasterLeftCard(int nChairdID);
	//����Ҫ�滻����
	BYTE getBuHuaCardData(WORD wChairID);
	//�������
	bool OnSubBuHuaOver(WORD wChairID);
	//�û�����
	bool OnSubOutCard(WORD wChairID, BYTE cbCardData);
	//�û�����
	bool OnSubOperateCard(WORD wChairID, DWORD dwOperateCode, BYTE cbOperateCard, bool &bHaveOperate);
	//�û��齱
	bool OnSubUserLottery(WORD wChairID, BYTE cbLotteryIndex);
	//�û��й�
	bool OnSubUserTrust(WORD wChairID, bool bTrust);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//�û�����
	//���Ʋ���
	bool PerformPengPai(WORD wChairID, DWORD dwOperateAction, BYTE cbOperateCard);
	//���Ʋ���
	bool PerformGangPai(WORD wChairID, DWORD dwOperateAction, BYTE cbOperateCard, bool bCheckQiangGangHu=true);
	//���Ʋ���
	bool PerformTingPai(WORD wChairID, DWORD dwOperateAction, BYTE cbOperateCard);
	//��ս����
	//bool PerformTiaoZhan(WORD wChairID, DWORD dwOperateAction, BYTE cbOperateCard, bool bZiMo);
	//���Ʋ���
	bool PerformHuPai(WORD wChairID, DWORD dwOperateAction, BYTE cbOperateCard, bool bZiMo);
	//��������
	bool PerformGiveUp(WORD wChairID, DWORD dwOperateAction, BYTE cbOperateCard);

	//��������
protected:
	//���ñ���
	void ResetAllData(bool bAll);
	//�������
	bool SendCardData(WORD wChairID, BYTE cbDirection, bool bBanker14=false);
	//���Ͳ���
	bool SendOperateNotify(WORD wChairID, BYTE cbCenterCard, enEstimatKind EstimatKind);
	//��Ӧ�ж�
	bool EstimateUserRespond(WORD wChairID, BYTE cbCenterCard, enEstimatKind EstimatKind);
	//����Լ��Ĳ������ܡ�����������
	bool EstimatSendCard(WORD wChairID, BYTE cbSendCard);
	//���������ҵ������ܡ�������
	bool EstimatOutCard(WORD wChairID, BYTE cbCenterCard);
	//��������������
	bool EstimatChiPengCard(WORD wChairID, BYTE cbCenterCard);
	//�ܲ���;
	bool EstimatGangCard(WORD wChairID, BYTE cbCenterCard);
	//��鲹�ܺ�������ҿɷ��
	bool EstimatBuGangCard(WORD wChairID, BYTE cbCenterCard);
	//�����Ҳ���
	bool EstimatUserBuHua(WORD wChairID);
	//�������ܷ���
	bool CheckChangeTingPai(WORD wChairID, tagTingPaiResult TingPaiResult);
	//��黨�ܵ÷�
	void CheckHuaGangScore(WORD wChairID);
	//���㷣��
	void CountPayMoneyScore(CMD_S_PayMoney &stPayMoney, BYTE cbPayType, LONG lBaseHuaCount);
	//�����Ƶ÷�
	void CheckOutCardScore(WORD wChairID);
	// ���ķ�;
	void CheckShouSiFeng(WORD wChairID);
	// ÷����;
	void CheckMeiSiZhang(WORD wChairID);
	// ������;
	void CheckSiLianFa(WORD wChairID);
	//�������Ƿ��а���
	void CheckHaveBaoPaiUser(WORD wChairID);
	//�����ս�÷�
	void CheckTiaoZhanScore(WORD wChairID, LONG lTiaoZhanScore[GAME_PLAYER]);
	//���ֲ����Ƿ����
	bool IsFirstBuHuaOver() { return m_bFirstBuHuaOver[0]&&m_bFirstBuHuaOver[1]&&m_bFirstBuHuaOver[2]&&m_bFirstBuHuaOver[3]; }
	//��ȡ��ҿɲ�������
	BYTE GetOperateCard(WORD wChairID);
	//��ȡ�����ǰ��������
	DWORD GetPreHuPaiKind(WORD wChairID, BYTE cbHuPaiCard);
	//��ȡ����ʱʣ��������
	void GetTingCardLastCount(tagTingPaiResult &TingPaiResult, const WORD wChairID);
	//��ȡ����Ŀ(�������ȥ���ƣ��ԡ��������ܵ���)
	int GetCardVisualCount(const BYTE cbCardData, const WORD wChairID);	
	//����
	bool IsZiMo(WORD wChairID);
	//���
	bool IsTianHu(WORD wChairID);
	//�غ�
	bool IsDiHu(WORD wChairID);
	//�޻���
	bool IsWuHuaGuo(WORD wChairID);
	//ѹ��
	bool IsYaJue(WORD wChairID, BYTE cbHuPaiCard);
	//��������
	bool IsHaiDiLaoYue(WORD wChairID);
	//С�ܿ���
	bool IsXiaoGangKaiHua(WORD wChairID);
	//��ܿ���
	bool IsDaGangKaiHua(WORD wChairID);
	//���ܺ�
	bool IsQiangGangHu(WORD wChairID);
	//���º�
	bool IsBiXiaHu();
	//�Ƿ��и߼�����
	bool IsHaveOperateCard(DWORD dwOperateMask);
	//�Ƿ���ȱһ��
	bool IsQueYiMen(WORD wChairID, BYTE cbCardIndex[MAX_INDEX]);
	//�Ƿ�ֻ���޻���
	bool IsOnlyHuWuHuaGuo(DWORD dwHuPaiKind);
	//�Ƿ��в˰���
	//bool IsBaoZi(WORD wChairID);
	//Ԥ�ж��Ƿ���Գɰ���;
	bool AnalyseBaoZi(WORD wChairID, WORD wProvideUser, BYTE cbCenterCard);
	//����;
	bool IsSanPeng(WORD wChairID);
	//������һɫ;
	bool IsSiPengQingyise(WORD wChairID, BYTE cbCenterCard, DWORD dwUserAction);
	//�����ṩ��;
	WORD GetBaoziProvideUser(WORD wChairID);
	//ʹ���ƿ�
	void UseCardLibrary();

	//���ƶ���ɾ��һЩ��
	bool RemoveCards(BYTE cbCards[],BYTE cbCardCount,BYTE cbRemoveCards[],BYTE cbRemoveCount);
	//�Ƿ���Բ���齱
	//bool IsLuckyDrawEnable( WORD wChairID )const;

	//�������õ�ע��Ϣ
	bool SendSetBaseScore(WORD wChairID, BYTE cbBaseScoreType);
	//���֪ͨ��ע
	bool CheckNotifyBaseScore();
	//�����ҽ��
	bool CheckMoneyLimit(LONG lScoreLimit);
	//�߳����
	void SendKickOutMsg(WORD ChairID);

	//���͵�����Ϣ;
	//void SendSpecialLightMsg(WORD wChairID);

public:
	//������Ϸ����
	static void SetGameServerOpertion(tagGameServiceOption *pGso);
	//��ȡ����������
	static bool ReadServerConfig();
	//����Ŀ¼
	static bool GetWorkDirectory(TCHAR szWorkDirectory[], WORD wBufferCount);

	//˽�˳���������;
public:
	//�Ƿ�����Ӧ����;
	bool IsHasGameRule(eNJMJRuleEnum eType);	
	//�Ƿ��а���;
	bool IsGameRuleBaoZi();
	//�Ƿ��ǽ�԰��;
	bool IsGameRuleJYZ();

	// �۷ּ�¼
	void PayMoneyLog(CMD_S_PayMoney & stPayMoney);
	void MyLog(TCHAR *szLog, ...);
};

//////////////////////////////////////////////////////////////////////////

#endif