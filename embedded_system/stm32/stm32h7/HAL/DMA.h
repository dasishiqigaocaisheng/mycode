#ifndef _DMA_H_
#define _DMA_H_
#include "sys.h"

#define TRANPORT_TYPE_MEM	1
#define TRANPORT_TYPE_PER	2


#define    DMA_WL_BYTE      1
#define    DMA_WL_HALF_WORD 2
#define    DMA_WL_WORD      3


typedef enum 
{
    DMAMUX1_ReqGen0=1,
    DMAMUX1_ReqGen1,
    DMAMUX1_ReqGen2,
    DMAMUX1_ReqGen3,
    DMAMUX1_ReqGen4,
    DMAMUX1_ReqGen5,
    DMAMUX1_ReqGen6,
    DMAMUX1_ReqGen7,
    DMAMUX1_ADC1,
    DMAMUX1_ADC2,
    DMAMUX1_TIM1_CH1,
    DMAMUX1_TIM1_CH2,
    DMAMUX1_TIM1_CH3,
    DMAMUX1_TIM1_CH4,
    DMAMUX1_TIM1_UP,
    DMAMUX1_TIM1_TRIG,
    DMAMUX1_TIM1_COM,
    DMAMUX1_TIM2_CH1,
    DMAMUX1_TIM2_CH2,
    DMAMUX1_TIM2_CH3,
    DMAMUX1_TIM2_CH4,
    DMAMUX1_TIM2_UP,
    DMAMUX1_TIM3_CH1,
    DMAMUX1_TIM3_CH2,
    DMAMUX1_TIM3_CH3,
    DMAMUX1_TIM3_CH4,
    DMAMUX1_TIM3_UP,
    DMAMUX1_TIM3_TRIG,
    DMAMUX1_TIM4_CH1,
    DMAMUX1_TIM4_CH2,
    DMAMUX1_TIM4_CH3,
    DMAMUX1_TIM4_UP,
    DMAMUX1_I2C1_RX,
    DMAMUX1_I2C1_TX,
    DMAMUX1_I2C2_RX,
    DMAMUX1_I2C2_TX,
    DMAMUX1_SPI1_RX,
    DMAMUX1_SPI1_TX,
    DMAMUX1_SPI2_RX,
    DMAMUX1_SPI2_TX,
    DMAMUX1_USART1_RX,
    DMAMUX1_USART1_TX,
    DMAMUX1_USART2_RX,
    DMAMUX1_USART2_TX,
    DMAMUX1_USART3_RX,
    DMAMUX1_USART3_TX,
    DMAMUX1_TIM8_CH1,
    DMAMUX1_TIM8_CH2,
    DMAMUX1_TIM8_CH3,
    DMAMUX1_TIM8_CH4,
    DMAMUX1_TIM8_UP,
    DMAMUX1_TIM8_TRIG,
    DMAMUX1_TIM8_COM,
    DMAMUX1_TIM5_CH1=55,
    DMAMUX1_TIM5_CH2,
    DMAMUX1_TIM5_CH3,
    DMAMUX1_TIM5_CH4,
    DMAMUX1_TIM5_UP,
    DMAMUX1_TIM5_TRIG,
    DMAMUX1_SPI3_RX,
    DMAMUX1_SPI3_TX,
    DMAMUX1_UART4_RX,
    DMAMUX1_UART4_TX,
    DMAMUX1_UART5_RX,
    DMAMUX1_UART5_TX,
    DMAMUX1_DAC1,
    DMAMUX1_DAC2,
    DMAMUX1_TIM6_UP,
    DMAMUX1_TIM7_UP,
    DMAMUX1_USART6_RX,
    DMAMUX1_USART6_TX,
    DMAMUX1_I2C3_RX,
    DMAMUX1_I2C3_TX,
    DMAMUX1_DCMI,
    DMAMUX1_CRYP_IN,
    DMAMUX1_CRYP_OUT,
    DMAMUX1_HASH_IN,
    DMAMUX1_UART7_RX,
    DMAMUX1_UART7_TX,
    DMAMUX1_UART8_RX,
    DMAMUX1_UART8_TX,
    DMAMUX1_SPI4_RX,
    DMAMUX1_SPI4_TX,
    DMAMUX1_SPI5_RX,
    DMAMUX1_SPI5_TX,
    DMAMUX1_SAI1_A,
    DMAMUX1_SAI1_B,
    DMAMUX1_SAI2_A,
    DMAMUX1_SAI2_B,
    DMAMUX1_SWPMI_RX,
    DMAMUX1_SWPMI_TX,
    DMAMUX1_SPDIFRX_DT,
    DMAMUX1_SPDIFRX_CS,
    DMAMUX1_HR_REQ_1,
    DMAMUX1_HR_REQ_2,
    DMAMUX1_HR_REQ_3,
    DMAMUX1_HR_REQ_4,
    DMAMUX1_HR_REQ_5,
    DMAMUX1_HR_REQ_6,
    DMAMUX1_dfsdm1_dma0,
    DMAMUX1_dfsdm1_dma1,
    DMAMUX1_dfsdm1_dma2,
    DMAMUX1_dfsdm1_dma3,
    DMAMUX1_TIM15_CH1,
    DMAMUX1_TIM15_UP,
    DMAMUX1_TIM15_TRIG,
    DMAMUX1_TIM15_COM,
    DMAMUX1_TIM16_CH1,
    DMAMUX1_TIM16_UP,
    DMAMUX1_TIM17_CH1,
    DMAMUX1_TIM17_UP,
    DMAMUX1_SAI3_A,
    DMAMUX1_SAI3_B,
    DMAMUX1_ADC3
} dmamux1_req_type;

typedef enum
{
    DMA1_Channel0,
    DMA1_Channel1,
    DMA1_Channel2,
    DMA1_Channel3,
    DMA1_Channel4,
    DMA1_Channel5,
    DMA1_Channel6,
    DMA1_Channel7,
    DMA2_Channel0,
    DMA2_Channel1,
    DMA2_Channel2,
    DMA2_Channel3,
    DMA2_Channel4,
    DMA2_Channel5,
    DMA2_Channel6,
    DMA2_Channel7
} dma_channel_type;

typedef enum
{
    Beat4_Burst1,
    Beat4_Burst2,
    Beat4_Burst3,
    Beat4_Burst4,
    Beat8_Burst1,
    Beat8_Burst2,
    Beat16_Burst1,
    Single
} dma_beat_burst;

typedef enum
{
    DMA_FIFO_Quarter1=0,
    DMA_FIFO_Half,
    DMA_FIFO_Quarter3,
    DMA_FIFO_Full,
    DMA_FIFO_Unused
} dma_fifo_mode;

#define DMA_Enable(x)	((x)->Executor.Stream->CR|=1)

#define DMA_Disable(x)	(x)->Executor.Stream->CR&=~1; \
                        while (((x)->Executor.Stream->CR&1)==1);
                        
#define DMA_Stop(x)     DMA_Disable(x); \
                        *((x)->Executor.SR)|=1<<((x)->Executor.SR_Pos+5)

#define DMA_Get_TCIF(x)	((*((x)->Executor.SR)>>((x)->Executor.SR_Pos+5))&1)

#define DMA_Clear_SR(x)	(*((x)->Executor.CSR)|=0x3d<<(x)->Executor.SR_Pos)

#define DMA_Get_Transfer_Num(x)	((x)->Controller.DataNumber-(x)->Executor.Stream->NDTR)

typedef void (*DMA_Interrupt_CallbackFunc)(void);

typedef struct TransferPort_Class
{
	u8 Type;
	void* Address;
	u8 IsFixed;
    u8 WordLength;
    dma_beat_burst Burst_Mode;
} tranport;

typedef struct StreamController_Class
{
	u8 Loop;
	u8 DualBuffer;
	u16 DataNumber;
	void* SecDataBuffer;
    dma_fifo_mode FIFO_Mode;
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

void DMA_Build_TransferPlan(tranplan* tp, dmamux1_req_type req, dma_channel_type ch);
void DMA_StreamController_Set(tranplan* tp);
void DMA_Interrupt_Set(tranplan* tp, u8 Group, u8 PrePriority, u8 SubPriority, DMA_Interrupt_CallbackFunc Callback);
void DMA_Reload(tranplan* tp);





#endif
