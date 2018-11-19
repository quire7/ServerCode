#ifndef GAME_SERVICE_HEAD_HEAD_FILE
#define GAME_SERVICE_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////

//MFC�ļ�
#include <Afxtempl.h>

//ƽ̨����
#include "..\..\ȫ�ֶ���\Platform.h"

//��Ϣ����
#include "..\..\��Ϣ����\CMD_Commom.h"
#include "..\..\��Ϣ����\CMD_Correspond.h"
#include "..\..\��Ϣ����\CMD_GameServer.h"

//ƽ̨�ļ�
#include "..\..\���������\�������\ServiceCoreHead.h"
#include "..\..\���������\�ں�����\KernelEngineHead.h"

//////////////////////////////////////////////////////////////////////////////////
//��������

//��������
#ifndef GAME_SERVICE_CLASS
#ifdef  GAME_SERVICE_DLL
#define GAME_SERVICE_CLASS _declspec(dllexport)
#else
#define GAME_SERVICE_CLASS _declspec(dllimport)
#endif
#endif

//ģ�鶨��
#ifndef _DEBUG
#define GAME_SERVICE_DLL_NAME	TEXT("GameService.dll")				//��� DLL ����
#else
#define GAME_SERVICE_DLL_NAME	TEXT("GameServiceD.dll")			//��� DLL ����
#endif

//////////////////////////////////////////////////////////////////////////////////
//�ӿ�˵��

//�û��ӿ�
interface IServerUserItem;
interface IServerUserManager;
interface IServerUserItemSink;

//����ӿ�
interface IAndroidUserItem;
interface IAndroidUserManager;
interface IAndroidUserItemSink;

//���ӽӿ�
interface ITableFrame;
interface ITableFrameSink;
interface ITableUserAction;

//����ӿ�
interface IMainServiceFrame;
interface IGameServiceManager;
interface IGameServiceCustomRule;

//////////////////////////////////////////////////////////////////////////////////
//��������

//Ⱥ������
#define BG_MOBILE					(BYTE)(0x01)						//�ֻ�Ⱥ��
#define BG_COMPUTER					(BYTE)(0x02)						//����Ⱥ��
#define BG_ALL_CLIENT				(BYTE)(0xFF)						//ȫ��Ⱥ��

//��������
#define INDEX_SOCKET				(WORD)(0x0000)						//��������
#define INDEX_ANDROID				(WORD)(0x2000)						//��������

//��������
#define GAME_SERVICE_CREATE_NAME	"CreateGameServiceManager"			//��������

//////////////////////////////////////////////////////////////////////////////////
//��������
//��������
#define ANDROID_WAITSTANDUP			0x01								//�ȴ�����
#define ANDROID_WAITLEAVE			0x02								//�ȴ��뿪	

//����ԭ��
#define GER_NORMAL					0x00								//�������
#define GER_DISMISS					0x01								//��Ϸ��ɢ
#define GER_USER_LEAVE				0x02								//�û��뿪
#define GER_NETWORK_ERROR			0x03								//�������
#define GER_BAOZI					0x04								//�˰���

//�뿪ԭ��
#define LER_NORMAL					0x00								//�����뿪
#define LER_SYSTEM					0x01								//ϵͳԭ��
#define LER_NETWORK					0x02								//����ԭ��
#define LER_USER_IMPACT				0x03								//�û���ͻ
#define LER_SERVER_FULL				0x04								//����Ϊ��
#define LER_SERVER_CONDITIONS		0x05								//��������

//��������
#define SCORE_TYPE_NULL				0x00								//��Ч����
#define SCORE_TYPE_WIN				0x01								//ʤ�ֻ���
#define SCORE_TYPE_LOSE				0x02								//��ֻ���
#define SCORE_TYPE_DRAW				0x03								//�;ֻ���
#define SCORE_TYPE_FLEE				0x04								//�Ӿֻ���
#define SCORE_TYPE_PRESENT			0x10								//���ͻ���
#define SCORE_TYPE_SERVICE			0x11								//�������

//��ʼģʽ
#define START_MODE_ALL_READY		0x00								//����׼��
#define START_MODE_FULL_READY		0x01								//���˿�ʼ
#define START_MODE_PAIR_READY		0x02								//��Կ�ʼ
#define START_MODE_TIME_CONTROL		0x10								//ʱ�����
#define START_MODE_MASTER_CONTROL	0x11								//�������

//����ѡ��
#define DISTRIBUTE_ALLOW			0x01								//�������
#define DISTRIBUTE_IMMEDIATE		0x02								//����ѡ��
#define DISTRIBUTE_LAST_TABLE		0x04								//ͬ��ѡ��
#define DISTRIBUTE_SAME_ADDRESS		0x08								//��ַѡ��

//////////////////////////////////////////////////////////////////////////////////
//ʱ���ʶ

//���ȷ�Χ
#define IDI_MAIN_MODULE_START		1									//��ʼ��ʶ
#define IDI_MAIN_MODULE_FINISH		99									//��ֹ��ʶ

//������Χ
#define IDI_REBOT_MODULE_START		100									//��ʼ��ʶ
#define IDI_REBOT_MODULE_FINISH		199									//��ֹ��ʶ

//������Χ
#define IDI_MATCH_MODULE_START		200									//��ʼ��ʶ
#define IDI_MATCH_MODULE_FINISH		999									//������ʶ

//˽�˳���Χ(����);
#define IDI_PRIVATE_MODULE_START		300								//��ʼ��ʶ
#define IDI_PRIVATE_MODULE_FINISH		399								//������ʶ

//���ӷ�Χ
#define IDI_TABLE_MODULE_START		10000								//��ʼ��ʶ
#define IDI_TABLE_MODULE_FINISH		50000								//��ֹ��ʶ

//////////////////////////////////////////////////////////////////////////////////
//ʱ�䷶Χ

//��Ϸʱ��
#define TIME_TABLE_SINK_RANGE		25									//��ʶ��Χ
#define TIME_TABLE_KERNEL_RANGE		30									//��ʶ��Χ
#define TIME_TABLE_MODULE_RANGE		50									//��ʶ��Χ

//////////////////////////////////////////////////////////////////////////////////
//��������

//��������
#define RESERVE_USER_COUNT			40L									//������Ŀ

//////////////////////////////////////////////////////////////////////////////////
//�ṹ����

//��������
struct tagGameServiceAttrib
{
	//�ں�����
	WORD							wKindID;							//���ƺ���
	WORD							wChairCount;						//������Ŀ
	WORD							wSupporType;						//֧������
	TCHAR							szGameName[LEN_KIND];				//��Ϸ����

	//���ܱ�־
	BYTE							cbAndroidUser;						//������־
	BYTE							cbDynamicJoin;						//��̬����
	BYTE							cbOffLineTrustee;					//���ߴ���

	//��������
	DWORD							dwServerVersion;					//��Ϸ�汾
	DWORD							dwClientVersion;					//��Ϸ�汾
	TCHAR							szDataBaseName[32];					//��Ϸ����
	TCHAR							szServerDLLName[LEN_PROCESS];		//��������
	TCHAR							szClientEXEName[LEN_PROCESS];		//��������
};

//��������
struct tagGameServiceOption
{
	//�ҽ�����
	WORD							wKindID;							//�ҽ�����
	//WORD							wNodeID;							//�ҽӽڵ�
	WORD							wSortID;							//���б�ʶ
	WORD							wServerID;							//�����ʶ

	//˰������
	LONG							lCellScore;							//��λ����
	WORD							wRevenueRatio;						//˰�ձ���
	SCORE							lServiceScore;						//�������

