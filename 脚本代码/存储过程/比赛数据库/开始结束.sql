
----------------------------------------------------------------------------------------------------

USE QPGameMatchDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_MatchStart]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_MatchStart]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_MatchEliminate]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_MatchEliminate]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_MatchOver]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_MatchOver]
GO



SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------
-- ������ʼ
CREATE PROC GSP_GR_MatchStart
	@wServerID		INT,		-- �����ʶ		
	@dwMatchID		INT,		-- ������ʶ
	@dwMatchNo		INT,		-- ��������
	@cbMatchType	TINYINT		-- ��������
AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN 
	RETURN 0
END

RETURN 0
	
GO

----------------------------------------------------------------------------------------------------

-- ������̭
CREATE PROC GSP_GR_MatchEliminate
	@dwUserID		INT,		-- �û���ʶ
	@wServerID		INT,		-- �����ʶ	
	@dwMatchID		INT,		-- ������ʶ
	@dwMatchNo		INT,		-- ��������
	@cbMatchType	TINYINT		-- ��������			
AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ɾ����¼
	DELETE StreamMatchFeeInfo 
	WHERE UserID=@dwUserID AND ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@dwMatchNo AND Effective=0	

	-- ���·���
	UPDATE MatchScoreInfo SET Score=0,WinCount=0,LostCount=0,DrawCount=0,FleeCount=0
	WHERE UserID=@dwUserID AND ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@dwMatchNo	
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------
-- ��������
CREATE PROC GSP_GR_MatchOver
	@wServerID		INT,		-- �����ʶ	
	@dwMatchID		INT,		-- ������ʶ
	@dwMatchNo		INT,		-- ��������
	@cbMatchType	TINYINT,	-- ��������
	@MatchStartTime DATETIME,	-- ����ʱ��
	@MatchEndTime	DATETIME	-- ����ʱ��		
AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ʱ����
	IF @cbMatchType = 0
	BEGIN
		-- ��������			
		SELECT 	MatchRank,RewardGold,RewardMedal,RewardExperience INTO #TempMatchReward FROM MatchReward(NOLOCK) 
		WHERE MatchID=@dwMatchID AND MatchNo=@dwMatchNo

		DECLARE @RankCount SMALLINT
		SET @RankCount=@@Rowcount

		-- ��������
		IF @RankCount > 0
		BEGIN
			-- ���پ���
			DECLARE @MinPlayCount INT
			SELECT @MinPlayCount=MinPlayCount FROM MatchLockTime WHERE MatchID=@dwMatchID AND MatchNo=@dwMatchNo
			
			-- ��������
			IF @MinPlayCount IS NULL SET @MinPlayCount=1
				
			-- ��ѯ�������	
			SELECT MatchRank=ROW_NUMBER() OVER (ORDER BY Score DESC,WinCount,SignupTime	),* INTO #RankUserList FROM MatchScoreInfo(NOLOCK) 	
			WHERE ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@dwMatchNo AND (WinCount+LostCount+FleeCount+DrawCount)>=@MinPlayCount
			ORDER BY Score DESC,WinCount,SignupTime	

			-- �ϲ���¼
			SELECT a.*,ISNULL(b.RewardGold,0) AS RewardGold,ISNULL(b.RewardMedal,0) AS RewardMedal,ISNULL(b.RewardExperience,0) AS RewardExperience INTO #RankMatchReward
			FROM #RankUserList a LEFT JOIN #TempMatchReward b ON a.MatchRank=b.MatchRank 

			-- �����¼
			INSERT INTO StreamMatchHistory(UserID,MatchID,MatchNo,MatchType,ServerID,RankID,MatchScore,UserRight,RewardGold,RewardMedal,
			RewardExperience,WinCount,LostCount,DrawCount,FleeCount,MatchStartTime,MatchEndTime,PlayTimeCount,OnlineTime,Machine,ClientIP)
			SELECT a.UserID,@dwMatchID,@dwMatchNo,0,@wServerID,a.MatchRank,a.Score,b.UserRight,a.RewardGold,a.RewardMedal,a.RewardExperience,
			a.WinCount,a.LostCount,a.DrawCount,a.FleeCount,@MatchStartTime,@MatchEndTime,a.PlayTimeCount,a.OnlineTime,
			b.LastLogonMachine,	b.LastLogonIP FROM #RankMatchReward a,GameScoreInfo b 
			WHERE a.UserID=b.UserID

			-- �����߱��������ʸ�
			IF Exists(SELECT * FROM MatchLockTime WHERE MatchID=@dwMatchID AND MatchNo=@dwMatchNo+1)
			BEGIN
				SELECT RecordID INTO #MatchFeeRecord FROM StreamMatchFeeInfo(NOLOCK)
				WHERE ServerID=@wServerID AND MatchID=@dwMatchID AND Effective=0 AND UserID NOT IN (SELECT UserID FROM #RankMatchReward) 
			
				-- �޸�״̬
				UPDATE StreamMatchFeeInfo SET Effective=1 FROM StreamMatchFeeInfo a,#MatchFeeRecord b
				WHERE a.RecordID=b.RecordID

				-- ��ѯ��ʼ������
				DECLARE @InitScore BIGINT
				SELECT @InitScore=InitScore FROM MatchLockTime WHERE MatchID=@dwMatchID AND MatchNo=@dwMatchNo+1

				-- ���±�����
				UPDATE MatchScoreInfo SET MatchNo=@dwMatchNo+1,Score=@InitScore,WinCount=0,LostCount=0,DrawCount=0,FleeCount=0,PlayTimeCount=0,OnlineTime=0
				WHERE ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@dwMatchNo AND 
				UserID IN (SELECT UserID FROM #RankMatchReward WHERE MatchRank<=@RankCount) 		
			END 
			ELSE BEGIN
				-- �޸�״̬
				UPDATE StreamMatchFeeInfo SET Effective=1
				WHERE ServerID=@wServerID AND MatchID=@dwMatchID AND Effective=0
			END		

			-- ɾ��������
			DELETE MatchScoreInfo WHERE ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@dwMatchNo

			-- �׳�������
			SELECT MatchRank AS RankID,UserID,Score,RewardGold,RewardMedal,RewardExperience FROM #RankMatchReward
			WHERE MatchRank<=@RankCount																																			
			
			-- ������ʱ��
			IF OBJECT_ID('tempdb..#RankUserList') IS NOT NULL DROP TABLE #RankUserList
			IF OBJECT_ID('tempdb..#MatchFeeRecord') IS NOT NULL DROP TABLE #MatchFeeRecord			
			IF OBJECT_ID('tempdb..#RankMatchReward') IS NOT NULL DROP TABLE #RankMatchReward
			IF OBJECT_ID('tempdb..#TempMatchReward') IS NOT NULL DROP TABLE #TempMatchReward

		END ELSE
		BEGIN
			-- ɾ��������
			DELETE MatchScoreInfo WHERE ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@dwMatchNo

			-- �޸�״̬
			UPDATE StreamMatchFeeInfo SET Effective=1
			WHERE ServerID=@wServerID AND MatchID=@dwMatchID AND Effective=0			
		END																																																																																																			
	END

	-- ��ʱ����
	IF @cbMatchType = 1
	BEGIN
		-- ��������			
		SELECT 	MatchRank,RewardGold,RewardMedal,RewardExperience INTO #TempMatchReward1 FROM MatchReward(NOLOCK) 
		WHERE MatchID=@dwMatchID AND MatchNo=1

		-- ��ѯ�����	
		SELECT MatchRank=ROW_NUMBER() OVER (ORDER BY Score DESC,WinCount,SignupTime),* INTO #RankUserList1 FROM MatchScoreInfo(NOLOCK) 	
		WHERE ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@dwMatchNo
		ORDER BY Score DESC,WinCount,SignupTime

		-- ɾ��������
		DELETE MatchScoreInfo WHERE ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@dwMatchNo

		-- �ϲ���¼
		SELECT a.*,ISNULL(b.RewardGold,0) AS RewardGold,ISNULL(b.RewardMedal,0) AS RewardMedal,ISNULL(b.RewardExperience,0) AS RewardExperience INTO #RankMatchReward1
		FROM #RankUserList1 a LEFT JOIN #TempMatchReward1 b ON a.MatchRank=b.MatchRank	

		-- �޸�״̬
		UPDATE StreamMatchFeeInfo SET Effective=1
		WHERE ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@dwMatchNo AND Effective=0

		-- �����¼
		INSERT INTO StreamMatchHistory(UserID,MatchID,MatchNo,MatchType,ServerID,RankID,MatchScore,UserRight,RewardGold,RewardMedal,
		RewardExperience,WinCount,LostCount,DrawCount,FleeCount,MatchStartTime,MatchEndTime,PlayTimeCount,OnlineTime,Machine,ClientIP)
		SELECT a.UserID,@dwMatchID,@dwMatchNo,1,@wServerID,a.MatchRank,a.Score,b.UserRight,a.RewardGold,a.RewardMedal,a.RewardExperience,
		a.WinCount,a.LostCount,a.DrawCount,a.FleeCount,@MatchStartTime,@MatchEndTime,a.PlayTimeCount,a.OnlineTime,
		b.LastLogonMachine,	b.LastLogonIP FROM #RankMatchReward1 a,GameScoreInfo b 
		WHERE a.UserID=b.UserID
	
		-- �׳�������
		SELECT MatchRank AS RankID,UserID,Score,RewardGold,RewardMedal,RewardExperience FROM #RankMatchReward1		

		-- ������ʱ��
		IF OBJECT_ID('tempdb..#RankUserList1') IS NOT NULL DROP TABLE #RankUserList1 
		IF OBJECT_ID('tempdb..#TempMatchReward1') IS NOT NULL DROP TABLE #TempMatchReward1 
		IF OBJECT_ID('tempdb..#RankMatchReward1') IS NOT NULL DROP TABLE #RankMatchReward1 		
	END
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------