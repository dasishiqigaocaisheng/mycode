#ifndef _W9825G6_H_
#define _W9825G6_H_
#include "sys.h"

//FANKE STM32H7  使用控制器1，也就是BANK5
//APOLLO STM32H7 使用控制器1，也就是BANK5
//256Mb=16M*16bit=32MB

#define BANK6_SDRAM_ADDR    ((u16*)0XD0000000) 
#define BANK5_SDRAM_ADDR    ((u16*)0XC0000000) 

#define SDRAM_ADDR          BANK5_SDRAM_ADDR

#define MEMORY_SIZE         33554432

void W9825G6_Init(void);

#endif
