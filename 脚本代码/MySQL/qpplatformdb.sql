/*
Navicat MySQL Data Transfer

Source Server         : 192.168.1.124
Source Server Version : 50622
Source Host           : 192.168.1.124:3306
Source Database       : qpplatformdb

Target Server Type    : MYSQL
Target Server Version : 50622
File Encoding         : 65001

Date: 2016-04-09 10:13:54
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `gameitem`
-- ----------------------------
DROP TABLE IF EXISTS `gameitem`;
CREATE TABLE `gameitem` (
  `GameID` int(11) NOT NULL DEFAULT '0' COMMENT '游戏号码',
  `GameName` varchar(31) DEFAULT NULL COMMENT '游戏名字',
  `SuportType` int(11) DEFAULT '0' COMMENT '支持类型',
  `DataBaseAddr` varchar(15) DEFAULT '' COMMENT '连接地址',
  `DataBaseName` varchar(31) DEFAULT NULL COMMENT '数据库名',
  `ServerVersion` int(11) DEFAULT '0' COMMENT '服务器版本',
  `ClientVersion` int(11) DEFAULT '0' COMMENT '客户端版本',
  `ServerDLLName` varchar(31) DEFAULT NULL COMMENT '服务端名字',
  PRIMARY KEY (`GameID`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312 COMMENT='游戏列表';

-- ----------------------------
-- Records of gameitem
-- ----------------------------
INSERT INTO `gameitem` VALUES ('7', '德州扑克', '0', '192.168.1.124', 'QPTreasureDB', '16777217', '16777217', 'DZShowHandServer.dll');
INSERT INTO `gameitem` VALUES ('101', '牛牛', '0', '192.168.1.124', 'QPTreasureDB', '16777217', '16777217', 'OxServer.dll');

-- ----------------------------
-- Table structure for `gameroominfo`
-- ----------------------------
DROP TABLE IF EXISTS `gameroominfo`;
CREATE TABLE `gameroominfo` (
  `ServerID` int(11) NOT NULL AUTO_INCREMENT COMMENT '房间号码',
  `ServerName` varchar(31) DEFAULT NULL COMMENT '房间名称',
  `KindID` int(11) DEFAULT '0' COMMENT '挂接类型',
  `NodeID` int(11) DEFAULT '0' COMMENT '挂接节点',
  `SortID` int(11) DEFAULT '0' COMMENT '排列标识',
  `GameID` int(11) DEFAULT '0' COMMENT '模块标识',
  `TableCount` int(11) DEFAULT '60' COMMENT '桌子数目',
  `ServerKind` int(11) DEFAULT '0',
  `ServerType` int(11) DEFAULT '0' COMMENT '房间类型',
  `ServerPort` int(11) DEFAULT '0' COMMENT '服务端口',
  `ServerPasswd` varchar(31) DEFAULT NULL,
  `DataBaseName` varchar(31) DEFAULT NULL COMMENT '数据库名',
  `DataBaseAddr` varchar(15) DEFAULT '' COMMENT '连接地址',
  `CellScore` bigint(20) DEFAULT '0' COMMENT '单位积分',
  `RevenueRatio` tinyint(4) DEFAULT NULL COMMENT '税收比例',
  `ServiceScore` bigint(20) DEFAULT '0' COMMENT '服务费用',
  `RestrictScore` bigint(20) DEFAULT '0' COMMENT '限制积分',
  `MinTableScore` bigint(20) DEFAULT '0' COMMENT '最低积分',
  `MinEnterScore` bigint(20) DEFAULT '0',
  `MaxEnterScore` bigint(20) DEFAULT '0',
  `DefPutScore` bigint(20) DEFAULT '0',
  `LowPutScore` bigint(20) DEFAULT '0',
  `ExchangeRatio` int(11) DEFAULT '0',
  `MagicExpScore` bigint(20) DEFAULT '0',
  `GratuityScore` bigint(20) DEFAULT '0',
  `WinExperience` int(11) DEFAULT NULL,
  `FailExperience` int(11) DEFAULT NULL,
  `MaxEnterMember` int(11) DEFAULT '0',
  `MaxPlayer` int(11) DEFAULT '0' COMMENT '最大数目',
  `ServerRule` int(11) DEFAULT '0' COMMENT '房间规则',
  `DistributeRule` int(11) DEFAULT '0' COMMENT '分组规则',
  `MinDistributeUser` int(11) DEFAULT '0' COMMENT '最少人数',
  `DistributeTimeSpace` int(11) DEFAULT '0' COMMENT '分组间隔',
  `DistributeDrawCount` int(11) DEFAULT '0' COMMENT '分组局数',
  `MinPartakeGameUser` int(11) DEFAULT '0' COMMENT '游戏最少人数',
  `MaxPartakeGameUser` int(11) DEFAULT '0' COMMENT '游戏最多人数',
  `AttachUserRight` int(11) DEFAULT '0',
  `ServiceMachine` varchar(32) DEFAULT '' COMMENT '运行机器',
  `CustomRule` varchar(2048) DEFAULT '' COMMENT '自定规则',
  `Nullity` tinyint(4) DEFAULT '0' COMMENT '禁止服务',
  `ServerNote` varchar(64) DEFAULT '' COMMENT '备注说明',
  `CacheServerPort` int(11) DEFAULT '6379' COMMENT '缓存服务器端口',
  `CreateDateTime` timestamp NULL DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
  `ModifyDateTime` timestamp NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '修改时间',
  `CacheServerAddr` varchar(32) DEFAULT '127.0.0.1' COMMENT '缓存服务器地址',
  `EnterPassword` varchar(32) DEFAULT '',
  `MinEnterMember` int(11) DEFAULT '0',
  PRIMARY KEY (`ServerID`)
) ENGINE=InnoDB AUTO_INCREMENT=8 DEFAULT CHARSET=gb2312 COMMENT='房间列表';

-- ----------------------------
-- Records of gameroominfo
-- ----------------------------
INSERT INTO `gameroominfo` VALUES ('3', '德州扑克', '0', '0', '0', '7', '5', '1', '1', '10001', '', 'QPTreasureDB', '192.168.1.124', '100', '0', '0', '0', '300', '0', '0', '0', '0', '0', '0', '0', '5', '1', '1', '0', '15730704', '0', '0', '0', '0', '0', '0', '0', '6B32B3463507A3EB2CE444381F545AD6', '00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000', '0', '', '6379', null, '2016-04-08 12:30:44', '192.168.1.124', '', '0');
INSERT INTO `gameroominfo` VALUES ('4', '牛牛', '0', '0', '0', '101', '60', '1', '1', '10001', '', 'QPTreasureDB', '192.168.1.124', '1', '0', '0', '0', '50', '0', '0', '0', '0', '0', '0', '0', '5', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '4D97D81B664307821361A097944E2784', '00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000', '0', '', '6379', null, '2016-04-06 13:47:17', '192.168.1.124', '', '0');
INSERT INTO `gameroominfo` VALUES ('5', '德州扑克', '0', '0', '0', '7', '3', '1', '1', '10001', '', 'QPTreasureDB', '192.168.1.124', '100', '0', '0', '0', '1000', '1000', '0', '10000', '5000', '1', '100', '100', '5', '1', '0', '0', '15730960', '0', '0', '0', '0', '0', '0', '0', '53157A17F071DFCF8699C47D527D0E94', '00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000', '0', '', '6379', null, '2016-04-06 18:59:35', '192.168.1.124', '', '0');
INSERT INTO `gameroominfo` VALUES ('6', '德州扑克', '0', '0', '0', '7', '60', '1', '1', '10001', '', 'QPTreasureDB', '192.168.1.124', '100', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '5', '1', '0', '0', '15730704', '0', '0', '0', '0', '0', '0', '0', 'BD40D44456A94D95E8B969DDE7837B11', '00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000', '0', '', '6379', null, '2016-04-06 13:47:16', '192.168.1.124', '', '0');
INSERT INTO `gameroominfo` VALUES ('7', '德州扑克', '0', '0', '0', '7', '15', '1', '1', '10001', '', 'QPTreasureDB', '192.168.1.124', '100', '0', '0', '0', '500', '0', '0', '10000', '5000', '1', '500', '500', '5', '1', '0', '0', '15730704', '0', '0', '0', '0', '0', '0', '0', 'C7ABF848D9AA8E5CAB54E404E1AD88D6', '00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000', '0', '', '6379', '2016-03-15 17:15:31', '2016-04-06 13:47:15', '192.168.1.124', '', '0');

-- ----------------------------
-- Table structure for `matchimmediate`
-- ----------------------------
DROP TABLE IF EXISTS `matchimmediate`;
CREATE TABLE `matchimmediate` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `MatchID` int(11) DEFAULT '0',
  `MatchNo` smallint(6) DEFAULT NULL,
  `StartUserCount` int(11) DEFAULT '0' COMMENT '开赛人数',
  `AndroidUserCount` int(11) DEFAULT '0' COMMENT '一组比赛中最大机器人数目',
  `InitialBase` int(11) DEFAULT '0' COMMENT '初始基数(即初始游戏底分)',
  `InitialScore` int(11) DEFAULT '0' COMMENT '初始比赛分(为0时 表示使用当前金币为比赛分)',
  `MinEnterGold` int(11) DEFAULT '0' COMMENT '最少进入金币',
  `PlayCount` int(11) DEFAULT '0' COMMENT '游戏局数',
  `SwitchTableCount` int(11) DEFAULT '0' COMMENT '换桌局数(为0时 表示不换桌)',
  `PrecedeTimer` int(11) DEFAULT '0' COMMENT '能得到优先分配的等待时间(单位 秒)',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of matchimmediate
-- ----------------------------
INSERT INTO `matchimmediate` VALUES ('1', '1', '1', '4', '3', '100', '10000', '0', '5', '3', '100');

-- ----------------------------
-- Table structure for `matchlocktime`
-- ----------------------------
DROP TABLE IF EXISTS `matchlocktime`;
CREATE TABLE `matchlocktime` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `MatchID` int(11) DEFAULT '0' COMMENT '比赛标识',
  `MatchNo` smallint(6) DEFAULT NULL COMMENT '比赛场次',
  `StartTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '开始时间',
  `EndTime` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '结束时间',
  `InitScore` bigint(20) DEFAULT NULL COMMENT '初始积分',
  `CullScore` bigint(20) DEFAULT NULL COMMENT '淘汰积分',
  `MinPlayCount` int(11) DEFAULT '0' COMMENT '有效局数',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of matchlocktime
-- ----------------------------

-- ----------------------------
-- Table structure for `matchpublic`
-- ----------------------------
DROP TABLE IF EXISTS `matchpublic`;
CREATE TABLE `matchpublic` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `MatchID` int(11) DEFAULT '0' COMMENT '比赛标识',
  `MatchNo` smallint(6) DEFAULT NULL COMMENT '比赛场次',
  `KindID` int(11) DEFAULT '0' COMMENT '类型标识',
  `MatchName` varchar(50) DEFAULT NULL COMMENT '比赛名称',
  `MatchType` int(11) DEFAULT '0' COMMENT '比赛类型 0:定时赛 1:即时赛',
  `MatchFeeType` int(11) DEFAULT '0' COMMENT '扣费类型 0:金币 1:元宝',
  `MatchFee` bigint(20) DEFAULT NULL COMMENT '扣费数量',
  `MemberOrder` int(11) DEFAULT '0' COMMENT '参加条件 会员类型大于或等于该值的玩家可参加',
  `CollectDate` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '创建日期',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of matchpublic
-- ----------------------------
INSERT INTO `matchpublic` VALUES ('1', '1', '1', '7', '测试', '1', '0', '0', '0', '2016-03-16 17:49:27');

-- ----------------------------
-- Table structure for `matchreward`
-- ----------------------------
DROP TABLE IF EXISTS `matchreward`;
CREATE TABLE `matchreward` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `MatchID` int(11) DEFAULT '0',
  `MatchNo` smallint(6) DEFAULT NULL COMMENT '比赛场次',
  `MatchRank` smallint(6) DEFAULT NULL,
  `RewardGold` bigint(20) DEFAULT NULL COMMENT '奖励的游戏币',
  `RewardMedal` int(11) DEFAULT '0' COMMENT '奖励的元宝',
  `RewardExperience` int(11) DEFAULT '0' COMMENT '奖励的经验值',
  `RewardDescibe` varchar(128) DEFAULT NULL COMMENT '奖励描述',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of matchreward
-- ----------------------------
INSERT INTO `matchreward` VALUES ('1', '1', '1', '1', '1000', '10', '20', '1000金币');

-- ----------------------------
-- Table structure for `matchwebshow`
-- ----------------------------
DROP TABLE IF EXISTS `matchwebshow`;
CREATE TABLE `matchwebshow` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `MatchID` int(11) DEFAULT NULL COMMENT '比赛标识',
  `MatchNo` smallint(6) DEFAULT NULL COMMENT '比赛场次',
  `SmallImageUrl` varchar(255) DEFAULT NULL COMMENT '小图地址',
  `BigImageUrl` varchar(255) DEFAULT NULL COMMENT '大图地址',
  `MatchSummary` varchar(255) DEFAULT NULL COMMENT '比赛摘要',
  `MatchContent` longtext COMMENT '比赛说明',
  `IsRecommend` tinyint(4) DEFAULT '0' COMMENT '是否推荐至首页',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of matchwebshow
-- ----------------------------

-- ----------------------------
-- Table structure for `onlinestatusinfo`
-- ----------------------------
DROP TABLE IF EXISTS `onlinestatusinfo`;
CREATE TABLE `onlinestatusinfo` (
  `KindID` int(11) NOT NULL DEFAULT '0' COMMENT '类型标识',
  `ServerID` int(11) NOT NULL DEFAULT '0' COMMENT '房间标识',
  `OnLineCount` int(11) DEFAULT '0' COMMENT '在线人数',
  `InsertDateTime` datetime DEFAULT NULL COMMENT '插入时间',
  `ModifyDateTime` datetime DEFAULT NULL COMMENT '修改时间',
  PRIMARY KEY (`KindID`,`ServerID`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312 COMMENT='服务器在线玩家信息';

-- ----------------------------
-- Records of onlinestatusinfo
-- ----------------------------

-- ----------------------------
-- Table structure for `onlinestreaminfo`
-- ----------------------------
DROP TABLE IF EXISTS `onlinestreaminfo`;
CREATE TABLE `onlinestreaminfo` (
  `ID` int(11) NOT NULL DEFAULT '0' COMMENT '房间标识',
  `MachineID` varchar(32) DEFAULT NULL COMMENT '机器标识',
  `MachineServer` varchar(32) DEFAULT NULL COMMENT '机器名字',
  `InsertDateTime` datetime DEFAULT NULL COMMENT '插入时间',
  `OnLineCountSum` int(11) DEFAULT '0' COMMENT '用户数目',
  `AndroidCountSum` int(11) DEFAULT '0',
  `OnLineCountKind` varchar(2048) DEFAULT NULL COMMENT '在线信息',
  `AndroidCountKind` varchar(2048) DEFAULT '',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312 COMMENT='服务器在线玩家信息';

-- ----------------------------
-- Records of onlinestreaminfo
-- ----------------------------

-- ----------------------------
-- Table structure for `systemmessage`
-- ----------------------------
DROP TABLE IF EXISTS `systemmessage`;
CREATE TABLE `systemmessage` (
  `ID` int(11) NOT NULL AUTO_INCREMENT COMMENT '索引标识',
  `MessageType` int(11) DEFAULT '0' COMMENT '消息范围(1:游戏,2:房间,3:全部)',
  `ServerRange` varchar(1024) DEFAULT NULL COMMENT '房间范围(0:所有房间)',
  `MessageString` varchar(1024) DEFAULT NULL COMMENT '消息内容',
  `StartTime` datetime DEFAULT NULL COMMENT '开始时间',
  `ConcludeTime` datetime DEFAULT NULL COMMENT '结束时间',
  `TimeRate` int(11) DEFAULT '0' COMMENT '时间频率(多长时间发送一次)',
  `Nullity` tinyint(4) DEFAULT NULL COMMENT '禁止标志',
  `CreateDate` datetime DEFAULT NULL COMMENT '创建时间',
  `CreateMasterID` int(11) DEFAULT '0' COMMENT '创建管理员ID',
  `UpdateDate` datetime DEFAULT NULL COMMENT '最后更新时间',
  `UpdateMasterID` int(11) DEFAULT '0' COMMENT '修改管理员ID',
  `UpdateCount` int(11) DEFAULT '0' COMMENT '修改次数',
  `CollectNote` varchar(512) DEFAULT NULL COMMENT '备注',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312 COMMENT='系统消息';

-- ----------------------------
-- Records of systemmessage
-- ----------------------------

-- ----------------------------
-- Table structure for `taskinfo`
-- ----------------------------
DROP TABLE IF EXISTS `taskinfo`;
CREATE TABLE `taskinfo` (
  `TaskID` int(11) NOT NULL DEFAULT '0' COMMENT '任务标识',
  `TaskName` varchar(50) DEFAULT '' COMMENT '任务名称',
  `TaskDescription` varchar(500) DEFAULT '' COMMENT '任务描述',
  `TaskType` int(11) DEFAULT '0' COMMENT '任务类型 1:总赢局 2:总局数 4:首胜 8:比赛任务',
  `UserType` tinyint(4) DEFAULT NULL COMMENT '可领取任务用户类型',
  `KindID` int(11) DEFAULT '0' COMMENT '任务所属游戏标识',
  `MatchID` int(11) DEFAULT '0' COMMENT '比赛任务 比赛ID',
  `Innings` int(11) DEFAULT '0' COMMENT '完成任务需要局数',
  `StandardAwardGold` int(11) DEFAULT '0' COMMENT '普通玩家奖励金币',
  `StandardAwardMedal` int(11) DEFAULT '0' COMMENT '普通玩家奖励元宝',
  `MemberAwardGold` int(11) DEFAULT '0' COMMENT '会员奖励金币',
  `MemberAwardMedal` int(11) DEFAULT '0' COMMENT '会员奖励元宝',
  `TimeLimit` int(11) DEFAULT '0' COMMENT '时间限制 单位:分钟',
  `InputDate` datetime DEFAULT NULL COMMENT '录入日期',
  PRIMARY KEY (`TaskID`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312 COMMENT='任务列表';

-- ----------------------------
-- Records of taskinfo
-- ----------------------------
INSERT INTO `taskinfo` VALUES ('1', '小试身手', '在任意牌局内进行5局游戏', '2', null, '7', '0', '5', '500', '0', '500', '0', '86400', null);
INSERT INTO `taskinfo` VALUES ('2', '崭露头角', '在任意牌局内获胜5局游戏', '1', null, '7', '0', '5', '100', '0', '100', '0', '86400', null);
INSERT INTO `taskinfo` VALUES ('3', '混个脸熟', '在任意牌局内进行30局游戏', '2', null, '7', '0', '30', '100', '0', '100', '0', '86400', null);
INSERT INTO `taskinfo` VALUES ('4', '牌技精湛', '在任意牌局内获胜30局游戏', '1', null, '7', '0', '30', '2000', '0', '2000', '0', '86400', null);
INSERT INTO `taskinfo` VALUES ('5', '牌场老手', '在任意牌局内进行50局游戏', '2', null, '7', '0', '50', '2000', '0', '2000', '0', '86400', null);
INSERT INTO `taskinfo` VALUES ('6', '胜券在握', '在任意牌局内获胜50局游戏', '1', null, '7', '0', '50', '3000', '0', '3000', '0', '86400', null);
INSERT INTO `taskinfo` VALUES ('7', '坚持不懈', '在任意牌局内进行80局游戏', '2', null, '7', '0', '80', '5000', '0', '5000', '0', '86400', null);
INSERT INTO `taskinfo` VALUES ('8', '大杀四方', '在任意牌局内获胜80局游戏', '1', null, '7', '0', '80', '6000', '0', '6000', '0', '86400', null);
INSERT INTO `taskinfo` VALUES ('9', '展现自己', '在任意牌局内发送5个表情', '4', null, '7', '0', '5', '500', '0', '500', '0', '86400', null);
INSERT INTO `taskinfo` VALUES ('10', '秀出自己', '在任意牌局内发送10个表情', '4', null, '7', '0', '10', '1000', '0', '1000', '0', '86400', null);
INSERT INTO `taskinfo` VALUES ('11', '我就是壕', '充值任意金额', '8', null, '7', '0', '1', '500', '0', '500', '0', '86400', null);

-- ----------------------------
-- Table structure for `versionlist`
-- ----------------------------
DROP TABLE IF EXISTS `versionlist`;
CREATE TABLE `versionlist` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `Name` varchar(16) DEFAULT '',
  `URL` varchar(32) DEFAULT '',
  `PatchFile` varchar(32) DEFAULT '' COMMENT '补丁包文件',
  `PatchDescribe` text,
  `IsReinstall` tinyint(4) DEFAULT '0' COMMENT '是否需要重新安装',
  `InputDate` timestamp NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '上传时间',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of versionlist
-- ----------------------------
INSERT INTO `versionlist` VALUES ('1', '1.0.1', 'http://192.168.1.124:8181/down/', '1.0.1.zip', '1', '0', '2016-04-03 11:36:33');
INSERT INTO `versionlist` VALUES ('2', '1.0.2', 'http://192.168.1.124:8181/down/', '1.0.2.zip', '2', '0', '2016-04-03 11:36:34');
INSERT INTO `versionlist` VALUES ('3', '1.0.3', 'http://192.168.1.124:8181/down/', '1.0.3.zip', '3', '0', '2016-04-03 14:01:32');
INSERT INTO `versionlist` VALUES ('4', '1.0.4', 'http://192.168.1.124:8181/down/', '1.0.4.zip', '4', '1', '2016-04-06 08:33:47');

-- ----------------------------
-- Procedure structure for `GSP_GP_AndroidAddParameter`
-- ----------------------------
DROP PROCEDURE IF EXISTS `GSP_GP_AndroidAddParameter`;
DELIMITER ;;
CREATE DEFINER=`root`@`%` PROCEDURE `GSP_GP_AndroidAddParameter`(
	wServerID INT,								-- 房间标识
	dwServiceMode INT,							-- 服务模式
	dwAndroidCount INT,						-- 机器数目
	dwEnterTime INT,							-- 进入时间
	dwLeaveTime INT,							-- 离开时间
	dwEnterMinInterval INT,					-- 进入间隔
	dwEnterMaxInterval INT,					-- 进入间隔
	dwLeaveMinInterval	INT,					-- 离开间隔
	dwLeaveMaxInterval	INT,					-- 离开间隔
	lTakeMinScore	BIGINT,						-- 携带分数
	lTakeMaxScore BIGINT,						-- 携带分数
	dwSwitchMinInnings INT,					-- 换桌局数
	dwSwitchMaxInnings INT						-- 换桌局数
)
BEGIN
	-- 查询批次	
	DECLARE dwBatchID INT;

	-- 插入数据
	INSERT qpaccountsdb.AndroidConfigure(ServerID,ServiceMode,AndroidCount,EnterTime,LeaveTime,EnterMinInterval,EnterMaxInterval,LeaveMinInterval,
			LeaveMaxInterval,TakeMinScore,TakeMaxScore,SwitchMinInnings,SwitchMaxInnings)
	VALUES(wServerID,dwServiceMode,dwAndroidCount,dwEnterTime,dwLeaveTime,dwEnterMinInterval,dwEnterMaxInterval,dwLeaveMinInterval,
			dwLeaveMaxInterval,lTakeMinScore,lTakeMaxScore,dwSwitchMinInnings,dwSwitchMaxInnings);

	SET dwBatchID=LAST_INSERT_ID();
	
	-- 查询数据
	SELECT * FROM qpaccountsdb.AndroidConfigure WHERE BatchID=dwBatchID;

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `GSP_GP_AndroidDeleteParameter`
-- ----------------------------
DROP PROCEDURE IF EXISTS `GSP_GP_AndroidDeleteParameter`;
DELIMITER ;;
CREATE DEFINER=`root`@`%` PROCEDURE `GSP_GP_AndroidDeleteParameter`(
	dwBatchID INT								-- 批次标识	
)
BEGIN

	-- 查询参数
	SELECT * FROM qpaccountsdb.AndroidConfigure WHERE BatchID=dwBatchID;

	-- 删除参数
	DELETE FROM qpaccountsdb.AndroidConfigure WHERE BatchID=dwBatchID;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `GSP_GP_AndroidGetParameter`
-- ----------------------------
DROP PROCEDURE IF EXISTS `GSP_GP_AndroidGetParameter`;
DELIMITER ;;
CREATE DEFINER=`root`@`%` PROCEDURE `GSP_GP_AndroidGetParameter`(
	wServerID INT								-- 房间标识	
)
BEGIN

	-- 查询参数
	SELECT * FROM qpaccountsdb.AndroidConfigure WHERE ServerID=wServerID;

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `GSP_GP_AndroidModifyParameter`
-- ----------------------------
DROP PROCEDURE IF EXISTS `GSP_GP_AndroidModifyParameter`;
DELIMITER ;;
CREATE DEFINER=`root`@`%` PROCEDURE `GSP_GP_AndroidModifyParameter`(
	dwDatchID INT,								-- 批次标识
	dwServiceMode INT,							-- 服务模式
	dwAndroidCount INT,						-- 机器数目
	dwEnterTime INT,							-- 进入时间
	dwLeaveTime INT,							-- 离开时间
	dwEnterMinInterval INT,					-- 进入间隔
	dwEnterMaxInterval INT,					-- 进入间隔
	dwLeaveMinInterval	INT,					-- 离开间隔
	dwLeaveMaxInterval	INT,					-- 离开间隔
	lTakeMinScore	BIGINT,						-- 携带分数
	lTakeMaxScore	BIGINT,						-- 携带分数
	dwSwitchMinInnings INT,					-- 换桌局数
	dwSwitchMaxInnings INT						-- 换桌局数
)
BEGIN

	-- 更新参数
	UPDATE qpaccountsdb.AndroidConfigure SET ServiceMode=dwServiceMode,AndroidCount=dwAndroidCount,EnterTime=dwEnterTime,LeaveTime=dwLeaveTime,
		EnterMinInterval=dwEnterMinInterval,EnterMaxInterval=dwEnterMaxInterval,LeaveMinInterval=dwLeaveMinInterval,
		LeaveMaxInterval=dwLeaveMaxInterval,TakeMinScore=lTakeMinScore,TakeMaxScore=lTakeMaxScore,SwitchMinInnings=dwSwitchMinInnings,
		SwitchMaxInnings=dwSwitchMaxInnings
	WHERE BatchID=dwDatchID;
	
	-- 查询数据
	SELECT * FROM qpaccountsdb.AndroidConfigure WHERE BatchID=dwDatchID;

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `GSP_GP_OnLineCountInfo`
-- ----------------------------
DROP PROCEDURE IF EXISTS `GSP_GP_OnLineCountInfo`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `GSP_GP_OnLineCountInfo`(
	strMachineID NVARCHAR(32),					-- 机器标识
	strMachineServer NVARCHAR(32),				-- 机器名称
	dwOnLineCountSum INT,						-- 总在线数
	dwAndroidCountSum INT,						-- 总在线数
	strOnLineCountKind NVARCHAR(2048),			-- 在线信息
	strAndroidCountKind NVARCHAR(2048)			-- 机器人数
)
BEGIN

INSERT OnLineStreamInfo (MachineID, MachineServer, OnLineCountSum,AndroidCountSum,OnLineCountKind,AndroidCountKind)
VALUES (strMachineID, strMachineServer, dwOnLineCountSum,dwAndroidCountSum,strOnLineCountKind,strAndroidCountKind);

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `GSP_GR_LoadTaskList`
-- ----------------------------
DROP PROCEDURE IF EXISTS `GSP_GR_LoadTaskList`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `GSP_GR_LoadTaskList`()
BEGIN

	-- 查询任务
	SELECT * FROM TaskInfo;

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `GSP_GR_QueryTaskInfo`
-- ----------------------------
DROP PROCEDURE IF EXISTS `GSP_GR_QueryTaskInfo`;
DELIMITER ;;
CREATE DEFINER=`root`@`%` PROCEDURE `GSP_GR_QueryTaskInfo`(wKindID INT,	-- 类型ID
	dwUserID INT,	-- 用户ID
	strPassword NCHAR(32),	-- 用户密码
	OUT strErrorDescribe NVARCHAR(127),-- 输出信息
	OUT nRet INT)
label_pro:BEGIN

	-- 查询用户
	IF not exists(SELECT UserID FROM QPAccountsDB.AccountsInfo WHERE UserID=dwUserID) THEN
	
		SET strErrorDescribe = '抱歉，你的用户信息不存在或者密码不正确！';
		SET nRet = 1;
		LEAVE label_pro;
	END IF;

	-- 超时处理
	UPDATE QPAccountsDB.AccountsTask SET TaskStatus=2 
	WHERE UserID=dwUserID AND TaskStatus=0 AND TimeLimit<TIMESTAMPDIFF(SECOND,DATE(InputDate),NOW());

	-- 查询任务
	SELECT ID, TaskID,TaskStatus,Progress,(TimeLimit-TIMESTAMPDIFF(SECOND,InputDate,NOW())) AS ResidueTime FROM QPAccountsDB.AccountsTask 
	WHERE UserID=dwUserID AND TIMESTAMPDIFF(DAY,DATE(InputDate),NOW())=0 AND (((wKindID=KindID) AND TaskStatus=0) OR wKindID=0);

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `GSP_GR_TaskForward`
-- ----------------------------
DROP PROCEDURE IF EXISTS `GSP_GR_TaskForward`;
DELIMITER ;;
CREATE DEFINER=`root`@`%` PROCEDURE `GSP_GR_TaskForward`(dwUserID INT, 	-- 用户ID
	dwID INT,	-- 类型ID
	wTaskProgress INT,	-- 用户ID
	cbTaskStatus INT,
	OUT strErrorDescribe NVARCHAR(127),-- 输出信息
	OUT nRet INT)
label_pro:BEGIN

	-- 查询用户
	IF not exists(SELECT UserID FROM QPAccountsDB.AccountsInfo WHERE UserID=dwUserID) THEN
	
		SET strErrorDescribe = '抱歉，你的用户信息不存在或者密码不正确！';
		SET nRet = 1;
		LEAVE label_pro;
	END IF;

	-- 超时处理
	UPDATE QPAccountsDB.AccountsTask SET TaskStatus=2 
	WHERE UserID=dwUserID AND TaskStatus=0 AND TimeLimit<TIMESTAMPDIFF(SECOND,DATE(InputDate),NOW());

	-- 任务更新
	UPDATE QPAccountsDB.AccountsTask set Progress = wTaskProgress, TaskStatus = cbTaskStatus where ID = dwID;

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `GSP_GR_TaskTake`
-- ----------------------------
DROP PROCEDURE IF EXISTS `GSP_GR_TaskTake`;
DELIMITER ;;
CREATE DEFINER=`root`@`%` PROCEDURE `GSP_GR_TaskTake`(dwUserID INT,								-- 用户 I D
	wTaskID  INT,								-- 任务 I D
	strPassword NCHAR(32),						-- 用户密码
	strClientIP NVARCHAR(15),					-- 连接地址
	strMachineID NVARCHAR(32),					-- 机器标识
	OUT strErrorDescribe NVARCHAR(127),				-- 输出信息
	out nRet INT)
label_pro:BEGIN

	-- 查询用户
	DECLARE nMemberOrder INT;

	-- 判断数目	
	DECLARE TaskTakeMaxCount INT;
	
	-- 统计任务
	DECLARE TaskTakeCount INT;

	-- 任务对象
	DECLARE nKindID INT;
	DECLARE nUserType INT;
	DECLARE nTimeLimit INT;
	DECLARE nTaskType INT;
	DECLARE nTaskObject INT;
	DECLARE nAwardGold INT;

	SELECT MemberOrder INTO nMemberOrder FROM QPAccountsDB.AccountsInfo 
	WHERE UserID=dwUserID AND LogonPass=strPassword;
	IF nMemberOrder IS NULL then
		SET strErrorDescribe = '抱歉，你的用户信息不存在或者密码不正确！';
		SET nRet = 1;
		leave label_pro;
	END if;

	-- 重复领取
	IF exists(SELECT * FROM QPAccountsDB.AccountsTask 
	WHERE UserID=dwUserID AND TaskID=wTaskID AND DATEDIFF(InputDate,CURRENT_TIMESTAMP())=0) 
	THEN
		SET strErrorDescribe = '抱歉，同一个任务每天只能领取一次！';
		SET nRet = 3;
		leave label_pro;
	END IF;

	-- 任务对象
	SELECT KindID,UserType,TimeLimit,TaskType,Innings, StandardAwardGold INTO nKindID,nUserType,nTimeLimit,nTaskType,nTaskObject, nAwardGold
	FROM TaskInfo WHERE TaskID=wTaskID;
	IF nKindID IS NULL THEN
		SET strErrorDescribe = '抱歉，系统未找到您领取的任务信息！';
		SET nRet = 4;
		leave label_pro;
	END IF;

	-- 普通玩家
	IF nMemberOrder=0 AND (nUserType&0x01)=0 THEN
		SET strErrorDescribe = '抱歉，该任务暂时不对普通玩家开放！';
		SET nRet = 5;
		leave label_pro;
	END IF;

	-- 会员玩家
	IF nMemberOrder>0 AND (nUserType&0x02)=0 THEN
		SET strErrorDescribe = '抱歉，该任务暂时不对会员玩家开放！';
		SET nRet = 6;
		leave label_pro;
	END	IF;

	-- 插入任务
	INSERT INTO QPAccountsDB.AccountsTask(UserID,TaskID,TaskType,TaskObject,KindID,TimeLimit,InputDate,AwardGold) 
	VALUES(dwUserID,wTaskID,nTaskType,nTaskObject,nKindID,nTimeLimit,NOW(),nAwardGold);

	-- 成功提示
	SET strErrorDescribe = '恭喜您，任务领取成功！';
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `GSP_GS_DeleteGameRoom`
-- ----------------------------
DROP PROCEDURE IF EXISTS `GSP_GS_DeleteGameRoom`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `GSP_GS_DeleteGameRoom`(wServerID INT,								-- 房间标识
	OUT strErrorDescribe NVARCHAR(127),		-- 输出信息
	OUT nRet INT)
label_pro:BEGIN

	-- 查找房间
	DECLARE nServerID INT;
	SELECT ServerID into nServerID FROM GameRoomInfo WHERE ServerID=wServerID;

	-- 结果判断
	IF nServerID IS NULL THEN
		SET strErrorDescribe='游戏房间不存在或者已经被删除了，房间修改失败！';
		SET nRet = 1;
		leave label_pro;
	END IF;

	-- 删除房间
	DELETE FROM GameRoomInfo WHERE ServerID=wServerID;

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `GSP_GS_InsertGameRoom`
-- ----------------------------
DROP PROCEDURE IF EXISTS `GSP_GS_InsertGameRoom`;
DELIMITER ;;
CREATE DEFINER=`root`@`%` PROCEDURE `GSP_GS_InsertGameRoom`(wGameID INT,								-- 模块标识
	wKindID INT,								-- 类型标识
	wNodeID INT,								-- 挂接标识
	wSortID INT,								-- 排序标识

	-- 积分配置
	lCellScore INT,							-- 单元积分
	wRevenueRatio INT,							-- 税收比例
	lServiceScore INT,							-- 服务费用

	-- 限制配置
	lRestrictScore	BIGINT,						-- 限制积分
	lMinTableScore	BIGINT,						-- 最低积分
	lMinEnterScore	BIGINT,						-- 最低积分
	lMaxEnterScore	BIGINT,						-- 最高积分

	-- 带入设置
	lDefPutScore BIGINT,						-- 默认带入积分
	lLowPutScore BIGINT,						-- 低于多少带入积分
	iExchangeRatio float,						-- 兑换比例
	
	-- 其他设置
	lMagicExpScore BIGINT,						-- 魔法表情
	lGratuityScore BIGINT,						-- 打赏积分
	dwWinExperience INT,
	dwFailExperience INT,
	
	-- 会员限制
	cbMinEnterMember INT,						-- 最低会员
	cbMaxEnterMember INT,						-- 最高会员

	-- 房间配置
	dwServerRule INT,							-- 房间规则
	dwAttachUserRight INT,						-- 附加权限

	-- 房间属性
	wMaxPlayer INT,							-- 游戏人数
	wTableCount INT,							-- 桌子数目
	wServerPort INT,							-- 服务端口
	wServerKind INT,							-- 房间类型
	wServerType INT,							-- 服务类型
	strServerName NVARCHAR(32),				-- 房间名字
	strServerPasswd NVARCHAR(32),				-- 房间密码

	-- 分组属性
	cbDistributeRule INT,						-- 分组规则
	wMinDistributeUser INT,					-- 最少人数	
	wDistributeTimeSpace INT,					-- 分组间隔
	wDistributeDrawCount INT,					-- 分组局数
	wMinPartakeGameUser INT,					-- 最少人数
	wMaxPartakeGameUser INT,					-- 最多人数	

	-- 连接信息
	strDataBaseName NVARCHAR(32),				-- 数据库名
	strDataBaseAddr NVARCHAR(15),				-- 连接地址

	strCacheServerAddr NVARCHAR(32),			-- 缓存服务器地址
	wCacheServerPort INT,						-- 缓存服务器端口
	
	-- 数据设置
	strCustomRule NVARCHAR(2048),				-- 定制规则
	strServiceMachine NVARCHAR(32),			-- 服务机器

	-- 输出信息
	OUT strErrorDescribe NVARCHAR(127),		-- 输出信息
	OUT nRet INT)
label_pro:BEGIN

	-- 查找模块
	DECLARE nGameID INT;
	
	-- 获取端口
	DECLARE nServerPort INT;
	
	SELECT GameID into nGameID FROM GameItem WHERE GameID=wGameID;

	-- 存在判断
	IF nGameID IS NULL THEN
		SET strErrorDescribe='相应的游戏服务组件信息不存在，房间创建失败！';
		SET nRet = 1;
		leave label_pro;
	END IF;

	-- 获取端口
	IF wServerPort<>0 THEN
		
		SELECT ServerPort into nServerPort FROM GameRoomInfo WHERE ServerPort=wServerPort AND ServiceMachine=strServiceMachine;

		-- 端口判断
		IF nServerPort=wServerPort THEN
			SET strErrorDescribe=N'存在服务端口相同的游戏房间，房间创建失败！';
			SET nRet = 1;
			leave label_pro;
		END IF;

	END IF;

	-- 创建房间
	INSERT INTO GameRoomInfo (ServerName, KindID, NodeID, SortID, GameID, TableCount, ServerPort, ServerKind, ServerType, CellScore,
		RevenueRatio, ServiceScore, RestrictScore, MinTableScore, MinEnterScore, MaxEnterScore, DefPutScore, LowPutScore, ExchangeRatio, MinEnterMember, MaxEnterMember,
		MaxPlayer, ServerRule, DistributeRule, MinDistributeUser, DistributeTimeSpace, DistributeDrawCount,MinPartakeGameUser,
		MaxPartakeGameUser, AttachUserRight, ServerPasswd, DataBaseName, DataBaseAddr, CacheServerAddr, CacheServerPort, ServiceMachine, CustomRule, MagicExpScore, GratuityScore, WinExperience, FailExperience)
	VALUES (strServerName, wKindID, wNodeID, wSortID, wGameID, wTableCount, wServerPort, wServerKind, wServerType, lCellScore,
		wRevenueRatio, lServiceScore, lRestrictScore, lMinTableScore, lMinEnterScore, lMaxEnterScore, lDefPutScore, lLowPutScore, iExchangeRatio, cbMinEnterMember,
		cbMaxEnterMember, wMaxPlayer, dwServerRule, cbDistributeRule, wMinDistributeUser,wDistributeTimeSpace,
		wDistributeDrawCount, wMinPartakeGameUser,wMaxPartakeGameUser, dwAttachUserRight, strServerPasswd, strDataBaseName, strDataBaseAddr,
		strCacheServerAddr, wCacheServerPort, strServiceMachine, strCustomRule, lMagicExpScore, lGratuityScore, dwWinExperience, dwFailExperience);

	-- 加载房间
	SELECT GameRoomInfo.*, GameItem.GameName, GameItem.ServerVersion, GameItem.ClientVersion, GameItem.ServerDLLName FROM GameRoomInfo, GameItem
	WHERE GameRoomInfo.GameID=GameItem.GameID AND ServerID=LAST_INSERT_ID();

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `GSP_GS_LoadGameItem`
-- ----------------------------
DROP PROCEDURE IF EXISTS `GSP_GS_LoadGameItem`;
DELIMITER ;;
CREATE DEFINER=`root`@`%` PROCEDURE `GSP_GS_LoadGameItem`()
BEGIN

	-- 加载模块
	SELECT * FROM GameItem ORDER BY GameName;

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `GSP_GS_LoadGameMatchItem`
-- ----------------------------
DROP PROCEDURE IF EXISTS `GSP_GS_LoadGameMatchItem`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `GSP_GS_LoadGameMatchItem`(wKindID   INT,								-- 类型标识
	dwMatchID INT,								-- 比赛标识	
	dwMatchNo INT)
BEGIN
	
	-- 加载比赛
	SELECT a.MatchName,a.MatchType,a.MatchFeetype,a.MatchFee,a.MemberOrder,d.* FROM MatchPublic AS a,
	((SELECT b.MatchID,b.MatchNo,b.StartTime,b.EndTime,b.InitScore,b.CullScore,b.MinPlayCount,c.StartUserCount,c.AndroidUserCount,
	c.InitialBase,c.InitialScore,c.MinEnterGold,c.PlayCount,c.SwitchTableCount,c.PrecedeTimer FROM MatchLockTime b
	LEFT OUTER JOIN MatchImmediate c  ON  b.MatchID=c.MatchID ) 
	UNION ALL 	
	(SELECT c.MatchID,c.MatchNo,b.StartTime,b.EndTime,b.InitScore,b.CullScore,b.MinPlayCount,c.StartUserCount,c.AndroidUserCount,
	c.InitialBase,c.InitialScore,c.MinEnterGold,c.PlayCount,c.SwitchTableCount,c.PrecedeTimer FROM MatchLockTime b
	RIGHT OUTER JOIN MatchImmediate c  ON  b.MatchID=c.MatchID )) AS d 
	WHERE (((a.KindID=wKindID AND a.MatchID=dwMatchID AND a.MatchNo=dwMatchNo) OR (dwMatchID=0 AND a.KindID=wKindID)) AND (a.MatchID=d.MatchID AND a.MatchNo=d.MatchNo));

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `GSP_GS_LoadGameRoomItem`
-- ----------------------------
DROP PROCEDURE IF EXISTS `GSP_GS_LoadGameRoomItem`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `GSP_GS_LoadGameRoomItem`(wServerID INT,								-- 房间标识
	strMachineID NCHAR(32),					-- 服务机器
	OUT strErrorDescribe NVARCHAR(127))
BEGIN

	-- 加载房间
	SELECT GameRoomInfo.*, GameItem.GameName, GameItem.ServerVersion, GameItem.ClientVersion,
		GameItem.ServerDLLName FROM GameRoomInfo, GameItem
	WHERE GameRoomInfo.GameID=GameItem.GameID AND ((wServerID=ServerID) OR (wServerID=0))
		AND ((ServiceMachine=strMachineID) OR (strMachineID=''))
	ORDER BY GameName,ServerName;

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `GSP_GS_LoadMatchReward`
-- ----------------------------
DROP PROCEDURE IF EXISTS `GSP_GS_LoadMatchReward`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `GSP_GS_LoadMatchReward`(dwMatchID INT,								-- 比赛标识	
	dwMatchNo INT)
BEGIN

	-- 加载奖励
	SELECT * FROM MatchReward 
	WHERE MatchID=dwMatchID AND MatchNo=dwMatchNo;

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `GSP_GS_ModifyGameRoom`
-- ----------------------------
DROP PROCEDURE IF EXISTS `GSP_GS_ModifyGameRoom`;
DELIMITER ;;
CREATE DEFINER=`root`@`%` PROCEDURE `GSP_GS_ModifyGameRoom`(wServerID INT,								-- 房间标识

	-- 挂接属性
	wKindID INT,								-- 类型标识
	wNodeID INT,								-- 挂接标识
	wSortID INT,								-- 排序标识

	-- 税收配置
	lCellScore INT,								-- 单元积分
	wRevenueRatio INT,							-- 税收比例
	lServiceScore INT,							-- 服务费用

	-- 限制配置
	lRestrictScore	BIGINT,						-- 限制积分
	lMinTableScore	BIGINT,						-- 最低积分
	lMinEnterScore	BIGINT,						-- 最低积分
	lMaxEnterScore	BIGINT,						-- 最高积分

	-- 带入设置
	lDefPutScore BIGINT,						-- 默认带入积分
	lLowPutScore BIGINT,						-- 低于多少带入积分
	iExchangeRatio float,						-- 兑换比例
	
	-- 其他设置
	lMagicExpScore BIGINT,						-- 魔法表情
	lGratuityScore BIGINT,						-- 打赏积分
	dwWinExperience INT,
	dwFailExperience INT,
	
	-- 会员限制
	cbMinEnterMember INT,						-- 最低会员
	cbMaxEnterMember INT,						-- 最高会员

	-- 房间配置
	dwServerRule INT,							-- 房间规则
	dwAttachUserRight INT,						-- 附加权限

	-- 房间属性
	wMaxPlayer INT,							-- 游戏人数
	wTableCount INT,							-- 桌子数目
	wServerPort INT,							-- 服务端口
	wServerKind INT,							-- 房间类型
	wServerType INT,							-- 服务类型
	strServerName NVARCHAR(32),				-- 房间名字
	strServerPasswd NVARCHAR(32),				-- 房间密码

	-- 分组属性
	cbDistributeRule INT,						-- 分组规则
	wMinDistributeUser INT,					-- 最少人数	
	wDistributeTimeSpace INT,					-- 分组间隔
	wDistributeDrawCount INT,					-- 分组局数
	wMinPartakeGameUser INT,					-- 最少人数
	wMaxPartakeGameUser INT,					-- 最多人数	

	-- 连接信息
	strDataBaseName NVARCHAR(32),				-- 数据库名
	strDataBaseAddr NVARCHAR(15),				-- 连接地址

	strCacheServerAddr NVARCHAR(32),			-- 缓存服务器地址
	wCacheServerPort INT,						-- 缓存服务器端口
	
	-- 数据设置
	strCustomRule NVARCHAR(2048),				-- 定制规则
	strServiceMachine NVARCHAR(32),			-- 服务机器

	-- 输出信息
	OUT strErrorDescribe NVARCHAR(127),		-- 输出信息
	OUT nRet INT)
label_pro:BEGIN

	-- 查找房间
	DECLARE nServerID INT;

	-- 获取端口
	DECLARE nServerPort INT;
	
	SELECT ServerID into nServerID FROM GameRoomInfo WHERE ServerID=wServerID;

	-- 结果判断
	IF nServerID IS NULL THEN
		SET strErrorDescribe = '游戏房间不存在或者已经被删除了，房间修改失败！';
		SET nRet = 2;
		leave label_pro;
	END IF;

	-- 获取端口
	IF wServerPort<>0 THEN
		SELECT ServerPort into nServerPort FROM GameRoomInfo WHERE ServerPort=wServerPort AND ServiceMachine=strServiceMachine AND ServerID<>wServerID;

		-- 端口判断
		IF nServerPort=wServerPort THEN
			SET strErrorDescribe='存在服务端口相同的游戏房间，房间修改失败！';
			SET nRet = 1;
			leave label_pro;
		END IF;

	END IF;

	-- 更新房间
	UPDATE GameRoomInfo SET ServerName=strServerName, KindID=wKindID, NodeID=wNodeID, SortID=wSortID, TableCount=wTableCount,
		ServerPort=wServerPort, ServerKind=wServerKind, ServerType=wServerType, CellScore=lCellScore, RevenueRatio=wRevenueRatio, 
		ServiceScore=lServiceScore,RestrictScore=lRestrictScore, MinTableScore=lMinTableScore, MinEnterScore=lMinEnterScore, 
		MaxEnterScore=lMaxEnterScore,MinEnterMember=cbMinEnterMember, MaxEnterMember=cbMaxEnterMember, MaxPlayer=wMaxPlayer, 
		ServerRule=dwServerRule,DistributeRule=cbDistributeRule, MinDistributeUser=wMinDistributeUser,DistributeTimeSpace=wDistributeTimeSpace, 
		DistributeDrawCount=wDistributeDrawCount, MinPartakeGameUser=wMinPartakeGameUser,MaxPartakeGameUser=wMaxPartakeGameUser,
		AttachUserRight=dwAttachUserRight, ServerPasswd=strServerPasswd, DataBaseName=strDataBaseName, DataBaseAddr=strDataBaseAddr, 
		CacheServerAddr=strCacheServerAddr, CacheServerPort=wCacheServerPort,ServiceMachine=strServiceMachine,CustomRule=strCustomRule,
		DefPutScore = lDefPutScore, LowPutScore = lLowPutScore, ExchangeRatio = iExchangeRatio, MagicExpScore = lMagicExpScore, 
		GratuityScore = lGratuityScore, WinExperience = dwWinExperience, FailExperience = dwFailExperience
	WHERE ServerID=nServerID;

	-- 加载房间
	SELECT GameRoomInfo.*, GameItem.GameName, GameItem.ServerVersion, GameItem.ClientVersion, GameItem.ServerDLLName FROM GameRoomInfo, GameItem
	WHERE GameRoomInfo.GameID=GameItem.GameID AND ServerID=wServerID;

END
;;
DELIMITER ;
