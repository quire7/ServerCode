
----------------------------------------------------------------------------------------------------

USE QPAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_CheckUserRight]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_CheckUserRight]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ���Ȩ��
CREATE PROC GSP_GP_CheckUserRight
	@strAccounts	NVARCHAR(31),		-- �û��ʺ�
	@dwcheckRight	INT,					-- У��Ȩ��	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯȨ��
	DECLARE @dwMasterRight INT
	SELECT @dwMasterRight=MasterRight FROM AccountsInfo WHERE Accounts=@strAccounts AND MasterOrder>0
	
	-- ����Ȩ��
	IF @dwMasterRight IS NULL
	BEGIN
		SET @dwMasterRight=0
	END

	-- ���Ȩ��
	IF (@dwMasterRight&@dwcheckRight)=0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ,������Ȩ�޲���,��¼ʧ�ܣ�'
		RETURN 1
	END	
			
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
