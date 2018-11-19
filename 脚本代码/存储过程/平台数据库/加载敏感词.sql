
----------------------------------------------------------------------------------------------------

USE QPPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadSensitiveWords]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadSensitiveWords]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

--��ȡϵͳ��Ϣ
CREATE  PROCEDURE dbo.GSP_GR_LoadSensitiveWords 
AS

--��������
SET NOCOUNT ON

SELECT ForbidWords AS SensitiveWords FROM SensitiveWords

RETURN 0

GO

----------------------------------------------------------------------------------------------------
