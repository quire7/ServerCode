#ifndef TABLE_FRAME_HEAD_FILE
#define TABLE_FRAME_HEAD_FILE

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////

//���鶨��
typedef IServerUserItem * CTableUserItemArray[MAX_CHAIR];				//��Ϸ����
typedef CYSArray<IServerUserItem *> CLookonUserItemArray;				//�Թ�����
typedef CYSArray<tagGameScoreRecord *> CGameScoreRecordArray;			//��¼����
typedef CYSArray<tagGameCardLibrary *> CGameCardLibraryArray;			//�ƿ�����
interface IMatchTableFrameNotify;
//////////////////////////////////////////////////////////////////////////////////

//���ӿ��
class CTableFrame : public ITableFrame
{
	//��Ϸ����
protected:
	WORD							m_wTableID;							//���Ӻ���
	WORD							m_wChairCount;						//������Ŀ
	BYTE							m_cbStartMode;						//��ʼģʽ
	WORD							m_wUserCount;						//�û���Ŀ

	//״̬����
protected:
	bool							m_bGameStarted;						//��Ϸ��־
	bool							m_bDrawStarted;						//��Ϸ��־
	bool							m_bTableStarted;					//��Ϸ��־
	bool							m_bTableInitFinish;					//��ʼ��ʶ

	//״̬����
protected:
	bool							m_bAllowLookon[MAX_CHAIR];			//�Թ۱�־
	SCORE							m_lFrozenedScore[MAX_CHAIR];		//�������

	//��Ϸ����
protected:
	LONG							m_lCellScore;						//��Ԫ����
	BYTE							m_cbGameStatus;						//��Ϸ״̬

	//ʱ�����
protected:
	DWORD							m_dwDrawStartTime;					//��ʼʱ��
	SYSTEMTIME						m_SystemTimeStart;					//��ʼʱ��
	WORD                            m_wDrawCount;                       //��Ϸ����

	//��̬����
protected:
	DWORD							m_dwTableOwnerID;					//�����û�
	TCHAR							m_szEnterPassword[LEN_PASSWORD];	//��������

	//���߱���
protected:
	WORD							m_wOffLineCount[MAX_CHAIR];			//���ߴ���
	DWORD							m_dwOffLineTime[MAX_CHAIR];			//����ʱ��

	//�û�����
protected:
	CTableUserItemArray				m_TableUserItemArray;				//��Ϸ�û�
	CLookonUserItemArray			m_LookonUserItemArray;				//�Թ��û�

	//����ӿ�
protected:
	ITimerEngine *					m_pITimerEngine;					//ʱ������
	ITableFrameSink	*				m_pITableFrameSink;					//���ӽӿ�
	IMainServiceFrame *				m_pIMainServiceFrame;				//����ӿ�
	IAndroidUserManager *			m_pIAndroidUserManager;				//�����ӿ�

	//��չ�ӿ�
protected:
	ITableUserAction *				m_pITableUserAction;				//�����ӿ�
	ITableUserRequest *				m_pITableUserRequest;				//����ӿ�	

	//���ݽӿ�
protected:
	IDataBaseEngine *				m_pIKernelDataBaseEngine;			//�ں�����
	IDataBaseEngine *				m_pIRecordDataBaseEngine;			//��¼����

	//������Ϣ
protected:
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//��������
	tagGameServiceOption *			m_pGameServiceOption;				//��������
	tagBenefitInfo*					m_pBenefitInfo;						//�ȼý�

	//��Ϸ��¼
protected:
	CGameScoreRecordArray			m_GameScoreRecordActive;			//��Ϸ��¼
	static CGameScoreRecordArray	m_GameScoreRecordBuffer;			//��Ϸ��¼

	//�����ӿ�
protected:
	ITableFrameHook	*				m_pITableFrameHook;					//�����ӿ�
	ITableUserAction *				m_pIMatchTableAction;				//�����ӿ�

	//˽�˳�����;
protected:
	tagPrivateFrameParameter		m_PrivateFramePrarameter;			// ˽�˳�����;
	tagPrivateFrameRecordInfo		m_PrivateFrameRecord;				// ��Ϸ������Ϣ;

	//�ƿ�����;
public:
	static CGameCardLibraryArray	m_GameCardLibraryBuffer;
	//��������
public:
	//���캯��
	CTableFrame();
	//��������
	virtual ~CTableFrame();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���Խӿ�
public:
	//���Ӻ���
	virtual WORD GetTableID() { return m_wTableID; }
	//��Ϸ����
	virtual WORD GetChairCount() { return m_wChairCount; }
	//��λ����Ŀ
	virtual WORD GetNullChairCount(){return m_wChairCount - m_wUserCount;}

