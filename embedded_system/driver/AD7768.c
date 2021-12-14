#include "AD7768.h"
#include "math.h"



void Method_Get_Once_Date(ad7768* dev,uint8_t* dat1,uint32_t num);
void Method_Reset(ad7768* dev);
void 	Method_Data_Binary_to_Float	(uint32_t* dat_before,float* dat_after,uint16_t num,uint16_t channel);
const double b1 = 0.0005762;
const float a1[4] = {14.0440, 0.2400,-0.8474, -4.3723};


void Method_Data_Binary_to_Float	(uint32_t* dat_before,float* dat_after,uint16_t num,uint16_t channel)
{
	uint16_t i;
	int val = 0;
	for(i=0;i<num;i++)
	{
		val = dat_before[i]&(0x00ffffff);
		if(val>0x7fffff) val -= 0x1000000;
		// wide frequency filter
		dat_after[i] = b1*val+a1[channel];
		// sinc5 filter
//		case 0: dat_after[i] = -0.0004745*val+15.6689; break;
//		case 1: dat_after[i] = 0.0004721*val+0.6211; break;
//		case 2: dat_after[i] = 0.0004727*val-4.8381; break;
//		case 3: dat_after[i] = 0.0004729*val+0.3223; break;
	}

}


void Method_Get_Once_Date(ad7768* dev,uint8_t* dat1,uint32_t num)
{
	//HAL_SAI_Receive_DMA(&hsai_BlockA1, dat1, num);
}


void Method_Reset(ad7768* dev)
{
	GPIO_Write(dev->RESET,0);
	delay_us(1);
	GPIO_Write(dev->RESET,1);
}


void AD7768_Prepare(ad7768* dev,gpio_type reset)
{
	dev->Reset = Method_Reset;
	dev->Get_Data = Method_Get_Once_Date;
	dev->Data_Binary_to_Float = Method_Data_Binary_to_Float;


    _RO_WRITE(dev->RESET,gpio_type,reset);

    Method_Reset(dev);

}
