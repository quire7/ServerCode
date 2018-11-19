#ifndef CMD_GAME_SERVER_HEAD_FILE
#define CMD_GAME_SERVER_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//��¼����

#define MDM_GR_LOGON				1									//��¼��Ϣ

//��¼ģʽ
#define SUB_GR_LOGON_USERID			1									//I D ��¼
#define SUB_GR_LOGON_MOBILE			2									//�ֻ���¼
#define SUB_GR_LOGON_ACCOUNTS		3									//�ʻ���¼

//��¼���
#define SUB_GR_LOGON_SUCCESS		100									//��¼�ɹ�
#define SUB_GR_LOGON_FAILURE		101									//��¼ʧ��
#define SUB_GR_LOGON_FINISH			102									//��¼���

//������ʾ
#define SUB_GR_UPDATE_NOTIFY		200									//������ʾ

//////////////////////////////////////////////////////////////////////////////////

//I D ��¼
struct CMD_GR_LogonUserID
{
	//�汾��Ϣ
	DWORD							dwPlazaVersion;						//�㳡�汾

	//��¼��Ϣ
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_MD5];				//��¼����
	//TCHAR                           szServerPasswd[LEN_PASSWORD];       //��������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
	//WORD							wKindID;							//��������
};

//�ֻ���¼;
struct CMD_GR_LogonMobile
{
	//�汾��Ϣ
	WORD							wGameID;							//��Ϸ��ʶ
	DWORD							dwProcessVersion;					//���̰汾

	//��������
	BYTE                            cbDeviceType;                       //�豸����
	WORD                            wBehaviorFlags;                     //��Ϊ��ʶ
	WORD                            wPageTableCount;                    //��ҳ����

	//��¼��Ϣ
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_MD5];				//��¼����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
};

//��¼�ɹ�
struct CMD_GR_LogonSuccess
{
	DWORD							dwUserID;							//�û�ID
	DWORD							dwUserRight;						//�û�Ȩ��
	DWORD							dwMasterRight;						//����Ȩ��
};

//��¼ʧ��
struct CMD_GR_LogonFailure
{
	LONG							lErrorCode;							//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//��¼���
struct CMD_GR_LogonFinish
{
	bool							bGuideTask;							//��������
};

//������ʾ
struct CMD_GR_UpdateNotify
{
	//������־
	BYTE							cbMustUpdatePlaza;					//ǿ������
	BYTE							cbMustUpdateClient;					//ǿ������
	BYTE							cbAdviceUpdateClient;				//��������

	//��ǰ�汾
	DWORD							dwCurrentPlazaVersion;				//��ǰ�汾
	DWORD							dwCurrentFrameVersion;				//��ǰ�汾
	DWORD							dwCurrentClientVersion;				//��ǰ�汾
};

//////////////////////////////////////////////////////////////////////////////////
//��������

#define MDM_GR_CONFIG				2									//������Ϣ

#define SUB_GR_CONFIG_COLUMN		100									//�б�����
#define SUB_GR_CONFIG_SERVER		101									//��������
#define SUB_GR_CONFIG_PROPERTY		102									//��������
#define SUB_GR_CONFIG_FINISH		103									//�������
#define SUB_GR_CONFIG_USER_RIGHT	104									//���Ȩ��

//////////////////////////////////////////////////////////////////////////////////

//��������
struct CMD_GR_ConfigServer
{
	////��������
	//WORD							wTableCount;						//������Ŀ
	//WORD							wChairCount;						//������Ŀ

	////��������
	//WORD							wServerType;						//��������
	//DWORD							dwServerRule;						//�������
	
	WORD							wRoomID;							//����ID
	SCORE							lCellScore;							//��Ԫ����
	SCORE							lDefPutScore;						//Ĭ�ϴ������
	int								iExchangeRatio;						//�һ�����
	
	SCORE							lMagicExpScore;						//ħ���������
	SCORE							lGratuityScore;						//���ͻ���

