#ifndef GAME_SERVICE_STRUCT_HEAD_FILE
#define GAME_SERVICE_STRUCT_HEAD_FILE

#include <AfxWin.h>

#include "..\..\ȫ�ֶ���\DataStream.h"

#pragma pack(1)


#pragma pack()
//////////////////////////////////////////////////////////////////////////////////
//�û�һ����Ӯ;
struct tagPrivateRandRecordChild
{
	DWORD						dwKindID;
	DWORD						dwVersion;
	int							iRecordID;
	int							iRecordChildID;
	std::vector<int>			kScore;
	SYSTEMTIME					kPlayTime;
	DataStream					kRecordGame;
	std::string					kUserDefine;
	tagPrivateRandRecordChild()
	{
		dwKindID = 0;
		dwVersion = 0;
		iRecordID = 0;
		iRecordChildID = 0;

		kScore.clear();
		kRecordGame.clear();
		kUserDefine.clear();
	}

	tagPrivateRandRecordChild(const tagPrivateRandRecordChild& refInfo)
	{
		dwKindID = refInfo.dwKindID;
		dwVersion = refInfo.dwVersion;
		iRecordID = refInfo.iRecordID;
		iRecordChildID = refInfo.iRecordChildID;
		kScore.assign(refInfo.kScore.begin(), refInfo.kScore.end());
		kPlayTime = refInfo.kPlayTime;
		kRecordGame.assign(refInfo.kRecordGame.begin(), refInfo.kRecordGame.end());
		kUserDefine = refInfo.kUserDefine;
	}

	tagPrivateRandRecordChild& operator=(const tagPrivateRandRecordChild& refInfo)
	{
		if ( this == &refInfo )
		{
			return *this;
		}
		dwKindID = refInfo.dwKindID;
		dwVersion = refInfo.dwVersion;
		iRecordID = refInfo.iRecordID;
		iRecordChildID = refInfo.iRecordChildID;
		kScore.assign(refInfo.kScore.begin(), refInfo.kScore.end());
		kPlayTime = refInfo.kPlayTime;
		kRecordGame.assign(refInfo.kRecordGame.begin(), refInfo.kRecordGame.end());
		kUserDefine = refInfo.kUserDefine;

		return *this;
	}

	void StreamValue(DataStream& kData, bool bSend)
	{
		Stream_VALUE(dwKindID);
		Stream_VALUE(dwVersion);
		Stream_VALUE(iRecordID);
		Stream_VALUE(iRecordChildID);
		StructVecotr(int, kScore);
		Stream_VALUE_SYSTEMTIME(kPlayTime);
		Stream_VALUE(kRecordGame);
		Stream_VALUE(kUserDefine);
	}
};

//�û�һ������Ӯ;
struct tagPrivateRandTotalRecord
{
	tagPrivateRandTotalRecord()
	{
		dwKindID = 0;
		dwVersion = 0;
		iRoomNum = 0;
		iRecordID = 0;

		kScore.clear();
		kUserID.clear();
		kNickName.clear();

		kUserDefine.clear();
	}
	DWORD						dwKindID;
	DWORD						dwVersion;
	int							iRoomNum;
	int							iRecordID;
	std::vector<int>			kScore;
	std::vector<int>			kUserID;
	std::vector<std::string>	kNickName;
	SYSTEMTIME					kPlayTime;
	std::string					kUserDefine;

	std::vector<tagPrivateRandRecordChild>	kRecordChild;
	
	tagPrivateRandTotalRecord(const tagPrivateRandTotalRecord& refInfo)
	{
		dwKindID = refInfo.dwKindID;
		dwVersion = refInfo.dwVersion;
		iRoomNum = refInfo.iRoomNum;
		iRecordID = refInfo.iRecordID;

		kScore.assign(refInfo.kScore.begin(), refInfo.kScore.end());
		kUserID.assign(refInfo.kUserID.begin(), refInfo.kUserID.end());
		kNickName.assign(refInfo.kNickName.begin(), refInfo.kNickName.end());

		kPlayTime = refInfo.kPlayTime;
		kUserDefine = refInfo.kUserDefine;

		kRecordChild.assign(refInfo.kRecordChild.begin(), refInfo.kRecordChild.end());
	}
	tagPrivateRandTotalRecord& operator=(const tagPrivateRandTotalRecord& refInfo)
	{
		if ( this == &refInfo )
		{
			return *this;
		}

		dwKindID = refInfo.dwKindID;
		dwVersion = refInfo.dwVersion;
		iRoomNum = refInfo.iRoomNum;
		iRecordID = refInfo.iRecordID;

		kScore.assign(refInfo.kScore.begin(), refInfo.kScore.end());
		kUserID.assign(refInfo.kUserID.begin(), refInfo.kUserID.end());
		kNickName.assign(refInfo.kNickName.begin(), refInfo.kNickName.end());

		kPlayTime = refInfo.kPlayTime;
		kUserDefine = refInfo.kUserDefine;

		kRecordChild.assign(refInfo.kRecordChild.begin(), refInfo.kRecordChild.end());
		return *this;
	}


	void StreamValue(DataStream& kData, bool bSend)
	{
		Stream_VALUE(dwKindID);
		Stream_VALUE(dwVersion);
		Stream_VALUE(iRoomNum);
		Stream_VALUE(iRecordID);
		StructVecotr(int, kScore);
		StructVecotr(int, kUserID);
		Stream_VALUE(kNickName);
		Stream_VALUE_SYSTEMTIME(kPlayTime);
		Stream_VALUE(kUserDefine);
		StructVecotrMember(tagPrivateRandRecordChild,kRecordChild);
	}
};

struct tagPrivateRandTotalRecordList
{
	DWORD		dwUserID;
	std::vector<tagPrivateRandTotalRecord> kList;

	tagPrivateRandTotalRecordList()
	{
		dwUserID = 0;
		kList.clear();
	}

	tagPrivateRandTotalRecordList(const tagPrivateRandTotalRecordList& refInfo)
	{
		dwUserID = refInfo.dwUserID;
		kList.assign(refInfo.kList.begin(), refInfo.kList.end());
	}

	tagPrivateRandTotalRecordList& operator=(const tagPrivateRandTotalRecordList& refInfo)
	{
		if ( this == &refInfo)
		{
			return *this;
		}
		dwUserID = refInfo.dwUserID;
		kList.assign(refInfo.kList.begin(), refInfo.kList.end());
		return *this;
	}

	void StreamValue(DataStream& kData, bool bSend)
	{
		Stream_VALUE(dwUserID);
		StructVecotrMember(tagPrivateRandTotalRecord,kList);
	}
};

// ��Ϸ¼��;
struct  tagGameRecordPlayer
{
	DWORD					dwUserID;			//�û�ID;
	DWORD					dwGameID;			//��Ϸ��ʶ;
	SCORE					lScore;				//��ǰ����;
	std::string				strHead;			//ͷ��url;
	std::string				strNickName;		//�ǳ�;
	std::vector<BYTE>		cbCardData;			//��ֵ����;

	tagGameRecordPlayer()
	{
		dwUserID = 0;
		dwGameID = 0;
		lScore = 0;
		strHead.clear();
		strNickName.clear();
		cbCardData.clear();
	}

	tagGameRecordPlayer(const tagGameRecordPlayer& refInfo)
	{
		dwUserID = refInfo.dwUserID;
		dwGameID = refInfo.dwGameID;
		lScore = refInfo.lScore;
		strHead = refInfo.strHead;
		strNickName = refInfo.strNickName;
		cbCardData.assign( refInfo.cbCardData.begin(), refInfo.cbCardData.end() );
	}

	tagGameRecordPlayer& operator=(const tagGameRecordPlayer& refInfo)
	{
		if ( this == &refInfo )
		{
			return *this;
		}

		dwUserID = refInfo.dwUserID;
		dwGameID = refInfo.dwGameID;
		lScore = refInfo.lScore;
		strHead = refInfo.strHead;
		strNickName = refInfo.strNickName;
		cbCardData.assign(refInfo.cbCardData.begin(), refInfo.cbCardData.end());

		return *this;
	}

	void StreamValue(DataStream& kData, bool bSend)
	{
		Stream_VALUE(dwUserID);
		Stream_VALUE(dwGameID);
		Stream_VALUE(lScore);
		Stream_VALUE(strHead);
		Stream_VALUE(strNickName);
		Stream_VALUE(cbCardData);
	}
};

struct  tagGameRecordOperateResult
{
	WORD			wSubCmdID;			//��Ϣ����;
	DataStream		subMessageData;		//��Ϣ����;

	tagGameRecordOperateResult()
	{
		wSubCmdID = 0;
		subMessageData.clear();
	}
	tagGameRecordOperateResult(const tagGameRecordOperateResult& refInfo)
	{
		wSubCmdID = refInfo.wSubCmdID;
		subMessageData.assign(refInfo.subMessageData.begin(), refInfo.subMessageData.end());
	}

	tagGameRecordOperateResult& operator=(const tagGameRecordOperateResult& refInfo)
	{
		if ( this == &refInfo )
		{
			return *this;
		}

		wSubCmdID = refInfo.wSubCmdID;
		subMessageData.assign(refInfo.subMessageData.begin(), refInfo.subMessageData.end());
		return *this;
	}

	void StreamValue(DataStream& kData, bool bSend)
	{
		Stream_VALUE(wSubCmdID);
		Stream_VALUE(subMessageData);
	}
};

