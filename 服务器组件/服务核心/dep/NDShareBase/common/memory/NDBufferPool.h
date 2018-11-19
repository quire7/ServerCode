/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\memory\NDBufferPool.h
	file base:	NDBufferPool
	purpose:	buffer pool;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_BUFFER_POOL_H__
#define __SHARE_BASE_ND_BUFFER_POOL_H__

#include <string>
using std::string;

#include "NDTypes.h"
#include "thread/NDMutex.h"


_NDSHAREBASE_BEGIN

#define REALPTR_TO_USERPTR(x)		((NDUint8*)(x) + sizeof(NDUint32))
#define USERPTR_TO_REALPTR(x)		((NDUint8*)(x) - sizeof(NDUint32))
#define USERSIZE_TO_REALSIZE(x)     ((x) + sizeof(NDUint32))
#define REALSIZE_FROM_USERPTR(u)	(*(NDUint32*)USERPTR_TO_REALPTR(ptr) + sizeof(NDUint32))
#define USERSIZE_FROM_USERPTR(u)	(*(NDUint32*)USERPTR_TO_REALPTR(ptr))

//使用NDBufferPool时,要用指针即NEW出来;
//因为NDBufferPool对象太大,会导致栈溢出;
class NDBufferPool
{
public:

    /** Only store buffers up to these sizes (in bytes) in each pool->
        Different pools have different management strategies.

        A large block is preallocated for tiny buffers; they are used with
        tremendous frequency.  Other buffers are allocated as demanded.
        Tiny buffers are 128 bytes long because that seems to align well with
        cache sizes on many machines.
      */
    enum {tinyBufferSize = 128, smallBufferSize = 1024, medBufferSize = 4096};

    /** 
       Most buffers we're allowed to store.
       250000 * 128  = 32 MB (preallocated)
        10000 * 1024 = 10 MB (allocated on demand)
         1024 * 4096 =  4 MB (allocated on demand)
     */
    enum {maxTinyBuffers = 250000, maxSmallBuffers = 10000, maxMedBuffers = 1024};

public:

    /** Count of memory allocations that have occurred. */
    int totalMallocs;
    int mallocsFromTinyPool;
    int mallocsFromSmallPool;
    int mallocsFromMedPool;

    /** Amount of memory currently allocated (according to the application). 
        This does not count the memory still remaining in the buffer pool,
        but does count extra memory required for rounding off to the size
        of a buffer.
        Primarily useful for detecting leaks.*/
    // TODO: make me an atomic int!
    volatile int bytesAllocated;

private:

    /** Pointer given to the program.  Unless in the tiny heap, the user size of the block is stored right in front of the pointer as a uint32.*/
    typedef void* UserPtr;

    /** Actual block allocated on the heap */
    typedef void* RealPtr;

    class MemBlock {
    public:
        UserPtr     ptr;
        size_t      bytes;

        inline MemBlock() : ptr(NULL), bytes(0) {}
        inline MemBlock(UserPtr p, size_t b) : ptr(p), bytes(b) {}
    };

    MemBlock smallPool[maxSmallBuffers];
    int smallPoolSize;

    MemBlock medPool[maxMedBuffers];
    int medPoolSize;

    /** The tiny pool is a single block of storage into which all tiny
        objects are allocated.  This provides better locality for
        small objects and avoids the search time, since all tiny
        blocks are exactly the same size. */
    void* tinyPool[maxTinyBuffers];
    int tinyPoolSize;

    /** Pointer to the data in the tiny pool */
    void* tinyHeap;

    NDSpinlock     m_lock;

	char	m_szPerformanceBuf[128];
	char	m_szStatusBuf[128];
public:
	NDBufferPool();
	~NDBufferPool();

	void*	realloc(void* ptr, size_t bytes);

	void*	malloc(size_t bytes);

	void	free(void* ptr);

	string performance() const;
	string status() const;

private:
	/** 
     malloc out of the tiny heap. Returns NULL if allocation failed.
     */
    UserPtr tinyMalloc(size_t bytes);

	 /** Returns true if this is a pointer into the tiny heap. */
	bool	inTinyHeap(UserPtr ptr);

	void	tinyFree(UserPtr ptr);

	void	flushPool(MemBlock* pool, int& poolSize);

	/** Allocate out of a specific pool.  Return NULL if no suitable 
		memory was found. */
	UserPtr malloc(MemBlock* pool, int& poolSize, size_t bytes);

	/** Provide synchronization between threads */
	void	lock()		{ m_lock.lock(); };
	void	unlock()	{ m_lock.unlock(); };
};

_NDSHAREBASE_END
#endif

