#ifndef _DMA_H_
#define _DMA_H_
#include "sys.h"

#define TRANPORT_TYPE_MEM	1
#define TRANPORT_TYPE_PER	2

#define BYTE		1
#define HALF_WORD	2
#define WORD		3

/***************DMA1***************/
//Stream0
#define DMA_SPI3_RX			100
#define DMA_I2C_RX			101
#define DMA_TIM4_CH1		102
#define DMA_I2S3_EXT_RX		103
#define DMA_USART5_RX		104
#define DMA_UART8_TX		105
#define DMA_TIM5_CH3		106
#define DMA_TIM5_UP			106

//Stream1
#define DMA_TIM2_UP			113
#define DMA_TIM2_CH3		113
#define DMA_USART3_RX		114
#define DMA_UART7_TX		115
#define DMA_TIM5_CH4		116
#define DMA_TIM5_TRIG		116
#define DMA_TIM6_UP			117

//Stream2
#define DMA_SPI3_RX_1		120
#define DMA_TIM7_UP			121
#define DMA_I2S3_EXT_RX_1	122
#define DMA_I2C3_RX			123
#define DMA_UART4_RX		124
#define DMA_TIM3_CH4		125
#define DMA_TIM3_UP			125
#define DMA_TIM5_CH1		126
#define DMA_I2C2_RX			127

//Stream3
#define DMA_SPI2_RX			130
#define DMA_TIM4_CH2		132
#define DMA_I2S2_EXT_RX		133
#define DMA_USART3_TX		134
#define DMA_UART7_RX		135
#define DMA_TIM5_CH4_1		136
#define DMA_TIM5_TRIG_1		137
#define DMA_I2C2_RX_1		138

//Stream4
#define DMA_SPI2_TX			140
#define DMA_TIM7_UP_1		141
#define DMA_I2S2_EXT_TX		142
#define DMA_I2C3_TX			143
#define DMA_USART4_TX		144
#define DMA_TIM3_CH1		145
#define DMA_TIM3_TRIG		145
#define DMA_TIM5_CH2		146
#define DMA_USART3_TX_1		147

//Stream5
#define DMA_SPI3_TX			150
#define DMA_I2C1_RX			151
#define DMA_I2S3_EXT_TX		152
#define DMA_TIM2_CH1		153
#define DMA_USART2_RX		154
#define DMA_TIM3_CH2		155
#define DMA_DAC1			157

//Stream6
#define DMA_I2C1_TX			161
#define DMA_TIM4_UP			162
#define DMA_TIM2_CH2		163
#define DMA_TIM2_CH4		163
#define DMA_USART2_TX		164
#define DMA_UART8_RX		165
#define DMA_TIM5_UP_1		166
#define DMA_DAC2			167

//Stream7
#define DMA_SPI3_TX_1		170
#define DMA_I2C1_TX_1		171
#define DMA_TIM4_CH3		172
#define DMA_TIM2_UP_1		173
#define DMA_TIM2_CH4_1		173
#define DMA_UART5_TX		174
#define DMA_TIM3_CH3		175
#define DMA_I2C2_TX			176

/*************DMA2*************/
//Stream0
#define DMA_ADC1			200
#define DMA_ADC3			202
#define DMA_SPI1_RX			203
#define DMA_SPI4_RX			204
#define DMA_TIM1_TRIG		206

//Stream1
#define DMA_DCMI			211
#define DMA_ADC3_1			212
#define DMA_SPI4_TX			214
#define DMA_USART6_RX		215
#define DMA_TIM1_CH1		216
#define DMA_TIM8_UP			217

//Stream2
#define DMA_TIM8_CH1		220
#define DMA_TIM8_CH2		220
#define DMA_TIM8_CH3		220
#define DMA_ADC2			221
#define DMA_SPI1_RX_1		223
#define DMA_USART1_RX		224
#define DMA_USART6_RX_1		225
#define DMA_TIM1_CH2		226
#define DMA_TIM8_CH1_1		227

//Stream3
#define DMA_ADC2_1			231
#define DMA_SPI5_RX			232
#define DMA_SPI1_TX			233
#define DMA_SDIO			234
#define DMA_SPI4_RX_1		235
#define DMA_TIM1_CH1_1		236
#define DMA_TIM8_CH2_1		237

//Stream4
#define DMA_ADC1_1			240
#define DMA_SPI5_TX			242
#define DMA_SPI4_TX_1		245
#define DMA_TIM1_CH4		246
#define DMA_TIM1_TRIG_1		246
#define DMA_TIM1_COM		246
#define DMA_TIM8_CH3_1		247

//Stream5
#define DMA_SPI6_TX			251
#define DMA_CRYP_OUT		252
#define DMA_SPI1_TX_1		253
#define DMA_USART1_RX_1		254
#define DMA_TIM1_UP			256
#define DMA_SPI5_RX_1		257

//Stream6
#define DMA_TIM1_CH1_2		260
#define DMA_TIM1_CH2_1		260
#define DMA_TIM1_CH3		260
#define DMA_SPI6_RX			261
#define DMA_CRYP_IN			262
#define DMA_SDIO_1			264
#define DMA_USART6_TX		265
#define DMA_TIM1_CH3_1		266
#define DMA_SPI5_TX_1		267

//Stream7
#define DMA_DCMI_1			271
#define DMA_HASH_IN			272
#define DMA_USART1_TX		274
#define DMA_USART6_TX_1		275
#define DMA_TIM8_CH4		277
#define DMA_TIM8_TRIG		277
#define DMA_TIM8_COM		277


#define DMA_Enable(x)	((x)->Executor.Stream->CR|=1)

#define DMA_Disable(x)	(x)->Executor.Stream->CR&=~1; \
                        while (((x)->Executor.Stream->CR&1)==1);
                        
#define DMA_Stop(x)     DMA_Disable(x); \
                        *((x)->Executor.SR)|=1<<((x)->Executor.SR_Pos+5)

#define DMA_Get_TCIF(x)	((*((x)->Executor.SR)>>((x)->Executor.SR_Pos+5))&1)

#define DMA_Clear_SR(x)	(*((x)->Executor.CSR)|=0x3d<<(x)->Executor.SR_Pos)

#define DMA_Get_Transfer_Num(x)	((x)->Controller.DataNumber-(x)->Executor.Stream->NDTR)

#define DMA_Reload(x)	((x)->Executor.Stream->NDTR=(x)->Controller.DataNumber)

typedef void (*DMA_Interrupt_CallbackFunc)(void);

typedef struct TransferPort_Class
{
	u8 Type;
	void* Address;
	u8 IsFixed;
} tranport;

typedef struct StreamController_Class
{
	u8 Loop;
	u8 DualBuffer;
	u8 WordLength;
	u16 DataNumber;
	void* SecDataBuffer;
} streamctrller;

typedef struct Transfer_Executor_Class
{
	DMA_TypeDef* DMA;
	DMA_Stream_TypeDef* Stream;
	u8 Channel;
	vu32* SR;
	vu32* CSR;
	u8 SR_Pos;
} tranexe;

typedef struct DMA_Transfer_Plan_Class
{
	tranport Start;
	tranport End;
	streamctrller Controller;
	tranexe Executor;
} tranplan;

void DMA_Build_TransferPlan(tranplan* tp, u16 DMA_Channel);
void DMA_StreamController_Set(tranplan* tp);
void DMA_Interrupt_Set(tranplan* tp, u8 Group, u8 PrePriority, u8 SubPriority, DMA_Interrupt_CallbackFunc Callback);
//void DMA_SR_Clear(tranplan* tp);





#endif
