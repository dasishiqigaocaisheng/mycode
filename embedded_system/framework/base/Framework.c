#include "Framework.h"

#if SYSTEM_TIMER==1





#define SYSTICK_1US_VALUE   (SYSTEM_TIMER_CLOCK/1000000.0f)
#define SYSTICK_MAX_US      (16777216/SYSTICK_1US_VALUE-1)

#define SYSTICK_1MS_VALUE   (SYSTEM_TIMER_CLOCK/1000.0f)
#define SYSTICK_MAX_MS      (16777216/SYSTICK_1MS_VALUE)
                            
#define SystemTimer_Reset()                 SysTick->CTRL&=~1; \
                                            SysTick->VAL=0
                                            
#define SystemTimer_Start()                 SysTick->CTRL|=1

//�����start��now������tick��
#define SystemTimer_Get_Time(start,now)     (now<start?(start)-(now):(start)+(0xffffff-(now)))         

//����now����tick���ֵ
#define SystemTimer_After_Time(now,tick)    (now>tick?(now)-(tick):0xffffff-((tick)-(now)))

volatile status_flag vtimer_running=Disable;

typedef struct 
{
    status_flag Used;           //�Ƿ��Ѿ���ע��
    uint32_t Ticks;             //��ʱ��Ӧ��tick��
    uint32_t LastCheckValue;    //��һ�μ��ʱ�ļ�����ֵ
    uint32_t PauseValue;        //��ͣʱ�ļ�����ֵ
} vtimer_info;

vtimer_info vtimers[VTIMER_MAX_NUM];

void SystemTimer_Init(void)
{
    SysTick->LOAD=0xffffff;
    SysTick->CTRL|=SYSTICK_CLOCK_SOURCE<<2;
    SysTick->CTRL|=1;
    
    for (int i=0;i<VTIMER_MAX_NUM;i++)
        vtimers[i].Used=Disable;
}

void SystemTimer_Delay_Us(float time)
{
    uint32_t ticks;
	uint32_t told,tnow,tcnt=0;
	uint32_t reload=SysTick->LOAD;			//LOAD��ֵ	    
	ticks=time*SYSTICK_1US_VALUE; 			//��Ҫ�Ľ����� 
    SystemTimer_Start();
	told=SysTick->VAL;        				//�ս���ʱ�ļ�����ֵ
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)
                tcnt+=told-tnow;	//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
			else 
                tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)
                break;			//ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
		}  
	}
}

void SystemTimer_Delay_Ms(uint32_t time)
{
    uint32_t i;
    for (i=0;i<time;i++)
        SystemTimer_Delay_Us(1000);
}

void SystemTimer_Delay_S(uint32_t time)
{
    SystemTimer_Delay_Ms(time*1000);
}

//���ⶨʱ��ע��
vtimer_handler SystemTimer_VTimer_Regist(uint32_t time)
{
    int i;
    
    //�ҵ���һ�����е����ⶨʱ��
    for (i=0;i<VTIMER_MAX_NUM;i++)
    {
        if (!vtimers[i].Used)
            break;
    }
    
    //���û���ҵ�
    if (i==VTIMER_MAX_NUM)
        return -1;
    else
    {
        uint32_t recent_value=SysTick->VAL;
        uint32_t ticks=SYSTICK_1US_VALUE*time;
        
        vtimers[i].Ticks=ticks;
        vtimers[i].LastCheckValue=recent_value;
        vtimers[i].Used=Enable;
    }
    return i;
}

//���ⶨʱ�����Ķ�ʱʱ��
void SystemTimer_VTimer_ChangeTime(vtimer_handler vt, uint32_t time)
{
    uint32_t recent_value=SysTick->VAL;
    uint32_t ticks=SYSTICK_1US_VALUE*time;
    
    vtimers[vt].Ticks=ticks;
    vtimers[vt].LastCheckValue=recent_value;
}

//���ⶨʱ������ʱ���
status_flag SystemTimer_VTimer_Check(vtimer_handler vt)
{
    if (vtimer_running)
    {
        uint32_t recent_value=SysTick->VAL;
        uint32_t passed_time=SystemTimer_Get_Time(vtimers[vt].LastCheckValue,recent_value);
        
        if (passed_time>=vtimers[vt].Ticks)
        {
            vtimers[vt].LastCheckValue=recent_value;
            return Enable;
        }
        else
            return Disable;
    }
    return Disable;
}

void SystemTimer_VTimer_Start(void)
{
    uint32_t recent_value=SysTick->VAL;
    
    for (int i=0;i<VTIMER_MAX_NUM;i++)
    {
        if (vtimers[i].Used)
            vtimers[i].LastCheckValue=recent_value;
    }
    vtimer_running=Enable;
}

void SystemTimer_VTimer_Pause(void)
{
    uint32_t recent_value=SysTick->VAL;
    
    for (int i=0;i<VTIMER_MAX_NUM;i++)
    {
        if (vtimers[i].Used)
            vtimers[i].PauseValue=recent_value;
    }
    vtimer_running=Disable;
}

void SystemTimer_VTimer_Resume(void)
{
    uint32_t recent_value=SysTick->VAL;
    uint32_t pass_time;
    
    for (int i=0;i<VTIMER_MAX_NUM;i++)
    {
        pass_time=SystemTimer_Get_Time(vtimers[i].LastCheckValue,vtimers[i].PauseValue);
        if (vtimers[i].Used)
            vtimers[i].LastCheckValue=SystemTimer_After_Time(recent_value,pass_time);
    }
    vtimer_running=Enable;
}

void SystemTimer_Timing_Ready(float time_us)
{
    SystemTimer_Reset();
    SysTick->LOAD=SYSTICK_1US_VALUE*time_us;
    SysTick->VAL=0;
}

#elif SYSTEM_TIMER==2

#endif
