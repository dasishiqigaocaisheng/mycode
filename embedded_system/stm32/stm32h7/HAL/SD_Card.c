#include "sys.h"
#include "delay.h"
#include "SD_Card.h"
#include "USART.h"
#include "Memory.h"
#include "Timer.h"

#define SD_READ_BLOCK_SIZE	 512	//��ȡ���С
#define SD_WRITE_BLOCK_SIZE	 1024	//д����С
#define SD_CARD_BLOCK_SIZE	 512	//SDHC���̶����С
#define SD_MAX_BLOCK_ADDRESS 67108863		//SD�������ַ
#define SD_MAX_BYTE_ADDRESS  34359738367	//SD������ֽڵ�ַ��4GB-1B��
#define SD_MAX_DLEN			 131072	//��󵥴δ�����������128*1024�ֽڣ�

/*�������Ӧ����*/
#define SD_RESPONSE_TYPE_NONE	0	//����Ӧ
#define SD_RESPONSE_TYPE_SHORT  1	//����Ӧ
#define SD_RESPONSE_TYPE_LONG	3	//����Ӧ

/*��ȡ��Ӧʱ�Ƿ���CRC*/
#define SD_RESPONSE_CRC_CARE		0	//���
#define SD_RESPONSE_CRC_IGNORE		1	//�����

#define SD_DMA2_SR_CLEAR() DMA2->HIFCR|=0xf<<18	//���DMA�жϱ�־
#define SD_CARD_SR_CLEAR() SDIO->ICR|=0xc007ff	//���SDIO�жϱ�־

#define SDIO_DCTRL_READ 	 0x0000009b	//ִ�ж�����ʱ��SDIO->DCTRL������
#define SDIO_DCTRL_WRITE	 0x00000099 //ִ��д����ʱ��SDIO->DCTRL������

#define INTERRUPT_TYPE_WRITE 1
#define INTERRUPT_TYPE_READ  2

#define WRITE_DELAY_TIME	7000
#define READ_DELAY_TIME		1500
#define SD_Write_Delay()	delay_us(6500)
//#define SD_Read_Delay()		delay_ms(15)

u8 Block_Ready=0;
u8 Data_Ready=0;
u8 Fast_Mode=0;
u8 Interrupt_Type=0;
u8 Interrupt_Flag=0;
u32 RCA;	//SD����ַ

u8 *Send_Buffer1,*Send_Buffer2;	//�������ͻ���
u8* Receive_Buffer;				//һ�����ջ���

GeneralTimer_Info SD_Timer;

/*����һ��CMD����*/
void _SD_Card_Send_Command(u8 CMD_Index, u32 CMD, u8 Response_Type);

/*����һ��ACMD����*/
void _SD_Card_Send_Application_Command(u8 CMD_Index, u32 CMD, u16 RCA, u8 Response_Type);

/*��ȡ��Ӧ*/
void _SD_Card_Read_Response(u8 Response_Type, u8 Response_CRC, u8* CMD_Index, void* Buffer);

/*������Ӧ������ȡ��*/
void _SD_Card_Skip_Response(u8 Response_CRC);

/*д���������飬������С��SD_WRITE_BLOCK_SIZE���壬��ַҲ�ᷢ���ı�*/
u8 _SD_Card_Write_Blocks(u32 Block_Address, u32 Block_Number, void* Data);

/*��ȡ���������Ŀ飬���С��SD_READ_BLOCK_SIZE���壬��ַҲ�ᷢ���ı�*/
u8 _SD_Card_Read_Blocks(u32 Block_Address, u32 Block_Number, void* Data);

