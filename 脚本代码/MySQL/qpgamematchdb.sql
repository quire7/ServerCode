/*
Navicat MySQL Data Transfer

Source Server         : 192.168.1.124
Source Server Version : 50622
Source Host           : 192.168.1.124:3306
Source Database       : qpgamematchdb

Target Server Type    : MYSQL
Target Server Version : 50622
File Encoding         : 65001

Date: 2016-03-18 15:33:43
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `gameproperty`
-- ----------------------------
DROP TABLE IF EXISTS `gameproperty`;
CREATE TABLE `gameproperty` (
  `ID` int(11) NOT NULL AUTO_INCREMENT COMMENT '道具标识',
  `Name` varchar(31) DEFAULT NULL COMMENT '道具名字',
  `Cash` double DEFAULT NULL COMMENT '道具价格',
  `Gold` bigint(20) NOT NULL DEFAULT '0' COMMENT '道具金币',
  `Discount` smallint(6) DEFAULT NULL COMMENT '会员折扣',
  `IssueArea` smallint(6) DEFAULT NULL COMMENT '发行范围',
  `ServiceArea` smallint(6) DEFAULT NULL COMMENT '使用范围',
  `SendLoveLiness` bigint(20) DEFAULT '0' COMMENT '增加魅力',
  `RecvLoveLiness` bigint(20) DEFAULT '0' COMMENT '增加魅力',
  `RegulationsInfo` varchar(255) DEFAULT NULL COMMENT '使用说明',
  `Nullity` int(11) DEFAULT '0' COMMENT '禁止标志',
  PRIMARY KEY (`ID`,`Gold`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of gameproperty
-- ----------------------------

-- ----------------------------
-- Table structure for `gamescoreinfo`
-- ----------------------------
DROP TABLE IF EXISTS `gamescoreinfo`;
CREATE TABLE `gamescoreinfo` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `UserID` int(11) DEFAULT '0' COMMENT '用户 ID',
  `UserRight` int(11) DEFAULT '0' COMMENT '用户权限',
  `MasterRight` int(11) DEFAULT '0' COMMENT '管理权限',
  `MasterOrder` int(11) DEFAULT '0' COMMENT '管理等级',
  `AllLogonTimes` int(11) DEFAULT '0' COMMENT '总登陆次数',
  `PlayTimeCount` int(11) DEFAULT '0' COMMENT '游戏时间',
  `OnLineTimeCount` int(11) DEFAULT '0' COMMENT '在线时间',
  `LastLogonIP` varchar(15) DEFAULT NULL COMMENT '上次登陆 IP',
  `LastLogonDate` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '上次登陆时间',
  `LastLogonMachine` varchar(32) DEFAULT NULL COMMENT '登录机器',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=281 DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of gamescoreinfo
-- ----------------------------
INSERT INTO `gamescoreinfo` VALUES ('1', '328', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:34:51', '');
INSERT INTO `gamescoreinfo` VALUES ('2', '366', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:35:28', '');
INSERT INTO `gamescoreinfo` VALUES ('3', '198', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:45:15', '');
INSERT INTO `gamescoreinfo` VALUES ('4', '406', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:47:34', '');
INSERT INTO `gamescoreinfo` VALUES ('5', '436', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:49:52', '');
INSERT INTO `gamescoreinfo` VALUES ('6', '466', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:52:22', '');
INSERT INTO `gamescoreinfo` VALUES ('7', '496', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:55:40', '');
INSERT INTO `gamescoreinfo` VALUES ('8', '510', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:55:52', '');
INSERT INTO `gamescoreinfo` VALUES ('9', '482', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:55:57', '');
INSERT INTO `gamescoreinfo` VALUES ('10', '509', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:55:59', '');
INSERT INTO `gamescoreinfo` VALUES ('11', '483', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:55:59', '');
INSERT INTO `gamescoreinfo` VALUES ('12', '484', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:56:03', '');
INSERT INTO `gamescoreinfo` VALUES ('13', '507', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:56:04', '');
INSERT INTO `gamescoreinfo` VALUES ('14', '504', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:56:08', '');
INSERT INTO `gamescoreinfo` VALUES ('15', '485', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:56:08', '');
INSERT INTO `gamescoreinfo` VALUES ('16', '486', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:56:10', '');
INSERT INTO `gamescoreinfo` VALUES ('17', '503', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:56:13', '');
INSERT INTO `gamescoreinfo` VALUES ('18', '487', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:56:15', '');
INSERT INTO `gamescoreinfo` VALUES ('19', '500', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:56:16', '');
INSERT INTO `gamescoreinfo` VALUES ('20', '488', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:56:19', '');
INSERT INTO `gamescoreinfo` VALUES ('21', '498', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:56:20', '');
INSERT INTO `gamescoreinfo` VALUES ('22', '511', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:56:23', '');
INSERT INTO `gamescoreinfo` VALUES ('23', '495', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:56:23', '');
INSERT INTO `gamescoreinfo` VALUES ('24', '508', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:56:25', '');
INSERT INTO `gamescoreinfo` VALUES ('25', '494', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:56:27', '');
INSERT INTO `gamescoreinfo` VALUES ('26', '506', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:56:29', '');
INSERT INTO `gamescoreinfo` VALUES ('27', '492', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:56:29', '');
INSERT INTO `gamescoreinfo` VALUES ('28', '505', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:56:33', '');
INSERT INTO `gamescoreinfo` VALUES ('29', '490', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:56:33', '');
INSERT INTO `gamescoreinfo` VALUES ('30', '497', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:56:35', '');
INSERT INTO `gamescoreinfo` VALUES ('31', '493', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:56:37', '');
INSERT INTO `gamescoreinfo` VALUES ('32', '499', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:56:39', '');
INSERT INTO `gamescoreinfo` VALUES ('33', '491', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:56:40', '');
INSERT INTO `gamescoreinfo` VALUES ('34', '502', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:56:43', '');
INSERT INTO `gamescoreinfo` VALUES ('35', '489', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:56:43', '');
INSERT INTO `gamescoreinfo` VALUES ('36', '501', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:56:47', '');
INSERT INTO `gamescoreinfo` VALUES ('37', '526', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:59:05', '');
INSERT INTO `gamescoreinfo` VALUES ('38', '540', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:59:06', '');
INSERT INTO `gamescoreinfo` VALUES ('39', '512', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:59:09', '');
INSERT INTO `gamescoreinfo` VALUES ('40', '539', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:59:11', '');
INSERT INTO `gamescoreinfo` VALUES ('41', '513', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:59:13', '');
INSERT INTO `gamescoreinfo` VALUES ('42', '537', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:59:14', '');
INSERT INTO `gamescoreinfo` VALUES ('43', '514', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:59:17', '');
INSERT INTO `gamescoreinfo` VALUES ('44', '535', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:59:18', '');
INSERT INTO `gamescoreinfo` VALUES ('45', '515', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:59:21', '');
INSERT INTO `gamescoreinfo` VALUES ('46', '516', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:59:23', '');
INSERT INTO `gamescoreinfo` VALUES ('47', '533', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:59:23', '');
INSERT INTO `gamescoreinfo` VALUES ('48', '517', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:59:26', '');
INSERT INTO `gamescoreinfo` VALUES ('49', '530', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:59:27', '');
INSERT INTO `gamescoreinfo` VALUES ('50', '518', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:59:29', '');
INSERT INTO `gamescoreinfo` VALUES ('51', '528', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:59:31', '');
INSERT INTO `gamescoreinfo` VALUES ('52', '519', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:59:32', '');
INSERT INTO `gamescoreinfo` VALUES ('53', '541', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:59:37', '');
INSERT INTO `gamescoreinfo` VALUES ('54', '538', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:59:39', '');
INSERT INTO `gamescoreinfo` VALUES ('55', '524', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:59:39', '');
INSERT INTO `gamescoreinfo` VALUES ('56', '536', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:59:41', '');
INSERT INTO `gamescoreinfo` VALUES ('57', '522', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:59:43', '');
INSERT INTO `gamescoreinfo` VALUES ('58', '534', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:59:44', '');
INSERT INTO `gamescoreinfo` VALUES ('59', '532', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:59:47', '');
INSERT INTO `gamescoreinfo` VALUES ('60', '520', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:59:48', '');
INSERT INTO `gamescoreinfo` VALUES ('61', '523', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:59:51', '');
INSERT INTO `gamescoreinfo` VALUES ('62', '529', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:59:52', '');
INSERT INTO `gamescoreinfo` VALUES ('63', '521', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:59:53', '');
INSERT INTO `gamescoreinfo` VALUES ('64', '527', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:59:54', '');
INSERT INTO `gamescoreinfo` VALUES ('65', '531', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:59:56', '');
INSERT INTO `gamescoreinfo` VALUES ('66', '525', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 18:59:56', '');
INSERT INTO `gamescoreinfo` VALUES ('67', '586', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:03:24', '');
INSERT INTO `gamescoreinfo` VALUES ('68', '600', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:03:25', '');
INSERT INTO `gamescoreinfo` VALUES ('69', '572', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:03:27', '');
INSERT INTO `gamescoreinfo` VALUES ('70', '599', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:03:30', '');
INSERT INTO `gamescoreinfo` VALUES ('71', '573', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:03:32', '');
INSERT INTO `gamescoreinfo` VALUES ('72', '574', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:03:34', '');
INSERT INTO `gamescoreinfo` VALUES ('73', '597', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:03:34', '');
INSERT INTO `gamescoreinfo` VALUES ('74', '594', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:03:37', '');
INSERT INTO `gamescoreinfo` VALUES ('75', '575', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:03:39', '');
INSERT INTO `gamescoreinfo` VALUES ('76', '616', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:04:10', '');
INSERT INTO `gamescoreinfo` VALUES ('77', '630', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:04:11', '');
INSERT INTO `gamescoreinfo` VALUES ('78', '602', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:04:13', '');
INSERT INTO `gamescoreinfo` VALUES ('79', '629', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:04:14', '');
INSERT INTO `gamescoreinfo` VALUES ('80', '646', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:15:56', '');
INSERT INTO `gamescoreinfo` VALUES ('81', '660', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:15:57', '');
INSERT INTO `gamescoreinfo` VALUES ('82', '632', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:16:00', '');
INSERT INTO `gamescoreinfo` VALUES ('83', '659', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:16:01', '');
INSERT INTO `gamescoreinfo` VALUES ('84', '633', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:16:04', '');
INSERT INTO `gamescoreinfo` VALUES ('85', '657', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:16:06', '');
INSERT INTO `gamescoreinfo` VALUES ('86', '634', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:16:08', '');
INSERT INTO `gamescoreinfo` VALUES ('87', '655', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:16:09', '');
INSERT INTO `gamescoreinfo` VALUES ('88', '635', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:16:11', '');
INSERT INTO `gamescoreinfo` VALUES ('89', '653', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:16:14', '');
INSERT INTO `gamescoreinfo` VALUES ('90', '636', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:16:16', '');
INSERT INTO `gamescoreinfo` VALUES ('91', '651', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:16:19', '');
INSERT INTO `gamescoreinfo` VALUES ('92', '637', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:16:21', '');
INSERT INTO `gamescoreinfo` VALUES ('93', '649', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:16:22', '');
INSERT INTO `gamescoreinfo` VALUES ('94', '638', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:16:23', '');
INSERT INTO `gamescoreinfo` VALUES ('95', '647', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:16:25', '');
INSERT INTO `gamescoreinfo` VALUES ('96', '645', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:16:26', '');
INSERT INTO `gamescoreinfo` VALUES ('97', '658', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:16:29', '');
INSERT INTO `gamescoreinfo` VALUES ('98', '644', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:16:29', '');
INSERT INTO `gamescoreinfo` VALUES ('99', '656', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:16:31', '');
INSERT INTO `gamescoreinfo` VALUES ('100', '642', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:16:32', '');
INSERT INTO `gamescoreinfo` VALUES ('101', '654', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:16:33', '');
INSERT INTO `gamescoreinfo` VALUES ('102', '652', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:16:37', '');
INSERT INTO `gamescoreinfo` VALUES ('103', '640', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:16:37', '');
INSERT INTO `gamescoreinfo` VALUES ('104', '643', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:16:40', '');
INSERT INTO `gamescoreinfo` VALUES ('105', '648', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:16:41', '');
INSERT INTO `gamescoreinfo` VALUES ('106', '661', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:16:44', '');
INSERT INTO `gamescoreinfo` VALUES ('107', '639', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:16:45', '');
INSERT INTO `gamescoreinfo` VALUES ('108', '650', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:16:47', '');
INSERT INTO `gamescoreinfo` VALUES ('109', '641', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:16:49', '');
INSERT INTO `gamescoreinfo` VALUES ('110', '681', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:23:43', '');
INSERT INTO `gamescoreinfo` VALUES ('111', '695', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:23:44', '');
INSERT INTO `gamescoreinfo` VALUES ('112', '667', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:23:47', '');
INSERT INTO `gamescoreinfo` VALUES ('113', '694', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:23:48', '');
INSERT INTO `gamescoreinfo` VALUES ('114', '668', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:23:52', '');
INSERT INTO `gamescoreinfo` VALUES ('115', '692', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:23:53', '');
INSERT INTO `gamescoreinfo` VALUES ('116', '690', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:23:57', '');
INSERT INTO `gamescoreinfo` VALUES ('117', '669', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:23:57', '');
INSERT INTO `gamescoreinfo` VALUES ('118', '689', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:23:59', '');
INSERT INTO `gamescoreinfo` VALUES ('119', '670', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:23:59', '');
INSERT INTO `gamescoreinfo` VALUES ('120', '687', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:24:01', '');
INSERT INTO `gamescoreinfo` VALUES ('121', '671', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:24:04', '');
INSERT INTO `gamescoreinfo` VALUES ('122', '685', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:24:05', '');
INSERT INTO `gamescoreinfo` VALUES ('123', '683', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:24:08', '');
INSERT INTO `gamescoreinfo` VALUES ('124', '672', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:24:09', '');
INSERT INTO `gamescoreinfo` VALUES ('125', '682', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:24:12', '');
INSERT INTO `gamescoreinfo` VALUES ('126', '680', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:24:12', '');
INSERT INTO `gamescoreinfo` VALUES ('127', '693', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:24:14', '');
INSERT INTO `gamescoreinfo` VALUES ('128', '679', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:24:14', '');
INSERT INTO `gamescoreinfo` VALUES ('129', '691', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:24:17', '');
INSERT INTO `gamescoreinfo` VALUES ('130', '677', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:24:18', '');
INSERT INTO `gamescoreinfo` VALUES ('131', '688', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:24:22', '');
INSERT INTO `gamescoreinfo` VALUES ('132', '675', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:24:23', '');
INSERT INTO `gamescoreinfo` VALUES ('133', '686', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:24:25', '');
INSERT INTO `gamescoreinfo` VALUES ('134', '673', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:24:25', '');
INSERT INTO `gamescoreinfo` VALUES ('135', '684', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:24:27', '');
INSERT INTO `gamescoreinfo` VALUES ('136', '678', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:24:27', '');
INSERT INTO `gamescoreinfo` VALUES ('137', '674', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:24:29', '');
INSERT INTO `gamescoreinfo` VALUES ('138', '696', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:24:30', '');
INSERT INTO `gamescoreinfo` VALUES ('139', '676', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:24:31', '');
INSERT INTO `gamescoreinfo` VALUES ('140', '711', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:25:44', '');
INSERT INTO `gamescoreinfo` VALUES ('141', '725', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:25:44', '');
INSERT INTO `gamescoreinfo` VALUES ('142', '724', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:25:47', '');
INSERT INTO `gamescoreinfo` VALUES ('143', '697', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:25:48', '');
INSERT INTO `gamescoreinfo` VALUES ('144', '723', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:25:52', '');
INSERT INTO `gamescoreinfo` VALUES ('145', '698', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:25:53', '');
INSERT INTO `gamescoreinfo` VALUES ('146', '721', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-16 19:25:54', '');
INSERT INTO `gamescoreinfo` VALUES ('147', '142', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:39:00', '');
INSERT INTO `gamescoreinfo` VALUES ('148', '156', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:39:01', '');
INSERT INTO `gamescoreinfo` VALUES ('149', '155', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:39:04', '');
INSERT INTO `gamescoreinfo` VALUES ('150', '128', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:39:06', '');
INSERT INTO `gamescoreinfo` VALUES ('151', '154', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:39:08', '');
INSERT INTO `gamescoreinfo` VALUES ('152', '129', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:39:10', '');
INSERT INTO `gamescoreinfo` VALUES ('153', '152', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:39:13', '');
INSERT INTO `gamescoreinfo` VALUES ('154', '130', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:39:13', '');
INSERT INTO `gamescoreinfo` VALUES ('155', '147', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:39:29', '');
INSERT INTO `gamescoreinfo` VALUES ('156', '144', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:39:32', '');
INSERT INTO `gamescoreinfo` VALUES ('157', '134', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:39:33', '');
INSERT INTO `gamescoreinfo` VALUES ('158', '135', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:39:37', '');
INSERT INTO `gamescoreinfo` VALUES ('159', '143', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:39:38', '');
INSERT INTO `gamescoreinfo` VALUES ('160', '140', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:39:39', '');
INSERT INTO `gamescoreinfo` VALUES ('161', '139', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:39:42', '');
INSERT INTO `gamescoreinfo` VALUES ('162', '153', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:39:44', '');
INSERT INTO `gamescoreinfo` VALUES ('163', '138', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:39:46', '');
INSERT INTO `gamescoreinfo` VALUES ('164', '136', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:39:48', '');
INSERT INTO `gamescoreinfo` VALUES ('165', '151', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:39:50', '');
INSERT INTO `gamescoreinfo` VALUES ('166', '141', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:39:53', '');
INSERT INTO `gamescoreinfo` VALUES ('167', '145', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:39:54', '');
INSERT INTO `gamescoreinfo` VALUES ('168', '137', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:39:56', '');
INSERT INTO `gamescoreinfo` VALUES ('169', '146', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:40:01', '');
INSERT INTO `gamescoreinfo` VALUES ('170', '157', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:40:13', '');
INSERT INTO `gamescoreinfo` VALUES ('171', '148', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:40:17', '');
INSERT INTO `gamescoreinfo` VALUES ('172', '172', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:57:43', '');
INSERT INTO `gamescoreinfo` VALUES ('173', '186', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:57:44', '');
INSERT INTO `gamescoreinfo` VALUES ('174', '158', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:57:47', '');
INSERT INTO `gamescoreinfo` VALUES ('175', '185', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:57:47', '');
INSERT INTO `gamescoreinfo` VALUES ('176', '183', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:57:49', '');
INSERT INTO `gamescoreinfo` VALUES ('177', '159', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:57:52', '');
INSERT INTO `gamescoreinfo` VALUES ('178', '182', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:57:52', '');
INSERT INTO `gamescoreinfo` VALUES ('179', '160', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:58:47', '');
INSERT INTO `gamescoreinfo` VALUES ('180', '180', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:58:47', '');
INSERT INTO `gamescoreinfo` VALUES ('181', '202', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:59:08', '');
INSERT INTO `gamescoreinfo` VALUES ('182', '216', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:59:09', '');
INSERT INTO `gamescoreinfo` VALUES ('183', '188', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:59:10', '');
INSERT INTO `gamescoreinfo` VALUES ('184', '215', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:59:12', '');
INSERT INTO `gamescoreinfo` VALUES ('185', '189', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:59:12', '');
INSERT INTO `gamescoreinfo` VALUES ('186', '213', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:59:14', '');
INSERT INTO `gamescoreinfo` VALUES ('187', '190', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 09:59:15', '');
INSERT INTO `gamescoreinfo` VALUES ('188', '232', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 10:04:14', '');
INSERT INTO `gamescoreinfo` VALUES ('189', '246', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 10:04:16', '');
INSERT INTO `gamescoreinfo` VALUES ('190', '218', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 10:04:16', '');
INSERT INTO `gamescoreinfo` VALUES ('191', '219', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 10:04:19', '');
INSERT INTO `gamescoreinfo` VALUES ('192', '245', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 10:05:14', '');
INSERT INTO `gamescoreinfo` VALUES ('193', '242', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 10:05:16', '');
INSERT INTO `gamescoreinfo` VALUES ('194', '220', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 10:05:17', '');
INSERT INTO `gamescoreinfo` VALUES ('195', '241', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 10:05:19', '');
INSERT INTO `gamescoreinfo` VALUES ('196', '239', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 10:05:21', '');
INSERT INTO `gamescoreinfo` VALUES ('197', '221', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 10:06:09', '');
INSERT INTO `gamescoreinfo` VALUES ('198', '222', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 10:06:11', '');
INSERT INTO `gamescoreinfo` VALUES ('199', '238', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 10:06:11', '');
INSERT INTO `gamescoreinfo` VALUES ('200', '235', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 10:06:13', '');
INSERT INTO `gamescoreinfo` VALUES ('201', '223', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 10:06:15', '');
INSERT INTO `gamescoreinfo` VALUES ('202', '234', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 10:06:18', '');
INSERT INTO `gamescoreinfo` VALUES ('203', '322', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 14:50:17', '');
INSERT INTO `gamescoreinfo` VALUES ('204', '336', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 14:50:18', '');
INSERT INTO `gamescoreinfo` VALUES ('205', '335', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 14:50:20', '');
INSERT INTO `gamescoreinfo` VALUES ('206', '308', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 14:50:22', '');
INSERT INTO `gamescoreinfo` VALUES ('207', '334', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 14:50:23', '');
INSERT INTO `gamescoreinfo` VALUES ('208', '332', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 14:50:25', '');
INSERT INTO `gamescoreinfo` VALUES ('209', '309', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 14:50:28', '');
INSERT INTO `gamescoreinfo` VALUES ('210', '310', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 14:50:32', '');
INSERT INTO `gamescoreinfo` VALUES ('211', '331', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 14:50:32', '');
INSERT INTO `gamescoreinfo` VALUES ('212', '311', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 14:50:34', '');
INSERT INTO `gamescoreinfo` VALUES ('213', '312', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 14:50:39', '');
INSERT INTO `gamescoreinfo` VALUES ('214', '326', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 14:50:39', '');
INSERT INTO `gamescoreinfo` VALUES ('215', '313', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 14:50:45', '');
INSERT INTO `gamescoreinfo` VALUES ('216', '324', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 14:50:45', '');
INSERT INTO `gamescoreinfo` VALUES ('217', '314', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 14:50:50', '');
INSERT INTO `gamescoreinfo` VALUES ('218', '337', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 14:50:50', '');
INSERT INTO `gamescoreinfo` VALUES ('219', '320', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 14:50:53', '');
INSERT INTO `gamescoreinfo` VALUES ('220', '333', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 14:50:54', '');
INSERT INTO `gamescoreinfo` VALUES ('221', '330', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 14:50:57', '');
INSERT INTO `gamescoreinfo` VALUES ('222', '319', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 14:50:58', '');
INSERT INTO `gamescoreinfo` VALUES ('223', '329', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 14:50:59', '');
INSERT INTO `gamescoreinfo` VALUES ('224', '316', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 14:51:00', '');
INSERT INTO `gamescoreinfo` VALUES ('225', '321', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 14:51:04', '');
INSERT INTO `gamescoreinfo` VALUES ('226', '323', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 14:51:04', '');
INSERT INTO `gamescoreinfo` VALUES ('227', '325', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 14:51:08', '');
INSERT INTO `gamescoreinfo` VALUES ('228', '318', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 14:51:09', '');
INSERT INTO `gamescoreinfo` VALUES ('229', '315', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 14:51:11', '');
INSERT INTO `gamescoreinfo` VALUES ('230', '327', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 14:51:12', '');
INSERT INTO `gamescoreinfo` VALUES ('231', '317', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-17 14:51:16', '');
INSERT INTO `gamescoreinfo` VALUES ('232', '166', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 11:24:38', '');
INSERT INTO `gamescoreinfo` VALUES ('233', '165', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 11:24:44', '');
INSERT INTO `gamescoreinfo` VALUES ('234', '163', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 11:24:48', '');
INSERT INTO `gamescoreinfo` VALUES ('235', '161', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 11:24:53', '');
INSERT INTO `gamescoreinfo` VALUES ('236', '167', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 11:25:16', '');
INSERT INTO `gamescoreinfo` VALUES ('237', '164', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 11:25:19', '');
INSERT INTO `gamescoreinfo` VALUES ('238', '150', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 11:25:19', '');
INSERT INTO `gamescoreinfo` VALUES ('239', '162', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 11:25:21', '');
INSERT INTO `gamescoreinfo` VALUES ('240', '149', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 11:25:29', '');
INSERT INTO `gamescoreinfo` VALUES ('241', '196', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 13:47:48', '');
INSERT INTO `gamescoreinfo` VALUES ('242', '195', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 13:47:50', '');
INSERT INTO `gamescoreinfo` VALUES ('243', '168', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 13:47:52', '');
INSERT INTO `gamescoreinfo` VALUES ('244', '194', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 13:47:55', '');
INSERT INTO `gamescoreinfo` VALUES ('245', '169', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 13:47:56', '');
INSERT INTO `gamescoreinfo` VALUES ('246', '192', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 13:47:57', '');
INSERT INTO `gamescoreinfo` VALUES ('247', '170', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 13:47:58', '');
INSERT INTO `gamescoreinfo` VALUES ('248', '171', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 13:48:02', '');
INSERT INTO `gamescoreinfo` VALUES ('249', '173', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 13:48:08', '');
INSERT INTO `gamescoreinfo` VALUES ('250', '184', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 13:48:10', '');
INSERT INTO `gamescoreinfo` VALUES ('251', '174', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 13:48:11', '');
INSERT INTO `gamescoreinfo` VALUES ('252', '197', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 13:48:13', '');
INSERT INTO `gamescoreinfo` VALUES ('253', '193', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 13:48:17', '');
INSERT INTO `gamescoreinfo` VALUES ('254', '179', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 13:48:19', '');
INSERT INTO `gamescoreinfo` VALUES ('255', '191', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 13:48:21', '');
INSERT INTO `gamescoreinfo` VALUES ('256', '177', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 13:48:21', '');
INSERT INTO `gamescoreinfo` VALUES ('257', '175', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 13:48:26', '');
INSERT INTO `gamescoreinfo` VALUES ('258', '178', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 13:48:31', '');
INSERT INTO `gamescoreinfo` VALUES ('259', '187', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 13:48:34', '');
INSERT INTO `gamescoreinfo` VALUES ('260', '176', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 13:48:36', '');
INSERT INTO `gamescoreinfo` VALUES ('261', '181', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 13:48:41', '');
INSERT INTO `gamescoreinfo` VALUES ('262', '212', '0', '0', '0', '0', '124', '0', '0.0.0.0', '2016-03-18 14:04:58', '');
INSERT INTO `gamescoreinfo` VALUES ('263', '226', '0', '0', '0', '0', '124', '0', '0.0.0.0', '2016-03-18 14:04:58', '');
INSERT INTO `gamescoreinfo` VALUES ('264', '225', '0', '0', '0', '0', '124', '0', '0.0.0.0', '2016-03-18 14:04:58', '');
INSERT INTO `gamescoreinfo` VALUES ('265', '224', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 13:59:34', '');
INSERT INTO `gamescoreinfo` VALUES ('266', '199', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 13:59:35', '');
INSERT INTO `gamescoreinfo` VALUES ('267', '200', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 13:59:37', '');
INSERT INTO `gamescoreinfo` VALUES ('268', '201', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 13:59:40', '');
INSERT INTO `gamescoreinfo` VALUES ('269', '203', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 13:59:45', '');
INSERT INTO `gamescoreinfo` VALUES ('270', '217', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 13:59:47', '');
INSERT INTO `gamescoreinfo` VALUES ('271', '204', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 13:59:47', '');
INSERT INTO `gamescoreinfo` VALUES ('272', '205', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 13:59:50', '');
INSERT INTO `gamescoreinfo` VALUES ('273', '214', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 13:59:51', '');
INSERT INTO `gamescoreinfo` VALUES ('274', '211', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 13:59:53', '');
INSERT INTO `gamescoreinfo` VALUES ('275', '210', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 13:59:55', '');
INSERT INTO `gamescoreinfo` VALUES ('276', '209', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 13:59:58', '');
INSERT INTO `gamescoreinfo` VALUES ('277', '207', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 14:00:00', '');
INSERT INTO `gamescoreinfo` VALUES ('278', '227', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 14:00:03', '');
INSERT INTO `gamescoreinfo` VALUES ('279', '208', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 14:00:05', '');
INSERT INTO `gamescoreinfo` VALUES ('280', '206', '0', '0', '0', '0', '0', '0', '0.0.0.0', '2016-03-18 14:00:43', '');

-- ----------------------------
-- Table structure for `gamescorelocker`
-- ----------------------------
DROP TABLE IF EXISTS `gamescorelocker`;
CREATE TABLE `gamescorelocker` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `UserID` int(11) DEFAULT '0' COMMENT '用户索引',
  `KindID` int(11) DEFAULT '0' COMMENT '游戏标识',
  `ServerID` int(11) DEFAULT '0' COMMENT '房间索引',
  `EnterID` int(11) DEFAULT '0' COMMENT '进出索引',
  `EnterIP` varchar(15) DEFAULT NULL COMMENT '进入地址',
  `EnterMachine` varchar(32) DEFAULT NULL COMMENT '进入机器',
  `CollectDate` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '录入日期',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of gamescorelocker
-- ----------------------------

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
-- Table structure for `matchscoreinfo`
-- ----------------------------
DROP TABLE IF EXISTS `matchscoreinfo`;
CREATE TABLE `matchscoreinfo` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `UserID` int(11) DEFAULT '0' COMMENT '用户标识',
  `ServerID` int(11) DEFAULT '0' COMMENT '房间标识',
  `MatchID` int(11) DEFAULT '0' COMMENT '比赛标识',
  `MatchNo` int(11) DEFAULT '0' COMMENT '场次编号',
  `Score` bigint(20) DEFAULT NULL COMMENT '比赛分数',
  `WinCount` int(11) DEFAULT '0' COMMENT '胜利局数',
  `LostCount` int(11) DEFAULT '0' COMMENT '失败局数',
  `DrawCount` int(11) DEFAULT '0' COMMENT '打平局数',
  `FleeCount` int(11) DEFAULT '0' COMMENT '逃跑局数',
  `UserRight` int(11) DEFAULT '0' COMMENT '用户权限',
  `PlayTimeCount` int(11) DEFAULT '0' COMMENT '游戏时长',
  `OnlineTime` int(11) DEFAULT '0' COMMENT '在线时长',
  `SignupTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '报名时间',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of matchscoreinfo
-- ----------------------------

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
-- Table structure for `recorddrawinfo`
-- ----------------------------
DROP TABLE IF EXISTS `recorddrawinfo`;
CREATE TABLE `recorddrawinfo` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `DrawID` int(11) DEFAULT NULL COMMENT '局数标识',
  `KindID` int(11) DEFAULT '0' COMMENT '类型标识',
  `ServerID` int(11) DEFAULT '0' COMMENT '房间标识',
  `TableID` int(11) DEFAULT '0' COMMENT '桌子号码',
  `UserCount` int(11) DEFAULT '0' COMMENT '用户数目',
  `AndroidCount` int(11) DEFAULT '0' COMMENT '机器数目',
  `Waste` bigint(20) DEFAULT '0' COMMENT '损耗数目',
  `Revenue` bigint(20) DEFAULT '0' COMMENT '税收数目',
  `UserMedal` int(11) DEFAULT '0' COMMENT '用户奖牌',
  `StartTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '开始时间',
  `ConcludeTime` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '结束时间',
  `InsertTime` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '插入时间',
  `DrawCourse` longblob,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of recorddrawinfo
-- ----------------------------
INSERT INTO `recorddrawinfo` VALUES ('1', null, '7', '3', '0', '1', '0', '-600', '0', '0', '2016-03-18 14:02:45', '2016-03-18 14:04:49', '0000-00-00 00:00:00', null);

-- ----------------------------
-- Table structure for `recorddrawscore`
-- ----------------------------
DROP TABLE IF EXISTS `recorddrawscore`;
CREATE TABLE `recorddrawscore` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `DrawID` int(11) DEFAULT '0' COMMENT '局数标识',
  `UserID` int(11) DEFAULT '0' COMMENT '用户标识',
  `ChairID` int(11) DEFAULT '0' COMMENT '椅子号码',
  `Score` bigint(20) DEFAULT NULL COMMENT '用户成绩',
  `Grade` bigint(20) DEFAULT NULL COMMENT '用户积分',
  `Revenue` bigint(20) DEFAULT NULL COMMENT '税收数目',
  `UserMedal` int(11) DEFAULT '0' COMMENT '用户奖牌',
  `PlayTimeCount` int(11) DEFAULT '0' COMMENT '游戏时长',
  `DBQuestID` int(11) DEFAULT '0' COMMENT '请求标识',
  `InoutIndex` int(11) DEFAULT '0' COMMENT '进出索引',
  `InsertTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '插入时间',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of recorddrawscore
-- ----------------------------
INSERT INTO `recorddrawscore` VALUES ('1', '1', '1505', '1', '600', '0', '0', '0', '124', '1', '0', '2016-03-18 14:04:58');

-- ----------------------------
-- Table structure for `recorduserinout`
-- ----------------------------
DROP TABLE IF EXISTS `recorduserinout`;
CREATE TABLE `recorduserinout` (
  `ID` int(11) NOT NULL AUTO_INCREMENT COMMENT '索引标识',
  `UserID` int(11) DEFAULT '0' COMMENT '用户标识',
  `KindID` int(11) DEFAULT '0' COMMENT '类型标识',
  `ServerID` int(11) DEFAULT '0' COMMENT '房间标识',
  `EnterTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '进入时间',
  `EnterScore` bigint(20) DEFAULT NULL COMMENT '进入积分',
  `EnterInsure` bigint(20) DEFAULT NULL COMMENT '进入银行',
  `EnterUserMedal` int(11) DEFAULT '0' COMMENT '进入奖牌',
  `EnterLoveliness` int(11) DEFAULT '0' COMMENT '进入魅力',
  `EnterMachine` varchar(33) DEFAULT NULL COMMENT '进入机器',
  `EnterClientIP` varchar(15) DEFAULT NULL COMMENT '登录地址',
  `LeaveTime` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '离开时间',
  `LeaveReason` int(11) DEFAULT '0' COMMENT '离开原因',
  `LeaveMachine` varchar(32) DEFAULT NULL COMMENT '离开机器',
  `LeaveClientIP` varchar(15) DEFAULT NULL COMMENT '离开地址',
  `Score` bigint(20) DEFAULT NULL COMMENT '变更积分',
  `Insure` bigint(20) DEFAULT NULL COMMENT '银行变更',
  `Revenue` bigint(20) DEFAULT NULL COMMENT '变更税收',
  `WinCount` int(11) DEFAULT '0' COMMENT '胜局变更',
  `LostCount` int(11) DEFAULT '0' COMMENT '输局变更',
  `DrawCount` int(11) DEFAULT '0' COMMENT '和局变更',
  `FleeCount` int(11) DEFAULT '0' COMMENT '逃局变更',
  `UserMedal` int(11) DEFAULT '0' COMMENT '奖牌数目',
  `LoveLiness` int(11) DEFAULT '0' COMMENT '魅力变更',
  `Experience` int(11) DEFAULT '0' COMMENT '经验变更',
  `PlayTimeCount` int(11) DEFAULT '0' COMMENT '游戏时间',
  `OnLineTimeCount` int(11) DEFAULT '0' COMMENT '在线时间',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=323 DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of recorduserinout
-- ----------------------------
INSERT INTO `recorduserinout` VALUES ('1', '328', '7', '3', '2016-03-16 18:34:51', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('2', '366', '7', '3', '2016-03-16 18:35:28', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('3', '198', '7', '3', '2016-03-16 18:45:15', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('4', '406', '7', '3', '2016-03-16 18:47:34', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('5', '436', '7', '3', '2016-03-16 18:49:52', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('6', '466', '7', '3', '2016-03-16 18:52:22', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('7', '496', '7', '3', '2016-03-16 18:55:40', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('8', '510', '7', '3', '2016-03-16 18:55:52', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('9', '482', '7', '3', '2016-03-16 18:55:57', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('10', '509', '7', '3', '2016-03-16 18:55:59', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('11', '483', '7', '3', '2016-03-16 18:55:59', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('12', '484', '7', '3', '2016-03-16 18:56:03', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('13', '507', '7', '3', '2016-03-16 18:56:04', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('14', '504', '7', '3', '2016-03-16 18:56:08', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('15', '485', '7', '3', '2016-03-16 18:56:08', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('16', '486', '7', '3', '2016-03-16 18:56:10', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('17', '503', '7', '3', '2016-03-16 18:56:13', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('18', '487', '7', '3', '2016-03-16 18:56:15', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('19', '500', '7', '3', '2016-03-16 18:56:16', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('20', '488', '7', '3', '2016-03-16 18:56:19', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('21', '498', '7', '3', '2016-03-16 18:56:20', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('22', '511', '7', '3', '2016-03-16 18:56:23', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('23', '495', '7', '3', '2016-03-16 18:56:23', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('24', '508', '7', '3', '2016-03-16 18:56:25', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('25', '494', '7', '3', '2016-03-16 18:56:27', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('26', '506', '7', '3', '2016-03-16 18:56:29', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('27', '492', '7', '3', '2016-03-16 18:56:29', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('28', '505', '7', '3', '2016-03-16 18:56:33', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('29', '490', '7', '3', '2016-03-16 18:56:33', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('30', '497', '7', '3', '2016-03-16 18:56:35', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('31', '493', '7', '3', '2016-03-16 18:56:37', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('32', '499', '7', '3', '2016-03-16 18:56:39', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('33', '491', '7', '3', '2016-03-16 18:56:40', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('34', '502', '7', '3', '2016-03-16 18:56:43', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('35', '489', '7', '3', '2016-03-16 18:56:43', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('36', '501', '7', '3', '2016-03-16 18:56:47', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('37', '526', '7', '3', '2016-03-16 18:59:05', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('38', '540', '7', '3', '2016-03-16 18:59:06', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('39', '512', '7', '3', '2016-03-16 18:59:09', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('40', '539', '7', '3', '2016-03-16 18:59:11', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('41', '513', '7', '3', '2016-03-16 18:59:13', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('42', '537', '7', '3', '2016-03-16 18:59:14', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('43', '514', '7', '3', '2016-03-16 18:59:17', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('44', '535', '7', '3', '2016-03-16 18:59:18', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('45', '515', '7', '3', '2016-03-16 18:59:21', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('46', '516', '7', '3', '2016-03-16 18:59:23', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('47', '533', '7', '3', '2016-03-16 18:59:24', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('48', '517', '7', '3', '2016-03-16 18:59:26', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('49', '530', '7', '3', '2016-03-16 18:59:27', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('50', '518', '7', '3', '2016-03-16 18:59:29', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('51', '528', '7', '3', '2016-03-16 18:59:31', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('52', '519', '7', '3', '2016-03-16 18:59:32', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('53', '541', '7', '3', '2016-03-16 18:59:37', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('54', '538', '7', '3', '2016-03-16 18:59:39', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('55', '524', '7', '3', '2016-03-16 18:59:39', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('56', '536', '7', '3', '2016-03-16 18:59:41', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('57', '522', '7', '3', '2016-03-16 18:59:43', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('58', '534', '7', '3', '2016-03-16 18:59:44', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('59', '532', '7', '3', '2016-03-16 18:59:47', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('60', '520', '7', '3', '2016-03-16 18:59:48', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('61', '523', '7', '3', '2016-03-16 18:59:51', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('62', '529', '7', '3', '2016-03-16 18:59:52', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('63', '521', '7', '3', '2016-03-16 18:59:53', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('64', '527', '7', '3', '2016-03-16 18:59:54', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('65', '531', '7', '3', '2016-03-16 18:59:56', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('66', '525', '7', '3', '2016-03-16 18:59:56', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('67', '586', '7', '3', '2016-03-16 19:03:24', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('68', '600', '7', '3', '2016-03-16 19:03:25', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('69', '572', '7', '3', '2016-03-16 19:03:27', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('70', '599', '7', '3', '2016-03-16 19:03:30', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('71', '573', '7', '3', '2016-03-16 19:03:32', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('72', '574', '7', '3', '2016-03-16 19:03:34', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('73', '597', '7', '3', '2016-03-16 19:03:34', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('74', '594', '7', '3', '2016-03-16 19:03:37', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('75', '575', '7', '3', '2016-03-16 19:03:39', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('76', '616', '7', '3', '2016-03-16 19:04:10', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('77', '630', '7', '3', '2016-03-16 19:04:11', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('78', '602', '7', '3', '2016-03-16 19:04:13', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('79', '629', '7', '3', '2016-03-16 19:04:14', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('80', '646', '7', '3', '2016-03-16 19:15:56', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('81', '660', '7', '3', '2016-03-16 19:15:57', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('82', '632', '7', '3', '2016-03-16 19:16:00', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('83', '659', '7', '3', '2016-03-16 19:16:01', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('84', '633', '7', '3', '2016-03-16 19:16:04', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('85', '657', '7', '3', '2016-03-16 19:16:06', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('86', '634', '7', '3', '2016-03-16 19:16:09', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('87', '655', '7', '3', '2016-03-16 19:16:09', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('88', '635', '7', '3', '2016-03-16 19:16:12', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('89', '653', '7', '3', '2016-03-16 19:16:14', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('90', '636', '7', '3', '2016-03-16 19:16:17', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('91', '651', '7', '3', '2016-03-16 19:16:19', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('92', '637', '7', '3', '2016-03-16 19:16:22', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('93', '649', '7', '3', '2016-03-16 19:16:22', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('94', '638', '7', '3', '2016-03-16 19:16:24', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('95', '647', '7', '3', '2016-03-16 19:16:25', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('96', '645', '7', '3', '2016-03-16 19:16:27', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('97', '658', '7', '3', '2016-03-16 19:16:29', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('98', '644', '7', '3', '2016-03-16 19:16:30', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('99', '656', '7', '3', '2016-03-16 19:16:31', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('100', '642', '7', '3', '2016-03-16 19:16:33', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('101', '654', '7', '3', '2016-03-16 19:16:33', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('102', '652', '7', '3', '2016-03-16 19:16:37', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('103', '640', '7', '3', '2016-03-16 19:16:38', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('104', '643', '7', '3', '2016-03-16 19:16:41', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('105', '648', '7', '3', '2016-03-16 19:16:41', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('106', '661', '7', '3', '2016-03-16 19:16:44', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('107', '639', '7', '3', '2016-03-16 19:16:46', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('108', '650', '7', '3', '2016-03-16 19:16:47', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('109', '641', '7', '3', '2016-03-16 19:16:50', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('110', '681', '7', '3', '2016-03-16 19:23:43', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('111', '695', '7', '3', '2016-03-16 19:23:44', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('112', '667', '7', '3', '2016-03-16 19:23:47', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('113', '694', '7', '3', '2016-03-16 19:23:49', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('114', '668', '7', '3', '2016-03-16 19:23:52', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('115', '692', '7', '3', '2016-03-16 19:23:53', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('116', '690', '7', '3', '2016-03-16 19:23:57', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('117', '669', '7', '3', '2016-03-16 19:23:57', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('118', '689', '7', '3', '2016-03-16 19:23:59', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('119', '670', '7', '3', '2016-03-16 19:23:59', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('120', '687', '7', '3', '2016-03-16 19:24:01', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('121', '671', '7', '3', '2016-03-16 19:24:04', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('122', '685', '7', '3', '2016-03-16 19:24:05', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('123', '683', '7', '3', '2016-03-16 19:24:08', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('124', '672', '7', '3', '2016-03-16 19:24:09', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('125', '682', '7', '3', '2016-03-16 19:24:12', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('126', '680', '7', '3', '2016-03-16 19:24:12', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('127', '693', '7', '3', '2016-03-16 19:24:14', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('128', '679', '7', '3', '2016-03-16 19:24:14', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('129', '691', '7', '3', '2016-03-16 19:24:17', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('130', '677', '7', '3', '2016-03-16 19:24:18', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('131', '688', '7', '3', '2016-03-16 19:24:22', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('132', '675', '7', '3', '2016-03-16 19:24:23', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('133', '686', '7', '3', '2016-03-16 19:24:25', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('134', '673', '7', '3', '2016-03-16 19:24:25', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('135', '684', '7', '3', '2016-03-16 19:24:27', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('136', '678', '7', '3', '2016-03-16 19:24:27', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('137', '674', '7', '3', '2016-03-16 19:24:29', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('138', '696', '7', '3', '2016-03-16 19:24:30', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('139', '676', '7', '3', '2016-03-16 19:24:31', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('140', '711', '7', '3', '2016-03-16 19:25:44', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('141', '725', '7', '3', '2016-03-16 19:25:44', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('142', '724', '7', '3', '2016-03-16 19:25:47', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('143', '697', '7', '3', '2016-03-16 19:25:48', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('144', '723', '7', '3', '2016-03-16 19:25:52', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('145', '698', '7', '3', '2016-03-16 19:25:53', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('146', '721', '7', '3', '2016-03-16 19:25:54', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('147', '142', '7', '3', '2016-03-17 09:39:00', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('148', '156', '7', '3', '2016-03-17 09:39:01', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('149', '155', '7', '3', '2016-03-17 09:39:04', '0', null, '0', '2700', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('150', '128', '7', '3', '2016-03-17 09:39:06', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('151', '154', '7', '3', '2016-03-17 09:39:08', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('152', '129', '7', '3', '2016-03-17 09:39:10', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('153', '152', '7', '3', '2016-03-17 09:39:13', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('154', '130', '7', '3', '2016-03-17 09:39:13', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('155', '147', '7', '3', '2016-03-17 09:39:29', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('156', '144', '7', '3', '2016-03-17 09:39:32', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('157', '134', '7', '3', '2016-03-17 09:39:33', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('158', '135', '7', '3', '2016-03-17 09:39:37', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('159', '143', '7', '3', '2016-03-17 09:39:38', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('160', '140', '7', '3', '2016-03-17 09:39:39', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('161', '139', '7', '3', '2016-03-17 09:39:42', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('162', '153', '7', '3', '2016-03-17 09:39:44', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('163', '138', '7', '3', '2016-03-17 09:39:46', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('164', '136', '7', '3', '2016-03-17 09:39:48', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('165', '151', '7', '3', '2016-03-17 09:39:50', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('166', '141', '7', '3', '2016-03-17 09:39:53', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('167', '145', '7', '3', '2016-03-17 09:39:54', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('168', '137', '7', '3', '2016-03-17 09:39:56', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('169', '146', '7', '3', '2016-03-17 09:40:01', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('170', '157', '7', '3', '2016-03-17 09:40:13', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('171', '148', '7', '3', '2016-03-17 09:40:17', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('172', '172', '7', '3', '2016-03-17 09:57:43', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('173', '186', '7', '3', '2016-03-17 09:57:44', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('174', '158', '7', '3', '2016-03-17 09:57:47', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('175', '185', '7', '3', '2016-03-17 09:57:47', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('176', '183', '7', '3', '2016-03-17 09:57:49', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('177', '159', '7', '3', '2016-03-17 09:57:52', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('178', '182', '7', '3', '2016-03-17 09:57:52', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('179', '160', '7', '3', '2016-03-17 09:58:47', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('180', '180', '7', '3', '2016-03-17 09:58:47', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('181', '202', '7', '3', '2016-03-17 09:59:08', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('182', '216', '7', '3', '2016-03-17 09:59:09', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('183', '188', '7', '3', '2016-03-17 09:59:10', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('184', '215', '7', '3', '2016-03-17 09:59:12', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('185', '189', '7', '3', '2016-03-17 09:59:12', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('186', '213', '7', '3', '2016-03-17 09:59:14', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('187', '190', '7', '3', '2016-03-17 09:59:15', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('188', '232', '7', '3', '2016-03-17 10:04:15', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('189', '246', '7', '3', '2016-03-17 10:04:16', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('190', '218', '7', '3', '2016-03-17 10:04:17', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('191', '219', '7', '3', '2016-03-17 10:04:20', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('192', '245', '7', '3', '2016-03-17 10:05:14', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('193', '242', '7', '3', '2016-03-17 10:05:16', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('194', '220', '7', '3', '2016-03-17 10:05:17', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('195', '241', '7', '3', '2016-03-17 10:05:19', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('196', '239', '7', '3', '2016-03-17 10:05:21', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('197', '221', '7', '3', '2016-03-17 10:06:09', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('198', '222', '7', '3', '2016-03-17 10:06:11', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('199', '238', '7', '3', '2016-03-17 10:06:11', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('200', '235', '7', '3', '2016-03-17 10:06:13', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('201', '223', '7', '3', '2016-03-17 10:06:15', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('202', '234', '7', '3', '2016-03-17 10:06:18', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('203', '322', '7', '3', '2016-03-17 14:50:17', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('204', '336', '7', '3', '2016-03-17 14:50:18', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('205', '335', '7', '3', '2016-03-17 14:50:20', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('206', '308', '7', '3', '2016-03-17 14:50:22', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('207', '334', '7', '3', '2016-03-17 14:50:23', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('208', '332', '7', '3', '2016-03-17 14:50:25', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('209', '309', '7', '3', '2016-03-17 14:50:28', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('210', '310', '7', '3', '2016-03-17 14:50:32', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('211', '331', '7', '3', '2016-03-17 14:50:32', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('212', '311', '7', '3', '2016-03-17 14:50:34', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('213', '328', '7', '3', '2016-03-17 14:50:34', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('214', '312', '7', '3', '2016-03-17 14:50:39', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('215', '326', '7', '3', '2016-03-17 14:50:39', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('216', '313', '7', '3', '2016-03-17 14:50:45', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('217', '324', '7', '3', '2016-03-17 14:50:45', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('218', '314', '7', '3', '2016-03-17 14:50:50', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('219', '337', '7', '3', '2016-03-17 14:50:50', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('220', '320', '7', '3', '2016-03-17 14:50:53', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('221', '333', '7', '3', '2016-03-17 14:50:54', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('222', '330', '7', '3', '2016-03-17 14:50:57', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('223', '319', '7', '3', '2016-03-17 14:50:58', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('224', '329', '7', '3', '2016-03-17 14:50:59', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('225', '316', '7', '3', '2016-03-17 14:51:00', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('226', '321', '7', '3', '2016-03-17 14:51:04', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('227', '323', '7', '3', '2016-03-17 14:51:04', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('228', '325', '7', '3', '2016-03-17 14:51:08', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('229', '318', '7', '3', '2016-03-17 14:51:09', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('230', '315', '7', '3', '2016-03-17 14:51:12', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('231', '327', '7', '3', '2016-03-17 14:51:12', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('232', '317', '7', '3', '2016-03-17 14:51:17', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('233', '152', '7', '3', '2016-03-18 11:24:37', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('234', '166', '7', '3', '2016-03-18 11:24:38', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('235', '138', '7', '3', '2016-03-18 11:24:40', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('236', '165', '7', '3', '2016-03-18 11:24:44', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('237', '139', '7', '3', '2016-03-18 11:24:47', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('238', '163', '7', '3', '2016-03-18 11:24:48', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('239', '161', '7', '3', '2016-03-18 11:24:53', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('240', '140', '7', '3', '2016-03-18 11:24:53', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('241', '160', '7', '3', '2016-03-18 11:24:58', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('242', '141', '7', '3', '2016-03-18 11:24:58', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('243', '142', '7', '3', '2016-03-18 11:25:03', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('244', '158', '7', '3', '2016-03-18 11:25:04', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('245', '143', '7', '3', '2016-03-18 11:25:07', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('246', '155', '7', '3', '2016-03-18 11:25:09', '0', null, '0', '2700', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('247', '153', '7', '3', '2016-03-18 11:25:11', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('248', '144', '7', '3', '2016-03-18 11:25:13', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('249', '167', '7', '3', '2016-03-18 11:25:16', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('250', '164', '7', '3', '2016-03-18 11:25:19', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('251', '150', '7', '3', '2016-03-18 11:25:19', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('252', '162', '7', '3', '2016-03-18 11:25:21', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('253', '148', '7', '3', '2016-03-18 11:25:22', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('254', '159', '7', '3', '2016-03-18 11:25:24', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('255', '157', '7', '3', '2016-03-18 11:25:26', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('256', '146', '7', '3', '2016-03-18 11:25:27', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('257', '149', '7', '3', '2016-03-18 11:25:29', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('258', '154', '7', '3', '2016-03-18 11:25:30', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('259', '156', '7', '3', '2016-03-18 11:25:35', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('260', '151', '7', '3', '2016-03-18 11:25:35', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('261', '147', '7', '3', '2016-03-18 11:25:39', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('262', '145', '7', '3', '2016-03-18 11:25:44', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('263', '182', '7', '3', '2016-03-18 13:47:47', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('264', '196', '7', '3', '2016-03-18 13:47:48', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('265', '195', '7', '3', '2016-03-18 13:47:50', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('266', '168', '7', '3', '2016-03-18 13:47:52', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('267', '194', '7', '3', '2016-03-18 13:47:55', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('268', '169', '7', '3', '2016-03-18 13:47:56', '0', null, '0', '670', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('269', '192', '7', '3', '2016-03-18 13:47:57', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('270', '170', '7', '3', '2016-03-18 13:47:58', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('271', '190', '7', '3', '2016-03-18 13:48:01', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('272', '171', '7', '3', '2016-03-18 13:48:02', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('273', '188', '7', '3', '2016-03-18 13:48:05', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('274', '172', '7', '3', '2016-03-18 13:48:05', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('275', '186', '7', '3', '2016-03-18 13:48:07', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('276', '173', '7', '3', '2016-03-18 13:48:08', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('277', '184', '7', '3', '2016-03-18 13:48:10', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('278', '174', '7', '3', '2016-03-18 13:48:11', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('279', '197', '7', '3', '2016-03-18 13:48:13', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('280', '180', '7', '3', '2016-03-18 13:48:14', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('281', '193', '7', '3', '2016-03-18 13:48:17', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('282', '179', '7', '3', '2016-03-18 13:48:19', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('283', '191', '7', '3', '2016-03-18 13:48:21', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('284', '177', '7', '3', '2016-03-18 13:48:21', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('285', '189', '7', '3', '2016-03-18 13:48:25', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('286', '175', '7', '3', '2016-03-18 13:48:26', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('287', '183', '7', '3', '2016-03-18 13:48:28', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('288', '185', '7', '3', '2016-03-18 13:48:31', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('289', '178', '7', '3', '2016-03-18 13:48:31', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('290', '187', '7', '3', '2016-03-18 13:48:34', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('291', '176', '7', '3', '2016-03-18 13:48:36', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('292', '181', '7', '3', '2016-03-18 13:48:41', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('293', '212', '7', '3', '2016-03-18 13:59:28', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('294', '226', '7', '3', '2016-03-18 13:59:29', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('295', '225', '7', '3', '2016-03-18 13:59:31', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('296', '198', '7', '3', '2016-03-18 13:59:33', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('297', '224', '7', '3', '2016-03-18 13:59:34', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('298', '199', '7', '3', '2016-03-18 13:59:35', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('299', '200', '7', '3', '2016-03-18 13:59:37', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('300', '222', '7', '3', '2016-03-18 13:59:39', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('301', '201', '7', '3', '2016-03-18 13:59:40', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('302', '219', '7', '3', '2016-03-18 13:59:42', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('303', '202', '7', '3', '2016-03-18 13:59:43', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('304', '203', '7', '3', '2016-03-18 13:59:45', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('305', '217', '7', '3', '2016-03-18 13:59:47', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('306', '204', '7', '3', '2016-03-18 13:59:47', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('307', '205', '7', '3', '2016-03-18 13:59:50', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('308', '214', '7', '3', '2016-03-18 13:59:51', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('309', '211', '7', '3', '2016-03-18 13:59:53', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('310', '210', '7', '3', '2016-03-18 13:59:55', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('311', '223', '7', '3', '2016-03-18 13:59:56', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('312', '209', '7', '3', '2016-03-18 13:59:58', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('313', '221', '7', '3', '2016-03-18 14:00:00', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('314', '207', '7', '3', '2016-03-18 14:00:00', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('315', '227', '7', '3', '2016-03-18 14:00:03', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('316', '208', '7', '3', '2016-03-18 14:00:05', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('317', '213', '7', '3', '2016-03-18 14:00:43', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('318', '206', '7', '3', '2016-03-18 14:00:43', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('319', '216', '7', '3', '2016-03-18 14:00:48', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('320', '220', '7', '3', '2016-03-18 14:00:51', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('321', '218', '7', '3', '2016-03-18 14:00:53', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');
INSERT INTO `recorduserinout` VALUES ('322', '215', '7', '3', '2016-03-18 14:00:58', '0', null, '0', '0', '', '0.0.0.0', '0000-00-00 00:00:00', '0', null, null, null, null, null, '0', '0', '0', '0', '0', '0', '0', '0', '0');

-- ----------------------------
-- Table structure for `streammatchfeeinfo`
-- ----------------------------
DROP TABLE IF EXISTS `streammatchfeeinfo`;
CREATE TABLE `streammatchfeeinfo` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT COMMENT '记录标识',
  `UserID` int(11) DEFAULT '0' COMMENT '用户标识',
  `ServerID` int(11) DEFAULT '0' COMMENT '房间标识',
  `MatchID` int(11) DEFAULT '0' COMMENT '比赛标识',
  `MatchNo` int(11) DEFAULT '0' COMMENT '比赛序号',
  `MatchType` smallint(6) DEFAULT NULL,
  `MatchFeeType` smallint(6) DEFAULT NULL,
  `MatchFee` bigint(20) DEFAULT NULL COMMENT '报名费',
  `Effective` int(11) DEFAULT '0' COMMENT '有效标识（0为有效 1为无效）',
  `CollectDate` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '收录时间',
  PRIMARY KEY (`RecordID`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of streammatchfeeinfo
-- ----------------------------

-- ----------------------------
-- Table structure for `streammatchhistory`
-- ----------------------------
DROP TABLE IF EXISTS `streammatchhistory`;
CREATE TABLE `streammatchhistory` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `UserID` int(11) DEFAULT '0' COMMENT '用户标识',
  `MatchID` int(11) DEFAULT '0' COMMENT '比赛标识',
  `MatchNo` int(11) DEFAULT '0' COMMENT '比赛场次',
  `MatchType` int(11) DEFAULT '0' COMMENT '比赛类型',
  `ServerID` int(11) DEFAULT '0' COMMENT '房间ID',
  `RankID` smallint(6) DEFAULT NULL COMMENT '比赛名次',
  `MatchScore` int(11) DEFAULT '0' COMMENT '比赛得分',
  `UserRight` int(11) DEFAULT '0' COMMENT '用户权限 如：有进入下一论的权限',
  `RewardGold` int(11) DEFAULT '0' COMMENT '奖励金币',
  `RewardMedal` int(11) DEFAULT '0' COMMENT '奖励奖牌',
  `RewardExperience` int(11) DEFAULT '0' COMMENT '奖励经验',
  `WinCount` int(11) DEFAULT '0' COMMENT '赢的局数',
  `LostCount` int(11) DEFAULT '0' COMMENT '输的局数',
  `DrawCount` int(11) DEFAULT '0' COMMENT '平的局数',
  `FleeCount` int(11) DEFAULT '0' COMMENT '逃跑局数',
  `MatchStartTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '比赛第一局开始时间',
  `MatchEndTime` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '比赛最后一句结束时间',
  `PlayTimeCount` int(11) DEFAULT '0' COMMENT '游戏时长 单位:秒',
  `OnlineTime` int(11) DEFAULT '0' COMMENT '在线时长 单位:秒',
  `Machine` varchar(32) DEFAULT NULL COMMENT '机器码',
  `ClientIP` varchar(15) DEFAULT NULL COMMENT '连接地址',
  `RecordDate` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '录入时间',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of streammatchhistory
-- ----------------------------

-- ----------------------------
-- Table structure for `systemstreaminfo`
-- ----------------------------
DROP TABLE IF EXISTS `systemstreaminfo`;
CREATE TABLE `systemstreaminfo` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `DateID` int(11) DEFAULT '0' COMMENT '日期标识',
  `KindID` int(11) DEFAULT '0' COMMENT '类型标识',
  `ServerID` int(11) DEFAULT '0' COMMENT '房间标识',
  `LogonCount` int(11) DEFAULT '0' COMMENT '进入数目',
  `RegisterCount` int(11) DEFAULT '0' COMMENT '注册数目',
  `CollectDate` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '录入时间',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=gb2312;

-- ----------------------------
-- Records of systemstreaminfo
-- ----------------------------

-- ----------------------------
-- Procedure structure for `GSP_GR_EfficacyMobile`
-- ----------------------------
DROP PROCEDURE IF EXISTS `GSP_GR_EfficacyMobile`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `GSP_GR_EfficacyMobile`(
	dwUserID INT,								-- 用户 I D
	strPassword NCHAR(32),						-- 用户密码
	strClientIP NVARCHAR(15),					-- 连接地址
	strMachineID NVARCHAR(32),					-- 机器标识
	wKindID SMALLINT,							-- 游戏 I D
	wServerID SMALLINT,						-- 房间 I D
	dwMatchID INT,								-- 比赛标识
	dwMatchNo INT,								-- 比赛编号	
	OUT strErrorDescribe NVARCHAR(127),		-- 输出信息
	OUT nRet INT
)
label_pro:BEGIN

	-- 基本信息
	DECLARE nUserID INT;
	DECLARE nFaceID SMALLINT;
	DECLARE nCustomID INT;
	DECLARE strNickName NVARCHAR(31);
	DECLARE strUnderWrite NVARCHAR(63);

	-- 扩展信息
	DECLARE nGameID INT;
	DECLARE nGroupID INT;
	DECLARE nUserRight INT;
	DECLARE nGender TINYINT;
	DECLARE nMasterRight INT;
	DECLARE nMasterOrder SMALLINT;
	DECLARE nMemberOrder SMALLINT;
	DECLARE strGroupName NVARCHAR(31);

	-- 积分变量
	DECLARE nScore BIGINT;
	DECLARE nWinCount INT;
	DECLARE nLostCount INT;
	DECLARE nDrawCount INT;
	DECLARE nFleeCount INT;
	DECLARE nUserMedal INT;
	DECLARE nExperience INT;
	DECLARE nLoveLiness INT;

	-- 辅助变量
	DECLARE nEnjoinLogon BIGINT;

	-- 查询用户
	DECLARE cbNullity BIT;
	DECLARE cbStunDown BIT;
	DECLARE strLogonPass NCHAR(32);
	DECLARE	strStoreMachineID NVARCHAR(32);
	DECLARE strMoorMachine TINYINT;
	
	-- 查询锁定
	DECLARE nLockKindID INT;
	DECLARE nLockServerID INT;

	-- 查询信息
	DECLARE strKindName NVARCHAR(31);
	DECLARE strServerName NVARCHAR(31);
	
	-- 查询赛事
	DECLARE cbMatchType TINYINT;

	-- 查询初始分
	DECLARE nInitScore BIGINT;
	
	-- 游戏信息
	DECLARE nAllLogonTimes INT;
	DECLARE nGameUserRight INT;
	DECLARE nGameMasterRight INT;
	DECLARE nGameMasterOrder SMALLINT;
	
	-- 记录标识
	DECLARE nInoutIndex BIGINT;
	
	-- 登录统计
	DECLARE nDateID INT;
	
	-- 系统暂停
	SELECT nEnjoinLogon=StatusValue FROM QPAccountsDB.SystemStatusInfo WHERE StatusName='EnjoinLogon';
	IF nEnjoinLogon IS NOT NULL AND nEnjoinLogon<>0 THEN
		SELECT strErrorDescribe=StatusString FROM QPAccountsDB.SystemStatusInfo WHERE StatusName='EnjoinLogon';
		SET nRet = 2;
		leave label_pro;
	END IF;

	-- 效验地址
	SELECT nEnjoinLogon=EnjoinLogon FROM ConfineAddress WHERE AddrString=strClientIP AND NOW()<EnjoinOverDate;
	IF nEnjoinLogon IS NOT NULL AND nEnjoinLogon<>0 THEN
		SET strErrorDescribe='抱歉地通知您，系统禁止了您所在的 IP 地址的游戏登录权限，请联系客户服务中心了解详细情况！';
		SET nRet = 4;
		leave label_pro;
	END IF;
	
	-- 查询用户
	SELECT nUserID=UserID, nGameID=GameID, strNickName=NickName, strUnderWrite=UnderWrite, strLogonPass=DynamicPass, nFaceID=FaceID, nCustomID=CustomID,
		nGender=Gender, cbNullity=Nullity, cbStunDown=StunDown, nUserMedal=UserMedal, nExperience=Experience, nLoveLiness=LoveLiness, nUserRight=UserRight,
		nMasterRight=MasterRight, nMasterOrder=MasterOrder, nMemberOrder=MemberOrder, strMoorMachine=MoorMachine, strStoreMachineID=LastLogonMachine
	FROM QPAccountsDB.AccountsInfo WHERE UserID=dwUserID;

	-- 查询用户
	IF nUserID IS NULL THEN
		SET strErrorDescribe='您的帐号不存在或者密码输入有误，请查证后再次尝试登录！';
		SET nRet = 1;
		leave label_pro;
	END IF;

	-- 帐号禁止
	IF cbNullity<>0 THEN
		SET strErrorDescribe='您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！';
		SET nRet = 2;
		leave label_pro;
	END IF;

	-- 帐号关闭
	IF cbStunDown<>0 THEN
		SET strErrorDescribe='您的帐号使用了安全关闭功能，必须重新开通后才能继续使用！';
		SET nRet = 2;
		leave label_pro;
	END IF;
	
	-- 固定机器
	IF strMoorMachine<>0 THEN
		SET strErrorDescribe='您的帐号使用固定机器登录功能，您不能使用手机终端进行登录！';
		SET nRet = 1;
		leave label_pro;
	END IF;

	-- 密码判断
	IF strLogonPass<>strPassword AND strClientIP<>'0.0.0.0' THEN
		SET strErrorDescribe='您的帐号不存在或者密码输入有误，请查证后再次尝试！';
		SET nRet = 3;
		leave label_pro;
	END IF;

	-- 查询锁定
	SELECT nLockKindID=KindID, nLockServerID=ServerID FROM QPTreasureDB.GameScoreLocker WHERE UserID=dwUserID AND ServerID=wServerID;

	-- 锁定判断
	IF nLockKindID IS NOT NULL AND nLockServerID IS NOT NULL THEN
		-- 查询类型
		IF nLockKindID<>0 THEN
			-- 查询信息
			SELECT strKindName=KindName FROM QPPlatformDB.GameKindItem WHERE KindID=nLockKindID;
			SELECT strServerName=ServerName FROM QPPlatformDB.GameRoomInfo WHERE ServerID=nLockServerID;

			-- 错误信息
			IF strKindName IS NULL THEN
				SET strKindName='未知游戏';
			END IF;
			IF strServerName IS NULL THEN
				SET strServerName='未知房间';
			END IF;
			SET strErrorDescribe='您已经在 [ '+strKindName+' ] 的 [ '+strServerName+' ] 游戏房间中，不能再次进入此游戏房间！';
			SET nRet = 4;
			leave label_pro;
		ELSE
			-- 提示消息
			SELECT ErrorDescribe='当前游戏房间的游戏资料已被系统锁定，暂时无法进入此游戏房间！';
			SET nRet = 4;
			leave label_pro;
		END IF;
	END IF;

	-- 查询赛事
	SELECT cbMatchType=MatchType FROM MatchPublic WHERE MatchID=dwMatchID AND MatchNo=dwMatchNO;
	
	-- 存在判断
	IF cbMatchType IS NULL THEN
		SET strErrorDescribe='抱歉，没有找到相关赛事的信息，无法进入此游戏房间！';
		SET nRet = 5;
		leave label_pro;
	END IF;

	-- 定时赛报名判断
	IF cbMatchType=0 THEN
		IF strClientIP <> '0.0.0.0' THEN
			-- 还未报名
			IF not exists(SELECT * FROM StreamMatchFeeInfo WHERE UserID=dwUserID AND ServerID=wServerID AND MatchID=dwMatchID AND Effective=0) THEN
				SET strErrorDescribe='抱歉，此场比赛为定时赛，请先进入比赛中心报名再来参与比赛！';
				SET nRet = 8;
				leave label_pro;
			END IF;
		ELSE
			IF NOT exists(SELECT * FROM MatchScoreInfo WHERE UserID=dwUserID AND ServerID=wServerID AND MatchID=dwMatchID AND MatchNo=dwMatchNO) THEN
				-- 查询初始分
				SELECT nInitScore=InitScore FROM MatchLockTime WHERE MatchID=dwMatchID AND MatchNo=dwMatchNO;
				
				-- 调整分数
				IF nInitScore IS NULL THEN
					SET nInitScore=0;
				END IF;

				-- 插入记录
				INSERT INTO MatchScoreInfo(UserID,ServerID,MatchID,MatchNo,Score)	
				VALUES(dwUserID,wServerID,dwMatchID,dwMatchNO,nInitScore);
			END IF;
		END IF;

		-- 分数信息
		SELECT nScore=Score, nWinCount=WinCount, nLostCount=LostCount, nDrawCount=DrawCount, nFleeCount=FleeCount FROM MatchScoreInfo
		WHERE UserID=dwUserID AND ServerID=wServerID AND MatchID=dwMatchID AND MatchNo=dwMatchNO;
	END IF;

	-- 分数信息
	IF nScore IS NULL THEN
		SELECT nScore=0, nWinCount=0, nLostCount=0,nDrawCount=0, nDrawCount=0, nFleeCount=0;
	END IF;

	-- 游戏信息
	SELECT 	nGameUserRight=UserRight, nGameMasterRight=MasterRight, nGameMasterOrder=MasterOrder, nAllLogonTimes=AllLogonTimes	
	FROM GameScoreInfo WHERE UserID=dwUserID;

	-- 存在判断
	IF nGameUserRight IS NULL THEN
		-- 插入资料
		INSERT INTO GameScoreInfo (UserID, LastLogonIP, LastLogonMachine) VALUES (dwUserID, strClientIP, strMachineID);

		-- 游戏信息
		SELECT nGameUserRight=UserRight, nGameMasterOrder=MasterOrder, nGameMasterRight=MasterRight, nAllLogonTimes=AllLogonTimes
		FROM GameScoreInfo WHERE UserID=dwUserID;
	END IF;

	-- 保留变量
	SET nGroupID=0;
	SET strGroupName='';

	-- 权限标志
	SET nUserRight=nUserRight|nGameUserRight;
	SET nMasterRight=nMasterRight|nGameMasterRight;

	-- 权限等级
	IF nGameMasterOrder>nMasterOrder THEN
		SET nMasterOrder=nGameMasterOrder;
	END IF;

	-- 进入记录
	INSERT RecordUserInout (UserID, EnterScore, EnterUserMedal,EnterLoveliness, KindID, ServerID, EnterClientIP, EnterMachine)
	VALUES (nUserID, nScore, nUserMedal, nLoveLiness, wKindID, wServerID, strClientIP, strMachineID);

	-- 记录标识
	SET nInoutIndex=LAST_INSERT_ID();

	-- 插入锁表
	INSERT QPTreasureDB.GameScoreLocker (UserID, ServerID, KindID, EnterID, EnterIP, EnterMachine) VALUES (dwUserID, wServerID, wKindID, nInoutIndex, strClientIP, strMachineID);
	IF @error<>0 THEN
		-- 错误信息
		SET strErrorDescribe='抱歉地通知你，游戏积分锁定操作失败，请联系客户服务中心了解详细情况！';
		SET nRet = 14;
		leave label_pro;
	END IF;

	-- 登录统计
	SET nDateID=UNIX_TIMESTAMP(NOW());

	-- 插入记录
	IF nAllLogonTimes>0 THEN
		UPDATE SystemStreamInfo SET LogonCount=LogonCount+1 WHERE DateID=nDateID AND KindID=wKindID AND ServerID=wServerID;
		IF ROW_COUNT()=0 THEN
			INSERT SystemStreamInfo (DateID, KindID, ServerID, LogonCount) VALUES (nDateID, wKindID, wServerID, 1);
		END IF;
	ELSE
		UPDATE SystemStreamInfo SET RegisterCount=RegisterCount+1 WHERE DateID=nDateID AND KindID=wKindID AND ServerID=wServerID;
		IF ROW_COUNT()=0 THEN
			INSERT SystemStreamInfo (DateID, KindID, ServerID, RegisterCount) VALUES (nDateID, wKindID, wServerID, 1);
		END IF;
	END IF;

	-- 输出变量
	SELECT nUserID AS UserID, nGameID AS GameID, nGroupID AS GroupID, strNickName AS NickName, strUnderWrite AS UnderWrite, nFaceID AS FaceID,
		nCustomID AS CustomID, nGender AS Gender, strGroupName AS GroupName, nMasterOrder AS MemberOrder, nUserRight AS UserRight, nMasterRight AS MasterRight,
		nMasterOrder AS MasterOrder, nMemberOrder AS MemberOrder, nScore AS Score,  0 AS Grade, 0 AS Insure,  nWinCount AS WinCount,  nLostCount AS LostCount,
		nDrawCount AS DrawCount, nFleeCount AS FleeCount, nUserMedal AS Ingot, nExperience AS Experience, nLoveLiness AS LoveLiness, nInoutIndex AS InoutIndex;

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `GSP_GR_EfficacyUserID`
-- ----------------------------
DROP PROCEDURE IF EXISTS `GSP_GR_EfficacyUserID`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `GSP_GR_EfficacyUserID`(dwUserID INT,								-- 用户 I D
	strPassword NCHAR(32),						-- 用户密码
	strClientIP NVARCHAR(15),					-- 连接地址
	strMachineID NVARCHAR(32),					-- 机器标识
	wKindID SMALLINT,							-- 游戏 I D
	wServerID SMALLINT,						-- 房间 I D
	dwMatchID INT,								-- 比赛标识
	dwMatchNo INT,								-- 比赛编号	
	OUT strErrorDescribe NVARCHAR(127),		-- 输出信息
	OUT nRet INT)
label_pro:BEGIN
	-- 基本信息
	DECLARE nUserID INT;
	DECLARE nFaceID SMALLINT;
	DECLARE nCustomID INT;
	DECLARE strNickName NVARCHAR(31);
	DECLARE strUnderWrite NVARCHAR(63);

	-- 扩展信息
	DECLARE nGameID INT;
	DECLARE nGroupID INT;
	DECLARE nUserRight INT;
	DECLARE nGender TINYINT;
	DECLARE nMasterRight INT;
	DECLARE nMasterOrder SMALLINT;
	DECLARE nMemberOrder SMALLINT;
	DECLARE strGroupName NVARCHAR(31);

	-- 积分变量
	DECLARE nScore BIGINT;
	DECLARE nWinCount INT;
	DECLARE nLostCount INT;
	DECLARE nDrawCount INT;
	DECLARE nFleeCount INT;
	DECLARE nUserMedal INT;
	DECLARE nExperience INT;
	DECLARE nLoveLiness INT;

	-- 定时賽变量
	DECLARE nTimeMatchEnter INT;

	-- 辅助变量
	DECLARE nEnjoinLogon INT;

	-- 查询用户
	DECLARE cbNullity BIT;
	DECLARE cbStunDown BIT;
	DECLARE strLogonPass NCHAR(32);
	DECLARE	strStoreMachineID NVARCHAR(32);
	DECLARE strMoorMachine TINYINT;
	
	-- 查询锁定
	DECLARE nLockKindID INT;
	DECLARE nLockServerID INT;

	-- 查询信息
	DECLARE strKindName NVARCHAR(31);
	DECLARE strServerName NVARCHAR(31);
	
	-- 查询赛事
	DECLARE cbMatchType TINYINT;

	-- 查询初始分
	DECLARE nInitScore BIGINT;
	
	-- 游戏信息
	DECLARE nAllLogonTimes INT;
	DECLARE nGameUserRight INT;
	DECLARE nGameMasterRight INT;
	DECLARE nGameMasterOrder SMALLINT;
	
	-- 记录标识
	DECLARE nInoutIndex BIGINT;
	
	-- 登录统计
	DECLARE nDateID INT;
 
	-- 查询用户
	SELECT UserID,NickName,UnderWrite,FaceID,CustomID,Gender,Nullity,StunDown,UserMedal,Experience,LoveLiness,UserRight
	,MasterRight,MasterOrder,MemberOrder,MoorMachine,LastLogonMachine 
	INTO nUserID, strNickName, strUnderWrite, nFaceID, nCustomID,nGender, cbNullity, cbStunDown, nUserMedal, nExperience, nLoveLiness, nUserRight,
		nMasterRight, nMasterOrder, nMemberOrder, strMoorMachine, strStoreMachineID FROM QPAccountsDB.AccountsInfo WHERE UserID=dwUserID;

	-- 查询用户
	IF nUserID IS NULL THEN
		SET strErrorDescribe='您的帐号不存在或者密码输入有误，请查证后再次尝试登录！';
		SET nRet = 1;
		leave label_pro;
	END	IF;

	-- 帐号禁止
	IF cbNullity<>0 THEN
		SET strErrorDescribe='您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！';
		SET nRet = 2;
		leave label_pro;
	END IF;

	-- 帐号关闭
	IF cbStunDown<>0 THEN
		SET strErrorDescribe='您的帐号使用了安全关闭功能，必须重新开通后才能继续使用！';
		SET nRet = 2;
		leave label_pro;
	END IF;
	
	-- 密码判断
	IF strLogonPass<>strPassword AND strClientIP<>'0.0.0.0' THEN
		SET strErrorDescribe='您的帐号不存在或者密码输入有误，请查证后再次尝试！';
		SET nRet = 3;
		leave label_pro;
	END IF;

	-- 查询赛事
	SELECT MatchType INTO cbMatchType FROM MatchPublic WHERE MatchID=dwMatchID AND MatchNo=dwMatchNO;
	
	-- 存在判断
	IF cbMatchType IS NULL THEN
		SET strErrorDescribe='抱歉，没有找到相关赛事的信息，无法进入此游戏房间！';
		SET nRet = 5;
		leave label_pro;
	END IF;

	-- 分数信息
	IF nScore IS NULL THEN
		SET nScore=0;
		SET nWinCount=0;
		SET nLostCount=0;
		SET nDrawCount=0;
		SET nDrawCount=0;
		SET nFleeCount=0;
	END IF;

	-- 游戏信息
	SELECT 	UserRight, MasterRight, MasterOrder, AllLogonTimes INTO nGameUserRight,nGameMasterRight,nGameMasterOrder,nAllLogonTimes
	FROM GameScoreInfo WHERE UserID=dwUserID;

	-- 存在判断
	IF nGameUserRight IS NULL THEN
		-- 插入资料
		INSERT INTO GameScoreInfo (UserID, LastLogonIP, LastLogonMachine) VALUES (dwUserID, strClientIP, strMachineID);

		-- 游戏信息
		SELECT UserRight, MasterOrder, MasterRight, AllLogonTimes	INTO nGameUserRight,nGameMasterOrder,nGameMasterRight,nAllLogonTimes
		FROM GameScoreInfo WHERE UserID=dwUserID;
	END IF;

	-- 保留变量
	SET nGroupID=0;
	SET strGroupName='';

	-- 权限标志
	SET nUserRight=nUserRight|nGameUserRight;
	SET nMasterRight=nMasterRight|nGameMasterRight;

	-- 权限等级
	IF nGameMasterOrder>nMasterOrder THEN
		SET nMasterOrder=nGameMasterOrder;
	END IF;

	-- 进入记录
	INSERT RecordUserInout (UserID, EnterScore, EnterUserMedal,EnterLoveliness, KindID, ServerID, EnterClientIP, EnterMachine)
	VALUES (nUserID, nScore, nUserMedal, nLoveLiness, wKindID, wServerID, strClientIP, strMachineID);

	-- 输出变量
	SELECT nUserID AS UserID, nGameID AS GameID, nGroupID AS GroupID, strNickName AS NickName, strUnderWrite AS UnderWrite,  nFaceID AS FaceID,
		nCustomID AS CustomID, nGender AS Gender, strGroupName AS GroupName, nMasterOrder AS MemberOrder, nUserRight AS UserRight, nMasterRight AS MasterRight,
		nMasterOrder AS MasterOrder, nMemberOrder AS MemberOrder, nScore AS Score,  0 AS Grade, 0 AS Insure,  nWinCount AS WinCount,  nLostCount AS LostCount,
		nDrawCount AS DrawCount, nFleeCount AS FleeCount, nUserMedal AS Ingot, nExperience AS Experience, nLoveLiness AS LoveLiness, nInoutIndex AS InoutIndex;

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `GSP_GR_GameColumnItem`
-- ----------------------------
DROP PROCEDURE IF EXISTS `GSP_GR_GameColumnItem`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `GSP_GR_GameColumnItem`()
BEGIN

-- 查询描述
SELECT * FROM GameColumnItem ORDER BY SortID;

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `GSP_GR_LeaveGameServer`
-- ----------------------------
DROP PROCEDURE IF EXISTS `GSP_GR_LeaveGameServer`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `GSP_GR_LeaveGameServer`(

	-- 用户信息
	dwUserID INT,								-- 用户 I D
	dwOnLineTimeCount INT,						-- 在线时间

	-- 系统信息
	dwInoutIndex INT,							-- 进出索引
	dwLeaveReason INT,							-- 离开原因

	-- 记录成绩
	lRecordScore BIGINT,						-- 用户分数
	lRecordGrade BIGINT,						-- 用户成绩
	lRecordInsure BIGINT,						-- 用户银行
	lRecordRevenue BIGINT,						-- 游戏税收
	lRecordWinCount INT,						-- 胜利盘数
	lRecordLostCount INT,						-- 失败盘数
	lRecordDrawCount INT,						-- 和局盘数
	lRecordFleeCount INT,						-- 断线数目
	lRecordUserMedal INT,						-- 用户奖牌
	lRecordExperience INT,						-- 用户经验
	lRecordLoveLiness INT,						-- 用户魅力
	dwRecordPlayTimeCount INT,					-- 游戏时间

	-- 变更成绩
	lVariationScore BIGINT,					-- 用户分数
	lVariationGrade BIGINT,					-- 用户成绩
	lVariationInsure BIGINT,					-- 用户银行
	lVariationRevenue BIGINT,					-- 游戏税收
	lVariationWinCount INT,					-- 胜利盘数
	lVariationLostCount INT,					-- 失败盘数
	lVariationDrawCount INT,					-- 和局盘数
	lVariationFleeCount INT,					-- 断线数目
	lVariationUserMedal INT,					-- 用户奖牌
	lVariationExperience INT,					-- 用户经验
	lVariationLoveLiness INT,					-- 用户魅力
	dwVariationPlayTimeCount INT,				-- 游戏时间

	-- 属性信息
	wKindID INT,								-- 游戏 I D
	wServerID INT,								-- 房间 I D
	strClientIP NVARCHAR(15),					-- 连接地址
	strMachineSerial NVARCHAR(32)				-- 机器标识
)
BEGIN

	-- 用户积分
	UPDATE GameScoreInfo SET PlayTimeCount=PlayTimeCount+dwVariationPlayTimeCount,	OnLineTimeCount=OnLineTimeCount+dwOnLineTimeCount
	WHERE UserID=dwUserID;

	-- 锁定解除
	DELETE FROM QPTreasureDB.GameScoreLocker WHERE UserID=dwUserID AND ServerID=wServerID;

	-- 离开记录
	UPDATE RecordUserInout SET LeaveTime=NOW(), LeaveReason=dwLeaveReason, LeaveMachine=strMachineSerial, LeaveClientIP=strClientIP,
		Score=lRecordScore, Insure=lRecordInsure, Revenue=lRecordRevenue, WinCount=lRecordWinCount, LostCount=lRecordLostCount,
		DrawCount=lRecordDrawCount, FleeCount=lRecordFleeCount, UserMedal=lRecordUserMedal, Experience=lRecordExperience, LoveLiness=lRecordLoveLiness,
		PlayTimeCount=dwRecordPlayTimeCount, OnLineTimeCount=dwOnLineTimeCount
	WHERE ID=dwInoutIndex AND UserID=dwUserID;

	-- 全局信息
	IF lVariationExperience>0 OR lVariationLoveLiness>0 OR lVariationUserMedal>0 THEN
		UPDATE QPAccountsDB.AccountsInfo SET Experience=Experience+lVariationExperience, LoveLiness=LoveLiness+lVariationLoveLiness,
			UserMedal=UserMedal+lVariationUserMedal
		WHERE UserID=dwUserID;
	END IF;

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `GSP_GR_LoadAndroidUser`
-- ----------------------------
DROP PROCEDURE IF EXISTS `GSP_GR_LoadAndroidUser`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `GSP_GR_LoadAndroidUser`(
	wServerID	SMALLINT,					-- 房间标识
	dwBatchID	INT,						-- 批次标识
	dwAndroidCount INT,						-- 机器数目
	OUT nRet INT
)
BEGIN
	-- 变量定义	
	DECLARE	return_value int;

	CALL QPAccountsDB.GSP_GR_LoadAndroidUser(wServerID, dwBatchID, dwAndroidCount, return_value);

	SET nRet = return_value;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `GSP_GR_LoadGameProperty`
-- ----------------------------
DROP PROCEDURE IF EXISTS `GSP_GR_LoadGameProperty`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `GSP_GR_LoadGameProperty`(
	wKindID SMALLINT,							-- 游戏 I D
	wServerID SMALLINT							-- 房间 I D
)
BEGIN

	-- 加载道具
	SELECT ID, IssueArea, Cash, Gold, Discount, SendLoveLiness, RecvLoveLiness FROM GameProperty
	WHERE Nullity=0 AND (IssueArea&6)<>0;

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `GSP_GR_LoadParameter`
-- ----------------------------
DROP PROCEDURE IF EXISTS `GSP_GR_LoadParameter`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `GSP_GR_LoadParameter`(
	wKindID SMALLINT,							-- 游戏 I D
	wServerID SMALLINT							-- 房间 I D
)
BEGIN

	-- 奖牌汇率
	DECLARE MedalRate INT;
	
	-- 银行税率
	DECLARE RevenueRate INT;

	-- 兑换比率
	DECLARE ExchangeRate INT;	

	-- 赢局经验
	DECLARE WinExperience INT;
	
	-- 程序版本
	DECLARE ClientVersion INT;
	DECLARE ServerVersion INT;
	
	SELECT MedalRate=StatusValue FROM QPAccountsDB.SystemStatusInfo WHERE StatusName='MedalRate';

	-- 银行税率
	SELECT RevenueRate=StatusValue FROM QPAccountsDB.SystemStatusInfo WHERE StatusName='RevenueRate';

	-- 兑换比率
	SELECT ExchangeRate=StatusValue FROM QPAccountsDB.SystemStatusInfo WHERE StatusName='MedalExchangeRate';

	-- 赢局经验
	SELECT WinExperience=StatusValue FROM QPAccountsDB.SystemStatusInfo WHERE StatusName='WinExperience';

	-- 参数调整
	IF MedalRate IS NULL THEN
		SET MedalRate=1;
	END IF;
	IF RevenueRate IS NULL THEN
		SET RevenueRate=1;
	END IF;

	-- 程序版本
	SELECT ClientVersion=TableGame.ClientVersion, ServerVersion=TableGame.ServerVersion
	FROM QPPlatformDB.GameGameItem TableGame,QPPlatformDB.GameKindItem TableKind
	WHERE TableGame.GameID=TableKind.GameID	AND TableKind.KindID=wKindID;

	-- 输出结果
	SELECT MedalRate AS MedalRate, RevenueRate AS RevenueRate,ExchangeRate AS ExchangeRate,WinExperience AS WinExperience, 
		ClientVersion AS ClientVersion, ServerVersion AS ServerVersion;

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `GSP_GR_LoadSystemMessage`
-- ----------------------------
DROP PROCEDURE IF EXISTS `GSP_GR_LoadSystemMessage`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `GSP_GR_LoadSystemMessage`(
	wServerID SMALLINT							-- 房间 I D
)
BEGIN

	-- 查询机器
	SELECT * FROM QPPlatformDB.SystemMessage
	WHERE (StartTime <= NOW()) AND (ConcludeTime > NOW()) AND (StartTime<ConcludeTime) AND Nullity=0 ORDER BY ID;

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `GSP_GR_ManageUserRight`
-- ----------------------------
DROP PROCEDURE IF EXISTS `GSP_GR_ManageUserRight`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `GSP_GR_ManageUserRight`(
	dwUserID		INT,				-- 用户 I D
	dwAddRight		INT,				-- 增加权限
	dwRemoveRight	INT					-- 删除权限
)
BEGIN
	-- 设置权限
	UPDATE GameScoreInfo SET UserRight=UserRight|dwAddRight WHERE UserID=dwUserID;
	UPDATE GameScoreInfo SET UserRight=(~dwRemoveRight)&UserRight WHERE UserID=dwUserID	;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `GSP_GR_MatchEliminate`
-- ----------------------------
DROP PROCEDURE IF EXISTS `GSP_GR_MatchEliminate`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `GSP_GR_MatchEliminate`(
	dwUserID		INT,		-- 用户标识
	wServerID		INT,		-- 房间标识	
	dwMatchID		INT,		-- 比赛标识
	dwMatchNo		INT,		-- 比赛场次
	cbMatchType	TINYINT		-- 比赛类型			
)
BEGIN

	-- 删除记录
	DELETE FROM StreamMatchFeeInfo 
	WHERE UserID=dwUserID AND ServerID=wServerID AND MatchID=dwMatchID AND MatchNo=dwMatchNo AND Effective=0;

	-- 更新分数
	UPDATE MatchScoreInfo SET Score=0,WinCount=0,LostCount=0,DrawCount=0,FleeCount=0
	WHERE UserID=dwUserID AND ServerID=wServerID AND MatchID=dwMatchID AND MatchNo=dwMatchNo;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `GSP_GR_MatchStart`
-- ----------------------------
DROP PROCEDURE IF EXISTS `GSP_GR_MatchStart`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `GSP_GR_MatchStart`(
	wServerID		INT,		-- 房间标识		
	dwMatchID		INT,		-- 比赛标识
	dwMatchNo		INT,		-- 比赛场次
	cbMatchType	TINYINT		-- 比赛类型
)
BEGIN 

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `GSP_GR_RecordDrawInfo`
-- ----------------------------
DROP PROCEDURE IF EXISTS `GSP_GR_RecordDrawInfo`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `GSP_GR_RecordDrawInfo`(

	-- 房间信息
	wKindID INT,								-- 游戏 I D
	wServerID INT,								-- 房间 I D

	-- 桌子信息
	wTableID INT,								-- 桌子号码
	wUserCount INT,							-- 用户数目
	wAndroidCount INT,							-- 机器数目

	-- 税收损耗
	lWasteCount BIGINT,						-- 损耗数目
	lRevenueCount BIGINT,						-- 游戏税收

	-- 统计信息
	dwUserMemal BIGINT,						-- 损耗数目
	dwPlayTimeCount INT,						-- 游戏时间

	-- 时间信息
	SystemTimeStart DATETIME,					-- 开始时间
	SystemTimeConclude DATETIME				-- 结束时间

)
BEGIN

	-- 插入记录
	INSERT RecordDrawInfo(KindID,ServerID,TableID,UserCount,AndroidCount,Waste,Revenue,UserMedal,StartTime,ConcludeTime)
	VALUES (wKindID,wServerID,wTableID,wUserCount,wAndroidCount,lWasteCount,lRevenueCount,dwUserMemal,SystemTimeStart,SystemTimeConclude);
	
	-- 读取记录
	SELECT LAST_INSERT_ID() AS DrawID;

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `GSP_GR_RecordDrawScore`
-- ----------------------------
DROP PROCEDURE IF EXISTS `GSP_GR_RecordDrawScore`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `GSP_GR_RecordDrawScore`(

	-- 房间信息
	dwDrawID INT,								-- 局数标识
	dwUserID INT,								-- 用户标识
	wChairID INT,								-- 椅子号码

	-- 用户信息
	dwDBQuestID INT,							-- 请求标识
	dwInoutIndex INT,							-- 进出索引

	-- 成绩信息
	lScore BIGINT,								-- 用户积分
	lGrade BIGINT,								-- 用户成绩
	lRevenue BIGINT,							-- 用户税收
	dwUserMedal INT,							-- 奖牌数目
	dwPlayTimeCount INT						-- 游戏时间

)
BEGIN

	-- 插入记录
	INSERT RecordDrawScore(DrawID,UserID,ChairID,Score,Grade,Revenue,UserMedal,PlayTimeCount,DBQuestID,InoutIndex)
	VALUES (dwDrawID,dwUserID,wChairID,lScore,lGrade,lRevenue,dwUserMedal,dwPlayTimeCount,dwDBQuestID,dwInoutIndex);
	
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `GSP_GR_UnlockAndroidUser`
-- ----------------------------
DROP PROCEDURE IF EXISTS `GSP_GR_UnlockAndroidUser`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `GSP_GR_UnlockAndroidUser`(
	wServerID	SMALLINT,					-- 房间标识	
	OUT nRet INT
)
BEGIN
	
	-- 变量定义	
	DECLARE	return_value int;

	CALL QPAccountsDB.GSP_GR_UnlockAndroidUser(@wServerID, return_value);

	SET nRet = return_value;

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `GSP_GR_UserMatchFee`
-- ----------------------------
DROP PROCEDURE IF EXISTS `GSP_GR_UserMatchFee`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `GSP_GR_UserMatchFee`(
	dwUserID INT,								-- 用户 I D
	lMatchFee BIGINT,							-- 报名费用
	wKindID INT,								-- 游戏 I D
	wServerID INT,								-- 房间 I D
	strClientIP NVARCHAR(15),					-- 连接地址
	dwMatchID	INT,							-- 比赛 I D
	dwMatchNo	INT,							-- 比赛场次
	strMachineID NVARCHAR(32),					-- 机器标识
	OUT strErrorDescribe NVARCHAR(127),		-- 输出信息
	OUT nRet INT
)
label_pro:BEGIN	
	-- 比赛信息
	DECLARE cbMatchFeeType TINYINT;
	DECLARE cbMemberOrder TINYINT;
	DECLARE InitialScore BIGINT;
	
	-- 用户信息
	DECLARE IsAndroidUser TINYINT;
	DECLARE UserMemberOrder TINYINT;

	-- 查询金币
	DECLARE Score BIGINT;

	-- 查询元宝
	DECLARE wUserMedal BIGINT;
	
	-- 查询金币
	DECLARE CurrScore BIGINT;
	
	-- 查询元宝
	DECLARE CurrIngot BIGINT;
	
	SELECT cbMemberOrder=a.MemberOrder,cbMatchFeeType=a.MatchFeeType,InitialScore=b.InitialScore FROM MatchPublic a,MatchImmediate b
	WHERE a.MatchID=dwMatchID AND a.MatchID=b.MatchID;
	IF cbMemberOrder IS NULL OR cbMatchFeeType IS NULL THEN
		SET strErrorDescribe = N'抱歉,您报名的比赛不存在！';
		SET nRet = 1;
		leave label_pro;	
	END IF;
	
	-- 用户信息
	SELECT IsAndroidUser=IsAndroid,UserMemberOrder=MemberOrder FROM QPAccountsDB.AccountsInfo WHERE UserID=dwUserID;
	
	-- 用户校验
	IF UserMemberOrder IS NULL THEN
		SET strErrorDescribe = '抱歉,您的用户信息不存在，请联系客服中心！';
		SET nRet = 2;
		leave label_pro;			
	END IF;

	-- 机器过滤
	IF IsAndroidUser=0 THEN
		-- 报名条件
		IF UserMemberOrder<cbMemberOrder THEN
			SET strErrorDescribe = '抱歉,您的等级不够不符合报名条件！';
			SET nRet = 3;
			leave label_pro;
		END IF;

		-- 扣除费用
		IF lMatchFee>0 THEN
			-- 金币支付
			IF cbMatchFeeType=0 THEN
				-- 查询金币
				SELECT Score=Score FROM QPTreasureDB.GameScoreInfo WHERE UserID=dwUserID;
				IF Score IS NULL THEN
					SET strErrorDescribe = '抱歉,没有找到您的金币信息,请您与我们的客服人员联系！';
					SET nRet = 4;
					leave label_pro;
				END IF;

				-- 金币不足
				IF Score < lMatchFee THEN
					SET strErrorDescribe = '抱歉,您身上的金币不足,系统无法为您成功报名！';
					SET nRet = 5;
					leave label_pro;
				END IF;

				-- 更新金币
				UPDATE QPTreasureDB.GameScoreInfo SET Score=Score-lMatchFee WHERE UserID=dwUserID;
			END IF;

			-- 元宝支付
			IF cbMatchFeeType=1 THEN
				-- 查询元宝
				SELECT wUserMedal=UserMedal FROM QPAccountsDB.AccountsInfo WHERE UserID=dwUserID;
				IF wUserMedal IS NULL THEN
					SET strErrorDescribe = '抱歉,没有找到您的元宝信息,请您与我们的客服人员联系！';
					SET nRet = 4;
					leave label_pro;
				END	IF;

				-- 元宝不足
				IF wUserMedal < lMatchFee THEN
					SET strErrorDescribe = '抱歉,您身上的元宝不足,系统无法为您成功报名！';
					SET nRet = 5;
					leave label_pro;
				END IF;

				-- 更新元宝
				UPDATE QPAccountsDB.AccountsInfo SET UserMedal=wUserMedal-lMatchFee WHERE UserID=dwUserID;
			END IF;
		END IF;
	END IF;

	-- 更新分数
	IF exists(SELECT * FROM MatchScoreInfo WHERE UserID=dwUserID AND ServerID=wServerID AND MatchID=dwMatchID AND MatchNo=dwMatchNo) THEN
		UPDATE MatchScoreInfo SET Score=InitialScore,WinCount=0,LostCount=0,DrawCount=0,FleeCount=0,PlayTimeCount=0,OnlineTime=0,
		UserRight=0x10000000,SignupTime=NOW()
		WHERE UserID=dwUserID AND ServerID=wServerID AND MatchID=dwMatchID AND MatchNo=dwMatchNo;
	ELSE
		INSERT INTO MatchScoreInfo (UserID,ServerID,MatchID,MatchNo,Score,UserRight) 
		VALUES (dwUserID,wServerID,dwMatchID,dwMatchNo,InitialScore,0x10000000);	
	END	IF;

	-- 插入记录	
	IF IsAndroidUser=0 THEN
		INSERT StreamMatchFeeInfo (UserID,ServerID,MatchID,MatchNo,MatchType,MatchFeeType,MatchFee,CollectDate) 
		VALUES(dwUserID,wServerID,dwMatchID,dwMatchNo,1,cbMatchFeeType,lMatchFee,NOW());
	END IF;

	-- 查询金币
	SELECT CurrScore=Score FROM QPTreasureDB.GameScoreInfo WHERE UserID=dwUserID;	
	
	-- 查询元宝
	SELECT CurrIngot=UserMedal FROM QPAccountsDB.AccountsInfo WHERE UserID=dwUserID;
	
	-- 调整数据
	IF CurrScore IS NULL THEN
		SET CurrScore=0;
	END IF;
	IF CurrIngot IS NULL THEN
		SET CurrIngot=0;
	END IF;

	-- 抛出数据
	SELECT 	CurrScore AS Score,CurrIngot AS Ingot;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `GSP_GR_UserMatchQuit`
-- ----------------------------
DROP PROCEDURE IF EXISTS `GSP_GR_UserMatchQuit`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `GSP_GR_UserMatchQuit`(
	dwUserID INT,								-- 用户 I D
	lMatchFee BIGINT,							-- 报名费用
	wKindID INT,								-- 游戏 I D
	wServerID INT,								-- 房间 I D
	strClientIP NVARCHAR(15),					-- 连接地址
	dwMatchID	INT,							-- 比赛 I D
	dwMatchNo	INT,							-- 比赛场次
	strMachineID NVARCHAR(32),				-- 机器标识
	OUT strErrorDescribe NVARCHAR(127),		-- 输出信息
	OUT nRet INT
)
label_pro:BEGIN
	-- 比赛信息
	DECLARE cbMatchFeeType TINYINT;
	
	-- 用户信息
	DECLARE IsAndroidUser TINYINT;
	
	-- 查询金币
	DECLARE Score BIGINT;
	
	-- 查询元宝
	DECLARE UserMedal BIGINT;

	-- 查询金币
	DECLARE CurrScore BIGINT;
	
	-- 查询元宝
	DECLARE CurrIngot BIGINT;
	
	SELECT cbMatchFeeType=MatchFeeType FROM MatchPublic WHERE MatchID=dwMatchID;
	IF cbMatchFeeType IS NULL THEN
		SET strErrorDescribe = '抱歉,您报名的比赛不存在！';
		SET nRet = 1;
		leave label_pro;
	END IF;

	-- 用户信息
	SELECT IsAndroidUser=IsAndroid FROM QPAccountsDB.AccountsInfo WHERE UserID=dwUserID;

	-- 存在判断
	IF IsAndroidUser IS NULL THEN
		SET strErrorDescribe = '抱歉,您的用户信息不存在！';
		SET nRet = 2;
		leave label_pro;
	END IF;

	-- 返还费用
	IF IsAndroidUser=0 AND lMatchFee>0 THEN
		-- 金币支付
		IF cbMatchFeeType=0 THEN
			-- 查询金币
			SELECT Score=Score FROM QPTreasureDB.GameScoreInfo WHERE UserID=dwUserID;
			IF Score IS NULL THEN
				SET strErrorDescribe = '没有找到您的金币信息,请您与我们的客服人员联系！';
				SET nRet = 3;
				leave label_pro;					
			END IF;

			-- 更新金币
			UPDATE QPTreasureDB.GameScoreInfo SET Score=Score+lMatchFee WHERE UserID=dwUserID;
		END IF;

		-- 奖牌支付
		IF cbMatchFeeType=1 THEN
			-- 查询元宝
			SELECT UserMedal=UserMedal FROM QPAccountsDB.AccountsInfo WHERE UserID=dwUserID;
			IF UserMedal IS NULL THEN
				SET strErrorDescribe = '没有找到您的元宝信息,请您与我们的客服人员联系！';
				SET nRet = 4;
				leave label_pro;
			END	IF;

			-- 更新元宝
			UPDATE QPAccountsDB.AccountsInfo SET UserMedal=UserMedal+lMatchFee WHERE UserID=dwUserID;
		END IF;	
	END IF;

	-- 删除记录
	IF IsAndroidUser=0 THEN
		DELETE FROM StreamMatchFeeInfo WHERE UserID=dwUserID AND ServerID=wServerID AND MatchID=dwMatchID AND MatchNo=dwMatchNo;
	END IF;

	-- 查询金币
	SELECT CurrScore=Score FROM QPTreasureDB.GameScoreInfo WHERE UserID=dwUserID;
	
	-- 查询元宝
	SELECT CurrIngot=UserMedal FROM QPAccountsDB.AccountsInfo WHERE UserID=dwUserID;

	-- 调整数据
	IF CurrScore IS NULL THEN
		SET CurrScore=0;
	END IF;
	
	IF CurrIngot IS NULL THEN
		SET CurrIngot=0;
	END IF;

	-- 抛出数据
	SELECT 	CurrScore AS Score,CurrIngot AS Ingot;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `GSP_GR_WriteGameScore`
-- ----------------------------
DROP PROCEDURE IF EXISTS `GSP_GR_WriteGameScore`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `GSP_GR_WriteGameScore`(

	-- 系统信息
	dwUserID INT,								-- 用户 I D
	dwDBQuestID INT,							-- 请求标识
	dwInoutIndex INT,							-- 进出索引

	-- 变更成绩
	lVariationScore BIGINT,					-- 用户分数
	lVariationGrade BIGINT,					-- 用户成绩
	lVariationInsure BIGINT,					-- 用户银行
	lVariationRevenue BIGINT,					-- 游戏税收
	lVariationWinCount INT,					-- 胜利盘数
	lVariationLostCount INT,					-- 失败盘数
	lVariationDrawCount INT,					-- 和局盘数
	lVariationFleeCount INT,					-- 断线数目
	lVariationUserMedal INT,					-- 用户奖牌
	lVariationExperience INT,					-- 用户经验
	lVariationLoveLiness INT,					-- 用户魅力
	dwVariationPlayTimeCount INT,				-- 游戏时间

	-- 附件信息
	cbTaskForward TINYINT,						-- 任务跟进

	-- 比赛信息
	dwMatchID INT,							    -- 比赛 I D
	dwMatchNO INT,								-- 比赛场次

	-- 属性信息
	wKindID INT,								-- 游戏 I D
	wServerID INT,								-- 房间 I D
	strClientIP NVARCHAR(15)					-- 连接地址

)
BEGIN

	-- 比赛分数
	UPDATE MatchScoreInfo SET Score=Score+lVariationScore, WinCount=WinCount+lVariationWinCount, LostCount=LostCount+lVariationLostCount,
		DrawCount=DrawCount+lVariationDrawCount, FleeCount=FleeCount+lVariationFleeCount, PlayTimeCount=PlayTimeCount+dwVariationPlayTimeCount
	WHERE UserID=dwUserID AND ServerID=wServerID AND MatchID=dwMatchID AND MatchNo=dwMatchNo;

	-- 游戏时长
	UPDATE GameScoreInfo SET PlayTimeCount=PlayTimeCount+dwVariationPlayTimeCount WHERE UserID=dwUserID;

	-- 全局信息
	IF lVariationExperience>0 OR lVariationLoveLiness<>0 OR lVariationUserMedal>0 THEN
		UPDATE QPAccountsDB.AccountsInfo SET Experience=Experience+lVariationExperience, LoveLiness=LoveLiness+lVariationLoveLiness,
			UserMedal=UserMedal+lVariationUserMedal
		WHERE UserID=dwUserID;
	END IF;
	
	-- 任务跟进
--	IF cbTaskForward=1
--	BEGIN
--		call QPPlatformDB.GSP_GR_TaskForward(dwUserID,wKindID,0,lVariationWinCount,lVariationLostCount,lVariationDrawCount)
--	END

END
;;
DELIMITER ;
