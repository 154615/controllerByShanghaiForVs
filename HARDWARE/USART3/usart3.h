#ifndef __USART3_H
#define __USART3_H
#include "stm32f4xx_conf.h"
#include "sys.h" 

#define USART3_REC_LEN  			20  	//�����������ֽ��� 20
extern uint8_t  Usart3_rcv_flag;			//����3���յ�����
extern u8  USART3_RX_BUF[USART3_REC_LEN]; 	//���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 

void usart3_send_byte(uint8_t ch);			//����3���ͺ���
void usart3_init(u32 bound);

#endif


