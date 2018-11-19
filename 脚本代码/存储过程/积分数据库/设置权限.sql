
----------------------------------------------------------------------------------------------------

USE QPGameScoreDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_ManageUserRight]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_ManageUserRight]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ����Ȩ��
CREATE PROC GSP_GR_ManageUserRight
	@dwUserID		INT,				-- �û� I D
	@dwAddRight		INT,				-- ����Ȩ��
	@dwRemoveRight	INT					-- ɾ��Ȩ��
AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- ����Ȩ��
	UPDATE QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo SET UserRight=UserRight|@dwAddRight WHERE UserID=@dwUserID
	UPDATE QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo SET UserRight=(~@dwRemoveRight)&UserRight WHERE UserID=@dwUserID	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
