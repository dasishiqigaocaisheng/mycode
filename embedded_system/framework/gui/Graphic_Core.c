#include "Graphic_Config.h"
#include "Graphic_DataStructure.h"
#include "Memory.h"

/**********************************************************************************************************************************************************
 *
 *                                                                      函数声明列表
 *
 **********************************************************************************************************************************************************/
/*========================================================================内部函数=========================================================================*/

static void ChildCache2ParentCache(canvas *child_c, canvas *parent_c);
static uint16_t *_GRAM_Malloc(canvas *c);
static void _GRAM_Free(canvas *Canvas);
static void _Canvas_Init(canvas *c, uint16_t x, uint16_t y, uint16_t width, uint16_t height, status_flag vir, status_flag auto_malloc, const uint16_t *gram);

/*=======================================================================对外接口函数=======================================================================*/

canvas *Graphic_Screen_Add_ChildCanvas(canvas *parent_c, uint16_t x, uint16_t y, uint16_t width, uint16_t height, status_flag vir, status_flag auto_malloc, const uint16_t *gram);
void Graphic_Screen_Get_AbsoluteCoordinate(canvas *child_c, canvas *parent_c, uint16_t *x, uint16_t *y);
void Graphic_Screen_Refresh(screen *sc);
void Graphic_Screen_Refresh_Try(screen *sc);
void Graphic_Canvas_Virtual_Set(canvas *c, status_flag vir);
void Graphic_Canvas_WriteThrough_Set(canvas *c, status_flag wt);
void Graphic_Canvas_GRAM_Set(canvas *c, const uint16_t *gram);
void Graphic_Canvas_Update(canvas *c);
void Graphic_Canvas_Flush(canvas *c);
void Graphic_Canvas_Flush_Try(canvas *c);
void Graphic_Canvas_Dispose(canvas *c, status_flag also_dispose_childcanvas);

/**********************************************************************************************************************************************************
 *
 *                                                                      内部函数定义
 *
 **********************************************************************************************************************************************************/

static void ChildCache2ParentCache(canvas *child_c, canvas *parent_c)
{
    //父、子画布显存使能，直接进行显存搬运
    if (IS_CANVAS_CACHE_ENABLE(parent_c) && IS_CANVAS_CACHE_ENABLE(child_c))
    {
        uint16_t x, y;
        uint16_t *child_cache, *parent_cache;

        Graphic_Screen_Get_AbsoluteCoordinate(child_c, parent_c, &x, &y);
        child_cache = child_c->Address;
        parent_cache = CANVAS_GRAM_ADDR(parent_c, x, y);
        for (int i = 0; i < child_c->Area.Height; i++)
        {
            for (int j = 0; j < child_c->Area.Width; j++)
                parent_cache[j] = child_cache[j];
            child_cache += child_c->Area.Width;
            parent_cache += parent_c->Area.Width;
        }
    }
    else if (IS_CANVAS_CACHE_ENABLE(child_c)) //父画布没有显存使能，但子画布有，向上一级请求
    {
        //确保父画布不是基础画布
        if (parent_c->ParentCanvas != NULL)
            ChildCache2ParentCache(child_c, parent_c->ParentCanvas);
        else
        {
            uint16_t x, y;
            Graphic_Screen_Get_AbsoluteCoordinate(child_c, parent_c, &x, &y);
            parent_c->Screen->GRAM_Transfer(child_c->Screen, x, y, child_c->Area.Width, child_c->Area.Height, child_c->Address);
        }
    }
}

static uint16_t *_GRAM_Malloc(canvas *c)
{
    uint16_t *Addr;

    Addr = Malloc(GRAM_HEAP, (c->Area.Height + 2 * PROTECTED_ROOM) * c->Area.Width * 2);
    Addr = Addr == NULL ? NULL : Addr + c->Area.Width * PROTECTED_ROOM;
    return Addr;
}

static void _GRAM_Free(canvas *Canvas)
{
    Free(GRAM_HEAP, Canvas->Address - Canvas->Area.Width);
}

