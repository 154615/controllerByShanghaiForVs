#include "delay.h"
#include "usart2.h"
#include "usart6.h"
#include "led.h"
#include "beep.h"
#include "timer3.h"
#include "rtc.h"
#include "design.h"
#include "PlanTask.h"
#include "PC_Dealwith.h"
#include "init_Parameter.h"
#include "toSensor.h"


/* ������������ֵ */
static uint32_t abs_int(uint32_t a, uint32_t b)
{
	if (a >= b)
		return a - b;
	else
		return b - a;
}
/* �ж��Ƿ񵽴�ָ��λ�� */
static uint8_t deviceInThere(uint32_t ID, uint32_t there)
{
	MOVECMD dir;
	uint32_t pos;
	dir = ID == ID_XIAOCHE ? XiaoChe_Now_Direction : DaLiang_Now_Direction;
	pos = ID == ID_XIAOCHE ? XiaoChe_Now_Position : DaLiang_Now_Position;

	if (ReadStatus(ID_XIAOCHE))
	{
		if (dir == stop)
		{
			if (abs_int(pos, there) < PRECISION)
			{
				return 1;
			}
			else
				return 0;	//����Զ
		}
		else
			return 0;	//���Ǿ�ֹ״̬
	}
	else
		return 0;	//ͨ������
}


/* �ж�һ��ʱ��C�Ƿ�λ��A��B֮�� */
static uint8_t CompareTime(uint8_t A_Hour, uint8_t A_Minute, uint8_t B_Hour, uint8_t B_Minute, uint8_t C_Hour, uint8_t C_Minute)
{
	uint16_t A = 0, B = 0, C = 0;
	A = (A_Hour << 8) + A_Minute;
	B = (B_Hour << 8) + B_Minute;
	C = (C_Hour << 8) + C_Minute;
	if ((C > A) && (C < B))
		return 1;
	else
		return 0;
}

/* -----------------------------------------------------------------------------
�жϼ��ʱ���Ƿ񵽴�
----------------------------------------------------------------------------- */
static uint16_t ScanNextTimes(void)
{
	uint32_t t, t_hour, t_minute;
	uint8_t scan_flag;

	scan_flag = 0;
	if (Scan_Times == 0)	//1-�жϵ�ǰʱ���Ƿ������ʼʱ�䣬С�ڽ���ʱ��
	{
		if (CompareTime(scanStartTime.Hour, scanStartTime.Minute,
			scanStopTime.Hour, scanStopTime.Minute,
			RTC_TimeStruct.RTC_Hours, RTC_TimeStruct.RTC_Minutes))
			scan_flag = 1;
		else
			return 0;
	}
	else	//2-�жϵ�ǰʱ���Ƿ�����´�ɨ��ʱ�䣬����ɨ��
	{
		if (CompareTime(nextScanTime.Hour, nextScanTime.Minute,
			scanStopTime.Hour, scanStopTime.Minute,
			RTC_TimeStruct.RTC_Hours, RTC_TimeStruct.RTC_Minutes))
			scan_flag = 1;
		else
			return 0;
	}

	if (scan_flag)
	{
		if (Scan_Part())//����ɨ��
		{
			Scan_Day = RTC_DateStruct.RTC_Date;	//����ɨ������
			Scan_Times++;											//��¼ɨ�����
			nextScanTime.Hour = RTC_TimeStruct.RTC_Hours;	//��¼�´�ɨ��ʱ��		
			t = RTC_TimeStruct.RTC_Minutes + Scan_Interval;
			t_hour = t / 60;
			t_minute = t % 60;
			nextScanTime.Hour += t_hour;
			nextScanTime.Minute = t_minute;
			if (nextScanTime.Hour >= 24)
			{
				Scan_Times = 0;
			}
			return 1;
		}
		else
			return 0;
	}
	return 0;
}

/* -----------------------------------------------------------------------------
����ɨ��һ��
----------------------------------------------------------------------------- */
static uint8_t ScanOneTimes(void)
{
	uint8_t wr_buf[10];
	if (CompareTime(scanStartTime.Hour, scanStartTime.Minute,
		scanStopTime.Hour, scanStopTime.Minute,
		RTC_TimeStruct.RTC_Hours, RTC_TimeStruct.RTC_Minutes))
	{
		if (Scan_Part())//����ɨ��
		{
			scanRepeatStyle = noneRepeat;
			wr_buf[0] = (uint8_t)scanRepeatStyle;
			write_to_backup_sram(wr_buf, 1, 76); //���ı���RAM�����ظ���ʽ
			return 1;
		}
		else
			return 0;
	}
	else
		return 0;
}

