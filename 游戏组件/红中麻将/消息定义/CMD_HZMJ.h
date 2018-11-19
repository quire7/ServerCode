#ifndef CMD_SPARROW_HEAD_FILE
#define CMD_SPARROW_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//�����궨��
#pragma pack(1)

#define KIND_ID						305									//��Ϸ I D

//�������
#define GAME_PLAYER					4									//��Ϸ����
#define VERSION_SERVER				PROCESS_VERSION(6,0,3)				//����汾
#define VERSION_CLIENT				PROCESS_VERSION(6,0,3)				//����汾

#define GAME_NAME					TEXT("�����齫")						//��Ϸ����
#define GAME_GENRE					(GAME_GENRE_SCORE|GAME_GENRE_MATCH|GAME_GENRE_GOLD)	//��Ϸ����

//��Ϸ״̬
#define GS_MJ_FREE					GAME_STATUS_FREE								//����״̬
#define GS_MJ_PLAY					(GAME_STATUS_PLAY+1)							//��Ϸ״̬
#define GS_MJ_XIAOHU				(GAME_STATUS_PLAY+2)							//С��״̬

//��������
#define MAX_WEAVE					4									//������
#define MAX_INDEX					34									//�������
#define MAX_COUNT					14									//�����Ŀ
#define MAX_REPERTORY				108									//�����
#define MAX_REPERTORY_HZ			112									//�����齫�����

#define MAX_LEFT_NUM_HZ				59									//�����齫���ʣ������(59 == (MAX_REPERTORY_HZ - 13*4 - 1));

#define MAX_NIAO_CARD				8									//���������


#define MAX_RIGHT_COUNT				1									//���ȨλDWORD����	

#define GAME_TYPE_ZZ				0
#define GAME_TYPE_CS				1

//�����־(1:��־�Ƿ���AA);

enum eGAME_RULE_ZZ
{
	GAME_RULE_ZZ_258			=2,		//ֻ��258����;
	GAME_RULE_ZZ_ZIMOHU		    =4,		//ֻ����ģ��;
	GAME_RULE_ZZ_QIDUI			=8,		//�ɺ��߶�;
	GAME_RULE_ZZ_QIANGGANGHU	=16,	//�����ܺ�;
	GAME_RULE_ZZ_ZHANIAO4		=32,	//����4��;
	GAME_RULE_ZZ_ZHANIAO6		=64,	//����6��;
	GAME_RULE_ZZ_ZHANIAO8		=128,	//����8��;
	GAME_RULE_ZZ_ONESHOOT		=256,	//һ������;
	GAME_RULE_ZZ_HONGZHONG		=512,	//�����淨;
};

#define ZZ_ZHANIAO0		0		//����0��
#define ZZ_ZHANIAO4		4		//����4��
#define ZZ_ZHANIAO6		6		//����6��
#define ZZ_ZHANIAO8		8		//����8��


//�����������;
#define MAX_TING_OUT				14									//���ɴ������;
#define MAX_TING_COUNT				9									//�����9����;
//////////////////////////////////////////////////////////////////////////

//�������
struct CMD_WeaveItem
{
	BYTE							cbWeaveKind;						//�������
	BYTE							cbCenterCard;						//�����˿�
	BYTE							cbPublicCard;						//������־
	WORD							wProvideUser;						//��Ӧ�û�
};

//��������
struct tagTingPaiItem
{
	BYTE							cbOutCardData;						//���ȥ����;
	BYTE							cbTingCardCount;					//������Ŀ;
	BYTE							cbTingCardData[MAX_TING_COUNT];		//��������;
};

//���ƽ��;
struct tagTingPaiResult
{
	BYTE							cbTingPaiItemCount;					//�ɴ�����Ŀ;
	tagTingPaiItem					TingPaiItemArrary[MAX_TING_OUT];	//����������;
};

//////////////////////////////////////////////////////////////////////////
//����������ṹ

