
----------------------------------------------------------------------------------------------------

USE QPAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_QueryGrowLevel]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_QueryGrowLevel]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------
-- 查询等级
CREATE PROC GSP_GP_QueryGrowLevel
	@dwUserID INT,								-- 用户 I D
	@strPassword NCHAR(32),						-- 用户密码	
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NVARCHAR(32),					-- 机器标识
	@strUpgradeDescribe NVARCHAR(127) OUTPUT	-- 输出信息
AS

-- 属性设置
SET NOCOUNT ON

-- 财富变量
DECLARE @Score BIGINT
DECLARE @Ingot BIGINT

-- 执行逻辑
BEGIN
	
	-- 变量定义
	DECLARE @Experience BIGINT
	DECLARE	@GrowlevelID INT	

	-- 查询用户
	SELECT @Experience=Experience,@GrowlevelID=GrowLevelID FROM AccountsInfo 
	WHERE UserID=@dwUserID AND LogonPass=@strPassword

	-- 存在判断
	IF @Experience IS NULL OR @GrowlevelID IS NULL
	BEGIN
		return 1
	END

	-- 升级判断
	DECLARE @NowGrowLevelID INT
	SELECT TOP 1 @NowGrowLevelID=LevelID FROM QPPlatformDBLink.QPPlatformDB.dbo.GrowLevelConfig
	WHERE @Experience>=Experience ORDER BY LevelID DESC

	-- 调整变量
	IF @NowGrowLevelID IS NULL
	BEGIN
		SET @NowGrowLevelID=@GrowlevelID														
	END

	-- 升级处理
	IF @NowGrowLevelID>@GrowlevelID
	BEGIN
		DECLARE @UpgradeLevelCount INT
		DECLARE	@RewardGold BIGINT
		DECLARE	@RewardIngot BIGINT
		
		-- 升级增量
		SET @UpgradeLevelCount=@NowGrowLevelID-@GrowlevelID
		
		-- 查询奖励
		SELECT @RewardGold=SUM(RewardGold),@RewardIngot=SUM(RewardMedal) FROM QPPlatformDBLink.QPPlatformDB.dbo.GrowLevelConfig
		WHERE LevelID>@GrowlevelID AND LevelID<=@NowGrowLevelID

		-- 调整变量
		IF @RewardGold IS NULL SET @RewardGold=0				
		IF @RewardIngot IS NULL SET @RewardIngot=0

		-- 更新金币
		UPDATE QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo SET Score=Score+@RewardGold WHERE UserID=@dwUserID
		IF @@rowcount = 0
		BEGIN
			-- 插入资料
			INSERT INTO QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo (UserID,Score,LastLogonIP, LastLogonMachine, RegisterIP, RegisterMachine)
			VALUES (@dwUserID, @RewardGold, @strClientIP, @strMachineID, @strClientIP, @strMachineID)		
		END		

		-- 更新等级
		UPDATE AccountsInfo SET UserMedal=UserMedal+@RewardIngot,GrowLevelID=@NowGrowLevelID WHERE UserID=@dwUserID

		-- 升级提示
		SET @strUpgradeDescribe = N'恭喜您升为'+CAST(@NowGrowLevelID AS NVARCHAR)+N'级，系统奖励游戏币 '+CAST(@RewardGold AS NVARCHAR)+N' ,元宝 '+CAST(@RewardIngot AS NVARCHAR)

		-- 设置变量
		SET @GrowlevelID=@NowGrowLevelID		
	END

	-- 下一等级	
	DECLARE	@UpgradeRewardGold BIGINT
	DECLARE	@UpgradeRewardMedal BIGINT
	DECLARE @UpgradeExperience BIGINT	
	SELECT @UpgradeExperience=Experience,@UpgradeRewardGold=RewardGold,@UpgradeRewardMedal=RewardMedal FROM QPPlatformDBLink.QPPlatformDB.dbo.GrowLevelConfig
	WHERE LevelID=@GrowlevelID+1
	
	-- 调整变量
	IF @UpgradeExperience IS NULL SET @UpgradeExperience=0
	IF @UpgradeRewardGold IS NULL SET @UpgradeRewardGold=0
	IF @UpgradeRewardMedal IS NULL SET @UpgradeRewardMedal=0

	-- 查询游戏币
	SELECT @Score=Score FROM QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
	
	-- 查询元宝	
	SELECT @Ingot=UserMedal FROM AccountsInfo WHERE UserID=@dwUserID

	-- 抛出数据
	SELECT @GrowlevelID AS CurrLevelID,@Experience AS Experience,@UpgradeExperience AS UpgradeExperience, @UpgradeRewardGold AS RewardGold,
		   @UpgradeRewardMedal AS RewardMedal,@Score AS Score,@Ingot AS Ingot
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------