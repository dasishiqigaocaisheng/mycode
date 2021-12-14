#ifndef _STM32TIMER_H_
#define _STM32TIMER_H_

#include "sys.h"

#define ADVANCED_TIMER  0
#define GENERAL_TIMER   1
#define BASIC_TIMER     2

#define APB1TIMER_MAX_CLOCK (APB1_MAX_CLOCK<<1)
#define APB2TIMER_MAX_CLOCK (APB2_MAX_CLOCK<<1)





/***********************************************************************/
/*                                                                     */
/*                              CR1¼Ä´æÆ÷                              */
/*                                                                     */
/***********************************************************************/
/*****************************ÅäÖÃ²ÎÊý¶¨Òå******************************/
#define CR1_ClockDivison_None  0
#define CR1_ClockDivison_2     1
#define CR1_ClockDivison_4     2

#define CR1_CenterAlign_OneSide 0
#define CR1_CenterAlign_Mid1    1
#define CR1_CenterAlign_Mid2    2
#define CR1_CenterAlign_Mid3    3

#define CR1_CountDirection_Up   0
#define CR1_CountDirection_Down 1

#define CR1_UpdateReuqest_All   0
#define CR1_UpdateRequest_OnlyOverflow  1
/******************************¼Ä´æÆ÷Ð´²Ù×÷*****************************/
#define STM32Timer_CR1_UIFRemap_W(x,y)              BitClearSet((x)->CR1,11,Mask1Bit,y)
#define STM32Timer_CR1_ClockDivision_W(x,y)         BitClearSet((x)->CR1,8,Mask2Bit,y)
#define STM32Timer_CR1_ARRAutoReload_W(x,y)         BitClearSet((x)->CR1,7,Mask1Bit,y)
#define STM32Timer_CR1_CenterAlignMode_W(x,y)       BitClearSet((x)->CR1,5,Mask2Bit,y)
#define STM32Timer_CR1_Direction_W(x,y)             BitClearSet((x)->CR1,4,Mask1Bit,y)
#define STM32Timer_CR1_OnePulse_W(x,y)              BitClearSet((x)->CR1,3,Mask1Bit,y)
#define STM32Timer_CR1_UpdateRequest_W(x,y)         BitClearSet((x)->CR1,1,Mask1Bit,y)
#define STM32Timer_CR1_CounterEnable_W(x,y)         BitClearSet((x)->CR1,0,Mask1Bit,y)
/******************************¼Ä´æÆ÷¶Á²Ù×÷*****************************/
#define STM32Timer_CR1_UIFRemap_R(x)                BitRead((x)->CR1,11,Mask1Bit)
#define STM32Timer_CR1_ClockDivision_R(x)           BitRead((x)->CR1,8,Mask2Bit)
#define STM32Timer_CR1_ARRAutoReload_R(x)           BitRead((x)->CR1,7,Mask1Bit)
#define STM32Timer_CR1_CenterAlignMode_R(x)         BitRead((x)->CR1,5,Mask2Bit)
#define STM32Timer_CR1_Direction_R(x)               BitRead((x)->CR1,4,Mask1Bit)
#define STM32Timer_CR1_OnePulse_R(x)                BitRead((x)->CR1,3,Mask1Bit)
#define STM32Timer_CR1_UpdateDisable_R(x)           BitRead((x)->CR1,1,Mask1Bit)
#define STM32Timer_CR1_CounterEnable_R(x)           BitRead((x)->CR1,0,Mask1Bit)





/***********************************************************************/
/*                                                                     */
/*                              CR2¼Ä´æÆ÷                              */
/*                                                                     */
/***********************************************************************/
/*****************************ÅäÖÃ²ÎÊý¶¨Òå******************************/
#define CR2_MasterMode_Reset    0
#define CR2_MasterMode_Enable   1
#define CR2_MasterMode_Update   2
#define CR2_MasterMode_CC1IF    3
#define CR2_MasterMode_OC1REF   4
#define CR2_MasterMode_OC2REF   5
#define CR2_MasterMode_OC3REF   6
#define CR2_MasterMode_OC4REF   7
#define CR2_MasterMode_OC5REF   8
#define CR2_MasterMode_OC6REF   9
#define CR2_MasterMode_OC4REFRiseFall  10
#define CR2_MasterMode_OC6REFRiseFall   11
#define CR2_MasterMode_OC4REFOC6REFRise      12
#define CR2_MasterMode_OC4REFRiseOC6REFFall 13
#define CR2_MasterMode_OC5REFOC6REFRise     14
#define CR2_MasterMode_OC5REFRiseOC6REFFall 15

#define CR2_TI1_CH1             0
#define CR2_TI1_CH1xorCH2xorCH3 1

#define CR2_CaptureEvent_DMA         0
#define CR2_CompareEvent_DMA         1
/****************************¼Ä´æÆ÷Ð´²Ù×÷*****************************/
#define STM32Timer_CR2_MasterMode2_W(x,y)       BitClearSet((x)->CR2,20,Mask4Bit,y)
#define STM32Timer_CR2_TI1Select_W(x,y)         BitClearSet((x)->CR2,7,Mask1Bit,y)
#define STM32Timer_CR2_MasterMode1_W(x,y)       BitClearSet((x)->CR2,4,Mask3Bit,y)
#define STM32Timer_CR2_CaptureCompareDMA_W(x,y) BitClearSet((x)->CR2,3,Mask1Bit,y)
/****************************¼Ä´æÆ÷¶Á²Ù×÷*****************************/
#define STM32Timer_CR2_MasterMode2_R(x)         BitRead((x)->CR2,20,Mask4Bit)
#define STM32Timer_CR2_TI1Select_R(x)           BitRead((x)->CR2,7,Mask1Bit)
#define STM32Timer_CR2_MasterMode1_R(x)         BitRead((x)->CR2,4,Mask3Bit)
#define STM32Timer_CR2_CaptureCompareDMA_R(x)   BitRead((x)->CR2,3,Mask1Bit)







