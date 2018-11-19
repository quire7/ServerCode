#ifndef CMD_DZSHOWHAND_HEAD_FILE
#define CMD_DZSHOWHAND_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//�����궨��

#define KIND_ID							7									//��Ϸ I D
#define GAME_PLAYER						9									//��Ϸ����
#define GAME_NAME						TEXT("�����˿�")					//��Ϸ����

#define GOLD_RATE                       30                                 //�����ⶥ

//�汾��Ϣ
#define VERSION_SERVER					PROCESS_VERSION(0,0,1)				//����汾
#define VERSION_CLIENT					PROCESS_VERSION(0,0,1)				//����汾

//����ԭ��
#define GER_NO_PLAYER					0x10								//û�����

//��Ŀ����
#define FULL_COUNT					    52									//ȫ����Ŀ
#define MAX_COUNT						2									//�����Ŀ
#define MAX_CENTERCOUNT					5									//�����Ŀ

//��ֵ����
#define	CARD_MASK_COLOR				0xF0									//��ɫ����
#define	CARD_MASK_VALUE				0x0F									//��ֵ����

#define  SMALL_CARD_WIDTH			23
#define  SMALL_CARD_HEIGHT			30

//���ͻ���
#define DASHANG_COST    500

//�����˲���
#define INT_ROBOT_QUANYA				21									//ȫѺ���߿���
#define INT_ROBOT_GIVEUP				42									//���ò��������ʵ�����
#define INT_ROBOT_ADD   				32									//���ò��������ʵ�����
#define INT_ROBOT_CARDLEVEL				5									//�Ƶȼ�
#define INT_ROBOT_BEI   				5									//���ò����������õ�����
#define INT_ROBOT_AI  			    	3									//���ò���AI�ȼ�����
#define INT_ROBOT_FIRSTCARD				5									//�Ƶȼ�
#define INT_ROBOT_CARDTYPE              5                                   //����
//X ���з�ʽ
enum enXCollocateMode 
{
	enXLeft,						//�����
	enXCenter,						//�ж���
	enXRight,						//�Ҷ���
};

//Y ���з�ʽ
enum enYCollocateMode 
{ 
	enYTop,							//�϶���
	enYCenter,						//�ж���
	enYBottom,						//�¶���
};

#define VALIDE_CHAIRID(wChairID) ((wChairID) >=0 && (wChairID) < GAME_PLAYER)
//�˿˽ṹ
struct tagCardItem
{
	bool							bEffect;							//��Ч��־
	bool							bMy;								//�Լ���־
	bool							bShoot;								//�����־
	BYTE							cbCardData;							//�˿�����
};



//���Զ���
#define MAX_CARD_COUNT				5									//�˿���Ŀ
#define SPACE_CARD_DATA				255									//����˿�


//��ඨ��
#define DEF_X_DISTANCE				19									//Ĭ�ϼ��
#define DEF_Y_DISTANCE				17									//Ĭ�ϼ��
#define DEF_SHOOT_DISTANCE			20									//Ĭ�ϼ��

//��ע����
#define CHIP_TYPE_ADD     1    //��ע
#define CHIP_TYPE_GEN     2    //��
#define CHIP_TYPE_ALLIN   3    //ȫ��
#define CHIP_TYPE_PASS    4    //����
#define CHIP_TYPE_GIVEUP  5    //����
#define CHIP_TYPE_OVER    6    //������ע  ��Ϸ����
//////////////////////////////////////////////////////////////////////////
//����������ṹ
#define SUB_S_GAME_START				100							//��Ϸ��ʼ
#define SUB_S_ADD_SCORE					101							//��ע���
#define SUB_S_GIVE_UP					102							//������ע
#define SUB_S_TURN_OVER					103							//һ�ֽ���
#define SUB_S_SEND_CARD					104							//������Ϣ
#define SUB_S_GAME_END					105							//��Ϸ����
#define SUB_S_SIT_DOWN					106							//�û�����
#define SUB_S_OPEN_CARD					107							//�û�����
#define SUB_S_LOOK_CARD					108							//�û�����
#define SUB_S_USER_ROBOT				111							//������
#define SUB_S_GAN_READY                 112                         //������׼��

#define SUB_S_SET_BASESCORE				113							//���õ�ע
#define SUB_S_KICK_OUT					114							//�߳����
#define SUB_S_ADD_TAKEGOLD				115							//���Ӵ�����
#define SUB_S_DASHANG					116							//����

//��Ϸ״̬
struct CMD_S_StatusFree
{
	LONG							lBaseScore;							//��������
	LONG                            lAddTakeGoldMin;                    //���Ӵ�������Сֵ
	LONG                            lAddTakeGoldMax;                    //���Ӵ��������ֵ
	LONG                            lUserTakeGold[GAME_PLAYER];         //��Ҵ�����
};

//��Ϸ״̬
struct CMD_S_StatusPlay
{
	//��־����	
	//��ע��Ϣ
	LONG								lCellScore;							//��Ԫ��ע
	LONG								lTurnMaxScore[GAME_PLAYER];			//�����ע
	LONG								lTurnLessScore;						//��С��ע
	LONG								lCellMaxScore;						//�����ע
	LONG								lAddLessScore;						//����Сע
	LONG                                lAllChipScore;                      //��ǰ����ע
	LONG								lTableScore[GAME_PLAYER];			//��ע��Ŀ
	LONG								lTotalScore[GAME_PLAYER];			//�ۼ���ע
	LONG                                lTableBuffer[GAME_PLAYER];          //������ע
	LONG                                lUserMaxScore[GAME_PLAYER];         //���Я��
	LONG                                aryChipPool[GAME_PLAYER+1];         //�׳س���
	LONG                                aryEndScore[GAME_PLAYER];  

	//״̬��Ϣ
	WORD								wDUser;								//D���
	WORD								wDMUser[2];							//äע���

	WORD				 				wCurrentUser;						//��ǰ���
	BYTE								cbPlayStatus[GAME_PLAYER];			//��Ϸ״̬
	BYTE								cbGiveUpStatus[GAME_PLAYER];		//��Ϸ״̬
	BYTE							    cbBalanceCount;						//ƽ�����
	BYTE                                cbChipPoolCount;                    //�߳�����
	BYTE								bGameId;
	//�˿���Ϣ
	BYTE								cbCenterCardData[MAX_CENTERCOUNT];	//�˿���Ŀ
	BYTE								cbCenterCount;	                     //�����˿���Ŀ
	BYTE								cbHandCardData[MAX_COUNT];			//�����˿�
	LONG                                lAddTakeGoldMin;                    //���Ӵ�������Сֵ
	LONG                                lAddTakeGoldMax;                    //���Ӵ��������ֵ
	LONG                                lUserTakeGold[GAME_PLAYER];         //��Ҵ�����
};

//��������Ϣ
struct CMD_S_Robot
{
	BYTE								BRobot;                             //�Ƿ��û�
	BYTE                                BAi;                                //������AI����
	BYTE								cbCardData[GAME_PLAYER][MAX_COUNT];			//����	
	BYTE								cbCenterCardData[MAX_CENTERCOUNT];	//�����˿�
	LONG                                clLbei[INT_ROBOT_BEI];              //��������	
	LONG							    lRobotAdd[INT_ROBOT_ADD];			//�����˼�ע��
	LONG							    lRobotQuanYa[INT_ROBOT_QUANYA];		//������ȫѺ��
	LONG								lRobotGiveUp[INT_ROBOT_GIVEUP];		//�����˷�����	
};