static void _Canvas_Init(canvas *c, uint16_t x, uint16_t y, uint16_t width, uint16_t height, status_flag vir, status_flag auto_malloc, const uint16_t *gram)
{
    //初始化Area
    c->Area.X = x;
    c->Area.Y = y;
    c->Area.Width = width;
    c->Area.Height = height;

    //初始化Display
    c->Display = Enable;

    //初始化Virtual和WriteThrough
    if (!vir)
    {
        if (auto_malloc)
            _RO_WRITE(c->Address, vro uint16_t *, _GRAM_Malloc(c));
        else
            _RO_WRITE(c->Address, vro uint16_t *, gram);
        _RO_WRITE(c->WriteThrough, status_flag, Disable);
    }
    else
        _RO_WRITE(c->WriteThrough, status_flag, Enable);
    _RO_WRITE(c->Virtual, status_flag, vir);
    _RO_WRITE(c->Auto_Malloc, status_flag, auto_malloc);

    //分配父画布
    _RO_WRITE(c->ParentCanvas, canvas *, NULL);

    //指向屏幕
    _RO_WRITE(c->Screen, screen *, NULL);

    //子画布链表初始化
    LinkedList_Prepare(&c->ChildCanvas, sizeof(struct canvas_class));
}

/**********************************************************************************************************************************************************
 *
 *                                                                    外部接口函数定义
 *
 **********************************************************************************************************************************************************/

void Graphic_Screen_Init(screen *sc, int8_t id, uint16_t width, uint16_t height, uint16_t bmpdcdr_bufsize, uint16_t rstr_bufsize, uint16_t txtprs_bufsize, uint16_t txtprprs_bufsize, status_flag vir, status_flag auto_malloc, const uint16_t *gram)
{
    sc->Screen_ID = id;

    _RO_WRITE(sc->Width, uint16_t, width);
    _RO_WRITE(sc->Height, uint16_t, height);

    _RO_WRITE(sc->BMPDecoder_Buffer, void *, Malloc(BMPDECODER_BUFFER_HEAP, bmpdcdr_bufsize));
    _RO_WRITE(sc->Rasterization_Buffer, void *, Malloc(RASTERIZATION_BUFFER_HEAP, rstr_bufsize));
    _RO_WRITE(sc->TextProcess_Buffer, void *, Malloc(PRINT_BUFFER_HEAP, txtprs_bufsize));
    _RO_WRITE(sc->TextPreprocess_Buffer, void *, Malloc(PRINT_BUFFER_HEAP, txtprprs_bufsize));

    _RO_WRITE(sc->BMPDecoder_Buffer_Size, uint16_t, bmpdcdr_bufsize);
    _RO_WRITE(sc->Rasterization_Buffer_Size, uint16_t, rstr_bufsize);
    _RO_WRITE(sc->TextProcess_Buffer_Size, uint16_t, txtprs_bufsize);
    _RO_WRITE(sc->TextPreprocess_Buffer_Size, uint16_t, txtprprs_bufsize);

    _Canvas_Init(&sc->Canvas, 0, 0, width, height, vir, auto_malloc, gram);

    _RO_WRITE(sc->Canvas.Screen, screen *, sc);
}

canvas *Graphic_Screen_Add_ChildCanvas(canvas *parent_c, uint16_t x, uint16_t y, uint16_t width, uint16_t height, status_flag vir, status_flag auto_malloc, const uint16_t *gram)
{
    canvas *c = LinkedList_AddtoEnd(&parent_c->ChildCanvas);
    _Canvas_Init(c, x, y, width, height, vir, auto_malloc, gram);
    _RO_WRITE(c->ParentCanvas, canvas *, parent_c);
    _RO_WRITE(c->Screen, screen *, parent_c->Screen);
    return c;
}

void Graphic_Screen_Get_AbsoluteCoordinate(canvas *child_c, canvas *parent_c, uint16_t *x, uint16_t *y)
{
    canvas *c = child_c;
    uint16_t temp_x = 0, temp_y = 0;

    while (c != parent_c)
    {
        temp_x += c->Area.X;
        temp_y += c->Area.Y;
        c = c->ParentCanvas;
    }
    *x = temp_x;
    *y = temp_y;
}

void Graphic_Canvas_Virtual_Set(canvas *c, status_flag vir)
{
    if (c->Virtual) //如果是虚拟画布
    {
        //现在设置为非虚拟的，需要显存分配
        if ((!vir) && (c->Auto_Malloc))
            _RO_WRITE(c->Address, uint16_t *, _GRAM_Malloc(c));
    }
    else
    {
        //非虚拟画布设置为虚拟，则释放显存
        if (vir)
        {
            if (c->Auto_Malloc)
            {
                _GRAM_Free(c);
                _RO_WRITE(c->Address, uint16_t *, NULL);
            }
            _RO_WRITE(c->WriteThrough, status_flag, Enable);
        }
    }
    _RO_WRITE(c->Virtual, status_flag, vir);
}

