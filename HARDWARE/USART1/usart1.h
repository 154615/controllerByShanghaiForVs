#ifndef __USART1_H
#define __USART1_H
#include "stm32f4xx_conf.h"
#include "sys.h" 

#define USART1_REC_LEN  			20  	//�����������ֽ��� 20

extern u8  USART1_RX_BUF[USART1_REC_LEN];   //���ջ���
void usart1_init(u32 bound);
void usart1_send_byte(uint8_t ch);			//����1����

#endif


