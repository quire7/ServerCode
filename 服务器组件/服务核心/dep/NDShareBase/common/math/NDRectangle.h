/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\math\NDRectangle.h
	file base:	NDRectangle
	purpose:	rectangle;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_RECTANGLE_H__
#define __SHARE_BASE_ND_RECTANGLE_H__

#include "NDTypes.h"

_NDSHAREBASE_BEGIN

class NDRectangle
{
private:
	NDUint32	m_nWidth;
	NDUint32	m_nHeight;
public:
	explicit NDRectangle( NDUint32 nWidth, NDUint32 nHeight );

	NDRectangle();
	NDRectangle( const NDRectangle& other );
	NDRectangle& operator = ( const NDRectangle& other );

	bool operator  > ( const NDRectangle& other ) const;
	bool operator  < ( const NDRectangle& other ) const;
	bool operator == ( const NDRectangle& other ) const;
	
	void setWidth( NDUint32 nWidth );
	void setHeight( NDUint32 nHeight );
	void setWidthAndHeight( NDUint32 nWidth, NDUint32 nHeight );

	NDUint32 getWidth()		const;
	NDUint32 getHeight()	const;

	const NDUint32&	getConstRefWidth()	const;
	const NDUint32&	getConstRefHeight()	const;

};

_NDSHAREBASE_END
#endif
