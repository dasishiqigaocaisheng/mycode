#ifndef _IS61LV51216_H_
#define _IS61LV51216_H_
/*
IS62WV51216：512K*16Bit，每个地址对应一个16位数
引脚连接：
PD14---IO0		PF0----A0
PD15---IO1		PF1----A1
PD0----IO2		PF2----A2
PD1----IO3		PF3----A3
PE7----IO4		PF4----A4
PE8----IO5		PF5----A5
PE9----IO6		PF12---A6
PE10---IO7		PF13---A7
PE11---IO8		PF14---A8
PE12---IO9		PF15---A9
PE13---IO10		PG0----A10
PE14---IO11		PG1----A11
PE15---IO12		PG2----A12
PD8----IO13		PG3----A13
PD9----IO14		PG4----A14
PD10---IO15		PG5----A15
				PD11---A16
				PD12---A17
				PD13---A18

PD4----NOE
PD5----NWE
PE0----NBL0
PE1----NBL1
PG10---NE3

*/

/*FSMC_Bank1存储区域3的首地址*/
#define EXTERN_SRAM_ADDR ((volatile unsigned short*)0x68000000)

/*初始化FSMC*/
void IS62WV51216_Init(void);

#endif
