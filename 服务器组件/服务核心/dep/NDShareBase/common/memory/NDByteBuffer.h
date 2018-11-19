/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\memory\NDByteBuffer.h
	file base:	NDByteBuffer
	purpose:	byte buffer;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef _SHARE_BASE_ND_BYTE_BUFFER_H_
#define _SHARE_BASE_ND_BYTE_BUFFER_H_

#include "NDTypes.h"
#include "NDShareBaseMacros.h"

_NDSHAREBASE_BEGIN

class NDByteBufferImpl;
class NDByteBuffer
{
private:
	NDUint32			m_nPoolID;
	NDByteBufferImpl*	m_pNDByteBufferImpl;

public:
	explicit NDByteBuffer( NDUint32 nBufSize = MAX_MSGBUF_SIZE );
	NDByteBuffer( const NDByteBuffer& other );
	~NDByteBuffer(void);

	//clear NDMsgBuffer structure for reuse;
	void		release();

	char*		readBuffer();
	NDUint32	setReadBufSize( NDUint32 nSize );
	NDUint32	setWriteBufSize( NDUint32 nSize );

	void		setRollBackFlag();
	void		rollBack();
	void		cancelRollBack();

	//这个函数要慎用,当要读取的数据不足nSize时,有多少读多少;
	NDUint32	readBuffer( char *pszBuf, NDUint32 nSize );
	NDUint32	writeBuffer( const char *pszBuf, NDUint32 nSize );
	NDUint32	writeBuffer( const char *pszBuf );
	NDUint32	writeBuffer( const NDByteBuffer& refMsgBuf );
	NDUint32	writeBuffer( const NDByteBuffer& refMsgBuf, NDUint32 nSize );

	//ReadIntactBuffer是完整的读取nSize个字节的数据,如果数据不足,就不读;
	NDBool		readIntactBuffer( char *pszBuf, NDUint32 nSize );
	//WriteIntactBuffer是完整的写入nSize个字节的数据;
	NDBool		writeIntactBuffer( const char *pszBuf, NDUint32 nSize );

	NDBool		readInt8( NDInt8 &refInt8 );
	NDBool		readInt16( NDInt16 &refInt16 );
	NDBool		readInt32( NDInt32 &refInt32 );
	NDBool		readInt64( NDInt64 &refInt64 );

	NDBool		readUint8( NDUint8 &refUint8 );
	NDBool		readUint16( NDUint16 &refUint16 );
	NDBool		readUint32( NDUint32 &refUint32 );
	NDBool		readUint64( NDUint64 &refUint64 );

	NDBool		readFloat32( NDFloat32 &refFloat32 );
	NDBool		readFloat64( NDFloat64 &refFloat64 );

	NDBool		writeInt8( NDInt8 nInt8 );
	NDBool		writeInt16( NDInt16 nInt16 );
	NDBool		writeInt32( NDInt32 nInt32 );
	NDBool		writeInt64( NDInt64 nInt64 );

	NDBool		writeUint8( NDUint8 nUint8 );
	NDBool		writeUint16( NDUint16 nUint16 );
	NDBool		writeUint32( NDUint32 nUint32 );
	NDBool		writeUint64( NDUint64 nUint64 );

	NDBool		writeFloat32( NDFloat32 fFloat32 );
	NDBool		writeFloat64( NDFloat64 fFloat64 );


	//resume assign buffer size
	NDBool		reSet( NDUint32 nSize );
	
	NDUint32	getCapacitySize()	const;
	NDUint32	getSpaceSize()		const;
	NDUint32	getDataSize()		const;

	NDByteBuffer& operator = ( const NDByteBuffer& other );

	//在NDObjectPool中的位置，用以NDObjectPool的优化
	void		setPoolID( NDUint32 nPoolID );
	NDUint32	getPoolID()const;

};

_NDSHAREBASE_END
#endif