	//��������
	SCORE							lRestrictScore;						//���ƻ���
	SCORE							lMinTableScore;						//��ͻ���
	SCORE							lMinEnterScore;						//��ͻ���
	SCORE							lMaxEnterScore;						//��߻���

	//��������
	SCORE							lDefPutScore;						//Ĭ��һ�δ�����ٻ���
	SCORE							lLowPutScore;						//���ڶ��ٴ������
	int								iExchangeRatio;						//�һ�����

	//��������
	SCORE							lMagicExpScore;						//ħ���������
	SCORE							lGratuityScore;						//���ͻ���
	DWORD							dwWinExperience;					//ʤ������
	DWORD							dwFailExperience;					//ʧ�ܾ���

	//��Ա����
	BYTE							cbMinEnterMember;					//��ͻ�Ա
	BYTE							cbMaxEnterMember;					//��߻�Ա

	//��������
	DWORD							dwServerRule;						//�������
	DWORD							dwAttachUserRight;					//����Ȩ��

	//��������
	WORD							wMaxPlayer;							//�����Ŀ
	WORD							wTableCount;						//������Ŀ
	WORD							wServerPort;						//����˿�
	WORD                            wServerKind;                        //������� 
	WORD							wServerType;						//��������
	TCHAR							szServerName[LEN_SERVER];			//��������
	TCHAR                           szServerPasswd[LEN_PASSWORD];       //�������� 

	//��������
	BYTE							cbDistributeRule;					//�������
	WORD							wMinDistributeUser;					//��������
	WORD							wDistributeTimeSpace;				//������
	WORD							wDistributeDrawCount;				//�������
	WORD							wMinPartakeGameUser;				//��������
	WORD							wMaxPartakeGameUser;				//�������

	////��������
	//TCHAR							szDataBaseAddr[16];					//���ӵ�ַ
	//TCHAR							szDataBaseName[32];					//���ݿ���

	TCHAR							szCacheServerAddr[64];				//�����������ַ
	WORD							wCacheServerPort;					//����������˿�
	TCHAR							szCacheServerPass[LEN_PASSWORD];	//�������������

	//�Զ�����
	BYTE							cbCustomRule[1024];					//�Զ�����
};

//��������
struct tagGameMatchOption
{
	//������Ϣ
	DWORD							dwMatchID;							//������ʶ	
	DWORD							dwMatchNO;							//��������
	BYTE							cbMatchType;						//��������
	TCHAR							szMatchName[32];					//��������

	//������Ϣ
	SCORE							lMatchFee;							//��������
	BYTE							cbMatchFeeType;						//�۷�����	
	BYTE							cbMemberOrder;						//��Ա�ȼ�

	//������Ϣ
	union unGameMatchType
	{
		//��ʱ��
		struct tagLockTimeMatch
		{
			//ʱ������
			SYSTEMTIME						MatchStartTime;				//��ʼ����
			SYSTEMTIME						MatchEndTime;				//��������	

			//��������
			SCORE							lMatchInitScore;			//��ʼ����
			SCORE							lMatchCullScore;			//��̭����

			//��������
			WORD							wMatchPlayCount;			//���پ���
		} LockTime;

		//��ʱ��
		struct tagImmediateMatch
		{
			//������Ϣ
			WORD							wStartUserCount;			//��������	
			WORD							wAndroidUserCount;			//��������

			//������Ϣ
			DWORD							dwInitalBase;				//��ʼ����
			DWORD							dwInitalScore;				//��ʼ����=0ʱ ����ʹ�õ�ǰ���Ϊ����	
			WORD							wMinEnterGold;				//��ʼ����=0ʱ ������ٱ���Ҳ��б�������

			//������Ϣ
			BYTE							cbPlayCount;				//��Ϸ����
			BYTE							cbSwitchTableCount;			//��������(0Ϊ������)
			WORD							wPrecedeTimer;				//��������
		} Immediate;	
	} MatchType;

	//������Ϣ
	WORD							wRewardCount;						//��������

	//��Ϸ����	
	TCHAR							szMatchAwardContent[128];			//��������	
	TCHAR							szMatchContent[512];				//��������	
};


// ˽�˳�����
struct tagPrivateFrameParameter
{
	// ���ù���
	BYTE							cbGameTypeIdex;					//���ݺ����齫��һ����Ϸ�������������;
	BYTE							cbGameCout;						//��Ϸ����;
	DWORD							dwGameRule;						//��Ϸ����;
};

// ˽�˳������Ϣ
struct tagPrivateFrameRecordInfo
{
	BYTE							cbFinishCout;					//��ɾ���
};

//������Ϣ;
struct tagMatchRankInfo
{
	WORD							wRankID;							//��������
	DWORD							dwUserID;							//�û���ʶ
	SCORE							lMatchScore;						//��������
	SCORE							lRewardGold;						//�������
	DWORD							dwRewardIngot;						//����Ԫ��
	DWORD							dwRewardExperience;					//��������
};

//���Ӳ���
struct tagTableFrameParameter
{
	//�ں����
	ITimerEngine *					pITimerEngine;						//ʱ������
	IDataBaseEngine *				pIRecordDataBaseEngine;				//��������
	IDataBaseEngine *				pIKernelDataBaseEngine;				//��������

	//�������
	IMainServiceFrame *				pIMainServiceFrame;					//������
	IAndroidUserManager *			pIAndroidUserManager;				//��������
	IGameServiceManager *			pIGameServiceManager;				//�������

	//���ñ���
	tagGameServiceAttrib *			pGameServiceAttrib;					//��������
	tagGameServiceOption *			pGameServiceOption;					//���ò���
	tagBenefitInfo*					pBenefitInfo;						//�ȼý�
};

//��������
struct tagAndroidUserParameter
{
	//��������
	bool							bServiceContinue;					//��������
	DWORD							dwMinSitInterval;					//���¼��
	DWORD							dwMaxSitInterval;					//���¼��

	//���ñ���
	tagGameServiceAttrib *			pGameServiceAttrib;					//��������
	tagGameServiceOption *			pGameServiceOption;					//���ò���

	//�ں����
	ITimerEngine *					pITimerEngine;						//ʱ������
	IServerUserManager *			pIServerUserManager;				//�û�����
	IGameServiceManager *			pIGameServiceManager;				//�������
	ITCPNetworkEngineEvent *		pITCPNetworkEngineEvent;			//�¼��ӿ�
};

//////////////////////////////////////////////////////////////////////////////////
//�����ṹ;

//�ƿ�
struct tagGameCardLibrary
{
	WORD							wKindID;
	DWORD							dwLibID;
	DWORD							dwCustomID;
	BYTE							cbCount;				//����
	BYTE							cbData[LEN_CARD_LIB];	//����
};

//������Ϣ
struct tagScoreInfo
{
	BYTE							cbType;								//��������
	SCORE							lScore;								//�û�����
	SCORE							lGrade;								//�û��ɼ�
	SCORE							lRevenue;							//��Ϸ˰��
};

//�����Ϣ
struct tagVariationInfo
{
	//������Ϣ
	SCORE							lScore;								//�û�����
	SCORE							lGrade;								//�û��ɼ�
	SCORE							lInsure;							//�û�����
	SCORE							lRevenue;							//��Ϸ˰��	

	//��Ϸ��Ϣ
	DWORD							dwWinCount;							//ʤ������
	DWORD							dwLostCount;						//ʧ������
	DWORD							dwDrawCount;						//�;�����
	DWORD							dwFleeCount;						//��������
	DWORD							dwPlayTimeCount;					//��Ϸʱ��

	//ȫ����Ϣ
	SCORE							lIngot;								//�û�Ԫ��
	DWORD							dwExperience;						//�û�����
	LONG							lLoveLiness;						//�û�����
};

