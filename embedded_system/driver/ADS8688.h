#ifndef _ADS8688_H_
#define _ADS8688_H_

#include "sys.h"

/*Ä£¿éÒý½ÅÎ»ÖÃ
SDI		RST
CLK		CS
SDO		GND
DAISY	GND
GND		V+
*/

#define ADS8688_U16		1
#define ADS8688_FLOAT	2

#define ADS8688_DUAL	1
#define ADS8688_SINGLE	2

#define D10_24	1
#define D5_12	2
#define D2_56	3
#define S10_24	4
#define S5_12	5

typedef struct ADS8688_Plan_Class
{
	u8 CHx;
	u8 Range;
	u8 Data_Format;
	u16 Data_Num;
	u32 Samp_Freq;
	void* Data;
} ads8688_plan;

void ADS8688_Init(void);
void ADS8688_Timer_Init(TIM_TypeDef* TIMx, u32 APBx_Clock, u32 Timing_Freq);
void ADS8688_Send_Command(u16 CMD);
void ADS8688_Write_Register(u8 Addr, u8 Data);
u8 ADS8688_Read_Register(u8 Addr);
void ADS8688_Range_Set(u8 CHx, u8 Range);
void ADS8688_Get_Data(ads8688_plan* plan);
void ADS8688_Get_Data_Inst(u8 CHx, u8 DataFormat, void* Data);


#endif
