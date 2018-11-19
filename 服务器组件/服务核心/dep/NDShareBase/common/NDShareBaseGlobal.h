#ifndef __SHARE_BASE_ND_GLOBAL_H__
#define __SHARE_BASE_ND_GLOBAL_H__

#include <vector>
using std::vector;
#include <map>
using std::map;

#include <time.h>

#include "NDTypes.h"
#include "NDShareBaseMacros.h"


#if defined( __WIN32__ ) || defined( WIN32 ) || defined( _WIN32 )

#ifndef	WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <excpt.h>
#include <DbgHelp.h>
#pragma comment(lib, "dbghelp")

#pragma warning(disable:4312)
#pragma warning(disable:4127)

#endif




_NDSHAREBASE_BEGIN

/* Define this if you're using a big-endian machine */
#ifdef USING_BIG_ENDIAN
#include <machine/byte_order.h>
#define nd_bswap_16(x) NXSwapShort(x)
#define nd_bswap_32(x) NXSwapInt(x)
#define nd_bswap_64(x) NXSwapLongLong(x)
#endif

#ifdef USING_BIG_ENDIAN
#define GUID_HIPART(x)		(*((NDUint32*)&(x)))
#define GUID_LOPART(x)		(*(((NDUint32*)&(x))+1))
#define NDUINT32_HIPART(x)	(*((NDUint16*)&(x)))
#define NDUINT32_LOPART(x)	(*(((NDUint16*)&(x))+1))
#else
#define GUID_HIPART(x)		(*(((NDUint32*)&(x))+1))
#define GUID_LOPART(x)		(*((NDUint32*)&(x)))
#define NDUINT32_HIPART(x)	(*(((NDUint16*)&(x))+1))
#define NDUINT32_LOPART(x)	(*((NDUint16*)&(x)))
#endif


class StatData;
//most function isn't thread safe;
struct NDShareBaseGlobal
{
private:

#ifdef WIN32
	static LARGE_INTEGER				m_start;
	static LARGE_INTEGER				m_FrequencyCounter;
#else
	static struct timeval				m_start;
#endif

	static NDUint64						m_nRealWorldMSTimeInStartBoot;	//开启程序时的毫秒数(本地真实世界的时间);
	static NDUint64						m_nUTCMSTimeInStartBoot;		//开启程序时的毫秒数(UTC时间);
	static NDUint64						m_nServerGUIDBase;				//GUID的基数;

	static const NDUint8				m_szToupper[CTYPE_NUM_CHARS];
	static const NDUint8				m_szTolower[CTYPE_NUM_CHARS];
	static NDUint8						m_szMoudlePath[MAX_PATH_LEN];

private:
	typedef struct {
		NDInt32  t_val;
		char	*t_name;
	} TRANS;
	static const TRANS					m_priorities[9];


public:
	static	StatData*					g_spStatFunc;
	static	map<NDUint32, string>		g_sStatFuncNameMap;
	static	void						save_maptype_log( const char* szFilePathName );

public:
	/************************************************************************
	** function DESCRIPTION:
	**  call init function for NDShareBaseGlobal;
	**  nServerGroup is server group number.
	**  nServer is server number;
	************************************************************************/
	static NDBool		init( NDUint8 nServerGroup, NDUint8 nServerID );

	/************************************************************************
	** function DESCRIPTION:
	**	return true--CPU is little endian, false -- CPU is big endian.
	************************************************************************/
	static NDBool		isCPULittleEndian();

		/************************************************************************
	** function DESCRIPTION:
	**	return true-- x is right pointer, false -- x is error pointer.
	************************************************************************/
	static NDBool		isValidPointer( const void* x );

	/************************************************************************
	** function DESCRIPTION:
	**	return current module path string.
	**	sample: 
		if  run "E:\vs.net\NDServer\DebugBin\debug.exe"
		invoke getMoudlePath get "E:\vs.net\NDServer\DebugBin"
	************************************************************************/
	static const char*	getMoudlePath();