	TCHAR							szRoomName[LEN_SERVER];				//��������
};

//��������
struct CMD_GR_ConfigProperty
{
	BYTE							cbPropertyCount;					//������Ŀ
	tagPropertyInfo					PropertyInfo[MAX_PROPERTY];			//��������
};

//���Ȩ��
struct CMD_GR_ConfigUserRight
{
	DWORD							dwUserRight;						//���Ȩ��
};
//////////////////////////////////////////////////////////////////////////////////
//�û�����

#define MDM_GR_USER					3									//�û���Ϣ

//�û�����
#define SUB_GR_USER_RULE			1									//�û�����
#define SUB_GR_USER_LOOKON			2									//�Թ�����
#define SUB_GR_USER_SITDOWN			3									//��������
#define SUB_GR_USER_STANDUP			4									//��������
#define SUB_GR_USER_INVITE			5									//�û�����
#define SUB_GR_USER_INVITE_REQ		6									//��������
#define SUB_GR_USER_REPULSE_SIT  	7									//�ܾ��������
#define SUB_GR_USER_KICK_USER       8                                   //�߳��û�
#define SUB_GR_USER_CHAIR_REQ       10                                  //�������λ��
#define SUB_GR_USER_WAIT_DISTRIBUTE 12									//�ȴ�����
#define SUB_GR_USER_AUTOSIT_REQ		13									//�Զ���λ����

//�û�״̬
#define SUB_GR_USER_ENTER			100									//�û�����
#define SUB_GR_USER_SCORE			101									//�û�����
#define SUB_GR_USER_STATUS			102									//�û�״̬
#define SUB_GR_REQUEST_FAILURE		103									//����ʧ��

//��������
#define SUB_GR_USER_CHAT			201									//������Ϣ
#define SUB_GR_USER_EXPRESSION		202									//������Ϣ
#define SUB_GR_WISPER_CHAT			203									//˽����Ϣ
#define SUB_GR_WISPER_EXPRESSION	204									//˽�ı���
#define SUB_GR_COLLOQUY_CHAT		205									//�Ự��Ϣ
#define SUB_GR_COLLOQUY_EXPRESSION	206									//�Ự����

//��������
//#define SUB_GR_PROPERTY_BUY			300									//�������
//#define SUB_GR_PROPERTY_SUCCESS		301									//���߳ɹ�
//#define SUB_GR_PROPERTY_FAILURE		302									//����ʧ��
//#define SUB_GR_PROPERTY_MESSAGE     303                                 //������Ϣ
//#define SUB_GR_PROPERTY_EFFECT      304                                 //����ЧӦ
//#define SUB_GR_PROPERTY_TRUMPET		305                                 //������Ϣ

//////////////////////////////////////////////////////////////////////////////////

//�Թ�����
struct CMD_GR_UserLookon
{
	WORD							wTableID;							//����λ��
	WORD							wChairID;							//����λ��
};

//��������
struct CMD_GR_UserSitDown
{
	WORD							wTableID;							//����λ��
	WORD							wChairID;							//����λ��
	TCHAR							szPassword[LEN_PASSWORD];			//��������
};

//��������
struct CMD_GR_UserStandUp
{
	WORD							wTableID;							//����λ��
	WORD							wChairID;							//����λ��
	BYTE							cbForceLeave;						//ǿ���뿪
};

//�����û� 
struct CMD_GR_UserInvite
{
	WORD							wTableID;							//���Ӻ���
	DWORD							dwUserID;							//�û� I D
};

//�����û����� 
struct CMD_GR_UserInviteReq
{
	WORD							wTableID;							//���Ӻ���
	DWORD							dwUserID;							//�û� I D
};

//�û�����
struct CMD_GR_UserScore
{
	DWORD							dwUserID;							//�û���ʶ
	tagUserScore					UserScore;							//������Ϣ
};

//�û�״̬
struct CMD_GR_UserStatus
{
	DWORD							dwUserID;							//�û���ʶ
	tagUserStatus					UserStatus;							//�û�״̬
};

//����ʧ��
struct CMD_GR_RequestFailure
{
	BYTE							cbErrType;							//��������
	LONG							lErrorCode;							//�������
	TCHAR							szDescribeString[256];				//������Ϣ
};

//�û�����
struct CMD_GR_C_UserChat
{
	WORD							wChatLength;						//��Ϣ����
	DWORD							dwChatColor;						//��Ϣ��ɫ
	DWORD							dwSendUserID;						//�����û�
	DWORD							dwTargetUserID;						//Ŀ���û�
	TCHAR							szChatString[LEN_USER_CHAT];		//������Ϣ
};

//�û�����
struct CMD_GR_S_UserChat
{
	WORD							wChatLength;						//��Ϣ����
	DWORD							dwChatColor;						//��Ϣ��ɫ
	DWORD							dwSendUserID;						//�����û�
	DWORD							dwTargetUserID;						//Ŀ���û�
	TCHAR							szChatString[LEN_USER_CHAT];		//������Ϣ
};

//�û�����
struct CMD_GR_C_UserExpression
{
	WORD							wItemIndex;							//��������
	DWORD							dwSendUserID;						//�����û�
	DWORD							dwTargetUserID;						//Ŀ���û�
};

//�û�����
struct CMD_GR_S_UserExpression
{
	WORD							wItemIndex;							//��������
	DWORD							dwSendUserID;						//�����û�
	DWORD							dwTargetUserID;						//Ŀ���û�
};

//�û�˽��
struct CMD_GR_C_WisperChat
{
	WORD							wChatLength;						//��Ϣ����
	DWORD							dwChatColor;						//��Ϣ��ɫ
	DWORD							dwSendUserID;						//�����û�
	DWORD							dwTargetUserID;						//Ŀ���û�
	TCHAR							szChatString[LEN_USER_CHAT];		//������Ϣ
};

//�û�˽��
struct CMD_GR_S_WisperChat
{
	WORD							wChatLength;						//��Ϣ����
	DWORD							dwChatColor;						//��Ϣ��ɫ
	DWORD							dwSendUserID;						//�����û�
	DWORD							dwTargetUserID;						//Ŀ���û�
	TCHAR							szChatString[LEN_USER_CHAT];		//������Ϣ
};

//˽�ı���
struct CMD_GR_C_WisperExpression
{
	WORD							wItemIndex;							//��������
	DWORD							dwSendUserID;						//�����û�
	DWORD							dwTargetUserID;						//Ŀ���û�
};

//˽�ı���
struct CMD_GR_S_WisperExpression
{
	WORD							wItemIndex;							//��������
	DWORD							dwSendUserID;						//�����û�
	DWORD							dwTargetUserID;						//Ŀ���û�
};

//�û��Ự
struct CMD_GR_ColloquyChat
{
	WORD							wChatLength;						//��Ϣ����
	DWORD							dwChatColor;						//��Ϣ��ɫ
	DWORD							dwSendUserID;						//�����û�
	DWORD							dwConversationID;					//�Ự��ʶ
	DWORD							dwTargetUserID[16];					//Ŀ���û�
	TCHAR							szChatString[LEN_USER_CHAT];		//������Ϣ
};

//�����û�
struct CMD_GR_C_InviteUser
{
	WORD							wTableID;							//���Ӻ���
	DWORD							dwSendUserID;						//�����û�
};

//�����û�
struct CMD_GR_S_InviteUser
{
	DWORD							dwTargetUserID;						//Ŀ���û�
};

//�������
struct CMD_GR_C_PropertyBuy
{
	BYTE                            cbRequestArea;						//����Χ
	BYTE							cbConsumeScore;						//��������
	WORD							wItemCount;							//������Ŀ
	WORD							wPropertyIndex;						//��������	
	DWORD							dwTargetUserID;						//ʹ�ö���
};

//���߳ɹ�
struct CMD_GR_S_PropertySuccess
{
	BYTE                            cbRequestArea;						//ʹ�û���
	WORD							wItemCount;							//������Ŀ
	WORD							wPropertyIndex;						//��������
	DWORD							dwSourceUserID;						//Ŀ�����
	DWORD							dwTargetUserID;						//ʹ�ö���
};

//����ʧ��
struct CMD_GR_PropertyFailure
{
	WORD                            wRequestArea;                       //��������
	LONG							lErrorCode;							//�������
	TCHAR							szDescribeString[256];				//������Ϣ
};

//������Ϣ
struct CMD_GR_S_PropertyMessage
{
	//������Ϣ
	WORD                            wPropertyIndex;                     //��������
	WORD                            wPropertyCount;                     //������Ŀ
	DWORD                           dwSourceUserID;                     //Ŀ�����
	DWORD                           dwTargerUserID;                     //ʹ�ö���
};


//����ЧӦ
struct CMD_GR_S_PropertyEffect
{
	DWORD                           wUserID;					        //�� ��I D
	BYTE							cbMemberOrder;						//��Ա�ȼ�
};

//��������
struct CMD_GR_C_SendTrumpet
{
	BYTE                           cbRequestArea;                        //����Χ 
	WORD                           wPropertyIndex;                      //�������� 
	DWORD                          TrumpetColor;                        //������ɫ
	TCHAR                          szTrumpetContent[TRUMPET_MAX_CHAR];  //��������
};

//��������
struct CMD_GR_S_SendTrumpet
{
	WORD                           wPropertyIndex;                      //�������� 
	DWORD                          dwSendUserID;                         //�û� I D
	DWORD                          TrumpetColor;                        //������ɫ
	TCHAR                          szSendNickName[32];				    //����ǳ� 
	TCHAR                          szTrumpetContent[TRUMPET_MAX_CHAR];  //��������
};


//�û��ܾ�����������
struct CMD_GR_UserRepulseSit
{
	WORD							wTableID;							//���Ӻ���
	WORD							wChairID;							//����λ��
	DWORD							dwUserID;							//�û� I D
	DWORD							dwRepulseUserID;					//�û� I D
};

//��������ʹ������
struct CMD_GR_C_Property
{
	WORD	wPropertyIndex;		//��������
	WORD	wItemCount;			//��������
	WORD	wTargetChairID;		//Ŀ���û�
};

//��������ʹ����Ӧ
struct CMD_GF_S_Property
{
	WORD	wPropertyIndex;		// ��������
	WORD	wItemCount;			// ����
	SCORE	lRecvLoveLiness;	// ����ֵ
	WORD	wSendChairID;		// �����û�
	WORD	wTargetChairID;		// Ŀ���û�
};

//����ʹ�ý��
enum EN_PROPERTY_ERROR
{
	EN_PROP_SUCC = 0,			//ʹ�óɹ�
	EN_PROP_NULL,				//������
	EN_PROP_NUMBER,				//��������
	EN_PROP_OVER				//����
};

//////////////////////////////////////////////////////////////////////////////////

//�����־
#define UR_LIMIT_SAME_IP			0x01								//���Ƶ�ַ
#define UR_LIMIT_WIN_RATE			0x02								//����ʤ��
#define UR_LIMIT_FLEE_RATE			0x04								//��������
#define UR_LIMIT_GAME_SCORE			0x08								//���ƻ���

//�û�����
struct CMD_GR_UserRule
{
	BYTE							cbRuleMask;							//��������
	WORD							wMinWinRate;						//���ʤ��
	WORD							wMaxFleeRate;						//�������
	LONG							lMaxGameScore;						//��߷��� 
	LONG							lMinGameScore;						//��ͷ���
};

//////////////////////////////////////////////////////////////////////////////////
//״̬����

#define MDM_GR_STATUS				4									//״̬��Ϣ

#define SUB_GR_TABLE_INFO			100									//������Ϣ
#define SUB_GR_TABLE_STATUS			101									//����״̬

//////////////////////////////////////////////////////////////////////////////////

//������Ϣ
struct CMD_GR_TableInfo
{
	WORD							wTableCount;						//������Ŀ
	tagTableStatus					TableStatusArray[512];				//����״̬
};

//����״̬
struct CMD_GR_TableStatus
{
	WORD							wTableID;							//���Ӻ���
	tagTableStatus					TableStatus;						//����״̬
};

//////////////////////////////////////////////////////////////////////////////////
//��������

#define MDM_GR_INSURE				5									//�û���Ϣ

//��������
#define SUB_GR_ENABLE_INSURE_REQUEST 1									//��ͨ����
#define SUB_GR_QUERY_INSURE_INFO	2									//��ѯ����
#define SUB_GR_SAVE_SCORE_REQUEST	3									//������
#define SUB_GR_TAKE_SCORE_REQUEST	4									//ȡ�����
#define SUB_GR_TRANSFER_SCORE_REQUEST	5								//ȡ�����
#define SUB_GR_QUERY_USER_INFO_REQUEST	6								//��ѯ�û�

#define SUB_GR_USER_INSURE_INFO		100									//��������
#define SUB_GR_USER_INSURE_SUCCESS	101									//���гɹ�
#define SUB_GR_USER_INSURE_FAILURE	102									//����ʧ��
#define SUB_GR_USER_TRANSFER_USER_INFO	103								//�û�����
#define SUB_GR_USER_INSURE_ENABLE_RESULT 104							//��ͨ���

//////////////////////////////////////////////////////////////////////////////////

//��ͨ����
struct CMD_GR_C_EnableInsureRequest
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	DWORD							dwUserID;							//�û�I D
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
	TCHAR							szInsurePass[LEN_PASSWORD];			//��������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��ѯ����
struct CMD_GR_C_QueryInsureInfoRequest
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	TCHAR							szInsurePass[LEN_PASSWORD];			//��������
};

