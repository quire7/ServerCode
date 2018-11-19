#ifndef CMD_SPARROW_HEAD_FILE
#define CMD_SPARROW_HEAD_FILE

#pragma pack(push)  
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////////////////////
//�����궨��
#define KIND_ID						302										//��Ϸ I D
#define GAME_PLAYER					4										//��Ϸ����
#define GAME_NAME					TEXT("�Ͼ��齫��԰��")					//��Ϸ����

#define VERSION_SERVER				PROCESS_VERSION(6,0,3)					//����汾
#define VERSION_CLIENT				PROCESS_VERSION(6,0,3)					//����汾

//////////////////////////////////////////////////////////////////////////////////////////////////

//��������
#define MAX_WEAVE					4											//������
#define MAX_INDEX					42											//�������������
#define MAX_NORMAL					31											//�ǻ����������
#define MAX_COUNT					14											//�����Ŀ
#define MAX_HUA_COUNT				20											//���������Ŀ
#define MAX_REPERTORY				144											//�����

//�����������
#define MAX_TING_OUT				14											//���ɴ������
#define MAX_TING_COUNT				9											//�����9����

//���Ʒ�λ
#define SEND_CARD_FRONT				0											//��ǰ�淢��
#define SEND_CARD_BACK				1											//�Ӻ��淢��

//////////////////////////////////////////////////////////////////////////////////////////////////
//��ϷС��Ե��
#define PAY_MONEY_MINGGANG			0											//����
#define PAY_MONEY_ANGANG			1											//����
#define PAY_MONEY_HUAGANG			2											//����
#define	PAY_MONEY_SILIANFA			3											//������(�ĸ�������һ������)
#define PAY_MONEY_MEISIZHANG		4											//ù����(һ�˳�����һ������)
#define PAY_MONEY_SHOUSIFENG		5											//���ķ�(ǰ���Ŷ����ϡ��������򱱡������ϡ���)

//С��ʱ��������
#define BASE_HUA_COUNT_MGANG		10											//���ܣ�����
#define BASE_HUA_COUNT_AGANG		5											//���ܣ�����
#define BASE_HUA_COUNT_BGANG		10											//���ܣ�����

#define BASE_HUA_COUNT_HGANG		10											//���ܣ�����
#define BASE_HUA_COUNT_FAFEN		10											//���֣�����

#define BASE_HUA_COUNT_AGANG_WJZ	5											//���ܣ�����
#define BASE_HUA_COUNT_JIANGFEN_WJZ	5											//���֣�����
#define BASE_HUA_COUNT_JIANGFEN_BX	10											//���֣�����

#define BASE_BAO_ZI_SCORE			100											//���ӻ���;
//////////////////////////////////////////////////////////////////////////////////////////////////
//��Ϸ�齱
#define MAX_LOTTERY_COUNT			5											//��ѡ�齱�������
//////////////////////////////////////////////////////////////////////////////////////////////////
//��ʱ������
#define IDI_BASESCORE				1											//��ע���ö�ʱ��					
#define IDI_SHOOT_DICE				2											//�����Ӷ�ʱ��
#define IDI_DISPATCH_CARD			3											//���Ƽ�ʱ��
#define IDI_SEND_CARD				4											//���Ƽ�ʱ��
#define IDI_SEND_BUHUA				5											//������ʱ��
#define IDI_OUT_CARD				6											//���Ƽ�ʱ��
#define IDI_USER_OPERATE			7											//������ʱ��
#define IDI_USER_OFFLINE			10											//������ʱ�йܼ�ʱ��
//��ʱ��ʱ��
#define IDT_BASESCORE               25											//���õ�עʱ��
#define IDT_SHOOT_DICE				1											//������ʱ��
#define IDT_DISPATCH_CARD			10											//����ʱ��
#define IDT_SEND_CARD				2											//����ʱ��
#define IDT_SEND_BUHUA				2											//����ʱ��
#define IDT_OUT_CARD				10											//����ʱ��
#define IDT_USER_OPERATE			20											//����ʱ��
#define IDT_USER_TRUST				1											//�й�ʱ��
#define IDT_USER_TING				1											//����ʱ��
#define IDT_USER_OFFLINE			15											//������ʱ�й�ʱ��
//��ʱ������
#define IDS_TIMER_NORMAL			0											//����״̬
#define	IDS_TIMER_TRUST				1											//�й�״̬
#define IDS_TIMER_TING				2											//����״̬