	/************************************************************************
	** function DESCRIPTION:
	**	return current module path string.
	**	sample: 
		if  run "E:\vs.net\NDServer\DebugBin\debug.exe"
		invoke getCurDirectory get "E:\vs.net\NDServer\DebugBin"
	************************************************************************/
	static NDBool		getCurDirectory( char* szDir, NDUint32 nDirSize );

	/************************************************************************
	** function DESCRIPTION:
	**	return NDTrue or NDFalse.
	**	sample: 
		if  szDir="E:\vs.net\NDServer\DebugBin";
		invoke isDirectoryExist get NDTrue or NDFalse;
	************************************************************************/
	static NDBool		isDirectoryExist( const char* szDir );

	/************************************************************************
	** function DESCRIPTION:
	**	return create directory success or failed;
	**	sample: 
		if  szDir="E:\vs.net\NDServer\DebugBin";
		invoke createDirectory get NDTrue or NDFalse;
	************************************************************************/
	static NDBool		createDirectory( const char* szDir );


	/************************************************************************
	** function DESCRIPTION:
	**	return NDTrue or NDFalse.
	**	sample: 
		if  szFileName="E:\vs.net\NDServer\DebugBin\server.ini";
		invoke isFileExist get NDTrue or NDFalse;
	************************************************************************/
	static NDBool		isFileExist( const char* szFileName );

	/************************************************************************
	** function DESCRIPTION:
	**	return NDTrue or NDFalse.
	**	sample: 
		if  szFilePath="E:\vs.net\NDServer\DebugBin\server.ini";
		invoke isFileExist get  szFileName="server.ini";
	************************************************************************/
	static NDBool		extractFileName( const char* szFilePath, char* szFileName, NDUint32 nFileNameSize );

	/************************************************************************
	** function DESCRIPTION:
	**	return day of week(number from 1 to 7).
	**	sample:
	**	invoke getdayofweek( 2011, 6, 9) get the number 4.
	************************************************************************/
	static NDUint32		getdayofweek( NDUint32 nYear, NDUint32 nMonth, NDUint32 nDay );

	/************************************************************************
	** function DESCRIPTION:
	**  return local day time string.
	**  sample: "2010-4-16";
	************************************************************************/
	static string		getLocalDayTimeStr();

	/************************************************************************
	** function DESCRIPTION:
	**  return local day second time string.
	**  sample: "2010-4-16 10:18:30"
	************************************************************************/
	static string		getLocalSecondTimeStr();

	/************************************************************************
	** function DESCRIPTION:
	**  return NDTrue or NDFalse;
	**  szBuf get local day second time string("2010-4-16 10:18:30").
	**  nBufSize szBuf Buffer size;	 
	************************************************************************/
	static NDBool		getLocalSecondTimeStr( char* szBuf, NDUint32 nBufSize );

	/************************************************************************
	** function DESCRIPTION:
	**  return local day second time string only for single thread.
	**  sample: "2010-4-16 10:18:30";
	************************************************************************/
	static char*		getLocalSecondTimeStrForSingleThread();

	/************************************************************************
	** function DESCRIPTION:
	**  The returned NDUint32 is a count of system ticks 
	**  (or milliseconds) since boot time.(误差小于1ms must call initForGetMSTime ).
	************************************************************************/
	static NDTime		getMSTimeSinceStartBoot();

	/************************************************************************
	** function DESCRIPTION:
	**  The returned NDTime is milliseconds time from standard start time
	**	(1970-1-1) to current time( the real world time ). ( must call initForGetMSTime );
	************************************************************************/
	static NDTime		getCurMSTimeOfLoacl();

	/************************************************************************
	** function DESCRIPTION:
	**  The returned NDTime is milliseconds time from standard start time
	**	(1970-1-1) to current time( UTC ms, ret/1000=time(NULL); ). ( must call initForGetMSTime );
	************************************************************************/
	static NDTime		getCurMSTimeOfUTC();

