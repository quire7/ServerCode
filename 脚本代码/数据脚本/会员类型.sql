USE QPTreasureDB
GO

-- 会员类型
TRUNCATE TABLE MemberType
GO

INSERT INTO [dbo].[MemberType] ([MemberOrder], [MemberName], [MemberPrice], [PresentScore], [UserRight]) VALUES (1, N'蓝钻会员', 10.00, 100, 512)
INSERT INTO [dbo].[MemberType] ([MemberOrder], [MemberName], [MemberPrice], [PresentScore], [UserRight]) VALUES (2, N'黄钻会员', 30.00, 300000, 512)
INSERT INTO [dbo].[MemberType] ([MemberOrder], [MemberName], [MemberPrice], [PresentScore], [UserRight]) VALUES (3, N'白钻会员', 90.00, 900000, 512)
INSERT INTO [dbo].[MemberType] ([MemberOrder], [MemberName], [MemberPrice], [PresentScore], [UserRight]) VALUES (4, N'红钻会员', 180.00, 1800000, 512)
INSERT INTO [dbo].[MemberType] ([MemberOrder], [MemberName], [MemberPrice], [PresentScore], [UserRight]) VALUES (5, N'VIP会员', 360.00, 3600000, 512)

GO