#ifndef __RTC_H
#define __RTC_H	 
#include "sys.h" 

u8 My_RTC_Init(void);						//RTC��ʼ��
ErrorStatus RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm);	//RTCʱ������
ErrorStatus RTC_Set_Date(u8 year,u8 month,u8 date,u8 week); 	//RTC��������
void RTC_Set_AlarmA(u8 date,u8 hour,u8 min,u8 sec);		//��������ʱ��
void RTC_Set_WakeUp(u32 wksel,u16 cnt);				//�����Ի��Ѷ�ʱ������

int8_t write_to_backup_sram( uint8_t *data, uint16_t bytes, uint16_t offset );//д���ݵ�����SRAM
int8_t read_from_backup_sram(uint8_t *data, uint16_t bytes, uint16_t offset);//�ӱ���SRAM��ȡ����
int8_t write_to_backup_rtc( uint32_t *data, uint16_t bytes, uint16_t offset );//д����RTC�Ĵ���
int8_t read_from_backup_rtc( uint32_t *data, uint16_t bytes, uint16_t offset );//������RTC�Ĵ���

#endif

















