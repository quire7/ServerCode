USE QPTreasureDB
GO

-- ��Ա����
TRUNCATE TABLE MemberType
GO

INSERT INTO [dbo].[MemberType] ([MemberOrder], [MemberName], [MemberPrice], [PresentScore], [UserRight]) VALUES (1, N'�����Ա', 10.00, 100, 512)
INSERT INTO [dbo].[MemberType] ([MemberOrder], [MemberName], [MemberPrice], [PresentScore], [UserRight]) VALUES (2, N'�����Ա', 30.00, 300000, 512)
INSERT INTO [dbo].[MemberType] ([MemberOrder], [MemberName], [MemberPrice], [PresentScore], [UserRight]) VALUES (3, N'�����Ա', 90.00, 900000, 512)
INSERT INTO [dbo].[MemberType] ([MemberOrder], [MemberName], [MemberPrice], [PresentScore], [UserRight]) VALUES (4, N'�����Ա', 180.00, 1800000, 512)
INSERT INTO [dbo].[MemberType] ([MemberOrder], [MemberName], [MemberPrice], [PresentScore], [UserRight]) VALUES (5, N'VIP��Ա', 360.00, 3600000, 512)

GO