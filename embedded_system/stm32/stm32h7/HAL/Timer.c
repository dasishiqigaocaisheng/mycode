#include "Timer.h"
#include "USART.h"
//#include "delay.h"

#ifdef USE_FREQUENCY_MEASURE
#include "MathHelper.h"
#endif

#define ADVANCED_TIMER	1
#define GENERAL_TIMER1	2
#define GENERAL_TIMER2	2
#define BASIC_TIMER		3

#define DIRECTION_UP	1
#define DIRECTION_DOWN	2

#define ACCURACY		0.005f

Timer_Update_CallBackFunc_Type 
Timer1_CallBackFunc,Timer2_CallBackFunc,
Timer3_CallBackFunc,Timer4_CallBackFunc,
Timer5_CallBackFunc,Timer6_CallBackFunc,
Timer7_CallBackFunc,Timer8_CallBackFunc,
Timer9_CallBackFunc,Timer10_CallBackFunc,
Timer11_CallBackFunc,Timer12_CallBackFunc,
Timer13_CallBackFunc,Timer14_CallBackFunc;

u32 _Timer_Cnt_Calculate(u8 Length, u32 Clock, float Time_Us);
float _Timer_Update_Time_Calculate(u8 Length, u32 Clock, u32 ARR);
float _Timer_Measure_Period(Timer_Info* Main_Timer, u8 ICx, u8 Trigger_Time, float Min_Freq);
float _Timer_Pulse_Count(Timer_Info* Main_Timer, Timer_Info* Aux_Timer, u8 ICx, u8 TIx, u8 Trigger_Time, u16 Sample_Time);
float _Timer_Measure_Period_Error(float f, u32 tf);
float _Timer_Pulse_Count_Error(float f, u16 st);

float _Timer_Measure_Period_Error(float f, u32 tf)
{
	return f/(tf-f);
}

float _Timer_Pulse_Count_Error(float f, u16 st)
{
	float n;
	
	n=st*f/1000-1;
	
	return (f-n*1000/st)/f;
}

float _Timer_Measure_Period(Timer_Info* Main_Timer, u8 ICx, u8 Trigger_Time, float Min_Freq)
{
	u8 i;
	u32 save1,save2,Min_Freq_CNT_Value;
		
	i=0;
	Min_Freq_CNT_Value=Main_Timer->Timing_Frequency/Min_Freq;
	Timer_InputCatch_Reload(Main_Timer,ICx);
	Timer_InputCatch_Enable(Main_Timer,ICx);
	Timer_Enable(Main_Timer);
	while (Timer_Get_CCxIF(Main_Timer,ICx)!=1)
	{
		if (Main_Timer->TIMx->CNT>Min_Freq_CNT_Value)
		{
			Timer_Disable(Main_Timer);
			Timer_InputCatch_Disable(Main_Timer,ICx);
			Timer_Clear_CCxIF(Main_Timer,ICx);
			return -1;
		}
	}
	save1=Timer_Get_CatchValue(Main_Timer,ICx);
	while (i!=Trigger_Time)
	{
		Timer_Clear_CCxIF(Main_Timer,ICx);
		while (Timer_Get_CCxIF(Main_Timer,ICx)!=1)
		{
			if (Main_Timer->TIMx->CNT-save1>Min_Freq_CNT_Value)
			{
				Timer_Disable(Main_Timer);
				Timer_InputCatch_Disable(Main_Timer,ICx);
				Timer_Clear_CCxIF(Main_Timer,ICx);
				return -1;
			}
		}
		save2=Timer_Get_CatchValue(Main_Timer,ICx);
		i++;
	}
	Timer_Disable(Main_Timer);
	Timer_InputCatch_Disable(Main_Timer,ICx);
	return (float)Main_Timer->Timing_Frequency/(save2-save1);
}

float _Timer_Pulse_Count(Timer_Info* Main_Timer, Timer_Info* Aux_Timer, u8 ICx, u8 TIx, u8 Trigger_Time, u16 Sample_Time)
{
	u32 save;
	
	Timer_Timing_SetTime_ms(Aux_Timer,Sample_Time);
	Timer_Clear_UIF(Aux_Timer);
	Timer_InputCatch_Reload(Main_Timer,ICx);
	Timer_InputCatch_Disable(Main_Timer,ICx);
	Main_Timer->TIMx->PSC=0;
	Main_Timer->TIMx->EGR|=1;
	Main_Timer->TIMx->CCMR1|=1<<((TIx-1)*8);//通道选择
	//触发选择
	if (TIx==TI1)
		Main_Timer->TIMx->SMCR|=5<<4;
	else if (TIx==TI2)
		Main_Timer->TIMx->SMCR|=6<<4;
	Main_Timer->TIMx->SMCR|=7;	//使能外部时钟模式1
	Timer_Enable(Main_Timer);
	Timer_Enable(Aux_Timer);
	while (Timer_Get_UIF(Aux_Timer)!=1);
	Timer_Clear_UIF(Aux_Timer);
	save=Main_Timer->TIMx->CNT;
	Timer_Disable(Main_Timer);
	Timer_Disable(Aux_Timer);
	Main_Timer->TIMx->PSC=Main_Timer->Frequency_Div-1;
	Main_Timer->TIMx->SMCR&=~7;	//关闭外部时钟模式1
	Main_Timer->TIMx->EGR|=1;
	
	return ((float)save*1000/Trigger_Time)/Sample_Time;
}

u32 _Timer_Cnt_Calculate(u8 Length, u32 Clock, float Time_Us)
{
	u32 save;
	
	save=Time_Us*(Clock/1000000.0f);
	if (save>=1)
		save--;

	return save;
}

float _Timer_Update_Time_Calculate(u8 Length, u32 Clock, u32 ARR)
{
	u32 save;

	save=ARR+1;
    	
	return (1000000.0f/Clock)*save;
}
void Timer_Reset(TIM_TypeDef* Timer, Timer_Info* Info_Struct)
{
	Info_Struct->TIMx=Timer;
	Info_Struct->Frequency_Div=0;
	Info_Struct->PWM_Duty_Cycle[0]=0;
	Info_Struct->PWM_Duty_Cycle[1]=0;
	Info_Struct->PWM_Duty_Cycle[2]=0;
	Info_Struct->PWM_Duty_Cycle[3]=0;
	Info_Struct->PWM_Channel[0]=0;
	Info_Struct->PWM_Channel[1]=0;
	Info_Struct->PWM_Channel[2]=0;
	Info_Struct->PWM_Channel[3]=0;
	Info_Struct->PWM_Frequency=0;
	Info_Struct->Clock=0;
	Info_Struct->Timing_Frequency=0;
	Info_Struct->Timing_us=0;
	
    if (Timer==TIM1)
	{
		RCC->APB2RSTR|=1;
		delay_ms(2);
		RCC->APB2RSTR&=(~1);
		Info_Struct->Timer_Type=ADVANCED_TIMER;
		Info_Struct->CNT_Length=16;
		Info_Struct->One_Pulse=1;
	}
	else if (Timer==TIM8)
	{
		RCC->APB2RSTR|=1<<1;
		delay_ms(2);
		RCC->APB2RSTR&=~(1<<1);
		Info_Struct->Timer_Type=ADVANCED_TIMER;
		Info_Struct->CNT_Length=16;
		Info_Struct->One_Pulse=1;
	}
	else if (Timer==TIM2)
	{
		RCC->APB1LRSTR|=1;
		delay_ms(2);
		RCC->APB1LRSTR&=(~1);
		Info_Struct->Timer_Type=GENERAL_TIMER1;
		Info_Struct->CNT_Length=32;
		Info_Struct->One_Pulse=1;
	}
	else if (Timer==TIM3)
	{
		RCC->APB1LRSTR|=1<<1;
		delay_ms(2);
		RCC->APB1LRSTR&=~(1<<1);
		Info_Struct->Timer_Type=GENERAL_TIMER1;
		Info_Struct->CNT_Length=16;
		Info_Struct->One_Pulse=1;
	}
	else if (Timer==TIM4)
	{
		RCC->APB1LRSTR|=1<<2;
		delay_ms(2);
		RCC->APB1LRSTR&=~(1<<2);
		Info_Struct->Timer_Type=GENERAL_TIMER1;
		Info_Struct->CNT_Length=16;
		Info_Struct->One_Pulse=1;
	}
	else if (Timer==TIM5)
	{
		RCC->APB1LRSTR|=1<<3;
		delay_ms(2);
		RCC->APB1LRSTR&=~(1<<3);
		Info_Struct->Timer_Type=GENERAL_TIMER1;
		Info_Struct->CNT_Length=32;
		Info_Struct->One_Pulse=1;
	}
	else if (Timer==TIM12)
	{
		RCC->APB1LRSTR|=1<<6;
		delay_ms(2);
		RCC->APB1LRSTR&=~(1<<6);
		Info_Struct->Timer_Type=GENERAL_TIMER2;
		Info_Struct->CNT_Length=16;
		Info_Struct->One_Pulse=1;
	}
	else if (Timer==TIM13)
	{
		RCC->APB1LRSTR|=1<<7;
		delay_ms(2);
		RCC->APB1LRSTR&=~(1<<7);
		Info_Struct->Timer_Type=GENERAL_TIMER2;
		Info_Struct->CNT_Length=16;
		Info_Struct->One_Pulse=0;
	}
	else if (Timer==TIM14)
	{
		RCC->APB1LRSTR|=1<<8;
		delay_ms(2);
		RCC->APB1LRSTR&=~(1<<8);
		Info_Struct->Timer_Type=GENERAL_TIMER2;
		Info_Struct->CNT_Length=16;
		Info_Struct->One_Pulse=0;
	}
	else if (Timer==TIM6)
	{
		RCC->APB1LRSTR|=1<<4;
		delay_ms(2);
		RCC->APB1LRSTR&=~(1<<4);
		Info_Struct->Timer_Type=BASIC_TIMER;
		Info_Struct->CNT_Length=16;
		Info_Struct->One_Pulse=1;
	}
	else if (Timer==TIM7)
	{
		RCC->APB1LRSTR|=1<<5;
		delay_ms(2);
		RCC->APB1LRSTR&=~(1<<5);
		Info_Struct->Timer_Type=BASIC_TIMER;
		Info_Struct->CNT_Length=16;
		Info_Struct->One_Pulse=1;
	}
}