//˽�˳�������Ϣ;
struct CMD_GF_Private_Room_Info
{
	BYTE			bPlayCoutIdex;		//��Ҿ���
	BYTE			bGameTypeIdex;		//��Ϸ����
	DWORD			bGameRuleIdex;		//��Ϸ����

	BYTE			bStartGame;
	DWORD			dwPlayCout;			//��Ϸ����
	DWORD			dwRoomNum;
	DWORD			dwCreateUserID;
	DWORD			dwPlayTotal;		//�ܾ���

	BYTE			cbRoomType;

	std::vector<int>	kWinLoseScore;

	CMD_GF_Private_Room_Info()
	{
		bPlayCoutIdex = 0;
		bGameTypeIdex = 0;
		bGameRuleIdex = 0;
		
		bStartGame = 0;
		dwPlayCout = 0;
		dwRoomNum = 0;
		dwCreateUserID = 0;
		dwPlayTotal = 0;
		cbRoomType = 0;

		kWinLoseScore.clear();
	}

	CMD_GF_Private_Room_Info(const CMD_GF_Private_Room_Info& refInfo)
	{
		bPlayCoutIdex = refInfo.bPlayCoutIdex;
		bGameTypeIdex = refInfo.bGameTypeIdex;
		bGameRuleIdex = refInfo.bGameRuleIdex;

		bStartGame	= refInfo.bStartGame;
		dwPlayCout	= refInfo.dwPlayCout;
		dwRoomNum	= refInfo.dwRoomNum;
		dwCreateUserID	= refInfo.dwCreateUserID;
		dwPlayTotal		= refInfo.dwPlayTotal;

		cbRoomType		= refInfo.cbRoomType;

		kWinLoseScore.assign(refInfo.kWinLoseScore.begin(), refInfo.kWinLoseScore.end());
	}

	CMD_GF_Private_Room_Info& operator=(const CMD_GF_Private_Room_Info& refInfo)
	{
		if ( this == &refInfo)
		{
			return *this;
		}
		bPlayCoutIdex = refInfo.bPlayCoutIdex;
		bGameTypeIdex = refInfo.bGameTypeIdex;
		bGameRuleIdex = refInfo.bGameRuleIdex;

		bStartGame = refInfo.bStartGame;
		dwPlayCout = refInfo.dwPlayCout;
		dwRoomNum = refInfo.dwRoomNum;
		dwCreateUserID = refInfo.dwCreateUserID;
		dwPlayTotal = refInfo.dwPlayTotal;

		cbRoomType = refInfo.cbRoomType;

		kWinLoseScore.assign(refInfo.kWinLoseScore.begin(), refInfo.kWinLoseScore.end());
		return *this;
	}

	void StreamValue(DataStream& kData, bool bSend)
	{
		Stream_VALUE(bPlayCoutIdex);
		Stream_VALUE(bGameTypeIdex);
		Stream_VALUE(bGameRuleIdex);
		Stream_VALUE(bStartGame);
		Stream_VALUE(dwPlayCout);
		Stream_VALUE(dwRoomNum);
		Stream_VALUE(dwCreateUserID);
		Stream_VALUE(dwPlayTotal);
		Stream_VALUE(cbRoomType);
		StructVecotr(int, kWinLoseScore);
	}
};

struct  tagGameRecord
{
	WORD									wBankerUser;		//ׯ���û�;
	WORD									wEastUser;			//�״ζ���;
	CMD_GF_Private_Room_Info				roomInfo;			//��Ϸ������Ϣ;
	std::vector<tagGameRecordPlayer>		playersVec;			//ÿ����ҵĻ�����Ϣ;
	std::vector<tagGameRecordOperateResult> actionVec;			//�û�����;

	tagGameRecord()
	{
		wBankerUser = 0;
		wEastUser	= 0;

		playersVec.clear();
		actionVec.clear();
	}

	tagGameRecord(const tagGameRecord& refGameRecord)
	{
		wBankerUser = refGameRecord.wBankerUser;
		wEastUser	= refGameRecord.wEastUser;

		roomInfo	= refGameRecord.roomInfo;

		playersVec.assign(refGameRecord.playersVec.begin(), refGameRecord.playersVec.end());
		actionVec.assign(refGameRecord.actionVec.begin(), refGameRecord.actionVec.end());
	}

	bool StreamValue(DataStream& kData, bool bSend)
	{
		try
		{
			Stream_VALUE(wBankerUser);
			Stream_VALUE(wEastUser);
			roomInfo.StreamValue(kData, bSend);
			StructVecotrMember(tagGameRecordPlayer, playersVec);
			StructVecotrMember(tagGameRecordOperateResult, actionVec);

			return true;
		}
		catch (...)
		{
			return false;
		}
	}
};


#endif