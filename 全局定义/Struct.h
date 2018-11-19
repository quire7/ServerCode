#ifndef STRUCT_HEAD_FILE
#define STRUCT_HEAD_FILE


#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//��Ϸ�б�

//��Ϸ����
struct tagGameType
{
	WORD							wJoinID;							//�ҽ�����
	WORD							wSortID;							//��������
	WORD							wTypeID;							//��������
	TCHAR							szTypeName[LEN_TYPE];				//��������
};

//��Ϸ����
struct tagGameKind
{
	WORD							wTypeID;							//��������
	WORD							wJoinID;							//�ҽ�����
	WORD							wSortID;							//��������
	WORD							wKindID;							//��������
	WORD							wGameID;							//ģ������
	DWORD							dwOnLineCount;						//��������
	DWORD							dwAndroidCount;						//��������
	DWORD							dwFullCount;						//��Ա����
	TCHAR							szKindName[LEN_KIND];				//��Ϸ����
	TCHAR							szProcessName[LEN_PROCESS];			//��������
};

//��Ϸ�ڵ�
struct tagGameNode
{
	WORD							wKindID;							//��������
	WORD							wJoinID;							//�ҽ�����
	WORD							wSortID;							//��������
	WORD							wNodeID;							//�ڵ�����
	TCHAR							szNodeName[LEN_NODE];				//�ڵ�����
};

//��������
struct tagGamePage
{
	WORD							wPageID;							//ҳ������
	WORD							wKindID;							//��������
	WORD							wNodeID;							//�ڵ�����
	WORD							wSortID;							//��������
	WORD							wOperateType;						//��������
	TCHAR							szDisplayName[LEN_PAGE];			//��ʾ����
};

//��Ϸ����
struct tagGameServer
{
	WORD							wKindID;							//��������
	WORD							wNodeID;							//�ڵ�����
	WORD							wSortID;							//��������
	WORD							wServerID;							//��������
	WORD                            wServerKind;                        //��������
	WORD							wServerType;						//��������
	WORD							wServerPort;						//����˿�
	SCORE							lCellScore;							//��Ԫ����
	SCORE							lEnterScore;						//�������
	DWORD							dwServerRule;						//�������
	DWORD							dwOnLineCount;						//��������
	DWORD							dwAndroidCount;						//��������
	DWORD							dwFullCount;						//��Ա����
	TCHAR							szServerAddr[32];					//��������
	TCHAR							szServerName[LEN_SERVER];			//��������
};

//��������
struct tagSignupMatchInfo
{
	WORD							wServerID;							//�����ʶ
	DWORD							dwMatchID;							//������ʶ
	DWORD							dwMatchNO;							//��������
};

//������Ϣ
struct tagGameMatch
{
	//������Ϣ
	WORD							wServerID;							//�����ʶ
	DWORD							dwMatchID;							//������ʶ
	DWORD							dwMatchNO;							//��������	
	BYTE							cbMatchType;						//��������
	TCHAR							szMatchName[32];					//��������

	//������Ϣ
	BYTE							cbMemberOrder;						//��Ա�ȼ�
	BYTE							cbMatchFeeType;						//�۷�����
	SCORE							lMatchFee;							//��������	

	//������Ϣ
	WORD							wStartUserCount;					//��������
	WORD							wMatchPlayCount;					//��������

	//��������
	WORD							wRewardCount;						//��������

	//����ʱ��
	SYSTEMTIME						MatchStartTime;						//��ʼʱ��
	SYSTEMTIME						MatchEndTime;						//����ʱ��	
};

//��Ƶ����
struct tagAVServerOption
{
	WORD							wAVServerPort;						//��Ƶ�˿�
	DWORD							dwAVServerAddr;						//��Ƶ��ַ
};

//������Ϣ
struct tagOnLineInfoKind
{
	WORD							wKindID;							//���ͱ�ʶ
	DWORD							dwOnLineCount;						//��������
};

//����ͳ��
struct tagOnLineInfoKindEx
{
	WORD							wKindID;							//���ͱ�ʶ
	DWORD							dwOnLineCount;						//��������
	DWORD							dwAndroidCount;						//��������
};

//������Ϣ
struct tagOnLineInfoServer
{
	WORD							wServerID;							//�����ʶ
	DWORD							dwOnLineCount;						//��������
};