//��Ϸ��¼
struct tagGameScoreRecord
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	WORD							wChairID;							//���Ӻ���
	BYTE							cbAndroid;							//������־

	//�ɼ���Ϣ
	SCORE							lScore;								//�û�����
	SCORE							lGrade;								//�û��ɼ�
	SCORE							lRevenue;							//��Ϸ˰��

	//�û���Ϣ
	DWORD							dwDBQuestID;						//�����ʶ
	DWORD							dwInoutIndex;						//��¼����

	//������Ϣ
	DWORD							dwUserMemal;						//������Ŀ
	DWORD							dwPlayTimeCount;					//��Ϸʱ��
};

//�û�����
struct tagUserRule
{
	//�����־
	bool							bLimitSameIP;						//Ч���ַ
	bool							bLimitWinRate;						//����ʤ��
	bool							bLimitFleeRate;						//��������
	bool							bLimitGameScore;					//���Ʒ���

	//��������
	WORD							wMinWinRate;						//���ʤ��
	WORD							wMaxFleeRate;						//�������
	LONG							lMaxGameScore;						//��߷��� 
	LONG							lMinGameScore;						//��ͷ���
	TCHAR							szPassword[LEN_PASSWORD];			//��������
};

//�������
struct tagUserTaskInfo
{
	DWORD							dwID;								//��������
	WORD							wTaskID;							//�����ʶ
	BYTE							cbTaskStatus;						//����״̬	
	WORD							wTaskProgress;						//�������
	DWORD							dwResidueTime;						//ʣ��ʱ��
	DWORD							dwLastUpdateTime;					//����ʱ��	
};

//�������
struct tagUserTaskEntry
{
	DWORD							dwID;								//��������
	BYTE							cbTaskStatus;						//����״̬
	WORD							wTaskProgress;						//�������
	DWORD							dwResidueTime;						//ʣ��ʱ��
	DWORD							dwLastUpdateTime;					//����ʱ��		
	tagTaskParameter *				pTaskParameter;						//�������
	tagUserTaskEntry *				pNextTaskEntry;						//��һ����	
	tagUserTaskEntry *				pNextStatusEntry;					//��һ����	
};

//�û���Ϣ
struct tagUserInfoPlus
{
	//��¼��Ϣ
	DWORD							dwLogonTime;						//��¼ʱ��
	DWORD							dwInoutIndex;						//������ʶ

	//������Ϣ
	WORD							wBindIndex;							//������
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ

	//���ӱ���
	DWORD							dwUserRight;						//�û�Ȩ��
	DWORD							dwMasterRight;						//����Ȩ��
	SCORE							lRestrictScore;						//���ƻ���
	
	//��������
	SCORE							lDefPutScore;						//Ĭ��һ�δ�����ٻ���
	SCORE							lLowPutScore;						//���ڶ��ٴ������
	int								iExchangeRatio;						//�һ�����

	//�������
	BYTE							cbMaxCardType;
	BYTE							aryCardData[MAX_CARD_DATA_COUNT];

	//��������
	bool							bAndroidUser;						//�����û�
	TCHAR							szPassword[LEN_MD5];				//��������
};

//������
struct tagRequestResult
{
	BYTE							cbFailureCode;						//ʧ��ԭ��
	TCHAR							szFailureReason[128];				//ʧ��ԭ��
};

//����״��
struct tagTableUserInfo
{
	WORD							wMinUserCount;						//��������
	WORD							wTableUserCount;					//�û���Ŀ
	WORD							wTableReadyCount;					//׼����Ŀ
	WORD							wTableAndroidCount;					//������Ŀ
};

//����״��
struct tagAndroidUserInfo
{
	WORD							wFreeUserCount;						//�û���Ŀ
	WORD							wPlayUserCount;						//�û���Ŀ
	WORD							wSitdownUserCount;					//�û���Ŀ
	IAndroidUserItem *				pIAndroidUserFree[MAX_ANDROID];		//�����ӿ�
	IAndroidUserItem *				pIAndroidUserPlay[MAX_ANDROID];		//�����ӿ�
	IAndroidUserItem *				pIAndroidUserSitdown[MAX_ANDROID];	//�����ӿ�
};

//������Ϣ
struct tagAndroidService
{
	DWORD							dwSwitchTableInnings;				//��Ϸ����
};

//�ʺ���Ϣ
struct tagAndroidAccountsInfo
{
	DWORD							dwUserID;							//������ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//��������
};

//��������
struct tagAndroidItemConfig
{
	tagAndroidAccountsInfo			AndroidAccountsInfo;				//�ʺ���Ϣ		
	tagAndroidParameter *			pAndroidParameter;					//��������
};

//����״̬
enum EN_MatchStatus
{
	EN_NULL=0,
	EN_OUT,
};

//�ȼý𷢷ųɹ�
struct tagSendBenefitSuc
{
	DWORD							dwUserID;							//�û�ID	
	BYTE							cbBenefitTimes;						//���Ŵ���
	SYSTEMTIME						LastBenefitTime;					//����ʱ��
};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IGameServiceManager INTERFACE_VERSION(1,1)
static const GUID IID_IGameServiceManager={0xa975cceb,0x0331,0x4553,0xa1,0xe0,0xa7,0xc7,0x7a,0x7c,0x4e,0xfd};
#else
#define VER_IGameServiceManager INTERFACE_VERSION(1,1)
static const GUID IID_IGameServiceManager={0x4b2b9d8f,0xce1b,0x44f3,0xa5,0x22,0x65,0x1a,0x65,0xc9,0x0a,0x25};
#endif

//��Ϸ�ӿ�
interface IGameServiceManager : public IUnknownEx
{
	//�����ӿ�
public:
	//��������
	virtual VOID * CreateTableFrameSink(REFGUID Guid, DWORD dwQueryVer)=NULL;
	//��������
	virtual VOID * CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer)=NULL;
	//��������
	virtual VOID * CreateGameDataBaseEngineSink(REFGUID Guid, DWORD dwQueryVer)=NULL;

	//�����ӿ�
public:
	//�������
	virtual bool GetServiceAttrib(tagGameServiceAttrib & GameServiceAttrib)=NULL;
	//��������
	virtual bool RectifyParameter(tagGameServiceOption & GameServiceOption)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IGameServiceCustomRule INTERFACE_VERSION(1,1)
static const GUID IID_IGameServiceCustomRule={0x74a43b7d,0x7c29,0x4fb8,0xa5,0x00,0x8e,0x8d,0xff,0x6e,0x2a,0xdd};
#else
#define VER_IGameServiceCustomRule INTERFACE_VERSION(1,1)
static const GUID IID_IGameServiceCustomRule={0xc7ace01d,0x75f8,0x4af7,0xb1,0x80,0xa8,0x53,0xcd,0x2e,0x0a,0xb6};
#endif

