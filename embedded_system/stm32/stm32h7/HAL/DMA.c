#include "DMA.h"
#include "sys.h"
#include "USART.h"

DMA_Interrupt_CallbackFunc DMA1_Stream0_CallbackFunc;
DMA_Interrupt_CallbackFunc DMA1_Stream1_CallbackFunc;
DMA_Interrupt_CallbackFunc DMA1_Stream2_CallbackFunc;
DMA_Interrupt_CallbackFunc DMA1_Stream3_CallbackFunc;
DMA_Interrupt_CallbackFunc DMA1_Stream4_CallbackFunc;
DMA_Interrupt_CallbackFunc DMA1_Stream5_CallbackFunc;
DMA_Interrupt_CallbackFunc DMA1_Stream6_CallbackFunc;
DMA_Interrupt_CallbackFunc DMA1_Stream7_CallbackFunc;

DMA_Interrupt_CallbackFunc DMA2_Stream0_CallbackFunc;
DMA_Interrupt_CallbackFunc DMA2_Stream1_CallbackFunc;
DMA_Interrupt_CallbackFunc DMA2_Stream2_CallbackFunc;
DMA_Interrupt_CallbackFunc DMA2_Stream3_CallbackFunc;
DMA_Interrupt_CallbackFunc DMA2_Stream4_CallbackFunc;
DMA_Interrupt_CallbackFunc DMA2_Stream5_CallbackFunc;
DMA_Interrupt_CallbackFunc DMA2_Stream6_CallbackFunc;
DMA_Interrupt_CallbackFunc DMA2_Stream7_CallbackFunc;

void DMA_Build_TransferPlan(tranplan* tp, dmamux1_req_type req, dma_channel_type ch)
{
    RCC->D3AMR|=1<<0;
    if (ch<8)
    {
        tp->Executor.DMA=DMA1;
        tp->Executor.Channel=ch;
        tp->Executor.Stream=(DMA_Stream_TypeDef*)(DMA1_Stream0_BASE+0x18*ch);
        if (ch<4)
        {
            tp->Executor.SR=&DMA1->LISR;
            tp->Executor.CSR=&DMA1->LIFCR;
            if (ch==0)
                tp->Executor.SR_Pos=0;
            else if (ch==1)
                tp->Executor.SR_Pos=6;
            else if (ch==2)
                tp->Executor.SR_Pos=16;
            else if (ch==3)
                tp->Executor.SR_Pos=22;
        }
        else
        {
            tp->Executor.SR=&DMA1->HISR;
            tp->Executor.CSR=&DMA1->HIFCR;
            if (ch==4)
                tp->Executor.SR_Pos=0;
            else if (ch==5)
                tp->Executor.SR_Pos=6;
            else if (ch==6)
                tp->Executor.SR_Pos=16;
            else if (ch==7)
                tp->Executor.SR_Pos=22;
        }
    }
    else
    {
        tp->Executor.DMA=DMA2;
        tp->Executor.Channel=ch-8;
        tp->Executor.Stream=(DMA_Stream_TypeDef*)(DMA2_Stream0_BASE+0x18*(ch-8));
        ch-=8;
        if (ch<4)
        {
            tp->Executor.SR=&DMA2->LISR;
            tp->Executor.CSR=&DMA2->LIFCR;
            if (ch==0)
                tp->Executor.SR_Pos=0;
            else if (ch==1)
                tp->Executor.SR_Pos=6;
            else if (ch==2)
                tp->Executor.SR_Pos=16;
            else if (ch==3)
                tp->Executor.SR_Pos=22;
        }
        else
        {
            tp->Executor.SR=&DMA2->HISR;
            tp->Executor.CSR=&DMA2->HIFCR;
            if (ch==4)
                tp->Executor.SR_Pos=0;
            else if (ch==5)
                tp->Executor.SR_Pos=6;
            else if (ch==6)
                tp->Executor.SR_Pos=16;
            else if (ch==7)
                tp->Executor.SR_Pos=22;
        }
        ch+=8;
    }
    if (tp->Executor.DMA==DMA1)
		RCC->AHB1ENR|=1<<0;
	else
		RCC->AHB1ENR|=1<<1;
	DMA_Disable(tp);
    ((DMAMUX_Channel_TypeDef*)(DMAMUX1_Channel0_BASE+0x4*ch))->CCR|=req;
}

void DMA_StreamController_Set(tranplan* tp)
{
	if ((tp->Start.Type==TRANPORT_TYPE_MEM)&&(tp->End.Type==TRANPORT_TYPE_PER))
	{
		//存储器到外设
		tp->Executor.Stream->CR|=1<<6;
		tp->Executor.Stream->M0AR=(u32)tp->Start.Address;
		tp->Executor.Stream->PAR=(u32)tp->End.Address;
		
		//设置存储器数据大小
		tp->Executor.Stream->CR|=(tp->Start.WordLength-1)<<13;
		//存储器是否递增
		if (!tp->Start.IsFixed)
			tp->Executor.Stream->CR|=1<<10;
				
		//设置外设数据大小
		tp->Executor.Stream->CR|=(tp->End.WordLength-1)<<11;
		//外设是否递增
		if (!tp->End.IsFixed)
			tp->Executor.Stream->CR|=1<<9;
	}
	else if ((tp->Start.Type==TRANPORT_TYPE_PER)&&(tp->End.Type==TRANPORT_TYPE_MEM))
	{
		//外设到存储器
		tp->Executor.Stream->CR|=0<<6;
		tp->Executor.Stream->PAR=(u32)tp->Start.Address;
		tp->Executor.Stream->M0AR=(u32)tp->End.Address;
		
		//设置存储器数据大小
		tp->Executor.Stream->CR|=(tp->End.WordLength-1)<<13;
		//存储器是否递增
		if (!tp->End.IsFixed)
			tp->Executor.Stream->CR|=1<<10;
				
		//设置外设数据大小
		tp->Executor.Stream->CR|=(tp->Start.WordLength-1)<<11;
		//外设是否递增
		if (!tp->Start.IsFixed)
			tp->Executor.Stream->CR|=1<<9;
	}
    else if ((tp->Start.Type==TRANPORT_TYPE_MEM)&&(tp->End.Type==TRANPORT_TYPE_MEM))
    {
        tp->Executor.Stream->CR|=(tp->Start.WordLength-1)<<11;  //源存储器数据大小
        tp->Executor.Stream->CR|=(tp->End.WordLength-1)<<13;      //目标存储器数据大小
        
        tp->Executor.Stream->PAR=(u32)tp->Start.Address;
        tp->Executor.Stream->M0AR=(u32)tp->End.Address;
        
        //指针递增
        if (!tp->Start.IsFixed)
            tp->Executor.Stream->CR|=1<<9;
        if (!tp->End.IsFixed)
            tp->Executor.Stream->CR|=1<<10;
        
        //传输方向：存储器到存储器
        tp->Executor.Stream->CR|=2<<6;
        
        //外设突发配置
        if ((tp->Start.Burst_Mode==Beat4_Burst1)||(tp->Start.Burst_Mode==Beat4_Burst2)||
            (tp->Start.Burst_Mode==Beat4_Burst3)||(tp->Start.Burst_Mode==Beat4_Burst4))
            tp->Executor.Stream->CR|=1<<21;
        else if ((tp->Start.Burst_Mode==Beat8_Burst1)||(tp->Start.Burst_Mode==Beat8_Burst2))
            tp->Executor.Stream->CR|=2<<21;
        else if (tp->Start.Burst_Mode==Beat16_Burst1)
            tp->Executor.Stream->CR|=3<<21;
        else if (tp->Start.Burst_Mode==Single)
            tp->Executor.Stream->CR&=~(3<<21);
        
        //存储器突发配置
        if ((tp->End.Burst_Mode==Beat4_Burst1)||(tp->End.Burst_Mode==Beat4_Burst2)||
            (tp->End.Burst_Mode==Beat4_Burst3)||(tp->End.Burst_Mode==Beat4_Burst4))
            tp->Executor.Stream->CR|=1<<23;
        else if ((tp->End.Burst_Mode==Beat8_Burst1)||(tp->End.Burst_Mode==Beat8_Burst2))
            tp->Executor.Stream->CR|=2<<23;
        else if (tp->End.Burst_Mode==Beat16_Burst1)
            tp->Executor.Stream->CR|=3<<23;
        else if (tp->End.Burst_Mode==Single)
            tp->Executor.Stream->CR&=~(3<<23);
        
        //禁止直接模式
        tp->Executor.Stream->FCR|=1<<2;
        
        //设置FIFO容量
        tp->Executor.Stream->FCR|=tp->Controller.FIFO_Mode;
    }
    
    //传输数据项数
	tp->Executor.Stream->NDTR=tp->Controller.DataNumber;
	
    //开启双缓冲？
    if (tp->Controller.DualBuffer)
		tp->Executor.Stream->CR|=1<<18;
    
    //开启循环模式？
	if (tp->Controller.Loop)
		tp->Executor.Stream->CR|=1<<8;
}

void DMA_Interrupt_Set(tranplan* tp, u8 Group, u8 PrePriority, u8 SubPriority, DMA_Interrupt_CallbackFunc Callback)
{
	u32 Int_Num;
	
	tp->Executor.Stream->CR|=1<<4;
	switch ((u32)tp->Executor.Stream)
	{
		case (u32)DMA1_Stream0:
		{
			DMA1_Stream0_CallbackFunc=Callback;
			Int_Num=DMA1_Stream0_IRQn;
			break;
		}
		case (u32)DMA1_Stream1:
		{
			DMA1_Stream1_CallbackFunc=Callback;
			Int_Num=DMA1_Stream1_IRQn;
			break;
		}
		case (u32)DMA1_Stream2:
		{
			DMA1_Stream2_CallbackFunc=Callback;
			Int_Num=DMA1_Stream2_IRQn;
			break;
		}
		case (u32)DMA1_Stream3:
		{
			DMA1_Stream3_CallbackFunc=Callback;
			Int_Num=DMA1_Stream3_IRQn;
			break;
		}
		case (u32)DMA1_Stream4:
		{
			DMA1_Stream4_CallbackFunc=Callback;
			Int_Num=DMA1_Stream4_IRQn;
			break;
		}
		case (u32)DMA1_Stream5:
		{
			DMA1_Stream5_CallbackFunc=Callback;
			Int_Num=DMA1_Stream5_IRQn;
			break;
		}
		case (u32)DMA1_Stream6:
		{
			DMA1_Stream6_CallbackFunc=Callback;
			Int_Num=DMA1_Stream6_IRQn;
			break;
		}
		case (u32)DMA1_Stream7:
		{
			DMA1_Stream7_CallbackFunc=Callback;
			Int_Num=DMA1_Stream7_IRQn;
			break;
		}
		case (u32)DMA2_Stream0:
		{
			DMA2_Stream0_CallbackFunc=Callback;
			Int_Num=DMA2_Stream0_IRQn;
			break;
		}
		case (u32)DMA2_Stream1:
		{
			DMA2_Stream1_CallbackFunc=Callback;
			Int_Num=DMA2_Stream1_IRQn;
			break;
		}
		case (u32)DMA2_Stream2:
		{
			DMA2_Stream2_CallbackFunc=Callback;
			Int_Num=DMA2_Stream2_IRQn;
			break;
		}
		case (u32)DMA2_Stream3:
		{
			DMA2_Stream3_CallbackFunc=Callback;
			Int_Num=DMA2_Stream3_IRQn;
			break;
		}
		case (u32)DMA2_Stream4:
		{
			DMA2_Stream4_CallbackFunc=Callback;
			Int_Num=DMA2_Stream4_IRQn;
			break;
		}
		case (u32)DMA2_Stream5:
		{
			DMA2_Stream5_CallbackFunc=Callback;
			Int_Num=DMA2_Stream5_IRQn;
			break;
		}
		case (u32)DMA2_Stream6:
		{
			DMA2_Stream6_CallbackFunc=Callback;
			Int_Num=DMA2_Stream6_IRQn;
			break;
		}
		case (u32)DMA2_Stream7:
		{
			DMA2_Stream7_CallbackFunc=Callback;
			Int_Num=DMA2_Stream7_IRQn;
			break;
		}
		default:
			break;	
	}
	MY_NVIC_Init(PrePriority,SubPriority,Int_Num,Group);
}

void DMA_Reload(tranplan* tp)
{
	if ((tp->Start.Type==TRANPORT_TYPE_MEM)&&(tp->End.Type==TRANPORT_TYPE_PER))
	{
		tp->Executor.Stream->M0AR=(u32)tp->Start.Address;
		tp->Executor.Stream->PAR=(u32)tp->End.Address;
	}
	else if ((tp->Start.Type==TRANPORT_TYPE_PER)&&(tp->End.Type==TRANPORT_TYPE_MEM))
	{
		tp->Executor.Stream->PAR=(u32)tp->Start.Address;
		tp->Executor.Stream->M0AR=(u32)tp->End.Address;
	}
	tp->Executor.Stream->NDTR=tp->Controller.DataNumber; 
}

void DMA1_Stream0_IRQHandler(void)
{
	if ((DMA1->LISR>>5)&1)
		DMA1_Stream0_CallbackFunc();
	DMA1->LIFCR|=1<<5;
}

void DMA1_Stream1_IRQHandler(void)
{
	if ((DMA1->LISR>>11)&1)
		DMA1_Stream1_CallbackFunc();
	DMA1->LIFCR|=1<<11;
}

void DMA1_Stream2_IRQHandler(void)
{
	if ((DMA1->LISR>>21)&1)
		DMA1_Stream2_CallbackFunc();
	DMA1->LIFCR|=1<<21;
}

void DMA1_Stream3_IRQHandler(void)
{
	if ((DMA1->LISR>>27)&1)
		DMA1_Stream3_CallbackFunc();
	DMA1->LIFCR|=1<<27;
}

void DMA1_Stream4_IRQHandler(void)
{
	if ((DMA1->HISR>>5)&1)
		DMA1_Stream4_CallbackFunc();
	DMA1->HIFCR|=1<<5;
}

void DMA1_Stream5_IRQHandler(void)
{
	if ((DMA1->HISR>>11)&1)
		DMA1_Stream5_CallbackFunc();
	DMA1->HIFCR|=1<<11;
}

void DMA1_Stream6_IRQHandler(void)
{
	if ((DMA1->HISR>>21)&1)
		DMA1_Stream6_CallbackFunc();
	DMA1->HIFCR|=1<<21;
}

void DMA1_Stream7_IRQHandler(void)
{
	if ((DMA1->HISR>>27)&1)
		DMA1_Stream7_CallbackFunc();
	DMA1->HIFCR|=1<<27;
}








void DMA2_Stream0_IRQHandler(void)
{
	if ((DMA2->LISR>>5)&1)
		DMA2_Stream0_CallbackFunc();
	DMA2->LIFCR|=1<<5;
}

void DMA2_Stream1_IRQHandler(void)
{
	if ((DMA2->LISR>>11)&1)
		DMA2_Stream1_CallbackFunc();
	DMA2->LIFCR|=1<<11;
}

void DMA2_Stream2_IRQHandler(void)
{
	if ((DMA2->LISR>>21)&1)
		DMA2_Stream2_CallbackFunc();
	DMA2->LIFCR|=1<<21;
}

void DMA2_Stream3_IRQHandler(void)
{
	if ((DMA2->LISR>>27)&1)
		DMA2_Stream3_CallbackFunc();
	DMA2->LIFCR|=1<<27;
}

void DMA2_Stream4_IRQHandler(void)
{
	if ((DMA2->HISR>>5)&1)
		DMA2_Stream4_CallbackFunc();
	DMA2->HIFCR|=1<<5;
}

void DMA2_Stream5_IRQHandler(void)
{
	if ((DMA2->HISR>>11)&1)
		DMA2_Stream5_CallbackFunc();
	DMA2->HIFCR|=1<<11;
}

void DMA2_Stream6_IRQHandler(void)
{
	if ((DMA2->HISR>>21)&1)
		DMA2_Stream6_CallbackFunc();
	DMA2->HIFCR|=1<<21;
}

void DMA2_Stream7_IRQHandler(void)
{
	if ((DMA2->HISR>>27)&1)
		DMA2_Stream7_CallbackFunc();
	DMA2->HIFCR|=1<<27;
}

