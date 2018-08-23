#ifndef DESINGE_H
#define DESINGE_H

#include "sys.h"
#include "usart2.h"
#include "usart6.h"
#include "delay.h"
#include "main.h"

typedef struct
{
	uint8_t rcv_flag;		//���յ�����
	uint8_t receive_timer;	//���ճ�ʱ��ʱ��
	uint8_t receive_on;		//���ն�ʱ����
}UartManageVarStruct;

extern UartManageVarStruct loraManage;
extern UartManageVarStruct wifiManage;

typedef struct
{
	uint8_t Hour;
	uint8_t Minute;
	uint8_t Second;
}TIME;

extern TIME scanStartTime;
extern TIME scanStopTime;
extern TIME nextScanTime;

typedef struct
{
	uint16_t Plant_Row;	//ֲ������
	uint16_t Plant_Column;//ֲ������
	uint32_t Row[10];		//������ֵ
	uint32_t Column_First;//��ʼ������
	uint32_t Column_Last; //���������
	uint32_t Column_Interval;//�м��
}MAP;

extern MAP map;


typedef enum
{
	everyDay = 0x00,
	once,
	workDay,
	monday,
	tuesday,
	wednesday,
	thursday,
	friday,
	saturday,
	sunday,
	noneRepeat

}ScanRepeatStyle;

extern ScanRepeatStyle scanRepeatStyle;				//ɨ���ظ���ʽ


typedef enum
{
	lora = 6,
	wifi = 2,
	usb = 1,
	networkModul = 5,
}DEVICE;

typedef enum
{
	IS_OK = 0x00,
	IS_START,
	IS_END,
	IS_ERROR,
}ECHO;

typedef enum
{
	lowSpeed,
	middleSpeed,
	highSpeed,
}SPEED;

typedef enum
{
	RET_ERR = 0X00,
	RET_OK = 0X01,
}RETCODE;


extern u8 cmdStart[5];
extern u8 cmdEnd[3];
extern u8 cmdIdCar[3];
extern u8 cmdIdBridge[3];
extern u8 cmdIdDebug[3];

extern void loraInit(void);
extern void wifiInit(void);
extern void SendBuff(DEVICE device, u8 *buf, u8 len);
extern void loraClear(void);
extern void wifiClear(void);



extern void DebugMsg(uint8_t *str);
extern void DebugNum(uint32_t num);
extern int fputc(int ch, FILE *f);

#endif // !DESINGE_H
