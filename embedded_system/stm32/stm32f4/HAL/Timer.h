/************************************************************************************
*|--------------------------------STM32定时器操作库--------------------------------|*
*|---------------------------------@大四十七高材生---------------------------------|*
*|------------------------------------2021.1.19------------------------------------|*
*|包括了对STM32F4系列全部14个定时器的操作方法，包括精确定时、PWM输出等功能。-------|*
*************************************************************************************

STM32F4系列各定时器基本特性：
1MHz最大计时时间（不包括高级定时器的循环计数寄存器）

	┌────────┬──────┬──────┬──────┬──────────────────┬────────────┬────────────┐
	│ 定时器 │ 类型 │ 总线 │ 位数 │ 1MHz最大计时时间 │ 外部通道数 │ 外部时钟源 │
	├────────┼──────┼──────┼──────┼──────────────────┼────────────┼────────────┤
	│  TIM1  │ 高级 │ APB2 │  16  │		 65.5ms		 │		4	  │	   允许    │
	├────────┼──────┼──────┼──────┼──────────────────┼────────────┼────────────┤
	│  TIM2	 │ 通用 │ APB1 │  32  │		 71.6min	 │      4	  │	   允许    │
	├────────┼──────┼──────┼──────┼──────────────────┼────────────┼────────────┤
	│  TIM3  │ 通用 │ APB1 │  16  │	 	 65.5ms		 │  	4	  │	   允许    │
	├────────┼──────┼──────┼──────┼──────────────────┼────────────┼────────────┤
	│  TIM4  │ 通用 │ APB1 │  16  │	 	 65.5ms		 │  	4	  │	   允许    │
	├────────┼──────┼──────┼──────┼──────────────────┼────────────┼────────────┤
	│  TIM5  │ 通用 │ APB1 │  32  │	 	 71.6min	 │  	4	  │	   允许    │
	├────────┼──────┼──────┼──────┼──────────────────┼────────────┼────────────┤
	│  TIM6  │ 基本 │ APB1 │  16  │	 	 65.5ms		 │  	0	  │	  不允许   │
	├────────┼──────┼──────┼──────┼──────────────────┼────────────┼────────────┤
	│  TIM7  │ 基本 │ APB1 │  16  │	 	 65.5ms		 │  	0	  │	  不允许   │
	├────────┼──────┼──────┼──────┼──────────────────┼────────────┼────────────┤
	│  TIM8  │ 高级 │ APB2 │  16  │	 	 65.5ms		 │  	4	  │	   允许    │
	├────────┼──────┼──────┼──────┼──────────────────┼────────────┼────────────┤
	│  TIM9  │ 通用 │ APB2 │  16  │	 	 65.5ms		 │  	2	  │	   允许    │
	├────────┼──────┼──────┼──────┼──────────────────┼────────────┼────────────┤
	│  TIM10 │ 通用 │ APB2 │  16  │	 	 65.5ms		 │  	1	  │	  不允许   │
	├────────┼──────┼──────┼──────┼──────────────────┼────────────┼────────────┤
	│  TIM11 │ 通用 │ APB2 │  16  │	 	 65.5ms		 │  	1	  │	  不允许   │
	├────────┼──────┼──────┼──────┼──────────────────┼────────────┼────────────┤
	│  TIM12 │ 通用 │ APB1 │  16  │	 	 65.5ms		 │  	2	  │	   允许    │
	├────────┼──────┼──────┼──────┼──────────────────┼────────────┼────────────┤
	│  TIM13 │ 通用 │ APB1 │  16  │	 	 65.5ms		 │  	1	  │	  不允许   │
	├────────┼──────┼──────┼──────┼──────────────────┼────────────┼────────────┤
	│  TIM14 │ 通用 │ APB1 │  16  │	 	 65.5ms		 │  	1	  │	  不允许   │
	└────────┴──────┴──────┴──────┴──────────────────┴────────────┴────────────┘

*************************************************************************************
*|与定时器相关的中断号-------------------------------------------------------------|*
*||定时器1/9：TIM1_BRK_TIM9_IRQn---------------------------------------------------|*
*||定时器1/10：TIM1_UP_TIM10_IRQn--------------------------------------------------|*
*||定时器1/11：TIM1_TRG_COM_TIM11_IRQn---------------------------------------------|*
*||定时器1：TIM1_CC_IRQn-----------------------------------------------------------|*
*||定时器8/12：TIM8_BRK_TIM12_IRQn-------------------------------------------------|*
*||定时器8/13：TIM8_UP_TIM13_IRQn--------------------------------------------------|*
*||定时器8/14：TIM8_TRG_COM_TIM14_IRQn---------------------------------------------|*
*||定时器8：TIM8_CC_IRQn-----------------------------------------------------------|*
*||定时器2：TIM2_IRQn--------------------------------------------------------------|*
*||定时器3：TIM3_IRQn--------------------------------------------------------------|*
*||定时器4：TIM4_IRQn--------------------------------------------------------------|*
*||定时器5：TIM5_IRQn--------------------------------------------------------------|*
*||定时器6：TIM6_DAC_IRQn----------------------------------------------------------|*
*||定时器7：TIM7_IRQn--------------------------------------------------------------|*
*************************************************************************************/

#ifndef _TIME_H_
#define _TIME_H_
#include "sys.h"

//是否使用频率测量功能
//#define USE_FREQUENCY_MEASURE

/*参数宏定义*/
/*#if defined STM32F40_41xxx || defined STM32F407xx
#define APB1_MAX_CLOCK 84000000
#define APB2_MAX_CLOCK 168000000
#elif defined STM32F429xx
#define APB1_MAX_CLOCK 90000000.0f
#define APB2_MAX_CLOCK 180000000.0f
#endif*/

#define INSIDE_CLOCK_SOURCE 0	//内部时钟源
#define EXTERN_CLOCK_SOURCE 1	//外部时钟源

#define POLAR_POSITIVE (0<<1)	//上升沿有效
#define POLAR_NEGATIVE (1<<1)	//下降沿有效
#define POLAR_BOTH     (5<<1)	//上升与下降沿都有效

#define TI1 1	//TI1通道（触发输入）
#define TI2 2	//TI2通道（触发输入）
#define TI3 3	//TI3通道（触发输入）
#define TI4 4	//TI4通道（触发输入）

#define OC1 (1<<0)	//输出CH1
#define OC2 (1<<4)	//输出CH2
#define OC3 (1<<8)	//输出CH3
#define OC4 (1<<12)	//输出CH4

#define IC1 1	//输入CH1
#define IC2 2	//输入CH2
#define IC3 3	//输入CH3
#define IC4 4	//输入CH4

#define TRIGGER_OUTSIDE 	0	//外部触发
#define TRIGGER_SOFTWARE	1	//内部软件触发

#define PERIOD_MEASURE 	1	//测量周期
#define PULSE_COUNT	   	2	//测量频率
#define SELF_ADAPTE		3	//自适应
#define MINIMUM_ERROR	4	//最小误差

#define DO_UPDATE		1
#define NO_UPDATE		0

//定时器信息结构体，储存了一些定时器运行参数
typedef struct	
{
	TIM_TypeDef* TIMx;		//定时器寄存器组对应的结构体
	u8 Timer_Type;			//定时器类型（ADVANCED_TIMER/GENERAL_TIMER1/GENERAL_TIMER2/BASIC_TIMER）
	u8 CNT_Length;			//位数
	u8 One_Pulse;			//是否允许单脉冲模式
	u32 Clock;				//APBx总线时钟频率
	u16 Frequency_Div;		//分频系数（TIMx->PSC+1）
	u32 Timing_Frequency;	//实际计数频率（仅限内部时钟源）
	float Timing_us;		//预设计数事件，单位：us（工作在计数模式时有意义）
	u32 PWM_Frequency;		//PWM输出频率（工作在PWM输出模式时有意义）
	u8 PWM_Channel[4];
	float PWM_Duty_Cycle[4];//PWM输出占空比（工作在PWM输出模式时有意义）
} Timer_Info;

//频率测量结构体，存储了进行频率测量时需要的参数
typedef struct 
{
	u8 Method;		//测量模式（PERIOD_MEASURE/PULSE_COUNT/SELF_ADAPTE/MINIMUM_ERROR）
	u8 Real_Method;	//实际测量模式（在自适应或者最小误差模式下工作模式会发生改变）
	u8 Trigger_Time;//触发次数
	u8 Auto_Trigger;//是否自动触发
	u8 Time_Out;	//测量频率小于Min_Freq时置1
	u16 Sample_Time;//PULSE_COUNT模式下的计数时间
	float Min_Freq;	//允许的最小频率，如果测量的频率小于该值则Freqency=0
	float Freqency;	//测量的结果存入该值
	float Error;	//估计误差（它代表了在当前测量条件下可能出现的相对误差绝对值，该值只是一个最保守估计）
} Frequency_Measure_Info;
	
//开启定时器，x：定时器
#define Timer_Enable(x)    		(x)->TIMx->CR1|=1	
//关闭定时器，x：定时器
#define Timer_Disable(x)   		(x)->TIMx->CR1&=~1 

//清除UIF位（更新中断标志位），x：定时器
#define Timer_Clear_UIF(x) 		(x)->TIMx->SR&=~1	
//清除CCxIF位，x：定时器；y：输入通道
#define Timer_Clear_CCxIF(x,y)	(x)->TIMx->SR&=~((1<<y)+(1<<(y+8)))

//PWM输出使能，x：定时器；y：输出通道
#define Timer_PWMOutput_Enable(x,y) 	(x)->TIMx->CCER|=y
//PWM输出失能，x：定时器；y：输出通道
#define Timer_PWMOutput_Disable(x,y) 	(x)->TIMx->CCER&=~y

//输入捕获使能，x：定时器；y：输入通道
#define Timer_InputCatch_Enable(x,y)	(x)->TIMx->CCER|=1<<(y-1)*4
//输入捕获失能，x：定时器；y：输入通道
#define Timer_InputCatch_Disable(x,y)	(x)->TIMx->CCER&=~(1<<(y-1)*4)

//获取UIF位，x：定时器
#define Timer_Get_UIF(x)			((x)->TIMx->SR&1)
//获取CCxIF位（输入捕获标志位），x：定时器；y：输入通道
#define Timer_Get_CCxIF(x,y)		(((x)->TIMx->SR>>y)&1)
//获取输入捕获值，x：定时器；y：输入通道
#define Timer_Get_CatchValue(x,y)	*((u32*)((u32)((x)->TIMx)+0x34+(y-1)*4))

//相位同步，x：定时器；y：触发次数
#define Timer_Phase_Sync(x,y)	while ((x)->TIMx->CNT==y);while ((x)->TIMx->CNT!=y)

typedef void (*Timer_Update_CallBackFunc_Type)(void);

/****************************************************************
*功能：定时器复位												*
*参数：1.Timer：定时器对应的寄存器组结构体						*
*	   2.Info_Struct：将要复位的定时器信息结构体				*
*说明：该函数会复位定时器的寄存器同时初始化定时器信息结构体		*
*****************************************************************/
void Timer_Reset(TIM_TypeDef* Timer, Timer_Info* Info_Struct);

/********************************************************************************
*功能：定时器全局设置，定时器工作的全局参数										*
*参数：1.Timer：定时器信息结构体												*
*	   2.Clock_Source：时钟源（INSIDE_CLOCK_SOURCE/EXTERN_CLOCK_SOURCE）		*
*	   3.APBx_Clock：定时器对应的总线时钟频率									*
*	   4.Timing_Frequency：要设定的计数频率（只在内部时钟源有用）				*
*	   5.Trigger_Mode：是否使用触发模式，即在输入信号上升沿时开启定时器			*
*********************************************************************************/
void Timer_Global_Set(Timer_Info* Timer, u8 Clock_Source, u32 APBx_Clock, u32 Timing_Frequency, u8 Trigger_Mode);

/********************************************************************************
*功能：定时器外部时钟配置，在上升沿定时器计数									*
*参数：1.Timer：定时器信息结构体												*
*	   2.TIx：输入时钟的通道（只有TI1，TI2两个），其中TI1映射到IC1，TI2映射到IC2*
*	   3.Frequency_Div：分频系数												*
*********************************************************************************/
void Timer_ExternClock_Set(Timer_Info* Timer, u8 TIx, u16 Frequency_Div);

/************************************************************************************
*功能：定时器触发模式设置															*
*参数：1.Timer：定时器信息结构体													*
*	   2.TIx：触发信号的输入通道（只有TI1，TI2两个），其中TI1映射到IC1，TI2映射到IC2*
*	   3.Polar：触发信号极性，上升沿（0），下降沿（1），上升或下降（2）				*
*说明：配置完成后定时器将在指定通道出现上升/下降沿后开启							*
*************************************************************************************/
void Timer_TriggerMode_Set(Timer_Info* Timer, u8 TIx, u8 Polar);

/****************************************************************
*功能：自由计数模式初始化										*
*参数：1.Timer：定时器信息结构体								*
*说明：开启后定时器将自由向上计数，除非手动关闭定时器不会停止	*
*****************************************************************/
void Timer_Free_Run_Init(Timer_Info* Timer);

/************************************
*功能：自由计数模式复位				*
*参数：1.Timer：定时器信息结构体	*
*说明：计数值寄存器置零，并生成更新	*
*************************************/
void Timer_Free_Run_Reload(Timer_Info* Timer);

/****************************************
*功能：计时模式中断初始化				*
*参数：1.Timer：定时器信息结构体		*
*****************************************/
void Timer_Timing_Init(Timer_Info* Timer, u8 One_Pulse);

/****************************************************
*功能：计时模式中断初始化							*
*参数：1.Timer：定时器信息结构体					*
*	   2.Group：中断组号							*
*	   3.Preemption_Priority：抢占优先级			*
*	   4.Sub_Priority：响应优先级					*
*说明：配置溢出中断									*
*****************************************************/
void Timer_Timing_Interrupt_Set(Timer_Info* Timer, u8 Group, u8 Preemption_Priority, u8 Sub_Priority, 
								Timer_Update_CallBackFunc_Type CallBackFunc);

/************************************
*功能：计时器模式下，计时时间长短	*
*参数：1.Timer：定时器信息结构体	*
*	   2.Time：计时时间（us）		*
*************************************/
u8 Timer_Timing_SetTime_us(Timer_Info* Timer, float Time);

/************************************
*功能：计时器模式下，计时时间长短	*
*参数：1.Timer：定时器信息结构体	*
*	   2.Time：计时时间（ms）		*
*************************************/
u8 Timer_Timing_SetTime_ms(Timer_Info* Timer, u32 Time);

/************************************
*功能：计时器模式下，计时时间长短	*
*参数：1.Timer：定时器信息结构体	*
*	   2.Time：计时时间（s）		*
*************************************/
u8 Timer_Timing_SetTime_s(Timer_Info* Timer, u16 Time);

/************************************
*功能：计时器模式复位				*
*参数：1.Timer：定时器信息结构体	*
*************************************/
void Timer_Timing_Reload(Timer_Info* Timer);

/************************************
*功能：PWM输出模式初始化			*
*参数：1.Timer；定时器信息结构体	*
*	   2.PWM_Frequency：PWM频率		*
*************************************/
void Timer_PWMOutput_Init(Timer_Info* Timer, u32 PWM_Frequency);

void Timer_PWMOutput_Frequency_Set(Timer_Info* Timer, u32 PWM_Frequency, u8 Update);

/********************************************
*功能：设置某个输出通道的占空比				*
*参数：1.Timer；定时器信息结构体			*
*	   2.Duty_Cycle：要设置的占空比			*
*	   3.OCx：输出通道						*
*********************************************/	
void Timer_PWMOutput_Wave_Set(Timer_Info* Timer, float Duty_Cycle, u16 OCx, u8 Update);

/********************************************
*功能：输入捕获模式初始化					*
*参数：1.Timer；定时器信息结构体			*
*	   2.ICx：输入通道(IC1~4)				*
*	   3.Polar：信号极性					*
*********************************************/
void Timer_InputCatch_Init(Timer_Info* Timer, u8 ICx, u8 Polar);

/********************************************
*功能：输入捕获中断初始化					*
*参数：1.Timer；定时器信息结构体			*
*	   2.ICx：输入通道						*
*	   3.Group：中断组数					*
*	   4.Preemption_Priority：抢占优先级	*
*	   5.Sub_Priority：响应优先级			*
*********************************************/
void Timer_InputCatch_Interrupt_Set(Timer_Info* Timer, u8 ICx, u8 Group, u8 Preemption_Priority, u8 Sub_Priority);

/********************************************
*功能：输入捕获复位							*
*参数：1.Timer；定时器信息结构体			*
*	   2.ICx：输入通道						*
*********************************************/
void Timer_InputCatch_Reload(Timer_Info* Timer, u8 ICx);

//频率测量功能相关函数
#ifdef USE_FREQUENCY_MEASURE
/*******************************************************************************************
*|■ 频率测量功能介绍----------------------------------------------------------------------|*
*|├─■ 功能--------------------------------------------------------------------------------|*
*|│	│ 	测量一个周期信号的频率，要求该信号必须是数字的，上升沿或下降沿要足够陡峭。--------|*
*|│ │ 	测量所需的参数（如测量模式等）以及测量结果都被存放在频率测量信息结构体中（--------|*
*|│ │ 	Frequency_Measure_Info）。每进行一个测量计划都要创建一个该结构体。----------------|*
*|│ │ 	每进行一次测量（调用Timer_Get_Frequency函数）后，测量结果和估计误差也被存---------|*
*|│ └───放在频率测量信息结构体中。--------------------------------------------------------|*
*|│---------------------------------------------------------------------------------------|*
*|├─■ 定时器使用：进行频率测量至少要使用1个定时器，但是使用完整的测量功能必须需要2个------|*
*|│	└───定时器。第一个称为主定时器，第二个称为辅助定时器。--------------------------------|*
*|│---------------------------------------------------------------------------------------|*
*|├─■ 触发次数：触发次数由频率测量信息结构体中的Trigger_Time定义。------------------------|*
*|│	│	它代表要经过几次触发才能完全测量信号的一个完整周期。只经过一次触发就能完全--------|*
*|│	│	测量一个周期的信号被称为简单信号，正弦波、三级波、方波都属于简单信号。------------|*
*|│ │	使能自动触发（Auto_Trigger=1）以后，每次进行测量时，系统都会自动判断当前信--------|*
*|│ └───号的触发次数。--------------------------------------------------------------------|*
*|│---------------------------------------------------------------------------------------|*
*|├─■ 测量模式----------------------------------------------------------------------------|*
*|│ ├─■ 1.PERIOD_MEASURE（周期测量）：当使用该模式时，主定时器配置为输入捕获模式，辅助----|*
*|│ │ │		定时器被配置为自由计数模式。主定时器通过计算相邻数个上升/下降沿的时间间隔来获-|*
*|│ │ │		取信号周期，进而获得信号频率。该模式下，如果测量频率小于设定的最小频率，那么--|*
*|│ │ └─────结果直接是0。-----------------------------------------------------------------|*
*|│ │-------------------------------------------------------------------------------------|*
*|│ ├─■ 2.PULSE_COUNT（脉冲计数）：当使用该模式时，主定时器配置为外部时钟源自由计数模式，-|*
*|│ │ │		辅助定时器配置为计时模式。通过测量一定时间内通过的脉冲数，在得知触发次数的情--|*
*|│ │ └─────况下就能求出信号频率。该模式下，设定的最小频率不起作用。----------------------|*
*|│ │-------------------------------------------------------------------------------------|*
*|│ ├─■ 3.SELF_ADAPTE（自适应）：当使用该模式时，系统先使用“周期测量”的方法进行测量，再---|*
*|│ │ │		评估误差，如果误差大于最大可接受误差则使用“脉冲计数”法继续测量，并改变测量----|*
*|│ │ │		时间，直到估计误差小于最大可接受误差。理论上来说该方法得到的误差总是小于------|*
*|│ │ └─────最大可接受误差。--------------------------------------------------------------|*
*|│ │-------------------------------------------------------------------------------------|*
*|│ └─■ 4.MINIMUM_ERROR（最小误差）：当使用该模式时，系统分别用“周期测量”和“脉冲计数”-----|*
*|│	  └─────两种方法测量，然后比较两者误差，最后输出最小误差的数据。----------------------|*
*|│---------------------------------------------------------------------------------------|*
*|├─■ 误差估计公式------------------------------------------------------------------------|*
*|│ ├─■ 周期测量误差估计：e=f/(tf-f)，（f：测量频率，tf：主定时器计数频率）---------------|*
*|│ │-------------------------------------------------------------------------------------|*
*|│ └─■ 脉冲计数误差估计：e=t*f/1000-1，（t：测量时间（单位毫秒），f：测量频率）----------|*
*|│---------------------------------------------------------------------------------------|*
*|└─■ 测量端口配置：主定时器至少需要一个输入捕获端口，如果要使用脉冲计数，则还需要一个----|*
*|	└──────────外部时钟输入端。-----------------------------------------------------------|*
********************************************************************************************/

/********************************************
*功能：频率测量初始化						*
*参数：1.Main_Timer：主定时器				*
*	   2.Aux_Timer：辅助定时器				*
*	   3.ICx：主定时器输入捕获通道			*
*********************************************/
void Timer_Frequency_Measure_Init(Timer_Info* Main_Timer, Timer_Info* Aux_Timer, u8 ICx);

/********************************************
*功能：相位跟踪初始化						*
*参数：1.Timer；定时器信息结构体			*
*********************************************/
void Timer_Phase_Track_Init(Timer_Info* Timer, u8 TIx);

void Timer_Phase_Synchronize(Timer_Info* Timer, u8 Period);

/********************************************
*功能：获取频率								*
*参数：1.Main_Timer：主定时器				*
*	   2.Auxi_Timer：辅助定时器				*
*	   3.ICx：输入捕获通道					*
*	   4.TIx：外部时钟通道					*
*	   5.info：频率测量信息结构体			*
*********************************************/
u8 Timer_Get_Frequency(Timer_Info* Main_Timer, Timer_Info* Auxi_Timer, u8 ICx, u8 TIx, Frequency_Measure_Info* info);
#endif

/********************************************
*功能：用串口打印定时器信息结构体的内容		*
*参数：1.USART：串口寄存器结构体			*
*	   2.Timer：定时器信息结构体			*
*********************************************/
void Timer_Info_Print(USART_TypeDef* USART, Timer_Info* Timer);

#endif

