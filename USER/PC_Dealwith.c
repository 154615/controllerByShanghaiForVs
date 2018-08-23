/*
����ģ�����ݴ���
���Դ�����2��WIFI���ݺʹ���5����̫������

*/
#include "delay.h"
#include "usart2.h"
#include "uart5.h"
#include "led.h"
#include "beep.h"
#include "timer3.h"
#include "rtc.h"
#include "adc.h"
#include "design.h"
#include "PC_Dealwith.h"
#include "init_Parameter.h"
#include "toSensor.h"
#include "PlanTask.h"

/*
���������Ϳ�������������
*/
extern void SendConfig(uint8_t com)
{
	uint8_t send_buf[50];

	send_buf[0] = 0x0C;		//������
	send_buf[1] = RTC_DateStruct.RTC_Year + 2000;		//��4λ��LSB
	send_buf[2] = (RTC_DateStruct.RTC_Year + 2000) >> 8;
	send_buf[3] = 0;
	send_buf[4] = 0;
	send_buf[5] = RTC_DateStruct.RTC_Month;			//��
	send_buf[6] = RTC_DateStruct.RTC_Date;			//��
	send_buf[7] = RTC_TimeStruct.RTC_Hours;			//ʱ
	send_buf[8] = RTC_TimeStruct.RTC_Minutes;		//��
	send_buf[9] = RTC_TimeStruct.RTC_Seconds;		//��
	send_buf[10] = RTC_DateStruct.RTC_WeekDay;	//����
	send_buf[11] = 0x20;
	send_buf[12] = paulseStyle;					//���巽ʽ
	send_buf[13] = 0x20;
	send_buf[14] = scanStartTime.Hour;								 //��ʼʱ�䣺ʱ
	send_buf[15] = scanStartTime.Minute;							 //��ʼʱ�䣺��
	send_buf[16] = scanStopTime.Hour;							 //����ʱ�䣺ʱ
	send_buf[17] = scanStopTime.Minute;							 //����ʱ�䣺��
	send_buf[18] = Scan_Interval;							 //ɨ����(4���ֽ� LSB)
	send_buf[19] = Scan_Interval >> 8;
	send_buf[20] = 0;
	send_buf[21] = 0;
	send_buf[22] = (uint8_t)scanRepeatStyle;							//ɨ���ظ���ʽ	
	send_buf[23] = 0x20;
	send_buf[24] = map.Plant_Row;										//����(4���ֽڣ�LSB)
	send_buf[25] = map.Plant_Row >> 8;
	send_buf[26] = 0;
	send_buf[27] = 0;
	send_buf[28] = 0xFF;										   //Ԥ��(4���ֽڣ�LSB)
	send_buf[29] = 0xFF;
	send_buf[30] = 0xFF;
	send_buf[31] = 0xFF;
	send_buf[32] = map.Plant_Column;								//����(4���ֽڣ�LSB)
	send_buf[33] = map.Plant_Column >> 8;
	send_buf[34] = 0;
	send_buf[35] = 0;

	SendBuff((DEVICE)com, cmdStart, 5);
	SendBuff((DEVICE)com, send_buf, 36);
	SendBuff((DEVICE)com, cmdEnd, 3);
}


//��Ƭ������ 0x00:�ɹ�ִ��,0x01:ɨ������ʼ,0x02:ɨ���������,0x03:δ����ȷִ��
extern void SendBack(uint8_t com, ECHO echoCode)
{
	u8 echoBuf[2] = { 0x0B,0x00 };
	echoBuf[1] = echoCode;

	echoCode == IS_OK ? BeepOne() : BeepTwo();
	SendBuff((DEVICE)com, cmdStart, 5);
	SendBuff((DEVICE)com, echoBuf, 2);
	SendBuff((DEVICE)com, cmdEnd, 3);
}



