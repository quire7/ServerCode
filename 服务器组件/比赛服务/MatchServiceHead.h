#ifndef MATCH_SERVICE_HEAD_HEAD_FILE
#define MATCH_SERVICE_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////

//ƽ̨�ļ�;
#include "..\..\���������\��Ϸ����\MatchServiceInterface.h"


//////////////////////////////////////////////////////////////////////////////////
//��������;

//��������;
#ifndef MATCH_SERVICE_CLASS
	#ifdef  MATCH_SERVICE_DLL
		#define MATCH_SERVICE_CLASS _declspec(dllexport)
	#else
		#define MATCH_SERVICE_CLASS _declspec(dllimport)
	#endif
#endif

//ģ�鶨��;
#ifndef _DEBUG
	#define MATCH_SERVICE_DLL_NAME	TEXT("MatchService.dll")			//�������
#else
	#define MATCH_SERVICE_DLL_NAME	TEXT("MatchServiceD.dll")			//�������
#endif

//////////////////////////////////////////////////////////////////////////////////
//��������;
class CLockTimeMatch;

//��������;
#define MATCH_SERVICE_CREATE_NAME	"CreateMatchServiceManager"			//��������;

//////////////////////////////////////////////////////////////////////////////////
//��Ϸ����
DECLARE_MODULE_HELPER(MatchServiceManager, MATCH_SERVICE_DLL_NAME, MATCH_SERVICE_CREATE_NAME)

//////////////////////////////////////////////////////////////////////////////////

#endif