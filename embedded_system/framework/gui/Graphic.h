/*******************************************************************************
*|-----------------------------------图形库-----------------------------------|*
*|-------------------------------@大四十七高材生------------------------------|*
*|----------------------------------2021.1.22---------------------------------|*
*|基于显存操作的图形库，提供基本的图形绘制函数--------------------------------|*
********************************************************************************/
#ifndef _GRAPHIC_H_
#define _GRAPHIC_H_

#include "BMP.h"
#include "CharLib.h"
#include "Framework.h"

//矩形填充，在绘制矩形时，当边框厚度参数等于该值时，将会绘制填充的矩形
#define RECTANGLE_FILL 65535

//用来表示行数/列数不受限
#define INF  -1

//常用颜色
#define WHITE			0xffff  //白
#define BLACK			0x0000  //黑
#define TIANYI_BLUE		0x667f  //蓝
#define ALING_RED		0xe800  //红
#define YANHE_GREEN		0x7f9   //绿
#define LONGYA_GREEN	0x32c   //绿
#define QINGXIAN_YELLOW	0xffe0  //黄
#define MOKE_BLUE		0x41f   //蓝
#define PURE_BLUE		0x1f	//纯蓝
#define PURE_RED		0xf800	//纯红
#define DEEP_GRAY       0x3186  //深灰
#define GRAY            0x738e  //灰

//用来描述字符串的水平对齐方式
typedef enum
{
    Left_Align,     //左对齐
    Right_Align,    //右对齐
    HorMid          //居中
} horalign_type;

//用来描述字符串的垂直对齐方式
typedef enum
{
    Top_Align,      //上对齐
    Bottom_Align,   //下对齐
    VerMid          //居中
} veralign_type;

typedef struct canvas_class canvas;

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
*画布类：屏幕显示的基本单位，任何内容想要在屏幕上显示出来   *
*		 都需要依托一个画布，每一个画布类都对应了显存上的   *
*		 一块实际的内存，任何对画布的操作都是对这一块内存	*
*		 的操作，任何一块画布都可以拥有无数个子画布			*
*************************************************************/
struct canvas_class
{
	//是否显示
	volatile status_flag Display;
    
    vro status_flag Virtual;
    
    volatile status_flag WriteThrough;
	
    //画布区域
	area Area;
	
	//对应的显存地址
	uint16_t* Address;
};

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
    const charmold* Mold;
} font;

/************************************************************
*屏幕类：描述屏幕参数类型                                   *
*************************************************************/
typedef struct screen_class
{
    //屏宽
    uint16_t  Width;
    
    //屏高
    uint16_t  Height;
    
    //显存地址（仅在双显存模式下有效，如果选择单显存模式，该值为0）
    uint16_t* GRAM;
    
    //GRAM所对应的画布
    canvas    Canvas; 
} screen;

//包含了屏幕的信息
extern screen ScreenDevice;







/****************************************************************************
*功能：图形库缓存初始化，为缓存分配空间										
*参数：1.width：屏宽
*      2.height：屏高
*      3.gram：第二显存的地址（如果选择双显存模式）
*****************************************************************************/
void Graphic_Init(uint16_t width, uint16_t height, status_flag vrt, uint16_t* gram);

/****************************************************************************
*功能：屏幕刷新（双显存）					
*说明：将第二显存的数据刷新到屏幕显存
*****************************************************************************/
void Graphic_Screen_Refresh(void);

/****************************************************************************
*功能：屏幕刷新（双显存）					
*说明：尝试将第二显存的数据刷新到屏幕显存
*****************************************************************************/
void Graphic_Screen_Refresh_Try(void);

/****************************************************************************
*功能：创建画布																*
*参数：1.x：画布左上角横坐标												*
*	   2.y：画布左上角纵坐标												*
*	   3.width：宽度														*
*	   4.height：高度														*
*返回值：创建的画布															*
*****************************************************************************/
void Graphic_CreateCanvas(canvas* c ,uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, status_flag vrt);
//使用一个area结构来描述画布的区域
void Graphic_CreateCanvas2(canvas* c ,area a, status_flag vrt);

/****************************************************************************
*功能：删除画布																*
*参数：1.Canvas：画布地址													*
*说明：删除画布会首先删除该画布的所有子画布（包括子画布的子画布），然后删除	*
*	   画布本身，释放显存空间												*
*****************************************************************************/
void Graphic_Canvas_Delete(canvas* c);

void Graphic_Virtual_Set(canvas* c, status_flag state);

/****************************************************************************
*功能：刷新画布内容															*
*参数：1.Canvas：画布地址													*
*说明：调用此函数会立即将画布及其所有子画布的显存内容显示到屏幕上。而子画布	*
*	   的内容会覆盖父画布的内容。进行所有画布绘制函数后，只有调用此函数，	*
*	   内容才会更新的屏幕上。												*
*****************************************************************************/
void Graphic_RefreshCanvas(canvas* c);
void Graphic_RefreshCanvas_Try(canvas* c);



