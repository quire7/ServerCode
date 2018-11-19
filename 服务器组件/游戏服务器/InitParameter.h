#ifndef INIR_PARAMETER_HEAD_FILE
#define INIR_PARAMETER_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////

//���ò���
class CInitParameter
{
	//������Ϣ
public:
	WORD							m_wConnectTime;						//����ʱ��
	WORD							m_wCollectTime;						//ͳ��ʱ��

	//Э����Ϣ
public:
	WORD							m_wCorrespondPort;					//Э���˿�
	tagAddressInfo					m_CorrespondAddress;				//Э����ַ
	DWORD							m_dwRefrashOnlineUserTime;			//ˢ����������ʱ��

	//������Ϣ
public:
	tagAddressInfo					m_ServiceAddress;					//�����ַ
	tagDataBaseParameter			m_TreasureDBParameter;				//���ӵ�ַ
	tagDataBaseParameter			m_PlatformDBParameter;				//���ӵ�ַ
	tagDataBaseParameter			m_AccountsDBParameter;				//���Ӳ���
	tagBenefitInfo					m_BenefitInfo;						//�ȼý�����

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
};

//////////////////////////////////////////////////////////////////////////////////

#endif
