#ifndef INIR_PARAMETER_HEAD_FILE
#define INIR_PARAMETER_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////

//���ò���
class CInitParameter
{
	//ϵͳ����
public:
	WORD							m_wMaxConnect;						//�������
	WORD							m_wServicePort;						//����˿�

	//������Ϣ
public:
	tagDataBaseParameter			m_AccountsDBParameter;				//���ӵ�ַ
	tagDataBaseParameter			m_TreasureDBParameter;				//���ӵ�ַ
	tagDataBaseParameter			m_PlatformDBParameter;				//���ӵ�ַ

	tagRankingRewardEmailConfig		m_LoveLinessConfig;					//��������
	tagRankingRewardEmailConfig		m_TreasureConfig;					//�Ƹ�����

	//��������
public:
	//���캯��
	CInitParameter();
	//��������
	virtual ~CInitParameter();

	//���ܺ���
public:
	//��ʼ��
	VOID InitParameter();
	//��������
	VOID LoadInitParameter();
	//��������
	void SaveInitParameter();
};

//////////////////////////////////////////////////////////////////////////////////

#endif
