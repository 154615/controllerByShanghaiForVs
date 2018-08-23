#include "sys.h"
#include "delay.h"
#include "usart2.h"
#include "usart6.h"
#include "led.h"
#include "beep.h"
#include "timer3.h"
#include "design.h"
#include "toSensor.h"
#include "init_Parameter.h"



//ָ���ȴ���������ָ���������ʱ������ᱻ����һ�δ���
static void LoraWait(void)
{
	LED2_ON;        //������˸һ��
	delay_ms(100);
	LED2_OFF;
}

/* -------------------------------------------------------------------------------
1- �����������,���㷢�ͣ�С��ID=100������ID=200
����0x01��ǰ��0x02���0x03ֹͣ��
------------------------------------------------------------------------------- */

extern RETCODE MotorMove(uint8_t ID, MOVECMD dir)
{
	u8 tempBuf[5] = { 0x00,0x00,72,0x01,0x00 };

	tempBuf[0] = ID / 256;
	tempBuf[1] = ID % 256;
	LoraWait();
	
	clearReciveBuf(lora);
	if (ID != ID_XIAOCHE&&ID != ID_DALIANG)
	{
		return RET_ERR;
	}
	tempBuf[4] = dir;

	SendBuff(lora, tempBuf, 3);
	SendBuff(lora, cmdStart, 5);
	SendBuff(lora, tempBuf + 3, 2);
	SendBuff(lora, cmdEnd, 3);
	return waitLoraAck();
}

extern RETCODE waitLoraAck(void)
{
	u8 i = 0;
	while (getReciveLen(lora) < 3)
	{
		delay_ms(2);
		i++;
		if (i >= 500)
		{
			return RET_ERR;
		}
	}

	if ((USART6_RX_BUF[0] == 'A') && (USART6_RX_BUF[2] == 'K'))
		return RET_OK;
	else
		return RET_ERR;
}

/* -------------------------------------------------------------------------------
2- �˶���ָ��λ��,���㷢�ͣ�С��ID=100������ID=200
���ֽ�λ�����ݣ����ֽ���ǰ�����ֽ��ں����λ+1��Ϊ����ʹ��0
------------------------------------------------------------------------------- */
extern RETCODE MotorToPosition(uint8_t ID, uint32_t pos)
{
	uint8_t check, send_times;
	uint8_t send_buf[16] = { 0x00,0x00,72,'s','t','a','r','t',0x11,0x00,0x00,0x00,0x00,'e','n','d' };
	clearReciveBuf(lora);
	if (ID != ID_XIAOCHE && ID != ID_DALIANG)
	{
		return RET_ERR;
	}
	send_buf[0] = ID / 256;
	send_buf[1] = ID % 256;
	send_buf[9] = (pos >> 16) + 1;
	send_buf[10] = pos >> 8;
	send_buf[11] = pos;
	check = send_buf[9] + send_buf[10] + send_buf[11];
	send_buf[12] = check;

	send_times = 5;
	while (send_times--)		//��ʱ���յ�����ʱ���ط�5��
	{
		LoraWait();
		clearReciveBuf(lora);
		SendBuff(lora, send_buf, 16);

		if (waitLoraAck() == RET_OK)
		{
			return RET_OK;
		}
	}
	return RET_ERR;
}

/*
3- �˶������,���㷢�ͣ�С��ID=100������ID=200
*/
extern RETCODE MotorToZero(uint8_t ID)
{
	uint8_t  send_times;
	uint8_t send_buf[12] = { 0x00,0x00,72,'s','t','a','r','t',0x02,'e','n','d' };

	clearReciveBuf(lora);
	if (ID != ID_XIAOCHE && ID != ID_DALIANG)
	{
		return RET_ERR;
	}

	send_buf[0] = ID / 256;
	send_buf[1] = ID % 256;

	send_times = 5;
	while (send_times--)
	{
		LoraWait();
		SendBuff(lora, send_buf, 12);
		clearReciveBuf(lora);
		if (waitLoraAck() == RET_OK)
		{
			return RET_OK;
		}
	}
	return RET_ERR;
}

