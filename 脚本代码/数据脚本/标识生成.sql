
---ִ�б��ű�ǰ,���ȱ������ݿ�.�Ը����

USE [QPAccountsDB]
GO

-- �ر�����
SET NOCOUNT ON

--------------------------------------------------------------------------
-- ��ձ�ʶ 
-- ע�⣺������������ɵĻ�,��ȥ����������ǰ��"--",�����ID������,�ٴ����ɵĻ�.�벻Ҫȥ��

--Truncate table GameIdentifier
--Truncate table ReserveIdentifier 

--------------------------------------------------------------------------


-- ���Կ���
DECLARE @Debug BIT

-- ��������
DECLARE @BeginBase	INT
DECLARE @EndBase	INT
DECLARE @BaseString	VARCHAR(10)
DECLARE @ReserveEnd	INT

-- ��ʶ��ʼ�ͽ���
DECLARE @BeginGameID	INT
DECLARE @EndGameID		INT

-- ��ʶ����
DECLARE @MinIDLength		INT
DECLARE @MaxIDLength		INT

------------------------------------------------------------------------------
-- ������ʼ��
SET @Debug=1

SET @BeginBase	=0
SET @EndBase	=9
SET @BaseString	='0123456789'

SET @ReserveEnd=100000
SET @BeginGameID=1
SET @EndGameID	=999999

SET @MinIDLength=LEN(LTRIM(RTRIM(STR(@BeginGameID))))
SET @MaxIDLength=13

-- ������ʼ��
------------------------------------------------------------------------------

-------------------------------------------------------------------------
-- ɸѡ���� Begin
	DECLARE @tblFilterRules TABLE(Rules VARCHAR(64))
	DECLARE	@Tmp INT
	DECLARE @TmpRules VARCHAR(64)

	-------------------------------------------------------------------------
	-- AAAAA,BBBBB Begin
	SET @Tmp=0	
	SET @TmpRules=''
	WHILE (@BeginBase<@EndBase)
	BEGIN		
		SET @MinIDLength=LEN(LTRIM(RTRIM(STR(@BeginGameID))))

		WHILE (@MinIDLength<=@MaxIDLength)
		BEGIN
			SET @Tmp=0		
			WHILE (@Tmp<@MinIDLength)
			BEGIN
				SET @TmpRules=@TmpRules+'['+LTRIM(STR(@BeginBase+1))+']'
				
				SET @Tmp=@Tmp+1		
			END
			
			IF @TmpRules IS NOT NULL INSERT @tblFilterRules(Rules) VALUES(@TmpRules)			
			SET @MinIDLength=@MinIDLength+1
			SET @TmpRules=''
		END
		
		SET @BeginBase=@BeginBase+1			
	END

	-- ��ԭ����
	SET @Tmp=0	
	SET @TmpRules=''

	SET @BeginBase=0
	SET @MinIDLength=LEN(LTRIM(RTRIM(STR(@BeginGameID))))	 
	
	-- AAAAA,BBBBB END
	-------------------------------------------------------------------------
	INSERT @tblFilterRules (Rules)
			  SELECT '%000%'
	UNION ALL SELECT '%111%'
	UNION ALL SELECT '%222%'
	UNION ALL SELECT '%333%'
	UNION ALL SELECT '%444%'
	UNION ALL SELECT '%555%'
	UNION ALL SELECT '%666%'
	UNION ALL SELECT '%777%'
	UNION ALL SELECT '%888%'
	UNION ALL SELECT '%999%'
	-------------------------------------------------------------------------
	-- %AAA%,%BBB% Begin
	
	-- %AAA%,%BBB% End
	-------------------------------------------------------------------------

	-- ��ԭ����
	SET @Tmp=0	
	SET @TmpRules=''

	SET @BeginBase=0
	SET @MinIDLength=LEN(LTRIM(RTRIM(STR(@BeginGameID))))

	-------------------------------------------------------------------------
	-- %521%,%520% Begin
	INSERT @tblFilterRules (Rules)
	SELECT '%520%'
	UNION ALL SELECT '%521%'
	UNION ALL SELECT '%527%'

	-- %521%,%520% End
	-------------------------------------------------------------------------

	-------------------------------------------------------------------------
	-- %132%,%133% �绰���� Begin
	INSERT @tblFilterRules (Rules)
			  SELECT '130[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]'
	UNION ALL SELECT '131[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]'
	UNION ALL SELECT '132[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]'
	UNION ALL SELECT '133[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]'
	UNION ALL SELECT '134[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]'
	UNION ALL SELECT '135[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]'
	UNION ALL SELECT '136[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]'
	UNION ALL SELECT '137[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]'
	UNION ALL SELECT '138[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]'
	UNION ALL SELECT '139[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]'

	UNION ALL SELECT '150[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]'
	UNION ALL SELECT '151[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]'
	UNION ALL SELECT '152[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]'
	UNION ALL SELECT '153[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]'
	UNION ALL SELECT '154[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]'
	UNION ALL SELECT '155[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]'
	UNION ALL SELECT '156[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]'
	UNION ALL SELECT '157[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]'
	UNION ALL SELECT '158[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]'
	UNION ALL SELECT '159[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]'

	UNION ALL SELECT '180[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]'
	UNION ALL SELECT '181[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]'
	UNION ALL SELECT '182[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]'
	UNION ALL SELECT '183[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]'
	UNION ALL SELECT '184[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]'
	UNION ALL SELECT '185[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]'	
	UNION ALL SELECT '186[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]'
	UNION ALL SELECT '187[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]'
	UNION ALL SELECT '188[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]'
	UNION ALL SELECT '189[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]'

	-- %132%,%133% �绰���� End

	-- 195000 ���պ��� Begin
	SET @BeginBase	=1949
	SET @EndBase	=2050
	SET @TmpRules	=''
	WHILE (@BeginBase<@EndBase)
	BEGIN		
		SET @TmpRules=@TmpRules+''+LTRIM(STR(@BeginBase))+'[0-1][0-9]'		
		
		-- 194901
		SET @TmpRules=@TmpRules
		INSERT @tblFilterRules (Rules)	VALUES ('%'+@TmpRules+'%')

		-- 19490101
		SET @TmpRules='%'+@TmpRules+'[0-3][0-9]%'
		INSERT @tblFilterRules (Rules)	VALUES (@TmpRules)

		SET @BeginBase=@BeginBase+1			
		SET @TmpRules=''
	END
	-- 195000 ���պ��� End

	-- ������� Begin
	INSERT @tblFilterRules (Rules)
			  SELECT '168%'	
	UNION ALL SELECT '668%'
	UNION ALL SELECT '666%'
	UNION ALL SELECT '888%'
	UNION ALL SELECT '518%'
	UNION ALL SELECT '588%'
	UNION ALL SELECT '886%'
	UNION ALL SELECT '881%'
	UNION ALL SELECT '%1234%'
	UNION ALL SELECT '%4321%'
	UNION ALL SELECT '%110%'
	UNION ALL SELECT '%120%'
	UNION ALL SELECT '%119%'
	UNION ALL SELECT '800%'
	UNION ALL SELECT '400%'
	UNION ALL SELECT '600%'
	UNION ALL SELECT '%806%'
	UNION ALL SELECT '%668%'
	UNION ALL SELECT '%888%'
	UNION ALL SELECT '%986%'
	UNION ALL SELECT '%998%'

	UNION ALL SELECT '[1-9]00188'
	UNION ALL SELECT '[1-9]22188'
	UNION ALL SELECT '[1-9]33188'

	UNION ALL SELECT '[1-9]00168'
	UNION ALL SELECT '[1-9]22168'
	UNION ALL SELECT '[1-9]33168'

	UNION ALL SELECT '[1-9]00186'
	UNION ALL SELECT '[1-9]22186'
	UNION ALL SELECT '[1-9]33186'

	UNION ALL SELECT '[1-9]00[1-9]00'
	UNION ALL SELECT '[1-9]11[1-9]11'	
	UNION ALL SELECT '[1-9]22[1-9]22'	
	UNION ALL SELECT '[1-9]33[1-9]33'	
	UNION ALL SELECT '[1-9]44[1-9]44'	
	UNION ALL SELECT '[1-9]55[1-9]55'	
	UNION ALL SELECT '[1-9]66[1-9]66'	
	UNION ALL SELECT '[1-9]77[1-9]77'	
	UNION ALL SELECT '[1-9]88[1-9]88'
	UNION ALL SELECT '[1-9]99[1-9]99'

	UNION ALL SELECT '[1-9]88[1-9]88'
	UNION ALL SELECT '[1-9]888[1-9]888'
	UNION ALL SELECT '[1-9]8888[1-9]8888'
	UNION ALL SELECT '[1-9]88888[1-9]88888'
	
	UNION ALL SELECT '[1-9]00[1-9]00'
	UNION ALL SELECT '[1-9]000[1-9]000'
	UNION ALL SELECT '[1-9]0000[1-9]0000'
	UNION ALL SELECT '[1-9]00000[1-9]00000'

	UNION ALL SELECT '%1314%'		-- һ��һ��
	UNION ALL SELECT '%53770%'		-- ����������
	UNION ALL SELECT '%53719%'		-- ����������
	UNION ALL SELECT '%25184%'		-- ����һ����
	UNION ALL SELECT '%1392010%'	-- һ���Ͱ���һ��
	UNION ALL SELECT '%220250%'		-- �����㰮����
	UNION ALL SELECT '%584520%'		--  �ҷ����Ұ���
	UNION ALL SELECT '%246437%'		-- �����������
	UNION ALL SELECT '%1314925%'	-- һ��һ���Ͱ���
	UNION ALL SELECT '%594230%'		-- �Ҿ��ǰ�����
	UNION ALL SELECT '360%'			-- ������
	UNION ALL SELECT '%2010000%'	--  ����һ����
	UNION ALL SELECT '1372%'		-- һ����Ը
	UNION ALL SELECT '259695%'		-- ���Ҿ��˽���
	UNION ALL SELECT '74839%'		-- ��ʵ������
	UNION ALL SELECT '20999%'		-- ����þþ�
	UNION ALL SELECT '829475%'		-- ���������Ҹ�
	-- ������� End
	
	-------------------------------------------------------------------------	 

	-- ������Ϣ
	--IF @Debug=1 SELECT * FROM @tblFilterRules
	
