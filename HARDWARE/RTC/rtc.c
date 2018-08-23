#include "rtc.h"
#include "led.h"
#include "delay.h"
#include "main.h"

//RTCʱ������
//hour,min,sec:Сʱ,����,����
//ampm:@RTC_AM_PM_Definitions  :RTC_H12_AM/RTC_H12_PM
//����ֵ:SUCEE(1),�ɹ�
//       ERROR(0),�����ʼ��ģʽʧ�� 
ErrorStatus RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm)
{
	RTC_TimeTypeDef RTC_TimeTypeInitStructure;
	
	RTC_TimeTypeInitStructure.RTC_Hours=hour;
	RTC_TimeTypeInitStructure.RTC_Minutes=min;
	RTC_TimeTypeInitStructure.RTC_Seconds=sec;
	RTC_TimeTypeInitStructure.RTC_H12=ampm;
	
	return RTC_SetTime(RTC_Format_BIN,&RTC_TimeTypeInitStructure);
	
}
//RTC��������
//year,month,date:��(0-99),��(1-12),��(0-31)
//week:����(1-7,0,�Ƿ�!)
//����ֵ:SUCEE(1),�ɹ�
//       ERROR(0),�����ʼ��ģʽʧ�� 
ErrorStatus RTC_Set_Date(u8 year,u8 month,u8 date,u8 week)
{
	
	RTC_DateTypeDef RTC_DateTypeInitStructure;
	RTC_DateTypeInitStructure.RTC_Date=date;
	RTC_DateTypeInitStructure.RTC_Month=month;
	RTC_DateTypeInitStructure.RTC_WeekDay=week;
	RTC_DateTypeInitStructure.RTC_Year=year;
	return RTC_SetDate(RTC_Format_BIN,&RTC_DateTypeInitStructure);
}

//RTC��ʼ��
//����ֵ:0,��ʼ���ɹ�;
//       1,LSE����ʧ��;
//       2,�����ʼ��ģʽʧ��;
u8 My_RTC_Init(void)
{
	RTC_InitTypeDef RTC_InitStructure;
	u16 retry=0X1FFF; 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//ʹ��PWRʱ��
	PWR_BackupAccessCmd(ENABLE);	//ʹ�ܺ󱸼Ĵ������� 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE);//���ñ���SRAMʱ��
	PWR_BackupRegulatorCmd(ENABLE); //���ñ���SRAM�ĵ͹�����ѹ�����Ա�������VBATģʽ������
    
	if(RTC_ReadBackupRegister(RTC_BKP_DR0)!=0x5050)		//�Ƿ��һ������?
	{
		RCC_LSEConfig(RCC_LSE_ON);//LSE ����    
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)	//���ָ����RCC��־λ�������,�ȴ����پ������
			{
			retry++;
			delay_ms(10);
			}
		if(retry==0)return 1;		//LSE ����ʧ��. 
			
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//����RTCʱ��(RTCCLK),ѡ��LSE��ΪRTCʱ��    
		RCC_RTCCLKCmd(ENABLE);	//ʹ��RTCʱ�� 

		RTC_InitStructure.RTC_AsynchPrediv = 0x7F;//RTC�첽��Ƶϵ��(1~0X7F)
		RTC_InitStructure.RTC_SynchPrediv  = 0xFF;//RTCͬ����Ƶϵ��(0~7FFF)
		RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;//RTC����Ϊ,24Сʱ��ʽ
		RTC_Init(&RTC_InitStructure);
	 
		RTC_Set_Time(23,59,56,RTC_H12_AM);	//����ʱ��
		RTC_Set_Date(18,4,7,6);		//��������
	 
		RTC_WriteBackupRegister(RTC_BKP_DR0,0x5050);	//����Ѿ���ʼ������
	} 
 
	return 0;
}

