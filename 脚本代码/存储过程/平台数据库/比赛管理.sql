
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

-- ���ر���
CREATE  PROCEDURE dbo.GSP_GS_LoadGameMatchItem
	@wKindID   INT,								-- ���ͱ�ʶ
	@dwMatchID INT,								-- ������ʶ	
	@dwMatchNo INT,								-- ��������	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- ���ر���
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

-- ���ر����б�
CREATE  PROCEDURE dbo.GSP_GS_LoadMatchSignupList
	@dwUserID INT,								-- �û���ʶ	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ���ر���
	SELECT ServerID,MatchID,MatchNo FROM QPGameMatchDBLink.QPGameMatchDB.dbo.StreamMatchFeeInfo 
	WHERE UserID=@dwUserID AND MatchType=0 AND Effective=0

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ���ؽ���
CREATE  PROCEDURE dbo.GSP_GS_LoadMatchReward
	@dwMatchID INT,								-- ������ʶ	
	@dwMatchNo INT								-- ��������
AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ���ؽ���
	SELECT * FROM QPGameMatchDBLink.QPGameMatchDB.dbo.MatchReward 
	WHERE MatchID=@dwMatchID AND MatchNo=@dwMatchNo

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
