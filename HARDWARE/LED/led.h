#ifndef __LED_H
#define __LED_H
#include "sys.h"

//LED�˿ڶ���
#define LED1_ON 	GPIO_ResetBits(GPIOE,GPIO_Pin_8);	//PE8���õͣ��Ƶ���
#define LED1_OFF 	GPIO_SetBits(GPIOE,GPIO_Pin_8);		//PE8���øߣ��Ƶ���
#define LED1_BLINK	GPIO_ToggleBits(GPIOE,GPIO_Pin_8);//LED1�Ƶ���˸

#define LED2_ON 	GPIO_ResetBits(GPIOE,GPIO_Pin_9);	//PE9���õͣ�������
#define LED2_OFF 	GPIO_SetBits(GPIOE,GPIO_Pin_9);		//PE9���øߣ�������
#define LED2_BLINK	GPIO_ToggleBits(GPIOE,GPIO_Pin_9);//LED2������˸

void LED_Init(void);//��ʼ��	

#endif
