#ifndef __SHARE_BASE_ND_SHARE_BASE_MACROS_H__
#define __SHARE_BASE_ND_SHARE_BASE_MACROS_H__

#if defined( __WIN32__ ) || defined( WIN32 ) || defined( _WIN32 )

#pragma warning(disable:4127)

#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef SAFE_FREE
#define SAFE_FREE(P)	do { if ( NULL != (P) ) { free(P); (P) = NULL; } } while (NULL);
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(P)	do { if ( NULL != (P) ) { delete (P); (P) = NULL; } } while (NULL);
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(P)	do { if ( NULL != (P) ) { delete [](P); (P) = NULL; } } while (NULL);
#endif

#ifndef ND_ALIGN
#define ND_ALIGN( nSize, nBase)	\
		(((nSize) + ((nBase) - 1)) & ~((nBase) - 1))
#endif

//无效的ID值;
#ifndef ND_INVALID_ID
#define ND_INVALID_ID		( -1 )
#endif

#ifndef MSGBUF_POOL_SIZE
#define MSGBUF_POOL_SIZE	( 1000 )
#endif

//最大消息包长度60k=(60*1024字节);
#ifndef MAX_MSGPACKET_SIZE
#define MAX_MSGPACKET_SIZE	( 61440 )
#endif

#ifndef MAX_MSGBUF_SIZE
#define MAX_MSGBUF_SIZE		( 4096 )
#endif


#ifndef MAX_RECV_LENGTH
#define	MAX_RECV_LENGTH		( 8192 )
#endif


/** The length of a Huge string */
#ifndef HUGE_STRING_LEN
#define HUGE_STRING_LEN		( 8192 )
#endif

/** The default string length */
#ifndef MAX_STRING_LEN
#define MAX_STRING_LEN		HUGE_STRING_LEN
#endif

#ifndef BUF_LEN_32
#define BUF_LEN_32			( 32 )
#endif

#ifndef BUF_LEN_64
#define BUF_LEN_64			( 64 )
#endif

#ifndef BUF_LEN_128
#define BUF_LEN_128			( 128 )
#endif

#ifndef BUF_LEN_256
#define BUF_LEN_256			( 256 )
#endif

#ifndef BUF_LEN_512
#define BUF_LEN_512			( 512 )
#endif

#ifndef BUF_LEN_1024
#define BUF_LEN_1024		( 1024 )
#endif

#ifndef MAX_SERVER_NAME
#define MAX_SERVER_NAME		( 32 )
#endif

#ifndef MAX_PATH_LEN
#define MAX_PATH_LEN		( 256 )
#endif

#ifndef CTYPE_NUM_CHARS
#define CTYPE_NUM_CHARS		( 256 )
#endif

// ip default length(ipv4: 16个字符. ipv6:还不知道.);
#ifndef ND_IP_LENGTH
#define ND_IP_LENGTH		( 32 )
#endif

#ifndef ND_TIME_LENGTH
#define ND_TIME_LENGTH		( 32 )	//时间字符串最好定义成的字节;
#endif

#ifndef ND_TIME_STRLEN_MIN
#define ND_TIME_STRLEN_MIN	( 20 )	//时间字符串最少需要的字节;
#endif


//SHA1 table length
#ifndef ND_SHA1_LEN
#define ND_SHA1_LEN								(20)
#endif

//MD5 table length (MD5_DIGEST_LENGTH has defined in <openssl/md5.h>)
#ifndef ND_MD5_DIGEST_LENGTH
#define ND_MD5_DIGEST_LENGTH					(16)
#endif


//log file max size;
#ifndef DEFAULT_LOG_FILE_MAX_SIZE
#define DEFAULT_LOG_FILE_MAX_SIZE				(104857600)		//100M(1024*1024*100);
#endif

//log cache size;
#ifndef DEFAULT_LOG_CACHE_SIZE
#define DEFAULT_LOG_CACHE_SIZE					(4194304)		//4M(1024*1024*4);
#endif

//log sharememory unit size;
#ifndef ND_LOG_SMU_MAX
#define ND_LOG_SMU_MAX							(200)		
#endif

//WORLD(CENTERSERVER) NAME length;
#ifndef ND_WORLDNAME_LEN
#define ND_WORLDNAME_LEN						(32)
#endif

#ifndef ND_CMD_TIMER_NOTIFY_MILLISECONDS
#define ND_CMD_TIMER_NOTIFY_MILLISECONDS		(500)			//500 milli seconds;
#endif


#ifndef ND_CHECK_SESSION_MILLISECONDS
#define ND_CHECK_SESSION_MILLISECONDS			(60000)			//60000 milli seconds;
#endif

//check session's heart packet;
#ifndef ND_PING_MILLISECONDS
#define ND_PING_MILLISECONDS					(30000)			//30000 milli seconds;
#endif

//main loop heartbeat milli seconds;
#ifndef ND_MAIN_PROC_MILLISECONDS
#define ND_MAIN_PROC_MILLISECONDS				(100)			//100 milli seconds;
#endif

#ifndef ND_SMU_LOCK_LOOP_MAX
#define ND_SMU_LOCK_LOOP_MAX					(10)			//获得Sharememory单元轮询最大次数;
#endif

