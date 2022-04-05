/*
*版本：

2021.6.21（14组）：
【Framework】
1.【重大更新】增加对F7/H7的支持，F7/H7现在可以使用GPIO、SystemTimer全部功能，并且与F4/F1完全兼容
2.改写了System_Timer的操作方法
    ①增加SystemTimer_Init方法
    ②增加SystemTimer_Cancel方法
    ③删除SystemTimer_Reset方法
    ④SystemTimer_Delay_Us的参数改为float类型，该函数现在可以实现1us以内的延时
3.GPIO部分增加GPIO_Mode_In和GPIO_Mode_Out方法
4.GPIO写函数发生变化，现在的写函数为GPIO_Write(x,y)或GPIO_Pin_Set/GPIO_Pin_Reset

2021.6.30：
1.现在可以选择SysTick的时钟源

*/

#ifndef _FRAMEWORK_H_
#define _FRAMEWORK_H_

#include <stdint.h>
#include <stddef.h>

/****************************************************************************************/
/*                                                                                      */
/*                                       配置区                                         */
/*                                                                                      */
/****************************************************************************************/

/*======================================================================================*/

/*
*配置区的参数（宏）需要用户根据自己的实际情况自行配置，如果有“!必须的!”这样的标记，则说明
*这个宏必须被合理配置；如果有“（可选的）”这样的标记，则说明该宏对应着可选功能，如果用户并
*不想使用该功能，就可以不配置。
*
*/

/*******************************
*!必须的!                      *
*这些宏指明你使用的器件类型    *
*这会影响到后面GPIO操作部分    *
********************************/
#define USE_STM32H7 //H7系列
//#define USE_STM32F7   //F7系列
//#define USE_STM32F4   //F4系列
//#define USE_STM32F1   //F1系列
/*******************************/

/**********************************
*!必须的！                        *
*包含对应的头文件（stm32xxxx.h）  *
***********************************/
#include "stm32h743xx.h"
/**********************************/

/**************************************
*（可选的）                           *
*定义你的器件的最大主频               *
*对于F103系列这个值为72000000         *
*对于F407系列这个值为168000000        *
*对于F429/439系列这个值为180000000    *
*其他自己添加...                      *
***************************************/
//#define SYSCLK_MAX_CLOCK    48000000
//#define SYSCLK_MAX_CLOCK    72000000
//#define SYSCLK_MAX_CLOCK    168000000
//#define SYSCLK_MAX_CLOCK    180000000
#define SYSCLK_MAX_CLOCK 400000000
/**************************************/

/****************************************
*!必须的!                               *
*系统定时器的类型                       *
*系统定时器被用来提供一个公用的时钟基准 *
*它的作用是提供通用的延时函数和计时方法 *
*0：不使用系统定时器                    *
*1：systick定时器                       *
*2：用户自定义                          *
*****************************************/
#define SYSTEM_TIMER 1
/****************************************/

#if SYSTEM_TIMER != 0

/**************************************
*!必须的!                             *
*指定定时器的时钟频率对于systick定时  *
*器这个值通常是SYSCLK或               *
*SYSCLK/8                             *
***************************************/
#define SYSTEM_TIMER_CLOCK SYSCLK_MAX_CLOCK

#if SYSTEM_TIMER == 1

/**************************************
*!必须的!                             *
*指定SysTick定时器的时钟源            *
1：内部（也就是系统主频）             *
0：外部（系统主频或系统主频/8）       *
***************************************/
#define SYSTICK_CLOCK_SOURCE 1

#endif
/**************************************/

#endif

