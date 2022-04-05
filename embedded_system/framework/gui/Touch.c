#include "Touch.h"
#include "Graphic.h"
#include "FT5206.h"
#include "Memory.h"
#include "USART.h"
#include <math.h>

/*touch中的各种结构体所在的堆*/
#define TOUCH_HEAP  HEAP1   


uint8_t Get_Coordinates(uint16_t* xy)
{
    return FT5206_Get_Points(xy,1);
}



/*判断点(px,py)是否在由(ax,ay)和width(宽)height(高)定义的区域内*/
#define IS_IN_AREA(ax,ay,width,height,px,py) ((((px)<(ax)+(width))&&((px)>(ax)))&&(((py)<(ay)+(height))&&((py)>(ay))))

#define ABS(x)  ((x)>=0?(x):-(x))
#define EQUAL_RANGE 5
#define IS_EQUAL(x1,y1,x2,y2)                ((ABS(x1-x2)<EQUAL_RANGE)&&(ABS(y1-y2)<EQUAL_RANGE))

touch_area* Method_Create_Area    (touch_device* td, short x, short y, uint16_t width, uint16_t height);
touch_area* Method_Create_Area2   (touch_device* td, short xmin, short xmax, short ymin, short ymax);
void        Method_Delete_Area    (touch_device* td, touch_area* ta);
void        Method_Process        (touch_device* td);

/*更新坐标变换的参数*/
void _kb_Update(touch_device* td)
{
    if ((td->Rotation==CLOCKWISE0)||(td->Rotation==CLOCKWISE180))
    {
        _RO_WRITE(td->Xk,float,((float)td->X_Max-td->X_Min)/td->ScreenWidth);
        _RO_WRITE(td->Xb,float,td->X_Min);
        _RO_WRITE(td->Yk,float,((float)td->Y_Max-td->Y_Min)/td->ScreenHeight);
        _RO_WRITE(td->Yb,float,td->Y_Min);
    }
    else if ((td->Rotation==CLOCKWISE90)||(td->Rotation==CLOCKWISE270))
    {
        _RO_WRITE(td->Xk,float,((float)td->X_Max-td->X_Min)/td->ScreenHeight);
        _RO_WRITE(td->Xb,float,td->X_Min);
        _RO_WRITE(td->Yk,float,((float)td->Y_Max-td->Y_Min)/td->ScreenWidth);
        _RO_WRITE(td->Yb,float,td->Y_Min);
    }
}

void _TouchArea_Init(touch_area* ta, int16_t X, int16_t Y, uint16_t Width, uint16_t Height)
{
    ta->X=X;
    ta->Y=Y;
    ta->Width=Width;
    ta->Height=Height;
    ta->Association=NULL;
    
    ta->All_Event_Enable=Disable;
    
    ta->PressDown_Enable=Disable;
    _RO_WRITE(ta->PressDown_Happened,status_flag,Disable);
    
    ta->Release_Enable=Disable;
    _RO_WRITE(ta->Release_Happened,status_flag,Disable);
    
    ta->KeepPress_Enable=Disable;
    _RO_WRITE(ta->KeepPress_Happened,status_flag,Disable);
}

void _Coordinates_Update(touch_device* td)
{
    int i;
    short save[2];
    
    /**************************更新触摸设备的信息**************************/
    //获取新坐标
    i=Get_Coordinates((uint16_t*)save);
    _RO_WRITE(td->PointX,uint16_t,save[0]);
    _RO_WRITE(td->PointY,uint16_t,save[1]);
    //坐标变换
    if (td->Rotation==CLOCKWISE90)
    {
        save[0]=td->PointX;
        _RO_WRITE(td->PointX,short,td->PointY);
        _RO_WRITE(td->PointY,short,td->ScreenWidth-save[0]);
    }
    else if (td->Rotation==CLOCKWISE180)
    {
        _RO_WRITE(td->PointX,short,td->ScreenWidth-td->PointX);
        _RO_WRITE(td->PointY,short,td->ScreenHeight-td->PointY);
    }
    else if (td->Rotation==CLOCKWISE270)
    {
        save[0]=td->PointX;
        _RO_WRITE(td->PointX,short,td->ScreenHeight-td->PointY);
        _RO_WRITE(td->PointY,short,save[0]);
    }
    _RO_WRITE(td->PointX,short,td->PointX*td->Xk+td->Xb);
    _RO_WRITE(td->PointY,short,td->PointY*td->Yk+td->Yb);
    //判断是否有触点按下
    if ((i!=0)&&IS_IN_AREA(td->ValidArea_X,td->ValidArea_Y,td->ValidArea_Width,td->ValidArea_Height,td->PointX,td->PointY))
        _RO_WRITE(td->IsKeyDown,status_flag,Enable);
    else
        _RO_WRITE(td->IsKeyDown,status_flag,Disable);
    
    /**************************更新每个触摸区域的信息**************************/
    touch_area* ta;
    for (i=0;i<td->TouchArea_List.Nodes_Num;i++)
    {
        //存储旧坐标
        ta=LinkedList_Find(&td->TouchArea_List,i);
        _RO_WRITE(ta->IsKeyDown_Old,status_flag,ta->IsKeyDown);
        _RO_WRITE(ta->PointX_Old,short,ta->PointX);
        _RO_WRITE(ta->PointY_Old,short,ta->PointY);
        
        //更新坐标
        if (td->IsKeyDown)
        {
            if (IS_IN_AREA(ta->X,ta->Y,ta->Width,ta->Height,td->PointX,td->PointY))
            {
                _RO_WRITE(ta->PointX,short,td->PointX-ta->X);
                _RO_WRITE(ta->PointY,short,td->PointY-ta->Y);
                _RO_WRITE(ta->IsKeyDown,status_flag,Enable);
            }
            else
                _RO_WRITE(ta->IsKeyDown,status_flag,Disable);
        }
        else
            _RO_WRITE(ta->IsKeyDown,status_flag,Disable);
    }
}

