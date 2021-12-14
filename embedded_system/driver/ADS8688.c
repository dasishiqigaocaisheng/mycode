#include "ADS8688.h"
#include "F4GPIO.h"
#include "Timer.h"
#include "delay.h"

/*模块引脚位置
SDI		RST
CLK		CS
SDO		GND
DAISY	GND
GND		V+
*/
////////用户配置区//////////
#define USED_SPI	SPI2
#define AF_NUM		AF_SPI1_2
#define MOSI		GPIOB_15
#define MISO		GPIOB_14
#define SCK			GPIOB_13

#define CS_PIN		GPIOE_2	
////////////////////////////

#define CS			GPIO_Write(CS_PIN)

#define NO_OP		0x0000
#define STDBY		0x8200
#define PWR_DN		0x8300
#define RST			0x8500
#define AUTO_RST	0xa000
#define MAN_CH_0	0xc000
#define MAN_CH_1	0xc400
#define MAN_CH_2	0xc800
#define MAN_CH_3	0xcc00
#define MAN_CH_4	0xd000
#define MAN_CH_5	0xd400
#define MAN_CH_6	0xd800
#define MAN_CH_7	0xdc00
#define MAN_AUX		0xe000

#define AUTO_SEQ_EN_ADDR	0x01
#define CHNL_PWR_DN_ADDR	0x02
#define FTURE_SEL_ADDR		0x03
#define CH0_RANGE_ADDR		0x05
#define CH1_RANGE_ADDR		0x06
#define CH2_RANGE_ADDR		0x07
#define CH3_RANGE_ADDR		0x08
#define CH4_RANGE_ADDR		0x09
#define CH5_RANGE_ADDR		0x0a
#define CH6_RANGE_ADDR		0x0b
#define CH7_RANGE_ADDR		0x0c
#define CMD_RD_BAK_ADDR		0x3f

#define	_SPI_Send_Data(x)	USED_SPI->DR=(x);\
							while (((USED_SPI->SR>>1)&1)==0)

#define _SPI_Read_Data(x,y)	_SPI_Send_Data(x);\
							while ((USED_SPI->SR&1)==0);\
							*(y)=USED_SPI->DR						
			
#define _Get_Sample(x)		CS=0;\
							_SPI_Send_Data(NO_OP);\
							_SPI_Read_Data(NO_OP,x);\
							while (((USED_SPI->SR>>7)&1)==1);\
							CS=1
							
Timer_Info timer;

void _U16_To_Float(u16* dat, u16 Num, u8 Range)
{
	u16 i,d;
	float r;
	
	if (Range==D10_24)
	{
		d=32767;
		r=20.48f;
	}
	else if (Range==D5_12)
	{
		d=32767;
		//r=10.24f;
		r=20.48f;
	}
	else if (Range==D2_56)
	{
		d=32767;
		//r=5.12f;
		r=20.48f;
	}
	else if (Range==S10_24)
	{
		d=0;
		//r=10.24f;
		r=5.12f;
	}
	else if (Range==S5_12)
	{
		d=0;
		r=5.12f;
	}
	for (i=0;i<Num;i++)
		((float*)dat)[Num-1-i]=(dat[Num-1-i]-d)*r/65535;
}
							