	//���ò���
public:
	//�Զ�����
	virtual VOID * GetCustomRule() { return m_pGameServiceOption->cbCustomRule; };
	//��������
	virtual tagGameServiceAttrib * GetGameServiceAttrib() { return m_pGameServiceAttrib; }
	//��������
	virtual tagGameServiceOption * GetGameServiceOption() { return m_pGameServiceOption; }

	//���ýӿ�
public:
	//��ʼģʽ
	virtual BYTE GetStartMode() { return m_cbStartMode; }
	//��ʼģʽ
	virtual VOID SetStartMode(BYTE cbStartMode) { m_cbStartMode=cbStartMode; }

	//��Ԫ����
public:
	//��Ԫ����
	virtual LONG GetCellScore() { return m_lCellScore; }
	//��Ԫ����
	virtual VOID SetCellScore(LONG lCellScore);

	//״̬�ӿ�
public:
	//��ȡ״̬
	virtual BYTE GetGameStatus() { return m_cbGameStatus; }
	//����״̬
	virtual VOID SetGameStatus(BYTE cbGameStatus) { m_cbGameStatus=cbGameStatus; }

	//��Ϣ�ӿ�
public:
	//��Ϸ״̬
	virtual bool IsGameStarted() { return m_bGameStarted; }
	//��Ϸ״̬
	virtual bool IsDrawStarted() { return m_bDrawStarted; }
	//��Ϸ״̬
	virtual bool IsTableStarted() { return m_bTableStarted; }
	//����״̬
	virtual bool IsTableLocked() { return (m_szEnterPassword[0]!=0); }

	//���ƽӿ�
public:
	//��ʼ��Ϸ
	virtual bool StartGame();
	//��ɢ��Ϸ
	virtual bool DismissGame();
	//������Ϸ
	virtual bool ConcludeGame(BYTE cbGameStatus);
	//��������
	virtual bool ConcludeTable();

	//�û��ӿ�
public:
	//Ѱ���û�
	virtual IServerUserItem * SearchUserItem(DWORD dwUserID);
	//��Ϸ�û�
	virtual IServerUserItem * GetTableUserItem(WORD wChairID);
	//�Թ��û�
	virtual IServerUserItem * EnumLookonUserItem(WORD wEnumIndex);

	//д�ֽӿ�
public:
	//д�����
	virtual bool WriteUserScore(WORD wChairID, tagScoreInfo & ScoreInfo, DWORD dwGameMemal=INVALID_DWORD, DWORD dwPlayGameTime=INVALID_DWORD);
	//д�����
	virtual bool WriteTableScore(tagScoreInfo ScoreInfoArray[], WORD wScoreCount, DataStream& kData);

	//����ӿ�
public:
	//����˰��
	virtual SCORE CalculateRevenue(WORD wChairID, SCORE lScore);
	//��ѯ�޶�
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem);

	//ʱ��ӿ�
public:
	//����ʱ��
	virtual bool SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter);
	//ɾ��ʱ��
	virtual bool KillGameTimer(DWORD dwTimerID);

	//��Ϸ�û�
public:
	//��������
	virtual bool SendTableData(WORD wChairID, WORD wSubCmdID);
	//��������
	virtual bool SendTableData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize,WORD wMainCmdID=MDM_GF_GAME);

	//�Թ��û�
public:
	//��������
	virtual bool SendLookonData(WORD wChairID, WORD wSubCmdID);
	//��������
	virtual bool SendLookonData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize, WORD wMainCmdID = MDM_GF_GAME);

	//�����û�
public:
	//��������
	virtual bool SendUserItemData(IServerUserItem * pIServerUserItem, WORD wSubCmdID);
	//��������
	virtual bool SendUserItemData(IServerUserItem * pIServerUserItem, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//ϵͳ��Ϣ
public:
	//������Ϣ
	virtual bool SendGameMessage(LPCTSTR lpszMessage, WORD wType);
	//��Ϸ��Ϣ
	virtual bool SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType);
	//������Ϣ
	virtual bool SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType);

	//��������
public:
	//��������
	virtual bool PerformStandUpAction(IServerUserItem * pIServerUserItem, bool bInitiative=false);
	//�Թ۶���
	virtual bool PerformLookonAction(WORD wChairID, IServerUserItem * pIServerUserItem);
	//���¶���
	virtual bool PerformSitDownAction(WORD wChairID, IServerUserItem * pIServerUserItem, LPCTSTR lpszPassword=NULL);
	//�ָ��Թ�
	virtual bool PerformRecoerLookOn(WORD wChairID, IServerUserItem * pIServerUserItem);

	//���ܽӿ�
public:
	//���ͳ���
	virtual bool SendGameScene(IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);

	//�����ӿ�
public:
	//���ýӿ�
	virtual bool SetTableFrameHook(IUnknownEx * pIUnknownEx);
	//��ȡ�ӿ�
	virtual IUnknownEx * GetTableFrameHook(){ return m_pITableFrameHook;}

	//���ܺ���
public:
	//��Ϸ����
	WORD GetDrawCount();
	//��ȡ��λ
	WORD GetNullChairID();
	//�����λ
	WORD GetRandNullChairID();
	//�û���Ŀ
	WORD GetSitUserCount();
	//�Թ���Ŀ
	WORD GetLookonUserCount();
	//������Ŀ
	WORD GetOffLineUserCount();

	//���ܺ���
public:
	//�û�״��
	WORD GetTableUserInfo(tagTableUserInfo & TableUserInfo);
	//��������
	bool InitializationFrame(WORD wTableID, tagTableFrameParameter & TableFrameParameter);

	//�û��¼�
public:
	//�����¼�
	bool OnEventUserOffLine(IServerUserItem * pIServerUserItem);
	//�����¼�
	bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);

	//ϵͳ�¼�
public:
	//ʱ���¼�
	bool OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//��Ϸ�¼�
	bool OnEventSocketGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//����¼�
	bool OnEventSocketFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//��������
public:
	//����״̬
	bool SendTableStatus();
	//����ʧ��
	bool SendRequestFailure(IServerUserItem * pIServerUserItem, BYTE cbErrType, LONG lErrorCode, LPCTSTR pszDescribe = NULL);

	//Ч�麯��
public:
	//��ʼЧ��
	bool EfficacyStartGame(WORD wReadyChairID);
	//��ַЧ��
	bool EfficacyIPAddress(IServerUserItem * pIServerUserItem);
	//����Ч��
	bool EfficacyScoreRule(IServerUserItem * pIServerUserItem);
	//����Ч��
	bool EfficacyEnterTableScoreRule(WORD wChairID, IServerUserItem * pIServerUserItem);

public:
	//������
	bool CheckDistribute();
	//��Ϸ��¼
	void RecordGameScore(bool bDrawStarted, DWORD dwStartGameTime=INVALID_DWORD);

	//���Ƚӿ�
public:
	//�����Ϣ
	void ExportInformation(LPCTSTR pszString, bool bError = false);
	//�����ʽ����Ϣ
	void ExportFormatInfo(bool bError, LPCTSTR pszFormat, ...);

	//˽�з���
private:
	//���;ȼý�
	void SendBenefitInfo(IServerUserItem * pIServerUserItem, SYSTEMTIME SystemTime);



	//˽�˳��ӿ�
public:
	//����˽�˳���Ϣ;
	virtual void SetPrivateInfo(tagPrivateFrameParameter &PriavateFrame);
	//��ȡ˽�˳�����;
	virtual tagPrivateFrameParameter* GetPrivateFrameInfo();
	//��ȡ��ǰ�������;
	virtual tagPrivateFrameRecordInfo* GetPrivateFrameRecord();
	//ͳ��˽�˳���Ҿ���
	virtual VOID addPrivatePlayCout(WORD wCout);
	//����˽�˳�������Ϣ;
	virtual void ResetPrivateEndInfo();
	//���˽�˳�������Ϣ��
	virtual void GetPrivateEndInfo(DataStream &kDataStream, bool bSend);
	//�ж�˽�˳��Ƿ����
	virtual	bool IsPrivateEnd();


	//�ƿ�ӿ�;
public:
	//��ȡ�ƿ�
	virtual tagGameCardLibrary* GetGameCardLibrary(DWORD dwCustomID = 0xFFFFFFFF);
	//�����ƿ�
	static void AddGameCardLibrary(tagGameCardLibrary* pGameCard);
	//�޸��ƿ�
	static void UpdateGameCardLibrary(DWORD dwLibID, DWORD dwCustomID, BYTE cbCardCount, BYTE* pCardData);
	//ɾ���ƿ�
	static void ClearGameCardLibrary();

};

//////////////////////////////////////////////////////////////////////////////////

#endif