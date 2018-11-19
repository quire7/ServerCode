
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
-- ��ѯ�ȼ�
CREATE PROC GSP_GP_QueryGrowLevel
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����	
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@strUpgradeDescribe NVARCHAR(127) OUTPUT	-- �����Ϣ
AS

-- ��������
SET NOCOUNT ON

-- �Ƹ�����
DECLARE @Score BIGINT
DECLARE @Ingot BIGINT

-- ִ���߼�
BEGIN
	
	-- ��������
	DECLARE @Experience BIGINT
	DECLARE	@GrowlevelID INT	

	-- ��ѯ�û�
	SELECT @Experience=Experience,@GrowlevelID=GrowLevelID FROM AccountsInfo 
	WHERE UserID=@dwUserID AND LogonPass=@strPassword

	-- �����ж�
	IF @Experience IS NULL OR @GrowlevelID IS NULL
	BEGIN
		return 1
	END

	-- �����ж�
	DECLARE @NowGrowLevelID INT
	SELECT TOP 1 @NowGrowLevelID=LevelID FROM QPPlatformDBLink.QPPlatformDB.dbo.GrowLevelConfig
	WHERE @Experience>=Experience ORDER BY LevelID DESC

	-- ��������
	IF @NowGrowLevelID IS NULL
	BEGIN
		SET @NowGrowLevelID=@GrowlevelID														
	END

	-- ��������
	IF @NowGrowLevelID>@GrowlevelID
	BEGIN
		DECLARE @UpgradeLevelCount INT
		DECLARE	@RewardGold BIGINT
		DECLARE	@RewardIngot BIGINT
		
		-- ��������
		SET @UpgradeLevelCount=@NowGrowLevelID-@GrowlevelID
		
		-- ��ѯ����
		SELECT @RewardGold=SUM(RewardGold),@RewardIngot=SUM(RewardMedal) FROM QPPlatformDBLink.QPPlatformDB.dbo.GrowLevelConfig
		WHERE LevelID>@GrowlevelID AND LevelID<=@NowGrowLevelID

		-- ��������
		IF @RewardGold IS NULL SET @RewardGold=0				
		IF @RewardIngot IS NULL SET @RewardIngot=0

		-- ���½��
		UPDATE QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo SET Score=Score+@RewardGold WHERE UserID=@dwUserID
		IF @@rowcount = 0
		BEGIN
			-- ��������
			INSERT INTO QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo (UserID,Score,LastLogonIP, LastLogonMachine, RegisterIP, RegisterMachine)
			VALUES (@dwUserID, @RewardGold, @strClientIP, @strMachineID, @strClientIP, @strMachineID)		
		END		

		-- ���µȼ�
		UPDATE AccountsInfo SET UserMedal=UserMedal+@RewardIngot,GrowLevelID=@NowGrowLevelID WHERE UserID=@dwUserID

		-- ������ʾ
		SET @strUpgradeDescribe = N'��ϲ����Ϊ'+CAST(@NowGrowLevelID AS NVARCHAR)+N'����ϵͳ������Ϸ�� '+CAST(@RewardGold AS NVARCHAR)+N' ,Ԫ�� '+CAST(@RewardIngot AS NVARCHAR)

		-- ���ñ���
		SET @GrowlevelID=@NowGrowLevelID		
	END

	-- ��һ�ȼ�	
	DECLARE	@UpgradeRewardGold BIGINT
	DECLARE	@UpgradeRewardMedal BIGINT
	DECLARE @UpgradeExperience BIGINT	
	SELECT @UpgradeExperience=Experience,@UpgradeRewardGold=RewardGold,@UpgradeRewardMedal=RewardMedal FROM QPPlatformDBLink.QPPlatformDB.dbo.GrowLevelConfig
	WHERE LevelID=@GrowlevelID+1
	
	-- ��������
	IF @UpgradeExperience IS NULL SET @UpgradeExperience=0
	IF @UpgradeRewardGold IS NULL SET @UpgradeRewardGold=0
	IF @UpgradeRewardMedal IS NULL SET @UpgradeRewardMedal=0

	-- ��ѯ��Ϸ��
	SELECT @Score=Score FROM QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
	
	-- ��ѯԪ��	
	SELECT @Ingot=UserMedal FROM AccountsInfo WHERE UserID=@dwUserID

	-- �׳�����
	SELECT @GrowlevelID AS CurrLevelID,@Experience AS Experience,@UpgradeExperience AS UpgradeExperience, @UpgradeRewardGold AS RewardGold,
		   @UpgradeRewardMedal AS RewardMedal,@Score AS Score,@Ingot AS Ingot
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------