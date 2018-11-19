#ifndef CMD_OX_HEAD_FILE
#define CMD_OX_HEAD_FILE

#pragma pack(1)
//////////////////////////////////////////////////////////////////////////
//�����궨��

#define KIND_ID						101										//��Ϸ I D
#define GAME_PLAYER					4										//��Ϸ����
#define GAME_NAME					TEXT("ţţ")							//��Ϸ����
#define VERSION_SERVER				PROCESS_VERSION(6,0,3)					//����汾
#define VERSION_CLIENT				PROCESS_VERSION(6,0,3)					//����汾

#define GAME_GENRE					(GAME_GENRE_GOLD|GAME_GENRE_MATCH)		//��Ϸ����
#define MAXCOUNT					5										//�˿���Ŀ

//����ԭ��
#define GER_NO_PLAYER				0x10									//û�����

//��Ϸ״̬
#define GS_TK_FREE					GAME_STATUS_FREE                        //�ȴ���ʼ
#define GS_TK_CALL					GAME_STATUS_PLAY						//��ׯ״̬
#define GS_TK_SCORE					GAME_STATUS_PLAY+1						//��ע״̬
#define GS_TK_PLAYING				GAME_STATUS_PLAY+2						//��Ϸ����

//�û�״̬
#define USEX_NULL                   0                                       //�û�״̬
#define USEX_PLAYING                1                                       //�û�״̬
#define USEX_DYNAMIC                2                                       //�û�״̬   

//////////////////////////////////////////////////////////////////////////
//����������ṹ

#define SUB_S_GAME_START				100									//��Ϸ��ʼ
#define SUB_S_ADD_SCORE					101									//��ע���
#define SUB_S_PLAYER_EXIT				102									//�û�ǿ��
#define SUB_S_SEND_CARD					103									//������Ϣ
#define SUB_S_GAME_END					104									//��Ϸ����
#define SUB_S_OPEN_CARD					105									//�û�̯��
#define SUB_S_CALL_BANKER				106									//�û���ׯ

//��Ϸ״̬
struct CMD_S_StatusFree
{
	LONGLONG							lCellScore;							//��������
};

//��Ϸ״̬
struct CMD_S_StatusCall
{
	WORD							    	wCallBanker;						//��ׯ�û�
	BYTE							        cbPlayStatus[GAME_PLAYER];          //�û�״̬
};

//��Ϸ״̬
struct CMD_S_StatusScore
{
	//��ע��Ϣ
	LONGLONG								lTurnMaxScore;						//�����ע
	//LONGLONG								lTurnLessScore;						//��С��ע
	LONGLONG								lTableScore[GAME_PLAYER];			//��ע��Ŀ
	BYTE								    cbPlayStatus[GAME_PLAYER];          //�û�״̬
	WORD							    	wBankerUser;						//ׯ���û�
};

//��Ϸ״̬
struct CMD_S_StatusPlay
{
	//״̬��Ϣ	
	BYTE								    cbPlayStatus[GAME_PLAYER];          //�û�״̬
	LONGLONG								lTurnMaxScore;						//�����ע
	//LONGLONG								lTurnLessScore;						//��С��ע
	LONGLONG								lTableScore[GAME_PLAYER];			//��ע��Ŀ
	WORD								    wBankerUser;						//ׯ���û�

	//�˿���Ϣ
	BYTE							    	cbHandCardData[GAME_PLAYER][MAXCOUNT];//�����˿�
	BYTE						      		bOxCard[GAME_PLAYER];				//ţţ����
};

//�û���ׯ
struct CMD_S_CallBanker
{
	WORD							     	wCallBanker;						//��ׯ�û�
	bool							    	bFirstTimes;						//�״ν�ׯ
};

//��Ϸ��ʼ
struct CMD_S_GameStart
{
	//��ע��Ϣ
	LONGLONG								lTurnMaxScore;						//�����ע
	WORD							     	wBankerUser;						//ׯ���û�
};

//�û���ע
struct CMD_S_AddScore
{
	WORD							    	wAddScoreUser;						//��ע�û�
	LONGLONG								lAddScoreCount;						//��ע��Ŀ
};

//��Ϸ����
struct CMD_S_GameEnd
{
	LONGLONG								lGameTax[GAME_PLAYER];				//��Ϸ˰��
	LONGLONG								lGameScore[GAME_PLAYER];			//��Ϸ�÷�
	BYTE							     	cbCardData[GAME_PLAYER];			//�û��˿�
};

//�������ݰ�
struct CMD_S_SendCard
{
	BYTE								    cbCardData[GAME_PLAYER][MAXCOUNT];	//�û��˿�
};

//�û��˳�
struct CMD_S_PlayerExit
{
	WORD						      		wPlayerID;							//�˳��û�
};

//�û�̯��
struct CMD_S_Open_Card
{
	WORD							     	wPlayerID;							//̯���û�
	BYTE							      	bOpen;								//̯�Ʊ�־
};
//////////////////////////////////////////////////////////////////////////
//�ͻ�������ṹ
#define SUB_C_CALL_BANKER				1									//�û���ׯ
#define SUB_C_ADD_SCORE					2									//�û���ע
#define SUB_C_OPEN_CARD					3									//�û�̯��

//�û���ׯ
struct CMD_C_CallBanker
{
	BYTE							    	bBanker;							//��ׯ��־
};

//�û���ע
struct CMD_C_AddScore
{
	LONGLONG								lScore;								//��ע��Ŀ
};

//�û�̯��
struct CMD_C_OxCard
{
	BYTE							    	bOX;								//ţţ��־
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()

#endif
