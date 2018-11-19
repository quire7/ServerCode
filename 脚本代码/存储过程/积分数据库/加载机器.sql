
----------------------------------------------------------------------------------------------------

USE QPGameScoreDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'dbo.GSP_GR_LoadAndroidUser') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE dbo.GSP_GR_LoadAndroidUser
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'dbo.GSP_GR_UnLockAndroidUser') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE dbo.GSP_GR_UnLockAndroidUser
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 加载机器
CREATE PROC GSP_GR_LoadAndroidUser
	@wServerID	SMALLINT,					-- 房间标识
	@dwBatchID	INT,						-- 批次标识
	@dwAndroidCount INT						-- 机器数目
AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 变量定义	
	DECLARE	@return_value int

	EXEC @return_value = QPAccountsDBLink.QPAccountsDB.dbo.GSP_GR_LoadAndroidUser
		 @wServerID = @wServerID,
		 @dwBatchID = @dwBatchID,
		 @dwAndroidCount = @dwAndroidCount

	RETURN @return_value
END

GO

----------------------------------------------------------------------------------------------------

-- 解锁机器
CREATE PROC GSP_GR_UnlockAndroidUser
	@wServerID	SMALLINT					-- 房间标识	
AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	-- 变量定义	
	DECLARE	@return_value int

	EXEC @return_value = QPAccountsDBLink.QPAccountsDB.dbo.GSP_GR_UnlockAndroidUser
		 @wServerID = @wServerID

	RETURN @return_value

END

GO
----------------------------------------------------------------------------------------------------