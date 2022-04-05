#ifndef _AD9226_H_
#define _AD9226_H_

#include "sys.h"

//SDA:PC6--C9
//SCL:PC7--D9
//RST:PC8--B8
//CYC_NUM:PC9--E9
//START:PA8--E8
//DRDY:PA11--D8

//是否使用FPGA模式
//在该模式下，AD9226被FPGA之间操作
//而STM32作为一个主设备从FPGA处读取信息
#define FPGA_MODE	

#define U16_MODE	1	//u16格式输出
#define FLOAT_MODE	2	//float格式输出

#ifndef FPGA_MODE
/****************************************************************************************
*功能：AD9266初始化																		*
*参数：1.GPIO：用于数据输入的GPIO组，其中Pin0~Pin11将会用于接收数据						*
*	   而Pin12~Pin15可以另作他用														*
*	   2.Timer：该定时器将用于产生50%占空比的PWM波来驱动ADC								*
*	   3.APBClock：Timer所在总线的时钟频率												*
*	   4.OCx：PWM波的输出端口															*
*	   5.DMA_Channel：OCx对应的DMA通道													*
*说明：Timer的计数频率将会被配置为84MHz，因此最低采样频率约为1300Hz（对于16位计数器）	*
*	   或约1Hz（对于32位计数器）														*
*****************************************************************************************/
void AD9226_Init(GPIO_TypeDef* GPIO, TIM_TypeDef* Timer, u32 APBClock, u32 OCx, u16 DMA_Channel);

/************************************************************************
*功能：从AD9266采集数据													*
*参数：1.Freq：采样频率													*
*	   2.Num：采样点数													*
*	   3.Data：数据缓存地址												*
*	   4.DataMode：数据格式												*
*说明：对于u16的数据格式Data应至少有Num*2的空间，对于float格式应至少	*
*	   有Num*4空间														*
*************************************************************************/
void AD9226_Get_Data(u32 Freq, u16 Num, void* Data, u8 Data_Mode);
#endif

//如果使用FPGA模式
#ifdef FPGA_MODE

#define FPGA_Addr ((vu16*)0x64000000)

void AD9226_Init(void);

void AD9226_Reset(u8 ms);

u32 AD9226_Set_Cycle(u32 freq);

void AD9226_Set_DatNum(u16 Num);

void AD9266_Get_Data(void* Data, u8 DataFormat);

#endif


#endif
