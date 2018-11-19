
----------------------------------------------------------------------------------------------------

USE QPTreasureDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadParameter]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ��������
CREATE PROC GSP_GR_LoadParameter
	@wKindID SMALLINT,							-- ��Ϸ I D
	@wServerID SMALLINT							-- ���� I D
AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ���ƻ���
	DECLARE @MedalRate AS INT
	SELECT @MedalRate=StatusValue FROM QPAccountsDBLink.QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'MedalRate'

	-- ����˰��
	DECLARE @RevenueRate AS INT
	SELECT @RevenueRate=StatusValue FROM QPAccountsDBLink.QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'RevenueRate'

	-- �һ�����
	DECLARE @ExchangeRate AS INT
	SELECT @ExchangeRate=StatusValue FROM QPAccountsDBLink.QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'MedalExchangeRate'

	-- Ӯ�־���
	DECLARE @WinExperience AS INT
	SELECT @WinExperience=StatusValue FROM QPAccountsDBLink.QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'WinExperience'	

	-- ��������
	IF @MedalRate IS NULL SET @MedalRate=1
	IF @RevenueRate IS NULL SET @RevenueRate=1

	-- ����汾
	DECLARE @ClientVersion AS INT
	DECLARE @ServerVersion AS INT
	SELECT @ClientVersion=TableGame.ClientVersion, @ServerVersion=TableGame.ServerVersion
	FROM QPPlatformDBLink.QPPlatformDB.dbo.GameGameItem TableGame,QPPlatformDBLink.QPPlatformDB.dbo.GameKindItem TableKind
	WHERE TableGame.GameID=TableKind.GameID	AND TableKind.KindID=@wKindID

	-- ������
	SELECT @MedalRate AS MedalRate, @RevenueRate AS RevenueRate,@ExchangeRate AS ExchangeRate,@WinExperience AS WinExperience, 
		@ClientVersion AS ClientVersion, @ServerVersion AS ServerVersion

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------