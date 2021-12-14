/**************************************************************************
*|-----------------------------ͼ���û������----------------------------|*
*|-----------------------------@����ʮ�߸߲���---------------------------|*
*|-------------------------------2021.1.23-------------------------------|*
***************************************************************************/
#ifndef _GUI_H_
#define _GUI_H_

#include "Graphic.h"
#include "Touch.h"
#include "Framework.h"
#include "LinkedList.h"

//#define USE_TOUCH

//GUI_DisplayWaveform��Cover����ʹ�õĺ꣬����ͬʱ��ʾ�������ͼ��
//CLEAR�������һ�λ��ƵĲ���
//HOLD��������һ�λ��ƵĲ���
#define CLEAR 1	//canvas.s
#define HOLD  2

//���������������͵ĺ�
#define SCATTER 1	//ɢ��
#define COLORED 2	//���
#define CURVE	3	//��ͨ����

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
*�������ࣺ������ʾ�����е������ᣨAxis��������Net������ʽ�Լ���ʾ��Χ�Ȳ���		*
*Axis��صĲ���������X���Y�����ʽ�Լ���Χ��Net��صĲ����������������ʽ���ܶ�	*
*����Ϣ��																			*
*˵��������ǰ����»��ߣ�"_"���Ĳ�Ӧ�ֶ��༭��ϵͳ���Զ�����						*
*************************************************************************************/
typedef struct Axis_Class 
{	
	//�Ƿ���ʾ�����ᣬ�����ֵΪ0�򲻻���ʾ������
	uint8_t Axis_Display;
	
	//�Ƿ���ʾ���������ֵΪ0�򲻻���ʾ����
	uint8_t Net_Display;
	
	//������X��Χ�Ƿ����Զ��ģ���0���ǣ�0������
	uint8_t Is_XRange_Auto;
	
	//������Y��Χ�Ƿ����Զ��ģ���0���ǣ�0������
	uint8_t Is_YRange_Auto;
	
	//X��̶�������X���Ͻ�����ȵ���ʾX_Tick_Num���̶�
	uint16_t X_Tick_Num;
	
	//Y��̶�������Y���Ͻ�����ȵ���ʾY_Tick_Num���̶�
	uint16_t Y_Tick_Num;
	
	//X���ϣ���ֱ�������ܶȣ�ÿX_Net_Density���̶Ⱦͻ���ʾһ��������
	uint8_t X_Net_Density;
	
	//Y���ϣ���ֱ�������ܶȣ�ÿY_Net_Density���̶Ⱦͻ���ʾһ��������
	uint8_t Y_Net_Density;
	
	//Y������
	uint16_t _Y_Position;
	
	//X������
	uint16_t _X_Position;
	
	//X�����ֵ
	float X_Max;
	
	//X����Сֵ
	float X_Min;
	
	//Y�����ֵ
	float Y_Max;
	
	//Y����Сֵ
	float Y_Min;
	
	//��������ʽ
	line Net_Line;
	
	//��������ʽ��������̶�Ϊʵ�ߣ�Vacancy_Length�㶨Ϊ0��
	line Axis_Line;
} axis;

/************************************************************
*���������ࣺ������ʾ�����еĲ������ߵ���ʽ�Լ���Χ�Ȳ���	*
*************************************************************/
typedef struct Curve_Class
{
	//�Ƿ���ʾ�������ߣ������ֵΪ0�򲻻���ʾ����
	uint8_t Display;
	
	//X��Χ�Ƿ��Զ�
	uint8_t Is_XRange_Auto;
	
	//Y��Χ�Ƿ��Զ�
	uint8_t Is_YRange_Auto;
	
	//X���ֵ
	float X_Max;
	
	//X��Сֵ
	float X_Min;
	
	//Y���ֵ
	float Y_Max;
	
	//Y��Сֵ
	float Y_Min;
	
	//����
	float* Data;
	
	//���ݳ���
	uint16_t Length;
	
	//����������ʽ������Scope��StyleֵΪSCATTER��COLOREDʱ��Vacancy_Length��Ϊ0��
	line Curve_Line;
} curve;

/************************************************************************
*�������߷�Χ�����������ߵ�һЩ��ʾ���������еĳ�Ա����Ӧ���ֶ��ı䣬	*
*ϵͳ���Զ����㡣														*
*************************************************************************/
typedef struct Curve_Range_Class
{
	//X����ʼ����
	uint16_t _Valid_X_Begin;
	
	//X���������
	uint16_t _Valid_X_End;
	
	//Y����ʼ����
	uint16_t _Valid_Y_Begin;
	
	//Y���������
	uint16_t _Valid_Y_End;
	
	//���ݿ�ʼ����
	uint16_t _Begin_Index;
	
	//���ݽ�������
	uint16_t _End_Index;
	
	//���ݳ���
	uint16_t _Length;
} curve_range;


/****************************************
*ʾ�����ࣺ������ʾ���Ρ�����ͼ���		*
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
    
	//������ʽ
	//SCATTER��ɢ��ͼ��ÿ�����ݵ��λ�ö���һ��СԲȦ��������һ����ֱ�߶�����X��λ��
	//COLORED����䣬���ߺ�X��Χ�ɵķ��ͼ�λᱻ��������ߵ���ɫ
	//CURVE����ͨ���߻����ߣ�ȡ����Vacancy_Length������
	uint8_t Style;
	
	//��ʾ����
	//Display_Area�ı�����Χ����������ʾ���Ρ��������Լ�����
	rectangle Display_Area;
	
	//������ʾ��Χ���û���Ӧ�ֶ��������е��κ�����
	curve_range Curve_Range;
	
	//������
	axis Axis;
	
	//��������
	curve Curve;
    
    float* Data_Buffer;
    
    vro uint16_t Data_Length;
    
    touch_area* TouchArea;
    //����������ͼ�ν������������Ż�����
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
*GUI���ʼ����Ϊ��������ڴ�				*
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
*���ܣ�����һ��Scope��																	 *
*������1.Width�����																	 *
*	   2.Height���߶�																	 *
*	   3.FrameWidth���߿���															 *
*	   4.Frame_Color���߿���ɫ															 *
*	   5.BackColor��������ɫ��Ҳ����ʾ�����ı�����ɫ									 *
*����ֵ����ʼ�����ʾ������																 *
******************************************************************************************/
void GUI_CreateScope(scope* s, uint16_t x, uint16_t y, uint16_t Width, uint16_t Height, uint8_t FrameWidth, uint16_t FrameColor, uint16_t BackColor);

/*****************************************************************************
*���ܣ���ʾ����																 *
*������1.Canvas��������ַ													 *
*	   2.Scope��ʾ�����ṹ���ַ											 *
*	   3.X��ʾ�������Ͻ�����ڻ����ĺ�����									 *
*	   4.Y��ʾ�������Ͻ�����ڻ�����������									 *
******************************************************************************/
void GUI_DisplayWaveform(scope* Scope, uint8_t Cover, uint8_t refresh);

#endif
