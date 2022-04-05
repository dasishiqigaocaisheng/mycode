#include "AD9226.h"
#include "Timer.h"
#include "DMA.h"
#include "delay.h"
#include "USART.h"
#include "F4GPIO.h"

//真实电压值=测量值(0~0xfff)*k+b
#define k	(0.002442f)
#define b	(-5.0f)

#ifndef FPGA_MODE
u32 OC;
Timer_Info timer;
tranplan dmaplan;

void AD9226_Init(GPIO_TypeDef* GPIO, TIM_TypeDef* Timer, u32 APBClock, u32 OCx, u16 DMA_Channel)
{
	//配置定时器
	Timer_Reset(Timer,&timer);
	Timer_Global_Set(&timer,INSIDE_CLOCK_SOURCE,APBClock,84000000,TRIGGER_SOFTWARE);
	TIM1->DIER|=1<<9;
	Timer_PWMOutput_Init(&timer,2000);
	TIM1->CCER|=1<<2;
	TIM1->CCER|=1<<3;
	Timer_PWMOutput_Wave_Set(&timer,0.5f,OCx,DO_UPDATE);
	OC=OCx;
	
	//GPIO初始化
	GPIO_Set(GPIO,PIN0|PIN1|PIN2|PIN3|PIN4|PIN5|PIN6|PIN7|PIN8|PIN9|PIN10|PIN11,
			GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);
	
	//DMA初始化
	DMA_Build_TransferPlan(&dmaplan,DMA_Channel);
	dmaplan.Start.Address=(void*)&GPIO->IDR;
	dmaplan.Start.IsFixed=1;
	dmaplan.Start.Type=TRANPORT_TYPE_PER;
	
	dmaplan.End.IsFixed=0;
	dmaplan.End.Type=TRANPORT_TYPE_MEM;
	
	dmaplan.Controller.DualBuffer=0;
	dmaplan.Controller.Loop=0;
	dmaplan.Controller.WordLength=2;
	dmaplan.Controller.DataNumber=126;
	DMA_StreamController_Set(&dmaplan);
	DMA_Clear_SR(&dmaplan);	
	Timer_PWMOutput_Enable(&timer,OC);
	Timer_Enable(&timer);
}


void AD9226_Get_Data(u32 Freq, u16 Num, void* Data, u8 Data_Mode)
{
	u16 i;
	//重新设置频率
	Timer_PWMOutput_Frequency_Set(&timer,Freq,NO_UPDATE);
	dmaplan.End.Address=Data;
	dmaplan.Controller.DataNumber=Num;
	DMA_StreamController_Set(&dmaplan);
	DMA_Clear_SR(&dmaplan);
	delay_us(7000000/Freq);
	DMA_Enable(&dmaplan);
	while (DMA_Get_TCIF(&dmaplan)==0);
	DMA_Disable(&dmaplan);
	DMA_Clear_SR(&dmaplan);
	for (i=0;i<Num;i++)
		((u16*)Data)[i]&=0xfff;
	if (Data_Mode==FLOAT_MODE)
	{
		for (i=0;i<Num;i++)
		{
			((float*)Data)[Num-1-i]=((u16*)Data)[Num-1-i]*k+b;
		}
	}
}
#else

/*
FSMC引脚连接：
PD14---D0		PF0----A0
PD15---D1		PF1----A1
PD0----D2		PF2----A2
PD1----D3		PF3----A3
PE7----D4		PF4----A4
PE8----D5		PF5----A5
PE9----D6		PF12---A6
PE10---D7		PF13---A7
PE11---D8		PF14---A8
PE12---D9		PF15---A9
PE13---D10		PG0----A10
PE14---D11		PG1----A11
PE15---D12		PG2----A12
PD8----D13		PG3----A13
PD9----D14		PG4----A14
PD10---D15		PG5----A15

PD4----NOE
PG9----NE3
*/

///////////////用户配置区////////////////
#define FPGA_CLK	120000000

