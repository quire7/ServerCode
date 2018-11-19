/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\authentic\NDCrypt.h
	file base:	NDCrypt
	purpose:	data encrypt and decrypt;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_CRYPT_H__
#define __SHARE_BASE_ND_CRYPT_H__

#include <vector>
using std::vector;

#include "NDTypes.h"

_NDSHAREBASE_BEGIN

class NDCrypt
{
public:
	NDCrypt();
	~NDCrypt();
	
	void init();
	void setKey( const NDUint8* szKey, NDUint32 nLen );
	
	void encryptSend( NDUint8* szData, NDUint32 nLen );
	void decryptRecv( NDUint8* szData, NDUint32 nLen );


private:

	NDUint32 setDefaultKey();

private:

	NDUint8		nSendI;
	NDUint8		nSendJ;
	NDUint8		nRecvI;
	NDUint8		nRecvJ;

	vector<NDUint8>	keyVec;
};

_NDSHAREBASE_END
#endif
