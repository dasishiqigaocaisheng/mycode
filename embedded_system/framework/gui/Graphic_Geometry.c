#include "Graphic_Config.h"
#include "Graphic_DataStructure.h"
#include "MathHelper.h"
#include <stdarg.h>
#include <stdio.h>
#include <math.h>

/**********************************************************************************************************************************************************
 *
 *                                                                  绘制点/区域的宏
 *
 **********************************************************************************************************************************************************/
//在屏幕上(x,y)处绘制一个颜色为color的点
#define Pixel_Draw_Scn(sc, x, y, color) sc->Draw_Pixel(sc, x, y, color)

//在画布上(x,y)处绘制一个颜色为color的点
#define Pixel_Draw_Cnv(c, x, y, color) *CANVAS_GRAM_ADDR(c, x, y) = color

//在屏幕上由(x,y),width,height定义的区域内填充颜色color
#define Pixels_Fill_Scn(sc, x, y, width, height, color) sc->Draw_Area(sc, x, y, width, height, color)

//在画布上由(x,y),width,height定义的区域内填充颜色color
#define Pixels_Fill_Cnv(c, x, y, width, height, color) \
    {                                                  \
        uint16_t *gram = CANVAS_GRAM_ADDR(c, x, y);    \
        for (uint16_t _i = 0; _i < height; _i++)       \
        {                                              \
            for (uint16_t _j = 0; _j < width; _j++)    \
                gram[_j] = color;                      \
            gram += c->Area.Width;                     \
        }                                              \
    }

/**********************************************************************************************************************************************************
 *
 *                                                                      函数声明列表
 *
 **********************************************************************************************************************************************************/
/*=======================================================================对外接口函数=======================================================================*/

