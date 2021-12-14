#include "sys.h"
#include "delay.h"
#include "SD_Card.h"
#include "USART.h"
#include "Memory.h"
#include "Timer.h"

#define SD_READ_BLOCK_SIZE	 512	//读取块大小
#define SD_WRITE_BLOCK_SIZE	 1024	//写入块大小
#define SD_CARD_BLOCK_SIZE	 512	//SDHC卡固定块大小
#define SD_MAX_BLOCK_ADDRESS 67108863		//SD卡最大块地址
#define SD_MAX_BYTE_ADDRESS  34359738367	//SD卡最大字节地址（4GB-1B）
#define SD_MAX_DLEN			 131072	//最大单次传输数据量（128*1024字节）

/*命令的响应类型*/
#define SD_RESPONSE_TYPE_NONE	0	//无响应
#define SD_RESPONSE_TYPE_SHORT  1	//短响应
#define SD_RESPONSE_TYPE_LONG	3	//长响应

/*读取响应时是否检查CRC*/
#define SD_RESPONSE_CRC_CARE		0	//检查
#define SD_RESPONSE_CRC_IGNORE		1	//不检查

#define SD_DMA2_SR_CLEAR() DMA2->HIFCR|=0xf<<18	//清除DMA中断标志
#define SD_CARD_SR_CLEAR() SDIO->ICR|=0xc007ff	//清除SDIO中断标志

#define SDIO_DCTRL_READ 	 0x0000009b	//执行读操作时，SDIO->DCTRL的配置
#define SDIO_DCTRL_WRITE	 0x00000099 //执行写操作时，SDIO->DCTRL的配置

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
u32 RCA;	//SD卡地址

u8 *Send_Buffer1,*Send_Buffer2;	//两个发送缓冲
u8* Receive_Buffer;				//一个接收缓冲

GeneralTimer_Info SD_Timer;

/*发送一个CMD命令*/
void _SD_Card_Send_Command(u8 CMD_Index, u32 CMD, u8 Response_Type);

/*发送一个ACMD命令*/
void _SD_Card_Send_Application_Command(u8 CMD_Index, u32 CMD, u16 RCA, u8 Response_Type);

/*读取响应*/
void _SD_Card_Read_Response(u8 Response_Type, u8 Response_CRC, u8* CMD_Index, void* Buffer);

/*跳过响应（不读取）*/
void _SD_Card_Skip_Response(u8 Response_CRC);

/*写几个连续块，这里块大小由SD_WRITE_BLOCK_SIZE定义，地址也会发生改变*/
u8 _SD_Card_Write_Blocks(u32 Block_Address, u32 Block_Number, void* Data);

/*读取几个连续的块，块大小由SD_READ_BLOCK_SIZE定义，地址也会发生改变*/
u8 _SD_Card_Read_Blocks(u32 Block_Address, u32 Block_Number, void* Data);

