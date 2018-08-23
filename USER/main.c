#include "sys.h"
#include "delay.h"
#include "usart2.h"
#include "usart6.h"
#include "led.h"
#include "beep.h"
#include "timer3.h"
#include "rtc.h"
#include "adc.h"
#include "PlanTask.h"
#include "init_Parameter.h"


int main(void)
{
	u8 oldLen = 0;
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
	//Usart2_rcv_flag = 0;		//����2���յ�����
	//Usart2_receive_timer = 0;
	//Usart2_receive_on = 0;

	//Uart5_rcv_flag = 0;			//����5���յ�����
	//Uart5_receive_timer = 0;	//����5���ճ�ʱ��ʱ��
	//Uart5_receive_on = 0;		//����5���ն�ʱ����


	BreakFlag = 0;
	WorkStatus = 0;
	BeepOne();

	while (1)
	{
		//------------------����������ݴ���
		if (getReciveLen(wifi) != 0 && oldLen == getReciveLen(wifi))
		{
			//handlerFrame(wifi);
			handlerFrameTest(wifi);
			oldLen = 0;
			clearReciveBuf(wifi);
		}
		else
		{
			oldLen = getReciveLen(wifi);
		}
		if (Scan_SW)
		{
			PlanTask();
		}
		delay_ms(12);//�����޸�

	}
}

