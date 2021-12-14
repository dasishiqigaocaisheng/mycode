#ifndef _RGBLCD_H_
#define _RGBLCD_H_
#include "sys.h"


/*设置默认颜色*/
#define RGBLCD_Window_Default_Color_Set(color)	LTDC_Layer1->DCCR=color;LTDC->SRCR|=1

/*设置恒定Alpha值*/
#define RGBLCD_Const_Alpha_Set(alpha)			LTDC_Layer1->CACR|=alpha;LTDC->SRCR|=1


void RGBLCD_ON(void);
void RGBLCD_OFF(void);

void RGBLCD_Init(void);
void RGBLCD_Window_Range_Set(u16 X, u16 Y, u16 Width, u16 Height);
void RGBLCD_Fill_Color(u16 X, u16 Y, u16 Width, u16 Height, u32 Color);
void RGBLCD_GRAM_Transfer(u16 X, u16 Y, u16 Width, u16 Height, void* Addr);

#endif
