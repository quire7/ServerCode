
----------------------------------------------------------------------------------------------------

USE QPTreasureDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadSpreadInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadSpreadInfo]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- �ƹ㽱��
CREATE PROC GSP_GR_LoadSpreadInfo
	@dwUserID		INT				--�û���ʶ
AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- ������Ϣ
	DECLARE @SpreadCount INT
	DECLARE	@SpreadReward BIGINT
	
	-- ͳ������
	SELECT @SpreadCount=Count(*) FROM QPAccountsDBLink.QPAccountsDB.dbo.AccountsInfo	
	WHERE SpreaderID=@dwUserID

	-- ͳ�ƽ���
	SELECT @SpreadReward=SUM(Score)	FROM RecordSpreadInfo WHERE UserID=@dwUserID AND Score>0

	-- ��������
	IF @SpreadCount IS NULL SET @SpreadCount=0
	IF @SpreadReward IS NULL SET @SpreadReward=0

	--�׳�����
	SELECT @SpreadCount AS SpreadCount,@SpreadReward AS SpreadReward
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
