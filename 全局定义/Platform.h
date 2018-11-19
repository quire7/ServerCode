#ifndef PLATFORM_HEAD_FILE
#define PLATFORM_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//�����ļ�

//�����ļ�
#include "Macro.h"
#include "Define.h"

//�ṹ�ļ�
#include "Struct.h"
#include "Packet.h"
#include "Property.h"
#include "DataStream.h"

//ģ���ļ�
#include "Array.h"
#include "Module.h"
#include "PacketAide.h"
#include "ServerRule.h"
#include "RightDefine.h"

//////////////////////////////////////////////////////////////////////////////////

//����汾
#define VERSION_PLAZA				PROCESS_VERSION(0,0,1)				//�����汾
#define VERSION_MOBILE_ANDROID		PROCESS_VERSION(0,0,1)				//�ֻ��汾
#define VERSION_MOBILE_IOS			PROCESS_VERSION(0,0,1)				//�ֻ��汾

//�汾����
#define VERSION_EFFICACY			0									//Ч��汾

//////////////////////////////////////////////////////////////////////////////////
//ƽ̨����
const TCHAR szProduct[]=TEXT("��ǨӢ˶����ƽ̨");							//��Ʒ����
const TCHAR szProductKey[]=TEXT("YSQPGamePlatform");					//��Ʒ����

//���ݿ���
const TCHAR szPlatformDB[]=TEXT("fkplatformdb");						//ƽ̨���ݿ�
const TCHAR szAccountsDB[]=TEXT("fkaccountsdb");						//�û����ݿ�
const TCHAR	szTreasureDB[]=TEXT("fktreasuredb");						//�Ƹ����ݿ�
const TCHAR	szExerciseDB[]=TEXT("fkexercisedb");						//��ϰ���ݿ�


//////////////////////////////////////////////////////////////////////////////////


#endif