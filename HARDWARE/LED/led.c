#include "led.h" 

//��ʼ��PE8��PE9Ϊ�����    
//LED IO��ʼ��
void LED_Init(void)
{    	 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//ʹ��GPIOEʱ��

	//PB0,PC0��ʼ������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��
	
	GPIO_SetBits(GPIOE,GPIO_Pin_8);	//GPIOE8���øߣ�����
	GPIO_SetBits(GPIOE,GPIO_Pin_9);	//GPIOE9���øߣ�����

}