/*  ����Ŧ�۵�ص�ѹֵ  */
extern void SendBatteryVoltage(uint8_t com)
{
	u16 adc_value, bat1000;
	u8 bat_low;
	u8 batBuff[4] = { 0x0D,0x00,0x00,0x00 };
	float batv;

	adc_value = Get_Adc_Average(0, 5); //Ŧ�۵�ص�ѹ
	//batv = adc_value/4096*3300*2;//��׼��ѹ3.3V,�Ŵ�1000�����������ѹ
	batv = adc_value * 6600 / 4096;
	bat_low = (batv >= BAT_THRESHOLD) ? 1 : 2;
	bat1000 = batv;

	batBuff[1] = bat_low;
	batBuff[2] = bat1000;
	batBuff[3] = bat1000 >> 8;

	SendBuff((DEVICE)com, cmdStart, 5);
	SendBuff((DEVICE)com, batBuff, 4);
	SendBuff((DEVICE)com, cmdEnd, 3);
}

/*
����������ֵ
*/
extern void SendBackValue(uint8_t com, uint32_t d)
{
	u8 tempBuf[5] = { 0x0A };
	tempBuf[1] = d;
	tempBuf[2] = d >> 8;
	tempBuf[3] = d >> 16;
	tempBuf[4] = d >> 24;

	SendBuff((DEVICE)com, cmdStart, 5);
	SendBuff((DEVICE)com, tempBuf, 5);
	SendBuff((DEVICE)com, cmdEnd, 3);
}