void SD_Card_Init(void)
{
	u8 Command_Index;
	u32 Command[4];
	
	RCC->AHB1ENR|=1<<2;	//GPIOC时钟使能
	RCC->AHB1ENR|=1<<3;	//GPIOD时钟使能
	RCC->AHB1ENR|=1<<22;//DMA2时钟使能
	RCC->APB2ENR|=1<<11;//SDIO时钟使能
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

	SDIO->POWER|=3;		//SDIO上电
	SDIO->CLKCR|=158;	//时钟160分频，300kHz
	delay_us(500);		//该寄存器两次连续写入时间间隔不能小于三个SDIO时钟周期
	SDIO->CLKCR|=1<<8;	//使能时钟
	delay_us(500);
	SDIO->CLKCR|=1<<11;	//4位总线宽度
	delay_us(500);
	//SDIO->CLKCR|=1<<14;	//开启硬件流控制
	
	SDIO->MASK|=1<<8;	//数据块结束中断使能
	//MY_NVIC_Init(Pre_Priority,Sub_Priority,SDIO_IRQn,Interrupt_Group);
	MY_NVIC_Init(1,1,SDIO_IRQn,2);
	SD_CARD_SR_CLEAR();
	
	delay_ms(20);
	
	_SD_Card_Send_Command(0,0,SD_RESPONSE_TYPE_NONE);	//CMD0
	_SD_Card_Send_Command(8,0x1aa,SD_RESPONSE_TYPE_SHORT);	//CMD8，告知主机允许电压范围
	_SD_Card_Read_Response(SD_RESPONSE_TYPE_SHORT,SD_RESPONSE_CRC_CARE,&Command_Index,Command);

	_SD_Card_Send_Command(55,0,SD_RESPONSE_TYPE_SHORT);
	_SD_Card_Skip_Response(SD_RESPONSE_CRC_CARE);
	_SD_Card_Send_Command(41,0x40000000,SD_RESPONSE_TYPE_SHORT);	//ACMD41，告知主机允许SDHC
	_SD_Card_Read_Response(SD_RESPONSE_TYPE_SHORT,SD_RESPONSE_CRC_IGNORE,&Command_Index,Command);
	while (((Command[0]>>31)&1)!=1)	//循环发送ACMD41，直到SD卡上电完成
	{
		_SD_Card_Send_Application_Command(41,(1<<30)+(1<<20),0,SD_RESPONSE_TYPE_SHORT);
		_SD_Card_Read_Response(SD_RESPONSE_TYPE_SHORT,SD_RESPONSE_CRC_IGNORE,&Command_Index,Command);
	}
	_SD_Card_Send_Command(2,0,SD_RESPONSE_TYPE_LONG);	//CMD2，获取CID寄存器
	_SD_Card_Skip_Response(SD_RESPONSE_CRC_CARE);
	_SD_Card_Send_Command(3,0,SD_RESPONSE_TYPE_SHORT);	//CMD3，获取RCA地址
	_SD_Card_Read_Response(SD_RESPONSE_TYPE_SHORT,SD_RESPONSE_CRC_CARE,&Command_Index,Command);
	RCA=Command[0]>>16;
	_SD_Card_Send_Command(9,RCA<<16,SD_RESPONSE_TYPE_LONG);	//CMD9，获取CSD寄存器
	_SD_Card_Read_Response(SD_RESPONSE_TYPE_LONG,SD_RESPONSE_CRC_CARE,&Command_Index,Command);
	_SD_Card_Send_Command(7,RCA<<16,SD_RESPONSE_TYPE_SHORT);	//CMD7，进入传输模式
	_SD_Card_Skip_Response(SD_RESPONSE_CRC_CARE);
	
	_SD_Card_Send_Command(55,RCA<<16,SD_RESPONSE_TYPE_SHORT);//ACM6，设置数据宽度4位
	_SD_Card_Skip_Response(SD_RESPONSE_CRC_CARE);
	_SD_Card_Send_Command(6,2,SD_RESPONSE_TYPE_SHORT);
	_SD_Card_Skip_Response(SD_RESPONSE_CRC_CARE);
	
	SDIO->CLKCR&=~0xff;
	delay_us(500);
	SDIO->CLKCR|=0;	//2分频，24MHz
	
	DMA2_Stream6->CR|=1<<5;	//外设流控制器
	//DMA2_Stream6->CR|=1<<6;	//存储器到外设
	DMA2_Stream6->CR|=1<<10;//存储器地址递增
	DMA2_Stream6->CR|=1<<12;//外设数据大小32位
	//DMA2_Stream6->CR|=1<<14;//存储器数据大小32位
	DMA2_Stream6->CR|=1<<21;//外设4节拍增量突发传输
	//DMA2_Stream6->CR|=1<<23;//存储器4节拍增量突发传输
	DMA2_Stream6->CR|=3<<23;//存储器16节拍增量突发传输
	DMA2_Stream6->CR|=1<<27;//通道4
	
	DMA2_Stream6->FCR|=3<<0;//FIFO完整容量
	DMA2_Stream6->FCR|=1<<2;//禁止直接模式
	
	DMA2_Stream6->PAR=(u32)&SDIO->FIFO;
	
	Send_Buffer1=(u8*)Malloc(SD_WRITE_BLOCK_SIZE);
	Send_Buffer2=(u8*)Malloc(SD_WRITE_BLOCK_SIZE);
	Receive_Buffer=(u8*)Malloc(SD_READ_BLOCK_SIZE);
}

