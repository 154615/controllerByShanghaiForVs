#ifndef __BEEP_H
#define __BEEP_H	 
#include "sys.h" 

//BEEP�˿ڶ���,PA13

#define BEEP_ON     GPIO_SetBits(GPIOA,GPIO_Pin_1); 
#define BEEP_OFF    GPIO_ResetBits(GPIOA,GPIO_Pin_1); 
void BEEP_Init(void);//��ʼ��
void BeepOne(void);//��һ��
void BeepTwo(void);//������
void BeepThree(void);//������һ��
#endif

