-- ɸѡ���� End
-------------------------------------------------------------------------


-------------------------------------------------------------------------
-- ID���� Begin

	DECLARE @Reserve BIT
	SET @Reserve=0
	WHILE (@BeginGameID<=@EndGameID)
	BEGIN
		SET @Reserve=0

		-- ������ʶ
		IF @BeginGameID<=@ReserveEnd
		BEGIN
			SET @Reserve=1
			--print '������'+STR(@BeginGameID)+'  ' + @TTRules
			INSERT ReserveIdentifier(GameID) VALUES (@BeginGameID)
		END
		ELSE
		BEGIN
			-------------------------------------------------------------------------
			DECLARE @TTRules VARCHAR(64)

			DECLARE CUR_GameIDList CURSOR FOR 
			SELECT Rules FROM @tblFilterRules

			OPEN CUR_GameIDList

			FETCH NEXT FROM CUR_GameIDList INTO @TTRules

			WHILE @@FETCH_STATUS = 0
			BEGIN
				IF @BeginGameID LIKE @TTRules
				BEGIN
					SET @Reserve=1
					--print '������'+STR(@BeginGameID)+'  ' + @TTRules
					INSERT ReserveIdentifier(GameID) VALUES (@BeginGameID)
					BREAK
				END
				

				FETCH NEXT FROM CUR_GameIDList into @TTRules
			END

			CLOSE CUR_GameIDList
			DEALLOCATE CUR_GameIDList
			-------------------------------------------------------------------------
		END
	
		IF @Reserve=0
		BEGIN
			--print '������'+STR(@BeginGameID)
			INSERT [GameIdentifier] (GameID) VALUES(@BeginGameID)		
		END	

		SET @BeginGameID=@BeginGameID+1
	END


-- ID���� End
-------------------------------------------------------------------------

-- ���·���
UPDATE AccountsInfo
SET AccountsInfo.Gameid=GameIdentifier.Gameid
FROM GameIdentifier
WHERE AccountsInfo.userid=GameIdentifier.userid