#define SDA_PIN		GPIOE_0
#define SCL_PIN		GPIOE_1
#define CYC_NUM_PIN	GPIOE_2
#define START_PIN	GPIOE_3
#define DRDY_PIN	GPIOE_4
#define RST_PIN		GPIOE_5
////////////////////////////////////////

#define SDA		GPIO_Write(SDA_PIN)
#define SCL		GPIO_Write(SCL_PIN)
#define CYC_NUM	GPIO_Write(CYC_NUM_PIN)
#define START	GPIO_Write(START_PIN)
#define DRDY	GPIO_Read(DRDY_PIN)
#define	RST		GPIO_Write(RST_PIN)


#define COM_RESET()	SDA=0; \
					SCL=1; \
					delay_us(1); \
					SDA=1; \
					delay_us(1); \
					SDA=0

u16 Data_Num=0;

void _SCL_Delay(void)
{
	u16 i=2000;
	while (i)
		i--;
}

void AD9226_Init(void)
{
	GPIO_Init(SDA_PIN,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_25M,GPIO_PUPD_PU);
	GPIO_Init(SCL_PIN,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_25M,GPIO_PUPD_PU);
	GPIO_Init(CYC_NUM_PIN,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_25M,GPIO_PUPD_PU);
	GPIO_Init(START_PIN,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_25M,GPIO_PUPD_PU);
	GPIO_Init(RST_PIN,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_25M,GPIO_PUPD_PU);
	GPIO_Init(DRDY_PIN,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_25M,GPIO_PUPD_NONE);	
	
	SCL=0;
	SDA=0;
	CYC_NUM=1;
	START=0;
	RST=0;
	
	//FSMC时钟使能
	RCC->AHB3ENR|=1;
	
	RCC->AHB1ENR|=1<<3;	//GPIOD
	RCC->AHB1ENR|=1<<4;	//GPIOE
	RCC->AHB1ENR|=1<<5;	//GPIOF
	RCC->AHB1ENR|=1<<6;	//GPIOG

	/*D2,3,13,14,15,0,1*/
	GPIO_Set(GPIOD,PIN0|PIN1|PIN8|PIN9|PIN10|PIN14|PIN15,GPIO_MODE_AF,
				GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);
	
	/*D4,5,6,7,8,9,10,11,12*/
	GPIO_Set(GPIOE,PIN7|PIN8|PIN9|PIN10|PIN11|PIN12|PIN13|PIN14|PIN15,
				GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);
		
	/*A0,1,2,3,4,5,6,7,8,9*/
	GPIO_Set(GPIOF,PIN0|PIN1|PIN2|PIN3|PIN4|PIN5|PIN12|PIN13|PIN14|PIN15,
				GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);

	/*A10,11,12,13,14,15,NE2*/
	GPIO_Set(GPIOG,PIN0|PIN1|PIN2|PIN3|PIN4|PIN5|PIN9,GPIO_MODE_AF,GPIO_OTYPE_PP,
				GPIO_SPEED_100M,GPIO_PUPD_PU);

	//NOE			
	GPIO_Set(GPIOD,PIN4,GPIO_MODE_AF,GPIO_OTYPE_PP,
				GPIO_SPEED_100M,GPIO_PUPD_PU);		
					
	GPIO_AF_Set(GPIOD,0,AF_FSMC);
	GPIO_AF_Set(GPIOD,1,AF_FSMC);
	GPIO_AF_Set(GPIOD,4,AF_FSMC);
	GPIO_AF_Set(GPIOD,8,AF_FSMC);
	GPIO_AF_Set(GPIOD,9,AF_FSMC);
	GPIO_AF_Set(GPIOD,10,AF_FSMC);
	GPIO_AF_Set(GPIOD,14,AF_FSMC);
	GPIO_AF_Set(GPIOD,15,AF_FSMC);
	
	GPIO_AF_Set(GPIOE,7,AF_FSMC);
	GPIO_AF_Set(GPIOE,8,AF_FSMC);
	GPIO_AF_Set(GPIOE,9,AF_FSMC);
	GPIO_AF_Set(GPIOE,10,AF_FSMC);
	GPIO_AF_Set(GPIOE,11,AF_FSMC);
	GPIO_AF_Set(GPIOE,12,AF_FSMC);
	GPIO_AF_Set(GPIOE,13,AF_FSMC);
	GPIO_AF_Set(GPIOE,14,AF_FSMC);
	GPIO_AF_Set(GPIOE,15,AF_FSMC);
	
	GPIO_AF_Set(GPIOF,0,AF_FSMC);
	GPIO_AF_Set(GPIOF,1,AF_FSMC);
	GPIO_AF_Set(GPIOF,2,AF_FSMC);
	GPIO_AF_Set(GPIOF,3,AF_FSMC);
	GPIO_AF_Set(GPIOF,4,AF_FSMC);
	GPIO_AF_Set(GPIOF,5,AF_FSMC);
	GPIO_AF_Set(GPIOF,12,AF_FSMC);
	GPIO_AF_Set(GPIOF,13,AF_FSMC);
	GPIO_AF_Set(GPIOF,14,AF_FSMC);
	GPIO_AF_Set(GPIOF,15,AF_FSMC);
	
	GPIO_AF_Set(GPIOG,0,AF_FSMC);
	GPIO_AF_Set(GPIOG,1,AF_FSMC);
	GPIO_AF_Set(GPIOG,2,AF_FSMC);
	GPIO_AF_Set(GPIOG,3,AF_FSMC);
	GPIO_AF_Set(GPIOG,4,AF_FSMC);
	GPIO_AF_Set(GPIOG,5,AF_FSMC);
	GPIO_AF_Set(GPIOG,9,AF_FSMC);
	
	FSMC_Bank1->BTCR[2]=0x00000000;
	FSMC_Bank1->BTCR[3]=0x00000000;
	
	FSMC_Bank1->BTCR[2]|=1<<12;		//允许写入
	FSMC_Bank1->BTCR[2]|=1<<4;		//16位宽度
	FSMC_Bank1->BTCR[2]|=1;			//存储区域使能
	
	FSMC_Bank1->BTCR[3]|=32<<8;		//DATAST=16HCLK
	FSMC_Bank1->BTCR[3]|=15;			//ADDSET=1HCLK
	
	RST=1;
}

