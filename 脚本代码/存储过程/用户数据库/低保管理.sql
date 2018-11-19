
----------------------------------------------------------------------------------------------------

USE QPAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadBaseEnsure]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadBaseEnsure]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_TakeBaseEnsure]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_TakeBaseEnsure]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------
-- ���صͱ�
CREATE PROC GSP_GP_LoadBaseEnsure
AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ȡ����
	DECLARE @ScoreCondition AS BIGINT
	SELECT @ScoreCondition=StatusValue FROM SystemStatusInfo WHERE StatusName=N'SubsistenceAllowancesCondition'
	IF @ScoreCondition IS NULL SET @ScoreCondition=0

	-- ��ȡ����
	DECLARE @TakeTimes AS SMALLINT
	SELECT @TakeTimes=StatusValue FROM SystemStatusInfo WHERE StatusName=N'SubsistenceAllowancesNumber'
	IF @TakeTimes IS NULL SET @TakeTimes=0

	-- ��ȡ����
	DECLARE @ScoreAmount AS BIGINT
	SELECT @ScoreAmount=StatusValue FROM SystemStatusInfo WHERE StatusName=N'SubsistenceAllowancesGold'
	IF @ScoreAmount IS NULL SET @ScoreAmount=0

	-- �׳�����
	SELECT @ScoreCondition AS ScoreCondition,@TakeTimes AS TakeTimes,@ScoreAmount AS ScoreAmount
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��ȡ�ͱ�
CREATE PROC GSP_GP_TakeBaseEnsure
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@strNotifyContent NVARCHAR(127) OUTPUT		-- ��ʾ����
AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	IF not exists(SELECT * FROM AccountsInfo WHERE UserID=@dwUserID AND LogonPass=@strPassword)
	BEGIN
		SET @strNotifyContent = N'��Ǹ������û���Ϣ�����ڻ������벻��ȷ��'
		return 1
	END

	-- ��ȡ����
	DECLARE @ScoreCondition AS BIGINT
	SELECT @ScoreCondition=StatusValue FROM SystemStatusInfo WHERE StatusName=N'SubsistenceAllowancesCondition'
	IF @ScoreCondition IS NULL SET @ScoreCondition=0

	-- ��ȡ����
	DECLARE @TakeTimes AS SMALLINT
	SELECT @TakeTimes=StatusValue FROM SystemStatusInfo WHERE StatusName=N'SubsistenceAllowancesNumber'
	IF @TakeTimes IS NULL SET @TakeTimes=0

	-- ��ȡ����
	DECLARE @ScoreAmount AS BIGINT
	SELECT @ScoreAmount=StatusValue FROM SystemStatusInfo WHERE StatusName=N'SubsistenceAllowancesGold'
	IF @ScoreAmount IS NULL SET @ScoreAmount=0

	-- ��ȡ���
	DECLARE @Score BIGINT
	DECLARE @InsureScore BIGINT
	SELECT @Score=Score,@InsureScore=InsureScore FROM QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
	IF @@rowcount = 0
	BEGIN
		-- ��������
		INSERT INTO QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo (UserID,Score,LastLogonIP, LastLogonMachine, RegisterIP, RegisterMachine)
		VALUES (@dwUserID, 0, @strClientIP, @strMachineID, @strClientIP, @strMachineID)

		-- ���ý��
		SELECT @Score=0,@InsureScore=0
	END

	-- �������	
	SET @Score = @Score + @InsureScore

	-- ��ȡ�ж�
	IF @Score >= @ScoreCondition
	BEGIN
		SET @strNotifyContent = N'������Ϸ�Ҳ����� '+CAST(@ScoreCondition AS NVARCHAR)+N'��������ȡ��'	
		RETURN 1	
	END	

	-- ��ȡ��¼
	DECLARE @TodayDateID INT
	DECLARE @TodayTakeTimes INT		
	SET @TodayDateID=CAST(CAST(GetDate() AS FLOAT) AS INT)	
	SELECT @TodayTakeTimes=TakeTimes FROM AccountsBaseEnsure WHERE UserID=@dwUserID AND TakeDateID=@TodayDateID
	IF @TodayTakeTimes IS NULL SET @TodayTakeTimes=0	

	-- �����ж�
	IF @TodayTakeTimes >= @TakeTimes
	BEGIN
		SET @strNotifyContent = N'����������ȡ '+CAST(@TodayTakeTimes AS NVARCHAR)+N' �Σ���ȡʧ�ܣ�'
		return 3		
	END

	-- ���¼�¼
	IF @TodayTakeTimes=0
	BEGIN
		SET @TodayTakeTimes = 1
		INSERT INTO AccountsBaseEnsure(UserID,TakeDateID,TakeTimes) VALUES(@dwUserID,@TodayDateID,@TodayTakeTimes)		
	END ELSE
	BEGIN
		SET @TodayTakeTimes = @TodayTakeTimes+1
		UPDATE AccountsBaseEnsure SET TakeTimes = @TodayTakeTimes WHERE UserID = @dwUserID AND TakeDateID=@TodayDateID		
	END	

	-- ��ȡ���	
	UPDATE QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo SET Score = Score + @ScoreAmount WHERE UserID = @dwUserID

	-- ��ѯ���
	SELECT @Score=Score FROM QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo WHERE UserID = @dwUserID 	

	-- �����ʾ
	SET @strNotifyContent = N'��ϲ�����ͱ���ȡ�ɹ��������ջ�����ȡ '+CAST(@TakeTimes-@TodayTakeTimes AS NVARCHAR)+N' �Σ�'
	
	-- �׳�����
	SELECT @Score AS Score	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------