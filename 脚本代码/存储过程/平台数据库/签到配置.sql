
----------------------------------------------------------------------------------------------------

USE QPPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadCheckInReward]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadCheckInReward]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- º”‘ÿΩ±¿¯
CREATE PROC GSP_GP_LoadCheckInReward
AS

--  Ù–‘…Ë÷√
SET NOCOUNT ON

-- ÷¥––¬ﬂº≠
BEGIN

	-- ≤È—ØΩ±¿¯
	SELECT * FROM SigninConfig	

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