	/************************************************************************
	** function DESCRIPTION:
	**  The returned NDTime is seconds time from standard start time
	**	(1970-1-1) to current time. (just return time(NULL); );
	************************************************************************/
	static NDTime		getCurSecondTimeOfUTC();

	/************************************************************************
	** function DESCRIPTION:
	**  The returned NDTime is Milliseconds time from nEndSecondTime
	**	to nStartSecondTime. (just return difftime( nEndSecondTime, nStartSecondTime )*1000; );
	************************************************************************/
	static NDTime		getDiffMilliSecondTime( NDTime nEndSecondTime, NDTime nStartSecondTime );

	/************************************************************************
	** function DESCRIPTION:
	**  return second time string( transfer nTime to time string ).
	**  sample: "2010-4-16 10:18:30"
	************************************************************************/
	static string		transferSecondTimeToStr( NDTime nSecondTime );


	/************************************************************************
	** function DESCRIPTION:
	**  return second time ( transfer time string to NDUint32 value ).
	************************************************************************/
	static NDTime		transferSecondTimeStrToNDTime( const string& strTime );


	/************************************************************************
	** function DESCRIPTION:
	**  NDTime nSleepMSTime: milliseconds, sleep milliseconds.
	************************************************************************/
	static void			sleep( NDTime nSleepMSTime );

	/************************************************************************
	** function DESCRIPTION:
	**  NDTime nSleepMSTime: milliseconds, sleep milliseconds.
	************************************************************************/
	static void			sleep2( NDTime nSleepMSTime );

	/************************************************************************
	** function DESCRIPTION:
	**  judge const char* pParam is valid or not.
	**  return true--valid, false--invalid.
	************************************************************************/
	static NDBool		isValidStrParam( const char* pParam, NDUint32& refLen );



	/************************************************************************
	** function DESCRIPTION:
	**  safe string copy
	**  return copy size.
	************************************************************************/
	static NDUint32		safeStrCopy( char* pDest, const char* pSrc, NDUint32 nDestLen, NDUint32 nSrcLen );


	/************************************************************************
	** function DESCRIPTION:
	**  safe string copy
	**	Copy pSrc to string pDest of size nDestLen.  At most nDestLen-1 characters
	**	will be copied.  Always NUL terminates (unless nDestLen == 0).
	**  Returns strlen(pSrc); if retval >= nDestLen, truncation occurred.
	**  return copy size.
	************************************************************************/
	static NDUint32		strlcpy( char* pDest, const char* pSrc, NDUint32 nDestLen );

	/************************************************************************
	** function DESCRIPTION:
	** safe transfer string to Integer.
	** return Integer.
	** 功能：将字符串转换为数字类型;
	************************************************************************/
	static NDInt32		strToInt32( const char* szStr );
	static NDUint32		strToUint32( const char* szStr );
	static NDInt64		strToInt64( const char* szStr );
	static NDUint64		strToUint64( const char* szStr );
	static NDFloat32	strToFloat32( const char* szStr );
	static NDFloat64	strToFloat64( const char* szStr );

	/************************************************************************
	** function DESCRIPTION:
	** safe transfer Integer to string.
	** return NDBool.
	** 功能：将数字类型转换为字符串;
	************************************************************************/
	static NDBool		int32ToStr( NDInt32 nValue, char* szStr, NDUint32 nStrSize );
	static NDBool		uint32Tostr( NDUint32 nValue, char* szStr, NDUint32 nStrSize );
	static NDBool		int64ToStr( NDInt64 nValue, char* szStr, NDUint32 nStrSize );
	static NDBool		uint64ToStr( NDUint64 nValue, char* szStr, NDUint32 nStrSize );
	static NDBool		float32ToStr( NDFloat32 nValue, char* szStr, NDUint32 nStrSize, NDUint32 nDecplaces=3 );
	static NDBool		float64ToStr( NDFloat64 nValue, char* szStr, NDUint32 nStrSize, NDUint32 nDecplaces=3 );