void Graphic_Draw_Color(canvas *c, uint16_t color);
void Graphic_Draw_Rectangle_Full(canvas *c, uint16_t x, uint16_t y, rectangle *r);
void Graphic_Draw_Rectangle_FullDirect(canvas *c, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t bcolor, uint16_t thickness, uint16_t fcolor);
void Graphic_Draw_Rectangle_onlyFrame(canvas *c, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t thickness, uint16_t fcolor);
void Graphic_Draw_Rectangle_Direct(canvas *c, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
void Graphic_Draw_Line_Full(canvas *c, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, line *l);
void Graphic_Draw_Line_FullDirect(canvas *c, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t width, uint16_t color, uint8_t slen, uint8_t vlen);
void Graphic_Draw_Circle_Style1(canvas *c, uint16_t x, uint16_t y, uint16_t r, uint16_t color, uint8_t thickness);
void Graphic_Draw_Circle_Style2(canvas *c, uint16_t x, uint16_t y, uint16_t r, uint16_t color, int8_t thickness);
void Graphic_Draw_BMP_Direct(canvas *c, uint16_t x, uint16_t y, BMP *p);
void Graphic_Draw_BMP_inArea(canvas *c, uint16_t x, uint16_t y, uint16_t width, uint16_t height, BMP *p);
void Graphic_Draw_BMP_withSize(canvas *c, uint16_t x, uint16_t y, float size, BMP *p);
void Graphic_Draw_rBMP_Direct(canvas *c, uint16_t x, uint16_t y, rBMP *p);
void Graphic_Draw_rBMP_inArea(canvas *c, uint16_t x, uint16_t y, uint16_t width, uint16_t height, rBMP *p);
void Graphic_Draw_rBMP_withSize(canvas *c, uint16_t x, uint16_t y, float size, rBMP *p);
void Graphic_Draw_pBMP_Direct(canvas *c, uint16_t x, uint16_t y, pBMP *p);
void Graphic_Draw_pBMP_inArea(canvas *c, uint16_t x, uint16_t y, uint16_t width, uint16_t height, pBMP *p);
void Graphic_Draw_pBMP_withSize(canvas *c, uint16_t x, uint16_t y, float size, pBMP *p);
void Graphic_Draw_prBMP_Direct(canvas *c, uint16_t x, uint16_t y, prBMP *p);
void Graphic_Draw_prBMP_inArea(canvas *c, uint16_t x, uint16_t y, uint16_t width, uint16_t height, prBMP *p);
void Graphic_Draw_prBMP_withSize(canvas *c, uint16_t x, uint16_t y, float size, prBMP *p);
void Graphic_Draw_BMP_All_Direct(canvas *c, uint16_t x, uint16_t y, bmp *p);
void Graphic_Draw_BMP_All_inArea(canvas *c, uint16_t x, uint16_t y, uint16_t width, uint16_t height, bmp *p);
void Graphic_Draw_BMP_All_withSize(canvas *c, uint16_t x, uint16_t y, float size, bmp *p);
void Graphic_Draw_Char(canvas *c, uint16_t x, uint16_t y, uint8_t ch, uint16_t bcolor, uint16_t ccolor, const charmold *mold);
void Graphic_Draw_String_inAreaFull(canvas *c, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t valign, uint8_t halign, font *f, const char *str);
void Graphic_Draw_String_inGridFull(canvas *c, uint16_t x, uint16_t y, short line, short row, uint8_t valign, uint8_t halign, font *f, const char *str);
void Graphic_Draw_String_inAreaDirect(canvas *c, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t valign, uint8_t halign, uint16_t bcolor, uint16_t ccolor, const charmold *mold, const char *str);
void Graphic_Draw_String_inGridDirect(canvas *c, uint16_t x, uint16_t y, short line, short row, uint8_t valign, uint8_t halign, uint16_t bcolor, uint16_t ccolor, const charmold *mold, const char *str);
void Graphic_Printf_inAreaFull(canvas *c, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t valign, uint8_t halign, font *f, const char *fstr, ...);
void Graphic_Printf_inGridFull(canvas *c, uint16_t x, uint16_t y, short line, short row, uint8_t valign, uint8_t halign, font *f, const char *fstr, ...);
void Graphic_Printf_inAreaDirect(canvas *c, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t valign, uint8_t halign, uint16_t bcolor, uint16_t ccolor, const charmold *mold, const char *fstr, ...);
void Graphic_Printf_inGridDirect(canvas *c, uint16_t x, uint16_t y, short line, short row, uint8_t valign, uint8_t halign, uint16_t bcolor, uint16_t ccolor, const charmold *mold, const char *fstr, ...);

/*========================================================================内部函数=========================================================================*/

/***********************************************************************************************************************************************************
 *功能：绘制一个由(x,y),width,height定义的实心矩形
 *参数：1.c：画布对象
 *      2.width：矩形宽度
 *      3.height：矩形高度
 *      4.color：矩形的颜色
 *      5.x：矩形左上角x坐标
 *      6.y：矩形左上角y坐标
 *返回：void
 ***********************************************************************************************************************************************************/
static void _Draw_Rectangle(canvas *c, uint16_t width, uint16_t height, uint16_t color, uint16_t x, uint16_t y);

/***********************************************************************************************************************************************************
 *功能：使用Bresenham算法得到由(x1,y1),(x2,y2)定义的线段上各像素的高度
 *参数：1.y_out：储存线段上各个像素的高度
 *      2.x1：线段第一个端点的x坐标
 *      3.y1：线段第一个端点的y坐标
 *      4.x2：线段第二个端点的x坐标
 *      5.y2：线段第二个端点的y坐标
 *返回：void
 ***********************************************************************************************************************************************************/
static void _Bresenham_Get_LineCoordinates(uint16_t *y_out, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

/***********************************************************************************************************************************************************
 *功能：简单绘制一条由端点(x1,y1),(x2,y2)定义的线段，宽度为thickness，颜色为color
 *参数：1.c：画布对象
 *      2.color：线的颜色
 *      3.x1：线段第一个端点的x坐标
 *      4.y1：线段第一个端点的y坐标
 *      5.x2：线段第二个端点的x坐标
 *      6.y2：线段第二个端点的y坐标
 *      7.thickness：线宽
 *返回：void
 ***********************************************************************************************************************************************************/
static void _Draw_Line(canvas *c, uint16_t color, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t thickness);

/***********************************************************************************************************************************************************
 *功能：使用Bresenham算法得到由半径r定义的圆上各像素的高度（45°到90°的部分）
 *参数：1.y_out：储存线段上各个像素的高度
 *      2.r：半径
 *返回：void
 ***********************************************************************************************************************************************************/
static uint16_t _Bresenham_Get_CircleCoordinates(uint16_t *y_out, int r);

/***********************************************************************************************************************************************************
 *功能：字符串预处理（将字符串分行）
 *参数：1.out_buf：处理结果输出
 *      2.str：原始输入字符串
 *      3.width：字符串输出区域宽度
 *      4.height：字符串输入区域高度
 *      5.chmd：字模对象
 *返回：void
 ***********************************************************************************************************************************************************/
static uint16_t _String_Preprocess(char *out_buf, const char *str, uint16_t width, uint16_t height, const charmold *chmd);

/**********************************************************************************************************************************************************
 *
 *                                                                      内部函数定义
 *
 **********************************************************************************************************************************************************/

static void _Draw_Rectangle(canvas *c, uint16_t width, uint16_t height, uint16_t color, uint16_t x, uint16_t y)
{
    if (IS_CANVAS_CACHE_ENABLE(c))
    {
        uint16_t *cache = CANVAS_GRAM_ADDR(c, x, y);
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
                cache[j] = color;
            cache += c->Area.Width;
        }
    }
    else
    {
        if (c->ParentCanvas != NULL)
            _Draw_Rectangle(c->ParentCanvas, width, height, color, x + c->Area.X, y + c->Area.Y);
        else
            c->Screen->Draw_Area(c->Screen, x, y, width, height, color);
    }
}

static void _Bresenham_Get_LineCoordinates(uint16_t *y_out, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    short dx = x2 - x1, dy = y2 - y1;
    short d = (dy << 1) - dx;
    uint16_t x, y;

    for (x = x1, y = y1; x != x2; x++)
    {
        if (d < 0)
            d += dy << 1;
        else
        {
            d += (dy - dx) << 1;
            y++;
        }
        y_out[x - x1] = y;
    }
}

static void _Draw_Line(canvas *c, uint16_t color, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t thickness)
{
    if (thickness == 0)
        return;

    uint8_t thickness_half = thickness / 2;
    uint16_t *y_buf = (uint16_t *)c->Screen->Rasterization_Buffer;

    if (x1 >= c->Area.Width)
        x1 = c->Area.Width - 1;
    if (x2 >= c->Area.Width)
        x2 = c->Area.Width - 1;
    if (y1 >= c->Area.Height)
        y1 = c->Area.Height - 1;
    if (y2 >= c->Area.Height)
        y2 = c->Area.Height - 1;

    if (x2 >= x1)
    {
        if (IS_CANVAS_CACHE_ENABLE(c))
        {
            if (y2 > y1)
            {
                if (y2 - y1 <= x2 - x1)
                {
                    _Bresenham_Get_LineCoordinates(y_buf, 0, 0, x2 - x1, y2 - y1);
                    for (int i = x1; i != x2; i++)
                        Pixels_Fill_Cnv(c, i, y_buf[i - x1] + y1 - thickness_half, 1, thickness, color);
                }
                else
                {
                    _Bresenham_Get_LineCoordinates(y_buf, 0, 0, y2 - y1, x2 - x1);
                    for (int i = y1; i != y2; i++)
                        Pixels_Fill_Cnv(c, y_buf[i - y1] + x1 - thickness_half, i, thickness, 1, color);
                }
            }
            else
            {
                if (y1 - y2 <= x2 - x1)
                {
                    _Bresenham_Get_LineCoordinates(y_buf, 0, 0, x2 - x1, y1 - y2);
                    for (int i = x1; i != x2; i++)
                        Pixels_Fill_Cnv(c, i, y1 - y_buf[i - x1] - thickness_half, 1, thickness, color);
                }
                else
                {
                    _Bresenham_Get_LineCoordinates(y_buf, 0, 0, y1 - y2, x2 - x1);
                    for (int i = y1; i != y2; i--)
                        Pixels_Fill_Cnv(c, y_buf[y1 - i] + x1 - thickness_half, i, thickness, 1, color);
                }
            }
        }
        else
        {
            if (c->ParentCanvas != NULL)
                _Draw_Line(c->ParentCanvas, color, x1 + c->Area.X, y1 + c->Area.Y, x2 + c->Area.X, y2 + c->Area.Y, thickness);
            else
            {
                if (x2 >= x1)
                {
                    if (y2 > y1)
                    {
                        if (y2 - y1 <= x2 - x1)
                        {
                            _Bresenham_Get_LineCoordinates(y_buf, 0, 0, x2 - x1, y2 - y1);
                            for (int i = x1; i != x2; i++)
                                Pixels_Fill_Scn(c->Screen, i, y_buf[i - x1] + y1 - thickness_half, 1, thickness, color);
                        }
                        else
                        {
                            _Bresenham_Get_LineCoordinates(y_buf, 0, 0, y2 - y1, x2 - x1);
                            for (int i = y1; i != y2; i++)
                                Pixels_Fill_Scn(c->Screen, y_buf[i - y1] + x1 - thickness_half, i, thickness, 1, color);
                        }
                    }
                    else
                    {
                        if (y1 - y2 <= x2 - x1)
                        {
                            _Bresenham_Get_LineCoordinates(y_buf, 0, 0, x2 - x1, y1 - y2);
                            for (int i = x1; i != x2; i++)
                                Pixels_Fill_Scn(c->Screen, i, y1 - y_buf[i - x1] - thickness_half, 1, thickness, color);
                        }
                        else
                        {
                            _Bresenham_Get_LineCoordinates(y_buf, 0, 0, y1 - y2, x2 - x1);
                            for (int i = y1; i != y2; i--)
                                Pixels_Fill_Scn(c->Screen, y_buf[y1 - i] + x1 - thickness_half, i, thickness, 1, color);
                        }
                    }
                }
            }
        }
    }
    else
    {
        if (y2 > y1)
        {
            if (y2 - y1 <= x1 - x2)
                _Draw_Line(c, color, x2, y2, x1, y1, thickness);
            else
                _Draw_Line(c, color, x2, y2, x1, y1, thickness);
        }
        else
        {
            if (y1 - y2 <= x1 - x2)
                _Draw_Line(c, color, x2, y2, x1, y1, thickness);
            else
                _Draw_Line(c, color, x2, y2, x1, y1, thickness);
        }
    }
}

static uint16_t _Bresenham_Get_CircleCoordinates(uint16_t *y_out, int r)
{
    if (r < 0)
        r = 0;
    short d = 3 - (r << 1);
    short x, y;

    for (x = 0, y = r; x <= y; x++)
    {
        y_out[x] = y;
        if (d < 0)
            d += (x << 2) + 6;
        else
        {
            d += ((x - y) << 2) + 10;
            y--;
        }
    }
    return x;
}

static uint16_t _String_Preprocess(char *out_buf, const char *str, uint16_t width, uint16_t height, const charmold *chmd)
{
    uint16_t row_num = 1;
    uint16_t row_x = 0, column_y = 0;
    uint16_t out_buf_idx = 0;
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == '\n')
        {
            if ((column_y += chmd->Height) <= height)
            {
                row_x = 0;
                out_buf[out_buf_idx++] = '\n';
                row_num++;
            }
        }
        else if (str[i] == '\t')
        {
        TAB_JUDGE:
            if ((row_x += chmd[' '].Width * TAB_SIZE) <= width)
            {
                if (SPACE_TAB)
                {
                    for (int j = 0; j < TAB_SIZE; j++)
                        out_buf[out_buf_idx++] = ' ';
                }
                else
                    out_buf[out_buf_idx++] = '\t';
                row_x += chmd[' '].Width * TAB_SIZE;
            }
            else if ((column_y += chmd->Height) <= height)
            {
                out_buf[out_buf_idx++] = '\n';
                row_x = 0;
                row_num++;
                goto TAB_JUDGE;
            }
            else
                goto LOOP_END;
        }
        else
        {
        DEFAULT_JUDGE:
            if ((row_x += chmd[str[i]].Width) <= width)
                out_buf[out_buf_idx++] = str[i];
            else if ((column_y += chmd->Height) <= height)
            {
                out_buf[out_buf_idx++] = '\n';
                row_x = 0;
                row_num++;
                goto DEFAULT_JUDGE;
            }
            else
                goto LOOP_END;
        }
    }
