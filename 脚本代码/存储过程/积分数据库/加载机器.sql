
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

	-- ��������	
	DECLARE	@return_value int

	EXEC @return_value = QPAccountsDBLink.QPAccountsDB.dbo.GSP_GR_LoadAndroidUser
		 @wServerID = @wServerID,
		 @dwBatchID = @dwBatchID,
		 @dwAndroidCount = @dwAndroidCount

	RETURN @return_value
END

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
	
	-- ��������	
	DECLARE	@return_value int

	EXEC @return_value = QPAccountsDBLink.QPAccountsDB.dbo.GSP_GR_UnlockAndroidUser
		 @wServerID = @wServerID

	RETURN @return_value

END

GO
----------------------------------------------------------------------------------------------------