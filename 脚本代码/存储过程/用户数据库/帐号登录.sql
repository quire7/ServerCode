
----------------------------------------------------------------------------------------------------

USE QPAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_EfficacyAccounts]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_EfficacyAccounts]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_EfficacyAccounts]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_EfficacyAccounts]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- �ʺŵ�¼
CREATE PROC GSP_GP_EfficacyAccounts
	@strAccounts NVARCHAR(31),					-- �û��ʺ�
	@strPassword NCHAR(32),						-- �û�����
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@nNeeValidateMBCard BIT,					-- �ܱ�У��
	@strPassPortID NVARCHAR(19),				-- ֤��У��
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
AS

-- ��������
SET NOCOUNT ON

-- ������Ϣ
DECLARE @UserID INT
DECLARE @CustomID INT
DECLARE @FaceID SMALLINT
DECLARE @Accounts NVARCHAR(31)
DECLARE @NickName NVARCHAR(31)
DECLARE @UnderWrite NVARCHAR(63)
DECLARE @SpreaderID INT
DECLARE @PlayTimeCount INT

-- �Ƹ�����
DECLARE @Score BIGINT
DECLARE @Insure BIGINT
DECLARE @Beans decimal(18, 2)

-- ��չ��Ϣ
DECLARE @GameID INT
DECLARE @Gender TINYINT
DECLARE @UserMedal INT
DECLARE @Experience INT
DECLARE @LoveLiness INT
DECLARE @MemberOrder SMALLINT
DECLARE @MemberOverDate DATETIME
DECLARE @MemberSwitchDate DATETIME
DECLARE @ProtectID INT
DECLARE @PasswordID INT

-- ��������
DECLARE @EnjoinLogon AS INT

