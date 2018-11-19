
----------------------------------------------------------------------------------------------------

USE QPAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_AndroidGetParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_AndroidGetParameter]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_AndroidAddParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_AndroidAddParameter]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_AndroidModifyParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_AndroidModifyParameter]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_AndroidDeleteParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_AndroidDeleteParameter]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 读取参数
CREATE PROC GSP_GP_AndroidGetParameter
	@wServerID INT								-- 房间标识	
AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询参数
	SELECT * FROM AndroidConfigure WHERE ServerID=@wServerID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 添加参数
CREATE PROC GSP_GP_AndroidAddParameter
	@wServerID INT,								-- 房间标识
	@dwServiceMode INT,							-- 服务模式
	@dwAndroidCount INT,						-- 机器数目
	@dwEnterTime INT,							-- 进入时间
	@dwLeaveTime INT,							-- 离开时间
	@dwEnterMinInterval INT,					-- 进入间隔
	@dwEnterMaxInterval INT,					-- 进入间隔
	@dwLeaveMinInterval	INT,					-- 离开间隔
	@dwLeaveMaxInterval	INT,					-- 离开间隔
	@lTakeMinScore	BIGINT,						-- 携带分数
	@lTakeMaxScore BIGINT,						-- 携带分数
	@dwSwitchMinInnings INT,					-- 换桌局数
	@dwSwitchMaxInnings INT						-- 换桌局数
AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 插入数据
	INSERT AndroidConfigure(ServerID,ServiceMode,AndroidCount,EnterTime,LeaveTime,EnterMinInterval,EnterMaxInterval,LeaveMinInterval,
			LeaveMaxInterval,TakeMinScore,TakeMaxScore,SwitchMinInnings,SwitchMaxInnings)
	VALUES(@wServerID,@dwServiceMode,@dwAndroidCount,@dwEnterTime,@dwLeaveTime,@dwEnterMinInterval,@dwEnterMaxInterval,@dwLeaveMinInterval,
			@dwLeaveMaxInterval,@lTakeMinScore,@lTakeMaxScore,@dwSwitchMinInnings,@dwSwitchMaxInnings)

	-- 查询批次	
	DECLARE @dwBatchID INT
	SET @dwBatchID=SCOPE_IDENTITY()
	
	-- 查询数据
	SELECT * FROM AndroidConfigure WHERE BatchID=@dwBatchID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------


-- 修改参数
CREATE PROC GSP_GP_AndroidModifyParameter
	@dwDatchID INT,								-- 批次标识
	@dwServiceMode INT,							-- 服务模式
	@dwAndroidCount INT,						-- 机器数目
	@dwEnterTime INT,							-- 进入时间
	@dwLeaveTime INT,							-- 离开时间
	@dwEnterMinInterval INT,					-- 进入间隔
	@dwEnterMaxInterval INT,					-- 进入间隔
	@dwLeaveMinInterval	INT,					-- 离开间隔
	@dwLeaveMaxInterval	INT,					-- 离开间隔
	@lTakeMinScore	BIGINT,						-- 携带分数
	@lTakeMaxScore	BIGINT,						-- 携带分数
	@dwSwitchMinInnings INT,					-- 换桌局数
	@dwSwitchMaxInnings INT						-- 换桌局数
AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 更新参数
	UPDATE AndroidConfigure SET ServiceMode=@dwServiceMode,AndroidCount=@dwAndroidCount,EnterTime=@dwEnterTime,LeaveTime=@dwLeaveTime,
		EnterMinInterval=@dwEnterMinInterval,EnterMaxInterval=@dwEnterMaxInterval,LeaveMinInterval=@dwLeaveMinInterval,
		LeaveMaxInterval=@dwLeaveMaxInterval,TakeMinScore=@lTakeMinScore,TakeMaxScore=@lTakeMaxScore,SwitchMinInnings=@dwSwitchMinInnings,
		SwitchMaxInnings=@dwSwitchMaxInnings
	WHERE BatchID=@dwDatchID
	
	-- 查询数据
	SELECT * FROM AndroidConfigure WHERE BatchID=@dwDatchID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 删除参数
CREATE PROC GSP_GP_AndroidDeleteParameter
	@dwBatchID INT								-- 批次标识	
AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询参数
	SELECT * FROM AndroidConfigure WHERE BatchID=@dwBatchID

	-- 删除参数
	DELETE AndroidConfigure WHERE BatchID=@dwBatchID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------