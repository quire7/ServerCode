
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

-- ���ػ���
CREATE PROC GSP_GR_LoadAndroidUser
	@wServerID	SMALLINT,					-- �����ʶ
	@dwBatchID	INT,						-- ���α�ʶ
	@dwAndroidCount INT						-- ������Ŀ
AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- ����У��
	IF @wServerID=0 OR @dwBatchID=0 OR @dwAndroidCount=0 
	BEGIN
		RETURN 1
	END

	-- ��������
	UPDATE AndroidLockInfo SET AndroidStatus=0,ServerID=0,BatchID=0 
	WHERE AndroidStatus<>0 AND ServerID<>0 AND BatchID<>0 AND DATEDIFF(hh,LockDateTime,GetDate())>=12	

	-- ��������
	UPDATE AndroidLockInfo SET ServerID=@wServerID 
	WHERE UserID IN (SELECT TOP (@dwAndroidCount) UserID FROM AndroidLockInfo WHERE AndroidStatus=0 AND ServerID=0 ORDER BY NEWID())		 

	-- ��ѯ����
	SELECT a.UserID,b.LogonPass INTO #TempAndroids FROM AndroidLockInfo a,AccountsInfo b
	WHERE a.AndroidStatus=0 AND a.ServerID=@wServerID AND a.UserID=b.UserID

	-- ����״̬
	UPDATE AndroidLockInfo SET AndroidStatus=1,BatchID=@dwBatchID,LockDateTime=GetDate() 
	FROM AndroidLockInfo a,#TempAndroids b WHERE a.UserID = b.UserID

	-- ���·���
--	UPDATE GameScoreInfo SET Score=0 FROM GameScoreInfo a,#TempAndroids b
--	WHERE a.UserID = b.UserID	

	-- ��ѯ����
	SELECT * FROM #TempAndroids

	-- ������ʱ��
	IF OBJECT_ID('tempdb..#LockedAndroids') IS NOT NULL DROP TABLE #LockedAndroids
	IF OBJECT_ID('tempdb..#TempAndroids') IS NOT NULL DROP TABLE #TempAndroids
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��������
CREATE PROC GSP_GR_UnlockAndroidUser
	@wServerID	SMALLINT					-- �����ʶ	
AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ����״̬
	UPDATE AndroidLockInfo SET AndroidStatus=0,ServerID=0,BatchID=0,LockDateTime=GetDate()  
	WHERE AndroidStatus<>0 AND BatchID<>0 AND ServerID=@wServerID

END

RETURN 0

----------------------------------------------------------------------------------------------------