//�����˿�
struct CMD_S_GameStart
{
	WORD				 				wCurrentUser;						//��ǰ���
	WORD								wDUser;								//D���
	WORD								wSmallChipInUser;					//Сäע���	
	WORD								wMaxChipInUser;						//��äע���	
	LONG								lCellScore;							//��Ԫ��ע
	LONG								lTurnMaxScore[GAME_PLAYER];			//�����ע
	LONG								lUserMaxScore[GAME_PLAYER];			//���ע
	LONG								lTurnLessScore;						//��С��ע
	LONG								lAddLessScore;						//����Сע
	LONG								lAddSmall;						    //��Сäעϵ��
	LONG								lAddMax;						    //�Ӵ�äעϵ��
	LONG                                lTableScore[GAME_PLAYER];
	BYTE                                UserCount;                          //�������
	BYTE								cbCardData[MAX_COUNT];				//�û��˿�
	//BYTE								cbCardData[GAME_PLAYER][MAX_COUNT];	//�û��˿�
	//BYTE								cbAllData[GAME_PLAYER][MAX_COUNT];	//�û��˿�
	BYTE                                cbPlayStatus[GAME_PLAYER];
	BOOL                                cbShowHand[GAME_PLAYER];            //�������
};


//���õ�ע
struct CMD_S_Set_BaseScore
{
	BYTE                                bSetOrConfirm;                         //  0 δ���� 1��wPlayerID���� 2��wPlayerIDͬ��       
	WORD								wChairID;							//�������
	LONG                                lBaseScore;                         //��ע
	LONG                                lCellScore;                       //������С��ע����
	LONG                                lMaxCellScore;                    //���������ע����
};
struct CMD_S_KickOut
{
	BYTE                                bReasion;                     //����ԭ��
	WORD                                wChairID;                     //�������ID
};

//äע���
struct CMD_S_MangZhu
{
	BYTE                                 BPlayEndType;                 
};

//�û���ע
struct CMD_S_AddScore
{
	WORD								wCurrentUser;						//��ǰ�û�
	WORD								wAddScoreUser;						//��ע�û�
	LONG								lAddScoreCount;						//��ע��Ŀ
	LONG								lTurnLessScore;						//������ע
	LONG								lTurnMaxScore[GAME_PLAYER];			//�����ע
	LONG								lUserMaxScore[GAME_PLAYER];			//���ע
	LONG                                lTableScore[GAME_PLAYER];	        //��ǰ��ע
	LONG                                lTotalScore[GAME_PLAYER];	        //��ǰ��ע
	LONG								lAddLessScore;						//����Сע	
	BYTE                                cbChipTypeID;
	LONG                                lAllScore;
	BYTE                                cbBalanceCount;
};

//һ�ֽ���
struct CMD_S_TurnOver
{
	WORD								wAddScoreUser;						//��ע�û�
	LONG								lAddScoreCount;						//��ע��Ŀ
	LONG								lTurnLessScore;						//������ע
	LONG								lTurnMaxScore;						//�����ע(����ʣ�����)
	LONG								lAddLessScore;						//����Сע
	BYTE                                cbChipTypeID;
	LONG                                lAllScore;
	LONG                                lTableScore[GAME_PLAYER];	        //������ע
	LONG                                lTotalScore[GAME_PLAYER];	        //�ۼ���ע
	BYTE                                cbChipPoolCount;					//��������
	LONG                                aryChipPool[GAME_PLAYER + 1];		//�׳س���
	BYTE                                cbBalanceCount;
};

//�û�����
struct CMD_S_GiveUp
{
	WORD								wGiveUpUser;						//�����û�
	LONG								lLost;								//������	                           
	WORD                                wID;						        //�Ƿ���  
	WORD                                wLookUser;						    //�����û� 
	LONG                                LAllScore;                          //�׳س���
};

//�������ݰ�
struct CMD_S_SendCard
{
	BYTE								cbPublic;							//�Ƿ���
	WORD								wCurrentUser;						//��ǰ�û�
	BYTE								cbSendCardCount;					//������Ŀ
	BYTE								cbCenterCardData[MAX_CENTERCOUNT];	//�����˿�
	LONG                                LAllScore;
	LONG                                ChipPool[GAME_PLAYER+1];
	LONG                                lTableScore[GAME_PLAYER];
	LONG                                lTurnMaxScore;                      //��ǰ����ע���
};

