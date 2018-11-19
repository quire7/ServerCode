
----------------------------------------------------------------------------------------------------

USE QPPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadTaskList]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadTaskList]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_QueryTaskInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_QueryTaskInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_TaskTake]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_TaskTake]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_TaskForward]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_TaskForward]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_TaskReward]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_TaskReward]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ��������
CREATE PROC GSP_GR_LoadTaskList
AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ����
	SELECT * FROM TaskInfo	

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��ѯ����
CREATE PROC GSP_GR_QueryTaskInfo
	wKindID INT,								-- ���� I D
	dwUserID INT,								-- �û� I D
	strPassword NCHAR(32),						-- �û�����
	OUT strErrorDescribe NVARCHAR(127) 			-- �����Ϣ

AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- ��ѯ�û�
	IF not exists(SELECT * FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID)
	BEGIN
		SET @strErrorDescribe = N'��Ǹ������û���Ϣ�����ڻ������벻��ȷ��'
		return 1
	END

	-- ��ʱ����
	UPDATE QPAccountsDBLink.QPAccountsDB.dbo.AccountsTask SET TaskStatus=2 
	WHERE UserID=@dwUserID AND TaskStatus=0 AND TimeLimit<DateDiff(s,InputDate,GetDate())

	-- ��ѯ����
	SELECT TaskID,TaskStatus,Progress,(TimeLimit-DateDiff(ss,InputDate,GetDate())) AS ResidueTime FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsTask 
	WHERE UserID=@dwUserID AND DateDiff(d,InputDate,GetDate())=0 AND (((@wKindID=KindID) AND TaskStatus=0) OR @wKindID=0) 		
END

RETURN 0

GO
----------------------------------------------------------------------------------------------------

CREATE  PROCEDURE `GSP_GR_TaskTake`
	(dwUserID INT,								-- �û� I D
	wTaskID  INT,								-- ���� I D
	strPassword NCHAR(32),						-- �û�����
	strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	strMachineID NVARCHAR(32),					-- ������ʶ
	OUT strErrorDescribe NVARCHAR(127),				-- �����Ϣ
	out nRet INT)
label_pro:BEGIN

	-- ��ѯ�û�
	DECLARE nMemberOrder INT;

	-- �ж���Ŀ	
	DECLARE TaskTakeMaxCount INT;
	
	-- ͳ������
	DECLARE TaskTakeCount INT;

	-- �������
	DECLARE nKindID INT;
	DECLARE nUserType INT;
	DECLARE nTimeLimit INT;
	DECLARE nTaskType INT;
	DECLARE nTaskObject INT;
	
	SELECT MemberOrder INTO nMemberOrder FROM QPAccountsDB.AccountsInfo 
	WHERE UserID=dwUserID AND LogonPass=strPassword;
	IF nMemberOrder IS NULL then
		SET strErrorDescribe = '��Ǹ������û���Ϣ�����ڻ������벻��ȷ��';
		SET nRet = 1;
		leave label_pro;
	END if;

	-- �ظ���ȡ
	IF exists(SELECT * FROM QPAccountsDB.AccountsTask 
	WHERE UserID=dwUserID AND TaskID=wTaskID AND TIMESTAMPDIFF(DAY,InputDate,CURRENT_TIMESTAMP())=0) 
	THEN
		SET strErrorDescribe = '��Ǹ��ͬһ������ÿ��ֻ����ȡһ�Σ�';
		SET nRet = 3;
		leave label_pro;
	END IF;

	-- �������
	SELECT KindID,UserType,TimeLimit,TaskType,Innings INTO nKindID,nUserType,nTimeLimit,nTaskType,nTaskObject
	FROM TaskInfo WHERE TaskID=wTaskID;
	IF nKindID IS NULL THEN
		SET strErrorDescribe = '��Ǹ��ϵͳδ�ҵ�����ȡ��������Ϣ��';
		SET nRet = 4;
		leave label_pro;
	END IF;

	-- ��ͨ���
	IF nMemberOrder=0 AND (nUserType&0x01)=0 THEN
		SET strErrorDescribe = '��Ǹ����������ʱ������ͨ��ҿ��ţ�';
		SET nRet = 5;
		leave label_pro;
	END IF;

	-- ��Ա���
	IF nMemberOrder>0 AND (nUserType&0x02)=0 THEN
		SET strErrorDescribe = '��Ǹ����������ʱ���Ի�Ա��ҿ��ţ�';
		SET nRet = 6;
		leave label_pro;
	END	IF;

	-- ��������
	INSERT INTO QPAccountsDB.AccountsTask(UserID,TaskID,TaskType,TaskObject,KindID,TimeLimit) 
	VALUES(dwUserID,wTaskID,TaskType,TaskObject,KindID,TimeLimit);

	-- �ɹ���ʾ
	SET strErrorDescribe = '��ϲ����������ȡ�ɹ���';
END

----------------------------------------------------------------------------------------------------

-- �����ƽ�
CREATE PROC GSP_GR_TaskForward
	@dwUserID INT,								-- �û� I D
	@wKindID INT,								-- ��Ϸ��ʶ
	@wMatchID INT,								-- ������ʶ
	@lWinCount INT,								-- Ӯ�־���
	@lLostCount INT,							-- ��־���
	@lDrawCount	INT								-- �;־���
AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ����	
	SELECT ID,TaskType,TaskObject,TimeLimit,TaskStatus,InputDate,(case 
									  when TaskType=0x01 then Progress+@lWinCount
									  when TaskType=0x02 then Progress+@lWinCount+@lLostCount+@lDrawCount
									  when TaskType=0x04 then @lWinCount
									  else Progress end) AS NewProgress
	INTO #TempTaskInfo FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsTask 
	WHERE UserID=@dwUserID AND KindID=@wKindID AND TaskStatus=0 AND DateDiff(d,InputDate,GetDate())=0

	-- ����״̬�����������ʤδʤ����ʱ��
	UPDATE #TempTaskInfo SET TaskStatus=(case										 
										 when TaskType=0x04 AND NewProgress=0 then 2
										 when TimeLimit<DateDiff(s,InputDate,GetDate()) then 2
										 when NewProgress>=TaskObject then 1
										 else 0 end)
	-- ���������
	UPDATE QPAccountsDBLink.QPAccountsDB.dbo.AccountsTask SET a.Progress=b.NewProgress,a.TaskStatus=b.TaskStatus 
	FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsTask AS a,#TempTaskInfo AS b WHERE a.ID=b.ID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- ��ȡ����
CREATE PROC GSP_GR_TaskReward
	@dwUserID INT,								-- �û� I D
	@wTaskID  INT,								-- ���� I D
	@strPassword NCHAR(32),						-- �û�����
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	DECLARE @MemberOrder INT
	SELECT @MemberOrder=MemberOrder FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo 
	WHERE UserID=@dwUserID AND LogonPass=@strPassword
	IF @MemberOrder IS NULL
	BEGIN
		SET @strErrorDescribe = N'��Ǹ������û���Ϣ�����ڻ������벻��ȷ��'
		return 1
	END

	-- ��ѯ����
	DECLARE @TaskInputDate DATETIME
	SELECT @TaskInputDate=InputDate FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsTask 
	WHERE UserID=@dwUserID AND TaskID=@wTaskID AND TaskStatus=1 AND DateDiff(d,InputDate,GetDate())=0
	IF @TaskInputDate IS NULL
	BEGIN
		SET @strErrorDescribe = N'����ɵ�ǰ������������ȡ������'
		return 2		
	END		

	-- ��ѯ����
	DECLARE @StandardAwardGold INT
	DECLARE @StandardAwardIngot INT
	DECLARE @MemberAwardGold INT
	DECLARE @MemberAwardIngot INT
	SELECT @StandardAwardGold=StandardAwardGold,@StandardAwardIngot=StandardAwardMedal,
	@MemberAwardGold=MemberAwardGold,@MemberAwardIngot=MemberAwardMedal	
	FROM TaskInfo WHERE TaskID=@wTaskID

	-- ��������
	IF @StandardAwardGold IS NULL SET @StandardAwardGold=0	
	IF @StandardAwardIngot IS NULL SET @StandardAwardIngot=0
	IF @MemberAwardGold IS NULL SET @MemberAwardGold=0
	IF @MemberAwardIngot IS NULL SET @MemberAwardIngot=0

	-- ִ�н���
	DECLARE @AwardGold INT
	DECLARE @AwardIngot INT
	IF @MemberOrder=0
	BEGIN
		SELECT @AwardGold=@StandardAwardGold,@AwardIngot=@StandardAwardIngot
	END ELSE
	BEGIN
		SELECT @AwardGold=@MemberAwardGold,@AwardIngot=@MemberAwardIngot	
	END

	-- ����Ԫ��
	UPDATE QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo SET UserMedal=UserMedal+@AwardIngot 
	WHERE UserID=@dwUserID	
		
	-- ���½��
	UPDATE QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo SET Score=Score+@AwardGold 
	WHERE UserID=@dwUserID

	-- ��ѯ����
	DECLARE @UserScore BIGINT
	DECLARE @UserIngot BIGINT
	
	-- ��ѯ���
	SELECT @UserScore=Score FROM  QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo 
	WHERE UserID=@dwUserID				

	-- ��ѯԪ��
	SELECT @UserIngot=UserMedal FROM  QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo 
	WHERE UserID=@dwUserID
	
	-- ��������
	IF @UserScore IS NULL SET @UserScore=0		
	IF @UserIngot IS NULL SET @UserIngot=0

	-- ɾ������	
	UPDATE QPAccountsDBLink.QPAccountsDB.dbo.AccountsTask SET TaskStatus=3
	WHERE UserID=@dwUserID AND TaskID=@wTaskID AND DateDiff(d,InputDate,GetDate())=0
	
	-- �����¼
	INSERT QPRecordDBLink.QPRecordDB.dbo.RecordTask(DateID,UserID,TaskID,AwardGold,AwardMedal,InputDate)
	VALUES (CAST(CAST(@TaskInputDate AS FLOAT) AS INT),@dwUserID,@wTaskID,@AwardGold,@AwardIngot,GetDate())

	-- �ɹ���ʾ
	SET @strErrorDescribe = N'��ϲ����������ȡ�ɹ���'

	-- �׳�����
	SELECT @UserScore AS Score,@UserIngot AS Ingot
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------