void SD_Card_Init(void)
{
	u8 Command_Index;
	u32 Command[4];
	
	RCC->AHB1ENR|=1<<2;	//GPIOCʱ��ʹ��
	RCC->AHB1ENR|=1<<3;	//GPIODʱ��ʹ��
	RCC->AHB1ENR|=1<<22;//DMA2ʱ��ʹ��
	RCC->APB2ENR|=1<<11;//SDIOʱ��ʹ��
	RCC->APB2RSTR|=1<<11;
	
	GPIO_Set(GPIOC,PIN8|PIN9|PIN10|PIN11|PIN12,GPIO_MODE_AF,
				GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PD);
	GPIO_Set(GPIOD,PIN2,GPIO_MODE_AF,GPIO_OTYPE_PP,
				GPIO_SPEED_100M,GPIO_PUPD_PU);
	
	GPIO_AF_Set(GPIOC,8,12);
	GPIO_AF_Set(GPIOC,9,12);
	GPIO_AF_Set(GPIOC,10,12);
	GPIO_AF_Set(GPIOC,11,12);
	GPIO_AF_Set(GPIOC,12,12);
	GPIO_AF_Set(GPIOD,2,12);
	RCC->APB2RSTR&=~(1<<11);

	SDIO->POWER|=3;		//SDIO�ϵ�
	SDIO->CLKCR|=158;	//ʱ��160��Ƶ��300kHz
	delay_us(500);		//�üĴ�����������д��ʱ��������С������SDIOʱ������
	SDIO->CLKCR|=1<<8;	//ʹ��ʱ��
	delay_us(500);
	SDIO->CLKCR|=1<<11;	//4λ���߿��
	delay_us(500);
	//SDIO->CLKCR|=1<<14;	//����Ӳ��������
	
	SDIO->MASK|=1<<8;	//���ݿ�����ж�ʹ��
	//MY_NVIC_Init(Pre_Priority,Sub_Priority,SDIO_IRQn,Interrupt_Group);
	MY_NVIC_Init(1,1,SDIO_IRQn,2);
	SD_CARD_SR_CLEAR();
	
	delay_ms(20);
	
	_SD_Card_Send_Command(0,0,SD_RESPONSE_TYPE_NONE);	//CMD0
	_SD_Card_Send_Command(8,0x1aa,SD_RESPONSE_TYPE_SHORT);	//CMD8����֪���������ѹ��Χ
	_SD_Card_Read_Response(SD_RESPONSE_TYPE_SHORT,SD_RESPONSE_CRC_CARE,&Command_Index,Command);

	_SD_Card_Send_Command(55,0,SD_RESPONSE_TYPE_SHORT);
	_SD_Card_Skip_Response(SD_RESPONSE_CRC_CARE);
	_SD_Card_Send_Command(41,0x40000000,SD_RESPONSE_TYPE_SHORT);	//ACMD41����֪��������SDHC
	_SD_Card_Read_Response(SD_RESPONSE_TYPE_SHORT,SD_RESPONSE_CRC_IGNORE,&Command_Index,Command);
	while (((Command[0]>>31)&1)!=1)	//ѭ������ACMD41��ֱ��SD���ϵ����
	{
		_SD_Card_Send_Application_Command(41,(1<<30)+(1<<20),0,SD_RESPONSE_TYPE_SHORT);
		_SD_Card_Read_Response(SD_RESPONSE_TYPE_SHORT,SD_RESPONSE_CRC_IGNORE,&Command_Index,Command);
	}
	_SD_Card_Send_Command(2,0,SD_RESPONSE_TYPE_LONG);	//CMD2����ȡCID�Ĵ���
	_SD_Card_Skip_Response(SD_RESPONSE_CRC_CARE);
	_SD_Card_Send_Command(3,0,SD_RESPONSE_TYPE_SHORT);	//CMD3����ȡRCA��ַ
	_SD_Card_Read_Response(SD_RESPONSE_TYPE_SHORT,SD_RESPONSE_CRC_CARE,&Command_Index,Command);
	RCA=Command[0]>>16;
	_SD_Card_Send_Command(9,RCA<<16,SD_RESPONSE_TYPE_LONG);	//CMD9����ȡCSD�Ĵ���
	_SD_Card_Read_Response(SD_RESPONSE_TYPE_LONG,SD_RESPONSE_CRC_CARE,&Command_Index,Command);
	_SD_Card_Send_Command(7,RCA<<16,SD_RESPONSE_TYPE_SHORT);	//CMD7�����봫��ģʽ
	_SD_Card_Skip_Response(SD_RESPONSE_CRC_CARE);
	
	_SD_Card_Send_Command(55,RCA<<16,SD_RESPONSE_TYPE_SHORT);//ACM6���������ݿ��4λ
	_SD_Card_Skip_Response(SD_RESPONSE_CRC_CARE);
	_SD_Card_Send_Command(6,2,SD_RESPONSE_TYPE_SHORT);
	_SD_Card_Skip_Response(SD_RESPONSE_CRC_CARE);
	
	SDIO->CLKCR&=~0xff;
	delay_us(500);
	SDIO->CLKCR|=0;	//2��Ƶ��24MHz
	
	DMA2_Stream6->CR|=1<<5;	//������������
	//DMA2_Stream6->CR|=1<<6;	//�洢��������
	DMA2_Stream6->CR|=1<<10;//�洢����ַ����
	DMA2_Stream6->CR|=1<<12;//�������ݴ�С32λ
	//DMA2_Stream6->CR|=1<<14;//�洢�����ݴ�С32λ
	DMA2_Stream6->CR|=1<<21;//����4��������ͻ������
	//DMA2_Stream6->CR|=1<<23;//�洢��4��������ͻ������
	DMA2_Stream6->CR|=3<<23;//�洢��16��������ͻ������
	DMA2_Stream6->CR|=1<<27;//ͨ��4
	
	DMA2_Stream6->FCR|=3<<0;//FIFO��������
	DMA2_Stream6->FCR|=1<<2;//��ֱֹ��ģʽ
	
	DMA2_Stream6->PAR=(u32)&SDIO->FIFO;
	
	Send_Buffer1=(u8*)Malloc(SD_WRITE_BLOCK_SIZE);
	Send_Buffer2=(u8*)Malloc(SD_WRITE_BLOCK_SIZE);
	Receive_Buffer=(u8*)Malloc(SD_READ_BLOCK_SIZE);
}

