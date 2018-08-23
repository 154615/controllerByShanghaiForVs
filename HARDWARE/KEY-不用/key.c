#include "key.h"
#include "delay.h" 

/*
������ʼ������
KEY1 -- PC6
KEY2 -- PG8
KEY3 -- PG7		С�������жϣ���ӦJDQ4
KEY4 -- PG6		С��ǰ���жϣ���ӦJDQ3
KEY5 -- PG5		���������жϣ���ӦJDQ2
KEY6 -- PG4		����ǰ���жϣ���ӦJDQ1
*/
void KEY_Init(void)
{
	
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);//ʹ��GPIOC,GPIOGʱ��
 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7; //KEY3-6��Ӧ����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;        //��ͨ����ģʽ
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  //100M
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //����
    GPIO_Init(GPIOG, &GPIO_InitStructure);

}

/*����������
���ذ���ֵ
mode:0,��֧��������;1,֧��������;
0��û���κΰ�������
3��KEY3���� 
4��KEY4����
5��KEY5���� 
6��KEY6���� 
ע��˺�������Ӧ���ȼ�,KEY1>KEY2>KEY3>KEY4>KEY5>KEY6
mode=0��֧����������mode=1֧��������
*/
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1; //�������ɿ���־
	if(mode)key_up=1;   //֧������
    
	if(key_up&&(KEY3==0||KEY4==0||KEY5==0||KEY6==0))
	{
		delay_ms(10);//ȥ���� 
		key_up=0;
		if(KEY3==0)return 3;
		else if(KEY4==0)return 4;
		else if(KEY5==0)return 5;
		else if(KEY6==0)return 6;
	}
    else if(KEY3==1 && KEY4==1 && KEY5==1 && KEY6==1)
    {
        key_up=1;
 	}
    
 	return 0;// �ް�������
}

