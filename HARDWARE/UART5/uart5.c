#include "timer3.h"
#include "uart5.h"	

/*
PD2  Uart5-RX
PC12 Uart5-TX
*/

uint8_t Uart5_rcv_flag;					//����5���յ�����
uint8_t UART5_RX_BUF[UART5_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
uint8_t UART5_BUF_Index; 

//����5���ͺ���
void uart5_send_byte(uint8_t ch)
{ 	
	while((UART5->SR&0X40)==0);//ѭ������,ֱ���������   
	UART5->DR = (u8) ch;      
}

/*
��ʼ������5
bound:������
*/
void uart5_init(u32 bound){
	 //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD ,ENABLE); //ʹ��GPIOC��GPIODʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);//ʹ��UART5ʱ��

	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,GPIO_AF_UART5); 	//GPIOD2����ΪUART5-RX
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_UART5); //GPIOC12����ΪUART5-TX

	//UART5�˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 				//GPIOD2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 			//���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 			//����
	GPIO_Init(GPIOD,&GPIO_InitStructure); 					//��ʼ��PD2

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; 				//GPIOC12
	GPIO_Init(GPIOC,&GPIO_InitStructure); 					//��ʼ��PD2

	//UART5 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;		//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;			//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(UART5, &USART_InitStructure); 						//��ʼ������5

	USART_Cmd(UART5, ENABLE);  //ʹ�ܴ���5 
	//USART_ClearFlag(USART1, USART_FLAG_TC);
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//��������ж�

	//Uart5 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;		//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);							//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
}

/*
����5�жϷ������
*/
void UART5_IRQHandler(void)                	
{
	u8 Res;

	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)  	//�����ж�
	{
		Res =USART_ReceiveData(UART5);	//(UART5->DR);		//��ȡ���յ�������
		UART5_RX_BUF[UART5_BUF_Index++] = Res;
		Uart5_receive_timer = 0;	//����5���ճ�ʱ��ʱ����ֻҪ�������յ������嶨ʱ��
		Uart5_receive_on = 1;		//����5���ն�ʱ����
	} 
} 

