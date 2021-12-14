#include "AD736x.h"
#include "delay.h"
#include <stdarg.h>
#include "Memory.h"
#include "USART.h"

/////////////用户配置区///////////////////

#include "DMA.h"

#define AD7367_HEAP     HEAP0

//单通道最高样本数
#define MAX_SAMPLE_NUM  2048

void _DMA_Set(ro periperal* dma, u32* addr, u16 num)
{
    tranplan* dma_plan=dma->Instance;
    
    DMA_Disable(dma_plan);
    DMA_Clear_SR(dma_plan);
    dma_plan->End.Address=addr;
    dma_plan->Controller.DataNumber=num;
    DMA_StreamController_Set(dma_plan);    
}

void _DMA_Enable(ro periperal* dma)
{
    DMA_Enable((tranplan*)dma->Instance);
}

void _DMA_Disable(ro periperal* dma)
{
    DMA_Stop((tranplan*)dma->Instance);
}

/////////////////////////////////////////

u32* AD7367_Buffer=NULL;

u16 channel2_save;

uint32_t SPI_One_Line_Read(ad736x_device_type_type dev_t, gpio_type sclk, gpio_type out_a)
{
	uint16_t i;
	uint32_t dat=0;
	
	if (dev_t==AD7367)
	{
		for (i=0;i<28;i++)
		{
			GPIO_Write(sclk)=0;
			dat|=GPIO_Read(out_a);
			dat<<=1;
			GPIO_Write(sclk)=1;
		}
	}
	else
	{
		for (i=0;i<26;i++)
		{
			GPIO_Write(sclk)=0;
			dat|=GPIO_Read(out_a);
			dat<<=1;
			GPIO_Write(sclk)=1;
		}
	}
	
	return dat>>1;
}

uint32_t SPI_Two_Line_Read(ad736x_device_type_type dev_t, gpio_type sclk, gpio_type out_a, gpio_type out_b)
{
	uint16_t i;
	uint16_t dat_a=0,dat_b=0;
	
	for (i=0;i<dev_t;i++)
	{
        dat_a|=GPIO_Read(out_a);
		dat_b|=GPIO_Read(out_b);
		GPIO_Write(sclk)=0;
		
		GPIO_Write(sclk)=1;
		dat_a<<=1;
		dat_b<<=1;
	}
	
	return (((u32)dat_a)<<(dev_t-1))+(dat_b>>1);
}

void Method_AD736x_Start_Sample(struct ad736x_class* dev, uint32_t fs, uint16_t num, void* datbuf1, void* datbuf2, data_format_type fmt);
void Method_AD736x_Start_Sample2(struct ad736x_class* dev, uint32_t fs, uint16_t num, void* datbuf2, data_format_type fmt);

void Method_AD736x_Start_Sample(struct ad736x_class* dev, uint32_t fs, uint16_t num, void* datbuf1, void* datbuf2, data_format_type fmt)
{
	int i;
	uint32_t save=0;


    //设置系统定时器计时的周期
    SystemTimer_Timing_Ready(1000000.0f/fs);
    if (dev->Is_FPGA_Mode==Enable)
    {
        GPIO_Write(dev->RST)=0;
        SystemTimer_Delay_Us(10);
        GPIO_Write(dev->RST)=1;
        _DMA_Set(&dev->DMA,AD7367_Buffer,num*2);
        _DMA_Enable(&dev->DMA);
        SPI1->CR1|=1<<6;
        SystemTimer_Timing_Begin();
        for (i=0;i<num;i++)
        {
            GPIO_Write(dev->START)=1;
            while (SystemTimer_Timing_Check()==0);
            GPIO_Write(dev->START)=0;
        }
        //while (DMA_Get_TCIF((tranplan*)&dev->DMA.Instance)==0);
        SPI1->CR1&=~(1<<6);
        _DMA_Disable(&dev->DMA);
        for (i=1;i<=10;i++)
                save|=AD7367_Buffer[i*(num-1)/10];
        
        //for (i=0;i<num;i++)
        //    USART_Printf(USART1,"%d %x\n",i,((uint16_t*)datbuf1)[i]);
        
            //USART_Printf(USART1,"%x ",AD7367_Buffer[i]);
            
        if (((save>>16)&0xf)==0)
        {
            //USART_Printf(USART1,"in1 ");
            for (i=0;i<num;i++)
            {
                ((uint16_t*)datbuf1)[i]=(AD7367_Buffer[i]>>2)&0x3fff;
                ((uint16_t*)datbuf2)[i]=((AD7367_Buffer[i]<<12)+(AD7367_Buffer[i]>>20))&0x3fff;
            }
        }
        else if ((save&0xf)==0)
        {
            //USART_Printf(USART1,"in2 ");
            for (i=0;i<num;i++)
            {
                ((uint16_t*)datbuf2)[i]=(AD7367_Buffer[i]>>4)&0x3fff;
                ((uint16_t*)datbuf1)[i]=(AD7367_Buffer[i]>>18)&0x3fff;
            }
         }
    }
    else
    {
        if (dev->Line_Num==ONE_LINE)
        {
            //开启系统定时器的倒计时
            //开启以后每次达到设置的计时时间后，SystemTimer_Timing_Check()会返回1
            //当读到SystemTimer_Timing_Check()==1后，该标志位会自动清零
            //如果在到达计时时间后，一直未读取SystemTimer_Timing_Check()，那么该标志位会
            //一直为1，直到用户去读取
            SystemTimer_Timing_Begin();
            for (i=0;i<num;i++)
            {
                GPIO_Write(dev->CNVST_Pin)=0;
                GPIO_Write(dev->CNVST_Pin)=1;
                GPIO_Write(dev->CS_Pin)=0;
                while (GPIO_Read(dev->BUSY_Pin));
                save=SPI_One_Line_Read(dev->Device_Type,dev->SCLK_Pin,dev->DOUTA_Pin);
                GPIO_Write(dev->CS_Pin)=1;
                ((u16*)datbuf1)[i]=save>>dev->Device_Type;
                ((u16*)datbuf2)[i]=save&0x3fff;
                //读取标志位
                //等待计时时间到达
                while (SystemTimer_Timing_Check()==0);
            }
        }
        else if (dev->Line_Num==TWO_LINE)
        {
            SystemTimer_Timing_Begin();
            for (i=0;i<num;i++)
            {
                GPIO_Write(dev->CNVST_Pin)=0;
                GPIO_Write(dev->CNVST_Pin)=1;
                GPIO_Write(dev->CS_Pin)=0;
                while (GPIO_Read(dev->BUSY_Pin));
                save=SPI_Two_Line_Read(dev->Device_Type,dev->SCLK_Pin,dev->DOUTA_Pin,dev->DOUTB_Pin);
                GPIO_Write(dev->CS_Pin)=1;
                ((uint16_t*)datbuf1)[i]=save>>dev->Device_Type;
                ((uint16_t*)datbuf2)[i]=save&0x3fff;
                while (SystemTimer_Timing_Check()==0);
            }
        }
    }
    //停止计时
    //SystemTimer_Timing_End();	
    
    if (fmt==FLOAT_Format)
    {
        for (i=num-1;i>=0;i--)
        {
            if (((uint16_t*)datbuf1)[i]>=0x2000)
                ((uint16_t*)datbuf1)[i]|=0xc000;
            if (((uint16_t*)datbuf2)[i]>=0x2000)
                ((uint16_t*)datbuf2)[i]|=0xc000;
            ((float*)datbuf1)[i]=(((int16_t*)datbuf1)[i]*0.610351f)/1000.0f;
            ((float*)datbuf2)[i]=(((int16_t*)datbuf2)[i]*0.610351f)/1000.0f;
        }
    }
}

void Method_AD736x_Start_Sample2(struct ad736x_class* dev, uint32_t fs, uint16_t num, void* datbuf, data_format_type fmt)
{
	int i;
	uint32_t save=0;

   /* GPIO_Write(dev->RST)=0;
    SystemTimer_Delay_Us(3);
    GPIO_Write(dev->RST)=1;*/
    //设置系统定时器计时的周期
    SystemTimer_Timing_Ready(1000000.0f/fs);
    if (dev->Is_FPGA_Mode==Enable)
    {
        
        _DMA_Set(&dev->DMA,AD7367_Buffer,num*2);
        _DMA_Enable(&dev->DMA);
        SPI1->CR1|=1<<6;
        SystemTimer_Timing_Begin();
        for (i=0;i<num;i++)
        {
            GPIO_Write(dev->START)=1;
            while (SystemTimer_Timing_Check()==0);
            GPIO_Write(dev->START)=0;
        }
        SPI1->CR1&=~(1<<6);
        _DMA_Disable(&dev->DMA);
        for (i=1;i<=10;i++)
                save|=AD7367_Buffer[i*(num-1)/10];
            
        if (((save>>16)&0xf)==0)
        {
            //USART_Printf(USART1,"in1 ");
            for (i=0;i<num;i++)
            {
                ((uint16_t*)datbuf)[i<<1]=((AD7367_Buffer[i]<<12)+(AD7367_Buffer[i]>>20))&0x3fff;
                ((uint16_t*)datbuf)[(i<<1)+1]=(AD7367_Buffer[i]>>2)&0x3fff;
            }
        }
        else if ((save&0xf)==0)
        {
            //USART_Printf(USART1,"in2 ");
            for (i=0;i<num;i++)
            {
                ((uint16_t*)datbuf)[i<<1]=(AD7367_Buffer[i]>>4)&0x3fff;
                ((uint16_t*)datbuf)[(i<<1)+1]=(AD7367_Buffer[i]>>18)&0x3fff;
            }
         }
    }
    else
    {
        if (dev->Line_Num==ONE_LINE)
        {
            //开启系统定时器的倒计时
            //开启以后每次达到设置的计时时间后，SystemTimer_Timing_Check()会返回1
            //当读到SystemTimer_Timing_Check()==1后，该标志位会自动清零
            //如果在到达计时时间后，一直未读取SystemTimer_Timing_Check()，那么该标志位会
            //一直为1，直到用户去读取
            SystemTimer_Timing_Begin();
            for (i=0;i<num;i++)
            {
                GPIO_Write(dev->CNVST_Pin)=0;
                GPIO_Write(dev->CNVST_Pin)=1;
                GPIO_Write(dev->CS_Pin)=0;
                while (GPIO_Read(dev->BUSY_Pin));
                save=SPI_One_Line_Read(dev->Device_Type,dev->SCLK_Pin,dev->DOUTA_Pin);
                GPIO_Write(dev->CS_Pin)=1;
                ((u16*)datbuf)[i<<1]=save>>dev->Device_Type;
                ((u16*)datbuf)[(i<<1)+1]=save&0x3fff;
                //((u16*)datbuf1)[i]=save>>dev->Device_Type;
                //((u16*)datbuf2)[i]=save&0x3fff;
                //读取标志位
                //等待计时时间到达
                while (SystemTimer_Timing_Check()==0);
            }
        }
        else if (dev->Line_Num==TWO_LINE)
        {
            SystemTimer_Timing_Begin();
            for (i=0;i<num;i++)
            {
                GPIO_Write(dev->CNVST_Pin)=0;
                GPIO_Write(dev->CNVST_Pin)=1;
                GPIO_Write(dev->CS_Pin)=0;
                while (GPIO_Read(dev->BUSY_Pin));
                save=SPI_Two_Line_Read(dev->Device_Type,dev->SCLK_Pin,dev->DOUTA_Pin,dev->DOUTB_Pin);
                GPIO_Write(dev->CS_Pin)=1;
                ((u16*)datbuf)[i<<1]=save>>dev->Device_Type;
                ((u16*)datbuf)[(i<<1)+1]=save&0x3fff;
                //((uint16_t*)datbuf1)[i]=save>>dev->Device_Type;
                //((uint16_t*)datbuf2)[i]=save&0x3fff;
                while (SystemTimer_Timing_Check()==0);
            }
        }
    }
    //停止计时
    //SystemTimer_Timing_End();	
    
    if (fmt==FLOAT_Format)
    {
        for (i=num*2-1;i>=0;i--)
        {
            if (((uint16_t*)datbuf)[i]>=0x2000)
                ((uint16_t*)datbuf)[i]|=0xc000;
            ((float*)datbuf)[i]=(((int16_t*)datbuf)[i]*0.610351f)/1000.0f;
        }
    }
}

void AD736x_Prepare(struct ad736x_class* dev, ad736x_device_type_type dev_t, ad736x_line_num_type ln_num,
					gpio_type cnvst, gpio_type bsy, gpio_type cs, gpio_type sclk, ...)
{
	va_list vl;
	
	va_start(vl,sclk);
	
	_RO_WRITE(dev->Device_Type,ad736x_device_type_type,dev_t);
    _RO_WRITE(dev->Is_FPGA_Mode,status_flag,Disable);
	_RO_WRITE(dev->Line_Num,ad736x_line_num_type,ln_num);
	_RO_WRITE(dev->CNVST_Pin,gpio_type,cnvst);
	_RO_WRITE(dev->BUSY_Pin,gpio_type,bsy);
	_RO_WRITE(dev->CS_Pin,gpio_type,cs);
	_RO_WRITE(dev->SCLK_Pin,gpio_type,sclk);
    dev->AD736x_Start_Sample=Method_AD736x_Start_Sample;
    dev->AD736x_Start_Sample2=Method_AD736x_Start_Sample2;
	
	if (ln_num==ONE_LINE)
		_RO_WRITE(dev->DOUTA_Pin,gpio_type,va_arg(vl,gpio_type));
	else if (ln_num==TWO_LINE)
	{
		_RO_WRITE(dev->DOUTA_Pin,gpio_type,va_arg(vl,gpio_type));
		_RO_WRITE(dev->DOUTB_Pin,gpio_type,va_arg(vl,gpio_type));
	}
	va_end(vl);

	GPIO_Write(dev->CNVST_Pin)=1;
	GPIO_Write(dev->CS_Pin)=1;
    GPIO_Write(dev->RST)=0;
    SystemTimer_Delay_Ms(2);
    GPIO_Write(dev->RST)=1;
}

void AD736x_Prepare2(struct ad736x_class* dev, uint32_t value, void* reg0, void* reg1,
                     inst_type inst, gpio_type start, gpio_type rst)
{
    if (AD7367_Buffer==NULL)
        AD7367_Buffer=Malloc(AD7367_HEAP,MAX_SAMPLE_NUM*2*sizeof(uint16_t));
    
    _RO_WRITE(dev->Device_Type,ad736x_device_type_type,AD7367);
    _RO_WRITE(dev->Is_FPGA_Mode,status_flag,Enable);
    _RO_WRITE(dev->DMA.Value,uint32_t,value);
    _RO_WRITE(dev->DMA.Register0,void*,reg0);
    _RO_WRITE(dev->DMA.Register1,void*,reg1);
    _RO_WRITE(dev->DMA.Instance,inst_type,inst);
    _RO_WRITE(dev->START,gpio_type,start);
    _RO_WRITE(dev->RST,gpio_type,rst);
    dev->AD736x_Start_Sample=Method_AD736x_Start_Sample;
    dev->AD736x_Start_Sample2=Method_AD736x_Start_Sample2;
    
    GPIO_Write(dev->START)=0;
    GPIO_Write(dev->RST)=0;
    SystemTimer_Delay_Ms(2);
    GPIO_Write(dev->RST)=1;
}