	/************************************************************************
	** function DESCRIPTION:
	**  transfer c pParam to upper .
	**  return NDInt32.
	** 功能：将字符(c)转换为大写英文字母字符;
	************************************************************************/
	static NDInt32		toupper( NDInt32 c );

	/************************************************************************
	** function DESCRIPTION:
	**  transfer c strParam to lower .
	**  return void.
	** 功能：将字符(c)转换为小写英文字母字符;
	************************************************************************/
	static NDInt32		tolower( NDInt32 c );

	/************************************************************************
	** function DESCRIPTION:
	**  transfer char* pParam to upper .
	**  return void.
	** 功能：将字符串(pParam)转换为大写英文字母字符串;
	************************************************************************/
	static void			toupper( char* pParam, NDUint32 nLen );

	/************************************************************************
	** function DESCRIPTION:
	**  transfer string& strParam to upper .
	**  return void.
	** 功能：将string(strParam)转换为大写英文字母字符串组成的string;
	************************************************************************/
	static void			toupper( string& strParam );


	/************************************************************************
	** function DESCRIPTION:
	**  transfer char* pParam to lower .
	**  return void.
	** 功能：将字符串(pParam)转换为小写英文字母字符串;
	************************************************************************/
	static void			tolower( char* pParam, NDUint32 nLen );

	/************************************************************************
	** function DESCRIPTION:
	**  transfer string& strParam to lower .
	**  return void.
	** 功能：将string(strParam)转换为小写英文字母字符串组成的string;
	************************************************************************/
	static void			tolower( string& strParam );

	/************************************************************************
	** function DESCRIPTION:
	**  remove the begin and end of string& refStr szFlag character.
	**  return void.
	** 功能：将string(refStr)前后面的特殊字符szFlag去掉;
	************************************************************************/
	static void			strtrim( string& refStr, char szFlag );

	/************************************************************************
	** function DESCRIPTION:
	**  split const string& refStr to vector<string>& refStrVec depend on const string& strKey.
	**  return void.
	** 功能：将string(refStr)按照string(strkey)分开塞入vector<string>& refStrVec;
	************************************************************************/
	static void			strsplit( const string& refStr, const string& strKey, vector<string>& refStrVec );

	/************************************************************************
	** function DESCRIPTION:
	**  const char* pSrcBuf compress to  char* pDestBuf .
	**  return NDTrue--success NDFalse--failed.
	************************************************************************/
	static NDBool		compress( char* pDestBuf, NDUint32* nDestLen, const char* pSrcBuf, NDUint32 nSrcLen );

	/************************************************************************
	** function DESCRIPTION:
	**  const char* pSrcBuf uncompress to  char* pDestBuf .
	**  return NDTrue--success NDFalse--failed.
	************************************************************************/
	static NDBool		uncompress( char* pDestBuf, NDUint32* nDestLen, const char* pSrcBuf, NDUint32 nSrcLen );

	/************************************************************************
	** function DESCRIPTION:
	**  calculate CRC code.
	**  return CRC len.
	************************************************************************/
	static NDUint32		crc32( NDUint32 nCRC, const char* pBuf, NDUint32 nBufLen );


	/************************************************************************
	** function DESCRIPTION:
	**  this is BKDR HASH function.
	**  transform calculate string code to only NDUint32 code.
	**  return CRC len.
	************************************************************************/
	static NDUint32		bkdr_hash( const char* pBuf );


	/************************************************************************
	** function DESCRIPTION:
	**  this is get guid function.
	**  call getGUID before must call initGUID init function.
	**  nServer is server number;
	**  return NDUint64.
	************************************************************************/
	static NDUint64		getGUID();

	/************************************************************************
	** function DESCRIPTION:
	**  write exception information to szDumpFile
	************************************************************************/
	static NDInt32		generateDump( const char* szDumpFilePath, void* pExtend );


