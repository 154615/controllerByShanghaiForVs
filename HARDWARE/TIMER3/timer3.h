#ifndef _TIMER3_H
#define _TIMER3_H
#include "sys.h"

#define	COMM_TIMEOUT	5		//5msͨ�ų�ʱ

extern uint16_t ledCount;			//��˸�����ܶ�ʱ��

extern uint8_t  Usart2_rcv_flag;			//����2���յ�����
extern uint16_t Usart2_receive_timer;	//����2���ճ�ʱ��ʱ��
extern uint8_t  Usart2_receive_on;		//����2���ն�ʱ����

extern uint8_t  Uart5_rcv_flag;			  //����5���յ�����
extern uint16_t Uart5_receive_timer;	//����5���ճ�ʱ��ʱ��
extern uint8_t  Uart5_receive_on;		  //����5���ն�ʱ����

void TIM3_Int_Init(u16 arr,u16 psc);

#endif
