#ifndef __SHARE_BASE_ND_TYPES_H__
#define __SHARE_BASE_ND_TYPES_H__

#include "NDCPUCfg.h"
/************************************************************************
** TYPES:       NDUint8
**              NDUint8
** DESCRIPTION:
**  The int8 types are known to be 8 bits each. There is no type that
**      is equivalent to a plain "char". 
************************************************************************/
#if ND_BYTES_PER_BYTE == 1

#ifndef NDUint8
typedef unsigned char NDUint8;
#endif
/*
** Some cfront-based C++ compilers do not like 'signed char' and
** issue the warning message:
**     warning: "signed" not implemented (ignored)
** For these compilers, we have to define NDInt8 as plain 'char'.
** Make sure that plain 'char' is indeed signed under these compilers.
*/
#if (defined(HPUX) && defined(__cplusplus) \
	&& !defined(__GNUC__) && __cplusplus < 199707L) \
	|| (defined(SCO) && defined(__cplusplus) \
	&& !defined(__GNUC__) && __cplusplus == 1L)
#ifndef NDInt8
typedef char NDInt8;
#endif
#else
#ifndef NDInt8
typedef signed char NDInt8;
#endif
#endif

#else
#error No suitable type for NDInt8/NDUint8
#endif

/************************************************************************
* MACROS:      ND_INT8_MAX
*              ND_INT8_MIN
*              ND_UINT8_MAX
* DESCRIPTION:
*  The maximum and minimum values of a NDInt8 or NDUint8.
************************************************************************/
#ifndef	ND_INT8_MAX
#define ND_INT8_MAX	127
#endif
#ifndef	ND_INT8_MIN
#define ND_INT8_MIN (-128)
#endif
#ifndef	ND_UINT8_MAX
#define ND_UINT8_MAX 255U
#endif



/************************************************************************
** TYPES:       NDUint16
**              NDInt16
** DESCRIPTION:
**  The int16 types are known to be 16 bits each. 
************************************************************************/
#if ND_BYTES_PER_SHORT == 2

#ifndef	NDUint16
typedef unsigned short NDUint16;
#endif
#ifndef	NDInt16
typedef short NDInt16;
#endif

#else
#error No suitable type for NDInt16/NDUint16
#endif

/************************************************************************
* MACROS:      ND_INT16_MAX
*              ND_INT16_MIN
*              ND_UINT16_MAX
* DESCRIPTION:
*  The maximum and minimum values of a NDInt16 or NDUint16.
************************************************************************/
#ifndef	ND_INT16_MAX
#define ND_INT16_MAX 32767
#endif
#ifndef	ND_INT16_MIN
#define ND_INT16_MIN (-32768)
#endif
#ifndef	ND_UINT16_MAX
#define ND_UINT16_MAX 65535U
#endif



/************************************************************************
** TYPES:       PRUint32
**              PRInt32
** DESCRIPTION:
**  The int32 types are known to be 32 bits each. 
************************************************************************/
#if ND_BYTES_PER_INT == 4

#ifndef	NDUint32
typedef unsigned int NDUint32;
#endif
#ifndef	NDInt32
typedef int NDInt32;
#endif
#ifndef	ND_INT32
#define ND_INT32(x)  x
#endif
#ifndef	ND_UINT32
#define ND_UINT32(x) (x ## U)
#endif

#elif ND_BYTES_PER_LONG == 4
#ifndef	NDUint32
typedef unsigned long NDUint32;
#endif
#ifndef	NDInt32
typedef long NDInt32;
#endif
#ifndef	ND_INT32
#define ND_INT32(x)  (x ## L)
#endif
#ifndef	ND_UINT32
#define ND_UINT32(x) (x ## UL)
#endif

#else
#error No suitable type for NDInt32/NDUint32
#endif

/************************************************************************
* MACROS:      ND_INT32_MAX
*              ND_INT32_MIN
*              ND_UINT32_MAX
* DESCRIPTION:
*  The maximum and minimum values of a NDInt32 or NDUint32.
************************************************************************/
#ifndef	ND_INT32_MAX
#define ND_INT32_MAX	ND_INT32(2147483647)
#endif
#ifndef	ND_INT32_MIN
#define ND_INT32_MIN	(-ND_INT32_MAX - 1)
#endif
#ifndef	ND_UINT32_MAX
#define ND_UINT32_MAX	ND_UINT32(4294967295)
#endif

//NDInt32 and NDUint32 string length;(10+1);
#ifndef ND_INT32_STRLEN
#define ND_INT32_STRLEN		(11)
#endif

/************************************************************************
** TYPES:       NDUint64
**              NDInt64
** DESCRIPTION:
**  The int64 types are known to be 64 bits each. Care must be used when
**      declaring variables of type PRUint64 or PRInt64. Different hardware
**      architectures and even different compilers have varying support for
**      64 bit values. The only guaranteed portability requires the use of
**      the LL_ macros (see prlong.h).
************************************************************************/
#ifdef HAVE_LONG_LONG

