#ifndef __SHARE_BASE_ND_C_LIB_FUNCTION_H__
#define __SHARE_BASE_ND_C_LIB_FUNCTION_H__

//#if defined(_MSC_VER) && (_MSC_VER >= 1400 )
//// Microsoft visual studio, version 2005 and higher.
//#define	_CRT_SECURE_NO_DEPRECATE
//#endif

#include <stdlib.h>

//#if defined(_MSC_VER) && (_MSC_VER >= 1400 )
//	// Microsoft visual studio, version 2005 and higher.
//	#define ND_VSNPRINTF	_vsnprintf_s
//	#define ND_SNPRINTF		_snprintf_s
//	#define ND_SNSCANF		_snscanf_s
//	#define ND_SSCANF		sscanf_s
//#elif defined(_MSC_VER) && (_MSC_VER >= 1200 )
//	// Microsoft visual studio, version 6 and higher.
//	//#pragma message( "Using _sn* functions." )
//	#define ND_VSNPRINTF	_vsnprintf
//	#define ND_SNPRINTF		_snprintf
//	#define ND_SNSCANF		_snscanf
//	#define ND_SSCANF		sscanf
//#elif defined(__GNUC__) && (__GNUC__ >= 3 )
//	// GCC version 3 and higher.s
//	//#warning( "Using sn* functions." )
//	#define ND_VSNPRINTF	vsnprintf
//	#define ND_SNPRINTF		snprintf
//	#define ND_SNSCANF		snscanf
//	#define ND_SSCANF		sscanf
//#else
//	#define ND_SSCANF		sscanf
//#endif


#if defined(_MSC_VER) && (_MSC_VER >= 1200 )
	// Microsoft visual studio, version 6 and higher.
	//#pragma message( "Using _sn* functions." )
	#define ND_VSNPRINTF	_vsnprintf
	#define ND_SNPRINTF		_snprintf
	#define ND_SNSCANF		_snscanf
	#define ND_SSCANF		sscanf
	#define ND_STRNICMP		_strnicmp

#elif defined(__GNUC__) && (__GNUC__ >= 3 )
	// GCC version 3 and higher.s
	//#warning( "Using sn* functions." )
	#define ND_VSNPRINTF	vsnprintf
	#define ND_SNPRINTF		snprintf
	#define ND_SNSCANF		snscanf
	#define ND_SSCANF		sscanf
	#define ND_STRNICMP		strncasecmp

#else

	#define ND_VSNPRINTF	vsnprintf
	#define ND_SNPRINTF		snprintf
	#define ND_SNSCANF		snscanf
	#define ND_SSCANF		sscanf
	#define ND_STRNICMP		strncasecmp

#endif



#endif
