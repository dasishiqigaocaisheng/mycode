#include "sys.h"
#include "AD9834.h"


#define AD9834_SCLK GPIOB_7
#define AD9834_DATA GPIOB_8
#define AD9834_SYNC GPIOB_9

volatile u16 CONTROL_DATA=0;		//该值实时存储了当前控制寄存器的状态

void AD9834_Write_Register(u16 Data)
{
	u8 i;
	
	GPIO_Write(AD9834_SCLK,1);
	GPIO_Write(AD9834_SYNC,0);
	
	for (i=0;i<16;i++)
	{
		GPIO_Write(AD9834_SCLK,1);
		if (Data&0x8000)
			GPIO_Write(AD9834_DATA,1);
		else
			GPIO_Write(AD9834_DATA,0);
		GPIO_Write(AD9834_SCLK,0);
		SystemTimer_Delay_Us(5);
		Data<<=1;
	}
	GPIO_Write(AD9834_SYNC,1);
}

void AD9834_Write_ControlRegister(u8 Cmd, u16 ControlData)
{
	if (Cmd)
		CONTROL_DATA|=ControlData;
	else
		CONTROL_DATA&=~(ControlData);
	AD9834_Write_Register(CONTROL_DATA);
}
	
void AD9834_Init(void)
{
	GPIO_Write(AD9834_SYNC,1);
	AD9834_Write_ControlRegister(ON,RESET);
	SystemTimer_Delay_Ms(1);
	AD9834_Write_ControlRegister(OFF,RESET);
}

void AD9834_Write_28BitsFrequencyRegister(u8 Addr, u32 Frequency)
{
	u16 i,MSB,LSB;
	u32 a;
	
	LSB=0;
	MSB=0;
	a=(u32)(3.5791394133f*Frequency);
	
	LSB=(a&0x3fff);
	a>>=14;
	MSB=a;
	if (Addr)
	{
		LSB+=0x8000;
		MSB+=0x8000;
	}
	else
	{
		LSB+=0x4000;
		MSB+=0x4000; 
	}
	AD9834_Write_Register(LSB);
	AD9834_Write_Register(MSB);
}

void AD9834_Write_PhaseRegister(u8 Addr, float Phase)
{
	u16 a,MSB;
	u8 i;
	
	Phase*=651.89864f;
	a=(u16)Phase;
	for (i=0,MSB=0;i<12;i++)
	{
		MSB+=(a&1);
		a>>=1;
		MSB<<=1;
	}
	if (Addr)
		MSB|=1<<13;
	MSB|=3<<14;
	AD9834_Write_Register(MSB);
}