/* -----------------------------------------------------------------------------
������С���ع�ԭ��
-----------------------------------------------------------------------------  */
static uint8_t XYgoZero(void)
{
	uint8_t wait;
	uint32_t timeout;

	PCBreakFlag = 0;
	//1-С���ʹ�������
	DebugMsg("����С���ʹ�����ԭ��ָ��\r\n");
	if (MotorToZero(ID_XIAOCHE) == 0)
	{
		DebugMsg("��С��ͨ��ʧ��\r\n");
		return 0;
	}
	if (MotorToZero(ID_DALIANG) == 0)
	{
		DebugMsg("�ʹ���ͨ��ʧ��\r\n");
		return 0;
	}

	//2-��ѯ�ȴ�������С���������
	DebugMsg("��ѯ�ȴ�������С���������\r\n");
	timeout = 0;
	wait = 0x11;
	while (wait)
	{
		if (wait & 0x10)		//�ж�С��λ��
		{
			if (deviceInThere(ID_XIAOCHE, 0))
				wait &= 0xEF;
		}

		if (wait & 0x01)		//�жϴ���λ��
		{
			if (deviceInThere(ID_DALIANG, 0))
				wait &= 0xFE;
		}
		delay_ms(1000);
		timeout++;
		DebugNum(timeout);	//����ȴ�ʱ��
		if (timeout > DALIANG_TIMEOUT)//10���Ӳ��ܵ���ָ��λ�ã���ʱ�˳�
		{
			DebugMsg("�ȴ���ʱ����\r\n");
			return 0;
		}
		if (pcCheck(wifi))
		{
			if (PCBreakFlag)
			{
				MotorMove(ID_XIAOCHE, stop);
				MotorMove(ID_DALIANG, stop);
				return 0;
			}
		}
	}
	return 1;
}

/* -----------------------------------------------------------------------------
�ж�ɨ��ƻ�����
----------------------------------------------------------------------------- */
extern void PlanTask(void)
{
	switch (scanRepeatStyle)
	{
	case everyDay://ÿ��
		ScanNextTimes();
		break;

	case once://ֻһ��
		ScanOneTimes();
		break;

	case workDay://�����գ���һ�����壩
		if ((RTC_DateStruct.RTC_WeekDay == RTC_Weekday_Monday) ||
			(RTC_DateStruct.RTC_WeekDay == RTC_Weekday_Tuesday) ||
			(RTC_DateStruct.RTC_WeekDay == RTC_Weekday_Wednesday) ||
			(RTC_DateStruct.RTC_WeekDay == RTC_Weekday_Thursday) ||
			(RTC_DateStruct.RTC_WeekDay == RTC_Weekday_Friday))
			ScanNextTimes();
		break;

	case monday://��һ
		if (RTC_DateStruct.RTC_WeekDay == RTC_Weekday_Monday)
			ScanNextTimes();
		break;

	case tuesday://�ܶ�
		if (RTC_DateStruct.RTC_WeekDay == RTC_Weekday_Tuesday)
			ScanNextTimes();
		break;

	case wednesday://����
		if (RTC_DateStruct.RTC_WeekDay == RTC_Weekday_Wednesday)
			ScanNextTimes();
		break;

	case thursday://����
		if (RTC_DateStruct.RTC_WeekDay == RTC_Weekday_Thursday)
			ScanNextTimes();
		break;

	case friday://����
		if (RTC_DateStruct.RTC_WeekDay == RTC_Weekday_Friday)
			ScanNextTimes();
		break;

	case saturday://����
		if (RTC_DateStruct.RTC_WeekDay == RTC_Weekday_Saturday)
			ScanNextTimes();
		break;

	case sunday://����
		if (RTC_DateStruct.RTC_WeekDay == RTC_Weekday_Sunday)
			ScanNextTimes();
		break;

	case noneRepeat://��ɨ�裬����Զ�ɨ������

		break;
	}
}

