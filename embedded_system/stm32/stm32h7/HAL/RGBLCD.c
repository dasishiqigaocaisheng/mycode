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
/*LTDCʱ�ӳ�ʼ��*/
u8 _RGBLCD_Clock_Init(u32 pllsain, u32 pllsair, u32 pllsaidivr);

//LTDCʱ��(Fdclk)���ú���
//Fvco=Fin*pllsain; 
//Fdclk=Fvco/pllsair/2*2^pllsaidivr=Fin*pllsain/pllsair/2*2^pllsaidivr;
//Fvco:VCOƵ��
//Fin:����ʱ��Ƶ��һ��Ϊ1Mhz(����ϵͳʱ��PLLM��Ƶ���ʱ��,��ʱ����ͼ)
//pllsain:SAIʱ�ӱ�Ƶϵ��N,ȡֵ��Χ:50~432.  
//pllsair:SAIʱ�ӵķ�Ƶϵ��R,ȡֵ��Χ:2~7
//pllsaidivr:LCDʱ�ӷ�Ƶϵ��,ȡֵ��Χ:0~3,��Ӧ��Ƶ2^(pllsaidivr+1) 
//����:�ⲿ����Ϊ25M,pllm=25��ʱ��,Fin=1Mhz.
//����:Ҫ�õ�20M��LTDCʱ��,���������:pllsain=400,pllsair=5,pllsaidivr=1
//Fdclk=1*396/3/2*2^1=396/12=33Mhz
//����ֵ:0,�ɹ�;1,ʧ�ܡ�
u8 _RGBLCD_Clock_Init(u32 pllsain,u32 pllsair,u32 pllsaidivr)
{ 
	u16 retry=0;
	u8 status=0;
	u32 tempreg=0;
	RCC->CR&=~(1<<28);					//�ر�SAIʱ�� 
	while(((RCC->CR&(1<<29)))&&(retry<0X1FFF))retry++;//�ȴ�SAIʱ��ʧ��
 	if(retry==0X1FFF)status=1;			//LTDCʱ�ӹر�ʧ��
	else   
	{ 
		tempreg|=pllsain<<6;
		tempreg|=pllsair<<28;
		RCC->PLLSAICFGR=tempreg;		//����LTDC�ı�Ƶ�ͷ�Ƶ
		RCC->DCKCFGR&=~(3<<16);			//���ԭ��������.
		RCC->DCKCFGR|=pllsaidivr<<16;	//����fdclk��Ƶ

		RCC->CR|=1<<28;					//����SAIʱ�� 
		while(((RCC->CR&(1<<29))==0)&&(retry<0X1FFF))retry++;//�ȴ�SAIʱ������
		if(retry==0X1FFF)status=2;	
 	} 
	return status;
} 
#else
//LTDC ʱ��(Fdclk)���ú���
//Fvco=Fs*(pll3n/pll3m);
//Fr=Fvco/pll3r=Fs*(pll3n/(pll3m*pll3r));
//Fdclk=Fr;
//Fvco:VCO Ƶ��
//Fr:PLL3 �� r ��Ƶ���ʱ��Ƶ��
//Fs:PLL3 ����ʱ��Ƶ��,������ HSI,CSI,HSE ��.
//Fdclk:LTDC ������ʱ��
//pll3n:PLL3 ��Ƶϵ��(PLL ��Ƶ),ȡֵ��Χ:4~512.
//pll3m:PLL3 Ԥ��Ƶϵ��(�� PLL ֮ǰ�ķ�Ƶ),ȡֵ��Χ:2~63.
//pll3r:PLL3 �� r ��Ƶϵ��(PLL ֮��ķ�Ƶ),ȡֵ��Χ:1~128.
//��Ϊ LTDC �� SAI ���� pll3,����,��ʹ�� SAI ��ʱ��,���л���ͬ�� pll3n(һ���
//���� 300),���� LTDC �� plln Ҳ�仯��,�����������Ϊ 300,�õ�������ʱ����
//���� SAI �޸� pll3n ��ʱ��,ֻ���,����С.���ή�� LCD ��ˢ��Ƶ��.
//���,LTDC,����һ������ pll3m=25,pll3n=300,����,���Եõ� Fvco=300Mhz 
//Ȼ��,ֻ��Ҫͨ���޸� pll3r,��ƥ�䲻ͬ��Һ����ʱ�򼴿�.
//����:Ҫ�õ� 33M �� LTDC ʱ��,���������:pll3n=300,pll3m=25,pll3r=9
//Fdclk=300*(25/25)/9=300/9=33.33Mhz
//����ֵ:0,�ɹ�;1,ʧ�ܡ�
u8 _RGBLCD_Clock_Init(u32 pll3n,u32 pll3m,u32 pll3r)
{
    u16 retry=0;
    u8 status=0;
    RCC->CR&=~(1<<28); //�ر� PLL3 ʱ��
    while(((RCC->CR&(1<<29)))&&(retry<0X1FFF))retry++;//�ȴ� PLL3 ʱ��ʧ��
    if(retry==0X1FFF)
        status=1; //PLL3 ʱ�ӹر�ʧ��
    else
    {
        RCC->PLLCKSELR&=~(0X3F<<20); //��� DIVM3[5:0]ԭ��������
        RCC->PLLCKSELR|=pll3m<<20; //DIVM3[5:0]=25,���� PLL3 ��Ԥ��Ƶϵ��
        RCC->PLL3DIVR&=~(0X1FF<<0); //��� DIVN3[8:0]ԭ��������
        RCC->PLL3DIVR|=(pll3n-1)<<0; //DIVN3[8:0]=pll3n-1,���� PLL3 �ı�Ƶϵ��
        RCC->PLL3DIVR&=~(0X7F<<24); //��� DIVR2[6:0]ԭ��������
        RCC->PLL3DIVR|=(pll3r-1)<<24; //DIVR3[8:0]=pll3r-1,���� PLL3 �ķ�Ƶϵ��
        RCC->PLLCFGR&=~(0X0F<<8); //�� PLL3RGE[1:0]/VCOSEL/FRACEN ����
        RCC->PLLCFGR|=0<<10; //PLL3RGE[1:0]=0,����Ƶ���� 1~2Mhz ֮��
        RCC->PLLCFGR|=0<<9; //PLL3VCOSEL=0, VCO ��Χ,192~836Mhz
        RCC->PLLCFGR|=1<<24; //DIVR3EN=1,ʹ�� pll3_r_ck
        RCC->CR|=1<<28; //PLL3ON=1,ʹ�� PLL3
        while(((RCC->CR&(1<<29))==0)&&(retry<0X1FFF))
            retry++; //�ȴ� PLL3 ����
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
	RCC->APB2ENR|=1<<26;			//����LTDCʱ��
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
    
    GPIO_Set(GPIOB,PIN5,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_MID,GPIO_PUPD_PU);				//PB5 �������,���Ʊ���
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
	RCC->AHB1ENR|=1<<23;	//ʹ��DMA2Dʱ��
    #else
    RCC->AHB3ENR|=1<<4;	
    #endif
	DMA2D->FGOR=0;			//ǰ����ƫ��Ϊ0
	DMA2D->OPFCCR=LAYER1_PIXEL_FORMAT;	//�����ɫ��ʽ
	DMA2D->FGPFCCR=LAYER1_PIXEL_FORMAT;	//ǰ������ɫ��ʽ
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
