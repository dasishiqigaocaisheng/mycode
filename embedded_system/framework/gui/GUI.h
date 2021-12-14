/**************************************************************************
*|-----------------------------图形用户界面库----------------------------|*
*|-----------------------------@大四十七高材生---------------------------|*
*|-------------------------------2021.1.23-------------------------------|*
***************************************************************************/
#ifndef _GUI_H_
#define _GUI_H_

#include "Graphic.h"
#include "Touch.h"
#include "Framework.h"
#include "LinkedList.h"

//#define USE_TOUCH

//GUI_DisplayWaveform中Cover参数使用的宏，用于同时显示多个波形图像
//CLEAR：清除上一次绘制的波形
//HOLD：保留上一次绘制的波形
#define CLEAR 1	//canvas.s
#define HOLD  2

//描述波形曲线类型的宏
#define SCATTER 1	//散点
#define COLORED 2	//填充
#define CURVE	3	//普通曲线

#define BASICLAYER   0

#define PANEL_MAX_ROW   8
#define PANEL_MAX_LINE  8

typedef uint8_t layer_handler;

typedef struct gui_controller_class     gui_controller;
typedef struct component_class          component;
typedef struct component_layer_class    component_layer;
typedef struct button_class             button;
typedef struct label_class              label;
typedef struct panel_class              panel;
typedef struct toolbox_button_class     toolbox_button;
typedef struct toolbox_class            toolbox;
typedef struct scope_class              scope;

typedef union components_union_class    components_union;

typedef void (*Component_Event_Callback)(void* cm);

typedef enum
{
    BTN_PRESSDOWN,
    BTN_RELEASE,
    BTN_KEEPPRESS,
    BTN_CLICK,
    BTN_GETFOCUS,
    BTN_LOSTFOCUS,
    BTN_REPAINT
} button_event_type;


struct button_class
{
    vro layer_handler Layer;
    vro status_flag Active;
        status_flag Display;
    vro status_flag Focus;
        status_flag Updated;
    
        status_flag CustomBackground;
        
           status_flag AllEvent_Enable;
    vro    status_flag Event_PressDown_Enable;
    vro    status_flag Event_Release_Enable;
    vro    status_flag Event_KeepPress_Enable;
    vro    status_flag Event_Click_Enable;
    vro    status_flag Event_GetFocus_Enable;
    vro    status_flag Event_LostFocus_Enable;
    vro    status_flag Event_Repaint_Enable;
    
    vro    status_flag Event_PressDown_Happened;
    vro    status_flag Event_Release_Happened;
    vro    status_flag Event_KeepPress_Happened;
    vro    status_flag Event_Click_Happened;
    vro    status_flag Event_GetFocus_Happened;
    vro    status_flag Event_LostFocus_Happened;
    vro    status_flag Event_Repaint_Happened;
    
    rectangle Appearance;
    
    char Text[16];
    font Font;
    veralign_type Vertical_Align;
    horalign_type Horizontal_Align;
    canvas Canvas;
    touch_area* TouchArea;
    gui_controller* Controller;
    
    bmp* Background;
    
    Component_Event_Callback PressDown_Callback;
    Component_Event_Callback Release_Callback;
    Component_Event_Callback KeepPress_Callback;
    Component_Event_Callback Click_Callback;
    Component_Event_Callback GetFocus_Callback;
    Component_Event_Callback LostFocus_Callback;
    Component_Event_Callback Repaint_Callback;
    
    void (*Event_Set)           (button* btn, button_event_type event, status_flag status);    
    void (*Active_Set)          (button* btn, status_flag status);
    void (*Change_Text)         (button* btn, char* fmtstr, ...);
    void (*Repaint)             (button* btn);
    void (*Dispose)             (button* btn);
};
 
typedef enum
{
    LBL_REPAINT
} label_event_type;

struct label_class
{
    vro layer_handler   Layer;
    vro status_flag     Active;
        status_flag     Display;
        status_flag     Updated;
    
        status_flag CustomBackground;
        
           status_flag AllEvent_Enable;
    vro    status_flag Event_Repaint_Enable;
    
    vro    status_flag Event_Repaint_Happened;
    
    rectangle Appearance;
    
    char Text[32];
    font Font;
    veralign_type Vertical_Align;
    horalign_type Horizontal_Align;
    canvas Canvas;
    gui_controller* Controller;
    
    bmp* Background;
    
    Component_Event_Callback Repaint_Callback;
    