/* -----------------------------------------------------------------------------
ȫ��ɨ��
----------------------------------------------------------------------------- */
extern uint8_t Scan_Full(void)
{
	uint8_t wait, scan_row;
	uint32_t timeout;

	PCBreakFlag = 0;	//���PC��ֹ��־
	DebugMsg("��ʼִ��ȫ��ɨ��\r\n");

	if (XYgoZero())
	{
		DebugMsg("С���ʹ����ع�ԭ��ɹ���\r\n");
	}
	else
	{
		DebugMsg("С���ʹ����ع�ԭ��ʧ�ܣ�\r\n");
		return 0;
	}

	//1-���ʹ����ص�һ��ָ��
	DebugMsg("���ʹ����ص�һ��ָ��\r\n");
	if (MotorToPosition(ID_DALIANG, map.Column_First) == 0)
		return 0;

	//2-����С������ʼ��	ָ��
	DebugMsg("����С������ʼ��ָ��\r\n");
	if (MotorToPosition(ID_XIAOCHE, map.Row[0]) == 0)
		return 0;

	//3-��ѯ�ȴ�������С������ָ��λ��
	DebugMsg("��ѯ�ȴ�������С������ָ��λ��\r\n");
	timeout = 0;
	wait = 0x11;
	while (wait)
	{
		if (wait & 0x10)		//�ж�С��λ��
		{
			if (deviceInThere(ID_XIAOCHE, map.Row[0]))
			{
				wait &= 0xEF;
			}
		}

		if (wait & 0x01)		//�жϴ���λ��
		{

			if (deviceInThere(ID_DALIANG, map.Column_First))
				wait &= 0xFE;
		}
		delay_ms(1000);
		timeout++;
		DebugNum(timeout);	//����ȴ�ʱ��
		if (timeout > DALIANG_TIMEOUT)//10���Ӳ��ܵ���ָ��λ�ã���ʱ�˳�
		{
			DebugMsg("�ȴ���ʱ����\r\n");
			return 0;
		}
		if (pcCheck(wifi))
		{
			if (PCBreakFlag)
			{
				MotorMove(ID_XIAOCHE, stop);
				MotorMove(ID_DALIANG, stop);
				return 0;
			}
		}
	}

	//��ʼɨ��
	DebugMsg("��ʼɨ��\r\n");
	scan_row = 0;
	while (scan_row < map.Plant_Row)
	{
		//4-���5V�ź�
		DebugMsg("���5V�ź�\r\n");
		SetXiaoChe_5V_Level(paulseStyle);
		delay_ms(1000);

		//5-�����ƶ�������д�
		DebugMsg("�����ƶ���....\r\n");
		MotorToPosition(ID_DALIANG, map.Column_Last);
		timeout = 0;
		wait = 0x01;
		while (wait)
		{
			if (deviceInThere(ID_DALIANG, map.Column_Last)) wait = 0;
			delay_ms(1000);
			timeout++;
			DebugNum(timeout);	//����ȴ�ʱ��
			if (timeout > DALIANG_TIMEOUT)//10���Ӳ��ܵ���ָ��λ�ã���ʱ�˳�
			{
				DebugMsg("�ȴ���ʱ����\r\n");
				return 0;
			}
			if (pcCheck(wifi))
			{
				if (PCBreakFlag)
				{
					MotorMove(ID_XIAOCHE, stop);
					MotorMove(ID_DALIANG, stop);
					return 0;
				}
			}
		}
		//6-�ر�5V�ź�
		DebugMsg("�ر�5V�ź�\r\n");
		SetXiaoChe_0V_Level();
		delay_ms(1000);

		//7-�����ƶ�����ʼ�д�
		DebugMsg("�����ƶ�����ʼ�д�\r\n");
		MotorToPosition(ID_DALIANG, map.Column_First);
		timeout = 0;
		wait = 0x01;
		while (wait)
		{
			if (deviceInThere(ID_DALIANG, map.Column_First)) wait = 0;
			delay_ms(1000);
			timeout++;
			DebugNum(timeout);	//����ȴ�ʱ��
			if (timeout > DALIANG_TIMEOUT)//10���Ӳ��ܵ���ָ��λ�ã���ʱ�˳�
			{
				DebugMsg("�ȴ���ʱ����\r\n");
				return 0;
			}
			if (pcCheck(wifi))
			{
				if (PCBreakFlag)
				{
					MotorMove(ID_XIAOCHE, stop);
					MotorMove(ID_DALIANG, stop);
					return 0;
				}
			}
		}
		//8-С���ƶ�����һ��
		scan_row++;
		if (scan_row < map.Plant_Row)	//�����������У���С�����ƶ�
		{
			DebugMsg("С���ƶ�����һ��\r\n");
			MotorToPosition(ID_XIAOCHE, map.Row[scan_row]);
			timeout = 0;
			wait = 0x01;
			while (wait)
			{
				if (deviceInThere(ID_XIAOCHE, map.Row[scan_row])) wait = 0;
				delay_ms(1000);
				timeout++;
				DebugNum(timeout);	//����ȴ�ʱ��
				if (timeout > XIAOCHE_TIMEOUT)//10���Ӳ��ܵ���ָ��λ�ã���ʱ�˳�
				{
					DebugMsg("�ȴ���ʱ\r\n");
					return 0;
				}
				if (pcCheck(wifi))
				{
					if (PCBreakFlag)
					{
						MotorMove(ID_XIAOCHE, stop);
						MotorMove(ID_DALIANG, stop);
						return 0;
					}
				}
			}
		}
	}
	DebugMsg("ȫ��ɨ��ɹ����\r\n");
	BeepThree();
	return 1;
}