	/************************************************************************
	** function DESCRIPTION:
	**  sharememory unit lock and unlock function;
	************************************************************************/
	static NDBool		sm_try_lock( NDUint16* pUnitOwnType, NDUint16 nLockType );
	static NDBool		sm_try_unlock(NDUint16* pUnitOwnType, NDUint16 nUnlockType);

	/************************************************************************
	** function DESCRIPTION:
	**  atomic compare and set value;
	**  IN WINDOWS: Can use short,int,__int64 , use char error.
	************************************************************************/
	template <typename T>
	static T atomic_cas( T* pLock, T old, T set )
	{
#       if defined(WIN32)
			int nSize = sizeof(T);
			switch (nSize)
			{
			case 1:
				return (T)InterlockedCompareExchangePointer((PVOID*)&pLock, (PVOID)&set, (PVOID)&old);
			case 2:
				return (T)InterlockedCompareExchange16((short*)pLock, (short)set, (short)old);
			case 4:
				return (T)InterlockedCompareExchange((long*)pLock, (long)set, (long)old);
			case 8:
				return (T)InterlockedCompareExchange64((__int64*)pLock, (__int64)set, (__int64)old);
			default:
				return (T)InterlockedCompareExchangePointer((PVOID*)&pLock, (PVOID)&set, (PVOID)&old);
			}
			
#       elif defined(LINUX) || defined(FREEBSD) || defined(OSX)
			int nSize = sizeof(T);
			T prev;
			switch (nSize)
			{
			case 1:
				asm volatile ("lock; cmpxchgb %b1, %2"
								: "=a" (prev)
								: "q" (set), "m" (*pLock), "0"(old)
								: "memory", "cc");
				return prev;
			case 2:
				asm volatile ("lock; cmpxchgw %w1, %2"
								: "=a" (prev)
								: "r" (set), "m" (*pLock), "0"(old)
								: "memory", "cc");
				return prev;
			case 4:
				asm volatile ("lock; cmpxchgl %1, %2"
								: "=a" (prev)
								: "r" (set), "m" (*pLock), "0"(old)
								: "memory", "cc");
				return prev;
			case 8:
				asm volatile ("lock; cmpxchgq %1, %2"
								: "=a" (prev)
								: "r" (set), "m" (*pLock), "0"(old)
								: "memory", "cc");
				return prev;
			}
			return set;
#       endif
	}

	/************************************************************************
	** function DESCRIPTION:
	**  spin lock and unlock function;
	************************************************************************/
	template <typename T>
	static bool spin_lock_unlock(T* pLock, T old, T set)
	{
		NDUint8 nLockCount = 0;
		NDBool	bRet = NDFalse;
		do
		{
			if (old == atomic_cas(pLock, old, set))
			{
				bRet = NDTrue;
				break;
			}

			++nLockCount;
			sleep2(1);
			if (nLockCount > ND_SMU_LOCK_LOOP_MAX)
				break;
		} while (1);

		return bRet;
	}


	/************************************************************************
	** function DESCRIPTION:
	**  get log type name;
	************************************************************************/
	static const char*	getLogTypeName( NDInt32 nLogLevel );

#ifdef USING_BIG_ENDIAN

	static void			swap16(NDUint16* p);
	static void			swap32(NDUint32* p);
	static void			swap64(NDUint64* p);
	static NDFloat32	swapfloat( NDFloat32 p );
	static NDFloat64	swapdouble( NDFloat64 p );
	static void			swapfloat( NDFloat32 * p );
	static void			swapdouble(NDFloat64 * p);
	static NDUint16 	swap16(NDUint16 p);
	static NDUint32 	swap32(NDUint32 p);
	static NDUint64 	swap64(NDUint64 p);
	static void 		swap16(NDInt16* p);
	static void 		swap32(NDInt32* p);
	static void 		swap64(NDInt64* p);
	static NDInt16 		swap16(NDInt16 p);
	static NDInt32 		swap32(NDInt32 p);
	static NDInt64 		swap64(NDInt64 p);
	
#endif


private:
	/************************************************************************
	** function DESCRIPTION:
	**  init param for GetMSTime;
	************************************************************************/
	static void			initForGetMSTime();

	/************************************************************************
	** function DESCRIPTION:
	**  this is init guid function.
	**  nServerGroup is server group number.
	**  nServer is server number;
	**  return void.
	************************************************************************/
	static void			initGUID( NDUint8 nServerGroup, NDUint8 nServerID );

};


/* these can be optimized into assembly */
#ifdef USING_BIG_ENDIAN

/*ND_INLINE static void NDShareBaseGlobal::nd_swap16(NDUint16* p) { *p = ((*p >> 8) & 0xff) | (*p << 8); }
ND_INLINE static void NDShareBaseGlobal::nd_swap32(NDUint32* p) { *p = ((*p >> 24 & 0xff)) | ((*p >> 8) & 0xff00) | ((*p << 8) & 0xff0000) | (*p << 24); }
ND_INLINE static void NDShareBaseGlobal::nd_swap64(NDUint64* p) { *p = ((*p >> 56)) | ((*p >> 40) & 0x000000000000ff00ULL) | ((*p >> 24) & 0x0000000000ff0000ULL) | ((*p >> 8 ) & 0x00000000ff000000ULL) |
								((*p << 8 ) & 0x000000ff00000000ULL) | ((*p << 24) & 0x0000ff0000000000ULL) | ((*p << 40) & 0x00ff000000000000ULL) | ((*p << 56)); }*/

ND_INLINE static void NDShareBaseGlobal::swap16(NDUint16* p) { *p = nd_bswap_16((NDUint16)*p); }
ND_INLINE static void NDShareBaseGlobal::swap32(NDUint32* p) { *p = nd_bswap_32((NDUint32)*p); }
ND_INLINE static void NDShareBaseGlobal::swap64(NDUint64* p) { *p = nd_bswap_64((NDUint64)*p);; }

ND_INLINE static NDFloat32 NDShareBaseGlobal::swapfloat( NDFloat32 p )
{
	union { NDFloat32 asfloat; NDUint8 asbytes[4]; } u1, u2;
	u1.asfloat = p;
	/* swap! */
	u2.asbytes[0] = u1.asbytes[3];
	u2.asbytes[1] = u1.asbytes[2];
	u2.asbytes[2] = u1.asbytes[1];
	u2.asbytes[3] = u1.asbytes[0];
    
	return u2.asfloat;
}

ND_INLINE static NDFloat64 NDShareBaseGlobal::swapdouble( NDFloat64 p )
{
	union { NDFloat64 asfloat; NDUint8 asbytes[8]; } u1, u2;
	u1.asfloat = p;
	/* swap! */
	u2.asbytes[0] = u1.asbytes[7];
	u2.asbytes[1] = u1.asbytes[6];
	u2.asbytes[2] = u1.asbytes[5];
	u2.asbytes[3] = u1.asbytes[4];
	u2.asbytes[4] = u1.asbytes[3];
	u2.asbytes[5] = u1.asbytes[2];
	u2.asbytes[6] = u1.asbytes[1];
	u2.asbytes[7] = u1.asbytes[0];

	return u2.asfloat;
}

ND_INLINE static void NDShareBaseGlobal::swapfloat( NDFloat32 * p )
{
	union { NDFloat32 asfloat; NDUint8 asbytes[4]; } u1, u2;
	u1.asfloat = *p;
	/* swap! */
	u2.asbytes[0] = u1.asbytes[3];
	u2.asbytes[1] = u1.asbytes[2];
	u2.asbytes[2] = u1.asbytes[1];
	u2.asbytes[3] = u1.asbytes[0];
	*p = u2.asfloat;
}

