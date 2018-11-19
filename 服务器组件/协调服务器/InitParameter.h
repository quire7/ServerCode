#ifndef INIR_PARAMETER_HEAD_FILE
#define INIR_PARAMETER_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////

//配置参数
class CInitParameter
{
	//系统配置
public:
	WORD							m_wMaxConnect;						//最大连接
	WORD							m_wServicePort;						//服务端口

	//配置信息
public:
	tagDataBaseParameter			m_AccountsDBParameter;				//连接地址
	tagDataBaseParameter			m_TreasureDBParameter;				//连接地址
	tagDataBaseParameter			m_PlatformDBParameter;				//连接地址

	tagRankingRewardEmailConfig		m_LoveLinessConfig;					//魅力排行
	tagRankingRewardEmailConfig		m_TreasureConfig;					//财富排行

	//函数定义
public:
	//构造函数
	CInitParameter();
	//析构函数
	virtual ~CInitParameter();

	//功能函数
public:
	//初始化
	VOID InitParameter();
	//加载配置
	VOID LoadInitParameter();
	//保存配置
	void SaveInitParameter();
};

//////////////////////////////////////////////////////////////////////////////////

#endif
