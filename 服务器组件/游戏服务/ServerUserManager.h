#ifndef SERVER_USER_MANAGER_HEAD_FILE
#define SERVER_USER_MANAGER_HEAD_FILE

#pragma once

#include "AfxTempl.h"
#include "GameServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////

//�û���Ϣ
class GAME_SERVICE_CLASS CServerUserItem : public IServerUserItem, public IServerUserService
{
	//��Ԫ����
	friend class CServerUserManager;

	//���Ա���
protected:
	tagUserInfo						m_UserInfo;							//�û���Ϣ
	tagUserRule						m_UserRule;							//�û�����
	tagUserScore					m_ScoreFormer;						//��ǰ����
	//tagUserProperty                 m_UserProperty;                     //��ҵ���
	CYSArray<tagUserProperty>		m_UserProperty;

	//�����¼
protected:
	tagVariationInfo				m_RecordInfo;						//��¼��Ϣ
	tagVariationInfo				m_VariationInfo;					//�����Ϣ

	//���ӱ���
protected:
	DWORD							m_dwUserRight;						//�û�Ȩ��
	DWORD							m_dwMasterRight;					//����Ȩ��

	//�йܱ���
protected:
	SCORE	 						m_lTrusteeScore;					//�йܻ���(����ʣ�����)
	SCORE							m_lRestrictScore;					//���ƻ���
	SCORE	 						m_lFrozenedScore;					//�������

	//��������
	SCORE							m_lDefPutScore;						//Ĭ��һ�δ�����ٻ���
	SCORE							m_lLowPutScore;						//���ڶ��ٴ������
	SCORE							m_lNextPutScore;					//�´δ�����
	//int								m_iExchangeRatio;					//�һ�����

protected:
	//�ƿ����
	DWORD							m_dwCardLibIndex;					//ʹ���ƿ�����
	DWORD							m_dwStartLibTime;					//�ƿ⿪ʼʹ��ʱ��
	DWORD							m_dwStopLibTime;					//�ƿ����ʹ��ʱ��
	BYTE							m_cbCardLibEnabled;					//�ƿ���Ч��־

	//��¼��Ϣ
protected:
	DWORD							m_dwDBQuestID;						//�����ʶ
	DWORD							m_dwLogonTime;						//��¼ʱ��
	DWORD							m_dwInoutIndex;						//��������

	//�������
protected:
	BYTE							m_cbMaxCardType;
	BYTE							m_aryCardData[MAX_CARD_DATA_COUNT];

	//�ͻ�����
protected:
	bool							m_bAndroidUser;						//�����û�
	bool							m_bCheatUser;						//�����û�

	//������Ϣ
protected:
	DWORD							m_dwSignUpTime;						//����ʱ��
	BYTE							m_cbSignUpStatus;					//����״̬	
	VOID *							m_pMatchData;						//��������

	//��������
protected:
	bool							m_bTrusteeUser;						//ϵͳ�й�
	bool							m_bClientReady;						//���ӱ�־
	bool							m_bModifyScore;						//�޸ı�־
	TCHAR							m_szLogonPass[LEN_PASSWORD];		//�û�����
	BYTE							m_cbReUserStatus;					//��ԭ״̬

	//ϵͳ����
protected:
	WORD							m_wBindIndex;						//������
	DWORD							m_dwClientAddr;						//���ӵ�ַ
	TCHAR							m_szMachineID[LEN_MACHINE_ID];		//������ʶ

	//����ӿ�
protected:
	IServerUserItemSink *			m_pIServerUserItemSink;				//�ص��ӿ�

	//�ƿ���Ϣ;
public:
	static	DWORD					g_dwCardLibCount;					//�ƿ�����;
	static	BYTE					g_cbCardLibEnable;					//�ƿ��Ƿ����ʹ��;
	static	DWORD					g_dwLibStartDateTime;				//�ƿ⿪ʼʹ�ÿ�ʼʱ��;
	static	DWORD					g_dwLibEndDateTime;					//�ƿ����ʹ�ÿ�ʼʱ��;
	static	DWORD					g_dwCardLibCustomCount;				//�ƿ��Զ�������;

