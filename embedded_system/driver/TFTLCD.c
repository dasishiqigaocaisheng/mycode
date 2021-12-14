#include "delay.h"
#include "sys.h"
#include "TFTLCD.h"
#include "F407GPIO.h"
#include "Memory.h"
#include "MathHelper.h"
#include <string.h>

#ifdef TFTLCD_DEBUG
#include "USART.h"
#endif

extern _GPIO_typedef F407GPIO;
extern _PIN_typedef  F407PIN;

#define TFTLCD_RST_GPIO 	F407GPIO.B
#define TFTLCD_RST_PIN  	F407PIN.P0
#define TFTLCD_RST			PBout(0)

#define TFTLCD_BL_GPIO 		F407GPIO.B
#define TFTLCD_BL_PIN  		F407PIN.P1
#define TFTLCD_BL			PBout(1)

#define TFTLCD_SDA_GPIO 	F407GPIO.B
#define TFTLCD_SDA_PIN  	F407PIN.P2
#define TFTLCD_SDA			PBout(2)

#define TFTLCD_SCL_GPIO 	F407GPIO.B
#define TFTLCD_SCL_PIN  	F407PIN.P3
#define TFTLCD_SCL			PBout(3)

#define TFTLCD_PEN_GPIO 	F407GPIO.B
#define TFTLCD_PEN_PIN  	F407PIN.P4
#define TFTLCD_PEN			PBout(4)

#define TFTLCD_TCS_GPIO 	F407GPIO.B
#define TFTLCD_TCS_PIN  	F407PIN.P5
#define TFTLCD_TCS			PBout(5)

#define TOUCH_SDA_IN()  {GPIOB->MODER&=~((u32)3<<(2*2));GPIOB->MODER|=(u32)0<<2*2;}	//MOSI（数据线）输入模式
#define TOUCH_SDA_OUT() {GPIOB->MODER&=~((u32)3<<(2*2));GPIOB->MODER|=(u32)1<<2*2;} 	//MOSI（数据线）输出模式

//应手动更改以下定义
#define TOUCH_IIC_SCL    PBout(3) 	//SCL
#define TOUCH_IIC_SDA    PBout(2) 	//SDA	 
#define TOUCH_READ_SDA   PBin(2)  	//输入SDA 
//IO操作函数	 
#define GT_RST    	  PBout(5)	//GT9147复位引脚
#define GT_INT    	  PBin(4)	//GT9147中断引脚

//I2C读写命令	
#define GT_CMD_WR 		0X28     	//写命令
#define GT_CMD_RD 		0X29		//读命令

//GT9147 部分寄存器定义 
#define GT_CTRL_REG 	0X8040   	//GT9147控制寄存器
#define GT_CFGS_REG 	0X8047   	//GT9147配置起始地址寄存器
#define GT_CHECK_REG 	0X80FF   	//GT9147校验和寄存器
#define GT_PID_REG 		0X8140   	//GT9147产品ID寄存器
#define GT_GSTID_REG 	0X814E   	//GT9147当前检测到的触摸情况

//该数值储存了触摸屏芯片初始化所需写入的数据
const u8 GT9147_CFG_TBL[]=
{ 
	0X60,0XE0,0X01,0X20,0X03,0X05,0X35,0X00,0X02,0X08,
	0X1E,0X08,0X50,0X3C,0X0F,0X05,0X00,0X00,0XFF,0X67,
	0X50,0X00,0X00,0X18,0X1A,0X1E,0X14,0X89,0X28,0X0A,
	0X30,0X2E,0XBB,0X0A,0X03,0X00,0X00,0X02,0X33,0X1D,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X32,0X00,0X00,
	0X2A,0X1C,0X5A,0X94,0XC5,0X02,0X07,0X00,0X00,0X00,
	0XB5,0X1F,0X00,0X90,0X28,0X00,0X77,0X32,0X00,0X62,
	0X3F,0X00,0X52,0X50,0X00,0X52,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0X0F,0X03,0X06,0X10,0X42,0XF8,0X0F,0X14,0X00,0X00,
	0X00,0X00,0X1A,0X18,0X16,0X14,0X12,0X10,0X0E,0X0C,
	0X0A,0X08,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0Xff,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X29,0X28,0X24,0X22,0X20,0X1F,0X1E,0X1D,
	0X0E,0X0C,0X0A,0X08,0X06,0X05,0X04,0X02,0X00,0XFF,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF
};

void TFTLCD_Switch(u8 ON_OFF)
{
	if (ON_OFF==TFTLCD_ON)
		TFTLCD_BL=1;
	else
		TFTLCD_BL=0;
}

void TFTLCD_WriteCommand(u16 CMD, u16 PAR)
{
	TFTLCD_ORDER=CMD;
	TFTLCD_DATA=PAR;
}

void TFTLCD_PresentMode(u16 Scan_Mode, u16 x_min, u16 x_max, u16 y_min, u16 y_max)
{
	TFTLCD_WriteCommand(0x3600,Scan_Mode);
	
	TFTLCD_WriteCommand(0x2a00,x_min>>8);
	TFTLCD_WriteCommand(0x2a01,x_min&0xff);
	TFTLCD_WriteCommand(0x2a02,x_max>>8);
	TFTLCD_WriteCommand(0x2a03,x_max&0xff);
	
	TFTLCD_WriteCommand(0x2b00,y_min>>8);
	TFTLCD_WriteCommand(0x2b01,y_min&0xff);
	TFTLCD_WriteCommand(0x2b02,y_max>>8);
	TFTLCD_WriteCommand(0x2b03,y_max&0xff);
}