//��ע���
#define STATUS_SBS_ERROR			0
#define STATUS_SBS_SET				1
#define STATUS_SBS_WAIT				2
#define STATUS_SBS_NOTIFY			3
#define ACTION_SBS_SET				4
#define ACTION_SBS_NOSET			5
#define ACTION_SBS_AGREE			6
#define ACTION_SBS_DISAGREE			7
//�ͻ��˷��������ñ���  ��ע�ͽ�ҵı���
#define  GOLD_RATE                  400

//////////////////////////////////////////////////////////////////////////////////////////////////
//�������
struct tagWeaveItem
{
	DWORD							dwWeaveKind;								//�������
	BYTE							cbCenterCard;								//������
	BYTE							cbPublicCard;								//������־
	WORD							wProvideUser;								//��Ӧ�û�
};

struct tagTingPaiItemData;
//��������
struct tagTingPaiItem
{
	BYTE							cbOutCardData;								//���ȥ����
	BYTE							cbTingCardCount;							//������Ŀ
	BYTE							cbTingCardData[MAX_TING_COUNT];				//��������
	BYTE							cbLeftCardCount[MAX_TING_COUNT];			//ʣ����Ŀ
	DWORD							dwHuCardFanShu[MAX_TING_COUNT];				//���Ʒ���

	inline tagTingPaiItem& operator=(const tagTingPaiItemData& TingPaiItemData);
};
//���ƽ��
struct tagGangPaiResult
{
	BYTE							cbCardCount;								//����Ŀ
	BYTE							cbCardData[4];								//������
	DWORD                           dwGangType[4];								//��������
};

struct tagTingPaiDataStream;
//���ƽ��
struct tagTingPaiResult
{
	BYTE							cbTingPaiItemCount;							//�ɴ�����Ŀ
	tagTingPaiItem					TingPaiItemArrary[MAX_TING_OUT];			//����������

	tagTingPaiResult()
	{
		memset(this, 0, sizeof(tagTingPaiResult));
	}

	inline tagTingPaiResult(tagTingPaiDataStream& refTingPaiDataStream);
};

//��������
struct tagTingPaiItemData
{
	BYTE							cbOutCardData;								//���ȥ����
	BYTE							cbTingCardCount;							//������Ŀ
	std::vector<BYTE>				cbTingCardData;								//��������;
	std::vector<BYTE>				cbLeftCardCount;							//ʣ����Ŀ;
	std::vector<DWORD>				dwHuCardFanShu;								//���Ʒ���;

	tagTingPaiItemData()
	{
		cbOutCardData = 0;
		cbTingCardCount = 0;

		cbTingCardData.clear();
		cbLeftCardCount.clear();
		dwHuCardFanShu.clear();
	}

	tagTingPaiItemData(const tagTingPaiItemData& TingPaiItem)
	{
		cbOutCardData = TingPaiItem.cbOutCardData;
		cbTingCardCount = TingPaiItem.cbTingCardCount;

		cbTingCardData.assign(TingPaiItem.cbTingCardData.begin(), TingPaiItem.cbTingCardData.end());
		cbLeftCardCount.assign(TingPaiItem.cbLeftCardCount.begin(), TingPaiItem.cbLeftCardCount.end());
		dwHuCardFanShu.assign(TingPaiItem.dwHuCardFanShu.begin(), TingPaiItem.dwHuCardFanShu.end());
	}

	tagTingPaiItemData(const tagTingPaiItem& TingPaiItem)
	{
		cbOutCardData = TingPaiItem.cbOutCardData;
		cbTingCardCount = TingPaiItem.cbTingCardCount;

		cbTingCardData.assign(TingPaiItem.cbTingCardData, TingPaiItem.cbTingCardData + cbTingCardCount);
		cbLeftCardCount.assign(TingPaiItem.cbLeftCardCount, TingPaiItem.cbLeftCardCount + cbTingCardCount);
		dwHuCardFanShu.assign(TingPaiItem.dwHuCardFanShu, TingPaiItem.dwHuCardFanShu + cbTingCardCount);
	}

