#ifndef _GRAPHIC_DATASTRUCTURE_H_
#define _GRAPHIC_DATASTRUCTURE_H_

#include "Framework.h"
#include "LinkedList.h"
#include "Graphic_Config.h"
#include "CharLib.h"

typedef struct canvas_class canvas;
typedef struct screen_class screen;

//获取画布上某个像素对应的显存地址
#define CANVAS_GRAM_ADDR(CANVAS, X, Y) ((CANVAS)->Address + (X) + (Y) * (CANVAS)->Area.Width)

#define IS_CANVAS_CACHE_ENABLE(c) ((!c->Virtual) && (!c->WriteThrough))

/************************************************************
 *区域类：描述屏幕上的一块矩形区域							*
 *************************************************************/
typedef struct area_class
{
    //左上角X坐标
    uint16_t X;

    //左上角Y坐标
    uint16_t Y;

    //宽度
    uint16_t Width;

    //高度
    uint16_t Height;
} area;

/************************************************************
 *画布类：屏幕显示的基本单位，任何内容想要在屏幕上显示出来   *
 *		 都需要依托一个画布，每一个画布类都对应了显存上的   *
 *		 一块实际的内存，任何对画布的操作都是对这一块内存	*
 *		 的操作，任何一块画布都可以拥有无数个子画布			*
 *************************************************************/
struct canvas_class
{
    //父画布
    canvas *vro ParentCanvas;

    //对应的屏幕
    screen *vro Screen;

    //是否显示
    volatile status_flag Display;

    //自动分配显存
    vro status_flag Auto_Malloc;

    //是否为虚拟画布
    vro status_flag Virtual;

    //缓存是否透写
    vro status_flag WriteThrough;

    //画布区域
    area Area;

    //对应的显存地址
    uint16_t *vro Address;

    //子画布
    linkedlist ChildCanvas;
};

/************************************************************
 *屏幕类：描述屏幕参数类型                                   *
 *************************************************************/
struct screen_class
{
    //屏幕ID
    int8_t Screen_ID;

    //屏宽
    vro uint16_t Width;

    //屏高
    vro uint16_t Height;

    // GRAM所对应的画布
    canvas Canvas;

    ro void *ro BMPDecoder_Buffer;

    vro uint16_t BMPDecoder_Buffer_Size;

    ro void *ro Rasterization_Buffer;

    vro uint16_t Rasterization_Buffer_Size;

    ro void *ro TextProcess_Buffer;

    vro uint16_t TextProcess_Buffer_Size;

    ro void *ro TextPreprocess_Buffer;

    vro uint16_t TextPreprocess_Buffer_Size;

    void (*Draw_Pixel)(screen *sc, uint16_t x, uint16_t y, uint16_t color);
    void (*Draw_Area)(screen *sc, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
    void (*GRAM_Transfer)(screen *sc, uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint16_t *gram);
    void (*GRAM_Transfer_Try)(screen *sc, uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint16_t *gram);
};

//用来表示行数/列数不受限
#define INF -1

//矩形填充，在绘制矩形时，当边框厚度参数等于该值时，将会绘制填充的矩形
#define RECTANGLE_FILL 65535

/************************************************************
 *线类：描述线的特征。允许绘制虚线，绘制虚线时，会首先绘制	*
 *	   一小段实线，然后空缺一小段，再如此循环绘制直到到达	*
 *	   直到的位置。											*
 *************************************************************/
typedef struct line_class
{
    //线宽
    uint8_t Width;

    //颜色
    uint16_t Color;

    //实线部分的长度，当Vacancy_Length为0时，该值不起作用
    uint8_t Solid_Length;

    //空缺部分的长度，当该值为0时，Solid_Length不起作用
    uint8_t Vacancy_Length;
} line;

/************************************************************
 *矩形类：描述矩形的特征。允许绘制带边框的矩形，边框厚度等	*
 *		 于0时只会现实背景，边框厚度大于任意一边的二分之一	*
 *		 时只会显示边框。									*
 *************************************************************/
typedef struct rectangle_class
{
    //宽度
    uint16_t Width;

    //高度
    uint16_t Height;

    //背景颜色
    uint16_t Background_Color;

    //边框厚度
    uint16_t Frame_Thickness;

    //边框颜色
    uint16_t Frame_Color;

    //是否显示背景。如果等于0，则只会显示边框
    uint8_t Display_Background;
} rectangle;

/************************************************************
 *字体类：描述字体类型                                     	*
 *************************************************************/
typedef struct FontClass
{
    //背景颜色
    uint16_t BackColor;

    //字体颜色
    uint16_t CharColor;

    //字库地址
    const charmold *Mold;
} font;

//用来描述字符串的水平对齐方式
typedef enum
{
    Left_Align,  //左对齐
    Right_Align, //右对齐
    HorMid       //居中
} horalign_type;

//用来描述字符串的垂直对齐方式
typedef enum
{
    Top_Align,    //上对齐
    Bottom_Align, //下对齐
    VerMid        //居中
} veralign_type;

#endif
