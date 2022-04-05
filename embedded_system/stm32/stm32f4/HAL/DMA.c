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

void DMA_Build_TransferPlan(tranplan* tp, u16 DMA_Channel)
{
	u8 stream;
	
	stream=(DMA_Channel/10)%10;
	if ((DMA_Channel/100)==1)
	{
		tp->Executor.DMA=DMA1;
		tp->Executor.Stream=(DMA_Stream_TypeDef*)(DMA1_Stream0_BASE+stream*0x18);
		tp->Executor.Channel=DMA_Channel%10;

	}
	else
	{
		tp->Executor.DMA=DMA2;
		tp->Executor.Stream=(DMA_Stream_TypeDef*)(DMA2_Stream0_BASE+stream*0x18);
		tp->Executor.Channel=DMA_Channel%10;
	}
	if (stream<=3)
	{
		tp->Executor.SR=&tp->Executor.DMA->LISR;
		tp->Executor.CSR=&tp->Executor.DMA->LIFCR;
		if (stream==0)
			tp->Executor.SR_Pos=0;
		else if (stream==1)
			tp->Executor.SR_Pos=6;
		else if (stream==2)
			tp->Executor.SR_Pos=16;
		else if (stream==3)
			tp->Executor.SR_Pos=22;
	}
	else
	{
		tp->Executor.SR=&tp->Executor.DMA->HISR;
		tp->Executor.CSR=&tp->Executor.DMA->HIFCR;
		if (stream==4)
			tp->Executor.SR_Pos=0;
		else if (stream==5)
			tp->Executor.SR_Pos=6;
		else if (stream==6)
			tp->Executor.SR_Pos=16;
		else if (stream==7)
			tp->Executor.SR_Pos=22;
	}
	if (tp->Executor.DMA==DMA1)
		RCC->AHB1ENR|=1<<21;
	else
		RCC->AHB1ENR|=1<<22;
	DMA_Disable(tp);
}

void DMA_StreamController_Set(tranplan* tp)
{
	tp->Executor.Stream->CR|=tp->Executor.Channel<<25;
	if ((tp->Start.Type==TRANPORT_TYPE_MEM)&&(tp->End.Type==TRANPORT_TYPE_PER))
	{
		//存储器到外设
		tp->Executor.Stream->CR|=1<<6;
		tp->Executor.Stream->M0AR=(u32)tp->Start.Address;
		tp->Executor.Stream->PAR=(u32)tp->End.Address;
		
		//设置存储器数据大小
		tp->Executor.Stream->CR|=(tp->Controller.WordLength-1)<<13;
		//存储器是否递增
		if (!tp->Start.IsFixed)
			tp->Executor.Stream->CR|=1<<10;
				
		//设置外设数据大小
		tp->Executor.Stream->CR|=(tp->Controller.WordLength-1)<<11;
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
		tp->Executor.Stream->CR|=(tp->Controller.WordLength-1)<<13;
		//存储器是否递增
		if (!tp->End.IsFixed)
			tp->Executor.Stream->CR|=1<<10;
				
		//设置外设数据大小
		tp->Executor.Stream->CR|=(tp->Controller.WordLength-1)<<11;
		//外设是否递增
		if (!tp->Start.IsFixed)
			tp->Executor.Stream->CR|=1<<9;
	}
	tp->Executor.Stream->NDTR=tp->Controller.DataNumber;
	if (tp->Controller.DualBuffer)
		tp->Executor.Stream->CR|=1<<18;
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

/*void DMA_SR_Clear(tranplan* tp)
{
	switch ((u32)tp->Executor.Stream)
	{
		case (u32)DMA1_Stream0:
		{
			DMA1->LIFCR|=0x3d<<0;
			break;
		}
		case (u32)DMA1_Stream1:
		{
			DMA1->LIFCR|=0x3d<<6;
			break;
		}
		case (u32)DMA1_Stream2:
		{
			DMA1->LIFCR|=0x3d<<16;
			break;
		}
		case (u32)DMA1_Stream3:
		{
			DMA1->LIFCR|=0x3d<<22;
			break;
		}
		case (u32)DMA1_Stream4:
		{
			DMA1->HIFCR|=0x3d<<0;
			break;
		}
		case (u32)DMA1_Stream5:
		{
			DMA1->HIFCR|=0x3d<<6;
			break;
		}
		case (u32)DMA1_Stream6:
		{
			DMA1->HIFCR|=0x3d<<16;
			break;
		}
		case (u32)DMA1_Stream7:
		{
			DMA1->HIFCR|=0x3d<<22;
			break;
		}
		case (u32)DMA2_Stream0:
		{
			DMA2->LIFCR|=0x3d<<0;
			break;
		}
		case (u32)DMA2_Stream1:
		{
			DMA2->LIFCR|=0x3d<<6;
			break;
		}
		case (u32)DMA2_Stream2:
		{
			DMA2->LIFCR|=0x3d<<16;
			break;
		}
		case (u32)DMA2_Stream3:
		{
			DMA2->LIFCR|=0x3d<<22;
			break;
		}
		case (u32)DMA2_Stream4:
		{
			DMA2->HIFCR|=0x3d<<0;
			break;
		}
		case (u32)DMA2_Stream5:
		{
			DMA2->HIFCR|=0x3d<<6;
			break;
		}
		case (u32)DMA2_Stream6:
		{
			DMA2->HIFCR|=0x3d<<16;
			break;
		}
		case (u32)DMA2_Stream7:
		{
			DMA2->HIFCR|=0x3d<<22;
			break;
		}
		default:
			break;	
	}
}*/

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