	//��������
protected:
	//���캯��
	CServerUserItem();
	//��������
	virtual ~CServerUserItem();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//������Ϣ
public:
	//�������
	virtual WORD GetBindIndex() { return m_wBindIndex; }
	//�û���ַ
	virtual DWORD GetClientAddr() { return m_dwClientAddr; }
	//������ʶ
	virtual LPCTSTR GetMachineID() { return m_szMachineID; }
	//�Ƿ�����
	virtual bool IsOnline() { return m_wBindIndex!=INVALID_WORD; }

	//��¼��Ϣ
public:
	//�����ʶ
	virtual DWORD GetDBQuestID() { return m_dwDBQuestID++; }
	//��¼ʱ��
	virtual DWORD GetLogonTime() { return m_dwLogonTime; }
	//��¼����
	virtual DWORD GetInoutIndex() { return m_dwInoutIndex; }

	//�û���Ϣ
public:
	//�û���Ϣ
	virtual tagUserInfo * GetUserInfo() { return &m_UserInfo; }
	//�û�����
	virtual tagUserRule * GetUserRule() { return &m_UserRule; }
	//������Ϣ
	virtual CYSArray<tagUserProperty> & GetUserProperty() { return m_UserProperty; }
	//���õ���
	virtual void SetUserProperty(tagUserProperty &Property);
	//���ӵ���
	virtual void AddUserProperty(tagUserProperty &Property);
	//ʹ�õ���
	virtual EN_PROPERTY_ERROR UsedProperty(CMD_GR_C_Property *pProperty, SYSTEMTIME &SystemTime, DWORD &dwPropDBID);
	//ʱ����Ϣ
	virtual tagTimeInfo * GetTimeInfo() { return &m_UserInfo.TimerInfo; }

	//������Ϣ
public:
	//�û��Ա�
	virtual BYTE GetGender() { return m_UserInfo.cbGender; }
	//�û���ʶ
	virtual DWORD GetUserID() { return m_UserInfo.dwUserID; }
	//�û��ǳ�
	virtual LPCTSTR GetNickName() { return m_UserInfo.szNickName; }
	//�û�����
	virtual LPCTSTR GetPassword() { return	m_szLogonPass; }

	//״̬�ӿ�
public:
	//���Ӻ���
	virtual WORD GetTableID() { return m_UserInfo.wTableID; }
	//���Ӻ���
	virtual WORD GetLastTableID() { return m_UserInfo.wLastTableID; }
	//���Ӻ���
	virtual WORD GetChairID() { return m_UserInfo.wChairID; }
	//�û�״̬
	virtual BYTE GetUserStatus() { return m_UserInfo.cbUserStatus; }

	//Ȩ����Ϣ
public:
	//�û�Ȩ��
	virtual DWORD GetUserRight() { return m_dwUserRight; } 
	//����Ȩ��
	virtual DWORD GetMasterRight() { return m_dwMasterRight; } 

	//�ȼ���Ϣ
public:
	//�û�Ȩ��
	virtual BYTE GetMemberOrder() { return m_UserInfo.cbMemberOrder; } 
	//����Ȩ��
	virtual BYTE GetMasterOrder() { return m_UserInfo.cbMasterOrder; } 

	//������Ϣ
public:
	//�û����
	virtual SCORE GetUserGold() { return m_UserInfo.lGold; }
	//�û�����
	virtual SCORE GetUserScore() { return m_UserInfo.lScore; }
	//�û���ʯ
	virtual SCORE GetUserDiamond() { return m_UserInfo.lDiamond; }
	//�û�����
	virtual SCORE GetUserInsure() { return m_UserInfo.lInsure; }

	//����������
	virtual BYTE GetMaxCardData(BYTE* pData, BYTE cbCount);