/***********************************************************
*!必须的!                                                  *
*STM32中各种外设寄存器的定义                               *
*要使用的就把注释取消，如果没有你要用的外设可以自己添加进去*
************************************************************/
typedef union
{
	//ADC_TypeDef *adc;
	//ADC_Common_TypeDef*         adc_common;
	//CAN_TxMailBox_TypeDef*      can_txmailbox;
	//CAN_FIFOMailBox_TypeDef*    can_fifomailbox;
	//CAN_FilterRegister_TypeDef* filterregister;
	//CAN_TypeDef*                can;
	//CRC_TypeDef*                crc;
	//DAC_TypeDef*                dac;
	//DBGMCU_TypeDef*             debugmcu;
	//DCMI_TypeDef*               dcmi;
	//DMA_Stream_TypeDef*         dma_stream;
	//DMA_TypeDef*                dma;
	//DMA2D_TypeDef*              dma2d;
	//ETH_TypeDef*                eth;
	//EXTI_TypeDef*               exti;
	//FLASH_TypeDef*              flash;
	//FMC_Bank1_TypeDef*          fmc_bank1;
	//FMC_Bank1E_TypeDef*         fmc_bank1e;
	//FMC_Bank2_3_TypeDef*        fmc_bank2_3;
	//FMC_Bank4_TypeDef*          fmc_bank4;
	//FMC_Bank5_6_TypeDef*        fmc_bank5_6;
	//GPIO_TypeDef*               gpio;
	//SYSCFG_TypeDef*             syscfg;
	//I2C_TypeDef*                i2c;
	//IWDG_TypeDef*               iwdg;
	//LTDC_TypeDef*               ltdc;
	//LTDC_Layer_TypeDef*         ltdc_layer;
	//PWR_TypeDef*                pwr;
	//RCC_TypeDef*                rcc;
	//RTC_TypeDef*                rtc;
	//SAI_TypeDef*                sai;
	//SAI_Block_TypeDef*          sai_block;
	//SDIO_TypeDef*               sdio;
	//SPI_TypeDef *spi;
	//TIM_TypeDef*                tim;
	//USART_TypeDef*              usart;
	//WWDG_TypeDef*               wwdg;
	//RNG_TypeDef*                rng;
	//USB_OTG_GlobalTypeDef*      usa_otg_global;
	//USB_OTG_DeviceTypeDef*      usb_otg_device;
	//USB_OTG_INEndpointTypeDef*  usb_otg_inendpoint;
	//USB_OTG_OUTEndpointTypeDef* usb_otg_outendpoint;
	//USB_OTG_HostTypeDef*        usb_otg_host;
	//USB_OTG_HostChannelTypeDef* usb_otg_hostchannel;
} peripheral_register_type;
/**********************************************************/

/*====================================END=====================================*/

/****************************************************************************************/
/*                                                                                      */
/*                                       定义区                                         */
/*                                                                                      */
/****************************************************************************************/

/*======================================================================================*/

#if defined USE_STM32F4 || defined USE_STM32F1
#define AHB_MAX_CLOCK SYSCLK_MAX_CLOCK
#define APB1_MAX_CLOCK (SYSCLK_MAX_CLOCK >> 2)
#define APB2_MAX_CLOCK (SYSCLK_MAX_CLOCK >> 1)
#else
#define AHB_MAX_CLOCK (SYSCLK_MAX_CLOCK >> 1)
#define APB1_MAX_CLOCK (SYSCLK_MAX_CLOCK >> 2)
#define APB2_MAX_CLOCK (SYSCLK_MAX_CLOCK >> 2)
#endif

/*
*为了增强代码可读性，定义几种可能会用到的数据类型
*1.sign：标志。
*        如果一个变量只起到标志的作用（只具有有限的几种状态），
*        那么应该有sign修饰，例如一个变量用来表示ADC是否读取完
*        毕，那么改变量就是一个标志位，它的值只有逻辑的意义，
*        没有实际大小的意义。
*2.reg：寄存器。
*       起到寄存器作用的变量用其修饰，例如该变量对应了外部芯片
*       中的某个实际的寄存器，就应该有reg修饰。
*3.ro：只读。
*      该变量只希望被用户读取，用户不应该改变其值。如果一个被
*      ro修饰，那么该变量在最初赋值以后（最好）在整个程序执行
*      过程都不会改变。
*4.vro：只读并且可能经常被_RO_WRITE方法改变
*/
#define ro const		   //只读类型
#define vro volatile const //vro

//只读类型的写入方法
//三个参数意义：对象、对象的数据类型、要写入的值
#define _RO_WRITE(object, type, value) *((type *)((uint32_t)(&object))) = value

typedef void *inst_type; //实例类型，对应各种类的实例

/*
*外设接口类
*针对寄存器写法、标准库写法、HAL库写法的外设接口
*定义该类的目的在于让各种写法都能以统一方式访问外设
*/
typedef struct peripheral_interface_class
{
	//存放外设的寄存器地址，如：TIM1，USART1，SPI2等
	ro peripheral_register_type Register0;

	//（备用的）存放外设的寄存器地址，如：TIM1，USART1，SPI2等
	//如果一个Register0不够用才可以用这个
	ro peripheral_register_type Register1;

	//存放某些库中定义的结构体
	//如HAL库中的SPI_Handler等
	ro inst_type Instance;

	//由用户自行决定其作用（如果有必要使用的话），当然也可以不使用
	ro uint32_t Value;
} periperal;

//C语言基本数据类型
//在进行数据类型转换的时候，这些定义可能会有用
typedef enum
{
	UCHAR_Format,
	CHAR_Format,
	USHORT_Format,
	SHORT_Format,
	INT_Format,
	UINT_Format,
	FLOAT_Format,
	DOUBLE_Format
} data_format_type;

