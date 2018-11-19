#ifndef SERVICE_CORE_HEAD_FILE
#define SERVICE_CORE_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//�����ļ�

//MFC �ļ�
#include <Nb30.h>
#include <Afxmt.h>
#include <AtlBase.h>

//ϵͳ�ļ�
#include "..\..\ȫ�ֶ���\Platform.h"

//////////////////////////////////////////////////////////////////////////////////
//��������

//��������
#ifndef SERVICE_CORE_CLASS
	#ifdef  SERVICE_CORE_DLL
		#define SERVICE_CORE_CLASS _declspec(dllexport)
	#else
		#define SERVICE_CORE_CLASS _declspec(dllimport)
	#endif
#endif

//ģ�鶨��
#ifndef _DEBUG
	#define SERVICE_CORE_DLL_NAME	TEXT("ServiceCore.dll")			//�������
#else
	#define SERVICE_CORE_DLL_NAME	TEXT("ServiceCoreD.dll")		//�������
#endif

//////////////////////////////////////////////////////////////////////////////////
//�����ļ�

#ifndef SERVICE_CORE_DLL
	#include "YSThread.h"
	#include "YSRegKey.h"
	#include "YSEncrypt.h"
	#include "YSService.h"
	#include "YSIniData.h"
	#include "YSDataQueue.h"
	#include "YSDataLocker.h"
	#include "YSCommandLine.h"
	#include "YSCharUtils.h"
	#include "YSAutoBuffer.h"
	#include "RedisInstance.h"
	#include "json.h"
	#include "sha1.hpp"
	#include "base64.h"
	#include "YSAllocationID.h"
#endif



//////////////////////////////////////////////////////////////////////////////////

#endif