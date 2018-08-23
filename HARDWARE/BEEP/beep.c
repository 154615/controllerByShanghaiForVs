#include "beep.h" 
#include "delay.h"

/*
��ʼ��PA1Ϊ�����		    
BEEP IO��ʼ��
*/
void BEEP_Init(void)
{   
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;     //��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  //����
  GPIO_Init(GPIOA, &GPIO_InitStructure);            //��ʼ��GPIO
	
  GPIO_ResetBits(GPIOA,GPIO_Pin_1);                //��������Ӧ����GPIOA1
}

//��һ��
void BeepOne(void)
{
	BEEP_ON;
	delay_ms(100);
	BEEP_OFF;
}

//������
void BeepTwo(void)
{
	BeepOne(); 
	delay_ms(500);
	BeepOne(); 
}

//������һ��
void BeepThree(void)
{
	BEEP_ON;
	delay_ms(1000);
	BEEP_OFF; 
	delay_ms(300);
	BEEP_ON; 
	delay_ms(300);
	BEEP_OFF; 
	delay_ms(300);
	BEEP_ON; 
	delay_ms(1000);
	BEEP_OFF;  
}



