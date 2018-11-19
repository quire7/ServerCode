#ifndef DATA_BASE_PACKET_HEAD_FILE
#define DATA_BASE_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//�������ݰ�

//�û��¼�
#define	DBR_GR_LOGON_USERID			100									//I D��¼
#define	DBR_GR_LOGON_RELINE			101									//�û�����
#define	DBR_GR_LOGON_ACCOUNTS		102									//�ʺŵ�¼
#define DBR_GR_LOGON_MATCH			103									//������¼
#define	DBR_GR_LOGON_MOBILE			104									//�ֻ���¼

//ϵͳ�¼�
#define DBR_GR_WRITE_GAME_SCORE		200									//��Ϸ����
#define DBR_GR_LEAVE_GAME_SERVER	201									//�뿪����
#define DBR_GR_GAME_SCORE_RECORD	202									//���ּ�¼
#define DBR_GR_MANAGE_USER_RIGHT	203									//Ȩ�޹���
#define DBR_GR_LOAD_SYSTEM_MESSAGE	204									//ϵͳ��Ϣ
#define DBR_GR_LOAD_SENSITIVE_WORDS	205									//���д�	
#define DBR_GR_SEND_EMAIL			206									//�����ʼ�

#define DBR_GR_UNLOCK_ANDROID_USER	207									//����������
#define DBR_GR_LOAD_CARD_LIBRARY	208									//�����ƿ�
#define DBR_GR_PRIVATE_GAME_RECORD	209									//˽�˳���Ϸ��¼
#define DBR_GR_SINGLE_PRIVATE_RECORD	210								//������Ϸ��¼;
#define DBR_GR_PRIVATE_GAME_END		211									//˽�˳�����;

//�����¼�
#define DBR_GR_LOAD_PARAMETER		300									//���ز���
#define DBR_GR_LOAD_GAME_COLUMN		301									//�����б�
#define DBR_GR_LOAD_ANDROID_PARAMETER 302								//���ز���
#define DBR_GR_LOAD_ANDROID_USER	303									//���ػ���
#define DBR_GR_LOAD_GAME_PROPERTY	304									//���ص���
#define DBR_GR_LOAD_USER_PROPERTY	305									//�����û�����

//�����¼�
#define DBR_GR_USER_ENABLE_INSURE	400									//��ͨ����
#define DBR_GR_USER_SAVE_SCORE		401									//������Ϸ��
#define DBR_GR_USER_TAKE_SCORE		402									//��ȡ��Ϸ��
#define DBR_GR_USER_TRANSFER_SCORE	403									//ת����Ϸ��
#define DBR_GR_QUERY_INSURE_INFO	404									//��ѯ����
#define DBR_GR_QUERY_TRANSFER_USER_INFO	    405							//��ѯ�û�

//��Ϸ�¼�
#define DBR_GR_PROPERTY_REQUEST		500									//��������
#define DBR_GR_GAME_FRAME_REQUEST	502									//��Ϸ����

//�����¼�
#define DBR_GR_MATCH_SIGNUP			600									//��������
#define DBR_GR_MATCH_UNSIGNUP		601									//�˳�����
#define DBR_GR_MATCH_START			602									//������ʼ
#define DBR_GR_MATCH_OVER			603									//��������	
#define DBR_GR_MATCH_REWARD			604									//��������
#define DBR_GR_MATCH_ENTER_FOR		605									//��������
#define DBR_GR_MATCH_ELIMINATE      606									//������̭

//˽�˳��¼�
#define DBR_GR_PRIVATE_INFO			620									//˽�˳���Ϣ
#define DBR_GR_CREATE_PRIVAT		621									//����˽�˳�
#define DBR_GR_CREATE_PRIVAT_COST	622									//����˽�˳�

//�����¼�
#define DBR_GR_TASK_TAKE			700									//��ȡ����
#define DBR_GR_TASK_REWARD			701									//��ȡ����
#define DBR_GR_TASK_LOAD_LIST		702									//��������
#define DBR_GR_TASK_QUERY_INFO		703									//��ѯ����
#define DBR_GR_TASK_PROGRESS		704									//�������

//�һ��¼�
#define DBR_GR_LOAD_MEMBER_PARAMETER 800								//��Ա����
#define DBR_GR_PURCHASE_MEMBER		801									//�����Ա
#define DBR_GR_EXCHANGE_SCORE		802									//�һ���Ϸ��
#define DBR_GR_REFRESH_ONLINE_USER	900									//ˢ����������

//////////////////////////////////////////////////////////////////////////////////
//�����Ϣ

//�߼��¼�
#define DBO_GR_LOGON_SUCCESS		100									//��¼�ɹ�
#define DBO_GR_LOGON_FAILURE		101									//��¼ʧ��

//�����¼�
#define DBO_GR_GAME_PARAMETER		200									//������Ϣ
#define DBO_GR_GAME_COLUMN_INFO		201									//�б���Ϣ
#define DBO_GR_GAME_ANDROID_PARAMETER 202								//������Ϣ
#define DBO_GR_GAME_ANDROID_INFO	203									//������Ϣ
#define DBO_GR_GAME_UNLOCK_ANDROID	204									//��������
#define DBO_GR_GAME_PROPERTY_INFO	205									//������Ϣ
#define DBO_GR_GAME_MEMBER_PAREMETER 206								//��Ա����
#define DBO_GR_USER_PROPERTY_INFO	207									//�û�����
//��������
#define DBO_GR_USER_INSURE_INFO		300									//��������
#define DBO_GR_USER_INSURE_SUCCESS	301									//���гɹ�
#define DBO_GR_USER_INSURE_FAILURE	302									//����ʧ��
#define DBO_GR_USER_INSURE_USER_INFO   303								//�û�����
#define DBO_GR_USER_INSURE_ENABLE_RESULT 304							//��ͨ���

//��Ϸ�¼�
#define DBO_GR_PROPERTY_SUCCESS		400									//���߳ɹ�
#define DBO_GR_PROPERTY_FAILURE		401									//����ʧ��
#define DBO_GR_GAME_FRAME_RESULT	402									//��Ϸ���

//�����¼�
#define DBO_GR_MATCH_EVENT_START	500									//������ʶ
#define DBO_GR_MATCH_SIGNUP_RESULT	500									//�������
#define DBO_GR_MATCH_UNSIGNUP_RESULT 501								//�������
#define DBO_GR_MATCH_RANK_LIST		502									//��������
#define DBO_GR_MATCH_REWARD_RESULT  503									//�������

//˽�˳��¼�
#define DBO_GR_PRIVATE_EVENT_START	550									//˽�˳���ʼ��ʶ
#define DBO_GR_PRIVATE_INFO			551									//˽�˳���Ϣ	
#define DBO_GR_CREATE_PRIVATE		552									//˽�˳���Ϣ
#define DBO_GR_SINGLE_RECORD		553									//������Ϸ��¼;
#define DBO_GR_PRIVATE_EVENT_END	559									//˽�˳�������ʶ

#define DBO_GR_MATCH_EVENT_END		599									//������ʶ

//ϵͳ�¼�
#define DBO_GR_SYSTEM_MESSAGE_RESULT	600								//��Ϣ���
#define DBO_GR_SYSTEM_MESSAGE_FINISH	601								//��Ϣ���	
#define DBO_GR_SENSITIVE_WORDS		    602								//���д�
#define DBR_GR_SEND_EMAIL_SUCCESS		603								//�����ʼ�

//�����¼�
#define DBO_GR_TASK_LIST			700									//�����б�
#define DBO_GR_TASK_LIST_END		701									//�������
#define DBO_GR_TASK_INFO			702									//������Ϣ
#define DBO_GR_TASK_RESULT			703									//������

//�һ��¼�
#define DBO_GR_MEMBER_PARAMETER     800									//��Ա����
#define DBO_GR_PURCHASE_RESULT		801									//������
#define DBO_GR_EXCHANGE_RESULT		802									//�һ����

//�����¼�
#define DBO_GR_REREG_SERVER_IN_REDIS	900								//�����ڻ�����ע�������
//////////////////////////////////////////////////////////////////////////////////

//ID ��¼
struct DBR_GR_LogonUserID
{		
	//��¼��Ϣ
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_MD5];				//��¼����

	//������Ϣ
	DWORD							dwMatchID;							//������ʶ
	DWORD							dwMatchNO;							//�������
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
};

//mobile ��¼
struct DBR_GR_LogonMobile
{
	//��¼��Ϣ
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_MD5];				//��¼����
	BYTE                            cbDeviceType;                       //�豸����
	WORD                            wBehaviorFlags;                     //��Ϊ��ʶ
	WORD                            wPageTableCount;                    //��ҳ����

	//������Ϣ
	DWORD							dwMatchID;							//������ʶ
	DWORD							dwMatchNO;							//�������
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
};

//�û�����
struct DBR_GR_RELINE_USERID
{
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_MD5];				//��¼����
};

//��Ϸ����
struct DBR_GR_WriteGameScore
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ

	//�û���Ϣ
	DWORD							dwDBQuestID;						//�����ʶ
	DWORD							dwInoutIndex;						//��������

	//������Ϣ
	bool							bTaskForward;						//�������

	//�������
	BYTE							cbMaxCardType;
	BYTE							aryCardData[MAX_CARD_DATA_COUNT];

	//�ɼ�����
	tagVariationInfo				VariationInfo;						//��ȡ��Ϣ

	//������Ϣ
	DWORD							dwMatchID;							//������ʶ
	DWORD							dwMatchNO;							//��������
};

//�뿪����
struct DBR_GR_LeaveGameServer
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwInoutIndex;						//��¼����
	DWORD							dwLeaveReason;						//�뿪ԭ��
	DWORD							dwOnLineTimeCount;					//����ʱ��

	//�������
	BYTE							cbMaxCardType;
	BYTE							aryCardData[MAX_CARD_DATA_COUNT];

	//�ɼ�����
	tagVariationInfo				RecordInfo;							//��¼��Ϣ
	tagVariationInfo				VariationInfo;						//��ȡ��Ϣ

	//ϵͳ��Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
};

//��Ϸ��¼
struct DBR_GR_GameScoreRecord
{
	//������Ϣ
	WORD							wTableID;							//���Ӻ���
	WORD							wUserCount;							//�û���Ŀ
	WORD							wAndroidCount;						//������Ŀ

	//���˰��
	SCORE							lWasteCount;						//�����Ŀ
	SCORE							lRevenueCount;						//˰����Ŀ

	//ͳ����Ϣ
	DWORD							dwUserMemal;						//������Ŀ
	DWORD							dwPlayTimeCount;					//��Ϸʱ��

	//ʱ����Ϣ
	SYSTEMTIME						SystemTimeStart;					//��ʼʱ��
	SYSTEMTIME						SystemTimeConclude;					//����ʱ��

	//���ּ�¼
	WORD							wRecordCount;						//��¼��Ŀ
	tagGameScoreRecord				GameScoreRecord[MAX_CHAIR];			//��Ϸ��¼
};

//��ͨ����
struct DBR_GR_UserEnableInsure
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	DWORD							dwUserID;							//�û� I D	
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
	TCHAR							szInsurePass[LEN_PASSWORD];			//��������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//������Ϸ��
struct DBR_GR_UserSaveScore
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	DWORD							dwUserID;							//�û� I D
	SCORE							lSaveScore;							//������Ϸ��
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//ȡ����Ϸ��
struct DBR_GR_UserTakeScore
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	DWORD							dwUserID;							//�û� I D
	SCORE							lTakeScore;							//��ȡ��Ϸ��
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szPassword[LEN_PASSWORD];			//��������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//ת����Ϸ��
struct DBR_GR_UserTransferScore
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	DWORD							dwUserID;							//�û� I D
	DWORD							dwClientAddr;						//���ӵ�ַ	
	SCORE							lTransferScore;						//ת����Ϸ��
	TCHAR							szAccounts[LEN_NICKNAME];			//Ŀ���û�
	TCHAR							szPassword[LEN_PASSWORD];			//��������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
	TCHAR							szTransRemark[LEN_TRANS_REMARK];	//ת�ʱ�ע
};

//���ػ���
struct DBR_GR_LoadAndroidUser
{
	DWORD							dwBatchID;							//���α�ʶ
	DWORD							dwAndroidCount;						//������Ŀ
};

//��ѯ����
struct DBR_GR_QueryInsureInfo
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	DWORD							dwUserID;							//�û� I D
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szPassword[LEN_PASSWORD];			//��������
};

//��ѯ�û�
struct DBR_GR_QueryTransferUserInfo
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	BYTE                            cbByNickName;                       //�ǳ�����
	TCHAR							szAccounts[LEN_ACCOUNTS];			//Ŀ���û�
	DWORD							dwUserID;							//�û� I D
};

//��������
struct DBR_GR_PropertyRequest
{
	DWORD							dwDBID;								//�������ݿ�ID

	//������Ϣ
	WORD							wItemCount;							//������Ŀ
	WORD							wPropertyIndex;						//��������

	DWORD							dwSourceUserID;						//ʹ�ö���
	DWORD							dwTargetUserID;						//��ʹ�ö���

	SCORE							lRecvLoveLiness;					//��������

	////����ģʽ
	//BYTE                            cbRequestArea;			            //ʹ�÷�Χ 
	//BYTE							cbConsumeScore;						//��������
	//SCORE							lFrozenedScore;						//�������

	////�û�Ȩ��
	//DWORD                           dwUserRight;						//��ԱȨ��

	//ϵͳ��Ϣ
	WORD							wTableID;							//���Ӻ���
	DWORD							dwInoutIndex;						//�����ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ
	SYSTEMTIME						UsedTime;							//ʹ��ʱ��
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};


//��ѯ����
struct DBR_GR_TaskQueryInfo
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ	
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����
};

//��ȡ����
struct DBR_GR_TaskTake
{
	//�û���Ϣ
	WORD							wTaskID;							//�����ʶ
	DWORD							dwUserID;							//�û���ʶ	
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��ȡ����
struct DBR_GR_TaskReward
{
	//�û���Ϣ
	WORD							wTaskID;							//�����ʶ
	DWORD							dwUserID;							//�û���ʶ	
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//�������
struct DBR_GR_TaskProgress
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ

	//������Ϣ
	DWORD							dwID;								//��������
	WORD							wTaskProgress;						//�������
	BYTE							cbTaskStatus;						//����״̬
};

//�һ���Ϸ��
struct DBR_GR_ExchangeScore
{
	DWORD							dwUserID;							//�û���ʶ
	SCORE							lExchangeIngot;						//�һ�Ԫ��

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//////////////////////////////////////////////////////////////////////////////////

//��¼�ɹ�
struct DBO_GR_LogonSuccess
{
	//��������
	WORD							wFaceID;							//ͷ���ʶ
	DWORD							dwUserID;							//�û� ID
	DWORD							dwCustomID;							//�Զ�ͷ��
	DWORD							dwCustomFaceVer;					//�Զ�ͷ��汾
	TCHAR							szNickName[LEN_NICKNAME];			//�ʺ��ǳ�

	//�û�����
	BYTE							cbGender;							//�û��Ա�
	BYTE							cbMemberOrder;						//��Ա�ȼ�
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//����ǩ��

	//������Ϣ
	SCORE							lGold;								//�û����
	SCORE							lDiamond;							//�û���ʯ
	SCORE							lInsure;							//�û�����

	//��Ϸ��Ϣ
	DWORD							dwWinCount;							//ʤ������
	DWORD							dwLostCount;						//ʧ������
	DWORD							dwDrawCount;						//�;�����
	DWORD							dwFleeCount;						//��������
	DWORD							dwExperience;						//�û�����
	LONG							lLoveLiness;						//�û�����

	//�������
	BYTE							cbMaxCardType;
	BYTE							aryCardData[MAX_CARD_DATA_COUNT];

	//�û�Ȩ��
	DWORD							dwUserRight;						//�û�Ȩ��

	//��������
	DWORD							dwInoutIndex;						//��¼����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ

	//�������
	//WORD							wTaskID;							//�����ʶ
	//BYTE							cbTaskStatus;						//����״̬
	//DWORD							dwTaskProgress;						//�������	
	//SYSTEMTIME						TaskTakeTime;						//��ȡʱ��

	//�ȼý�
	BYTE							cbBenefitTimes;						//�������
	SYSTEMTIME						LastBenefitTime;					//�ϴ���ȡʱ��

	//�ֻ�����
	BYTE                            cbDeviceType;                       //�豸����

	//������Ϣ
	TCHAR							szPassword[LEN_MD5];				//��¼����
	TCHAR							szDescribeString[128];				//������Ϣ