//////////////////////////////////////////////////////////////////////////////////
//��������
struct tagAndroidParameter
{
	//���Ա���
	DWORD							dwBatchID;							//���α�ʶ
	DWORD							dwServiceMode;						//����ģʽ
	DWORD							dwAndroidCount;						//������Ŀ
	DWORD							dwEnterTime;						//����ʱ��
	DWORD							dwLeaveTime;						//�뿪ʱ��
	SCORE							lTakeMinScore;						//Я������
	SCORE							lTakeMaxScore;						//Я������
	DWORD							dwEnterMinInterval;					//������
	DWORD							dwEnterMaxInterval;					//������
	DWORD							dwLeaveMinInterval;					//�뿪���
	DWORD							dwLeaveMaxInterval;					//�뿪���	
	DWORD							dwSwitchMinInnings;					//��������
	DWORD							dwSwitchMaxInnings;					//��������	
};

//////////////////////////////////////////////////////////////////////////////////
//�û���Ϣ

//����״̬
struct tagTableStatus
{
	BYTE							cbTableLock;						//������־
	BYTE							cbPlayStatus;						//��Ϸ��־
	LONG							lCellScore;							//��Ԫ����
};

//�û�״̬
struct tagUserStatus
{
	WORD							wTableID;							//��������
	WORD							wChairID;							//����λ��
	BYTE							cbUserStatus;						//�û�״̬
};

//�û�����
struct tagUserAttrib
{
	BYTE							cbCompanion;						//�û���ϵ
};

//�û�����
struct tagUserScore
{
	//������Ϣ
	SCORE							lScore;								//�û��������
	SCORE							lHoldScore;							//���ϻ���

	//��Ӯ��Ϣ
	DWORD							dwWinCount;							//ʤ������
	DWORD							dwLostCount;						//ʧ������
	DWORD							dwDrawCount;						//�;�����
	DWORD							dwFleeCount;						//��������

	//ȫ����Ϣ
	DWORD							dwExperience;						//�û�����
};


//����ʹ��
//struct tagUsePropertyInfo
//{
//	WORD                            wPropertyCount;                     //������Ŀ
//	WORD                            dwValidNum;						    //��Ч����
//	DWORD                           dwEffectTime;                       //��Чʱ��
//};


//�û�����
struct tagUserProperty
{
	DWORD							dwDBID;								//�������ݿ�ID
	WORD                            wPropertyUseMark;                   //���߱�ʾ
	WORD                            wPropertyCount;                     //������Ŀ
	//WORD                            dwValidNum;						    //��Ч����
	//SYSTEMTIME                      EffectTime;                         //��Чʱ��
	SYSTEMTIME						OverTime;							//����ʱ��

	//tagUsePropertyInfo              PropertyInfo[MAX_PT_MARK];			//ʹ����Ϣ   
};

//���߰���
struct tagPropertyPackage
{
	WORD                            wTrumpetCount;                     //С������
	WORD                            wTyphonCount;                      //��������
};

//ʱ����Ϣ
struct tagTimeInfo
{
	DWORD						dwEnterTableTimer;						//��������ʱ��
	DWORD						dwLeaveTableTimer;						//�뿪����ʱ��
	DWORD						dwStartGameTimer;						//��ʼ��Ϸʱ��
	DWORD						dwEndGameTimer;							//�뿪��Ϸʱ��
};

//�û���Ϣ
struct tagUserInfo
{
	//��������
	DWORD							dwUserID;							//�û� I D
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//����ǩ��

	//ͷ����Ϣ
	WORD							wFaceID;							//ͷ������
	//DWORD							dwCustomID;							//�Զ���ʶ
	DWORD							dwCustomFaceVer;					//�Զ���ͷ��汾

	//�û�����
	BYTE							cbGender;							//�û��Ա�
	BYTE							cbMemberOrder;						//��Ա�ȼ�
	BYTE							cbMasterOrder;						//����ȼ�

	//�û�״̬
	WORD							wTableID;							//��������
	WORD							wLastTableID;					    //��Ϸ����
	WORD							wChairID;							//��������
	BYTE							cbUserStatus;						//�û�״̬

	//������Ϣ
	SCORE							lGold;								//�û����
	SCORE							lScore;								//�û�����
	SCORE							lDiamond;							//�û���ʯ
	SCORE							lInsure;							//�û�����

	//��Ϸ��Ϣ
	DWORD							dwWinCount;							//ʤ������
	DWORD							dwLostCount;						//ʧ������
	DWORD							dwDrawCount;						//�;�����
	DWORD							dwFleeCount;						//��������	
	DWORD							dwExperience;						//�û�����
	DWORD							dwUserMedal;						//���ƽ���
	LONG							lLoveLiness;						//�û�����

	//�ȼý�
	BYTE							cbBenefitTimes;						//�������
	SYSTEMTIME						LastBenefitTime;					//�ϴ���ȡʱ��

	//ʱ����Ϣ
	tagTimeInfo						TimerInfo;							//ʱ����Ϣ	
};

//�û���Ϣ
struct tagUserInfoHead
{
	//�û�����
	DWORD							dwUserID;							//�û� I D

	//ͷ����Ϣ
	WORD							wFaceID;							//ͷ������
	DWORD							dwCustomFaceVer;					//�Զ���ʶ�汾

	//�û�����
	BYTE							cbGender;							//�û��Ա�
	BYTE							cbMemberOrder;						//��Ա�ȼ�

	//�û�״̬
	WORD							wTableID;							//��������
	WORD							wChairID;							//��������
	BYTE							cbUserStatus;						//�û�״̬

	//������Ϣ
	SCORE							lScore;								//�û�����
	SCORE							lHoldScore;							//���ϻ���
	//SCORE							lDiamond;							//�û���ʯ
	//SCORE							lInsure;							//�û�����

	////��Ϸ��Ϣ
	//DWORD							dwWinCount;							//ʤ������
	//DWORD							dwLostCount;						//ʧ������
	//DWORD							dwDrawCount;						//�;�����
	//DWORD							dwFleeCount;						//��������
	//DWORD							dwExperience;						//�û�����
	//LONG							lLoveLiness;						//�û�����
};

//ͷ����Ϣ
struct tagCustomFaceInfo
{
	DWORD							dwDataSize;							//���ݴ�С
	DWORD							dwCustomFace[FACE_CX*FACE_CY];		//ͼƬ��Ϣ
};

//�û���Ϣ
struct tagUserRemoteInfo
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwGameID;							//��Ϸ��ʶ
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�

	//�ȼ���Ϣ
	BYTE							cbGender;							//�û��Ա�
	BYTE							cbMemberOrder;						//��Ա�ȼ�
	BYTE							cbMasterOrder;						//����ȼ�

	//λ����Ϣ
	WORD							wKindID;							//���ͱ�ʶ
	WORD							wServerID;							//�����ʶ
	TCHAR							szGameServer[LEN_SERVER];			//����λ��
};

//////////////////////////////////////////////////////////////////////////////////
//�������
struct tagTaskParameter
{
	//������Ϣ
	WORD							wTaskID;							//�����ʶ
	WORD							wTaskType;							//��������
	WORD							wTaskObject;						//����Ŀ��
	BYTE							cbPlayerType;						//�������
	WORD							wKindID;							//���ͱ�ʶ
	DWORD							dwTimeLimit;						//ʱ������

	//������Ϣ
	SCORE							lStandardAwardGold;					//�������
	SCORE							lStandardAwardMedal;				//��������
	SCORE							lMemberAwardGold;					//�������
	SCORE							lMemberAwardMedal;					//��������

	//������Ϣ
	//TCHAR							szTaskName[LEN_TASK_NAME];			//��������
	//TCHAR							szTaskDescribe[320];				//��������
};

//����״̬
struct tagTaskStatus
{
	WORD							wTaskID;							//�����ʶ
	WORD							wTaskProgress;						//�������
	BYTE							cbTaskStatus;						//����״̬
};

//�ͱ�����
struct tagBaseEnsureParameter
{
	SCORE							lScoreCondition;					//��Ϸ������
	SCORE							lScoreAmount;						//��Ϸ������
	BYTE							cbTakeTimes;						//��ȡ����	
};

//�ƹ���Ϣ
struct tagUserSpreadInfo
{
	DWORD							dwSpreadCount;						//�ƹ�����
	SCORE							lSpreadReward;						//�ƹ㽱��
};

//�ȼ�����
struct tagGrowLevelConfig
{
	WORD							wLevelID;							//�ȼ� I D
	DWORD							dwExperience;						//��Ӧ����
};

//�ȼ�����
struct tagGrowLevelParameter
{
	WORD							wCurrLevelID;						//��ǰ�ȼ�
	DWORD							dwExperience;						//��ǰ����
	DWORD							dwUpgradeExperience;				//�¼�����
	SCORE							lUpgradeRewardGold;					//��������
	SCORE							lUpgradeRewardIngot;				//��������
};