#if ND_BYTES_PER_LONG == 8
#ifndef	NDInt64
typedef long NDInt64;
#endif
#ifndef	NDUint64
typedef unsigned long NDUint64;
#endif
#elif defined(WIN16)
#ifndef	NDInt64
typedef __int64 NDInt64;
#endif
#ifndef	NDUint64
typedef unsigned __int64 NDUint64;
#endif
#elif defined(WIN32) && !defined(__GNUC__)
#ifndef	NDInt64
typedef __int64  NDInt64;
#endif
#ifndef	NDUint64
typedef unsigned __int64 NDUint64;
#endif
#else
#ifndef	NDInt64
typedef long long NDInt64;
#endif
#ifndef	NDUint64
typedef unsigned long long NDUint64;
#endif
#endif /* ND_BYTES_PER_LONG == 8 */

#else  /* !HAVE_LONG_LONG */
typedef struct {
#ifdef IS_LITTLE_ENDIAN
	NDUint32 lo, hi;
#else
	NDUint32 hi, lo;
#endif
} NDInt64;
#ifndef	NDUint64
typedef NDInt64 NDUint64;
#endif

#endif /* !HAVE_LONG_LONG */

//NDInt64 and NDUint64 string length;(20+1);
#ifndef ND_INT64_STRLEN
#define ND_INT64_STRLEN		(21)
#endif

/************************************************************************
** TYPES:       NDFloat64
** DESCRIPTION:
**   float type.
************************************************************************/
#ifndef	NDFloat32
typedef float			NDFloat32;
#endif

#ifndef	NDFloat64
typedef double          NDFloat64;
#endif

/************************************************************************
** TYPES:       NDSize
** DESCRIPTION:
**  A type for representing the size of objects. 
************************************************************************/
#ifndef	NDSize
#ifdef LINUX
#include <sys/types.h>
#endif
typedef size_t			NDSize;
#endif


/************************************************************************
** TYPES:       NDBool
** DESCRIPTION:
**  Use NDBool for variables and parameter types. Use NDFalse and NDTrue
**      for clarity of target type in assignments and actual arguments. Use
**      'if (bool)', 'while (!bool)', '(bool) ? x : y' etc., to test booleans
**      juast as you would C int-valued conditions. 
************************************************************************/
#ifndef	NDBool
typedef bool	NDBool;
const NDBool NDTrue	 = true;
const NDBool NDFalse = false;
#endif


#ifndef NDWord
typedef unsigned short	NDWord;
#endif

#ifndef NDDword
typedef unsigned long	NDDword;
#endif


#include <string>
using std::string;

#ifndef NDString
typedef string		NDString;
#endif

//define NDTime just for time_t;
#ifndef NDTime
typedef	NDUint64	NDTime;
#endif

#ifndef NDGroup
typedef unsigned int	NDGroup;
#endif

/*************************************************************************
	Dynamic Library import / export control conditional
	(Define ND_EXPORTS to export symbols, else they are imported)
*************************************************************************/
#if defined( WIN32 ) && !defined(ND_STATIC)
#   ifdef ND_EXPORTS
#       define NDEXPORT __declspec(dllexport)
#   else
#       define NDEXPORT __declspec(dllimport)
#   endif
#else
#       define NDEXPORT
#endif

#ifdef	WIN32
#ifndef STDCALL
#define STDCALL		__stdcall
#endif
#else
#ifndef STDCALL
#define STDCALL
#endif
#endif

/***********************************************************************
** MACROS:      ND_BEGIN_MACRO
**              ND_END_MACRO
** DESCRIPTION:
**      Macro body brackets so that macros with compound statement definitions
**      behave syntactically more like functions when called.
***********************************************************************/
#define ND_BEGIN_MACRO  do {
#define ND_END_MACRO    } while (0)

/***********************************************************************
** MACROS:      ND_BEGIN_EXTERN_C
**              ND_END_EXTERN_C
** DESCRIPTION:
**      Macro shorthands for conditional C++ extern block delimiters.
***********************************************************************/
#ifdef __cplusplus
#define ND_BEGIN_EXTERN_C       extern "C" {
#define ND_END_EXTERN_C         }
#else
#define ND_BEGIN_EXTERN_C
#define ND_END_EXTERN_C
#endif

#if defined(__cplusplus)
#ifndef _NDSHAREBASE
#define	_NDSHAREBASE				::NDShareBase::
#define _NDSHAREBASE_BEGIN			namespace NDShareBase {
#define _NDSHAREBASE_END			}
#endif
#endif

#ifndef FD_SETSIZE
#define	FD_SETSIZE				(8192)
#endif

#ifdef	WIN32
	#include <BaseTsd.h>
	typedef UINT_PTR			SOCKET;
	typedef	unsigned int		ThreadID;
	#define INVALID_SOCKET		(SOCKET)(~0)
#else
	typedef	int					SOCKET;
	#define	INVALID_SOCKET		(-1)
	#define SOCKET_ERROR		(-1)
	typedef pthread_t			ThreadID;
#endif

#ifndef PParsePacketFun
	namespace NDShareBase {
		//解析包需要的延伸数据;
		struct	NDParseSessionDataEx
		{
			NDUint8		m_nProtocolType;			// session dispose protocol type;
			NDUint8		m_nParsePacket;				// parse packet number;
			NDUint32	m_nSessionID;				// session ID;
			NDUint32	m_nErrorCode;				// parse packet error code;	
		};
		class	NDByteBuffer;
	}
	using NDShareBase::NDParseSessionDataEx;
	using NDShareBase::NDByteBuffer;
	typedef	NDBool	(*PParsePacketFun)( NDByteBuffer& recvMsgOriginalBuf, NDParseSessionDataEx& refNDParseSessionDataEx );
#endif

#endif
