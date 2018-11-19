use QPAccountsDB

-- ϵͳ����
TRUNCATE TABLE [dbo].[SystemStatusInfo]

INSERT INTO [dbo].[SystemStatusInfo] ([StatusName], [StatusValue], [StatusString], [StatusTip], [StatusDescription], [SortID]) VALUES (N'BankPrerequisite', 20, N'��ȡ��������ȡ��Ϸ�ұ�����ڴ����ſɲ�����', N'��ȡ����', N'��ֵ����ʾ��ȡ�����������ڴ����ſɴ�ȡ', 35)
INSERT INTO [dbo].[SystemStatusInfo] ([StatusName], [StatusValue], [StatusString], [StatusTip], [StatusDescription], [SortID]) VALUES (N'EnjoinInsure', 0, N'����ϵͳά������ʱֹͣ��Ϸϵͳ�ı��չ������������վ������Ϣ��', N'���з���', N'��ֵ��0-������1-�ر�', 30)
INSERT INTO [dbo].[SystemStatusInfo] ([StatusName], [StatusValue], [StatusString], [StatusTip], [StatusDescription], [SortID]) VALUES (N'EnjoinLogon', 0, N'����ϵͳά������ʱֹͣ��Ϸϵͳ�ĵ�¼������������վ������Ϣ��', N'��¼����', N'��ֵ��0-������1-�ر�', 25)
INSERT INTO [dbo].[SystemStatusInfo] ([StatusName], [StatusValue], [StatusString], [StatusTip], [StatusDescription], [SortID]) VALUES (N'EnjoinRegister', 0, N'����ϵͳά������ʱֹͣ��Ϸϵͳ��ע�������������վ������Ϣ��', N'ע�����', N'��ֵ��0-������1-�ر�', 10)
INSERT INTO [dbo].[SystemStatusInfo] ([StatusName], [StatusValue], [StatusString], [StatusTip], [StatusDescription], [SortID]) VALUES (N'GrantIPCount', 5, N'���û�ע��ÿ���������ƣ�', N'ע����������', N'��ֵ����ʾͬһ��IP������͵Ĵ��������������������ͽ��', 20)
INSERT INTO [dbo].[SystemStatusInfo] ([StatusName], [StatusValue], [StatusString], [StatusTip], [StatusDescription], [SortID]) VALUES (N'GrantScoreCount', 1000000, N'���û�ע��ϵͳ����Ϸ�ҵ���Ŀ��', N'ע������', N'��ֵ����ʾ���͵���Ϸ������', 15)
INSERT INTO [dbo].[SystemStatusInfo] ([StatusName], [StatusValue], [StatusString], [StatusTip], [StatusDescription], [SortID]) VALUES (N'MedalExchangeRate', 1000, N'Ԫ������Ϸ�Ҷһ���', N'Ԫ���һ���', N'��ֵ��1��Ԫ���һ�������Ϸ��', 90)
INSERT INTO [dbo].[SystemStatusInfo] ([StatusName], [StatusValue], [StatusString], [StatusTip], [StatusDescription], [SortID]) VALUES (N'PresentExchangeRate', 1500, N'��������Ϸ�Ҷһ���', N'�����һ���', N'��ֵ: 1�������һ�������Ϸ��', 95)
INSERT INTO [dbo].[SystemStatusInfo] ([StatusName], [StatusValue], [StatusString], [StatusTip], [StatusDescription], [SortID]) VALUES (N'RateCurrency', 1, N'���������Ϸ���Ļ��� �����:��Ϸ��', N'��Ϸ������', N'��ֵ�����������Ϸ���Ļ���', 1)
INSERT INTO [dbo].[SystemStatusInfo] ([StatusName], [StatusValue], [StatusString], [StatusTip], [StatusDescription], [SortID]) VALUES (N'RateGold', 10, N'��Ϸ������Ϸ�ҵĻ��� ��Ϸ��:��Ϸ��', N'��Ϸ�һ���', N'��ֵ����Ϸ������Ϸ�ҵĻ���', 5)
INSERT INTO [dbo].[SystemStatusInfo] ([StatusName], [StatusValue], [StatusString], [StatusTip], [StatusDescription], [SortID]) VALUES (N'RevenueRateTake', 10, N'ȡ�����˰�ձ��ʣ�ǧ�ֱȣ���', N'ȡ��˰��', N'��ֵ����ʾ����ȡ�����˰�ձ���ֵ��ǧ�ֱȣ���', 40)
INSERT INTO [dbo].[SystemStatusInfo] ([StatusName], [StatusValue], [StatusString], [StatusTip], [StatusDescription], [SortID]) VALUES (N'RevenueRateTransfer', 10, N'ת�˲���˰�ձ��ʣ�ǧ�ֱȣ���', N'��ͨת��˰��', N'��ֵ����ʾ��ͨ�������ת�˲���˰�ձ���ֵ��ǧ�ֱȣ���', 55)
INSERT INTO [dbo].[SystemStatusInfo] ([StatusName], [StatusValue], [StatusString], [StatusTip], [StatusDescription], [SortID]) VALUES (N'RevenueRateTransferMember', 0, N'ת�˲�����Ա˰�ձ��ʣ�ǧ�ֱȣ���', N'��Աת��˰��', N'��ֵ����ʾ��Ա�������ת�˲���˰�ձ���ֵ��ǧ�ֱȣ���', 60)
INSERT INTO [dbo].[SystemStatusInfo] ([StatusName], [StatusValue], [StatusString], [StatusTip], [StatusDescription], [SortID]) VALUES (N'SubsistenceAllowancesCondition', 1000, N'��ȡ�ͱ������Ϸ�Ҳ��ܵ���', N'�ͱ���ȡ����', N'��ֵ����ȡ�ͱ���ҽ�Ҳ��ܵ��ڸý����', 80)
INSERT INTO [dbo].[SystemStatusInfo] ([StatusName], [StatusValue], [StatusString], [StatusTip], [StatusDescription], [SortID]) VALUES (N'SubsistenceAllowancesGold', 1000, N'ÿ�εͱ�����Ϸ����', N'�ͱ�ÿ�ν��', N'��ֵ��ÿ�εͱ��Ľ����', 70)
INSERT INTO [dbo].[SystemStatusInfo] ([StatusName], [StatusValue], [StatusString], [StatusTip], [StatusDescription], [SortID]) VALUES (N'SubsistenceAllowancesNumber', 1000, N'ÿ����ȡ�ͱ���������', N'�ͱ�ÿ�մ���', N'��ֵ��ÿ����ȡ�ͱ���������', 75)
INSERT INTO [dbo].[SystemStatusInfo] ([StatusName], [StatusValue], [StatusString], [StatusTip], [StatusDescription], [SortID]) VALUES (N'TaskTakeCount', 5, N'ÿ�����ȡ����������Ŀ', N'��ȡ������Ŀ', N'��ֵ��ÿ�����ȡ����������Ŀ', 101)
INSERT INTO [dbo].[SystemStatusInfo] ([StatusName], [StatusValue], [StatusString], [StatusTip], [StatusDescription], [SortID]) VALUES (N'TransferMaxTax', 1000, N'ת�˷ⶥ˰�գ�', N'ת��˰�շⶥ', N'��ֵ������ת�˷ⶥ˰�գ�0-���ⶥ', 65)
INSERT INTO [dbo].[SystemStatusInfo] ([StatusName], [StatusValue], [StatusString], [StatusTip], [StatusDescription], [SortID]) VALUES (N'TransferPrerequisite', 10000, N'ת��������ת����Ϸ����������ڴ����ſ�ת�ˣ�', N'ת������', N'��ֵ����ʾת�˽����������ڴ����ſ�ת��', 50)
INSERT INTO [dbo].[SystemStatusInfo] ([StatusName], [StatusValue], [StatusString], [StatusTip], [StatusDescription], [SortID]) VALUES (N'TransferStauts', 1, N'ת�˹��ܱ��رգ���������վ����', N'ת��״̬', N'��ֵ��ת���Ƿ�������ֵ��0-�ر�ת�ˣ�1-����ת��', 45)
INSERT INTO [dbo].[SystemStatusInfo] ([StatusName], [StatusValue], [StatusString], [StatusTip], [StatusDescription], [SortID]) VALUES (N'WinExperience', 10, N'Ӯ�ֽ����ľ���ֵ', N'Ӯ�־���', N'��ֵ��Ӯ�ֽ����ľ���ֵ', 100)