void SD_Card_Fast_Mode_Enable(TIM_TypeDef* Timer, u8 Interrupt_Group, u8 Pre_Priority, u8 Sub_Priority)
{
	//SDIO->MASK|=1<<8;	//数据块结束中断使能
	GeneralTimer_Reset(Timer,&SD_Timer);
	GeneralTimer_Global_Set(&SD_Timer,APB1_MAX_CLOCK,INSIDE_CLOCK_SOURCE,1000000,TRIGGER_SOFTWARE);
	GeneralTimer_Timing_Init(&SD_Timer);
	GeneralTimer_Timing_Interrupt_Set(&SD_Timer,Interrupt_Group,Pre_Priority,Sub_Priority);
	MY_NVIC_Init(Pre_Priority,Sub_Priority,SDIO_IRQn,Interrupt_Group);
}

void SD_Card_Fast_Mode_On(void)
{
	SD_CARD_SR_CLEAR();
	SDIO->MASK|=1<<8;	//数据块结束中断使能
	Fast_Mode=1;
}

void SD_Card_Fast_Mode_Off(void)
{
	SDIO->MASK&=~(1<<8);	//数据块结束中断失能
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
	//对SDIO->CMD的两次连续写入时间间隔不能少于3个SDIOCLK周期
	save=SDIO->CMD;
	save&=~0xff;
	save|=CMD_Index;
	save|=Response_Type<<6;
	save|=0<<8;
	save|=1<<10;
	SDIO->CMD=save;
	
	while (((SDIO->STA>>11)&1)==1);	//命令传输正在进行
	SDIO->ICR|=1<<7;	//命令已发送清零
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
		while (((SDIO->STA>>6)&1)==0);	//命令响应未接收
		SDIO->ICR|=1<<6;
	}
	else if (Response_CRC==SD_RESPONSE_CRC_IGNORE)
	{
		while ((SDIO->STA&1)==0);	//命令响应未接收
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
		while (((SDIO->STA>>6)&1)==0);	//命令响应未接收
		SDIO->ICR|=1<<6;
	}
	else if (Response_CRC==SD_RESPONSE_CRC_IGNORE)
	{
		while ((SDIO->STA&1)==0);	//命令响应未接收
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
	
	DMA2_Stream6->CR|=1<<6;	//存储器到外设
	DMA2_Stream6->M0AR=(u32)Data;
	
	Real_Block_Address=Block_Address*(SD_WRITE_BLOCK_SIZE/SD_CARD_BLOCK_SIZE);//换算成实际物理地址
	if (Block_Number>SD_MAX_DLEN/SD_WRITE_BLOCK_SIZE)//数据量大于最大单次发送数量
	{
		//u8 save;
		//save=Fast_Mode;
		Max_Length_Transfer_Time=(Block_Number*SD_WRITE_BLOCK_SIZE)/SD_MAX_DLEN;
		Remained_Block_Number=Block_Number-(Max_Length_Transfer_Time*SD_MAX_DLEN)/SD_WRITE_BLOCK_SIZE;
		//SD_Card_Fast_Mode_Off();
		for (i=0;i<Max_Length_Transfer_Time;i++)
		{
			SDIO->DLEN=SD_MAX_DLEN;
			SDIO->DTIMER=1000000;	//数据定时器值
			
			_SD_Card_Send_Command(25,Real_Block_Address+i*(SD_MAX_DLEN/SD_CARD_BLOCK_SIZE),SD_RESPONSE_TYPE_SHORT);
			_SD_Card_Skip_Response(SD_RESPONSE_CRC_CARE);
			
			DMA2_Stream6->CR|=1;	//DMA使能
			SDIO->DCTRL=SDIO_DCTRL_WRITE;
			
			while (Interrupt_Flag==0);
			Interrupt_Flag=0;
			
			_SD_Card_Send_Command(12,0,SD_RESPONSE_TYPE_SHORT);
			_SD_Card_Skip_Response(SD_RESPONSE_CRC_CARE);
			SD_Write_Delay();
		}
		//if (save==1)
		//	SD_Card_Fast_Mode_On();
		if (Remained_Block_Number!=0)//剩下还有块未发送
		{
			DMA2_Stream6->M0AR=(u32)((u32*)Data+(Max_Length_Transfer_Time*SD_MAX_DLEN)/4);
			SDIO->DLEN=Remained_Block_Number*SD_WRITE_BLOCK_SIZE;
			SDIO->DTIMER=1000000;	//数据定时器值
				
			_SD_Card_Send_Command(25,Real_Block_Address+Max_Length_Transfer_Time*(SD_MAX_DLEN/SD_CARD_BLOCK_SIZE),SD_RESPONSE_TYPE_SHORT);
			_SD_Card_Skip_Response(SD_RESPONSE_CRC_CARE);

			DMA2_Stream6->CR|=1;	//DMA使能
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
	else//一次可以发送完毕
	{
		//USART_Printf(USART1,"ll\n");
		SDIO->DLEN=Block_Number*SD_WRITE_BLOCK_SIZE;
		//USART_Printf(USART1,"%d\n",SDIO->DLEN);
		SDIO->DTIMER=1000000;	//数据定时器值
			
		_SD_Card_Send_Command(25,Real_Block_Address,SD_RESPONSE_TYPE_SHORT);
		_SD_Card_Skip_Response(SD_RESPONSE_CRC_CARE);
	
		//SD_CARD_SR_CLEAR();
		DMA2_Stream6->CR|=1;	//DMA使能
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
	
	DMA2_Stream6->CR&=~(1<<6);	//外设到存储器
	DMA2_Stream6->M0AR=(u32)Data;
	
	if (Block_Number>SD_MAX_DLEN/SD_READ_BLOCK_SIZE)
	{
		Max_Length_Transfer_Time=(Block_Number*SD_READ_BLOCK_SIZE)/SD_MAX_DLEN;
		Remained_Block_Number=Block_Number-(Max_Length_Transfer_Time*SD_MAX_DLEN)/SD_READ_BLOCK_SIZE;
		for (i=0;i<Max_Length_Transfer_Time;i++)
		{
			SDIO->DLEN=SD_MAX_DLEN;
			SDIO->DTIMER=1000000;	//数据定时器值
			
			_SD_Card_Send_Command(18,Block_Address,SD_RESPONSE_TYPE_SHORT);
			_SD_Card_Skip_Response(SD_RESPONSE_CRC_CARE);
			
			DMA2_Stream6->CR|=1;	//DMA使能
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
			SDIO->DTIMER=1000000;	//数据定时器值
				
			_SD_Card_Send_Command(18,Block_Address,SD_RESPONSE_TYPE_SHORT);
			_SD_Card_Skip_Response(SD_RESPONSE_CRC_CARE);
				
			DMA2_Stream6->CR|=1;	//DMA使能
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
		SDIO->DTIMER=1000000;	//数据定时器值
			
		_SD_Card_Send_Command(18,Block_Address,SD_RESPONSE_TYPE_SHORT);
		_SD_Card_Skip_Response(SD_RESPONSE_CRC_CARE);
			
		SD_CARD_SR_CLEAR();
		DMA2_Stream6->CR|=1;	//DMA使能
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
	
	/*该函数内的块地址都是逻辑地址*/
	u32 Start_Block_Address,End_Block_Address;//起始块地址、结束块地址
	u16 Start_Block_Valid_Length,End_Block_Valid_Length;//起始块的有效长度、结束块地址
	
	Start_Block_Address=Byte_Address/SD_WRITE_BLOCK_SIZE;
	End_Block_Address=(Byte_Address+Length)/SD_WRITE_BLOCK_SIZE;
	
	Start_Block_Valid_Length=SD_WRITE_BLOCK_SIZE-Byte_Address%SD_WRITE_BLOCK_SIZE;
	End_Block_Valid_Length=(Byte_Address+Length)%SD_WRITE_BLOCK_SIZE;
	if ((Start_Block_Valid_Length==SD_WRITE_BLOCK_SIZE)&&(End_Block_Valid_Length==0))	//头尾块对齐
	{
		_SD_Card_Write_Blocks(Start_Block_Address,End_Block_Address-Start_Block_Address,Data);
	}
	else if ((Start_Block_Valid_Length==SD_WRITE_BLOCK_SIZE)&&(End_Block_Valid_Length!=0))	//头对齐，尾不对齐
	{
		_SD_Card_Read_Blocks(End_Block_Address*2,2,Send_Buffer1);
		for (i=0;i<End_Block_Valid_Length;i++)
			Send_Buffer1[i]=((u8*)Data)[Length-End_Block_Valid_Length+i];
		_SD_Card_Write_Blocks(Start_Block_Address,End_Block_Address-Start_Block_Address,Data);
		_SD_Card_Write_Blocks(End_Block_Address,1,Send_Buffer1);
	}
	else if ((Start_Block_Valid_Length!=SD_WRITE_BLOCK_SIZE)&&(End_Block_Valid_Length==0))	//头不对齐，尾对齐
	{
		_SD_Card_Read_Blocks(Start_Block_Address*2,2,Send_Buffer1);
		for (i=0;i<Start_Block_Valid_Length;i++)
			Send_Buffer1[SD_WRITE_BLOCK_SIZE-Start_Block_Valid_Length+i]=((u8*)Data)[i];
		_SD_Card_Write_Blocks(Start_Block_Address,1,Send_Buffer1);
		_SD_Card_Write_Blocks(Start_Block_Address+1,End_Block_Address-Start_Block_Address-1,((u8*)Data)+Start_Block_Valid_Length);
	}
	else	//头尾都不对齐
	{
		if ((Length<SD_WRITE_BLOCK_SIZE)&&(Start_Block_Address==End_Block_Address))	//长度小于块大小，且所有数据位于一个块内
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
	else if ((Start_Block_Valid_Length==SD_READ_BLOCK_SIZE)&&(End_Block_Valid_Length!=0))	//头对齐，尾不对齐
	{
		_SD_Card_Read_Blocks(Start_Block_Address,End_Block_Address-Start_Block_Address,Data);
		_SD_Card_Read_Blocks(End_Block_Address,1,Receive_Buffer);
		for (i=0;i<End_Block_Valid_Length;i++)
			((u8*)Data)[Length-End_Block_Valid_Length+i]=Receive_Buffer[i];
	}
	else if ((Start_Block_Valid_Length!=SD_READ_BLOCK_SIZE)&&(End_Block_Valid_Length==0))	//头不对齐，尾对齐
	{
		_SD_Card_Read_Blocks(Start_Block_Address,1,Receive_Buffer);
		for (i=0;i<Start_Block_Valid_Length;i++)
			((u8*)Data)[i]=Receive_Buffer[SD_READ_BLOCK_SIZE-Start_Block_Valid_Length+i];
		_SD_Card_Read_Blocks(Start_Block_Address+1,End_Block_Address-Start_Block_Address-1,((u8*)Data)+Start_Block_Valid_Length);
	}
	else	//头尾都不对齐
	{
		if ((Length<SD_READ_BLOCK_SIZE)&&(Start_Block_Address==End_Block_Address))	//长度小于块大小，且所有数据位于一个块内
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
	
	if ((Start_Block_Valid_Length==SD_WRITE_BLOCK_SIZE)&&(End_Block_Valid_Length==0))	//头尾块对齐
	{
		while ((DMA2_Stream6->CR&1)!=0);
		DMA2_Stream6->CR&=~(1<<10);//存储器地址不递增
		_SD_Card_Write_Blocks(Start_Block_Address,End_Block_Address-Start_Block_Address,&Data);
	}
	else if ((Start_Block_Valid_Length==SD_WRITE_BLOCK_SIZE)&&(End_Block_Valid_Length!=0))	//头对齐，尾不对齐
	{
		_SD_Card_Read_Blocks(End_Block_Address*2,2,Send_Buffer1);
		for (i=0;i<End_Block_Valid_Length;i++)
			Send_Buffer1[i]=Data;
		_SD_Card_Write_Blocks(End_Block_Address,1,Send_Buffer1);
		while ((DMA2_Stream6->CR&1)!=0);
		DMA2_Stream6->CR&=~(1<<10);//存储器地址不递增
		_SD_Card_Write_Blocks(Start_Block_Address,End_Block_Address-Start_Block_Address,&Data);
	}
	else if ((Start_Block_Valid_Length!=SD_WRITE_BLOCK_SIZE)&&(End_Block_Valid_Length==0))	//头不对齐，尾对齐
	{
		_SD_Card_Read_Blocks(Start_Block_Address*2,2,Send_Buffer1);
		for (i=0;i<Start_Block_Valid_Length;i++)
			Send_Buffer1[SD_WRITE_BLOCK_SIZE-Start_Block_Valid_Length+i]=Data;
		_SD_Card_Write_Blocks(Start_Block_Address,1,Send_Buffer1);
		while ((DMA2_Stream6->CR&1)!=0);
		DMA2_Stream6->CR&=~(1<<10);//存储器地址不递增
		_SD_Card_Write_Blocks(Start_Block_Address+1,End_Block_Address-Start_Block_Address,&Data);
	}
	else	//头尾都不对齐
	{
		if ((Length<SD_WRITE_BLOCK_SIZE)&&(Start_Block_Address==End_Block_Address))	//长度小于块大小，且所有数据位于一个块内
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
		DMA2_Stream6->CR&=~(1<<10);//存储器地址不递增
		_SD_Card_Write_Blocks(Start_Block_Address+1,End_Block_Address-Start_Block_Address-1,&Data);
	}
	
	while ((DMA2_Stream6->CR&1)!=0);
	DMA2_Stream6->CR|=1<<10;//存储器地址递增
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
	USART_Printf(USART,"\n/*******************SD卡测试*******************/\n");
	USART_Printf(USART,"SD卡写测试：\n");
	SD_Card_Write_Data(1024*1024,1024,(u8*)0x20000000);
	SD_Card_Write_Data(1024*1024,1024,(u8*)0x20000000);
	TIM5->CNT=0;
	GeneralTimer_Enable(&Timer5);
	SD_Card_Write_Data(1024*1024,1024,(u8*)0x20000000);
	save=TIM5->CNT;
	GeneralTimer_Disable(&Timer5);
	USART_Printf(USART,"1KB最高写速度：%.1f KB/s\n",1.0f/((float)save/4000000));
	
	TIM5->CNT=0;
	GeneralTimer_Enable(&Timer5);
	SD_Card_Write_Data(1024*1024+2,1024,(u8*)0x20000000);
	save=TIM5->CNT;
	GeneralTimer_Disable(&Timer5);
	USART_Printf(USART,"1KB最低写速度：%.1f KB/s\n",1.0f/((float)save/4000000));
	
	TIM5->CNT=0;
	GeneralTimer_Enable(&Timer5);
	SD_Card_Write_Data(1024*1024,10240,(u8*)0x20000000);
	save=TIM5->CNT;
	GeneralTimer_Disable(&Timer5);
	USART_Printf(USART,"10KB最高写速度：%.1f KB/s\n",10.0f/((float)save/4000000));
	
	TIM5->CNT=0;
	GeneralTimer_Enable(&Timer5);
	SD_Card_Write_Data(1024*1024+2,10240,(u8*)0x20000000);
	save=TIM5->CNT;
	GeneralTimer_Disable(&Timer5);
	USART_Printf(USART,"10KB最低写速度：%.1f KB/s\n",10.0f/((float)save/4000000));
	
	USART_Printf(USART,"SD卡读测试：\n");
	SD_Card_Read_Data(1024*1024,1024,Send_Buffer1);
	SD_Card_Read_Data(1024*1024,1024,Send_Buffer1);
	TIM5->CNT=0;
	GeneralTimer_Enable(&Timer5);
	SD_Card_Read_Data(1024*1024,1024,Send_Buffer1);
	save=TIM5->CNT;
	GeneralTimer_Disable(&Timer5);
	USART_Printf(USART,"1KB最高读速度：%.1f KB/s\n",1.0f/((float)save/4000000));
	
	TIM5->CNT=0;
	GeneralTimer_Enable(&Timer5);
	SD_Card_Read_Data(1024*1024+2,1024,Send_Buffer1);
	save=TIM5->CNT;
	GeneralTimer_Disable(&Timer5);
	USART_Printf(USART,"1KB最低读速度：%.1f KB/s\n",1.0f/((float)save/4000000));
	USART_Printf(USART,"/*******************SD卡测试结束*******************/\n");
	/*TIM5->CNT=0;
	GeneralTimer_Enable(&Timer5);
	SD_Card_Read_Data(1024*1024+2,10240,(u8*)0x20000000);
	save=TIM5->CNT;
	GeneralTimer_Disable(&Timer5);
	USART_Printf(USART,"10KB最低写速度：%.1f\n",10.0f/((float)save/2000000));
	
	TIM5->CNT=0;
	GeneralTimer_Enable(&Timer5);
	SD_Card_Read_Data(1024*1024+2,10240,(u8*)0x20000000);
	save=TIM5->CNT;
	GeneralTimer_Disable(&Timer5);
	USART_Printf(USART,"10KB最低写速度：%.1f\n",10.0f/((float)save/2000000));*/
	
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