#define SUB_S_GAME_START			100									//��Ϸ��ʼ
#define SUB_S_OUT_CARD				101									//��������
#define SUB_S_SEND_CARD				102									//�����˿�
#define SUB_S_OPERATE_NOTIFY		104									//������ʾ
#define SUB_S_OPERATE_RESULT		105									//��������
#define SUB_S_GAME_END				106									//��Ϸ����
#define SUB_S_TRUSTEE				107									//�û��й�
#define SUB_S_CHI_HU				108									//
#define SUB_S_GANG_SCORE			110									//
#define SUB_S_TING_PAI				111									//

//��Ϸ״̬
struct CMD_S_StatusFree
{
	int								lCellScore;							//�������
	WORD							wBankerUser;						//ׯ���û�
	WORD							wEastUser;							//�����û�
	bool							bTrustee[GAME_PLAYER];				//�Ƿ��й�
};

//��Ϸ״̬
struct CMD_S_StatusPlay
{
	//��Ϸ����
	int								lCellScore;									//��Ԫ����
	WORD							wBankerUser;								//ׯ���û�
	WORD							wCurrentUser;								//��ǰ�û�
	WORD							wEastUser;									//�����û�

	//״̬����
	BYTE							cbActionCard;								//�����˿�
	DWORD							cbActionMask;								//��������
	BYTE							cbLeftCardCount;							//ʣ����Ŀ
	bool							bTrustee[GAME_PLAYER];						//�Ƿ��й�
	WORD							wWinOrder[GAME_PLAYER];						//

	//������Ϣ
	WORD							wOutCardUser;								//�����û�
	BYTE							cbOutCardData;								//�����˿�
	BYTE							cbDiscardCount[GAME_PLAYER];				//������Ŀ
	BYTE							cbDiscardCard[GAME_PLAYER][60];				//������¼

	//�˿�����
	BYTE							cbCardCount;								//�˿���Ŀ
	BYTE							cbCardData[MAX_COUNT];						//�˿��б�
	BYTE							cbSendCardData;								//�����˿�

	//����˿�
	BYTE							cbWeaveCount[GAME_PLAYER];					//�����Ŀ
	CMD_WeaveItem					WeaveItemArray[GAME_PLAYER][MAX_WEAVE];		//����˿�

	tagTingPaiItem					TingPaiResult;								//���ƽ��;

	BYTE							cbOutCardTime;								//����ʱ��
	BYTE							cbOperateTime;								//����ʱ��
};

//��Ϸ��ʼ
struct CMD_S_GameStart
{
	int								lSiceCount;									//���ӵ���
	WORD							wBankerUser;								//ׯ���û�
	WORD							wCurrentUser;								//��ǰ�û�
	WORD							wEastUser;									//�����û�
	DWORD							cbUserAction;								//�û�����
	BYTE							cbCardData[MAX_COUNT];						//�˿��б�
	BYTE							cbLeftCardCount;							//
	BYTE							cbXiaoHuTag;								//С����� 0 ûС�� 1 ��С����

	BYTE							cbOutCardTime;								//����ʱ��
	BYTE							cbOperateTime;								//����ʱ��
};

//�����˿�ʼ;
struct CMD_S_AndroidGameStart
{
	CMD_S_GameStart					GameStart;
	BYTE							cbCardData[MAX_COUNT*GAME_PLAYER];			//�˿��б�
};

//��������
struct CMD_S_OutCard
{
	WORD							wOutCardUser;						//�����û�
	BYTE							cbOutCardData;						//�����˿�
};

//�����˿�
struct CMD_S_SendCard
{
	BYTE							cbCardData;							//�˿�����
	DWORD							cbActionMask;						//��������
	WORD							wCurrentUser;						//��ǰ�û�
	bool							bTail;								//ĩβ����
};