void TFTLCD_Init(void)
{
	RCC->AHB1ENR|=1<<3;
	RCC->AHB1ENR|=1<<4;
	RCC->AHB1ENR|=1<<TFTLCD_RST_GPIO.DEF;
	RCC->AHB1ENR|=1<<TFTLCD_BL_GPIO.DEF;
	RCC->AHB3ENR|=1;
	
	GPIO_Set(TFTLCD_RST_GPIO.ADDR,TFTLCD_RST_PIN[0],GPIO_MODE_OUT,GPIO_OTYPE_PP,
			GPIO_SPEED_50M,GPIO_PUPD_PU);			//RST(LCD)引脚使能
	GPIO_Set(TFTLCD_BL_GPIO.ADDR,TFTLCD_BL_PIN[0],GPIO_MODE_OUT,GPIO_OTYPE_PP,
			GPIO_SPEED_50M,GPIO_PUPD_PU);			//BL(LCD)引脚使能
	GPIO_Set(GPIOD,PIN14|PIN0|PIN9|PIN8|PIN1|PIN15|PIN10,GPIO_MODE_AF,GPIO_OTYPE_PP,
			GPIO_SPEED_100M,GPIO_PUPD_PU);			//FSMC_Dx 复用
	GPIO_Set(GPIOE,PIN7|PIN9|PIN11|PIN13|PIN15|PIN14|PIN12|PIN10|PIN8,GPIO_MODE_AF,GPIO_OTYPE_PP,
			GPIO_SPEED_100M,GPIO_PUPD_PU);			//FSMC_Dx 复用
	GPIO_Set(GPIOD,PIN13|PIN4|PIN5|PIN7,GPIO_MODE_AF,GPIO_OTYPE_PP,
			GPIO_SPEED_100M,GPIO_PUPD_PU);			//FSMC_A18/NOE/NE1/NWE 复用
	GPIO_Set(GPIOE,PIN3,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,
			GPIO_PUPD_PU);
	
	GPIO_AF_Set(GPIOD,13,12);
	GPIO_AF_Set(GPIOD,4,12);
	GPIO_AF_Set(GPIOD,14,12);
	GPIO_AF_Set(GPIOD,0,12);
	GPIO_AF_Set(GPIOD,9,12);
	GPIO_AF_Set(GPIOD,10,12);
	GPIO_AF_Set(GPIOD,8,12);
	GPIO_AF_Set(GPIOD,1,12);
	GPIO_AF_Set(GPIOD,15,12);
	GPIO_AF_Set(GPIOD,5,12);
	GPIO_AF_Set(GPIOD,7,12);
	GPIO_AF_Set(GPIOE,7,12);
	GPIO_AF_Set(GPIOE,9,12);
	GPIO_AF_Set(GPIOE,11,12);
	GPIO_AF_Set(GPIOE,13,12);
	GPIO_AF_Set(GPIOE,15,12);
	GPIO_AF_Set(GPIOE,14,12);
	GPIO_AF_Set(GPIOE,12,12);
	GPIO_AF_Set(GPIOE,10,12);
	GPIO_AF_Set(GPIOE,3,12);
	GPIO_AF_Set(GPIOE,8,12);			//AF12（FSMC复用）	

	FSMC_Bank1->BTCR[0]=0x00000000;
	FSMC_Bank1->BTCR[1]=0x00000000;
	FSMC_Bank1E->BWTR[0]=0x00000000;
	
	FSMC_Bank1->BTCR[0]|=1<<14;			//EXTMOD=1，拓展模式使能		
	FSMC_Bank1->BTCR[0]|=1<<12;			//WREN=1，写使能
	FSMC_Bank1->BTCR[0]|=1<<4;			//MWID[1:0]=1，数据16位宽度
	FSMC_Bank1->BTCR[0]|=1<<0;			//MBKEN=1，储存块使能
	
	FSMC_Bank1->BTCR[1]|=60<<8;			//DataST[7:0]=60，读时数据保持时间360ns
	FSMC_Bank1->BTCR[1]|=15;			//ADDSET[3:0]=15，读时地址建立时间90ns
	
	FSMC_Bank1E->BWTR[0]|=4<<8;			//DataST[7:0]=4，写时数据保持时间18ns
	FSMC_Bank1E->BWTR[0]|=4;			//ADDSET[3:0]=4，写时地址建立时间18ns

	TFTLCD_RST=0;
	delay_ms(200);
	TFTLCD_RST=1;
	delay_ms(200);
	TFTLCD_WriteCommand(0xF000,0x55);
	TFTLCD_WriteCommand(0xF001,0xAA);
	TFTLCD_WriteCommand(0xF002,0x52);
	TFTLCD_WriteCommand(0xF003,0x08);
	TFTLCD_WriteCommand(0xF004,0x01);
	TFTLCD_WriteCommand(0xB000,0x0D);
	TFTLCD_WriteCommand(0xB001,0x0D);
	TFTLCD_WriteCommand(0xB002,0x0D);
	TFTLCD_WriteCommand(0xB600,0x34);
	TFTLCD_WriteCommand(0xB601,0x34);
	TFTLCD_WriteCommand(0xB602,0x34);
	TFTLCD_WriteCommand(0xB100,0x0D);
	TFTLCD_WriteCommand(0xB101,0x0D);
	TFTLCD_WriteCommand(0xB102,0x0D);
	TFTLCD_WriteCommand(0xB700,0x34);
	TFTLCD_WriteCommand(0xB701,0x34);
	TFTLCD_WriteCommand(0xB702,0x34);
	TFTLCD_WriteCommand(0xB200,0x00);
	TFTLCD_WriteCommand(0xB201,0x00);
	TFTLCD_WriteCommand(0xB202,0x00);
	TFTLCD_WriteCommand(0xB800,0x24);
	TFTLCD_WriteCommand(0xB801,0x24);
	TFTLCD_WriteCommand(0xB802,0x24);
	TFTLCD_WriteCommand(0xBF00,0x01);
	TFTLCD_WriteCommand(0xB300,0x0F);
	TFTLCD_WriteCommand(0xB301,0x0F);
	TFTLCD_WriteCommand(0xB302,0x0F);
	TFTLCD_WriteCommand(0xB900,0x34);
	TFTLCD_WriteCommand(0xB901,0x34);
	TFTLCD_WriteCommand(0xB902,0x34);
	TFTLCD_WriteCommand(0xB500,0x08);
	TFTLCD_WriteCommand(0xB501,0x08);
	TFTLCD_WriteCommand(0xB502,0x08);
	TFTLCD_WriteCommand(0xC200,0x03);
	TFTLCD_WriteCommand(0xBA00,0x24);
	TFTLCD_WriteCommand(0xBA01,0x24);
	TFTLCD_WriteCommand(0xBA02,0x24);
	TFTLCD_WriteCommand(0xBC00,0x00);
	TFTLCD_WriteCommand(0xBC01,0x78);
	TFTLCD_WriteCommand(0xBC02,0x00);
	TFTLCD_WriteCommand(0xBD00,0x00);
	TFTLCD_WriteCommand(0xBD01,0x78);
	TFTLCD_WriteCommand(0xBD02,0x00);
	TFTLCD_WriteCommand(0xBE00,0x00);
	TFTLCD_WriteCommand(0xBE01,0x64);
	TFTLCD_WriteCommand(0xD100,0x00);
	TFTLCD_WriteCommand(0xD101,0x33);
	TFTLCD_WriteCommand(0xD102,0x00);
	TFTLCD_WriteCommand(0xD103,0x34);
	TFTLCD_WriteCommand(0xD104,0x00);
	TFTLCD_WriteCommand(0xD105,0x3A);
	TFTLCD_WriteCommand(0xD106,0x00);
	TFTLCD_WriteCommand(0xD107,0x4A);
	TFTLCD_WriteCommand(0xD108,0x00);
	TFTLCD_WriteCommand(0xD109,0x5C);
	TFTLCD_WriteCommand(0xD10A,0x00);
	TFTLCD_WriteCommand(0xD10B,0x81);
	TFTLCD_WriteCommand(0xD10C,0x00);
	TFTLCD_WriteCommand(0xD10D,0xA6);
	TFTLCD_WriteCommand(0xD10E,0x00);
	TFTLCD_WriteCommand(0xD10F,0xE5);
	TFTLCD_WriteCommand(0xD110,0x01);
	TFTLCD_WriteCommand(0xD111,0x13);
	TFTLCD_WriteCommand(0xD112,0x01);
	TFTLCD_WriteCommand(0xD113,0x54);
	TFTLCD_WriteCommand(0xD114,0x01);
	TFTLCD_WriteCommand(0xD115,0x82);
	TFTLCD_WriteCommand(0xD116,0x01);
	TFTLCD_WriteCommand(0xD117,0xCA);
	TFTLCD_WriteCommand(0xD118,0x02);
	TFTLCD_WriteCommand(0xD119,0x00);
	TFTLCD_WriteCommand(0xD11A,0x02);
	TFTLCD_WriteCommand(0xD11B,0x01);
	TFTLCD_WriteCommand(0xD11C,0x02);
	TFTLCD_WriteCommand(0xD11D,0x34);
	TFTLCD_WriteCommand(0xD11E,0x02);
	TFTLCD_WriteCommand(0xD11F,0x67);
	TFTLCD_WriteCommand(0xD120,0x02);
	TFTLCD_WriteCommand(0xD121,0x84);
	TFTLCD_WriteCommand(0xD122,0x02);
	TFTLCD_WriteCommand(0xD123,0xA4);
	TFTLCD_WriteCommand(0xD124,0x02);
	TFTLCD_WriteCommand(0xD125,0xB7);
	TFTLCD_WriteCommand(0xD126,0x02);
	TFTLCD_WriteCommand(0xD127,0xCF);
	TFTLCD_WriteCommand(0xD128,0x02);
	TFTLCD_WriteCommand(0xD129,0xDE);
	TFTLCD_WriteCommand(0xD12A,0x02);
	TFTLCD_WriteCommand(0xD12B,0xF2);
	TFTLCD_WriteCommand(0xD12C,0x02);
	TFTLCD_WriteCommand(0xD12D,0xFE);
	TFTLCD_WriteCommand(0xD12E,0x03);
	TFTLCD_WriteCommand(0xD12F,0x10);
	TFTLCD_WriteCommand(0xD130,0x03);
	TFTLCD_WriteCommand(0xD131,0x33);
	TFTLCD_WriteCommand(0xD132,0x03);
	TFTLCD_WriteCommand(0xD133,0x6D);
	TFTLCD_WriteCommand(0xD200,0x00);
	TFTLCD_WriteCommand(0xD201,0x33);
	TFTLCD_WriteCommand(0xD202,0x00);
	TFTLCD_WriteCommand(0xD203,0x34);
	TFTLCD_WriteCommand(0xD204,0x00);
	TFTLCD_WriteCommand(0xD205,0x3A);
	TFTLCD_WriteCommand(0xD206,0x00);
	TFTLCD_WriteCommand(0xD207,0x4A);
	TFTLCD_WriteCommand(0xD208,0x00);
	TFTLCD_WriteCommand(0xD209,0x5C);
	TFTLCD_WriteCommand(0xD20A,0x00);
	TFTLCD_WriteCommand(0xD20B,0x81);
	TFTLCD_WriteCommand(0xD20C,0x00);
	TFTLCD_WriteCommand(0xD20D,0xA6);
	TFTLCD_WriteCommand(0xD20E,0x00);
	TFTLCD_WriteCommand(0xD20F,0xE5);
	TFTLCD_WriteCommand(0xD210,0x01);
	TFTLCD_WriteCommand(0xD211,0x13);
	TFTLCD_WriteCommand(0xD212,0x01);
	TFTLCD_WriteCommand(0xD213,0x54);
	TFTLCD_WriteCommand(0xD214,0x01);
	TFTLCD_WriteCommand(0xD215,0x82);
	TFTLCD_WriteCommand(0xD216,0x01);
	TFTLCD_WriteCommand(0xD217,0xCA);
	TFTLCD_WriteCommand(0xD218,0x02);
	TFTLCD_WriteCommand(0xD219,0x00);
	TFTLCD_WriteCommand(0xD21A,0x02);
	TFTLCD_WriteCommand(0xD21B,0x01);
	TFTLCD_WriteCommand(0xD21C,0x02);
	TFTLCD_WriteCommand(0xD21D,0x34);
	TFTLCD_WriteCommand(0xD21E,0x02);
	TFTLCD_WriteCommand(0xD21F,0x67);
	TFTLCD_WriteCommand(0xD220,0x02);
	TFTLCD_WriteCommand(0xD221,0x84);
	TFTLCD_WriteCommand(0xD222,0x02);
	TFTLCD_WriteCommand(0xD223,0xA4);
	TFTLCD_WriteCommand(0xD224,0x02);
	TFTLCD_WriteCommand(0xD225,0xB7);
	TFTLCD_WriteCommand(0xD226,0x02);
	TFTLCD_WriteCommand(0xD227,0xCF);
	TFTLCD_WriteCommand(0xD228,0x02);
	TFTLCD_WriteCommand(0xD229,0xDE);
	TFTLCD_WriteCommand(0xD22A,0x02);
	TFTLCD_WriteCommand(0xD22B,0xF2);
	TFTLCD_WriteCommand(0xD22C,0x02);
	TFTLCD_WriteCommand(0xD22D,0xFE);
	TFTLCD_WriteCommand(0xD22E,0x03);
	TFTLCD_WriteCommand(0xD22F,0x10);
	TFTLCD_WriteCommand(0xD230,0x03);
	TFTLCD_WriteCommand(0xD231,0x33);
	TFTLCD_WriteCommand(0xD232,0x03);
	TFTLCD_WriteCommand(0xD233,0x6D);
	TFTLCD_WriteCommand(0xD300,0x00);
	TFTLCD_WriteCommand(0xD301,0x33);
	TFTLCD_WriteCommand(0xD302,0x00);
	TFTLCD_WriteCommand(0xD303,0x34);
	TFTLCD_WriteCommand(0xD304,0x00);
	TFTLCD_WriteCommand(0xD305,0x3A);
	TFTLCD_WriteCommand(0xD306,0x00);
	TFTLCD_WriteCommand(0xD307,0x4A);
	TFTLCD_WriteCommand(0xD308,0x00);
	TFTLCD_WriteCommand(0xD309,0x5C);
	TFTLCD_WriteCommand(0xD30A,0x00);
	TFTLCD_WriteCommand(0xD30B,0x81);
	TFTLCD_WriteCommand(0xD30C,0x00);
	TFTLCD_WriteCommand(0xD30D,0xA6);
	TFTLCD_WriteCommand(0xD30E,0x00);
	TFTLCD_WriteCommand(0xD30F,0xE5);
	TFTLCD_WriteCommand(0xD310,0x01);
	TFTLCD_WriteCommand(0xD311,0x13);
	TFTLCD_WriteCommand(0xD312,0x01);
	TFTLCD_WriteCommand(0xD313,0x54);
	TFTLCD_WriteCommand(0xD314,0x01);
	TFTLCD_WriteCommand(0xD315,0x82);
	TFTLCD_WriteCommand(0xD316,0x01);
	TFTLCD_WriteCommand(0xD317,0xCA);
	TFTLCD_WriteCommand(0xD318,0x02);
	TFTLCD_WriteCommand(0xD319,0x00);
	TFTLCD_WriteCommand(0xD31A,0x02);
	TFTLCD_WriteCommand(0xD31B,0x01);
	TFTLCD_WriteCommand(0xD31C,0x02);
	TFTLCD_WriteCommand(0xD31D,0x34);
	TFTLCD_WriteCommand(0xD31E,0x02);
	TFTLCD_WriteCommand(0xD31F,0x67);
	TFTLCD_WriteCommand(0xD320,0x02);
	TFTLCD_WriteCommand(0xD321,0x84);
	TFTLCD_WriteCommand(0xD322,0x02);
	TFTLCD_WriteCommand(0xD323,0xA4);
	TFTLCD_WriteCommand(0xD324,0x02);
	TFTLCD_WriteCommand(0xD325,0xB7);
	TFTLCD_WriteCommand(0xD326,0x02);
	TFTLCD_WriteCommand(0xD327,0xCF);
	TFTLCD_WriteCommand(0xD328,0x02);
	TFTLCD_WriteCommand(0xD329,0xDE);
	TFTLCD_WriteCommand(0xD32A,0x02);
	TFTLCD_WriteCommand(0xD32B,0xF2);
	TFTLCD_WriteCommand(0xD32C,0x02);
	TFTLCD_WriteCommand(0xD32D,0xFE);
	TFTLCD_WriteCommand(0xD32E,0x03);
	TFTLCD_WriteCommand(0xD32F,0x10);
	TFTLCD_WriteCommand(0xD330,0x03);
	TFTLCD_WriteCommand(0xD331,0x33);
	TFTLCD_WriteCommand(0xD332,0x03);
	TFTLCD_WriteCommand(0xD333,0x6D);
	TFTLCD_WriteCommand(0xD400,0x00);
	TFTLCD_WriteCommand(0xD401,0x33);
	TFTLCD_WriteCommand(0xD402,0x00);
	TFTLCD_WriteCommand(0xD403,0x34);
	TFTLCD_WriteCommand(0xD404,0x00);
	TFTLCD_WriteCommand(0xD405,0x3A);
	TFTLCD_WriteCommand(0xD406,0x00);
	TFTLCD_WriteCommand(0xD407,0x4A);
	TFTLCD_WriteCommand(0xD408,0x00);
	TFTLCD_WriteCommand(0xD409,0x5C);
	TFTLCD_WriteCommand(0xD40A,0x00);
	TFTLCD_WriteCommand(0xD40B,0x81);
	TFTLCD_WriteCommand(0xD40C,0x00);
	TFTLCD_WriteCommand(0xD40D,0xA6);
	TFTLCD_WriteCommand(0xD40E,0x00);
	TFTLCD_WriteCommand(0xD40F,0xE5);
	TFTLCD_WriteCommand(0xD410,0x01);
	TFTLCD_WriteCommand(0xD411,0x13);
	TFTLCD_WriteCommand(0xD412,0x01);
	TFTLCD_WriteCommand(0xD413,0x54);
	TFTLCD_WriteCommand(0xD414,0x01);
	TFTLCD_WriteCommand(0xD415,0x82);
	TFTLCD_WriteCommand(0xD416,0x01);
	TFTLCD_WriteCommand(0xD417,0xCA);
	TFTLCD_WriteCommand(0xD418,0x02);
	TFTLCD_WriteCommand(0xD419,0x00);
	TFTLCD_WriteCommand(0xD41A,0x02);
	TFTLCD_WriteCommand(0xD41B,0x01);
	TFTLCD_WriteCommand(0xD41C,0x02);
	TFTLCD_WriteCommand(0xD41D,0x34);
	TFTLCD_WriteCommand(0xD41E,0x02);
	TFTLCD_WriteCommand(0xD41F,0x67);
	TFTLCD_WriteCommand(0xD420,0x02);
	TFTLCD_WriteCommand(0xD421,0x84);
	TFTLCD_WriteCommand(0xD422,0x02);
	TFTLCD_WriteCommand(0xD423,0xA4);
	TFTLCD_WriteCommand(0xD424,0x02);
	TFTLCD_WriteCommand(0xD425,0xB7);
	TFTLCD_WriteCommand(0xD426,0x02);
	TFTLCD_WriteCommand(0xD427,0xCF);
	TFTLCD_WriteCommand(0xD428,0x02);
	TFTLCD_WriteCommand(0xD429,0xDE);
	TFTLCD_WriteCommand(0xD42A,0x02);
	TFTLCD_WriteCommand(0xD42B,0xF2);
	TFTLCD_WriteCommand(0xD42C,0x02);
	TFTLCD_WriteCommand(0xD42D,0xFE);
	TFTLCD_WriteCommand(0xD42E,0x03);
	TFTLCD_WriteCommand(0xD42F,0x10);
	TFTLCD_WriteCommand(0xD430,0x03);
	TFTLCD_WriteCommand(0xD431,0x33);
	TFTLCD_WriteCommand(0xD432,0x03);
	TFTLCD_WriteCommand(0xD433,0x6D);
	TFTLCD_WriteCommand(0xD500,0x00);
	TFTLCD_WriteCommand(0xD501,0x33);
	TFTLCD_WriteCommand(0xD502,0x00);
	TFTLCD_WriteCommand(0xD503,0x34);
	TFTLCD_WriteCommand(0xD504,0x00);
	TFTLCD_WriteCommand(0xD505,0x3A);
	TFTLCD_WriteCommand(0xD506,0x00);
	TFTLCD_WriteCommand(0xD507,0x4A);
	TFTLCD_WriteCommand(0xD508,0x00);
	TFTLCD_WriteCommand(0xD509,0x5C);
	TFTLCD_WriteCommand(0xD50A,0x00);
	TFTLCD_WriteCommand(0xD50B,0x81);
	TFTLCD_WriteCommand(0xD50C,0x00);
	TFTLCD_WriteCommand(0xD50D,0xA6);
	TFTLCD_WriteCommand(0xD50E,0x00);
	TFTLCD_WriteCommand(0xD50F,0xE5);
	TFTLCD_WriteCommand(0xD510,0x01);
	TFTLCD_WriteCommand(0xD511,0x13);
	TFTLCD_WriteCommand(0xD512,0x01);
	TFTLCD_WriteCommand(0xD513,0x54);
	TFTLCD_WriteCommand(0xD514,0x01);
	TFTLCD_WriteCommand(0xD515,0x82);
	TFTLCD_WriteCommand(0xD516,0x01);
	TFTLCD_WriteCommand(0xD517,0xCA);
	TFTLCD_WriteCommand(0xD518,0x02);
	TFTLCD_WriteCommand(0xD519,0x00);
	TFTLCD_WriteCommand(0xD51A,0x02);
	TFTLCD_WriteCommand(0xD51B,0x01);
	TFTLCD_WriteCommand(0xD51C,0x02);
	TFTLCD_WriteCommand(0xD51D,0x34);
	TFTLCD_WriteCommand(0xD51E,0x02);
	TFTLCD_WriteCommand(0xD51F,0x67);
	TFTLCD_WriteCommand(0xD520,0x02);
	TFTLCD_WriteCommand(0xD521,0x84);
	TFTLCD_WriteCommand(0xD522,0x02);
	TFTLCD_WriteCommand(0xD523,0xA4);
	TFTLCD_WriteCommand(0xD524,0x02);
	TFTLCD_WriteCommand(0xD525,0xB7);
	TFTLCD_WriteCommand(0xD526,0x02);
	TFTLCD_WriteCommand(0xD527,0xCF);
	TFTLCD_WriteCommand(0xD528,0x02);
	TFTLCD_WriteCommand(0xD529,0xDE);
	TFTLCD_WriteCommand(0xD52A,0x02);
	TFTLCD_WriteCommand(0xD52B,0xF2);
	TFTLCD_WriteCommand(0xD52C,0x02);
	TFTLCD_WriteCommand(0xD52D,0xFE);
	TFTLCD_WriteCommand(0xD52E,0x03);
	TFTLCD_WriteCommand(0xD52F,0x10);
	TFTLCD_WriteCommand(0xD530,0x03);
	TFTLCD_WriteCommand(0xD531,0x33);
	TFTLCD_WriteCommand(0xD532,0x03);
	TFTLCD_WriteCommand(0xD533,0x6D);
	TFTLCD_WriteCommand(0xD600,0x00);
	TFTLCD_WriteCommand(0xD601,0x33);
	TFTLCD_WriteCommand(0xD602,0x00);
	TFTLCD_WriteCommand(0xD603,0x34);
	TFTLCD_WriteCommand(0xD604,0x00);
	TFTLCD_WriteCommand(0xD605,0x3A);
	TFTLCD_WriteCommand(0xD606,0x00);
	TFTLCD_WriteCommand(0xD607,0x4A);
	TFTLCD_WriteCommand(0xD608,0x00);
	TFTLCD_WriteCommand(0xD609,0x5C);
	TFTLCD_WriteCommand(0xD60A,0x00);
	TFTLCD_WriteCommand(0xD60B,0x81);
	TFTLCD_WriteCommand(0xD60C,0x00);
	TFTLCD_WriteCommand(0xD60D,0xA6);
	TFTLCD_WriteCommand(0xD60E,0x00);
	TFTLCD_WriteCommand(0xD60F,0xE5);
	TFTLCD_WriteCommand(0xD610,0x01);
	TFTLCD_WriteCommand(0xD611,0x13);
	TFTLCD_WriteCommand(0xD612,0x01);
	TFTLCD_WriteCommand(0xD613,0x54);
	TFTLCD_WriteCommand(0xD614,0x01);
	TFTLCD_WriteCommand(0xD615,0x82);
	TFTLCD_WriteCommand(0xD616,0x01);
	TFTLCD_WriteCommand(0xD617,0xCA);
	TFTLCD_WriteCommand(0xD618,0x02);
	TFTLCD_WriteCommand(0xD619,0x00);
	TFTLCD_WriteCommand(0xD61A,0x02);
	TFTLCD_WriteCommand(0xD61B,0x01);
	TFTLCD_WriteCommand(0xD61C,0x02);
	TFTLCD_WriteCommand(0xD61D,0x34);
	TFTLCD_WriteCommand(0xD61E,0x02);
	TFTLCD_WriteCommand(0xD61F,0x67);
	TFTLCD_WriteCommand(0xD620,0x02);
	TFTLCD_WriteCommand(0xD621,0x84);
	TFTLCD_WriteCommand(0xD622,0x02);
	TFTLCD_WriteCommand(0xD623,0xA4);
	TFTLCD_WriteCommand(0xD624,0x02);
	TFTLCD_WriteCommand(0xD625,0xB7);
	TFTLCD_WriteCommand(0xD626,0x02);
	TFTLCD_WriteCommand(0xD627,0xCF);
	TFTLCD_WriteCommand(0xD628,0x02);
	TFTLCD_WriteCommand(0xD629,0xDE);
	TFTLCD_WriteCommand(0xD62A,0x02);
	TFTLCD_WriteCommand(0xD62B,0xF2);
	TFTLCD_WriteCommand(0xD62C,0x02);
	TFTLCD_WriteCommand(0xD62D,0xFE);
	TFTLCD_WriteCommand(0xD62E,0x03);
	TFTLCD_WriteCommand(0xD62F,0x10);
	TFTLCD_WriteCommand(0xD630,0x03);
	TFTLCD_WriteCommand(0xD631,0x33);
	TFTLCD_WriteCommand(0xD632,0x03);
	TFTLCD_WriteCommand(0xD633,0x6D);
	TFTLCD_WriteCommand(0xF000,0x55);
	TFTLCD_WriteCommand(0xF001,0xAA);
	TFTLCD_WriteCommand(0xF002,0x52);
	TFTLCD_WriteCommand(0xF003,0x08);
	TFTLCD_WriteCommand(0xF004,0x00);
	TFTLCD_WriteCommand(0xB100,0xCC);
	TFTLCD_WriteCommand(0xB101,0x00);
	TFTLCD_WriteCommand(0xB600,0x05);
	TFTLCD_WriteCommand(0xB700,0x70);
	TFTLCD_WriteCommand(0xB701,0x70);
	TFTLCD_WriteCommand(0xB800,0x01);
	TFTLCD_WriteCommand(0xB801,0x03);
	TFTLCD_WriteCommand(0xB802,0x03);
	TFTLCD_WriteCommand(0xB803,0x03);
	TFTLCD_WriteCommand(0xBC00,0x02);
	TFTLCD_WriteCommand(0xBC01,0x00);
	TFTLCD_WriteCommand(0xBC02,0x00);
	TFTLCD_WriteCommand(0xC900,0xD0);
	TFTLCD_WriteCommand(0xC901,0x02);
	TFTLCD_WriteCommand(0xC902,0x50);
	TFTLCD_WriteCommand(0xC903,0x50);
	TFTLCD_WriteCommand(0xC904,0x50);
	TFTLCD_WriteCommand(0x3500,0x00);
	TFTLCD_WriteCommand(0x3A00,0x55);
	TFTLCD_ORDER=0x1100;
	delay_us(120);
	TFTLCD_ORDER=0x2900;
	TFTLCD_BL=0;
}

void TFTLCD_DisplayColor(u16 Color)
{
	u16 i,j;
	TFTLCD_PresentMode(L2R_U2D,0,479,0,799);
	TFTLCD_ORDER=0x2c00;
	for (i=0;i<800;i++)
	{
		for (j=0;j<480;j++)
			TFTLCD_DATA=Color;
	}
}

void TFTLCD_DisplaySquare(u16 X, u16 Y, u16 Wide, u16 Hight, u16 Color)
{
	u16 i,j;
	TFTLCD_PresentMode(L2R_U2D,X,X+Wide-1,Y,Y+Hight-1);
	TFTLCD_ORDER=0x2c00;
	for (i=0;i<Hight;i++)
	{
		for (j=0;j<Wide;j++)
			TFTLCD_DATA=Color;
	}
}

void TFTLCD_DisplayLine(u16 X1, u16 Y1, u16 X2, u8 Y2, u16 Color)
{
	u16 i,j,k;
	float Step;
	
	if (X1>X2)
	{
		j=X1-X2+1;
		if (Y1>=Y2)
		{
			Step=(float)(Y1-Y2)/(X1-X2);
			for (i=0;i<j;i++)
			{
				TFTLCD_PresentMode(D2U_R2L,(u16)(799-Y2+i*Step),799,479-X2+i,479);
				TFTLCD_ORDER=0x2c00;
				for (k=0;k<(u16)Step;k++)
					TFTLCD_DATA=Color;
			}
		}
		else
			Step=(float)(Y2-Y1)/(X1-X2);
		
	}
	else if (X1<X2)
	{
		if (Y1>=Y2)
			Step=(float)(Y1-Y2)/(X2-X1);
		else
			Step=(float)(Y2-Y1)/(X2-X1);
		j=X2-X1+1;
	}
	else
	{
		if (Y1>=Y2)
			TFTLCD_DisplaySquare(X1,Y2,1,Y1-Y2+1,Color);
		else
			TFTLCD_DisplaySquare(X1,Y1,1,Y2-Y1+1,Color);
	}
}

void TFTLCD_DisplayBMP(u16 X, u16 Y, BMP Picture)
{
	u16 i,j,Wide,Hight;
	Wide=Picture.Width;
	Hight=Picture.Height;
	TFTLCD_PresentMode(L2R_U2D,X,X+Wide-1,Y,799);
	TFTLCD_ORDER=0x2c00;
	for (i=0;i<Hight;i++)
	{
		for (j=0;j<Wide;j++)
			TFTLCD_DATA=*(Picture.Colors+i*Wide+j);
	}
}

void TFTLCD_DisplayrBMP(u16 X, u16 Y, rBMP Picture)
{
	u16 i,q,Wide;
	u32 j,Size;
	Wide=Picture.Width;
	Size=Picture.Size;
	TFTLCD_PresentMode(L2R_U2D,X,X+Wide-1,Y,799);
	TFTLCD_ORDER=0x2c00;
	for (j=0;j<Size;j+=2)
	{
		i=Picture.Colors[j];
		for (q=0;q<i;q++)
			TFTLCD_DATA=Picture.Colors[j+1];
	}
}

void TFTLCD_DisplaypBMP(u16 X, u16 Y, pBMP Picture)
{
	u16 i,j,Wide,Hight;
	Wide=Picture.Width;
	Hight=Picture.Height;
	TFTLCD_PresentMode(L2R_U2D,X,X+Wide-1,Y,799);
	TFTLCD_ORDER=0x2c00;
	for (i=0;i<Hight;i++)
	{
		for (j=0;j<Wide;j++)
			TFTLCD_DATA=Picture.Color_Palette[*(Picture.Colors+i*Wide+j)];
	}
}
	
void TFTLCD_DisplayprBMP(u16 X, u16 Y, prBMP Picture)
{
	u16 i,q,Wide;
	u32 j,Size;
	Wide=Picture.Width;
	Size=Picture.Size;
	TFTLCD_PresentMode(L2R_U2D,X,X+Wide-1,Y,799);
	TFTLCD_ORDER=0x2c00;
	for (j=0;j<Size;j++)
	{
		i=Picture.Color_Blocks[j];
		for (q=0;q<i;q++)
			TFTLCD_DATA=Picture.Color_Palette[Picture.Colors[j]];
	}
}
	
void TFTLCD_Display_Waveform(u16 X, u16 Y, u16 Canvas_Wide, u16 Canvas_Hight, u16 Data_Lenth, u8 Line_Wide, u16 Color, float *Data)
{
	u16 i;
	float Max_Slope,Min_Inter;
	float k,Step,save1,save2;
	
	Step=(float)Data_Lenth/Canvas_Hight;
	Max_Slope=Data[0];
	Min_Inter=Max_Slope;
	for (i=0;i<Data_Lenth;i++)
	{
		if (Data[i]>Max_Slope)
			Max_Slope=Data[i];
		else if (Data[i]<=Min_Inter)
			Min_Inter=Data[i];
	}

	Max_Slope=(float)Canvas_Wide/(Max_Slope-Min_Inter);
	Min_Inter=X-Max_Slope*Min_Inter;
	
	for (i=0,k=0;i<Canvas_Hight-1;i++,k+=Step)
	{
		save1=Data[(u16)k]*Max_Slope+Min_Inter;
		save2=Data[(u16)(k+Step)]*Max_Slope+Min_Inter;
		if ((u16)(k+Step)>=Data_Lenth)
			goto SKIP;
		if (save1>save2)
			TFTLCD_DisplaySquare(save2-1,i+Y,save1-save2+2,Line_Wide,Color);
		else
			TFTLCD_DisplaySquare(save1-1,i+Y,save2-save1+2,Line_Wide,Color);
		SKIP:
			TFTLCD_DisplaySquare(save1,i+Y,1,Line_Wide,Color);
	}
}






//控制I2C速度的延时
/*void TOUCH_IIC_Delay(void)
{
	delay_us(2);
}

//产生IIC起始信号
void TOUCH_IIC_Start(void)
{
	TOUCH_SDA_OUT();     //sda线输出
	TOUCH_IIC_SDA=1;	  	  
	TOUCH_IIC_SCL=1;
	delay_us(30);
 	TOUCH_IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	TOUCH_IIC_Delay();
	TOUCH_IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void TOUCH_IIC_Stop(void)
{
	TOUCH_SDA_OUT();//sda线输出
	TOUCH_IIC_SCL=1;
	delay_us(30);
	TOUCH_IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
	TOUCH_IIC_Delay();
	TOUCH_IIC_SDA=1;//发送I2C总线结束信号  
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 TOUCH_IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	TOUCH_SDA_IN();      //SDA设置为输入  
	TOUCH_IIC_SDA=1;	   
	TOUCH_IIC_SCL=1;
	TOUCH_IIC_Delay();
	while(TOUCH_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			TOUCH_IIC_Stop();
			return 1;
		} 
		TOUCH_IIC_Delay();
	}
	TOUCH_IIC_SCL=0;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void TOUCH_IIC_Ack(void)
{
	TOUCH_IIC_SCL=0;
	TOUCH_SDA_OUT();
	TOUCH_IIC_Delay();
	TOUCH_IIC_SDA=0;
	TOUCH_IIC_Delay();
	TOUCH_IIC_SCL=1;
	TOUCH_IIC_Delay();
	TOUCH_IIC_SCL=0;
}
//不产生ACK应答		    
void TOUCH_IIC_NAck(void)
{
	TOUCH_IIC_SCL=0;
	TOUCH_SDA_OUT();
	TOUCH_IIC_Delay();
	TOUCH_IIC_SDA=1;
	TOUCH_IIC_Delay();
	TOUCH_IIC_SCL=1;
	TOUCH_IIC_Delay();
	TOUCH_IIC_SCL=0;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void TOUCH_IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	TOUCH_SDA_OUT(); 	    
    TOUCH_IIC_SCL=0;//拉低时钟开始数据传输
	TOUCH_IIC_Delay();
	for(t=0;t<8;t++)
    {              
        TOUCH_IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	      
		TOUCH_IIC_SCL=1; 
		TOUCH_IIC_Delay();
		TOUCH_IIC_SCL=0;	
		TOUCH_IIC_Delay();
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 TOUCH_IIC_Read_Byte(unsigned char ack)
{
	u8 i,receive=0;
 	TOUCH_SDA_IN();//SDA设置为输入
	delay_us(30);
	for(i=0;i<8;i++ )
	{ 
		TOUCH_IIC_SCL=0; 	    	   
		TOUCH_IIC_Delay();
		TOUCH_IIC_SCL=1;	 
		receive<<=1;
		if(TOUCH_READ_SDA)receive++;   
	}	  				 
	if (!ack)TOUCH_IIC_NAck();//发送nACK
	else TOUCH_IIC_Ack(); //发送ACK   
 	return receive;
}

//发送GT9147配置参数
//mode:0,参数不保存到flash
//     1,参数保存到flash
u8 TOUCH_Send_Cfg(u8 mode)
{
	u8 buf[2];
	u8 i=0;
	buf[0]=0;
	buf[1]=mode;	//是否写入到GT9147 FLASH?  即是否掉电保存
	for(i=0;i<sizeof(GT9147_CFG_TBL);i++)buf[0]+=GT9147_CFG_TBL[i];//计算校验和
    buf[0]=(~buf[0])+1;
	TOUCH_Write(GT_CFGS_REG,(u8*)GT9147_CFG_TBL,sizeof(GT9147_CFG_TBL));//发送寄存器配置
	TOUCH_Write(GT_CHECK_REG,buf,2);//写入校验和,和配置更新标记
	return 0;
} 
//向GT9147写入一次数据
//reg:起始寄存器地址
//buf:数据缓缓存区
//len:写数据长度
//返回值:0,成功;1,失败.
u8 TOUCH_Write(u16 reg,u8 *buf,u8 len)
{
	u8 i;
	u8 ret=0;
	TOUCH_IIC_Start();	
 	TOUCH_IIC_Send_Byte(GT_CMD_WR);   	//发送写命令 	 
	TOUCH_IIC_Wait_Ack();
	TOUCH_IIC_Send_Byte(reg>>8);   	//发送高8位地址
	TOUCH_IIC_Wait_Ack(); 	 										  		   
	TOUCH_IIC_Send_Byte(reg&0XFF);   	//发送低8位地址
	TOUCH_IIC_Wait_Ack();  
	for(i=0;i<len;i++)
	{	   
    	TOUCH_IIC_Send_Byte(buf[i]);  	//发数据
		ret=TOUCH_IIC_Wait_Ack();
		if(ret)break;  
	}
    TOUCH_IIC_Stop();					//产生一个停止条件	    
	return ret; 
}
//从GT9147读出一次数据
//reg:起始寄存器地址
//buf:数据缓缓存区
//len:读数据长度			  
void TOUCH_Read(u16 reg,u8 *buf,u8 len)
{
	u8 i; 
 	TOUCH_IIC_Start();	
 	TOUCH_IIC_Send_Byte(GT_CMD_WR);   //发送写命令 	 
	TOUCH_IIC_Wait_Ack();
 	TOUCH_IIC_Send_Byte(reg>>8);   	//发送高8位地址
	TOUCH_IIC_Wait_Ack(); 	 										  		   
 	TOUCH_IIC_Send_Byte(reg&0XFF);   	//发送低8位地址
	TOUCH_IIC_Wait_Ack();  
 	TOUCH_IIC_Start();  	 	   
	TOUCH_IIC_Send_Byte(GT_CMD_RD);   //发送读命令		   
	TOUCH_IIC_Wait_Ack();	   
	for(i=0;i<len;i++)
	{	   
    	buf[i]=TOUCH_IIC_Read_Byte(i==(len-1)?0:1); //发数据	  
	} 
    TOUCH_IIC_Stop();//产生一个停止条件    
} 

void TOUCH_Init(void)
{
	u8 temp[5];  
	RCC->AHB1ENR|=1<<TFTLCD_PEN_GPIO.DEF;
	RCC->AHB1ENR|=1<<TFTLCD_TCS_GPIO.DEF;
	GPIO_Set(TFTLCD_PEN_GPIO.ADDR,TFTLCD_PEN_PIN[0],GPIO_MODE_IN,
			0,0,GPIO_PUPD_PU);										//PEN设置为推挽输出
	GPIO_Set(TFTLCD_TCS_GPIO.ADDR,TFTLCD_TCS_PIN[0],GPIO_MODE_OUT,
			GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); 			//TCS设置为推挽输出
	
	RCC->AHB1ENR|=1<<TFTLCD_SDA_GPIO.DEF;    						//使能数据线（MOSI）时钟    
	RCC->AHB1ENR|=1<<TFTLCD_SCL_GPIO.DEF;    						//使能时钟线（CLK）时钟  
	GPIO_Set(TFTLCD_SDA_GPIO.ADDR,TFTLCD_SDA_PIN[0],GPIO_MODE_OUT,
			GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); 			//设置MOSI为推挽输出
	GPIO_Set(TFTLCD_SCL_GPIO.ADDR,TFTLCD_SCL_PIN[0],GPIO_MODE_OUT,
			GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);  			//设置CLK为推挽输出
	
	GT_RST=0;				//复位
	delay_ms(10);
 	GT_RST=1;				//释放复位	
	delay_ms(10); 
	GPIO_Set(TFTLCD_PEN_GPIO.ADDR,TFTLCD_PEN_PIN[0],GPIO_MODE_IN,0,0,GPIO_PUPD_NONE);//PC5设置为上拉输入
	
	delay_ms(100);  
	temp[0]=0X02;
	TOUCH_Write(GT_CTRL_REG,temp,1);//软复位GT9147
	TOUCH_Read(GT_CFGS_REG,temp,1);//读取GT_CFGS_REG寄存器
	if(temp[0]<0X60)//默认版本比较低,需要更新flash配置
	{
		TOUCH_Send_Cfg(1);//更新并保存配置
	}
	delay_ms(10);
	temp[0]=0X00;	 
	TOUCH_Write(GT_CTRL_REG,temp,1);//结束复位 
}*/
/*
触点个数检测函数
功能：检测当前的触点个数
返回值：有效的触点个数
*/
/*u8 TOUCH_Get_Points_Number(void)
{
	u8 save[2]={0};
	
	TOUCH_Read(0x814e,save,1);
	//TOUCH_Write(0x814e,save+1,1);
	
	switch (save[0])
	{
		case 0:
			return 0;
		case 255:
			return 0;
		case 128:
			return 0;
		case 129:
			return 1;
		case 130:
			return 2;
		case 131:
			return 3;
		case 132:
			return 4;
		case 133:
			return 5;
		default :
			return 0;
	}
}*/

/*
坐标读取函数
参数：1.*DATA：读取出来的坐标要存放的地址，数据按触点按下的时间顺序线性排列
			   数据存放格式为：DATA[0]=第一个触点X坐标，DATA[1]=第一个触点Y坐标
							   DATA[2]=第二个触点X坐标，DATA[3]=第二个触点Y坐标
							   ....（以此类推）
返回值：触点个数
*/
/*u8 TOUCH_Get_Coordinates(u16 *DATA)
{
	u8 Number,i=0;
	u8 Original_Coordinates[4];
	u16 Coordinates_XY[2]={0};
	
	if ((Number=TOUCH_Get_Points_Number())==0)
	{
		TOUCH_Write(0x814e,&i,1);
		return 0;
	}
	
	for (i=0;i<Number;i++,Coordinates_XY[0]=0,Coordinates_XY[1]=0)
	{
		TOUCH_Read(0x8150+i*8,Original_Coordinates,4);
		Coordinates_XY[0]|=Original_Coordinates[1];
		Coordinates_XY[0]<<=8;
		Coordinates_XY[0]|=Original_Coordinates[0];
		Coordinates_XY[1]|=Original_Coordinates[3];
		Coordinates_XY[1]<<=8;
		Coordinates_XY[1]|=Original_Coordinates[2];
		DATA[i*2]=Coordinates_XY[0];
		DATA[i*2+1]=Coordinates_XY[1];
	}
	i=0;
	TOUCH_Write(0x814e,&i,1);
	
	return Number;
}*/