	tagTingPaiItemData& operator=(const tagTingPaiItemData& refInfo)
	{
		if (this == &refInfo)
		{
			return *this;
		}

		cbOutCardData = refInfo.cbOutCardData;
		cbTingCardCount = refInfo.cbTingCardCount;

		cbTingCardData.assign(refInfo.cbTingCardData.begin(), refInfo.cbTingCardData.end());
		cbLeftCardCount.assign(refInfo.cbLeftCardCount.begin(), refInfo.cbLeftCardCount.end());
		dwHuCardFanShu.assign(refInfo.dwHuCardFanShu.begin(), refInfo.dwHuCardFanShu.end());

		return *this;
	}

	bool StreamValue(DataStream& kData, bool bSend)
	{
		try
		{
			Stream_VALUE(cbOutCardData);
			Stream_VALUE(cbTingCardCount);

			StructVecotr(BYTE, cbTingCardData);
			StructVecotr(BYTE, cbLeftCardCount);
			StructVecotr(DWORD, dwHuCardFanShu);

			return true;
		}
		catch (...)
		{
			return false;
		}
	}
};

// ���ƽ��;
struct tagTingPaiDataStream
{
	BYTE							cbTingPaiItemCount;							//�ɴ�����Ŀ
	DataStream						kdataStream;								//������;

	tagTingPaiDataStream()
	{
		cbTingPaiItemCount = 0;
		kdataStream.clear();
	}

	tagTingPaiDataStream(const tagTingPaiDataStream& TingPaiResult)
	{
		cbTingPaiItemCount = TingPaiResult.cbTingPaiItemCount;
		kdataStream.assign(TingPaiResult.kdataStream.begin(), TingPaiResult.kdataStream.end());
	}

	tagTingPaiDataStream(const tagTingPaiResult& TingPaiResult)
	{
		cbTingPaiItemCount = TingPaiResult.cbTingPaiItemCount;

		for (size_t i = 0; i < cbTingPaiItemCount; i++)
		{
			tagTingPaiItemData tmpItem(TingPaiResult.TingPaiItemArrary[i]);
			tmpItem.StreamValue(kdataStream, true);
		}
	}

	tagTingPaiDataStream& operator=(const tagTingPaiDataStream& refInfo)
	{
		if (this == &refInfo)
		{
			return *this;
		}

		cbTingPaiItemCount = refInfo.cbTingPaiItemCount;
		kdataStream.assign(refInfo.kdataStream.begin(), refInfo.kdataStream.end());

		return *this;
	}

	bool StreamValue(DataStream& kData, bool bSend)
	{
		try
		{
			Stream_VALUE(cbTingPaiItemCount);
			Stream_VALUE(kdataStream);

			return true;
		}
		catch (...)
		{
			return false;
		}
	}
};

inline tagTingPaiItem& tagTingPaiItem::operator = (const tagTingPaiItemData& TingPaiItemData)
{
	cbOutCardData = TingPaiItemData.cbOutCardData;
	cbTingCardCount = TingPaiItemData.cbTingCardCount;


	if (cbTingCardCount > 0)
	{
		memcpy(cbTingCardData, &TingPaiItemData.cbTingCardData[0], cbTingCardCount*sizeof(BYTE));
		memcpy(cbLeftCardCount, &TingPaiItemData.cbLeftCardCount[0], cbTingCardCount*sizeof(BYTE));
		memcpy(dwHuCardFanShu, &TingPaiItemData.dwHuCardFanShu[0], cbTingCardCount*sizeof(DWORD));
	}
	return *this;
}

inline tagTingPaiResult::tagTingPaiResult(tagTingPaiDataStream& refTingPaiDataStream)
{
	cbTingPaiItemCount = refTingPaiDataStream.cbTingPaiItemCount;
	for (BYTE i = 0; i < cbTingPaiItemCount; ++i)
	{
		tagTingPaiItemData	tempTingPaiItemData;
		tempTingPaiItemData.StreamValue(refTingPaiDataStream.kdataStream, false);
		TingPaiItemArrary[i] = tempTingPaiItemData;
	}
}

//���ƽ��
struct tagHuPaiResult
{
	DWORD							dwHuPaiKind;								//��������
	DWORD							dwHuPaiFanShu;								//������Ŀ
};
//�齱�ṹ����
struct tagLotteryPrizeResult
{
	BYTE							cbPrizeType;								//��Ʒ����
	LONG							lPrizeCount;								//��Ʒ����
};

//////////////////////////////////////////////////////////////////////////////////////////////////
//��Ϸ״̬����
#define GS_MJ_FREE					GAME_STATUS_FREE							//����״̬
#define GS_MJ_DICE					(GAME_STATUS_PLAY+1)						//������״̬
#define GS_MJ_PLAY					(GAME_STATUS_PLAY+2)						//��Ϸ״̬

