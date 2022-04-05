#include "sys.h"
#include "PE4302.h"
#include <math.h>

#define ROUND(x)	((uint32_t)((x)+0.5f))

void PE4302_Write_Latch(pe4302* dev, uint8_t dat)
{
	signed char i;
	
    GPIO_Write(dev->LE,0);
	for (i=5;i>=0;i--)
	{
        GPIO_Write(dev->DATA,(dat>>i)&1);
		SystemTimer_Delay_Us(2);
        GPIO_Write(dev->CLK,1);
		SystemTimer_Delay_Us(2);
        GPIO_Write(dev->CLK,0);
		SystemTimer_Delay_Us(2);
	}
    GPIO_Write(dev->LE,1);
	SystemTimer_Delay_Us(2);
    GPIO_Write(dev->LE,0);
}

void Method_Set_Attenuation(struct pe4302_class* device ,float atn);
void Method_Set_Attenuation2(struct pe4302_class* device ,float atn);

void Method_Set_Attenuation(struct pe4302_class* device ,float atn)
{
	uint8_t save=ROUND(atn*2);
	
	//范围判断
	if (atn<0)
		atn=0;
	else if (atn>31.5f)
		atn=31.5f;
	save=ROUND(atn*2);
    PE4302_Write_Latch(device,save);
	_RO_WRITE(device->Actual_Attenuation,float,(float)save);
}

void Method_Set_Attenuation2(struct pe4302_class* device ,float atn)
{
	Method_Set_Attenuation(device,20*log10(atn));
}

void PE4302_Prepare(pe4302* dev, gpio_type data, gpio_type sclk, gpio_type le)
{
    _RO_WRITE(dev->DATA,gpio_type,data);
    _RO_WRITE(dev->CLK,gpio_type,sclk);
    _RO_WRITE(dev->LE,gpio_type,le);
    
    dev->Set_Attenuation=Method_Set_Attenuation;
    dev->Set_Attenuation2=Method_Set_Attenuation2;
}


	
