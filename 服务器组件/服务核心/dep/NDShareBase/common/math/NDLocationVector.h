/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\math\NDLocationVector.h
	file base:	NDLocationVector
	purpose:	location infor;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_LOCATION_VECTOR_H__
#define __SHARE_BASE_ND_LOCATION_VECTOR_H__
///////////////////////////////////////////////////////////
// Location vector class (X, Y, Z, O)
//////////////////////////////////////////////////////////
#include "NDTypes.h"

_NDSHAREBASE_BEGIN

template<typename T>
class NDLocationVector
{
public:
	// Constructors
	NDLocationVector(T X, T Y, T Z) : x(X), y(Y), z(Z), o(0) {}
	NDLocationVector(T X, T Y, T Z, T O) : x(X), y(Y), z(Z), o(O) {}
	NDLocationVector() : x(0), y(0), z(0), o(0) {}

	// (dx * dx + dy * dy + dz * dz)
	T DistanceSq(const NDLocationVector & comp)
	{
		T delta_x = comp.x - x;
		T delta_y = comp.y - y;
		T delta_z = comp.z - z;

		return (delta_x*delta_x + delta_y*delta_y + delta_z*delta_z);
	}

	T DistanceSq(const T &X, const T &Y, const T &Z)
	{
		T delta_x = X - x;
		T delta_y = Y - y;
		T delta_z = Z - z;

		return (delta_x*delta_x + delta_y*delta_y + delta_z*delta_z);
	}

	// sqrt(dx * dx + dy * dy + dz * dz)
	T Distance(const NDLocationVector & comp)
	{
		T delta_x = comp.x - x;
		T delta_y = comp.y - y;
		T delta_z = comp.z - z;

		return sqrtf(delta_x*delta_x + delta_y*delta_y + delta_z*delta_z);
	}

	T Distance(const T &X, const T &Y, const T &Z)
	{
		T delta_x = X - x;
		T delta_y = Y - y;
		T delta_z = Z - z;

		return sqrtf(delta_x*delta_x + delta_y*delta_y + delta_z*delta_z);
	}

	T Distance2DSq(const NDLocationVector & comp)
	{
		T delta_x = comp.x - x;
		T delta_y = comp.y - y;
		return (delta_x*delta_x + delta_y*delta_y);
	}

	T Distance2DSq(const T & X, const T & Y)
	{
		T delta_x = X - x;
		T delta_y = Y - y;
		return (delta_x*delta_x + delta_y*delta_y);
	}

	T Distance2D(NDLocationVector & comp)
	{
		T delta_x = comp.x - x;
		T delta_y = comp.y - y;
		return sqrtf(delta_x*delta_x + delta_y*delta_y);
	}

	T Distance2D(const T & X, const T & Y)
	{
		T delta_x = X - x;
		T delta_y = Y - y;
		return sqrtf(delta_x*delta_x + delta_y*delta_y);
	}

	// atan2(dx / dy)
	T CalcAngTo(const NDLocationVector & dest)
	{
		T dx = dest.x - x;
		T dy = dest.y - y;
		if(dy != 0.0f)
			return atan2(dy, dx);
		else 
			return 0.0f;
	}

	T CalcAngFrom(const NDLocationVector & src)
	{
		T dx = x - src.x;
		T dy = y - src.y;
		if(dy != 0.0f)
			return atan2(dy, dx);
		else
			return 0.0f;
	}

	void ChangeCoords(T X, T Y, T Z, T O)
	{
		x = X;
		y = Y;
		z = Z;
		o = O;
	}

	void ChangeCoords(T X, T Y, T Z)
	{
		x = X;
		y = Y;
		z = Z;
	}

	// add/subtract/equality vectors
	NDLocationVector & operator += (const NDLocationVector & add)
	{
		x += add.x;
		y += add.y;
		z += add.z;
		o += add.o;
		return *this;
	}

	NDLocationVector & operator -= (const NDLocationVector & sub)
	{
		x -= sub.x;
		y -= sub.y;
		z -= sub.z;
		o -= sub.o;
		return *this;
	}

	NDLocationVector & operator = (const NDLocationVector & eq)
	{
		x = eq.x;
		y = eq.y;
		z = eq.z;
		o = eq.o;
		return *this;
	}

	bool operator == (const NDLocationVector & eq)
	{
		if(eq.x == x && eq.y == y && eq.z == z)
			return true;
		else
			return false;
	}

	T x;
	T y;
	T z;
	T o;
};

_NDSHAREBASE_END
#endif
