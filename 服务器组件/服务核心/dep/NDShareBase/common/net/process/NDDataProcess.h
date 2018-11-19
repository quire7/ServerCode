/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\net\process\NDDataProcess.h
	file base:	NDDataProcess
	purpose:	data process;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef	__SHARE_BASE_ND_DATA_PROCESS_H__
#define __SHARE_BASE_ND_DATA_PROCESS_H__

#include <string>
using std::string;

#include <map>
using std::map;

#include "NDTypes.h"
#include "NDShareBaseMacros.h"

#include "NDProcess.h"


_NDSHAREBASE_BEGIN

struct NDProtocolHeader
{
	NDUint32 m_nSessionID;
	NDUint16 m_nProtocolID;
};

class NDProtocolCallBack
{
public:
	virtual ~NDProtocolCallBack(){}
	virtual NDBool process( NDIStream& rIStream, NDProtocolHeader& protocolHeader ) = 0;
};


class NDDataProcess : public NDProcess
{
public:

	NDDataProcess(void);
	virtual ~NDDataProcess(void);

	void registerCallBack( NDUint16 nProtocolID, NDProtocolCallBack *pNDCallBack );
	//just for show error start;
	void registerProtocolIDName( NDUint16 nProtocolID, string strProtocolIDName );
	//just for show error end;

	virtual NDBool process( NDIStream &rIStream );
	virtual NDBool process( NDIStream &rIStream, NDProtocolHeader& protocolHeader, NDBool bShowError = NDTrue );




private:
	NDDataProcess(const NDDataProcess& other);
	NDDataProcess& operator = (const NDDataProcess& other);

protected:
	typedef std::map< NDUint16, string >					ProtocolIDNameMap;
	typedef ProtocolIDNameMap::iterator						ProtocolIDNameMapIter;

	ProtocolIDNameMap										m_ProtocolIDNameMap;

private:
	typedef std::map< NDUint16, NDProtocolCallBack* >		CallBackMap;
	typedef CallBackMap::iterator							CallBackMapIter;

	CallBackMap												m_CallBackMap;
};

#ifndef NDRegisterCallBackMACRO
#define NDRegisterCallBackMACRO( pDProcess, nProtocolID, pNDCallBack )	\
		pDProcess->registerProtocolIDName( nProtocolID, #nProtocolID );	\
		pDProcess->registerCallBack( nProtocolID, pNDCallBack );
#endif


_NDSHAREBASE_END

#endif
