
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
-- 加载低保
CREATE PROC GSP_GP_LoadBaseEnsure
AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 领取条件
	DECLARE @ScoreCondition AS BIGINT
	SELECT @ScoreCondition=StatusValue FROM SystemStatusInfo WHERE StatusName=N'SubsistenceAllowancesCondition'
	IF @ScoreCondition IS NULL SET @ScoreCondition=0

	-- 领取次数
	DECLARE @TakeTimes AS SMALLINT
	SELECT @TakeTimes=StatusValue FROM SystemStatusInfo WHERE StatusName=N'SubsistenceAllowancesNumber'
	IF @TakeTimes IS NULL SET @TakeTimes=0

	-- 领取数量
	DECLARE @ScoreAmount AS BIGINT
	SELECT @ScoreAmount=StatusValue FROM SystemStatusInfo WHERE StatusName=N'SubsistenceAllowancesGold'
	IF @ScoreAmount IS NULL SET @ScoreAmount=0

	-- 抛出数据
	SELECT @ScoreCondition AS ScoreCondition,@TakeTimes AS TakeTimes,@ScoreAmount AS ScoreAmount
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 领取低保
CREATE PROC GSP_GP_TakeBaseEnsure
	@dwUserID INT,								-- 用户 I D
	@strPassword NCHAR(32),						-- 用户密码
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NVARCHAR(32),					-- 机器标识
	@strNotifyContent NVARCHAR(127) OUTPUT		-- 提示内容
AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询用户
	IF not exists(SELECT * FROM AccountsInfo WHERE UserID=@dwUserID AND LogonPass=@strPassword)
	BEGIN
		SET @strNotifyContent = N'抱歉，你的用户信息不存在或者密码不正确！'
		return 1
	END

	-- 领取条件
	DECLARE @ScoreCondition AS BIGINT
	SELECT @ScoreCondition=StatusValue FROM SystemStatusInfo WHERE StatusName=N'SubsistenceAllowancesCondition'
	IF @ScoreCondition IS NULL SET @ScoreCondition=0

	-- 领取次数
	DECLARE @TakeTimes AS SMALLINT
	SELECT @TakeTimes=StatusValue FROM SystemStatusInfo WHERE StatusName=N'SubsistenceAllowancesNumber'
	IF @TakeTimes IS NULL SET @TakeTimes=0

	-- 领取数量
	DECLARE @ScoreAmount AS BIGINT
	SELECT @ScoreAmount=StatusValue FROM SystemStatusInfo WHERE StatusName=N'SubsistenceAllowancesGold'
	IF @ScoreAmount IS NULL SET @ScoreAmount=0

	-- 读取金币
	DECLARE @Score BIGINT
	DECLARE @InsureScore BIGINT
	SELECT @Score=Score,@InsureScore=InsureScore FROM QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
	IF @@rowcount = 0
	BEGIN
		-- 插入资料
		INSERT INTO QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo (UserID,Score,LastLogonIP, LastLogonMachine, RegisterIP, RegisterMachine)
		VALUES (@dwUserID, 0, @strClientIP, @strMachineID, @strClientIP, @strMachineID)

		-- 设置金币
		SELECT @Score=0,@InsureScore=0
	END

	-- 调整金币	
	SET @Score = @Score + @InsureScore

	-- 领取判断
	IF @Score >= @ScoreCondition
	BEGIN
		SET @strNotifyContent = N'您的游戏币不低于 '+CAST(@ScoreCondition AS NVARCHAR)+N'，不可领取！'	
		RETURN 1	
	END	

	-- 领取记录
	DECLARE @TodayDateID INT
	DECLARE @TodayTakeTimes INT		
	SET @TodayDateID=CAST(CAST(GetDate() AS FLOAT) AS INT)	
	SELECT @TodayTakeTimes=TakeTimes FROM AccountsBaseEnsure WHERE UserID=@dwUserID AND TakeDateID=@TodayDateID
	IF @TodayTakeTimes IS NULL SET @TodayTakeTimes=0	

	-- 次数判断
	IF @TodayTakeTimes >= @TakeTimes
	BEGIN
		SET @strNotifyContent = N'您今日已领取 '+CAST(@TodayTakeTimes AS NVARCHAR)+N' 次，领取失败！'
		return 3		
	END

	-- 更新记录
	IF @TodayTakeTimes=0
	BEGIN
		SET @TodayTakeTimes = 1
		INSERT INTO AccountsBaseEnsure(UserID,TakeDateID,TakeTimes) VALUES(@dwUserID,@TodayDateID,@TodayTakeTimes)		
	END ELSE
	BEGIN
		SET @TodayTakeTimes = @TodayTakeTimes+1
		UPDATE AccountsBaseEnsure SET TakeTimes = @TodayTakeTimes WHERE UserID = @dwUserID AND TakeDateID=@TodayDateID		
	END	

	-- 领取金币	
	UPDATE QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo SET Score = Score + @ScoreAmount WHERE UserID = @dwUserID

	-- 查询金币
	SELECT @Score=Score FROM QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo WHERE UserID = @dwUserID 	

	-- 输出提示
	SET @strNotifyContent = N'恭喜您，低保领取成功！您今日还可领取 '+CAST(@TakeTimes-@TodayTakeTimes AS NVARCHAR)+N' 次！'
	
	-- 抛出数据
	SELECT @Score AS Score	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------