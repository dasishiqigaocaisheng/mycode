/****************************************************************
*------------------------BMP图像信息结构体库--------------------*
*--------------------------@大四十七高材生----------------------*
*														  		*
*																*
*BMP.h包含了一些与bmp图像文件有关的数据结构						*
*下面是一些介绍，详细的解释应查看文件内对这些结构的注释			*
*		1.rBMP_Class：rBMP类									*
*		2.BMP_Class：BMP类										*
*		3.prBMP_Class：prBMP类									*
*		4.pBMP_Class：pBMP类									*
*****************************************************************/
#ifndef _BMP_H_
#define _BMP_H_

#include "stdint.h"

/************************************************************************************************************
*BMP图像信息结构体																						   	*
*Wide：图像宽度																							   	*
*Hight：图像高度																							*
*Colors：图像信息。其组成规则与bmp格式文件中数据区对图像信息的组成规则一致，即：一个元素代表一个像素的颜色，*
*				  排列顺序为从左到右，从下到上。														   	*
*************************************************************************************************************/
typedef struct BMP_Class
{
	uint16_t Width;
	uint16_t Height;
	uint16_t *Colors;
} BMP;

/************************************************************************************************************
*rBMP压缩图像信息结构体																					   	*
*Size：Colors中的元素个数																				   	*
*Wide：图像宽度																							   	*
*Hight：图像高度																							*
*Colors：图像信息。其组成规则为：将这些元素从1开始排序，则位于偶数位置的元素为一个16位十六进制数，代表颜色。*
*								每个十六进制数前有一个十进制数，其代表后面这个颜色重复的次数。			   	*
*************************************************************************************************************/
typedef struct rBMP_Class
{
	uint32_t Size;
	uint16_t Width;
	uint16_t Height;
	uint16_t *Colors;
} rBMP;

/****************************************************************************
*pBMP图像信息结构体															*
*只有颜色数量小于等于255的图像可以以该方式储存，在调色盘格式的结构体中，	*
*像素信息是该颜色在调色盘中的索引											*
*Width：图像宽度															*
*Height：图像高度															*
*Color_Palette：调色盘														*
*Colors：像素信息															*
*****************************************************************************/
typedef struct pBMP_Class
{
	uint16_t Width;
	uint16_t Height;
	uint16_t *Color_Palette;
	uint8_t *Colors;
} pBMP;

/****************************************************************************
*prBMP图像信息结构体														*
*只有颜色数量小于等于255的图像可以以该方式储存，在调色盘格式的结构体中，	*
*像素信息是该颜色在调色盘中的索引											*
*Size：Color_Blocks或Colors的元素个数										*
*Width：图像宽度															*
*Height：图像高度															*
*Color_Palette：调色盘														*
*Color_Blocks：像素重复次数													*
*Colors：像素信息															*
*****************************************************************************/
typedef struct prBMP_Class
{
	uint32_t Size;
	uint16_t Width;
	uint16_t Height;
	uint16_t *Color_Palette;
	uint16_t *Color_Blocks;
	uint8_t *Colors;
} prBMP;

typedef enum
{
    BMP_f,
    rBMP_f,
    pBMP_f,
    prBMP_f
} bmp_format;
    

typedef union
{
    BMP   bmp;
    rBMP  rbmp;
    pBMP  pbmp;
    prBMP prbmp;
} bmp_type;

typedef struct
{
    bmp_format Format;
    bmp_type   BMP_Data;
} bmp;

/************************************************************************
*功能：rBMP格式解码为BMP格式											*
*参数：1.In：待解码的rBMP结构体											*
*	   2.Out：输出的BMP结构体											*
*	   3.Start：起始行数												*
*	   4.Lines：要解码的行数											*
*************************************************************************/
uint8_t rBMP2BMP_Decode(const rBMP* In, BMP* Out, uint16_t Start, uint16_t Lines);

/************************************************************************
*功能：pBMP格式解码为BMP格式											*
*参数：1.In：待解码的pBMP结构体											*
*	   2.Out：输出的BMP结构体											*
*	   3.Start：起始行数												*
*	   4.Lines：要解码的行数											*
*************************************************************************/
uint8_t pBMP2BMP_Decode(const pBMP* In, BMP* Out, uint16_t Start, uint16_t Lines);

/************************************************************************
*功能：prBMP格式解码为BMP格式											*
*参数：1.In：待解码的prBMP结构体										*
*	   2.Out：输出的BMP结构体											*
*	   3.Start：起始行数												*
*	   4.Lines：要解码的行数											*
*************************************************************************/
uint8_t prBMP2BMP_Decode(const prBMP* In, BMP* Out, uint16_t Start, uint16_t Lines);

#endif
