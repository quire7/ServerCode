
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

-- ��ȡ����
CREATE PROC GSP_GP_AndroidGetParameter
	@wServerID INT								-- �����ʶ	
AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ����
	SELECT * FROM AndroidConfigure WHERE ServerID=@wServerID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��Ӳ���
CREATE PROC GSP_GP_AndroidAddParameter
	@wServerID INT,								-- �����ʶ
	@dwServiceMode INT,							-- ����ģʽ
	@dwAndroidCount INT,						-- ������Ŀ
	@dwEnterTime INT,							-- ����ʱ��
	@dwLeaveTime INT,							-- �뿪ʱ��
	@dwEnterMinInterval INT,					-- ������
	@dwEnterMaxInterval INT,					-- ������
	@dwLeaveMinInterval	INT,					-- �뿪���
	@dwLeaveMaxInterval	INT,					-- �뿪���
	@lTakeMinScore	BIGINT,						-- Я������
	@lTakeMaxScore BIGINT,						-- Я������
	@dwSwitchMinInnings INT,					-- ��������
	@dwSwitchMaxInnings INT						-- ��������
AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��������
	INSERT AndroidConfigure(ServerID,ServiceMode,AndroidCount,EnterTime,LeaveTime,EnterMinInterval,EnterMaxInterval,LeaveMinInterval,
			LeaveMaxInterval,TakeMinScore,TakeMaxScore,SwitchMinInnings,SwitchMaxInnings)
	VALUES(@wServerID,@dwServiceMode,@dwAndroidCount,@dwEnterTime,@dwLeaveTime,@dwEnterMinInterval,@dwEnterMaxInterval,@dwLeaveMinInterval,
			@dwLeaveMaxInterval,@lTakeMinScore,@lTakeMaxScore,@dwSwitchMinInnings,@dwSwitchMaxInnings)

	-- ��ѯ����	
	DECLARE @dwBatchID INT
	SET @dwBatchID=SCOPE_IDENTITY()
	
	-- ��ѯ����
	SELECT * FROM AndroidConfigure WHERE BatchID=@dwBatchID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------


-- �޸Ĳ���
CREATE PROC GSP_GP_AndroidModifyParameter
	@dwDatchID INT,								-- ���α�ʶ
	@dwServiceMode INT,							-- ����ģʽ
	@dwAndroidCount INT,						-- ������Ŀ
	@dwEnterTime INT,							-- ����ʱ��
	@dwLeaveTime INT,							-- �뿪ʱ��
	@dwEnterMinInterval INT,					-- ������
	@dwEnterMaxInterval INT,					-- ������
	@dwLeaveMinInterval	INT,					-- �뿪���
	@dwLeaveMaxInterval	INT,					-- �뿪���
	@lTakeMinScore	BIGINT,						-- Я������
	@lTakeMaxScore	BIGINT,						-- Я������
	@dwSwitchMinInnings INT,					-- ��������
	@dwSwitchMaxInnings INT						-- ��������
AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ���²���
	UPDATE AndroidConfigure SET ServiceMode=@dwServiceMode,AndroidCount=@dwAndroidCount,EnterTime=@dwEnterTime,LeaveTime=@dwLeaveTime,
		EnterMinInterval=@dwEnterMinInterval,EnterMaxInterval=@dwEnterMaxInterval,LeaveMinInterval=@dwLeaveMinInterval,
		LeaveMaxInterval=@dwLeaveMaxInterval,TakeMinScore=@lTakeMinScore,TakeMaxScore=@lTakeMaxScore,SwitchMinInnings=@dwSwitchMinInnings,
		SwitchMaxInnings=@dwSwitchMaxInnings
	WHERE BatchID=@dwDatchID
	
	-- ��ѯ����
	SELECT * FROM AndroidConfigure WHERE BatchID=@dwDatchID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ɾ������
CREATE PROC GSP_GP_AndroidDeleteParameter
	@dwBatchID INT								-- ���α�ʶ	
AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ����
	SELECT * FROM AndroidConfigure WHERE BatchID=@dwBatchID

	-- ɾ������
	DELETE AndroidConfigure WHERE BatchID=@dwBatchID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------