/* ------------------------------------------------------------------------------
WIFI���ݴ���
------------------------------------------------------------------------------ */
extern uint8_t WIFI_Dealwith(DEVICE com)
{
	uint8_t rcv_buf[UART5_REC_LEN], i, j;
	uint8_t write_ram_buf[50];
	uint32_t k;


	if (com == wifi)
	{
		for (i = 0; i < USART2_BUF_Index; i++)	rcv_buf[i] = USART2_RX_BUF[i];
		USART2_BUF_Index = 0;
	}
	else if (com == networkModul)
	{
		for (i = 0; i < UART5_BUF_Index; i++)	rcv_buf[i] = UART5_RX_BUF[i];
		UART5_BUF_Index = 0;
	}
	else
		return 0;

	if ((rcv_buf[0] == COMM_HEAD1) && (rcv_buf[2] == COMM_HEAD3) && (rcv_buf[4] == COMM_HEAD5))
	{
		switch (rcv_buf[5])
		{
			//1-�������ں�ʱ��
		case 0x0A: //��(67),��(8),��(9),ʱ(10),��(11),��(12),����(13)������Ϊ7��
					//RTC_Set_Date(u8 year,u8 month,u8 date,u8 week)	
			PWR_BackupAccessCmd(ENABLE);
			RTC_Set_Date((rcv_buf[6] + (rcv_buf[7] << 8)) - 2000, rcv_buf[8], rcv_buf[9], rcv_buf[13]);
			//RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm)
			RTC_Set_Time(rcv_buf[10], rcv_buf[11], rcv_buf[12], RTC_H12_AM);
			PWR_BackupAccessCmd(DISABLE);
			SendBack(com, IS_OK);
			break;

			//2-��ͼ���ã�������������
		case 0x0C://����(4���ֽڣ�LSB),����(4���ֽڣ�LSB)
			write_ram_buf[0] = 0;
			write_ram_buf[1] = 0;
			write_ram_buf[2] = rcv_buf[7];
			write_ram_buf[3] = rcv_buf[6];
			map.Plant_Row = (rcv_buf[7] << 8) + rcv_buf[6];
			write_to_backup_sram(write_ram_buf, 4, 11); //����

			write_ram_buf[0] = 0;
			write_ram_buf[1] = 0;
			write_ram_buf[2] = rcv_buf[11];
			write_ram_buf[3] = rcv_buf[10];
			map.Plant_Column = (rcv_buf[11] << 8) + rcv_buf[10];
			write_to_backup_sram(write_ram_buf, 4, 15); //����
			SendBack(com, IS_OK);
			if (map.Plant_Column == 1)
				map.Column_Interval = 0;
			else if (map.Plant_Column > 1)
				map.Column_Interval = (map.Column_Last - map.Column_First) / (map.Plant_Column - 1);
			DebugMsg("�м�ࣺ");
			DebugNum(map.Column_Interval);
			DebugMsg("\r\n");
			break;

			//3-��ͼ�б궨����ǰλ��Ϊָ���кŵ�����
		case 0x01:
			if (ReadStatus(ID_XIAOCHE))
			{
				write_ram_buf[0] = XiaoChe_Now_Position >> 24;
				write_ram_buf[1] = XiaoChe_Now_Position >> 16;
				write_ram_buf[2] = XiaoChe_Now_Position >> 8;
				write_ram_buf[3] = XiaoChe_Now_Position;
				write_to_backup_sram(write_ram_buf, 4, BASEADDR_ROW + 4 * (rcv_buf[6] - 1)); //������
				SendBackValue(com, XiaoChe_Now_Position);
			}
			else
			{
				SendBack(com, IS_ERROR);
			}
			break;

			//4-��ͼ�б궨����ǰλ��Ϊ��ʼ�л�����е�����
		case 0x02:
			if (ReadStatus(ID_DALIANG))
			{
				write_ram_buf[0] = DaLiang_Now_Position >> 24;
				write_ram_buf[1] = DaLiang_Now_Position >> 16;
				write_ram_buf[2] = DaLiang_Now_Position >> 8;
				write_ram_buf[3] = DaLiang_Now_Position;

				if (rcv_buf[6] == 0x01)
				{
					map.Column_First = DaLiang_Now_Position;//��ʼ������
					write_to_backup_sram(write_ram_buf, 4, BASEADDR_COL_FIRST); //����
					SendBackValue(com, map.Column_First);
					map.Column_Interval = (map.Column_Last - map.Column_First) / map.Plant_Column;
				}
				else if (rcv_buf[6] == 0x02)
				{
					map.Column_Last = DaLiang_Now_Position;//���������
					write_to_backup_sram(write_ram_buf, 4, BASEADDR_COL_LAST); //����
					SendBackValue(com, map.Column_Last);
					map.Column_Interval = (map.Column_Last - map.Column_First) / map.Plant_Column;
				}
				else
				{
					SendBack(com, IS_ERROR);
				}
			}
			else
			{
				SendBack(com, IS_ERROR);
			}
			break;

			//5-��������
		case 0x0D:
			switch (rcv_buf[6])
			{
			case 0x00://ǰ����С��ǰ����Y���
				if (MotorMove(ID_XIAOCHE, forward))
					SendBack(com, IS_OK);
				else
					SendBack(com, IS_ERROR);
				break;

			case 0x01://���ˣ�С�����ˣ�Y��С��
				if (MotorMove(ID_XIAOCHE, back))
					SendBack(com, IS_OK);
				else
					SendBack(com, IS_ERROR);
				break;

			case 0x02://�󣨴������ˣ�X��С��
				if (MotorMove(ID_DALIANG, back))
					SendBack(com, IS_OK);
				else
					SendBack(com, IS_ERROR);
				break;

			case 0x03://�ң�����ǰ����X���
				if (MotorMove(ID_DALIANG, forward))
					SendBack(com, IS_OK);
				else
					SendBack(com, IS_ERROR);
				break;

			case 0x04://ֹͣ������С��ȫ��ֹͣ��
				i = MotorMove(ID_XIAOCHE, stop);
				j = MotorMove(ID_DALIANG, stop);
				if ((i + j) < 2)
					SendBack(com, IS_ERROR);
				else
					SendBack(com, IS_OK);
				break;

			case 0x05://�������ģʽ
				DebugFlag = 1;
				SendBack(com, IS_OK);
				break;

			case 0x06://�˳�����ģʽ
				DebugFlag = 0;
				SendBack(com, IS_OK);
				break;

			case 0x07://��ȡ����
				SendConfig(com);
				break;

			case 0x08://X��������ԭ��
				if (MotorToZero(ID_DALIANG))
					SendBack(com, IS_OK);
				else
					SendBack(com, IS_ERROR);
				break;

			case 0x09://YС�����ص�ԭ��
				if (MotorToZero(ID_XIAOCHE))
					SendBack(com, IS_OK);
				else
					SendBack(com, IS_ERROR);
				break;

			case 0x0A://�����������
				if (ChangeSpeed(highSpeed))
					SendBack(com, IS_OK);
				else
					SendBack(com, IS_ERROR);
				break;

			case 0x0B://�����������
				if (ChangeSpeed(middleSpeed))
					SendBack(com, IS_OK);
				else
					SendBack(com, IS_ERROR);
				break;

			case 0x0C://�����������
				if (ChangeSpeed(lowSpeed))
					SendBack(com, IS_OK);
				else
					SendBack(com, IS_ERROR);
				break;
			}
			break;

			//6-�ֶ�����ɨ�����
		case 0x0F:
			switch (rcv_buf[6])
			{
			case 0://������ʼһ��ɨ�裨������λ�����ã��п�������ɨ������ɨ��
				WorkStatus = 1;
				SendBack(com, IS_OK);
				Scan_Part();
				WorkStatus = 0;
				break;

			case 1://������ʼһ��ȫɨ��
				WorkStatus = 1;
				SendBack(com, IS_OK);
				Scan_Full();
				WorkStatus = 0;
				break;

			case 2://������ʼɨ��ָ����
				WorkStatus = 1;
				SendBack(com, IS_OK);
				Scan_Row(rcv_buf[7]);
				WorkStatus = 0;
				break;

			case 3://������ʼɨ��ָ����
				WorkStatus = 1;
				SendBack(com, IS_OK);
				k = rcv_buf[7] + (rcv_buf[8] << 8);
				Scan_Column(k);
				WorkStatus = 0;
				break;
			}
			break;

			//�ƻ�����ɨ����Ʋ���
		case 0x0B:
			paulseStyle = rcv_buf[6];//���������ʽ
			scanStartTime.Hour = rcv_buf[7];				//��ʼʱ�䣺ʱ
			scanStartTime.Minute = rcv_buf[8];				//��ʼʱ�䣺��
			scanStopTime.Hour = rcv_buf[9];					//����ʱ�䣺ʱ
			scanStopTime.Minute = rcv_buf[10];				//����ʱ�䣺��
			Scan_Interval = rcv_buf[11] + (rcv_buf[12] << 8); //ɨ��ʱ����
			scanRepeatStyle = (ScanRepeatStyle)rcv_buf[15];	//ɨ���ظ���ʽ

			write_ram_buf[0] = paulseStyle;					//���������ʽ
			write_ram_buf[1] = scanStartTime.Hour;			//��ʼʱ�䣺ʱ
			write_ram_buf[2] = scanStartTime.Minute;       //��ʼʱ�䣺��
			write_ram_buf[3] = scanStopTime.Hour;           //����ʱ�䣺ʱ
			write_ram_buf[4] = scanStopTime.Minute;         //����ʱ�䣺��
			write_ram_buf[5] = rcv_buf[11];					 //ɨ��ʱ����
			write_ram_buf[6] = rcv_buf[12];
			write_ram_buf[7] = rcv_buf[13];
			write_ram_buf[8] = rcv_buf[14];
			write_ram_buf[9] = rcv_buf[15];					//ɨ���ظ���ʽ
			write_to_backup_sram(write_ram_buf, 10, 67); //д������RAM
			SendBack(com, IS_OK);
			break;

			//��ɨ��λ�趨
		case 0x03:
			for (i = 0; i < rcv_buf[7] + 1; i++) write_ram_buf[i] = rcv_buf[i + 7];	//���ݳ���rcv_buf[6]
			write_to_backup_sram(write_ram_buf, rcv_buf[7] + 1, BASEADDR_COLUMN_BIT + 40 * (rcv_buf[6] - 1)); //��ʼ��ַ100,ÿ�����40�ֽ�
			delay_ms(20);			//��ʱ�ȴ��ϴβ������
			SendBack(com, IS_OK);
			break;

			//��ȡŦ�۵�ص�ѹֵ
		case 0x04:
			SendBatteryVoltage(com);
			break;
			//��ȡ��������
		case 0x05:

			break;

			//ɨ�����񿪹�
		case 0x06:
			Scan_SW = rcv_buf[6]; 	//����1��1����ɨ�裬0��ֹɨ��
			write_ram_buf[0] = rcv_buf[6];
			write_to_backup_sram(write_ram_buf, 1, 77); //�Զ�ɨ�����񿪹�
			SendBack(com, IS_OK);
			break;
		}
	}
	return 1;
}

/* ------------------------------------------------------------------------------
WIFI���ݴ���,ֻ��Ӧ����ֹͣ����
------------------------------------------------------------------------------ */
extern uint8_t WIFI_Stop(uint8_t com)
{
	uint8_t rcv_buf[UART5_REC_LEN], i, j;
	uint8_t write_ram_buf[10];

	if (com == 2)
	{
		for (i = 0; i < USART2_BUF_Index; i++)	rcv_buf[i] = USART2_RX_BUF[i];
		USART2_BUF_Index = 0;
	}
	else if (com == 5)
	{
		for (i = 0; i < UART5_BUF_Index; i++)	rcv_buf[i] = UART5_RX_BUF[i];
		UART5_BUF_Index = 0;
	}
	else
		return 0;

	if ((rcv_buf[0] == COMM_HEAD1) && (rcv_buf[2] == COMM_HEAD3) && (rcv_buf[4] == COMM_HEAD5))
	{
		switch (rcv_buf[5])
		{
			//��ֹ��ǰ����
		case 0x05:
			PCBreakFlag = 1;
			break;

			//ɨ�����񿪹�
		case 0x06:
			Scan_SW = rcv_buf[6]; 	//����1��1����ɨ�裬0��ֹɨ��
			write_ram_buf[0] = rcv_buf[6];
			write_to_backup_sram(write_ram_buf, 1, 77); //�Զ�ɨ�����񿪹�
			SendBack(com, IS_OK);
			break;

			//5-��������
		case 0x0D:
			switch (rcv_buf[6])
			{
			case 0x04://ֹͣ������С��ȫ��ֹͣ��
				i = MotorMove(ID_XIAOCHE, stop);
				j = MotorMove(ID_DALIANG, stop);
				if ((i + j) < 2)
					SendBack(com, IS_ERROR);
				else
					SendBack(com, IS_OK);
				break;

			case 0x0A://�����������
				if (ChangeSpeed(highSpeed))
					SendBack(com, IS_OK);
				else
					SendBack(com, IS_ERROR);

				break;

			case 0x0B://�����������
				if (ChangeSpeed(middleSpeed))
					SendBack(com, IS_OK);
				else
					SendBack(com, IS_ERROR);
				break;

			case 0x0C://�����������
				if (ChangeSpeed(lowSpeed))
					SendBack(com, IS_OK);
				else
					SendBack(com, IS_ERROR);
				break;
			}
			break;
		}
	}
	return 1;
}

/* ------------------------------------------------------------------------------
�ж��Ƿ������λ����ָֹ��,����1��ֹɨ��
------------------------------------------------------------------------------ */
extern uint8_t PC_Stop(void)
{
	//1-������WIFIͨ��
	if (Usart2_rcv_flag)
	{
		Usart2_rcv_flag = 0;
		WIFI_Stop(wifi);
		return 1;
	}

	//2-��������̫��ͨ��
	if (Uart5_rcv_flag)
	{
		Uart5_rcv_flag = 0;
		WIFI_Stop(networkModul);
		return 1;
	}
	return 0;
}

extern uint8_t Is_PCStop(DEVICE com)
{
	uint8_t cmd = 0xff;
	getCmdFrame(com, &cmd);
	if (cmd == 0x05)//0x0D,0x04
	{
		DebugMsg("��λ����ֹ�ȴ�\r\n");
		PCBreakFlag = 1;
		return 1;
	}
	return 0;

}
