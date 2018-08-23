/*
Timer3��Ϊϵͳ��ʱ������ʱ����1ms
*/
#include "timer3.h"
#include "usart2.h"
#include "uart5.h"
#include "led.h"
#include "init_Parameter.h"

uint16_t ledCount = 0;			//��˸�����ܶ�ʱ��
uint16_t Laser1_send_timer;		//����ģ��1�������ʱ��
uint16_t Laser2_send_timer;		//����ģ��2�������ʱ��
uint16_t Laser1_receive_timer;	//����ģ��1�������ʱ��
uint16_t Laser2_receive_timer;	//����ģ��2�������ʱ��

uint16_t Uart5_receive_timer;	//����5���ճ�ʱ��ʱ��
uint8_t  Uart5_receive_on;		//����5���ն�ʱ����

//ͨ�ö�ʱ��3�жϳ�ʼ��
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ��3!
void TIM3_Int_Init(u16 arr, u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);             ///ʹ��TIM3ʱ��

	TIM_TimeBaseInitStructure.TIM_Period = arr; 	                //�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;                    //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;   //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);              //��ʼ��TIM3

	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);                        //����ʱ��3�����ж�
	TIM_Cmd(TIM3, ENABLE); //ʹ�ܶ�ʱ��3

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;                   //��ʱ��3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;      //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;             //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}

//��ʱ��3�жϷ�����
void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET) //����ж�
	{
		//1-��������˸
		if (++ledCount >= 500)	//500ms��˸
		{
			ledCount = 0;
			LED1_BLINK;	//LED1�Ƶ���˸
			RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
			RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);

			//������ڷ����仯�������ɨ�����
			if (Scan_Times != 0)
			{
				if (RTC_DateStruct.RTC_Date != Scan_Day)
				{
					Scan_Times = 0;	//����ɨ�����
				}
			}
		}

		////2-����תWIFIͨ��ģ�飬��ʱ��һ�����ݽ���ǿ�ƽ���
		//if(Usart2_receive_on)
		//{
		//	Usart2_receive_timer++;
		//	if(Usart2_receive_timer >= COMM_TIMEOUT)
		//	{
		//		Usart2_receive_on = 0;
		//		Usart2_receive_timer = 0;
		//		Usart2_rcv_flag = 1;
		//	}
		//}
		//
		////3-��̫��ͨ��ģ�飬��ʱ��һ�����ݽ���ǿ�ƽ���
		//if(Uart5_receive_on)
		//{
		//	Uart5_receive_timer++;
		//	if(Uart5_receive_timer >= COMM_TIMEOUT)
		//	{
		//		Uart5_receive_on = 0;
		//		Uart5_receive_timer = 0;
		//		Uart5_rcv_flag = 1;
		//	}
		//}

	}
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //����жϱ�־λ
}