/*
�������ӣ��������֣����������ں����ڼ���ֻ��ʱ���й�
����ȡֵ0-31
*/
void RTC_Set_AlarmA(u8 date,u8 hour,u8 min,u8 sec)
{ 
	EXTI_InitTypeDef   EXTI_InitStructure;
	RTC_AlarmTypeDef RTC_AlarmTypeInitStructure;
	RTC_TimeTypeDef RTC_TimeTypeInitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;

	RTC_AlarmCmd(RTC_Alarm_A,DISABLE);//�ر�����A 

	RTC_TimeTypeInitStructure.RTC_Hours=hour;//Сʱ
	RTC_TimeTypeInitStructure.RTC_Minutes=min;//����
	RTC_TimeTypeInitStructure.RTC_Seconds=sec;//��
	RTC_TimeTypeInitStructure.RTC_H12=RTC_H12_AM;
	RTC_AlarmTypeInitStructure.RTC_AlarmTime=RTC_TimeTypeInitStructure;

	RTC_AlarmTypeInitStructure.RTC_AlarmMask=RTC_AlarmMask_DateWeekDay; //�����������ڼ�/ÿ����һ�죬ֻ��ʱ�����й�
	RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDaySel=RTC_AlarmDateWeekDaySel_Date;//��������
	RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDay=date;//����
	
	RTC_SetAlarm(RTC_Format_BIN,RTC_Alarm_A,&RTC_AlarmTypeInitStructure);


	RTC_ClearITPendingBit(RTC_IT_ALRA);//���RTC����A�ı�־
	EXTI_ClearITPendingBit(EXTI_Line17);//���LINE17�ϵ��жϱ�־λ 

	RTC_ITConfig(RTC_IT_ALRA,ENABLE);//��������A�ж�
	RTC_AlarmCmd(RTC_Alarm_A,ENABLE);//��������A 

	EXTI_InitStructure.EXTI_Line = EXTI_Line17;//LINE17
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //�����ش��� 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//ʹ��LINE17
	EXTI_Init(&EXTI_InitStructure);//����

	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);//����
}

//�����Ի��Ѷ�ʱ������  
/*wksel:  @ref RTC_Wakeup_Timer_Definitions
#define RTC_WakeUpClock_RTCCLK_Div16        ((uint32_t)0x00000000)
#define RTC_WakeUpClock_RTCCLK_Div8         ((uint32_t)0x00000001)
#define RTC_WakeUpClock_RTCCLK_Div4         ((uint32_t)0x00000002)
#define RTC_WakeUpClock_RTCCLK_Div2         ((uint32_t)0x00000003)
#define RTC_WakeUpClock_CK_SPRE_16bits      ((uint32_t)0x00000004)
#define RTC_WakeUpClock_CK_SPRE_17bits      ((uint32_t)0x00000006)
*/
//cnt:�Զ���װ��ֵ.����0,�����ж�.
void RTC_Set_WakeUp(u32 wksel,u16 cnt)
{ 
	EXTI_InitTypeDef   EXTI_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;

	RTC_WakeUpCmd(DISABLE);//�ر�WAKE UP

	RTC_WakeUpClockConfig(wksel);//����ʱ��ѡ��

	RTC_SetWakeUpCounter(cnt);//����WAKE UP�Զ���װ�ؼĴ���

	RTC_ClearITPendingBit(RTC_IT_WUT); //���RTC WAKE UP�ı�־
	EXTI_ClearITPendingBit(EXTI_Line22);//���LINE22�ϵ��жϱ�־λ 
	 
	RTC_ITConfig(RTC_IT_WUT,ENABLE);//����WAKE UP ��ʱ���ж�
	RTC_WakeUpCmd( ENABLE);//����WAKE UP ��ʱ����

	EXTI_InitStructure.EXTI_Line = EXTI_Line22;//LINE22
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //�����ش��� 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//ʹ��LINE22
	EXTI_Init(&EXTI_InitStructure);//����
 
	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);//����
}

//RTC�����жϷ�����
void RTC_Alarm_IRQHandler(void)
{    
	if(RTC_GetFlagStatus(RTC_FLAG_ALRAF)==SET)//ALARM A�ж�?
	{
		RTC_ClearFlag(RTC_FLAG_ALRAF);//����жϱ�־
	}   
	EXTI_ClearITPendingBit(EXTI_Line17);	//����ж���17���жϱ�־ 											 
}

