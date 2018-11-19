#ifndef __SHARE_BASE_ND_CPU_CFG_H__
#define __SHARE_BASE_ND_CPU_CFG_H__

//编译时一定要注意LONG是4个字节,还是8字节(sizeof(long));

#ifndef HAVE_LONG_LONG
#define	HAVE_LONG_LONG	1
#endif

#ifndef ND_AF_INET6
#define ND_AF_INET6 30  /* same as AF_INET6 */
#endif


#ifndef ND_BYTES_PER_BYTE
#define ND_BYTES_PER_BYTE   1L
#endif
#ifndef ND_BYTES_PER_SHORT
#define ND_BYTES_PER_SHORT  2L
#endif
#ifndef ND_BYTES_PER_INT
#define ND_BYTES_PER_INT    4L
#endif
#ifndef ND_BYTES_PER_INT64
#define ND_BYTES_PER_INT64  8L
#endif
#ifndef ND_BYTES_PER_LONG
#define ND_BYTES_PER_LONG   4L
#endif
//#ifndef ND_BYTES_PER_LONG
//#define ND_BYTES_PER_LONG   8L
//#endif
#ifndef ND_BYTES_PER_FLOAT
#define ND_BYTES_PER_FLOAT  4L
#endif
#ifndef ND_BYTES_PER_DOUBLE
#define ND_BYTES_PER_DOUBLE 8L
#endif
#ifndef ND_BYTES_PER_WORD
#define ND_BYTES_PER_WORD   4L
#endif
#ifndef ND_BYTES_PER_DWORD
#define ND_BYTES_PER_DWORD  8L
#endif


#ifndef ND_BITS_PER_BYTE
#define ND_BITS_PER_BYTE    8L
#endif
#ifndef ND_BITS_PER_SHORT
#define ND_BITS_PER_SHORT   16L
#endif
#ifndef ND_BITS_PER_INT
#define ND_BITS_PER_INT     32L
#endif
#ifndef ND_BITS_PER_INT64
#define ND_BITS_PER_INT64   64L
#endif
#ifndef ND_BITS_PER_LONG
#define ND_BITS_PER_LONG    32L
#endif
//#ifndef ND_BITS_PER_LONG
//#define ND_BITS_PER_LONG    64L
//#endif
#ifndef ND_BITS_PER_FLOAT
#define ND_BITS_PER_FLOAT   32L
#endif
#ifndef ND_BITS_PER_DOUBLE
#define ND_BITS_PER_DOUBLE  64L
#endif
#ifndef ND_BITS_PER_WORD
#define ND_BITS_PER_WORD    32L
#endif
#ifndef ND_BITS_PER_DWORD
#define ND_BITS_PER_DWORD	64L
#endif

#endif
