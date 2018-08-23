/*
����2��WIFI���ڽ��յ������ݴ���

*/
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
#include "adc.h"

#define BAT_THRESHOLD   2200*4096/2/3000    //1501,����2.2V���� Vbat = D/4096*3000*2

//����֡ͷ
void UART2_SendHead(void)
{
	uart5_send_byte(COMM_HEAD1);
	uart5_send_byte(COMM_HEAD2);
	uart5_send_byte(COMM_HEAD3);
	uart5_send_byte(COMM_HEAD4);
	uart5_send_byte(COMM_HEAD5);
}

//����֡β
void UART2_SendEnd(void)
{
	uart5_send_byte(COMM_END1);
	uart5_send_byte(COMM_END2);
	uart5_send_byte(COMM_END3);
}

//������ȷִ�з���
void UART2_SendACK(void)
{
	UART5_SendHead();
	uart5_send_byte(0x14);
	UART5_SendEnd();
}

//���ʹ�����շ���
void UART2_SendError(void)
{
	UART5_SendHead();
	uart5_send_byte(0x15);
	UART5_SendEnd();
}

//���͵�ǰ���⴫��������
void UART2_SendLaser(void)
{
	UART5_SendHead();
	uart5_send_byte(0x17);
//	uart5_send_byte(Laser1_Value/256);
//	uart5_send_byte(Laser1_Value%256);
//	uart5_send_byte(Laser2_Value/256);
//	uart5_send_byte(Laser2_Value%256);
	UART5_SendEnd();
}


/*
WIFI���ݴ���
*/
u8 Usart2_WIFI_Dealwith(void)
{
    u16 adc_value;
    u8 bat_low;
    
	if( (UART5_RX_BUF[0] == COMM_HEAD1) && 
		(UART5_RX_BUF[1] == COMM_HEAD2) && 
		(UART5_RX_BUF[2] == COMM_HEAD3) && 
		(UART5_RX_BUF[3] == COMM_HEAD4) && 
		(UART5_RX_BUF[4] == COMM_HEAD5) )
	{
		switch(UART5_RX_BUF[5])
		{
			case 0x01:	//�������ں�ʱ��
				RTC_Set_Date(UART5_RX_BUF[6],UART5_RX_BUF[7],UART5_RX_BUF[8],RTC_Weekday_Monday);   //RTC_Set_Date(u8 year,u8 month,u8 date,u8 week)
				RTC_Set_Time(UART5_RX_BUF[9],UART5_RX_BUF[10],0x00,RTC_H12_AM);	                    //RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm)
				UART2_SendACK();
			break;
			
			case 0x02:	//����ÿ��ɨ��ʱ��
				RTC_Set_AlarmA(1,UART5_RX_BUF[6],UART5_RX_BUF[7],30);    //�������������ӣ����ڣ�ʱ����
				UART2_SendACK();
			break;
			
			case 0x03:	//�趨ԭ������
				X0 = UART5_RX_BUF[6]*256+UART5_RX_BUF[7];       //Xԭ������
				Y0 = UART5_RX_BUF[8]*256+UART5_RX_BUF[9];       //Yԭ������
				Xmax = UART5_RX_BUF[10]*256+UART5_RX_BUF[11];   //X�������
				Ymax = UART5_RX_BUF[12]*256+UART5_RX_BUF[13];   //Y�������
				LineNum = UART5_RX_BUF[14];                     //����

				RTC_WriteBackupRegister(RTC_BKP_DR4,X0);        //����ԭ��X0��������
				RTC_WriteBackupRegister(RTC_BKP_DR5,Y0);        //����ԭ��Y0С������
				RTC_WriteBackupRegister(RTC_BKP_DR6,Xmax);      //�Խ�����Xmax��������
				RTC_WriteBackupRegister(RTC_BKP_DR7,Ymax);      //�Խ�����YmaxС������
				RTC_WriteBackupRegister(RTC_BKP_DR8,LineNum);   //������

				RTC_WriteBackupRegister(RTC_BKP_DR9,0x5050);          //ԭ���ѱ�ʶ
				Config_Flag = 1;
				
				UART5_SendHead();       //����ͷ 0x18 ���� ����β
				uart5_send_byte(0x18);
				uart5_send_byte(0x01);
				UART5_SendEnd();
                
			break;
			
			case 0x04:	//����ǰ��
				//JDQ2_OFF;
				delay_ms(300);
				//JDQ1_ON;
				UART2_SendACK();
			break;
			
			case 0x05:	//��������
				//JDQ1_OFF;
				delay_ms(300);
				//JDQ2_ON;
				UART2_SendACK();
			break;
			
			case 0x06:	//����ֹͣ
				//JDQ1_OFF;
				//JDQ2_OFF;
				//JDQ3_OFF;
				//JDQ4_OFF;
				//JDQ5_OFF;   //��բ
				BreakFlag = 1;
				UART5_SendACK();
			break;
			
			case 0x07:	//С��ǰ��
				//JDQ4_OFF;
				delay_ms(300);
				//JDQ3_ON;
				//JDQ5_ON;    //��բ
				UART5_SendACK();
			break;
			
			case 0x08:	//С������
				//JDQ3_OFF;
				delay_ms(300);
				//JDQ4_ON;
				//JDQ5_ON;    //��բ
				UART5_SendACK();
			break;
			
			case 0x09:	//С��ֹͣ
				//JDQ1_OFF;
				//JDQ2_OFF;
				//JDQ3_OFF;
				//JDQ4_OFF;
				//JDQ5_OFF;   //��բ
				BreakFlag = 1;
				UART2_SendACK();
			break;
			
			case 0x0A:	//δʹ��
				UART2_SendACK();
			break;
			
			case 0x0B:	//�����ٶ�
				LowSpeed = UART5_RX_BUF[6]*256+UART5_RX_BUF[7];
				MidSpeed = UART5_RX_BUF[8]*256+UART5_RX_BUF[9];
				HighSpeed = UART5_RX_BUF[10]*256+UART5_RX_BUF[11];
				RTC_WriteBackupRegister(RTC_BKP_DR10,LowSpeed);     //����
				RTC_WriteBackupRegister(RTC_BKP_DR11,MidSpeed);     //����
				RTC_WriteBackupRegister(RTC_BKP_DR12,HighSpeed);    //����             
				UART2_SendACK();
			break;
			
			case 0x0C:	//�л�������
				NowSpeed = 1;      //��ǰ����ٶ�
				UART2_SendACK();
			break;
			
			case 0x0D:	//�л�������
				NowSpeed = 2;      //��ǰ����ٶ�
				UART2_SendACK();
			break;
			
			case 0x0E:	//�л�������
				NowSpeed = 3;      //��ǰ����ٶ�
				UART2_SendACK();
			break;
			
			case 0x0F:	//�����ź�
				adc_value = Get_Adc_Average(9,5); //Ŧ�۵�ص�ѹ
				if(adc_value>=BAT_THRESHOLD)
						bat_low = 1;
				 else
						bat_low = 2;
            
				UART5_SendHead();
				uart5_send_byte(0x16);
				uart5_send_byte(Config_Flag);   //ԭ���ѱ�ʶ��־
				uart5_send_byte(bat_low);   	//ԭ���ѱ�ʶ��־
				uart5_send_byte(NowSpeed);      //��ǰ����ٶ�
				UART5_SendEnd();
			break;

			case 0x10:	//����/�رն�ʱ����
				RTC_AlarmCmd(RTC_Alarm_A,DISABLE);//�ر�����A
			break;
			
			case 0x11:	//ɨ���������
				ScanPart_X0 = UART5_RX_BUF[6]*256 + UART5_RX_BUF[7];    //��ʼ��X����
				ScanPart_Xstep = UART5_RX_BUF[8]*256 + UART5_RX_BUF[9]; //��ɨ�賤��
				ScanPart_StartLine = UART5_RX_BUF[10];                  //��ʼ��
				ScanPart_LineNum = UART5_RX_BUF[11];                    //����
				ScanPart_SW = 1;
			break;
			
			case 0x12:	//���⴫������
//				LaserFB = 1;
//				Laser1_ON = 1;
//				Laser2_ON = 1;
				UART5_SendACK();
			break;
			
			case 0x13:	//���⴫������
//				LaserFB = 0;
//				Laser1_ON = 0;
//				Laser2_ON = 0;
				usart2_send_byte('C');
				usart3_send_byte('C');
				UART2_SendACK();
			break;
            
			case 0x40:	//�����Զ�ɨ��һ��
				PlanTask_SW = 1;
			break;
            
			case 0x41:	//��ȡ��Ƭ������״̬
				UART5_SendHead();
				uart5_send_byte(0x19);
        uart5_send_byte(WorkStatus); 
				UART2_SendEnd();
			break;           
            
      case 0x42:	//ִ��һ�δ���x�ƶ�������ָ��
				X_goto = UART5_RX_BUF[6]*256 + UART5_RX_BUF[7]; //XĿ������
				Xgoto_SW = 1;
				UART2_SendACK();
			break;
            
      case 0x43:	//ִ��һ��С��y�ƶ�������ָ��
				Y_goto = UART5_RX_BUF[6]*256 + UART5_RX_BUF[7]; //YĿ������
				Ygoto_SW = 1;
				UART2_SendACK();
			break;
           
		}
	}
	else
	{
		UART2_SendError();	//У����������ط�
	}
	return 1;
}
