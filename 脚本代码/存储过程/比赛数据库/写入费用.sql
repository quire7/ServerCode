USE QPGameMatchDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UserMatchFee]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UserMatchFee]

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UserMatchQuit]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UserMatchQuit]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 扣除费用
CREATE PROC GSP_GR_UserMatchFee
	@dwUserID INT,								-- 用户 I D
	@lMatchFee BIGINT,							-- 报名费用
	@wKindID INT,								-- 游戏 I D
	@wServerID INT,								-- 房间 I D
	@strClientIP NVARCHAR(15),					-- 连接地址
	@dwMatchID	INT,							-- 比赛 I D
	@dwMatchNo	INT,							-- 比赛场次
	@strMachineID NVARCHAR(32),					-- 机器标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN	
	-- 比赛信息
	DECLARE @cbMatchFeeType TINYINT
	DECLARE @cbMemberOrder TINYINT		
	DECLARE @InitialScore BIGINT
	SELECT @cbMemberOrder=a.MemberOrder,@cbMatchFeeType=a.MatchFeeType,@InitialScore=b.InitialScore FROM MatchPublic(NOLOCK) a,MatchImmediate b
	WHERE a.MatchID=@dwMatchID AND a.MatchID=b.MatchID
	IF @cbMemberOrder IS NULL OR @cbMatchFeeType IS NULL
	BEGIN
		SET @strErrorDescribe = N'抱歉,您报名的比赛不存在！'
		return 1		
	END
	
	-- 用户信息
	DECLARE @IsAndroidUser TINYINT
	DECLARE @UserMemberOrder TINYINT
	SELECT @IsAndroidUser=IsAndroid,@UserMemberOrder=MemberOrder FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID
	
	-- 用户校验
	IF @UserMemberOrder IS NULL
	BEGIN
		SET @strErrorDescribe = N'抱歉,您的用户信息不存在，请联系客服中心！'
		return 2				
	END

	-- 机器过滤
	IF @IsAndroidUser=0
	BEGIN
		-- 报名条件
		IF @UserMemberOrder<@cbMemberOrder	
		BEGIN
			SET @strErrorDescribe = N'抱歉,您的等级不够不符合报名条件！'
			return 3		
		END

		-- 扣除费用
		IF @lMatchFee>0
		BEGIN	
			-- 金币支付
			IF @cbMatchFeeType=0
			BEGIN
				-- 查询金币
				DECLARE @Score BIGINT
				SELECT @Score=Score FROM QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
				IF @Score IS NULL
				BEGIN
					SET @strErrorDescribe = N'抱歉,没有找到您的金币信息,请您与我们的客服人员联系！'
					return 4						
				END

				-- 金币不足
				IF @Score < @lMatchFee		
				BEGIN
					SET @strErrorDescribe = N'抱歉,您身上的金币不足,系统无法为您成功报名！'
					return 5				
				END

				-- 更新金币
				UPDATE QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo SET Score=@Score-@lMatchFee WHERE UserID=@dwUserID				
			END

			-- 元宝支付
			IF @cbMatchFeeType=1
			BEGIN
				-- 查询元宝
				DECLARE @wUserMedal BIGINT
				SELECT @wUserMedal=UserMedal FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID
				IF @wUserMedal IS NULL
				BEGIN
					SET @strErrorDescribe = N'抱歉,没有找到您的元宝信息,请您与我们的客服人员联系！'
					return 4						
				END	

				-- 元宝不足
				IF @wUserMedal < @lMatchFee		
				BEGIN
					SET @strErrorDescribe = N'抱歉,您身上的元宝不足,系统无法为您成功报名！'
					return 5				
				END

				-- 更新元宝
				UPDATE QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo SET UserMedal=@wUserMedal-@lMatchFee WHERE UserID=@dwUserID	
			END
		END
	END

	-- 更新分数
	IF exists(SELECT * FROM MatchScoreInfo(NOLOCK) WHERE UserID=@dwUserID AND ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@dwMatchNo)
	BEGIN
		UPDATE MatchScoreInfo SET Score=@InitialScore,WinCount=0,LostCount=0,DrawCount=0,FleeCount=0,PlayTimeCount=0,OnlineTime=0,
		UserRight=0x10000000,SignupTime=GetDate()
		WHERE UserID=@dwUserID AND ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@dwMatchNo				
	END	ELSE
	BEGIN
		INSERT INTO MatchScoreInfo (UserID,ServerID,MatchID,MatchNo,Score,UserRight) 
		VALUES (@dwUserID,@wServerID,@dwMatchID,@dwMatchNo,@InitialScore,0x10000000)				
	END				

	-- 插入记录	
	IF @IsAndroidUser=0
	BEGIN
		INSERT StreamMatchFeeInfo (UserID,ServerID,MatchID,MatchNo,MatchType,MatchFeeType,MatchFee,CollectDate) 
		VALUES(@dwUserID,@wServerID,@dwMatchID,@dwMatchNo,1,@cbMatchFeeType,@lMatchFee,GETDATE())
	END

	-- 查询金币
	DECLARE @CurrScore BIGINT
	SELECT @CurrScore=Score FROM QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID		
	
	-- 查询元宝
	DECLARE @CurrIngot BIGINT	
	SELECT @CurrIngot=UserMedal FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID
	
	-- 调整数据
	IF @CurrScore IS NULL SET @CurrScore=0
	IF @CurrIngot IS NULL SET @CurrIngot=0

	-- 抛出数据
	SELECT 	@CurrScore AS Score,@CurrIngot AS Ingot		
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------