/****************************************************************************
*功能：将整块画布显示某种颜色												*
*参数：1.c：要绘制的画布												*
*	   2.color：颜色														*
*****************************************************************************/
void Graphic_Draw_Color(canvas* c, uint16_t color);

/*********************************************************************************
*功能：显示矩形（画布模式）													
*参数：1.c：要绘制的画布											
*	   2.x：矩形左上角相对画布的x坐标										
*	   3.y：矩形左上角相对画布的y坐标						
*	   4.r：矩形类，它定义了该矩形的样式						
*说明：该函数有三个衍生型，它们有的只是参数发生变化，有的则功能不同
*      1.Graphic_Canvas_DisplayRectangle2：将r替换为一个用，width（宽）
*        height（高），bcolor（背景颜色），thickness（边框厚度）和fcolor（边框颜色）
*        所描述的矩形区域。
*      2.Graphic_Canvas_DisplayRectangle3：x,y,width,height用来描述矩形区域，
*        thickness表示边框厚度，fcolor表示边框颜色。该方法不能显示带有背景
*        的矩形。
*      3.Graphic_Canvas_DisplayRectangle4：x,y,width,height用来描述矩形区域，
*        color代表背景颜色。该方法默认矩形边框厚度为0。
***********************************************************************************/
void Graphic_Draw_Rectangle(canvas* c, uint16_t re_x, uint16_t re_y, rectangle* r);
void Graphic_Draw_Rectangle2(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, uint16_t bcolor, uint16_t thickness, uint16_t fcolor);
void Graphic_Draw_Rectangle3(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, uint16_t thickness, uint16_t fcolor);
void Graphic_Draw_Rectangle4(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, uint16_t color);

/****************************************************************************
*功能：显示线段（画布模式）										
*参数：1.c：要绘制的画布											
*	   2.re_x1：线一端相对画布的横坐标											
*	   3.re_y1：线一端相对画布的纵坐标											
*	   4.re_x2：线另一端相对画布的横坐标										
*	   5.re_y2：线另一端相对画布的纵坐标										
*	   6.Line：线类，它定义了线的样式										
*说明：该函数有一个衍生型，它具有不同形式的参数
*      1.Graphic_Canvas_DisplayLine2：使用参数width（线宽），color（线颜色），
*        slen（实线长度），vlen（虚线长度）来代替l
*****************************************************************************/
void Graphic_Draw_Line(canvas* c, uint16_t re_x1, uint16_t re_y1, uint16_t re_x2, uint16_t re_y2, line* l);
void Graphic_Draw_Line2(canvas* c, uint16_t re_x1, uint16_t re_y1, uint16_t re_x2, uint16_t re_y2, uint8_t width, uint16_t color, uint8_t slen, uint8_t vlen);

/****************************************************************************
*功能：显示圆（画布模式）									
*参数：1.c：要绘制的画布											
*	   2.re_x：圆心相对画布的横坐标											
*	   3.re_y：圆心相对画布的纵坐标											
*	   4.r：半径															
*	   5.color：颜色														
*	   6.thickness：圆环厚度												
*****************************************************************************/
void Graphic_Draw_Circle(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t r, uint16_t color, uint8_t thickness);

/****************************************************************************
*功能：显示一副BMP格式图片（画布模式）													
*参数：1.c：要绘制的画布												
*	   2.re_x：图片左上角相对于画布的横坐标									
*	   3.re_y：图片左上角相对于画布的纵坐标																				
*	   4.p：图片的BMP结构体地址										
*说明：该函数有两个两个衍生型，它们在参数和功能上都有一定区别
*      1.Graphic_Canvas_DisplayBMP2：width和height指定显示区域的宽和高，而
*        图片会平铺在区域内，这意味着图像可能会被放缩。
*      2.Graphic_Canvas_DisplayBMP3：size表示图像的缩放比例，图像会按照这个
*        系数进行缩放。
*****************************************************************************/
void Graphic_Draw_BMP(canvas* c, uint16_t re_x, uint16_t re_y, const BMP* p);
void Graphic_Draw_BMP2(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, const BMP* p);
void Graphic_Draw_BMP3(canvas* c, uint16_t re_x, uint16_t re_y, float size, const BMP* p);


/****************************************************************************
*功能：显示一幅rBMP格式图片（画布模式）											
*参数：1.c：要绘制的画布												
*	   2.re_x：图片左上角相对于画布的横坐标									
*	   3.re_y：图片左上角相对于画布的纵坐标																				
*	   6.p：图片的rBMP结构体地址			
*说明：该函数有两个衍生型，它们在参数和功能上有一定区别
*      1.Graphic_Canvas_DisplayrBMP2：width和height指定显示区域的宽和高，而
*        图片会平铺在区域内，这意味着图像可能会被放缩。
*      2.Graphic_Canvas_DisplayrBMP3：size表示图像的缩放比例，图像会按照这个
*        系数进行缩放。
*****************************************************************************/
void Graphic_Draw_rBMP(canvas* c, uint16_t re_x, uint16_t re_y, const rBMP* p);
void Graphic_Draw_rBMP2(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, const rBMP* p);
void Graphic_Draw_rBMP3(canvas* c, uint16_t re_x, uint16_t re_y, float size, const rBMP* p);

/****************************************************************************
*功能：显示一幅pBMP格式的图片												
*参数：1.c：要绘制的画布											
*	   2.re_x：图片左上角相对于画布的横坐标									
*	   3.re_y：图片左上角相对于画布的纵坐标									
*	   6.p：图片的pBMP结构体地址										
*说明：该函数有两个衍生型，它们在参数和功能上有一定区别
*      1.Graphic_Canvas_DisplaypBMP2：width和height指定显示区域的宽和高，而
*        图片会平铺在区域内，这意味着图像可能会被放缩。
*      2.Graphic_Canvas_DisplaypBMP3：size表示图像的缩放比例，图像会按照这个
*        系数进行缩放。
*****************************************************************************/
void Graphic_Draw_pBMP(canvas* c, uint16_t re_x, uint16_t re_y, const pBMP* p);
void Graphic_Draw_pBMP2(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, const pBMP* p);
void Graphic_Draw_pBMP3(canvas* c, uint16_t re_x, uint16_t re_y, float size, const pBMP* p);

/****************************************************************************
*功能：显示一幅prBMP格式的图片												
*参数：1.c：要绘制的画布												
*	   2.re_x：图片左上角相对于画布的横坐标									
*	   3.re_y：图片左上角相对于画布的纵坐标									
*	   6.p：图片的prBMP结构体地址				
*说明：该函数有两个衍生型，它们在参数和功能上有一定区别
*      1.Graphic_Canvas_DisplayprBMP2：width和height指定显示区域的宽和高，而
*        图片会平铺在区域内，这意味着图像可能会被放缩。
*      2.Graphic_Canvas_DisplayprBMP3：size表示图像的缩放比例，图像会按照这个
*        系数进行缩放。
*****************************************************************************/
void Graphic_Draw_prBMP(canvas* c, uint16_t re_x, uint16_t re_y, const prBMP* p);
void Graphic_Draw_prBMP2(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, const prBMP* p);
void Graphic_Draw_prBMP3(canvas* c, uint16_t re_x, uint16_t re_y, float size, const prBMP* p);


void Graphic_Draw_BMP_All(canvas* c, uint16_t re_x, uint16_t re_y, const bmp* p);
void Graphic_Draw_BMP_All2(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, const bmp* p);
void Graphic_Draw_BMP_All3(canvas* c, uint16_t re_x, uint16_t re_y, float size, const bmp* p);



void Graphic_Draw_Char(canvas* c, uint16_t re_x, uint16_t re_y, uint8_t ch, uint16_t width, uint16_t height, uint16_t bcolor, uint16_t ccolor, const charmold* mold);
void Graphic_Draw_Char2(canvas* c, uint16_t re_x, uint16_t re_y, uint8_t ch, uint16_t bcolor, uint16_t ccolor, const charmold* mold);



void Graphic_Draw_String(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, uint8_t valign, uint8_t halign, font* f, char* str);
void Graphic_Draw_String2(canvas* c, uint16_t re_x, uint16_t re_y, short line, short row, uint8_t valign, uint8_t halign, font* f, char* str);
void Graphic_Draw_String3(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width ,uint16_t height, uint8_t valign, uint8_t halign, uint16_t bcolor, uint16_t ccolor, const charmold* mold, char* str);
void Graphic_Draw_String4(canvas* c, uint16_t re_x, uint16_t re_y, short line, short row, uint8_t valign, uint8_t halign, uint16_t bcolor, uint16_t ccolor, const charmold* mold, char* str);



void Graphic_Printf(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, uint8_t valign, uint8_t halign, font* f, char* fstr, ...);
void Graphic_Printf2(canvas* c, uint16_t re_x, uint16_t re_y, short line, short row, uint8_t valign, uint8_t halign, font* f, char* fstr, ...);
void Graphic_Printf3(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, uint8_t valign, uint8_t halign, uint16_t bcolor, uint16_t ccolor, const charmold* mold, char* fstr, ...);
void Graphic_Printf4(canvas* c, uint16_t re_x, uint16_t re_y, short line, short row, uint8_t valign, uint8_t halign, uint16_t bcolor, uint16_t ccolor, const charmold* mold, char* fstr, ...);



#endif