//�������
struct CMD_GR_C_SaveScoreRequest
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	SCORE							lSaveScore;							//�����Ŀ
};

//ȡ������
struct CMD_GR_C_TakeScoreRequest
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	SCORE							lTakeScore;							//ȡ����Ŀ
	TCHAR							szInsurePass[LEN_PASSWORD];			//��������
};

//ת�˽��
struct CMD_GP_C_TransferScoreRequest
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	SCORE							lTransferScore;						//ת�˽��
	TCHAR							szAccounts[LEN_ACCOUNTS];			//Ŀ���û�
	TCHAR							szInsurePass[LEN_PASSWORD];			//��������
	TCHAR							szTransRemark[LEN_TRANS_REMARK];	//ת�˱�ע
};

//��ѯ�û�
struct CMD_GR_C_QueryUserInfoRequest
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	BYTE                            cbByNickName;                       //�ǳ�����
	TCHAR							szAccounts[LEN_ACCOUNTS];			//Ŀ���û�
};

//��������
struct CMD_GR_S_UserInsureInfo
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	BYTE							cbEnjoinTransfer;					//ת�˿���
	WORD							wRevenueTake;						//˰�ձ���
	WORD							wRevenueTransfer;					//˰�ձ���
	WORD							wRevenueTransferMember;				//˰�ձ���	
	WORD							wServerID;							//�����ʶ
	SCORE							lUserScore;							//�û����
	SCORE							lUserInsure;						//���н��
	SCORE							lTransferPrerequisite;				//ת������
};