//����״̬
struct CMD_S_StatusFree
{
	bool							bCurBiXiaHu;								//���ֱ��º�
	LONG							lBaseScore;									//�����ע
	LONG                            lCellScore;									//������С��ע����
	LONG                            lMaxCellScore;								//���������ע����
	BYTE                            cbSBSStatus;								//0 δ���� 1��wPlayerID���� 2��wPlayerIDͬ��    

};
//������״̬
struct CMD_S_StatusDice
{
	LONG							lBaseScore;									//�����ע
	bool							bCurBiXiaHu;								//���ֱ��º�
	WORD							wBankerUser;								//ׯ���û�
	WORD							wEastUser;									//�����û�
};
//��Ϸ״̬
struct CMD_S_StatusPlay
{
	//��Ϸ����
	LONG							lBaseScore;									//�����ע
	bool							bCurBiXiaHu;								//���ֱ��º�
	WORD							wSiceCount;									//���ӵ���
	WORD							wBankerUser;								//ׯ���û�
	WORD							wCurrentUser;								//��ǰ�û�
	WORD							wEastUser;									//�����û�

	//״̬����
	BYTE							cbLeftCardCount;							//ʣ����Ŀ
	bool							bUserTrust[GAME_PLAYER];					//�Ƿ��й�
	bool							bUserTing[GAME_PLAYER];						//�Ƿ�����
	LONG							lTiaoZhanScore;								//��ս�÷�

	//������Ϣ
	WORD							wOutCardUser;								//�����û�
	BYTE							cbOutCardData;								//������
	BYTE							cbDiscardCount[GAME_PLAYER];				//������Ŀ
	BYTE							cbDiscardCard[GAME_PLAYER][72];				//������¼

	//������
	BYTE							cbCardCount[GAME_PLAYER];					//����Ŀ
	BYTE							cbCardData[MAX_COUNT];						//���б�
	BYTE							cbSendCardData;								//������
	BYTE							cbHuaPaiCount[GAME_PLAYER];					//������Ŀ
	BYTE							cbHuaPaiData[GAME_PLAYER][MAX_HUA_COUNT];	//��������

	//�����
	BYTE							cbWeaveCount[GAME_PLAYER];					//�����Ŀ
	tagWeaveItem					WeaveItemArray[GAME_PLAYER][MAX_WEAVE];		//�����

	LONG							lOutCardScore[GAME_PLAYER][2];				//��ҳ��Ƶ÷� 0:���� 1:����
	LONG							lGangScore[GAME_PLAYER];					//��Ҹܷ�
};

//////////////////////////////////////////////////////////////////////////////////////////////////
//����������ṹ
#define SUB_S_SET_BASESCORE			100											//���õ�ע
#define SUB_S_KICK_OUT				101											//�߳����
#define SUB_S_SHOOT_DICE			102											//������
#define SUB_S_GAME_START			103											//��Ϸ��ʼ
#define SUB_S_SEND_BUHUA			104											//��Ҳ���
#define SUB_S_SEND_CARD				105											//������
#define SUB_S_OUT_CARD				106											//��������
#define SUB_S_OPERATE_NOTIFY		107											//������ʾ
#define SUB_S_OPERATE_RESULT		108											//��������
#define SUB_S_PAY_MONEY				109											//��Ϸ�����еĽ���
#define SUB_S_GAME_END				110											//��Ϸ����
#define SUB_S_GAME_LOTTERY			111											//��Ϸ�齱
#define SUB_S_USER_TRUST			112											//�û��й�
#define SUB_S_SPECIAL_LIGHT			113											//�������;
#define SUB_S_MASTER_LEFTCARD		114											//ʣ���ƶ�

//���õ�ע
struct CMD_S_SetBaseScore
{
	WORD							wChairID;									//�������
	LONG                            lBaseScore;									//��ע
	LONG                            lCellScore;									//������С��ע����
	LONG                            lMaxCellScore;								//���������ע����
	BYTE                            cbSBSStatus;								//0 δ���� 1��wPlayerID���� 2��wPlayerIDͬ��       
};
//�߳��û�
struct CMD_S_KickOut
{
	WORD                            wChairID;									//�������ID
	BYTE                            cbReasion;									//����ԭ��
};