/***********************************************************************/
/*                                                                     */
/*                              SMCR¼Ä´æÆ÷                             */
/*                                                                     */
/***********************************************************************/
/*****************************ÅäÖÃ²ÎÊý¶¨Òå******************************/
#define SMCR_TriggerSelect_ITR0   0
#define SMCR_TriggerSelect_ITR1   1
#define SMCR_TriggerSelect_ITR2   2
#define SMCR_TriggerSelect_ITR3   3
#define SMCR_TriggerSelect_TI1F_ED   4
#define SMCR_TriggerSelect_TI1FP1    5
#define SMCR_TriggerSelect_TI2FP2    6
#define SMCR_TriggerSelect_ETRF      7

#define SMCR_SlaveMode_Disable       0
#define SMCR_SlaveMode_Encoder1      1
#define SMCR_SlaveMode_Encoder2      2
#define SMCR_SlaveMode_Encoder3      3
#define SMCR_SlaveMode_Reset         4
#define SMCR_SlaveMode_Gate          5
#define SMCR_SlaveMode_Trigger       6
#define SMCR_SlaveMode_ExternClock1  7
#define SMCR_SlaveMode_Combine       8

#define SMCR_ExternalTriggerPolarPositive  0
#define SMCR_ExternalTriggerPolarNegative  1

#define SMCR_ExternalTriggerPrescaler_Close  0
#define SMCR_ExternalTriggerPrescaler_2      1
#define SMCR_ExternalTriggerPrescaler_4      2
#define SMCR_ExternalTriggerPrescaler_8      3

#define SMCR_ExternalTriggerFilter_fDTS_None 0
#define SMCR_ExternalTriggerFilter_fINT_2 1
#define SMCR_ExternalTriggerFilter_fINT_4 2
#define SMCR_ExternalTriggerFilter_fINT_8 3
#define SMCR_ExternalTriggerFilter_fDTS2_6 4
#define SMCR_ExternalTriggerFilter_fDTS2_8 5
#define SMCR_ExternalTriggerFilter_fDTS4_6 6
#define SMCR_ExternalTriggerFilter_fDTS4_8 7
#define SMCR_ExternalTriggerFilter_fDTS8_6 8
#define SMCR_ExternalTriggerFilter_fDTS8_8 9
#define SMCR_ExternalTriggerFilter_fDTS16_5 10
#define SMCR_ExternalTriggerFilter_fDTS16_6 11
#define SMCR_ExternalTriggerFilter_fDTS16_8 12
#define SMCR_ExternalTriggerFilter_fDTS32_5 13
#define SMCR_ExternalTriggerFilter_fDTS32_6 14
#define SMCR_ExternalTriggerFilter_fDTS32_8 15
/****************************¼Ä´æÆ÷Ð´²Ù×÷*****************************/
#define STM32Timer_SMCR_TriggerSelect_W(x,y)                BitClearSet((x)->CR2,20,y>>3);BitClearSet((x)->SMCR,4,Mask3Bit,y)
#define STM32Timer_SMCR_SlaveModeSelect_W(x,y)              BitClearSet((x)->SMCR,16,Mask1Bit,y>>3);BitClearSet((x)->SMCR,0,Mask3Bit,y)
#define STM32Timer_SMCR_ExternalTriggerPolar_W(x,y)         BitClearSet((x)->SMCR,15,Mask1Bit,y)
#define STM32Timer_SMCR_ExternalClock2Enable_W(x,y)         BitClearSet((x)->SMCR,14,Mask1Bit,y)
#define STM32Timer_SMCR_ExternalTriggerPrescaler_W(x,y)     BitClearSet((x)->SMCR,12,Mask2Bit,y)
#define STM32Timer_SMCR_ExternalTriggerFilter_W(x,y)        BitClearSet((x)->SMCR,8,Mask4Bit,y)
#define STM32Timer_SMCR_MasterSlaveMode_W(x,y)              BitClearSet((x)->SMCR,7,Mask1Bit,y)
/****************************¼Ä´æÆ÷¶Á²Ù×÷*****************************/
#define STM32Timer_SMCR_TriggerSelect_R(x)                  ((BitRead((x)->CR2,20,Mask2Bit)<<3)+BitRead((x)->SMCR,4,Mask3Bit))
#define STM32Timer_SMCR_SlaveModeSelect_R(x)                ((BitRead((x)->SMCR,16,Mask1Bit)<<3)+BitRead((x)->SMCR,0,Mask3Bit))
#define STM32Timer_SMCR_ExternalTriggerPolar_R(x)           BitRead((x)->SMCR,15,Mask1Bit)
#define STM32Timer_SMCR_ExternalClock2Enable_R(x)           BitRead((x)->SMCR,14,Mask1Bit)
#define STM32Timer_SMCR_ExternalTriggerPrescaler_R(x)       BitRead((x)->SMCR,12,Mask2Bit)
#define STM32Timer_SMCR_ExternalTriggerFilter_R(x)          BitRead((x)->SMCR,8,Mask4Bit)
#define STM32Timer_SMCR_MasterSlaveMode_R(x)                BitRead((x)->SMCR,7,Mask1Bit)













/***********************************************************************/
/*                                                                     */
/*                              DIER¼Ä´æÆ÷                             */
/*                                                                     */
/***********************************************************************/
/****************************¼Ä´æÆ÷Ð´²Ù×÷*******************************/
#define STM32Timer_DIER_TriggerDMARequestEnable_W(x,y)    BitClearSet((x)->DIER,14,Mask1Bit,y)
#define STM32Timer_DIER_COMDMARequestEnable_W(x,y)        BitClearSet((x)->DIER,13,Mask1Bit,y)
#define STM32Timer_DIER_CC4DMARequestEnable_W(x,y)        BitClearSet((x)->DIER,12,Mask1Bit,y)
#define STM32Timer_DIER_CC3DMARequestEnable_W(x,y)        BitClearSet((x)->DIER,11,Mask1Bit,y)
#define STM32Timer_DIER_CC2DMARequestEnable_W(x,y)        BitClearSet((x)->DIER,10,Mask1Bit,y)
#define STM32Timer_DIER_CC1DMARequestEnable_W(x,y)        BitClearSet((x)->DIER,9,Mask1Bit,y)
#define STM32Timer_DIER_UpdateDMARequestEnable_W(x,y)     BitClearSet((x)->DIER,8,Mask1Bit,y)
#define STM32Timer_DIER_BreakInterruptEnable_W(x,y)       BitClearSet((x)->DIER,7,Mask1Bit,y)
#define STM32Timer_DIER_TriggerInterruptEnable_W(x,y)     BitClearSet((x)->DIER,6,Mask1Bit,y)
#define STM32Timer_DIER_COMInterruptEnable_W(x,y)         BitClearSet((x)->DIER,5,Mask1Bit,y)
#define STM32Timer_DIER_CC4InterruptEnable_W(x,y)         BitClearSet((x)->DIER,4,Mask1Bit,y)
#define STM32Timer_DIER_CC3InterruptEnable_W(x,y)         BitClearSet((x)->DIER,3,Mask1Bit,y)
#define STM32Timer_DIER_CC2InterruptEnable_W(x,y)         BitClearSet((x)->DIER,2,Mask1Bit,y)
#define STM32Timer_DIER_CC1InterruptEnable_W(x,y)         BitClearSet((x)->DIER,1,Mask1Bit,y)
#define STM32Timer_DIER_UpdateInterruptEnable_W(x,y)      BitClearSet((x)->DIER,0,Mask1Bit,y)
/****************************¼Ä´æÆ÷¶Á²Ù×÷*******************************/
#define STM32Timer_DIER_TriggerDMARequestEnable_R(x)    BitRead((x)->DIER,14,Mask1Bit)
#define STM32Timer_DIER_COMDMARequestEnable_R(x)        BitRead((x)->DIER,13,Mask1Bit)
#define STM32Timer_DIER_CC4DMARequestEnable_R(x)        BitRead((x)->DIER,12,Mask1Bit)
#define STM32Timer_DIER_CC3DMARequestEnable_R(x)        BitRead((x)->DIER,11,Mask1Bit)
#define STM32Timer_DIER_CC2DMARequestEnable_R(x)        BitRead((x)->DIER,10,Mask1Bit)
#define STM32Timer_DIER_CC1DMARequestEnable_R(x)        BitRead((x)->DIER,9,Mask1Bit)
#define STM32Timer_DIER_UpdateDMARequestEnable_R(x)     BitRead((x)->DIER,8,Mask1Bit)
#define STM32Timer_DIER_BreakInterruptEnable_R(x)       BitRead((x)->DIER,7,Mask1Bit)
#define STM32Timer_DIER_TriggerInterruptEnable_R(x)     BitRead((x)->DIER,6,Mask1Bit)
#define STM32Timer_DIER_COMInterruptEnable_R(x)         BitRead((x)->DIER,5,Mask1Bit)
#define STM32Timer_DIER_CC4InterruptEnable_R(x)         BitRead((x)->DIER,4,Mask1Bit)
#define STM32Timer_DIER_CC3InterruptEnable_R(x)         BitRead((x)->DIER,3,Mask1Bit)
#define STM32Timer_DIER_CC2InterruptEnable_R(x)         BitRead((x)->DIER,2,Mask1Bit)
#define STM32Timer_DIER_CC1InterruptEnable_R(x)         BitRead((x)->DIER,1,Mask1Bit)
#define STM32Timer_DIER_UpdateInterruptEnable_R(x)      BitRead((x)->DIER,0,Mask1Bit)














/***********************************************************************/
/*                                                                     */
/*                              SR¼Ä´æÆ÷                               */
/*                                                                     */
/***********************************************************************/
/****************************¼Ä´æÆ÷ÇåÁã²Ù×÷*****************************/
#define STM32Timer_SR_Compare6InterruptFlag_C(x)     BitClear((x)->SR,17,Mask1Bit)
#define STM32Timer_SR_Compare5InterruptFlag_C(x)     BitClear((x)->SR,15,Mask1Bit)
#define STM32Timer_SR_SystemBreakInterruptFlag_C(x)  BitClear((x)->SR,13,Mask1Bit)
#define STM32Timer_SR_CC4OverCaptureFlag_C(x)      BitClear((x)->SR,12,Mask1Bit)
#define STM32Timer_SR_CC3OverCaptureFlag_C(x)      BitClear((x)->SR,11,Mask1Bit)
#define STM32Timer_SR_CC2OverCaptureFlag_C(x)       BitClear((x)->SR,10,Mask1Bit)
#define STM32Timer_SR_CC1OverCaptureFlag_C(x)       BitClear((x)->SR,9,Mask1Bit)
#define STM32Timer_SR_Break2InterruptFlag_C(x)      BitClear((x)->SR,8,Mask1Bit)
#define STM32Timer_SR_BreakInterruptFlag_C(x)       BitClear((x)->SR,7,Mask1Bit)
#define STM32Timer_SR_TriggerInterruptFlag_C(x)         BitClear((x)->SR,6,Mask1Bit)
#define STM32Timer_SR_COMInterruptFlag_C(x)         BitClear((x)->SR,5,Mask1Bit)
#define STM32Timer_SR_CC4InterruptFlag_C(x)         BitClear((x)->SR,4,Mask1Bit)
#define STM32Timer_SR_CC3InterruptFlag_C(x)         BitClear((x)->SR,3,Mask1Bit)
#define STM32Timer_SR_CC2InterruptFlag_C(x)         BitClear((x)->SR,2,Mask1Bit)
#define STM32Timer_SR_CC1InterruptFlag_C(x)         BitClear((x)->SR,1,Mask1Bit)
#define STM32Timer_SR_UpdateInterruptFlag_C(x)      BitClear((x)->SR,0,Mask1Bit)
/****************************¼Ä´æÆ÷¶Á²Ù×÷******************************/
#define STM32Timer_SR_Compare6InterruptFlag_R(x)     BitRead((x)->SR,17,Mask1Bit)
#define STM32Timer_SR_Compare5InterruptFlag_R(x)     BitRead((x)->SR,15,Mask1Bit)
#define STM32Timer_SR_SystemBreakInterruptFlag_R(x)  BitRead((x)->SR,13,Mask1Bit)
#define STM32Timer_SR_CC4OverCaptureFlag_R(x)      BitRead((x)->SR,12,Mask1Bit)
#define STM32Timer_SR_CC3OverCaptureFlag_R(x)      BitRead((x)->SR,11,Mask1Bit)
#define STM32Timer_SR_CC2OverCaptureFlag_R(x)       BitRead((x)->SR,10,Mask1Bit)
#define STM32Timer_SR_CC1OverCaptureFlag_R(x)       BitRead((x)->SR,9,Mask1Bit)
#define STM32Timer_SR_Break2InterruptFlag_R(x)      BitRead((x)->SR,8,Mask1Bit)
#define STM32Timer_SR_BreakInterruptFlag_R(x)       BitRead((x)->SR,7,Mask1Bit)
#define STM32Timer_SR_TriggerInterruptFlag_R(x)         BitRead((x)->SR,6,Mask1Bit)
#define STM32Timer_SR_COMInterruptFlag_R(x)         BitRead((x)->SR,5,Mask1Bit)
#define STM32Timer_SR_CC4InterruptFlag_R(x)         BitRead((x)->SR,4,Mask1Bit)
#define STM32Timer_SR_CC3InterruptFlag_R(x)         BitRead((x)->SR,3,Mask1Bit)
#define STM32Timer_SR_CC2InterruptFlag_R(x)         BitRead((x)->SR,2,Mask1Bit)
#define STM32Timer_SR_CC1InterruptFlag_R(x)         BitRead((x)->SR,1,Mask1Bit)
#define STM32Timer_SR_UpdateInterruptFlag_R(x)      BitRead((x)->SR,0,Mask1Bit)





















/***********************************************************************/
/*                                                                     */
/*                              EGR¼Ä´æÆ÷                              */
/*                                                                     */
/***********************************************************************/
/****************************¼Ä´æÆ÷ÖÃÎ»²Ù×÷*****************************/
#define STM32Timer_EGR_Break2Generate_S(x)      BitSet((x)->GER,8,Mask1Bit,Enable)
#define STM32Timer_EGR_BreakGenerate_S(x)       BitSet((x)->GER,7,Mask1Bit,Enable)
#define STM32Timer_EGR_TriggerGenerate_S(x)     BitSet((x)->GER,6,Mask1Bit,Enable)
#define STM32Timer_EGR_COMGenerate_S(x)         BitSet((x)->GER,5,Mask1Bit,Enable)
#define STM32Timer_EGR_CC4Generate_S(x)         BitSet((x)->GER,4,Mask1Bit,Enable)
#define STM32Timer_EGR_CC3Generate_S(x)         BitSet((x)->GER,3,Mask1Bit,Enable)
#define STM32Timer_EGR_CC2Generate_S(x)         BitSet((x)->GER,2,Mask1Bit,Enable)
#define STM32Timer_EGR_CC1Generate_S(x)         BitSet((x)->GER,1,Mask1Bit,Enable)
#define STM32Timer_EGR_UpdateGenerate_S(x)      BitSet((x)->GER,0,Mask1Bit,Enable)

























/***********************************************************************/
/*                                                                     */
/*                              CCMR1¼Ä´æÆ÷                            */
/*                                                                     */
/***********************************************************************/
/*****************************ÅäÖÃ²ÎÊý¶¨Òå******************************/
#define CCMR1_OutputCompareMode_Freaze   0
#define CCMR1_OutputCompareMode_MatchValid 1
#define CCMR1_OutputCompareMode_MatchInvalid  2
#define CCMR1_OutputCompareMode_Reverse       3
#define CCMR1_OutputCompareMode_ForceInvalid     4
#define CCMR1_OutputCompareMode_ForceValid       5
#define CCMR1_OutputCompareMode_PWM1         6
#define CCMR1_OutputCompareMode_PWM2         7

#define CCMR1_CC1Select_Output  0
#define CCMR1_CC1Select_IC1TI1   1
#define CCMR1_CC1Select_IC1TI2   2
#define CCMR1_CC1Select_IC1TRC   3

#define CCMR1_CC2Select_Output  0
#define CCMR1_CC2Select_IC2TI2   1
#define CCMR1_CC2Select_IC2TI1   2
#define CCMR1_CC2Select_IC2TRC   3

#define CCMR1_InputCaptureFilter_fDTS    0
#define CCMR1_InputCaptureFilter_fINT_2  1
#define CCMR1_InputCaptureFilter_fINT_4  2
#define CCMR1_InputCaptureFilter_fINT_8  3
#define CCMR1_InputCaptureFilter_fDTS2_6  3
#define CCMR1_InputCaptureFilter_fDTS2_8  3
#define CCMR1_InputCaptureFilter_fDTS4_6  3
#define CCMR1_InputCaptureFilter_fDTS4_8  3
#define CCMR1_InputCaptureFilter_fDTS8_6  3
#define CCMR1_InputCaptureFilter_fDTS8_8  3
#define CCMR1_InputCaptureFilter_fDTS16_5  3
#define CCMR1_InputCaptureFilter_fDTS16_6  3
#define CCMR1_InputCaptureFilter_fDTS16_8  3
#define CCMR1_InputCaptureFilter_fDTS32_5  3
#define CCMR1_InputCaptureFilter_fDTS32_6  3
#define CCMR1_InputCaptureFilter_fDTS32_8  3

#define CCMR1_InputCapturePrescaler_None 0
#define CCMR1_InputCapturePrescaler_2    1
#define CCMR1_InputCapturePrescaler_4    2
#define CCMR1_InputCapturePrescaler_8    3

/****************************¼Ä´æÆ÷Ð´²Ù×÷*******************************/
#define STM32Timer_CCMR1_OC2Mode_W(x,y)             BitClearSet((x)->CCMR1,24,Mask1Bit,y>>3);BitClearSet((x)->CCMR1,12,Mask3Bit,y)
#define STM32Timer_CCMR1_OC2ClearEnable_W(x,y)      BitClearSet((x)->CCMR1,15,Mask1Bit,y)
#define STM32Timer_CCMR1_OC2PreloadEnable_W(x,y)    BitClearSet((x)->CCMR1,11,Mask1Bit,y)
#define STM32Timer_CCMR1_OC2FastEnable_W(x,y)       BitClearSet((x)->CCMR1,10,Mask1Bit,y)
#define STM32Timer_CCMR1_CC2Select_W(x,y)           BitClearSet((x)->CCMR1,8,Mask2Bit,y)
#define STM32Timer_CCMR1_OC1Mode_W(x,y)             BitClearSet((x)->CCMR1,16,Mask1Bit,y>>3);BitClearSet((x)->CCMR1,4,Mask3Bit,y)
#define STM32Timer_CCMR1_OC1ClearEnable_W(x,y)      BitClearSet((x)->CCMR1,7,Mask1Bit,y)
#define STM32Timer_CCMR1_OC1PreloadEnable_W(x,y)    BitClearSet((x)->CCMR1,3,Mask1Bit,y)
#define STM32Timer_CCMR1_OC1FastEnable_W(x,y)       BitClearSet((x)->CCMR1,2,Mask1Bit,y)
#define STM32Timer_CCMR1_CC1Select_W(x,y)           BitClearSet((x)->CCMR1,0,Mask2Bit,y)
#define STM32Timer_CCMR1_IC2Filter_W(x,y)           BitClearSet((x)->CCMR1,12,Mask4Bit,y)
#define STM32Timer_CCMR1_IC2Prescaler_W(x,y)          BitClearSet((x)->CCMR1,10,Mask2Bit,y)
#define STM32Timer_CCMR1_IC1Filter_W(x,y)           BitClearSet((x)->CCMR1,4,Mask4Bit,y)
#define STM32Timer_CCMR1_IC1Prescaler_W(x,y)        BitClearSet((x)->CCMR1,2,Mask2Bit,y)
/****************************¼Ä´æÆ÷¶Á²Ù×÷*******************************/
#define STM32Timer_CCMR1_OC2Mode_R(x)           ((BitRead((x)->CCMR1,24,Mask1Bit)<<3)+BitRead((x)->CCMR1,12,Mask3Bit))
#define STM32Timer_CCMR1_OC2ClearEnable_R(x)    BitRead((x)->CCMR1,15,Mask1Bit)
#define STM32Timer_CCMR1_OC2PreloadEnable_R(x)  BitRead((x)->CCMR1,11,Mask1Bit)
#define STM32Timer_CCMR1_OC2FastEnable_R(x)     BitRead((x)->CCMR1,10,Mask1Bit)
#define STM32Timer_CCMR1_CC2Select_R(x)           BitRead((x)->CCMR1,8,Mask2Bit)
#define STM32Timer_CCMR1_OC1Mode_R(x)           ((BitRead((x)->CCMR1,16,Mask1Bit)<<3)+BitRead((x)->CCMR1,4,Mask3Bit))
#define STM32Timer_CCMR1_OC1ClearEnable_R(x)    BitRead((x)->CCMR1,7,Mask1Bit)
#define STM32Timer_CCMR1_OC1PreloadEnable_R(x)    BitRead((x)->CCMR1,3,Mask1Bit)
#define STM32Timer_CCMR1_OC1FastEnable_R(x)       BitRead((x)->CCMR1,2,Mask1Bit)
#define STM32Timer_CCMR1_CC1Select_R(x)           BitRead((x)->CCMR1,0,Mask2Bit)
#define STM32Timer_CCMR1_IC2Filter_R(x)           BitRead((x)->CCMR1,12,Mask4Bit)
#define STM32Timer_CCMR1_IC2Prescaler_R(x)          BitRead((x)->CCMR1,10,Mask2Bit)
#define STM32Timer_CCMR1_IC1Filter_R(x)           BitRead((x)->CCMR1,4,Mask4Bit)
#define STM32Timer_CCMR1_IC1Prescaler_R(x)        BitRead((x)->CCMR1,2,Mask2Bit)





/***********************************************************************/
/*                                                                     */
/*                              CCMR2¼Ä´æÆ÷                            */
/*                                                                     */
/***********************************************************************/
/*****************************ÅäÖÃ²ÎÊý¶¨Òå******************************/
#define CCMR2_OutputCompareMode_Freaze   0
#define CCMR2_OutputCompareMode_MatchValid 1
#define CCMR2_OutputCompareMode_MatchInvalid  2
#define CCMR2_OutputCompareMode_Reverse       3
#define CCMR2_OutputCompareMode_ForceInvalid     4
#define CCMR2_OutputCompareMode_ForceValid       5
#define CCMR2_OutputCompareMode_PWM1         6
#define CCMR2_OutputCompareMode_PWM2         7

#define CCMR2_CC1Select_Output  0
#define CCMR2_CC1Select_IC1TI1   1
#define CCMR2_CC1Select_IC1TI2   2
#define CCMR2_CC1Select_IC1TRC   3

#define CCMR2_CC2Select_Output  0
#define CCMR2_CC2Select_IC2TI2   1
#define CCMR2_CC2Select_IC2TI1   2
#define CCMR2_CC2Select_IC2TRC   3

#define CCMR2_InputCaptureFilter_fDTS    0
#define CCMR2_InputCaptureFilter_fINT_2  1
#define CCMR2_InputCaptureFilter_fINT_4  2
#define CCMR2_InputCaptureFilter_fINT_8  3
#define CCMR2_InputCaptureFilter_fDTS2_6  3
#define CCMR2_InputCaptureFilter_fDTS2_8  3
#define CCMR2_InputCaptureFilter_fDTS4_6  3
#define CCMR2_InputCaptureFilter_fDTS4_8  3
#define CCMR2_InputCaptureFilter_fDTS8_6  3
#define CCMR2_InputCaptureFilter_fDTS8_8  3
#define CCMR2_InputCaptureFilter_fDTS16_5  3
#define CCMR2_InputCaptureFilter_fDTS16_6  3
#define CCMR2_InputCaptureFilter_fDTS16_8  3
#define CCMR2_InputCaptureFilter_fDTS32_5  3
#define CCMR2_InputCaptureFilter_fDTS32_6  3
#define CCMR2_InputCaptureFilter_fDTS32_8  3

#define CCMR2_InputCapturePrescaler_None 0
#define CCMR2_InputCapturePrescaler_2    1
#define CCMR2_InputCapturePrescaler_4    2
#define CCMR2_InputCapturePrescaler_8    3

/****************************¼Ä´æÆ÷Ð´²Ù×÷*******************************/
#define STM32Timer_CCMR2_OC2Mode_W(x,y)   BitClearSet((x)->CCMR2,24,Mask1Bit,y>>3);BitClearSet((x)->CCMR2,12,Mask3Bit,y)
#define STM32Timer_CCMR2_OC2ClearEnable_W(x,y)    BitClearSet((x)->CCMR2,15,Mask1Bit,y)
#define STM32Timer_CCMR2_OC2PreloadEnable_W(x,y)  BitClearSet((x)->CCMR2,11,Mask1Bit,y)
#define STM32Timer_CCMR2_OC2FastEnable_W(x,y)     BitClearSet((x)->CCMR2,10,Mask1Bit,y)
#define STM32Timer_CCMR2_CC2Select_W(x,y)           BitClearSet((x)->CCMR2,8,Mask2Bit,y)
#define STM32Timer_CCMR2_OC1Mode_W(x,y)         BitClearSet((x)->CCMR2,16,Mask1Bit,y>>3);BitClearSet((x)->CCMR2,4,Mask3Bit,y)
#define STM32Timer_CCMR2_OC1ClearEnable_W(x,y)  BitClearSet((x)->CCMR2,7,Mask1Bit,y)
#define STM32Timer_CCMR2_OC1PreloadEnable_W(x,y)    BitClearSet((x)->CCMR2,3,Mask1Bit,y)
#define STM32Timer_CCMR2_OC1FastEnable_W(x,y)       BitClearSet((x)->CCMR2,2,Mask1Bit,y)
#define STM32Timer_CCMR2_CC1Select_W(x,y)           BitClearSet((x)->CCMR2,0,Mask2Bit,y)
#define STM32Timer_CCMR2_IC2Filter_W(x,y)           BitClearSet((x)->CCMR2,12,Mask4Bit,y)
#define STM32Timer_CCMR2_IC2Prescaler_W(x,y)          BitClearSet((x)->CCMR2,10,Mask2Bit,y)
#define STM32Timer_CCMR2_IC1Filter_W(x,y)           BitClearSet((x)->CCMR2,4,Mask4Bit,y)
#define STM32Timer_CCMR2_IC1Prescaler_W(x,y)        BitClearSet((x)->CCMR2,2,Mask2Bit,y)
/****************************¼Ä´æÆ÷¶Á²Ù×÷*******************************/
#define STM32Timer_CCMR2_OC2Mode_R(x)   ((BitRead((x)->CCMR2,24,Mask1Bit)<<3)+BitRead((x)->CCMR2,12,Mask3Bit))
#define STM32Timer_CCMR2_OC2ClearEnable_R(x)    BitRead((x)->CCMR2,15,Mask1Bit)
#define STM32Timer_CCMR2_OC2PreloadEnable_R(x)  BitRead((x)->CCMR2,11,Mask1Bit)
#define STM32Timer_CCMR2_OC2FastEnable_R(x)     BitRead((x)->CCMR2,10,Mask1Bit)
#define STM32Timer_CCMR2_CC2Select_R(x)           BitRead((x)->CCMR2,8,Mask2Bit)
#define STM32Timer_CCMR2_OC1Mode_R(x)         ((BitRead((x)->CCMR2,16,Mask1Bit)<<3)+BitRead((x)->CCMR2,4,Mask3Bit))
#define STM32Timer_CCMR2_OC1ClearEnable_R(x)  BitRead((x)->CCMR2,7,Mask1Bit)
#define STM32Timer_CCMR2_OC1PreloadEnable_R(x)    BitRead((x)->CCMR2,3,Mask1Bit)
#define STM32Timer_CCMR2_OC1FastEnable_R(x)       BitRead((x)->CCMR2,2,Mask1Bit)
#define STM32Timer_CCMR2_CC1Select_R(x)           BitRead((x)->CCMR2,0,Mask2Bit)
#define STM32Timer_CCMR2_IC2Filter_R(x)           BitRead((x)->CCMR2,12,Mask4Bit)
#define STM32Timer_CCMR2_IC2Prescaler_R(x)          BitRead((x)->CCMR2,10,Mask2Bit)
#define STM32Timer_CCMR2_IC1Filter_R(x)           BitRead((x)->CCMR2,4,Mask4Bit)
#define STM32Timer_CCMR2_IC1Prescaler_R(x)        BitRead((x)->CCMR2,2,Mask2Bit)








/***********************************************************************/
/*                                                                     */
/*                              CCER¼Ä´æÆ÷                             */
/*                                                                     */
/***********************************************************************/
/*****************************ÅäÖÃ²ÎÊý¶¨Òå******************************/
#define CCER_POSITIVE 0
#define CCER_NEGATIVE 1
/****************************¼Ä´æÆ÷Ð´²Ù×÷*******************************/
#define STM32Timer_CCER_CC6Polarity_W(x,y)    BitClearSet((x)->CCER,21,Mask1Bit,y)
#define STM32Timer_CCER_CC6OutputEnable_W(x,y)  BitClearSet((x)->CCER,20,Mask1Bit,y)
#define STM32Timer_CCER_CC5Polarity_W(x,y)      BitClearSet((x)->CCER,17,Mask1Bit,y)
#define STM32Timer_CCER_CC5OutputEnable_W(x,y)  BitClearSet((x)->CCER,16,Mask1Bit,y)
#define STM32Timer_CCER_CC4NPolarity_W(x,y)     BitClearSet((x)->CCER,15,Mask1Bit,y)
#define STM32Timer_CCER_CC4Polarity_W(x,y)      BitClearSet((x)->CCER,13,Mask1Bit,y)
#define STM32Timer_CCER_CC4OutputEnable_W(x,y)    BitClearSet((x)->CCER,12,Mask1Bit,y)
#define STM32Timer_CCER_OC3NPolarity_W(x,y)     BitClearSet((x)->CCER,11,Mask1Bit,y)
#define STM32Timer_CCER_CC3NOutputEnable_W(x,y) BitClearSet((x)->CCER,10,Mask1Bit,y)
#define STM32Timer_CCER_OC3Polarity_W(x,y)      BitClearSet((x)->CCER,9,Mask1Bit,y)
#define STM32Timer_CCER_OC3OutputEnable_W(x,y)  BitClearSet((x)->CCER,8,Mask1Bit,y)
#define STM32Timer_CCER_OC2NPolarity_W(x,y)     BitClearSet((x)->CCER,7,Mask1Bit,y)
#define STM32Timer_CCER_CC2NOutputEnable_W(x,y) BitClearSet((x)->CCER,6,Mask1Bit,y)
#define STM32Timer_CCER_OC2Polarity_W(x,y)      BitClearSet((x)->CCER,5,Mask1Bit,y)
#define STM32Timer_CCER_OC2OutputEnable_W(x,y)  BitClearSet((x)->CCER,4,Mask1Bit,y)
#define STM32Timer_CCER_OC1NPolarity_W(x,y)     BitClearSet((x)->CCER,3,Mask1Bit,y)
#define STM32Timer_CCER_CC1NOutputEnable_W(x,y) BitClearSet((x)->CCER,2,Mask1Bit,y)
#define STM32Timer_CCER_OC1Polarity_W(x,y)      BitClearSet((x)->CCER,1,Mask1Bit,y)
#define STM32Timer_CCER_OC1OutputEnable_W(x,y)  BitClearSet((x)->CCER,0,Mask1Bit,y)
#define STM32Timer_CCER_IC4Polarity_W(x,y)      BitClearSet((x)->CCER,15,Mask1Bit,y>>1);BitClearSet((x)->CCER,13,Mask1Bit,y)
#define STM32Timer_CCER_IC4CaptureEnable_W(x,y) BitClearSet((x)->CCER,12,Mask1Bit,y)
#define STM32Timer_CCER_IC3Polarity_W(x,y)      BitClearSet((x)->CCER,11,Mask1Bit,y>>1);BitClearSet((x)->CCER,9,Mask1Bit,y)
#define STM32Timer_CCER_IC3CaptureEnable_W(x,y) BitClearSet((x)->CCER,8,Mask1Bit,y)
#define STM32Timer_CCER_IC2Polarity_W(x,y)      BitClearSet((x)->CCER,7,Mask1Bit,y>>1);BitClearSet((x)->CCER,5,Mask1Bit,y)
#define STM32Timer_CCER_IC2CaptureEnable_W(x,y) BitClearSet((x)->CCER,4,Mask1Bit,y)
#define STM32Timer_CCER_IC1Polarity_W(x,y)      BitClearSet((x)->CCER,3,Mask1Bit,y>>1);BitClearSet((x)->CCER,1,Mask1Bit,y)
#define STM32Timer_CCER_IC1CaptureEnable_W(x,y) BitClearSet((x)->CCER,0,Mask1Bit,y)
/****************************¼Ä´æÆ÷¶Á²Ù×÷*****************************/
#define STM32Timer_CCER_CC6Polarity_R(x)    BitRead((x)->CCER,21,Mask1Bit)
#define STM32Timer_CCER_CC6OutputEnable_R(x)  BitRead((x)->CCER,20,Mask1Bit)
#define STM32Timer_CCER_CC5Polarity_R(x)      BitRead((x)->CCER,17,Mask1Bit)
#define STM32Timer_CCER_CC5OutputEnable_R(x)  BitRead((x)->CCER,16,Mask1Bit)
#define STM32Timer_CCER_CC4NPolarity_R(x)     BitRead((x)->CCER,15,Mask1Bit)
#define STM32Timer_CCER_CC4Polarity_R(x)      BitRead((x)->CCER,13,Mask1Bit)
#define STM32Timer_CCER_CC4OutputEnable_R(x)    BitRead((x)->CCER,12,Mask1Bit)
#define STM32Timer_CCER_OC3NPolarity_R(x)     BitRead((x)->CCER,11,Mask1Bit)
#define STM32Timer_CCER_CC3NOutputEnable_R(x) BitRead((x)->CCER,10,Mask1Bit)
#define STM32Timer_CCER_OC3Polarity_R(x)      BitRead((x)->CCER,9,Mask1Bit)
#define STM32Timer_CCER_OC3OutputEnable_R(x)  BitRead((x)->CCER,8,Mask1Bit)
#define STM32Timer_CCER_OC2NPolarity_R(x)     BitRead((x)->CCER,7,Mask1Bit)
#define STM32Timer_CCER_CC2NOutputEnable_R(x) BitRead((x)->CCER,6,Mask1Bit)
#define STM32Timer_CCER_OC2Polarity_R(x)      BitRead((x)->CCER,5,Mask1Bit)
#define STM32Timer_CCER_OC2OutputEnable_R(x)  BitRead((x)->CCER,4,Mask1Bit)
#define STM32Timer_CCER_OC1NPolarity_R(x)     BitRead((x)->CCER,3,Mask1Bit)
#define STM32Timer_CCER_CC1NOutputEnable_R(x) BitRead((x)->CCER,2,Mask1Bit)
#define STM32Timer_CCER_OC1Polarity_R(x)      BitRead((x)->CCER,1,Mask1Bit)
#define STM32Timer_CCER_OC1OutputEnable_R(x)  BitRead((x)->CCER,0,Mask1Bit)
#define STM32Timer_CCER_IC4Polarity_R(x)      ((BitRead((x)->CCER,15,Mask1Bit)<<1)+BitRead((x)->CCER,13,Mask1Bit))
#define STM32Timer_CCER_IC4CaptureEnable_R(x) BitRead((x)->CCER,12,Mask1Bit)
#define STM32Timer_CCER_IC3Polarity_R(x)      ((BitRead((x)->CCER,11,Mask1Bit)<<1)+BitRead((x)->CCER,9,Mask1Bit))
#define STM32Timer_CCER_IC3CaptureEnable_R(x) BitRead((x)->CCER,8,Mask1Bit)
#define STM32Timer_CCER_IC2Polarity_R(x)      ((BitRead((x)->CCER,7,Mask1Bit)<<1)+BitRead((x)->CCER,5,Mask1Bit))
#define STM32Timer_CCER_IC2CaptureEnable_R(x) BitRead((x)->CCER,4,Mask1Bit,)
#define STM32Timer_CCER_IC1Polarity_R(x)      ((BitRead((x)->CCER,3,Mask1Bit)<<1)+BitRead((x)->CCER,1,Mask1Bit))
#define STM32Timer_CCER_IC1CaptureEnable_R(x) BitRead((x)->CCER,0,Mask1Bit)








/***********************************************************************/
/*                                                                     */
/*                             CNT¼Ä´æÆ÷                               */
/*                                                                     */
/***********************************************************************/
/****************************¼Ä´æÆ÷Ð´²Ù×÷*******************************/
#define STM32Timer_CNT_W(x,y)  ((x==TIM2)||(x==TIM5))?(x)->CNT=y&Mask32Bit:(x)->CNT=y&Mask16Bit
/****************************¼Ä´æÆ÷¶Á²Ù×÷*******************************/
#define STM32Timer_CNT_R(x)  ((x==TIM2)||(x==TIM5))?BitRead((x)->CNT,0,Mask32Bit):BitRead((x)->CNT,0,Mask16Bit)

/***********************************************************************/
/*                                                                     */
/*                            PSC¼Ä´æÆ÷                                */
/*                                                                     */
/***********************************************************************/
/****************************¼Ä´æÆ÷Ð´²Ù×÷*******************************/
#define STM32Timer_PSC_R(x) BitRead((x)->PSC,0,Mask16Bit)
/****************************¼Ä´æÆ÷¶Á²Ù×÷*******************************/
#define STM32Timer_PSC_W(x,y)   (x)->PSC=y&Mask16Bit


/***********************************************************************/
/*                                                                     */
/*                            ARR¼Ä´æÆ÷                                */
/*                                                                     */
/***********************************************************************/
/****************************¼Ä´æÆ÷Ð´²Ù×÷*******************************/
#define STM32Timer_ARR_W(x,y)  ((x==TIM2)||(x==TIM5))?(x)->ARR=y&Mask32Bit:(x)->ARR=y&Mask16Bit
/****************************¼Ä´æÆ÷¶Á²Ù×÷*******************************/
#define STM32Timer_ARR_R(x)  ((x==TIM2)||(x==TIM5))?BitRead((x)->ARR,0,Mask32Bit):BitRead((x)->ARR,0,Mask16Bit)


/***********************************************************************/
/*                                                                     */
/*                            CCR1¼Ä´æÆ÷                               */
/*                                                                     */
/***********************************************************************/
/****************************¼Ä´æÆ÷Ð´²Ù×÷*******************************/
#define STM32Timer_CCR1_W(x,y)  ((x==TIM2)||(x==TIM5))?(x)->CCR1=y&Mask32Bit:(x)->CCR1=y&Mask16Bit
/****************************¼Ä´æÆ÷¶Á²Ù×÷*******************************/
#define STM32Timer_CCR1_R(x)  ((x==TIM2)||(x==TIM5))?BitRead((x)->CCR1,0,Mask32Bit):BitRead((x)->CCR1,0,Mask16Bit)


/***********************************************************************/
/*                                                                     */
/*                            CCR2¼Ä´æÆ÷                               */
/*                                                                     */
/***********************************************************************/
/****************************¼Ä´æÆ÷Ð´²Ù×÷*******************************/
#define STM32Timer_CCR2_W(x,y)  ((x==TIM2)||(x==TIM5))?(x)->CCR2=y&Mask32Bit:(x)->CCR2=y&Mask16Bit
/****************************¼Ä´æÆ÷¶Á²Ù×÷*******************************/
#define STM32Timer_CCR2_R(x)  ((x==TIM2)||(x==TIM5))?BitRead((x)->CCR2,0,Mask32Bit):BitRead((x)->CCR2,0,Mask16Bit)


/***********************************************************************/
/*                                                                     */
/*                            CCR3¼Ä´æÆ÷                               */
/*                                                                     */
/***********************************************************************/
/****************************¼Ä´æÆ÷Ð´²Ù×÷*******************************/
#define STM32Timer_CCR3_W(x,y)  ((x==TIM2)||(x==TIM5))?(x)->CCR3=y&Mask32Bit:(x)->CCR3=y&Mask16Bit
/****************************¼Ä´æÆ÷¶Á²Ù×÷*******************************/
#define STM32Timer_CCR3_R(x)  ((x==TIM2)||(x==TIM5))?BitRead((x)->CCR3,0,Mask32Bit):BitRead((x)->CCR3,0,Mask16Bit)


/***********************************************************************/
/*                                                                     */
/*                            CCR4¼Ä´æÆ÷                               */
/*                                                                     */
/***********************************************************************/
/****************************¼Ä´æÆ÷Ð´²Ù×÷*******************************/
#define STM32Timer_CCR4_W(x,y)  ((x==TIM2)||(x==TIM5))?(x)->CCR4=y&Mask32Bit:(x)->CCR4=y&Mask16Bit
/****************************¼Ä´æÆ÷¶Á²Ù×÷*******************************/
#define STM32Timer_CCR4_R(x)  ((x==TIM2)||(x==TIM5))?BitRead((x)->CCR4,0,Mask32Bit):BitRead((x)->CCR4,0,Mask16Bit)


#endif