-- ִ���߼�
BEGIN
	-- ϵͳ��ͣ
	SELECT @EnjoinLogon=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
		RETURN 2
	END

	-- Ч���ַ
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND (EnjoinOverDate>GETDATE() OR EnjoinOverDate IS NULL)
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�������ڵ� IP ��ַ�ĵ�¼���ܣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 4
	END
	
	-- Ч�����
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND (EnjoinOverDate>GETDATE() OR EnjoinOverDate IS NULL)
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�����Ļ����ĵ�¼���ܣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 7
	END
 
	-- ��ѯ�û�
	DECLARE @Nullity TINYINT
	DECLARE @StunDown TINYINT
	DECLARE @LogonPass AS NCHAR(32)
	DECLARE @InsurePass AS NCHAR(32)
	DECLARE	@MachineSerial NCHAR(32)
	DECLARe @strLastLogonIP NCHAR(15)
	DECLARE @MoorMachine AS TINYINT
	SELECT @UserID=UserID, @GameID=GameID, @Accounts=Accounts, @NickName=NickName, @UnderWrite=UnderWrite, @LogonPass=LogonPass,@InsurePass=InsurePass,
		@FaceID=FaceID, @CustomID=CustomID, @Gender=Gender, @Nullity=Nullity, @StunDown=StunDown, @UserMedal=UserMedal, @Experience=Experience,
		@LoveLiness=LoveLiness, @MemberOrder=MemberOrder, @MemberOverDate=MemberOverDate, @MemberSwitchDate=MemberSwitchDate,@strLastLogonIP=LastLogonIP,
		@MoorMachine=MoorMachine, @MachineSerial=LastLogonMachine,@SpreaderID=SpreaderID,@PlayTimeCount=PlayTimeCount,@PasswordID=PasswordID,@ProtectID=ProtectID
	FROM AccountsInfo(NOLOCK) WHERE Accounts=@strAccounts

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
		IF @MachineSerial<>@strMachineID
		BEGIN
			SET @strErrorDescribe=N'�����ʺ�ʹ�ù̶�������¼���ܣ�������ʹ�õĻ���������ָ���Ļ�����'
			RETURN 1
		END
	END

	-- �����ж�
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ������������������֤���ٴγ��Ե�¼��'
		RETURN 3
	END

	-- �ܱ�У��
	IF @nNeeValidateMBCard=1 AND @PasswordID<>0
	BEGIN
		SELECT @PasswordID AS PasswordID
		RETURN 18
	END

	-- ֤��У��
	IF @strLastLogonIP <> @strClientIP
	BEGIN
		DECLARE @RegisterPassPortID NVARCHAR(19)
		SELECT @RegisterPassPortID=PassportID FROM AccountsProtect WHERE UserID=@UserID
		IF @strPassPortID <> @RegisterPassPortID
		BEGIN
			IF @strPassPortID<>N''
			BEGIN
				SET @strErrorDescribe=N'����������֤��������ע��ʱ��д�Ĳ�һ�£�'		
				RETURN 1		
			END
			RETURN 19
		END				
	END

	-- �̶�����
	IF @MoorMachine=2
	BEGIN
		SET @MoorMachine=1
		SET @strErrorDescribe=N'�����ʺųɹ�ʹ���˹̶�������¼���ܣ�'
		UPDATE AccountsInfo SET MoorMachine=@MoorMachine, LastLogonMachine=@strMachineID WHERE UserID=@UserID
	END

	-- �ƹ�Ա���
	IF @SpreaderID<>0 
	BEGIN
		DECLARE @GrantTime	INT
		DECLARE @GrantScore	BIGINT
		DECLARE @Note NVARCHAR(512)
		SET @Note = N'��Ϸʱ�����һ���Խ���'

		SELECT @GrantTime=PlayTimeCount,@GrantScore=PlayTimeGrantScore FROM QPTreasureDBLink.QPTreasureDB.dbo.GlobalSpreadInfo
		WHERE ID=1
		IF @GrantTime IS NULL OR @GrantTime=0
		BEGIN
			SET @GrantTime = 108000 -- 30Сʱ
			SET @GrantScore = 200000
		END			
		IF @PlayTimeCount>=@GrantTime
		BEGIN
			-- ��ȡ�����Ϣ
			DECLARE @RecordID INT
			SELECT @RecordID=RecordID FROM QPTreasureDBLink.QPTreasureDB.dbo.RecordSpreadInfo
			WHERE UserID = @SpreaderID AND ChildrenID = @UserID AND TypeID = 2
			
			IF @RecordID IS NULL
			BEGIN
				INSERT INTO QPTreasureDBLink.QPTreasureDB.dbo.RecordSpreadInfo(
					UserID,Score,TypeID,ChildrenID,CollectNote)
				VALUES(@SpreaderID,@GrantScore,2,@UserID,@Note)	
			END		
		END
	END

	-- ��ѯ���
	SELECT @Score=Score, @Insure=InsureScore FROM QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo WHERE UserID=@UserID

	-- ��ѯ��Ϸ��
	SELECT @Beans=Currency FROM QPTreasureDBLink.QPTreasureDB.dbo.UserCurrencyInfo WHERE UserID=@UserID		
	
	-- ���ݵ���
	IF @Score IS NULL SET @Score=0
	IF @Insure IS NULL SET @Insure=0
	IF @Beans IS NULL SET @Beans=0

	-- ��Ա�ȼ�
	IF @MemberOrder<>0 AND GETDATE()>@MemberSwitchDate
	BEGIN
		DECLARE @UserRight INT	
		SET @UserRight=0
		
		-- ɾ����Ա
		DELETE AccountsMember WHERE UserID=@UserID AND MemberOverDate<=GETDATE()

		-- ������Ա
		SELECT @MemberOverDate=MAX(MemberOverDate), @MemberOrder=MAX(MemberOrder), @MemberSwitchDate=MIN(MemberOverDate)
			FROM AccountsMember(NOLOCK) WHERE UserID=@UserID

		-- ���ݵ���
		IF @MemberOrder IS NULL 
		BEGIN
			SET @MemberOrder=0
			SET @UserRight=512
		END
		IF @MemberOverDate IS NULL SET @MemberOverDate='1980-1-1'
		IF @MemberSwitchDate IS NULL SET @MemberSwitchDate='1980-1-1'

		-- ��������
		UPDATE AccountsInfo SET MemberOrder=@MemberOrder, MemberOverDate=@MemberOverDate, MemberSwitchDate=@MemberSwitchDate,
			UserRight=UserRight&~@UserRight WHERE UserID=@UserID
	END

	-- ������Ϣ
	UPDATE AccountsInfo SET GameLogonTimes=GameLogonTimes+1,LastLogonDate=GETDATE(), LastLogonIP=@strClientIP,DynamicPassTime=GETDATE(),
		DynamicPass=CONVERT(nvarchar(32),REPLACE(newid(),'-','')),LastLogonMachine=@strMachineID 
	WHERE UserID=@UserID

	-- ��̬����
	DECLARE @szDynamicPass  nchar(32)
	SELECT @szDynamicPass=DynamicPass FROM AccountsInfo WHERE UserID=@UserID

	-- ��¼��־
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
	UPDATE SystemStreamInfo SET GameLogonSuccess=GameLogonSuccess+1 WHERE DateID=@DateID
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, GameLogonSuccess) VALUES (@DateID, 1)

	-- ���б�ʶ
	DECLARE @InsureEnabled TINYINT
	SET @InsureEnabled=0
	IF @InsurePass <> '' SET @InsureEnabled=1
	
	-- �������
	SELECT @UserID AS UserID, @GameID AS GameID, @Accounts AS Accounts, @NickName AS NickName,@szDynamicPass AS DynamicPass,
		@UnderWrite AS UnderWrite,@FaceID AS FaceID, @CustomID AS CustomID, @Gender AS Gender, @UserMedal AS Ingot, 
		@Experience AS Experience, @Score AS Score, @Insure AS Insure, @Beans AS Beans, @LoveLiness AS LoveLiness, @MemberOrder AS MemberOrder, 
		@MemberOverDate AS MemberOverDate, @MoorMachine AS MoorMachine, @PasswordID as PasswordID, @InsureEnabled AS InsureEnabled
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- �ʺŵ�¼
CREATE PROC GSP_MB_EfficacyAccounts
	@strAccounts NVARCHAR(31),					-- �û��ʺ�
	@strPassword NCHAR(32),						-- �û�����
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@strMobilePhone NVARCHAR(11),				-- �ֻ�����
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
AS

-- ��������
SET NOCOUNT ON

-- ������Ϣ
DECLARE @UserID INT
DECLARE @CustomID INT
DECLARE @FaceID SMALLINT
DECLARE @Accounts NVARCHAR(31)
DECLARE @NickName NVARCHAR(31)
DECLARE @UnderWrite NVARCHAR(63)
DECLARE @SpreaderID INT
DECLARE @PlayTimeCount INT

-- �Ƹ�����
DECLARE @Score BIGINT
DECLARE @Insure BIGINT
DECLARE @Beans decimal(18, 2)

-- ��չ��Ϣ
DECLARE @GameID INT
DECLARE @Gender TINYINT
DECLARE @UserMedal INT
DECLARE @Experience INT
DECLARE @LoveLiness INT
DECLARE @MemberOrder SMALLINT
DECLARE @MemberOverDate DATETIME
DECLARE @MemberSwitchDate DATETIME

-- ��������
DECLARE @EnjoinLogon AS INT

-- ִ���߼�
BEGIN
	-- ϵͳ��ͣ
	SELECT @EnjoinLogon=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
		RETURN 2
	END

	-- Ч���ַ
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�������ڵ� IP ��ַ�ĵ�¼���ܣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 4
	END
	
	-- Ч�����
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�����Ļ����ĵ�¼���ܣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 7
	END

	-- ��ѯ�û�
	DECLARE @Nullity TINYINT
	DECLARE @StunDown TINYINT
	DECLARE @LogonPass AS NCHAR(32)
	DECLARE @InsurePass AS NCHAR(32)
	DECLARE @MoorMachine AS TINYINT
	SELECT @UserID=UserID, @GameID=GameID, @Accounts=Accounts, @NickName=NickName, @UnderWrite=UnderWrite, @LogonPass=LogonPass,@InsurePass=InsurePass,
		@FaceID=FaceID, @CustomID=CustomID, @Gender=Gender, @Nullity=Nullity, @StunDown=StunDown, @UserMedal=UserMedal, @Experience=Experience,
		@LoveLiness=LoveLiness, @MemberOrder=MemberOrder, @MemberOverDate=MemberOverDate, @MemberSwitchDate=MemberSwitchDate,
		@MoorMachine=MoorMachine,@SpreaderID=SpreaderID,@PlayTimeCount=PlayTimeCount
	FROM AccountsInfo(NOLOCK) WHERE Accounts=@strAccounts

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
	IF @MoorMachine <> 0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ�ʹ�ù̶�������¼���ܣ�������ʹ���ֻ��ն˽��е�¼��'
		RETURN 1
	END

	-- �����ж�
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ������������������֤���ٴγ��Ե�¼��'
		RETURN 3
	END

	-- �ƹ�Ա���
	IF @SpreaderID<>0 
	BEGIN
		DECLARE @GrantTime	INT
		DECLARE @GrantScore	BIGINT
		DECLARE @Note NVARCHAR(512)
		SET @Note = N'��Ϸʱ�����һ���Խ���'

		SELECT @GrantTime=PlayTimeCount,@GrantScore=PlayTimeGrantScore FROM QPTreasureDBLink.QPTreasureDB.dbo.GlobalSpreadInfo
		WHERE ID=1
		IF @GrantTime IS NULL OR @GrantTime=0
		BEGIN
			SET @GrantTime = 108000 -- 30Сʱ
			SET @GrantScore = 200000
		END			
		IF @PlayTimeCount>=@GrantTime
		BEGIN
			-- ��ȡ�����Ϣ
			DECLARE @RecordID INT
			SELECT @RecordID=RecordID FROM QPTreasureDBLink.QPTreasureDB.dbo.RecordSpreadInfo
			WHERE UserID = @SpreaderID AND ChildrenID = @UserID AND TypeID = 2
			IF @RecordID IS NULL
			BEGIN
				INSERT INTO QPTreasureDBLink.QPTreasureDB.dbo.RecordSpreadInfo(
					UserID,Score,TypeID,ChildrenID,CollectNote)
				VALUES(@SpreaderID,@GrantScore,2,@UserID,@Note)	
			END		
		END
	END

	-- ��ѯ���
	SELECT @Score=Score, @Insure=InsureScore FROM QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo WHERE UserID=@UserID

	-- ��ѯ��Ϸ��
	SELECT @Beans=Currency FROM QPTreasureDBLink.QPTreasureDB.dbo.UserCurrencyInfo WHERE UserID=@UserID		
	
	-- ���ݵ���
	IF @Score IS NULL SET @Score=0
	IF @Insure IS NULL SET @Insure=0
	IF @Beans IS NULL SET @Beans=0

	-- ��Ա�ȼ�
	IF @MemberOrder<>0 AND GETDATE()>@MemberSwitchDate
	BEGIN
		DECLARE @UserRight INT	
		SET @UserRight=0
		
		-- ɾ����Ա
		DELETE AccountsMember WHERE UserID=@UserID AND MemberOverDate<=GETDATE()

		-- ������Ա
		SELECT @MemberOverDate=MAX(MemberOverDate), @MemberOrder=MAX(MemberOrder), @MemberSwitchDate=MIN(MemberOverDate)
			FROM AccountsMember(NOLOCK) WHERE UserID=@UserID

		-- ���ݵ���
		IF @MemberOrder IS NULL 
		BEGIN
			SET @MemberOrder=0
			SET @UserRight=512
		END
		IF @MemberOverDate IS NULL SET @MemberOverDate='1980-1-1'
		IF @MemberSwitchDate IS NULL SET @MemberSwitchDate='1980-1-1'

		-- ��������
		UPDATE AccountsInfo SET MemberOrder=@MemberOrder, MemberOverDate=@MemberOverDate, MemberSwitchDate=@MemberSwitchDate,
			UserRight=UserRight&~@UserRight WHERE UserID=@UserID
	END

	-- ������Ϣ
	UPDATE AccountsInfo SET GameLogonTimes=GameLogonTimes+1, LastLogonDate=GETDATE(), LastLogonIP=@strClientIP,DynamicPassTime=GETDATE(),
		DynamicPass=CONVERT(nvarchar(32),REPLACE(newid(),'-','')),LastLogonMachine=@strMachineID, LastLogonMobile=@strMobilePhone 
	WHERE UserID=@UserID

	-- ��̬����
	DECLARE @szDynamicPass  nchar(32)
	SELECT @szDynamicPass=DynamicPass FROM AccountsInfo WHERE UserID=@UserID

	-- ��¼��־
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
	UPDATE SystemStreamInfo SET GameLogonSuccess=GameLogonSuccess+1 WHERE DateID=@DateID
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, GameLogonSuccess) VALUES (@DateID, 1)

	-- ���б�ʶ
	DECLARE @InsureEnabled TINYINT
	SET @InsureEnabled=0
	IF @InsurePass <> '' SET @InsureEnabled=1

	-- �������
	SELECT @UserID AS UserID, @GameID AS GameID, @Accounts AS Accounts, @NickName AS NickName,@szDynamicPass AS DynamicPass,
		@UnderWrite AS UnderWrite,@FaceID AS FaceID, @CustomID AS CustomID, @Gender AS Gender, @UserMedal AS Ingot,		
		@Experience AS Experience,@Score AS Score, @Insure AS Insure, @Beans AS Beans, @LoveLiness AS LoveLiness, @MemberOrder AS MemberOrder, 
		@MemberOverDate AS MemberOverDate,@MoorMachine AS MoorMachine, @InsureEnabled AS InsureEnabled

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
