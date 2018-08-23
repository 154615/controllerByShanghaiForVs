#ifndef __JDQ_H
#define __JDQ_H	 
#include "sys.h" 

/*
�͵�ƽ�̵�������
�̵���1�����PE10������ǰ��
�̵���2�����PE8 ����������
�̵���3�����PE14��С��ǰ��
�̵���4�����PE12��С������
�̵���5�����PC6�� С����բ���رռ̵�������
�̵���6�����PG8�� ���5V�������ź�
*/

#define JDQ1_ON    GPIO_ResetBits(GPIOE,GPIO_Pin_10); 
#define JDQ1_OFF   GPIO_SetBits(GPIOE,GPIO_Pin_10); 

#define JDQ2_ON    GPIO_ResetBits(GPIOE,GPIO_Pin_8); 
#define JDQ2_OFF   GPIO_SetBits(GPIOE,GPIO_Pin_8); 

#define JDQ3_ON    GPIO_ResetBits(GPIOE,GPIO_Pin_14); 
#define JDQ3_OFF   GPIO_SetBits(GPIOE,GPIO_Pin_14); 

#define JDQ4_ON    GPIO_ResetBits(GPIOE,GPIO_Pin_12); 
#define JDQ4_OFF   GPIO_SetBits(GPIOE,GPIO_Pin_12); 

#define JDQ5_ON   GPIO_SetBits(GPIOC,GPIO_Pin_6); 
#define JDQ5_OFF  GPIO_ResetBits(GPIOC,GPIO_Pin_6); 

#define JDQ6_ON   GPIO_SetBits(GPIOG,GPIO_Pin_8); 
#define JDQ6_OFF  GPIO_ResetBits(GPIOG,GPIO_Pin_8); 

void JDQ_Init(void);	//��ʼ��
		 				    
#endif

