	//������Ϣ
	WORD							wTaskCount;							//������Ŀ
	tagUserTaskInfo					UserTaskInfo[TASK_MAX_COUNT];		//������Ϣ
};

//��¼ʧ��
struct DBO_GR_LogonFailure
{
	LONG							lResultCode;						//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//������Ϣ
struct DBO_GR_GameAndroidParameter
{
	LONG							lResultCode;						//�������
	WORD							wParameterCount;					//������Ŀ
	tagAndroidParameter				AndroidParameter[MAX_BATCH];		//������Ϣ
};

//������Ϣ
struct DBO_GR_GameAndroidInfo
{
	LONG							lResultCode;						//�������
	DWORD							dwBatchID;							//���α�ʶ
	WORD							wAndroidCount;						//�û���Ŀ
	tagAndroidAccountsInfo			AndroidAccountsInfo[MAX_ANDROID];	//�����ʺ�
};

//������Ϣ
struct DBO_GR_GamePropertyInfo
{
	LONG							lResultCode;						//�������
	BYTE							cbPropertyCount;					//������Ŀ
	tagPropertyInfo					PropertyInfo[MAX_PROPERTY];			//������Ϣ
};

//��������
struct DBO_GR_UserInsureInfo
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	BYTE							cbEnjoinTransfer;					//ת�ʿ���
	WORD							wRevenueTake;						//˰�ձ���
	WORD							wRevenueTransfer;					//˰�ձ���
	WORD							wRevenueTransferMember;				//˰�ձ���
	WORD							wServerID;							//�����ʶ
	SCORE							lUserScore;							//�û���Ϸ��
	SCORE							lUserInsure;						//������Ϸ��
	SCORE							lTransferPrerequisite;				//ת������
};

//���гɹ�
struct DBO_GR_UserInsureSuccess
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	DWORD							dwUserID;							//�û� I D
	SCORE							lSourceScore;						//ԭ����Ϸ��
	SCORE							lSourceInsure;						//ԭ����Ϸ��
	SCORE							lInsureRevenue;						//����˰��
	SCORE							lFrozenedScore;						//�������
	SCORE							lVariationScore;					//��Ϸ�ұ仯
	SCORE							lVariationInsure;					//���б仯
	TCHAR							szDescribeString[128];				//������Ϣ
};

//����ʧ��
struct DBO_GR_UserInsureFailure
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	LONG							lResultCode;						//��������
	SCORE							lFrozenedScore;						//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//�û�����
struct DBO_GR_UserTransferUserInfo
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	DWORD							dwGameID;							//�û� I D
	TCHAR							szAccounts[LEN_ACCOUNTS];			//�ʺ��ǳ�
};

//��ͨ���
struct DBO_GR_UserInsureEnableResult
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	BYTE							cbInsureEnabled;					//ʹ�ܱ�ʶ
	TCHAR							szDescribeString[128];				//������Ϣ
};

//���߳ɹ�
struct DBO_GR_S_PropertySuccess
{
	//������Ϣ
	WORD							wItemCount;							//������Ŀ
	WORD							wPropertyIndex;						//��������
	DWORD							dwSourceUserID;						//�������
	DWORD							dwTargetUserID;						//ʹ�ö���

	//����ģʽ
	BYTE                            cbRequestArea;						//����Χ
	BYTE							cbConsumeScore;						//��������
	SCORE							lFrozenedScore;						//�������

	//��ԱȨ��
	DWORD                           dwUserRight;						//��ԱȨ��

	//�����Ϣ
	SCORE							lConsumeGold;						//������Ϸ��
	LONG							lSendLoveLiness;					//��������
	LONG							lRecvLoveLiness;					//��������

	//��Ա��Ϣ
	BYTE							cbMemberOrder;						//��Ա�ȼ�
};

//�����б�
struct DBO_GR_TaskListInfo
{
	WORD							wTaskCount;							//������Ŀ
};

//������Ϣ
struct DBO_GR_TaskInfo
{
	WORD							wTaskCount;							//��������
	tagTaskStatus					TaskStatus[TASK_MAX_COUNT];			//����״̬
};

//ǩ�����
struct DBO_GR_TaskResult
{
	//�����Ϣ
	bool							bSuccessed;							//�ɹ���ʶ
	WORD							wCommandID;							//�����ʶ
	WORD							wCurrTaskID;						//��ǰ����

	//�Ƹ���Ϣ
	SCORE							lCurrScore;							//��ǰ���
	SCORE							lCurrIngot;							//��ǰԪ��
	
	//��ʾ��Ϣ
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//��Ա����
struct DBO_GR_MemberParameter
{
	WORD							wMemberCount;						//��Ա��Ŀ
	tagMemberParameter				MemberParameter[64];				//��Ա����
};

//������
struct DBO_GR_PurchaseResult
{
	bool							bSuccessed;							//�ɹ���ʶ
	BYTE							cbMemberOrder;						//��Աϵ��
	DWORD							dwUserRight;						//�û�Ȩ��
	SCORE							lCurrScore;							//��ǰ��Ϸ��
	TCHAR							szNotifyContent[128];				//��ʾ����
};
 
//�һ����
struct DBO_GR_ExchangeResult
{
	bool							bSuccessed;							//�ɹ���ʶ
	SCORE							lCurrScore;							//��ǰ��Ϸ��
	SCORE							lCurrIngot;							//��ǰԪ��
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//����ʧ��
struct DBO_GR_PropertyFailure
{
	BYTE                            cbRequestArea;						//����Χ
	LONG							lResultCode;						//��������
	SCORE							lFrozenedScore;						//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

////��������
//struct DBO_GR_MatchRank
//{
//	BYTE							cbRank;								//��������
//	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
//	LONG							lMatchScore;						//�û��÷�
//};

//�������
struct DBO_GR_MatchSingupResult
{
	bool							bResultCode;						//�������
	DWORD							dwUserID;							//�û���ʶ
	SCORE							lCurrGold;							//��ǰ���
	SCORE							lCurrIngot;							//��ǰԪ��
	TCHAR							szDescribeString[128];				//������Ϣ
};

//��������
struct DBO_GR_MatchRankList
{
	WORD							wUserCount;							//�û���Ŀ
	DWORD							dwMatchID;							//������ʶ
	DWORD							dwMatchNO;							//��������
	tagMatchRankInfo				MatchRankInfo[128];					//������Ϣ
};

//Ȩ�޹���
struct DBR_GR_ManageUserRight
{
	DWORD							dwUserID;							//Ŀ���û�
	DWORD							dwAddRight;							//���Ȩ��
	DWORD							dwRemoveRight;						//ɾ��Ȩ��
	bool							bGameRight;							//��ϷȨ��
};

//Ȩ�޹���
struct DBR_GR_ManageMatchRight
{	
	DWORD							dwUserID;							//Ŀ���û�
	DWORD							dwAddRight;							//���Ȩ��
	DWORD							dwRemoveRight;						//ɾ��Ȩ��	
	DWORD							dwMatchID;							//������ʶ
	DWORD							dwMatchNO;							//��������
};

//��������
struct DBR_GR_MatchSignup
{
	//��������
	DWORD							dwUserID;							//�û�ID
	SCORE							lMatchFee;							//��������

	//ϵͳ��Ϣ
	DWORD							dwInoutIndex;						//�����ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ
	DWORD							dwMatchID;							//����ID
	DWORD							dwMatchNO;							//��������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//������ʼ
struct DBR_GR_MatchStart
{
	WORD							wServerID;							//�����ʶ
	BYTE							cbMatchType;						//��������
	DWORD							dwMatchID;							//������ʶ
	DWORD							dwMatchNO;							//��������	
};

//��������
struct DBR_GR_MatchOver
{
	WORD							wServerID;							//�����ʶ
	BYTE							cbMatchType;						//��������
	DWORD							dwMatchID;							//������ʶ
	DWORD							dwMatchNO;							//��������	
	SYSTEMTIME						MatchStartTime;						//��ʼʱ��
	SYSTEMTIME						MatchEndTime;						//����ʱ��
};

//������̭
struct DBR_GR_MatchEliminate
{
	DWORD							dwUserID;							//�û���ʶ
	WORD							wServerID;							//�����ʶ
	BYTE							cbMatchType;						//��������
	DWORD							dwMatchID;							//������ʶ
	DWORD							dwMatchNO;							//��������	
};


//��������
struct DBR_GR_MatchReward
{
	DWORD							dwUserID;							//�û� I D
	SCORE							lRewardGold;						//�������
	DWORD							dwRewardIngot;						//����Ԫ��
    DWORD							dwRewardExperience;					//�û�����	
	DWORD							dwClientAddr;						//���ӵ�ַ
};

//�������
struct DBR_GR_MatchRewardResult
{
	bool							bResultCode;						//�������
	DWORD							dwUserID;							//�û���ʶ
	SCORE							lCurrGold;							//��ǰ���
	SCORE							lCurrIngot;							//��ǰԪ��
	DWORD							dwCurrExperience;					//��ǰ����
};

//ϵͳ��Ϣ
struct DBR_GR_SystemMessage
{
	DWORD                           dwMessageID;                        //��ϢID
	BYTE							cbMessageType;						//��Ϣ����
	BYTE                            cbAllRoom;                          //ȫ�巿��
	DWORD							dwTimeRate;						    //ʱ��Ƶ��
	__time64_t						tStartTime;							//��ʼʱ��
	__time64_t                      tConcludeTime;                      //����ʱ��
	TCHAR							szSystemMessage[LEN_USER_CHAT];		//ϵͳ��Ϣ
};

//�ʼ��ṹ
struct DBR_GR_EMAIL
{
	DWORD							dwRecUserID;						//�����û�
	BYTE							cbType;								//�ʼ�����
	SYSTEMTIME						createTime;							//����ʱ��

	TCHAR							szRewardProp[256];					//��������ID
	TCHAR							szEmailTitle[100];					//�ʼ�����
	TCHAR							szEmailMsg[320];					//�ʼ�����
};

//ˢ�����������
struct DBR_GR_RefreshOnlineUser
{
	char							szKey[32];
	char							szUserCount[8];
};

//���ص���
struct DBR_GR_LOADPROPERTY
{
	DWORD							dwUserID;
};

//�����б�
struct DBO_GR_PropertyListInfo
{
	WORD							wPropertyCount;						//��Ŀ
};

//////////////////////////////////////////////////////////////////////////////////

//˽�˳���Ϣ
struct DBR_GR_Private_Info
{
	WORD							wKindID;
};


//����˽�˳�
struct DBR_GR_Create_Private
{
	WORD							wKindID;
	DWORD							dwUserID;
	DWORD							dwCost;
	DWORD							dwCostType;
	BYTE							cbRoomType;
	DWORD							dwAgaginTable;
	BYTE							bPlayCoutIdex;							//��Ϸ����
	BYTE							bGameTypeIdex;							//��Ϸ����
	DWORD							bGameRuleIdex;							//��Ϸ����

	BYTE							cbEnterRoomScoreType;					//������ʱ�ķ�������;
	SCORE							lInitEnterRoomScore;					//���뷿��ʱ�ķ���;
};

//����˽�˳�
struct DBR_GR_Create_Private_Cost
{
	DWORD							dwUserID;
	DWORD							dwCost;
	DWORD							dwCostType;
};


//˽�˳���Ϣ
struct DBO_GR_Private_Info
{
	BYTE							cbDBExecSuccess;						//����ִ���Ƿ�ɹ�;
	WORD							wKindID;
	SCORE							lCostGold;
	BYTE							bPlayCout[4];							//��Ҿ���
	SCORE							lPlayCost[4];							//���ĵ���
};

//��������
struct DBO_GR_CreatePrivateResult
{
	BOOL							bSucess;
	BYTE							cbRoomType;
	TCHAR							szDescribeString[128];					//������Ϣ
	SCORE							lCurSocre;								//��ǰʣ��
	WORD							wAgaginTable;							//���¼�������
	BYTE							bPlayCoutIdex;							//��Ҿ���
	BYTE							bGameTypeIdex;							//��Ϸ����
	DWORD							bGameRuleIdex;							//��Ϸ����

	BYTE							cbEnterRoomScoreType;					//������ʱ�ķ�������;
	SCORE							lInitEnterRoomScore;					//���뷿��ʱ�ķ���;

	DWORD							dwRecordID;								//¼���¼�ܱ������ID;
};

//����������
struct DBR_GR_UnlockAndroidUser
{
	WORD							wServerID;							//����ID
	WORD							wBatchID;							//����ID
};

//��һ��Ϸ��¼����recordID;
struct DBO_GR_SingleGameRecord
{
	DWORD							dwCreaterUserID;					//������;
	DWORD							dwRecordID;							//��Ϸ��¼����ID;
};

//˽�˳�����
struct DBO_GR_PrivateGameEnd
{
	DWORD							dwUserID;
	SCORE							lScore;
	WORD							wPlayCount;
	DWORD							dwCardLibIndex;
	WORD							wKindID;
	WORD							wServerID;
};


//�ƿ�;
struct DBO_GR_GameCardLibrary
{
	WORD							wKindID;
	DWORD							dwLibID;
	DWORD							dwCustomID;
	BYTE							cbCardCount;
	TCHAR							szCardData[128];
};

//�ƿ�����;
struct DBO_GR_GameCardLibraryCfg
{
	BYTE							cbEnable;			//�ƿ��Ƿ���Ч;
	DWORD							dwCount;			//�ƿ�����;
	DWORD							dwStartDateTime;	//��ʼʱ��;
	DWORD							dwEndDateTime;		//����ʱ��;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif