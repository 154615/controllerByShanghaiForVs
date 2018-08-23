#ifndef __MAIN_H
#define __MAIN_H	 
#include "sys.h" 
#include "stdio.h"

void Test(void);    //���Գ���
extern RTC_TimeTypeDef RTC_TimeStruct;
extern RTC_DateTypeDef RTC_DateStruct;
extern uint8_t DebugFlag;
extern u8 Config_Flag;      //��Ƭ���ѱ�ʶ��־
extern u8 WorkStatus;       //����״̬

void init_Parameter(void);	//��ʼ�����в���

//��PCͨ��������غ���
#define	COMM_HEAD1	0x73	//start
#define	COMM_HEAD2	0x74
#define	COMM_HEAD3	0x61
#define	COMM_HEAD4	0x72
#define	COMM_HEAD5	0x74
#define COMM_END1	0x65	//end
#define COMM_END2	0x6E
#define COMM_END3	0x64
#define ID_XIAOCHE	100
#define ID_DALIANG  200
#define ID_DEBUG		300
#define BASEADDR_ROW	500
#define BASEADDR_COL_FIRST	59
#define BASEADDR_COL_LAST	63
#define BASEADDR_COLUMN_BIT	100

#define PRECISION	30		//30���������
#define DALIANG_TIMEOUT	600		//�����˶���ʱ
#define XIAOCHE_TIMEOUT 200		//С���˶���ʱ

u8 WIFI_Dealwith(DEVICE com);	//WIFI���ݴ���
uint8_t WIFI_Stop(uint8_t com);	//WIFI���ݴ�����ͣ����
uint8_t PC_Stop(void);			//�ж��Ƿ������λ����ָֹ��,����1����ֹɨ��

//ֲ��ɨ��������ز���
#define CAMERATIME	1							//��λɨ�������ͣʱ��

extern u8 BreakFlag;              //�ж�ִ�б�־
extern uint8_t paulseStyle;//���������ʽ


extern uint16_t Scan_Interval;		//ɨ��ʱ��������λ����


extern uint16_t Scan_Times;				//����ɨ�����
extern uint8_t	Scan_Day;					//ɨ�����ڣ����ڱ仯��ɨ���������

void SendBatteryVoltage(uint8_t com);//����Ŧ�۵�ص�ѹֵ
extern uint8_t  Scan_SW;						 //����ɨ�迪��
extern uint8_t  PCBreakFlag;			 //��λ����ֹ��ǰ����

void PlanTask(void);			  //�ж�ɨ��ƻ�����
uint8_t Scan_Full(void);		//ȫ��ɨ�� 
uint8_t Scan_Part(void);		//����ɨ�� 
uint8_t Scan_Row(uint8_t row);//ɨ��ָ���� 
uint8_t Scan_Column(uint16_t column);//ɨ��ָ����

//С���ʹ������ƺ���
extern volatile uint8_t XiaoChe_Now_Direction;	//0x01��ǰ��0x02���0x03ֹͣ��04���������
extern volatile uint32_t XiaoChe_Now_Position;
extern volatile uint8_t DaLiang_Now_Direction;
extern volatile uint32_t DaLiang_Now_Position;
extern uint8_t NowSpeed; //��ǰ��������ٶ�
uint8_t MotorMove(uint8_t ID,uint8_t dir);
uint8_t MotorToPosition(uint8_t ID,uint32_t pos);
uint8_t MotorToZero(uint8_t ID);
uint8_t ReadStatus(uint8_t ID);
uint8_t SetXiaoChe_5V_Level(uint8_t levelmode);
uint8_t SetXiaoChe_0V_Level(void);
uint8_t CommTest(uint8_t ID);
uint8_t ChangeSpeed(uint8_t speed);

#endif


