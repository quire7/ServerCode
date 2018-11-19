#ifndef _GMEM_POOL_H_
#define _GMEM_POOL_H_

/*************************************************************************
	Dynamic Library import / export control conditional
	(Define GMEMPOOL_EXPORTS to export symbols, else they are imported)
*************************************************************************/
#if defined( WIN32 ) && !defined(GMEMPOOL_STATIC)
#   ifdef GMEMPOOL_EXPORTS
#       define GMEMPOOLEXPORT __declspec(dllexport)
#   else
#       define GMEMPOOLEXPORT __declspec(dllimport)
#   endif
#else
#       define GMEMPOOLEXPORT
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

#ifdef	__cplusplus
extern "C" {
#endif

GMEMPOOLEXPORT typedef void GMemPool;

GMEMPOOLEXPORT void STDCALL GInitMemoryDLL();

GMEMPOOLEXPORT void STDCALL GCloseMemoryDLL();

GMEMPOOLEXPORT GMemPool* STDCALL GNewMemory();

GMEMPOOLEXPORT void STDCALL GDestroyMemory(GMemPool *pPool);

GMEMPOOLEXPORT void* STDCALL GMalloc(GMemPool *pPool, int size);

GMEMPOOLEXPORT void STDCALL GClearPool(GMemPool *pPool);


#ifdef	__cplusplus
}
#endif

#endif //_GMEM_POOL_H_
