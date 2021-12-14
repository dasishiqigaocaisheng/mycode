#include "RGBLCD.h"
#include "Framework.h"

#define ARGB8888	0
#define RGB888		1
#define RGB565		2
#define ARGB1555	3
#define ARGB4444	4
#define L8			5
#define AL44		6
#define AL88		7

#define LAYER1_GRAM_ADDR	0xd1f44800  //BANK6_SDRAM_ADDR+32M-800*480*2

#define SCREEN_WIDTH	800
#define HS_PULSE_WIDTH	1
#define HS_BLANKING		80
#define HS_FRONT_PORCH	200

#define SCREEN_HEIGHT	480
#define VS_PULSE_WIDTH	1
#define VS_BLANKING		40
#define VS_FRONT_PORCH	22

#define LAYER1_PIXEL_FORMAT	RGB565

#define LCD_BL	GPIO_Write(GPIOD_13)

/*LTDC时钟初始化*/
u8 _RGBLCD_Clock_Init(u32 pllsain, u32 pllsair, u32 pllsaidivr);

void RGBLCD_ON(void)
{
	LCD_BL=1;
}

void RGBLCD_OFF(void)
{
	LCD_BL=0;
}

//LTDC时钟(Fdclk)设置函数
//Fvco=Fin*pllsain; 
//Fdclk=Fvco/pllsair/2*2^pllsaidivr=Fin*pllsain/pllsair/2*2^pllsaidivr;
//Fvco:VCO频率
//Fin:输入时钟频率一般为1Mhz(来自系统时钟PLLM分频后的时钟,见时钟树图)
//pllsain:SAI时钟倍频系数N,取值范围:50~432.  
//pllsair:SAI时钟的分频系数R,取值范围:2~7
//pllsaidivr:LCD时钟分频系数,取值范围:0~3,对应分频2^(pllsaidivr+1) 
//假设:外部晶振为25M,pllm=25的时候,Fin=1Mhz.
//例如:要得到20M的LTDC时钟,则可以设置:pllsain=400,pllsair=5,pllsaidivr=1
//Fdclk=1*396/3/2*2^1=396/12=33Mhz
//返回值:0,成功;1,失败。
u8 _RGBLCD_Clock_Init(u32 pllsain,u32 pllsair,u32 pllsaidivr)
{ 
	u16 retry=0;
	u8 status=0;
	u32 tempreg=0;
	RCC->CR&=~(1<<28);					//关闭SAI时钟 
	while(((RCC->CR&(1<<29)))&&(retry<0X1FFF))retry++;//等待SAI时钟失锁
 	if(retry==0X1FFF)status=1;			//LTDC时钟关闭失败
	else   
	{ 
		tempreg|=pllsain<<6;
		tempreg|=pllsair<<28;
		RCC->PLLSAICFGR=tempreg;		//设置LTDC的倍频和分频
		RCC->DCKCFGR&=~(3<<16);			//清除原来的设置.
		RCC->DCKCFGR|=pllsaidivr<<16;	//设置fdclk分频

		RCC->CR|=1<<28;					//开启SAI时钟 
		while(((RCC->CR&(1<<29))==0)&&(retry<0X1FFF))retry++;//等待SAI时钟锁定
		if(retry==0X1FFF)status=2;	
 	} 
	return status;
} 

void RGBLCD_Window_Range_Set(u16 X, u16 Y, u16 Width, u16 Height)
{
	u32 hor_save=0,ver_save=0;
	
	hor_save|=((LTDC->BPCR&0x0fff000)>>16)+1+X;
	hor_save|=(((LTDC->BPCR&0x0fff000)>>16)+X+Width)<<16;
	ver_save|=(LTDC->BPCR&0x7ff)+1+Y;
	ver_save|=((LTDC->BPCR&0x7ff)+Y+Height)<<16;
	LTDC_Layer1->WHPCR=hor_save;
	LTDC_Layer1->WVPCR=ver_save;

}

