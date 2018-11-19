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

-- 比赛奖励
CREATE PROC GSP_GR_MatchReward
	@dwUserID INT,								-- 用户 I D				
	@dwRewardGold INT,							-- 奖励金币
	@dwRewardIngot INT,							-- 奖励金牌
	@dwRewardExperience INT,					-- 奖励经验
	@wKindID INT,								-- 游戏 I D
	@wServerID INT,								-- 房间 I D
	@strClientIP NVARCHAR(15)					-- 连接地址
AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	-- 更新金币
	IF @dwRewardGold>0
	BEGIN
		UPDATE QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo SET Score=Score+@dwRewardGold WHERE UserID=@dwUserID
		IF @@ROWCOUNT=0
		BEGIN
			INSERT QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo (UserID,Score,LastLogonIP) 
			VALUES(@dwUserID,@dwRewardGold,@strClientIP)
		END
	END
	
	-- 更新元宝和经验
	IF @dwRewardIngot>0 OR @dwRewardExperience>0
	BEGIN
		UPDATE QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo SET UserMedal=UserMedal+@dwRewardIngot,Experience=Experience+@dwRewardExperience 
		WHERE UserID=@dwUserID
		IF @@ROWCOUNT=0
		BEGIN
			RETURN 1
		END
	END	
	
	-- 查询金币
	DECLARE @CurrGold BIGINT	
	SELECT @CurrGold=Score FROM QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID

	-- 查询元宝和经验
	DECLARE @CurrIngot INT
	DECLARE @CurrExperience INT	
	SELECT @CurrIngot=UserMedal,@CurrExperience=Experience FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	-- 调整数据
	IF @CurrGold IS NULL SET @CurrGold=0	
	IF @CurrIngot IS NULL SET @CurrIngot=0
	IF @CurrExperience IS NULL SET @CurrExperience=0

	-- 抛出数据
	SELECT @CurrGold AS Score,@CurrIngot AS Ingot,@CurrExperience AS Experience
END

RETURN 0
GO