void Timer_Global_Set(Timer_Info* Timer, u8 Clock_Source, u32 APBx_Clock, u8 Clock_Division, u32 Timing_Frequency, u8 Trigger_Mode)
{
    #if !defined USE_STM32H7
	if (Timer->TIMx==TIM1)
		RCC->APB2ENR|=1<<0;
	else if (Timer->TIMx==TIM8)
		RCC->APB2ENR|=1<<1;
	else if (Timer->TIMx==TIM2)
		RCC->APB1ENR|=1<<0;
	else if (Timer->TIMx==TIM3)
		RCC->APB1ENR|=1<<1;
	else if (Timer->TIMx==TIM4)
		RCC->APB1ENR|=1<<2;
	else if (Timer->TIMx==TIM5)
		RCC->APB1ENR|=1<<3;
	else if (Timer->TIMx==TIM9)
		RCC->APB2ENR|=1<<16;
	else if (Timer->TIMx==TIM10)
		RCC->APB2ENR|=1<<17;
	else if (Timer->TIMx==TIM11)
		RCC->APB2ENR|=1<<18;
	else if (Timer->TIMx==TIM12)
		RCC->APB2ENR|=1<<6;
	else if (Timer->TIMx==TIM13)
		RCC->APB2ENR|=1<<7;
	else if (Timer->TIMx==TIM14)
		RCC->APB2ENR|=1<<8;
	else if (Timer->TIMx==TIM6)
		RCC->APB1ENR|=1<<4;
	else if (Timer->TIMx==TIM7)
		RCC->APB1ENR|=1<<5;
	#else
	if (Timer->TIMx==TIM1)
		RCC->APB2ENR|=1<<0;
	else if (Timer->TIMx==TIM8)
		RCC->APB2ENR|=1<<1;
	else if (Timer->TIMx==TIM2)
		RCC->APB1LENR|=1<<0;
	else if (Timer->TIMx==TIM3)
		RCC->APB1LENR|=1<<1;
	else if (Timer->TIMx==TIM4)
		RCC->APB1LENR|=1<<2;
	else if (Timer->TIMx==TIM5)
		RCC->APB1LENR|=1<<3;
	else if (Timer->TIMx==TIM12)
		RCC->APB2ENR|=1<<6;
	else if (Timer->TIMx==TIM13)
		RCC->APB2ENR|=1<<7;
	else if (Timer->TIMx==TIM14)
		RCC->APB2ENR|=1<<8;
	else if (Timer->TIMx==TIM6)
		RCC->APB1LENR|=1<<4;
	else if (Timer->TIMx==TIM7)
		RCC->APB1LENR|=1<<5;
    #endif
	Timer->TIMx->CR1|=1<<2;	//只有定时器溢出才会产生中断，UG位置1将不会产生中断
	if (Clock_Source==INSIDE_CLOCK_SOURCE)
	{
		u16 PSC;
		PSC=APBx_Clock/Timing_Frequency-1;
		Timer->TIMx->PSC=PSC;
		
		Timer->Frequency_Div=PSC+1;
		Timer->Clock=APBx_Clock;
		Timer->Timing_Frequency=(float)APBx_Clock/(Timer->TIMx->PSC+1);
	}
	else if (Clock_Source==EXTERN_CLOCK_SOURCE)
		Timer->TIMx->SMCR|=7;		//外部时钟模式1使能
    
    Timer->TIMx->CR1|=Clock_Division<<8;
	
	if (Trigger_Mode==TRIGGER_OUTSIDE)
	{
		Timer->TIMx->SMCR&=~7;
		Timer->TIMx->SMCR|=6;	//触发模式
	}
}

void Timer_ExternClock_Set(Timer_Info* Timer, u8 TIx, u16 Frequency_Div)
{
	Timer->TIMx->PSC=Frequency_Div-1;
	Timer->TIMx->CCMR1|=1<<((TIx-1)*8);//通道选择
	//触发选择
	if (TIx==TI1)
		Timer->TIMx->SMCR|=5<<4;
	else if (TIx==TI2)
		Timer->TIMx->SMCR|=6<<4;
	Timer->Frequency_Div=Frequency_Div;
}

void Timer_TriggerMode_Set(Timer_Info* Timer, u8 TIx, u8 Polar)
{
	if (TIx==TI1)
	{
		Timer->TIMx->SMCR|=5<<4;//触发信号选择TI1FP1
		Timer->TIMx->CCMR1|=1;	//IC1映射到TI1上
		Timer->TIMx->CCER&=~(5<<1);
		Timer->TIMx->CCER|=Polar;
	}
	else if (TIx==TI2)
	{
		Timer->TIMx->SMCR|=6<<4;//触发信号选择TI2FP2
		Timer->TIMx->CCMR1|=1<<8;//IC2映射到TI2上
		Timer->TIMx->CCER&=~(5<<5);
		Timer->TIMx->CCER|=Polar<<4;
	}
}

void Timer_Free_Run_Init(Timer_Info* Timer)
{
	Timer->TIMx->EGR|=1;	//生成更新事件
}

void Timer_Free_Run_Reload(Timer_Info* Timer)
{
	Timer->TIMx->CR1&=~1;
	Timer->TIMx->CNT=0;
	if ((Timer->TIMx==TIM1)||(Timer->TIMx==TIM8))
		Timer->TIMx->RCR=0;
}

void Timer_Timing_Init(Timer_Info* Timer, u8 One_Pulse)
{
	if ((Timer->One_Pulse)&&One_Pulse)
		Timer->TIMx->CR1|=1<<3;	//单脉冲模式
}

void Timer_Timing_Interrupt_Set(Timer_Info* Timer, u8 Group, u8 Preemption_Priority, u8 Sub_Priority, 
								Timer_Update_CallBackFunc_Type CallBackFunc)
{
	Timer_Clear_UIF(Timer);
	Timer->TIMx->DIER|=1;	//更新中断使能
	if (Timer->TIMx==TIM1)
	{
		MY_NVIC_Init(Preemption_Priority,Sub_Priority,TIM1_UP_TIM10_IRQn,Group);
		Timer1_CallBackFunc=CallBackFunc;
	}
	else if (Timer->TIMx==TIM8)
	{
		MY_NVIC_Init(Preemption_Priority,Sub_Priority,TIM8_UP_TIM13_IRQn,Group);
		Timer8_CallBackFunc=CallBackFunc;
	}
	else if (Timer->TIMx==TIM2)
	{
		MY_NVIC_Init(Preemption_Priority,Sub_Priority,TIM2_IRQn,Group);
		Timer2_CallBackFunc=CallBackFunc;
	}
	else if (Timer->TIMx==TIM3)
	{
		MY_NVIC_Init(Preemption_Priority,Sub_Priority,TIM3_IRQn,Group);
		Timer3_CallBackFunc=CallBackFunc;;
	}
	else if (Timer->TIMx==TIM4)
	{
		MY_NVIC_Init(Preemption_Priority,Sub_Priority,TIM4_IRQn,Group);
		Timer4_CallBackFunc=CallBackFunc;;
	}
	else if (Timer->TIMx==TIM5)
	{
		MY_NVIC_Init(Preemption_Priority,Sub_Priority,TIM5_IRQn,Group);
		Timer5_CallBackFunc=CallBackFunc;
	}
    #if !defined USE_STM32H7
	else if (Timer->TIMx==TIM9)
	{
		MY_NVIC_Init(Preemption_Priority,Sub_Priority,TIM1_BRK_TIM9_IRQn,Group);
		Timer9_CallBackFunc=CallBackFunc;
	}
	else if (Timer->TIMx==TIM10)
	{
		MY_NVIC_Init(Preemption_Priority,Sub_Priority,TIM1_UP_TIM10_IRQn,Group);
		Timer10_CallBackFunc=CallBackFunc;
	}
	else if (Timer->TIMx==TIM11)
	{
		MY_NVIC_Init(Preemption_Priority,Sub_Priority,TIM1_TRG_COM_TIM11_IRQn,Group);
		Timer11_CallBackFunc=CallBackFunc;
	}
    #endif
	else if (Timer->TIMx==TIM12)
	{
		MY_NVIC_Init(Preemption_Priority,Sub_Priority,TIM8_BRK_TIM12_IRQn,Group);
		Timer12_CallBackFunc=CallBackFunc;
	}
	else if (Timer->TIMx==TIM13)
	{
		MY_NVIC_Init(Preemption_Priority,Sub_Priority,TIM8_UP_TIM13_IRQn,Group);
		Timer13_CallBackFunc=CallBackFunc;
	}
	else if (Timer->TIMx==TIM14)
	{
		MY_NVIC_Init(Preemption_Priority,Sub_Priority,TIM8_TRG_COM_TIM14_IRQn,Group);
		Timer14_CallBackFunc=CallBackFunc;
	}
	else if (Timer->TIMx==TIM6)
	{
		MY_NVIC_Init(Preemption_Priority,Sub_Priority,TIM6_DAC_IRQn,Group);
		Timer6_CallBackFunc=CallBackFunc;
	}
	else if (Timer->TIMx==TIM7)
	{
		MY_NVIC_Init(Preemption_Priority,Sub_Priority,TIM7_IRQn,Group);
		Timer7_CallBackFunc=CallBackFunc;
	}
}

u8 Timer_Timing_SetTime_us(Timer_Info* Timer, float Time)
{
	float Max_Time;
	
	Timer->TIMx->CR1|=1<<2;	//只有定时器溢出才会产生中断，UG位置1将不会产生中断
	if (Timer->Timer_Type==ADVANCED_TIMER)
	{
		Max_Time=((float)0xffff/Timer->Timing_Frequency)*1000000;
		
		if (Max_Time>=Time)
		{
			Timer->TIMx->ARR=_Timer_Cnt_Calculate(16,Timer->Timing_Frequency,Time);
			Timer->TIMx->RCR=0;
		}
		else
		{
			float Load_Time;
			u8 Repeat_Time;
			
			Repeat_Time=(u8)(Time/Max_Time+1);
			Load_Time=(float)Time/Repeat_Time;
			
			Timer->TIMx->ARR=_Timer_Cnt_Calculate(16,Timer->Timing_Frequency,Load_Time);
			Timer->TIMx->RCR=Repeat_Time-1;
		}
		
		Timer->TIMx->EGR|=1<<0;	//产生一次更新事件
		Timer->Timing_us=(Timer->TIMx->RCR+1)*_Timer_Update_Time_Calculate(16,Timer->Timing_Frequency,Timer->TIMx->ARR);
		Timer->TIMx->CR1&=~(1<<2);	
	}
	else
	{
		Timer->TIMx->ARR=_Timer_Cnt_Calculate(Timer->CNT_Length,Timer->Timing_Frequency,Time);
		Timer->TIMx->EGR|=1<<0;	//产生一次更新事件
		Timer->Timing_us=_Timer_Update_Time_Calculate(Timer->CNT_Length,Timer->Timing_Frequency,Timer->TIMx->ARR);
		Timer->TIMx->CR1&=~(1<<2);
	}


	return 0;
}

u8 Timer_Timing_SetTime_ms(Timer_Info* Timer, u32 Time)
{
	float Max_Time;
	
	Timer->TIMx->CR1|=1<<2;	//只有定时器溢出才会产生中断，UG位置1将不会产生中断
	if (Timer->Timer_Type==ADVANCED_TIMER)
	{
		Max_Time=((float)0xffff/Timer->Timing_Frequency)*1000;
		
		if (Max_Time>=Time)
		{
			Timer->TIMx->ARR=_Timer_Cnt_Calculate(16,Timer->Timing_Frequency,Time*1000);
			Timer->TIMx->RCR=0;
		}
		else
		{
			float Load_Time;
			u16 Repeat_Time;
			
			Repeat_Time=(u8)(Time/Max_Time+1);
			Load_Time=(float)Time/Repeat_Time;
			
			Timer->TIMx->ARR=_Timer_Cnt_Calculate(16,Timer->Timing_Frequency,Load_Time*1000);
			Timer->TIMx->RCR=Repeat_Time-1;
		}
		
        Timer->Timing_us=(Timer->TIMx->RCR+1)*_Timer_Update_Time_Calculate(16,Timer->Timing_Frequency,Timer->TIMx->ARR);
		Timer->TIMx->EGR|=1<<0;	//产生一次更新事件
		Timer->TIMx->CR1&=~(1<<2);	
	}
	else
	{
		Timer->TIMx->ARR=_Timer_Cnt_Calculate(Timer->CNT_Length,Timer->Timing_Frequency,Time*1000);
        Timer->Timing_us=_Timer_Update_Time_Calculate(Timer->CNT_Length,Timer->Timing_Frequency,Timer->TIMx->ARR);
		Timer->TIMx->EGR|=1<<0;	//产生一次更新事件
		Timer->TIMx->CR1&=~(1<<2);	
	}

	return 0;
}

u8 Timer_Timing_SetTime_s(Timer_Info* Timer, u16 Time)
{
	float Max_Time;

	Timer->TIMx->CR1|=1<<2;	//只有定时器溢出才会产生中断，UG位置1将不会产生中断	
	if (Timer->Timer_Type==ADVANCED_TIMER)
	{
		Max_Time=(float)0xffff/Timer->Timing_Frequency;
		
		if (Max_Time>=Time)
		{
			Timer->TIMx->ARR=_Timer_Cnt_Calculate(16,Timer->Timing_Frequency,Time*1000000);
			Timer->TIMx->RCR=0;
		}
		else
		{
			float Load_Time;
			u8 Repeat_Time;
			
			Repeat_Time=(u8)(Time/Max_Time+1);
			Load_Time=(float)Time/Repeat_Time;
			
			Timer->TIMx->ARR=_Timer_Cnt_Calculate(16,Timer->Timing_Frequency,Load_Time*1000000);
			Timer->TIMx->RCR=Repeat_Time-1;
		}
		
		Timer->TIMx->EGR|=1<<0;	//产生一次更新事件
		Timer->Timing_us=(Timer->TIMx->RCR+1)*_Timer_Update_Time_Calculate(16,Timer->Timing_Frequency,Timer->TIMx->ARR);
		Timer->TIMx->CR1&=~(1<<2);	
	}
	else
	{
		Timer->TIMx->ARR=_Timer_Cnt_Calculate(16,Timer->Timing_Frequency,Time*1000000);
		Timer->TIMx->EGR|=1<<0;	//产生一次更新事件
		Timer->Timing_us=_Timer_Update_Time_Calculate(Timer->CNT_Length,Timer->Timing_Frequency,Timer->TIMx->ARR);
		Timer->TIMx->CR1&=~(1<<2);	
	}
	
	return 0;
}

void Timer_Timing_Reload(Timer_Info* Timer)
{
	Timer->TIMx->CR1|=1<<2;	//只有定时器溢出才会产生中断，UG位置1将不会产生中断
	Timer->TIMx->EGR|=1;	//产生一次更新事件
	Timer->TIMx->CR1&=~(1<<2);	
}

void Timer_PWMOutput_Init(Timer_Info* Timer, u32 PWM_Frequency)
{
	Timer->TIMx->ARR=_Timer_Cnt_Calculate(Timer->CNT_Length,Timer->Timing_Frequency,1000000.0f/PWM_Frequency);
	Timer->TIMx->CR1|=1<<7;		//ARR寄存器缓冲
	if (Timer->Timer_Type==ADVANCED_TIMER)
		Timer->TIMx->BDTR|=1<<15;	//主输出使能
	Timer->TIMx->EGR|=1;		//产生一次更新事件
	Timer->PWM_Frequency=1000000/_Timer_Update_Time_Calculate(Timer->CNT_Length,Timer->Timing_Frequency,Timer->TIMx->ARR);
    Timer->PWM_Duty_Cycle[0]=0.5f;
    Timer->PWM_Duty_Cycle[1]=0.5f;
    Timer->PWM_Duty_Cycle[2]=0.5f;
    Timer->PWM_Duty_Cycle[3]=0.5f;
}

void Timer_PWMOutput_Frequency_Set(Timer_Info* Timer, u32 PWM_Frequency, u8 Update)
{
	u8 i;
	Timer->TIMx->ARR=_Timer_Cnt_Calculate(Timer->CNT_Length,Timer->Timing_Frequency,1000000.0f/PWM_Frequency);
	Timer->PWM_Frequency=1000000/_Timer_Update_Time_Calculate(Timer->CNT_Length,Timer->Timing_Frequency,Timer->TIMx->ARR);
	for (i=0;i<4;i++)
	{
		if (Timer->PWM_Channel[i])
			Timer_PWMOutput_Wave_Set(Timer,Timer->PWM_Duty_Cycle[i],1<<(i*4),Update);
	}
	if (Update)
		Timer->TIMx->EGR|=1;	//产生一次更新事件
}

void Timer_PWMOutput_Wave_Set(Timer_Info* Timer, float Duty_Cycle, u16 OCx, u8 Update)
{
	if (OCx==OC1)
	{
		Timer->TIMx->CCMR1|=1<<3;	//输出比较1预装载使能
		Timer->TIMx->CCMR1|=6<<4;	//输出比较1配置为PWM模式1
		if (Duty_Cycle==0.0f)
			Timer->TIMx->CCR1=0;
		else if (Duty_Cycle==1.0f)
			Timer->TIMx->CCR1=Timer->TIMx->ARR+1;
		else
			Timer->TIMx->CCR1=_Timer_Cnt_Calculate(Timer->CNT_Length,Timer->Timing_Frequency,(1000000*Duty_Cycle)/Timer->PWM_Frequency);
		Timer->PWM_Duty_Cycle[0]=_Timer_Update_Time_Calculate(Timer->CNT_Length,Timer->Timing_Frequency,Timer->TIMx->CCR1)*Timer->PWM_Frequency/1000000.0f;
		Timer->PWM_Channel[0]=1;
	}
	else if (OCx==OC2)
	{
		Timer->TIMx->CCMR1|=1<<11;	//输出比较2预装载使能
		Timer->TIMx->CCMR1|=6<<12;	//输出比较2配置为PWM模式1
		if (Duty_Cycle==0.0f)
			Timer->TIMx->CCR2=0;
		else if (Duty_Cycle==1.0f)
			Timer->TIMx->CCR2=Timer->TIMx->ARR+1;
		else
			Timer->TIMx->CCR2=_Timer_Cnt_Calculate(Timer->CNT_Length,Timer->Timing_Frequency,(1000000*Duty_Cycle)/Timer->PWM_Frequency);
		Timer->PWM_Duty_Cycle[1]=_Timer_Update_Time_Calculate(Timer->CNT_Length,Timer->Timing_Frequency,Timer->TIMx->CCR2)*Timer->PWM_Frequency/1000000.0f;
		Timer->PWM_Channel[1]=1;
	}
	else if (OCx==OC3)
	{
		Timer->TIMx->CCMR2|=1<<3;	//输出比较3预装载使能
		Timer->TIMx->CCMR2|=6<<4;	//输出比较3配置为PWM模式1
		if (Duty_Cycle==0.0f)
			Timer->TIMx->CCR3=0;
		else if (Duty_Cycle==1.0f)
			Timer->TIMx->CCR3=Timer->TIMx->ARR+1;
		else
			Timer->TIMx->CCR3=_Timer_Cnt_Calculate(Timer->CNT_Length,Timer->Timing_Frequency,(1000000*Duty_Cycle)/Timer->PWM_Frequency);
		Timer->PWM_Duty_Cycle[2]=_Timer_Update_Time_Calculate(Timer->CNT_Length,Timer->Timing_Frequency,Timer->TIMx->CCR3)*Timer->PWM_Frequency/1000000.0f;
		Timer->PWM_Channel[2]=1;
	}
	else if (OCx==OC4)
	{
		Timer->TIMx->CCMR2|=1<<11;	//输出比较4预装载使能
		Timer->TIMx->CCMR2|=6<<12;	//输出比较4配置为PWM模式1
		if (Duty_Cycle==0.0f)
			Timer->TIMx->CCR4=0;
		else if (Duty_Cycle==1.0f)
			Timer->TIMx->CCR4=Timer->TIMx->ARR+1;
		else
			Timer->TIMx->CCR4=_Timer_Cnt_Calculate(Timer->CNT_Length,Timer->Timing_Frequency,(1000000*Duty_Cycle)/Timer->PWM_Frequency);
		Timer->PWM_Duty_Cycle[3]=_Timer_Update_Time_Calculate(Timer->CNT_Length,Timer->Timing_Frequency,Timer->TIMx->CCR4)*Timer->PWM_Frequency/1000000.0f;
		Timer->PWM_Channel[3]=1;
	}
	if (Update)
		Timer->TIMx->EGR|=1;	//产生一次更新事件
}

void Timer_InputCatch_Init(Timer_Info* Timer, u8 ICx, u8 Polar)
{
	if (ICx==IC1)
	{
		Timer->TIMx->CCMR1|=1;	//IC1映射到TI1
		Timer->TIMx->CCER&=~(5<<1);
		Timer->TIMx->CCER|=Polar;
	}
	else if (ICx==IC2)
	{
		Timer->TIMx->CCMR1|=1<<8;	//IC2映射到TI2
		Timer->TIMx->CCER&=~(5<<5);
		Timer->TIMx->CCER|=Polar<<4;
	}
	else if (ICx==IC3)
	{
		Timer->TIMx->CCMR2|=1;	//IC3映射到TI3
		Timer->TIMx->CCER&=~(5<<8);
		Timer->TIMx->CCER|=Polar<<7;
	}
	else if (ICx==IC4)
	{
		Timer->TIMx->CCMR2|=1<<8;	//IC4映射到TI4
		if (Polar==POLAR_POSITIVE)
			Timer->TIMx->CCER&=~(1<<13);
		else if (Polar==POLAR_NEGATIVE)
			Timer->TIMx->CCER|=1<<13;
		else if (Polar==POLAR_BOTH)
			Timer->TIMx->CCER|=1<<13;
	}
	Timer->TIMx->EGR|=1;	//产生一次更新事件
	Timer->TIMx->CR1&=~(1<<2);	
}

void Timer_InputCatch_Interrupt_Set(Timer_Info* Timer, u8 ICx, u8 Group, u8 Preemption_Priority, u8 Sub_Priority)
{
	Timer->TIMx->DIER|=1<<ICx;
	if (Timer->TIMx==TIM1)
		MY_NVIC_Init(Preemption_Priority,Sub_Priority,TIM1_CC_IRQn,Group);
	else if (Timer->TIMx==TIM8)
		MY_NVIC_Init(Preemption_Priority,Sub_Priority,TIM8_CC_IRQn,Group);
	else if (Timer->TIMx==TIM2)
		MY_NVIC_Init(Preemption_Priority,Sub_Priority,TIM2_IRQn,Group);
	else if (Timer->TIMx==TIM3)
		MY_NVIC_Init(Preemption_Priority,Sub_Priority,TIM3_IRQn,Group);
	else if (Timer->TIMx==TIM4)
		MY_NVIC_Init(Preemption_Priority,Sub_Priority,TIM4_IRQn,Group);
	else if (Timer->TIMx==TIM5)
		MY_NVIC_Init(Preemption_Priority,Sub_Priority,TIM5_IRQn,Group);
}

void Timer_InputCatch_Reload(Timer_Info* Timer, u8 ICx)
{
	Timer_Clear_CCxIF(Timer,ICx);
	Timer->TIMx->CNT=0;
}

#ifdef USE_FREQUENCY_MEASURE
void Timer_Frequency_Measure_Init(Timer_Info* Main_Timer, Timer_Info* Aux_Timer, u8 ICx)
{
	Timer_InputCatch_Init(Main_Timer,ICx,POLAR_POSITIVE);
	Timer_Timing_Init(Aux_Timer,1);
}

void Timer_Phase_Track_Init(Timer_Info* Timer, u8 TIx)
{
	Timer_ExternClock_Set(Timer,TIx,1);
	Timer_Enable(Timer);
}

void Timer_Phase_Synchronize(Timer_Info* Timer, u8 Period)
{
	Timer->TIMx->ARR=Period;
	Timer->TIMx->EGR|=1;	//产生一次更新事件
	Timer_Clear_UIF(Timer);
	while (Timer_Get_UIF(Timer));
}

u8 Timer_Get_Frequency(Timer_Info* Main_Timer, Timer_Info* Auxi_Timer, u8 ICx, u8 TIx, Frequency_Measure_Info* info)
{
	float save;
	if (info->Auto_Trigger==0)
	{
		if (info->Method==PERIOD_MEASURE)
		{
			save=_Timer_Measure_Period(Main_Timer,ICx,info->Trigger_Time,info->Min_Freq);
			if (save<info->Min_Freq)
				info->Time_Out=1;
			else
			{
				info->Time_Out=0;
				info->Freqency=save;
				info->Error=_Timer_Measure_Period_Error(info->Freqency,Main_Timer->Timing_Frequency);
			}
			info->Real_Method=PERIOD_MEASURE;
			return 0;
		}
		else if (info->Method==PULSE_COUNT)
		{
			save=_Timer_Pulse_Count(Main_Timer,Auxi_Timer,ICx,TIx,info->Trigger_Time,info->Sample_Time);
			if (save<info->Min_Freq)
				info->Time_Out=1;
			else
			{
				info->Time_Out=0;
				info->Freqency=save;
				info->Error=_Timer_Pulse_Count_Error(info->Freqency,info->Sample_Time);
			}
			info->Real_Method=PULSE_COUNT;
			return 0;
		}
		else if (info->Method==SELF_ADAPTE)
		{
			u32 SampTime;
			float save,error;
			
			save=_Timer_Measure_Period(Main_Timer,ICx,info->Trigger_Time,info->Min_Freq);
			if (save<0)
			{
				info->Time_Out=1;
				return 1;
			}
			if ((error=_Timer_Measure_Period_Error(save,Main_Timer->Timing_Frequency))>ACCURACY)
			{
				do
				{
					SampTime=1000.0f/(ACCURACY*save)+1;
					save=_Timer_Pulse_Count(Main_Timer,Auxi_Timer,ICx,TIx,info->Trigger_Time,SampTime);
					if (save<info->Min_Freq)
					{
						info->Sample_Time=SampTime;
						info->Time_Out=1;
						info->Real_Method=PULSE_COUNT;
						return 1;
					}
				}while ((error=_Timer_Pulse_Count_Error(save,SampTime))>ACCURACY);
				info->Time_Out=0;
				info->Sample_Time=SampTime;
				info->Real_Method=PULSE_COUNT;
			}
			else
				info->Real_Method=PERIOD_MEASURE;
			info->Freqency=save;
			info->Error=error;
			info->Time_Out=0;
			return 0;
		}
		else if (info->Method==MINIMUM_ERROR)
		{
			float freq1,freq2;
			float err1,err2;
			
			freq1=_Timer_Measure_Period(Main_Timer,ICx,info->Trigger_Time,info->Min_Freq);
			freq2=_Timer_Pulse_Count(Main_Timer,Auxi_Timer,ICx,TIx,info->Trigger_Time,info->Sample_Time);
			
			if ((freq1<info->Min_Freq)||(freq2<info->Min_Freq))
			{
				info->Time_Out=1;
				return 1;
			}
			
			err1=_Timer_Measure_Period_Error(freq1,Main_Timer->Timing_Frequency);
			err2=_Timer_Pulse_Count_Error(freq2,info->Sample_Time);
			
			if (err1>err2)
			{
				info->Time_Out=0;
				info->Freqency=freq2;
				info->Error=err2;
				info->Real_Method=PULSE_COUNT;
				return 0;
			}
			else
			{
				info->Time_Out=0;
				info->Freqency=freq1;
				info->Error=err1;
				info->Real_Method=PERIOD_MEASURE;
				return 0;
			}
		}
	}
	else
	{
		signed char i;
		u16 index;
		u32 save1,save2,Min_Freq_CNT_Value;
		float in[32];
		float out[9];
		
		Min_Freq_CNT_Value=Main_Timer->Timing_Frequency/info->Min_Freq;
		Timer_InputCatch_Reload(Main_Timer,ICx);
		Timer_InputCatch_Enable(Main_Timer,ICx);
		Timer_Enable(Main_Timer);
		while (Timer_Get_CCxIF(Main_Timer,ICx)!=1)
		{
			if (Main_Timer->TIMx->CNT>Min_Freq_CNT_Value)
			{
				info->Freqency=0;
				return 1;
			}
		}
		save1=Timer_Get_CatchValue(Main_Timer,ICx);
		for (i=0;i<16;i++)
		{
			while (Timer_Get_CCxIF(Main_Timer,ICx)!=1)
			{
				if (Main_Timer->TIMx->CNT-save1>Min_Freq_CNT_Value)
				{
					Timer_Disable(Main_Timer);
					info->Freqency=0;
					return 1;
				}
			}
			save2=Timer_Get_CatchValue(Main_Timer,ICx);
			in[i*2]=save2-save1;
			in[i*2+1]=0;
			save1=save2;
		}
		Timer_Disable(Main_Timer);
		
		for (i=0;i<16;i++)
			in[1]+=in[i*2];
		in[1]/=16;
		for (i=0;i<16;i++)
			in[i*2]-=in[1];
		in[1]=0;
		arm_cfft_f32(MathHelper_Get_arm_fft_struct(16),in,0,1);
		arm_cmplx_mag_f32(in,out,9);
		for (save1=0,i=1;i<9;i++)
			save1+=out[i];
		if (save1<30)
			info->Trigger_Time=1;
		else
		{
			MathHelper_FindMax_Float(&out[0],9,&index);
			if (index==0)
			{
				info->Trigger_Time=1;
			}
			else if (index==8)
			{
				in[0]=8-(float)out[7]/(out[7]+out[8]);
				in[0]=16/in[0];
				if (in[0]-(u16)in[0]>=0.5f)
					info->Trigger_Time=(u16)in[0]+1;
				else
					info->Trigger_Time=(u16)in[0];
			}
			else
			{
				if (out[index-1]>out[index+1])
					in[0]=index-(float)out[index-1]/(out[index]+out[index-1]);
				else if (out[index-1]<out[index+1])
					in[0]=index+(float)out[index+1]/(out[index]+out[index+1]);
				else
					in[0]=index;
				in[0]=16/in[0];
				if (in[0]-(u16)in[0]>=0.5f)
					info->Trigger_Time=(u16)in[0]+1;
				else
					info->Trigger_Time=(u16)in[0];
			}
		}
		
		info->Auto_Trigger=0;
		Timer_Get_Frequency(Main_Timer,Auxi_Timer,ICx,TIx,info);
		info->Auto_Trigger=1;
		
		return 0;
	}
	
	return 0;
}
#endif

#if !defined USE_STM32H7
void TIM1_UP_TIM10_IRQHandler(void)
{
	if (TIM1->SR&1)
	{
		Timer1_CallBackFunc();
		TIM1->SR&=~1;
	}
	else if (TIM10->SR&1)
	{
		Timer10_CallBackFunc();
		TIM10->SR&=~1;
	}
}
#else
void TIM1_UP_IRQHandler(void)
{
	if (TIM1->SR&1)
	{
		Timer1_CallBackFunc();
		TIM1->SR&=~1;
	}
}
#endif

void TIM2_IRQHandler(void)
{
	if ((TIM2->SR)&1)
	{
		Timer2_CallBackFunc();
		TIM2->SR&=~1;
	}
}

void TIM3_IRQHandler(void)
{
	if ((TIM3->SR)&1)
	{
		Timer3_CallBackFunc();
		TIM3->SR&=~1;
	}
}

void TIM4_IRQHandler(void)
{
	if ((TIM4->SR)&1)
	{
		Timer4_CallBackFunc();
		TIM4->SR&=~1;
	}
}

void TIM5_IRQHandler(void)
{
	if (TIM5->SR&1)
	{
		Timer5_CallBackFunc();
		TIM5->SR&=~1;
	}
}

void TIM6_DAC_IRQHandler(void)
{
	if (TIM6->SR&1)
	{
		Timer6_CallBackFunc();
		TIM6->SR&=~1;
	}
}

void TIM7_IRQHandler(void)
{
	if (TIM7->SR&1)
	{
		Timer7_CallBackFunc();
		TIM7->SR&=~1;
	}
}

void TIM8_UP_TIM13_IRQHandler(void)
{
	if (TIM8->SR&1)
	{
		Timer8_CallBackFunc();
		TIM8->SR&=~1;
	}
	else if (TIM13->SR&1)
	{
		Timer13_CallBackFunc();
		TIM13->SR&=~1;
	}
}

#if !defined USE_STM32H7
void TIM1_BRK_TIM9_IRQHandler(void)
{
	if (TIM9->SR&1)
	{
		Timer9_CallBackFunc();
		TIM9->SR&=~1;
	}
}

void TIM1_TRG_COM_TIM11_IRQHandler(void)
{
	if (TIM11->SR&1)
	{
		Timer11_CallBackFunc();
		TIM11->SR&=~1;
	}
}
#endif

void TIM8_BRK_TIM12_IRQHandler(void)
{
	if (TIM12->SR&1)
	{
		Timer12_CallBackFunc();
		TIM12->SR&=~1;
	}
}

void TIM8_TRG_COM_TIM14_IRQHandler(void)
{
	if (TIM14->SR&1)
	{
		Timer14_CallBackFunc();
		TIM14->SR&=~1;
	}
}





void Timer_Info_Print(USART_TypeDef* USART, Timer_Info* Timer)
{
	if (Timer->TIMx==TIM1)
		USART_Printf(USART,"Timer1:\n");
	else if (Timer->TIMx==TIM8)
		USART_Printf(USART,"Timer8:\n");
	else if (Timer->TIMx==TIM2)
		USART_Printf(USART,"Timer2:\n");
	else if (Timer->TIMx==TIM3)
		USART_Printf(USART,"Timer3:\n");
	else if (Timer->TIMx==TIM4)
		USART_Printf(USART,"Timer4:\n");
	else if (Timer->TIMx==TIM5)
		USART_Printf(USART,"Timer5:\n");
    #if !defined USE_STM32H7
	else if (Timer->TIMx==TIM9)
		USART_Printf(USART,"Timer9:\n");
	else if (Timer->TIMx==TIM10)
		USART_Printf(USART,"Timer10:\n");
	else if (Timer->TIMx==TIM11)
		USART_Printf(USART,"Timer11:\n");
    #endif
	else if (Timer->TIMx==TIM12)
		USART_Printf(USART,"Timer12:\n");
	else if (Timer->TIMx==TIM13)
		USART_Printf(USART,"Timer13:\n");
	else if (Timer->TIMx==TIM14)
		USART_Printf(USART,"Timer14:\n");
	else if (Timer->TIMx==TIM6)
		USART_Printf(USART,"Timer6:\n");
	else if (Timer->TIMx==TIM7)
		USART_Printf(USART,"Timer7:\n");
	
	USART_Printf(USART,"Timer_Type: ");
	if (Timer->Timer_Type==ADVANCED_TIMER)
		USART_Printf(USART,"Advanced Timer\n");
	else if ((Timer->Timer_Type==GENERAL_TIMER1)||(Timer->Timer_Type==GENERAL_TIMER2))
		USART_Printf(USART,"General Timer\n");
	else if (Timer->Timer_Type==BASIC_TIMER)
		USART_Printf(USART,"Basic Timer\n");
	
	USART_Printf(USART,"Count Register Length: %d\n",Timer->CNT_Length);
	USART_Printf(USART,"Bus Clock: %d\n",Timer->Clock);
	USART_Printf(USART,"Frequency Division: %d\n",Timer->Frequency_Div);
	USART_Printf(USART,"Timing Frequency: %d\n",Timer->Timing_Frequency);
	USART_Printf(USART,"Update Time: %.3f(us)\n",Timer->Timing_us);
	USART_Printf(USART,"PWM Frequency: %d\n",Timer->PWM_Frequency);
	USART_Printf(USART,"PWM1 Duty Cycle: %f\n",Timer->PWM_Duty_Cycle[0]);
	USART_Printf(USART,"PWM2 Duty Cycle: %f\n",Timer->PWM_Duty_Cycle[1]);
	USART_Printf(USART,"PWM3 Duty Cycle: %f\n",Timer->PWM_Duty_Cycle[2]);
	USART_Printf(USART,"PWM4 Duty Cycle: %f\n",Timer->PWM_Duty_Cycle[3]);
}