//������ʾ
struct CMD_S_OperateNotify
{
	WORD							wResumeUser;						//��ԭ�û�
	DWORD							cbActionMask;						//��������
	BYTE							cbActionCard;						//�����˿�
};

//��������
struct CMD_S_OperateResult
{
	WORD							wOperateUser;						//�����û�
	WORD							wProvideUser;						//��Ӧ�û�
	DWORD							cbOperateCode;						//��������
	BYTE							cbOperateCard;						//�����˿�
};

//��Ϸ����
struct CMD_S_GameEnd
{
	BYTE							cbCardCount[GAME_PLAYER];			//
	BYTE							cbCardData[GAME_PLAYER][MAX_COUNT];	//
	//������Ϣ
	WORD							wProvideUser[GAME_PLAYER];			//��Ӧ�û�
	DWORD							dwChiHuRight[GAME_PLAYER];			//��������
	DWORD							dwStartHuRight[GAME_PLAYER];		//���ֺ�������
	LONG							lStartHuScore[GAME_PLAYER];			//���ֺ��Ʒ���

	//������Ϣ
	LONG							lGameScore[GAME_PLAYER];			//��Ϸ����
	int								lGameTax[GAME_PLAYER];				//

	WORD							wWinOrder[GAME_PLAYER];				//��������

	LONG							lGangScore[GAME_PLAYER];//��ϸ�÷�
	BYTE							cbGenCount[GAME_PLAYER];			//
	WORD							wLostFanShu[GAME_PLAYER][GAME_PLAYER];
	WORD							wLeftUser;	//

	//����˿�
	BYTE							cbWeaveCount[GAME_PLAYER];					//�����Ŀ
	CMD_WeaveItem					WeaveItemArray[GAME_PLAYER][MAX_WEAVE];		//����˿�


	BYTE							cbCardDataNiao[MAX_NIAO_CARD];	// ����
	BYTE							cbNiaoCount;	//���Ƹ���
	BYTE							cbNiaoPick;	//�������

	BYTE							cbLeftNum;									//ʣ������;
	BYTE							cbLeftCardData[MAX_LEFT_NUM_HZ];			//ʣ����ֵ;

	BYTE							cbChiHuCard;								//���Ƶ���ֵ;
};

//�û��й�
struct CMD_S_Trustee
{
	bool							bTrustee;							//�Ƿ��й�
	WORD							wChairID;							//�й��û�
};

//
struct CMD_S_ChiHu
{
	WORD							wChiHuUser;							//
	WORD							wProviderUser;						//
	BYTE							cbChiHuCard;						//
	BYTE							cbCardCount;						//
	LONG							lGameScore;							//
	BYTE							cbWinOrder;							//
};

//
struct CMD_S_GangScore
{
	WORD							wChairId;							//
	BYTE							cbXiaYu;							//
	LONG							lGangScore[GAME_PLAYER];			//
};

//////////////////////////////////////////////////////////////////////////
//�ͻ�������ṹ

#define SUB_C_OUT_CARD				1									//��������
#define SUB_C_OPERATE_CARD			3									//�����˿�
#define SUB_C_TRUSTEE				4									//�û��й�
#define SUB_C_XIAOHU				5									//С��
#define SUB_C_MASTER_CHEAKCARD		10									//ѡ�����

//��������
struct CMD_C_OutCard
{
	BYTE							cbCardData;							//�˿�����
};

//��������
struct CMD_C_OperateCard
{
	BYTE							cbOperateCode;						//��������
	BYTE							cbOperateCard;						//�����˿�
};

//�û��й�
struct CMD_C_Trustee
{
	bool							bTrustee;							//�Ƿ��й�	
};

//����С��
struct CMD_C_XiaoHu
{
	BYTE							cbOperateCode;						//��������
	BYTE							cbOperateCard;						//�����˿�
};

struct CMD_C_MaterCheckCard
{
	BYTE							cbCheakCard;						//�����˿�
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif
