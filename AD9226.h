#ifndef _AD9226_H_
#define _AD9226_H_

#include "sys.h"

//SDA:PC6--C9
//SCL:PC7--D9
//RST:PC8--B8
//CYC_NUM:PC9--E9
//START:PA8--E8
//DRDY:PA11--D8

//�Ƿ�ʹ��FPGAģʽ
//�ڸ�ģʽ�£�AD9226��FPGA֮�����
//��STM32��Ϊһ�����豸��FPGA����ȡ��Ϣ
#define FPGA_MODE	

#define U16_MODE	1	//u16��ʽ���
#define FLOAT_MODE	2	//float��ʽ���

#ifndef FPGA_MODE
/****************************************************************************************
*���ܣ�AD9266��ʼ��																		*
*������1.GPIO���������������GPIO�飬����Pin0~Pin11�������ڽ�������						*
*	   ��Pin12~Pin15������������														*
*	   2.Timer���ö�ʱ�������ڲ���50%ռ�ձȵ�PWM��������ADC								*
*	   3.APBClock��Timer�������ߵ�ʱ��Ƶ��												*
*	   4.OCx��PWM��������˿�															*
*	   5.DMA_Channel��OCx��Ӧ��DMAͨ��													*
*˵����Timer�ļ���Ƶ�ʽ��ᱻ����Ϊ84MHz�������Ͳ���Ƶ��ԼΪ1300Hz������16λ��������	*
*	   ��Լ1Hz������32λ��������														*
*****************************************************************************************/
void AD9226_Init(GPIO_TypeDef* GPIO, TIM_TypeDef* Timer, u32 APBClock, u32 OCx, u16 DMA_Channel);

/************************************************************************
*���ܣ���AD9266�ɼ�����													*
*������1.Freq������Ƶ��													*
*	   2.Num����������													*
*	   3.Data�����ݻ����ַ												*
*	   4.DataMode�����ݸ�ʽ												*
*˵��������u16�����ݸ�ʽDataӦ������Num*2�Ŀռ䣬����float��ʽӦ����	*
*	   ��Num*4�ռ�														*
*************************************************************************/
void AD9226_Get_Data(u32 Freq, u16 Num, void* Data, u8 Data_Mode);
#endif

//���ʹ��FPGAģʽ
#ifdef FPGA_MODE

#define FPGA_Addr ((vu16*)0x64000000)

void AD9226_Init(void);

void AD9226_Reset(u8 ms);

u32 AD9226_Set_Cycle(u32 freq);

void AD9226_Set_DatNum(u16 Num);

void AD9266_Get_Data(void* Data, u8 DataFormat);

#endif


#endif
