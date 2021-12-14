#include "Encoder.h"
#include "STM32Timer.h"



int encoder_save=0;

uint8_t Encoder_Check(void)
{
    volatile int temp=0;
    
    temp=((int)STM32Timer_CNT_R(TIM2))-encoder_save;
    temp/=2;
    encoder_save+=temp*2;
    if (temp>0)
        return ENCODER_UP;
    else if (temp<0)
        return ENCODER_DOWN;
    return ENCODER_NOCHANGE;
}