/* -----------------------------------------------------------------------------
����ɨ��
----------------------------------------------------------------------------- */
extern uint8_t Scan_Part(void)
{
	uint32_t timeout, x;
	uint8_t wait, i, j, k, m, rd_buf[40];
	uint8_t scan_long, nowbyte;			//ɨ���ֽڸ���
	uint16_t scan_num;	//ɨ�赽��ֲ��

	PCBreakFlag = 0;	//���PC��ֹ��־
	DebugMsg("��ʼִ������ɨ������\r\n");

	//2-�ع�ԭ��
	if (XYgoZero())
	{
		DebugMsg("С���ʹ����ع�ԭ��ɹ���\r\n");
	}
	else
	{
		DebugMsg("С���ʹ����ع�ԭ��ʧ�ܣ�\r\n");
		return 0;
	}

	//3-��ʼɨ��
	DebugMsg("��ʼ����ɨ��\r\n");
	for (i = 0; i < map.Plant_Row; i++)		//����ɨ��
	{
		//4-С���ƶ���ɨ����
		DebugMsg("С���ƶ����� "); 	DebugNum(i + 1); DebugMsg(" ��,");
		DebugMsg("   Ŀ�����꣺"); 	DebugNum(map.Row[i]); DebugMsg("\r\n");
		MotorToPosition(ID_XIAOCHE, map.Row[i]);
		timeout = 0;
		wait = 0x01;
		while (wait)
		{
			if (deviceInThere(ID_XIAOCHE, map.Row[i]))
				wait = 0;
			delay_ms(1000);
			timeout++;
			DebugNum(timeout);	//����ȴ�ʱ��
			if (timeout > XIAOCHE_TIMEOUT)//10���Ӳ��ܵ���ָ��λ�ã���ʱ�˳�
			{
				DebugMsg("�ȴ���ʱ\r\n");
				return 0;
			}
			if (pcCheck(wifi))
			{
				if (PCBreakFlag)
				{
					MotorMove(ID_XIAOCHE, stop);
					MotorMove(ID_DALIANG, stop);
					return 0;
				}
			}
		}

		//5-��ȡɨ��λͼ
		for (m = 0; m < 40; m++) rd_buf[m] = 0;
		read_from_backup_sram(rd_buf, 40, 100 + 40 * i);
		scan_long = rd_buf[0];
		scan_num = 0;
		for (j = 0; j < scan_long; j++)
		{
			nowbyte = rd_buf[j + 1];
			for (k = 0; k < 8; k++)
			{
				if (nowbyte & 0x01)	//��ӦλΪ1���ƶ������У���ͣ����������ʾ�ź�
				{
					//6-�����ƶ�����־��
					x = map.Column_First + map.Column_Interval*(8 * j + k);
					DebugMsg("�����ƶ�Ŀ�꣺ "); DebugNum(x); DebugMsg("\r\n");
					MotorToPosition(ID_DALIANG, x);
					timeout = 0;
					wait = 0x01;
					while (wait)
					{
						if (deviceInThere(ID_DALIANG, x))
							wait = 0;
						delay_ms(1000);
						timeout++;
						DebugNum(timeout);	//����ȴ�ʱ��
						if (timeout > DALIANG_TIMEOUT)//10���Ӳ��ܵ���ָ��λ�ã���ʱ�˳�
						{
							DebugMsg("�ȴ���ʱ\r\n");
							return 0;
						}
						if (pcCheck(wifi))
						{
							if (PCBreakFlag)
							{
								MotorMove(ID_XIAOCHE, stop);
								MotorMove(ID_DALIANG, stop);
								return 0;
							}
						}
					}
					//7-����5V�źŲ��ȴ�
					DebugMsg("�ҵ�ɨ���λ\r\n");
					SetXiaoChe_5V_Level(paulseStyle);
					delay_ms(CAMERATIME);
					SetXiaoChe_0V_Level();
					scan_num++;
					DebugMsg("������ɨ��Ŀ�� ");
					DebugNum(scan_num);
					DebugMsg(" ��\r\n");
				}
				nowbyte >>= 1;
			}
		}
	}

	//8-ɨ����ϣ�������
	DebugMsg("����ɨ�����\r\n");
	BeepThree();
	return 1;
}