//打印NDMemoryPoolExManager内存情况,时间为600s;
#ifndef ND_PRINT_OBJMEMEX_INFO_MILLISECONDS
#define ND_PRINT_OBJMEMEX_INFO_MILLISECONDS		(600000)		//600 second;
#endif


#ifndef ND_THREAD_NAME_MAX
#define ND_THREAD_NAME_MAX						(64)			//thread name max length;
#endif


#ifndef APLOG_MARK
#define APLOG_MARK	__FILE__,__LINE__

#define	APLOG_EMERG		0	/* system is unusable */
#define	APLOG_ALERT		1	/* action must be taken immediately */
#define	APLOG_CRIT		2	/* critical conditions */
#define	APLOG_ERR		3	/* error conditions */
#define	APLOG_WARNING	4	/* warning conditions */
#define	APLOG_NOTICE	5	/* normal but significant condition */
#define	APLOG_INFO		6	/* informational */
#define	APLOG_DEBUG		7	/* debug-level messages */

#define	APLOG_LEVELMASK	7	/* mask off the level value */

#endif


#ifdef	WIN32
#define ND_INLINE __forceinline
#else
#define ND_INLINE inline
#endif


#if defined(__cplusplus)

#define ND_STATIC_CAST(__type, __ptr)      static_cast< __type >(__ptr)
#define ND_CONST_CAST(__type, __ptr)       const_cast< __type >(__ptr)

#define ND_REINTERPRET_POINTER_CAST(__type, __ptr)    reinterpret_cast< __type >(__ptr)
#define ND_REINTERPRET_NONPOINTER_CAST(__type, __obj) reinterpret_cast< __type >(__obj)
#define ND_REINTERPRET_CAST(__type, __expr)           reinterpret_cast< __type >(__expr)

#else

#define ND_STATIC_CAST(__type, __ptr)      ((__type)(__ptr))
#define ND_CONST_CAST(__type, __ptr)       ((__type)(__ptr))

#define ND_REINTERPRET_POINTER_CAST(__type, __ptr)     ((__type)((void*)(__ptr)))
#define ND_REINTERPRET_NONPOINTER_CAST(__type, __obj)  ((__type)(__obj))

  /* Note: the following is only appropriate for pointers. */
#define ND_REINTERPRET_CAST(__type, __expr)            ND_REINTERPRET_POINTER_CAST(__type, __expr)
  /*
    Why cast to a |void*| first?  Well, when old-style casting from
    a pointer to a base to a pointer to a derived class, the cast will be
    ambiguous if the source pointer type appears multiple times in the
    destination, e.g.,
    
      class Base {};
      class Derived : public Base, public Base {};
      
      void foo( Base* b )
        {
          ((Derived*)b)->some_derived_member ... // Error: Ambiguous, expand from which |Base|?
        }

    an old-style cast (like |static_cast|) will change the pointer, but
    here, doesn't know how.  The cast to |void*| prevents it from thinking
    it needs to expand the original pointer.

    The cost is, |NS_REINTERPRET_CAST| is no longer appropriate for non-pointer
    conversions.  Also, mis-applying |NS_REINTERPRET_CAST| to cast |this| to something
    will still expand the pointer to the outer object in standards complying compilers.
  */

  /*
    No sense in making an NS_DYNAMIC_CAST() macro: you can't duplicate
    the semantics. So if you want to dynamic_cast, then just use it
    "straight", no macro.
  */
#endif



// checked_cast - Uses fast static_cast in Release build,
// but checks cast with an ASSERT in Debug.
//
// Typical usage:
//   class Foo { /* ... */  };
//   class Bar : public Foo { /* ... */  };
//   Foo * pFoo = new Bar;
//   Bar * pBar = checked_cast<Bar *>(pFoo);
template < class TypeTo, class TypeFrom >                                    
TypeTo checked_cast(TypeFrom p)                                             
{
	assert(dynamic_cast<TypeTo>(p));
	return static_cast<TypeTo>(p);
}

// this macro is look for dump's stack content in Debug.
//
// Typical usage:
//	NDBool longFunction()
//	{
//		ND_RETURN_MONITOR
//		//function body
//	}
#ifdef	_DEBUG
class NDReturnMonitor
{
public:
	~NDReturnMonitor()
	{
	}
};
#define	ND_RETURN_MONITOR	NDReturnMonitor monitor;
#else
#define ND_RETURN_MONITOR
#endif

#ifndef WIN32

#ifndef HANDLE
typedef void*	HANDLE;
#endif

#endif

#ifndef NDPTHREADFUN
typedef void (*NDPTHREADFUN)(void *);
#endif


#ifndef FILELINE
#define FILELINE		__FILE__,__LINE__
#endif

#ifdef WIN32
#define FUNC_NAME		__FUNCTION__
#define FILELINE_FUNC	__FILE__,__LINE__,__FUNCTION__
#define NEWLINE_SYMBOL	"\r\n"
#else
#define FUNC_NAME		__PRETTY_FUNCTION__
#define FILELINE_FUNC	__FILE__,__LINE__,__PRETTY_FUNCTION__
#define NEWLINE_SYMBOL	"\n"
#endif




#endif


