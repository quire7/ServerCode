#pragma once



#ifndef VC_EXTRALEAN

#define VC_EXTRALEAN		// �� Windows ͷ���ų�����ʹ�õ�����

#endif



// ���������ʹ��������ָ����ƽ̨֮ǰ��ƽ̨�����޸�����Ķ��塣

// �йز�ͬƽ̨����Ӧֵ��������Ϣ����ο� MSDN��

#ifndef WINVER                          // ָ��Ҫ������ƽ̨�� Windows Vista��
#define WINVER 0x0600           // ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
#endif

#ifndef _WIN32_WINNT            // ָ��Ҫ������ƽ̨�� Windows Vista��
#define _WIN32_WINNT 0x0600     // ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
#endif

#ifndef _WIN32_WINDOWS          // ָ��Ҫ������ƽ̨�� Windows 98��
#define _WIN32_WINDOWS 0x0410 // ����ֵ����Ϊ�ʵ���ֵ���������� Windows Me ����߰汾��
#endif

#ifndef _WIN32_IE                       // ָ��Ҫ������ƽ̨�� Internet Explorer 7.0��
#define _WIN32_IE 0x0700        // ����ֵ����Ϊ��Ӧ��ֵ���������� IE �������汾��
#endif



#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// ĳЩ CString ���캯����Ϊ��ʽ��



#include <afxwin.h>         // MFC ��������ͱ�׼���

#include <afxext.h>         // MFC ��չ



#ifndef _AFX_NO_OLE_SUPPORT

#include <afxole.h>         // MFC OLE ��

#include <afxodlgs.h>       // MFC OLE �Ի�����

#include <afxdisp.h>        // MFC �Զ�����

#endif // _AFX_NO_OLE_SUPPORT



#include <afxdtctl.h>		// MFC �� Internet Explorer 4 �����ؼ���֧��

#ifndef _AFX_NO_AFXCMN_SUPPORT

#include <afxcmn.h>			// MFC �� Windows �����ؼ���֧��

#endif // _AFX_NO_AFXCMN_SUPPORT



//////////////////////////////////////////////////////////////////////////



//���ͷ�ļ�


#include "..\..\���������\��Ϸ����\GameServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////
//���Ӵ���

#ifndef _DEBUG
	#ifndef _UNICODE
		#pragma comment (lib,"../../���ӿ�/Ansi/ServiceCore.lib")
		#pragma comment (lib,"../../���ӿ�/Ansi/GameService.lib")
		#pragma comment (lib,"../../���ӿ�/Ansi/KernelEngine.lib")
	#else
		#pragma comment (lib,"../../���ӿ�/Unicode/ServiceCore.lib")
		#pragma comment (lib,"../../���ӿ�/Unicode/GameService.lib")
		#pragma comment (lib,"../../���ӿ�/Unicode/KernelEngine.lib")
	#endif
#else
	#ifndef _UNICODE
		#pragma comment (lib,"../../���ӿ�/Ansi/ServiceCoreD.lib")
		#pragma comment (lib,"../../���ӿ�/Ansi/GameServiceD.lib")
		#pragma comment (lib,"../../���ӿ�/Ansi/KernelEngineD.lib")
	#else
		#pragma comment (lib,"../../���ӿ�/Unicode/ServiceCoreD.lib")
		#pragma comment (lib,"../../���ӿ�/Unicode/GameServiceD.lib")
		#pragma comment (lib,"../../���ӿ�/Unicode/KernelEngineD.lib")
	#endif
#endif

//////////////////////////////////////////////////////////////////////////



