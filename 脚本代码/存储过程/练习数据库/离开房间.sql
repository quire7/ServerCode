
----------------------------------------------------------------------------------------------------

USE QPEducateDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LeaveGameServer]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LeaveGameServer]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- �뿪����
CREATE PROC GSP_GR_LeaveGameServer

	-- �û���Ϣ
	@dwUserID INT,								-- �û� I D
	@dwOnLineTimeCount INT,						-- ����ʱ��

	-- ϵͳ��Ϣ
	@dwInoutIndex INT,							-- ��������
	@dwLeaveReason INT,							-- �뿪ԭ��

	-- ��¼�ɼ�
	@lRecordScore BIGINT,						-- �û�����
	@lRecordGrade BIGINT,						-- �û��ɼ�
	@lRecordInsure BIGINT,						-- �û�����
	@lRecordRevenue BIGINT,						-- ��Ϸ˰��
	@lRecordWinCount INT,						-- ʤ������
	@lRecordLostCount INT,						-- ʧ������
	@lRecordDrawCount INT,						-- �;�����
	@lRecordFleeCount INT,						-- ������Ŀ
	@lRecordUserMedal INT,						-- �û�����
	@lRecordExperience INT,						-- �û�����
	@lRecordLoveLiness INT,						-- �û�����
	@dwRecordPlayTimeCount INT,					-- ��Ϸʱ��

	-- ����ɼ�
	@lVariationScore BIGINT,					-- �û�����
	@lVariationGrade BIGINT,					-- �û��ɼ�
	@lVariationInsure BIGINT,					-- �û�����
	@lVariationRevenue BIGINT,					-- ��Ϸ˰��
	@lVariationWinCount INT,					-- ʤ������
	@lVariationLostCount INT,					-- ʧ������
	@lVariationDrawCount INT,					-- �;�����
	@lVariationFleeCount INT,					-- ������Ŀ
	@lVariationUserMedal INT,					-- �û�����
	@lVariationExperience INT,					-- �û�����
	@lVariationLoveLiness INT,					-- �û�����
	@dwVariationPlayTimeCount INT,				-- ��Ϸʱ��

	-- ������Ϣ
	@wKindID INT,								-- ��Ϸ I D
	@wServerID INT,								-- ���� I D
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineSerial NVARCHAR(32)				-- ������ʶ

AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	------------------------------------ �ݷֿ�ʼ --------------------------------------------
	DECLARE @ServerID INT -- (-3:�ⶥ����,-2:���ַ��乫������,-1����ҷ��乫������)
	DECLARE @PresentMember NVARCHAR(50)
	DECLARE @MaxDatePresent INT
	DECLARE @MaxPresent	INT
	DECLARE @CellPlayPresnet INT
	DECLARE @CellPlayTime INT
	DECLARE @StartPlayTime INT
	DECLARE @CellOnlinePresent INT
	DECLARE @CellOnlineTime INT
	DECLARE @StartOnlineTime INT
	DECLARE @IsPlayPresent TINYINT
	DECLARE @IsOnlinePresent TINYINT
	DECLARE @PresentScore INT
	SET @PresentScore=0
	DECLARE @DateID	INT
	SET @DateID = CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- �ݵ����
	SELECT @ServerID=ServerID,@PresentMember=PresentMember,@CellPlayPresnet=CellPlayPresnet,@CellPlayTime=CellPlayTime,@StartPlayTime=StartPlayTime,
		@CellOnlinePresent=CellOnlinePresent,@CellOnlineTime=CellOnlineTime,@StartOnlineTime=StartOnlineTime,
		@IsPlayPresent=IsPlayPresent,@IsOnlinePresent=IsOnlinePresent
	FROM QPPlatformDBLink.QPPlatformDB.dbo.GlobalPlayPresent WHERE ServerID=@wServerID
	IF @ServerID IS NULL
	BEGIN
		SELECT @ServerID=ServerID,@PresentMember=PresentMember,@CellPlayPresnet=CellPlayPresnet,@CellPlayTime=CellPlayTime,@StartPlayTime=StartPlayTime,
		@CellOnlinePresent=CellOnlinePresent,@CellOnlineTime=CellOnlineTime,@StartOnlineTime=StartOnlineTime,
		@IsPlayPresent=IsPlayPresent,@IsOnlinePresent=IsOnlinePresent
		FROM QPPlatformDBLink.QPPlatformDB.dbo.GlobalPlayPresent WHERE ServerID=-2
		IF @ServerID IS NULL
		BEGIN
			SET @IsPlayPresent=0
			SET @IsOnlinePresent=0
		END
	END

	-- �ⶥ����
	SELECT @MaxDatePresent=MaxDatePresent,@MaxPresent=MaxPresent
	FROM QPPlatformDBLink.QPPlatformDB.dbo.GlobalPlayPresent WHERE ServerID=-3
	IF @MaxDatePresent IS NULL
	BEGIN
		SET @MaxDatePresent=0
		SET @MaxPresent=0
	END

	-- ��Ϸʱ���ݷ�
	IF @IsPlayPresent=1
	BEGIN
		IF @dwRecordPlayTimeCount<>0 AND @CellPlayTime<>0
		BEGIN
			IF @dwRecordPlayTimeCount>=@StartPlayTime
			BEGIN
				SET @PresentScore=@PresentScore+(@dwRecordPlayTimeCount/@CellPlayTime)*@CellPlayPresnet
			END
		END		
	END
	
	-- ����ʱ���ݷ�
	IF @IsOnlinePresent=1
	BEGIN
		IF @dwOnLineTimeCount<>0 AND @CellOnlineTime<>0
		BEGIN
			IF @dwOnLineTimeCount>=@StartOnlineTime
			BEGIN
				SET @PresentScore=@PresentScore+(@dwOnLineTimeCount/@CellOnlineTime)*@CellOnlinePresent
			END
		END		
	END
	
	-- �ⶥ�ж�
	IF @MaxDatePresent>0
	BEGIN
		DECLARE @UserDatePresent INT
		SELECT @UserDatePresent=PresentScore FROM QPTreasureDBLink.QPTreasureDB.dbo.StreamPlayPresent WHERE DateID=@DateID AND UserID=@dwUserID
		IF @UserDatePresent IS NULL
		BEGIN
			SET @UserDatePresent=0
		END
		IF @UserDatePresent+@PresentScore>@MaxDatePresent
		BEGIN
			SET @PresentScore=@MaxDatePresent-@UserDatePresent
			IF @PresentScore<0
			BEGIN
				SET @PresentScore=0
			END
		END
	END
	IF @MaxPresent>0
	BEGIN
		DECLARE @UserPresent INT
		SELECT @UserPresent=SUM(PresentScore) FROM QPTreasureDBLink.QPTreasureDB.dbo.StreamPlayPresent WHERE UserID=@dwUserID
		IF @UserPresent IS NULL
		BEGIN
			SET @UserPresent=0
		END
		IF @UserPresent+@PresentScore>@MaxPresent
		BEGIN
			SET @PresentScore=@MaxPresent-@UserPresent
			IF @PresentScore<0
			BEGIN
				SET @PresentScore=0
			END
		END
	END	

	-- ���Ͷ����ж�
	DECLARE @MemberOrder TINYINT
    SELECT @MemberOrder=MemberOrder FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID
	IF CHARINDEX(LTRIM(STR(@MemberOrder)),@PresentMember)=0
	BEGIN
		SET @PresentScore=0
	END 

	-- �����˲��ݷ�
	IF @strClientIP=N'0.0.0.0'
	BEGIN
		SET @PresentScore=0
	END
	
	-- ����ͳ��
	IF @PresentScore>0
	BEGIN		
		-- �����û����
		UPDATE QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo set Score=Score+@PresentScore WHERE UserID=@dwUserID

		UPDATE QPTreasureDBLink.QPTreasureDB.dbo.StreamPlayPresent SET PresentCount=PresentCount+1,PresentScore=PresentScore+@PresentScore,
			PlayTimeCount=PlayTimeCount+@dwRecordPlayTimeCount,OnLineTimeCount=OnLineTimeCount+@dwOnLineTimeCount,
			LastDate=GETDATE()
		WHERE DateID=@DateID AND UserID=@dwUserID
		IF @@ROWCOUNT=0
		BEGIN
			INSERT QPTreasureDBLink.QPTreasureDB.dbo.StreamPlayPresent (DateID,UserID,PresentCount,PresentScore,PlayTimeCount,OnLineTimeCount)
			VALUES (@DateID,@dwUserID,1,@PresentScore,@dwRecordPlayTimeCount,@dwOnLineTimeCount)
		END
	END
	
	------------------------------------ �ݷֽ��� --------------------------------------------

	-- �û�����
	UPDATE GameScoreInfo SET Score=Score+@lVariationScore, WinCount=WinCount+@lVariationWinCount, LostCount=LostCount+@lVariationLostCount,
		DrawCount=DrawCount+@lVariationDrawCount, FleeCount=FleeCount+@lVariationFleeCount, PlayTimeCount=PlayTimeCount+@dwVariationPlayTimeCount,
		OnLineTimeCount=OnLineTimeCount+@dwOnLineTimeCount
	WHERE UserID=@dwUserID

	-- �������
	DELETE GameScoreLocker WHERE UserID=@dwUserID AND ServerID=@wServerID

	-- �뿪��¼
	UPDATE RecordUserInout SET LeaveTime=GetDate(), LeaveReason=@dwLeaveReason, LeaveMachine=@strMachineSerial, LeaveClientIP=@strClientIP,
		Score=@lRecordScore, Insure=@lRecordInsure, Revenue=@lRecordRevenue, WinCount=@lRecordWinCount, LostCount=@lRecordLostCount,
		DrawCount=@lRecordDrawCount, FleeCount=@lRecordFleeCount, UserMedal=@lRecordUserMedal, Experience=@lRecordExperience, LoveLiness=@lRecordLoveLiness,
		PlayTimeCount=@dwRecordPlayTimeCount, OnLineTimeCount=@dwOnLineTimeCount
	WHERE ID=@dwInoutIndex AND UserID=@dwUserID

	-- ȫ����Ϣ
	UPDATE QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo SET Experience=Experience+@lVariationExperience, LoveLiness=LoveLiness+@lVariationLoveLiness,
		UserMedal=UserMedal+@lVariationUserMedal,PlayTimeCount=PlayTimeCount+@dwRecordPlayTimeCount,OnLineTimeCount=OnLineTimeCount+@dwOnLineTimeCount
	WHERE UserID=@dwUserID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------