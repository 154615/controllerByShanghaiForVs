#include "sys.h"
#include "delay.h"
#include "usart1.h"
#include "usart2.h"
#include "usart3.h"
#include "uart5.h"
#include "usart6.h"
#include "led.h"
#include "beep.h"
#include "timer3.h"
#include "main.h"
#include "rtc.h"
#include "adc.h"
#include "design.h"

RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;

int main(void)
{
	DebugFlag = 0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	My_RTC_Init();				//��ʼ��RTC
	delay_init(168);			//��ʱ��ʼ�� 

	wifiInit();
	loraInit();
	Adc_Init();
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	BEEP_Init();					//��������ʼ��
	TIM3_Int_Init(10 - 1, 8400 - 1);//��ʱ��ʱ��84M����Ƶϵ��8400������84M/8400=10Khz�ļ���Ƶ�ʣ�����10��Ϊ1ms

	init_Parameter();

	Usart2_rcv_flag = 0;		//����2���յ�����
	Usart2_receive_timer = 0;
	Usart2_receive_on = 0;

	Uart5_rcv_flag = 0;			//����5���յ�����
	Uart5_receive_timer = 0;	//����5���ճ�ʱ��ʱ��
	Uart5_receive_on = 0;		//����5���ն�ʱ����

	BreakFlag = 0;
	WorkStatus = 0;
	BeepOne();

	//Test();
	while (1)
	{
		//1-������WIFIͨ��
		if (Usart2_rcv_flag)
		{
			WorkStatus = 1;
			Usart2_rcv_flag = 0;
			WIFI_Dealwith(2);
			WorkStatus = 0;
		}

		//2-��������̫��ͨ��
		if (Uart5_rcv_flag)
		{
			WorkStatus = 1;
			Uart5_rcv_flag = 0;
			WIFI_Dealwith(5);
			WorkStatus = 0;
		}

		//3-ִ�мƻ��������ʱ�����㣬��������ɨ�迪��
		if (Scan_SW)
			PlanTask();

	}
}