/* -----------------------------------------------------------------------------
ɨ��ָ����
----------------------------------------------------------------------------- */
extern uint8_t Scan_Row(uint8_t scan_row)
{
	uint8_t wait;
	uint32_t timeout;

	PCBreakFlag = 0;	//���PC��ֹ��־
	DebugMsg("ɨ��ָ��������\r\n");
	//�ع�ԭ��
	if (XYgoZero())
	{
		DebugMsg("С���ʹ����ع�ԭ��ɹ���\r\n");
	}
	else
	{
		DebugMsg("С���ʹ����ع�ԭ��ʧ�ܣ�\r\n");
		return 0;
	}

	//1-���ʹ�������ʼ��ָ��
	DebugMsg("���ʹ�������ʼ��ָ��\r\n");
	if (MotorToPosition(ID_DALIANG, map.Column_First) == 0)
		return 0;

	//2-����С������ʼ��ָ��
	DebugMsg("����С�������"); DebugNum(scan_row); DebugMsg("��ָ��\r\n");
	if (MotorToPosition(ID_XIAOCHE, map.Row[scan_row - 1]) == 0)
		return 0;

	//3-��ѯ�ȴ�������С������ָ��λ��
	DebugMsg("��ѯ�ȴ�������С������ָ��λ��\r\n");
	timeout = 0;
	wait = 0x11;
	while (wait)
	{
		if (wait & 0x10)		//�ж�С��λ��
		{
			if (deviceInThere(ID_XIAOCHE, map.Row[scan_row - 1])) wait &= 0xEF;
		}

		if (wait & 0x01)		//�жϴ���λ��
		{
			if (deviceInThere(ID_DALIANG, map.Column_First)) wait &= 0xFE;
		}
		delay_ms(1000);
		timeout++;
		DebugNum(timeout);	//����ȴ�ʱ��
		if (timeout > DALIANG_TIMEOUT)//10���Ӳ��ܵ���ָ��λ�ã���ʱ�˳�
		{
			DebugMsg("�ȴ���ʱ\r\n");
			return 0;
		}
		if (pcCheck(wifi))
		{
			if (PCBreakFlag)
			{
				MotorMove(ID_XIAOCHE, stop);
				MotorMove(ID_DALIANG, stop);
				return 0;
			}
		}
	}

	//4-���5V�ź�
	DebugMsg("���5V�ź�\r\n");
	SetXiaoChe_5V_Level(paulseStyle);
	delay_ms(1000);

	//5-�����ƶ�������д�
	DebugMsg("�����ƶ���....\r\n");
	MotorToPosition(ID_DALIANG, map.Column_Last);
	timeout = 0;
	wait = 0x01;
	while (wait)
	{
		if (deviceInThere(ID_DALIANG, map.Column_Last)) wait = 0;
		delay_ms(1000);
		timeout++;
		DebugNum(timeout);	//����ȴ�ʱ��
		if (timeout > DALIANG_TIMEOUT)//10���Ӳ��ܵ���ָ��λ�ã���ʱ�˳�
		{
			DebugMsg("�ȴ���ʱ\r\n");
			return 0;
		}
		if (pcCheck(wifi))
		{
			if (PCBreakFlag)
			{
				MotorMove(ID_XIAOCHE, stop);
				MotorMove(ID_DALIANG, stop);
				return 0;
			}
		}
	}

	//6-�ر�5V�ź�
	DebugMsg("�ر�5V�ź�\r\n");
	SetXiaoChe_0V_Level();
	delay_ms(1000);

	DebugMsg("ɨ��ָ�����������\r\n");
	BeepThree();
	return 1;
}

