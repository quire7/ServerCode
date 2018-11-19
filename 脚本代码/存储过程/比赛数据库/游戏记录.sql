
----------------------------------------------------------------------------------------------------

USE QPGameMatchDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_RecordDrawInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_RecordDrawInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_RecordDrawScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_RecordDrawScore]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ��Ϸ��¼
CREATE PROC GSP_GR_RecordDrawInfo

	-- ������Ϣ
	@wKindID INT,								-- ��Ϸ I D
	@wServerID INT,								-- ���� I D

	-- ������Ϣ
	@wTableID INT,								-- ���Ӻ���
	@wUserCount INT,							-- �û���Ŀ
	@wAndroidCount INT,							-- ������Ŀ

	-- ˰�����
	@lWasteCount BIGINT,						-- �����Ŀ
	@lRevenueCount BIGINT,						-- ��Ϸ˰��

	-- ͳ����Ϣ
	@dwUserMemal BIGINT,						-- �����Ŀ
	@dwPlayTimeCount INT,						-- ��Ϸʱ��

	-- ʱ����Ϣ
	@SystemTimeStart DATETIME,					-- ��ʼʱ��
	@SystemTimeConclude DATETIME				-- ����ʱ��

AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- �����¼
	INSERT RecordDrawInfo(KindID,ServerID,TableID,UserCount,AndroidCount,Waste,Revenue,UserMedal,StartTime,ConcludeTime)
	VALUES (@wKindID,@wServerID,@wTableID,@wUserCount,@wAndroidCount,@lWasteCount,@lRevenueCount,@dwUserMemal,@SystemTimeStart,@SystemTimeConclude)
	
	-- ��ȡ��¼
	SELECT SCOPE_IDENTITY() AS DrawID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��Ϸ��¼
CREATE PROC GSP_GR_RecordDrawScore

	-- ������Ϣ
	@dwDrawID INT,								-- ������ʶ
	@dwUserID INT,								-- �û���ʶ
	@wChairID INT,								-- ���Ӻ���

	-- �û���Ϣ
	@dwDBQuestID INT,							-- �����ʶ
	@dwInoutIndex INT,							-- ��������

	-- �ɼ���Ϣ
	@lScore BIGINT,								-- �û�����
	@lGrade BIGINT,								-- �û��ɼ�
	@lRevenue BIGINT,							-- �û�˰��
	@dwUserMedal INT,							-- ������Ŀ
	@dwPlayTimeCount INT						-- ��Ϸʱ��

AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- �����¼
	INSERT RecordDrawScore(DrawID,UserID,ChairID,Score,Grade,Revenue,UserMedal,PlayTimeCount,DBQuestID,InoutIndex)
	VALUES (@dwDrawID,@dwUserID,@wChairID,@lScore,@lGrade,@lRevenue,@dwUserMedal,@dwPlayTimeCount,@dwDBQuestID,@dwInoutIndex)
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