ND_INLINE static void NDShareBaseGlobal::swapdouble(NDFloat64 * p)
{
	union { NDFloat64 asfloat; NDUint8 asbytes[8]; } u1, u2;
	u1.asfloat = *p;
	/* swap! */
	u2.asbytes[0] = u1.asbytes[7];
	u2.asbytes[1] = u1.asbytes[6];
	u2.asbytes[2] = u1.asbytes[5];
	u2.asbytes[3] = u1.asbytes[4];
	u2.asbytes[4] = u1.asbytes[3];
	u2.asbytes[5] = u1.asbytes[2];
	u2.asbytes[6] = u1.asbytes[1];
	u2.asbytes[7] = u1.asbytes[0];
	*p = u2.asfloat;
}

/*ND_INLINE static NDUint16 NDShareBaseGlobal::nd_swap16(NDUint16 p) { return ((p >> 8) & 0xff) | (p << 8); }
ND_INLINE static NDUint32 NDShareBaseGlobal::nd_swap32(NDUint32 p) { return ((p >> 24) & 0xff) | ((p >> 8) & 0xff00) | ((p << 8) & 0xff0000) | (p << 24); }
ND_INLINE static NDUint64 NDShareBaseGlobal::nd_swap64(NDUint64 p)  { p = (((p >> 56) & 0xff)) | ((p >> 40) & 0x000000000000ff00ULL) | ((p >> 24) & 0x0000000000ff0000ULL) | ((p >> 8 ) & 0x00000000ff000000ULL) |
								((p << 8 ) & 0x000000ff00000000ULL) | ((p << 24) & 0x0000ff0000000000ULL) | ((p << 40) & 0x00ff000000000000ULL) | ((p << 56)); }

ND_INLINE static void NDShareBaseGlobal::nd_swap16(NDInt16* p) { *p = ((*p >> 8) & 0xff) | (*p << 8); }
ND_INLINE static void NDShareBaseGlobal::nd_swap32(NDInt32* p) { *p = ((*p >> 24) & 0xff) | ((*p >> 8) & 0xff00) | ((*p << 8) & 0xff0000) | (*p << 24); }
ND_INLINE static void NDShareBaseGlobal::nd_swap64(NDInt64* p) { *p = ((*p >> 56) & 0xff) | ((*p >> 40) & 0x000000000000ff00ULL) | ((*p >> 24) & 0x0000000000ff0000ULL) | ((*p >> 8 ) & 0x00000000ff000000ULL) |
								((*p << 8 ) & 0x000000ff00000000ULL) | ((*p << 24) & 0x0000ff0000000000ULL) | ((*p << 40) & 0x00ff000000000000ULL) | ((*p << 56)); }

ND_INLINE static NDInt16 NDShareBaseGlobal::nd_swap16(NDInt16 p) { return ((p >> 8) & 0xff) | (p << 8); }
ND_INLINE static NDInt32 NDShareBaseGlobal::nd_swap32(NDInt32 p) { return ((p >> 24) & 0xff) | ((p >> 8) & 0xff00) | ((p << 8) & 0xff0000) | (p << 24); }
ND_INLINE static NDInt64 NDShareBaseGlobal::nd_swap64(NDInt64 p)  { return ((((p >> 56) & 0xff)) | ((p >> 40) & 0x000000000000ff00ULL) | ((p >> 24) & 0x0000000000ff0000ULL) | ((p >> 8 ) & 0x00000000ff000000ULL) |
								((p << 8 ) & 0x000000ff00000000ULL) | ((p << 24) & 0x0000ff0000000000ULL) | ((p << 40) & 0x00ff000000000000ULL) | ((p << 56))); }*/

ND_INLINE static NDUint16 NDShareBaseGlobal::swap16(NDUint16 p) { return nd_bswap_16((NDUint16)p); }
ND_INLINE static NDUint32 NDShareBaseGlobal::swap32(NDUint32 p) { return nd_bswap_32((NDUint32)p); }
ND_INLINE static NDUint64 NDShareBaseGlobal::swap64(NDUint64 p) { return nd_bswap_64((NDUint64)p); }