/* -----------------------------------------------------------------------------
ɨ��ָ����
----------------------------------------------------------------------------- */
extern uint8_t Scan_Column(uint16_t scan_column)
{
	uint8_t wait;
	uint32_t timeout, column_position;

	PCBreakFlag = 0;	//���PC��ֹ��־
	DebugMsg("ɨ��ָ���п�ʼ\r\n");
	//�ع�ԭ��
	if (XYgoZero())
	{
		DebugMsg("С���ʹ����ع�ԭ��ɹ���\r\n");
	}
	else
	{
		DebugMsg("С���ʹ����ع�ԭ��ʧ�ܣ�\r\n");
		return 0;
	}

	//1-���ʹ�����ָ����ָ��
	DebugMsg("���ʹ��������"); DebugNum(scan_column); DebugMsg("��ָ��\r\n");
	column_position = map.Column_First + map.Column_Interval*(scan_column - 1);
	if (MotorToPosition(ID_DALIANG, column_position) == 0)return 0;

	//2-����С������ʼ��ָ��
	DebugMsg("����С��������ʼ��ָ��\r\n");
	if (MotorToPosition(ID_XIAOCHE, map.Row[0]) == 0)return 0;

	//3-��ѯ�ȴ�������С������ָ��λ��
	DebugMsg("��ѯ�ȴ�������С������ָ��λ��\r\n");
	timeout = 0;
	wait = 0x11;
	while (wait)
	{
		if (wait & 0x10)		//�ж�С��λ��
		{
			if (deviceInThere(ID_XIAOCHE, map.Row[0]))
				wait &= 0xEF;
		}

		if (wait & 0x01)		//�жϴ���λ��
		{
			if (deviceInThere(ID_DALIANG, column_position))
				wait &= 0xFE;
		}
		delay_ms(1000);
		timeout++;
		DebugNum(timeout);	//����ȴ�ʱ��
		if (timeout > DALIANG_TIMEOUT)//10���Ӳ��ܵ���ָ��λ�ã���ʱ�˳�
		{
			DebugMsg("�ȴ���ʱ\r\n");
			return 0;
		}
		if (pcCheck(wifi))
		{
			if (PCBreakFlag)
			{
				MotorMove(ID_XIAOCHE, stop);
				MotorMove(ID_DALIANG, stop);
				return 0;
			}
		}
	}

	//4-���5V�ź�
	DebugMsg("���5V�ź�\r\n");
	SetXiaoChe_5V_Level(paulseStyle);
	delay_ms(1000);

	//5-С���ƶ�������д�
	DebugMsg("С���ƶ���....\r\n");
	MotorToPosition(ID_XIAOCHE, map.Row[map.Plant_Row - 1]);
	timeout = 0;
	wait = 0x01;
	while (wait)
	{

		if (deviceInThere(ID_XIAOCHE, map.Row[map.Plant_Row - 1]))
			wait = 0;
		delay_ms(1000);
		timeout++;
		DebugNum(timeout);	//����ȴ�ʱ��
		if (timeout > XIAOCHE_TIMEOUT)//10���Ӳ��ܵ���ָ��λ�ã���ʱ�˳�
		{
			DebugMsg("�ȴ���ʱ\r\n");
			return 0;
		}
		if (pcCheck(wifi))
		{
			if (PCBreakFlag)
			{
				MotorMove(ID_XIAOCHE, stop);
				MotorMove(ID_DALIANG, stop);
				return 0;
			}
		}
	}

	//6-�ر�5V�ź�
	DebugMsg("�ر�5V�ź�\r\n");
	SetXiaoChe_0V_Level();
	delay_ms(1000);

	DebugMsg("ɨ��ָ����������ɣ�\r\n");
	BeepThree();
	return 1;
}