	//�ȼý���Ϣ
public:
	//��ȡ����
	virtual BYTE GetBenefitTimes() { return m_UserInfo.cbBenefitTimes; }
	//�ϴ���ȡʱ��
	virtual SYSTEMTIME GetLastBenefitTime() { return m_UserInfo.LastBenefitTime; }
	//������ȡ��Ϣ
	virtual	VOID SetBenefitInfo(tagSendBenefitSuc* pbenefit);

	//�й���Ϣ
public:
	//�йܻ���
	virtual SCORE GetTrusteeScore() { return m_lTrusteeScore; }
	//�������
	virtual SCORE GetFrozenedScore() { return m_lFrozenedScore; }
	//�������
	virtual SCORE TakeGold(SCORE lScore, bool isAtOnce = true);
	//������������
	virtual bool WriteOtherScore(SCORE lScore, BYTE cbType);

	//������Ϣ
public:
	//�û�ʤ��
	virtual WORD GetUserWinRate();
	//�û�����
	virtual WORD GetUserLostRate();
	//�û�����
	virtual WORD GetUserDrawRate();
	//�û�����
	virtual WORD GetUserFleeRate();
	//��Ϸ����
	virtual DWORD GetUserPlayCount() { return m_UserInfo.dwWinCount+m_UserInfo.dwLostCount+m_UserInfo.dwDrawCount+m_UserInfo.dwFleeCount; }

	//Ч��ӿ�
public:
	//�Ա��ʺ�
	virtual bool ContrastNickName(LPCTSTR pszNickName);
	//�Ա�����
	virtual bool ContrastLogonPass(LPCTSTR pszPassword);

	//�й�״̬
public:
	//�ж�״̬
	virtual bool IsTrusteeUser() { return m_bTrusteeUser; }
	//����״̬
	virtual VOID SetTrusteeUser(bool bTrusteeUser) { m_bTrusteeUser=bTrusteeUser; }

	//��Ϸ״̬
public:
	//����״̬
	virtual bool IsClientReady() { return m_bClientReady; };
	//��������
	virtual VOID SetClientReady(bool bClientReady) { m_bClientReady=bClientReady; }
	//��ԭ״̬;
	virtual BYTE GetReUserStatus(){ return m_cbReUserStatus; }
	//���û�ԭ״̬;
	virtual void SetReUserStatus(BYTE cbReUserStatus) { m_cbReUserStatus = cbReUserStatus; }

	//�����û�
public:
	//����״̬
	virtual bool IsAndroidUser() { return m_bAndroidUser; }
	//���ÿ���
	virtual VOID SetAndroidUser(bool bAndroidUser) { m_bAndroidUser=bAndroidUser; }
	//�����û�
	virtual bool IsCheatUser() { return m_bCheatUser; }
	//���������û�
	virtual void SetCheatUser(bool bFlag) { m_bCheatUser = bFlag; }

	//�����ӿ�
public:
	//��������
	virtual VOID * GetMatchData() { return m_pMatchData; }
	//��������
	virtual VOID SetMatchData(VOID * pMatchData) { m_pMatchData=pMatchData; }
	//����ʱ��
	virtual DWORD GetSignUpTime() { return m_dwSignUpTime; }
	//����ʱ��
	virtual VOID SetSignUpTime(DWORD dwSignUpTime) { m_dwSignUpTime=dwSignUpTime; }
	//����״̬
	virtual BYTE GetSignUpStatus() { return m_cbSignUpStatus; }
	//����״̬
	virtual VOID SetSignUpStatus(BYTE cbSignUpStatus) { m_cbSignUpStatus=cbSignUpStatus; }

	//��¼�ӿ�
public:
	//����ж�
	virtual bool IsVariation() { return m_bModifyScore; }
	//��ѯ��¼
	virtual bool QueryRecordInfo(tagVariationInfo & RecordInfo);
	//��ȡ���
	virtual bool DistillVariation(tagVariationInfo & VariationInfo);