ND_INLINE static void NDShareBaseGlobal::swap16(NDInt16* p) { *p = nd_bswap_16((NDUint16)*p); }
ND_INLINE static void NDShareBaseGlobal::swap32(NDInt32* p) { *p = nd_bswap_32((NDUint32)*p); }
ND_INLINE static void NDShareBaseGlobal::swap64(NDInt64* p) { *p = nd_bswap_64((NDUint64)*p); }

ND_INLINE static NDInt16 NDShareBaseGlobal::swap16(NDInt16 p) { return nd_bswap_16((NDInt16)p); }
ND_INLINE static NDInt32 NDShareBaseGlobal::swap32(NDInt32 p) { return nd_bswap_32((NDUint32)p); }
ND_INLINE static NDInt64 NDShareBaseGlobal::swap64(NDInt64 p) { return nd_bswap_64((NDUint64)p); }

#endif




#ifndef ND_EXCEPTION_PROTECTED
#define ND_EXCEPTION_PROTECTED							

#ifdef WIN32

#define ND_EXCEPTION_PROTECTED_BEGIN()									_try {
#define ND_EXCEPTION_PROTECTED_CATCH( szDumpPath, NDPTHREADFUN )		} __except( NDShareBaseGlobal::generateDump( szDumpPath, GetExceptionInformation() ) ) { NDPTHREADFUN(NULL); }

#else
#define ND_EXCEPTION_PROTECTED_BEGIN()									try {
#define ND_EXCEPTION_PROTECTED_CATCH( szDumpPath, NDPTHREADFUN )		} catch(...) { NDShareBaseGlobal::generateDump( szDumpPath, NULL ); NDPTHREADFUN(NULL); }

#endif

#endif


#ifndef ND_STRINGTOINT32
#define ND_STRINGTOINT32( s )		NDShareBaseGlobal::strToInt32( s )
#endif

#ifndef ND_STRINGTOUINT32
#define ND_STRINGTOUINT32( s )		NDShareBaseGlobal::strToUint32( s )	
#endif

#ifndef ND_STRINGTOINT64
#define ND_STRINGTOINT64( s )		NDShareBaseGlobal::strToInt64( s )
#endif

#ifndef ND_STRINGTOUINT64
#define ND_STRINGTOUINT64( s )		NDShareBaseGlobal::strToUint64( s )	
#endif

#ifndef ND_STRINGTOFLOAT32
#define ND_STRINGTOFLOAT32( s )		NDShareBaseGlobal::strToFloat32( s )	
#endif

#ifndef ND_STRINGTOFLOAT64
#define ND_STRINGTOFLOAT64( s )		NDShareBaseGlobal::strToFloat64( s )
#endif


#ifndef ND_INT32TOSTRING
#define ND_INT32TOSTRING( v, s, n )			NDShareBaseGlobal::int32ToStr( v, s, n )
#endif

#ifndef ND_UINT32TOSTRING
#define ND_UINT32TOSTRING( v, s, n )		NDShareBaseGlobal::uint32Tostr( v, s, n )	
#endif

#ifndef ND_INT64TOSTRING
#define ND_INT64TOSTRING( v, s, n )			NDShareBaseGlobal::int64ToStr( v, s, n )	
#endif

#ifndef ND_UINT64TOSTRING
#define ND_UINT64TOSTRING( v, s, n )		NDShareBaseGlobal::uint64ToStr( v, s, n )	
#endif

#ifndef ND_FLOAT32TOSTRING
#define ND_FLOAT32TOSTRING( v, s, n, j )	NDShareBaseGlobal::float32ToStr( v, s, n, j )	
#endif

#ifndef ND_FLOAT64TOSTRING
#define ND_FLOAT64TOSTRING( v, s, n, j )	NDShareBaseGlobal::float64ToStr( v, s, n, j )	
#endif


_NDSHAREBASE_END
#endif

