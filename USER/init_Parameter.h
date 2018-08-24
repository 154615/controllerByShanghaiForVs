#ifndef INITPARAMS_H
#define INITPARAMS_H
#include "design.h"


extern uint8_t DebugFlag;
extern u8 WorkStatus;					//����״̬
extern u8 BreakFlag;					//�ж�ִ�б�־
extern uint8_t  PCBreakFlag;			//��λ����ֹ��ǰ����
extern LEVELMODE paulseStyle;				//���������ʽ
extern uint16_t Scan_Interval;			//ɨ��ʱ��������λ����
extern uint16_t Scan_Times;				//����ɨ�����
extern uint8_t  Scan_SW;				//����ɨ�迪��
extern uint8_t	Scan_Day;				//ɨ�����ڣ����ڱ仯��ɨ���������


extern RTC_TimeTypeDef RTC_TimeStruct;
extern RTC_DateTypeDef RTC_DateStruct;

extern volatile MOVECMD XiaoChe_Now_Direction;	//0x01��ǰ��0x02���0x03ֹͣ��04���������
extern volatile uint32_t XiaoChe_Now_Position;
extern volatile MOVECMD DaLiang_Now_Direction;
extern volatile uint32_t DaLiang_Now_Position;

extern void init_Parameter(void);


#endif