void RGBLCD_Init(void)
{
	u8 Layer1_Pixel_Byte,Layer2_Pixel_Byte;
	u32 save;
	
	RCC->APB2ENR|=1<<26;			//开启LTDC时钟
	RCC->AHB1ENR|=1<<1;				//使能PORTB时钟 
	RCC->AHB1ENR|=1<<3;				//使能PORTD时钟
	RCC->AHB1ENR|=0XF<<5; 			//使能PF/PG/PH/PI时钟
	
	GPIO_Set(GPIOH,PIN8|PIN9|PIN10|PIN11|PIN13|PIN14|PIN15,
				GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);
	GPIO_Set(GPIOI,PIN0|PIN1|PIN2|PIN4|PIN5|PIN6|PIN7|PIN9|PIN10,
				GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);
	GPIO_Set(GPIOD,PIN6,
				GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);
	GPIO_Set(GPIOD,PIN13,
				GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_25M,GPIO_PUPD_PU);
	GPIO_Set(GPIOG,PIN6|PIN7,
				GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);
	GPIO_Set(GPIOB,PIN1,
				GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);
	GPIO_Set(GPIOF,PIN10,
				GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);
	
	GPIO_AF_Set(GPIOH,8,14);
	GPIO_AF_Set(GPIOH,9,14);
	GPIO_AF_Set(GPIOH,10,14);
	GPIO_AF_Set(GPIOH,11,14);
	GPIO_AF_Set(GPIOB,1,14);
	GPIO_AF_Set(GPIOG,6,14);
	
	GPIO_AF_Set(GPIOH,13,14);
	GPIO_AF_Set(GPIOH,14,14);
	GPIO_AF_Set(GPIOH,15,14);
	GPIO_AF_Set(GPIOI,0,14);
	GPIO_AF_Set(GPIOI,1,14);
	GPIO_AF_Set(GPIOI,2,14);
	
	GPIO_AF_Set(GPIOD,6,14);
	GPIO_AF_Set(GPIOG,11,14);
	GPIO_AF_Set(GPIOI,4,14);
	GPIO_AF_Set(GPIOI,5,14);
	GPIO_AF_Set(GPIOI,6,14);
	GPIO_AF_Set(GPIOI,7,14);
	
	GPIO_AF_Set(GPIOG,7,14);
	GPIO_AF_Set(GPIOF,10,14);
	GPIO_AF_Set(GPIOI,9,14);
	GPIO_AF_Set(GPIOI,10,14);
	
	if (LAYER1_PIXEL_FORMAT==ARGB8888)
		Layer1_Pixel_Byte=4;
	else if (LAYER1_PIXEL_FORMAT==RGB888)
		Layer1_Pixel_Byte=3;
	else if (LAYER1_PIXEL_FORMAT==RGB565)
		Layer1_Pixel_Byte=2;
	else if (LAYER1_PIXEL_FORMAT==ARGB1555)
		Layer1_Pixel_Byte=2;
	else if (LAYER1_PIXEL_FORMAT==ARGB4444)
		Layer1_Pixel_Byte=2;
	else
		Layer1_Pixel_Byte=1;
	
	_RGBLCD_Clock_Init(396,3,1);
	
	save=((HS_PULSE_WIDTH-1)<<16)+VS_PULSE_WIDTH-1;;
	LTDC->SSCR=save;
	
	save=((HS_PULSE_WIDTH+HS_BLANKING-1)<<16)+VS_PULSE_WIDTH+VS_BLANKING-1;
	LTDC->BPCR=save;
	
	save=((HS_PULSE_WIDTH+HS_BLANKING+SCREEN_WIDTH-1)<<16)+VS_PULSE_WIDTH+VS_BLANKING+SCREEN_HEIGHT-1;
	LTDC->AWCR=save;
	
	save=((HS_PULSE_WIDTH+HS_BLANKING+SCREEN_WIDTH+HS_FRONT_PORCH-1)<<16)+VS_PULSE_WIDTH+VS_BLANKING+SCREEN_HEIGHT+VS_FRONT_PORCH-1;
	LTDC->TWCR=save;
	
	LTDC->BCCR|=0xff0000;
	
	LTDC_Layer1->CACR=0xff;
	
	save=(4<<8)+5;
	LTDC_Layer1->BFCR=save;
	
	RGBLCD_Window_Default_Color_Set(0xff000000);
	
	LTDC_Layer1->CFBAR=LAYER1_GRAM_ADDR;
	
	save=((SCREEN_WIDTH*Layer1_Pixel_Byte)<<16)+SCREEN_WIDTH*Layer1_Pixel_Byte+3;
	LTDC_Layer1->CFBLR=save;
	
	LTDC_Layer1->CFBLNR=SCREEN_HEIGHT;
	
	LTDC_Layer1->PFCR|=LAYER1_PIXEL_FORMAT;
	
	RGBLCD_Window_Range_Set(0,0,800,480);

	LTDC_Layer1->CR|=1;

	LTDC->SRCR|=1<<1;
	LTDC->GCR|=1;
	
	RGBLCD_OFF();
	
	RCC->AHB1ENR|=1<<23;	//使能DMA2D时钟
	DMA2D->FGOR=0;			//前景层偏移为0
	DMA2D->OPFCCR=LAYER1_PIXEL_FORMAT;	//输出颜色格式
	DMA2D->FGPFCCR=LAYER1_PIXEL_FORMAT;	//前景层颜色格式
}

void RGBLCD_Fill_Color(u16 X, u16 Y, u16 Width, u16 Height, u32 Color)
{
	u32 save=0;
	
	while ((DMA2D->CR&1)==1);
	DMA2D->IFCR|=1<<1;
	DMA2D->CR|=3<<16;
	DMA2D->OCOLR=Color;
	DMA2D->OOR=SCREEN_WIDTH-Width;
	save=(Width<<16)+Height;
	DMA2D->NLR=save;
	DMA2D->OMAR=LAYER1_GRAM_ADDR+Y*SCREEN_WIDTH*2+X*2;
    DMA2D_Clear_TCIF();
	DMA2D->CR|=1;
}

void RGBLCD_GRAM_Transfer(u16 X, u16 Y, u16 Width, u16 Height, void* Src, void* Dst)
{
	u32 save=0;

    while ((DMA2D->CR&1)==1);
	DMA2D->IFCR|=1<<1;
	DMA2D->CR&=~(3<<16);
	DMA2D->FGMAR=(u32)Src;
	DMA2D->OOR=SCREEN_WIDTH-Width;
	DMA2D->OMAR=(u32)Dst+Y*SCREEN_WIDTH*2+X*2;
	save=(Width<<16)+Height;
	DMA2D->NLR=save;
    DMA2D_Clear_TCIF();
	DMA2D->CR|=1;
    //while ((DMA2D->CR&1)==1);
}