void SD_Card_Fast_Mode_Enable(TIM_TypeDef* Timer, u8 Interrupt_Group, u8 Pre_Priority, u8 Sub_Priority)
{
	//SDIO->MASK|=1<<8;	//���ݿ�����ж�ʹ��
	GeneralTimer_Reset(Timer,&SD_Timer);
	GeneralTimer_Global_Set(&SD_Timer,APB1_MAX_CLOCK,INSIDE_CLOCK_SOURCE,1000000,TRIGGER_SOFTWARE);
	GeneralTimer_Timing_Init(&SD_Timer);
	GeneralTimer_Timing_Interrupt_Set(&SD_Timer,Interrupt_Group,Pre_Priority,Sub_Priority);
	MY_NVIC_Init(Pre_Priority,Sub_Priority,SDIO_IRQn,Interrupt_Group);
}

void SD_Card_Fast_Mode_On(void)
{
	SD_CARD_SR_CLEAR();
	SDIO->MASK|=1<<8;	//���ݿ�����ж�ʹ��
	Fast_Mode=1;
}

void SD_Card_Fast_Mode_Off(void)
{
	SDIO->MASK&=~(1<<8);	//���ݿ�����ж�ʧ��
	Fast_Mode=0;
}

u8 SD_Card_Data_Ready(void)
{
	return Block_Ready;
}

void _SD_Card_Send_Command(u8 CMD_Index, u32 CMD, u8 Response_Type)
{
	u32 save;
	
	SDIO->ARG=CMD;
	//��SDIO->CMD����������д��ʱ������������3��SDIOCLK����
	save=SDIO->CMD;
	save&=~0xff;
	save|=CMD_Index;
	save|=Response_Type<<6;
	save|=0<<8;
	save|=1<<10;
	SDIO->CMD=save;
	
	while (((SDIO->STA>>11)&1)==1);	//��������ڽ���
	SDIO->ICR|=1<<7;	//�����ѷ�������
}

void _SD_Card_Send_Application_Command(u8 CMD_Index, u32 CMD, u16 RCA, u8 Response_Type)
{
	_SD_Card_Send_Command(55,RCA<<16,SD_RESPONSE_TYPE_SHORT);
	_SD_Card_Skip_Response(SD_RESPONSE_CRC_CARE);
	_SD_Card_Send_Command(CMD_Index,CMD,Response_Type);
}

void _SD_Card_Read_Response(u8 Response_Type, u8 Response_CRC, u8* CMD_Index, void* Buffer)
{
	u32 *save;
	
	save=(u32*)Buffer;
	
	if (Response_CRC==SD_RESPONSE_CRC_CARE)
	{
		while (((SDIO->STA>>6)&1)==0);	//������Ӧδ����
		SDIO->ICR|=1<<6;
	}
	else if (Response_CRC==SD_RESPONSE_CRC_IGNORE)
	{
		while ((SDIO->STA&1)==0);	//������Ӧδ����
		SDIO->ICR|=1<<0;
	}
	
	if (Response_Type==SD_RESPONSE_TYPE_SHORT)
	{
		*CMD_Index=(u8)SDIO->RESPCMD;
		*save=SDIO->RESP1;
	}
	else
	{
		save[0]=SDIO->RESP4;
		save[1]=SDIO->RESP3;
		save[2]=SDIO->RESP2;
		save[3]=SDIO->RESP1;
	}
}

void _SD_Card_Skip_Response(u8 Response_CRC)
{
	if (Response_CRC==SD_RESPONSE_CRC_CARE)
	{
		while (((SDIO->STA>>6)&1)==0);	//������Ӧδ����
		SDIO->ICR|=1<<6;
	}
	else if (Response_CRC==SD_RESPONSE_CRC_IGNORE)
	{
		while ((SDIO->STA&1)==0);	//������Ӧδ����
		SDIO->ICR|=1<<0;
	}
}

