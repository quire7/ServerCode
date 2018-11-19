
----------------------------------------------------------------------------------------------------

USE QPGameScoreDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'dbo.GSP_GR_LoadAndroidConfigure') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE dbo.GSP_GR_LoadAndroidConfigure
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ���ػ�������
CREATE PROC GSP_GR_LoadAndroidConfigure
	@wKindID SMALLINT,							-- ��Ϸ I D
	@wServerID SMALLINT							-- ���� I D
AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ����
	SELECT BatchID,ServiceMode,AndroidCount,EnterTime,LeaveTime,EnterMinInterval,EnterMaxInterval,LeaveMinInterval,
		LeaveMaxInterval,TakeMinScore,TakeMaxScore,SwitchMinInnings,SwitchMaxInnings FROM QPAccountsDBLink.QPAccountsDB.dbo.Androidconfigure
	WHERE ServerID=@wServerID ORDER BY BatchID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------