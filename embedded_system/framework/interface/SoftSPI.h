
/*
* 软件SPI收发器驱动
* 一、功能介绍
*	该库提供了通用的软件模拟SPI收发器的功能，允许用户生成一个自定义的
*	SPI收发器，用户可以定义该SPI收发器的时钟相位（CPHA）、时钟极性（CPOL）、
*	时钟频率，且一个时钟周期内高/低电平时间都可以分别设置，此外，还允许设置
*	从1~32的帧长度。SPI收发器可以仅工作在接收或发送模式。
* 二、参考例程
*	下面代码使用GPIOC0（SCLK），GPIOC1（MOSI），GPIOC2（MISO）构造了一个工作在
*	主模式、CPHA=1、CPOL=1、帧长=16、LSB先发/收、CS不受控制、时钟周期4us（占空
*	比50%）的SPI收发器，且用它交换了16帧信息
*	
*	GPIO_Init(GPIOC0,GPIO_MODE_OUT,GPIO_OTYPE_PP);	//GPIOC0，推挽输出
*	GPIO_Init(GPIOC1,GPIO_MODE_OUT,GPIO_OTYPE_PP);	//GPIOC1，推挽输出
*	GPIO_Init(GPIOC2,GPIO_MODE_IN,GPIO_PUPD_NONE);	//GPIOC2，浮空输入
*	
*	uint16_t txbuf[16];	//发送缓冲区
*	uint16_t rxbuf[16];	//接收缓冲区
*
*	softspi my_softspi;	//声明类实例
*	
*	SoftSPI_Prepare(&my_softspi,SOFTSPI_MASTER,CPHA1,CPOL1,16,SOFTSPI_LSB,GPIOC0,GPIOC1,GPIOC2,0);	//初始化实例
*	my_softspi.ValidTime	=	2;			//有效时间2us
*	my_softspi.InvalidTime	=	2;			//空闲时间2us
*	my_softspi.CS_Control	=	Disable;	//不允许CS控制
*	
*	my_softspi.Transmit_DataFlow(&my_softspi,txbuf,rxbuf,16);	//开始一次数据交换（一共16帧）
* 
*/

#ifndef _SOFTSPI_H_
#define _SOFTSPI_H_

#include "sys.h"
#include "GPIO.h"

//软件SPI类
typedef struct softspi_class softspi;

//定义SPI的主/从模式
typedef enum
{
    SOFTSPI_MASTER, //SPI收发器工作在主模式
    SOFTSPI_SLAVE   //SPI收发器工作在从模式
} softspi_mode_type;

//定义SPI收发器的CPHA参数
typedef enum
{
    CPHA0, //CPHA=0，在时钟第一个边沿采样
    CPHA1  //CPHA=1，在时钟第二个边沿采样
} softspi_cpha_type;

//定义SPI收发器的CPOL参数
typedef enum
{
    CPOL0 = 0, //CPOL=0，空闲时刻低电平
    CPOL1      //CPOL=1，空闲时刻高电平
} softspi_cpol_type;

//定义SPI收发器先发/收的位
typedef enum
{
    SOFTSPI_MSB, //MSB先发/收
    SOFTSPI_LSB  //LSB先发/收
} softspi_firstbit_type;

//SPI收发器类定义
struct softspi_class
{
    vro softspi_mode_type Mode;         //主/从模式
    vro softspi_cpha_type CPHA;         //CPHA参数
    vro softspi_cpol_type CPOL;         //CPOL参数
    vro uint8_t FrameLength;            //帧长度（bit），最小为1，最大为32
    status_flag CS_Control;             //是否允许CS控制，当CS_Control=Enable时，主模式下，SPI收发器自动控制CS脚的电平；
                                        //从模式下，SPI收发器会判断CS是否为低电平。当CS_Control=Disable时，主模式下，SPI
                                        //收发器不会改变CS脚的电平，从模式下，SPI收发器不会判断CS是否为低电平。该成员可
                                        //以运行时动态变换。如果允许CS控制，则当连续发送多个帧时，CS会一直保持低电平
    vro softspi_firstbit_type FirstBit; //先发/收的位，也就是MSB或LSB

    vro gpio SCLK; //时钟引脚
    vro gpio MOSI; //MOSI引脚
    vro gpio MISO; //MISO引脚
    vro gpio CS;   //CS引脚，如果CS_Control=Disable，该参数无效；如果CS_Control=Enable，该参数必须被正确填写

    vro uint8_t Buffer_Length; //该参数表明储存FrameLength长度的数据最少需要的字节数
                               //该参数只会是以下几个值：
                               //当FrameLength<=8，Buffer_Length=1
                               //当8<FrameLength<=16，Buffer_Length=2
                               //当16<FrameLength<=32，Buffer_Length=4

    volatile float ValidTime;   //在一个时钟周期内，有效电平所占的时间（us），该值加上InvalidTime等于时钟周期
    volatile float InvalidTime; //在一个时钟周期内，空闲电平所占的时间（us），该值加上ValidTime等于时钟周期

    /*
    * 数据交换函数
    * 功能：
    *       让SPI收发器产生一次主从机之间的数据交换
    * 参数：
    *       1.hd：softspi的类实例
    *       2.TxD：待发送数组的地址
    *       3.RxD：接收数组的地址
    *       4.len：要交换的帧数量
    * 说明：
    *       虽然TxD和RxD是void*类型，但是函数内部对TxD和RxD的读取/写入格式与Buffer_Length有关：
    *       如果Buffer_Length=1，那么TxD和RxD会被转换为uint8_t*
    *       如果Buffer_Length=2，那么TxD和RxD会被转换为uint16_t*
    *       如果Buffer_Length=4，那么TxD和RxD会被转换为uint32_t*
    *       这意味着一个帧的数据会被放入最小的能容下它的数据类型
	*		如果允许了CS控制，那么执行该函数期间，CS电平都会保持有效（仅限主模式）
	*		TxD和RxD任意一个都可以是NULL，如果TxD为NULL，则函数只接受不发送
	*		如果RxD为NULL，则函数只发送不接收，不允许TxD和RxD都为NULL
    */
    void (*Transmit_DataFlow)(softspi *hd, void *TxD, void *RxD, uint32_t len);
};

/*
* SoftSPI初始化
* 功能：
*		初始化softspi类实例
* 参数：
*		1.hd：softspi的类实例
*		2.mode：该SPI收发器将工作在主/从模式，对应hd->Mode
*		3.cpha：该SPI收发器的CPHA参数，对应hd->CPHA
*		4.cpol：该SPI收发器的CPOL参数，对应hd->CPOL
*		5.frmlen：帧长度，对应hd->FrameLength
*		6.fsb：该SPI收发器将会先发/收LSB还是MSB，对应hd->FirstBit
*		7.sclk：时钟引脚，对应hd->SCLK
*		8.mosi：MOSI引脚，对应hd->MOSI
*		9.miso：MISO引脚，对应hd->MISO
*		10.cs：CS引脚，对应hd->CS
*/
void SoftSPI_Prepare(softspi *hd, softspi_mode_type mode, softspi_cpha_type cpha, softspi_cpol_type cpol, uint8_t frmlen, softspi_firstbit_type fsb,
                     gpio sclk, gpio mosi, gpio miso, gpio cs);

#endif