void _Touch_Area_Event_Update(touch_area* ta)
{
    if (ta->All_Event_Enable)
    {
        if (ta->PressDown_Enable)
        {
            //之前未按下，现在按下
            if ((!ta->IsKeyDown_Old)&&ta->IsKeyDown)
                _RO_WRITE(ta->PressDown_Happened,status_flag,Enable);
        }
        if (ta->Release_Enable)
        {
            //之前按下，现在未按下
            if (ta->IsKeyDown_Old&&(!ta->IsKeyDown))
                    _RO_WRITE(ta->Release_Happened,status_flag,Enable);
        }
        if (ta->KeepPress_Enable)
        {
            //之前按下，现在也按下
            if (ta->IsKeyDown_Old&&ta->IsKeyDown)
                _RO_WRITE(ta->KeepPress_Happened,status_flag,Enable);
        }
        if (ta->Click_Enable)
        {
            //如果按下
            if ((!ta->IsKeyDown_Old)&&ta->IsKeyDown)
                _RO_WRITE(ta->IsMoved,status_flag,Disable);
            else 
            {
                //如果没有移动过
                if (!ta->IsMoved)
                {
                    //判断是否移动
                    if (ta->IsKeyDown&&ta->IsKeyDown_Old)
                    {
                        if (IS_EQUAL(ta->PointX,ta->PointY,ta->PointX_Old,ta->PointY_Old))
                            _RO_WRITE(ta->IsMoved,status_flag,Disable);
                        else
                            _RO_WRITE(ta->IsMoved,status_flag,Enable);
                    }
                    else if (ta->IsKeyDown_Old&&(!ta->IsKeyDown))
                        _RO_WRITE(ta->Click_Happened,status_flag,Enable);
                }
            }
        }
    }
}


touch_area* Method_Create_Area(touch_device* td, short x, short y, uint16_t width, uint16_t height)
{
    touch_area* new_ta;
    
    new_ta=LinkedList_Add(&td->TouchArea_List,td->TouchArea_List.Nodes_Num);
    _TouchArea_Init(new_ta,x,y,width,height);
    return new_ta;
}

touch_area* Method_Create_Area2(touch_device* td, short xmin, short xmax, short ymin, short ymax)
{
    return Method_Create_Area(td,xmin,ymin,xmax-xmin,ymax-ymin);
}

void Method_Delete_Area(touch_device* td, touch_area* ta)
{
    LinkedList_Dispose2(&td->TouchArea_List,ta);
}

void Method_Process(touch_device* td)
{
    int i;
    touch_area* ta;
    
    _Coordinates_Update(td);    //坐标更新
    for (i=0;i<td->TouchArea_List.Nodes_Num;i++)    //遍历触摸区域
    {
        ta=LinkedList_Find(&td->TouchArea_List,i);
        _Touch_Area_Event_Update(ta);   //事件更新
        if (ta->PressDown_Happened)
            ta->PressDown_CallBackFunc(td,ta);
        if (ta->Release_Happened)
            ta->Release_CallBackFunc(td,ta);
        if (ta->KeepPress_Happened)
            ta->KeepPress_CallBackFunc(td,ta);
        if (ta->Click_Happened)
            ta->Click_CallBackFunc(td,ta);
        _RO_WRITE(ta->PressDown_Happened,status_flag,Disable);
        _RO_WRITE(ta->KeepPress_Happened,status_flag,Disable);
        _RO_WRITE(ta->Release_Happened,status_flag,Disable);
        _RO_WRITE(ta->Click_Happened,status_flag,Disable);
    }
}

void TouchDevice_Init(touch_device* td, uint16_t sw, uint16_t sh ,short xmax, short xmin, short ymax, short ymin, rotation_type rotation)
{
    _RO_WRITE(td->ScreenWidth,uint16_t,sw);
    _RO_WRITE(td->ScreenHeight,uint16_t,sh);
    
    _RO_WRITE(td->X_Max,uint16_t,xmax);
    _RO_WRITE(td->X_Min,uint16_t,xmin);
    _RO_WRITE(td->Y_Max,uint16_t,ymax);
    _RO_WRITE(td->Y_Min,uint16_t,ymin);
    _RO_WRITE(td->Rotation,rotation_type,rotation);
    
    td->ValidArea_X=xmin;
    td->ValidArea_Y=ymin;
    td->ValidArea_Width=xmax-xmin;
    td->ValidArea_Height=ymax-ymin;
    
    _RO_WRITE(td->IsKeyDown,status_flag,Disable);
    
    td->Create_Area=Method_Create_Area;
    td->Create_Area2=Method_Create_Area2;
    td->Delete_Area=Method_Delete_Area;
    td->Process=Method_Process;

    _kb_Update(td);
        
    LinkedList_Prepare(&td->TouchArea_List,sizeof(touch_area));
}