void ADS8688_Init(void)
{
	GPIO_Init(CS_PIN,GPIO_MODE_OUT,GPIO_OTYPE_PP,
				GPIO_SPEED_50M,GPIO_PUPD_PU);
	CS=1;
	
	GPIO_Init(MOSI,GPIO_MODE_AF,GPIO_OTYPE_PP,
				GPIO_SPEED_50M,GPIO_PUPD_PU);
	GPIO_Init(MISO,GPIO_MODE_AF,GPIO_OTYPE_PP,
				GPIO_SPEED_50M,GPIO_PUPD_NONE);
	GPIO_Init(SCK,GPIO_MODE_AF,GPIO_OTYPE_PP,
				GPIO_SPEED_50M,GPIO_PUPD_PU);
	
	GPIO_AF_Init(MOSI,AF_NUM);
	GPIO_AF_Init(MISO,AF_NUM);
	GPIO_AF_Init(SCK,AF_NUM);
	
	if (USED_SPI==SPI1)
		RCC->APB2ENR|=1<<12;	//SPI1
	else if (USED_SPI==SPI2)
		RCC->APB1ENR|=1<<14;	//SPI2
	else if (USED_SPI==SPI3)
		RCC->APB1ENR|=1<<15;
	#ifdef STM32F429xx
	else if (USED_SPI==SPI4)
		RCC->APB2ENR|=1<<13;
	else if (USED_SPI==SPI5)
		RCC->APB2ENR|=1<<20;
	else if (USED_SPI==SPI6)
		RCC->APB2ENR|=1<<21;
	#endif
	
	//CPOL=1,CPHA=0
	u16 save=0;
	save|=1<<9;	//软件从器件管理
	save|=1<<8;	//SSI=1
	save|=1<<11;//16bit数据帧
	save|=0<<7;	//先发送MSB
	save|=2<<3;	//波特率fpclk/16
	save|=1<<2;	//主模式
	save|=1<<1;	//空闲时高电平
	save|=0;	//第一个时钟边沿采集数据
	save|=1<<6;	//SPI使能
	USED_SPI->CR1=save;
	
	delay_ms(10);
	ADS8688_Send_Command(RST);
}

void ADS8688_Timer_Init(TIM_TypeDef* TIMx, u32 APBx_Clock, u32 Timing_Freq)
{
	Timer_Reset(TIMx,&timer);
	Timer_Global_Set(&timer,INSIDE_CLOCK_SOURCE,APBx_Clock,Timing_Freq,TRIGGER_SOFTWARE);
	Timer_Timing_Init(&timer,0);
	Timer_Clear_UIF(&timer);
}

void ADS8688_Send_Command(u16 CMD)
{
	CS=0;
	_SPI_Send_Data(CMD);
	while (((USED_SPI->SR>>7)&1)==1);
	CS=1;
}

void ADS8688_Write_Register(u8 Addr, u8 Data)
{
	CS=0;
	_SPI_Send_Data((Addr<<9)+(1<<8)+Data);
	_SPI_Send_Data(NO_OP);
	while (((USED_SPI->SR>>7)&1)==1);
	CS=1;
}

u8 ADS8688_Read_Register(u8 Addr)
{
	u16 dat;
	
	CS=0;
	_SPI_Send_Data(Addr<<9);
	_SPI_Read_Data(NO_OP,&dat);
	while (((USED_SPI->SR>>7)&1)==1);
	CS=1;
	return dat>>8;
}

void ADS8688_Range_Set(u8 CHx, u8 Range)
{
	ADS8688_Write_Register(CH0_RANGE_ADDR,Range-1);	
}

void ADS8688_Get_Data(ads8688_plan* plan)
{
	int i;
	
	Timer_Timing_SetTime_us(&timer,1000000.0f/plan->Samp_Freq);
	ADS8688_Range_Set(plan->CHx,plan->Range);
	ADS8688_Send_Command(MAN_CH_0+plan->CHx*0x400);
	for (i=0;i<plan->Data_Num;i++)
	{
		Timer_Clear_UIF(&timer);
		Timer_Enable(&timer);
		_Get_Sample(&((u16*)plan->Data)[i]);
		while (Timer_Get_UIF(&timer)==0);	
	}
	Timer_Disable(&timer);
	if (plan->Data_Format==ADS8688_FLOAT)
		_U16_To_Float(plan->Data,plan->Data_Num,plan->Range);
}

void ADS8688_Get_Data_Inst(u8 CHx, u8 DataFormat, void* Data)
{
	//if (DataFormat==ADS8688_FLOAT)
	//	*((float*)Data)=_Get_Sample(
	
}