//���гɹ�
struct CMD_GR_S_UserInsureSuccess
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	SCORE							lUserScore;							//���Ͻ��
	SCORE							lUserInsure;						//���н��
	TCHAR							szDescribeString[128];				//������Ϣ
};

//����ʧ��
struct CMD_GR_S_UserInsureFailure
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	LONG							lErrorCode;							//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//�û���Ϣ
struct CMD_GR_S_UserTransferUserInfo
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	DWORD							dwTargetGameID;						//Ŀ���û�
	TCHAR							szAccounts[LEN_ACCOUNTS];			//Ŀ���û�
};

//��ͨ���
struct CMD_GR_S_UserInsureEnableResult
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	BYTE							cbInsureEnabled;					//ʹ�ܱ�ʶ
	TCHAR							szDescribeString[128];				//������Ϣ
};

//////////////////////////////////////////////////////////////////////////////////
//��������
#define MDM_GR_TASK					6									//��������

#define SUB_GR_TASK_LOAD_INFO		1									//��������
#define SUB_GR_TASK_TAKE			2									//��ȡ����
#define SUB_GR_TASK_REWARD			3									//������

#define SUB_GR_TASK_INFO			4									//������Ϣ
#define SUB_GR_TASK_FINISH			5									//�������
#define SUB_GR_TASK_LIST			6									//�����б�
#define SUB_GR_TASK_RESULT			7									//������

//////////////////////////////////////////////////////////////////////////////////
//��������
struct CMD_GR_C_LoadTaskInfo
{
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
};

//��ȡ����
struct CMD_GR_C_TakeTask
{
	WORD							wTaskID;							//�����ʶ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��ȡ����
struct CMD_GR_C_TaskReward
{
	WORD							wTaskID;							//�����ʶ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//������Ϣ
struct CMD_GR_S_TaskInfo
{
	WORD							wTaskCount;							//��������
	tagTaskStatus					TaskStatus[TASK_MAX_COUNT];			//����״̬
};

//�������
struct CMD_GR_S_TaskFinish
{
	WORD							wFinishTaskID;						//�����ʶ
	//TCHAR							szTaskName[LEN_TASK_NAME];			//��������
};

//������
struct CMD_GR_S_TaskResult
{
	//�����Ϣ
	bool							bSuccessed;							//�ɹ���ʶ
	WORD							wCommandID;							//�����ʶ

	//�Ƹ���Ϣ
	SCORE							lCurrScore;							//��ǰ���
	SCORE							lCurrIngot;							//��ǰԪ��
	
	//��ʾ��Ϣ
	TCHAR							szNotifyContent[128];				//��ʾ����
};


//////////////////////////////////////////////////////////////////////////////////
//�һ�����

#define MDM_GR_EXCHANGE				7									//�һ�����

#define SUB_GR_EXCHANGE_LOAD_INFO   400									//������Ϣ
#define SUB_GR_EXCHANGE_PARAM_INFO	401									//�һ�����
#define SUB_GR_PURCHASE_MEMBER		402									//�����Ա
#define SUB_GR_PURCHASE_RESULT		403									//������
#define SUB_GR_EXCHANGE_SCORE		404									//�һ���Ϸ��
#define SUB_GR_EXCHANGE_RESULT		405									//�һ����

//////////////////////////////////////////////////////////////////////////////////

//��ѯ����
struct CMD_GR_ExchangeParameter
{
	WORD							wExchangeRate;						//�һ�����
	WORD							wMemberCount;						//��Ա��Ŀ
	tagMemberParameter				MemberParameter[10];				//��Ա����
};

//�����Ա
struct CMD_GR_PurchaseMember
{
	DWORD							dwUserID;							//�û���ʶ
	BYTE							cbMemberOrder;						//��Ա��ʶ
	WORD							wPurchaseTime;						//����ʱ��
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
};

//������
struct CMD_GR_PurchaseResult
{
	bool							bSuccessed;							//�ɹ���ʶ
	BYTE							cbMemberOrder;						//��Աϵ��
	SCORE							lCurrScore;							//��ǰ��Ϸ��
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//�һ���Ϸ��
struct CMD_GR_ExchangeScore
{
	DWORD							dwUserID;							//�û���ʶ
	SCORE							lExchangeIngot;						//�һ�Ԫ��
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
};

//�һ����
struct CMD_GR_ExchangeResult
{
	bool							bSuccessed;							//�ɹ���ʶ
	SCORE							lCurrScore;							//��ǰ��Ϸ��
	SCORE							lCurrIngot;							//��ǰԪ��
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//////////////////////////////////////////////////////////////////////////////////
//��������

#define MDM_GR_MANAGE				8									//��������

#define SUB_GR_SEND_WARNING			1									//���;���
#define SUB_GR_SEND_MESSAGE			2									//������Ϣ
#define SUB_GR_LOOK_USER_IP			3									//�鿴��ַ
#define SUB_GR_KILL_USER			4									//�߳��û�
#define SUB_GR_LIMIT_ACCOUNS		5									//�����ʻ�
#define SUB_GR_SET_USER_RIGHT		6									//Ȩ������

//��������
#define SUB_GR_QUERY_OPTION			7									//��ѯ����
#define SUB_GR_OPTION_SERVER		8									//��������
#define SUB_GR_OPTION_CURRENT		9									//��ǰ����
#define SUB_GR_LIMIT_USER_CHAT		10									//��������
#define SUB_GR_KICK_ALL_USER		11									//�߳��û�
#define SUB_GR_DISMISSGAME		    12									//��ɢ��Ϸ

//////////////////////////////////////////////////////////////////////////////////

//���;���
struct CMD_GR_SendWarning
{
	WORD							wChatLength;						//��Ϣ����
	DWORD							dwTargetUserID;						//Ŀ���û�
	TCHAR							szWarningMessage[LEN_USER_CHAT];	//������Ϣ
};

//ϵͳ��Ϣ
struct CMD_GR_SendMessage
{
	BYTE							cbGame;								//��Ϸ��Ϣ
	BYTE							cbRoom;								//��Ϸ��Ϣ
	BYTE							cbAllRoom;							//��Ϸ��Ϣ
	BYTE                            cbLoop;                             //ѭ����־
	DWORD                           dwTimeRate;                         //ѭ�����
	__time64_t                      tConcludeTime;                      //����ʱ��
	WORD							wChatLength;						//��Ϣ����
	TCHAR							szSystemMessage[LEN_USER_CHAT];		//ϵͳ��Ϣ
};

//�鿴��ַ
struct CMD_GR_LookUserIP
{
	DWORD							dwTargetUserID;						//Ŀ���û�
};

//�߳��û�
struct CMD_GR_KickUser
{
	DWORD							dwTargetUserID;						//Ŀ���û�
};

//�����ʻ�
struct CMD_GR_LimitAccounts
{
	DWORD							dwTargetUserID;						//Ŀ���û�
};

//Ȩ������
struct CMD_GR_SetUserRight
{
	//Ŀ���û�
	DWORD							dwTargetUserID;						//Ŀ���û�

	//�󶨱���
	BYTE							cbGameRight;						//�ʺ�Ȩ��
	BYTE							cbAccountsRight;					//�ʺ�Ȩ��

	//Ȩ�ޱ仯
	BYTE							cbLimitRoomChat;					//��������
	BYTE							cbLimitGameChat;					//��Ϸ����
	BYTE							cbLimitPlayGame;					//��ϷȨ��
	BYTE							cbLimitSendWisper;					//������Ϣ
	BYTE							cbLimitLookonGame;					//�Թ�Ȩ��
};

//��������
struct CMD_GR_OptionCurrent
{
	DWORD							dwRuleMask;							//��������
	tagServerOptionInfo				ServerOptionInfo;					//��������
};

//��������
struct CMD_GR_ServerOption
{
	tagServerOptionInfo				ServerOptionInfo;					//��������
};

//�߳������û�
struct CMD_GR_KickAllUser
{
	TCHAR							szKickMessage[LEN_USER_CHAT];		//�߳���ʾ
};

//��ɢ��Ϸ
struct CMD_GR_DismissGame
{
	WORD							wDismissTableNum;		            //��ɢ����
};
//////////////////////////////////////////////////////////////////////////////////

//���ñ�־
#define OSF_ROOM_CHAT				1									//��������
#define OSF_GAME_CHAT				2									//��Ϸ����
#define OSF_ROOM_WISPER				3									//����˽��
#define OSF_ENTER_TABLE				4									//������Ϸ
#define OSF_ENTER_SERVER			5									//���뷿��
#define OSF_SEND_BUGLE				12									//��������

//��������
struct CMD_GR_OptionServer
{
	BYTE							cbOptionFlags;						//���ñ�־
	BYTE							cbOptionValue;						//���ñ�־
};

//��������
struct CMD_GR_LimitUserChat
{
	DWORD							dwTargetUserID;						//Ŀ���û�
	BYTE							cbLimitFlags;						//���Ʊ�־
	BYTE							cbLimitValue;						//�������
};
//////////////////////////////////////////////////////////////////////////////////
//��������

#define MDM_GR_MATCH				9									//��������

#define SUB_GR_MATCH_FEE			400									//��������
#define SUB_GR_MATCH_NUM			401									//�ȴ�����
#define SUB_GR_LEAVE_MATCH			402									//�˳�����
#define SUB_GR_MATCH_INFO			403									//������Ϣ
#define SUB_GR_MATCH_WAIT_TIP		404									//�ȴ���ʾ
#define SUB_GR_MATCH_RESULT			405									//�������
#define SUB_GR_MATCH_STATUS			406									//����״̬
#define SUB_GR_MATCH_DESC			408									//��������
#define SUB_GR_MATCH_GOLDUPDATE		409									//��Ҹ���
#define SUB_GR_MATCH_ELIMINATE		410									//������̭

////////////////�Ķ�����ʱ �뽫��Ϸ����CMD_GAME.H��ͬʱ�Ķ�////////////////////////////
//#define SUB_GR_MATCH_INFO_ER_SPARROWS	410									//������Ϣ(2���齫)

//��������
struct CMD_GR_Match_Fee
{
	SCORE							lMatchFee;							//��������
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//��������
struct CMD_GR_Match_Num
{
	DWORD							dwWaitting;							//�ȴ�����
	DWORD							dwTotal;							//��������
};

//������Ϣ
struct CMD_GR_Match_Info
{
	TCHAR							szTitle[4][64];						//��Ϣ����
    WORD							wGameCount;							//��Ϸ����
};

//��ʾ��Ϣ
struct CMD_GR_Match_Wait_Tip
{
	SCORE							lScore;								//��ǰ����
	WORD							wRank;								//��ǰ����
	WORD							wCurTableRank;						//��������
	WORD							wUserCount;							//��ǰ����
	WORD							wCurGameCount;						//��ǰ����
	WORD							wGameCount;							//�ܹ�����
	WORD							wPlayingTable;						//��Ϸ����
	TCHAR							szMatchName[LEN_SERVER];			//��������
};

//�������
struct CMD_GR_MatchResult
{	
	SCORE							lGold;								//��ҽ���
	DWORD							dwIngot;							//Ԫ������
	DWORD							dwExperience;						//���齱��
	TCHAR							szDescribe[256];					//�ý�����
};

#define MAX_MATCH_DESC				4									//�������

//��������
struct CMD_GR_MatchDesc
{
	TCHAR							szTitle[MAX_MATCH_DESC][16];		//��Ϣ����
	TCHAR							szDescribe[MAX_MATCH_DESC][64];		//��������
	COLORREF						crTitleColor;						//������ɫ
	COLORREF						crDescribeColor;					//������ɫ
};

//��Ҹ���
struct CMD_GR_MatchGoldUpdate
{
	SCORE							lCurrGold;							//��ǰ���
	SCORE							lCurrIngot;							//��ǰԪ��
	DWORD							dwCurrExprience;					//��ǰ����
};

//////////////////////////////////////////////////////////////////////////////////
//˽�˳�����

#define MDM_GR_PRIVATE					10									//˽�˳�����

#define SUB_GR_PRIVATE_INFO				401									//˽�˳���Ϣ
#define SUB_GR_CREATE_PRIVATE			402									//����˽�˳�
#define SUB_GR_CREATE_PRIVATE_SUCESS	403									//����˽�˳��ɹ�
#define SUB_GR_JOIN_PRIVATE				404									//����˽�˳�
#define SUB_GF_PRIVATE_ROOM_INFO		405									//˽�˳�������Ϣ
#define SUB_GR_PRIVATE_DISMISS			406									//˽�˳������ɢ
#define SUB_GF_GM_PRIVATE_END			407									//˽�˳�����(������Ϸ��ͳ����Ϣ)
#define SUB_GR_RIVATE_AGAIN				408									//���¼���˽�˳�(������Ϣû��);
#define SUB_GR_PRIVATE_DISMISS_RESULT	409									//˽�˳���ɢ���
#define SUB_GR_PRIVATE_HAS_CREATED		410									//˽�˳��Ѵ���

//˽�˳���Ϣ
struct CMD_GR_Private_Info
{
	WORD							wKindID;
	SCORE							lCostGold;
	BYTE							bPlayCout[4];							//��Ҿ���
	SCORE							lPlayCost[4];							//���ĵ���
};

//��������
struct CMD_GR_Create_Private
{
	BYTE							cbGameType;								//��Ϸ����
	BYTE							bPlayCoutIdex;							//��Ϸ����
	BYTE							bGameTypeIdex;							//��Ϸ����
	DWORD							bGameRuleIdex;							//��Ϸ����

	BYTE							cbEnterRoomScoreType;					//������ʱ�ķ�������;
	SCORE							lInitEnterRoomScore;					//���뷿��ʱ�ķ���;
};


//���¿�ʼ
struct CMD_GR_Again_Private
{
};

//��������
struct CMD_GF_Create_Private_Sucess
{
	SCORE							lCurSocre;								//��ǰʣ��
	DWORD							dwRoomNum;								//����ID
};

//��������
struct CMD_GR_Join_Private
{
	DWORD							dwRoomNum;								//����ID
};

//��ɢ����;
struct CMD_GR_Dismiss_Private
{
	BYTE			bDismiss;			//��ɢ;
};

//˽�˳���ɢ��Ϣ;
struct CMD_GF_Private_Dismiss_Info
{
	CMD_GF_Private_Dismiss_Info()
	{
		ZeroMemory(this, sizeof(CMD_GF_Private_Dismiss_Info));
	}
	DWORD			dwDissUserCout;
	DWORD			dwDissChairID[MAX_CHAIR];
	DWORD			dwNotAgreeUserCout;
	DWORD			dwNotAgreeChairID[MAX_CHAIR];
};

//˽�˳���ɢ���;
struct CMD_GF_Private_Dismiss_Result
{
	bool			bSuccess;
};

#define MAX_PRIVATE_ACTION 8

//˽�˳�������Ϣ;
struct CMD_GF_Private_End_Info
{
	bool		bNextJiang;			//�Ƿ�ʼ��һ��;

	std::vector<SCORE>	lPlayerMaxScore;
	std::vector<SCORE> lPlayerWinLose;
	std::vector<BYTE> lPlayerAction;