u8 _SD_Card_Write_Blocks(u32 Block_Address, u32 Block_Number, void* Data)
{
	if (Block_Number==0)
		return 1;
	
	u32 i;
	
	u32 Max_Length_Transfer_Time,Real_Block_Address;
	u16 Remained_Block_Number;
	
	DMA2_Stream6->CR|=1<<6;	//�洢��������
	DMA2_Stream6->M0AR=(u32)Data;
	
	Real_Block_Address=Block_Address*(SD_WRITE_BLOCK_SIZE/SD_CARD_BLOCK_SIZE);//�����ʵ�������ַ
	if (Block_Number>SD_MAX_DLEN/SD_WRITE_BLOCK_SIZE)//������������󵥴η�������
	{
		//u8 save;
		//save=Fast_Mode;
		Max_Length_Transfer_Time=(Block_Number*SD_WRITE_BLOCK_SIZE)/SD_MAX_DLEN;
		Remained_Block_Number=Block_Number-(Max_Length_Transfer_Time*SD_MAX_DLEN)/SD_WRITE_BLOCK_SIZE;
		//SD_Card_Fast_Mode_Off();
		for (i=0;i<Max_Length_Transfer_Time;i++)
		{
			SDIO->DLEN=SD_MAX_DLEN;
			SDIO->DTIMER=1000000;	//���ݶ�ʱ��ֵ
			
			_SD_Card_Send_Command(25,Real_Block_Address+i*(SD_MAX_DLEN/SD_CARD_BLOCK_SIZE),SD_RESPONSE_TYPE_SHORT);
			_SD_Card_Skip_Response(SD_RESPONSE_CRC_CARE);
			
			DMA2_Stream6->CR|=1;	//DMAʹ��
			SDIO->DCTRL=SDIO_DCTRL_WRITE;
			
			while (Interrupt_Flag==0);
			Interrupt_Flag=0;
			
			_SD_Card_Send_Command(12,0,SD_RESPONSE_TYPE_SHORT);
			_SD_Card_Skip_Response(SD_RESPONSE_CRC_CARE);
			SD_Write_Delay();
		}
		//if (save==1)
		//	SD_Card_Fast_Mode_On();
		if (Remained_Block_Number!=0)//ʣ�»��п�δ����
		{
			DMA2_Stream6->M0AR=(u32)((u32*)Data+(Max_Length_Transfer_Time*SD_MAX_DLEN)/4);
			SDIO->DLEN=Remained_Block_Number*SD_WRITE_BLOCK_SIZE;
			SDIO->DTIMER=1000000;	//���ݶ�ʱ��ֵ
				
			_SD_Card_Send_Command(25,Real_Block_Address+Max_Length_Transfer_Time*(SD_MAX_DLEN/SD_CARD_BLOCK_SIZE),SD_RESPONSE_TYPE_SHORT);
			_SD_Card_Skip_Response(SD_RESPONSE_CRC_CARE);

			DMA2_Stream6->CR|=1;	//DMAʹ��
			SDIO->DCTRL=SDIO_DCTRL_WRITE;
				
			/*if (Fast_Mode==1)
			{
				Block_Ready=0;
				Interrupt_Type=INTERRUPT_TYPE_WRITE;
				GeneralTimer_Timing_SetTime_us(&SD_Timer,WRITE_DELAY_TIME);
				return 2;
			}*/
			while (Interrupt_Flag==0);
			Interrupt_Flag=0;

			_SD_Card_Send_Command(12,0,SD_RESPONSE_TYPE_SHORT);
			_SD_Card_Skip_Response(SD_RESPONSE_CRC_CARE);

			SD_Write_Delay();
		}
		//else
		//	Block_Ready=1;
	}
	else//һ�ο��Է������
	{
		//USART_Printf(USART1,"ll\n");
		SDIO->DLEN=Block_Number*SD_WRITE_BLOCK_SIZE;
		//USART_Printf(USART1,"%d\n",SDIO->DLEN);
		SDIO->DTIMER=1000000;	//���ݶ�ʱ��ֵ
			
		_SD_Card_Send_Command(25,Real_Block_Address,SD_RESPONSE_TYPE_SHORT);
		_SD_Card_Skip_Response(SD_RESPONSE_CRC_CARE);
	
		//SD_CARD_SR_CLEAR();
		DMA2_Stream6->CR|=1;	//DMAʹ��
		SDIO->DCTRL=SDIO_DCTRL_WRITE;
			
		/*if (Fast_Mode==1)
		{
			Block_Ready=0;
			Interrupt_Type=INTERRUPT_TYPE_WRITE;
			GeneralTimer_Timing_SetTime_us(&SD_Timer,WRITE_DELAY_TIME);
			return 2;
		}*/
		//while (((SDIO->STA>>8)&1)==0);
		
		while (Interrupt_Flag==0);
		Interrupt_Flag=0;

		_SD_Card_Send_Command(12,0,SD_RESPONSE_TYPE_SHORT);
		_SD_Card_Skip_Response(SD_RESPONSE_CRC_CARE);

		SD_Write_Delay();
	}
	
	return 0;
}