/* -------------------------------------------------------------------------------
4- ��ȡ��ǰ״̬,���㷢�ͣ�С��ID=100������ID=200
------------------------------------------------------------------------------- */
extern RETCODE ReadStatus(uint8_t ID)
{
	uint8_t check;
	u8 tempBuf[4] = { 0x00,0x00,72,0x00 };
	MOVECMD dir;
	uint32_t pos;

	if (ID != ID_DALIANG && ID != ID_XIAOCHE)
	{
		return RET_ERR;
	}

	clearReciveBuf(lora);
	LoraWait();
	tempBuf[0] = ID / 256;
	tempBuf[1] = ID % 256;
	tempBuf[3] = 0x03;//״̬��ȡָ��
	SendBuff(lora, tempBuf, 3);

	SendBuff(lora, cmdStart, 5);
	SendBuff(lora, tempBuf + 3, 1);
	SendBuff(lora, cmdEnd, 3);


	USART6_RX_BUF[0] = '0';
	delay_ms(1000);

	if ((USART6_RX_BUF[0] == 's') && (USART6_RX_BUF[1] == 't') && (USART6_RX_BUF[12] == 'd'))
	{
		check = USART6_RX_BUF[6] + USART6_RX_BUF[7] + USART6_RX_BUF[8];
		if (USART6_RX_BUF[9] == check)	//�ж�У��
		{
			dir = (MOVECMD)USART6_RX_BUF[5];
			pos = USART6_RX_BUF[6] - 1;		//���λ+1�����ⷢ��0ʱģ�����׶�ʧ
			pos <<= 8;
			pos += USART6_RX_BUF[7];
			pos <<= 8;
			pos += USART6_RX_BUF[8];

			if (ID == ID_XIAOCHE)
			{
				XiaoChe_Now_Direction = dir;
				XiaoChe_Now_Position = pos;
			}
			else
			{
				DaLiang_Now_Direction = dir;
				DaLiang_Now_Position = pos;
			}

			/* ����С���ʹ���λ�� */
			DebugMsg("С��״̬��");
			switch (XiaoChe_Now_Direction)
			{
			case 1:
				DebugMsg("ǰ�������꣺");
				break;
			case 2:
				DebugMsg("���ˣ����꣺");
				break;
			case 3:
				DebugMsg("��ֹ�����꣺");
				break;

			}
			DebugNum(XiaoChe_Now_Position);
			DebugMsg("\r\n");

			DebugMsg("����״̬��");
			switch (DaLiang_Now_Direction)
			{
			case 1:
				DebugMsg("ǰ�������꣺");
				break;
			case 2:
				DebugMsg("���ˣ����꣺");
				break;
			case 3:
				DebugMsg("��ֹ�����꣺");
				break;
			}
			DebugNum(DaLiang_Now_Position);
			DebugMsg("\r\n");
			return RET_OK;
		}
		else
			return RET_ERR;
	}
	else
		return RET_ERR;
}

/* -------------------------------------------------------------------------------
5- С�����5V�źţ�ֻ��С����Ч
���04��+������1������ƽ��2���壬3������
���أ�ACK
------------------------------------------------------------------------------- */
extern uint8_t SetXiaoChe_5V_Level(uint8_t levelmode)
{
	u8 tempBuf[2] = { 0x00,0x00 };

	clearReciveBuf(lora);
	LoraWait();
	tempBuf[0] = 0x04;
	tempBuf[1] = levelmode;

	SendBuff(lora, cmdIdCar, 3);
	SendBuff(lora, cmdStart, 5);
	SendBuff(lora, tempBuf, 2);
	SendBuff(lora, cmdEnd, 3);

	return waitLoraAck();
}

/* -------------------------------------------------------------------------------
6- С������5V�źţ�ֻ��С����Ч
���07��+����
���أ�ACK
------------------------------------------------------------------------------- */
extern uint8_t SetXiaoChe_0V_Level(void)
{
	u8 tempBuf[1] = { 0x00 };

	clearReciveBuf(lora);
	LoraWait();

	SendBuff(lora, cmdIdCar, 3);
	SendBuff(lora, cmdStart, 5);
	SendBuff(lora, tempBuf, 1);
	SendBuff(lora, cmdEnd, 3);

	return waitLoraAck();
}


/* -------------------------------------------------------------------------------
7- �л��ٶȣ�ֻ�Դ�����Ч��
 ���06��+������0���٣�1���٣�2���٣�
���أ�ACK
------------------------------------------------------------------------------- */
extern RETCODE ChangeSpeed(SPEED speed)
{
	u8 tempBuf[2] = { 0x00,0x00 };
	clearReciveBuf(lora);
	LoraWait();

	tempBuf[0] = 0x06;
	tempBuf[1] = (u8)speed;

	SendBuff(lora, cmdIdBridge, 3);
	SendBuff(lora, cmdStart, 5);
	SendBuff(lora, tempBuf, 2);
	SendBuff(lora, cmdEnd, 3);

	return waitLoraAck();
}

