#ifndef __USART6_H
#define __USART6_H
#include "stm32f4xx_conf.h"
#include "sys.h" 
#include "stdio.h"

#define USART6_RCV_LEN  			30  	//�����������ֽ��� 20

extern uint8_t Usart6_rcv_buf[USART6_RCV_LEN];     //���ջ���
extern uint8_t Usart6_rcv_index;

void usart6_init(u32 bound);
void usart6_send_byte(uint8_t ch);			//����6����

#endif

