#ifndef GAME_PROPERTY_MANAGER_HEAD_FILE
#define GAME_PROPERTY_MANAGER_HEAD_FILE

#pragma once

#include "GameServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////

//���Ͷ���
typedef CYSArray<tagPropertyInfo>	CPropertyInfoArray;					//��������

//////////////////////////////////////////////////////////////////////////////////

//���߹���
class GAME_SERVICE_CLASS CGamePropertyManager
{
public:
	static CGamePropertyManager *GetInstance(void){ return m_pInstance; }

private:
	static CGamePropertyManager				*m_pInstance;								//ʵ��ָ��

	//��������
protected:
	CPropertyInfoArray				m_PropertyInfoArray;				//��������

	//��������
public:
	//���캯��
	CGamePropertyManager();
	//��������
	virtual ~CGamePropertyManager();

	//���ú���
public:
	//���õ���
	bool SetGamePropertyInfo(tagPropertyInfo PropertyInfo[], WORD wPropertyCount);

	//���ܺ���
public:
	//���ҵ���
	tagPropertyInfo * SearchPropertyItem(WORD wPropertyIndex);
};

//////////////////////////////////////////////////////////////////////////////////

#endif