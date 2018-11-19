USE QPGameMatchDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_MatchReward]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_MatchReward]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ��������
CREATE PROC GSP_GR_MatchReward
	@dwUserID INT,								-- �û� I D				
	@dwRewardGold INT,							-- �������
	@dwRewardIngot INT,							-- ��������
	@dwRewardExperience INT,					-- ��������
	@wKindID INT,								-- ��Ϸ I D
	@wServerID INT,								-- ���� I D
	@strClientIP NVARCHAR(15)					-- ���ӵ�ַ
AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- ���½��
	IF @dwRewardGold>0
	BEGIN
		UPDATE QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo SET Score=Score+@dwRewardGold WHERE UserID=@dwUserID
		IF @@ROWCOUNT=0
		BEGIN
			INSERT QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo (UserID,Score,LastLogonIP) 
			VALUES(@dwUserID,@dwRewardGold,@strClientIP)
		END
	END
	
	-- ����Ԫ���;���
	IF @dwRewardIngot>0 OR @dwRewardExperience>0
	BEGIN
		UPDATE QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo SET UserMedal=UserMedal+@dwRewardIngot,Experience=Experience+@dwRewardExperience 
		WHERE UserID=@dwUserID
		IF @@ROWCOUNT=0
		BEGIN
			RETURN 1
		END
	END	
	
	-- ��ѯ���
	DECLARE @CurrGold BIGINT	
	SELECT @CurrGold=Score FROM QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID

	-- ��ѯԪ���;���
	DECLARE @CurrIngot INT
	DECLARE @CurrExperience INT	
	SELECT @CurrIngot=UserMedal,@CurrExperience=Experience FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	-- ��������
	IF @CurrGold IS NULL SET @CurrGold=0	
	IF @CurrIngot IS NULL SET @CurrIngot=0
	IF @CurrExperience IS NULL SET @CurrExperience=0

	-- �׳�����
	SELECT @CurrGold AS Score,@CurrIngot AS Ingot,@CurrExperience AS Experience
END

RETURN 0
GO