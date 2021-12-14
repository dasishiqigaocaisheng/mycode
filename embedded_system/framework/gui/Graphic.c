#include "Graphic.h"
#include "Memory.h"
#include "MathHelper.h"
#include "CharLib.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "USART.h"

///////////////////////用户配置区///////////////////////
#include "RGBLCD.h"

//0：单显存
//1：双显存
#define GRAM_MODE                   1

//图形库专属缓存0所在的堆
#define GRAPHIC_BUFFER0_HEAP		HEAP1

//图形库专属缓存0大小，该值越大在显示较大的压缩格式的BMP格式图象时，显示速度越快
//建议该值大于2048
#define GRAPHIC_BUFFER0_SIZE	    4096

//图形库专属缓存1所在的堆
#define GRAPHIC_BUFFER1_HEAP        HEAP1

//图形库专属缓存1大小，此缓存区用来显示线/圆
//建议该值大于max(屏幕宽度,屏幕高度)*2
#define GRAPHIC_BUFFER1_SIZE	    2048

//printf函数所使用的暂存字符串的缓存大小
//实际上会开辟Print_Buffer0和Print_Buffer1两个缓存区
//每个的大小都是PRINT_BUFFER_SIZE
//其中Print_Buffer0会存在于GRAPHIC_BUFFER0_HEAP
//而Print_Buffer1会存在于GRAPHIC_BUFFER1_HEAP
#define PRINT_BUFFER_SIZE           256

//显存所在的堆空间，此空间用来存储画布开辟的存储区
#define GRAM_HEAP   	            HEAP2

//保护空间大小
//该值代表在每一块显存的首位都会有PROTECTED_ROOM*Canvas.Width*2长度空闲
//显存空间，该部分空间也被视为显存的一部分，虽然并不会向其中写图像信息。这么做是为
//了防止对显存写的时候超出了显存地址而错误地写入了相邻的画布显存内，造成图像损坏。
#define PROTECTED_ROOM	 1


/***********************************************************
*功能：将本地的显存数据刷新到屏幕上的一块矩形区域
*参数：1.x：区域左上角的x坐标
*      2.y：区域左上角的y坐标
*      3.width：宽度
*      4.height：高度
*      5.gram：显存地址
************************************************************/
void GRAM_Transfer(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t* gram)
{
    /*int i,j;
    uint16_t* gram_addr=gram;
    
    TFTLCD_PresentMode(U2D_R2L,x,x+width-1,y,y+height-1);
    TFTLCD_ORDER=0x2c00;
    for (i=0;i<height;i++)
    {
        for (j=0;j<width;j++)
            TFTLCD_DATA=gram_addr[j];
        gram_addr+=width;
    }*/
    RGBLCD_GRAM_Transfer(x,y,width,height,gram,(u16*)LAYER1_GRAM_ADDR);
}

/*****************************************************************
*功能：尝试将本地的显存数据刷新到屏幕上的一块矩形区域，此函数可以
*      看作GRAM_Transfer的非阻塞模式，如果屏幕刷新已经在进行，此
*      函数直接返回；如果屏幕没有在刷新，则执行显存刷新
*参数：1.x：区域左上角的x坐标
*      2.y：区域左上角的y坐标
*      3.width：宽度
*      4.height：高度
*      5.gram：显存地址
******************************************************************/
void GRAM_Transfer_Try(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t* gram)
{
    //GRAM_Transfer(x,y,width,height,gram);
    if (!DMA2D_Is_Running())
        RGBLCD_GRAM_Transfer_Begin(x,y,width,height,gram,(u16*)LAYER1_GRAM_ADDR);
}


/******************************************************
*功能：在屏幕上绘制一个像素点
*参数：1.x：像素的x坐标
*      2.y：像素的y坐标
*      3.color：颜色
*******************************************************/
void Pixel_Draw_Pri(uint16_t x, uint16_t y, uint16_t color)
{
    /*TFTLCD_PresentMode(U2D_R2L,x,x,y,y);
    TFTLCD_ORDER=0x2c00;
    TFTLCD_DATA=color;*/
    *(((uint16_t*)LAYER1_GRAM_ADDR)+y*ScreenDevice.Width+x)=color;
}

/******************************************************
*功能：在屏幕上填充一块矩形区域
*参数：1.x：区域左上角的x坐标
*      2.y：区域左上角的y坐标
*      3.width：宽度
*      4.height：高度
*      5.color：颜色
*******************************************************/
void Pixels_Fill_Pri(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color) 
{
    /*int i,j;
    
    TFTLCD_PresentMode(U2D_R2L,x,x+width-1,y,y+height-1);
    TFTLCD_ORDER=0x2c00;
    for (i=0;i<height;i++)
    {
        for (j=0;j<width;j++)
            TFTLCD_DATA=color;
    }*/
    int i,j;
    uint16_t* addr=(uint16_t*)LAYER1_GRAM_ADDR;
    
    for (i=0;i<height;i++)
    {
        for (j=0;j<width;j++)
            addr[j]=color;
        addr+=ScreenDevice.Width;
    }
}

void Memory_Transfer(uint16_t* src, uint16_t* tgt, uint16_t width, uint16_t height, uint16_t src_skip, uint16_t tgt_skip)
{
    uint16_t i,j;

    for (i=0;i<height;i++)
    {
        for (j=0;j<width;j++)
            tgt[j]=src[j];
        tgt+=tgt_skip;
        src+=src_skip;
    }
}

void Memory_Transfer_Try(uint16_t* src, uint16_t* tgt, uint16_t width, uint16_t height, uint16_t src_skip, uint16_t tgt_skip)
{
    Memory_Transfer(src,tgt,width,height,src_skip,tgt_skip);
}


/////////////////////////////////////////////////////////






//获取画布上某个像素对应的显存地址
#define CANVAS_GRAM_ADDR(CANVAS,X,Y) ((CANVAS)->Address+(X)+(Y)*(CANVAS)->Area.Width)

//用于printf函数字符串的缓存
char* Print_Buffer0;
char* Print_Buffer1;

//图形库专属缓存0
void* Graphic_Buffer0;

//图形库专属缓存1
void* Graphic_Buffer1;

//代表屏幕的基础画布
screen ScreenDevice;





#define Pixel_Draw_Sub(x,y,color)   *CANVAS_GRAM_ADDR(&ScreenDevice.Canvas,x,y)=color

void Pixels_Fill_Sub(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color) 
{
    uint16_t* gram=CANVAS_GRAM_ADDR(&ScreenDevice.Canvas,x,y);
    uint16_t i,j;
    
    for (i=0;i<height;i++)
    {
        for (j=0;j<width;j++)
            gram[j]=color;
        gram+=ScreenDevice.Width;
    }
}



/*显存分配*/
uint16_t* _GRAM_Malloc(uint16_t width, uint16_t height);

/*显存释放*/
void _GRAM_Free(canvas* Canvas);

uint16_t* _GRAM_Malloc(uint16_t width, uint16_t height)
{
	uint16_t* Addr;
	
	Addr=Malloc(GRAM_HEAP,(height+2*PROTECTED_ROOM)*width*2);
	Addr=Addr==NULL?NULL:Addr+width*PROTECTED_ROOM;
	return Addr;
}

void _GRAM_Free(canvas* Canvas)
{
	Free(GRAM_HEAP,Canvas->Address-Canvas->Area.Width);
}

























void Graphic_Screen_Refresh(void)
{
    if (!ScreenDevice.Canvas.Virtual)
        GRAM_Transfer(0,0,ScreenDevice.Width,ScreenDevice.Height,ScreenDevice.GRAM);
}

void Graphic_Screen_Refresh_Try(void)
{
    if (!ScreenDevice.Canvas.Virtual)
        GRAM_Transfer_Try(0,0,ScreenDevice.Width,ScreenDevice.Height,ScreenDevice.GRAM);
}

void Graphic_Screen_Scan(uint16_t color)
{
    if (ScreenDevice.Canvas.Virtual)
        Pixels_Fill_Pri(0,0,ScreenDevice.Width,ScreenDevice.Height,color);
    else
        Pixels_Fill_Sub(0,0,ScreenDevice.Width,ScreenDevice.Height,color);
}



void Graphic_Init(uint16_t width, uint16_t height, status_flag vrt, uint16_t* gram)
{
	ScreenDevice.Width=width;
	ScreenDevice.Height=height;
    ScreenDevice.GRAM=gram;
    ScreenDevice.Canvas.Address=gram;
    ScreenDevice.Canvas.Area.Height=height;
    ScreenDevice.Canvas.Area.Width=width;
    ScreenDevice.Canvas.Area.X=0;
    ScreenDevice.Canvas.Area.Y=0;
    ScreenDevice.Canvas.Display=1;
    
    _RO_WRITE(ScreenDevice.Canvas.Virtual,status_flag,vrt);
    
    ScreenDevice.Canvas.WriteThrough=vrt;
    
	Graphic_Buffer0=Malloc(GRAPHIC_BUFFER0_HEAP,GRAPHIC_BUFFER0_SIZE);
    Graphic_Buffer1=Malloc(GRAPHIC_BUFFER1_HEAP,GRAPHIC_BUFFER1_SIZE);
    Print_Buffer0=Malloc(GRAPHIC_BUFFER0_HEAP,PRINT_BUFFER_SIZE);
    Print_Buffer1=Malloc(GRAPHIC_BUFFER1_HEAP,PRINT_BUFFER_SIZE);
}

void Graphic_CreateCanvas(canvas* c ,uint16_t x, uint16_t y, uint16_t width, uint16_t height, status_flag vrt)
{
	c->Display=1;
	c->Area.X=x;
	c->Area.Y=y;
	c->Area.Width=width;
	c->Area.Height=height;
    _RO_WRITE(c->Virtual,status_flag,vrt);
    if (!vrt)
    {
        c->Address=_GRAM_Malloc(width,height);
        c->WriteThrough=Disable;
    }
    else
    {
        c->Address=NULL;
        c->WriteThrough=Enable;
    }
}

void Graphic_CreateCanvas2(canvas* c ,area a, status_flag vrt)
{
	c->Display=1;
	c->Area=a;
    _RO_WRITE(c->Virtual,status_flag,vrt);
    if (!vrt)
    {
        c->Address=_GRAM_Malloc(a.Width,a.Height);
        c->WriteThrough=Disable;
    }
    else
    {
        c->Address=NULL;
        c->WriteThrough=Enable;
    }
}

void Graphic_Canvas_Delete(canvas* c)
{
    if (!c->Virtual)
        Free(GRAM_HEAP,c->Address); //释放显存空间
}

void Graphic_Virtual_Set(canvas* c, status_flag state)
{
    if (state)
    {
        if (!c->Virtual)
            Graphic_Canvas_Delete(c);
    }
    else
    {
        if (c->Virtual)
            c->Address=_GRAM_Malloc(c->Area.Width,c->Area.Height);
    }
    _RO_WRITE(c->Virtual,status_flag,state);
}

void Graphic_RefreshCanvas(canvas* c)
{
    if (!c->WriteThrough)
    {
        if (!c->Virtual)
        {
            if (ScreenDevice.Canvas.WriteThrough)
                GRAM_Transfer(c->Area.X,c->Area.Y,c->Area.Width,c->Area.Height,c->Address);
            else
            {
                if (!ScreenDevice.Canvas.Virtual)
                {
                    uint16_t* sgram=CANVAS_GRAM_ADDR(&ScreenDevice.Canvas,c->Area.X,c->Area.Y);
                    Memory_Transfer(c->Address,sgram,c->Area.Width,c->Area.Height,c->Area.Width,ScreenDevice.Width);
                }
            }
        }
    }
    
}

void Graphic_RefreshCanvas_Try(canvas* c)
{
    if (!c->WriteThrough)
    {
        if (!c->Virtual)
        {
            if (ScreenDevice.Canvas.WriteThrough)
                GRAM_Transfer_Try(c->Area.X,c->Area.Y,c->Area.Width,c->Area.Height,c->Address);
            else
            {
                if (!ScreenDevice.Canvas.Virtual)
                {
                    uint16_t* sgram=CANVAS_GRAM_ADDR(&ScreenDevice.Canvas,c->Area.X,c->Area.Y);
                    Memory_Transfer_Try(c->Address,sgram,c->Area.Width,c->Area.Height,c->Area.Width,ScreenDevice.Width);
                }
            }
        }
    }
}





void Graphic_Canvas_FillColor(canvas* c, uint16_t color);

void Graphic_FillColor(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
{
    if (ScreenDevice.Canvas.Virtual)
        Pixels_Fill_Pri(x,y,width,height,color);
    else
        Pixels_Fill_Sub(x,y,width,height,color);
}

void Graphic_Canvas_FillColor(canvas* c, uint16_t color)
{
	uint16_t i,j;
	uint16_t *GRAM_Addr;
	
	GRAM_Addr=c->Address;
	for (i=0;i<c->Area.Height;i++)
	{
		for (j=0;j<c->Area.Width;j++)
			GRAM_Addr[j]=color;
        GRAM_Addr+=c->Area.Width;
	}
}

void Graphic_Draw_Color(canvas* c, uint16_t color)
{
    if (c->WriteThrough)
        Graphic_FillColor(c->Area.X,c->Area.Y,c->Area.Width,c->Area.Height,color);
    else
    {
        if (!c->Virtual)
            Graphic_Canvas_FillColor(c,color);
    }
}









void Graphic_Canvas_DisplayRectangle(canvas* c, uint16_t re_x, uint16_t re_y, rectangle* r);
void Graphic_Canvas_DisplayRectangle2(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, uint16_t bcolor, uint16_t thickness, uint16_t fcolor);
void Graphic_Canvas_DisplayRectangle3(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, uint16_t thickness, uint16_t fcolor);
void Graphic_Canvas_DisplayRectangle4(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, uint16_t color);
/*直接模式*/
void Graphic_DisplayRectangle(uint16_t ab_x, uint16_t ab_y, rectangle* r);
void Graphic_DisplayRectangle2(uint16_t ab_x, uint16_t ab_y, uint16_t width, uint16_t height, uint16_t bcolor, uint16_t thickness, uint16_t fcolor);
void Graphic_DisplayRectangle3(uint16_t ab_x, uint16_t ab_y, uint16_t width, uint16_t height, uint16_t thickness, uint16_t fcolor);
void Graphic_DisplayRectangle4(uint16_t ab_x, uint16_t ab_y, uint16_t width, uint16_t height, uint16_t color);

void _Graphic_DisplayRectangle(uint16_t width, uint16_t height, uint16_t color, uint16_t x, uint16_t y)
{
    if (ScreenDevice.Canvas.Virtual)
        Pixels_Fill_Pri(x,y,width,height,color);
    else
        Pixels_Fill_Sub(x,y,width,height,color);
}



void _Graphic_Canvas_DisplayRectangle(canvas* c, uint16_t width, uint16_t height, uint16_t Color, uint16_t x, uint16_t y)
{
	uint16_t i,j;
	uint16_t* GRAM_Addr;
	
	GRAM_Addr=CANVAS_GRAM_ADDR(c,x,y);
	for (i=0;i<height;i++)
	{
		for (j=0;j<width;j++)
		{
			*GRAM_Addr=Color;
			GRAM_Addr++;
		}
		GRAM_Addr+=c->Area.Width-width;
	}
}



void Graphic_Canvas_DisplayRectangle(canvas* Canvas, uint16_t x, uint16_t y, rectangle* r)
{
	if (x+r->Width>Canvas->Area.Width)
		r->Width=Canvas->Area.Width-x;
	if (y+r->Height>Canvas->Area.Height)
		r->Height=Canvas->Area.Height-y;
	
	if (r->Frame_Thickness!=0)
	{
		if ((r->Frame_Thickness>=r->Width/2)||(r->Frame_Thickness>=r->Height/2))
		{
			if (r->Display_Background)
				_Graphic_Canvas_DisplayRectangle(Canvas,r->Width,r->Height,r->Frame_Color,x,y);
			return;
		}
		else
		{
			_Graphic_Canvas_DisplayRectangle(Canvas,r->Width,r->Frame_Thickness,r->Frame_Color,x,y);
			_Graphic_Canvas_DisplayRectangle(Canvas,r->Width,r->Frame_Thickness,r->Frame_Color,
										x,y+r->Height-r->Frame_Thickness);
			_Graphic_Canvas_DisplayRectangle(Canvas,r->Frame_Thickness,r->Height-2*r->Frame_Thickness,
										r->Frame_Color,x,y+r->Frame_Thickness);
			_Graphic_Canvas_DisplayRectangle(Canvas,r->Frame_Thickness,r->Height-2*r->Frame_Thickness,
										r->Frame_Color,x+r->Width-r->Frame_Thickness,y+r->Frame_Thickness);
			if (r->Display_Background)
				_Graphic_Canvas_DisplayRectangle(Canvas,r->Width-2*r->Frame_Thickness,r->Height-2*r->Frame_Thickness,
											r->Background_Color,x+r->Frame_Thickness,y+r->Frame_Thickness);
		}
	}
	else
	{
		if (r->Display_Background)
			_Graphic_Canvas_DisplayRectangle(Canvas,r->Width,r->Height,r->Background_Color,x,y);
	}
}

void Graphic_Canvas_DisplayRectangle2(canvas* c, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t bcolor, uint16_t thickness, uint16_t fcolor)
{
	if (x+width>c->Area.Width)
		width=c->Area.Width-x;
	if (y+height>c->Area.Height)
		height=c->Area.Height-y;
	
	if (thickness==0)
		return;
	
	if ((thickness>=width/2)||(thickness>=height/2))
		_Graphic_Canvas_DisplayRectangle(c,width,height,fcolor,x,y);
	else
	{
		_Graphic_Canvas_DisplayRectangle(c,width,thickness,fcolor,x,y);
		_Graphic_Canvas_DisplayRectangle(c,width,thickness,fcolor,
									x,y+height-thickness);
		_Graphic_Canvas_DisplayRectangle(c,thickness,height-2*thickness,
									fcolor,x,y+thickness);
		_Graphic_Canvas_DisplayRectangle(c,thickness,height-2*thickness,
									fcolor,x+width-thickness,y+thickness);
        _Graphic_Canvas_DisplayRectangle(c,width-2*thickness,height-2*thickness,fcolor,x+thickness,y+thickness);
	}
}

void Graphic_Canvas_DisplayRectangle3(canvas* c, uint16_t X, uint16_t Y, uint16_t width, uint16_t height, uint16_t thickness, uint16_t fcolor)
{
    if (X+width>c->Area.Width)
		width=c->Area.Width-X;
	if (Y+height>c->Area.Height)
		height=c->Area.Height-Y;
	
	if (thickness==0)
		return;
    
    if ((thickness>=width/2)||(thickness>=height/2))
		_Graphic_Canvas_DisplayRectangle(c,width,height,fcolor,X,Y);
	else
	{
		_Graphic_Canvas_DisplayRectangle(c,width,thickness,fcolor,X,Y);
		_Graphic_Canvas_DisplayRectangle(c,width,thickness,fcolor,
									X,Y+height-thickness);
		_Graphic_Canvas_DisplayRectangle(c,thickness,height-2*thickness,
									fcolor,X,Y+thickness);
		_Graphic_Canvas_DisplayRectangle(c,thickness,height-2*thickness,
									fcolor,X+width-thickness,Y+thickness);
	}
}

void Graphic_Canvas_DisplayRectangle4(canvas* c, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
{
    _Graphic_Canvas_DisplayRectangle(c,width,height,color,x,y);
}



void Graphic_DisplayRectangle(uint16_t x, uint16_t y, rectangle* r)
{
	if (x+r->Width>ScreenDevice.Width)
		r->Width=ScreenDevice.Width-x;
	if (y+r->Height>ScreenDevice.Height)
		r->Height=ScreenDevice.Height-y;
	
	if (r->Frame_Thickness!=0)
	{
		if ((r->Frame_Thickness>=r->Width/2)||(r->Frame_Thickness>=r->Height/2))
		{
			if (r->Display_Background)
				_Graphic_DisplayRectangle(r->Width,r->Height,r->Frame_Color,x,y);
			return;
		}
		else
		{
			_Graphic_DisplayRectangle(r->Width,r->Frame_Thickness,r->Frame_Color,x,y);
			_Graphic_DisplayRectangle(r->Width,r->Frame_Thickness,r->Frame_Color,
										x,y+r->Height-r->Frame_Thickness);
			_Graphic_DisplayRectangle(r->Frame_Thickness,r->Height-2*r->Frame_Thickness,
										r->Frame_Color,x,y+r->Frame_Thickness);
			_Graphic_DisplayRectangle(r->Frame_Thickness,r->Height-2*r->Frame_Thickness,
										r->Frame_Color,x+r->Width-r->Frame_Thickness,y+r->Frame_Thickness);
			if (r->Display_Background)
				_Graphic_DisplayRectangle(r->Width-2*r->Frame_Thickness,r->Height-2*r->Frame_Thickness,
											r->Background_Color,x+r->Frame_Thickness,y+r->Frame_Thickness);
		}
	}
	else
	{
		if (r->Display_Background)
			_Graphic_DisplayRectangle(r->Width,r->Height,r->Background_Color,x,y);
	}
}

void Graphic_DisplayRectangle2(uint16_t X, uint16_t Y, uint16_t width, uint16_t height, uint16_t bcolor, uint16_t thichness, uint16_t fcolor)
{
	if (X+width>ScreenDevice.Width)
		width=ScreenDevice.Width-X;
	if (Y+height>ScreenDevice.Height)
		height=ScreenDevice.Height-Y;
	
	if (thichness==0)
		return;
	
	if ((thichness>=width/2)||(thichness>=height/2))
		_Graphic_DisplayRectangle(width,height,fcolor,X,Y);
	else
	{
		_Graphic_DisplayRectangle(width,thichness,fcolor,X,Y);
		_Graphic_DisplayRectangle(width,thichness,fcolor,X,Y+height-thichness);
		_Graphic_DisplayRectangle(thichness,height-2*thichness,fcolor,X,Y+thichness);
		_Graphic_DisplayRectangle(thichness,height-2*thichness,fcolor,X+width-thichness,Y+thichness);
        _Graphic_DisplayRectangle(width-2*thichness,height-2*thichness,bcolor,X+thichness,Y+thichness);
	}
}

void Graphic_DisplayRectangle3(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t thickness, uint16_t fcolor)
{
    if (x+width>ScreenDevice.Width)
		width=ScreenDevice.Width-x;
	if (y+height>ScreenDevice.Height)
		height=ScreenDevice.Height-y;
	
	if (thickness==0)
		return;
    
    if ((thickness>=width/2)||(thickness>=height/2))
		_Graphic_DisplayRectangle(width,height,fcolor,x,y);
	else
	{
		_Graphic_DisplayRectangle(width,thickness,fcolor,x,y);
		_Graphic_DisplayRectangle(width,thickness,fcolor,x,y+height-thickness);
		_Graphic_DisplayRectangle(thickness,height-2*thickness,fcolor,x,y+thickness);
		_Graphic_DisplayRectangle(thickness,height-2*thickness,fcolor,x+width-thickness,y+thickness);
	}
}

void Graphic_DisplayRectangle4(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
{
    _Graphic_DisplayRectangle(width,height,color,x,y);
}

void Graphic_Draw_Rectangle(canvas* c, uint16_t re_x, uint16_t re_y, rectangle* r)
{
    if (c->WriteThrough)
        Graphic_DisplayRectangle(c->Area.X+re_x,c->Area.Y+re_y,r);
    else
    {
        if (!c->Virtual)
            Graphic_Canvas_DisplayRectangle(c,re_x,re_y,r);
    }
}

void Graphic_Draw_Rectangle2(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, uint16_t bcolor, uint16_t thickness, uint16_t fcolor)
{
    if (c->WriteThrough)
        Graphic_DisplayRectangle2(c->Area.X+re_x,c->Area.Y+re_y,width,height,bcolor,thickness,fcolor);
    else
    {
        if (!c->Virtual)
            Graphic_Canvas_DisplayRectangle2(c,re_x,re_y,width,height,bcolor,thickness,fcolor);
    }
}
void Graphic_Draw_Rectangle3(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, uint16_t thickness, uint16_t fcolor)
{
    if (c->WriteThrough)
        Graphic_DisplayRectangle3(c->Area.X+re_x,c->Area.Y+re_y,width,height,thickness,fcolor);
    else
    {
        if (!c->Virtual)
            Graphic_Canvas_DisplayRectangle3(c,re_x,re_y,width,height,thickness,fcolor);
    }
}
void Graphic_Draw_Rectangle4(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, uint16_t color)
{
    if (c->WriteThrough)
        Graphic_DisplayRectangle4(c->Area.X+re_x,c->Area.Y+re_y,width,height,color);
    else
    {
        if (!c->Virtual)
            Graphic_Canvas_DisplayRectangle4(c,re_x,re_y,width,height,color);
    }
}




















void Graphic_Canvas_DisplayLine(canvas* c, uint16_t re_x1, uint16_t re_y1, uint16_t re_x2, uint16_t re_y2, line* l);
void Graphic_Canvas_DisplayLine2(canvas* c, uint16_t re_x1, uint16_t re_y1, uint16_t re_x2, uint16_t re_y2, uint8_t width, uint16_t color, uint8_t slen, uint8_t vlen);
/*直接模式*/
void Graphic_DisplayLine(uint16_t ab_x1, uint16_t ab_y1, uint16_t ab_x2, uint16_t ab_y2, line* l);
void Graphic_DisplayLine2(uint16_t ab_x1, uint16_t ab_y1, uint16_t ab_x2, uint16_t ab_y2, uint8_t width, uint16_t color, uint8_t slen, uint8_t vlen);


void Bresenham_Get_LineCoordinates(uint16_t* y_out, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    short dx=x2-x1,dy=y2-y1;
    short d=(dy<<1)-dx;
    uint16_t x=x1,y=y1;
    
    for (x=x1,y=y1;x!=x2;x++)
    {
        if (d<0)
            d+=dy<<1;
        else
        {
            d+=(dy-dx)<<1;
            y++;
        }
        y_out[x-x1]=y;
    }
}

void _Graphic_Canvas_DisplayLine(canvas* c, uint16_t color, uint16_t re_x1, uint16_t re_y1, uint16_t re_x2, uint16_t re_y2, uint8_t thickness)
{
    if (thickness==0)
		return;
	
	int i;
    uint8_t thickness_half=thickness/2;
    uint16_t* y_buf=Graphic_Buffer1;
	
	if (re_x1>=ScreenDevice.Width)
		re_x1=ScreenDevice.Width-1;
	if (re_x2>=ScreenDevice.Width)
		re_x2=ScreenDevice.Width-1;
	if (re_y1>=ScreenDevice.Height)
		re_y1=ScreenDevice.Height-1;
	if (re_y2>=ScreenDevice.Height)
		re_y2=ScreenDevice.Height-1;
	
    if (re_x2>=re_x1)
    {
        if (re_y2>re_y1)
        {
            if (re_y2-re_y1<=re_x2-re_x1)
            {
                Bresenham_Get_LineCoordinates(y_buf,0,0,re_x2-re_x1,re_y2-re_y1);
                for (i=re_x1;i!=re_x2;i++)
                    _Graphic_Canvas_DisplayRectangle(c,1,thickness,color,i,y_buf[i-re_x1]+re_y1-thickness_half);
            }
            else
            {
                Bresenham_Get_LineCoordinates(y_buf,0,0,re_y2-re_y1,re_x2-re_x1);
                for (i=re_y1;i!=re_y2;i++)
                    _Graphic_Canvas_DisplayRectangle(c,thickness,1,color,y_buf[i-re_y1]+re_x1-thickness_half,i);
            }
        }
        else
        {
            if (re_y1-re_y2<=re_x2-re_x1)
            {
                Bresenham_Get_LineCoordinates(y_buf,0,0,re_x2-re_x1,re_y1-re_y2);
                for (i=re_x1;i!=re_x2;i++)
                    _Graphic_Canvas_DisplayRectangle(c,1,thickness,color,i,re_y1-y_buf[i-re_x1]-thickness_half);
            }
            else
            {
                Bresenham_Get_LineCoordinates(y_buf,0,0,re_y1-re_y2,re_x2-re_x1);
                for (i=re_y1;i!=re_y2;i--)
                    _Graphic_Canvas_DisplayRectangle(c,thickness,1,color,y_buf[re_y1-i]+re_x1-thickness_half,i);
            }
        }
    }
    else
    {
        if (re_y2>re_y1)
        {
            if (re_y2-re_y1<=re_x1-re_x2)
                _Graphic_Canvas_DisplayLine(c,color,re_x2,re_y2,re_x1,re_y1,thickness);
            else
                _Graphic_Canvas_DisplayLine(c,color,re_x2,re_y2,re_x1,re_y1,thickness);
        }
        else
        {
            if (re_y1-re_y2<=re_x1-re_x2)
                _Graphic_Canvas_DisplayLine(c,color,re_x2,re_y2,re_x1,re_y1,thickness);
            else
                _Graphic_Canvas_DisplayLine(c,color,re_x2,re_y2,re_x1,re_y1,thickness);
        }
    }
}

void _Graphic_DisplayLine(uint16_t color, uint16_t ab_x1, uint16_t ab_y1, uint16_t ab_x2, uint16_t ab_y2, uint8_t thickness)
{
    if (ScreenDevice.Canvas.Virtual)
    {
        if (thickness==0)
            return;
        
        int i;
        uint8_t thickness_half=thickness/2;
        uint16_t* y_buf=Graphic_Buffer1;
        
        if (ab_x1>=ScreenDevice.Width)
            ab_x1=ScreenDevice.Width-1;
        if (ab_x2>=ScreenDevice.Width)
            ab_x2=ScreenDevice.Width-1;
        if (ab_y1>=ScreenDevice.Height)
            ab_y1=ScreenDevice.Height-1;
        if (ab_y2>=ScreenDevice.Height)
            ab_y2=ScreenDevice.Height-1;
        
        if (ab_x2>=ab_x1)
        {
            if (ab_y2>ab_y1)
            {
                if (ab_y2-ab_y1<=ab_x2-ab_x1)
                {
                    Bresenham_Get_LineCoordinates(y_buf,0,0,ab_x2-ab_x1,ab_y2-ab_y1);
                    for (i=ab_x1;i!=ab_x2;i++)
                        Pixels_Fill_Pri(i,y_buf[i-ab_x1]+ab_y1-thickness_half,1,thickness,color);
                }
                else
                {
                    Bresenham_Get_LineCoordinates(y_buf,0,0,ab_y2-ab_y1,ab_x2-ab_x1);
                    for (i=ab_y1;i!=ab_y2;i++)
                        Pixels_Fill_Pri(y_buf[i-ab_y1]+ab_x1-thickness_half,i,thickness,1,color);
                }
            }
            else
            {
                if (ab_y1-ab_y2<=ab_x2-ab_x1)
                {
                    Bresenham_Get_LineCoordinates(y_buf,0,0,ab_x2-ab_x1,ab_y1-ab_y2);
                    for (i=ab_x1;i!=ab_x2;i++)
                        Pixels_Fill_Pri(i,ab_y1-y_buf[i-ab_x1]-thickness_half,1,thickness,color);
                }
                else
                {
                    Bresenham_Get_LineCoordinates(y_buf,0,0,ab_y1-ab_y2,ab_x2-ab_x1);
                    for (i=ab_y1;i!=ab_y2;i--)
                        Pixels_Fill_Pri(y_buf[ab_y1-i]+ab_x1-thickness_half,i,thickness,1,color);
                }
            }
        }
        else
        {
            if (ab_y2>ab_y1)
            {
                if (ab_y2-ab_y1<=ab_x1-ab_x2)
                    _Graphic_DisplayLine(color,ab_x2,ab_y2,ab_x1,ab_y1,thickness);
                else
                    _Graphic_DisplayLine(color,ab_x2,ab_y2,ab_x1,ab_y1,thickness);
            }
            else
            {
                if (ab_y1-ab_y2<=ab_x1-ab_x2)
                    _Graphic_DisplayLine(color,ab_x2,ab_y2,ab_x1,ab_y1,thickness);
                else
                    _Graphic_DisplayLine(color,ab_x2,ab_y2,ab_x1,ab_y1,thickness);
            }
        }
    }
    else
    {
        if (thickness==0)
            return;
        
        int i;
        uint8_t thickness_half=thickness/2;
        uint16_t* y_buf=Graphic_Buffer1;
        
        if (ab_x1>=ScreenDevice.Width)
            ab_x1=ScreenDevice.Width-1;
        if (ab_x2>=ScreenDevice.Width)
            ab_x2=ScreenDevice.Width-1;
        if (ab_y1>=ScreenDevice.Height)
            ab_y1=ScreenDevice.Height-1;
        if (ab_y2>=ScreenDevice.Height)
            ab_y2=ScreenDevice.Height-1;
        
        if (ab_x2>=ab_x1)
        {
            if (ab_y2>ab_y1)
            {
                if (ab_y2-ab_y1<=ab_x2-ab_x1)
                {
                    Bresenham_Get_LineCoordinates(y_buf,0,0,ab_x2-ab_x1,ab_y2-ab_y1);
                    for (i=ab_x1;i!=ab_x2;i++)
                        Pixels_Fill_Sub(i,y_buf[i-ab_x1]+ab_y1-thickness_half,1,thickness,color);
                }
                else
                {
                    Bresenham_Get_LineCoordinates(y_buf,0,0,ab_y2-ab_y1,ab_x2-ab_x1);
                    for (i=ab_y1;i!=ab_y2;i++)
                        Pixels_Fill_Sub(y_buf[i-ab_y1]+ab_x1-thickness_half,i,thickness,1,color);
                }
            }
            else
            {
                if (ab_y1-ab_y2<=ab_x2-ab_x1)
                {
                    Bresenham_Get_LineCoordinates(y_buf,0,0,ab_x2-ab_x1,ab_y1-ab_y2);
                    for (i=ab_x1;i!=ab_x2;i++)
                        Pixels_Fill_Sub(i,ab_y1-y_buf[i-ab_x1]-thickness_half,1,thickness,color);
                }
                else
                {
                    Bresenham_Get_LineCoordinates(y_buf,0,0,ab_y1-ab_y2,ab_x2-ab_x1);
                    for (i=ab_y1;i!=ab_y2;i--)
                        Pixels_Fill_Sub(y_buf[ab_y1-i]+ab_x1-thickness_half,i,thickness,1,color);
                }
            }
        }
        else
        {
            if (ab_y2>ab_y1)
            {
                if (ab_y2-ab_y1<=ab_x1-ab_x2)
                    _Graphic_DisplayLine(color,ab_x2,ab_y2,ab_x1,ab_y1,thickness);
                else
                    _Graphic_DisplayLine(color,ab_x2,ab_y2,ab_x1,ab_y1,thickness);
            }
            else
            {
                if (ab_y1-ab_y2<=ab_x1-ab_x2)
                    _Graphic_DisplayLine(color,ab_x2,ab_y2,ab_x1,ab_y1,thickness);
                else
                    _Graphic_DisplayLine(color,ab_x2,ab_y2,ab_x1,ab_y1,thickness);
            }
        }
    }
}

void Graphic_Canvas_DisplayLine(canvas* c, uint16_t re_x1, uint16_t re_y1, uint16_t re_x2, uint16_t re_y2, line* l)
{
	uint16_t i,j;
	float Div_X1,Div_Y1,Div_X2,Div_Y2;
	float Degree,x,y;
	
	if ((re_x2==re_x1)&&(re_y2==re_y1))
		return;
	
	if (l->Vacancy_Length==0)
	{
		_Graphic_Canvas_DisplayLine(c,l->Color,re_x1,re_y1,re_x2,re_y2,l->Width);
		return;
	}
	
	if (re_x2-re_x1==0)
	{
		Div_X1=0;
		Div_X2=0;
		Div_Y1=l->Solid_Length;
		Div_Y2=l->Vacancy_Length;
	}
	else
	{
		Degree=atan2f(re_y2-re_y1,re_x2-re_x1);
		Div_X1=l->Solid_Length*cosf(Degree);
		Div_Y1=l->Solid_Length*sinf(Degree);
		Div_X2=l->Vacancy_Length*cosf(Degree);
		Div_Y2=l->Vacancy_Length*sinf(Degree);
	}
	
	if (re_x2-re_x1!=0)
		i=MATHHELPER_ABS(re_x2-re_x1)/MATHHELPER_ABS(Div_X1+Div_X2);
	else 
		i=MATHHELPER_ABS(re_y2-re_y1)/MATHHELPER_ABS(Div_Y1+Div_Y2);
	for (j=0,x=re_x1,y=re_y1;j<i;j++)
	{
		_Graphic_Canvas_DisplayLine(c,l->Color,x,y,x+Div_X1,y+Div_Y1,l->Width);
		x+=Div_X1+Div_X2;
		y+=Div_Y1+Div_Y2;
	}
	
	if (MATHHELPER_ABS(re_x2-x)>MATHHELPER_ABS(Div_X1))
		_Graphic_Canvas_DisplayLine(c,l->Color,x,y,x+Div_X1,y+Div_Y1,l->Width);
	else if (MATHHELPER_ABS(re_x2-x)<MATHHELPER_ABS(Div_X1))
		_Graphic_Canvas_DisplayLine(c,l->Color,x,y,re_x2,re_y2,l->Width);
	else if (MATHHELPER_ABS(re_y2-y)>MATHHELPER_ABS(Div_Y1))
		_Graphic_Canvas_DisplayLine(c,l->Color,x,y,x+Div_X1,y+Div_Y1,l->Width);
	else if (MATHHELPER_ABS(re_y2-y)<MATHHELPER_ABS(Div_Y1))
		_Graphic_Canvas_DisplayLine(c,l->Color,x,y,re_x2,re_y2,l->Width);
}

void Graphic_Canvas_DisplayLine2(canvas* c, uint16_t re_x1, uint16_t re_y1, uint16_t re_x2, uint16_t re_y2, uint8_t width, uint16_t color, uint8_t slen, uint8_t vlen)
{
    line l={width,color,slen,vlen};
    Graphic_Canvas_DisplayLine(c,re_x1,re_y1,re_x2,re_y2,&l);
}

void Graphic_DisplayLine(uint16_t ab_x1, uint16_t ab_y1, uint16_t ab_x2, uint16_t ab_y2, line* l)
{
	uint16_t i,j;
	float Div_X1,Div_Y1,Div_X2,Div_Y2;
	float Degree,x,y;
	
	if ((ab_x2==ab_x1)&&(ab_y2==ab_y1))
		return;
	
	if (l->Vacancy_Length==0)
	{
		_Graphic_DisplayLine(l->Color,ab_x1,ab_y1,ab_x2,ab_y2,l->Width);
		return;
	}
	
	if (ab_x2==ab_x1)
	{
		Div_X1=0;
		Div_X2=0;
        if (ab_y1>ab_y2)
        {
            Div_Y1=-l->Solid_Length;
            Div_Y2=-l->Vacancy_Length;
        }
        else
        {
            Div_Y1=l->Solid_Length;
            Div_Y2=l->Vacancy_Length;
        }
	}
	else if (ab_x2>ab_x1)
	{
		Degree=atan2f(ab_y2-ab_y1,ab_x2-ab_x1);
		Div_X1=l->Solid_Length*cosf(Degree);
		Div_Y1=l->Solid_Length*sinf(Degree);
		Div_X2=l->Vacancy_Length*cosf(Degree);
		Div_Y2=l->Vacancy_Length*sinf(Degree);
	}
    else
    {
        Graphic_DisplayLine(ab_x2,ab_y2,ab_x1,ab_y1,l);
        return;
    }
	
	if (ab_x2!=ab_x1)
		i=MATHHELPER_ABS(ab_x2-ab_x1)/MATHHELPER_ABS(Div_X1+Div_X2);
	else 
		i=MATHHELPER_ABS(ab_y2-ab_y1)/MATHHELPER_ABS(Div_Y1+Div_Y2);
	for (j=0,x=ab_x1,y=ab_y1;j<i;j++)
	{
		_Graphic_DisplayLine(l->Color,x,y,x+Div_X1,y+Div_Y1,l->Width);
		x+=Div_X1+Div_X2;
		y+=Div_Y1+Div_Y2;
	}
	
	if (MATHHELPER_ABS(ab_x2-x)>MATHHELPER_ABS(Div_X1))
		_Graphic_DisplayLine(l->Color,x,y,x+Div_X1,y+Div_Y1,l->Width);
	else if (MATHHELPER_ABS(ab_x2-x)<MATHHELPER_ABS(Div_X1))
		_Graphic_DisplayLine(l->Color,x,y,ab_x2,ab_y2,l->Width);
	else if (MATHHELPER_ABS(ab_y2-y)>MATHHELPER_ABS(Div_Y1))
		_Graphic_DisplayLine(l->Color,x,y,x+Div_X1,y+Div_Y1,l->Width);
	else if (MATHHELPER_ABS(ab_y2-y)<MATHHELPER_ABS(Div_Y1))
		_Graphic_DisplayLine(l->Color,x,y,ab_x2,ab_y2,l->Width);
}

void Graphic_DisplayLine2(uint16_t ab_x1, uint16_t ab_y1, uint16_t ab_x2, uint16_t ab_y2, uint8_t width, uint16_t color, uint8_t slen, uint8_t vlen)
{
    line l={width,color,slen,vlen};
    Graphic_DisplayLine(ab_x1,ab_y1,ab_x2,ab_y2,&l);
}

void Graphic_Draw_Line(canvas* c, uint16_t re_x1, uint16_t re_y1, uint16_t re_x2, uint16_t re_y2, line* l)
{
    if (c->WriteThrough)
        Graphic_DisplayLine(c->Area.X+re_x1,c->Area.Y+re_y1,c->Area.X+re_x2,c->Area.Y+re_y2,l);
    else
    {
        if (!c->Virtual)
            Graphic_Canvas_DisplayLine(c,re_x1,re_y1,re_x2,re_y2,l);
    }
}

void Graphic_Draw_Line2(canvas* c, uint16_t re_x1, uint16_t re_y1, uint16_t re_x2, uint16_t re_y2, uint8_t width, uint16_t color, uint8_t slen, uint8_t vlen)
{
    if (c->WriteThrough)
        Graphic_DisplayLine2(c->Area.X+re_x1,c->Area.Y+re_y1,c->Area.X+re_x2,c->Area.Y+re_y2,width,color,slen,vlen);
    else
    {
        if (!c->Virtual)
            Graphic_Canvas_DisplayLine2(c,re_x1,re_y1,re_x2,re_y2,width,color,slen,vlen);
    }
}



























void Graphic_Canvas_DisplayCircle(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t r, uint16_t color, uint8_t thickness);
/*直接模式*/
void Graphic_DisplayCircle(uint16_t ab_x, uint16_t ab_y, uint16_t r, uint16_t color, uint8_t thickness);

uint16_t Bresenham_Get_CircleCoordinates(uint16_t* y_out, uint16_t r)
{
    short d=3-(r<<1);
    uint16_t x,y,i;
    
    for (x=0,y=r;x<=y;x++)
    {
        y_out[x]=y;
        if (d<0)
            d+=(x<<2)+6;
        else
        {
            d+=((x-y)<<2)+10;
            y--;
        }
    }
    return x;
}

void Graphic_Canvas_DisplayCircle(canvas* c, uint16_t x, uint16_t y, uint16_t r, uint16_t color, uint8_t thickness)
{
	if (thickness==0)
		return;
	
	short d1,d2;
	u16 _y,x1,x2,i;
	int save1,save2;
	u16 *GRAM_Addr1,*GRAM_Addr2,*GRAM_Addr3,*GRAM_Addr4;
	
	GRAM_Addr1=CANVAS_GRAM_ADDR(c,x,y);
	GRAM_Addr2=GRAM_Addr1;
	GRAM_Addr3=GRAM_Addr1;
	GRAM_Addr4=GRAM_Addr1;
	x1=r-thickness+1;
	x2=r;
	d1=3-(x1<<1);
	d2=3-(x2<<1);
	for (_y=0;_y<=x1;_y++)
	{
		if (d1<0)
		{
			*(GRAM_Addr1+x1)=color;
			*(GRAM_Addr1-x1)=color;
			*(GRAM_Addr2+x1)=color;
			*(GRAM_Addr2-x1)=color;
			save1=x1*c->Area.Width;
			*(GRAM_Addr3+save1)=color;
			*(GRAM_Addr3-save1)=color;
			*(GRAM_Addr4+save1)=color;
			*(GRAM_Addr4-save1)=color;
			d1=d1+(_y<<2)+6;
		}
		else
		{
			*(GRAM_Addr1+x1-1)=color;
			*(GRAM_Addr1-x1+1)=color;
			*(GRAM_Addr2+x1-1)=color;
			*(GRAM_Addr2-x1+1)=color;
			save1=(x1-1)*c->Area.Width;
			*(GRAM_Addr3+save1)=color;
			*(GRAM_Addr3-save1)=color;
			*(GRAM_Addr4+save1)=color;
			*(GRAM_Addr4-save1)=color;
			d1=d1+((_y-x1)<<2)+10;
			x1--;
		}
		if (d2<0)
		{
			for (i=0;i<x2-x1;i++)
			{
				*(GRAM_Addr1+x2-i)=color;
				*(GRAM_Addr1-x2+i)=color;
				*(GRAM_Addr2+x2-i)=color;
				*(GRAM_Addr2-x2+i)=color;
				save1=(x2-i)*c->Area.Width;
				*(GRAM_Addr3+save1)=color;
				*(GRAM_Addr3-save1)=color;
				*(GRAM_Addr4+save1)=color;
				*(GRAM_Addr4-save1)=color;
			}
			save2=x2-x1;
			d2=d2+(_y<<2)+6;
		}
		else
		{
			for (i=0;i<x2-x1;i++)
			{
				*(GRAM_Addr1+x2-1-i)=color;
				*(GRAM_Addr1-x2+1+i)=color;
				*(GRAM_Addr2+x2-1-i)=color;
				*(GRAM_Addr2-x2+1+i)=color;
				save1=(x2-1-i)*c->Area.Width;
				*(GRAM_Addr3+save1)=color;
				*(GRAM_Addr3-save1)=color;
				*(GRAM_Addr4+save1)=color;
				*(GRAM_Addr4-save1)=color;
			}
			save2=x2-x1;
			d2=d2+((_y-x2)<<2)+10;
			x2--;
		}
		GRAM_Addr1+=c->Area.Width;
		GRAM_Addr2-=c->Area.Width;
		GRAM_Addr3++;
		GRAM_Addr4--;
	}
	
	for (;_y<=x2;_y++)
	{
		if (d2<0)
		{
			for (i=0;i<save2;i++)
			{
				*(GRAM_Addr1+x2-i)=color;
				*(GRAM_Addr1-x2+i)=color;
				*(GRAM_Addr2+x2-i)=color;
				*(GRAM_Addr2-x2+i)=color;
				save1=(x2-i)*c->Area.Width;
				*(GRAM_Addr3+save1)=color;
				*(GRAM_Addr3-save1)=color;
				*(GRAM_Addr4+save1)=color;
				*(GRAM_Addr4-save1)=color;
			}
			d2=d2+(_y<<2)+6;
		}
		else
		{
			for (i=0;i<save2;i++)
			{
				*(GRAM_Addr1+x2-1-i)=color;
				*(GRAM_Addr1-x2+1+i)=color;
				*(GRAM_Addr2+x2-1-i)=color;
				*(GRAM_Addr2-x2+1+i)=color;
				save1=(x2-1-i)*c->Area.Width;
				*(GRAM_Addr3+save1)=color;
				*(GRAM_Addr3-save1)=color;
				*(GRAM_Addr4+save1)=color;
				*(GRAM_Addr4-save1)=color;
			}
			d2=d2+((_y-x2)<<2)+10;
			x2--;
			save2--;
		}
		GRAM_Addr1+=c->Area.Width;
		GRAM_Addr2-=c->Area.Width;
		GRAM_Addr3++;
		GRAM_Addr4--;
	}
}

void Graphic_DisplayCircle(uint16_t x, uint16_t y, uint16_t r, uint16_t color, uint8_t thickness)
{
    if (ScreenDevice.Canvas.Virtual)
    {
        if (thickness==0)
            return;
        
        int i,j;
        uint16_t *datbuf1,*datbuf2;
        uint16_t len1,len2,save;
        
        datbuf1=Graphic_Buffer1;
        datbuf2=&Graphic_Buffer1[GRAPHIC_BUFFER1_SIZE/4];
        
        len1=Bresenham_Get_CircleCoordinates(datbuf1,r);
        len2=Bresenham_Get_CircleCoordinates(datbuf2,r+thickness);
        
        if (thickness==1)
        {
            for (i=0;i<len1;i++)
            {
                Pixel_Draw_Pri(x+i,y-datbuf1[i],color);    //45~90
                Pixel_Draw_Pri(x+datbuf1[i],y-i,color);    //0~45
                Pixel_Draw_Pri(x+i,y+datbuf1[i],color);    //-45~-90
                Pixel_Draw_Pri(x+datbuf1[i],y+i,color);    //0~-45
                
                Pixel_Draw_Pri(x-i,y-datbuf1[i],color);    //90~135
                Pixel_Draw_Pri(x-datbuf1[i],y-i,color);    //135~180
                Pixel_Draw_Pri(x-i,y+datbuf1[i],color);    //-90~-135
                Pixel_Draw_Pri(x-datbuf1[i],y+i,color);    //-135~-180
            }
        }
        else
        {
            for (i=0;i<len1;i++)
            {
                Pixels_Fill_Pri(x+i,y-datbuf2[i],1,datbuf2[i]-datbuf1[i],color);    //45~90
                Pixels_Fill_Pri(x+datbuf1[i],y-i,datbuf2[i]-datbuf1[i],1,color);    //0~45
                Pixels_Fill_Pri(x+i,y+datbuf1[i],1,datbuf2[i]-datbuf1[i],color);    //-45~-90
                Pixels_Fill_Pri(x+datbuf1[i],y+i,datbuf2[i]-datbuf1[i],1,color);    //0~-45
                
                Pixels_Fill_Pri(x-i,y-datbuf2[i],1,datbuf2[i]-datbuf1[i],color);    //90~135
                Pixels_Fill_Pri(x-datbuf2[i],y-i,datbuf2[i]-datbuf1[i],1,color);    //135~180
                Pixels_Fill_Pri(x-i,y+datbuf1[i],1,datbuf2[i]-datbuf1[i],color);    //-90~-135
                Pixels_Fill_Pri(x-datbuf2[i],y+i,datbuf2[i]-datbuf1[i],1,color);    //-135~-180
            }
            
            for (;i<len2;i++)
            {
                Pixels_Fill_Pri(x+i,y-datbuf2[i],1,datbuf2[i]-len1,color);          //45~90
                Pixels_Fill_Pri(x+len1,y-i,datbuf2[i]-len1,1,color);                //0~45
                Pixels_Fill_Pri(x+i,y+len1,1,datbuf2[i]-len1,color);                //-45~-90
                Pixels_Fill_Pri(x+len1,y+i,datbuf2[i]-len1,1,color);                //0~-45
                
                Pixels_Fill_Pri(x-i,y-datbuf2[i],1,datbuf2[i]-len1,color);          //90~135
                Pixels_Fill_Pri(x-datbuf2[i],y-i,datbuf2[i]-len1,1,color);          //135~180
                Pixels_Fill_Pri(x-i,y+len1,1,datbuf2[i]-len1,color);                //-90~-135
                Pixels_Fill_Pri(x-datbuf2[i],y+i,datbuf2[i]-len1,1,color);                //-135~-180
            }
        }
    }
    else
    {
        if (thickness==0)
            return;
        
        int i,j;
        uint16_t *datbuf1,*datbuf2;
        uint16_t len1,len2,save;
        
        datbuf1=Graphic_Buffer1;
        datbuf2=&Graphic_Buffer1[GRAPHIC_BUFFER1_SIZE/4];
        
        len1=Bresenham_Get_CircleCoordinates(datbuf1,r);
        len2=Bresenham_Get_CircleCoordinates(datbuf2,r+thickness);
        
        if (thickness==1)
        {
            for (i=0;i<len1;i++)
            {
                Pixel_Draw_Sub(x+i,y-datbuf1[i],color);    //45~90
                Pixel_Draw_Sub(x+datbuf1[i],y-i,color);    //0~45
                Pixel_Draw_Sub(x+i,y+datbuf1[i],color);    //-45~-90
                Pixel_Draw_Sub(x+datbuf1[i],y+i,color);    //0~-45
                
                Pixel_Draw_Sub(x-i,y-datbuf1[i],color);    //90~135
                Pixel_Draw_Sub(x-datbuf1[i],y-i,color);    //135~180
                Pixel_Draw_Sub(x-i,y+datbuf1[i],color);    //-90~-135
                Pixel_Draw_Sub(x-datbuf1[i],y+i,color);    //-135~-180
            }
        }
        else
        {
            for (i=0;i<len1;i++)
            {
                Pixels_Fill_Sub(x+i,y-datbuf2[i],1,datbuf2[i]-datbuf1[i],color);    //45~90
                Pixels_Fill_Sub(x+datbuf1[i],y-i,datbuf2[i]-datbuf1[i],1,color);    //0~45
                Pixels_Fill_Sub(x+i,y+datbuf1[i],1,datbuf2[i]-datbuf1[i],color);    //-45~-90
                Pixels_Fill_Sub(x+datbuf1[i],y+i,datbuf2[i]-datbuf1[i],1,color);    //0~-45
                
                Pixels_Fill_Sub(x-i,y-datbuf2[i],1,datbuf2[i]-datbuf1[i],color);    //90~135
                Pixels_Fill_Sub(x-datbuf2[i],y-i,datbuf2[i]-datbuf1[i],1,color);    //135~180
                Pixels_Fill_Sub(x-i,y+datbuf1[i],1,datbuf2[i]-datbuf1[i],color);    //-90~-135
                Pixels_Fill_Sub(x-datbuf2[i],y+i,datbuf2[i]-datbuf1[i],1,color);    //-135~-180
            }
            
            for (;i<len2;i++)
            {
                Pixels_Fill_Sub(x+i,y-datbuf2[i],1,datbuf2[i]-len1,color);          //45~90
                Pixels_Fill_Sub(x+len1,y-i,datbuf2[i]-len1,1,color);                //0~45
                Pixels_Fill_Sub(x+i,y+len1,1,datbuf2[i]-len1,color);                //-45~-90
                Pixels_Fill_Sub(x+len1,y+i,datbuf2[i]-len1,1,color);                //0~-45
                
                Pixels_Fill_Sub(x-i,y-datbuf2[i],1,datbuf2[i]-len1,color);          //90~135
                Pixels_Fill_Sub(x-datbuf2[i],y-i,datbuf2[i]-len1,1,color);          //135~180
                Pixels_Fill_Sub(x-i,y+len1,1,datbuf2[i]-len1,color);                //-90~-135
                Pixels_Fill_Sub(x-datbuf2[i],y+i,datbuf2[i]-len1,1,color);                //-135~-180
            }
        }
    }
}

void Graphic_Draw_Circle(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t r, uint16_t color, uint8_t thickness)
{
    if (c->WriteThrough)
        Graphic_DisplayCircle(c->Area.X+re_x,c->Area.Y+re_y,r,color,thickness);
    else
    {
        if (!c->Virtual)
            Graphic_Canvas_DisplayCircle(c,re_x,re_y,r,color,thickness);
    }
}






















void Graphic_Canvas_DisplayBMP(canvas* c, uint16_t re_x, uint16_t re_y, const BMP* p);
void Graphic_Canvas_DisplayBMP2(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, const BMP* p);
void Graphic_Canvas_DisplayBMP3(canvas* c, uint16_t re_x, uint16_t re_y, float size, const BMP* p);
/*直接模式*/
void Graphic_DisplayBMP(uint16_t ab_x, uint16_t ab_y, const BMP* p);
void Graphic_DisplayBMP2(uint16_t ab_x, uint16_t ab_y, uint16_t width, uint16_t height, const BMP* p);
void Graphic_DisplayBMP3(uint16_t ab_x, uint16_t ab_y, float size, const BMP* p);

void Graphic_Canvas_DisplayBMP(canvas* c, uint16_t x, uint16_t y, const BMP* p)
{
	uint16_t i,j;
	uint32_t k;
	uint16_t* GRAM_Addr;
	
	GRAM_Addr=CANVAS_GRAM_ADDR(c,x,y);
	for (i=0,k=0;i<p->Height;i++)
	{
		for (j=0;j<p->Width;j++,k++)
			*(GRAM_Addr+j)=p->Colors[k];
		GRAM_Addr+=c->Area.Width;
	}
}

void Graphic_Canvas_DisplayBMP2(canvas* c, uint16_t x, uint16_t y, uint16_t width, uint16_t height, const BMP* p)
{
	if ((width==0)||(height==0))
		return;
	else if ((x+width>c->Area.Width)||(y+height>c->Area.Height))
		return;
	
	uint16_t i,j;
	float k;
	volatile float Pic_Y,Pic_X,X_Step,Y_Step;
	uint16_t* GRAM_Addr;
	
	X_Step=(float)p->Width/width;
	Y_Step=(float)p->Height/height;
	
	GRAM_Addr=CANVAS_GRAM_ADDR(c,x,y);
	Pic_Y=0;
	for (i=0;i<height;i++)
	{
		Pic_X=0;
		for (j=0;j<width;j++)
		{
			*(GRAM_Addr+j)=p->Colors[(uint32_t)(k+Pic_X)];
			Pic_X+=X_Step;
		}
		Pic_Y+=Y_Step;
		k=(uint16_t)Pic_Y;
		k*=p->Width;
		GRAM_Addr+=c->Area.Width;
	}
}

void Graphic_Canvas_DisplayBMP3(canvas* c, uint16_t x, uint16_t y, float size, const BMP* p)
{
	if (size==0)
		return;

	uint16_t width,height;
	
	width=MathHelper_Round(p->Width*size);
	height=MathHelper_Round(p->Height*size);
	Graphic_Canvas_DisplayBMP2(c,x,y,width,height,p);
}

void Graphic_DisplayBMP(uint16_t x, uint16_t y, const BMP* p)
{
    if (ScreenDevice.Canvas.Virtual)
        GRAM_Transfer(x,y,p->Width,p->Height,p->Colors);
    else
        Memory_Transfer(p->Colors,CANVAS_GRAM_ADDR(&ScreenDevice.Canvas,x,y),p->Width,p->Height,p->Width,ScreenDevice.Width);
}

void Graphic_DisplayBMP2(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const BMP* p)
{
    if (ScreenDevice.Canvas.Virtual)
    {
        if ((width==0)||(height==0))
            return;
        else if ((x+width>ScreenDevice.Width)||(y+height>ScreenDevice.Height))
            return;
        else if ((width==p->Width)&&(height==p->Height))
        {
            Graphic_DisplayBMP(x,y,p);
            return;
        }
        
        uint16_t i,j;
        float k;
        volatile float Pic_Y,Pic_X,X_Step,Y_Step;
        uint16_t* GRAM_Addr;
        
        X_Step=(float)p->Width/width;
        Y_Step=(float)p->Height/height;
        
        Pic_Y=0;
        for (i=0;i<height;i++)
        {
            Pic_X=0;
            for (j=0;j<width;j++)
            {
                Pixel_Draw_Pri(j+x,i+y,p->Colors[(uint32_t)(k+Pic_X)]);
                Pic_X+=X_Step;
            }
            Pic_Y+=Y_Step;
            k=(uint16_t)Pic_Y;
            k*=p->Width;
        }
    }
    else
    {
        if ((width==0)||(height==0))
            return;
        else if ((x+width>ScreenDevice.Width)||(y+height>ScreenDevice.Height))
            return;
        else if ((width==p->Width)&&(height==p->Height))
        {
            Graphic_DisplayBMP(x,y,p);
            return;
        }
        
        uint16_t i,j;
        float k;
        volatile float Pic_Y,Pic_X,X_Step,Y_Step;
        uint16_t* GRAM_Addr;
        
        X_Step=(float)p->Width/width;
        Y_Step=(float)p->Height/height;
        
        Pic_Y=0;
        for (i=0;i<height;i++)
        {
            Pic_X=0;
            for (j=0;j<width;j++)
            {
                Pixel_Draw_Sub(j+x,i+y,p->Colors[(uint32_t)(k+Pic_X)]);
                Pic_X+=X_Step;
            }
            Pic_Y+=Y_Step;
            k=(uint16_t)Pic_Y;
            k*=p->Width;
        }
    }
}

void Graphic_DisplayBMP3(uint16_t x, uint16_t y, float size, const BMP* p)
{
	if (size==0)
		return;

	uint16_t width,height;
	
	width=MathHelper_Round(p->Width*size);
	height=MathHelper_Round(p->Height*size);
	Graphic_DisplayBMP2(x,y,width,height,p);
}

void Graphic_Draw_BMP(canvas* c, uint16_t re_x, uint16_t re_y, const BMP* p)
{
    if (c->WriteThrough)
        Graphic_DisplayBMP(c->Area.X+re_x,c->Area.Y+re_y,p);
    else
    {
        if (!c->Virtual)
            Graphic_Canvas_DisplayBMP(c,re_x,re_y,p);
    }
}

void Graphic_Draw_BMP2(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, const BMP* p)
{
    if (c->WriteThrough)
        Graphic_DisplayBMP2(c->Area.X+re_x,c->Area.Y+re_y,width,height,p);
    else
    {
        if (!c->Virtual)
            Graphic_Canvas_DisplayBMP2(c,re_x,re_y,width,height,p);
    }
}

void Graphic_Draw_BMP3(canvas* c, uint16_t re_x, uint16_t re_y, float size, const BMP* p)
{
    if (c->WriteThrough)
        Graphic_DisplayBMP3(c->Area.X+re_x,c->Area.Y+re_y,size,p);
    else
    {
        if (!c->Virtual)
            Graphic_Canvas_DisplayBMP3(c,re_x,re_y,size,p);
    }
}














void Graphic_Canvas_DisplayrBMP(canvas* c, uint16_t re_x, uint16_t re_y, const rBMP* p);
void Graphic_Canvas_DisplayrBMP2(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, const rBMP* p);
void Graphic_Canvas_DisplayrBMP3(canvas* c, uint16_t re_x, uint16_t re_y, float size, const rBMP* p);
/*直接模式*/
void Graphic_DisplayrBMP(uint16_t ab_x, uint16_t ab_y, const rBMP* p);
void Graphic_DisplayrBMP2(uint16_t ab_x, uint16_t ab_y, uint16_t width, uint16_t height, const rBMP* p);
void Graphic_DisplayrBMP3(uint16_t ab_x, uint16_t ab_y, float size, const rBMP* p);

void Graphic_Canvas_DisplayrBMP(canvas* c, uint16_t x, uint16_t y, const rBMP* p)
{
    uint16_t i;
    uint16_t Start_Line;             //解码（也是绘制）的起始行
	uint16_t Max_Line,Remained_Line; //可以一次绘制的最大行数，剩余不足最大行数的行数
    
    uint16_t MaxLine_DrawTime;
	
	BMP save={c->Area.Width,c->Area.Height,Graphic_Buffer0};
	
	Max_Line=(GRAPHIC_BUFFER0_SIZE/2)/p->Width;
    
    MaxLine_DrawTime=p->Height/Max_Line;
    Remained_Line=p->Height%Max_Line;
    
    //先按照最大行数绘制
    for (i=0,Start_Line=0;i<MaxLine_DrawTime;i++,Start_Line+=Max_Line)
    {
        rBMP2BMP_Decode(p,&save,Start_Line,Max_Line);
        Graphic_Canvas_DisplayBMP(c,x,y+Start_Line,&save);
    }
    
    //绘制不足的那部分
    rBMP2BMP_Decode(p,&save,Start_Line,Remained_Line);
    save.Height=Remained_Line;
    Graphic_Canvas_DisplayBMP(c,x,y+Start_Line,&save);
}

void Graphic_Canvas_DisplayrBMP2(canvas* c, uint16_t x, uint16_t y, uint16_t width, uint16_t height, const rBMP* p)
{
	uint16_t Start_Index,End_Index;  //rBMP要解码部分起始行，结束行
	float Start_Line,End_Line;  //画布要画部分起始行，结束行
	uint16_t Max_Line;               //单次最大解码行数
	float Delta_Line;           //Max_Line对应的绘制的行数
	
	BMP save={width,height,Graphic_Buffer0};
	
	Max_Line=(GRAPHIC_BUFFER0_SIZE/2)/p->Width;
    
    //如果可以一次解码
	if (Max_Line>=p->Height)
	{
		rBMP2BMP_Decode(p,&save,0,p->Height);
		Graphic_Canvas_DisplayBMP2(c,x,y,width,height,&save);
	}
	else
	{
        //恰好不需要分批次绘制
        if ((width==p->Width)&&(height==p->Height))
            Graphic_Canvas_DisplayrBMP(c,x,y,p);
        else
        {
            Delta_Line=(float)Max_Line*height/p->Height;
                
            Start_Index=0;
            End_Index=Max_Line-1;
            //首先按最大行数绘制
            for (Start_Line=y;End_Index<=p->Height-1;Start_Index+=Max_Line,End_Index+=Max_Line)
            {
                End_Line=MathHelper_Round(Start_Line+Delta_Line);
                    
                rBMP2BMP_Decode(p,&save,Start_Index,Max_Line);
                    
                Graphic_Canvas_DisplayBMP2(c,x,MathHelper_Round(Start_Line),width,MathHelper_Round(End_Line-Start_Line),&save);
                Start_Line=End_Line;
            }
                
            //绘制剩余的
            if (Start_Index<p->Height)
            {
                if (Start_Line>y+height-1)
                    return;
                    
                End_Index=p->Height-1;
                End_Line=y+height-1;

                rBMP2BMP_Decode(p,&save,Start_Index,End_Index-Start_Index+1);
                Graphic_Canvas_DisplayBMP2(c,x,MathHelper_Round(Start_Line),width,MathHelper_Round(End_Line-Start_Line+1),&save);
            }
            else
                return;
        }
	}
}

void Graphic_Canvas_DisplayrBMP3(canvas* c, uint16_t x, uint16_t y, float size, const rBMP* p)
{
    if (size==0)
		return;

    Graphic_Canvas_DisplayrBMP2(c,x,y,MathHelper_Round(p->Width*size),MathHelper_Round(p->Height*size),p);
}




void Graphic_DisplayrBMP(uint16_t x, uint16_t y, const rBMP* p)
{
    uint16_t i;
    uint16_t Start_Line;             //解码（也是绘制）的起始行
	uint16_t Max_Line,Remained_Line; //可以一次绘制的最大行数，剩余不足最大行数的行数
    
    uint16_t MaxLine_DrawTime;
	
	BMP save={0,0,Graphic_Buffer0};
	
	Max_Line=(GRAPHIC_BUFFER0_SIZE/2)/p->Width;
    
    MaxLine_DrawTime=p->Height/Max_Line;
    Remained_Line=p->Height%Max_Line;
    
    //先按照最大行数绘制
    for (i=0,Start_Line=0;i<MaxLine_DrawTime;i++,Start_Line+=Max_Line)
    {
        rBMP2BMP_Decode(p,&save,Start_Line,Max_Line);
        Graphic_DisplayBMP(x,y+Start_Line,&save);
    }
    
    //绘制不足的那部分
    rBMP2BMP_Decode(p,&save,Start_Line,Remained_Line);
    save.Height=Remained_Line;
    Graphic_DisplayBMP(x,y+Start_Line,&save);
}

void Graphic_DisplayrBMP2(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const rBMP* p)
{
	uint16_t Start_Index,End_Index;  //rBMP要解码部分起始行，结束行
	float Start_Line,End_Line;  //画布要画部分起始行，结束行
	uint16_t Max_Line;               //单次最大解码行数
	float Delta_Line;           //Max_Line对应的绘制的行数
	
	BMP save={width,height,Graphic_Buffer0};
	
	Max_Line=(GRAPHIC_BUFFER0_SIZE/2)/p->Width;
    
    //如果可以一次解码
	if (Max_Line>=p->Height)
	{
		rBMP2BMP_Decode(p,&save,0,p->Height);
		Graphic_DisplayBMP2(x,y,width,height,&save);
	}
	else
	{
        //恰好不需要分批次绘制
        if ((width==p->Width)&&(height==p->Height))
            Graphic_DisplayrBMP(x,y,p);
        else
        {
            Delta_Line=(float)Max_Line*height/p->Height;
                
            Start_Index=0;
            End_Index=Max_Line-1;
            //首先按最大行数绘制
            for (Start_Line=y;End_Index<=p->Height-1;Start_Index+=Max_Line,End_Index+=Max_Line)
            {
                End_Line=MathHelper_Round(Start_Line+Delta_Line);
                    
                rBMP2BMP_Decode(p,&save,Start_Index,Max_Line);
                    
                Graphic_DisplayBMP2(x,MathHelper_Round(Start_Line),width,MathHelper_Round(End_Line-Start_Line),&save);
                Start_Line=End_Line;
            }
                
            //绘制剩余的
            if (Start_Index<p->Height)
            {
                if (Start_Line>y+height-1)
                    return;
                    
                End_Index=p->Height-1;
                End_Line=y+height-1;

                rBMP2BMP_Decode(p,&save,Start_Index,End_Index-Start_Index+1);
                Graphic_DisplayBMP2(x,MathHelper_Round(Start_Line),width,MathHelper_Round(End_Line-Start_Line+1),&save);
            }
            else
                return;
        }
	}
}

void Graphic_DisplayrBMP3(uint16_t x, uint16_t y, float size, const rBMP* p)
{
    if (size==0)
		return;

    Graphic_DisplayrBMP2(x,y,MathHelper_Round(p->Width*size),MathHelper_Round(p->Height*size),p);
}

void Graphic_Draw_rBMP(canvas* c, uint16_t re_x, uint16_t re_y, const rBMP* p)
{
    if (c->WriteThrough)
        Graphic_DisplayrBMP(c->Area.X+re_x,c->Area.Y+re_y,p);
    else
    {
        if (!c->Virtual)
            Graphic_Canvas_DisplayrBMP(c,re_x,re_y,p);
    }
}

void Graphic_Draw_rBMP2(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, const rBMP* p)
{
    if (c->WriteThrough)
        Graphic_DisplayrBMP2(c->Area.X+re_x,c->Area.Y+re_y,width,height,p);
    else
    {
        if (!c->Virtual)
            Graphic_Canvas_DisplayrBMP2(c,re_x,re_y,width,height,p);
    }
}

void Graphic_Draw_rBMP3(canvas* c, uint16_t re_x, uint16_t re_y, float size, const rBMP* p)
{
    if (c->WriteThrough)
        Graphic_DisplayrBMP3(c->Area.X+re_x,c->Area.Y+re_y,size,p);
    else
    {
        if (!c->Virtual)
            Graphic_Canvas_DisplayrBMP3(c,re_x,re_y,size,p);
    }
}













void Graphic_Canvas_DisplaypBMP(canvas* c, uint16_t re_x, uint16_t re_y, const pBMP* p);
void Graphic_Canvas_DisplaypBMP2(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, const pBMP* p);
void Graphic_Canvas_DisplaypBMP3(canvas* c, uint16_t re_x, uint16_t re_y, float size, const pBMP* p);
/*直接模式*/
void Graphic_DisplaypBMP(uint16_t ab_x, uint16_t ab_y, const pBMP* p);
void Graphic_DisplaypBMP2(uint16_t ab_x, uint16_t ab_y, uint16_t width, uint16_t height, const pBMP* p);
void Graphic_DisplaypBMP3(uint16_t ab_x, uint16_t ab_y, float size, const pBMP* p);

void Graphic_Canvas_DisplaypBMP(canvas* c, uint16_t x, uint16_t y, const pBMP* p)
{
    uint16_t i;
    uint16_t Start_Line;             //解码（也是绘制）的起始行
	uint16_t Max_Line,Remained_Line; //可以一次绘制的最大行数，剩余不足最大行数的行数
    
    uint16_t MaxLine_DrawTime;
	
	BMP save={c->Area.Width,c->Area.Height,Graphic_Buffer0};
	
	Max_Line=(GRAPHIC_BUFFER0_SIZE/2)/p->Width;
    
    MaxLine_DrawTime=p->Height/Max_Line;
    Remained_Line=p->Height%Max_Line;
    
    //先按照最大行数绘制
    for (i=0,Start_Line=0;i<MaxLine_DrawTime;i++,Start_Line+=Max_Line)
    {
        pBMP2BMP_Decode(p,&save,Start_Line,Max_Line);
        Graphic_Canvas_DisplayBMP(c,x,y+Start_Line,&save);
    }
    
    //绘制不足的那部分
    pBMP2BMP_Decode(p,&save,Start_Line,Remained_Line);
    save.Height=Remained_Line;
    Graphic_Canvas_DisplayBMP(c,x,y+Start_Line,&save);
}

void Graphic_Canvas_DisplaypBMP2(canvas* c, uint16_t x, uint16_t y, uint16_t width, uint16_t height, const pBMP* p)
{
	uint16_t Start_Index,End_Index;
	float Start_Line,End_Line;
	uint16_t Max_Line;
	float Delta_Line;
	
	BMP save;
	
	save.Colors=Graphic_Buffer0;
	Max_Line=(GRAPHIC_BUFFER0_SIZE/2)/p->Width;

	if (Max_Line>=p->Height)
	{
		pBMP2BMP_Decode(p,&save,0,p->Height);
		Graphic_Canvas_DisplayBMP2(c,x,y,width,height,&save);
	}
	else
	{
        if ((width==p->Width)&&(height==p->Height))
            Graphic_Canvas_DisplaypBMP(c,x,y,p);
        else
        {
            Delta_Line=(float)Max_Line*height/p->Height;
            
            Start_Index=0;
            End_Index=Max_Line-1;
            for (Start_Line=y;End_Index<=p->Height-1;Start_Index+=Max_Line,End_Index+=Max_Line)
            {
                End_Line=MathHelper_Round(Start_Line+Delta_Line);
                
                pBMP2BMP_Decode(p,&save,Start_Index,Max_Line);
                
                Graphic_Canvas_DisplayBMP2(c,x,MathHelper_Round(Start_Line),width,MathHelper_Round(End_Line-Start_Line),&save);
                Start_Line=End_Line;
            }
            
            if (Start_Index<p->Height)
            {
                if (Start_Line>y+height-1)
                    return;
                
                End_Index=p->Height-1;
                End_Line=y+height-1;

                pBMP2BMP_Decode(p,&save,Start_Index,End_Index-Start_Index);
                Graphic_Canvas_DisplayBMP2(c,x,MathHelper_Round(Start_Line),width,MathHelper_Round(End_Line-Start_Line),&save);
            }
            else
                return;
        }
	}
}

void Graphic_Canvas_DisplaypBMP3(canvas* c, uint16_t x, uint16_t y, float size, const pBMP* p)
{
    if (size==0)
		return;

    Graphic_Canvas_DisplaypBMP2(c,x,y,MathHelper_Round(p->Width*size),MathHelper_Round(p->Height*size),p);
}

void Graphic_DisplaypBMP(uint16_t x, uint16_t y, const pBMP* p)
{
    uint16_t i;
    uint16_t Start_Line;             //解码（也是绘制）的起始行
	uint16_t Max_Line,Remained_Line; //可以一次绘制的最大行数，剩余不足最大行数的行数
    
    uint16_t MaxLine_DrawTime;
	
	BMP save={0,0,Graphic_Buffer0};
	
	Max_Line=(GRAPHIC_BUFFER0_SIZE/2)/p->Width;
    
    MaxLine_DrawTime=p->Height/Max_Line;
    Remained_Line=p->Height%Max_Line;
    
    //先按照最大行数绘制
    for (i=0,Start_Line=0;i<MaxLine_DrawTime;i++,Start_Line+=Max_Line)
    {
        pBMP2BMP_Decode(p,&save,Start_Line,Max_Line);
        Graphic_DisplayBMP(x,y+Start_Line,&save);
    }
    
    //绘制不足的那部分
    pBMP2BMP_Decode(p,&save,Start_Line,Remained_Line);
    save.Height=Remained_Line;
    Graphic_DisplayBMP(x,y+Start_Line,&save);
}

void Graphic_DisplaypBMP2(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const pBMP* p)
{
	uint16_t Start_Index,End_Index;
	float Start_Line,End_Line;
	uint16_t Max_Line;
	float Delta_Line;
	
	BMP save;
	
	save.Colors=Graphic_Buffer0;
	Max_Line=(GRAPHIC_BUFFER0_SIZE/2)/p->Width;

	if (Max_Line>=p->Height)
	{
		pBMP2BMP_Decode(p,&save,0,p->Height);
		Graphic_DisplayBMP2(x,y,width,height,&save);
	}
	else
	{
        if ((width==p->Width)&&(height==p->Height))
            Graphic_DisplaypBMP(x,y,p);
        else
        {
            Delta_Line=(float)Max_Line*height/p->Height;
            
            Start_Index=0;
            End_Index=Max_Line-1;
            for (Start_Line=y;End_Index<=p->Height-1;Start_Index+=Max_Line,End_Index+=Max_Line)
            {
                End_Line=MathHelper_Round(Start_Line+Delta_Line);
                
                pBMP2BMP_Decode(p,&save,Start_Index,Max_Line);
                
                Graphic_DisplayBMP2(x,MathHelper_Round(Start_Line),width,MathHelper_Round(End_Line-Start_Line),&save);
                Start_Line=End_Line;
            }
            
            if (Start_Index<p->Height)
            {
                if (Start_Line>y+height-1)
                    return;
                
                End_Index=p->Height-1;
                End_Line=y+height-1;

                pBMP2BMP_Decode(p,&save,Start_Index,End_Index-Start_Index);
                Graphic_DisplayBMP2(x,MathHelper_Round(Start_Line),width,MathHelper_Round(End_Line-Start_Line),&save);
            }
            else
                return;
        }
	}
}

void Graphic_DisplaypBMP3(uint16_t x, uint16_t y, float size, const pBMP* p)
{
    if (size==0)
		return;

    Graphic_DisplaypBMP2(x,y,MathHelper_Round(p->Width*size),MathHelper_Round(p->Height*size),p);
}

void Graphic_Draw_pBMP(canvas* c, uint16_t re_x, uint16_t re_y, const pBMP* p)
{
    if (c->WriteThrough)
        Graphic_DisplaypBMP(c->Area.X+re_x,c->Area.Y+re_y,p);
    else
    {
        if (!c->Virtual)
            Graphic_Canvas_DisplaypBMP(c,re_x,re_y,p);
    }
}

void Graphic_Draw_pBMP2(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, const pBMP* p)
{
    if (c->WriteThrough)
        Graphic_DisplaypBMP2(c->Area.X+re_x,c->Area.Y+re_y,width,height,p);
    else
    {
        if (!c->Virtual)
            Graphic_Canvas_DisplaypBMP2(c,re_x,re_y,width,height,p);
    }
}

void Graphic_Draw_pBMP3(canvas* c, uint16_t re_x, uint16_t re_y, float size, const pBMP* p)
{
    if (c->WriteThrough)
        Graphic_DisplaypBMP3(c->Area.X+re_x,c->Area.Y+re_y,size,p);
    else
    {
        if (!c->Virtual)
            Graphic_Canvas_DisplaypBMP3(c,re_x,re_y,size,p);
    }
}















void Graphic_Canvas_DisplayprBMP(canvas* c, uint16_t re_x, uint16_t re_y, const prBMP* p);
void Graphic_Canvas_DisplayprBMP2(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, const prBMP* p);
void Graphic_Canvas_DisplayprBMP3(canvas* c, uint16_t re_x, uint16_t re_y, float size, const prBMP* p);
/*直接模式*/
void Graphic_DisplayprBMP(uint16_t ab_x, uint16_t ab_y, const prBMP* p);
void Graphic_DisplayprBMP2(uint16_t ab_x, uint16_t ab_y, uint16_t width, uint16_t height, const prBMP* p);
void Graphic_DisplayprBMP3(uint16_t ab_x, uint16_t ab_y, float size, const prBMP* p);

void Graphic_Canvas_DisplayprBMP(canvas* c, uint16_t x, uint16_t y, const prBMP* p)
{
    uint16_t i;
    uint16_t Start_Line;             //解码（也是绘制）的起始行
	uint16_t Max_Line,Remained_Line; //可以一次绘制的最大行数，剩余不足最大行数的行数
    
    uint16_t MaxLine_DrawTime;
	
	BMP save={c->Area.Width,c->Area.Height,Graphic_Buffer0};
	
	Max_Line=(GRAPHIC_BUFFER0_SIZE/2)/p->Width;
    
    MaxLine_DrawTime=p->Height/Max_Line;
    Remained_Line=p->Height%Max_Line;
    
    //先按照最大行数绘制
    for (i=0,Start_Line=0;i<MaxLine_DrawTime;i++,Start_Line+=Max_Line)
    {
        prBMP2BMP_Decode(p,&save,Start_Line,Max_Line);
        Graphic_Canvas_DisplayBMP(c,x,y+Start_Line,&save);
    }
    
    //绘制不足的那部分
    prBMP2BMP_Decode(p,&save,Start_Line,Remained_Line);
    save.Height=Remained_Line;
    Graphic_Canvas_DisplayBMP(c,x,y+Start_Line,&save);
}

void Graphic_Canvas_DisplayprBMP2(canvas* c, uint16_t x, uint16_t y, uint16_t width, uint16_t height, const prBMP* p)
{
	uint16_t Start_Index,End_Index;
	float Start_Line,End_Line;
	uint16_t Max_Line;
	float Delta_Line;
	
	BMP save;
	
	save.Colors=Graphic_Buffer0;
	Max_Line=(GRAPHIC_BUFFER0_SIZE/2)/p->Width;

	if (Max_Line>=p->Height)
	{
		prBMP2BMP_Decode(p,&save,0,p->Height);
		Graphic_Canvas_DisplayBMP2(c,x,y,width,height,&save);
	}
	else
	{
        if ((width==p->Width)&&(height==p->Height))
            Graphic_Canvas_DisplayprBMP(c,x,y,p);
        else
        {
            Delta_Line=(float)Max_Line*height/p->Height;
            
            Start_Index=0;
            End_Index=Max_Line-1;
            for (Start_Line=y;End_Index<=p->Height-1;Start_Index+=Max_Line,End_Index+=Max_Line)
            {
                End_Line=MathHelper_Round(Start_Line+Delta_Line);
                
                prBMP2BMP_Decode(p,&save,Start_Index,Max_Line);
                
                Graphic_Canvas_DisplayBMP2(c,x,MathHelper_Round(Start_Line),width,MathHelper_Round(End_Line-Start_Line),&save);
                Start_Line=End_Line;
            }
            
            if (Start_Index<p->Height)
            {
                if (Start_Line>y+height-1)
                    return;
                
                End_Index=p->Height-1;
                End_Line=y+height-1;

                prBMP2BMP_Decode(p,&save,Start_Index,End_Index-Start_Index);
                Graphic_Canvas_DisplayBMP2(c,x,MathHelper_Round(Start_Line),width,MathHelper_Round(End_Line-Start_Line),&save);
            }
            else
                return;
        }
	}
}



void Graphic_Canvas_DisplayprBMP3(canvas* c, uint16_t x, uint16_t y, float size, const prBMP* p)
{
    if (size==0)
		return;

    Graphic_Canvas_DisplayprBMP2(c,x,y,MathHelper_Round(p->Width*size),MathHelper_Round(p->Height*size),p);
}

void Graphic_DisplayprBMP(uint16_t x, uint16_t y, const prBMP* p)
{
    uint16_t i;
    uint16_t Start_Line;             //解码（也是绘制）的起始行
	uint16_t Max_Line,Remained_Line; //可以一次绘制的最大行数，剩余不足最大行数的行数
    
    uint16_t MaxLine_DrawTime;
	
	BMP save={0,0,Graphic_Buffer0};
	
	Max_Line=(GRAPHIC_BUFFER0_SIZE/2)/p->Width;
    
    MaxLine_DrawTime=p->Height/Max_Line;
    Remained_Line=p->Height%Max_Line;
    
    //先按照最大行数绘制
    for (i=0,Start_Line=0;i<MaxLine_DrawTime;i++,Start_Line+=Max_Line)
    {
        prBMP2BMP_Decode(p,&save,Start_Line,Max_Line);
        Graphic_DisplayBMP(x,y+Start_Line,&save);
    }
    
    //绘制不足的那部分
    prBMP2BMP_Decode(p,&save,Start_Line,Remained_Line);
    save.Height=Remained_Line;
    Graphic_DisplayBMP(x,y+Start_Line,&save);
}

void Graphic_DisplayprBMP2(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const prBMP* p)
{
	uint16_t Start_Index,End_Index;
	float Start_Line,End_Line;
	uint16_t Max_Line;
	float Delta_Line;
	
	BMP save;
	
	save.Colors=Graphic_Buffer0;
	Max_Line=(GRAPHIC_BUFFER0_SIZE/2)/p->Width;

	if (Max_Line>=p->Height)
	{
		prBMP2BMP_Decode(p,&save,0,p->Height);
		Graphic_DisplayBMP2(x,y,width,height,&save);
	}
	else
	{
        if ((width==p->Width)&&(height==p->Height))
            Graphic_DisplayprBMP(x,y,p);
        else
        {
            Delta_Line=(float)Max_Line*height/p->Height;
            
            Start_Index=0;
            End_Index=Max_Line-1;
            for (Start_Line=y;End_Index<=p->Height-1;Start_Index+=Max_Line,End_Index+=Max_Line)
            {
                End_Line=MathHelper_Round(Start_Line+Delta_Line);
                
                prBMP2BMP_Decode(p,&save,Start_Index,Max_Line);
                
                Graphic_DisplayBMP2(x,MathHelper_Round(Start_Line),width,MathHelper_Round(End_Line-Start_Line),&save);
                Start_Line=End_Line;
            }
            
            if (Start_Index<p->Height)
            {
                if (Start_Line>y+height-1)
                    return;
                
                End_Index=p->Height-1;
                End_Line=y+height-1;

                prBMP2BMP_Decode(p,&save,Start_Index,End_Index-Start_Index);
                Graphic_DisplayBMP2(x,MathHelper_Round(Start_Line),width,MathHelper_Round(End_Line-Start_Line),&save);
            }
            else
                return;
        }
	}
}

void Graphic_DisplayprBMP3(uint16_t x, uint16_t y, float size, const prBMP* p)
{
    if (size==0)
		return;

    Graphic_DisplayprBMP2(x,y,MathHelper_Round(p->Width*size),MathHelper_Round(p->Height*size),p);
}

void Graphic_Draw_prBMP(canvas* c, uint16_t re_x, uint16_t re_y, const prBMP* p)
{
    if (c->WriteThrough)
        Graphic_DisplayprBMP(c->Area.X+re_x,c->Area.Y+re_y,p);
    else
    {
        if (!c->Virtual)
            Graphic_Canvas_DisplayprBMP(c,re_x,re_y,p);
    }
}

void Graphic_Draw_prBMP2(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, const prBMP* p)
{
    if (c->WriteThrough)
        Graphic_DisplayprBMP2(c->Area.X+re_x,c->Area.Y+re_y,width,height,p);
    else
    {
        if (!c->Virtual)
            Graphic_Canvas_DisplayprBMP2(c,re_x,re_y,width,height,p);
    }
}

void Graphic_Draw_prBMP3(canvas* c, uint16_t re_x, uint16_t re_y, float size, const prBMP* p)
{
    if (c->WriteThrough)
        Graphic_DisplayprBMP3(c->Area.X+re_x,c->Area.Y+re_y,size,p);
    else
    {
        if (!c->Virtual)
            Graphic_Canvas_DisplayprBMP3(c,re_x,re_y,size,p);
    }
}

























void Graphic_Canvas_DisplayBMP_All(canvas* c, uint16_t re_x, uint16_t re_y, const bmp* p);
void Graphic_Canvas_DisplayBMP_All2(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, const bmp* p);
void Graphic_Canvas_DisplayBMP_All3(canvas* c, uint16_t re_x, uint16_t re_y, float size, const bmp* p);

void Graphic_DisplayBMP_All(uint16_t ab_x, uint16_t ab_y, const bmp* p);
void Graphic_DisplayBMP_All2(uint16_t ab_x, uint16_t ab_y, uint16_t width, uint16_t height, const bmp* p);
void Graphic_DisplayBMP_All3(uint16_t ab_x, uint16_t ab_y, float size, const bmp* p);

void Graphic_Canvas_DisplayBMP_All(canvas* c, uint16_t x, uint16_t y, const bmp* p)
{
    if (p->Format==BMP_f)
        Graphic_Canvas_DisplayBMP(c,x,y,&p->BMP_Data.bmp);
    else if (p->Format==rBMP_f)
        Graphic_Canvas_DisplayrBMP(c,x,y,&p->BMP_Data.rbmp);
    else if (p->Format==pBMP_f)
        Graphic_Canvas_DisplaypBMP(c,x,y,&p->BMP_Data.pbmp);
    else if (p->Format==prBMP_f)
        Graphic_Canvas_DisplayprBMP(c,x,y,&p->BMP_Data.prbmp);
}

void Graphic_Canvas_DisplayBMP_All2(canvas* c, uint16_t x, uint16_t y, uint16_t width, uint16_t height, const bmp* p)
{
    if (p->Format==BMP_f)
        Graphic_Canvas_DisplayBMP2(c,x,y,width,height,&p->BMP_Data.bmp);
    else if (p->Format==rBMP_f)
        Graphic_Canvas_DisplayrBMP2(c,x,y,width,height,&p->BMP_Data.rbmp);
    else if (p->Format==pBMP_f)
        Graphic_Canvas_DisplaypBMP2(c,x,y,width,height,&p->BMP_Data.pbmp);
    else if (p->Format==prBMP_f)
        Graphic_Canvas_DisplayprBMP2(c,x,y,width,height,&p->BMP_Data.prbmp);
}
void Graphic_Canvas_DisplayBMP_All3(canvas* c, uint16_t x, uint16_t y, float size, const bmp* p)
{
    if (p->Format==BMP_f)
        Graphic_Canvas_DisplayBMP3(c,x,y,size,&p->BMP_Data.bmp);
    else if (p->Format==rBMP_f)
        Graphic_Canvas_DisplayrBMP3(c,x,y,size,&p->BMP_Data.rbmp);
    else if (p->Format==pBMP_f)
        Graphic_Canvas_DisplaypBMP3(c,x,y,size,&p->BMP_Data.pbmp);
    else if (p->Format==prBMP_f)
        Graphic_Canvas_DisplayprBMP3(c,x,y,size,&p->BMP_Data.prbmp);   
}

void Graphic_DisplayBMP_All(uint16_t x, uint16_t y, const bmp* p)
{
    if (p->Format==BMP_f)
        Graphic_DisplayBMP(x,y,&p->BMP_Data.bmp);
    else if (p->Format==rBMP_f)
        Graphic_DisplayrBMP(x,y,&p->BMP_Data.rbmp);
    else if (p->Format==pBMP_f)
        Graphic_DisplaypBMP(x,y,&p->BMP_Data.pbmp);
    else if (p->Format==prBMP_f)
        Graphic_DisplayprBMP(x,y,&p->BMP_Data.prbmp);
}

void Graphic_DisplayBMP_All2(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const bmp* p)
{
    if (p->Format==BMP_f)
        Graphic_DisplayBMP2(x,y,width,height,&p->BMP_Data.bmp);
    else if (p->Format==rBMP_f)
        Graphic_DisplayrBMP2(x,y,width,height,&p->BMP_Data.rbmp);
    else if (p->Format==pBMP_f)
        Graphic_DisplaypBMP2(x,y,width,height,&p->BMP_Data.pbmp);
    else if (p->Format==prBMP_f)
        Graphic_DisplayprBMP2(x,y,width,height,&p->BMP_Data.prbmp);
}

void Graphic_DisplayBMP_All3(uint16_t x, uint16_t y, float size, const bmp* p)
{
    if (p->Format==BMP_f)
        Graphic_DisplayBMP3(x,y,size,&p->BMP_Data.bmp);
    else if (p->Format==rBMP_f)
        Graphic_DisplayrBMP3(x,y,size,&p->BMP_Data.rbmp);
    else if (p->Format==pBMP_f)
        Graphic_DisplaypBMP3(x,y,size,&p->BMP_Data.pbmp);
    else if (p->Format==prBMP_f)
        Graphic_DisplayprBMP3(x,y,size,&p->BMP_Data.prbmp);  
}

void Graphic_Draw_BMP_All(canvas* c, uint16_t re_x, uint16_t re_y, const bmp* p)
{
    if (c->WriteThrough)
        Graphic_DisplayBMP_All(c->Area.X+re_x,c->Area.Y+re_y,p);
    else
    {
        if (!c->Virtual)
            Graphic_Canvas_DisplayBMP_All(c,re_x,re_y,p);
    }
}

void Graphic_Draw_BMP_All2(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, const bmp* p)
{
    if (c->WriteThrough)
        Graphic_DisplayBMP_All2(c->Area.X+re_x,c->Area.Y+re_y,width,height,p);
    else
    {
        if (!c->Virtual)
            Graphic_Canvas_DisplayBMP_All2(c,re_x,re_y,width,height,p);
    }
}

void Graphic_Draw_BMP_All3(canvas* c, uint16_t re_x, uint16_t re_y, float size, const bmp* p)
{
    if (c->WriteThrough)
        Graphic_DisplayBMP_All3(c->Area.X+re_x,c->Area.Y+re_y,size,p);
    else
    {
        if (!c->Virtual)
            Graphic_Canvas_DisplayBMP_All3(c,re_x,re_y,size,p);
    }
}

















void Graphic_Canvas_DisplayChar(canvas* c, uint16_t re_x, uint16_t re_y, uint8_t ch, uint16_t width, uint16_t height, uint16_t bcolor, uint16_t ccolor, const charmold* mold);
void Graphic_Canvas_DisplayChar2(canvas* c, uint16_t re_x, uint16_t re_y, uint8_t ch, uint16_t bcolor, uint16_t ccolor, const charmold* mold);
void Graphic_DisplayChar(uint16_t ab_x, uint16_t ab_y, uint8_t ch, uint16_t width, uint16_t height, uint16_t bcolor, uint16_t ccolor, const charmold* mold);
void Graphic_DisplayChar2(uint16_t ab_x, uint16_t ab_y, uint8_t ch, uint16_t bcolor, uint16_t ccolor, const charmold* mold);

void Graphic_Canvas_DisplayChar(canvas* c, uint16_t X, uint16_t Y, uint8_t Char, uint16_t width, uint16_t height, uint16_t bcolor, uint16_t ccolor, const charmold* mold)
{
	uint16_t i,m,color;
	uint32_t k,j;
	charmold* chrmod;
	BMP p;
	
    chrmod=(charmold*)&mold[Char-0x20];
	k=chrmod->Width*chrmod->Height;
	color=bcolor;
	for (j=0,i=0,i=0;j<k;i++)
	{
		for (m=0;m<chrmod->Blocks[i];m++,j++)
			((uint16_t*)Graphic_Buffer0)[j]=color;
		if (color==bcolor)
			color=ccolor;
		else
			color=bcolor;
	}
	p.Colors=(uint16_t*)Graphic_Buffer0;
	p.Height=chrmod->Height;
	p.Width=chrmod->Width;
	Graphic_Canvas_DisplayBMP2(c,X,Y,width,height,&p);
}

void Graphic_Canvas_DisplayChar2(canvas* c, uint16_t x, uint16_t y, uint8_t ch, uint16_t bcolor, uint16_t ccolor, const charmold* mold)
{
    Graphic_Canvas_DisplayChar(c,x,y,ch,mold[ch-0x20].Width,mold[ch-0x20].Height,bcolor,ccolor,mold);
}

void Graphic_DisplayChar(uint16_t x, uint16_t y, uint8_t ch, uint16_t width, uint16_t height, uint16_t bcolor, uint16_t ccolor, const charmold* mold)
{
	uint16_t i,m,color;
	uint32_t k,j;
	charmold* chrmod;
	BMP p;
	
    chrmod=(charmold*)&mold[ch-0x20];
	k=chrmod->Width*chrmod->Height;
	color=bcolor;
	for (j=0,i=0,i=0;j<k;i++)
	{
		for (m=0;m<chrmod->Blocks[i];m++,j++)
			((uint16_t*)Graphic_Buffer0)[j]=color;
		if (color==bcolor)
			color=ccolor;
		else
			color=bcolor;
	}
	p.Colors=(uint16_t*)Graphic_Buffer0;
	p.Height=chrmod->Height;
	p.Width=chrmod->Width;
	Graphic_DisplayBMP2(x,y,width,height,&p);
}

void Graphic_DisplayChar2(uint16_t x, uint16_t y, uint8_t ch, uint16_t bcolor, uint16_t ccolor, const charmold* mold)
{
    Graphic_DisplayChar(x,y,ch,mold[ch-0x20].Width,mold[ch-0x20].Height,bcolor,ccolor,mold);
}

void Graphic_Draw_Char(canvas* c, uint16_t re_x, uint16_t re_y, uint8_t ch, uint16_t width, uint16_t height, uint16_t bcolor, uint16_t ccolor, const charmold* mold)
{
    if (c->WriteThrough)
        Graphic_DisplayChar(c->Area.X+re_x,c->Area.Y+re_y,ch,width,height,bcolor,ccolor,mold);
    else
    {
        if (!c->Virtual)
            Graphic_Canvas_DisplayChar(c,re_x,re_y,ch,width,height,bcolor,ccolor,mold);
    }
}

void Graphic_Draw_Char2(canvas* c, uint16_t re_x, uint16_t re_y, uint8_t ch, uint16_t bcolor, uint16_t ccolor, const charmold* mold)
{
    if (c->WriteThrough)
        Graphic_DisplayChar2(c->Area.X+re_x,c->Area.Y+re_y,ch,bcolor,ccolor,mold);
    else
    {
        if (!c->Virtual)
            Graphic_Canvas_DisplayChar2(c,re_x,re_y,ch,bcolor,ccolor,mold);
    }
}



















void Graphic_Canvas_DisplayString(canvas* Canvas, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, uint8_t valign, uint8_t halign, font* f, char* str);
void Graphic_Canvas_DisplayString2(canvas* Canvas, uint16_t re_x, uint16_t re_y, short line, short row, uint8_t valign, uint8_t halign, font* f, char* str);
void Graphic_Canvas_DisplayString3(canvas* Canvas, uint16_t re_x, uint16_t re_y, uint16_t width ,uint16_t height, uint8_t valign, uint8_t halign, uint16_t bcolor, uint16_t ccolor, const charmold* mold, char* str);
void Graphic_Canvas_DisplayString4(canvas* Canvas, uint16_t re_x, uint16_t re_y, short line, short row, uint8_t valign, uint8_t halign, uint16_t bcolor, uint16_t ccolor, const charmold* mold, char* str);

void Graphic_DisplayString(uint16_t ab_x, uint16_t ab_y, uint16_t width, uint16_t height, uint8_t valign, uint8_t halign, font* f, char* str);
void Graphic_DisplayString2(uint16_t ab_x, uint16_t ab_y, short line, short row, uint8_t valign, uint8_t halign, font* f, char* str);
void Graphic_DisplayString3(uint16_t ab_x, uint16_t ab_y, uint16_t width ,uint16_t height, uint8_t valign, uint8_t halign, uint16_t bcolor, uint16_t ccolor, const charmold* mold, char* str);
void Graphic_DisplayString4(uint16_t ab_x, uint16_t ab_y, short line, short row, uint8_t valign, uint8_t halign, uint16_t bcolor, uint16_t ccolor, const charmold* mold, char* str);


uint8_t _String_Preprocess(char* str, uint16_t width, uint16_t charwidth)
{
    uint16_t i=0,k=0,m=0;
    uint16_t row=width/charwidth;
    short j;
    
    while (1)
    {
        //将字符串分割为行
        for (j=0;j<row;j++,i++)
        {
            if (str[i]=='\n')
            {
                i++;
                break;
            }
            else if (str[i]=='\0')
                break;
            else
            {
                Print_Buffer1[m]=str[i];
                m++;
            }
        }
        
        k++;
        Print_Buffer1[m]='\0';
        m++;
        
        if (str[i]=='\0')
            break;
        else if ((str[i]=='\n')&&(j==row))  //恰好满足一行的长度则忽略一个'\n'
            i++;
    }
    return k;
}

void _Display_String_Line(uint16_t x, uint16_t y, uint16_t width, horalign_type halign, char* str, font* f)
{
    uint16_t i,j;
    short _x;
    uint16_t len=strlen(str);
    
    if (halign==Left_Align)
        _x=x;
    else if (halign==HorMid)
        _x=x+(width-f->Mold->Width*len)/2;
    else if (halign==Right_Align)
        _x=x+width-f->Mold->Width*len;
    
    for (i=0;i<len;i++)
    {
        Graphic_DisplayChar2(_x,y,str[i],f->BackColor,f->CharColor,f->Mold);
        _x+=f->Mold->Width;
    }
}





void _Canvas_Display_String_Line(canvas* c, uint16_t x, uint16_t y, uint16_t width, horalign_type halign, char* str, font* f)
{
    uint16_t i,j;
    short _x;
    uint16_t len=strlen(str);
    
    if (halign==Left_Align)
        _x=x;
    else if (halign==HorMid)
        _x=x+(width-f->Mold->Width*len)/2;
    else if (halign==Right_Align)
        _x=x+width-f->Mold->Width*len;
    
    for (i=0;i<len;i++)
    {
        Graphic_Canvas_DisplayChar2(c,_x,y,str[i],f->BackColor,f->CharColor,f->Mold);
        _x+=f->Mold->Width;
    }
}



void Graphic_Canvas_DisplayString(canvas* c, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t valign, uint8_t halign, font* f, char* str)
{
    uint16_t i,k;
    uint16_t len,linenum;
	uint16_t _y;
    
    linenum=_String_Preprocess(str,width,f->Mold->Width);
        
    if (valign==Top_Align)
        _y=y;
    else if (valign==VerMid)
        _y=y+(height-linenum*f->Mold->Height)/2;
    else if (valign==Bottom_Align)
        _y=y+height-linenum*f->Mold->Height;
    
    
    for (i=0,k=0;i<linenum;i++)
    {
        _Canvas_Display_String_Line(c,x,_y+i*f->Mold->Height,width,halign,Print_Buffer1+k,f);
        if (i!=linenum-1)
        {
            while (Print_Buffer1[k]!='\0')
                k++;
            k++;
        }
    }
}

void Graphic_Canvas_DisplayString2(canvas* c, uint16_t x, uint16_t y, short line, short row, uint8_t valign, uint8_t halign, font* f, char* str)
{
    if (line==INF)
    {
        valign=Top_Align;
        line=c->Area.Height-y-1;
    }
    else
        line=f->Mold->Height*line;
    
    if (row==INF)
    {
        halign=Left_Align;
        row=c->Area.Width-x-1;
    }
    else
        row=f->Mold->Width*row;
    
    Graphic_Canvas_DisplayString(c,x,y,row,line,valign,halign,f,str);
}

void Graphic_Canvas_DisplayString3(canvas* c, uint16_t x, uint16_t y, uint16_t width ,uint16_t height, uint8_t valign, uint8_t halign, uint16_t bcolor, uint16_t ccolor, const charmold* mold, char* str)
{
	font f={bcolor,ccolor,mold};
	Graphic_Canvas_DisplayString(c,x,y,width,height,valign,halign,&f,str);
}

void Graphic_Canvas_DisplayString4(canvas* c, uint16_t x, uint16_t y, short line, short row, uint8_t valign, uint8_t halign, uint16_t bcolor, uint16_t ccolor, const charmold* mold, char* str)
{
    if (line==INF)
    {
        valign=Top_Align;
        line=c->Area.Height-y-1;
    }
    else
        line=mold->Height*line;
    
    if (row==INF)
    {
        halign=Left_Align;
        row=c->Area.Width-x-1;
    }
    else
        row=mold->Width*row;
    
    Graphic_Canvas_DisplayString3(c,x,y,row,line,valign,halign,bcolor,ccolor,mold,str);
}

void Graphic_DisplayString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t valign, uint8_t halign, font* f, char* str)
{
	uint16_t i,k;
    uint16_t len,linenum;
	uint16_t _y;
    
    linenum=_String_Preprocess(str,width,f->Mold->Width);
        
    if (valign==Top_Align)
        _y=y;
    else if (valign==VerMid)
        _y=y+(height-linenum*f->Mold->Height)/2;
    else if (valign==Bottom_Align)
        _y=y+height-linenum*f->Mold->Height;
    
    
    for (i=0,k=0;i<linenum;i++)
    {
        _Display_String_Line(x,_y+i*f->Mold->Height,width,halign,Print_Buffer1+k,f);
        if (i!=linenum-1)
        {
            while (Print_Buffer1[k]!='\0')
                k++;
            k++;
        }
    }
}

void Graphic_DisplayString2(uint16_t x, uint16_t y, short line, short row, uint8_t valign, uint8_t halign, font* f, char* str)
{
    if (line==INF)
    {
        valign=Top_Align;
        line=ScreenDevice.Height-y-1;
    }
    else
        line=f->Mold->Height*line;
    
    if (row==INF)
    {
        halign=Left_Align;
        row=ScreenDevice.Width-x-1;
    }
    else
        row=f->Mold->Width*row;
    
    Graphic_DisplayString(x,y,row,line,valign,halign,f,str);
}

void Graphic_DisplayString3(uint16_t x, uint16_t y, uint16_t width ,uint16_t height, uint8_t valign, uint8_t halign, uint16_t bcolor, uint16_t ccolor, const charmold* mold, char* str)
{
	font f={bcolor,ccolor,mold};
	Graphic_DisplayString(x,y,width,height,valign,halign,&f,str);
}

void Graphic_DisplayString4(uint16_t X, uint16_t Y, short line, short row, uint8_t valign, uint8_t halign, uint16_t bcolor, uint16_t ccolor, const charmold* mold, char* str)
{
    if (line==INF)
    {
        valign=Top_Align;
        line=ScreenDevice.Height-Y-1;
    }
    else
        line=mold->Height*line;
    
    if (row==INF)
    {
        halign=Left_Align;
        row=ScreenDevice.Width-X-1;
    }
    else
        row=mold->Width*row;
    
    Graphic_DisplayString3(X,Y,row,line,valign,halign,bcolor,ccolor,mold,str);
}

void Graphic_Draw_String(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, uint8_t valign, uint8_t halign, font* f, char* str)
{
    if (c->WriteThrough)
        Graphic_DisplayString(c->Area.X+re_x,c->Area.Y+re_y,width,height,valign,halign,f,str);
    else
    {
        if (!c->Virtual)
            Graphic_Canvas_DisplayString(c,re_x,re_y,width,height,valign,halign,f,str);
    }
}

void Graphic_Draw_String2(canvas* c, uint16_t re_x, uint16_t re_y, short line, short row, uint8_t valign, uint8_t halign, font* f, char* str)
{
    if (c->WriteThrough)
        Graphic_DisplayString2(c->Area.X+re_x,c->Area.Y+re_y,line,row,valign,halign,f,str);
    else
    {
        if (!c->Virtual)
            Graphic_Canvas_DisplayString2(c,re_x,re_y,line,row,valign,halign,f,str);
    }
}

void Graphic_Draw_String3(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width ,uint16_t height, uint8_t valign, uint8_t halign, uint16_t bcolor, uint16_t ccolor, const charmold* mold, char* str)
{
    if (c->WriteThrough)
        Graphic_DisplayString3(c->Area.X+re_x,c->Area.Y+re_y,width,height,valign,halign,bcolor,ccolor,mold,str);
    else
    {
        if (!c->Virtual)
            Graphic_Canvas_DisplayString3(c,re_x,re_y,width,height,valign,halign,bcolor,ccolor,mold,str);
    }
}

void Graphic_Draw_String4(canvas* c, uint16_t re_x, uint16_t re_y, short line, short row, uint8_t valign, uint8_t halign, uint16_t bcolor, uint16_t ccolor, const charmold* mold, char* str)
{
    if (c->WriteThrough)
        Graphic_DisplayString4(c->Area.X+re_x,c->Area.Y+re_y,line,row,valign,halign,bcolor,ccolor,mold,str);
    else
    {
        if (!c->Virtual)
            Graphic_Canvas_DisplayString4(c,re_x,re_y,line,row,valign,halign,bcolor,ccolor,mold,str);
    }
}

















void Graphic_Canvas_Printf(canvas* Canvas, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, uint8_t valign, uint8_t halign, font* f, char* fstr, ...);
void Graphic_Canvas_Printf2(canvas* Canvas, uint16_t re_x, uint16_t re_y, short line, short row, uint8_t valign, uint8_t halign, font* f, char* fstr, ...);
void Graphic_Canvas_Printf3(canvas* Canvas, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, uint8_t valign, uint8_t halign, uint16_t bcolor, uint16_t ccolor, const charmold* mold, char* fstr, ...);
void Graphic_Canvas_Printf4(canvas* Canvas, uint16_t re_x, uint16_t re_y, short line, short row, uint8_t valign, uint8_t halign, uint16_t bcolor, uint16_t ccolor, const charmold* mold, char* fstr, ...);

void _Graphic_Printf(uint16_t ab_x, uint16_t ab_y, uint16_t width, uint16_t height, uint8_t valign, uint8_t halign, font* f, char* fstr, ...);
void _Graphic_Printf2(uint16_t ab_x, uint16_t ab_y, short line, short row, uint8_t valign, uint8_t halign, font* f, char* fstr, ...);
void _Graphic_Printf3(uint16_t ab_x, uint16_t ab_y, uint16_t width, uint16_t height, uint8_t valign, uint8_t halign, uint16_t bcolor, uint16_t ccolor, const charmold* mold, char* fstr, ...);
void _Graphic_Printf4(uint16_t ab_x, uint16_t ab_y, short line, short row, uint8_t valign, uint8_t halign, uint16_t bcolor, uint16_t ccolor, const charmold* mold, char* fstr, ...);


void Graphic_Canvas_Printf(canvas* c, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t valign, uint8_t halign, font* f, char* fstr, ...)
{
    int i;
	va_list val;
	
	va_start(val,fstr);
	i=vsprintf(Print_Buffer0,fstr,val);
    Print_Buffer0[i]='\0';
	Graphic_Canvas_DisplayString(c,x,y,width,height,valign,halign,f,Print_Buffer0);
	va_end(val);
}

void Graphic_Canvas_Printf2(canvas* c, uint16_t x, uint16_t y, short line, short row, uint8_t valign, uint8_t halign, font* f, char* fstr, ...)
{
    int i;
	va_list val;
    
    if (line==INF)
    {
        valign=Top_Align;
        line=c->Area.Height-y-1;
    }
    else
        line=f->Mold->Height*line;
    
    if (row==INF)
    {
        halign=Left_Align;
        row=c->Area.Width-x-1;
    }
    else
        row=f->Mold->Width*row;
	
	va_start(val,fstr);
	i=vsprintf(Print_Buffer0,fstr,val);
    Print_Buffer0[i]='\0';
	Graphic_Canvas_DisplayString(c,x,y,row,line,valign,halign,f,Print_Buffer0);
	va_end(val);
    
}

void Graphic_Canvas_Printf3(canvas* c, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t valign, uint8_t halign, uint16_t bcolor, uint16_t ccolor, const charmold* mold, char* fstr, ...)
{
    int i;
	va_list val;
	font f={bcolor,ccolor,mold};
	
	va_start(val,fstr);
	i=vsprintf(Print_Buffer0,fstr,val);
    Print_Buffer0[i]='\0';
	Graphic_Canvas_DisplayString(c,x,y,width,height,valign,halign,&f,Print_Buffer0);
	va_end(val);
}

void Graphic_Canvas_Printf4(canvas* c, uint16_t x, uint16_t y, short line, short row, uint8_t valign, uint8_t halign, uint16_t bcolor, uint16_t ccolor, const charmold* mold, char* fstr, ...)
{
    int i;
	va_list val;
	font f={bcolor,ccolor,mold};

    if (line==INF)
    {
        valign=Top_Align;
        line=c->Area.Height-y-1;
    }
    else
        line=mold->Height*line;
    
    if (row==INF)
    {
        halign=Left_Align;
        row=c->Area.Width-x-1;
    }
    else
        row=mold->Width*row;
    
	va_start(val,fstr);
	i=vsprintf(Print_Buffer0,fstr,val);
    Print_Buffer0[i]='\0';
	Graphic_Canvas_DisplayString(c,x,y,row,line,valign,halign,&f,Print_Buffer0);
	va_end(val);
}

void _Graphic_Printf(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t valign, uint8_t halign, font* Font, char* fstr, ...)
{
    int i;
	va_list val;
	
	va_start(val,fstr);
	i=vsprintf(Print_Buffer0,fstr,val);
    Print_Buffer0[i]='\0';
	Graphic_DisplayString(x,y,width,height,valign,halign,Font,Print_Buffer0);
	va_end(val);
}

void _Graphic_Printf2(uint16_t x, uint16_t y, short line, short row, uint8_t valign, uint8_t halign, font* f, char* fstr, ...)
{
    int i;
	va_list val;

    if (line==INF)
    {
        valign=Top_Align;
        line=ScreenDevice.Height-y-1;
    }
    else
        line=f->Mold->Height*line;
    
    if (row==INF)
    {
        halign=Left_Align;
        row=ScreenDevice.Width-x-1;
    }
    else
        row=f->Mold->Width*row;
    
	va_start(val,fstr);
	i=vsprintf(Print_Buffer0,fstr,val);
    Print_Buffer0[i]='\0';
	Graphic_DisplayString(x,y,row,line,valign,halign,f,Print_Buffer0);
	va_end(val);
}

void _Graphic_Printf3(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t valign, uint8_t halign, uint16_t bcolor, uint16_t ccolor, const charmold* mold, char* fstr, ...)
{
    int i;
	va_list val;
	font f={bcolor,ccolor,mold};
	
	va_start(val,fstr);
	i=vsprintf(Print_Buffer0,fstr,val);
    Print_Buffer0[i]='\0';
	Graphic_DisplayString(x,y,width,height,valign,halign,&f,Print_Buffer0);
	va_end(val);
}

void _Graphic_Printf4(uint16_t x, uint16_t y, short line, short row, uint8_t valign, uint8_t halign, uint16_t bcolor, uint16_t ccolor, const charmold* mold, char* fstr, ...)
{
    int i;
	va_list val;
	font f={bcolor,ccolor,mold};
    
    if (line==INF)
    {
        valign=Top_Align;
        line=ScreenDevice.Height-y-1;
    }
    else
        line=mold->Height*line;
    
    if (row==INF)
    {
        halign=Left_Align;
        row=ScreenDevice.Width-x-1;
    }
    else
        row=mold->Width*row;
    
	va_start(val,fstr);
	i=vsprintf(Print_Buffer0,fstr,val);
    Print_Buffer0[i]='\0';
	Graphic_DisplayString(x,y,row,line,valign,halign,&f,Print_Buffer0);
	va_end(val);
}

void Graphic_Printf(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, uint8_t valign, uint8_t halign, font* f, char* fstr, ...)
{
    if (c->WriteThrough)
    {
        int i;
        va_list val;
        
        va_start(val,fstr);
        i=vsprintf(Print_Buffer0,fstr,val);
        Print_Buffer0[i]='\0';
        Graphic_DisplayString(c->Area.X+re_x,c->Area.Y+re_y,width,height,valign,halign,f,Print_Buffer0);
        va_end(val);
    }
    else
    {
        if (!c->Virtual)
        {
            int i;
            va_list val;
            
            va_start(val,fstr);
            i=vsprintf(Print_Buffer0,fstr,val);
            Print_Buffer0[i]='\0';
            Graphic_Canvas_DisplayString(c,re_x,re_y,width,height,valign,halign,f,Print_Buffer0);
            va_end(val);
        }
    }
}

void Graphic_Printf2(canvas* c, uint16_t re_x, uint16_t re_y, short line, short row, uint8_t valign, uint8_t halign, font* f, char* fstr, ...)
{
    if (c->WriteThrough)
    {
        int i;
        va_list val;

        if (line==INF)
        {
            valign=Top_Align;
            line=ScreenDevice.Height-re_y-1;
        }
        else
            line=f->Mold->Height*line;
        
        if (row==INF)
        {
            halign=Left_Align;
            row=ScreenDevice.Width-re_x-1;
        }
        else
            row=f->Mold->Width*row;
        
        va_start(val,fstr);
        i=vsprintf(Print_Buffer0,fstr,val);
        Print_Buffer0[i]='\0';
        Graphic_DisplayString(c->Area.X+re_x,c->Area.Y+re_y,row,line,valign,halign,f,Print_Buffer0);
        va_end(val);
    }
    else
    {
        if (!c->Virtual)
        {
            int i;
            va_list val;
            
            if (line==INF)
            {
                valign=Top_Align;
                line=c->Area.Height-re_y-1;
            }
            else
                line=f->Mold->Height*line;
            
            if (row==INF)
            {
                halign=Left_Align;
                row=c->Area.Width-re_x-1;
            }
            else
                row=f->Mold->Width*row;
            
            va_start(val,fstr);
            i=vsprintf(Print_Buffer0,fstr,val);
            Print_Buffer0[i]='\0';
            Graphic_Canvas_DisplayString(c,re_x,re_y,row,line,valign,halign,f,Print_Buffer0);
            va_end(val);
        }
    }
}

void Graphic_Printf3(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, uint8_t valign, uint8_t halign, uint16_t bcolor, uint16_t ccolor, const charmold* mold, char* fstr, ...)
{
    if (c->WriteThrough)
    {
        int i;
        va_list val;
        font f={bcolor,ccolor,mold};
        
        va_start(val,fstr);
        i=vsprintf(Print_Buffer0,fstr,val);
        Print_Buffer0[i]='\0';
        Graphic_DisplayString(c->Area.X+re_x,c->Area.Y+re_y,width,height,valign,halign,&f,Print_Buffer0);
        va_end(val);
    }
    else
    {
        if (!c->Virtual)
        {
            int i;
            va_list val;
            font f={bcolor,ccolor,mold};
            
            va_start(val,fstr);
            i=vsprintf(Print_Buffer0,fstr,val);
            Print_Buffer0[i]='\0';
            Graphic_Canvas_DisplayString(c,re_x,re_y,width,height,valign,halign,&f,Print_Buffer0);
            va_end(val);
    
        }
    }
}

void Graphic_Printf4(canvas* c, uint16_t re_x, uint16_t re_y, short line, short row, uint8_t valign, uint8_t halign, uint16_t bcolor, uint16_t ccolor, const charmold* mold, char* fstr, ...)
{
    if (c->WriteThrough)
    {
        int i;
        va_list val;
        font f={bcolor,ccolor,mold};
        
        if (line==INF)
        {
            valign=Top_Align;
            line=ScreenDevice.Height-re_y-1;
        }
        else
            line=mold->Height*line;
        
        if (row==INF)
        {
            halign=Left_Align;
            row=ScreenDevice.Width-re_x-1;
        }
        else
            row=mold->Width*row;
        
        va_start(val,fstr);
        i=vsprintf(Print_Buffer0,fstr,val);
        Print_Buffer0[i]='\0';
        Graphic_DisplayString(c->Area.X+re_x,c->Area.Y+re_y,row,line,valign,halign,&f,Print_Buffer0);
        va_end(val);
    }
    else
    {
        if (!c->Virtual)
        {
            int i;
            va_list val;
            font f={bcolor,ccolor,mold};

            if (line==INF)
            {
                valign=Top_Align;
                line=c->Area.Height-re_y-1;
            }
            else
                line=mold->Height*line;
            
            if (row==INF)
            {
                halign=Left_Align;
                row=c->Area.Width-re_x-1;
            }
            else
                row=mold->Width*row;
            
            va_start(val,fstr);
            i=vsprintf(Print_Buffer0,fstr,val);
            Print_Buffer0[i]='\0';
            Graphic_Canvas_DisplayString(c,re_x,re_y,row,line,valign,halign,&f,Print_Buffer0);
            va_end(val);
        }
    }
}



