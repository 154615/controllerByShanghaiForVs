#include "JDQ.h" 

/*
�͵�ƽ�̵�������
�̵���1�����PE10
�̵���2�����PE8
�̵���3�����PE14
�̵���4�����PE12
�̵���5�����PC6
�̵���6�����PG8
*/
void JDQ_Init(void)
{   
    GPIO_InitTypeDef  GPIO_InitStructure;

    //JDQ1-JDQ4
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOG, ENABLE);//ʹ��GPIOEʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_10 | GPIO_Pin_12 |GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;     //��ͨ���ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;    //����
    GPIO_Init(GPIOE, &GPIO_InitStructure);            //��ʼ��GPIO

    GPIO_SetBits(GPIOE,GPIO_Pin_8);                   //����
    GPIO_SetBits(GPIOE,GPIO_Pin_10);                  //����
    GPIO_SetBits(GPIOE,GPIO_Pin_12);                  //����
    GPIO_SetBits(GPIOE,GPIO_Pin_14);                  //����

    //JDQ5
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;     //��ͨ���ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;    //����
    GPIO_Init(GPIOC, &GPIO_InitStructure);            //��ʼ��GPIO

    GPIO_ResetBits(GPIOC,GPIO_Pin_6);                   //����

    //JDQ6
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;     //��ͨ���ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;    //����
    GPIO_Init(GPIOG, &GPIO_InitStructure);            //��ʼ��GPIO

    GPIO_ResetBits(GPIOG,GPIO_Pin_8);                   //����  
}