//��Ա����
struct tagMemberParameter
{
	BYTE							cbMemberOrder;						//��Ա��ʶ
	TCHAR							szMemberName[16];					//��Ա����
	SCORE							lMemberPrice;						//��Ա�۸�
	SCORE							lPresentScore;						//������Ϸ��
};

//////////////////////////////////////////////////////////////////////////////////

//�㳡����
struct tagGamePlaza
{
	WORD							wPlazaID;							//�㳡��ʶ
	TCHAR							szServerAddr[32];					//�����ַ
	TCHAR							szServerName[32];					//��������
};

//��������
struct tagLevelItem
{
	LONG							lLevelScore;						//�������
	TCHAR							szLevelName[16];					//��������
};

//��Ա����
struct tagMemberItem
{
	BYTE							cbMemberOrder;						//�ȼ���ʶ
	TCHAR							szMemberName[16];					//�ȼ�����
};

//��������
struct tagMasterItem
{
	BYTE							cbMasterOrder;						//�ȼ���ʶ
	TCHAR							szMasterName[16];					//�ȼ�����
};

//�б�����
struct tagColumnItem
{
	BYTE							cbColumnWidth;						//�б���
	BYTE							cbDataDescribe;						//�ֶ�����
	TCHAR							szColumnName[16];					//�б�����
};

//��ַ��Ϣ
struct tagAddressInfo
{
	TCHAR							szAddress[32];						//�����ַ
};

//������Ϣ
struct tagDataBaseParameter
{
	WORD							wDataBasePort;						//���ݿ�˿�
	TCHAR							szDataBaseAddr[32];					//���ݿ��ַ
	TCHAR							szDataBaseUser[32];					//���ݿ��û�
	TCHAR							szDataBasePass[32];					//���ݿ�����
	TCHAR							szDataBaseName[32];					//���ݿ�����
};

//���������
struct tagCacheServerParameter
{
	WORD							wServerPort;						//����������˿�
	TCHAR							szServerAddr[64];					//�����������ַ
	TCHAR							szServerPass[33];					//�������������
};

//��������
struct tagServerOptionInfo
{
	//�ҽ�����
	WORD							wKindID;							//�ҽ�����
	WORD							wNodeID;							//�ҽӽڵ�
	WORD							wSortID;							//���б�ʶ

	//˰������
	WORD							wRevenueRatio;						//˰�ձ���
	SCORE							lServiceScore;						//�������

	//��������
	SCORE							lRestrictScore;						//���ƻ���
	SCORE							lMinTableScore;						//��ͻ���
	SCORE							lMinEnterScore;						//��ͻ���
	SCORE							lMaxEnterScore;						//��߻���

	//��Ա����
	BYTE							cbMinEnterMember;					//��ͻ�Ա
	BYTE							cbMaxEnterMember;					//��߻�Ա

	//��������
	DWORD							dwServerRule;						//�������
	TCHAR							szServerName[LEN_SERVER];			//��������
};

//�ȼý�����
struct tagBenefitInfo
{
	SCORE							lGoldLimit;							//��ȡ�ʸ�			
	BYTE							cbReceiveTimes;						//ÿ�տ���ɴ���
	WORD							wTimeIntervals;						//���η���ʱ����
	//WORD							RewardPropID;						//��������ID
	TCHAR							szRewardProp[256];					//��������
	TCHAR							szPromptMsg[128];					//��ʾ��Ϣ
	TCHAR							szEmailTitle[128];					//�ʼ�����
	TCHAR							szEmailMsg[320];					//�ʼ�����
};

//���а����ʼ�����
struct tagRankingRewardEmailConfig
{
	TCHAR							szEmailTitle[128];					//�ʼ�����
	TCHAR							szEmailMsg[320];					//�ʼ�����
};

//���а�������
struct tagRankingRewardConfig
{
	WORD							wRankingID;							//����ID
	char							szPropList[128];					//�����б�
	char							szPropListDesc[128];				//��������
};

//////////////////////////////////////////////////////////////////////////////////

//������Ϣ
struct tagUpdateInfo
{
	WORD							wStructSize;						//�ṹ��С
	WORD							wUpdateCount;						//������Ŀ
	char							szDownLoadAddress[MAX_PATH];		//���ص�ַ
};

//��������
struct tagUpdateItem
{
	WORD							wStructSize;						//�ṹ��С
	char							szFileCheckSum[33];					//�ļ�У���
	char							szFileName[128];					//�ļ�����
};

//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif