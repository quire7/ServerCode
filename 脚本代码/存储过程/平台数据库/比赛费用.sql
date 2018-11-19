USE QPPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_DeductMatchFee]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_DeductMatchFee]

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_ReturnMatchFee]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_ReturnMatchFee]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 扣除费用
CREATE PROC GSP_GR_DeductMatchFee
	@dwUserID INT,								-- 用户 I D
	@strPassword NCHAR(32),						-- 用户密码
	@wServerID INT,								-- 房间 I D	
	@dwMatchID	INT,							-- 比赛 I D
	@dwMatchNO	INT,							-- 比赛编号
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NVARCHAR(32),					-- 机器标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
AS

-- 执行逻辑
BEGIN
	-- 查询用户
	DECLARE @LogonPassword NCHAR(32) 
	DECLARE	@UserMemberOrder TINYINT
	SELECT @UserMemberOrder=MemberOrder,@LogonPassword=LogonPass FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID
	IF @LogonPassword IS NULL
	BEGIN
		SET @strErrorDescribe = N'您的用户信息不存在,请与我们的客服人员联系！'
		return 1				
	END

	-- 校验密码
	IF @LogonPassword <> @strPassword
	BEGIN
		SET @strErrorDescribe = N'您的登录密码不匹配,系统无法为您成功报名！'
		return 2		
	END

	-- 变量声明
	DECLARE	@cbMatchType TINYINT
	DECLARE @cbMatchFeeType SMALLINT 
	DECLARE	@cbMemberOrder TINYINT
	DECLARE @dwMatchFee BIGINT
	DECLARE @dwMatchInitScore BIGINT
	DECLARE @MatchStartTime datetime
	DECLARE	@MatchEndTime datetime	
	
	-- 查询赛事
	SELECT @cbMatchType=a.MatchType,@cbMatchFeeType=a.MatchFeeType,@dwMatchFee=a.MatchFee,@cbMemberOrder=a.MemberOrder,
	@MatchStartTime=b.StartTime,@MatchEndTime=b.EndTime,@dwMatchInitScore=b.InitScore 
	FROM QPGameMatchDBLink.QPGameMatchDB.dbo.MatchPublic AS a,QPGameMatchDBLink.QPGameMatchDB.dbo.MatchLockTime AS b 
	WHERE a.MatchID=@dwMatchID AND a.MatchNo=@dwMatchNo AND a.MatchID=b.MatchID AND a.MatchNo=b.MatchNo
	IF @cbMatchType IS NULL OR @cbMatchFeeType IS NULL
	BEGIN
		SET @strErrorDescribe = N'没有找到该赛事的信息,请您与我们的客服人员联系！'
		return 3
	END

	-- 判断类型
	IF @cbMatchType <> 0
	BEGIN
		SET @strErrorDescribe = N'抱歉,比赛中心暂时只支持定时赛相关赛事的报名！'
		return 4		
	END

	-- 比赛结束			
	IF DATEDIFF(ss,@MatchEndTime,GetDate()) > 0
	BEGIN
		SET @strErrorDescribe = N'本场比赛已经结束,请您下次再来报名参加！'
		return 5		
	END

	-- 报名条件
	IF @UserMemberOrder < @cbMemberOrder
	BEGIN
		SET @strErrorDescribe = N'抱歉,您的会员等级不够不符合报名条件！'
		return 6
	END

	-- 重复报名
	IF Exists(SELECT * FROM QPGameMatchDBLink.QPGameMatchDB.dbo.StreamMatchFeeInfo 
	WHERE UserID=@dwUserID AND ServerID=@wServerID AND MatchID=@dwMatchID AND Effective=0)
	BEGIN
		SET @strErrorDescribe = N'您已经报名该场比赛,不需要重复报名！'
		return 7		
	END	

	-- 金币支付
	IF @cbMatchFeeType=0
	BEGIN
		-- 查询金币
		DECLARE @Score BIGINT
		SELECT @Score=Score FROM QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
		IF @Score IS NULL
		BEGIN
			SET @strErrorDescribe = N'抱歉,没有找到您的金币信息,请您与我们的客服人员联系！'
			return 8						
		END

		-- 金币不足
		IF @Score < @dwMatchFee		
		BEGIN
			SET @strErrorDescribe = N'抱歉,您身上的金币不足,系统无法为您成功报名！'
			return 9				
		END

		-- 更新金币
		UPDATE QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo SET Score=@Score-@dwMatchFee WHERE UserID=@dwUserID				
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
			return 8						
		END	

		-- 元宝不足
		IF @wUserMedal < @dwMatchFee		
		BEGIN
			SET @strErrorDescribe = N'抱歉,您身上的元宝不足,系统无法为您成功报名！'
			return 9				
		END

		-- 更新元宝
		UPDATE QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo SET UserMedal=@wUserMedal-@dwMatchFee WHERE UserID=@dwUserID	
	END

	-- 插入记录	
	INSERT QPGameMatchDBLink.QPGameMatchDB.dbo.StreamMatchFeeInfo (UserID,ServerID,MatchID,MatchNo,MatchFeeType,MatchFee,CollectDate) 
			VALUES(@dwUserID,@wServerID,@dwMatchID,@dwMatchNO,@cbMatchFeeType,@dwMatchFee,GETDATE())

	-- 更新分数
	IF Exists(SELECT * FROM QPGameMatchDBLink.QPGameMatchDB.dbo.MatchScoreInfo 
	WHERE UserID=@dwUserID AND ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@dwMatchNo)
	BEGIN
		-- 更新数据
		UPDATE QPGameMatchDBLink.QPGameMatchDB.dbo.MatchScoreInfo 
		SET Score=@dwMatchInitScore,WinCount=0,LostCount=0,DrawCount=0,FleeCount=0
		WHERE UserID=@dwUserID AND ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@dwMatchNo						
	END ELSE						
	BEGIN	
		-- 插入资料
		INSERT INTO QPGameMatchDBLink.QPGameMatchDB.dbo.MatchScoreInfo(UserID,ServerID,MatchID,MatchNo,Score)
		VALUES (@dwUserID,@wServerID,@dwMatchID,@dwMatchNo,@dwMatchInitScore)
	END	

	-- 抛出提示
	SET @strErrorDescribe = N'恭喜您,报名成功,请于 '+convert(char(8),@MatchStartTime,108)+ N' - '+convert(char(8),@MatchEndTime,108)+N' 准时参加比赛！'  
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------

