
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

-- 加载任务
CREATE PROC GSP_GR_LoadTaskList
AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询任务
	SELECT * FROM TaskInfo	

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 查询任务
CREATE PROC GSP_GR_QueryTaskInfo
	wKindID INT,								-- 类型 I D
	dwUserID INT,								-- 用户 I D
	strPassword NCHAR(32),						-- 用户密码
	OUT strErrorDescribe NVARCHAR(127) 			-- 输出信息

AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	-- 查询用户
	IF not exists(SELECT * FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID)
	BEGIN
		SET @strErrorDescribe = N'抱歉，你的用户信息不存在或者密码不正确！'
		return 1
	END

	-- 超时处理
	UPDATE QPAccountsDBLink.QPAccountsDB.dbo.AccountsTask SET TaskStatus=2 
	WHERE UserID=@dwUserID AND TaskStatus=0 AND TimeLimit<DateDiff(s,InputDate,GetDate())

	-- 查询任务
	SELECT TaskID,TaskStatus,Progress,(TimeLimit-DateDiff(ss,InputDate,GetDate())) AS ResidueTime FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsTask 
	WHERE UserID=@dwUserID AND DateDiff(d,InputDate,GetDate())=0 AND (((@wKindID=KindID) AND TaskStatus=0) OR @wKindID=0) 		
END

RETURN 0

GO
----------------------------------------------------------------------------------------------------

CREATE  PROCEDURE `GSP_GR_TaskTake`
	(dwUserID INT,								-- 用户 I D
	wTaskID  INT,								-- 任务 I D
	strPassword NCHAR(32),						-- 用户密码
	strClientIP NVARCHAR(15),					-- 连接地址
	strMachineID NVARCHAR(32),					-- 机器标识
	OUT strErrorDescribe NVARCHAR(127),				-- 输出信息
	out nRet INT)
label_pro:BEGIN

	-- 查询用户
	DECLARE nMemberOrder INT;

	-- 判断数目	
	DECLARE TaskTakeMaxCount INT;
	
	-- 统计任务
	DECLARE TaskTakeCount INT;

	-- 任务对象
	DECLARE nKindID INT;
	DECLARE nUserType INT;
	DECLARE nTimeLimit INT;
	DECLARE nTaskType INT;
	DECLARE nTaskObject INT;
	
	SELECT MemberOrder INTO nMemberOrder FROM QPAccountsDB.AccountsInfo 
	WHERE UserID=dwUserID AND LogonPass=strPassword;
	IF nMemberOrder IS NULL then
		SET strErrorDescribe = '抱歉，你的用户信息不存在或者密码不正确！';
		SET nRet = 1;
		leave label_pro;
	END if;

	-- 重复领取
	IF exists(SELECT * FROM QPAccountsDB.AccountsTask 
	WHERE UserID=dwUserID AND TaskID=wTaskID AND TIMESTAMPDIFF(DAY,InputDate,CURRENT_TIMESTAMP())=0) 
	THEN
		SET strErrorDescribe = '抱歉，同一个任务每天只能领取一次！';
		SET nRet = 3;
		leave label_pro;
	END IF;

	-- 任务对象
	SELECT KindID,UserType,TimeLimit,TaskType,Innings INTO nKindID,nUserType,nTimeLimit,nTaskType,nTaskObject
	FROM TaskInfo WHERE TaskID=wTaskID;
	IF nKindID IS NULL THEN
		SET strErrorDescribe = '抱歉，系统未找到您领取的任务信息！';
		SET nRet = 4;
		leave label_pro;
	END IF;

	-- 普通玩家
	IF nMemberOrder=0 AND (nUserType&0x01)=0 THEN
		SET strErrorDescribe = '抱歉，该任务暂时不对普通玩家开放！';
		SET nRet = 5;
		leave label_pro;
	END IF;

	-- 会员玩家
	IF nMemberOrder>0 AND (nUserType&0x02)=0 THEN
		SET strErrorDescribe = '抱歉，该任务暂时不对会员玩家开放！';
		SET nRet = 6;
		leave label_pro;
	END	IF;

	-- 插入任务
	INSERT INTO QPAccountsDB.AccountsTask(UserID,TaskID,TaskType,TaskObject,KindID,TimeLimit) 
	VALUES(dwUserID,wTaskID,TaskType,TaskObject,KindID,TimeLimit);

	-- 成功提示
	SET strErrorDescribe = '恭喜您，任务领取成功！';
END

----------------------------------------------------------------------------------------------------

-- 任务推进
CREATE PROC GSP_GR_TaskForward
	@dwUserID INT,								-- 用户 I D
	@wKindID INT,								-- 游戏标识
	@wMatchID INT,								-- 比赛标识
	@lWinCount INT,								-- 赢局局数
	@lLostCount INT,							-- 输局局数
	@lDrawCount	INT								-- 和局局数
AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询任务	
	SELECT ID,TaskType,TaskObject,TimeLimit,TaskStatus,InputDate,(case 
									  when TaskType=0x01 then Progress+@lWinCount
									  when TaskType=0x02 then Progress+@lWinCount+@lLostCount+@lDrawCount
									  when TaskType=0x04 then @lWinCount
									  else Progress end) AS NewProgress
	INTO #TempTaskInfo FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsTask 
	WHERE UserID=@dwUserID AND KindID=@wKindID AND TaskStatus=0 AND DateDiff(d,InputDate,GetDate())=0

	-- 更新状态（完成任务、首胜未胜、超时）
	UPDATE #TempTaskInfo SET TaskStatus=(case										 
										 when TaskType=0x04 AND NewProgress=0 then 2
										 when TimeLimit<DateDiff(s,InputDate,GetDate()) then 2
										 when NewProgress>=TaskObject then 1
										 else 0 end)
	-- 更新物理表
	UPDATE QPAccountsDBLink.QPAccountsDB.dbo.AccountsTask SET a.Progress=b.NewProgress,a.TaskStatus=b.TaskStatus 
	FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsTask AS a,#TempTaskInfo AS b WHERE a.ID=b.ID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- 领取奖励
CREATE PROC GSP_GR_TaskReward
	@dwUserID INT,								-- 用户 I D
	@wTaskID  INT,								-- 任务 I D
	@strPassword NCHAR(32),						-- 用户密码
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NVARCHAR(32),					-- 机器标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询用户
	DECLARE @MemberOrder INT
	SELECT @MemberOrder=MemberOrder FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo 
	WHERE UserID=@dwUserID AND LogonPass=@strPassword
	IF @MemberOrder IS NULL
	BEGIN
		SET @strErrorDescribe = N'抱歉，你的用户信息不存在或者密码不正确！'
		return 1
	END

	-- 查询任务
	DECLARE @TaskInputDate DATETIME
	SELECT @TaskInputDate=InputDate FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsTask 
	WHERE UserID=@dwUserID AND TaskID=@wTaskID AND TaskStatus=1 AND DateDiff(d,InputDate,GetDate())=0
	IF @TaskInputDate IS NULL
	BEGIN
		SET @strErrorDescribe = N'请完成当前的任务再来领取奖励！'
		return 2		
	END		

	-- 查询奖励
	DECLARE @StandardAwardGold INT
	DECLARE @StandardAwardIngot INT
	DECLARE @MemberAwardGold INT
	DECLARE @MemberAwardIngot INT
	SELECT @StandardAwardGold=StandardAwardGold,@StandardAwardIngot=StandardAwardMedal,
	@MemberAwardGold=MemberAwardGold,@MemberAwardIngot=MemberAwardMedal	
	FROM TaskInfo WHERE TaskID=@wTaskID

	-- 调整奖励
	IF @StandardAwardGold IS NULL SET @StandardAwardGold=0	
	IF @StandardAwardIngot IS NULL SET @StandardAwardIngot=0
	IF @MemberAwardGold IS NULL SET @MemberAwardGold=0
	IF @MemberAwardIngot IS NULL SET @MemberAwardIngot=0

	-- 执行奖励
	DECLARE @AwardGold INT
	DECLARE @AwardIngot INT
	IF @MemberOrder=0
	BEGIN
		SELECT @AwardGold=@StandardAwardGold,@AwardIngot=@StandardAwardIngot
	END ELSE
	BEGIN
		SELECT @AwardGold=@MemberAwardGold,@AwardIngot=@MemberAwardIngot	
	END

	-- 更新元宝
	UPDATE QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo SET UserMedal=UserMedal+@AwardIngot 
	WHERE UserID=@dwUserID	
		
	-- 更新金币
	UPDATE QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo SET Score=Score+@AwardGold 
	WHERE UserID=@dwUserID

	-- 查询分数
	DECLARE @UserScore BIGINT
	DECLARE @UserIngot BIGINT
	
	-- 查询金币
	SELECT @UserScore=Score FROM  QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo 
	WHERE UserID=@dwUserID				

	-- 查询元宝
	SELECT @UserIngot=UserMedal FROM  QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo 
	WHERE UserID=@dwUserID
	
	-- 调整分数
	IF @UserScore IS NULL SET @UserScore=0		
	IF @UserIngot IS NULL SET @UserIngot=0

	-- 删除任务	
	UPDATE QPAccountsDBLink.QPAccountsDB.dbo.AccountsTask SET TaskStatus=3
	WHERE UserID=@dwUserID AND TaskID=@wTaskID AND DateDiff(d,InputDate,GetDate())=0
	
	-- 任务记录
	INSERT QPRecordDBLink.QPRecordDB.dbo.RecordTask(DateID,UserID,TaskID,AwardGold,AwardMedal,InputDate)
	VALUES (CAST(CAST(@TaskInputDate AS FLOAT) AS INT),@dwUserID,@wTaskID,@AwardGold,@AwardIngot,GetDate())

	-- 成功提示
	SET @strErrorDescribe = N'恭喜您，奖励领取成功！'

	-- 抛出数据
	SELECT @UserScore AS Score,@UserIngot AS Ingot
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------