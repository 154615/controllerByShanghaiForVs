#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h" 

/*����ķ�ʽ��ͨ��ֱ�Ӳ����⺯����ʽ��ȡIO*/
#define KEY3 		GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_7) //PG7
#define KEY4 		GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_6) //PG6
#define KEY5 		GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_5) //PG5
#define KEY6 		GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_4) //PG4

#define KEY3_PRES	3
#define KEY4_PRES	4
#define KEY5_PRES	5
#define KEY6_PRES	6

void KEY_Init(void);	//IO��ʼ��
u8 KEY_Scan(u8);  		//����ɨ�躯��	

#endif