//RTC WAKE UP�жϷ�����
void RTC_WKUP_IRQHandler(void)
{    
	if(RTC_GetFlagStatus(RTC_FLAG_WUTF)==SET)//WK_UP�ж�?
	{ 
		RTC_ClearFlag(RTC_FLAG_WUTF);	//����жϱ�־
	}   
	EXTI_ClearITPendingBit(EXTI_Line22);//����ж���22���жϱ�־ 								
}

//д���ݵ�����SRAM
int8_t write_to_backup_sram( uint8_t *data, uint16_t bytes, uint16_t offset ) 
{
    const uint16_t backup_size = 0x1000;
    uint8_t* base_addr = (uint8_t *) BKPSRAM_BASE;
    uint16_t i;
    
    /* ERROR : the last byte is outside the backup SRAM region */
    if( bytes + offset >= backup_size ) 
    {
        return -1;
    }
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE); //disable backup domain write protection
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);     //set RCC->APB1ENR.pwren
    PWR_BackupAccessCmd(ENABLE);                            //set PWR->CR.dbp = 1;
    
    /** enable the backup regulator (used to maintain the backup SRAM content in
    * standby and Vbat modes). NOTE : this bit is not reset when the device
    * wakes up from standby, system reset or power reset. You can check that
    * the backup regulator is ready on PWR->CSR.brr, see rm p144 */
    
    PWR_BackupRegulatorCmd(ENABLE);  // set PWR->CSR.bre = 1;
    for( i = 0; i < bytes; i++ ) 
    {
        *(base_addr + offset + i) = *(data + i);
    }
    PWR_BackupAccessCmd(DISABLE);      // reset PWR->CR.dbp = 0;
    return 0;
}

//�ӱ���SRAM��ȡ����
int8_t read_from_backup_sram(uint8_t *data, uint16_t bytes, uint16_t offset) 
{
    const uint16_t backup_size = 0x1000;
    uint8_t* base_addr = (uint8_t *) BKPSRAM_BASE;
    uint16_t i;
    
    /* ERROR : the last byte is outside the backup SRAM region */
    if( bytes + offset >= backup_size ) 
    {
        return -1;
    }
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE);
    for( i = 0; i < bytes; i++ ) 
    {
        *(data + i) = *(base_addr + offset + i);
    }
    
    return 0;
}

//д����RTC�Ĵ���
int8_t write_to_backup_rtc( uint32_t *data, uint16_t bytes, uint16_t offset ) 
{
    const uint16_t backup_size = 80;
    volatile uint32_t* base_addr = &(RTC->BKP0R);
    uint16_t i;
    
    if(bytes + offset >= backup_size ) 
    {
        /* ERROR : the last byte is outside the backup SRAM region */
        return -1;
    } 
    else if( offset % 4 || bytes % 4 ) 
    {
        /* ERROR: data start or num bytes are not word aligned */
        return -2;
    } 
    else 
    {
        bytes >>= 2;  /* divide by 4 because writing words */
    }
    
    /* disable backup domain write protection */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); // set RCC->APB1ENR.pwren
    PWR_BackupAccessCmd(ENABLE);       // set PWR->CR.dbp = 1;
    for( i = 0; i < bytes; i++ ) 
    {
        *(base_addr + offset + i) = *(data + i);
    }
    PWR_BackupAccessCmd(DISABLE);      // reset PWR->CR.dbp = 0;
    // consider also disabling the power peripherial?
    return 0;
}

//������RTC�Ĵ���
int8_t read_from_backup_rtc( uint32_t *data, uint16_t bytes, uint16_t offset ) 
{
    const uint16_t backup_size = 80;
    volatile uint32_t* base_addr = &(RTC->BKP0R);
    uint16_t i;
    
    if( bytes + offset >= backup_size ) {
        /* ERROR : the last byte is outside the backup SRAM region */
        return -1;
    } 
    else if( offset % 4 || bytes % 4 ) 
    {
        /* ERROR: data start or num bytes are not word aligned */
        return -2;
    } 
    else 
    {
        bytes >>= 2;  /* divide by 4 because writing words */
    }
    
    /* read should be 32 bit aligned */
    for( i = 0; i < bytes; i++ ) 
    {
        *(data + i) = *(base_addr + offset + i);
    }
    return 0;
}