-- 退还费用
CREATE PROC GSP_GR_UserMatchQuit
	@dwUserID INT,								-- 用户 I D
	@lMatchFee BIGINT,							-- 报名费用
	@wKindID INT,								-- 游戏 I D
	@wServerID INT,								-- 房间 I D
	@strClientIP NVARCHAR(15),					-- 连接地址
	@dwMatchID	INT,							-- 比赛 I D
	@dwMatchNo	INT,							-- 比赛场次
	@strMachineID NVARCHAR(32)	,				-- 机器标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	-- 比赛信息
	DECLARE @cbMatchFeeType TINYINT
	SELECT @cbMatchFeeType=MatchFeeType FROM MatchPublic WHERE MatchID=@dwMatchID
	IF @cbMatchFeeType IS NULL
	BEGIN
		SET @strErrorDescribe = N'抱歉,您报名的比赛不存在！'
		return 1		
	END

	-- 用户信息
	DECLARE @IsAndroidUser TINYINT	
	SELECT @IsAndroidUser=IsAndroid FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	-- 存在判断
	IF @IsAndroidUser IS NULL
	BEGIN
		SET @strErrorDescribe = N'抱歉,您的用户信息不存在！'
		return 2		
	END

	-- 返还费用
	IF @IsAndroidUser=0 AND @lMatchFee>0
	BEGIN
		-- 金币支付
		IF @cbMatchFeeType=0
		BEGIN
			-- 查询金币
			DECLARE @Score BIGINT
			SELECT @Score=Score FROM QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
			IF @Score IS NULL
			BEGIN
				SET @strErrorDescribe = N'没有找到您的金币信息,请您与我们的客服人员联系！'
				return 3						
			END

			-- 更新金币
			UPDATE QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo SET Score=@Score+@lMatchFee WHERE UserID=@dwUserID				
		END

		-- 奖牌支付
		IF @cbMatchFeeType=1
		BEGIN
			-- 查询元宝
			DECLARE @UserMedal BIGINT
			SELECT @UserMedal=UserMedal FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID
			IF @UserMedal IS NULL
			BEGIN
				SET @strErrorDescribe = N'没有找到您的元宝信息,请您与我们的客服人员联系！'
				return 4						
			END	

			-- 更新元宝
			UPDATE QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo SET UserMedal=@UserMedal+@lMatchFee WHERE UserID=@dwUserID	
		END	
	END

	-- 删除记录
	IF @IsAndroidUser=0 
	BEGIN
		DELETE StreamMatchFeeInfo WHERE UserID=@dwUserID AND ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@dwMatchNo
	END

	-- 查询金币
	DECLARE @CurrScore BIGINT
	SELECT @CurrScore=Score FROM QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID		
	
	-- 查询元宝
	DECLARE @CurrIngot BIGINT	
	SELECT @CurrIngot=UserMedal FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	-- 调整数据
	IF @CurrScore IS NULL SET @CurrScore=0
	IF @CurrIngot IS NULL SET @CurrIngot=0

	-- 抛出数据
	SELECT 	@CurrScore AS Score,@CurrIngot AS Ingot
END

RETURN 0
GO