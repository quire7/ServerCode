#ifndef PRIVATE_SERVICE_HEAD_HEAD_FILE
#define PRIVATE_SERVICE_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////


//ƽ̨�ļ�;
#include "..\..\���������\��Ϸ����\MatchServiceInterface.h"


//////////////////////////////////////////////////////////////////////////////////
//��������

//��������
#ifndef PRIVATE_SERVICE_CLASS
	#ifdef  PRIVATE_SERVICE_DLL
		#define PRIVATE_SERVICE_CLASS _declspec(dllexport)
	#else
		#define PRIVATE_SERVICE_CLASS _declspec(dllimport)
	#endif
#endif

//ģ�鶨��
#ifndef _DEBUG
	#define PRIVATE_SERVICE_DLL_NAME	TEXT("PrivateService.dll")			//�������
#else
	#define PRIVATE_SERVICE_DLL_NAME	TEXT("PrivateServiceD.dll")			//�������
#endif

//////////////////////////////////////////////////////////////////////////////////
//��������
class PriaveteGame;

//��������;
#define PRIVATE_SERVICE_CREATE_NAME	"CreatePrivateServiceManager"			//��������;

//��Ϸ����
//DECLARE_MODULE_HELPER(MatchServiceManager, PRIVATE_SERVICE_DLL_NAME, PRIVATE_SERVICE_CREATE_NAME)

//////////////////////////////////////////////////////////////////////////////////

#endif