//���ýӿ�
interface IGameServiceCustomRule : public IUnknownEx
{
	//��ȡ����
	virtual bool SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)=NULL;
	//Ĭ������
	virtual bool DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)=NULL;
	//��������
	virtual HWND CreateCustomRule(CWnd * pParentWnd, CRect rcCreate, LPBYTE pcbCustomRule, WORD wCustonSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IGameDataBaseEngine INTERFACE_VERSION(1,1)
static const GUID IID_IGameDataBaseEngine={0x47dcd531,0x2a19,0x4c0a,0x89,0xb6,0x9f,0xd8,0xe0,0xa7,0x85,0xee};
#else
#define VER_IGameDataBaseEngine INTERFACE_VERSION(1,1)
static const GUID IID_IGameDataBaseEngine={0x4310e733,0xc49b,0x4592,0xa3,0xb1,0x73,0x18,0xd1,0x53,0x2f,0x3e};
#endif

//��Ϸ����
interface IGameDataBaseEngine : public IUnknownEx
{
	//���ò���
public:
	//�Զ�����
	virtual VOID * GetCustomRule()=NULL;
	//��������
	virtual tagGameServiceAttrib * GetGameServiceAttrib()=NULL;
	//��������
	virtual tagGameServiceOption * GetGameServiceOption()=NULL;

	//��ȡ����
public:
	//��ȡ����
	virtual VOID * GetDataBase(REFGUID Guid, DWORD dwQueryVer)=NULL;
	//��ȡ����
	virtual VOID * GetDataBaseEngine(REFGUID Guid, DWORD dwQueryVer)=NULL;

	//���ܽӿ�
public:
	//Ͷ�ݽ��
	virtual bool PostGameDataBaseResult(WORD wRequestID, VOID * pData, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IGameDataBaseEngineSink INTERFACE_VERSION(1,1)
static const GUID IID_IGameDataBaseEngineSink={0x9b9111d9,0x7a71,0x41a1,0xae,0x78,0xd4,0xf3,0x20,0x08,0x24,0xdf};
#else
#define VER_IGameDataBaseEngineSink INTERFACE_VERSION(1,1)
static const GUID IID_IGameDataBaseEngineSink={0xa6c5e2cc,0x34c1,0x422c,0xa0,0x1b,0x54,0xab,0x68,0xfa,0xe6,0x81};
#endif

//���ݽӿ�
interface IGameDataBaseEngineSink : public IUnknownEx
{
	//���ýӿ�
	virtual bool InitializeSink(IUnknownEx * pIUnknownEx)=NULL;
	//���ݴ���
	virtual bool OnGameDataBaseRequest(DWORD dwUserID, WORD wTableID, WORD wChairID);
};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IServerUserItem INTERFACE_VERSION(1,1)
static const GUID IID_IServerUserItem={0xcd43dce8,0x1e12,0x43be,0x8b,0x4f,0x94,0x95,0x92,0xa4,0xf6,0x19};
#else
#define VER_IServerUserItem INTERFACE_VERSION(1,1)
static const GUID IID_IServerUserItem={0xb5ce01a7,0x5cd1,0x4788,0x94,0x6c,0xa1,0xef,0x5b,0x30,0x2c,0xb7};
#endif

//�û��ӿ�
interface IServerUserItem : public IUnknownEx
{
	//������Ϣ
public:
	//�û�����
	virtual WORD GetBindIndex()=NULL;
	//�û���ַ
	virtual DWORD GetClientAddr()=NULL;
	//������ʶ
	virtual LPCTSTR GetMachineID()=NULL;
	//�Ƿ�����
	virtual bool IsOnline()=NULL;

	//��¼��Ϣ
public:
	//�����ʶ
	virtual DWORD GetDBQuestID()=NULL;
	//��¼ʱ��
	virtual DWORD GetLogonTime()=NULL;
	//��¼����
	virtual DWORD GetInoutIndex()=NULL;

	//�û���Ϣ
public:
	//�û���Ϣ
	virtual tagUserInfo * GetUserInfo()=NULL;
	//�û�����
	virtual tagUserRule * GetUserRule()=NULL;
	//������Ϣ
	//virtual tagUserProperty * GetUserProperty()=NULL;
	virtual CYSArray<tagUserProperty> & GetUserProperty() = NULL;
	//���õ���
	virtual void SetUserProperty(tagUserProperty &Property) = NULL;
	//���ӵ���
	virtual void AddUserProperty(tagUserProperty &Property) = NULL;
	//ʹ�õ���
	virtual EN_PROPERTY_ERROR UsedProperty(CMD_GR_C_Property *pProperty, SYSTEMTIME &SystemTime, DWORD &dwPropDBID) = NULL;
	//ʱ����Ϣ
	virtual tagTimeInfo * GetTimeInfo()=NULL;

	//������Ϣ
public:
	//�û��Ա�
	virtual BYTE GetGender()=NULL;
	//�û���ʶ
	virtual DWORD GetUserID()=NULL;
	//�û��ǳ�
	virtual LPCTSTR GetNickName()=NULL;
	//�û�����
	virtual LPCTSTR GetPassword() = NULL;

	//״̬�ӿ�
public:
	//���Ӻ���
	virtual WORD GetTableID()=NULL;
	//���Ӻ���
	virtual WORD GetLastTableID()=NULL;
	//���Ӻ���
	virtual WORD GetChairID()=NULL;
	//�û�״̬
	virtual BYTE GetUserStatus()=NULL;

	//Ȩ����Ϣ
public:
	//�û�Ȩ��
	virtual DWORD GetUserRight()=NULL;
	//����Ȩ��
	virtual DWORD GetMasterRight()=NULL;

	//Ȩ����Ϣ
public:
	//�û�Ȩ��
	virtual BYTE GetMemberOrder()=NULL;
	//����Ȩ��
	virtual BYTE GetMasterOrder()=NULL;

	//������Ϣ
public:
	//�û����
	virtual SCORE GetUserGold() = NULL;
	//�û�����
	virtual SCORE GetUserScore()=NULL;
	//�û���ʯ
	virtual SCORE GetUserDiamond()=NULL;
	//�û�����
	virtual SCORE GetUserInsure()=NULL;

	//����������
	virtual BYTE GetMaxCardData(BYTE* pData, BYTE cbCount) = NULL;

	//�ȼý���Ϣ
public:
	//��ȡ����
	virtual BYTE GetBenefitTimes() = NULL;
	//�ϴ���ȡʱ��
	virtual SYSTEMTIME GetLastBenefitTime() = NULL;
	//������ȡ��Ϣ
	virtual	VOID SetBenefitInfo(tagSendBenefitSuc* pbenefit) = NULL;

	//�й���Ϣ
public:
	//�йܻ���
	virtual SCORE GetTrusteeScore()=NULL;
	//��������
	virtual SCORE GetFrozenedScore()=NULL;
	//�������
	virtual SCORE TakeGold(SCORE lScore, bool isAtOnce = true) = NULL;
	//��������
	virtual bool WriteOtherScore(SCORE lScore, BYTE cbType) = NULL;

	//������Ϣ
public:
	//�û�ʤ��
	virtual WORD GetUserWinRate()=NULL;
	//�û�����
	virtual WORD GetUserLostRate()=NULL;
	//�û�����
	virtual WORD GetUserDrawRate()=NULL;
	//�û�����
	virtual WORD GetUserFleeRate()=NULL;
	//��Ϸ����
	virtual DWORD GetUserPlayCount()=NULL;

	//Ч��ӿ�
public:
	//�Ա��ʺ�
	virtual bool ContrastNickName(LPCTSTR pszNickName)=NULL;
	//�Ա�����
	virtual bool ContrastLogonPass(LPCTSTR pszPassword)=NULL;

	//�й�״̬
public:
	//�ж�״̬
	virtual bool IsTrusteeUser()=NULL;
	//����״̬
	virtual VOID SetTrusteeUser(bool bTrusteeUser)=NULL;

	//��Ϸ״̬
public:
	//����״̬
	virtual bool IsClientReady()=NULL;
	//��������
	virtual VOID SetClientReady(bool bClientReady)=NULL;
	//��ԭ״̬;
	virtual BYTE GetReUserStatus() = NULL;
	//���û�ԭ״̬;
	virtual void SetReUserStatus(BYTE cbReUserStatus) = NULL;

	//�����û�
public:
	//����״̬
	virtual bool IsAndroidUser()=NULL;
	//���ÿ���
	virtual VOID SetAndroidUser(bool bbMachineUser)=NULL;
	//�����û�
	virtual bool IsCheatUser() = NULL;
	//���������û�
	virtual void SetCheatUser(bool bFlag) = NULL;

	//�����ӿ�
public:
	//��������
	virtual VOID * GetMatchData()=NULL;
	//��������
	virtual VOID SetMatchData(VOID * pMatchData)=NULL;
	//����ʱ��
	virtual DWORD GetSignUpTime()=NULL;
	//����ʱ��
	virtual VOID SetSignUpTime(DWORD dwSignUpTime)=NULL;
	//����״̬
	virtual BYTE GetSignUpStatus()=NULL;
	//����״̬
	virtual VOID SetSignUpStatus(BYTE cbSignUpStatus)=NULL;

	//��¼�ӿ�
public:
	//����ж�
	virtual bool IsVariation()=NULL;
	//��ѯ��¼
	virtual bool QueryRecordInfo(tagVariationInfo & UserRecordInfo)=NULL;
	//��ȡ���
	virtual bool DistillVariation(tagVariationInfo & UserVariationInfo)=NULL;

	//����ӿ�
public:
	//����״̬
	virtual bool SetUserStatus(BYTE cbUserStatus, WORD wTableID, WORD wChairID)=NULL;
	//д�����
	virtual bool WriteUserScore(SCORE lScore, SCORE lGrade, SCORE lRevenue, SCORE lIngot, BYTE cbScoreType, DWORD dwPlayTimeCount,DWORD dwWinExperience)=NULL;
	//�޸�Ȩ��
	virtual VOID ModifyUserRight( DWORD dwAddRight, DWORD dwRemoveRight, BYTE cbRightKind=UR_KIND_GAME)=NULL;

	//����ӿ�
public:
	//�������
	virtual bool FrozenedUserScore(SCORE lScore)=NULL;
	//�ⶳ����
	virtual bool UnFrozenedUserScore(SCORE lScore)=NULL;

	//�޸Ľӿ�
public:
	//�޸���Ϣ
	virtual bool ModifyUserProperty(SCORE lScore, LONG lLoveLiness)=NULL;
	//����������
	virtual bool ModifyMaxCardType(BYTE cbCurType, BYTE aryData[], BYTE cbPokerCount)=NULL;

	//�߼��ӿ�
public:
	//�����
	virtual bool DetachBindStatus()=NULL;
	//���в���
	virtual bool ModifyUserInsure(SCORE lScore, SCORE lInsure, SCORE lRevenue)=NULL;
	//���ò���
	virtual bool SetUserParameter(DWORD dwClientAddr, WORD wBindIndex, TCHAR szMachineID[LEN_MACHINE_ID], bool bAndroidUser, bool bClientReady)=NULL;

	//�ƿ�ӿ�
public:
	//�Ƿ����ʹ���ƿ�
	virtual bool canUseCardLib() = NULL;
	//��ȡ�ƿ�����
	virtual DWORD getCardLibIndex() = NULL;
	//�����ƿ�����
	virtual void setCardLibIndex(DWORD dwCardLibIndex) = NULL;
	//�ۼ��ƿ�����
	virtual void addCardLibIndex() = NULL;
	//�����ƿ������
	virtual void setCardLibEnabled(bool bEnabled) = NULL;
	//�����ƿ�ʹ��ʱ���
	virtual void setUseCardLibTime(DWORD dwStartTime, DWORD dwEndTime) = NULL;
};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IServerUserService INTERFACE_VERSION(1,1)
static const GUID IID_IServerUserService={0x2f4e25a9,0xad87,0x4a37,0x98,0x96,0x5c,0x50,0xf9,0x91,0x05,0x31};
#else
#define VER_IServerUserService INTERFACE_VERSION(1,1)
static const GUID IID_IServerUserService={0xcc4e885f,0x3357,0x4d68,0xb2,0x6b,0x78,0x8f,0x4b,0x3e,0x64,0xc3};
#endif

//�û�����
interface IServerUserService : public IUnknownEx
{
};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IServerUserItemSink INTERFACE_VERSION(1,1)
static const GUID IID_IServerUserItemSink={0x415be3e4,0xd48d,0x4a77,0x94,0xb7,0xd1,0x05,0xcd,0xa3,0x82,0x81};
#else
#define VER_IServerUserItemSink INTERFACE_VERSION(1,1)
static const GUID IID_IServerUserItemSink={0x9d0cfe02,0x0fe9,0x4a8b,0x97,0x95,0xac,0x32,0x67,0x5a,0xf8,0xb1};
#endif

//״̬�ӿ�
interface IServerUserItemSink : public IUnknownEx
{
	//�û�����
	virtual bool OnEventUserItemScore(IServerUserItem * pIServerUserItem, BYTE cbReason)=NULL;
	//�û�״̬
	virtual bool OnEventUserItemStatus(IServerUserItem * pIServerUserItem, WORD wOldTableID=INVALID_TABLE, WORD wOldChairID=INVALID_CHAIR)=NULL;
	//�û�Ȩ��
	virtual bool OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight,BYTE cbRightKind)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IServerUserManager INTERFACE_VERSION(1,1)
static const GUID IID_IServerUserManager={0xeb413ed6,0x185b,0x4ceb,0xa8,0xbd,0x54,0x74,0x19,0x89,0x6a,0x53};
#else
#define VER_IServerUserManager INTERFACE_VERSION(1,1)
static const GUID IID_IServerUserManager={0x77a3c4df,0x1d95,0x48c6,0xac,0x9d,0x75,0xd7,0x6c,0x2a,0x3c,0x0e};
#endif

//�û�����
interface IServerUserManager : public IUnknownEx
{
	//���ýӿ�
public:
	//���ýӿ�
	virtual bool SetServerUserItemSink(IUnknownEx * pIUnknownEx)=NULL;

	//���ҽӿ�
public:
	//ö���û�
	virtual IServerUserItem * EnumUserItem(WORD wEnumIndex)=NULL;
	//�����û�
	virtual IServerUserItem * SearchUserItem(DWORD dwUserID)=NULL;
	//�����û�
	virtual IServerUserItem * SearchUserItem(LPCTSTR pszNickName)=NULL;

	//ͳ�ƽӿ�
public:
	//��������
	virtual DWORD GetAndroidCount()=NULL;
	//��������
	virtual DWORD GetUserItemCount()=NULL;

	//����ӿ�
public:
	//ɾ���û�
	virtual bool DeleteUserItem()=NULL;
	//ɾ���û�
	virtual bool DeleteUserItem(IServerUserItem * pIServerUserItem)=NULL;
	//�����û�
	virtual bool InsertUserItem(IServerUserItem * * pIServerUserResult, tagUserInfo & UserInfo,tagUserInfoPlus & UserInfoPlus)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_ITableFrame INTERFACE_VERSION(1,1)
static const GUID IID_ITableFrame={0x2e577d5f,0x1e01,0x44ff,0x9f,0xf4,0x01,0x16,0x23,0x1b,0x76,0x15};
#else
#define VER_ITableFrame INTERFACE_VERSION(1,1)
static const GUID IID_ITableFrame={0x860cd1cf,0x1a4f,0x4e35,0xb8,0x0c,0xd2,0x46,0xa4,0xef,0xfd,0xfb};
#endif

//���ӽӿ�
interface ITableFrame : public IUnknownEx
{
	//���Խӿ�
public:
	//���Ӻ���
	virtual WORD GetTableID()=NULL;
	//��Ϸ����
	virtual WORD GetChairCount()=NULL;
	//��λ����Ŀ
	virtual WORD GetNullChairCount()=NULL;
	//�û���Ŀ
	virtual WORD GetSitUserCount() = NULL;

	//���ò���
public:
	//�Զ�����
	virtual VOID * GetCustomRule()=NULL;
	//��������
	virtual tagGameServiceAttrib * GetGameServiceAttrib()=NULL;
	//��������
	virtual tagGameServiceOption * GetGameServiceOption()=NULL;

	//���ýӿ�
public:
	//��ʼģʽ
	virtual BYTE GetStartMode()=NULL;
	//��ʼģʽ
	virtual VOID SetStartMode(BYTE cbStartMode)=NULL;

	//��Ԫ����
public:
	//��Ԫ����
	virtual LONG GetCellScore()=NULL;
	//��Ԫ����
	virtual VOID SetCellScore(LONG lCellScore)=NULL;

	//��Ϣ�ӿ�
public:
	//����״̬
	virtual bool IsTableLocked()=NULL;
	//��Ϸ״̬
	virtual bool IsGameStarted()=NULL;
	//��Ϸ״̬
	virtual bool IsDrawStarted()=NULL;
	//��Ϸ״̬
	virtual bool IsTableStarted()=NULL;

	//״̬�ӿ�
public:
	//��ȡ״̬
	virtual BYTE GetGameStatus()=NULL;
	//����״̬
	virtual VOID SetGameStatus(BYTE bGameStatus)=NULL;

	//���ƽӿ�
public:
	//��ʼ��Ϸ
	virtual bool StartGame()=NULL;
	//��ɢ��Ϸ
	virtual bool DismissGame()=NULL;
	//������Ϸ
	virtual bool ConcludeGame(BYTE cbGameStatus)=NULL;

	//д�ֽӿ�
public:
	//д�����
	virtual bool WriteUserScore(WORD wChairID, tagScoreInfo & ScoreInfo, DWORD dwGameMemal=INVALID_DWORD, DWORD dwPlayGameTime=INVALID_DWORD)=NULL;
	//д�����
	virtual bool WriteTableScore(tagScoreInfo ScoreInfoArray[], WORD wScoreCount, DataStream& kData) = NULL;

	//����ӿ�
public:
	//����˰��
	virtual SCORE CalculateRevenue(WORD wChairID, SCORE lScore)=NULL;
	//�����޶�
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem)=NULL;

	//�û��ӿ�
public:
	//Ѱ���û�
	virtual IServerUserItem * SearchUserItem(DWORD dwUserID)=NULL;
	//��Ϸ�û�
	virtual IServerUserItem * GetTableUserItem(WORD wChairID)=NULL;
	//�Թ��û�
	virtual IServerUserItem * EnumLookonUserItem(WORD wEnumIndex)=NULL;

	//ʱ��ӿ�
public:
	//����ʱ��
	virtual bool SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter)=NULL;
	//ɾ��ʱ��
	virtual bool KillGameTimer(DWORD dwTimerID)=NULL;

	//����ӿ�
public:
	//��������
	virtual bool SendTableData(WORD wChairID, WORD wSubCmdID)=NULL;
	//��������
	virtual bool SendTableData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize,WORD wMainCmdID=MDM_GF_GAME)=NULL;
	//��������
	virtual bool SendLookonData(WORD wChairID, WORD wSubCmdID)=NULL;
	//��������
	virtual bool SendLookonData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize, WORD wMainCmdID = MDM_GF_GAME) = NULL;
	//��������
	virtual bool SendUserItemData(IServerUserItem * pIServerUserItem, WORD wSubCmdID)=NULL;
	//��������
	virtual bool SendUserItemData(IServerUserItem * pIServerUserItem, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;

	//���ܽӿ�
public:
	//������Ϣ
	virtual bool SendGameMessage(LPCTSTR lpszMessage, WORD wType)=NULL;
	//��Ϸ��Ϣ
	virtual bool SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)=NULL;
	//������Ϣ
	virtual bool SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)=NULL;

	//��������
public:
	//��������
	virtual bool PerformStandUpAction(IServerUserItem * pIServerUserItem, bool bInitiative=false)=NULL;
	//�Թ۶���
	virtual bool PerformLookonAction(WORD wChairID, IServerUserItem * pIServerUserItem)=NULL;
	//���¶���
	virtual bool PerformSitDownAction(WORD wChairID, IServerUserItem * pIServerUserItem, LPCTSTR lpszPassword=NULL)=NULL;

	//���ܽӿ�
public:
	//���ͳ���
	virtual bool SendGameScene(IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)=NULL;

	//�����ӿ�
public:
	//���ýӿ�
	virtual bool SetTableFrameHook(IUnknownEx * pIUnknownEx)=NULL;
	//��ȡ�ӿ�
	virtual IUnknownEx * GetTableFrameHook()=NULL;

	//���Ƚӿ�
public:
	//�����Ϣ
	virtual void ExportInformation(LPCTSTR pszString, bool bError = false) = NULL;
	//�����ʽ����Ϣ
	virtual void ExportFormatInfo(bool bError, LPCTSTR pszFormat, ...) = NULL;

	//˽�˷�;
public:
	//����˽�˳���Ϣ;
	virtual void SetPrivateInfo(tagPrivateFrameParameter &PriavateFrame) = NULL;
	//��ȡ˽�˳�����
	virtual tagPrivateFrameParameter* GetPrivateFrameInfo() = NULL;
	//��ȡ��ǰ�������
	virtual tagPrivateFrameRecordInfo* GetPrivateFrameRecord() = NULL;
	//ͳ��˽�˳���Ҿ���
	virtual VOID addPrivatePlayCout(WORD wCout) = NULL;
	//����˽�˳�������Ϣ;
	virtual void ResetPrivateEndInfo() = NULL;
	//���˽�˳�������Ϣ��
	virtual void GetPrivateEndInfo(DataStream &kDataStream, bool bSend) = NULL;
	//�ж�˽�˳��Ƿ����
	virtual	bool IsPrivateEnd() = NULL;

	//�ƿ�ӿ�
public:
	//��ȡ�ƿ�
	virtual tagGameCardLibrary* GetGameCardLibrary(DWORD dwIndex = 0xFFFFFFFF) = NULL;

};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_ITableFrameSink INTERFACE_VERSION(1,1)
static const GUID IID_ITableFrameSink={0x9476b154,0x8beb,0x4f7e,0xaf,0x64,0xd2,0xb1,0x1a,0xda,0x5e,0xc4};
#else
#define VER_ITableFrameSink INTERFACE_VERSION(1,1)
static const GUID IID_ITableFrameSink={0x38a74df5,0x6245,0x46c7,0xb6,0xce,0x53,0xf9,0xd5,0xbf,0x6d,0xe6};
#endif

//�ص��ӿ�
interface ITableFrameSink : public IUnknownEx
{
	//����ӿ�
public:
	//��λ�ӿ�
	virtual VOID RepositionSink()=NULL;
	//���ýӿ�
	virtual bool Initialization(IUnknownEx * pIUnknownEx)=NULL;

	//��ѯ�ӿ�
public:
	//��ѯ�޶�
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem)=NULL;
	//���ٻ���
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem)=NULL;
	//��ѯ�Ƿ�۷����
	virtual bool QueryBuckleServiceCharge(WORD wChairID)=NULL;

	//��Ϸ�¼�
public:
	//��Ϸ��ʼ
	virtual bool OnEventGameStart()=NULL;
	//��Ϸ����
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)=NULL;
	//���ͳ���
	virtual bool OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)=NULL;

	//�¼��ӿ�
public:
	//ʱ���¼�
	virtual bool OnTimerMessage(DWORD dwTimerID, WPARAM dwBindParameter)=NULL;
	//�����¼�
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize)=NULL;
	//�����¼�
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)=NULL;

	//����ӿ�
public:
	//��Ϸ��Ϣ
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)=NULL;
	//�����Ϣ
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)=NULL;

	//�����ӿ�
public:
	//���û���
	virtual void SetGameBaseScore(LONG lBaseScore)=NULL;

	//˽�˳��ӿ�;
public:
	//����˽�˳���Ϣ;
	virtual void SetPrivateInfo(BYTE bGameTypeIdex, DWORD	bGameRuleIdex) = NULL;
	//����˽�˳�������Ϣ;
	virtual void ResetPrivateEndInfo() = NULL;
	//���˽�˳�������Ϣ��;
	virtual void GetPrivateEndInfo(DataStream &kDataStream, bool bSend) = NULL;
	//�ж�˽�˳��Ƿ����;
	virtual	bool IsPrivateEnd() = NULL;
};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_ITableUserAction INTERFACE_VERSION(1,1)
static const GUID IID_ITableUserAction={0x0f9aa3f9,0xdba4,0x49cb,0x88,0x4f,0xd9,0x11,0xaf,0x24,0xfb,0x8d};
#else
#define VER_ITableUserAction INTERFACE_VERSION(1,1)
static const GUID IID_ITableUserAction={0xc97c060b,0xcf0e,0x40b7,0x93,0x30,0x97,0xa4,0xf6,0x8c,0xca,0x84};
#endif

//�û�����
interface ITableUserAction : public IUnknownEx
{
	//�û�����
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)=NULL;
	//�û�����
	virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem)=NULL;
	//�û�����
	virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)=NULL;
	//�û�����
	virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)=NULL;
	//�û�ͬ��
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_ITableUserRequest INTERFACE_VERSION(1,1)
static const GUID IID_ITableUserRequest={0x7a810ebe,0x3835,0x41b5,0xba,0x7e,0x02,0x97,0x8c,0x13,0x73,0x50};
#else
#define VER_ITableUserRequest INTERFACE_VERSION(1,1)
static const GUID IID_ITableUserRequest={0x7ad17e89,0xcb5b,0x472a,0xac,0xeb,0x84,0x4d,0x4f,0xa1,0x4c,0x38};
#endif

//�û�����
interface ITableUserRequest : public IUnknownEx
{
	//�Թ�����
	virtual bool OnUserRequestLookon(WORD wChairID, IServerUserItem * pIServerUserItem, tagRequestResult & RequestResult)=NULL;
	//��������
	virtual bool OnUserRequestSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, tagRequestResult & RequestResult)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IMainServiceFrame INTERFACE_VERSION(1,1)
static const GUID IID_IMainServiceFrame={0xef3efa64,0x788b,0x4299,0x80,0x99,0xdd,0xed,0x08,0xde,0x57,0xc1};
#else
#define VER_IMainServiceFrame INTERFACE_VERSION(1,1)
static const GUID IID_IMainServiceFrame={0xbaaf5584,0xf9b4,0x41b6,0xae,0x6b,0xef,0x4d,0x54,0x41,0xf8,0x32};
#endif

//������
interface IMainServiceFrame : public IUnknownEx
{
	//��Ϣ�ӿ�
public:
	//������Ϣ
	virtual bool SendRoomMessage(LPCTSTR lpszMessage, WORD wType)=NULL;
	//��Ϸ��Ϣ
	virtual bool SendGameMessage(LPCTSTR lpszMessage, WORD wType)=NULL;
	//������Ϣ
	virtual bool SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)=NULL;
	//��Ϸ��Ϣ
	virtual bool SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)=NULL;
	//������Ϣ
	virtual bool SendRoomMessage(DWORD dwSocketID, LPCTSTR lpszMessage, WORD wType, bool bAndroid)=NULL;

	//����ӿ�
