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
	uint8_t cmd = 0xff;
	uint8_t * dataPtr = (void *)0;
	DEVICE device = wifi;
	
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

	BreakFlag = 0;
	WorkStatus = 0;
	BeepOne();

	while (1)
	{
		//------------------����������ݴ���
		if (getReciveLen(device) != 0 && oldLen == getReciveLen(device))
		{
			dataPtr = getCmdFrame(device, &cmd);
			handlerFrame(device, cmd, dataPtr);
			oldLen = 0;
			clearReciveBuf(device);
		}
		else
		{
			oldLen = getReciveLen(device);
		}
		if (Scan_SW)
		{
			PlanTask();
		}
		delay_ms(12);//�����޸�

	}
}