//硬件或软件
//指明一些外设功能的实现方法
//如硬件SPI或软件模拟SPI
typedef enum
{
	SOFTWARE,
	HARDWARE
} S_H_type;

typedef enum
{
	Enable = 1,
	Disable = 0
} status_flag;

#define Mask1Bit 0x00000001
#define Mask2Bit 0x00000003
#define Mask3Bit 0x00000007
#define Mask4Bit 0x0000000f
#define Mask5Bit 0x0000001f
#define Mask6Bit 0x0000003f
#define Mask7Bit 0x0000007f
#define Mask8Bit 0x000000ff
#define Mask9Bit 0x000001ff
#define Mask10Bit 0x000003ff
#define Mask11Bit 0x000007ff
#define Mask12Bit 0x00000fff
#define Mask13Bit 0x00001fff
#define Mask14Bit 0x00003fff
#define Mask15Bit 0x00007fff
#define Mask16Bit 0x0000ffff
#define Mask17Bit 0x0001ffff
#define Mask18Bit 0x0003ffff
#define Mask19Bit 0x0007ffff
#define Mask20Bit 0x000fffff
#define Mask21Bit 0x001fffff
#define Mask22Bit 0x003fffff
#define Mask23Bit 0x007fffff
#define Mask24Bit 0x00ffffff
#define Mask25Bit 0x01ffffff
#define Mask26Bit 0x03ffffff
#define Mask27Bit 0x07ffffff
#define Mask28Bit 0x0fffffff
#define Mask29Bit 0x1fffffff
#define Mask30Bit 0x3fffffff
#define Mask31Bit 0x7fffffff
#define Mask32Bit 0xffffffff

#define BitClear(target, offset, mask) (target) &= ~((mask) << (offset))
#define BitSet(target, offset, mask, data) (target) |= ((data) & (mask)) << (offset)
#define BitClearSet(target, offset, mask, data) (target) = ((target) & ~((mask) << (offset))) | ((data) << (offset))
#define BitRead(target, offset, mask) (((target) >> offset) & mask)

/*========================================END===========================================*/

/****************************************************************************************/
/*                                                                                      */
/*                                       GPIO操作                                       */
/*                                                                                      */
/****************************************************************************************/

/*======================================================================================*/

/*
*这部分代码参考了正点原子
*如果使用F4/F1则原理是利用Cortex内核的位带操作功能，实现GPIO的高效读/写
*如果使用F7/H7则原理是操作GPIO寄存器
*所有操作使用宏实现，提高操作速度
*细节不加赘述，下面是使用方法
*
*
*1.GPIO的拉高/低(GPIO_Write())
*例：GPIO_Write(GPIOA_6,1);该语句将PA6拉高
*例：GPIO_Write(GPIOI_15,0);该语句将PI15拉低
*或者：
*GPIO_Pin_Set(GPIOB_3);该语句将PB3拉高
*GPIO_Pin_Reset(GPIOF_3);该语句将PF3拉低
*
*2.GPIO输入读取(GPIO_Read())
*例：GPIO_Read(GPIOB_0);该语句返回PB0上的输入值
*
*3.获取IO组中某个端口的偏移(GPIO_Get_Pin_Addr())
*例：GPIO_Get_Pin_Addr(GPIOB_3);该语句返回:1<<3
*例：GPIO_Get_Pin_Addr(GPIOA_12);该语句返回:1<<12
*
*4.获取IO组中某个端口的序号(GPIO_Get_Pin_Num())
*例：GPIO_Get_Pin_Num(GPIOA_3);该语句返回:3
*例：GPIO_Get_Pin_Num(GPIOA_12);该语句返回:12
*
*5.GPIO输入/输出模式设置，这一点在一些通信协议里面会很有用（如IIC）
*例：GPIO_Mode_In(GPIOA_4);该语句将PA4设置为输入模式
*例：GPIO_Mode_Out(GPIOC_8);该语句将PC8设置为输出模式
*/

//////////////////////////////////////////////////////////////////////////////////

//bit0~bit15：1<<Pinx
//bit16~bit19：GPIOA=0,GPIOB=1,GPIOC=2,...
//bit20~bit23：PIN0_ADDR=0,PIN1_ADDR=1,PIN3_ADDR=3,...

