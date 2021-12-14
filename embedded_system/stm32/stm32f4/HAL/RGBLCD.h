#ifndef _RGBLCD_H_
#define _RGBLCD_H_
#include "sys.h"

#define LAYER1_GRAM_ADDR	0xd1f44800  //BANK6_SDRAM_ADDR+32M-800*480*2
#define LAYER2_GRAM_ADDR    0xd1e89000  //BANK6_SDRAM_ADDR+32M-800*480*4

/*设置默认颜色*/
#define RGBLCD_Window_Default_Color_Set(color)	LTDC_Layer1->DCCR=color;LTDC->SRCR|=1

/*设置恒定Alpha值*/
#define RGBLCD_Const_Alpha_Set(alpha)			LTDC_Layer1->CACR|=alpha;LTDC->SRCR|=1

#define DMA2D_Get_TCIF()    ((DMA2D->ISR>>1)&1)

#define DMA2D_Clear_TCIF()  DMA2D->IFCR|=1<<1

void RGBLCD_ON(void);
void RGBLCD_OFF(void);

void RGBLCD_Init(void);
void RGBLCD_Window_Range_Set(u16 X, u16 Y, u16 Width, u16 Height);
void RGBLCD_Fill_Color(u16 X, u16 Y, u16 Width, u16 Height, u32 Color);
void RGBLCD_GRAM_Transfer(u16 X, u16 Y, u16 Width, u16 Height, void* Src, void* Dst);

#endif
