
----------------------------------------------------------------------------------------------------

USE QPTreasureDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UserEnableInsure]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UserEnableInsure]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UserSaveScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UserSaveScore]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UserTakeScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UserTakeScore]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UserTransferScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UserTransferScore]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_QueryUserInsureInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_QueryUserInsureInfo]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON
GO

----------------------------------------------------------------------------------------------------

-- ��ͨ����
CREATE PROC GSP_GR_UserEnableInsure
	@dwUserID INT,								-- �û� I D
	@strLogonPass NCHAR(32),					-- ��¼����
	@strInsurePass NCHAR(32),					-- ��������	
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	IF NOT Exists(SELECT * FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID AND LogonPass=@strLogonPass)
	BEGIN
		SET @strErrorDescribe=N'������֤ʧ�ܣ��޷���ͨ���У�'
		RETURN 1		
	END
	
	-- ��������
	UPDATE QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo SET InsurePass=@strInsurePass	WHERE UserID=@dwUserID		

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ������
CREATE PROC GSP_GR_UserSaveScore
	@dwUserID INT,								-- �û� I D
	@lSaveScore BIGINT,							-- �����Ŀ
	@wKindID SMALLINT,							-- ��Ϸ I D
	@wServerID SMALLINT,						-- ���� I D
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
AS

-- ��������
SET NOCOUNT ON

-- ��ұ���
DECLARE @SourceScore BIGINT
DECLARE @SourceInsure BIGINT
DECLARE @InsureRevenue BIGINT
DECLARE @VariationScore BIGINT
DECLARE @VariationInsure BIGINT

-- ִ���߼�
BEGIN

	-- ��������
	DECLARE @EnjoinLogon INT
	DECLARE @EnjoinInsure INT

	-- ϵͳ��ͣ
	SELECT @EnjoinInsure=StatusValue FROM QPAccountsDBLink.QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinInsure'
	IF @EnjoinInsure IS NOT NULL AND @EnjoinInsure<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM QPAccountsDBLink.QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinInsure'
		RETURN 2
	END

	-- Ч���ַ
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�������ڵ� IP ��ַ����Ϸ����Ȩ�ޣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 4
	END
	
	-- Ч�����
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�����Ļ�������Ϸ����Ȩ�ޣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 7
	END
 
	-- ��ѯ�û�
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE	@MachineID NVARCHAR(32)
	DECLARE @MoorMachine AS TINYINT
	SELECT @UserID=UserID, @Nullity=Nullity, @StunDown=StunDown, @MoorMachine=MoorMachine, @MachineID=LastLogonMachine
	FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	-- ��ѯ�û�
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ������������������֤���ٴγ��Ե�¼��'
		RETURN 1
	END	

	-- �ʺŽ�ֹ
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����'
		RETURN 2
	END	

	-- �ʺŹر�
	IF @StunDown<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ�ʹ���˰�ȫ�رչ��ܣ��������¿�ͨ����ܼ���ʹ�ã�'
		RETURN 2
	END	
	
	-- �̶�����
	IF @MoorMachine=1
	BEGIN
		IF @MachineID<>@strMachineID
		BEGIN
			SET @strErrorDescribe=N'�����ʺ�ʹ�ù̶�������¼���ܣ�������ʹ�õĻ���������ָ���Ļ�����'
			RETURN 1
		END
	END
	
	-- ����ж�
	DECLARE @BankPrerequisite AS INT
	SELECT @BankPrerequisite=StatusValue FROM QPAccountsDBLink.QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'BankPrerequisite'
	IF @BankPrerequisite IS NULL SET @BankPrerequisite=0
	IF @lSaveScore<@BankPrerequisite
	BEGIN
		SET @strErrorDescribe=N'�������е���Ϸ����Ŀ�������� '+LTRIM(STR(@BankPrerequisite))+'����Ϸ�Ҵ���ʧ�ܣ�'
		RETURN 4
	END

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- ��Ϸ��Ϣ
	SELECT @SourceScore=Score, @SourceInsure=InsureScore FROM GameScoreInfo WHERE UserID=@dwUserID

	-- ����ж�
	IF @SourceScore IS NULL OR @SourceScore<@lSaveScore
	BEGIN
		-- ��������
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- ������Ϣ
		SET @strErrorDescribe=N'����ǰ��Ϸ�ҵĿ������㣬��Ϸ�Ҵ���ʧ�ܣ�'
		RETURN 4
	END

	-- ��ѯ����
	DECLARE @LockKindID INT
	DECLARE @LockServerID INT
	SELECT @LockKindID=KindID, @LockServerID=ServerID FROM GameScoreLocker WHERE UserID=@dwUserID

	-- ��������
	IF @LockKindID IS NULL SET @LockKindID=0
	IF @LockServerID IS NULL SET @LockServerID=0

	-- �����ж�
	IF (@LockKindID<>0 and @LockKindID<>@wKindID) OR (@LockServerID<>0 and @LockServerID<>@wServerID)
	BEGIN

		-- ��������
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- ��ѯ����
		IF @LockKindID<>0 AND @LockServerID<>0
		BEGIN
			-- ��ѯ��Ϣ
			DECLARE @KindName NVARCHAR(31)
			DECLARE @ServerName NVARCHAR(31)
			SELECT @KindName=KindName FROM QPPlatformDBLink.QPPlatformDB.dbo.GameKindItem WHERE KindID=@LockKindID
			SELECT @ServerName=ServerName FROM QPPlatformDBLink.QPPlatformDB.dbo.GameRoomInfo WHERE ServerID=@LockServerID

			-- ������Ϣ
			IF @KindName IS NULL SET @KindName=N'δ֪��Ϸ'
			IF @ServerName IS NULL SET @ServerName=N'δ֪����'
			SET @strErrorDescribe=N'������ [ '+@KindName+N' ] �� [ '+@ServerName+N' ] ��Ϸ�����У����ܽ��е�ǰ�����в�����'
			RETURN 4

		END
		ELSE
		BEGIN
			-- ��ʾ��Ϣ
			SELECT [ErrorDescribe]=N'������ʹ����վҳ��������д�������У����ܽ��е�ǰ�����в�����'
			SET @strErrorDescribe=N'������ʹ����վҳ��������д�������У����ܽ��е�ǰ�����в�����'
			RETURN 4
		END
	END

	-- �������
	SET @InsureRevenue=0
	SET @VariationScore=-@lSaveScore
	SET @VariationInsure=@lSaveScore

	-- ������Ϣ
	SET @strErrorDescribe=N'��Ϸ�Ҵ������в����ɹ�������������ʻ���Ϣ��'

	-- ��������
	UPDATE GameScoreInfo SET Score=Score+@VariationScore, InsureScore=InsureScore+@VariationInsure, Revenue=Revenue+@InsureRevenue	
	WHERE UserID=@dwUserID

	-- ��¼��־
	INSERT INTO RecordInsure(KindID,ServerID,SourceUserID,SourceGold,SourceBank,
		SwapScore,Revenue,IsGamePlaza,TradeType,ClientIP)
	VALUES(@wKindID,@wServerID,@UserID,@SourceScore,@SourceInsure,@lSaveScore,@InsureRevenue,0,1,@strClientIP)		

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- ������
	SELECT @dwUserID AS UserID, @SourceScore AS SourceScore, @SourceInsure AS SourceInsure, @VariationScore AS VariationScore,
		@VariationInsure AS VariationInsure, @InsureRevenue AS InsureRevenue, @LockKindID AS KindID, @LockServerID AS ServerID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��ȡ���
CREATE PROC GSP_GR_UserTakeScore
	@dwUserID INT,								-- �û� I D
	@lTakeScore BIGINT,							-- �����Ŀ
	@strPassword NCHAR(32),						-- �û�����
	@wKindID SMALLINT,							-- ��Ϸ I D
	@wServerID SMALLINT,						-- ���� I D
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
AS

-- ��������
SET NOCOUNT ON

-- ��ұ���
DECLARE @SourceScore BIGINT
DECLARE @SourceInsure BIGINT
DECLARE @InsureRevenue BIGINT
DECLARE @VariationScore BIGINT
DECLARE @VariationInsure BIGINT

-- ִ���߼�
BEGIN

	-- ��������
	DECLARE @EnjoinLogon INT
	DECLARE @EnjoinInsure INT

	-- ϵͳ��ͣ
	SELECT @EnjoinInsure=StatusValue FROM QPAccountsDBLink.QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinInsure'
	IF @EnjoinInsure IS NOT NULL AND @EnjoinInsure<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM QPAccountsDBLink.QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinInsure'
		RETURN 2
	END

	-- Ч���ַ
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�������ڵ� IP ��ַ����Ϸ����Ȩ�ޣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 4
	END
	
	-- Ч�����
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�����Ļ�������Ϸ����Ȩ�ޣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 7
	END
 
	-- ��ѯ�û�
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @InsurePass AS NCHAR(32)
	DECLARE	@MachineID NVARCHAR(32)
	DECLARE @MoorMachine AS TINYINT
	SELECT @UserID=UserID, @InsurePass=InsurePass, @Nullity=Nullity, @StunDown=StunDown, @MoorMachine=MoorMachine, @MachineID=LastLogonMachine
	FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	-- ��ѯ�û�
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ������������������֤���ٴγ��Ե�¼��'
		RETURN 1
	END	

	-- �ʺŽ�ֹ
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����'
		RETURN 2
	END	

	-- �ʺŹر�
	IF @StunDown<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ�ʹ���˰�ȫ�رչ��ܣ��������¿�ͨ����ܼ���ʹ�ã�'
		RETURN 2
	END	
	
	-- �̶�����
	IF @MoorMachine=1
	BEGIN
		IF @MachineID<>@strMachineID
		BEGIN
			SET @strErrorDescribe=N'�����ʺ�ʹ�ù̶�������¼���ܣ�������ʹ�õĻ���������ָ���Ļ�����'
			RETURN 1
		END
	END

	-- �����ж�
	IF @InsurePass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'�����������벻��ȷ���������������֤���ٴγ��ԣ�'
		RETURN 3
	END

	-- ����ж�
	DECLARE @BankPrerequisite AS INT
	SELECT @BankPrerequisite=StatusValue FROM QPAccountsDBLink.QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'BankPrerequisite'
	IF @BankPrerequisite IS NULL SET @BankPrerequisite=0
	IF @lTakeScore<@BankPrerequisite
	BEGIN
		SET @strErrorDescribe=N'������ȡ������Ϸ����Ŀ�������� '+LTRIM(STR(@BankPrerequisite))+'����Ϸ����ȡʧ�ܣ�'
		RETURN 4
	END

	-- ����˰��
	DECLARE @RevenueRate INT
	SELECT @RevenueRate=StatusValue FROM QPAccountsDBLink.QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'RevenueRateTake'

	-- ˰�յ���
	IF @RevenueRate>300 SET @RevenueRate=300
	IF @RevenueRate IS NULL SET @RevenueRate=1

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- ��Ϸ��Ϣ
	SELECT @SourceScore=Score, @SourceInsure=InsureScore FROM GameScoreInfo WHERE UserID=@dwUserID

	-- ����ж�
	IF @SourceInsure IS NULL OR @SourceInsure<@lTakeScore
	BEGIN
		-- ��������
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- ������Ϣ
		SET @strErrorDescribe=N'����ǰ���е���Ϸ�����㣬��Ϸ����ȡʧ�ܣ�'
		RETURN 4
	END

	-- ��ѯ����
	DECLARE @LockKindID INT
	DECLARE @LockServerID INT
	SELECT @LockKindID=KindID, @LockServerID=ServerID FROM GameScoreLocker WHERE UserID=@dwUserID

	-- �������
	SET @InsureRevenue=@lTakeScore*@RevenueRate/1000
	SET @VariationScore=@lTakeScore-@InsureRevenue
	SET @VariationInsure=-@lTakeScore

	-- ������Ϣ
	SET @strErrorDescribe=N'������ȡ��Ϸ�Ҳ����ɹ�������������ʻ���Ϣ��'

	-- ��������
	UPDATE GameScoreInfo SET Score=Score+@VariationScore, InsureScore=InsureScore+@VariationInsure, Revenue=Revenue+@InsureRevenue
	WHERE UserID=@dwUserID

	-- ��¼��־
	INSERT INTO RecordInsure(KindID,ServerID,SourceUserID,SourceGold,SourceBank,
		SwapScore,Revenue,IsGamePlaza,TradeType,ClientIP)
	VALUES(@wKindID,@wServerID,@UserID,@SourceScore,@SourceInsure,@lTakeScore,@InsureRevenue,0,2,@strClientIP)	

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- ������
	SELECT @dwUserID AS UserID, @SourceScore AS SourceScore, @SourceInsure AS SourceInsure, @VariationScore AS VariationScore,
		@VariationInsure AS VariationInsure, @InsureRevenue AS InsureRevenue, @LockKindID AS KindID, @LockServerID AS ServerID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ת�˽��
CREATE PROC GSP_GR_UserTransferScore
	@dwUserID INT,								-- �û� I D	
	@lTransferScore BIGINT,						-- �����Ŀ
	@strPassword NCHAR(32),						-- �û�����
	@strNickName NVARCHAR(31),					-- �û��ǳ�
	@strTransRemark NVARCHAR(32),				-- ת�˱�ע	
	@wKindID SMALLINT,							-- ��Ϸ I D
	@wServerID SMALLINT,						-- ���� I D
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
AS

-- ��������
SET NOCOUNT ON

-- ��ұ���
DECLARE @SourceScore BIGINT
DECLARE @SourceInsure BIGINT

DECLARE @InsureRevenue BIGINT
DECLARE @VariationInsure BIGINT

-- ִ���߼�
BEGIN

	-- ��������
	DECLARE @EnjoinLogon INT
	DECLARE @EnjoinInsure INT
	DECLARE @EnjoinTransfer INT

	-- ϵͳ��ͣ
	SELECT @EnjoinInsure=StatusValue FROM QPAccountsDBLink.QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinInsure'
	IF @EnjoinInsure IS NOT NULL AND @EnjoinInsure<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM QPAccountsDBLink.QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinInsure'
		RETURN 2
	END	
	
	-- ת����ͣ
	SELECT @EnjoinTransfer=StatusValue FROM QPAccountsDBLink.QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'TransferStauts'
	IF @EnjoinTransfer IS NOT NULL AND @EnjoinTransfer<>1
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM QPAccountsDBLink.QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'TransferStauts'
		RETURN 3
	END	

	-- Ч���ַ
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�������ڵ� IP ��ַ����Ϸ����Ȩ�ޣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 4
	END
	
	-- Ч�����
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�����Ļ�������Ϸ����Ȩ�ޣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 7
	END

	-- ��ѯ�û�
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @InsurePass AS NCHAR(32)
	DECLARE	@MachineID NVARCHAR(32)
	DECLARE @MemberOrder AS TINYINT
	DECLARE @MoorMachine AS TINYINT
	SELECT @UserID=UserID, @InsurePass=InsurePass, @Nullity=Nullity, @StunDown=StunDown, @MoorMachine=MoorMachine, @MemberOrder=MemberOrder, @MachineID=LastLogonMachine
	FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	-- ��ѯ�û�
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�'
		RETURN 1
	END	

	-- �ʺŽ�ֹ
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����'
		RETURN 2
	END	

	-- �ʺŹر�
	IF @StunDown<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ�ʹ���˰�ȫ�رչ��ܣ��������¿�ͨ����ܼ���ʹ�ã�'
		RETURN 2
	END	
	
	-- �̶�����
	IF @MoorMachine=1
	BEGIN
		IF @MachineID<>@strMachineID
		BEGIN
			SET @strErrorDescribe=N'�����ʺ�ʹ�ù̶�������¼���ܣ�������ʹ�õĻ���������ָ���Ļ�����'
			RETURN 1
		END
	END

	-- �����ж�
	IF @InsurePass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'�����������벻��ȷ���������������֤���ٴγ��ԣ�'
		RETURN 3
	END

	-- ����ж�
	DECLARE @TransferPrerequisite AS BIGINT
	SELECT @TransferPrerequisite=StatusValue FROM QPAccountsDBLink.QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'TransferPrerequisite'
	IF @TransferPrerequisite IS NULL SET @TransferPrerequisite=0
	IF @lTransferScore<@TransferPrerequisite
	BEGIN
		SET @strErrorDescribe=N'������ת�˵���Ϸ����Ŀ�������� '+LTRIM(STR(@TransferPrerequisite))+'����Ϸ��ת��ʧ�ܣ�'
		RETURN 4
	END

	-- Ŀ���û�
	DECLARE @TargetUserID INT
	SELECT @TargetUserID=UserID FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo WHERE NickName=@strNickName	
--	IF @cbByNickName=1
--		SELECT @TargetUserID=UserID FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo WHERE NickName=@strNickName
--	ELSE
--		SELECT @TargetUserID=UserID FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo WHERE GameID=@strNickName	

	-- ��ѯ�û�
	IF @TargetUserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'����Ҫת�˵��û���'+@strNickName+'�������ڻ��������������֤���ٴγ��ԣ�'
		RETURN 5
	END

	-- ��ͬ�ж�
	IF @TargetUserID=@dwUserID
	BEGIN
		SET @strErrorDescribe=N'����ʹ���Լ����ʺ�Ϊ�Լ�ת����Ϸ�ң����֤���ٴγ��ԣ�'
		RETURN 6
	END

	-- ��Ҳ�ѯ
	DECLARE @TargetScore BIGINT
	DECLARE @TargetInsure BIGINT
	SELECT @TargetScore=Score, @TargetInsure=InsureScore FROM GameScoreInfo(NOLOCK) WHERE UserID=@TargetUserID

	-- �����ж�
	IF @TargetScore IS NULL
	BEGIN
		-- ��������
		INSERT INTO GameScoreInfo (UserID,LastLogonIP,RegisterIP) VALUES (@TargetUserID,@strClientIP,@strClientIP)

		-- ��Ϸ��Ϣ
		SELECT @TargetScore=Score, @TargetInsure=InsureScore FROM GameScoreInfo(NOLOCK) WHERE UserID=@TargetUserID
	END

	-- ����˰��	
	DECLARE @MaxTax BIGINT
	DECLARE @RevenueRate INT
	IF @MemberOrder = 0
	BEGIN
		SELECT @MaxTax=StatusValue FROM QPAccountsDBLink.QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'TransferMaxTax'
		SELECT @RevenueRate=StatusValue FROM QPAccountsDBLink.QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'RevenueRateTransfer'

		-- ˰�յ���
		IF @RevenueRate IS NULL SET @RevenueRate=1
	END ELSE
	BEGIN
		SELECT @MaxTax=StatusValue FROM QPAccountsDBLink.QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'TransferMaxTax'
		SELECT @RevenueRate=StatusValue FROM QPAccountsDBLink.QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'RevenueRateTransferMember'		
		
		-- ˰�յ���
		IF @RevenueRate IS NULL SET @RevenueRate=0
	END	
	
	-- ˰�յ���
	IF @RevenueRate>300 SET @RevenueRate=300	

	-- ���б���
	DECLARE @TransferRetention INT	-- ���ٱ���
	DECLARE @SurplusScore BIGINT	-- ת������
	SELECT @TransferRetention=ISNULL(StatusValue,0) FROM QPAccountsDBLink.QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'TransferRetention'

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- ��Ϸ��Ϣ
	SELECT @SourceScore=Score, @SourceInsure=InsureScore FROM GameScoreInfo WHERE UserID=@dwUserID

	-- ����ж�
	IF @SourceInsure IS NULL OR @SourceInsure<@lTransferScore
	BEGIN
		-- ��������
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- ������Ϣ
		SET @strErrorDescribe=N'����ǰ���е���Ϸ�����㣬��Ϸ��ת��ʧ�ܣ�'
		RETURN 4
	END

	-- ���б���
	IF @TransferRetention<>0
	BEGIN
		SET @SurplusScore=@SourceInsure-@lTransferScore
		IF @SurplusScore<@TransferRetention
		BEGIN
			-- ��������
			ROLLBACK TRAN
			SET TRANSACTION ISOLATION LEVEL READ COMMITTED

			SET @strErrorDescribe=N'�ǳ���Ǹ,ת�˺������е���������'+LTRIM(@TransferRetention)+'���'
			RETURN 7
		END
	END

	-- ��ѯ����
	DECLARE @LockKindID INT
	DECLARE @LockServerID INT
	SELECT @LockKindID=KindID, @LockServerID=ServerID FROM GameScoreLocker WHERE UserID=@dwUserID

	-- ������Ϣ
	SET @strErrorDescribe=N'ת�� '+LTRIM(STR(@lTransferScore))+' ��Ϸ�ҵ���'+@strNickName+'�������в����ɹ�������������ʻ���Ϣ��'

	-- �������
	SET @InsureRevenue=@lTransferScore*@RevenueRate/1000
	SET @VariationInsure=-@lTransferScore

	-- ˰�շⶥ
	IF @MaxTax<>0
	BEGIN
		IF @InsureRevenue > @MaxTax
			SET @InsureRevenue=@MaxTax
	END

	-- �۳����
	UPDATE GameScoreInfo SET InsureScore=InsureScore+@VariationInsure,Revenue=Revenue+@InsureRevenue WHERE UserID=@dwUserID

	-- ���ӽ��
	UPDATE GameScoreInfo SET InsureScore=InsureScore+@lTransferScore-@InsureRevenue WHERE UserID=@TargetUserID

	-- ��¼��־
	INSERT INTO RecordInsure(KindID,ServerID,SourceUserID,SourceGold,SourceBank,
		TargetUserID,TargetGold,TargetBank,SwapScore,Revenue,IsGamePlaza,TradeType,ClientIP,CollectNote)
	VALUES(@wKindID,@wServerID,@UserID,@SourceScore,@SourceInsure,@TargetUserID,@TargetScore,@TargetInsure,
		@lTransferScore,@InsureRevenue,0,3,@strClientIP,@strTransRemark)

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- ������
	SELECT @dwUserID AS UserID, @SourceScore AS SourceScore, @SourceInsure AS SourceInsure, 0 AS VariationScore,
		@VariationInsure AS VariationInsure, @InsureRevenue AS InsureRevenue, @LockKindID AS KindID, @LockServerID AS ServerID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��ѯ����
CREATE PROC GSP_GR_QueryUserInsureInfo
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
AS

-- ��������
SET NOCOUNT ON

-- ������Ϣ
DECLARE @Score BIGINT
DECLARE @Insure BIGINT
DECLARE @ServerID SMALLINT

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	DECLARE @LogonPass AS NCHAR(32)
	SELECT @LogonPass=LogonPass FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	-- �����ж�
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'�������в�ѯ���벻��ȷ��������Ϣ��ѯʧ�ܣ�'
		RETURN 1
	END

	-- ����˰��
	DECLARE @RevenueRateTake AS INT
	DECLARE @RevenueRateTransfer AS INT
	DECLARE @RevenueRateTransferMember AS INT
	DECLARE @TransferPrerequisite AS BIGINT  
	DECLARE	@EnjoinTransfer AS TINYINT
	SELECT @RevenueRateTake=StatusValue FROM QPAccountsDBLink.QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'RevenueRateTake'
	SELECT @RevenueRateTransfer=StatusValue FROM QPAccountsDBLink.QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'RevenueRateTransfer'
	SELECT @RevenueRateTransferMember=StatusValue FROM QPAccountsDBLink.QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'RevenueRateTransferMember'
	SELECT @TransferPrerequisite=StatusValue FROM QPAccountsDBLink.QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'TransferPrerequisite'
	SELECT @EnjoinTransfer=StatusValue FROM QPAccountsDBLink.QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'TransferStauts'

	-- ��������
	IF @EnjoinTransfer IS NULL SET @EnjoinTransfer=0
	IF @RevenueRateTake IS NULL SET @RevenueRateTake=1
	IF @RevenueRateTransfer IS NULL SET @RevenueRateTransfer=1	
	IF @TransferPrerequisite IS NULL SET @TransferPrerequisite=0	
	IF @RevenueRateTransferMember IS NULL SET @RevenueRateTransfer=0

	-- ��ѯ����
	SELECT @ServerID=ServerID FROM GameScoreLocker(NOLOCK) WHERE UserID=@dwUserID

	-- ��ѯ����
	SELECT @Score=Score, @Insure=InsureScore FROM GameScoreInfo(NOLOCK) WHERE UserID=@dwUserID

	-- ���ݵ���
	IF @Score IS NULL SET @Score=0
	IF @Insure IS NULL SET @Insure=0
	IF @ServerID IS NULL SET @ServerID=0

	-- ������
	SELECT @dwUserID AS UserID, @Score AS Score, @Insure AS Insure, @ServerID AS ServerID, @RevenueRateTake AS RevenueTake, 
		   @RevenueRateTransfer AS RevenueTransfer, @RevenueRateTransferMember AS RevenueTransferMember, @TransferPrerequisite AS TransferPrerequisite,
		   @EnjoinTransfer AS EnjoinTransfer

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
