#include "sys.h"
#include "delay.h"
#include "usart1.h"
#include "usart2.h"
#include "usart3.h"
#include "uart5.h"
#include "led.h"
#include "beep.h"
#include "timer3.h"
#include "main.h"
#include "rtc.h"
#include "design.h"

uint8_t DebugFlag;		 //����ģʽ
uint8_t WorkStatus;      //����״̬
uint8_t BreakFlag;       //�ж�ִ�б�־
uint8_t  PCBreakFlag;	 //��λ����ֹ��ǰ����


uint8_t paulseStyle;					//���������ʽ

uint16_t Scan_Interval;					//ɨ��ʱ��������λ����
uint16_t Scan_Times;					//����ɨ�����
uint8_t  Scan_SW;						//����ɨ�迪��
uint8_t	 Scan_Day;						//ɨ�����ڣ����ڱ仯��ɨ���������



/*
��ʼ�����в��������жϲ�������Ч��
RTC_BKP_DR0		RTC��ʼ����־�����=0x5050��ʾ�Ѿ���ʼ������
*/

void init_Parameter(void)
{
	uint8_t rd_buf[50],i;
	
	//1-ֲ������
	read_from_backup_sram(rd_buf,4,11);	
 	map.Plant_Row = rd_buf[3];
	DebugMsg("ֲ��������"); DebugNum(map.Plant_Row); DebugMsg("\r\n");

	//2-ֲ������
	read_from_backup_sram(rd_buf,4,15);	
 	map.Plant_Column = (rd_buf[2]<<8)+rd_buf[3];
	DebugMsg("ֲ��������"); DebugNum(map.Plant_Column); DebugMsg("\r\n");
	
	//3-ֲ��������
	for(i=0;i<map.Plant_Row;i++)
	{
		read_from_backup_sram(rd_buf,4,BASEADDR_ROW+4*i);	
		map.Row[i] = (rd_buf[0]<<24)+(rd_buf[1]<<16)+(rd_buf[2]<<8)+rd_buf[3];
		DebugMsg("�� "); DebugNum(i+1); DebugMsg(" �����꣺ "); DebugNum(map.Row[i]); DebugMsg("\r\n");
	}
	
	//4-��������
	read_from_backup_sram(rd_buf,4,BASEADDR_COL_FIRST);	
	map.Column_First = (rd_buf[0]<<24)+(rd_buf[1]<<16)+(rd_buf[2]<<8)+rd_buf[3];
	DebugMsg("�������꣺"); DebugNum(map.Column_First); DebugMsg("\r\n");
	
	//5-���������
	read_from_backup_sram(rd_buf,4,BASEADDR_COL_LAST);		
	map.Column_Last =  (rd_buf[0]<<24)+(rd_buf[1]<<16)+(rd_buf[2]<<8)+rd_buf[3];
	DebugMsg("ĩ�����꣺"); DebugNum(map.Column_Last); DebugMsg("\r\n");
	
	//6-�����м�϶
	if(map.Plant_Column==1)
		map.Column_Interval = 0;
	else if(map.Plant_Column>1)
		map.Column_Interval = (map.Column_Last-map.Column_First)/(map.Plant_Column-1);
	DebugMsg("�м�ࣺ"); DebugNum(map.Column_Interval); DebugMsg("\r\n");
	
	//7-���������ʽ��ɨ����ʼʱ�䣬ɨ�����ʱ�䣬ɨ������ɨ���ظ���ʽ
	read_from_backup_sram(rd_buf,11,67);	
	paulseStyle = rd_buf[0];//���������ʽ
	scanStartTime.Hour = rd_buf[1];			 //��ʼʱ�䣺ʱ
	scanStartTime.Minute = rd_buf[2];		 //��ʼʱ�䣺��
	scanStopTime.Hour = rd_buf[3];				 //����ʱ�䣺ʱ
	scanStopTime.Minute = rd_buf[4];			 //����ʱ�䣺��
	Scan_Interval = rd_buf[5] + (rd_buf[6]<<8) + (rd_buf[7]<<16) + (rd_buf[8]<<24); //ɨ��ʱ����
	scanRepeatStyle =(ScanRepeatStyle) rd_buf[9];	//ɨ���ظ���ʽ
	Scan_SW = rd_buf[10];	//�����ܿ���

	Scan_Times = 0;				//����ɨ�����
	Scan_Day = 0;					//����ɨ������
	nextScanTime.Hour = 0;		//�´�ɨ��ʱ��
	nextScanTime.Minute = 0;
	
}