u8 _SD_Card_Read_Blocks(u32 Block_Address, u32 Block_Number, void* Data)
{
	if (Block_Number==0)
		return 1;

	u32 i;
	
	u32 Max_Length_Transfer_Time;
	u16 Remained_Block_Number;
	
	DMA2_Stream6->CR&=~(1<<6);	//���赽�洢��
	DMA2_Stream6->M0AR=(u32)Data;
	
	if (Block_Number>SD_MAX_DLEN/SD_READ_BLOCK_SIZE)
	{
		Max_Length_Transfer_Time=(Block_Number*SD_READ_BLOCK_SIZE)/SD_MAX_DLEN;
		Remained_Block_Number=Block_Number-(Max_Length_Transfer_Time*SD_MAX_DLEN)/SD_READ_BLOCK_SIZE;
		for (i=0;i<Max_Length_Transfer_Time;i++)
		{
			SDIO->DLEN=SD_MAX_DLEN;
			SDIO->DTIMER=1000000;	//���ݶ�ʱ��ֵ
			
			_SD_Card_Send_Command(18,Block_Address,SD_RESPONSE_TYPE_SHORT);
			_SD_Card_Skip_Response(SD_RESPONSE_CRC_CARE);
			
			DMA2_Stream6->CR|=1;	//DMAʹ��
			SDIO->DCTRL=SDIO_DCTRL_READ;
			
			while (Interrupt_Flag==0);
			Interrupt_Flag=0;
			
			_SD_Card_Send_Command(12,0,SD_RESPONSE_TYPE_SHORT);
			_SD_Card_Skip_Response(SD_RESPONSE_CRC_CARE);
		}
		if (Remained_Block_Number!=0)
		{
			DMA2_Stream6->M0AR=(u32)((u32*)Data+(Max_Length_Transfer_Time*SD_MAX_DLEN)/4);
			SDIO->DLEN=Remained_Block_Number*SD_READ_BLOCK_SIZE;
			SDIO->DTIMER=1000000;	//���ݶ�ʱ��ֵ
				
			_SD_Card_Send_Command(18,Block_Address,SD_RESPONSE_TYPE_SHORT);
			_SD_Card_Skip_Response(SD_RESPONSE_CRC_CARE);
				
			DMA2_Stream6->CR|=1;	//DMAʹ��
			SDIO->DCTRL=SDIO_DCTRL_READ;

			/*if (Fast_Mode==1)
			{
				Block_Ready=0;
				Interrupt_Type=INTERRUPT_TYPE_READ;
				GeneralTimer_Timing_SetTime_us(&SD_Timer,READ_DELAY_TIME);
				return 2;
			}*/
			while (Interrupt_Flag==0);
			Interrupt_Flag=0;
			
			_SD_Card_Send_Command(12,0,SD_RESPONSE_TYPE_SHORT);
			_SD_Card_Skip_Response(SD_RESPONSE_CRC_CARE);
		}
	}
	else
	{
		SDIO->DLEN=Block_Number*SD_READ_BLOCK_SIZE;
		SDIO->DTIMER=1000000;	//���ݶ�ʱ��ֵ
			
		_SD_Card_Send_Command(18,Block_Address,SD_RESPONSE_TYPE_SHORT);
		_SD_Card_Skip_Response(SD_RESPONSE_CRC_CARE);
			
		SD_CARD_SR_CLEAR();
		DMA2_Stream6->CR|=1;	//DMAʹ��
		SDIO->DCTRL=SDIO_DCTRL_READ;

		/*if (Fast_Mode==1)
		{
			Block_Ready=0;
			Interrupt_Type=INTERRUPT_TYPE_READ;
			GeneralTimer_Timing_SetTime_us(&SD_Timer,READ_DELAY_TIME);
			return 2;
		}*/
		//while (((SDIO->STA>>8)&1)==0);
		
		while (Interrupt_Flag==0);
		Interrupt_Flag=0;
		//while (
		
		_SD_Card_Send_Command(12,0,SD_RESPONSE_TYPE_SHORT);
		_SD_Card_Skip_Response(SD_RESPONSE_CRC_CARE);
		
		//delay_ms(5);
	}
	
	return 0;
}

