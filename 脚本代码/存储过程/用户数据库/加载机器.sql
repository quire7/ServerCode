
----------------------------------------------------------------------------------------------------

USE QPAccountsDB
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
	-- 参数校验
	IF @wServerID=0 OR @dwBatchID=0 OR @dwAndroidCount=0 
	BEGIN
		RETURN 1
	END

	-- 解锁机器
	UPDATE AndroidLockInfo SET AndroidStatus=0,ServerID=0,BatchID=0 
	WHERE AndroidStatus<>0 AND ServerID<>0 AND BatchID<>0 AND DATEDIFF(hh,LockDateTime,GetDate())>=12	

	-- 锁定机器
	UPDATE AndroidLockInfo SET ServerID=@wServerID 
	WHERE UserID IN (SELECT TOP (@dwAndroidCount) UserID FROM AndroidLockInfo WHERE AndroidStatus=0 AND ServerID=0 ORDER BY NEWID())		 

	-- 查询机器
	SELECT a.UserID,b.LogonPass INTO #TempAndroids FROM AndroidLockInfo a,AccountsInfo b
	WHERE a.AndroidStatus=0 AND a.ServerID=@wServerID AND a.UserID=b.UserID

	-- 更新状态
	UPDATE AndroidLockInfo SET AndroidStatus=1,BatchID=@dwBatchID,LockDateTime=GetDate() 
	FROM AndroidLockInfo a,#TempAndroids b WHERE a.UserID = b.UserID

	-- 更新分数
--	UPDATE GameScoreInfo SET Score=0 FROM GameScoreInfo a,#TempAndroids b
--	WHERE a.UserID = b.UserID	

	-- 查询机器
	SELECT * FROM #TempAndroids

	-- 销毁临时表
	IF OBJECT_ID('tempdb..#LockedAndroids') IS NOT NULL DROP TABLE #LockedAndroids
	IF OBJECT_ID('tempdb..#TempAndroids') IS NOT NULL DROP TABLE #TempAndroids
END

RETURN 0

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

	-- 更新状态
	UPDATE AndroidLockInfo SET AndroidStatus=0,ServerID=0,BatchID=0,LockDateTime=GetDate()  
	WHERE AndroidStatus<>0 AND BatchID<>0 AND ServerID=@wServerID

END

RETURN 0

----------------------------------------------------------------------------------------------------