//GPIO引脚编号定义
/*typedef enum
{
	PIN0_ADDR 	= 1 << 0,
	PIN1_ADDR 	= 1 << 1,
	PIN2_ADDR 	= 1 << 2,
	PIN3_ADDR 	= 1 << 3,
	PIN4_ADDR 	= 1 << 4,
	PIN5_ADDR 	= 1 << 5,
	PIN6_ADDR 	= 1 << 6,
	PIN7_ADDR 	= 1 << 7,
	PIN8_ADDR 	= 1 << 8,
	PIN9_ADDR 	= 1 << 9,
	PIN10_ADDR 	= 1 << 10,
	PIN11_ADDR 	= 1 << 11,
	PIN12_ADDR 	= 1 << 12,
	PIN13_ADDR 	= 1 << 13,
	PIN14_ADDR 	= 1 << 14,
	PIN15_ADDR 	= 1 << 15
} pin_type;

typedef enum
{
	GPIO_GROUPA = 0 << 16,
	GPIO_GROUPB = 1 << 16,
	GPIO_GROUPC = 2 << 16,
	GPIO_GROUPD = 3 << 16,
	GPIO_GROUPE = 4 << 16,
	GPIO_GROUPF = 5 << 16,
	GPIO_GROUPG = 6 << 16,
	GPIO_GROUPH = 7 << 16,
	GPIO_GROUPI = 8 << 16
} gpio_group_type;

typedef enum
{
	PIN0_NUM 	= 0 << 20,
	PIN1_NUM 	= 1 << 20,
	PIN2_NUM 	= 2 << 20,
	PIN3_NUM 	= 3 << 20,
	PIN4_NUM 	= 4 << 20,
	PIN5_NUM 	= 5 << 20,
	PIN6_NUM 	= 6 << 20,
	PIN7_NUM 	= 7 << 20,
	PIN8_NUM 	= 8 << 20,
	PIN9_NUM 	= 9 << 20,
	PIN10_NUM 	= 10 << 20,
	PIN11_NUM 	= 11 << 20,
	PIN12_NUM 	= 12 << 20,
	PIN13_NUM 	= 13 << 20,
	PIN14_NUM 	= 14 << 20,
	PIN15_NUM 	= 15 << 20
} pin_num_type;

typedef enum
{
	GPIOA_0 = PIN0_ADDR + PIN0_NUM + GPIO_GROUPA,
	GPIOA_1 = PIN1_ADDR + PIN1_NUM + GPIO_GROUPA,
	GPIOA_2 = PIN2_ADDR + PIN2_NUM + GPIO_GROUPA,
	GPIOA_3 = PIN3_ADDR + PIN3_NUM + GPIO_GROUPA,
	GPIOA_4 = PIN4_ADDR + PIN4_NUM + GPIO_GROUPA,
	GPIOA_5 = PIN5_ADDR + PIN5_NUM + GPIO_GROUPA,
	GPIOA_6 = PIN6_ADDR + PIN6_NUM + GPIO_GROUPA,
	GPIOA_7 = PIN7_ADDR + PIN7_NUM + GPIO_GROUPA,
	GPIOA_8 = PIN8_ADDR + PIN8_NUM + GPIO_GROUPA,
	GPIOA_9 = PIN9_ADDR + PIN9_NUM + GPIO_GROUPA,
	GPIOA_10 = PIN10_ADDR + PIN10_NUM + GPIO_GROUPA,
	GPIOA_11 = PIN11_ADDR + PIN11_NUM + GPIO_GROUPA,
	GPIOA_12 = PIN12_ADDR + PIN12_NUM + GPIO_GROUPA,
	GPIOA_13 = PIN13_ADDR + PIN13_NUM + GPIO_GROUPA,
	GPIOA_14 = PIN14_ADDR + PIN14_NUM + GPIO_GROUPA,
	GPIOA_15 = PIN15_ADDR + PIN15_NUM + GPIO_GROUPA,

	GPIOB_0 = PIN0_ADDR + PIN0_NUM + GPIO_GROUPB,
	GPIOB_1 = PIN1_ADDR + PIN1_NUM + GPIO_GROUPB,
	GPIOB_2 = PIN2_ADDR + PIN2_NUM + GPIO_GROUPB,
	GPIOB_3 = PIN3_ADDR + PIN3_NUM + GPIO_GROUPB,
	GPIOB_4 = PIN4_ADDR + PIN4_NUM + GPIO_GROUPB,
	GPIOB_5 = PIN5_ADDR + PIN5_NUM + GPIO_GROUPB,
	GPIOB_6 = PIN6_ADDR + PIN6_NUM + GPIO_GROUPB,
	GPIOB_7 = PIN7_ADDR + PIN7_NUM + GPIO_GROUPB,
	GPIOB_8 = PIN8_ADDR + PIN8_NUM + GPIO_GROUPB,
	GPIOB_9 = PIN9_ADDR + PIN9_NUM + GPIO_GROUPB,
	GPIOB_10 = PIN10_ADDR + PIN10_NUM + GPIO_GROUPB,
	GPIOB_11 = PIN11_ADDR + PIN11_NUM + GPIO_GROUPB,
	GPIOB_12 = PIN12_ADDR + PIN12_NUM + GPIO_GROUPB,
	GPIOB_13 = PIN13_ADDR + PIN13_NUM + GPIO_GROUPB,
	GPIOB_14 = PIN14_ADDR + PIN14_NUM + GPIO_GROUPB,
	GPIOB_15 = PIN15_ADDR + PIN15_NUM + GPIO_GROUPB,

	GPIOC_0 = PIN0_ADDR + PIN0_NUM + GPIO_GROUPC,
	GPIOC_1 = PIN1_ADDR + PIN1_NUM + GPIO_GROUPC,
	GPIOC_2 = PIN2_ADDR + PIN2_NUM + GPIO_GROUPC,
	GPIOC_3 = PIN3_ADDR + PIN3_NUM + GPIO_GROUPC,
	GPIOC_4 = PIN4_ADDR + PIN4_NUM + GPIO_GROUPC,
	GPIOC_5 = PIN5_ADDR + PIN5_NUM + GPIO_GROUPC,
	GPIOC_6 = PIN6_ADDR + PIN6_NUM + GPIO_GROUPC,
	GPIOC_7 = PIN7_ADDR + PIN7_NUM + GPIO_GROUPC,
	GPIOC_8 = PIN8_ADDR + PIN8_NUM + GPIO_GROUPC,
	GPIOC_9 = PIN9_ADDR + PIN9_NUM + GPIO_GROUPC,
	GPIOC_10 = PIN10_ADDR + PIN10_NUM + GPIO_GROUPC,
	GPIOC_11 = PIN11_ADDR + PIN11_NUM + GPIO_GROUPC,
	GPIOC_12 = PIN12_ADDR + PIN12_NUM + GPIO_GROUPC,
	GPIOC_13 = PIN13_ADDR + PIN13_NUM + GPIO_GROUPC,
	GPIOC_14 = PIN14_ADDR + PIN14_NUM + GPIO_GROUPC,
	GPIOC_15 = PIN15_ADDR + PIN15_NUM + GPIO_GROUPC,

	GPIOD_0 = PIN0_ADDR + PIN0_NUM + GPIO_GROUPD,
	GPIOD_1 = PIN1_ADDR + PIN1_NUM + GPIO_GROUPD,
	GPIOD_2 = PIN2_ADDR + PIN2_NUM + GPIO_GROUPD,
	GPIOD_3 = PIN3_ADDR + PIN3_NUM + GPIO_GROUPD,
	GPIOD_4 = PIN4_ADDR + PIN4_NUM + GPIO_GROUPD,
	GPIOD_5 = PIN5_ADDR + PIN5_NUM + GPIO_GROUPD,
	GPIOD_6 = PIN6_ADDR + PIN6_NUM + GPIO_GROUPD,
	GPIOD_7 = PIN7_ADDR + PIN7_NUM + GPIO_GROUPD,
	GPIOD_8 = PIN8_ADDR + PIN8_NUM + GPIO_GROUPD,
	GPIOD_9 = PIN9_ADDR + PIN9_NUM + GPIO_GROUPD,
	GPIOD_10 = PIN10_ADDR + PIN10_NUM + GPIO_GROUPD,
	GPIOD_11 = PIN11_ADDR + PIN11_NUM + GPIO_GROUPD,
	GPIOD_12 = PIN12_ADDR + PIN12_NUM + GPIO_GROUPD,
	GPIOD_13 = PIN13_ADDR + PIN13_NUM + GPIO_GROUPD,
	GPIOD_14 = PIN14_ADDR + PIN14_NUM + GPIO_GROUPD,
	GPIOD_15 = PIN15_ADDR + PIN15_NUM + GPIO_GROUPD,

	GPIOE_0 = PIN0_ADDR + PIN0_NUM + GPIO_GROUPE,
	GPIOE_1 = PIN1_ADDR + PIN1_NUM + GPIO_GROUPE,
	GPIOE_2 = PIN2_ADDR + PIN2_NUM + GPIO_GROUPE,
	GPIOE_3 = PIN3_ADDR + PIN3_NUM + GPIO_GROUPE,
	GPIOE_4 = PIN4_ADDR + PIN4_NUM + GPIO_GROUPE,
	GPIOE_5 = PIN5_ADDR + PIN5_NUM + GPIO_GROUPE,
	GPIOE_6 = PIN6_ADDR + PIN6_NUM + GPIO_GROUPE,
	GPIOE_7 = PIN7_ADDR + PIN7_NUM + GPIO_GROUPE,
	GPIOE_8 = PIN8_ADDR + PIN8_NUM + GPIO_GROUPE,
	GPIOE_9 = PIN9_ADDR + PIN9_NUM + GPIO_GROUPE,
	GPIOE_10 = PIN10_ADDR + PIN10_NUM + GPIO_GROUPE,
	GPIOE_11 = PIN11_ADDR + PIN11_NUM + GPIO_GROUPE,
	GPIOE_12 = PIN12_ADDR + PIN12_NUM + GPIO_GROUPE,
	GPIOE_13 = PIN13_ADDR + PIN13_NUM + GPIO_GROUPE,
	GPIOE_14 = PIN14_ADDR + PIN14_NUM + GPIO_GROUPE,
	GPIOE_15 = PIN15_ADDR + PIN15_NUM + GPIO_GROUPE,

	GPIOF_0 = PIN0_ADDR + PIN0_NUM + GPIO_GROUPF,
	GPIOF_1 = PIN1_ADDR + PIN1_NUM + GPIO_GROUPF,
	GPIOF_2 = PIN2_ADDR + PIN2_NUM + GPIO_GROUPF,
	GPIOF_3 = PIN3_ADDR + PIN3_NUM + GPIO_GROUPF,
	GPIOF_4 = PIN4_ADDR + PIN4_NUM + GPIO_GROUPF,
	GPIOF_5 = PIN5_ADDR + PIN5_NUM + GPIO_GROUPF,
	GPIOF_6 = PIN6_ADDR + PIN6_NUM + GPIO_GROUPF,
	GPIOF_7 = PIN7_ADDR + PIN7_NUM + GPIO_GROUPF,
	GPIOF_8 = PIN8_ADDR + PIN8_NUM + GPIO_GROUPF,
	GPIOF_9 = PIN9_ADDR + PIN9_NUM + GPIO_GROUPF,
	GPIOF_10 = PIN10_ADDR + PIN10_NUM + GPIO_GROUPF,
	GPIOF_11 = PIN11_ADDR + PIN11_NUM + GPIO_GROUPF,
	GPIOF_12 = PIN12_ADDR + PIN12_NUM + GPIO_GROUPF,
	GPIOF_13 = PIN13_ADDR + PIN13_NUM + GPIO_GROUPF,
	GPIOF_14 = PIN14_ADDR + PIN14_NUM + GPIO_GROUPF,
	GPIOF_15 = PIN15_ADDR + PIN15_NUM + GPIO_GROUPF,

	GPIOG_0 = PIN0_ADDR + PIN0_NUM + GPIO_GROUPG,
	GPIOG_1 = PIN1_ADDR + PIN1_NUM + GPIO_GROUPG,
	GPIOG_2 = PIN2_ADDR + PIN2_NUM + GPIO_GROUPG,
	GPIOG_3 = PIN3_ADDR + PIN3_NUM + GPIO_GROUPG,
	GPIOG_4 = PIN4_ADDR + PIN4_NUM + GPIO_GROUPG,
	GPIOG_5 = PIN5_ADDR + PIN5_NUM + GPIO_GROUPG,
	GPIOG_6 = PIN6_ADDR + PIN6_NUM + GPIO_GROUPG,
	GPIOG_7 = PIN7_ADDR + PIN7_NUM + GPIO_GROUPG,
	GPIOG_8 = PIN8_ADDR + PIN8_NUM + GPIO_GROUPG,
	GPIOG_9 = PIN9_ADDR + PIN9_NUM + GPIO_GROUPG,
	GPIOG_10 = PIN10_ADDR + PIN10_NUM + GPIO_GROUPG,
	GPIOG_11 = PIN11_ADDR + PIN11_NUM + GPIO_GROUPG,
	GPIOG_12 = PIN12_ADDR + PIN12_NUM + GPIO_GROUPG,
	GPIOG_13 = PIN13_ADDR + PIN13_NUM + GPIO_GROUPG,
	GPIOG_14 = PIN14_ADDR + PIN14_NUM + GPIO_GROUPG,
	GPIOG_15 = PIN15_ADDR + PIN15_NUM + GPIO_GROUPG,

	GPIOH_0 = PIN0_ADDR + PIN0_NUM + GPIO_GROUPH,
	GPIOH_1 = PIN1_ADDR + PIN1_NUM + GPIO_GROUPH,
	GPIOH_2 = PIN2_ADDR + PIN2_NUM + GPIO_GROUPH,
	GPIOH_3 = PIN3_ADDR + PIN3_NUM + GPIO_GROUPH,
	GPIOH_4 = PIN4_ADDR + PIN4_NUM + GPIO_GROUPH,
	GPIOH_5 = PIN5_ADDR + PIN5_NUM + GPIO_GROUPH,
	GPIOH_6 = PIN6_ADDR + PIN6_NUM + GPIO_GROUPH,
	GPIOH_7 = PIN7_ADDR + PIN7_NUM + GPIO_GROUPH,
	GPIOH_8 = PIN8_ADDR + PIN8_NUM + GPIO_GROUPH,
	GPIOH_9 = PIN9_ADDR + PIN9_NUM + GPIO_GROUPH,
	GPIOH_10 = PIN10_ADDR + PIN10_NUM + GPIO_GROUPH,
	GPIOH_11 = PIN11_ADDR + PIN11_NUM + GPIO_GROUPH,
	GPIOH_12 = PIN12_ADDR + PIN12_NUM + GPIO_GROUPH,
	GPIOH_13 = PIN13_ADDR + PIN13_NUM + GPIO_GROUPH,
	GPIOH_14 = PIN14_ADDR + PIN14_NUM + GPIO_GROUPH,
	GPIOH_15 = PIN15_ADDR + PIN15_NUM + GPIO_GROUPH,

	GPIOI_0 = PIN0_ADDR + PIN0_NUM + GPIO_GROUPI,
	GPIOI_1 = PIN1_ADDR + PIN1_NUM + GPIO_GROUPI,
	GPIOI_2 = PIN2_ADDR + PIN2_NUM + GPIO_GROUPI,
	GPIOI_3 = PIN3_ADDR + PIN3_NUM + GPIO_GROUPI,
	GPIOI_4 = PIN4_ADDR + PIN4_NUM + GPIO_GROUPI,
	GPIOI_5 = PIN5_ADDR + PIN5_NUM + GPIO_GROUPI,
	GPIOI_6 = PIN6_ADDR + PIN6_NUM + GPIO_GROUPI,
	GPIOI_7 = PIN7_ADDR + PIN7_NUM + GPIO_GROUPI,
	GPIOI_8 = PIN8_ADDR + PIN8_NUM + GPIO_GROUPI,
	GPIOI_9 = PIN9_ADDR + PIN9_NUM + GPIO_GROUPI,
	GPIOI_10 = PIN10_ADDR + PIN10_NUM + GPIO_GROUPI,
	GPIOI_11 = PIN11_ADDR + PIN11_NUM + GPIO_GROUPI,
	GPIOI_12 = PIN12_ADDR + PIN12_NUM + GPIO_GROUPI,
	GPIOI_13 = PIN13_ADDR + PIN13_NUM + GPIO_GROUPI,
	GPIOI_14 = PIN14_ADDR + PIN14_NUM + GPIO_GROUPI,
	GPIOI_15 = PIN15_ADDR + PIN15_NUM + GPIO_GROUPI,

	GPIO_NONE
} gpio_type;

#define GPIO_Get_Pin_Num(x) 	(x >> 20)
#define _Get_GPIO_Num(x) 		((x >> 16) & 0xf)

#define GPIO_Get_Pin_Addr(x) 	(x & 0xffff)
#define _Get_GPIO_Addr(x) 		((GPIO_TypeDef *)((_Get_GPIO_Num(x) << 10) + GPIOA_BASE))

#define GPIO_Mode_In(x) 		_Get_GPIO_Addr(x)->MODER &= ~(3 << (GPIO_Get_Pin_Num(x) << 1))
#define GPIO_Mode_Out(x) 		\
								GPIO_Mode_In(x);     \
								_Get_GPIO_Addr(x)->MODER |= 1 << (GPIO_Get_Pin_Num(x) << 1)*/

