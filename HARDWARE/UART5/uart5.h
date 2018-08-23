#ifndef __UART5_H
#define __UART5_H
#include "stm32f4xx_conf.h"
#include "sys.h" 

#define UART5_REC_LEN  			100  	//�����������ֽ���

extern uint8_t  Uart5_rcv_flag;			//����5���յ�����
extern u8  UART5_RX_BUF[UART5_REC_LEN]; //���ջ���,���UART5_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u8 UART5_BUF_Index; 
void uart5_init(u32 bound);

void uart5_send_byte(uint8_t ch);			//����5���ͺ���

#endif


