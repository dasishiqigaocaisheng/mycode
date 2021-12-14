#include "RGBLCD.h"
#include "Framework.h"

#define ZHENGDIAN_1024x600

#define ARGB8888	0
#define RGB888		1
#define RGB565		2
#define ARGB1555	3
#define ARGB4444	4
#define L8			5
#define AL44		6
#define AL88		7

#define LAYER1_GRAM_ADDR	0xd1f44800  //BANK6_SDRAM_ADDR+32M-800*480*2

#if defined FANKE_800x480
#define SCREEN_WIDTH	800
#define HS_PULSE_WIDTH	1
#define HS_BLANKING		80
#define HS_FRONT_PORCH	200

#define SCREEN_HEIGHT	480
#define VS_PULSE_WIDTH	1
#define VS_BLANKING		40
#define VS_FRONT_PORCH	22
#elif defined ZHENGDIAN_1024x600
#define SCREEN_WIDTH	1024
#define HS_PULSE_WIDTH	20
#define HS_BLANKING		140
#define HS_FRONT_PORCH	160

#define SCREEN_HEIGHT	600
#define VS_PULSE_WIDTH	3
#define VS_BLANKING		20
#define VS_FRONT_PORCH	12
#endif

#define LAYER1_PIXEL_FORMAT	RGB565

#if defined FANKE_STM32F429
#define LCD_BL_PIN      GPIOD_13
#elif defined APOLLO_STM32H743
#define LCD_BL_PIN      GPIOB_5
#endif

#if !defined USE_STM32H7
/*LTDC时钟初始化*/
u8 _RGBLCD_Clock_Init(u32 pllsain, u32 pllsair, u32 pllsaidivr);

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
#else
//LTDC 时钟(Fdclk)设置函数
//Fvco=Fs*(pll3n/pll3m);
//Fr=Fvco/pll3r=Fs*(pll3n/(pll3m*pll3r));
//Fdclk=Fr;
//Fvco:VCO 频率
//Fr:PLL3 的 r 分频输出时钟频率
//Fs:PLL3 输入时钟频率,可以是 HSI,CSI,HSE 等.
//Fdclk:LTDC 的输入时钟
//pll3n:PLL3 倍频系数(PLL 倍频),取值范围:4~512.
//pll3m:PLL3 预分频系数(进 PLL 之前的分频),取值范围:2~63.
//pll3r:PLL3 的 r 分频系数(PLL 之后的分频),取值范围:1~128.
//因为 LTDC 和 SAI 共用 pll3,所以,当使用 SAI 的时候,会切换不同的 pll3n(一般会
//大于 300),导致 LTDC 的 plln 也变化了,因此这里设置为 300,得到的像素时钟在
//后面 SAI 修改 pll3n 的时候,只会大,不会小.不会降低 LCD 的刷新频率.
//因此,LTDC,我们一般设置 pll3m=25,pll3n=300,这样,可以得到 Fvco=300Mhz 
//然后,只需要通过修改 pll3r,来匹配不同的液晶屏时序即可.
//例如:要得到 33M 的 LTDC 时钟,则可以设置:pll3n=300,pll3m=25,pll3r=9
//Fdclk=300*(25/25)/9=300/9=33.33Mhz
//返回值:0,成功;1,失败。
u8 _RGBLCD_Clock_Init(u32 pll3n,u32 pll3m,u32 pll3r)
{
    u16 retry=0;
    u8 status=0;
    RCC->CR&=~(1<<28); //关闭 PLL3 时钟
    while(((RCC->CR&(1<<29)))&&(retry<0X1FFF))retry++;//等待 PLL3 时钟失锁
    if(retry==0X1FFF)
        status=1; //PLL3 时钟关闭失败
    else
    {
        RCC->PLLCKSELR&=~(0X3F<<20); //清除 DIVM3[5:0]原来的设置
        RCC->PLLCKSELR|=pll3m<<20; //DIVM3[5:0]=25,设置 PLL3 的预分频系数
        RCC->PLL3DIVR&=~(0X1FF<<0); //清除 DIVN3[8:0]原来的设置
        RCC->PLL3DIVR|=(pll3n-1)<<0; //DIVN3[8:0]=pll3n-1,设置 PLL3 的倍频系数
        RCC->PLL3DIVR&=~(0X7F<<24); //清除 DIVR2[6:0]原来的设置
        RCC->PLL3DIVR|=(pll3r-1)<<24; //DIVR3[8:0]=pll3r-1,设置 PLL3 的分频系数
        RCC->PLLCFGR&=~(0X0F<<8); //清 PLL3RGE[1:0]/VCOSEL/FRACEN 设置
        RCC->PLLCFGR|=0<<10; //PLL3RGE[1:0]=0,输入频率在 1~2Mhz 之间
        RCC->PLLCFGR|=0<<9; //PLL3VCOSEL=0, VCO 范围,192~836Mhz
        RCC->PLLCFGR|=1<<24; //DIVR3EN=1,使能 pll3_r_ck
        RCC->CR|=1<<28; //PLL3ON=1,使能 PLL3
        while(((RCC->CR&(1<<29))==0)&&(retry<0X1FFF))
            retry++; //等待 PLL3 锁定
        if(retry==0X1FFF)
            status=2;
    }
    return status;
} 
#endif

#if !defined USE_STM32H7
void RGBLCD_ON(void)
{
	GPIO_Write(LCD_BL_PIN)=1;
}

void RGBLCD_OFF(void)
{
	GPIO_Write(LCD_BL_PIN)=0;
}
#else
void RGBLCD_ON(void)
{
	GPIO_Write(LCD_BL_PIN,1);
}

void RGBLCD_OFF(void)
{
	GPIO_Write(LCD_BL_PIN,0);
}

#endif


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
	
    #if !defined USE_STM32H7
	RCC->APB2ENR|=1<<26;			//开启LTDC时钟
	#else
    RCC->APB3ENR|=1<<3;
    #endif
    
	#if defined FANKE_STM32F429
	GPIO_Set(GPIOH,PIN8|PIN9|PIN10|PIN11|PIN13|PIN14|PIN15,
				GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_PU);
	GPIO_Set(GPIOI,PIN0|PIN1|PIN2|PIN4|PIN5|PIN6|PIN7|PIN9|PIN10,
				GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_PU);
	GPIO_Set(GPIOD,PIN6,
				GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_PU);
	GPIO_Set(GPIOD,PIN13,
				GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_LOW,GPIO_PUPD_PU);
	GPIO_Set(GPIOG,PIN6|PIN7,
				GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_PU);
	GPIO_Set(GPIOB,PIN1,
				GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_PU);
	GPIO_Set(GPIOF,PIN10,
				GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_PU);
	
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
	#elif defined APOLLO_STM32H743
    
    GPIO_Set(GPIOB,PIN5,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_MID,GPIO_PUPD_PU);				//PB5 推挽输出,控制背光
	GPIO_Set(GPIOF,PIN10,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_PU);				//PF10		
	GPIO_Set(GPIOG,3<<6|1<<11,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_PU);			//PG6/7/11
	GPIO_Set(GPIOH,0X7F<<9,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_PU);			//PH9~15
	GPIO_Set(GPIOI,7<<0|0XF<<4|3<<9,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_PU);	//PI0~2/4~7/9/10
	 
 	GPIO_AF_Set(GPIOF,10,14);		//PF10,AF14
	
 	GPIO_AF_Set(GPIOG,6,14);		//PG6,AF14
 	GPIO_AF_Set(GPIOG,7,14);		//PG7,AF14
 	GPIO_AF_Set(GPIOG,11,14);		//PG11,AF14
	
 	GPIO_AF_Set(GPIOH,9,14);		//PH9,AF14
 	GPIO_AF_Set(GPIOH,10,14);		//PH10,AF14
 	GPIO_AF_Set(GPIOH,11,14);		//PH11,AF14
 	GPIO_AF_Set(GPIOH,12,14);		//PH12,AF14
 	GPIO_AF_Set(GPIOH,13,14);		//PH13,AF14
 	GPIO_AF_Set(GPIOH,14,14);		//PH14,AF14
 	GPIO_AF_Set(GPIOH,15,14);		//PH15,AF14
	
 	GPIO_AF_Set(GPIOI,0,14);		//PI0,AF14
 	GPIO_AF_Set(GPIOI,1,14);		//PI1,AF14
 	GPIO_AF_Set(GPIOI,2,14);		//PI2,AF14
 	GPIO_AF_Set(GPIOI,4,14);		//PI4,AF14
 	GPIO_AF_Set(GPIOI,5,14);		//PI5,AF14
 	GPIO_AF_Set(GPIOI,6,14);		//PI6,AF14
 	GPIO_AF_Set(GPIOI,7,14);		//PI7,AF14
 	GPIO_AF_Set(GPIOI,9,14);		//PI9,AF14
 	GPIO_AF_Set(GPIOI,10,14);		//PI10,AF14
    
    #endif
    
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
	
    #if !defined USE_STM32H7
	_RGBLCD_Clock_Init(396,3,1);
	#else
    _RGBLCD_Clock_Init(300,25,6);
    #endif
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
	
    #if !defined USE_STM32H7
	RCC->AHB1ENR|=1<<23;	//使能DMA2D时钟
    #else
    RCC->AHB3ENR|=1<<4;	
    #endif
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
	DMA2D->CR|=1;
}

void RGBLCD_GRAM_Transfer(u16 X, u16 Y, u16 Width, u16 Height, void* Addr)
{
	u32 save=0;
	
	
	DMA2D->IFCR|=1<<1;
	DMA2D->CR&=~(3<<16);
	DMA2D->FGMAR=(u32)Addr;
	DMA2D->OOR=SCREEN_WIDTH-Width;
	DMA2D->OMAR=LAYER1_GRAM_ADDR+Y*SCREEN_WIDTH*2+X*2;
	save=(Width<<16)+Height;
	DMA2D->NLR=save;
	DMA2D->CR|=1;
    while ((DMA2D->CR&1)==1);
}
