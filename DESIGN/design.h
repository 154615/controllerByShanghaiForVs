#ifndef DESINGE_H
#define DESINGE_H

#include "sys.h"
#include "usart2.h"
#include "usart6.h"
#include "delay.h"
#include "rtc.h"

//��PCͨ��������غ���
#define	COMM_HEAD1	0x73	//start
#define	COMM_HEAD2	0x74
#define	COMM_HEAD3	0x61
#define	COMM_HEAD4	0x72
#define	COMM_HEAD5	0x74
#define COMM_END1	0x65	//end
#define COMM_END2	0x6E
#define COMM_END3	0x64
#define ID_XIAOCHE	100
#define ID_DALIANG  200
#define ID_DEBUG		300
#define BASEADDR_ROW	500
#define BASEADDR_COL_FIRST	59
#define BASEADDR_COL_LAST	63
#define BASEADDR_COLUMN_BIT	100

#define PRECISION	30			//30���������
#define DALIANG_TIMEOUT	60000		//�����˶���ʱ
#define XIAOCHE_TIMEOUT 20000		//С���˶���ʱ
//ֲ��ɨ��������ز���
#define CAMERATIME	1							//��λɨ�������ͣʱ��


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


typedef enum
{
	forward = 0x01,
	back,
	stop,
	toZeroIng,
}MOVECMD;

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
	highLevel = 1,
	pullse,
	other,
}LEVELMODE;

extern u8 cmdStart[5];
extern u8 cmdEnd[3];
extern u8 cmdIdCar[3];
extern u8 cmdIdBridge[3];
extern u8 cmdIdDebug[3];

extern void loraInit(void);
extern void wifiInit(void);
extern void SendBuff(DEVICE device, u8 *buf, u8 len);
extern void DebugMsg(uint8_t *str);
extern void DebugNum(uint32_t num);
extern int fputc(int ch, FILE *f);

extern u8 getReciveLen(DEVICE com);
extern void clearReciveBuf(DEVICE com);
extern uint8_t * getCmdFrame(DEVICE com, uint8_t *cmd);
extern void handlerFrame(DEVICE com, uint8_t cmd, uint8_t * dataStr);

#endif // !DESINGE_H