    void (*Event_Set)           (label* lbl, label_event_type event, status_flag status);    
    void (*Active_Set)          (label* lbl, status_flag status);
    void (*Change_Text)         (label* lbl, char* fmtstr, ...);
    void (*Repaint)             (label* lbl);
    void (*Dispose)             (label* lbl);
};

struct panel_class
{
    vro layer_handler Layer;
    
    area Area;
    
    status_flag UseRatio;
    status_flag Always_Average;
    
    uint8_t Boundary;
    
    uint16_t RowsNum;
    uint16_t LinesNum;
    
    vro void* Data_Array; 
    
    uint16_t* vro Blocks_Width;
    uint16_t* vro Blocks_Height;
    
    float* vro Horizontal_Ratio;
    float* vro Vertical_Ratio;
    
    vro uint16_t* vro Blocks_X;
    vro uint16_t* vro Blocks_Y;
    
    gui_controller* Controller;
    
    void (*Data_Update)(panel* pnl);
    void (*Dispose)(panel* pnl);
};

typedef enum
{
    TBX_CLICK,
    TBX_PRESSDOWN,
    TBX_KEEPPRESS,
    TBX_RELEASE,
    TBX_REPAINT,
    TBX_GETFOCUS,
    TBX_LOSTFOCUS
} toolbox_event_type;

struct toolbox_button_class
{
    vro status_flag Active;
    
        status_flag CustomBackground;
        
           status_flag AllEvent_Enable;
    vro    status_flag Event_Click_Enable;
    
    vro    status_flag Event_Click_Happened;
    
    rectangle Appearance;
    
    char Text[16];
    font Font;
    veralign_type Vertical_Align;
    horalign_type Horizontal_Align;
    canvas Canvas;
    touch_area* TouchArea;
    toolbox* ToolBox;
    
    bmp* Background;
    
    Component_Event_Callback Click_Callback;

    void (*Event_Set)           (toolbox_button* tbx_btn, toolbox_event_type event, status_flag status);    
    void (*Active_Set)          (toolbox_button* tbx_btn, status_flag status);
    void (*Change_Text)         (toolbox_button* tbx_btn, char* fmtstr, ...);
};

struct toolbox_class
{
    vro layer_handler Layer;
    vro status_flag Active;
        status_flag Display;
        status_flag Buttons_Display;
    vro status_flag Focus;
        status_flag Updated;
    
        status_flag CustomBackground;
        
           status_flag AllEvent_Enable;
    vro    status_flag Event_PressDown_Enable;
    vro    status_flag Event_Release_Enable;
    vro    status_flag Event_KeepPress_Enable;
    vro    status_flag Event_Click_Enable;
    vro    status_flag Event_GetFocus_Enable;
    vro    status_flag Event_LostFocus_Enable;
    vro    status_flag Event_Repaint_Enable;
    
    vro    status_flag Event_PressDown_Happened;
    vro    status_flag Event_Release_Happened;
    vro    status_flag Event_KeepPress_Happened;
    vro    status_flag Event_Click_Happened;
    vro    status_flag Event_GetFocus_Happened;
    vro    status_flag Event_LostFocus_Happened;
    vro    status_flag Event_Repaint_Happened;
    
    rectangle Appearance;
    
    uint16_t Button_Wdith;
    uint16_t Button_Height;
    
    char Text[16];
    font Font;
    veralign_type Vertical_Align;
    horalign_type Horizontal_Align;
    canvas Canvas;
    touch_area* TouchArea;
    gui_controller* Controller;
    linkedlist  Buttons;
    
    bmp* Background;
    
    Component_Event_Callback PressDown_Callback;
    Component_Event_Callback Release_Callback;
    Component_Event_Callback KeepPress_Callback;
    Component_Event_Callback Click_Callback;
    Component_Event_Callback GetFocus_Callback;
    Component_Event_Callback LostFocus_Callback;
    Component_Event_Callback Repaint_Callback;
    
    void (*Event_Set)           (toolbox* tbx, toolbox_event_type event, status_flag status);    
    void (*Active_Set)          (toolbox* tbx, status_flag status);
    void (*Change_Text)         (toolbox* tbx, char* fmtstr, ...);
    toolbox_button* (*Add_Button)          (toolbox* tbx, char* text);
    void (*Delete_Button)       (toolbox* tbx, toolbox_button* tbx_btn);
    void (*Repaint)             (toolbox* tbx);
    void (*Dispose)             (toolbox* tbx);
};

typedef enum
{
    BUTTON,
    LABEL,
    PANEL,
    TOOLBOX,
    TOOLBOX_BUTTON,
    SCOPE,
    NONE
} component_type;




struct component_layer_class
{
    volatile status_flag Active;
    uint16_t Color;
    linkedlist Components;
};

struct gui_controller_class
{
    vro status_flag IsInit;
        status_flag Updated;
    touch_device* vro TouchDevice;
    screen* Screen;
    linkedlist Layers;
    touch_area* Overall_TouchArea;
    
    vro layer_handler Active_Layer;
    component* vro    Focus_Component;
    
    layer_handler (*Add_Layer)(gui_controller* guictrl, uint16_t color);
    void (*Remove_Layer)(gui_controller* guictrl, layer_handler layer);
    void (*ActiveLayer_Set)(gui_controller* guictrl, layer_handler layer);
    
    button* (*Create_Button)(gui_controller* guictrl, uint16_t x, uint16_t y, uint16_t width, uint16_t height, status_flag virt, char* text, layer_handler layer);
    label*  (*Create_Label) (gui_controller* guictrl, uint16_t x, uint16_t y, uint16_t width, uint16_t height, status_flag virt, char* text, layer_handler layer);
    panel*  (*Create_Panel) (gui_controller* guictrl, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t rownum, uint8_t linenum, layer_handler layer);
    toolbox* (*Create_ToolBox)(gui_controller* guictrl, uint16_t x, uint16_t y, uint16_t width, uint16_t height, status_flag virt, char* text, layer_handler layer);
    scope*  (*Create_Scope)  (gui_controller* guictrl, uint16_t x, uint16_t y, uint16_t width, uint16_t height, status_flag virt, uint16_t max_dlen, status_flag vscale, layer_handler layer); 
    
    void (*Process)(gui_controller* guictrl);
};

void GUI_Init(gui_controller* guictrl, screen* sc, touch_device* td, uint16_t color);

void GUI_Info_Print(USART_TypeDef* usart, gui_controller* guictrl);
void GUI_Panel_Info_Print(USART_TypeDef* usart, panel* pnl);




typedef enum
{
    SCP_CLICK,
    SCP_PRESSDOWN,
    SCP_KEEPPRESS,
    SCP_RELEASE,
    SCP_REPAINT,
    SCP_GETFOCUS,
    SCP_LOSTFOCUS,
    SCP_STOPCHANGE
} scope_event_type;

/************************************************************************************
*坐标轴类：描述了示波器中的坐标轴（Axis）和网格（Net）的样式以及显示范围等参数		*
*Axis相关的参数描述了X轴和Y轴的样式以及范围，Net相关的参数描述了网格的样式和密度	*
*等信息。																			*
*说明：参数前面带下划线（"_"）的不应手动编辑，系统会自动生成						*
*************************************************************************************/
typedef struct Axis_Class 
{	
	//是否显示坐标轴，如果该值为0则不会显示坐标轴
	uint8_t Axis_Display;
	
	//是否显示网格，如果该值为0则不会显示网格
	uint8_t Net_Display;
	
	//坐标轴X范围是否是自动的，非0：是；0：不是
	uint8_t Is_XRange_Auto;
	
	//坐标轴Y范围是否是自动的，非0：是；0：不是
	uint8_t Is_YRange_Auto;
	
	//X轴刻度数量，X轴上将会均匀地显示X_Tick_Num个刻度
	uint16_t X_Tick_Num;
	
	//Y轴刻度数量，Y轴上将会均匀地显示Y_Tick_Num个刻度
	uint16_t Y_Tick_Num;
	
	//X轴上（竖直）网格密度，每X_Net_Density个刻度就会显示一条网格线
	uint8_t X_Net_Density;
	
	//Y轴上（竖直）网格密度，每Y_Net_Density个刻度就会显示一条网格线
	uint8_t Y_Net_Density;
	
	//Y轴坐标
	uint16_t _Y_Position;
	
	//X轴坐标
	uint16_t _X_Position;
	
	//X轴最大值
	float X_Max;
	
	//X轴最小值
	float X_Min;
	
	//Y轴最大值
	float Y_Max;
	
	//Y轴最小值
	float Y_Min;
	
	//网格线样式
	line Net_Line;
	
	//坐标轴样式（坐标轴固定为实线，Vacancy_Length恒定为0）
	line Axis_Line;
} axis;

/************************************************************
*波形曲线类：描述了示波器中的波形曲线的样式以及范围等参数	*
*************************************************************/
typedef struct Curve_Class
{
	//是否显示波形曲线，如果该值为0则不会显示曲线
	uint8_t Display;
	
	//X范围是否自动
	uint8_t Is_XRange_Auto;
	
	//Y范围是否自动
	uint8_t Is_YRange_Auto;
	
	//X最大值
	float X_Max;
	
	//X最小值
	float X_Min;
	
	//Y最大值
	float Y_Max;
	
	//Y最小值
	float Y_Min;
	
	//数据
	float* Data;
	
	//数据长度
	uint16_t Length;
	
	//波形曲线样式（必须Scope中Style值为SCATTER或COLORED时，Vacancy_Length恒为0）
	line Curve_Line;
} curve;

/************************************************************************
*波形曲线范围：描述了曲线的一些显示参数，所有的成员都不应被手动改变，	*
*系统会自动计算。														*
*************************************************************************/
typedef struct Curve_Range_Class
{
	//X轴起始坐标
	uint16_t _Valid_X_Begin;
	
	//X轴结束坐标
	uint16_t _Valid_X_End;
	
	//Y轴起始坐标
	uint16_t _Valid_Y_Begin;
	
	//Y轴结束坐标
	uint16_t _Valid_Y_End;
	
	//数据开始索引
	uint16_t _Begin_Index;
	
	//数据结束索引
	uint16_t _End_Index;
	
	//数据长度
	uint16_t _Length;
} curve_range;


/****************************************
*示波器类：用于显示波形、函数图像等		*
*****************************************/
struct scope_class
{
    vro layer_handler   Layer;
    vro status_flag     Active;
        status_flag     Display;
    vro status_flag     Focus;
        status_flag     Update;
        status_flag     Cover;
    vro status_flag     Stop;
    vro status_flag     Unfixed_Axis;
        status_flag     Data_Updated;
    
    vro status_flag     Operating;
    vro status_flag     InXAxisRange;
    vro status_flag     InYAxisRange;
    vro uint16_t        SaveX;
    vro uint16_t        SaveY;
    
	//曲线样式
	//SCATTER：散点图，每个数据点的位置都有一个小圆圈，并且由一个竖直线段连到X轴位置
	//COLORED：填充，曲线和X轴围成的封闭图形会被填充上曲线的颜色
	//CURVE：普通曲线或虚线（取决于Vacancy_Length参数）
	uint8_t Style;
	
	//显示区域
	//Display_Area的背景范围将会用来显示波形、坐标轴以及网格
	rectangle Display_Area;
	
	//曲线显示范围，用户不应手动更改其中的任何内容
	curve_range Curve_Range;
	
	//坐标轴
	axis Axis;
	
	//波形曲线
	curve Curve;
    
    float* Data_Buffer;
    
    vro uint16_t Data_Length;
    
    touch_area* TouchArea;
    //画布，波形图形将被绘制在这张画布上
    canvas Canvas;
    gui_controller* Controller;
    
           status_flag AllEvent_Enable;
    vro    status_flag Event_PressDown_Enable;
    vro    status_flag Event_Release_Enable;
    vro    status_flag Event_KeepPress_Enable;
    vro    status_flag Event_Click_Enable;
    vro    status_flag Event_GetFocus_Enable;
    vro    status_flag Event_LostFocus_Enable;
    vro    status_flag Event_Repaint_Enable;
    vro    status_flag Event_StopChange_Enable;
    
    vro    status_flag Event_PressDown_Happened;
    vro    status_flag Event_Release_Happened;
    vro    status_flag Event_KeepPress_Happened;
    vro    status_flag Event_Click_Happened;
    vro    status_flag Event_GetFocus_Happened;
    vro    status_flag Event_LostFocus_Happened;
    vro    status_flag Event_Repaint_Happened;
    vro    status_flag Event_StopChange_Happened;
    
    Component_Event_Callback PressDown_Callback;
    Component_Event_Callback Release_Callback;
    Component_Event_Callback KeepPress_Callback;
    Component_Event_Callback Click_Callback;
    Component_Event_Callback GetFocus_Callback;
    Component_Event_Callback LostFocus_Callback;
    Component_Event_Callback Repaint_Callback;
    Component_Event_Callback StopChange_Callback;
    
    void (*Event_Set)           (scope* scp, scope_event_type event, status_flag status);    
    void (*Active_Set)          (scope* scp, status_flag status);
    void (*Unfixed_Axis_Set)    (scope* scp, status_flag state);
    void (*Repaint)             (scope* scp);
    void (*Dispose)             (scope* scp);
};

union components_union_class
{
    button          Button;
    label           Label;
    panel           Panel;
    toolbox         ToolBox;
    toolbox_button  ToolBox_Button;
    scope           Scope;
};

struct component_class
{
    component_type      Type;
    components_union    Component;
    
    void (*Focus_Set)   (component* cm, status_flag state);
    void (*Active_Set)  (component* cm, status_flag state);
    void (*Repaint)     (component* cm);
    void (*Dispose)     (component* cm);
};

/********************************************
*GUI库初始化，为缓存分配内存				*
*********************************************/
//void GUI_Init(void);

/*void GUI_Change_WinColor(u16 Color);

void GUI_Create_Button(button** Btn, u16 X, u16 Y, u16 Width, u16 Height, char* Text);

void GUI_Delete_Button(button* Btn);

void GUI_Button_Change_Text(button* Btn, char* FormatString, ...);

void GUI_Button_Event_Enable(button* Btn, u8 Event, Event_CallbackFunc CallbackFunc);

void GUI_Button_Event_Disable(button* Btn, u8 Event);

void GUI_Button_Refresh(button* Btn);


void GUI_Create_TextBox(textbox** txt, u16 X, u16 Y, u16 Width, u16 Height, u8 Touch, char* Text);

void GUI_Delete_TextBox(textbox* txt);

void GUI_TextBox_Change_Text(textbox* txt, char* FormatString, ...);

void GUI_TextBox_Event_Enable(textbox* txt, u8 Event, Event_CallbackFunc CallbackFunc);

void GUI_TextBox_Event_Disable(textbox* txt, u8 Event);

void GUI_TextBox_Refresh(textbox* txt);


void GUI_Create_ToolBox(toolbox** tbx, u16 X, u16 Y, u16 Width, u16 Height, char* Text);

void GUI_Delete_ToolBox(toolbox* tbx);

void GUI_ToolBox_Change_Text(toolbox* tbx, char* Text);

void GUI_ToolBox_Fold_Event(toolbox* tbx, Event_CallbackFunc CallbackFunc);

void GUI_ToolBox_Unfold_Event(toolbox* tbx, Event_CallbackFunc CallbackFunc);

button* GUI_ToolBox_Add_Button(toolbox* tbx, u8 ID, char* Text);

void GUI_ToolBox_Remove_Button(toolbox* tbx, u8 ID);

button* GUI_ToolBox_Get_Button(toolbox* tbx, u8 ID);

void GUI_ToolBox_Button_Event_Enable(toolbox* tbx, u8 ID, u8 Event, Event_CallbackFunc CallbackFunc);

void GUI_ToolBox_Button_Event_Disable(toolbox* tbx, u8 ID, u8 Event);

void GUI_ToolBox_Refresh(toolbox* tbx);


void GUI_Create_Panel(panel** pnl, u16 X, u16 Y, u16 Width, u16 Height, u8 Touch, int Rows, int Lines, ...);

void GUI_Delete_Panel(panel* pnl);

void GUI_Panel_Create_Component(panel* pnl, u8 Row, u8 Line, u8 Type, u8 Touch, char* Text, void** Component);

void GUI_Panel_Delete_Component(panel* pnl, void** Component);

void GUI_Panel_Event_Enable(panel* pnl, u8 Event, Event_CallbackFunc CallbackFunc);

void GUI_Panel_Event_Disable(panel* pnl, u8 Event);*/


/*****************************************************************************************
*功能：创建一个Scope类																	 *
*参数：1.Width：宽度																	 *
*	   2.Height：高度																	 *
*	   3.FrameWidth：边框宽度															 *
*	   4.Frame_Color：边框颜色															 *
*	   5.BackColor：背景颜色，也就是示波器的背景颜色									 *
*返回值：初始化后的示波器类																 *
******************************************************************************************/
void GUI_CreateScope(scope* s, uint16_t x, uint16_t y, uint16_t Width, uint16_t Height, uint8_t FrameWidth, uint16_t FrameColor, uint16_t BackColor);

/*****************************************************************************
*功能：显示波形																 *
*参数：1.Canvas：画布地址													 *
*	   2.Scope：示波器结构体地址											 *
*	   3.X：示波器左上角相对于画布的横坐标									 *
*	   4.Y：示波器左上角相对于画布的纵坐标									 *
******************************************************************************/
void GUI_DisplayWaveform(scope* Scope, uint8_t Cover, uint8_t refresh);

#endif