-- 退还费用
CREATE PROC GSP_GR_ReturnMatchFee
	@dwUserID INT,								-- 用户 I D
	@strPassword NCHAR(32),						-- 用户密码
	@wServerID INT,								-- 房间 I D	
	@dwMatchID	INT,							-- 比赛 I D
	@dwMatchNO	INT,							-- 比赛编号	
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NVARCHAR(32),					-- 机器标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
AS

-- 执行逻辑
BEGIN		

	-- 查询用户
	DECLARE @LogonPassword NCHAR(32) 
	SELECT @LogonPassword=LogonPass FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID
	IF @LogonPassword IS NULL
	BEGIN
		SET @strErrorDescribe = N'您的用户信息不存在,请与我们的客服人员联系！'
		return 1				
	END

	-- 校验密码
	IF @LogonPassword <> @strPassword
	BEGIN
		SET @strErrorDescribe = N'您的登录密码不匹配,系统无法为您取消报名！'
		return 2		
	END

	-- 报名记录
	DECLARE @MatchNo INT
	DECLARE @dwMatchFee BIGINT
	DECLARE @cbMatchFeeType SMALLINT 	
	SELECT TOP 1 @MatchNo=MatchNo,@cbMatchFeeType=MatchFeeType,@dwMatchFee=MatchFee FROM QPGameMatchDBLink.QPGameMatchDB.dbo.StreamMatchFeeInfo 
	WHERE UserID=@dwUserID AND ServerID=@wServerID AND MatchID=@dwMatchID AND Effective=0
	IF @cbMatchFeeType IS NULL OR @dwMatchFee IS NULL
	BEGIN
		SET @strErrorDescribe = N'系统没有找到您在该场赛事的报名记录或者比赛已结束！'
		return 3		
	END

	-- 查询赛事
	DECLARE @MatchEndTime datetime	
	DECLARE	@MatchStartTime datetime	
	SELECT @MatchStartTime=StartTime,@MatchEndTime=EndTime FROM QPGameMatchDBLink.QPGameMatchDB.dbo.MatchLockTime 
	WHERE MatchID=@dwMatchID AND MatchNo=@dwMatchNo
	IF @MatchStartTime IS NULL OR @MatchEndTime IS NULL
	BEGIN
		SET @strErrorDescribe = N'没有找到该赛事的信息,请您与我们的客服人员联系！'
		return 3		
	END

	-- 比赛结束			
	IF DATEDIFF(ss,@MatchEndTime,GetDate()) > 0
	BEGIN
		SET @strErrorDescribe = N'本场比赛已经结束,无法为您取消报名！'
		return 5		
	END
	
	-- 比赛开始
	IF DATEDIFF(ss,@MatchStartTime,GetDate()) > 0
	BEGIN
		-- 统计局数
		DECLARE @PlayTotalCount INT
		SELECT @PlayTotalCount=WinCount+LostCount+DrawCount+FleeCount FROM QPGameMatchDBLink.QPGameMatchDB.dbo.MatchScoreInfo
		WHERE UserID=@dwUserID AND ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@dwMatchNo

		-- 已参赛不取消
		IF @PlayTotalCount > 0 OR @dwMatchNo < 1
		BEGIN 		
			SET @strErrorDescribe = N'抱歉,您已参加过本场赛事的比赛,不能取消报名！'
			return 6		
		END
	END

	-- 比赛场次
	IF @MatchNo=@dwMatchNo
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
				return 8						
			END

			-- 更新金币
			UPDATE QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo SET Score=@Score+@dwMatchFee WHERE UserID=@dwUserID				
		END

		-- 元宝支付
		IF @cbMatchFeeType=1
		BEGIN
			-- 查询金币
			DECLARE @UserMedal BIGINT
			SELECT @UserMedal=UserMedal FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID
			IF @UserMedal IS NULL
			BEGIN
				SET @strErrorDescribe = N'没有找到您的元宝信息,请您与我们的客服人员联系！'
				return 9						
			END	

			-- 更新元宝
			UPDATE QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo SET UserMedal=@UserMedal+@dwMatchFee WHERE UserID=@dwUserID	
		END			
	END

	-- 删除记录
	DELETE QPGameMatchDBLink.QPGameMatchDB.dbo.StreamMatchFeeInfo 
	WHERE UserID=@dwUserID AND ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@MatchNo AND Effective=0	

	-- 更新分数
	UPDATE QPGameMatchDBLink.QPGameMatchDB.dbo.MatchScoreInfo 
	SET Score=0,WinCount=0,LostCount=0,DrawCount=0,FleeCount=0,UserRight=0
	WHERE UserID=@dwUserID AND ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@dwMatchNo	
	
	-- 抛出提示
	SET @strErrorDescribe = N'系统已为您成功取消报名！'
END

RETURN 0
GO