u8 SD_Card_Write_Data(long long unsigned int Byte_Address, long long unsigned int Length, void* Data)
{
	u16 i;
	
	/*�ú����ڵĿ��ַ�����߼���ַ*/
	u32 Start_Block_Address,End_Block_Address;//��ʼ���ַ���������ַ
	u16 Start_Block_Valid_Length,End_Block_Valid_Length;//��ʼ�����Ч���ȡ��������ַ
	
	Start_Block_Address=Byte_Address/SD_WRITE_BLOCK_SIZE;
	End_Block_Address=(Byte_Address+Length)/SD_WRITE_BLOCK_SIZE;
	
	Start_Block_Valid_Length=SD_WRITE_BLOCK_SIZE-Byte_Address%SD_WRITE_BLOCK_SIZE;
	End_Block_Valid_Length=(Byte_Address+Length)%SD_WRITE_BLOCK_SIZE;
	if ((Start_Block_Valid_Length==SD_WRITE_BLOCK_SIZE)&&(End_Block_Valid_Length==0))	//ͷβ�����
	{
		_SD_Card_Write_Blocks(Start_Block_Address,End_Block_Address-Start_Block_Address,Data);
	}
	else if ((Start_Block_Valid_Length==SD_WRITE_BLOCK_SIZE)&&(End_Block_Valid_Length!=0))	//ͷ���룬β������
	{
		_SD_Card_Read_Blocks(End_Block_Address*2,2,Send_Buffer1);
		for (i=0;i<End_Block_Valid_Length;i++)
			Send_Buffer1[i]=((u8*)Data)[Length-End_Block_Valid_Length+i];
		_SD_Card_Write_Blocks(Start_Block_Address,End_Block_Address-Start_Block_Address,Data);
		_SD_Card_Write_Blocks(End_Block_Address,1,Send_Buffer1);
	}
	else if ((Start_Block_Valid_Length!=SD_WRITE_BLOCK_SIZE)&&(End_Block_Valid_Length==0))	//ͷ�����룬β����
	{
		_SD_Card_Read_Blocks(Start_Block_Address*2,2,Send_Buffer1);
		for (i=0;i<Start_Block_Valid_Length;i++)
			Send_Buffer1[SD_WRITE_BLOCK_SIZE-Start_Block_Valid_Length+i]=((u8*)Data)[i];
		_SD_Card_Write_Blocks(Start_Block_Address,1,Send_Buffer1);
		_SD_Card_Write_Blocks(Start_Block_Address+1,End_Block_Address-Start_Block_Address-1,((u8*)Data)+Start_Block_Valid_Length);
	}
	else	//ͷβ��������
	{
		if ((Length<SD_WRITE_BLOCK_SIZE)&&(Start_Block_Address==End_Block_Address))	//����С�ڿ��С������������λ��һ������
		{
			_SD_Card_Read_Blocks(Start_Block_Address*2,2,Send_Buffer1);
			for (i=0;i<Length;i++)
				Send_Buffer1[SD_WRITE_BLOCK_SIZE-Start_Block_Valid_Length+i]=((u8*)Data)[i];
			_SD_Card_Write_Blocks(Start_Block_Address,1,Send_Buffer1);
			
			return 0;
		}
		
		_SD_Card_Read_Blocks(Start_Block_Address*2,2,Send_Buffer1);
		for (i=0;i<Start_Block_Valid_Length;i++)
			Send_Buffer1[SD_WRITE_BLOCK_SIZE-Start_Block_Valid_Length+i]=((u8*)Data)[i];
		_SD_Card_Read_Blocks(End_Block_Address*2,2,Send_Buffer2);
		for (i=0;i<End_Block_Valid_Length;i++)
			Send_Buffer2[i]=((u8*)Data)[Length-End_Block_Valid_Length+i];
		_SD_Card_Write_Blocks(Start_Block_Address,1,Send_Buffer1);
		_SD_Card_Write_Blocks(Start_Block_Address+1,End_Block_Address-Start_Block_Address-1,((u8*)Data)+Start_Block_Valid_Length);
		_SD_Card_Write_Blocks(End_Block_Address,1,Send_Buffer2);
	}
	
	return 0;
}

u8 SD_Card_Read_Data(long long unsigned int Byte_Address, long long unsigned int Length, void* Data)
{
	u16 i;
	
	u32 Start_Block_Address,End_Block_Address;
	u16 Start_Block_Valid_Length,End_Block_Valid_Length;
	
	Start_Block_Address=Byte_Address/SD_READ_BLOCK_SIZE;
	End_Block_Address=(Byte_Address+Length)/SD_READ_BLOCK_SIZE;
	
	Start_Block_Valid_Length=SD_READ_BLOCK_SIZE-Byte_Address%SD_READ_BLOCK_SIZE;
	End_Block_Valid_Length=(Byte_Address+Length)%SD_READ_BLOCK_SIZE;
	if ((Start_Block_Valid_Length==SD_READ_BLOCK_SIZE)&&(End_Block_Valid_Length==0))
		_SD_Card_Read_Blocks(Start_Block_Address,End_Block_Address-Start_Block_Address,Data);
	else if ((Start_Block_Valid_Length==SD_READ_BLOCK_SIZE)&&(End_Block_Valid_Length!=0))	//ͷ���룬β������
	{
		_SD_Card_Read_Blocks(Start_Block_Address,End_Block_Address-Start_Block_Address,Data);
		_SD_Card_Read_Blocks(End_Block_Address,1,Receive_Buffer);
		for (i=0;i<End_Block_Valid_Length;i++)
			((u8*)Data)[Length-End_Block_Valid_Length+i]=Receive_Buffer[i];
	}
	else if ((Start_Block_Valid_Length!=SD_READ_BLOCK_SIZE)&&(End_Block_Valid_Length==0))	//ͷ�����룬β����
	{
		_SD_Card_Read_Blocks(Start_Block_Address,1,Receive_Buffer);
		for (i=0;i<Start_Block_Valid_Length;i++)
			((u8*)Data)[i]=Receive_Buffer[SD_READ_BLOCK_SIZE-Start_Block_Valid_Length+i];
		_SD_Card_Read_Blocks(Start_Block_Address+1,End_Block_Address-Start_Block_Address-1,((u8*)Data)+Start_Block_Valid_Length);
	}
	else	//ͷβ��������
	{
		if ((Length<SD_READ_BLOCK_SIZE)&&(Start_Block_Address==End_Block_Address))	//����С�ڿ��С������������λ��һ������
		{
			_SD_Card_Read_Blocks(Start_Block_Address,1,Receive_Buffer);
			for (i=0;i<Length;i++)
				((u8*)Data)[i]=Receive_Buffer[SD_READ_BLOCK_SIZE-Start_Block_Valid_Length+i];
			
			return 0;
		}
		
		_SD_Card_Read_Blocks(Start_Block_Address,1,Receive_Buffer);
		for (i=0;i<Start_Block_Valid_Length;i++)
			((u8*)Data)[i]=Receive_Buffer[SD_READ_BLOCK_SIZE-Start_Block_Valid_Length+i];
		_SD_Card_Read_Blocks(End_Block_Address,1,Receive_Buffer);
		for (i=0;i<End_Block_Valid_Length;i++)
			((u8*)Data)[Length-End_Block_Valid_Length+i]=Receive_Buffer[i];
		_SD_Card_Read_Blocks(Start_Block_Address+1,End_Block_Address-Start_Block_Address-1,((u8*)Data)+Start_Block_Valid_Length);
	}
	
	return 0;
}

