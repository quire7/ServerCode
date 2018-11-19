/********************************************************************
	created:	2010/04/27
	file base:	XmlConfigBase
	file ext:	h
	author:		quire
	
	purpose:	parse XML file config;
*********************************************************************/

#ifndef __SHARE_BASE_ND_XML_CONFIG_BASE_H__
#define __SHARE_BASE_ND_XML_CONFIG_BASE_H__

#include <vector>
using std::vector;

#include "NDTypes.h"

class TiXmlElement;

_NDSHAREBASE_BEGIN

class NDXmlConfigBase
{
public:
	virtual ~NDXmlConfigBase(void);

	NDBool readXmlConfig( const char *pFileName );

protected:
	NDXmlConfigBase(void);
	virtual NDBool readXmlConfigContent(const TiXmlElement *pRootElement) = 0;

protected:
	const char* getElement(const TiXmlElement *pXmlElement, const char* pszFlag);

	NDBool getElement(const TiXmlElement *pXmlElement, const char* pszFlag, NDInt32& rIntValue);
	NDBool getElement(const TiXmlElement *pXmlElement, const char* pszFlag, NDInt64& rLongValue);
	NDBool getElement(const TiXmlElement *pXmlElement, const char* pszFlag, NDFloat64& rDoubleValue);
	NDBool getElement(const TiXmlElement *pXmlElement, const char* pszFlag, vector<const char*>& rFlagNameVec, vector<const char*>& rContentVec);
	NDBool getElement(const TiXmlElement *pXmlElement, const char* pszFlag, vector<const char*>& rFlagNameVec, vector<NDInt32>& rIntVec);

	const char* getElement2(const TiXmlElement *pParentXmlElement, const char* pszFlag);

	NDBool getElement2(const TiXmlElement *pParentXmlElement, const char* pszFlag, NDInt32& rIntValue);
	NDBool getElement2(const TiXmlElement *pParentXmlElement, const char* pszFlag, NDInt64& rLongValue);
	NDBool getElement2(const TiXmlElement *pParentXmlElement, const char* pszFlag, NDFloat64& rDoubleValue);
	NDBool getElement2(const TiXmlElement *pParentXmlElement, const char* pszFlag, vector<const char*>& rFlagNameVec, vector<const char*>& rContentVec);
	NDBool getElement2(const TiXmlElement *pParentXmlElement, const char* pszFlag, vector<const char*>& rFlagNameVec, vector<NDInt32>& rIntVec);
};

_NDSHAREBASE_END

#endif

