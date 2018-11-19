

USE QPAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_SystemFaceInsert]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_SystemFaceInsert]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_CustomFaceInsert]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_CustomFaceInsert]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_CustomFaceDelete]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_CustomFaceDelete]
GO

----------------------------------------------------------------------------------------------------

-- ����ͷ��
CREATE PROC GSP_GP_SystemFaceInsert
	@dwUserID INT,								-- �û���ʶ
	@strPassword NCHAR(32),						-- �û�����
	@wFaceID SMALLINT,							-- ͷ���ʶ
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineSerial NVARCHAR(32),				-- ������ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	DECLARE @LogonPass AS NCHAR(32)
	SELECT @LogonPass=LogonPass FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- �����ж�
	IF @LogonPass IS NULL OR @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�'
		RETURN 1
	END

	-- �����û�
	UPDATE AccountsInfo SET FaceID=@wFaceID, CustomID=0 WHERE UserID=@dwUserID

	-- ���ؽ��
	SELECT @wFaceID AS FaceID

END	

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ����ͷ��
CREATE PROC GSP_GP_CustomFaceInsert
	@dwUserID INT,								-- �û���ʶ
	@strPassword NCHAR(32),						-- �û�����
	@cbCustomFace IMAGE,						-- ͼ������
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineSerial NVARCHAR(12),				-- ������ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	DECLARE @LogonPass AS NCHAR(32)
	SELECT @LogonPass=LogonPass FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- �����ж�
	IF @LogonPass IS NULL OR @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�'
		RETURN 1
	END

	-- ����ͷ��
	INSERT AccountsFace (UserID, CustomFace, InsertAddr, InsertMachine)
	VALUES (@dwUserID, @cbCustomFace, @strClientIP, @strMachineSerial)

	-- �����û�
	DECLARE @CustomID INT
	SELECT @CustomID=SCOPE_IDENTITY()
	UPDATE AccountsInfo SET CustomID=@CustomID WHERE UserID=@dwUserID

	-- ���ؽ��
	SELECT @CustomID AS CustomID

END	

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ɾ��ͷ��
CREATE PROC GSP_GP_CustomFaceDelete 
	@dwUserID INT,								-- �û���ʶ
	@strPassword NCHAR(32),						-- �û�����
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	DECLARE @LogonPass AS NCHAR(32)
	SELECT @LogonPass=LogonPass FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- �����ж�
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�'
		RETURN 1
	END

	-- �����û�
	UPDATE AccountsInfo SET CustomID=0 WHERE UserID=@dwUserID

END	

RETURN 0

GO

----------------------------------------------------------------------------------------------------
