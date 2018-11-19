USE QPPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_DeductMatchFee]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_DeductMatchFee]

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_ReturnMatchFee]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_ReturnMatchFee]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- �۳�����
CREATE PROC GSP_GR_DeductMatchFee
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����
	@wServerID INT,								-- ���� I D	
	@dwMatchID	INT,							-- ���� I D
	@dwMatchNO	INT,							-- �������
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
AS

-- ִ���߼�
BEGIN
	-- ��ѯ�û�
	DECLARE @LogonPassword NCHAR(32) 
	DECLARE	@UserMemberOrder TINYINT
	SELECT @UserMemberOrder=MemberOrder,@LogonPassword=LogonPass FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID
	IF @LogonPassword IS NULL
	BEGIN
		SET @strErrorDescribe = N'�����û���Ϣ������,�������ǵĿͷ���Ա��ϵ��'
		return 1				
	END

	-- У������
	IF @LogonPassword <> @strPassword
	BEGIN
		SET @strErrorDescribe = N'���ĵ�¼���벻ƥ��,ϵͳ�޷�Ϊ���ɹ�������'
		return 2		
	END

	-- ��������
	DECLARE	@cbMatchType TINYINT
	DECLARE @cbMatchFeeType SMALLINT 
	DECLARE	@cbMemberOrder TINYINT
	DECLARE @dwMatchFee BIGINT
	DECLARE @dwMatchInitScore BIGINT
	DECLARE @MatchStartTime datetime
	DECLARE	@MatchEndTime datetime	
	
	-- ��ѯ����
	SELECT @cbMatchType=a.MatchType,@cbMatchFeeType=a.MatchFeeType,@dwMatchFee=a.MatchFee,@cbMemberOrder=a.MemberOrder,
	@MatchStartTime=b.StartTime,@MatchEndTime=b.EndTime,@dwMatchInitScore=b.InitScore 
	FROM QPGameMatchDBLink.QPGameMatchDB.dbo.MatchPublic AS a,QPGameMatchDBLink.QPGameMatchDB.dbo.MatchLockTime AS b 
	WHERE a.MatchID=@dwMatchID AND a.MatchNo=@dwMatchNo AND a.MatchID=b.MatchID AND a.MatchNo=b.MatchNo
	IF @cbMatchType IS NULL OR @cbMatchFeeType IS NULL
	BEGIN
		SET @strErrorDescribe = N'û���ҵ������µ���Ϣ,���������ǵĿͷ���Ա��ϵ��'
		return 3
	END

	-- �ж�����
	IF @cbMatchType <> 0
	BEGIN
		SET @strErrorDescribe = N'��Ǹ,����������ʱֻ֧�ֶ�ʱ��������µı�����'
		return 4		
	END

	-- ��������			
	IF DATEDIFF(ss,@MatchEndTime,GetDate()) > 0
	BEGIN
		SET @strErrorDescribe = N'���������Ѿ�����,�����´����������μӣ�'
		return 5		
	END

	-- ��������
	IF @UserMemberOrder < @cbMemberOrder
	BEGIN
		SET @strErrorDescribe = N'��Ǹ,���Ļ�Ա�ȼ����������ϱ���������'
		return 6
	END

	-- �ظ�����
	IF Exists(SELECT * FROM QPGameMatchDBLink.QPGameMatchDB.dbo.StreamMatchFeeInfo 
	WHERE UserID=@dwUserID AND ServerID=@wServerID AND MatchID=@dwMatchID AND Effective=0)
	BEGIN
		SET @strErrorDescribe = N'���Ѿ������ó�����,����Ҫ�ظ�������'
		return 7		
	END	

	-- ���֧��
	IF @cbMatchFeeType=0
	BEGIN
		-- ��ѯ���
		DECLARE @Score BIGINT
		SELECT @Score=Score FROM QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
		IF @Score IS NULL
		BEGIN
			SET @strErrorDescribe = N'��Ǹ,û���ҵ����Ľ����Ϣ,���������ǵĿͷ���Ա��ϵ��'
			return 8						
		END

		-- ��Ҳ���
		IF @Score < @dwMatchFee		
		BEGIN
			SET @strErrorDescribe = N'��Ǹ,�����ϵĽ�Ҳ���,ϵͳ�޷�Ϊ���ɹ�������'
			return 9				
		END

		-- ���½��
		UPDATE QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo SET Score=@Score-@dwMatchFee WHERE UserID=@dwUserID				
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
			return 8						
		END	

		-- Ԫ������
		IF @wUserMedal < @dwMatchFee		
		BEGIN
			SET @strErrorDescribe = N'��Ǹ,�����ϵ�Ԫ������,ϵͳ�޷�Ϊ���ɹ�������'
			return 9				
		END

		-- ����Ԫ��
		UPDATE QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo SET UserMedal=@wUserMedal-@dwMatchFee WHERE UserID=@dwUserID	
	END

	-- �����¼	
	INSERT QPGameMatchDBLink.QPGameMatchDB.dbo.StreamMatchFeeInfo (UserID,ServerID,MatchID,MatchNo,MatchFeeType,MatchFee,CollectDate) 
			VALUES(@dwUserID,@wServerID,@dwMatchID,@dwMatchNO,@cbMatchFeeType,@dwMatchFee,GETDATE())

	-- ���·���
	IF Exists(SELECT * FROM QPGameMatchDBLink.QPGameMatchDB.dbo.MatchScoreInfo 
	WHERE UserID=@dwUserID AND ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@dwMatchNo)
	BEGIN
		-- ��������
		UPDATE QPGameMatchDBLink.QPGameMatchDB.dbo.MatchScoreInfo 
		SET Score=@dwMatchInitScore,WinCount=0,LostCount=0,DrawCount=0,FleeCount=0
		WHERE UserID=@dwUserID AND ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@dwMatchNo						
	END ELSE						
	BEGIN	
		-- ��������
		INSERT INTO QPGameMatchDBLink.QPGameMatchDB.dbo.MatchScoreInfo(UserID,ServerID,MatchID,MatchNo,Score)
		VALUES (@dwUserID,@wServerID,@dwMatchID,@dwMatchNo,@dwMatchInitScore)
	END	

	-- �׳���ʾ
	SET @strErrorDescribe = N'��ϲ��,�����ɹ�,���� '+convert(char(8),@MatchStartTime,108)+ N' - '+convert(char(8),@MatchEndTime,108)+N' ׼ʱ�μӱ�����'  
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------

-- �˻�����
CREATE PROC GSP_GR_ReturnMatchFee
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����
	@wServerID INT,								-- ���� I D	
	@dwMatchID	INT,							-- ���� I D
	@dwMatchNO	INT,							-- �������	
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
AS

-- ִ���߼�
BEGIN		

	-- ��ѯ�û�
	DECLARE @LogonPassword NCHAR(32) 
	SELECT @LogonPassword=LogonPass FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID
	IF @LogonPassword IS NULL
	BEGIN
		SET @strErrorDescribe = N'�����û���Ϣ������,�������ǵĿͷ���Ա��ϵ��'
		return 1				
	END

	-- У������
	IF @LogonPassword <> @strPassword
	BEGIN
		SET @strErrorDescribe = N'���ĵ�¼���벻ƥ��,ϵͳ�޷�Ϊ��ȡ��������'
		return 2		
	END

	-- ������¼
	DECLARE @MatchNo INT
	DECLARE @dwMatchFee BIGINT
	DECLARE @cbMatchFeeType SMALLINT 	
	SELECT TOP 1 @MatchNo=MatchNo,@cbMatchFeeType=MatchFeeType,@dwMatchFee=MatchFee FROM QPGameMatchDBLink.QPGameMatchDB.dbo.StreamMatchFeeInfo 
	WHERE UserID=@dwUserID AND ServerID=@wServerID AND MatchID=@dwMatchID AND Effective=0
	IF @cbMatchFeeType IS NULL OR @dwMatchFee IS NULL
	BEGIN
		SET @strErrorDescribe = N'ϵͳû���ҵ����ڸó����µı�����¼���߱����ѽ�����'
		return 3		
	END

	-- ��ѯ����
	DECLARE @MatchEndTime datetime	
	DECLARE	@MatchStartTime datetime	
	SELECT @MatchStartTime=StartTime,@MatchEndTime=EndTime FROM QPGameMatchDBLink.QPGameMatchDB.dbo.MatchLockTime 
	WHERE MatchID=@dwMatchID AND MatchNo=@dwMatchNo
	IF @MatchStartTime IS NULL OR @MatchEndTime IS NULL
	BEGIN
		SET @strErrorDescribe = N'û���ҵ������µ���Ϣ,���������ǵĿͷ���Ա��ϵ��'
		return 3		
	END

	-- ��������			
	IF DATEDIFF(ss,@MatchEndTime,GetDate()) > 0
	BEGIN
		SET @strErrorDescribe = N'���������Ѿ�����,�޷�Ϊ��ȡ��������'
		return 5		
	END
	
	-- ������ʼ
	IF DATEDIFF(ss,@MatchStartTime,GetDate()) > 0
	BEGIN
		-- ͳ�ƾ���
		DECLARE @PlayTotalCount INT
		SELECT @PlayTotalCount=WinCount+LostCount+DrawCount+FleeCount FROM QPGameMatchDBLink.QPGameMatchDB.dbo.MatchScoreInfo
		WHERE UserID=@dwUserID AND ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@dwMatchNo

		-- �Ѳ�����ȡ��
		IF @PlayTotalCount > 0 OR @dwMatchNo < 1
		BEGIN 		
			SET @strErrorDescribe = N'��Ǹ,���Ѳμӹ��������µı���,����ȡ��������'
			return 6		
		END
	END

	-- ��������
	IF @MatchNo=@dwMatchNo
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
				return 8						
			END

			-- ���½��
			UPDATE QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo SET Score=@Score+@dwMatchFee WHERE UserID=@dwUserID				
		END

		-- Ԫ��֧��
		IF @cbMatchFeeType=1
		BEGIN
			-- ��ѯ���
			DECLARE @UserMedal BIGINT
			SELECT @UserMedal=UserMedal FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID
			IF @UserMedal IS NULL
			BEGIN
				SET @strErrorDescribe = N'û���ҵ�����Ԫ����Ϣ,���������ǵĿͷ���Ա��ϵ��'
				return 9						
			END	

			-- ����Ԫ��
			UPDATE QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo SET UserMedal=@UserMedal+@dwMatchFee WHERE UserID=@dwUserID	
		END			
	END

	-- ɾ����¼
	DELETE QPGameMatchDBLink.QPGameMatchDB.dbo.StreamMatchFeeInfo 
	WHERE UserID=@dwUserID AND ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@MatchNo AND Effective=0	

	-- ���·���
	UPDATE QPGameMatchDBLink.QPGameMatchDB.dbo.MatchScoreInfo 
	SET Score=0,WinCount=0,LostCount=0,DrawCount=0,FleeCount=0,UserRight=0
	WHERE UserID=@dwUserID AND ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@dwMatchNo	
	
	-- �׳���ʾ
	SET @strErrorDescribe = N'ϵͳ��Ϊ���ɹ�ȡ��������'
END

RETURN 0
GO