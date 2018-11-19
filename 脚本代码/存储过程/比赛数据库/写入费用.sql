USE QPGameMatchDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UserMatchFee]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UserMatchFee]

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UserMatchQuit]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UserMatchQuit]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- �۳�����
CREATE PROC GSP_GR_UserMatchFee
	@dwUserID INT,								-- �û� I D
	@lMatchFee BIGINT,							-- ��������
	@wKindID INT,								-- ��Ϸ I D
	@wServerID INT,								-- ���� I D
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@dwMatchID	INT,							-- ���� I D
	@dwMatchNo	INT,							-- ��������
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN	
	-- ������Ϣ
	DECLARE @cbMatchFeeType TINYINT
	DECLARE @cbMemberOrder TINYINT		
	DECLARE @InitialScore BIGINT
	SELECT @cbMemberOrder=a.MemberOrder,@cbMatchFeeType=a.MatchFeeType,@InitialScore=b.InitialScore FROM MatchPublic(NOLOCK) a,MatchImmediate b
	WHERE a.MatchID=@dwMatchID AND a.MatchID=b.MatchID
	IF @cbMemberOrder IS NULL OR @cbMatchFeeType IS NULL
	BEGIN
		SET @strErrorDescribe = N'��Ǹ,�������ı��������ڣ�'
		return 1		
	END
	
	-- �û���Ϣ
	DECLARE @IsAndroidUser TINYINT
	DECLARE @UserMemberOrder TINYINT
	SELECT @IsAndroidUser=IsAndroid,@UserMemberOrder=MemberOrder FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID
	
	-- �û�У��
	IF @UserMemberOrder IS NULL
	BEGIN
		SET @strErrorDescribe = N'��Ǹ,�����û���Ϣ�����ڣ�����ϵ�ͷ����ģ�'
		return 2				
	END

	-- ��������
	IF @IsAndroidUser=0
	BEGIN
		-- ��������
		IF @UserMemberOrder<@cbMemberOrder	
		BEGIN
			SET @strErrorDescribe = N'��Ǹ,���ĵȼ����������ϱ���������'
			return 3		
		END

		-- �۳�����
		IF @lMatchFee>0
		BEGIN	
			-- ���֧��
			IF @cbMatchFeeType=0
			BEGIN
				-- ��ѯ���
				DECLARE @Score BIGINT
				SELECT @Score=Score FROM QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
				IF @Score IS NULL
				BEGIN
					SET @strErrorDescribe = N'��Ǹ,û���ҵ����Ľ����Ϣ,���������ǵĿͷ���Ա��ϵ��'
					return 4						
				END

				-- ��Ҳ���
				IF @Score < @lMatchFee		
				BEGIN
					SET @strErrorDescribe = N'��Ǹ,�����ϵĽ�Ҳ���,ϵͳ�޷�Ϊ���ɹ�������'
					return 5				
				END

				-- ���½��
				UPDATE QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo SET Score=@Score-@lMatchFee WHERE UserID=@dwUserID				
			END

			-- Ԫ��֧��
			IF @cbMatchFeeType=1
			BEGIN
				-- ��ѯԪ��
				DECLARE @wUserMedal BIGINT
				SELECT @wUserMedal=UserMedal FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID
				IF @wUserMedal IS NULL
				BEGIN
					SET @strErrorDescribe = N'��Ǹ,û���ҵ�����Ԫ����Ϣ,���������ǵĿͷ���Ա��ϵ��'
					return 4						
				END	

				-- Ԫ������
				IF @wUserMedal < @lMatchFee		
				BEGIN
					SET @strErrorDescribe = N'��Ǹ,�����ϵ�Ԫ������,ϵͳ�޷�Ϊ���ɹ�������'
					return 5				
				END

				-- ����Ԫ��
				UPDATE QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo SET UserMedal=@wUserMedal-@lMatchFee WHERE UserID=@dwUserID	
			END
		END
	END

	-- ���·���
	IF exists(SELECT * FROM MatchScoreInfo(NOLOCK) WHERE UserID=@dwUserID AND ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@dwMatchNo)
	BEGIN
		UPDATE MatchScoreInfo SET Score=@InitialScore,WinCount=0,LostCount=0,DrawCount=0,FleeCount=0,PlayTimeCount=0,OnlineTime=0,
		UserRight=0x10000000,SignupTime=GetDate()
		WHERE UserID=@dwUserID AND ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@dwMatchNo				
	END	ELSE
	BEGIN
		INSERT INTO MatchScoreInfo (UserID,ServerID,MatchID,MatchNo,Score,UserRight) 
		VALUES (@dwUserID,@wServerID,@dwMatchID,@dwMatchNo,@InitialScore,0x10000000)				
	END				

	-- �����¼	
	IF @IsAndroidUser=0
	BEGIN
		INSERT StreamMatchFeeInfo (UserID,ServerID,MatchID,MatchNo,MatchType,MatchFeeType,MatchFee,CollectDate) 
		VALUES(@dwUserID,@wServerID,@dwMatchID,@dwMatchNo,1,@cbMatchFeeType,@lMatchFee,GETDATE())
	END

	-- ��ѯ���
	DECLARE @CurrScore BIGINT
	SELECT @CurrScore=Score FROM QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID		
	
	-- ��ѯԪ��
	DECLARE @CurrIngot BIGINT	
	SELECT @CurrIngot=UserMedal FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID
	
	-- ��������
	IF @CurrScore IS NULL SET @CurrScore=0
	IF @CurrIngot IS NULL SET @CurrIngot=0

	-- �׳�����
	SELECT 	@CurrScore AS Score,@CurrIngot AS Ingot		
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------

