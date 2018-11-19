
----------------------------------------------------------------------------------------------------

USE QPPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_LoadGameMatchItem]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_LoadGameMatchItem]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_LoadMatchSignupList]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_LoadMatchSignupList]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_LoadMatchReward]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_LoadMatchReward]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 加载比赛
CREATE  PROCEDURE dbo.GSP_GS_LoadGameMatchItem
	@wKindID   INT,								-- 类型标识
	@dwMatchID INT,								-- 比赛标识	
	@dwMatchNo INT,								-- 比赛场次	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	-- 加载比赛
	SELECT a.MatchName,a.MatchType,a.MatchFeetype,a.MatchFee,a.MemberOrder,d.* FROM QPGameMatchDBLink.QPGameMatchDB.dbo.MatchPublic AS a,
	((SELECT b.MatchID,b.MatchNo,b.StartTime,b.EndTime,b.InitScore,b.CullScore,b.MinPlayCount,c.StartUserCount,c.AndroidUserCount,
	c.InitialBase,c.InitialScore,c.MinEnterGold,c.PlayCount,c.SwitchTableCount,c.PrecedeTimer FROM QPGameMatchDBLink.QPGameMatchDB.dbo.MatchLockTime b
	LEFT OUTER JOIN QPGameMatchDBLink.QPGameMatchDB.dbo.MatchImmediate c  ON  b.MatchID=c.MatchID ) 
	UNION ALL 	
	(SELECT c.MatchID,c.MatchNo,b.StartTime,b.EndTime,b.InitScore,b.CullScore,b.MinPlayCount,c.StartUserCount,c.AndroidUserCount,
	c.InitialBase,c.InitialScore,c.MinEnterGold,c.PlayCount,c.SwitchTableCount,c.PrecedeTimer FROM QPGameMatchDBLink.QPGameMatchDB.dbo.MatchLockTime b
	RIGHT OUTER JOIN QPGameMatchDBLink.QPGameMatchDB.dbo.MatchImmediate c  ON  b.MatchID=c.MatchID )) AS d 
	WHERE (((a.KindID=@wKindID AND a.MatchID=@dwMatchID AND a.MatchNo=@dwMatchNo) OR (@dwMatchID=0 AND a.KindID=@wKindID)) AND (a.MatchID=d.MatchID AND a.MatchNo=d.MatchNo))

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 加载报名列表
CREATE  PROCEDURE dbo.GSP_GS_LoadMatchSignupList
	@dwUserID INT,								-- 用户标识	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 加载报名
	SELECT ServerID,MatchID,MatchNo FROM QPGameMatchDBLink.QPGameMatchDB.dbo.StreamMatchFeeInfo 
	WHERE UserID=@dwUserID AND MatchType=0 AND Effective=0

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 加载奖励
CREATE  PROCEDURE dbo.GSP_GS_LoadMatchReward
	@dwMatchID INT,								-- 比赛标识	
	@dwMatchNo INT								-- 比赛场次
AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 加载奖励
	SELECT * FROM QPGameMatchDBLink.QPGameMatchDB.dbo.MatchReward 
	WHERE MatchID=@dwMatchID AND MatchNo=@dwMatchNo

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