//������
struct CMD_S_ShootDice
{
	bool							bCurBiXiaHu;								//���ֱ��º�
	WORD							wBankerUser;								//ׯ���û�
	WORD							wEastUser;									//�����û�
};
//��Ϸ��ʼ
struct CMD_S_GameStart
{
	WORD							wSiceCount;									//���ӵ���
	BYTE							cbCardData[MAX_COUNT];						//���б�
};
//������
struct CMD_S_SendCard
{
	WORD							wSendCardUser;								//�����û�
	BYTE							cbCardData;									//������
	BYTE							cbDirection;								//��ǰ(��)��
};
//��Ҳ���
struct CMD_S_SendBuHua
{
	WORD							wBuHuaUser;									//�����û�
	bool							bFirstBuHua;								//���ֲ���
	BYTE							cbBuHuaCount;								//������Ŀ
	BYTE							cbBuHuaData[MAX_COUNT];						//��������
	BYTE							cbReplaceCardData[MAX_COUNT];				//�滻����
};
//��������
struct CMD_S_OutCard
{
	WORD							wOutCardUser;								//�����û�
	BYTE							cbOutCardData;								//������
};
//������ʾ
struct CMD_S_OperateNotify_Send
{
	WORD							wOperateUser;								//�������
	WORD							wCurrentUser;								//��ǰ�û�
	DWORD							dwActionMask;								//��������
	BYTE							cbActionCard;								//������
	tagGangPaiResult                GangPaiResult;								//���Ʋ���

	void StreamValue(DataStream& kData, bool bSend)
	{
		if (bSend)
		{
			kData.pushValue(this, sizeof(CMD_S_OperateNotify_Send));
		}
		else
		{
			kData.popValue(this, sizeof(CMD_S_OperateNotify_Send));
		}
	}
};

//��������
struct CMD_S_OperateResult
{
	WORD							wOperateUser;								//�����û�
	WORD							wProvideUser;								//��Ӧ�û�
	DWORD							dwOperateCode;								//��������
	BYTE							cbOperateCard;								//������
	LONG							lTiaoZhanScore;								//��ս�÷�
};
//��ϷС��
struct CMD_S_PayMoney
{
	BYTE							cbPayReason;								//֧��Ե��
	WORD							wPayToUser;									//��Ǯ���
	LONG							lPayToNumber;								//��Ǯ��Ŀ
	WORD							wPayFromUser[GAME_PLAYER-1];				//��Ǯ���
	LONG							lPayFromNumber[GAME_PLAYER-1];				//֧������
};
//��Ϸ����
struct CMD_S_GameEnd
{
	//������Ϣ
	BYTE							cbGameEndType;								//��Ϸ�������� 0:����,1:��ׯ,2:����,3:��ɢ
	WORD							wBankerUser;								//ׯ���û�
	WORD							wEscapeUser;								//�������
	WORD							wProvideUser;								//��Ӧ�û�
	BYTE							cbProvideCard;								//��Ӧ��
	bool							bHaveBiXiaHu;								//�Ƿ���º�
	LONG							lCellScore;									//�����ע

	//������Ϣ
	LONG							lGameScore[GAME_PLAYER];					//��Ϸ����
	LONG							lHuScore[GAME_PLAYER];						//���Ʒ�;
	LONG							lGangScore[GAME_PLAYER];					//�ܷ�;	
	LONG							lOutScore[GAME_PLAYER];						//���Ʒ�;
	LONG							lBaoZiScore[GAME_PLAYER];					//���Ʒ�;

	LONG							lTiaoZhanScore[GAME_PLAYER];				//��ս�÷�
	LONG							lTingPaiScore[GAME_PLAYER];					//��������
	BYTE							cbHuaPaiCount[GAME_PLAYER][2];				//��������
	BYTE							cbHuCardData;								//������

	//����Ϣ
	BYTE							cbCardCount[GAME_PLAYER];					//����Ŀ
	BYTE							cbCardData[GAME_PLAYER][MAX_COUNT];			//������

	//�����
	BYTE							cbWeaveItemCount[GAME_PLAYER];				//�����Ŀ
	tagWeaveItem					WeaveItemArray[GAME_PLAYER][MAX_WEAVE];		//�����
	tagHuPaiResult					stHuPaiResult[GAME_PLAYER];					//���ƽ��

	void StreamValue(DataStream& kData, bool bSend)
	{
		if (bSend)
		{
			kData.pushValue(this, sizeof(CMD_S_GameEnd));
		}
		else
		{
			kData.popValue(this, sizeof(CMD_S_GameEnd));
		}
	}
};

//�û��й�
struct CMD_S_UserTrust
{
	bool							bTrust;										//�Ƿ��й�
	WORD							wChairID;									//�й��û�
};

//////////////////////////////////////////////////////////////////////////////////////////////////
//�ͻ�������ṹ
#define SUB_C_SET_BASESCORE			1											//�û����õ�ע
#define	SUB_C_SHOOT_DICE			2											//������
#define SUB_C_DISPATCH_OVER			3											//�������
#define SUB_C_SENDCARD_OVER			4											//���ƶ������
#define SUB_C_BUHUA_OVER			5											//�����������
#define SUB_C_OUT_CARD				6											//��������
#define SUB_C_OPERATE_CARD			7											//������
#define SUB_C_GAME_LOTTERY			8											//��Ϸ�齱
#define SUB_C_USER_TRUST			9											//�û��й�
#define SUB_C_MASTER_CHEAKCARD		10											//ѡ�����
#define SUB_C_MASTER_LEFTCARD		11											//ʣ���ƶ�

//�û����õ�ע
struct CMD_C_SetBaseScore
{
	LONG                            lBaseScore;									//��ע
	BYTE							cbSBSAction;								//1���� 0ȷ��
};
//��������
struct CMD_C_OutCard
{
	BYTE							cbCardData;									//������
};
//��������
struct CMD_C_OperateCard
{
	DWORD							dwOperateCode;								//��������
	BYTE							cbOperateCard;								//������
};
//��Ϸ�齱
struct CMD_C_GameLottery
{
	BYTE							cbLotteryIndex;								//�齱ʱ�����ѡ��ƬID
};
//�û��й�
struct CMD_C_UserTrust
{
	bool							bTrust;										//�Ƿ��й�	
};

struct CMD_C_MaterCheckCard
{
	BYTE							cbCheakCard;						//��������
};

struct MasterLeftCard
{
	BYTE      kMasterCheakCard;										// ��ǰѡ��;
	BYTE      kMasterLeftIndex[MAX_INDEX];							// �Ŷ�;
};

//////////////////////////////////////////////////////////////////////////
// ˽�˳���¼����(����ܳ���MAX_PRIVATE_ACTION)
enum PRIVATE_Action
{
	PA_ZIMO = 0,		//��������
	PA_ZHUOPAO,			//���ڴ���
	PA_DIANPAO,			//���ڴ���
	PA_ANGANG,			//���ܴ���
	PA_MINGGANG,		//���ܴ���
	PA_WIN,				//ʤ��
	PA_LOSE,			//ʧ��
	PA_ALLSCORE,		//�ܳɼ�
};

// ��Ϸ��ɢ�ṹ
struct CMD_S_Private_End_Info
{
	WORD		wMainChairID;				// ����
	BYTE		cbZiMoCout[GAME_PLAYER];	// ��������
	BYTE		cbZhuoPaoCout[GAME_PLAYER];	// ���ڴ���
	BYTE		cbDianPaoCout[GAME_PLAYER];	// ���ڴ���
	BYTE		cbAnGang[GAME_PLAYER];		// ���ܴ���
	BYTE		cbMingGang[GAME_PLAYER];	// ���ܴ���
	BYTE		cbWinCout[GAME_PLAYER];		// ʤ��
	BYTE		cbLoseCout[GAME_PLAYER];	// ʧ��
	SCORE		lBaoZiScore[GAME_PLAYER];	// �������;
	SCORE	    lAllScore[GAME_PLAYER];		// �ܳɼ�
	SCORE		lMaxScore[GAME_PLAYER];		// ��ߵ÷�

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

// ��Ϸ����(1<<0 �ϴη���AAʹ�ã�����Ϸ����ʹ��);
enum eNJMJRuleEnum
{
	eNJMJRuleEnum_AA = 1,						// ����AA;
	eNJMJRuleEnum_JYZ = (1 << 1),				// 0 ��԰�� 1 ����ͷ;
	eNJMJRuleEnum_BAO = (1 << 2),				// 0 ����� 1 �����;
};

#pragma pack(pop)

//////////////////////////////////////////////////////////////////////////////////////////////////
extern void MyLog( TCHAR *szLog, ... );
#endif