#ifndef _SD_CARD_H_
#define _SD_CARD_H_

#include "sys.h"

#define SD_CARD_DEBUG

void SD_Card_Init(void);
//void SD_Card_Fast_Mode_Enable(TIM_TypeDef* Timer, u8 Interrupt_Group, u8 Pre_Priority, u8 Sub_Priority);
//void SD_Card_Fast_Mode_On(void);
//void SD_Card_Fast_Mode_Off(void);
//u8 SD_Card_Data_Ready(void);

u8 SD_Card_Write_Data(long long unsigned int Byte_Address, long long unsigned int Length, void* Data);
u8 SD_Card_Read_Data(long long unsigned int Byte_Address, long long unsigned int Length, void* Data);

u8 SD_Card_Set_Memory(long long unsigned int Byte_Address, long long unsigned int Length, u8 Data);

#ifdef SD_CARD_DEBUG
void SD_Card_Test(USART_TypeDef* USART);
#endif

#endif
