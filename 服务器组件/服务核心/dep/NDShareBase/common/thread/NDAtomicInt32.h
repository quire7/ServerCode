/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\thread\NDAtomicInt32.h
	file base:	NDAtomicInt32
	purpose:	atomic int32;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_ATOMIC_INT32_H__
#define __SHARE_BASE_ND_ATOMIC_INT32_H__

#if defined(WIN32)

#ifndef	WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#endif

#if defined(OSX)
#   include <libkern/OSAtomic.h>
#endif

_NDSHAREBASE_BEGIN
/**
 An integer that may safely be used on different threads without
 external locking.

 On Win32, Linux, FreeBSD, and Mac OS X this is implemented without locks.  

 <B>BETA API</B>  This is unsupported and may change
 */
class NDAtomicInt32 {
private:
#   if defined(WIN32)
    volatile long           m_value;
#   elif defined(OSX)
    int32_t                 m_value;
#   else
    volatile int			m_value;
#   endif


public:

    /** Initial value is undefined. */
    NDAtomicInt32() {}

    /** Atomic set */
    explicit NDAtomicInt32(const int x) {
        m_value = x;
    }

    /** Atomic set */
    NDAtomicInt32(const NDAtomicInt32& x) {
        m_value = x.m_value;
    }

    /** Atomic set */
    const NDAtomicInt32& operator=(const int x) {
        m_value = x;
        return *this;
    }

    /** Atomic set */
    void operator=(const NDAtomicInt32& x) {
        m_value = x.m_value;
    }

    /** Returns the current value */
    int value() const {
        return m_value;
    }

    /** Returns the old value, before the add. */
    int add(const int x) {
#       if defined(WIN32)

            return InterlockedExchangeAdd(&m_value, x);

#       elif defined(LINUX) || defined(FREEBSD)

            NDInt32 old;
            asm volatile ("lock; xaddl %0,%1"
                  : "=r"(old), "=m"(m_value) /* outputs */
                  : "0"(x), "m"(m_value)   /* inputs */
                  : "memory", "cc");
            return old;
            
#       elif defined(OSX)

            int32 old = m_value;
            OSAtomicAdd32(x, &m_value);
            return old;

#       endif
    }

    /** Returns old value. */
    int sub(const int x) {
        return add(-x);
    }

    void increment() {
#       if defined(WIN32)
            // Note: returns the newly incremented value
            InterlockedIncrement(&m_value);
#       elif defined(LINUX) || defined(FREEBSD)
            add(1);
#       elif defined(OSX)
            // Note: returns the newly incremented value
            OSAtomicIncrement32(&m_value);
#       endif
    }

    /** Returns zero if the result is zero after decrement, non-zero otherwise.*/
    int decrement() {
#       if defined(WIN32)
            // Note: returns the newly decremented value
            return InterlockedDecrement(&m_value);
#       elif defined(LINUX)  || defined(FREEBSD)
            unsigned char nz;

            asm volatile ("lock; decl %1;\n\t"
                          "setnz %%al"
                          : "=a" (nz)
                          : "m" (m_value)
                          : "memory", "cc");
            return nz;
#       elif defined(OSX)
            // Note: returns the newly decremented value
            return OSAtomicDecrement32(&m_value);
#       endif
    }


    /** Atomic test-and-set:  if <code>*this == comperand</code> then <code>*this := exchange</code> else do nothing.
        In both cases, returns the old value of <code>*this</code>.
    
        Performs an atomic comparison of this with the Comperand value. 
        If this is equal to the Comperand value, the Exchange value is stored in this.
        Otherwise, no operation is performed.

        Under VC6 the sign bit may be lost.
     */ 
    int compareAndSet(const int comperand, const int exchange) {
#       if defined(WIN32)
            return InterlockedCompareExchange(&m_value, exchange, comperand);
#       elif defined(LINUX) || defined(FREEBSD) || defined(OSX)
            // Based on Apache Portable Runtime
            // http://koders.com/c/fid3B6631EE94542CDBAA03E822CA780CBA1B024822.aspx
            int ret;
            asm volatile ("lock; cmpxchgl %1, %2"
                          : "=a" (ret)
                          : "r" (exchange), "m" (m_value), "0"(comperand)
                          : "memory", "cc");
            return ret;

            // Note that OSAtomicCompareAndSwap32 does not return a useful value for us
            // so it can't satisfy the cmpxchgl contract.
#       endif
    }

};

_NDSHAREBASE_END
#endif