#if defined USE_STM32F4 || defined USE_STM32F1
//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).M4同M3类似,只是寄存器地址变了.
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF) << 5) + (bitnum << 2))
#define MEM_ADDR(addr) *((volatile unsigned long *)(addr))
#define BIT_ADDR(addr, bitnum) MEM_ADDR(BITBAND(addr, bitnum))

//ODR或IDR地址偏移量
#if defined USE_STM32F4
#define ODR_OFFSET 20
#define IDR_OFFSET 16
#elif defined USE_STM32F1
#define ODR_OFFSET 12
#define IDR_OFFSET 8
#endif

#define _GPIO_Write(x) BIT_ADDR(((uint32_t)_Get_GPIO_Addr(x) + ODR_OFFSET), GPIO_Get_Pin_Num(x))
#define GPIO_Read(x) BIT_ADDR(((uint32_t)_Get_GPIO_Addr(x) + IDR_OFFSET), GPIO_Get_Pin_Num(x))

#define GPIO_Write(x, y) _GPIO_Write(x) = y

#define GPIO_Pin_Set(x) _GPIO_Write(x) = 1
#define GPIO_Pin_Reset(x) _GPIO_Write(x) = 0

#endif

#if defined USE_STM32F7 || defined USE_STM32H7

/*#define BSRR_OFFSET 0x18
#define IDR_OFFSET 0x10

//x：GPIO端口，y：要写入的值（0/1）
#define GPIO_Write(x, y) *(((uint16_t *)((uint32_t)_Get_GPIO_Addr(x) + BSRR_OFFSET)) + ((~(y)) & 1)) |= GPIO_Get_Pin_Addr(x)
#define GPIO_Read(x) (((*((uint16_t *)((uint32_t)_Get_GPIO_Addr(x) + IDR_OFFSET))) >> GPIO_Get_Pin_Num(x)) & 1)

#define GPIO_Pin_Set(x) GPIO_Write(x, 1)
#define GPIO_Pin_Reset(x) GPIO_Write(x, 0)*/

#endif
/*=========================================END==========================================*/

/****************************************************************************************/
/*                                                                                      */
/*                                     系统定时器                                       */
/*                                                                                      */
/****************************************************************************************/

/*======================================================================================*/

/*
*系统定时器是一个公用的定时器，为整个系统提供通用的、简单的、标准的倒计时或延时功能
*任何一个库的代码都可以使用该定时器，并且该定时器可以在不同的库中被复用（不用再为每个器件单独分配一个外设定时器了）
*默认情况下该定时器是SysTick定时器（推荐），但是用户也可以指配为其他定时器
*当系统定时器被指定为SysTick定时器时，用户不需要为其编写任何驱动代码，而可以直接使用
*但是要注意不同器件中SysTick的时钟频率可能不同，在配置区要为系统定时器提供正确的时钟频率，否则会导致定时时间不准
*！！如果使用F4/F1系列，不要和正点原子的delay函数混用！！
*倒计时功能演示：
*
*SystemTimer_Timing_Ready(1000);   //系统定时器准备，计时时间为：x(us)
*SystemTimer_Timing_Begin();    //开始倒计时
*for (i=0;i<10;i++)
*{
*   Process();                     //执行一些操作
*   while (SystemTimer_Timing_Check()==0);  //等待倒计时结束
*}
*
*执行上面程序的结果是：Process()被执行了10次，而且每1000us才执行一次
*
*倒计时准备后如果没有调用开始倒计时函数，则应当取消倒计时：
*SystemTimer_Timing_Ready(1000);   //系统定时器准备，计时时间为：x(us)
*SystemTimer_Timing_Cancel();    //取消倒计时
*
*
*
*/

//如果使用系统定时器
#if SYSTEM_TIMER != 0

#define VTIMER_MAX_NUM 16

typedef int8_t vtimer_handler;

void SystemTimer_Init(void);

//us级延时
void SystemTimer_Delay_Us(float time);

//ms级延时
void SystemTimer_Delay_Ms(uint32_t time);

//s级延时
void SystemTimer_Delay_S(uint32_t time);

//虚拟定时器注册
vtimer_handler SystemTimer_VTimer_Regist(uint32_t time);

//虚拟定时器更改定时时间
void SystemTimer_VTimer_ChangeTime(vtimer_handler vt, uint32_t time);

//虚拟定时器开始计时
void SystemTimer_VTimer_Start(void);

//虚拟定时器暂停
void SystemTimer_VTimer_Pause(void);

//虚拟定时器继续
void SystemTimer_VTimer_Resume(void);

//虚拟定时器倒计时检查
status_flag SystemTimer_VTimer_Check(vtimer_handler vt);

//如果使用SysTick定时器
#if SYSTEM_TIMER == 1

//开始倒计时
#define SystemTimer_Timing_Begin() SysTick->CTRL |= 1

//检查倒计时结束标志位
#define SystemTimer_Timing_Check() ((SysTick->CTRL >> 16) & 1)

//取消倒计时
#define SystemTimer_Timing_Cancel() SysTick->CTRL |= 1

//准备倒计时
void SystemTimer_Timing_Ready(float time_us);

#define delay_ms(x) SystemTimer_Delay_Ms(x)
#define delay_us(x) SystemTimer_Delay_Us(x)

#elif SYSTEM_TIMER == 2

void SystemTimer_Timing_Ready(uint32_t time_us);

void SystemTimer_Reset();

void SystemTimer_Timing_Begin();

uint8_t SystemTimer_Timing_Check();

void SystemTimer_Timing_End();

void SystemTimer_Reset(void);

#endif

#endif

/*===========================================END========================================*/

#endif
