#include "RGBLCD.h"
#include "Framework.h"
#include "GPIO.h"

//#define FANKE_STM32F429
//#define APOLLO_STM32H743
#define FANKE_STM32H743

#define FANKE_800x480
//#define ZHENGDIAN_1024x600

#define ARGB8888 0
#define RGB888 1
#define RGB565 2
#define ARGB1555 3
#define ARGB4444 4
#define L8 5
#define AL44 6
#define AL88 7

#if defined FANKE_800x480

#define SCREEN_WIDTH 800
#define HS_PULSE_WIDTH 1
#define HS_BLANKING 80
#define HS_FRONT_PORCH 200

#define SCREEN_HEIGHT 480
#define VS_PULSE_WIDTH 1
#define VS_BLANKING 40
#define VS_FRONT_PORCH 22

#elif defined ZHENGDIAN_1024x600

#define SCREEN_WIDTH 1024
#define HS_PULSE_WIDTH 20
#define HS_BLANKING 140
#define HS_FRONT_PORCH 160

#define SCREEN_HEIGHT 600
#define VS_PULSE_WIDTH 3
#define VS_BLANKING 20
#define VS_FRONT_PORCH 12

#endif

#if defined FANKE_STM32H743

#define R0 GPIOG_13
#define R1 GPIOA_2
#define R2 GPIOH_8
#define R3 GPIOH_9
#define R4 GPIOH_10
#define R5 GPIOH_11
#define R6 GPIOH_12
#define R7 GPIOG_6

#define G0 GPIOE_5
#define G1 GPIOE_6
#define G2 GPIOH_13
#define G3 GPIOH_14
#define G4 GPIOH_15
#define G5 GPIOI_0
#define G6 GPIOI_1
#define G7 GPIOI_2

#define B0 GPIOG_14
#define B1 GPIOG_12
#define B2 GPIOD_6
#define B3 GPIOA_8
#define B4 GPIOI_4
#define B5 GPIOI_5
#define B6 GPIOI_6
#define B7 GPIOI_7

#define BL GPIOH_6

#define CLK GPIOG_7
#define DE GPIOF_10
#define VS GPIOI_9
#define HS GPIOI_10

#elif defined APOLLO_STM32H743

#define R0 GPIOH_9
#define R1 GPIOH_9
#define R2 GPIOH_9
#define R3 GPIOH_9
#define R4 GPIOH_10
#define R5 GPIOH_11
#define R6 GPIOH_12
#define R7 GPIOG_6

#define G0 GPIOH_13
#define G1 GPIOH_13
#define G2 GPIOH_13
#define G3 GPIOH_14
#define G4 GPIOH_15
#define G5 GPIOI_0
#define G6 GPIOI_1
#define G7 GPIOI_2

#define B0 GPIOG_11
#define B1 GPIOG_11
#define B2 GPIOG_11
#define B3 GPIOG_11
#define B4 GPIOI_4
#define B5 GPIOI_5
#define B6 GPIOI_6
#define B7 GPIOI_7

#define BL GPIOB_5

#define CLK GPIOG_7
#define DE GPIOF_10
#define VS GPIOI_9
#define HS GPIOI_10

#endif

#define LAYER1_PIXEL_FORMAT RGB565

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
u8 _RGBLCD_Clock_Init(u32 pll3n, u32 pll3m, u32 pll3r)
{
    u16 retry = 0;
    u8 status = 0;
    RCC->CR &= ~(1 << 28); //关闭 PLL3 时钟
    while (((RCC->CR & (1 << 29))) && (retry < 0X1FFF))
        retry++; //等待 PLL3 时钟失锁
    if (retry == 0X1FFF)
        status = 1; //PLL3 时钟关闭失败
    else
    {
        RCC->PLLCKSELR &= ~(0X3F << 20);    //清除 DIVM3[5:0]原来的设置
        RCC->PLLCKSELR |= pll3m << 20;      //DIVM3[5:0]=25,设置 PLL3 的预分频系数
        RCC->PLL3DIVR &= ~(0X1FF << 0);     //清除 DIVN3[8:0]原来的设置
        RCC->PLL3DIVR |= (pll3n - 1) << 0;  //DIVN3[8:0]=pll3n-1,设置 PLL3 的倍频系数
        RCC->PLL3DIVR &= ~(0X7F << 24);     //清除 DIVR2[6:0]原来的设置
        RCC->PLL3DIVR |= (pll3r - 1) << 24; //DIVR3[8:0]=pll3r-1,设置 PLL3 的分频系数
        RCC->PLLCFGR &= ~(0X0F << 8);       //清 PLL3RGE[1:0]/VCOSEL/FRACEN 设置
        RCC->PLLCFGR |= 0 << 10;            //PLL3RGE[1:0]=0,输入频率在 1~2Mhz 之间
        RCC->PLLCFGR |= 0 << 9;             //PLL3VCOSEL=0, VCO 范围,192~836Mhz
        RCC->PLLCFGR |= 1 << 24;            //DIVR3EN=1,使能 pll3_r_ck
        RCC->CR |= 1 << 28;                 //PLL3ON=1,使能 PLL3
        while (((RCC->CR & (1 << 29)) == 0) && (retry < 0X1FFF))
            retry++; //等待 PLL3 锁定
        if (retry == 0X1FFF)
            status = 2;
    }
    return status;
}

void RGBLCD_ON(void)
{
    GPIO_Write(BL, 1);
}

void RGBLCD_OFF(void)
{
    GPIO_Write(BL, 0);
}

void RGBLCD_Window_Range_Set(u16 X, u16 Y, u16 Width, u16 Height)
{
    u32 hor_save = 0, ver_save = 0;

    hor_save |= ((LTDC->BPCR & 0x0fff000) >> 16) + 1 + X;
    hor_save |= (((LTDC->BPCR & 0x0fff000) >> 16) + X + Width) << 16;
    ver_save |= (LTDC->BPCR & 0x7ff) + 1 + Y;
    ver_save |= ((LTDC->BPCR & 0x7ff) + Y + Height) << 16;
    LTDC_Layer1->WHPCR = hor_save;
    LTDC_Layer1->WVPCR = ver_save;
}

void RGBLCD_Init(void)
{
    u8 Layer1_Pixel_Byte, Layer2_Pixel_Byte;
    u32 save;

    RCC->APB3ENR |= 1 << 3;

#if defined FANKE_STM32F429

    GPIO_Set(GPIOH, PIN8 | PIN9 | PIN10 | PIN11 | PIN13 | PIN14 | PIN15,
             GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_HIGH, GPIO_PUPD_PU);
    GPIO_Set(GPIOI, PIN0 | PIN1 | PIN2 | PIN4 | PIN5 | PIN6 | PIN7 | PIN9 | PIN10,
             GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_HIGH, GPIO_PUPD_PU);
    GPIO_Set(GPIOD, PIN6,
             GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_HIGH, GPIO_PUPD_PU);
    GPIO_Set(GPIOD, PIN13,
             GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_LOW, GPIO_PUPD_PU);
    GPIO_Set(GPIOG, PIN6 | PIN7,
             GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_HIGH, GPIO_PUPD_PU);
    GPIO_Set(GPIOB, PIN1,
             GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_HIGH, GPIO_PUPD_PU);
    GPIO_Set(GPIOF, PIN10,
             GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_HIGH, GPIO_PUPD_PU);

    GPIO_AF_Set(GPIOH, 8, 14);
    GPIO_AF_Set(GPIOH, 9, 14);
    GPIO_AF_Set(GPIOH, 10, 14);
    GPIO_AF_Set(GPIOH, 11, 14);
    GPIO_AF_Set(GPIOB, 1, 14);
    GPIO_AF_Set(GPIOG, 6, 14);

    GPIO_AF_Set(GPIOH, 13, 14);
    GPIO_AF_Set(GPIOH, 14, 14);
    GPIO_AF_Set(GPIOH, 15, 14);
    GPIO_AF_Set(GPIOI, 0, 14);
    GPIO_AF_Set(GPIOI, 1, 14);
    GPIO_AF_Set(GPIOI, 2, 14);

    GPIO_AF_Set(GPIOD, 6, 14);
    GPIO_AF_Set(GPIOG, 11, 14);
    GPIO_AF_Set(GPIOI, 4, 14);
    GPIO_AF_Set(GPIOI, 5, 14);
    GPIO_AF_Set(GPIOI, 6, 14);
    GPIO_AF_Set(GPIOI, 7, 14);

    GPIO_AF_Set(GPIOG, 7, 14);
    GPIO_AF_Set(GPIOF, 10, 14);
    GPIO_AF_Set(GPIOI, 9, 14);
    GPIO_AF_Set(GPIOI, 10, 14);

#elif defined APOLLO_STM32H743

    GPIO_Set(R3, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(R4, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(R5, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(R6, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(R7, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);

    GPIO_Set(G2, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(G3, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(G4, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(G5, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(G6, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(G7, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);

    GPIO_Set(B3, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(B4, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(B5, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(B6, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(B7, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);

    GPIO_Set(BL, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);

    GPIO_Set(CLK, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(DE, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(VS, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(HS, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);

    GPIO_AF_Set(R3, 14);
    GPIO_AF_Set(R4, 14);
    GPIO_AF_Set(R5, 14);
    GPIO_AF_Set(R6, 14);
    GPIO_AF_Set(R7, 14);

    GPIO_AF_Set(G2, 14);
    GPIO_AF_Set(G3, 14);
    GPIO_AF_Set(G4, 14);
    GPIO_AF_Set(G5, 14);
    GPIO_AF_Set(G6, 14);
    GPIO_AF_Set(G7, 14);

    GPIO_AF_Set(B3, 14);
    GPIO_AF_Set(B4, 14);
    GPIO_AF_Set(B5, 14);
    GPIO_AF_Set(B6, 14);
    GPIO_AF_Set(B7, 14);

    GPIO_AF_Set(CLK, 14);
    GPIO_AF_Set(DE, 14);
    GPIO_AF_Set(VS, 14);
    GPIO_AF_Set(HS, 14);

#elif defined FANKE_STM32H743

    GPIO_Set(R0, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(R1, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(R2, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(R3, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(R4, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(R5, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(R6, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(R7, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);

    GPIO_Set(G0, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(G1, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(G2, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(G3, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(G4, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(G5, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(G6, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(G7, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);

    GPIO_Set(B0, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(B1, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(B2, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(B3, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(B4, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(B5, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(B6, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(B7, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);

    GPIO_Set(BL, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);

    GPIO_Set(CLK, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(DE, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(VS, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);
    GPIO_Set(HS, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_PU);

    GPIO_AF_Set(R3, 14);
    GPIO_AF_Set(R4, 14);
    GPIO_AF_Set(R5, 14);
    GPIO_AF_Set(R6, 14);
    GPIO_AF_Set(R7, 14);

    GPIO_AF_Set(G2, 14);
    GPIO_AF_Set(G3, 14);
    GPIO_AF_Set(G4, 14);
    GPIO_AF_Set(G5, 14);
    GPIO_AF_Set(G6, 14);
    GPIO_AF_Set(G7, 14);

    GPIO_AF_Set(B3, 14);
    GPIO_AF_Set(B4, 14);
    GPIO_AF_Set(B5, 14);
    GPIO_AF_Set(B6, 14);
    GPIO_AF_Set(B7, 14);

    GPIO_AF_Set(CLK, 14);
    GPIO_AF_Set(DE, 14);
    GPIO_AF_Set(VS, 14);
    GPIO_AF_Set(HS, 14);

    GPIO_Write(R0, 0);
    GPIO_Write(R1, 0);
    GPIO_Write(R2, 0);

    GPIO_Write(G0, 0);
    GPIO_Write(G1, 0);

    GPIO_Write(B0, 0);
    GPIO_Write(B1, 0);
    GPIO_Write(B2, 0);

#endif

    if (LAYER1_PIXEL_FORMAT == ARGB8888)
        Layer1_Pixel_Byte = 4;
    else if (LAYER1_PIXEL_FORMAT == RGB888)
        Layer1_Pixel_Byte = 3;
    else if (LAYER1_PIXEL_FORMAT == RGB565)
        Layer1_Pixel_Byte = 2;
    else if (LAYER1_PIXEL_FORMAT == ARGB1555)
        Layer1_Pixel_Byte = 2;
    else if (LAYER1_PIXEL_FORMAT == ARGB4444)
        Layer1_Pixel_Byte = 2;
    else
        Layer1_Pixel_Byte = 1;

    _RGBLCD_Clock_Init(300, 25, 6);

    save = ((HS_PULSE_WIDTH - 1) << 16) + VS_PULSE_WIDTH - 1;
    ;
    LTDC->SSCR = save;

    save = ((HS_PULSE_WIDTH + HS_BLANKING - 1) << 16) + VS_PULSE_WIDTH + VS_BLANKING - 1;
    LTDC->BPCR = save;

    save = ((HS_PULSE_WIDTH + HS_BLANKING + SCREEN_WIDTH - 1) << 16) + VS_PULSE_WIDTH + VS_BLANKING + SCREEN_HEIGHT - 1;
    LTDC->AWCR = save;

    save = ((HS_PULSE_WIDTH + HS_BLANKING + SCREEN_WIDTH + HS_FRONT_PORCH - 1) << 16) + VS_PULSE_WIDTH + VS_BLANKING + SCREEN_HEIGHT + VS_FRONT_PORCH - 1;
    LTDC->TWCR = save;

    LTDC->BCCR |= 0xff0000;

    LTDC_Layer1->CACR = 0xff;

    save = (4 << 8) + 5;
    LTDC_Layer1->BFCR = save;

    RGBLCD_Window_Default_Color_Set(0xff000000);

    LTDC_Layer1->CFBAR = LAYER1_GRAM_ADDR;

    save = ((SCREEN_WIDTH * Layer1_Pixel_Byte) << 16) + SCREEN_WIDTH * Layer1_Pixel_Byte + 3;
    LTDC_Layer1->CFBLR = save;

    LTDC_Layer1->CFBLNR = SCREEN_HEIGHT;

    LTDC_Layer1->PFCR |= LAYER1_PIXEL_FORMAT;

    RGBLCD_Window_Range_Set(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    LTDC_Layer1->CR |= 1;

    LTDC->SRCR |= 1 << 1;
    LTDC->GCR |= 1;

    RGBLCD_OFF();

    RCC->AHB3ENR |= 1 << 4;               //使能DMA2D时钟
    DMA2D->FGOR = 0;                      //前景层偏移为0
    DMA2D->OPFCCR = LAYER1_PIXEL_FORMAT;  //输出颜色格式
    DMA2D->FGPFCCR = LAYER1_PIXEL_FORMAT; //前景层颜色格式
}

void RGBLCD_Fill_Color(u16 X, u16 Y, u16 Width, u16 Height, u32 Color)
{
    u32 save = 0;

    while ((DMA2D->CR & 1) == 1)
        ;
    DMA2D->IFCR |= 1 << 1;
    DMA2D->CR |= 3 << 16;
    DMA2D->OCOLR = Color;
    DMA2D->OOR = SCREEN_WIDTH - Width;
    save = (Width << 16) + Height;
    DMA2D->NLR = save;
    DMA2D->OMAR = LAYER1_GRAM_ADDR + Y * SCREEN_WIDTH * 2 + X * 2;
    DMA2D->CR |= 1;
}

void RGBLCD_GRAM_Transfer(u16 X, u16 Y, u16 Width, u16 Height, void *Src, void *Dst)
{
    u32 save = 0;

    while ((DMA2D->CR & 1) == 1)
        ;
    DMA2D->IFCR |= 1 << 1;
    DMA2D->CR &= ~(3 << 16);
    DMA2D->FGMAR = (u32)Src;
    DMA2D->OOR = SCREEN_WIDTH - Width;
    DMA2D->OMAR = (u32)Dst + Y * SCREEN_WIDTH * 2 + X * 2;
    save = (Width << 16) + Height;
    DMA2D->NLR = save;
    DMA2D_Clear_TCIF();
    DMA2D->CR |= 1;
    while ((DMA2D->CR & 1) == 1)
        ;
    DMA2D_Clear_TCIF();
}

void RGBLCD_GRAM_Transfer_Begin(u16 X, u16 Y, u16 Width, u16 Height, void *Src, void *Dst)
{
    u32 save = 0;

    while ((DMA2D->CR & 1) == 1)
        ;
    DMA2D->IFCR |= 1 << 1;
    DMA2D->CR &= ~(3 << 16);
    DMA2D->FGMAR = (u32)Src;
    DMA2D->OOR = SCREEN_WIDTH - Width;
    DMA2D->OMAR = (u32)Dst + Y * SCREEN_WIDTH * 2 + X * 2;
    save = (Width << 16) + Height;
    DMA2D->NLR = save;
    DMA2D_Clear_TCIF();
    DMA2D->CR |= 1;
}
