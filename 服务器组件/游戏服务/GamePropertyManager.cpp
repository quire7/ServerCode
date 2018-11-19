#include "StdAfx.h"
#include "GamePropertyManager.h"

//////////////////////////////////////////////////////////////////////////////////

CGamePropertyManager* CGamePropertyManager::m_pInstance = NULL;

//���캯��
CGamePropertyManager::CGamePropertyManager()
{
	ASSERT(m_pInstance == NULL);
	m_pInstance = this;
}

//��������
CGamePropertyManager::~CGamePropertyManager()
{
	if (m_pInstance == this)
	{
		m_pInstance = NULL;
	}
}

//���õ���
bool CGamePropertyManager::SetGamePropertyInfo(tagPropertyInfo PropertyInfo[], WORD wPropertyCount)
{
	//���ñ���
	m_PropertyInfoArray.SetSize(wPropertyCount);

	//��������
	CopyMemory(m_PropertyInfoArray.GetData(),PropertyInfo,sizeof(tagPropertyInfo)*wPropertyCount);

	return true;
}

//���ҵ���
tagPropertyInfo * CGamePropertyManager::SearchPropertyItem(WORD wPropertyIndex)
{
	//���ҵ���
	for (INT_PTR i=0;i<m_PropertyInfoArray.GetCount();i++)
	{
		if (m_PropertyInfoArray[i].wIndex==wPropertyIndex)
		{
			return &m_PropertyInfoArray[i];
		}
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////////////