//��Ϸ����
struct CMD_S_GameEnd
{
	BYTE								cbTotalEnd;							//ǿ�˱�־
	LONG								lGameTax[GAME_PLAYER];				//��Ϸ�����
	LONG								lGameScore[GAME_PLAYER];			//��Ϸ�÷�
	LONG								lChipScore[GAME_PLAYER];			//ʣ�����
	LONG								lTotalScore[GAME_PLAYER];			//�ۼ���ע
	LONG								lWinChipScore[GAME_PLAYER];			//Ӯ�õĳ���
	LONG								lChipPool[GAME_PLAYER+1];           //�߳س���
	BYTE								cbCardData[GAME_PLAYER][MAX_COUNT];	//�û��˿�	
	BYTE								cbCenterCardData[MAX_CENTERCOUNT];	//�����˿�
	BYTE								cbLastCenterCardData[GAME_PLAYER][MAX_CENTERCOUNT];//����˿�
	BYTE                                cbCardType[GAME_PLAYER];				//�������
	BYTE                                cbWinUserId[GAME_PLAYER][GAME_PLAYER];	//ÿһ�����뽫��˭���
	LONG								lChipPoolScore[GAME_PLAYER][GAME_PLAYER];//ÿ�������г������
	BYTE                                cbWinCount[GAME_PLAYER];			//ÿһ�����뱻�����˻��
	BYTE                                cbChipPoolCount;                    //�߳�����
	BYTE                                cbPlayStatus[GAME_PLAYER];          //��Ϸ״̬
	BYTE                                cbPlayGiveUp[GAME_PLAYER];          //����״̬
	LONG                                lAllScore;
};

//�û�����
struct CMD_S_LiangCard
{
	BYTE                                BLiangId;                       //�Ƿ�����
	WORD								wLiangUser;						//�����û�
	BYTE								cbCardData[MAX_COUNT];							//�����˿�	

};

//��Ϸ����
struct CMD_S_OpenCard
{
	BYTE                                wID;                              
	WORD								wWinUser;							//�û�����
};

//���Ӵ���
struct CMD_S_Add_TakeGold
{
	WORD             wChairID;                //���
	LONG             lAddTakeGold;            //���ӵĴ�������
	LONG             lAllTakeGold;            //������д�����
};
//����
struct CMD_S_DaShang
{
	WORD	wChairID;
	//LONG       lUserTakeGold[GAME_PLAYER];         //��Ҵ����ұ仯
	LONG	lUserScore;
};
////�û�����
//struct CMD_S_SitDown
//{
//	WORD								wChairID;							//�û�����
//};

//////////////////////////////////////////////////////////////////////////
//�ͻ�������ṹ
#define SUB_C_ADD_SCORE					1									//�û���ע
#define SUB_C_GIVE_UP					2									//������Ϣ
#define SUB_C_NOLOOK_CARD				6									//������Ϣ
#define SUB_C_LOOK_CARD					16									//������Ϣ
#define SUB_C_LOOKNO_CARD				17									//��������Ϣ
#define SUB_C_GAMEOVER				   12									//�Ƿ������Ϸ
#define SUB_C_START_ROB				   13									//�Ƿ�ʼ��ע

#define SUB_C_PLAY_END				   14									//��������
#define SUB_C_SET_BASESCORE			   15									//�û����õ�ע
#define SUB_C_ADD_TAKEGOLD             18                                   //������Ӵ���
#define SUB_C_DASHANG                  19                                   //����

#define SETBASE_WAIT                0
#define SETBASE_SET                 1
#define SETBASE_CONFIRM             2
#define SETBASE_OVER                3
#define SETBASE_NOTAGREE            4
//�û���ע
struct CMD_C_AddScore
{
	LONG								lScore;								//��ע��Ŀ
	BYTE                                ID;                                 
};

//�û���ע
struct CMD_C_PlatEnd
{
	BYTE                                ID;                                 
};
//�û����õ�ע
struct CMD_C_SetBaseScore
{
	BYTE                                bSetOrConfirm;                           //1 ���� 0 ȷ��
	WORD								bUserChairID;								//
	LONG                                lBaseScore;
};
//���Ӵ�����
struct CMD_C_Add_TakeGold
{
	LONG       lAddTakeGold;
};
//����
struct CMD_C_DaShang
{
	WORD    wChairID;
};
//////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif