#ifndef _TOUCH_H_
#define _TOUCH_H_

#include "Framework.h"
#include "LinkedList.h"

/*
*事件定义
*/
typedef enum
{
    PressDown,  //按下事件
    Release,    //松开事件
    KeepPress,  //持续按下事件
    Click
} touch_event_type;

/*
*定义屏幕的旋转类型
*/
typedef enum
{
    CLOCKWISE0,      //不旋转
    CLOCKWISE90,     //顺时针90°
    CLOCKWISE180,    //顺时针180°
    CLOCKWISE270     //顺时针270°
} rotation_type;

typedef struct touch_area_class     touch_area;
typedef struct touch_device_class   touch_device;

typedef void (*Touch_Event_Callback)(touch_device* td, touch_area* ta);

/*
*触摸区域类
*描述触摸区域的基本参数
*/
struct touch_area_class
{
                short        X;                     //区域X坐标
                short        Y;                     //区域Y坐标
                uint16_t     Width;                 //区域宽度
                uint16_t     Height;                //区域高度
    
    volatile    status_flag  All_Event_Enable;      //所有事件使能
    
    volatile    status_flag  PressDown_Enable;      //按下事件使能
    volatile    status_flag  Release_Enable;        //松开事件使能
    volatile    status_flag  KeepPress_Enable;      //持续触摸事件使能
    volatile    status_flag  Click_Enable;
    
    vro         status_flag  PressDown_Happened;    //按下事件发生
    vro         status_flag  Release_Happened;      //松开事件发生
    vro         status_flag  KeepPress_Happened;    //持续触摸事件发生
    vro         status_flag  Click_Happened;
    
    vro         status_flag  IsKeyDown;             //是否有触点按下
    vro         status_flag  IsKeyDown_Old;
    
    vro         status_flag  IsMoved;               //触点按下后是否发生移动
    
    vro         short        PointX;                //触点X坐标
    vro         short        PointY;                //触点Y坐标
    vro         short        PointX_Old;            //触点X坐标
    vro         short        PointY_Old;            //触点Y坐标
    
                void*        Association;           //关联对象
    
    /*各事件的回调函数*/
    Touch_Event_Callback         PressDown_CallBackFunc;
    Touch_Event_Callback         Release_CallBackFunc;
    Touch_Event_Callback         KeepPress_CallBackFunc;
    Touch_Event_Callback         Click_CallBackFunc;
};

/*
*触摸设备类
*描述触摸设备的基本参数
*/
struct touch_device_class
{
    
    /************************************成员************************************/
    
    /*
    *屏宽/高，这两个参数指明读取到的原始坐标参数的范围
    *这几个值由触摸屏的硬件设备决定
    */
    vro uint16_t        ScreenWidth;    //屏宽
    vro uint16_t        ScreenHeight;   //屏高
    
    /*
    *这些参数指明设定的坐标范围以及旋转角度
    *这些值由用户自己定义
    */
    vro short           X_Max;      //X最大值
    vro short           X_Min;      //X最小值
    vro short           Y_Max;      //Y最大值
    vro short           Y_Min;      //Y最小值
    vro rotation_type   Rotation;   //旋转角度
    
    /*用于坐标变换的参数*/
    vro float           Xk;
    vro float           Xb;
    vro float           Yk;
    vro float           Yb;
    
    /*这几个值用于设定有效的触摸范围*/
        short           ValidArea_X;
        short           ValidArea_Y;
        short           ValidArea_Width;
        short           ValidArea_Height;
        
    /*是否有触点按下*/
    vro status_flag     IsKeyDown;
    
    /*触点的坐标*/
    vro short           PointX;
    vro short           PointY;
    
        linkedlist      TouchArea_List; //存储所有触摸区域的链表
        
        void*           Association;    //关联对象
    
    
    
    
    
    /************************************方法************************************/
    
    /***************************************************************************************************************
    *功能：创建触摸区域
    *参数：1.td：触摸设备的实例，触摸区域将创建在该设备上
    *      2.x：区域的x坐标
    *      3.y：区域的y坐标
    *      4.width：区域的宽度
    *      5.height；区域的高度
    *返回值：实例的地址
    ***************************************************************************************************************/
    touch_area* (*Create_Area)    (struct touch_device_class* td, short x, short y, uint16_t width, uint16_t height);
    
    /*************************************************************************************************************
    *功能：创建触摸区域，该方法是Create_Area的衍生
    *参数：1.td：触摸设备的实例，触摸区域将创建在该设备上
    *      2.xmin：x最小坐标
    *      3.xmax：x最大坐标
    *      4.ymin：y最小坐标
    *      5.ymax：y最大坐标
    *返回值：实例的地址
    *说明：该方法构建一个由x轴区间(xmin,xmax)和y轴区间(ymin,ymax)限定的触摸区域
    *************************************************************************************************************/
    touch_area* (*Create_Area2)   (struct touch_device_class* td, short xmin, short xmax, short ymin, short ymax);
    
    /*****************************************************************************
    *功能：删除触摸区域，并释放其资源
    *参数：1.td：触摸设备的实例
    *      2.ta：要删除的触摸区域的实例
    *****************************************************************************/
    void        (*Delete_Area)    (struct touch_device_class* td, touch_area* ta);

    /*********************************************************************************
    *功能：执行触摸库主进程函数，
    *参数：1.td：触摸设备的实例
    *说明：程序在该函数中更新触摸屏状态并检测事件是否发生，应该周期性执行该函数不断
    *      检测触摸屏状态，对触摸及时做出反应
    *********************************************************************************/
    void        (*Process)        (struct touch_device_class* td);
};




/*****************************************************************************************************************************************
*功能：触摸设备初始化
*参数：1.td：触摸设备类实例
*      2.sw：屏宽
*      3.sh：屏高
*      4.xmax：X坐标最大值
*      5.xmin：X坐标最小值
*      6.ymax：Y坐标最大值
*      7.ymin：Y坐标最小值
*      8.rotation：旋转角度
*****************************************************************************************************************************************/
void TouchDevice_Init(touch_device* td, uint16_t sw, uint16_t sh ,short xmax, short xmin, short ymax, short ymin, rotation_type rotation);








#endif