u8 SD_Card_Set_Memory(long long unsigned int Byte_Address, long long unsigned int Length, u8 Data)
{
	u16 i;
	
	u32 Start_Block_Address,End_Block_Address;
	u16 Start_Block_Valid_Length,End_Block_Valid_Length;
	
	Start_Block_Address=Byte_Address/SD_WRITE_BLOCK_SIZE;
	End_Block_Address=(Byte_Address+Length)/SD_WRITE_BLOCK_SIZE;
	
	Start_Block_Valid_Length=SD_WRITE_BLOCK_SIZE-Byte_Address%SD_WRITE_BLOCK_SIZE;
	End_Block_Valid_Length=(Byte_Address+Length)%SD_WRITE_BLOCK_SIZE;
	
	if ((Start_Block_Valid_Length==SD_WRITE_BLOCK_SIZE)&&(End_Block_Valid_Length==0))	//ͷβ�����
	{
		while ((DMA2_Stream6->CR&1)!=0);
		DMA2_Stream6->CR&=~(1<<10);//�洢����ַ������
		_SD_Card_Write_Blocks(Start_Block_Address,End_Block_Address-Start_Block_Address,&Data);
	}
	else if ((Start_Block_Valid_Length==SD_WRITE_BLOCK_SIZE)&&(End_Block_Valid_Length!=0))	//ͷ���룬β������
	{
		_SD_Card_Read_Blocks(End_Block_Address*2,2,Send_Buffer1);
		for (i=0;i<End_Block_Valid_Length;i++)
			Send_Buffer1[i]=Data;
		_SD_Card_Write_Blocks(End_Block_Address,1,Send_Buffer1);
		while ((DMA2_Stream6->CR&1)!=0);
		DMA2_Stream6->CR&=~(1<<10);//�洢����ַ������
		_SD_Card_Write_Blocks(Start_Block_Address,End_Block_Address-Start_Block_Address,&Data);
	}
	else if ((Start_Block_Valid_Length!=SD_WRITE_BLOCK_SIZE)&&(End_Block_Valid_Length==0))	//ͷ�����룬β����
	{
		_SD_Card_Read_Blocks(Start_Block_Address*2,2,Send_Buffer1);
		for (i=0;i<Start_Block_Valid_Length;i++)
			Send_Buffer1[SD_WRITE_BLOCK_SIZE-Start_Block_Valid_Length+i]=Data;
		_SD_Card_Write_Blocks(Start_Block_Address,1,Send_Buffer1);
		while ((DMA2_Stream6->CR&1)!=0);
		DMA2_Stream6->CR&=~(1<<10);//�洢����ַ������
		_SD_Card_Write_Blocks(Start_Block_Address+1,End_Block_Address-Start_Block_Address,&Data);
	}
	else	//ͷβ��������
	{
		if ((Length<SD_WRITE_BLOCK_SIZE)&&(Start_Block_Address==End_Block_Address))	//����С�ڿ��С������������λ��һ������
		{
			_SD_Card_Read_Blocks(Start_Block_Address*2,2,Send_Buffer1);
			for (i=0;i<Length;i++)
				Send_Buffer1[SD_WRITE_BLOCK_SIZE-Start_Block_Valid_Length+i]=Data;
			_SD_Card_Write_Blocks(Start_Block_Address,1,Send_Buffer1);
			
			return 0;
		}
		_SD_Card_Read_Blocks(Start_Block_Address*2,2,Send_Buffer1);
		for (i=0;i<Start_Block_Valid_Length;i++)
			Send_Buffer1[SD_WRITE_BLOCK_SIZE-Start_Block_Valid_Length+i]=Data;
		_SD_Card_Read_Blocks(End_Block_Address*2,2,Send_Buffer2);
		for (i=0;i<End_Block_Valid_Length;i++)
			Send_Buffer2[i]=Data;
		_SD_Card_Write_Blocks(Start_Block_Address,1,Send_Buffer1);
		_SD_Card_Write_Blocks(End_Block_Address,1,Send_Buffer2);
		while ((DMA2_Stream6->CR&1)!=0);
		DMA2_Stream6->CR&=~(1<<10);//�洢����ַ������
		_SD_Card_Write_Blocks(Start_Block_Address+1,End_Block_Address-Start_Block_Address-1,&Data);
	}
	
	while ((DMA2_Stream6->CR&1)!=0);
	DMA2_Stream6->CR|=1<<10;//�洢����ַ����
	return 0;
}

