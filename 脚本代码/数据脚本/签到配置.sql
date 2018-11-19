USE QPPlatformDB
GO

-- «©µΩ≈‰÷√
TRUNCATE TABLE SigninConfig
GO

INSERT INTO [dbo].[SigninConfig] ([DayID], [RewardGold]) VALUES (1, 1000)
INSERT INTO [dbo].[SigninConfig] ([DayID], [RewardGold]) VALUES (2, 2000)
INSERT INTO [dbo].[SigninConfig] ([DayID], [RewardGold]) VALUES (3, 4000)
INSERT INTO [dbo].[SigninConfig] ([DayID], [RewardGold]) VALUES (4, 8000)
INSERT INTO [dbo].[SigninConfig] ([DayID], [RewardGold]) VALUES (5, 16000)
INSERT INTO [dbo].[SigninConfig] ([DayID], [RewardGold]) VALUES (6, 32000)
INSERT INTO [dbo].[SigninConfig] ([DayID], [RewardGold]) VALUES (7, 64000)

GO