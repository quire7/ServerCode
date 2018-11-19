/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\stl\NDSortedVector.h
	file base:	NDSortedVector
	purpose:	sorted vector,malloc enough size before used;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_SORTED_VECTOR_H__
#define __SHARE_BASE_ND_SORTED_VECTOR_H__

#include <vector>
using std::vector;

#include <algorithm>
#include <functional>

_NDSHAREBASE_BEGIN

template<class T, class C= std::less<T>  >
class NDSortedVector
{
public:
	typedef std::vector< T >			Impl;
	typedef	C							value_compare;

	typedef typename Impl::size_type				size_type;
	typedef typename Impl::difference_type 			difference_type;
	typedef typename Impl::reference 				reference;
	typedef typename Impl::const_reference 			const_reference;
	typedef typename Impl::iterator 				iterator;
	typedef typename Impl::const_iterator 			const_iterator;
	typedef typename Impl::reverse_iterator 		reverse_iterator;
	typedef typename Impl::const_reverse_iterator 	const_reverse_iterator;
	typedef typename Impl::pointer			 		pointer;
	typedef typename Impl::const_pointer		 	const_pointer;

	iterator		begin()			{ return m_Impl.begin(); }
	const_iterator	begin() const	{ return m_Impl.begin(); }
	iterator		end()			{ return m_Impl.end(); }
	const_iterator	end()	const	{ return m_Impl.end(); }

	bool			empty()	const	{ return m_Impl.empty(); }
	void			clear()			{ return m_Impl.clear(); }

	size_type		size()  const	{ return m_Impl.size(); }
	size_type		capacity() const{ return m_Impl.capacity(); }

	void			reserve( size_type nCount )			{ return m_Impl.reserve( nCount ); }

	reference		operator[]( size_type nPos ) 		{ return m_Impl[nPos]; }
	const_reference	operator[]( size_type nPos ) const	{ return m_Impl[nPos]; }

	iterator		erase( iterator it )					{ return m_Impl.erase( it ); }
	iterator		erase( iterator first, iterator last )	{ return m_Impl.erase( first, last ); }	
	iterator		erase( const T& refKey )
	{
		iterator lb = std::lower_bound( m_Impl.begin(), m_Impl.end(), refKey, C() );
		iterator ub = std::upper_bound( m_Impl.begin(), m_Impl.end(), refKey, C() );
		return m_Impl.erase( lb, ub );
	}

	iterator		find( const T& refKey )
	{
		iterator iterFind = std::lower_bound( m_Impl.begin(), m_Impl.end(), refKey, C() );
		if ( iterFind == m_Impl.end() || not_equals( refKey, *iterFind) )
		{
			return m_Impl.end();
		}
		return iterFind;
	}

	const_iterator	find( const T& refKey ) const
	{
		const_iterator	b = m_Impl.begin();
		const_iterator	e = m_Impl.end();
		const_iterator	iterFind = std::lower_bound( b, e, refKey, C() ); //bug in STLport;
		//iterator iterFind = std::lower_bound( m_Impl.begin(), m_Impl.end(), refKey, C() );
		if ( iterFind == m_Impl.end() || not_equals( refKey, *iterFind) )
		{
			return m_Impl.end();
		}
		return iterFind;
	}

	//vector中的位置(从0开始,-1表示未找到);
	size_type		findIndex( const T& refKey ) const
	{
		const_iterator fIter = find( refKey );
		if ( fIter != m_Impl.end() )
		{
			return fIter - m_Impl.begin();
		}

		return -1;
	}

	std::pair<iterator, bool> insert( const T& refKey, bool bUnique=true )
	{
		std::pair<iterator, bool> p;
		p.first = std::lower_bound( m_Impl.begin(), m_Impl.end(), refKey, C() );

		if ( bUnique )
		{
			p.second = ( ( p.first == m_Impl.end() ) || not_equals( refKey, *p.first) );
		}
		else
		{
			p.second = true;
		}

		if ( p.second )
		{
			ptrdiff_t d = p.first - m_Impl.begin();
			size_type nCapacity = capacity();
			size_type nSize		= size();
			if ( nSize >= nCapacity )
			{
				++nCapacity;
				nCapacity = ( ( (nCapacity) + ((4) - 1) ) & ~ ((4) - 1) ); //容量是4的倍数;
				m_Impl.reserve( nCapacity );
			}
			p.first = m_Impl.begin() + d;
			m_Impl.insert( p.first, refKey );
		}

		return p;
	}

	iterator lower_bound( const T& refKey )
	{
		return std::lower_bound( m_Impl.begin(), m_Impl.end(), refKey, C() );
	}

	iterator upper_bound( const T& refKey )
	{
		return std::upper_bound( m_Impl.begin(), m_Impl.end(), refKey, C() );
	}

	const_iterator lower_bound( const T& refKey ) const
	{
		const_iterator	b = m_Impl.begin();
		const_iterator	e = m_Impl.end();
		return std::lower_bound( b, e, refKey, C() );
	}

	const_iterator upper_bound( const T& refKey ) const
	{
		const_iterator	b = m_Impl.begin();
		const_iterator	e = m_Impl.end();
		return std::upper_bound( b, e, refKey, C() );
	}

	std::pair<iterator, iterator> equal_range( const T& refKey )
	{
		return std::make_pair( lower_bound(refKey), upper_bound(refKey) );
	}

	value_compare	value_comp() const { return value_compare(); }

protected:
	bool			not_equals( const T& pLHS, const T& pRHS ) const
	{
		C comp;
		return comp( pLHS, pRHS ) || comp( pRHS, pLHS );
	}

private:
	Impl	m_Impl;
};


_NDSHAREBASE_END
#endif


//class NDClassTest
//{
//public:
//	NDClassTest( int nKey, int nValue ):m_nKey(nKey),m_nValue(nValue){};
//	friend bool operator < ( const NDClassTest& lhs, const NDClassTest& rhs )
//	{
//		return lhs.m_nKey < rhs.m_nKey;
//	}
//public:
//	int m_nKey;
//	int m_nValue;
//};
//
//NDSortedVector<NDClassTest> sortedVec;
//
//NDClassTest NDClassTest7( 7, 8 );
//sortedVec.insert( NDClassTest7 );
//NDClassTest NDClassTest6( 6, 7 );
//sortedVec.insert( NDClassTest6 );
//NDClassTest NDClassTest5( 5, 6 );
//sortedVec.insert( NDClassTest5 );
//NDClassTest NDClassTest4( 4, 5 );
//sortedVec.insert( NDClassTest4 );
//NDClassTest NDClassTest3( 3, 4 );
//sortedVec.insert( NDClassTest3 );
//NDClassTest NDClassTest2( 2, 3 );
//sortedVec.insert( NDClassTest2 );
//NDClassTest NDClassTest1( 1, 2 );
//sortedVec.insert( NDClassTest1 );
//
//for ( int i = 0 ; i < (int)sortedVec.size(); ++i )
//{
//	NDClassTest& refData = sortedVec[i];
//	std::cout << refData.m_nKey << "  " << refData.m_nValue << std::endl;
//}
//
//NDSortedVector<NDClassTest>::iterator it = sortedVec.find( NDClassTest3 );
//
//int nIndex = (int)sortedVec.findIndex( NDClassTest1 );
//std::cout << nIndex << std::endl;



