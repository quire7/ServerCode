
----------------------------------------------------------------------------------------------------

USE QPGameScoreDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadGameProperty]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadGameProperty]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_ConsumeProperty]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_ConsumeProperty]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ���ص���
CREATE PROC GSP_GR_LoadGameProperty
	@wKindID SMALLINT,							-- ��Ϸ I D
	@wServerID SMALLINT							-- ���� I D
AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ���ص���
	SELECT ID, IssueArea, Cash, Gold, Discount, SendLoveLiness, RecvLoveLiness FROM GameProperty(NOLOCK)
	WHERE Nullity=0 AND (IssueArea&6)<>0

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ���ѵ���
CREATE PROC GSP_GR_ConsumeProperty

	-- ������Ϣ
	@dwSourceUserID INT,						-- �û���ʶ
	@dwTargetUserID INT,						-- �û���ʶ
	@wPropertyID INT,							-- ���߱�ʶ
	@wPropertyCount INT,						-- ������Ŀ

	-- ��������
	@wKindID INT,								-- ��Ϸ��ʶ
	@wServerID INT,								-- �����ʶ
	@wTableID INT,								-- ���ӱ�ʾ

	-- ����ʽ
	@cbConsumeScore BIT,						-- ��������
	@lFrozenedScore BIGINT,						-- �������

	-- ϵͳ��Ϣ
	@dwEnterID INT,								-- ��������
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ

AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ��Ա
	DECLARE @Nullity BIT
	DECLARE @CurrMemberOrder SMALLINT	
	SELECT @Nullity=Nullity, @CurrMemberOrder=MemberOrder FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwSourceUserID

	-- �û��ж�
	IF @CurrMemberOrder IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 1
	END	

	-- �ʺŽ�ֹ
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����'
		RETURN 2
	END

	-- Ŀ���û�
	IF @dwTargetUserID<>0 AND NOT EXISTS( SELECT UserID FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwTargetUserID )
	BEGIN
		SET @strErrorDescribe=N'�������͵�Ŀ���û������ڣ����֤���ٴγ��ԣ�'
		RETURN 3
	END

	-- ��������
	DECLARE @Gold AS BIGINT
	DECLARE @Discount AS SMALLINT
	DECLARE @SendLoveLiness AS BIGINT
	DECLARE @RecvLoveLiness AS BIGINT
	DECLARE @EffectRows AS INT

	-- �����ж�
	SELECT @Gold=Gold, @Discount=Discount, @SendLoveLiness=SendLoveLiness, @RecvLoveLiness=RecvLoveLiness
	FROM GameProperty(NOLOCK) WHERE Nullity=0 AND ID=@wPropertyID

	-- �����ж�
	IF @SendLoveLiness IS NULL
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ������������ĵ������ﲻ���ڻ�������ά���У�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 4
	END

	-- ���ü���
	DECLARE @ConsumeGold BIGINT
	IF @CurrMemberOrder=0 SELECT @ConsumeGold=@Gold*@wPropertyCount
	ELSE SELECT @ConsumeGold=@Gold*@wPropertyCount*@Discount/100

	-- ���п۷�
	IF @cbConsumeScore=0
	BEGIN
		-- ��ȡ����
		DECLARE @Insure BIGINT
		SELECT @Insure=InsureScore FROM QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwSourceUserID
	
		-- �����ж�
		IF @Insure<@ConsumeGold
		BEGIN
			-- ������Ϣ
			SET @strErrorDescribe=N'���ı��չ���Ϸ�����㣬��ѡ�������Ĺ���ʽ���������չ�����㹻����Ϸ�Һ��ٴγ��ԣ�'
			RETURN 5
		END

		-- ���п۷�
		UPDATE QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo SET InsureScore=InsureScore-@ConsumeGold WHERE UserID=@dwSourceUserID	
	END
	ELSE
	BEGIN

		-- ��ѯ����
		DECLARE @EnterID INT
		DECLARE @LockKindID INT
		DECLARE @LockServerID INT
		SELECT @LockKindID=KindID, @LockServerID=ServerID, @EnterID=EnterID FROM GameScoreLocker WHERE UserID=@dwSourceUserID

		-- �����ж�
		IF @LockKindID<>@wKindID OR @LockServerID<>@wServerID OR @dwEnterID<>@EnterID
		BEGIN
			-- ������Ϣ
			SET @strErrorDescribe=N'���ķ���������¼��ϢЧ��ʧ�ܣ�����ϵ�ͻ����������˽���ϸ�����'
			RETURN 5
		END

		-- �����ж�
		IF @lFrozenedScore<@ConsumeGold
		BEGIN
			-- ������Ϣ
			SET @strErrorDescribe=N'����ǰ����Ϸ�����㣬��ѡ�������Ĺ���ʽ���߳�ֵ���ٴγ��ԣ�'
			RETURN 5
		END

	END
	
	-- ��������
	IF @wPropertyID=16
	BEGIN
		UPDATE GameScoreInfo SET Score=0 WHERE UserID=@dwTargetUserID			
	END

	-- ��Ա��
	IF @wPropertyID >= 22 AND @wPropertyID <= 26
	BEGIN
		-- ��Ա����
		DECLARE @MemberOrder		 INT
		DECLARE @MemberRight		 INT
		DECLARE @MemberValidMonth	 INT			
		
		DECLARE @MaxMemberOrder INT
		DECLARE @MaxUserRight INT
		DECLARE @MemberOverDate DATETIME
		DECLARE @MemberSwitchDate DATETIME

		-- ���ñ���
		SET @MemberOrder=@wPropertyID-22+1
		SET @MemberRight=512

		-- ��Ч����
		SELECT @MemberValidMonth= @wPropertyCount * 1

		-- ɾ������
		DELETE FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsMember
		WHERE UserID=@dwTargetUserID AND MemberOrder=@MemberOrder AND MemberOverDate<=GETDATE()

		-- ���»�Ա
		UPDATE QPAccountsDBLink.QPAccountsDB.dbo.AccountsMember SET MemberOverDate=DATEADD(mm,@MemberValidMonth, GETDATE())
		WHERE UserID=@dwTargetUserID AND MemberOrder=@MemberOrder
		IF @@ROWCOUNT=0
		BEGIN
			INSERT QPAccountsDBLink.QPAccountsDB.dbo.AccountsMember(UserID,MemberOrder,UserRight,MemberOverDate)
			VALUES (@dwTargetUserID,@MemberOrder,@MemberRight,DATEADD(mm,@MemberValidMonth, GETDATE()))
		END

		-- �󶨻�Ա,(��Ա�������л�ʱ��)
		SELECT @MaxMemberOrder=MAX(MemberOrder),@MemberOverDate=MAX(MemberOverDate),@MemberSwitchDate=MIN(MemberOverDate)
		FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsMember WHERE UserID=@dwTargetUserID

		-- ��ԱȨ��
		SELECT @MaxUserRight=UserRight FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsMember
		WHERE UserID=@dwTargetUserID AND MemberOrder=@MaxMemberOrder
		
		-- ���ӻ�Ա����Ϣ
		UPDATE QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo
		SET MemberOrder=@MaxMemberOrder,UserRight=@MaxUserRight,MemberOverDate=@MemberOverDate,MemberSwitchDate=@MemberSwitchDate
		WHERE UserID=@dwTargetUserID
	END

	-- ��Ա�ȼ�
	SELECT @CurrMemberOrder=MemberOrder FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwTargetUserID

	-- �����¼
	IF @wPropertyID<=13 -- ����
	BEGIN
		INSERT QPRecordDBLink.QPRecordDB.dbo.RecordSendPresent(PresentID,RcvUserID,SendUserID,LovelinessRcv,LovelinessSend,PresentPrice,PresentCount,KindID,ServerID,SendTime,ClientIP)
		VALUES(@wPropertyID,@dwTargetUserID,@dwSourceUserID,@RecvLoveLiness,@SendLoveLiness,@Gold,@wPropertyCount,@wKindID,@wServerID,GETDATE(),@strClientIP)
	END
	ELSE
	BEGIN -- ����
		INSERT QPRecordDBLink.QPRecordDB.dbo.RecordSendProperty(PropID,SourceUserID,TargetUserID,PropPrice,PropCount,KindID,ServerID,SendTime,ClientIP)
		VALUES(@wPropertyID,@dwSourceUserID,@dwTargetUserID,@Gold,@wPropertyCount,@wKindID,@wServerID,GETDATE(),@strClientIP)		
	END

	-- �����¼
	SELECT @ConsumeGold AS ConsumeGold, @SendLoveLiness*@wPropertyCount AS SendLoveLiness, @RecvLoveLiness*@wPropertyCount AS RecvLoveLiness,@CurrMemberOrder AS MemberOrder

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------