void AD9226_Reset(u8 ms)
{
	RST=0;
	delay_ms(ms);
	RST=1;
}

u32 AD9226_Set_Cycle(u32 freq)
{
	u8 i;
	u16 dat;
	
	dat=FPGA_CLK/freq-1;
	while (DRDY==0);
	CYC_NUM=1;
	COM_RESET();
	for (i=0;i<16;i++)
	{
		SCL=0;
		SDA=(dat>>i)&1;
		_SCL_Delay();
		SCL=1;
		_SCL_Delay();
	}
	delay_us(20);
	return FPGA_CLK/(dat+1);
}


void AD9226_Set_DatNum(u16 Num)
{
	u8 i;
	
	CYC_NUM=0;
	while (DRDY==0);
	COM_RESET();
	for (i=0;i<16;i++)
	{
		SCL=0;
		SDA=(Num>>i)&1;
		_SCL_Delay();
		SCL=1;
		_SCL_Delay();
	}
	Data_Num=Num;
	delay_us(20);
	
}

void AD9266_Get_Data(void* Data, u8 DataFormat)
{
	u16 i;
	
	START=1;
	delay_us(1);
	START=0;
	while (DRDY!=1);
	for (i=0;i<Data_Num;i++)
		((u16*)Data)[i]=FPGA_Addr[i];
	if (DataFormat==FLOAT_MODE)
	{
		for (i=0;i<Data_Num;i++)
		{
			((float*)Data)[Data_Num-1-i]=((u16*)Data)[Data_Num-1-i]*k+b;
		}
	}
	delay_us(20);
}

#endif






