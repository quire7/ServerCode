
----------------------------------------------------------------------------------------------------

USE QPPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadPlatformParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadPlatformParameter]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ��������
CREATE PROC GSP_GR_LoadPlatformParameter
AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- �һ�����
	DECLARE @ExchangeRate AS INT
	SELECT @ExchangeRate=StatusValue FROM QPAccountsDBLink.QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'MedalExchangeRate'

	-- ������
	SELECT @ExchangeRate AS ExchangeRate
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------