void Graphic_Canvas_WriteThrough_Set(canvas *c, status_flag wt)
{
    //非虚拟画布，则允许不透写
    if (!c->Virtual)
        _RO_WRITE(c->WriteThrough, status_flag, wt);
    else //虚拟画布必须透写
        _RO_WRITE(c->WriteThrough, status_flag, Enable);
}

void Graphic_Canvas_GRAM_Set(canvas *c, const uint16_t *gram)
{
    if (!c->Auto_Malloc)
        _RO_WRITE(c->Address, uint16_t *, (uint16_t *)gram);
}

void Graphic_Canvas_Update(canvas *c)
{
    canvas *child_c;

    //遍历子画布，分别执行Update
    //子画布Update完后，父画布搬运子画布显存到自己的显存
    for (int i = 0; i < c->ChildCanvas.Nodes_Num; i++)
    {
        child_c = LinkedList_Find(&c->ChildCanvas, i);
        Graphic_Canvas_Update(child_c);
        //只有当子画布显存使能时才执行搬运任务
        //防止重复显存搬运
        if (IS_CANVAS_CACHE_ENABLE(child_c))
            ChildCache2ParentCache(child_c, c);
    }
}

void Graphic_Canvas_Flush(canvas *c)
{
    //如果画布显存使能则立即进行显存搬运
    if (IS_CANVAS_CACHE_ENABLE(c))
    {
        uint16_t x, y;
        Graphic_Screen_Get_AbsoluteCoordinate(c, &c->Screen->Canvas, &x, &y);
        c->Screen->GRAM_Transfer(c->Screen, x, y, c->Area.Width, c->Area.Height, c->Address);
    }
    else //否则搬运子画布的显存
    {
        canvas *child_c;
        for (int i = 0; i < c->ChildCanvas.Nodes_Num; i++)
        {
            child_c = LinkedList_Find(&c->ChildCanvas, i);
            Graphic_Canvas_Flush(child_c);
        }
    }
}

void Graphic_Canvas_Flush_Try(canvas *c)
{
    if (IS_CANVAS_CACHE_ENABLE(c))
    {
        c->Screen->GRAM_Transfer_Try(c->Screen, c->Area.X, c->Area.Y, c->Area.Width, c->Area.Height, c->Address);
    }
    else
    {
        canvas *child_c;
        for (int i = 0; i < c->ChildCanvas.Nodes_Num; i++)
        {
            child_c = LinkedList_Find(&c->ChildCanvas, i);
            Graphic_Canvas_Flush_Try(child_c);
        }
    }
}

void Graphic_Canvas_Dispose(canvas *c, status_flag also_dispose_childcanvas)
{
    if (c == &c->Screen->Canvas)
        return;
    if (also_dispose_childcanvas)
    {
        //销毁所有子画布
        for (int i = 0; i < c->ChildCanvas.Nodes_Num; i++)
            Graphic_Canvas_Dispose(LinkedList_Get_FirstNode(&c->ChildCanvas), Enable);
    }
    else
    {
        //将子画布添加到父画布的子画布中
        for (int i = 0; i < c->ChildCanvas.Nodes_Num; i++)
        {
            canvas *child_c = LinkedList_Remove(&c->ChildCanvas, 0);
            Graphic_Screen_Get_AbsoluteCoordinate(child_c, c->ParentCanvas, &child_c->Area.X, &child_c->Area.Y);
            LinkedList_AddtoEnd2(&c->ParentCanvas->ChildCanvas, child_c);
        }
    }
    //释放显存
    if (c->Auto_Malloc && !c->Virtual)
        _GRAM_Free(c);
    //从父画布中销毁自己
    LinkedList_Dispose2(&c->ParentCanvas->ChildCanvas, c);
}

void Graphic_Screen_Refresh(screen *sc)
{
    Graphic_Canvas_Flush(&sc->Canvas);
}

void Graphic_Screen_Refresh_Try(screen *sc)
{
    Graphic_Canvas_Flush_Try(&sc->Canvas);
}
