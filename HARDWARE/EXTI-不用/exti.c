#include "exti.h"
#include "delay.h" 
#include "led.h" 
#include "key.h"
#include "JDQ.h"

/*
KEY1 -- PC6
KEY2 -- PG8
KEY3 -- PG7		С�������жϣ���ӦJDQ4
KEY4 -- PG6		С��ǰ���жϣ���ӦJDQ3
KEY5 -- PG5		���������жϣ���ӦJDQ2
KEY6 -- PG4		����ǰ���жϣ���ӦJDQ1
*/

//�ⲿ�ж�4�������
void EXTI4_IRQHandler(void)
{
	delay_ms(10);	//����
	if(KEY6 == 0)
	{
		JDQ1_OFF; 	//����ǰ���̵����ر� 
	}		 
	 EXTI_ClearITPendingBit(EXTI_Line4); //���LINE4�ϵ��жϱ�־λ 
}	
//�ⲿ�ж�2�������
void EXTI9_5_IRQHandler(void)
{
	delay_ms(10);	//����
	
	if(KEY5 == 0)
	{
		JDQ2_OFF; 	//����ǰ���̵����ر� 
	}		 
	EXTI_ClearITPendingBit(EXTI_Line5); //���LINE5�ϵ��жϱ�־λ 	
	
	if(KEY4 == 0)
	{
		JDQ3_OFF; 	//����ǰ���̵����ر� 
	}		 
	EXTI_ClearITPendingBit(EXTI_Line6); //���LINE6�ϵ��жϱ�־λ 		
	
	if(KEY3 == 0)
	{
		JDQ4_OFF; 	//����ǰ���̵����ر� 
	}		 
	EXTI_ClearITPendingBit(EXTI_Line7); //���LINE7�ϵ��жϱ�־λ 	
}

	   
/*
�ⲿ�жϳ�ʼ������
*/
void EXTIX_Init(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	
	KEY_Init(); //������Ӧ��IO�ڳ�ʼ��
 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//ʹ��SYSCFGʱ��
 
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, EXTI_PinSource4);//PG4 ���ӵ��ж���4
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, EXTI_PinSource5);//PG5 ���ӵ��ж���5
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, EXTI_PinSource6);//PG6 ���ӵ��ж���6
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, EXTI_PinSource7);//PG7 ���ӵ��ж���7
	
	/* ����EXTI_Line0 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line4 | EXTI_Line5 | EXTI_Line6 | EXTI_Line7;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�½��ش��� 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//ʹ��LINE
	EXTI_Init(&EXTI_InitStructure);//����
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;//�ⲿ�ж�4
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);//����

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;//�ⲿ�ж�2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);//����
	   
}