	//����ӿ�
public:
	//����״̬
	virtual bool SetUserStatus(BYTE cbUserStatus, WORD wTableID, WORD wChairID);
	//д�����
    virtual bool WriteUserScore(SCORE lScore, SCORE lGrade, SCORE lRevenue, SCORE lIngot, BYTE cbScoreType, DWORD dwPlayTimeCount,DWORD dwWinExperience);

	//����ӿ�
public:
	//�������
	virtual bool FrozenedUserScore(SCORE lScore);
	//�ⶳ����
	virtual bool UnFrozenedUserScore(SCORE lScore);

	//�޸Ľӿ�
public:
	//�޸���Ϣ
	virtual bool ModifyUserProperty(SCORE lScore, LONG lLoveLiness);
	//����������
	virtual bool ModifyMaxCardType(BYTE cbCurType, BYTE aryData[], BYTE cbPokerCount);

	//�����ӿ�
public:
	//�����
	virtual bool DetachBindStatus();
	//���в���
	virtual bool ModifyUserInsure(SCORE lScore, SCORE lInsure, SCORE lRevenue);
	//���ò���
	virtual bool SetUserParameter(DWORD dwClientAddr, WORD wBindIndex, TCHAR szMachineID[LEN_MACHINE_ID], bool bAndroidUser, bool bClientReady);
	//�޸�Ȩ��
	virtual VOID ModifyUserRight( DWORD dwAddRight, DWORD dwRemoveRight, BYTE cbRightKind=UR_KIND_GAME);

	//�ƿ�ӿ�
public:
	//�Ƿ����ʹ���ƿ�
	virtual bool canUseCardLib();
	//��ȡ�ƿ�����
	virtual DWORD getCardLibIndex();
	//�����ƿ�����
	virtual void setCardLibIndex(DWORD dwCardLibIndex);
	//�ۼ��ƿ�����
	virtual void addCardLibIndex();
	//�����ƿ������
	virtual void setCardLibEnabled(bool bEnabled);
	//�����ƿ�ʹ��ʱ���
	virtual void setUseCardLibTime(DWORD dwStartTime, DWORD dwEndTime);

	//��������
private:
	//��������
	VOID ResetUserItem();
};

//////////////////////////////////////////////////////////////////////////////////

//�û�������
typedef CYSArray<CServerUserItem *> CServerUserItemArray;
typedef CMap<DWORD,DWORD,CServerUserItem *,CServerUserItem *> CServerUserItemMap;

//�û�������
class GAME_SERVICE_CLASS CServerUserManager : public IServerUserManager
{
	//�û�����
protected:
	CServerUserItemMap				m_UserIDMap;						//�û�����
	CServerUserItemArray			m_UserItemArray;					//�û�����
	CServerUserItemArray			m_UserItemStore;					//�洢�û�

	//����ӿ�
protected:
	IServerUserItemSink *			m_pIServerUserItemSink;				//�ص��ӿ�

	//��������
public:
	//���캯��
	CServerUserManager();
	//��������
	virtual ~CServerUserManager();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { return; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���ú���
public:
	//���ýӿ�
	virtual bool SetServerUserItemSink(IUnknownEx * pIUnknownEx);

	//���Һ���
public:
	//ö���û�
	virtual IServerUserItem * EnumUserItem(WORD wEnumIndex);
	//�����û�
	virtual IServerUserItem * SearchUserItem(DWORD dwUserID);
	//�����û�
	virtual IServerUserItem * SearchUserItem(LPCTSTR pszNickName);

	//ͳ�ƺ���
public:
	//��������
	virtual DWORD GetAndroidCount();
	//��������
	virtual DWORD GetUserItemCount() { return (DWORD)m_UserItemArray.GetCount(); }

	//������
public:
	//ɾ���û�
	virtual bool DeleteUserItem();
	//ɾ���û�
	virtual bool DeleteUserItem(IServerUserItem * pIServerUserItem);
	//�����û�
	virtual bool InsertUserItem(IServerUserItem * * pIServerUserResult, tagUserInfo & UserInfo,tagUserInfoPlus & UserInfoPlus);
};

//////////////////////////////////////////////////////////////////////////////////

#endif