#ifdef SD_CARD_DEBUG

void SD_Card_Test(USART_TypeDef* USART)
{
	u32 save;
	GeneralTimer_Info Timer5;
	
	GeneralTimer_Reset(TIM5,&Timer5);
	GeneralTimer_Global_Set(&Timer5,APB1_MAX_CLOCK,INSIDE_CLOCK_SOURCE,4000000,TRIGGER_SOFTWARE);
	GeneralTimer_Free_Run_Init(&Timer5);
	USART_Printf(USART,"\n/*******************SD������*******************/\n");
	USART_Printf(USART,"SD��д���ԣ�\n");
	SD_Card_Write_Data(1024*1024,1024,(u8*)0x20000000);
	SD_Card_Write_Data(1024*1024,1024,(u8*)0x20000000);
	TIM5->CNT=0;
	GeneralTimer_Enable(&Timer5);
	SD_Card_Write_Data(1024*1024,1024,(u8*)0x20000000);
	save=TIM5->CNT;
	GeneralTimer_Disable(&Timer5);
	USART_Printf(USART,"1KB���д�ٶȣ�%.1f KB/s\n",1.0f/((float)save/4000000));
	
	TIM5->CNT=0;
	GeneralTimer_Enable(&Timer5);
	SD_Card_Write_Data(1024*1024+2,1024,(u8*)0x20000000);
	save=TIM5->CNT;
	GeneralTimer_Disable(&Timer5);
	USART_Printf(USART,"1KB���д�ٶȣ�%.1f KB/s\n",1.0f/((float)save/4000000));
	
	TIM5->CNT=0;
	GeneralTimer_Enable(&Timer5);
	SD_Card_Write_Data(1024*1024,10240,(u8*)0x20000000);
	save=TIM5->CNT;
	GeneralTimer_Disable(&Timer5);
	USART_Printf(USART,"10KB���д�ٶȣ�%.1f KB/s\n",10.0f/((float)save/4000000));
	
	TIM5->CNT=0;
	GeneralTimer_Enable(&Timer5);
	SD_Card_Write_Data(1024*1024+2,10240,(u8*)0x20000000);
	save=TIM5->CNT;
	GeneralTimer_Disable(&Timer5);
	USART_Printf(USART,"10KB���д�ٶȣ�%.1f KB/s\n",10.0f/((float)save/4000000));
	
	USART_Printf(USART,"SD�������ԣ�\n");
	SD_Card_Read_Data(1024*1024,1024,Send_Buffer1);
	SD_Card_Read_Data(1024*1024,1024,Send_Buffer1);
	TIM5->CNT=0;
	GeneralTimer_Enable(&Timer5);
	SD_Card_Read_Data(1024*1024,1024,Send_Buffer1);
	save=TIM5->CNT;
	GeneralTimer_Disable(&Timer5);
	USART_Printf(USART,"1KB��߶��ٶȣ�%.1f KB/s\n",1.0f/((float)save/4000000));
	
	TIM5->CNT=0;
	GeneralTimer_Enable(&Timer5);
	SD_Card_Read_Data(1024*1024+2,1024,Send_Buffer1);
	save=TIM5->CNT;
	GeneralTimer_Disable(&Timer5);
	USART_Printf(USART,"1KB��Ͷ��ٶȣ�%.1f KB/s\n",1.0f/((float)save/4000000));
	USART_Printf(USART,"/*******************SD�����Խ���*******************/\n");
	/*TIM5->CNT=0;
	GeneralTimer_Enable(&Timer5);
	SD_Card_Read_Data(1024*1024+2,10240,(u8*)0x20000000);
	save=TIM5->CNT;
	GeneralTimer_Disable(&Timer5);
	USART_Printf(USART,"10KB���д�ٶȣ�%.1f\n",10.0f/((float)save/2000000));
	
	TIM5->CNT=0;
	GeneralTimer_Enable(&Timer5);
	SD_Card_Read_Data(1024*1024+2,10240,(u8*)0x20000000);
	save=TIM5->CNT;
	GeneralTimer_Disable(&Timer5);
	USART_Printf(USART,"10KB���д�ٶȣ�%.1f\n",10.0f/((float)save/2000000));*/
	
}

#endif

void SDIO_IRQHandler(void)
{
	//USART_Printf(USART1,"in\n");
	if (((SDIO->STA>>8)&1)==1)
	{	
		/*_SD_Card_Send_Command(12,0,SD_RESPONSE_TYPE_SHORT);
		_SD_Card_Skip_Response(SD_RESPONSE_CRC_CARE);
		GeneralTimer_Enable(&SD_Timer);*/
		Interrupt_Flag=1;
	}
	SD_CARD_SR_CLEAR();
	SD_DMA2_SR_CLEAR();
}

void TIMx_IRQHandler(void)
{
	Block_Ready=1;
	GeneralTimer_Clear_UIF(&SD_Timer);
}