public:
	//��������
	virtual bool SendData(BYTE cbSendMask, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
	//��������
	virtual bool SendData(DWORD dwContextID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
	//��������
	virtual bool SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
	//Ⱥ������
	virtual bool SendDataToTable(WORD wTableID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize) = NULL;
	//���͵�ǰ���ӵ��û���ָ���ֻ��û�
	virtual bool SendUserInfo(IServerUserItem * pIServerUserItem, WORD wBindIndex) = NULL;

	//���ܽӿ�
public:
	//�������
	virtual bool InsertDistribute(IServerUserItem * pIServerUserItem)=NULL;
	//ɾ���û�
	virtual bool DeleteDistribute(IServerUserItem * pIServerUserItem)=NULL;
	//���дʹ���
	virtual void SensitiveWordFilter(LPCTSTR pMsg, LPTSTR pszFiltered, int nMaxLen)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IAndroidUserItem INTERFACE_VERSION(1,1)
static const GUID IID_IAndroidUserItem={0xf6856fe1,0xc93e,0x4166,0xbc,0x92,0xf7,0x43,0xad,0x97,0x1c,0xa8};
#else
#define VER_IAndroidUserItem INTERFACE_VERSION(1,1)
static const GUID IID_IAndroidUserItem={0xb1faa2f4,0x9804,0x4c6f,0x9d,0xfc,0xb1,0x0a,0x08,0x8a,0x22,0x69};
#endif

//�����˽ӿ�
interface IAndroidUserItem : public IUnknownEx
{
	//��Ϣ�ӿ�
public:
	//��ȡ I D
	virtual DWORD GetUserID()=NULL;
	//���Ӻ���
	virtual WORD GetTableID()=NULL;
	//���Ӻ���
	virtual WORD GetChairID()=NULL;

	//״̬����
public:
	//��ȡ״̬
	virtual BYTE GetGameStatus()=NULL;
	//����״̬
	virtual VOID SetGameStatus(BYTE cbGameStatus)=NULL;

	//������Ϣ
public:
	//��ȡ״̬
	virtual tagAndroidService * GetAndroidService()=NULL;
	//��ȡ����
	virtual tagAndroidParameter * GetAndroidParameter()=NULL;

	//���ܽӿ�
public:
	//��ȡ�Լ�
	virtual IServerUserItem * GetMeUserItem()=NULL;
	//��Ϸ�û�
	virtual IServerUserItem * GetTableUserItem(WORD wChariID)=NULL;

	//���нӿ�
public:
	//������Ϸ��
	virtual bool PerformSaveScore(SCORE lScore)=NULL;
	//��ȡ��Ϸ��
	virtual bool PerformTakeScore(SCORE lScore)=NULL;

	//����ӿ�
public:
	//���ͺ���
	virtual bool SendSocketData(WORD wSubCmdID)=NULL;
	//���ͺ���
	virtual bool SendSocketData(WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;

	//�����ӿ�
public:
	//��������
	virtual bool JudgeAndroidActionAndRemove(WORD wAction)=NULL;

	//���ܽӿ�
public:	
	//ɾ��ʱ��
	virtual bool KillGameTimer(UINT nTimerID)=NULL;
	//����ʱ��
	virtual bool SetGameTimer(UINT nTimerID, UINT nElapse)=NULL;
	//����׼��
	virtual bool SendUserReady(VOID * pData, WORD wDataSize)=NULL;
	//��������
	virtual bool SendChatMessage(DWORD dwTargetUserID, LPCTSTR pszChatString, COLORREF crFontColor)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IAndroidUserItemSink INTERFACE_VERSION(1,1)
static const GUID IID_IAndroidUserItemSink={0x0967632c,0x93da,0x4f7f,0x98,0xe4,0x6f,0x9f,0xf2,0xca,0x7b,0xc4};
#else
#define VER_IAndroidUserItemSink INTERFACE_VERSION(1,1)
static const GUID IID_IAndroidUserItemSink={0x1e8a1918,0x572b,0x453b,0xbc,0x0b,0x6b,0x61,0x70,0xa3,0x3c,0xca};
#endif

//�����˽ӿ�
interface IAndroidUserItemSink : public IUnknownEx
{
	//���ƽӿ�
public:
	//���ýӿ�
	virtual bool RepositionSink()=NULL;
	//��ʼ�ӿ�
	virtual bool Initialization(IUnknownEx * pIUnknownEx)=NULL;

	//��Ϸ�¼�
public:
	//ʱ����Ϣ
	virtual bool OnEventTimer(UINT nTimerID)=NULL;
	//��Ϸ��Ϣ
	virtual bool OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
	//��Ϸ��Ϣ
	virtual bool OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
	//������Ϣ
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize)=NULL;

	//�û��¼�
public:
	//�û�����
	virtual VOID OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)=NULL;
	//�û��뿪
	virtual VOID OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)=NULL;
	//�û�����
	virtual VOID OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)=NULL;
	//�û�״̬
	virtual VOID OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IUserTaskManagerSink INTERFACE_VERSION(1,1)
static const GUID IID_IUserTaskManagerSink={0xe71c3ecf,0x490e,0x4a32,0xb4,0x75,0x2c,0xba,0x9b,0x55,0x26,0xd4};
#else
#define VER_IUserTaskManagerSink INTERFACE_VERSION(1,1)
static const GUID IID_IUserTaskManagerSink={0x1c721508,0x81f8,0x403f,0xb5,0xc9,0x3e,0xb3,0xe3,0xe5,0xcd,0xa9};
#endif

//����ӿ�
interface IUserTaskManagerSink : public IUnknownEx
{	
	//�������
public:
	//�Ƴ�����
	virtual VOID RemoveTaskParameter()=NULL;
	//���Ҳ���
	virtual tagTaskParameter * SearchTaskParameter(WORD wTaskID)=NULL;
	//ö�ٲ���
	virtual tagTaskParameter * EnumTaskParameter(POSITION & Position)=NULL;
	//��Ӳ���
	virtual bool AddTaskParameter(tagTaskParameter TaskParameter[], WORD wPatemterCount)=NULL;	

	//�û�����
public:
	//�Ƴ�����
	virtual VOID RemoveUserTask(DWORD dwUserID)=NULL;
	//��ȡ����
	virtual tagUserTaskEntry * GetUserTaskEntry(DWORD dwUserID)=NULL;
	//��ȡ����
	virtual tagUserTaskEntry * GetUserTaskEntry(DWORD dwUserID,BYTE cbTaskStatus)=NULL;
	//��������
	virtual VOID SetUserTaskInfo(DWORD dwUserID,tagUserTaskInfo UserTaskInfo[],WORD wTaskCount)=NULL;	
};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IAndroidUserManager INTERFACE_VERSION(1,1)
static const GUID IID_IAndroidUserManager={0x0c963240,0xa798,0x4e33,0x81,0x28,0x97,0x3d,0x05,0xe7,0x7f,0x89};
#else
#define VER_IAndroidUserManager INTERFACE_VERSION(1,1)
static const GUID IID_IAndroidUserManager={0xba43054d,0x924b,0x4013,0xb2,0x6d,0xa6,0x91,0xb6,0x20,0x23,0xb2};
#endif

//�����˽ӿ�
interface IAndroidUserManager : public IUnknownEx
{
	//���ƽӿ�
public:
	//��������
	virtual bool StartService()=NULL;
	//ֹͣ����
	virtual bool ConcludeService()=NULL;

	//���ýӿ�
public:	
	//�������
	virtual bool InitAndroidUser(tagAndroidUserParameter & AndroidUserParameter)=NULL;
	//�Ƴ�����
	virtual bool RemoveAndroidParameter(DWORD dwBatchID)=NULL;
	//��Ӳ���
	virtual bool AddAndroidParameter(tagAndroidParameter AndroidParameter[], WORD wParemeterCount)=NULL;
	//�������
	virtual bool InsertAndroidInfo(tagAndroidAccountsInfo AndroidAccountsInfo[],WORD wAndroidCount,DWORD dwBatchID)=NULL;

	//����ӿ�
public:
	//ɾ������
	virtual bool DeleteAndroidUserItem(DWORD dwAndroidID,bool bStockRetrieve)=NULL;
	//���һ���
	virtual IAndroidUserItem * SearchAndroidUserItem(DWORD dwUserID, DWORD dwContextID)=NULL;
	//��������
	virtual IAndroidUserItem * CreateAndroidUserItem(tagAndroidItemConfig & AndroidItemConfig)=NULL;

	//�¼��ӿ�
public:
	//�����¼�
	virtual bool OnEventTimerPulse(DWORD dwTimerID, WPARAM dwBindParameter)=NULL;

	//״̬�ӿ�
public:
	//������Ŀ
	virtual WORD GetAndroidCount()=NULL;
	//���ػ���
	virtual bool GetAndroidLoadInfo(DWORD & dwBatchID,DWORD & dwLoadCount)=NULL;
	//�û�״��
	virtual WORD GetAndroidUserInfo(tagAndroidUserInfo & AndroidUserInfo, DWORD dwServiceMode)=NULL;	

	//����ӿ�
public:
	//��������
	virtual bool SendDataToClient(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
	//��������
	virtual bool SendDataToClient(DWORD dwAndroidID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
	//��������
	virtual bool SendDataToServer(DWORD dwAndroidID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IQueryServiceSink INTERFACE_VERSION(1,1)
static const GUID IID_IQueryServiceSink={0x0c963248,0xa798,0x4e33,0x88,0x28,0x98,0x3d,0x05,0xe7,0x7f,0x89};
#else
#define VER_IQueryServiceSink INTERFACE_VERSION(1,1)
static const GUID IID_IQueryServiceSink={0xba43054d,0x924b,0x4013,0xb2,0x6d,0xa6,0x96,0xb6,0x26,0x26,0xb2};
#endif

//��ѯ�ӿ�
interface IQueryServiceSink : public IUnknownEx
{
	//��ѯ�Ƿ�۷����
	virtual bool QueryBuckleServiceCharge(WORD wChairID)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IDBCorrespondManager INTERFACE_VERSION(1,1)
static const GUID IID_IDBCorrespondManager={0x0c963248,0xa796,0x4e33,0x86,0x28,0x98,0x3d,0x05,0xe7,0x7f,0x89};
#else
#define VER_IDBCorrespondManager INTERFACE_VERSION(1,1)
static const GUID IID_IDBCorrespondManager={0xba43154d,0x924b,0x4018,0xb2,0x6d,0xa6,0x96,0xb6,0x28,0x26,0xb2};
#endif

//��ѯ�ӿ�
interface IDBCorrespondManager : public IServiceModule
{
	//���ýӿ�
public:
	//����ģ��
	virtual bool __cdecl InitDBCorrespondManager(IDataBaseEngine * pIDataBaseEngine)=NULL;

	//�����¼�
public:
	//�����¼�
	virtual bool __cdecl PostDataBaseRequest(DWORD dwUserID, WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize, BYTE  cbCache = FALSE)=NULL;

	//ͬ���¼�
public:
	//�������
	virtual bool __cdecl OnPostRequestComplete(DWORD dwUserID, bool bSucceed)=NULL;

	//��ʱ�¼�
public:
	//��ʱ�¼�
	virtual bool __cdecl OnTimerNotify()=NULL;
};
//////////////////////////////////////////////////////////////////////////////////

//�����ļ�
#ifndef GAME_SERVICE_DLL
	#include "AndroidUserItem.h"
	#include "UserTaskManager.h"
	#include "ServerUserManager.h"
	#include "AndroidUserManager.h"
	#include "GamePropertyManager.h"
#endif

//��Ϸ����
DECLARE_MODULE_DYNAMIC(GameServiceManager)
DECLARE_MODULE_DYNAMIC(AndroidUserItemSink)

//////////////////////////////////////////////////////////////////////////////////

#endif