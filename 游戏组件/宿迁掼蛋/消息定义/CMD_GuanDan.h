#ifndef CMD_SHOWHAND_HEAD_FILE
#define CMD_SHOWHAND_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//�����궨��


#define KIND_ID						26									//��Ϸ I D;
//��Ϸ������Ϣ
#define GAME_NAME					TEXT("�走")							//��Ϸ����;

//�������
#define VERSION_SERVER				PROCESS_VERSION(6,0,3)				//����汾;
#define VERSION_CLIENT				PROCESS_VERSION(6,0,3)				//����汾;

//��Ϸ��Ϣ
#define GAME_PLAYER					4									//��Ϸ����;
#define MAX_COUNT					28									//�������;
#define HALF_COUNT					MAX_COUNT/2							//һ������;
//��Ϸ������Ϣ
#define SHUANG_XIA					10									//��Ϸ˫��;
#define DAN_XIA						11									//��Ϸ����;
#define OTHER						12									//����ĩ��;
//����״̬
#define GS_WK_FREE					GAME_STATUS_FREE					//�ȴ���ʼ;
#define GS_WK_SEND_CARD				GAME_STATUS_PLAY					//����״̬;
#define GS_WK_TRIBUTE			    (GAME_STATUS_PLAY+1)				//����״̬;
#define GS_WK_PLAYING				(GAME_STATUS_PLAY+2)				//����״̬;
#define GS_WK_WAIT_CONTINUE			(GAME_STATUS_PLAY+3)				//������Ϸ�ȴ�״̬;

//�����Ӯ����
#define MAX_RATE					3

//�������
#define	REQUEST_QUIT_COUNT			3

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//�����׶�
enum enPayStatus
{
	enNone,																//��;
	enJin_Gong,															//����;
	enHui_Gong,															//�ع�;
	enGet_Hui_Gong,														//��ȡ�ع�;
	enKang_Gong															//����;
};