-- �˻�����
CREATE PROC GSP_GR_UserMatchQuit
	@dwUserID INT,								-- �û� I D
	@lMatchFee BIGINT,							-- ��������
	@wKindID INT,								-- ��Ϸ I D
	@wServerID INT,								-- ���� I D
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@dwMatchID	INT,							-- ���� I D
	@dwMatchNo	INT,							-- ��������
	@strMachineID NVARCHAR(32)	,				-- ������ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- ������Ϣ
	DECLARE @cbMatchFeeType TINYINT
	SELECT @cbMatchFeeType=MatchFeeType FROM MatchPublic WHERE MatchID=@dwMatchID
	IF @cbMatchFeeType IS NULL
	BEGIN
		SET @strErrorDescribe = N'��Ǹ,�������ı��������ڣ�'
		return 1		
	END

	-- �û���Ϣ
	DECLARE @IsAndroidUser TINYINT	
	SELECT @IsAndroidUser=IsAndroid FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	-- �����ж�
	IF @IsAndroidUser IS NULL
	BEGIN
		SET @strErrorDescribe = N'��Ǹ,�����û���Ϣ�����ڣ�'
		return 2		
	END

	-- ��������
	IF @IsAndroidUser=0 AND @lMatchFee>0
	BEGIN
		-- ���֧��
		IF @cbMatchFeeType=0
		BEGIN
			-- ��ѯ���
			DECLARE @Score BIGINT
			SELECT @Score=Score FROM QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
			IF @Score IS NULL
			BEGIN
				SET @strErrorDescribe = N'û���ҵ����Ľ����Ϣ,���������ǵĿͷ���Ա��ϵ��'
				return 3						
			END

			-- ���½��
			UPDATE QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo SET Score=@Score+@lMatchFee WHERE UserID=@dwUserID				
		END

		-- ����֧��
		IF @cbMatchFeeType=1
		BEGIN
			-- ��ѯԪ��
			DECLARE @UserMedal BIGINT
			SELECT @UserMedal=UserMedal FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID
			IF @UserMedal IS NULL
			BEGIN
				SET @strErrorDescribe = N'û���ҵ�����Ԫ����Ϣ,���������ǵĿͷ���Ա��ϵ��'
				return 4						
			END	

			-- ����Ԫ��
			UPDATE QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo SET UserMedal=@UserMedal+@lMatchFee WHERE UserID=@dwUserID	
		END	
	END

	-- ɾ����¼
	IF @IsAndroidUser=0 
	BEGIN
		DELETE StreamMatchFeeInfo WHERE UserID=@dwUserID AND ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@dwMatchNo
	END

	-- ��ѯ���
	DECLARE @CurrScore BIGINT
	SELECT @CurrScore=Score FROM QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID		
	
	-- ��ѯԪ��
	DECLARE @CurrIngot BIGINT	
	SELECT @CurrIngot=UserMedal FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	-- ��������
	IF @CurrScore IS NULL SET @CurrScore=0
	IF @CurrIngot IS NULL SET @CurrIngot=0

	-- �׳�����
	SELECT 	@CurrScore AS Score,@CurrIngot AS Ingot
END

RETURN 0
GO