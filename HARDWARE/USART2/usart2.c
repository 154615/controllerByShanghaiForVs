#include "usart2.h"	
#include "timer3.h"
#include "main.h"

uint8_t  Usart2_rcv_flag;		//����2���յ�����
uint16_t  Usart2_receive_timer;	//����2���ճ�ʱ��ʱ����ֻҪ�������յ������嶨ʱ��
uint8_t  Usart2_receive_on;		//����2���ն�ʱ����
//����2���ͺ���
void usart2_send_byte(uint8_t ch)
{ 	
	while((USART2->SR&0X40)==0);//ѭ������,ֱ���������   
	USART2->DR = (u8) ch;      
}
 
//����2�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART2_RX_BUF[USART2_REC_LEN];     //���ջ���,���USART2_REC_LEN���ֽ�.
u8 USART2_BUF_Index; 

//��ʼ��IO ����2��bound:������
void usart2_init(u32 bound)
{
    int i;
    
    //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//ʹ��USART2ʱ��
 
	//����2��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); //GPIOA2����ΪUSART2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); //GPIOA3����ΪUSART2
	
	//USART1�˿�����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;  //GPIOA2��GPIOA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 			//���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 			//����
	GPIO_Init(GPIOA,&GPIO_InitStructure); 					//��ʼ��PA2��PA3

	//USART2 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;		//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;			//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART2, &USART_InitStructure); 						//��ʼ������2
	
	USART_Cmd(USART2, ENABLE);  //ʹ�ܴ���2 
	
	USART_ClearFlag(USART2, USART_FLAG_TC);
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;		//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);							//����ָ���Ĳ�����ʼ��VIC�Ĵ�����

	USART2_BUF_Index = 0;
	for(i=0;i<USART2_REC_LEN;i++) USART2_RX_BUF[i] = 0; 
}

/*
����2�жϷ������
*/
void USART2_IRQHandler(void)                	
{
	u8 Res;

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  	//�����ж�
	{
		Res =USART_ReceiveData(USART2);//(USART2->DR);		//��ȡ���յ�������
		USART2_RX_BUF[USART2_BUF_Index++] = Res;
		Usart2_receive_timer = 0;	//����2���ճ�ʱ��ʱ����ֻҪ�������յ������嶨ʱ��
		Usart2_receive_on = 1;		//����2���ն�ʱ����
	} 
} 

