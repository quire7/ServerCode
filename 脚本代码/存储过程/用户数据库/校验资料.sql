
----------------------------------------------------------------------------------------------------

USE QPAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_VerifyIndividual]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_VerifyIndividual]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ��ѯ����
CREATE PROC GSP_GP_VerifyIndividual
	@wVerifyMask INT,							-- У������
	@strVerifyContent NVARCHAR(32),				-- У������
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- У���ʺ�
	IF @wVerifyMask=1
	BEGIN
		IF exists(SELECT * FROM AccountsInfo WHERE Accounts=@strVerifyContent)
		BEGIN
			SET @strErrorDescribe=N'���ʺ��ѱ�ռ�ã�'
			RETURN 1			
		END
	END

	-- У���ǳ�
	IF @wVerifyMask=2
	BEGIN
		IF exists(SELECT * FROM AccountsInfo WHERE NickName=@strVerifyContent)
		BEGIN
			SET @strErrorDescribe=N'���ǳ��ѱ�ռ�ã�'
			RETURN 1
		END
	END

	RETURN 0

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------