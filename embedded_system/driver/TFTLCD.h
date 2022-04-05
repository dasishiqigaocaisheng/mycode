/***********************************************************TFTLCD触摸屏操作库***************************************************************************
																																						*
适用于STM32F407xxxx系列																																	*
模块信息：																																				*
			  1.800x480像素																																*
			  2.LCD部分16位并口通信，触摸屏部分IIC协议通信																								*
			  3.支持24位色，数据格式见说明书																											*
			  4.触摸屏的坐标范围在触摸屏初始化函数中（GT9147_Send_Cfg）已经设置，范围480*800，与TFTLCD实际物理坐标										*
				完全重合，左上角为（0，0）点，宽为x坐标，高为y坐标，即理论上有：0<=x<=480,0<=y<=800														*
			  5.执行完触摸屏初始化函数后，INT（中断信号引脚）已配置为下拉输入模式，但没有开启中断，需要配置外部中断的一些参数（如优先级）与中断函数内容	*
			  6.FSMC的地址建立与数据保留时间不宜过短，容易造成输出不稳定，出现图像不完整的现象															*
																																						*
引脚信息(默认状态）：																																	*
			  RS----A18=PD13																															*
			  RD----NOE=PD4																																*
			  CS----NE1=PD7																																*
			  WR----NWE=PD5																																*
			  D0----D0=PD14		D1----D1=PD15																											*
			  D2----D2=PD0		D3----D3=PD1																											*
			  D4----D4=PE7		D5----D5=PE8																											*
			  D6----D6=PE9		D7----D7=PE10																											*
			  D8----D8=PE11		D9----D9=PE12																											*
			  D10---D10=PE13	D11---D11=PE14																											*
			  D12---D12=PE15	D13---D13=PD8																											*
			  D14---D14=PD9		D15---D15=PD10																											*
			  以上引脚均为stm32中FSMC模块定义																											*
			  RST---PC0																																	*
			  BL----PC1																																	*
			  MOSI--PC2(touch)																															*
			  CLK---PC3(touch)																															*
			  PEN---PC4(touch)																															*
			  TCS---PC5(touch)																															*
																																						*
			  P1----PA0																																	*
			  P2----PA1																																	*
																																						*
*********************************************************************************************************************************************************/

#ifndef _TFTLCD_H_
#define _TFTLCD_H_
#include "sys.h"
#include "BMP.h"

#define TFTLCD_DEBUG

#define TFTLCD_ON 1
#define TFTLCD_OFF 2

// BANK1,区域1
//#define TFTLCD_ORDER (*((vu16 *)0x60000000)) //一个地址，对应着虚拟的外部储存器地址，此时A18为0
//#define TFTLCD_DATA (*((vu16 *)0x60100000))	 //同上，另一个地址，此时A18为1

#define TFTLCD_ORDER (*((vu16 *)0X6007FFFE)) //一个地址，对应着虚拟的外部储存器地址，此时A18为0
#define TFTLCD_DATA (*((vu16 *)0X60080000))	 //同上，另一个地址，此时A18为1

/********************以下8个是调整TFTLCD屏扫描方向与顺序的宏********************/
/*******************************************************************************
方向说明：
			------------------			↑
			|    水平方向    |			上
			|                |		←左    右→
			|                |			下
			|垂              |			↓
			|直              |
			|方              |
			|向              |
			|                |
			|                |
			|                |
			|                |
			------------------
			|	4.3' TFTLCD  |
			------------------
需要注意，屏幕上下左右的方向是绝对的（上图，此外，垂直与水平方向也是绝对的），
但是x轴y轴的位置是相对，这与扫描方向的选择有关————先扫描的方向是x轴正方向，
后扫描的方向是y轴正方向。例如，如果选择了L2R_U2D（左到右，上到下），
那么从左到右就是x正方向，上到下就是y正方向l；
如果选择了U2D_R2L（下到上，右到左），那么下到上就是x正方向，右到左就是y正方向。
在本文件中，默认方向为L2R_U2D
********************************************************************************/
#define L2R_U2D 0	  //左到右，上到下
#define L2R_D2U 0x80  //左到右，下到上
#define R2L_U2D 0x40  //右到左，上到下
#define R2L_D2U 0xc0  //右到左，下到上
#define U2D_L2R 0x20  //上到下，左到右
#define U2D_R2L 0x60  //上到下，右到左
#define D2U_L2R 0xa0  //下到上，左到右
#define D2U_R2L 0xe0  //下到上，右到左
#define VERTICAL 0x20 //垂直方向，Disply_Line函数专用宏
#define HORIZONTAL 0  //水平方向，Disply_Line函数专用宏

//常用颜色
#define WHITE 0xffff
#define BLACK 0x0000
#define TIANYI_BLUE 0x667f
#define ALING_RED 0xe800
#define YANHE_GREEN 0x7f9
#define LONGYA_GREEN 0x32c
#define QINGXIAN_YELLOW 0xffe0
#define MOKE_BLUE 0x41f
#define PURE_BLUE 0x1f	//纯蓝
#define PURE_RED 0xf800 //纯红

void TFTLCD_Switch(u8 ON_OFF); // TFTLCD屏背光开关

/*************************************
TFTLCD屏写命令函数（写寄存器函数）	 *
功能：向一个指定的寄存器写入一个值	 *
参数说明：1. CMD：指令（寄存器地址） *
		  2. PAR：参数（要写入的值） *
**************************************/
void TFTLCD_WriteCommand(u16 CMD, u16 PAR);

/********************************************************************
TFTLCD屏幕扫描方式设置函数											*
功能：设置LCD的扫描方式（包括扫描方向与扫描范围）					*
参数说明：1. Scan_Mode：扫描方向，具体参见TFTLCD_TOUCH.h中的宏定义	*
		  2.x_min：扫描x方向最小坐标								*
		  3.x_max：扫描x方向最大坐标								*
		  4.y_min：扫描y方向最小坐标								*
		  5.y_max：扫描y方向最大坐标								*
		  需要注意，这里的x与y的不是绝对的						    *
*********************************************************************/
void TFTLCD_PresentMode(u16 Scan_Mode, u16 x_min, u16 x_max, u16 y_min, u16 y_max);

/***************************************************************
TFTLCD屏初始化函数											   *
功能：1.使能要用到的GPIO								       *
		  2.使能并配置FSMC									   *
		  3.对TFTLCD屏进行初始化，主要为对寄存器写入一些初始值 *
****************************************************************/
void TFTLCD_Init(void);

/*********************************
刷屏函数						 *
其功能为：令全屏幕显示某个颜色	 *
参数说明：1. Color：要显示的颜色 *
**********************************/
void TFTLCD_DisplayColor(u16 Color);

/*********************************************************************************
实心矩形绘制函数																 *
其功能为：在屏幕的某个坐标处，以该点为左上角，显示一个特定颜色特点尺寸的实心矩形 *
参数说明：1. X：以屏幕左上角为零点，要显示处的x坐标								 *
		  2. Y：以屏幕左上角为零点，要显示处的y坐标								 *
		  3. Wide：矩形的宽度													 *
		  4. Hight：矩形的高度													 *
		  5. Color：矩形的颜色												   	 *
**********************************************************************************/
void TFTLCD_DisplaySquare(u16 X, u16 Y, u16 Wide, u16 Hight, u16 Color);

/******************************************************************************************
画线函数																				  *
其功能为：在屏幕某个坐标处，以一定的方向一定的颜色与特定的长度，显示一条宽度为1像素的直线 *
参数说明：1. X：以屏幕左上角为零点，要显示处的x坐标										  *
		  2. Y：以屏幕左上角为零点，要显示处的y坐标										  *
		  3. Lenth：线的长度															  *
		  4. Direction：画线的方向，参考TFTLCD_TOUCH.h中的宏定义						  *
		  5. Color：线的颜色															  *
*******************************************************************************************/
void TFTLCD_DisplayLine(u16 X1, u16 Y1, u16 X2, u8 Y2, u16 Color);

/**********************************************************************
BMP位图显示函数														  *
其功能为：在屏幕某坐标处，以该点为左上角显示一副含有bmp数据格式的图像 *
参数说明：1. X：以屏幕左上角为零点，要显示处的x坐标					  *
		  2. Y：以屏幕左上角为零点，要显示处的y坐标					  *
		  3. Picture：储存了要显示的图像信息的结构体				  *
***********************************************************************/
void TFTLCD_DisplayBMP(u16 X, u16 Y, BMP Picture);

/***********************************************************************
压缩格式图片显示函数												   *
其功能为：在屏幕某坐标处，以该点为左上角显示一副含有rbmp数据格式的图像 *
参数说明：1. X：以屏幕左上角为零点，要显示处的x坐标					   *
		  2. Y：以屏幕左上角为零点，要显示处的y坐标					   *
		  3. Picture：储存了要显示的图像信息的结构体				   *
************************************************************************/
void TFTLCD_DisplayrBMP(u16 X, u16 Y, rBMP Picture);

void TFTLCD_DisplaypBMP(u16 X, u16 Y, pBMP Picture);
void TFTLCD_DisplayprBMP(u16 X, u16 Y, prBMP Picture);

void TFTLCD_Display_Waveform(u16 X, u16 Y, u16 Canvas_Wide, u16 Canvas_Hight, u16 Data_Lenth, u8 Line_Wide, u16 Color, float *Data);

/*void TOUCH_Init(void);						//触摸屏初始化函数
void TOUCH_IIC_Start(void);					//发送IIC开始信号
void TOUCH_IIC_Stop(void);	  				//发送IIC停止信号
void TOUCH_IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 TOUCH_IIC_Read_Byte(unsigned char ack);	//IIC读取一个字节
u8 TOUCH_IIC_Wait_Ack(void); 				//IIC等待ACK信号
void TOUCH_IIC_Ack(void);					//IIC发送ACK信号
void TOUCH_IIC_NAck(void);					//IIC不发送ACK信号
void TOUCH_Read(u16 reg,u8 *buf,u8 len);	//读触摸屏寄存器的值
u8 TOUCH_Send_Cfg(u8 mode);					//向触摸屏发送配置参数
u8 TOUCH_Write(u16 reg,u8 *buf,u8 len);		//向触摸屏写参数

u8 TOUCH_Get_Points_Number(void);
u8 TOUCH_Get_Coordinates(u16 *DATA);*/

#endif
