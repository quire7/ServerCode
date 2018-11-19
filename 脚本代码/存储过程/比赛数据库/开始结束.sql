
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
-- 比赛开始
CREATE PROC GSP_GR_MatchStart
	@wServerID		INT,		-- 房间标识		
	@dwMatchID		INT,		-- 比赛标识
	@dwMatchNo		INT,		-- 比赛场次
	@cbMatchType	TINYINT		-- 比赛类型
AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN 
	RETURN 0
END

RETURN 0
	
GO

----------------------------------------------------------------------------------------------------

-- 比赛淘汰
CREATE PROC GSP_GR_MatchEliminate
	@dwUserID		INT,		-- 用户标识
	@wServerID		INT,		-- 房间标识	
	@dwMatchID		INT,		-- 比赛标识
	@dwMatchNo		INT,		-- 比赛场次
	@cbMatchType	TINYINT		-- 比赛类型			
AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 删除记录
	DELETE StreamMatchFeeInfo 
	WHERE UserID=@dwUserID AND ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@dwMatchNo AND Effective=0	

	-- 更新分数
	UPDATE MatchScoreInfo SET Score=0,WinCount=0,LostCount=0,DrawCount=0,FleeCount=0
	WHERE UserID=@dwUserID AND ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@dwMatchNo	
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------
-- 比赛结束
CREATE PROC GSP_GR_MatchOver
	@wServerID		INT,		-- 房间标识	
	@dwMatchID		INT,		-- 比赛标识
	@dwMatchNo		INT,		-- 比赛场次
	@cbMatchType	TINYINT,	-- 比赛类型
	@MatchStartTime DATETIME,	-- 开赛时间
	@MatchEndTime	DATETIME	-- 结束时间		
AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 定时赛制
	IF @cbMatchType = 0
	BEGIN
		-- 奖励名次			
		SELECT 	MatchRank,RewardGold,RewardMedal,RewardExperience INTO #TempMatchReward FROM MatchReward(NOLOCK) 
		WHERE MatchID=@dwMatchID AND MatchNo=@dwMatchNo

		DECLARE @RankCount SMALLINT
		SET @RankCount=@@Rowcount

		-- 奖励处理
		IF @RankCount > 0
		BEGIN
			-- 最少局数
			DECLARE @MinPlayCount INT
			SELECT @MinPlayCount=MinPlayCount FROM MatchLockTime WHERE MatchID=@dwMatchID AND MatchNo=@dwMatchNo
			
			-- 调整局数
			IF @MinPlayCount IS NULL SET @MinPlayCount=1
				
			-- 查询晋级玩家	
			SELECT MatchRank=ROW_NUMBER() OVER (ORDER BY Score DESC,WinCount,SignupTime	),* INTO #RankUserList FROM MatchScoreInfo(NOLOCK) 	
			WHERE ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@dwMatchNo AND (WinCount+LostCount+FleeCount+DrawCount)>=@MinPlayCount
			ORDER BY Score DESC,WinCount,SignupTime	

			-- 合并记录
			SELECT a.*,ISNULL(b.RewardGold,0) AS RewardGold,ISNULL(b.RewardMedal,0) AS RewardMedal,ISNULL(b.RewardExperience,0) AS RewardExperience INTO #RankMatchReward
			FROM #RankUserList a LEFT JOIN #TempMatchReward b ON a.MatchRank=b.MatchRank 

			-- 插入记录
			INSERT INTO StreamMatchHistory(UserID,MatchID,MatchNo,MatchType,ServerID,RankID,MatchScore,UserRight,RewardGold,RewardMedal,
			RewardExperience,WinCount,LostCount,DrawCount,FleeCount,MatchStartTime,MatchEndTime,PlayTimeCount,OnlineTime,Machine,ClientIP)
			SELECT a.UserID,@dwMatchID,@dwMatchNo,0,@wServerID,a.MatchRank,a.Score,b.UserRight,a.RewardGold,a.RewardMedal,a.RewardExperience,
			a.WinCount,a.LostCount,a.DrawCount,a.FleeCount,@MatchStartTime,@MatchEndTime,a.PlayTimeCount,a.OnlineTime,
			b.LastLogonMachine,	b.LastLogonIP FROM #RankMatchReward a,GameScoreInfo b 
			WHERE a.UserID=b.UserID

			-- 晋级者保留比赛资格
			IF Exists(SELECT * FROM MatchLockTime WHERE MatchID=@dwMatchID AND MatchNo=@dwMatchNo+1)
			BEGIN
				SELECT RecordID INTO #MatchFeeRecord FROM StreamMatchFeeInfo(NOLOCK)
				WHERE ServerID=@wServerID AND MatchID=@dwMatchID AND Effective=0 AND UserID NOT IN (SELECT UserID FROM #RankMatchReward) 
			
				-- 修改状态
				UPDATE StreamMatchFeeInfo SET Effective=1 FROM StreamMatchFeeInfo a,#MatchFeeRecord b
				WHERE a.RecordID=b.RecordID

				-- 查询初始比赛分
				DECLARE @InitScore BIGINT
				SELECT @InitScore=InitScore FROM MatchLockTime WHERE MatchID=@dwMatchID AND MatchNo=@dwMatchNo+1

				-- 更新比赛分
				UPDATE MatchScoreInfo SET MatchNo=@dwMatchNo+1,Score=@InitScore,WinCount=0,LostCount=0,DrawCount=0,FleeCount=0,PlayTimeCount=0,OnlineTime=0
				WHERE ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@dwMatchNo AND 
				UserID IN (SELECT UserID FROM #RankMatchReward WHERE MatchRank<=@RankCount) 		
			END 
			ELSE BEGIN
				-- 修改状态
				UPDATE StreamMatchFeeInfo SET Effective=1
				WHERE ServerID=@wServerID AND MatchID=@dwMatchID AND Effective=0
			END		

			-- 删除比赛分
			DELETE MatchScoreInfo WHERE ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@dwMatchNo

			-- 抛出获奖名单
			SELECT MatchRank AS RankID,UserID,Score,RewardGold,RewardMedal,RewardExperience FROM #RankMatchReward
			WHERE MatchRank<=@RankCount																																			
			
			-- 销毁临时表
			IF OBJECT_ID('tempdb..#RankUserList') IS NOT NULL DROP TABLE #RankUserList
			IF OBJECT_ID('tempdb..#MatchFeeRecord') IS NOT NULL DROP TABLE #MatchFeeRecord			
			IF OBJECT_ID('tempdb..#RankMatchReward') IS NOT NULL DROP TABLE #RankMatchReward
			IF OBJECT_ID('tempdb..#TempMatchReward') IS NOT NULL DROP TABLE #TempMatchReward

		END ELSE
		BEGIN
			-- 删除比赛分
			DELETE MatchScoreInfo WHERE ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@dwMatchNo

			-- 修改状态
			UPDATE StreamMatchFeeInfo SET Effective=1
			WHERE ServerID=@wServerID AND MatchID=@dwMatchID AND Effective=0			
		END																																																																																																			
	END

	-- 即时赛制
	IF @cbMatchType = 1
	BEGIN
		-- 奖励名次			
		SELECT 	MatchRank,RewardGold,RewardMedal,RewardExperience INTO #TempMatchReward1 FROM MatchReward(NOLOCK) 
		WHERE MatchID=@dwMatchID AND MatchNo=1

		-- 查询获奖玩家	
		SELECT MatchRank=ROW_NUMBER() OVER (ORDER BY Score DESC,WinCount,SignupTime),* INTO #RankUserList1 FROM MatchScoreInfo(NOLOCK) 	
		WHERE ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@dwMatchNo
		ORDER BY Score DESC,WinCount,SignupTime

		-- 删除比赛分
		DELETE MatchScoreInfo WHERE ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@dwMatchNo

		-- 合并记录
		SELECT a.*,ISNULL(b.RewardGold,0) AS RewardGold,ISNULL(b.RewardMedal,0) AS RewardMedal,ISNULL(b.RewardExperience,0) AS RewardExperience INTO #RankMatchReward1
		FROM #RankUserList1 a LEFT JOIN #TempMatchReward1 b ON a.MatchRank=b.MatchRank	

		-- 修改状态
		UPDATE StreamMatchFeeInfo SET Effective=1
		WHERE ServerID=@wServerID AND MatchID=@dwMatchID AND MatchNo=@dwMatchNo AND Effective=0

		-- 插入记录
		INSERT INTO StreamMatchHistory(UserID,MatchID,MatchNo,MatchType,ServerID,RankID,MatchScore,UserRight,RewardGold,RewardMedal,
		RewardExperience,WinCount,LostCount,DrawCount,FleeCount,MatchStartTime,MatchEndTime,PlayTimeCount,OnlineTime,Machine,ClientIP)
		SELECT a.UserID,@dwMatchID,@dwMatchNo,1,@wServerID,a.MatchRank,a.Score,b.UserRight,a.RewardGold,a.RewardMedal,a.RewardExperience,
		a.WinCount,a.LostCount,a.DrawCount,a.FleeCount,@MatchStartTime,@MatchEndTime,a.PlayTimeCount,a.OnlineTime,
		b.LastLogonMachine,	b.LastLogonIP FROM #RankMatchReward1 a,GameScoreInfo b 
		WHERE a.UserID=b.UserID
	
		-- 抛出获奖名单
		SELECT MatchRank AS RankID,UserID,Score,RewardGold,RewardMedal,RewardExperience FROM #RankMatchReward1		

		-- 销毁临时表
		IF OBJECT_ID('tempdb..#RankUserList1') IS NOT NULL DROP TABLE #RankUserList1 
		IF OBJECT_ID('tempdb..#TempMatchReward1') IS NOT NULL DROP TABLE #TempMatchReward1 
		IF OBJECT_ID('tempdb..#RankMatchReward1') IS NOT NULL DROP TABLE #RankMatchReward1 		
	END
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------