#ifndef __USART2_H
#define __USART2_H
#include "stm32f4xx_conf.h"
#include "sys.h" 

#define USART2_REC_LEN  			100  	//�����������ֽ���
extern uint8_t  Usart2_rcv_flag;		//����2���յ�����
extern u8  USART2_RX_BUF[USART2_REC_LEN];   //���ջ��� 
extern u8 USART2_BUF_Index; 
void usart2_init(u32 bound);
void usart2_send_byte(uint8_t ch);				//����2���ͺ���
	
#endif


