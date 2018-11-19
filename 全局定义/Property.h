#ifndef PROPERTY_HEAD_FILE
#define PROPERTY_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////

//���з�Χ
#define PT_ISSUE_AREA_WEB			0x01								//�̳ǵ���
#define PT_ISSUE_AREA_GAME			0x02								//��Ϸ����
#define PT_ISSUE_AREA_SERVER		0x04								//�������

//ʹ�÷�Χ
#define PT_SERVICE_AREA_MESELF		0x0001								//�Լ���Χ
#define PT_SERVICE_AREA_PLAYER		0x0002								//��ҷ�Χ
#define PT_SERVICE_AREA_LOOKON		0x0004								//�Թ۷�Χ

//��������
#define PT_TYPE_ERROR               0                                   //�������� 
#define PT_TYPE_PROPERTY            1	                                //��������  
#define PT_TYPE_PRESENT             2                                   //��������

#define PROPERTY_ID_VIP1_CARD		22									//�»�Ա����
#define PROPERTY_ID_VIP2_CARD		23									//���Ȼ�Ա����
#define PROPERTY_ID_VIP3_CARD		24  								//�����Ա���� 

//////////////////////////////////////////////////////////////////////////////////

//������Ϣ
struct tagPropertyInfo
{
	//������Ϣ
	WORD							wIndex;								//���߱�ʶ
	WORD							wDiscount;							//��Ա�ۿ�
	WORD							wIssueArea;							//������Χ

	//���ۼ۸�
	SCORE							lPropertyGold;						//���߽��
	DOUBLE							dPropertyCash;						//���߼۸�

	//��������
	SCORE							lSendLoveLiness;					//��������
	SCORE							lRecvLoveLiness;					//��������
};


//��������
struct tagPropertyAttrib
{
	WORD							wIndex;								//���߱�ʶ
	WORD                            wPropertyType;                      //��������
	WORD							wServiceArea;						//ʹ�÷�Χ
	TCHAR                           szMeasuringunit[8];					//������λ 
	TCHAR							szPropertyName[32];					//��������
	TCHAR							szRegulationsInfo[256];				//ʹ����Ϣ
};

//��������
struct tagPropertyItem
{
	tagPropertyInfo					PropertyInfo;						//������Ϣ
	tagPropertyAttrib				PropertyAttrib;						//��������
};

//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif