
----------------------------------------------------------------------------------------------------

USE QPPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadGrowLevelConfig]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadGrowLevelConfig]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------
-- µ»º∂≈‰÷√
CREATE PROC GSP_GR_LoadGrowLevelConfig
AS

--  Ù–‘…Ë÷√
SET NOCOUNT ON

-- ÷¥––¬ﬂº≠
BEGIN
	-- ≤È—Ø≈‰÷√
	SELECT LevelID,Experience FROM GrowLevelConfig ORDER BY LevelID
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------