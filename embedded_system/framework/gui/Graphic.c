#include "Graphic.h"
#include "Memory.h"
#include "MathHelper.h"
#include "CharLib.h"
#include "Graphic_Config.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "USART.h"

#include "RGBLCD.h"
#include "TFTLCD.h"

extern void Graphic_Screen_Init(screen *sc, int8_t id, uint16_t width, uint16_t height, uint16_t bmpdcdr_bufsize, uint16_t rstr_bufsize, uint16_t txtprs_bufsize, uint16_t txtprprs_bufsize, status_flag vir, status_flag auto_malloc, const uint16_t *gram);

static void Method_Draw_Pixel(screen *sc, uint16_t x, uint16_t y, uint16_t color);
static void Method_Draw_Area(screen *sc, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
static void Method_GRAM_Transfer(screen *sc, uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint16_t *gram);
static void Method_GRAM_Transfer_Try(screen *sc, uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint16_t *gram);

void _Print_ChildCanvas(canvas *c, uint8_t offset);

static void Method_Draw_Pixel(screen *sc, uint16_t x, uint16_t y, uint16_t color)
{
    switch (sc->Screen_ID)
    {
    // 0:RGBLCD
    case 0:
    {
        *(((uint16_t *)LAYER1_GRAM_ADDR) + y * sc->Width + x) = color;
    }
    break;
    // 1:TFTLCD
    case 1:
    {
        TFTLCD_PresentMode(U2D_R2L, x, x + 1, y, y + 1);
        TFTLCD_ORDER = 0x2c00;
        TFTLCD_DATA = color;
    }
    break;
    default:
        break;
    }
}

static void Method_Draw_Area(screen *sc, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
{
    switch (sc->Screen_ID)
    {
    case 0:
    {
        uint16_t *cache = (uint16_t *)LAYER1_GRAM_ADDR + y * sc->Width + x;
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
                cache[j] = color;
            cache += sc->Width;
        }
    }
    break;
    case 1:
    {
        TFTLCD_PresentMode(U2D_R2L, x, x + width - 1, y, y + height - 1);
        TFTLCD_ORDER = 0x2c00;
        for (int i = 0; i < height * width; i++)
            TFTLCD_DATA = color;
    }
    break;
    default:
        break;
    }
}

static void Method_GRAM_Transfer(screen *sc, uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint16_t *gram)
{
    switch (sc->Screen_ID)
    {
    case 0:
    {
        RGBLCD_GRAM_Transfer(x, y, width, height, (uint16_t *)gram, (uint16_t *)LAYER1_GRAM_ADDR);
    }
    break;
    case 1:
    {
        TFTLCD_PresentMode(U2D_R2L, x, x + width - 1, y, y + height - 1);
        TFTLCD_ORDER = 0x2c00;
        for (int i = 0; i < height * width; i++)
            TFTLCD_DATA = gram[i];
    }
    break;
    default:
        break;
    }
}

static void Method_GRAM_Transfer_Try(screen *sc, uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint16_t *gram)
{
    switch (sc->Screen_ID)
    {
    case 0:
    {
        if (!DMA2D_Is_Running())
            RGBLCD_GRAM_Transfer_Begin(x, y, width, height, (uint16_t *)gram, (uint16_t *)LAYER1_GRAM_ADDR);
    }
    break;
    case 1:
    {
        TFTLCD_PresentMode(U2D_R2L, x, x + width - 1, y, y + height - 1);
        TFTLCD_ORDER = 0x2c00;
        for (int i = 0; i < height * width; i++)
            TFTLCD_DATA = gram[i];
    }
    break;
    default:
        break;
    }
}

void _Print_ChildCanvas(canvas *c, uint8_t offset)
{
    char temp = '-';
    for (int j = 0; j < offset; j++)
        USART_Send_Data_Flow(USART1, &temp, 1);
    USART_Printf(USART1, "%#x\r\n", c);
    for (int i = 0; i < c->ChildCanvas.Nodes_Num; i++)
    {
        canvas *child_c = LinkedList_Find(&c->ChildCanvas, i);
        _Print_ChildCanvas(child_c, offset + 1);
    }
}

void Graphic_Screen_Prepare(screen *sc, int8_t id, uint16_t width, uint16_t height, uint16_t bmpdcdr_bufsize, uint16_t rstr_bufsize, uint16_t txtprs_bufsize, uint16_t txtprprs_bufsize, status_flag vir, status_flag auto_malloc, const uint16_t *gram)
{
    Graphic_Screen_Init(sc, id, width, height, bmpdcdr_bufsize, rstr_bufsize, txtprs_bufsize, txtprprs_bufsize, vir, auto_malloc, gram);
    sc->Draw_Pixel = Method_Draw_Pixel;
    sc->Draw_Area = Method_Draw_Area;
    sc->GRAM_Transfer = Method_GRAM_Transfer;
    sc->GRAM_Transfer_Try = Method_GRAM_Transfer_Try;
}

void Graphic_Print_CanvasInfo(canvas *c)
{
    USART_Printf(USART1, "X:                %d\r\n", c->Area.X);
    USART_Printf(USART1, "Y:                %d\r\n", c->Area.Y);
    USART_Printf(USART1, "Width:            %d\r\n", c->Area.Width);
    USART_Printf(USART1, "Height:           %d\r\n", c->Area.Height);
    USART_Printf(USART1, "Display:          %d\r\n", c->Display);
    USART_Printf(USART1, "Auto_Malloc:      %d\r\n", c->Auto_Malloc);
    USART_Printf(USART1, "Virtual:          %d\r\n", c->Virtual);
    USART_Printf(USART1, "WriteThrough:     %d\r\n", c->Virtual);

    USART_Printf(USART1, "Screen:           %#x\r\n", c->Screen);
    USART_Printf(USART1, "ParentCanvas:     %#x\r\n", c->ParentCanvas);
    USART_Printf(USART1, "GRAM_Address:     %#x\r\n", c->Address);

    USART_Printf(USART1, "ChildCanvas num:  %d\r\n", c->ChildCanvas.Nodes_Num);
    for (int i = 0; i < c->ChildCanvas.Nodes_Num; i++)
        USART_Printf(USART1, "#%d:              %#x\r\n", i, LinkedList_Find(&c->ChildCanvas, i));
    USART_Printf(USART1, "\r\n");
}

void Graphic_Print_ChildCanvas(canvas *c)
{
    _Print_ChildCanvas(c, 0);
    USART_Printf(USART1, "\r\n");
}