//��Ϸ����
struct Series
{
	BYTE                            cbOurSeries;                        //�ҷ�����;
	BYTE                            cbOtherSeries;                      //�Է�����;
	BYTE                            cbCurrentSeries;                    //���ּ���;
};
//������Ϣ
struct TributeInfo
{
	WORD							wFrom;						//��(��)�����;
	WORD							wTo;						//��(��)����˭;
	BYTE							cbCard;						//��(��)������;
	TributeInfo()
	{
		wFrom = INVALID_CHAIR;
		wTo = INVALID_CHAIR;
		cbCard = 0;
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//����״̬
struct CMD_S_StatusFree
{
	BYTE							cbTimePayTribute;					//����ʱ��;
	BYTE							cbTimeRetTribute;					//�ع�ʱ��;
	BYTE							cbTimeHeadOutCard;					//�׳�ʱ��;
	BYTE							cbTimeOutCard;						//����ʱ��;
	BYTE							cbTimeTrustee;						//�й�ʱ��;

	BYTE							cbCurSeries;						//��ǰ����;
	LONG							lBaseScore;							//�����ע;
};

//���Ƴ���
struct CDM_S_StatusSendCard
{
	BYTE							cbTimePayTribute;					//����ʱ��;
	BYTE							cbTimeRetTribute;					//�ع�ʱ��;
	BYTE							cbTimeHeadOutCard;					//�׳�ʱ��;
	BYTE							cbTimeOutCard;						//����ʱ��;
	BYTE							cbTimeTrustee;						//�й�ʱ��;

	LONG							lBaseScore;							//�����ע;
	Series							stSeries;							//��ǰ�ȼ����;
	bool							bWin;								//ż�����Ͼ��Ƿ�Ӯ;
	BYTE							cbHandCardCount[GAME_PLAYER];		//�����������;
	BYTE							cbHandCardData[27];					//�Լ���������;
};

//����ʱ����
struct CMD_S_StatusTribute
{
	BYTE							cbTimePayTribute;					//����ʱ��;
	BYTE							cbTimeRetTribute;					//�ع�ʱ��;
	BYTE							cbTimeHeadOutCard;					//�׳�ʱ��;
	BYTE							cbTimeOutCard;						//����ʱ��;
	BYTE							cbTimeTrustee;						//�й�ʱ��;

	LONG							lBaseScore;							//�����ע;
	BYTE							cbCardData[MAX_COUNT];				//��������;
	BYTE							cbCardCount[GAME_PLAYER];			//������Ŀ;
	Series                          stSeries;                           //��Ϸ����;
	bool							bLastRoundWin;						//ż�����Ͼ��Ƿ�Ӯ;
	BYTE							cbUserTrustee[4];					//����й�״̬;
};

//��ʱ����
struct CMD_S_StatusPlay
{
	BYTE							cbTimePayTribute;					//����ʱ��;
	BYTE							cbTimeRetTribute;					//�ع�ʱ��;
	BYTE							cbTimeHeadOutCard;					//�׳�ʱ��;
	BYTE							cbTimeOutCard;						//����ʱ��;
	BYTE							cbTimeTrustee;						//�й�ʱ��;

	LONG							lBaseScore;							//�����ע;
	WORD							wLastOutUser;						//���Ƶ���;
	WORD				 			wCurrentUser;						//��ǰ���;
	BYTE							cbCardData[27];						//�����˿�;
	BYTE							cbCardCount[4];						//�˿���Ŀ;
	BYTE							cbTurnCardData[10];					//�����б�;
	BYTE							cbTurnCardCount;					//��������;
	Series                          stSeries;                           //��Ϸ����;
	bool							bWin;								//ż�����Ͼ��Ƿ�Ӯ;
	BYTE							cbUserTrustee[4];					//����й�״̬;
	BYTE							cbRanking[4];						//����;
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////
//����������ṹ
#define SUB_S_START_OUT				101									//������ɺ�ʼ�׼ҳ���;
#define SUB_S_OUT_CARD				102									//�û�����;
#define SUB_S_PASS_CARD				103									//��������;
#define SUB_S_PAY_TRIBUTE_NOTIFY	104                                 //֪ͨ�û�����;
#define SUB_S_PAY_TRIBUTE_CARD		105									//��������;
#define SUB_S_PAY_TRIBUTE_RESULT	106                                 //�������;
#define SUB_S_RETURN_TRIBUTE		107									//�ع��˿�;
#define SUB_S_GAME_END				108									//��Ϸ����;
#define SUB_S_REQUEST_QUIT			109									//�����˳�;
#define SUB_S_RESPONSE_QUIT         110									//��Ӧ�˳�;
#define SUB_S_OPPOSITE_CARD			111									//�Լҵ���;

#define SUB_S_ALL_CARD				112									//�����˿�;
#define SUB_S_USER_OUT_OVER			113									//�û�����;
#define SUB_S_SEND_CARD				121									//������Ϣ;
#define SUB_S_USER_TRUST			122									//�û��й�;
#define SUB_S_FORCE_QUIT			124									//����;


//�û�������Ϣ;
struct CMD_S_User_Out_Over
{
	WORD							wChairID;							//�����û�;
	BYTE							cbRanking;							//����;
};

//�����˿�;
struct CMD_S_AllCard
{
	BYTE cbHandCardCount[GAME_PLAYER];
	BYTE cbHandCardData[GAME_PLAYER][MAX_COUNT];
};

//�����˿�;
struct CMD_S_Send_Card
{
	Series							stSeries;							//��һ�ֵȼ�;
	BYTE							cbCardData[27];						//������;
};

//��ʼ����;
struct CMD_S_StartOut
{
	WORD				 			wCurrentUser;						//��ǰ���;
	WORD							wWinSideUser;						//̨�����;
	BYTE							cbHandCardCount;					//�˿�����;
	BYTE							cbHandCardData[27];					//�˿�����;
};

//�Լҵ���;
struct CMD_S_Opposite_Card
{
	WORD							wFromUser;							//�Լ�ID;
	BYTE							cbCardCount;						//�˿˴�С;
	BYTE							cbCardData[27];						//�˿�����;
};
//�û�����;
struct CMD_S_OutCard
{
	WORD				 			wCurrentUser;						//��ǰ���;
	WORD							wOutCardUser;						//�������;
	WORD							wLastOutUser;						//�ϼҳ������;
	BYTE							cbCardType;							//��������;
	BYTE							cbCardCount;						//�˿���Ŀ;
	BYTE							cbCardData[10];						//�˿��б�;
};
//��������;
struct CMD_S_PassCard
{
	BYTE							bNewTurn;							//һ�ֿ�ʼ;
	WORD				 			wPassUser;							//�������;
	WORD				 			wCurrentUser;						//��ǰ���;
	WORD							wJiefengUser;						//�ӷ��û�;
};

//֪ͨ����;
struct CMD_S_PayTributeNotify
{	
	BYTE							cbPayStatus;						//�����׶�;
};

//�����˿�;
struct CMD_S_PayTributeCard
{
	WORD							wChairID;							//��λ��;
	BYTE							cbCardData;							//��;
};

//�������;
struct CMD_S_PayTributeResult
{
	TributeInfo						stTribute[2];						//��(��)����Ϣ;
};

//�û��й�;
struct CMD_S_User_Trust
{
	WORD							wChairID;							//��λ��;
	BYTE							cbUserTrustee;						//�й�״̬;
};
//��Ϸ����;
struct CMD_S_GameEnd
{
	BYTE							cbGameEndType;						//��������;
	LONG							lGameScore[4];						//��Ϸ����;
	BYTE							cbCardCount[4];						//�˿���Ŀ;
	BYTE							cbCardData[108];					//�˿��б�;
	BYTE							cbRanking[4];						//��Ϸ����;
	bool							bTongGuan;							//�Ƿ�ͨ��;
	Series                          stSeries;                           //��Ϸ����;
};

//�������ݰ�;
struct CMD_S_RequestQuit
{
	WORD							wChairID;							//��λ��;
};
//����Ϣ;
struct CMD_S_ResponseQuit
{
	WORD							wChairID;							//��λ��;
	bool                            bResponse;							//��Ӧ��;
};
//����;
struct CMD_S_ForceQuit
{
	WORD							wChairID;
};


//////////////////////////////////////////////////////////////////////////
//�ͻ�������ṹ;
#define SUB_C_OUT_CARD				2									//�û�����;
#define SUB_C_PASS_CARD				3									//��������;
#define SUB_C_PAY_TRIBUTE           4                                   //��ҽ���;
#define SUB_C_RETURN_TRIBUTE        5                                   //��һع�;
#define SUB_C_USER_TRUST			6									//�û��й�;
#define SUB_C_HALFWAY_QUIT			7									//��;�˳�;
#define SUB_C_REQUEST_QUIT			8									//ǿ���˳�;
#define SUB_C_RESPONSE_QUIT         9									//��Ӧ��;

//�������Ӳ���;
struct CMD_C_SetTableInfo
{
	BYTE							cbSetSeries;						//����򵽼�;
	LONG							lBaseScore;							//���ӵ�ע;
	BYTE							cbSetTableAction;					//�Ƿ����û�ͬ��;
};

//�û��й�;
struct CMD_C_UserTrust
{
	BYTE							cbUserTrustee;						//�й�״̬;
};
//�������ݰ�;
struct CMD_C_OutCard
{
	BYTE							cbCardCount;						//������Ŀ;
	BYTE							cbCardData[27];						//�˿��б�;
};
//�������ݰ�;
struct CMD_C_RequestQuit
{
	WORD							wChairID;							//��λ��;
	BYTE							cbRequestQuitType;					//0:�����˳� 1:ǿ��;
};
//����Ϣ;
struct CMD_C_ResposeQuit
{
	WORD							wChairID;							//��λ��;
	BYTE                            cbResponseQuitType;					//��Ӧ��;
};

//�û�����;
struct CMD_C_PayTribute
{
	BYTE							cbCard;								//��(��)������;
};

// ��Ϸ��ɢ�ṹ;
struct CMD_S_Private_End_Info
{
	WORD		wMainChairID;				// ����;
	BYTE		cbFinishCout;				// ��ɾ���;
	SCORE	    lAllScore[GAME_PLAYER];		// �ܳɼ�;
	SCORE	    lDetails[GAME_PLAYER][16];	// �ɼ���ϸ(Ŀǰ�������16��);

	void StreamValue(DataStream& kData, bool bSend)
	{
		if (bSend)
		{
			kData.pushValue(this, sizeof(CMD_S_Private_End_Info));
		}
		else
		{
			kData.popValue(this, sizeof(CMD_S_Private_End_Info));
		}
	}
};

#pragma pack()

#endif