LOOP_END:
    out_buf[out_buf_idx] = '\0';
    return row_num;
}

/**********************************************************************************************************************************************************
 *
 *                                                                      接口函数定义
 *
 **********************************************************************************************************************************************************/

void Graphic_Draw_Color(canvas *c, uint16_t color)
{
    if (IS_CANVAS_CACHE_ENABLE(c))
    {
        uint16_t *cache = c->Address;
        for (int i = 0; i < c->Area.Height; i++)
        {
            for (int j = 0; j < c->Area.Width; j++)
                cache[j] = color;
            cache += c->Area.Width;
        }
    }
    else
    {
        if (c->ParentCanvas != NULL)
            _Draw_Rectangle(c->ParentCanvas, c->Area.Width, c->Area.Height, color, c->Area.X, c->Area.Y);
        else
            c->Screen->Draw_Area(c->Screen, c->Area.X, c->Area.Y, c->Area.Width, c->Area.Height, color);
    }
}

void Graphic_Draw_Rectangle_Full(canvas *Canvas, uint16_t x, uint16_t y, rectangle *r)
{
    uint16_t width = r->Width, height = r->Height;

    if (x + width > Canvas->Area.Width)
        width = Canvas->Area.Width - x;
    if (y + height > Canvas->Area.Height)
        height = Canvas->Area.Height - y;

    if (r->Frame_Thickness != 0)
    {
        if ((r->Frame_Thickness >= width / 2) || (r->Frame_Thickness >= height / 2))
        {
            if (r->Display_Background)
                _Draw_Rectangle(Canvas, width, height, r->Frame_Color, x, y);
            return;
        }
        else
        {
            _Draw_Rectangle(Canvas, width, r->Frame_Thickness, r->Frame_Color, x, y);
            _Draw_Rectangle(Canvas, width, r->Frame_Thickness, r->Frame_Color,
                            x, y + height - r->Frame_Thickness);
            _Draw_Rectangle(Canvas, r->Frame_Thickness, height - 2 * r->Frame_Thickness,
                            r->Frame_Color, x, y + r->Frame_Thickness);
            _Draw_Rectangle(Canvas, r->Frame_Thickness, height - 2 * r->Frame_Thickness,
                            r->Frame_Color, x + width - r->Frame_Thickness, y + r->Frame_Thickness);
            if (r->Display_Background)
                _Draw_Rectangle(Canvas, width - 2 * r->Frame_Thickness, height - 2 * r->Frame_Thickness,
                                r->Background_Color, x + r->Frame_Thickness, y + r->Frame_Thickness);
        }
    }
    else
    {
        if (r->Display_Background)
            _Draw_Rectangle(Canvas, width, height, r->Background_Color, x, y);
    }
}

void Graphic_Draw_Rectangle_FullDirect(canvas *c, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t bcolor, uint16_t thickness, uint16_t fcolor)
{
    if (x + width > c->Area.Width)
        width = c->Area.Width - x;
    if (y + height > c->Area.Height)
        height = c->Area.Height - y;

    if (thickness == 0)
        return;

    if ((thickness >= width / 2) || (thickness >= height / 2))
        _Draw_Rectangle(c, width, height, fcolor, x, y);
    else
    {
        _Draw_Rectangle(c, width, thickness, fcolor, x, y);
        _Draw_Rectangle(c, width, thickness, fcolor,
                        x, y + height - thickness);
        _Draw_Rectangle(c, thickness, height - 2 * thickness,
                        fcolor, x, y + thickness);
        _Draw_Rectangle(c, thickness, height - 2 * thickness,
                        fcolor, x + width - thickness, y + thickness);
        _Draw_Rectangle(c, width - 2 * thickness, height - 2 * thickness, fcolor, x + thickness, y + thickness);
    }
}

void Graphic_Draw_Rectangle_onlyFrame(canvas *c, uint16_t X, uint16_t Y, uint16_t width, uint16_t height, uint16_t thickness, uint16_t fcolor)
{
    if (X + width > c->Area.Width)
        width = c->Area.Width - X;
    if (Y + height > c->Area.Height)
        height = c->Area.Height - Y;

    if (thickness == 0)
        return;

    if ((thickness >= width / 2) || (thickness >= height / 2))
        _Draw_Rectangle(c, width, height, fcolor, X, Y);
    else
    {
        _Draw_Rectangle(c, width, thickness, fcolor, X, Y);
        _Draw_Rectangle(c, width, thickness, fcolor,
                        X, Y + height - thickness);
        _Draw_Rectangle(c, thickness, height - 2 * thickness,
                        fcolor, X, Y + thickness);
        _Draw_Rectangle(c, thickness, height - 2 * thickness,
                        fcolor, X + width - thickness, Y + thickness);
    }
}

void Graphic_Draw_Rectangle_Direct(canvas *c, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
{
    _Draw_Rectangle(c, width, height, color, x, y);
}

void Graphic_Draw_Line_Full(canvas *c, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, line *l)
{
    uint16_t i, j;
    float Div_X1, Div_Y1, Div_X2, Div_Y2;
    float Degree, x, y;

    if ((x2 == x1) && (y2 == y1))
        return;

    if (l->Vacancy_Length == 0)
    {
        _Draw_Line(c, l->Color, x1, y1, x2, y2, l->Width);
        return;
    }

    if (x2 == x1)
    {
        Div_X1 = 0;
        Div_X2 = 0;
        if (y1 > y2)
        {
            Div_Y1 = -l->Solid_Length;
            Div_Y2 = -l->Vacancy_Length;
        }
        else
        {
            Div_Y1 = l->Solid_Length;
            Div_Y2 = l->Vacancy_Length;
        }
    }
    else if (x2 > x1)
    {
        Degree = atan2f(y2 - y1, x2 - x1);
        Div_X1 = l->Solid_Length * cosf(Degree);
        Div_Y1 = l->Solid_Length * sinf(Degree);
        Div_X2 = l->Vacancy_Length * cosf(Degree);
        Div_Y2 = l->Vacancy_Length * sinf(Degree);
    }
    else
    {
        Graphic_Draw_Line_Full(c, x2, y2, x1, y1, l);
        return;
    }

    if (x2 != x1)
        i = MATHHELPER_ABS(x2 - x1) / MATHHELPER_ABS(Div_X1 + Div_X2);
    else
        i = MATHHELPER_ABS(y2 - y1) / MATHHELPER_ABS(Div_Y1 + Div_Y2);
    for (j = 0, x = x1, y = y1; j < i; j++)
    {
        _Draw_Line(c, l->Color, x, y, x + Div_X1, y + Div_Y1, l->Width);
        x += Div_X1 + Div_X2;
        y += Div_Y1 + Div_Y2;
    }

    if (MATHHELPER_ABS(x2 - x) > MATHHELPER_ABS(Div_X1))
        _Draw_Line(c, l->Color, x, y, x + Div_X1, y + Div_Y1, l->Width);
    else if (MATHHELPER_ABS(x2 - x) < MATHHELPER_ABS(Div_X1))
        _Draw_Line(c, l->Color, x, y, x2, y2, l->Width);
    else if (MATHHELPER_ABS(y2 - y) > MATHHELPER_ABS(Div_Y1))
        _Draw_Line(c, l->Color, x, y, x + Div_X1, y + Div_Y1, l->Width);
    else if (MATHHELPER_ABS(y2 - y) < MATHHELPER_ABS(Div_Y1))
        _Draw_Line(c, l->Color, x, y, x2, y2, l->Width);
}

void Graphic_Draw_Line_FullDirect(canvas *c, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t width, uint16_t color, uint8_t slen, uint8_t vlen)
{
    line l = {width, color, slen, vlen};
    Graphic_Draw_Line_Full(c, x1, y1, x2, y2, &l);
}

void Graphic_Draw_Circle_Style1(canvas *c, uint16_t x, uint16_t y, uint16_t r, uint16_t color, uint8_t thickness)
{
    if (thickness == 0)
        return;
    else if (thickness > r * 2)
        thickness = r * 2;
    if (IS_CANVAS_CACHE_ENABLE(c))
    {
        uint16_t len1, len2;

        uint16_t *datbuf1 = (uint16_t *)c->Screen->Rasterization_Buffer;
        uint16_t *datbuf2 = (uint16_t *)((uint32_t)c->Screen->Rasterization_Buffer + c->Screen->Rasterization_Buffer_Size / 2);

        len1 = _Bresenham_Get_CircleCoordinates(datbuf1, r - thickness / 2);

        if (thickness == 1)
        {
            for (int i = 0; i < len1; i++)
                datbuf2[i] = datbuf1[i];
            len2 = len1;
        }
        else
        {
            len2 = _Bresenham_Get_CircleCoordinates(datbuf2, r + thickness / 2 - 1);
            for (int i = 0; i < len2 - len1; i++)
                datbuf1[len1 + i] = datbuf1[len1 - 1];
        }

        for (uint16_t i = 0; i < len2; i++)
        {
            Pixels_Fill_Cnv(c, x + i, y - datbuf2[i], 1, datbuf2[i] - datbuf1[i] + 1, color); // 45~90
            Pixels_Fill_Cnv(c, x + datbuf1[i], y - i, datbuf2[i] - datbuf1[i] + 1, 1, color); // 0~45
            Pixels_Fill_Cnv(c, x + i, y + datbuf1[i], 1, datbuf2[i] - datbuf1[i] + 1, color); //-45~-90
            Pixels_Fill_Cnv(c, x + datbuf1[i], y + i, datbuf2[i] - datbuf1[i] + 1, 1, color); // 0~-45

            Pixels_Fill_Cnv(c, x - i, y - datbuf2[i], 1, datbuf2[i] - datbuf1[i] + 1, color); // 90~135
            Pixels_Fill_Cnv(c, x - datbuf2[i], y - i, datbuf2[i] - datbuf1[i] + 1, 1, color); // 135~180
            Pixels_Fill_Cnv(c, x - i, y + datbuf1[i], 1, datbuf2[i] - datbuf1[i] + 1, color); //-90~-135
            Pixels_Fill_Cnv(c, x - datbuf2[i], y + i, datbuf2[i] - datbuf1[i] + 1, 1, color); //-135~-180
        }
    }
    else
    {
        if (c->ParentCanvas != NULL)
            Graphic_Draw_Circle_Style1(c->ParentCanvas, x + c->Area.X, y + c->Area.Y, r, color, thickness);
        else
        {
            uint16_t len1, len2;

            uint16_t *datbuf1 = (uint16_t *)c->Screen->Rasterization_Buffer;
            uint16_t *datbuf2 = (uint16_t *)((uint32_t)c->Screen->Rasterization_Buffer + c->Screen->Rasterization_Buffer_Size / 2);

            len1 = _Bresenham_Get_CircleCoordinates(datbuf1, r - thickness / 2);

            if (thickness == 1)
            {
                for (int i = 0; i < len1; i++)
                    datbuf2[i] = datbuf1[i];
                len2 = len1;
            }
            else
            {
                len2 = _Bresenham_Get_CircleCoordinates(datbuf2, r + thickness / 2 - 1);
                for (int i = 0; i < len2 - len1; i++)
                    datbuf1[len1 + i] = datbuf1[len1 - 1];
            }

            for (uint16_t i = 0; i < len2; i++)
            {
                Pixels_Fill_Scn(c->Screen, x + i, y - datbuf2[i], 1, datbuf2[i] - datbuf1[i] + 1, color); // 45~90
                Pixels_Fill_Scn(c->Screen, x + datbuf1[i], y - i, datbuf2[i] - datbuf1[i] + 1, 1, color); // 0~45
                Pixels_Fill_Scn(c->Screen, x + i, y + datbuf1[i], 1, datbuf2[i] - datbuf1[i] + 1, color); //-45~-90
                Pixels_Fill_Scn(c->Screen, x + datbuf1[i], y + i, datbuf2[i] - datbuf1[i] + 1, 1, color); // 0~-45

                Pixels_Fill_Scn(c->Screen, x - i, y - datbuf2[i], 1, datbuf2[i] - datbuf1[i] + 1, color); // 90~135
                Pixels_Fill_Scn(c->Screen, x - datbuf2[i], y - i, datbuf2[i] - datbuf1[i] + 1, 1, color); // 135~180
                Pixels_Fill_Scn(c->Screen, x - i, y + datbuf1[i], 1, datbuf2[i] - datbuf1[i] + 1, color); //-90~-135
                Pixels_Fill_Scn(c->Screen, x - datbuf2[i], y + i, datbuf2[i] - datbuf1[i] + 1, 1, color); //-135~-180
            }
        }
    }
}

void Graphic_Draw_Circle_Style2(canvas *c, uint16_t x, uint16_t y, uint16_t r, uint16_t color, int8_t thickness)
{
    if (thickness == 0)
        return;

    if (IS_CANVAS_CACHE_ENABLE(c))
    {
        uint16_t len1, len2;

        uint16_t *datbuf1 = (uint16_t *)c->Screen->Rasterization_Buffer;
        uint16_t *datbuf2 = ((uint16_t *)c->Screen->Rasterization_Buffer) + c->Screen->Rasterization_Buffer_Size / 4;

        if (thickness == 0)
            Graphic_Draw_Circle_Style1(c, x, y, r, color, 1);
        else if (thickness > 0)
        {
            len1 = _Bresenham_Get_CircleCoordinates(datbuf1, r);
            len2 = _Bresenham_Get_CircleCoordinates(datbuf2, r + thickness);
        }
        else
        {
            len1 = _Bresenham_Get_CircleCoordinates(datbuf1, r + thickness);
            len2 = _Bresenham_Get_CircleCoordinates(datbuf2, r);
        }
        for (int i = 0; i < len2 - len1; i++)
            datbuf1[len1 + i] = datbuf1[len1 - 1];
        for (uint16_t i = 0; i < len2; i++)
        {
            Pixels_Fill_Cnv(c, x + i, y - datbuf2[i], 1, datbuf2[i] - datbuf1[i] + 1, color); // 45~90
            Pixels_Fill_Cnv(c, x + datbuf1[i], y - i, datbuf2[i] - datbuf1[i] + 1, 1, color); // 0~45
            Pixels_Fill_Cnv(c, x + i, y + datbuf1[i], 1, datbuf2[i] - datbuf1[i] + 1, color); //-45~-90
            Pixels_Fill_Cnv(c, x + datbuf1[i], y + i, datbuf2[i] - datbuf1[i] + 1, 1, color); // 0~-45

            Pixels_Fill_Cnv(c, x - i, y - datbuf2[i], 1, datbuf2[i] - datbuf1[i] + 1, color); // 90~135
            Pixels_Fill_Cnv(c, x - datbuf2[i], y - i, datbuf2[i] - datbuf1[i] + 1, 1, color); // 135~180
            Pixels_Fill_Cnv(c, x - i, y + datbuf1[i], 1, datbuf2[i] - datbuf1[i] + 1, color); //-90~-135
            Pixels_Fill_Cnv(c, x - datbuf2[i], y + i, datbuf2[i] - datbuf1[i] + 1, 1, color); //-135~-180
        }
    }
    else
    {
        if (c->ParentCanvas != NULL)
            Graphic_Draw_Circle_Style1(c->ParentCanvas, x + c->Area.X, y + c->Area.Y, r, color, thickness);
        else
        {
            uint16_t len1, len2;

            uint16_t *datbuf1 = (uint16_t *)c->Screen->Rasterization_Buffer;
            uint16_t *datbuf2 = ((uint16_t *)c->Screen->Rasterization_Buffer) + c->Screen->Rasterization_Buffer_Size / 4;

            if (thickness == 0)
                Graphic_Draw_Circle_Style1(c, x, y, r, color, 1);
            else if (thickness > 0)
            {
                len1 = _Bresenham_Get_CircleCoordinates(datbuf1, r);
                len2 = _Bresenham_Get_CircleCoordinates(datbuf2, r + thickness);
            }
            else
            {
                len1 = _Bresenham_Get_CircleCoordinates(datbuf1, r - thickness);
                len2 = _Bresenham_Get_CircleCoordinates(datbuf2, r);
            }
            for (int i = 0; i < len2 - len1; i++)
                datbuf1[len1 + i] = datbuf1[len1 - 1];
            for (uint16_t i = 0; i < len2; i++)
            {
                Pixels_Fill_Scn(c->Screen, x + i, y - datbuf2[i], 1, datbuf2[i] - datbuf1[i] + 1, color); // 45~90
                Pixels_Fill_Scn(c->Screen, x + datbuf1[i], y - i, datbuf2[i] - datbuf1[i] + 1, 1, color); // 0~45
                Pixels_Fill_Scn(c->Screen, x + i, y + datbuf1[i], 1, datbuf2[i] - datbuf1[i] + 1, color); //-45~-90
                Pixels_Fill_Scn(c->Screen, x + datbuf1[i], y + i, datbuf2[i] - datbuf1[i] + 1, 1, color); // 0~-45

                Pixels_Fill_Scn(c->Screen, x - i, y - datbuf2[i], 1, datbuf2[i] - datbuf1[i] + 1, color); // 90~135
                Pixels_Fill_Scn(c->Screen, x - datbuf2[i], y - i, datbuf2[i] - datbuf1[i] + 1, 1, color); // 135~180
                Pixels_Fill_Scn(c->Screen, x - i, y + datbuf1[i], 1, datbuf2[i] - datbuf1[i] + 1, color); //-90~-135
                Pixels_Fill_Scn(c->Screen, x - datbuf2[i], y + i, datbuf2[i] - datbuf1[i] + 1, 1, color); //-135~-180
            }
        }
    }
}

void Graphic_Draw_BMP_Direct(canvas *c, uint16_t x, uint16_t y, BMP *p)
{
    if (IS_CANVAS_CACHE_ENABLE(c))
    {
        uint16_t *gram_addr = CANVAS_GRAM_ADDR(c, x, y);
        uint16_t *bmp_addr = p->Colors;
        for (int i = 0; i < p->Height; i++)
        {
            for (int j = 0; j < p->Width; j++)
                gram_addr[j] = bmp_addr[j];
            gram_addr += c->Area.Width;
            bmp_addr += p->Width;
        }
    }
    else
    {
        if (c->ParentCanvas == NULL)
            c->Screen->GRAM_Transfer(c->Screen, x, y, p->Width, p->Height, p->Colors);
        else
            Graphic_Draw_BMP_Direct(c->ParentCanvas, x + c->Area.X, y + c->Area.Y, p);
    }
}

void Graphic_Draw_BMP_inArea(canvas *c, uint16_t x, uint16_t y, uint16_t width, uint16_t height, BMP *p)
{
    if (width == 0 || height == 0)
        return;
    if (width == p->Width && height == p->Height)
        Graphic_Draw_BMP_Direct(c, x, y, p);
    else
    {
        uint16_t *bmp_addr;
        uint16_t *width_interpolation = (uint16_t *)c->Screen->Rasterization_Buffer;
        uint16_t *height_interpolation = ((uint16_t *)c->Screen->Rasterization_Buffer) + c->Screen->Rasterization_Buffer_Size / 4;

        //计算行、列的内插
        _Bresenham_Get_LineCoordinates(width_interpolation, 0, 0, width > p->Width ? width : p->Width, width < p->Width ? width : p->Width);
        _Bresenham_Get_LineCoordinates(height_interpolation, 0, 0, height > p->Height ? height : p->Height, height < p->Height ? height : p->Height);

        //如果宽度缩小，则需要抽取
        if (width < p->Width)
        {
            uint16_t temp = width_interpolation[0];
            for (uint16_t i = 1, j = 1; i < width; i++, j++)
            {
                while (temp == width_interpolation[j])
                    j++;
                width_interpolation[i] = j;
                temp = width_interpolation[j];
            }
        }
        else
        {
            for (int i = 0; i < width; i++)
                width_interpolation[i] = width_interpolation[i] - 1;
        }
        //如果高度缩小，则需要抽取
        if (height < p->Height)
        {
            uint16_t temp = height_interpolation[0];
            for (uint16_t i = 1, j = 1; i < height; i++, j++)
            {
                while (temp == height_interpolation[j])
                    j++;
                height_interpolation[i] = j;
                temp = height_interpolation[j];
            }
        }
        else
        {
            for (int i = 0; i < height; i++)
                height_interpolation[i] = height_interpolation[i] - 1;
        }

        if (IS_CANVAS_CACHE_ENABLE(c))
        {
            uint16_t *canv_addr = CANVAS_GRAM_ADDR(c, x, y);
            for (int i = 0; i < height; i++)
            {
                bmp_addr = p->Colors + height_interpolation[i] * p->Width;
                for (int j = 0; j < width; j++)
                    canv_addr[j] = bmp_addr[width_interpolation[j]];
                canv_addr += c->Area.Width;
            }
        }
        else
        {
            if (c->ParentCanvas != NULL)
                Graphic_Draw_BMP_inArea(c->ParentCanvas, x + c->Area.X, y + c->Area.Y, width, height, p);
            else
            {
                for (int i = 0; i < height; i++)
                {
                    bmp_addr = p->Colors + height_interpolation[i] * p->Width;
                    for (int j = 0; j < width; j++)
                        c->Screen->Draw_Pixel(c->Screen, x + j, y + i, bmp_addr[width_interpolation[j]]);
                }
            }
        }
    }
}

void Graphic_Draw_BMP_withSize(canvas *c, uint16_t x, uint16_t y, float size, BMP *p)
{
    Graphic_Draw_BMP_inArea(c, x, y, p->Width * size, p->Height * size, p);
}

void Graphic_Draw_rBMP_Direct(canvas *c, uint16_t x, uint16_t y, rBMP *p)
{
    uint16_t i;
    uint16_t Start_Line;              //解码（也是绘制）的起始行
    uint16_t Max_Line, Remained_Line; //可以一次绘制的最大行数，剩余不足最大行数的行数

    uint16_t MaxLine_DrawTime;

    BMP temp_p = {c->Area.Width, c->Area.Height, (uint16_t *)c->Screen->BMPDecoder_Buffer};

    Max_Line = (c->Screen->BMPDecoder_Buffer_Size / 2) / p->Width;

    MaxLine_DrawTime = p->Height / Max_Line;
    Remained_Line = p->Height % Max_Line;

    //先按照最大行数绘制
    for (i = 0, Start_Line = 0; i < MaxLine_DrawTime; i++, Start_Line += Max_Line)
    {
        rBMP2BMP_Decode(p, &temp_p, Start_Line, Max_Line);
        Graphic_Draw_BMP_Direct(c, x, y + Start_Line, &temp_p);
    }

    //绘制不足的那部分
    rBMP2BMP_Decode(p, &temp_p, Start_Line, Remained_Line);
    temp_p.Height = Remained_Line;
    Graphic_Draw_BMP_Direct(c, x, y + Start_Line, &temp_p);
}

void Graphic_Draw_rBMP_inArea(canvas *c, uint16_t x, uint16_t y, uint16_t width, uint16_t height, rBMP *p)
{
    if (width == 0 || height == 0)
        return;
    uint16_t Start_Index, End_Index; // rBMP要解码部分起始行，结束行
    float Start_Line, End_Line;      //画布要画部分起始行，结束行
    uint16_t Max_Line;               //单次最大解码行数
    float Delta_Line;                // Max_Line对应的绘制的行数

    BMP save = {width, height, (uint16_t *)c->Screen->BMPDecoder_Buffer};

    Max_Line = (c->Screen->BMPDecoder_Buffer_Size / 2) / p->Width;

    //如果可以一次解码
    if (Max_Line >= p->Height)
    {
        rBMP2BMP_Decode(p, &save, 0, p->Height);
        Graphic_Draw_BMP_inArea(c, x, y, width, height, &save);
    }
    else
    {
        //恰好不需要分批次绘制
        if ((width == p->Width) && (height == p->Height))
            Graphic_Draw_rBMP_Direct(c, x, y, p);
        else
        {
            Delta_Line = (float)Max_Line * height / p->Height;

            Start_Index = 0;
            End_Index = Max_Line - 1;
            //首先按最大行数绘制
            for (Start_Line = y; End_Index <= p->Height - 1; Start_Index += Max_Line, End_Index += Max_Line)
            {
                End_Line = MathHelper_Round(Start_Line + Delta_Line);

                rBMP2BMP_Decode(p, &save, Start_Index, Max_Line);

                Graphic_Draw_BMP_inArea(c, x, MathHelper_Round(Start_Line), width, MathHelper_Round(End_Line - Start_Line), &save);
                Start_Line = End_Line;
            }

            //绘制剩余的
            if (Start_Index < p->Height)
            {
                if (Start_Line > y + height - 1)
                    return;

                End_Index = p->Height - 1;
                End_Line = y + height - 1;

                rBMP2BMP_Decode(p, &save, Start_Index, End_Index - Start_Index + 1);
                Graphic_Draw_BMP_inArea(c, x, MathHelper_Round(Start_Line), width, MathHelper_Round(End_Line - Start_Line + 1), &save);
            }
            else
                return;
        }
    }
}

void Graphic_Draw_rBMP_withSize(canvas *c, uint16_t x, uint16_t y, float size, rBMP *p)
{
    Graphic_Draw_rBMP_inArea(c, x, y, MathHelper_Round(p->Width * size), MathHelper_Round(p->Height * size), p);
}

void Graphic_Draw_pBMP_Direct(canvas *c, uint16_t x, uint16_t y, pBMP *p)
{
    uint16_t i;
    uint16_t Start_Line;              //解码（也是绘制）的起始行
    uint16_t Max_Line, Remained_Line; //可以一次绘制的最大行数，剩余不足最大行数的行数

    uint16_t MaxLine_DrawTime;

    BMP save = {c->Area.Width, c->Area.Height, (uint16_t *)c->Screen->BMPDecoder_Buffer};

    Max_Line = (c->Screen->BMPDecoder_Buffer_Size / 2) / p->Width;

    MaxLine_DrawTime = p->Height / Max_Line;
    Remained_Line = p->Height % Max_Line;

    //先按照最大行数绘制
    for (i = 0, Start_Line = 0; i < MaxLine_DrawTime; i++, Start_Line += Max_Line)
    {
        pBMP2BMP_Decode(p, &save, Start_Line, Max_Line);
        Graphic_Draw_BMP_Direct(c, x, y + Start_Line, &save);
    }

    //绘制不足的那部分
    pBMP2BMP_Decode(p, &save, Start_Line, Remained_Line);
    save.Height = Remained_Line;
    Graphic_Draw_BMP_Direct(c, x, y + Start_Line, &save);
}

void Graphic_Draw_pBMP_inArea(canvas *c, uint16_t x, uint16_t y, uint16_t width, uint16_t height, pBMP *p)
{
    if (width == 0 || height == 0)
        return;

    uint16_t Start_Index, End_Index;
    float Start_Line, End_Line;
    uint16_t Max_Line;
    float Delta_Line;

    BMP save;

    save.Colors = (uint16_t *)c->Screen->BMPDecoder_Buffer;
    Max_Line = (c->Screen->BMPDecoder_Buffer_Size / 2) / p->Width;

    if (Max_Line >= p->Height)
    {
        pBMP2BMP_Decode(p, &save, 0, p->Height);
        Graphic_Draw_BMP_inArea(c, x, y, width, height, &save);
    }
    else
    {
        if ((width == p->Width) && (height == p->Height))
            Graphic_Draw_pBMP_Direct(c, x, y, p);
        else
        {
            Delta_Line = (float)Max_Line * height / p->Height;

            Start_Index = 0;
            End_Index = Max_Line - 1;
            for (Start_Line = y; End_Index <= p->Height - 1; Start_Index += Max_Line, End_Index += Max_Line)
            {
                End_Line = MathHelper_Round(Start_Line + Delta_Line);
                pBMP2BMP_Decode(p, &save, Start_Index, Max_Line);
                Graphic_Draw_BMP_inArea(c, x, MathHelper_Round(Start_Line), width, MathHelper_Round(End_Line - Start_Line), &save);
                Start_Line = End_Line;
            }

            if (Start_Index < p->Height)
            {
                if (Start_Line > y + height - 1)
                    return;

                End_Index = p->Height - 1;
                End_Line = y + height - 1;

                pBMP2BMP_Decode(p, &save, Start_Index, End_Index - Start_Index);
                Graphic_Draw_BMP_inArea(c, x, MathHelper_Round(Start_Line), width, MathHelper_Round(End_Line - Start_Line), &save);
            }
            else
                return;
        }
    }
}

void Graphic_Draw_pBMP_withSize(canvas *c, uint16_t x, uint16_t y, float size, pBMP *p)
{
    Graphic_Draw_pBMP_inArea(c, x, y, MathHelper_Round(p->Width * size), MathHelper_Round(p->Height * size), p);
}

void Graphic_Draw_prBMP_Direct(canvas *c, uint16_t x, uint16_t y, prBMP *p)
{
    uint16_t i;
    uint16_t Start_Line;              //解码（也是绘制）的起始行
    uint16_t Max_Line, Remained_Line; //可以一次绘制的最大行数，剩余不足最大行数的行数

    uint16_t MaxLine_DrawTime;

    BMP save = {c->Area.Width, c->Area.Height, (uint16_t *)c->Screen->BMPDecoder_Buffer};

    Max_Line = (c->Screen->BMPDecoder_Buffer_Size / 2) / p->Width;

    MaxLine_DrawTime = p->Height / Max_Line;
    Remained_Line = p->Height % Max_Line;

    //先按照最大行数绘制
    for (i = 0, Start_Line = 0; i < MaxLine_DrawTime; i++, Start_Line += Max_Line)
    {
        prBMP2BMP_Decode(p, &save, Start_Line, Max_Line);
        Graphic_Draw_BMP_Direct(c, x, y + Start_Line, &save);
    }

    //绘制不足的那部分
    prBMP2BMP_Decode(p, &save, Start_Line, Remained_Line);
    save.Height = Remained_Line;
    Graphic_Draw_BMP_Direct(c, x, y + Start_Line, &save);
}

void Graphic_Draw_prBMP_inArea(canvas *c, uint16_t x, uint16_t y, uint16_t width, uint16_t height, prBMP *p)
{
    if (width == 0 || height == 0)
        return;
    uint16_t Start_Index, End_Index;
    float Start_Line, End_Line;
    uint16_t Max_Line;
    float Delta_Line;

    BMP save;

    save.Colors = (uint16_t *)c->Screen->BMPDecoder_Buffer;
    Max_Line = (c->Screen->BMPDecoder_Buffer_Size / 2) / p->Width;

    if (Max_Line >= p->Height)
    {
        prBMP2BMP_Decode(p, &save, 0, p->Height);
        Graphic_Draw_BMP_inArea(c, x, y, width, height, &save);
    }
    else
    {
        if ((width == p->Width) && (height == p->Height))
            Graphic_Draw_BMP_Direct(c, x, y, &save);
        else
        {
            Delta_Line = (float)Max_Line * height / p->Height;

            Start_Index = 0;
            End_Index = Max_Line - 1;
            for (Start_Line = y; End_Index <= p->Height - 1; Start_Index += Max_Line, End_Index += Max_Line)
            {
                End_Line = MathHelper_Round(Start_Line + Delta_Line);

                prBMP2BMP_Decode(p, &save, Start_Index, Max_Line);

                Graphic_Draw_BMP_inArea(c, x, MathHelper_Round(Start_Line), width, MathHelper_Round(End_Line - Start_Line), &save);
                Start_Line = End_Line;
            }

            if (Start_Index < p->Height)
            {
                if (Start_Line > y + height - 1)
                    return;

                End_Index = p->Height - 1;
                End_Line = y + height - 1;

                prBMP2BMP_Decode(p, &save, Start_Index, End_Index - Start_Index);
                Graphic_Draw_BMP_inArea(c, x, MathHelper_Round(Start_Line), width, MathHelper_Round(End_Line - Start_Line), &save);
            }
            else
                return;
        }
    }
}

void Graphic_Draw_prBMP_withSize(canvas *c, uint16_t x, uint16_t y, float size, prBMP *p)
{
    Graphic_Draw_prBMP_inArea(c, x, y, MathHelper_Round(p->Width * size), MathHelper_Round(p->Height * size), p);
}

void Graphic_Draw_BMP_All_Direct(canvas *c, uint16_t x, uint16_t y, bmp *p)
{
    if (p->Format == BMP_f)
        Graphic_Draw_BMP_Direct(c, x, y, &p->BMP_Data.bmp);
    else if (p->Format == rBMP_f)
        Graphic_Draw_rBMP_Direct(c, x, y, &p->BMP_Data.rbmp);
    else if (p->Format == pBMP_f)
        Graphic_Draw_pBMP_Direct(c, x, y, &p->BMP_Data.pbmp);
    else if (p->Format == prBMP_f)
        Graphic_Draw_prBMP_Direct(c, x, y, &p->BMP_Data.prbmp);
}

void Graphic_Draw_BMP_All_inArea(canvas *c, uint16_t x, uint16_t y, uint16_t width, uint16_t height, bmp *p)
{
    if (p->Format == BMP_f)
        Graphic_Draw_BMP_inArea(c, x, y, width, height, &p->BMP_Data.bmp);
    else if (p->Format == rBMP_f)
        Graphic_Draw_rBMP_inArea(c, x, y, width, height, &p->BMP_Data.rbmp);
    else if (p->Format == pBMP_f)
        Graphic_Draw_pBMP_inArea(c, x, y, width, height, &p->BMP_Data.pbmp);
    else if (p->Format == prBMP_f)
        Graphic_Draw_prBMP_inArea(c, x, y, width, height, &p->BMP_Data.prbmp);
}

void Graphic_Draw_BMP_All_withSize(canvas *c, uint16_t x, uint16_t y, float size, bmp *p)
{
    if (p->Format == BMP_f)
        Graphic_Draw_BMP_withSize(c, x, y, size, &p->BMP_Data.bmp);
    else if (p->Format == rBMP_f)
        Graphic_Draw_rBMP_withSize(c, x, y, size, &p->BMP_Data.rbmp);
    else if (p->Format == pBMP_f)
        Graphic_Draw_pBMP_withSize(c, x, y, size, &p->BMP_Data.pbmp);
    else if (p->Format == prBMP_f)
        Graphic_Draw_prBMP_withSize(c, x, y, size, &p->BMP_Data.prbmp);
}

void Graphic_Draw_Char(canvas *c, uint16_t x, uint16_t y, uint8_t ch, uint16_t bcolor, uint16_t ccolor, const charmold *mold)
{
    uint16_t color;
    uint32_t k;
    charmold *chrmod;
    BMP p;

    chrmod = (charmold *)&mold[ch];
    k = chrmod->Width * chrmod->Height;
    color = bcolor;
    for (uint16_t j = 0, i = 0; j < k; i++)
    {
        for (uint16_t m = 0; m < chrmod->Blocks[i]; m++, j++)
            ((uint16_t *)c->Screen->BMPDecoder_Buffer)[j] = color;
        if (color == bcolor)
            color = ccolor;
        else
            color = bcolor;
    }
    p.Colors = (uint16_t *)c->Screen->BMPDecoder_Buffer;
    p.Height = chrmod->Height;
    p.Width = chrmod->Width;
    Graphic_Draw_BMP_Direct(c, x, y, &p);
}

void Graphic_Draw_String_inAreaFull(canvas *c, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t valign, uint8_t halign, font *f, const char *str)
{
    uint16_t substring_len = 0, substring_idx = 0;
    uint16_t row_x, column_y;
    char *txt_prprsbuf = (char *)c->Screen->TextPreprocess_Buffer;

    uint16_t row_num = _String_Preprocess(txt_prprsbuf, str, width, height, f->Mold);

    if (valign == Top_Align)
        column_y = y;
    else if (valign == VerMid)
        column_y = y + (height - f->Mold->Height * row_num) / 2;
    else if (valign == Bottom_Align)
        column_y = y + height - f->Mold->Height * row_num;

    for (uint16_t i = 0; i < row_num; i++)
    {
        for (substring_len = 0; txt_prprsbuf[substring_idx + substring_len] != '\n' && txt_prprsbuf[substring_idx + substring_len] != '\0';)
            substring_len++;

        uint16_t row_width_sum = 0;
        for (uint16_t j = 0; j < substring_len; j++)
            row_width_sum += f->Mold[txt_prprsbuf[substring_idx + j]].Width;

        row_x = x;
        if (halign == HorMid)
            row_x += (width - row_width_sum) / 2;
        else if (halign == Right_Align)
            row_x += width - row_width_sum;

        for (uint16_t j = 0; j < substring_len; j++)
        {
            Graphic_Draw_Char(c, row_x, column_y, txt_prprsbuf[substring_idx + j], f->BackColor, f->CharColor, f->Mold);
            row_x += f->Mold[str[substring_idx + j] - 0x20].Width;
        }
        column_y += f->Mold->Height;
        substring_idx += substring_len + 1;
    }
}

void Graphic_Draw_String_inGridFull(canvas *c, uint16_t x, uint16_t y, short line, short row, uint8_t valign, uint8_t halign, font *f, const char *str)
{
    if (line == INF)
    {
        valign = Top_Align;
        line = c->Area.Height - y - 1;
    }
    else
        line = f->Mold->Height * line;

    if (row == INF)
    {
        halign = Left_Align;
        row = c->Area.Width - x - 1;
    }
    else
        row = f->Mold->Width * row;

    Graphic_Draw_String_inAreaFull(c, x, y, row, line, valign, halign, f, str);
}

void Graphic_Draw_String_inAreaDirect(canvas *c, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t valign, uint8_t halign, uint16_t bcolor, uint16_t ccolor, const charmold *mold, const char *str)
{
    font f = {bcolor, ccolor, mold};
    Graphic_Draw_String_inAreaFull(c, x, y, width, height, valign, halign, &f, str);
}

void Graphic_Draw_String_inGridDirect(canvas *c, uint16_t x, uint16_t y, short line, short row, uint8_t valign, uint8_t halign, uint16_t bcolor, uint16_t ccolor, const charmold *mold, const char *str)
{
    if (line == INF)
    {
        valign = Top_Align;
        line = c->Area.Height - y - 1;
    }
    else
        line = mold->Height * line;

    if (row == INF)
    {
        halign = Left_Align;
        row = c->Area.Width - x - 1;
    }
    else
        row = mold->Width * row;
    font f = {bcolor, ccolor, mold};
    Graphic_Draw_String_inAreaFull(c, x, y, row, line, valign, halign, &f, str);
}

void Graphic_Printf_inAreaFull(canvas *c, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t valign, uint8_t halign, font *f, const char *fstr, ...)
{
    int i;
    va_list val;

    va_start(val, fstr);
    i = vsprintf((char *)c->Screen->TextProcess_Buffer, fstr, val);
    ((char *)c->Screen->TextProcess_Buffer)[i] = '\0';
    Graphic_Draw_String_inAreaFull(c, x, y, width, height, valign, halign, f, (char *)c->Screen->TextProcess_Buffer);
    va_end(val);
}

void Graphic_Printf_inGridFull(canvas *c, uint16_t x, uint16_t y, short line, short row, uint8_t valign, uint8_t halign, font *f, const char *fstr, ...)
{
    int i;
    va_list val;

    if (line == INF)
    {
        valign = Top_Align;
        line = c->Area.Height - y - 1;
    }
    else
        line = f->Mold->Height * line;

    if (row == INF)
    {
        halign = Left_Align;
        row = c->Area.Width - x - 1;
    }
    else
        row = f->Mold->Width * row;

    va_start(val, fstr);
    i = vsprintf((char *)c->Screen->TextProcess_Buffer, fstr, val);
    ((char *)c->Screen->TextProcess_Buffer)[i] = '\0';
    Graphic_Draw_String_inAreaFull(c, x, y, row, line, valign, halign, f, (char *)c->Screen->TextProcess_Buffer);
    va_end(val);
}

void Graphic_Printf_inAreaDirect(canvas *c, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t valign, uint8_t halign, uint16_t bcolor, uint16_t ccolor, const charmold *mold, const char *fstr, ...)
{
    int i;
    va_list val;
    font f = {bcolor, ccolor, mold};

    va_start(val, fstr);
    i = vsprintf((char *)c->Screen->TextProcess_Buffer, fstr, val);
    ((char *)c->Screen->TextProcess_Buffer)[i] = '\0';
    Graphic_Draw_String_inAreaFull(c, x, y, width, height, valign, halign, &f, (char *)c->Screen->TextProcess_Buffer);
    va_end(val);
}

void Graphic_Printf_inGridDirect(canvas *c, uint16_t x, uint16_t y, short line, short row, uint8_t valign, uint8_t halign, uint16_t bcolor, uint16_t ccolor, const charmold *mold, const char *fstr, ...)
{
    int i;
    va_list val;
    font f = {bcolor, ccolor, mold};

    if (line == INF)
    {
        valign = Top_Align;
        line = c->Area.Height - y - 1;
    }
    else
        line = mold->Height * line;

    if (row == INF)
    {
        halign = Left_Align;
        row = c->Area.Width - x - 1;
    }
    else
        row = mold->Width * row;

    va_start(val, fstr);
    i = vsprintf((char *)c->Screen->TextProcess_Buffer, fstr, val);
    ((char *)c->Screen->TextProcess_Buffer)[i] = '\0';
    Graphic_Draw_String_inAreaFull(c, x, y, row, line, valign, halign, &f, (char *)c->Screen->TextProcess_Buffer);
    va_end(val);
}