	void StreamValue(DataStream& kData, bool bSend)
	{
		Stream_VALUE(bNextJiang);

		Stream_VALUE(lPlayerMaxScore);
		Stream_VALUE(lPlayerWinLose);
		Stream_VALUE(lPlayerAction);
	}
};


//////////////////////////////////////////////////////////////////////////////////
//�������

#define MDM_GF_FRAME				100									//�������

//////////////////////////////////////////////////////////////////////////////////
//�������

//�û�����
#define SUB_GF_GAME_OPTION			1									//��Ϸ����
#define SUB_GF_USER_READY			2									//�û�׼��
#define SUB_GF_LOOKON_CONFIG		3									//�Թ�����
#define SUB_GF_GAME_OPTION_TO_USER  4									//֪ͨ���������Ϸ����
#define SUB_GF_TAKE_GOLD			5									//������

//��������
#define SUB_GF_USER_CHAT			10									//�û�����
#define SUB_GF_USER_EXPRESSION		11									//�û�����
#define SUB_GF_MAGIC_EXPRESSION		12									//ħ������

//��Ϸ��Ϣ
#define SUB_GF_GAME_STATUS			100									//��Ϸ״̬
#define SUB_GF_GAME_SCENE			101									//��Ϸ����
#define SUB_GF_LOOKON_STATUS		102									//�Թ�״̬

//ϵͳ��Ϣ
#define SUB_GF_SYSTEM_MESSAGE		200									//ϵͳ��Ϣ
#define SUB_GF_ACTION_MESSAGE		201									//������Ϣ

//������Ϣ
#define SUB_GF_PROPERTY_SEND		300									//ʹ�õ���
#define SUB_GF_PROPERTY_SUCC		301									//ʹ�óɹ�

//////////////////////////////////////////////////////////////////////////////////

//��Ϸ����
struct CMD_GF_GameOption
{
	BYTE							cbAllowLookon;						//�Թ۱�־
	DWORD							dwFrameVersion;						//��ܰ汾
	DWORD							dwClientVersion;					//��Ϸ�汾
};

//�Թ�����
struct CMD_GF_LookonConfig
{
	DWORD							dwUserID;							//�û���ʶ
	BYTE							cbAllowLookon;						//�����Թ�
};

//�Թ�״̬
struct CMD_GF_LookonStatus
{
	BYTE							cbAllowLookon;						//�����Թ�
};

//��Ϸ����
struct CMD_GF_GameStatus
{
	BYTE							cbGameStatus;						//��Ϸ״̬
	BYTE							cbAllowLookon;						//�Թ۱�־
};

//�û�����
struct CMD_GF_C_UserChat
{
	WORD							wChatLength;						//��Ϣ����
	DWORD							dwChatColor;						//��Ϣ��ɫ
	DWORD							dwTargetUserID;						//Ŀ���û�
	TCHAR							szChatString[LEN_USER_CHAT];		//������Ϣ
};

//�û�����
struct CMD_GF_S_UserChat
{
	WORD							wChatLength;						//��Ϣ����
	DWORD							dwChatColor;						//��Ϣ��ɫ
	DWORD							dwSendUserID;						//�����û�
	DWORD							dwTargetUserID;						//Ŀ���û�
	TCHAR							szChatString[LEN_USER_CHAT];		//������Ϣ
};

//�û�����
struct CMD_GF_C_UserExpression
{
	WORD							wItemIndex;							//��������
	DWORD							dwTargetUserID;						//Ŀ���û�
	WORD							wPropertyID;						//�������ID
};

//�û�����
struct CMD_GF_S_UserExpression
{
	WORD							wItemIndex;							//��������
	DWORD							dwSendUserID;						//�����û�
	DWORD							dwTargetUserID;						//Ŀ���û�
};

//ħ������
struct CMD_GF_C_MagicExpression
{
	WORD							wItemIndex;							//��������
	WORD							wTargetChairID;						//Ŀ���û�
};

struct CMD_GF_S_MagicExpression 
{
	WORD							wItemIndex;							//��������
	WORD							wSendChairID;						//�����û�
	WORD							wTargetChairID;						//Ŀ���û�					
};

//////////////////////////////////////////////////////////////////////////////////
//��Ϸ����

#define MDM_GF_GAME					200									//��Ϸ����

//////////////////////////////////////////////////////////////////////////////////
//Я����Ϣ

//������Ϣ
#define DTP_GR_TABLE_PASSWORD		1									//��������

//�û�����
#define DTP_GR_NICK_NAME			10									//�û��ǳ�
#define DTP_GR_GROUP_NAME			11									//��������
#define DTP_GR_UNDER_WRITE			12									//����ǩ��

//������Ϣ
#define DTP_GR_USER_NOTE			20									//�û���ע
#define DTP_GR_CUSTOM_FACE			21									//�Զ�ͷ��

//////////////////////////////////////////////////////////////////////////////////

//�������
#define REQUEST_FAILURE_NORMAL		0									//����ԭ��(������Ĳ���)
#define REQUEST_FAILURE_NOGOLD		1									//��Ҳ���
#define REQUEST_FAILURE_NOSCORE		2									//���ֲ���
#define REQUEST_FAILURE_PASSWORD	3									//�������

//////////////////////////////////////////////////////////////////////////////////

//�һ����
struct CMD_GP_ExchangeResult
{
	bool							bSuccessed;							//�ɹ���ʶ
	SCORE							lCurrScore;							//��ǰ��Ϸ��
	SCORE							lCurrIngot;							//��ǰԪ��
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//�ֻ�����ʧ��
enum EN_MOBILE_CODE
{
	EN_MC_NULL = 0,			//Ĭ�Ͽ���Ϣ
	EN_MC_NO_LEAVE ,		//��������Ϸ�У���ʱ�����뿪�����Ƚ�����ǰ��Ϸ��
	EN_MC_FULL_PEOPLE,		//��ǰ��Ϸ�����Ѿ�����Ϊ���ˣ���ʱû�п������������λ�ã����Ժ����ԣ�
	EN_MC_NO_CHAIR,			//���ڴ���Ϸ����ʱû�п������������λ���ˣ���ѡ���������Ϸ����
	EN_MC_NO_TABLE,			//û�ҵ��ɽ������Ϸ����
	EN_MC_NO_LOOKON_TABLE,	//��Ǹ����ǰ��Ϸ���ӽ�ֹ�û��Թۣ�
	EN_MC_NO_LOOKON_ROOM,	//��Ǹ����ǰ��Ϸ�����ֹ�û��Թۣ�
	EN_MC_NO_PLAYER,		//���������λ��û����Ϸ��ң��޷��Թ۴���Ϸ����
	EN_MC_NO_LOOKON_PASSW,	//��Ϸ���������벻��ȷ�������Թ���Ϸ��
	EN_MC_NO_ENTER,			//��Ǹ����ǰ��Ϸ���ӽ�ֹ�û����룡
	EN_MC_NO_ENTER_START,	//��Ϸ�Ѿ���ʼ�ˣ����ڲ��ܽ�����Ϸ����
	EN_MC_IP_SAME,			//����Ϸ����������˲�����ͬ IP ��ַ�������Ϸ���� IP ��ַ�����ҵ� IP ��ַ��ͬ�����ܼ�����Ϸ��
	EN_MC_YOU_IP_SAME,		//�������˲�����ͬ IP ��ַ�������Ϸ������Ϸ���������� IP ��ַ��ͬ����ң����ܼ�����Ϸ��
	EN_MC_ACCOUNT_IN_ROOM,	//���ʺ����ڴ˷�����Ϸ���Ҳ������߳�������ѯ����Ա��
	EN_MC_LOGIN_PASSW_ERR,	//�ܱ�Ǹ�����ĵ�¼������󣬲�����������룡
	EN_MC_NO_LOOKON_START,	//��Ϸ��û�п�ʼ�������Թ۴���Ϸ����
};


struct CMD_GF_TakeGold
{
	SCORE lScore;		// ������
};


#pragma pack()

#endif