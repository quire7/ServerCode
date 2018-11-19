USE QPTreasureDB
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
	@dwMatchFee INT,							-- ��������
	@wKindID INT,								-- ��Ϸ I D
	@wServerID INT,								-- ���� I D
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@dwMatchID	INT,							-- ���� I D
	@dwMatchNO	INT,							-- ��������
	@strMachineID NVARCHAR(32)	,				-- ������ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
AS

-- ִ���߼�
BEGIN	
	DECLARE @UserRight INT 
	DECLARE @ReturnValue INT
	SET @ReturnValue=0
	SELECT @UserRight=UserRight FROM QPAccountsDB..AccountsInfo WHERE UserID=@dwUserID 
	SELECT @UserRight=@UserRight|UserRight FROM GameScoreInfo WHERE UserID=@dwUserID

	IF EXISTS(SELECT * FROM QPTreasureDB..GameScoreInfo WHERE UserID=@dwUserID)
	BEGIN
		DECLARE @Score BIGINT
		SELECT @Score=Score FROM QPTreasureDB..GameScoreInfo WHERE UserID=@dwUserID
		IF @Score < @dwMatchFee
		BEGIN
			SET @ReturnValue=1
		END
		ELSE
		BEGIN
			UPDATE QPTreasureDB..GameScoreInfo SET Score=Score-@dwMatchFee WHERE UserID=@dwUserID
			INSERT StreamMatchFeeInfo (UserID,ServerID,MatchID,MatchNo,Fee,CollectDate) 
								VALUES(@dwUserID,@wServerID,@dwMatchID,@dwMatchNO,@dwMatchFee,GETDATE())
			SET @ReturnValue=0
		END
	END	
	ELSE
		SET @ReturnValue=2

END

RETURN @ReturnValue
GO

----------------------------------------------------------------------------------------------------

-- �˻�����
CREATE PROC GSP_GR_UserMatchQuit
	@dwUserID INT,								-- �û� I D
	@dwMatchFee INT,							-- ��������
	@wKindID INT,								-- ��Ϸ I D
	@wServerID INT,								-- ���� I D
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@dwMatchID	INT,							-- ���� I D
	@dwMatchNO	INT,							-- ��������
	@strMachineID NVARCHAR(32)	,				-- ������ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
AS

-- ִ���߼�
BEGIN
		
	DECLARE @UserRight INT 
	DECLARE @ReturnValue INT
	SET @ReturnValue=0
	SELECT @UserRight=UserRight FROM QPAccountsDB..AccountsInfo WHERE UserID=@dwUserID 
	SELECT @UserRight=@UserRight|UserRight FROM GameScoreInfo WHERE UserID=@dwUserID

	IF  EXISTS(SELECT * FROM QPTreasureDB..GameScoreInfo WHERE UserID=@dwUserID)
	BEGIN
		DECLARE @Score BIGINT
		SELECT @Score=Score FROM QPTreasureDB..GameScoreInfo WHERE UserID=@dwUserID
		BEGIN
			IF EXISTS(SELECT * FROM StreamMatchFeeInfo 
					WHERE UserID=@dwUserID AND ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNO=@dwMatchNO)
			BEGIN
				UPDATE QPTreasureDB..GameScoreInfo SET Score=Score+@dwMatchFee WHERE UserID=@dwUserID
				DELETE StreamMatchFeeInfo WHERE UserID=@dwUserID AND ServerID=@wServerID AND MatchID=@dwMatchID
			END					
			SET @ReturnValue=0
		END
	END	
	ELSE
		SET @ReturnValue=2

END

RETURN @ReturnValue
GO