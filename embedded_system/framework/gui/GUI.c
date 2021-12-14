#include "GUI.h"
#include "Memory.h"
#include "MathHelper.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "USART.h"


#define GRAM_MODE                   0

#define GUI_HEAP	HEAP1

//GUI�������ڵĶѿռ�
#define GUI_BUFFER_HEAP HEAP1

//GUI�����С��ʵ���ϻ���������СΪGUI_BUFFER_SIZE�Ķ�������
//ʾ��������ʾ�����������ֵ��С�йأ�������=GUI_BUFFER_SIZE/4
#define GUI_BUFFER_SIZE 4096

#define COLOR_UNFOCUS	0xb596
#define COLOR_FRAME		0x4a69
#define COLOR_FOCUS		0xc618
#define COLOR_TEXTBOX	0xa534
#define COLOR_TEXTFRAME	BLACK

//GUI����0
void* GUI_Buffer0;
//GUI����1
void* GUI_Buffer1;



#define ROUND(x)                ((int32_t)(x+0.5f))
#define GET_COMPONENT_ADDR(x)   ((component*)(((uint32_t)(x))-4))

/*GUI Controller*/
layer_handler   Method_GUI_Controller_Add_Layer            (gui_controller* guictrl, uint16_t color);
void            Method_GUI_Controller_Remove_Layer         (gui_controller* guictrl, layer_handler layer);
void            Method_GUI_Controller_ActiveLayer_Set      (gui_controller* guictrl, layer_handler layer);
button*         Method_GUI_Controller_Create_Button        (gui_controller* guictrl, uint16_t x, uint16_t y, uint16_t width, uint16_t height, status_flag virt, char* text, layer_handler layer);
label*          Method_GUI_Controller_Create_Label         (gui_controller* guictrl, uint16_t x, uint16_t y, uint16_t width, uint16_t height, status_flag virt, char* text, layer_handler layer);
panel*          Method_GUI_COntroller_Create_Panel         (gui_controller* guictrl, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t rownum, uint8_t linenum, layer_handler layer);
toolbox*        Method_GUI_Controller_Create_ToolBox       (gui_controller* guictrl, uint16_t x, uint16_t y, uint16_t width, uint16_t height, status_flag virt, char* text, layer_handler layer);
scope*          Method_GUI_Controller_Create_Scope         (gui_controller* guictrl, uint16_t x, uint16_t y, uint16_t width, uint16_t height, status_flag virt, uint16_t max_dlen, status_flag vscale, layer_handler layer); 
void            Method_GUI_Controller_Process              (gui_controller* guictrl);


/*Component*/
void            Method_Component_Repaint    (component* cm);
void            Method_Component_Dispose    (component* cm);
void            Method_Component_FocusSet   (component* cm, status_flag state);
void            Method_Component_ActiveSet  (component* cm, status_flag state);

void            _Layer_Init                 (component_layer* cl, uint16_t color);

void            Global_PressDown_Callback   (touch_device* td, touch_area* ta);
void            Global_Release_Callback     (touch_device* td, touch_area* ta);
void            Global_KeepPress_Callback   (touch_device* td, touch_area* ta);
void            Global_Click_Callback       (touch_device* td, touch_area* ta);
void            Overall_Click_Callback      (touch_device* td, touch_area* ta);
    

/********************************************************************************************************/
/*                                              Button                                                  */
/********************************************************************************************************/

void            Method_Button_Event_Set     (button* btn, button_event_type event, status_flag status);
void            Method_Button_Active_Set    (button* btn, status_flag status);
void            Method_Button_Change_Text   (button* btn, char* fmtstr, ...);
void            Method_Button_Repaint       (button* btn);
void            Method_Button_Dispose       (button* btn);

void            _Button_Init                (button* btn, touch_device* td, uint16_t x, uint16_t y, uint16_t width, uint16_t height, status_flag virt, char* text, layer_handler layer);


/********************************************************************************************************/
/*                                              Label                                                   */
/********************************************************************************************************/

void            Method_Label_Event_Set     (label* lbl, label_event_type event, status_flag status);
void            Method_Label_Active_Set    (label* lbl, status_flag status);
void            Method_Label_Change_Text   (label* lbl, char* fmtstr, ...);
void            Method_Label_Repaint       (label* lbl);
void            Method_Label_Dispose       (label* lbl);

void            _Label_Init                (label* lbl, touch_device* td, uint16_t x, uint16_t y, uint16_t width, uint16_t height, status_flag virt, char* text, layer_handler layer);


/********************************************************************************************************/
/*                                              Panel                                                   */
/********************************************************************************************************/
void            Method_Panel_Data_Update               (panel* pnl);
void            Method_Panel_Dispose                   (panel* pnl);

void            _Panel_Init                            (panel* pnl, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t rownum, uint8_t linenum, layer_handler layer);
void            _Panel_Data_Calculate                  (panel* pnl);


/********************************************************************************************************/
/*                                              ToolBox                                                 */
/********************************************************************************************************/
void            Method_ToolBox_Event_Set                (toolbox* tbx, toolbox_event_type event, status_flag status);
void            Method_ToolBox_Active_Set               (toolbox* tbx, status_flag status);
void            Method_ToolBox_Change_Text              (toolbox* tbx, char* fmtstr, ...);
toolbox_button* Method_ToolBox_Add_Button               (toolbox* tbx, char* text);
void            Method_ToolBox_Delete_Button            (toolbox* tbx, toolbox_button* tbx_btn);
void            Method_ToolBox_Repaint                  (toolbox* tbx);
void            Method_ToolBox_Dispose                  (toolbox* tbx);

void            _ToolBox_Init                           (toolbox* tbx, touch_device* td, uint16_t x, uint16_t y, uint16_t width, uint16_t height, status_flag virt, char* text, layer_handler layer);
void            _ToolBox_Button_Init                    (toolbox_button* tbx_btn, touch_device* td, uint16_t width, uint16_t height, status_flag virt, char* text);
void            _ToolBox_Button_Repermutation           (toolbox* tbx);

void            Method_ToolBox_Button_Event_Set           (toolbox_button* tbx_btn, toolbox_event_type event, status_flag status);    
void            Method_ToolBox_Button_Active_Set          (toolbox_button* tbx_btn, status_flag status);
void            Method_ToolBox_Button_Change_Text         (toolbox_button* tbx_btn, char* fmtstr, ...);


/********************************************************************************************************/
/*                                              Scope                                                   */
/********************************************************************************************************/
void            Method_Scope_Event_Set                (scope* scp, scope_event_type event, status_flag status);
void            Method_Scope_Active_Set               (scope* scp, status_flag status);
void            Method_Scope_Unfixed_Axis_Set         (scope* scp, status_flag state);
void            Method_Scope_Repaint                  (scope* scp);
void            Method_Scope_Dispose                  (scope* scp);
    
void            _Scope_Init                           (scope* scp, touch_device* td, uint16_t x, uint16_t y, uint16_t width, uint16_t height, status_flag virt, uint16_t max_dlen, status_flag vscale, layer_handler layer);
void            _Scope_CurveRange_Update              (float* data, uint16_t dlen,axis* ax, curve* cur, curve_range* cur_range, rectangle* area);
void            _Scope_Data_Prepare                   (float* datin, float* datout, axis* ax, curve_range* cur_range);
void            _Scope_Draw_Waveform                  (canvas* c, float* dat, line* l, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t axis_pos, uint16_t dlen, uint8_t style);
void            _Scope_Repaint                        (scope* scp);
    
    
void _Layer_Init(component_layer* cl, uint16_t color)
{
    cl->Active=Disable;
    cl->Color=color;
    LinkedList_Prepare(&cl->Components,sizeof(components_union));
}

layer_handler Method_GUI_Controller_Add_Layer(struct gui_controller_class* guictrl, uint16_t color)
{
    component_layer* cl;

    cl=LinkedList_Add(&guictrl->Layers,guictrl->Layers.Nodes_Num);
    _Layer_Init(cl,color);
    return guictrl->Layers.Nodes_Num-1;
}

void Method_GUI_Controller_Remove_Layer(struct gui_controller_class* guictrl, layer_handler layer)
{
    int i;
    component_layer* cl;
    component* cm;
    
    //�ҵ���
    cl=LinkedList_Find(&guictrl->Layers,layer);
    for (i=0;i<cl->Components.Nodes_Num;i++)    //�Ƴ�����ÿ���ؼ�
    {
        cm=LinkedList_Find(&cl->Components,i);
        cm->Dispose(cm);
    }
    LinkedList_Dispose(&guictrl->Layers,layer);  //�Ƴ���
}

void Method_GUI_Controller_ActiveLayer_Set(gui_controller* guictrl, layer_handler layer)
{
    uint16_t i;
    component_layer* cl;
    component* cm;
    
    //�ҵ���ǰ��Ծ��
    cl=LinkedList_Find(&guictrl->Layers,guictrl->Active_Layer);
    for (i=0;i<cl->Components.Nodes_Num;i++)
    {
        cm=LinkedList_Find(&cl->Components,i);
        cm->Active_Set(cm,Disable);
    }
    
    //�ҵ���
    cl=LinkedList_Find(&guictrl->Layers,layer);
    for (i=0;i<cl->Components.Nodes_Num;i++)
    {
        cm=LinkedList_Find(&cl->Components,i);
        cm->Active_Set(cm,Enable);
    }
    //���Ļ�Ծ��
    _RO_WRITE(guictrl->Active_Layer,layer_handler,layer);
    //���±��
    guictrl->Updated=Enable;
}

button* Method_GUI_Controller_Create_Button(struct gui_controller_class* guictrl, uint16_t x, uint16_t y, uint16_t width, uint16_t height, status_flag virt, char* text, layer_handler layer)
{
    component_layer* cl;
    component* cm;
    
    //��ȡ��
    cl=LinkedList_Find(&guictrl->Layers,layer);
    //�½��ؼ�
    cm=LinkedList_Add(&cl->Components,cl->Components.Nodes_Num);
    
    //�󶨷���
    cm->Repaint=Method_Component_Repaint;
    cm->Active_Set=Method_Component_ActiveSet;
    cm->Focus_Set=Method_Component_FocusSet;
    cm->Dispose=Method_Component_Dispose;
    
    
    //ת��Ϊbutton
    cm->Type=BUTTON;
    //button������ʼ��
    _Button_Init(&cm->Component.Button,guictrl->TouchDevice,x,y,width,height,virt,text,layer);
    //�봥�����򴴽�����
    cm->Component.Button.TouchArea->Association=cm;
    //�����������
    cm->Component.Button.Controller=guictrl;
    
    return &cm->Component.Button;
}

label* Method_GUI_Controller_Create_Label(gui_controller* guictrl, uint16_t x, uint16_t y, uint16_t width, uint16_t height, status_flag virt, char* text, layer_handler layer)
{
    component_layer* cl;
    component* cm;
    
    //��ȡ��
    cl=LinkedList_Find(&guictrl->Layers,layer);
    //�½��ؼ�
    cm=LinkedList_Add(&cl->Components,cl->Components.Nodes_Num);
    
    //�󶨷���
    cm->Repaint=Method_Component_Repaint;
    cm->Active_Set=Method_Component_ActiveSet;
    cm->Focus_Set=Method_Component_FocusSet;
    cm->Dispose=Method_Component_Dispose;
    
    //ת��Ϊlabel
    cm->Type=LABEL;
    //label������ʼ��
    _Label_Init(&cm->Component.Label,guictrl->TouchDevice,x,y,width,height,virt,text,layer);
    //�����������
    cm->Component.Label.Controller=guictrl;
    
    return &cm->Component.Label;
}

panel* Method_GUI_COntroller_Create_Panel(gui_controller* guictrl, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t rownum, uint8_t linenum, layer_handler layer)
{
    component_layer* cl;
    component* cm;
    
    //��ȡ��
    cl=LinkedList_Find(&guictrl->Layers,layer);
    //�½��ؼ�
    cm=LinkedList_Add(&cl->Components,cl->Components.Nodes_Num);
    
    //�󶨷���
    cm->Repaint=Method_Component_Repaint;
    cm->Active_Set=Method_Component_ActiveSet;
    cm->Focus_Set=Method_Component_FocusSet;
    cm->Dispose=Method_Component_Dispose;
    
    //ת��Ϊpanel
    cm->Type=PANEL;
    //panel������ʼ��
    _Panel_Init(&cm->Component.Panel,x,y,width,height,rownum,linenum,layer);
    //�����������
    cm->Component.Panel.Controller=guictrl;
    
    return &cm->Component.Panel;
}

toolbox* Method_GUI_Controller_Create_ToolBox(gui_controller* guictrl, uint16_t x, uint16_t y, uint16_t width, uint16_t height, status_flag virt, char* text, layer_handler layer)
{
    component_layer* cl;
    component* cm;
    
    //��ȡ��
    cl=LinkedList_Find(&guictrl->Layers,layer);
    //�½��ؼ�
    cm=LinkedList_Add(&cl->Components,cl->Components.Nodes_Num);
    
    //�󶨷���
    cm->Repaint=Method_Component_Repaint;
    cm->Active_Set=Method_Component_ActiveSet;
    cm->Focus_Set=Method_Component_FocusSet;
    cm->Dispose=Method_Component_Dispose;
    
    //ת��Ϊtoolbox
    cm->Type=TOOLBOX;
    //toolbox������ʼ��
    _ToolBox_Init(&cm->Component.ToolBox,guictrl->TouchDevice,x,y,width,height,virt,text,layer);
    //�봥�����򴴽�����
    cm->Component.ToolBox.TouchArea->Association=cm;
    //�����������
    cm->Component.ToolBox.Controller=guictrl;
    
    return &cm->Component.ToolBox;
}

scope* Method_GUI_Controller_Create_Scope(gui_controller* guictrl, uint16_t x, uint16_t y, uint16_t width, uint16_t height, status_flag virt, uint16_t max_dlen, status_flag vscale, layer_handler layer)
{
    component_layer* cl;
    component* cm;
    
    //��ȡ��
    cl=LinkedList_Find(&guictrl->Layers,layer);
    //�½��ؼ�
    cm=LinkedList_Add(&cl->Components,cl->Components.Nodes_Num);
    
    //�󶨷���
    cm->Repaint=Method_Component_Repaint;
    cm->Active_Set=Method_Component_ActiveSet;
    cm->Focus_Set=Method_Component_FocusSet;
    cm->Dispose=Method_Component_Dispose;
    
    //ת��Ϊscope
    cm->Type=SCOPE;
    //button������ʼ��
    _Scope_Init(&cm->Component.Scope,guictrl->TouchDevice,x,y,width,height,virt,max_dlen,Disable,layer);
    //�봥�����򴴽�����
    cm->Component.Scope.TouchArea->Association=cm;
    //�����������
    cm->Component.Scope.Controller=guictrl;
    
    return &cm->Component.Scope;
}











void Method_GUI_Controller_Process(gui_controller* guictrl)
{
    int i,j;
    status_flag AnyCompUpdated=Disable;
    status_flag ToolboxFocus=Disable;   //�������ý���
    component* cm;
    component_layer* cl;
    
    //������Ϣ����
    guictrl->TouchDevice->Process(guictrl->TouchDevice);
    cl=LinkedList_Find(&guictrl->Layers,guictrl->Active_Layer);
    for (i=0;i<cl->Components.Nodes_Num;i++)
    {
        cm=LinkedList_Find(&cl->Components,i);
        switch (cm->Type)
        {
            //������Button���¼�����
            case BUTTON:
            {
                button* btn=&cm->Component.Button;
                if (btn->Event_PressDown_Happened)
                {
                    if (btn->AllEvent_Enable&&btn->Event_PressDown_Enable)
                        btn->PressDown_Callback(btn);
                    _RO_WRITE(btn->Event_PressDown_Happened,status_flag,Disable);
                }
                if (btn->Event_Release_Happened)
                {
                    if (btn->AllEvent_Enable&&btn->Event_Release_Enable)
                        btn->Release_Callback(btn);
                    _RO_WRITE(btn->Event_Release_Happened,status_flag,Disable);
                }
                if (btn->Event_KeepPress_Happened)
                {
                    if (btn->AllEvent_Enable&&btn->Event_KeepPress_Enable)
                        btn->KeepPress_Callback(&cm->Component);
                    _RO_WRITE(btn->Event_KeepPress_Happened,status_flag,Disable);
                }
                if (btn->Event_Click_Happened)
                {
                    if (!btn->Focus)    //��ý���
                        _RO_WRITE(btn->Event_GetFocus_Happened,status_flag,Enable);
                    else    //ʧȥ����
                        _RO_WRITE(btn->Event_LostFocus_Happened,status_flag,Enable);
                    //��������¼�ʹ��
                    if (btn->AllEvent_Enable&&btn->Event_Click_Enable)
                        btn->Click_Callback(btn);
                    _RO_WRITE(btn->Event_Click_Happened,status_flag,Disable);
                }
                if (btn->Event_GetFocus_Happened)
                {
                    if (guictrl->Focus_Component!=NULL)
                        guictrl->Focus_Component->Focus_Set(guictrl->Focus_Component,Disable);
                    _RO_WRITE(btn->Focus,status_flag,Enable);
                    _RO_WRITE(guictrl->Focus_Component,component*,cm);
                    if (btn->AllEvent_Enable&&btn->Event_GetFocus_Enable)
                        btn->GetFocus_Callback(btn);
                    _RO_WRITE(btn->Event_GetFocus_Happened,status_flag,Disable);
                }
                if (btn->Event_LostFocus_Happened)
                {
                    _RO_WRITE(btn->Focus,status_flag,Disable);
                    _RO_WRITE(guictrl->Focus_Component,component*,NULL);
                    if (btn->AllEvent_Enable&&btn->Event_LostFocus_Enable)
                        btn->LostFocus_Callback(btn);
                    _RO_WRITE(btn->Event_LostFocus_Happened,status_flag,Disable);
                }
                if (btn->Updated&&(!guictrl->Updated))
                {
                    btn->Repaint(btn);
                    btn->Updated=Disable;
                    AnyCompUpdated=Enable;
                }
                else if (guictrl->Updated)
                    btn->Updated=Disable;
                break;
            }
            //��ǩ��Label���¼�����
            case LABEL:
            {
                label* lbl=&cm->Component.Label;
                if (lbl->Updated&&(!guictrl->Updated))
                {
                    lbl->Repaint(lbl);
                    lbl->Updated=Disable;
                    AnyCompUpdated=Enable;
                }
                else if (guictrl->Updated)
                    lbl->Updated=Disable;
                break;
            }
            //�����䣨ToolBox���¼�����
            case TOOLBOX:
            {
                toolbox* tbx=&cm->Component.ToolBox;
                toolbox_button* tbx_btn;
                for (j=0;j<tbx->Buttons.Nodes_Num;j++)
                {
                    tbx_btn=&((component*)LinkedList_Find(&tbx->Buttons,j))->Component.ToolBox_Button;
                    if (tbx_btn->Event_Click_Happened)
                    {
                        if (tbx_btn->AllEvent_Enable&&tbx_btn->Event_Click_Enable)
                            tbx_btn->Click_Callback(tbx_btn);
                        _RO_WRITE(tbx_btn->Event_Click_Happened,status_flag,Disable);
                    }
                }
                
                if (tbx->Event_PressDown_Happened)
                {
                    if (tbx->AllEvent_Enable&&tbx->Event_PressDown_Enable)
                        tbx->PressDown_Callback(tbx);
                    _RO_WRITE(tbx->Event_PressDown_Happened,status_flag,Disable);
                }
                if (tbx->Event_Release_Happened)
                {
                    if (tbx->AllEvent_Enable&&tbx->Event_Release_Enable)
                        tbx->Release_Callback(tbx);
                    _RO_WRITE(tbx->Event_Release_Happened,status_flag,Disable);
                }
                if (tbx->Event_KeepPress_Happened)
                {
                    if (tbx->AllEvent_Enable&&tbx->Event_KeepPress_Enable)
                        tbx->KeepPress_Callback(&cm->Component);
                    _RO_WRITE(tbx->Event_KeepPress_Happened,status_flag,Disable);
                }
                if (tbx->Event_Click_Happened)
                {
                    if (!tbx->Focus)    //��ý���
                        _RO_WRITE(tbx->Event_GetFocus_Happened,status_flag,Enable);
                    //��������¼�ʹ��
                    if (tbx->AllEvent_Enable&&tbx->Event_Click_Enable)
                        tbx->Click_Callback(tbx);
                    _RO_WRITE(tbx->Event_Click_Happened,status_flag,Disable);
                }
                if (tbx->Event_GetFocus_Happened)
                {
                    
                    if (guictrl->Focus_Component!=NULL)
                        guictrl->Focus_Component->Focus_Set(guictrl->Focus_Component,Disable);
                    _RO_WRITE(tbx->Focus,status_flag,Enable);
                    _RO_WRITE(guictrl->Focus_Component,component*,cm);
                    //�����䱻�򿪣�ȫ��ˢ��
                    guictrl->Updated=Enable;
                    //ȫ�ִ��������¼�ʹ��
                    guictrl->Overall_TouchArea->All_Event_Enable=Enable;
                    if (tbx->AllEvent_Enable&&tbx->Event_GetFocus_Enable)
                        tbx->GetFocus_Callback(tbx);
                    _RO_WRITE(tbx->Event_GetFocus_Happened,status_flag,Disable);
                    
                    //���пؼ������¼�ʧ��
                    component* cm;
                    for (j=0;j<cl->Components.Nodes_Num;j++)
                    {
                        cm=LinkedList_Find(&cl->Components,j);
                        cm->Active_Set(cm,Disable);
                    }
                    //���а��������¼�ʹ��
                    for (j=0;j<tbx->Buttons.Nodes_Num;j++)
                    {
                        cm=LinkedList_Find(&tbx->Buttons,j);
                        cm->Component.ToolBox_Button.TouchArea->All_Event_Enable=Enable;
                    }
                }
                if (tbx->Event_LostFocus_Happened)
                {
                    _RO_WRITE(guictrl->Focus_Component,component*,NULL);
                    _RO_WRITE(tbx->Focus,status_flag,Disable);
                    //�����䱻�رգ�ȫ��ˢ��
                    guictrl->Updated=Enable;
                    //ȫ�ִ��������¼�ʧ��
                    guictrl->Overall_TouchArea->All_Event_Enable=Disable;
                    if (tbx->AllEvent_Enable&&tbx->Event_LostFocus_Enable)
                        tbx->LostFocus_Callback(tbx);
                    _RO_WRITE(tbx->Event_LostFocus_Happened,status_flag,Disable);
                    
                    //���пؼ������¼�ʹ��
                    component* cm;
                    for (j=0;j<cl->Components.Nodes_Num;j++)
                    {
                        cm=LinkedList_Find(&cl->Components,j);
                        cm->Active_Set(cm,Enable);
                    }
                    //���а��������¼�ʧ��
                    for (j=0;j<tbx->Buttons.Nodes_Num;j++)
                    {
                        cm=LinkedList_Find(&tbx->Buttons,j);
                        cm->Component.ToolBox_Button.TouchArea->All_Event_Enable=Disable;
                    }
                }
                if (tbx->Updated&&(!guictrl->Updated))
                {
                    tbx->Repaint(tbx);
                    tbx->Updated=Disable;
                    AnyCompUpdated=Enable;
                }
                else if (guictrl->Updated)
                    tbx->Updated=Disable;
                break;
            }
            case SCOPE:
            {
                scope* scp=&cm->Component.Scope;
                if (scp->Event_PressDown_Happened)
                {
                    //����ǿɱ�������
                    if (scp->Unfixed_Axis)
                    {
                        _RO_WRITE(scp->SaveX,uint16_t,scp->TouchArea->PointX);
                        _RO_WRITE(scp->SaveY,uint16_t,scp->TouchArea->PointY);
                        _RO_WRITE(scp->Operating,status_flag,Enable);    //���ڲ���
                        if ((scp->SaveX<scp->Axis._Y_Position+10)&&(scp->SaveX>scp->Axis._Y_Position-10))   //��Y�����
                        {
                            _RO_WRITE(scp->InYAxisRange,status_flag,Enable);
                            _RO_WRITE(scp->InXAxisRange,status_flag,Disable);
                        }
                        else if ((scp->SaveY<scp->Axis._X_Position+10)&&(scp->SaveY>scp->Axis._X_Position-10))  //��X�����
                        {
                            _RO_WRITE(scp->InXAxisRange,status_flag,Enable);
                            _RO_WRITE(scp->InYAxisRange,status_flag,Disable);
                        }
                    }
                    
                    if (scp->AllEvent_Enable&&scp->Event_PressDown_Enable)
                        scp->PressDown_Callback(scp);
                    _RO_WRITE(scp->Event_PressDown_Happened,status_flag,Disable);
                }
                if (scp->Event_Release_Happened)
                {
                    //����ǿɱ�������
                    if (scp->Unfixed_Axis)
                    {
                        _RO_WRITE(scp->Operating,status_flag,Disable);
                        _RO_WRITE(scp->InXAxisRange,status_flag,Disable);
                        _RO_WRITE(scp->InYAxisRange,status_flag,Disable);
                    }
                    
                    if (scp->AllEvent_Enable&&scp->Event_Release_Enable)
                        scp->Release_Callback(scp);
                    _RO_WRITE(scp->Event_Release_Happened,status_flag,Disable);
                }
                if (scp->Event_KeepPress_Happened)
                {
                    //����ǿɱ�������
                    if (scp->Unfixed_Axis)
                    {
                        //���ڲ���
                        if (scp->Operating)
                        {
                            short deltax,deltay;
                            
                            deltax=scp->TouchArea->PointX-scp->SaveX;
                            deltay=scp->TouchArea->PointY-scp->SaveY;
                            if (scp->InXAxisRange)  //��X�����
                            {
                                deltax*=-1;
                                scp->Axis.X_Max*=1+(float)deltax/scp->Canvas.Area.Width;
                                scp->Axis.X_Min*=1+(float)deltax/scp->Canvas.Area.Width;
                            }
                            else if (scp->InYAxisRange) //��Y�����
                            {
                                deltay*=-1;
                                scp->Axis.Y_Max*=1+(float)deltay/scp->Canvas.Area.Height;
                                scp->Axis.Y_Min*=1+(float)deltay/scp->Canvas.Area.Height;
                            }
                            else
                            {
                                deltax*=-1;
                                scp->Axis.X_Max+=(float)deltax*(scp->Axis.X_Max-scp->Axis.X_Min)/scp->Canvas.Area.Width;
                                scp->Axis.X_Min+=(float)deltax*(scp->Axis.X_Max-scp->Axis.X_Min)/scp->Canvas.Area.Width;
                                scp->Axis.Y_Max+=(float)deltay*(scp->Axis.Y_Max-scp->Axis.Y_Min)/scp->Canvas.Area.Height;
                                scp->Axis.Y_Min+=(float)deltay*(scp->Axis.Y_Max-scp->Axis.Y_Min)/scp->Canvas.Area.Height;
                            }
                            
                            _RO_WRITE(scp->SaveX,uint16_t,scp->TouchArea->PointX);
                            _RO_WRITE(scp->SaveY,uint16_t,scp->TouchArea->PointY);
                        }
                    }
                    
                    if (scp->AllEvent_Enable&&scp->Event_KeepPress_Enable)
                        scp->KeepPress_Callback(&cm->Component);
                    _RO_WRITE(scp->Event_KeepPress_Happened,status_flag,Disable);
                }
                if (scp->Event_Click_Happened)
                {
                    if (scp->Unfixed_Axis)
                    {
                        if (scp->Stop)
                            _RO_WRITE(scp->Stop,status_flag,Disable);
                        else
                            _RO_WRITE(scp->Stop,status_flag,Enable);
                        scp->Update=Enable;
                    }
                    if (!scp->Focus)    //��ý���
                        _RO_WRITE(scp->Event_GetFocus_Happened,status_flag,Enable);
                    else    //ʧȥ����
                        _RO_WRITE(scp->Event_LostFocus_Happened,status_flag,Enable);
                    //��������¼�ʹ��
                    if (scp->AllEvent_Enable&&scp->Event_Click_Enable)
                        scp->Click_Callback(scp);
                    _RO_WRITE(scp->Event_Click_Happened,status_flag,Disable);
                }
                if (scp->Event_GetFocus_Happened)
                {
                    if (guictrl->Focus_Component!=NULL)
                        guictrl->Focus_Component->Focus_Set(guictrl->Focus_Component,Disable);
                    _RO_WRITE(scp->Focus,status_flag,Enable);
                    _RO_WRITE(guictrl->Focus_Component,component*,cm);
                    if (scp->AllEvent_Enable&&scp->Event_GetFocus_Enable)
                        scp->GetFocus_Callback(scp);
                    _RO_WRITE(scp->Event_GetFocus_Happened,status_flag,Disable);
                }
                if (scp->Event_LostFocus_Happened)
                {
                    _RO_WRITE(scp->Focus,status_flag,Disable);
                    _RO_WRITE(guictrl->Focus_Component,component*,NULL);
                    if (scp->AllEvent_Enable&&scp->Event_LostFocus_Enable)
                        scp->LostFocus_Callback(scp);
                    _RO_WRITE(scp->Event_LostFocus_Happened,status_flag,Disable);
                }
                if (scp->Update&&(!guictrl->Updated))
                {
                    scp->Repaint(scp);
                    scp->Update=Disable;
                    AnyCompUpdated=Enable;
                }
                else if (guictrl->Updated)
                    scp->Update=Disable;
                break;
            }
            default:
                break;
        }
    }
    
    //����ǳ�ʼ������
    if (guictrl->IsInit)
    {
        component* cm;
        //���Ʊ���
        Graphic_Draw_Color(&guictrl->Screen->Canvas,cl->Color);
        for (i=0;i<cl->Components.Nodes_Num;i++)
        {
            cm=LinkedList_Find(&cl->Components,i);
            cm->Repaint(cm);
        }
        Graphic_Screen_Refresh();
        _RO_WRITE(guictrl->IsInit,status_flag,Disable);
    }
    else if (guictrl->Updated)  //ȫ��ˢ��
    {
        component* cm;
        //���Ʊ���
        Graphic_Draw_Color(&guictrl->Screen->Canvas,cl->Color);
        //���ƿؼ�
        for (i=0;i<cl->Components.Nodes_Num;i++)
        {
            cm=LinkedList_Find(&cl->Components,i);
            cm->Repaint(cm);
        }
        Graphic_Screen_Refresh();
        guictrl->Updated=Disable;
    }
    else
    {
        //������κοؼ�����
        if (AnyCompUpdated)
        {
            //����й�����ؼ���չ��״̬���ػ�
            if ((guictrl->Focus_Component!=NULL)&&(guictrl->Focus_Component->Type==TOOLBOX))
                guictrl->Focus_Component->Component.ToolBox.Repaint(&guictrl->Focus_Component->Component.ToolBox);
            Graphic_Screen_Refresh();
        }
    }
}





















void Method_Component_Repaint(component* cm)
{
    if (cm->Type==BUTTON)
        cm->Component.Button.Repaint(&cm->Component.Button);
    else if (cm->Type==LABEL)
        cm->Component.Label.Repaint(&cm->Component.Label);
    else if (cm->Type==TOOLBOX)
        cm->Component.ToolBox.Repaint(&cm->Component.ToolBox);
    else if (cm->Type==SCOPE)
        cm->Component.Scope.Repaint(&cm->Component.Scope);
}

void Method_Component_Dispose(component* cm)
{
    if (cm->Type==BUTTON)
        cm->Component.Button.Dispose(&cm->Component.Button);
    else if (cm->Type==LABEL)
        cm->Component.Label.Dispose(&cm->Component.Label);
    else if (cm->Type==TOOLBOX)
        cm->Component.ToolBox.Dispose(&cm->Component.ToolBox);
    else if (cm->Type==SCOPE)
        cm->Component.Scope.Dispose(&cm->Component.Scope);
}

void Method_Component_FocusSet(component* cm, status_flag state)
{
    if (cm->Type==BUTTON)
        _RO_WRITE(cm->Component.Button.Focus,status_flag,state);
    else if (cm->Type==TOOLBOX)
        _RO_WRITE(cm->Component.ToolBox.Focus,status_flag,state);
    else if (cm->Type==SCOPE)
        _RO_WRITE(cm->Component.Scope.Focus,status_flag,state);
}

void Method_Component_ActiveSet(component* cm, status_flag state)
{
    if (cm->Type==BUTTON)
        cm->Component.Button.Active_Set(&cm->Component.Button,state);
    else if (cm->Type==LABEL)
        cm->Component.Label.Active_Set(&cm->Component.Label,state);
    else if (cm->Type==TOOLBOX)
        cm->Component.ToolBox.Active_Set(&cm->Component.ToolBox,state);
    else if (cm->Type==SCOPE)
        cm->Component.Scope.Active_Set(&cm->Component.Scope,state);
}





















void Method_Button_Event_Set(button* btn, button_event_type event, status_flag status)
{
    switch (event)
    {
        case BTN_PRESSDOWN:
        {
            _RO_WRITE(btn->Event_PressDown_Enable,status_flag,status);
            btn->TouchArea->PressDown_Enable=status;
            break;
        }
        case BTN_RELEASE:
        {
            _RO_WRITE(btn->Event_Release_Enable,status_flag,status);
            btn->TouchArea->Release_Enable=status;
            break;
        }
        case BTN_KEEPPRESS:
        {
            _RO_WRITE(btn->Event_KeepPress_Enable,status_flag,status);
            btn->TouchArea->KeepPress_Enable=status;
            break;
        }
        case BTN_CLICK:
        {
            _RO_WRITE(btn->Event_Click_Enable,status_flag,status);
            break;
        }
        case BTN_GETFOCUS:
        {
            _RO_WRITE(btn->Event_GetFocus_Enable,status_flag,status);
            break;
        }
        case BTN_LOSTFOCUS:
        {
            _RO_WRITE(btn->Event_LostFocus_Enable,status_flag,status);
            break;
        }
        case BTN_REPAINT:
        {
            _RO_WRITE(btn->Event_Repaint_Enable,status_flag,status);
            break;
        }
        default:
            break;
    }
}

void Method_Button_Active_Set(button* btn, status_flag status)
{
    _RO_WRITE(btn->Active,status_flag,status);
    btn->TouchArea->All_Event_Enable=status;
}

void Method_Button_Change_Text(button* btn, char* fmtstr, ...)
{
    va_list vl;
    
    va_start(vl,fmtstr);
    vsprintf(btn->Text,fmtstr,vl);
    va_end(vl);
    btn->Updated=Enable;
}

void Method_Button_Repaint(button* btn)
{
    if (btn->Display)
    {
        //�����ʾ�û�����
        if (btn->CustomBackground)
            Graphic_Draw_BMP_All(&btn->Canvas,0,0,btn->Background);
        else
        {
            Graphic_Draw_Rectangle(&btn->Canvas,0,0,&btn->Appearance);
            Graphic_Draw_String(&btn->Canvas,
                                btn->Appearance.Frame_Thickness,btn->Appearance.Frame_Thickness,
                                btn->Canvas.Area.Width-2*btn->Appearance.Frame_Thickness,btn->Canvas.Area.Height-2*btn->Appearance.Frame_Thickness,
                                btn->Vertical_Align,btn->Horizontal_Align,&btn->Font,btn->Text);
        }
        if (btn->Active&&btn->AllEvent_Enable&&btn->Event_Repaint_Enable)
            btn->Repaint_Callback(btn);
    }
    else
    {
        component_layer* cl;
        cl=LinkedList_Find(&btn->Controller->Layers,btn->Controller->Active_Layer);
        Graphic_Draw_Color(&btn->Canvas,cl->Color);
    }
    Graphic_RefreshCanvas(&btn->Canvas);
}

void Method_Button_Dispose(button* btn)
{
    component* cm=GET_COMPONENT_ADDR(btn);
    component_layer* cl=LinkedList_Find(&btn->Controller->Layers,btn->Layer);
    
    if (btn->Controller->Focus_Component==cm)
        _RO_WRITE(btn->Controller->Focus_Component,component*,NULL);
    //ɾ����������
    btn->Controller->TouchDevice->Delete_Area(btn->Controller->TouchDevice,btn->TouchArea);
    //ɾ������
    Graphic_Canvas_Delete(&btn->Canvas);
    //ɾ���ؼ�
    LinkedList_Dispose2(&cl->Components,cm);
}

void _Button_Init(button* btn, touch_device* td, uint16_t x, uint16_t y, uint16_t width, uint16_t height, status_flag virt, char* text, layer_handler layer)
{    
    btn->CustomBackground=Disable;
    btn->Display=Enable;
    btn->AllEvent_Enable=Disable;
    btn->Updated=Disable;
    _RO_WRITE(btn->Active,status_flag,Enable);
    _RO_WRITE(btn->Focus,status_flag,Disable);
    _RO_WRITE(btn->Event_PressDown_Enable,status_flag,Disable);
    _RO_WRITE(btn->Event_Release_Enable,status_flag,Disable);
    _RO_WRITE(btn->Event_KeepPress_Enable,status_flag,Disable);
    _RO_WRITE(btn->Event_Click_Enable,status_flag,Disable);
    _RO_WRITE(btn->Event_GetFocus_Enable,status_flag,Disable);
    _RO_WRITE(btn->Event_LostFocus_Enable,status_flag,Disable);
    _RO_WRITE(btn->Event_Repaint_Enable,status_flag,Disable);
    
    btn->Font.BackColor=WHITE;
    btn->Font.CharColor=BLACK;
    btn->Font.Mold=BasicChar_Courier_New;
    
    btn->Appearance.Background_Color=WHITE;
    btn->Appearance.Display_Background=Enable;
    btn->Appearance.Frame_Color=BLACK;
    btn->Appearance.Frame_Thickness=5;
    btn->Appearance.Height=height;
    btn->Appearance.Width=width;
    
    btn->Vertical_Align=VerMid;
    btn->Horizontal_Align=HorMid;
    
    strcpy(btn->Text,text);
    
    _RO_WRITE(btn->Layer,layer_handler,layer);
    
    //�󶨷���
    btn->Event_Set=Method_Button_Event_Set;
    btn->Active_Set=Method_Button_Active_Set;
    btn->Change_Text=Method_Button_Change_Text;
    btn->Dispose=Method_Button_Dispose;
    btn->Repaint=Method_Button_Repaint;
    
    Graphic_CreateCanvas(&btn->Canvas,x,y,width,height,virt);
    btn->TouchArea=td->Create_Area(td,x,y,width,height);
    
    //���������¼�ʹ��
    btn->TouchArea->All_Event_Enable=Enable;
    
    //����¼�ʹ��
    btn->TouchArea->Click_Enable=Enable;
    
    //���ô����ص�����
    btn->TouchArea->PressDown_CallBackFunc=Global_PressDown_Callback;
    btn->TouchArea->Release_CallBackFunc=Global_Release_Callback;
    btn->TouchArea->KeepPress_CallBackFunc=Global_KeepPress_Callback;
    btn->TouchArea->Click_CallBackFunc=Global_Click_Callback;
}






















void Method_Label_Event_Set(label* lbl, label_event_type event, status_flag status)
{
    switch (event)
    {
        case LBL_REPAINT:
        {
            _RO_WRITE(lbl->Event_Repaint_Enable,status_flag,status);
            break;
        }
        default:
            break;
    }
}

void Method_Label_Active_Set(label* lbl, status_flag status)
{
    _RO_WRITE(lbl->Active,status_flag,status);
}

void Method_Label_Change_Text(label* lbl, char* fmtstr, ...)
{
    va_list vl;
    
    va_start(vl,fmtstr);
    vsprintf(lbl->Text,fmtstr,vl);
    va_end(vl);
    lbl->Updated=Enable;
}

void Method_Label_Repaint(label* lbl)
{
    if (lbl->Display)
    {
        //�����ʾ�û�����
        if (lbl->CustomBackground)
            Graphic_Draw_BMP_All(&lbl->Canvas,0,0,lbl->Background);
        else
        {
            Graphic_Draw_Rectangle(&lbl->Canvas,0,0,&lbl->Appearance);
            Graphic_Draw_String(&lbl->Canvas,
                                lbl->Appearance.Frame_Thickness,lbl->Appearance.Frame_Thickness,
                                lbl->Canvas.Area.Width-2*lbl->Appearance.Frame_Thickness,lbl->Canvas.Area.Height-2*lbl->Appearance.Frame_Thickness,
                                lbl->Vertical_Align,lbl->Horizontal_Align,&lbl->Font,lbl->Text);
        }
        if (lbl->Active&&lbl->AllEvent_Enable&&lbl->Event_Repaint_Enable)
            lbl->Repaint_Callback(lbl);
    }
    else
    {
        component_layer* cl;
        cl=LinkedList_Find(&lbl->Controller->Layers,lbl->Controller->Active_Layer);
        Graphic_Draw_Color(&lbl->Canvas,cl->Color);
    }
    Graphic_RefreshCanvas(&lbl->Canvas);
}

void Method_Label_Dispose(label* lbl)
{
    component* cm=GET_COMPONENT_ADDR(lbl);
    component_layer* cl=LinkedList_Find(&lbl->Controller->Layers,lbl->Layer);
    
    //ɾ������
    if (!lbl->Canvas.Virtual)
        Graphic_Canvas_Delete(&lbl->Canvas);
    //ɾ���ؼ�
    LinkedList_Dispose2(&cl->Components,cm);
}

void _Label_Init(label* lbl, touch_device* td, uint16_t x, uint16_t y, uint16_t width, uint16_t height, status_flag virt, char* text, layer_handler layer)
{
    lbl->CustomBackground=Disable;
    lbl->Display=Enable;
    lbl->AllEvent_Enable=Disable;
    lbl->Updated=Disable;
    _RO_WRITE(lbl->Layer,layer_handler,layer);
    _RO_WRITE(lbl->Active,status_flag,Enable);
    _RO_WRITE(lbl->Event_Repaint_Enable,status_flag,Disable);
    
    lbl->Font.BackColor=WHITE;
    lbl->Font.CharColor=BLACK;
    lbl->Font.Mold=BasicChar_Courier_New;
    
    lbl->Appearance.Background_Color=WHITE;
    lbl->Appearance.Display_Background=Enable;
    lbl->Appearance.Frame_Color=BLACK;
    lbl->Appearance.Frame_Thickness=5;
    lbl->Appearance.Height=height;
    lbl->Appearance.Width=width;
    
    lbl->Vertical_Align=VerMid;
    lbl->Horizontal_Align=HorMid;
    
    strcpy(lbl->Text,text);    
    
    //�󶨷���
    lbl->Event_Set=Method_Label_Event_Set;
    lbl->Active_Set=Method_Label_Active_Set;
    lbl->Change_Text=Method_Label_Change_Text;
    lbl->Dispose=Method_Label_Dispose;
    lbl->Repaint=Method_Label_Repaint;
    
    Graphic_CreateCanvas(&lbl->Canvas,x,y,width,height,virt);
}
















void Method_Panel_Data_Update(panel* pnl)
{
    if (pnl->Always_Average)
    {
        uint16_t i;
        
        for (i=0;i<pnl->RowsNum;i++)
        {
            pnl->Vertical_Ratio[i]=1.0f/pnl->RowsNum;
            pnl->Blocks_Height[i]=pnl->Area.Height*pnl->Vertical_Ratio[i];
        }
        for (i=0;i<pnl->LinesNum;i++)
        {
            pnl->Horizontal_Ratio[i]=1.0f/pnl->LinesNum;
            pnl->Blocks_Width[i]=pnl->Area.Width*pnl->Horizontal_Ratio[i];
        }
    }
    _Panel_Data_Calculate(pnl);
}

void Method_Panel_Dispose(panel* pnl)
{
    component* cm=GET_COMPONENT_ADDR(pnl);
    component_layer* cl=LinkedList_Find(&pnl->Controller->Layers,pnl->Layer);
    
    //ɾ���ؼ�
    LinkedList_Dispose2(&cl->Components,cm);
}

void _Panel_Data_Calculate(panel* pnl)
{
    int i;
    short save;
    
    //����ÿ���ȣ��ޱ߽磩
    pnl->Blocks_Width[pnl->LinesNum-1]=pnl->Area.Width;
    for (i=0;i<pnl->LinesNum-1;i++)
    {
        if (pnl->UseRatio)
            pnl->Blocks_Width[i]=ROUND((float)pnl->Area.Width/pnl->LinesNum);
        pnl->Blocks_Width[pnl->LinesNum-1]-=pnl->Blocks_Width[i];
    }
    
    //����ÿ������꣨�б߽磩
    _RO_WRITE(pnl->Blocks_X[0],uint16_t,pnl->Boundary+pnl->Area.X);
    for (i=1;i<pnl->LinesNum;i++)
        _RO_WRITE(pnl->Blocks_X[i],uint16_t,pnl->Blocks_X[i-1]+pnl->Blocks_Width[i-1]+pnl->Area.X);
    
    //����ÿ���ȱ������ޱ߽磩
    for (i=0;i<pnl->LinesNum;i++)
        pnl->Horizontal_Ratio[i]=(float)pnl->Blocks_Width[i]/pnl->Area.Width;
    
    //���¼���ÿ���ȣ����Ǳ߽磩
    for (i=0;i<pnl->LinesNum;i++)
    {
        save=pnl->Blocks_Width[i]-2*pnl->Boundary;
        if (save<0) //������̫С����Ϊ0
            save=0;
        pnl->Blocks_Width[i]=save;
    }
    
    //����ÿ��߶ȣ��ޱ߽磩
    pnl->Blocks_Height[pnl->RowsNum-1]=pnl->Area.Height;
    for (i=0;i<pnl->RowsNum-1;i++)
    {
        if (pnl->UseRatio)
            pnl->Blocks_Height[i]=ROUND((float)pnl->Area.Width/pnl->RowsNum);
        pnl->Blocks_Height[pnl->RowsNum-1]-=pnl->Blocks_Height[i];
    }
    
    //����ÿ�������꣨�б߽磩
    _RO_WRITE(pnl->Blocks_Y[0],uint16_t,pnl->Boundary+pnl->Area.Y);
    for (i=1;i<pnl->RowsNum;i++)
        _RO_WRITE(pnl->Blocks_Y[i],uint16_t,pnl->Blocks_Y[i-1]+pnl->Blocks_Height[i-1]+pnl->Area.Y);
    
    //����ÿ��߶ȱ������ޱ߽磩
    for (i=0;i<pnl->RowsNum;i++)
        pnl->Vertical_Ratio[i]=(float)pnl->Blocks_Height[i]/pnl->Area.Height;
    
    //���¼���ÿ��߶ȣ����Ǳ߽磩
    for (i=0;i<pnl->RowsNum;i++)
    {
        save=pnl->Blocks_Height[i]-2*pnl->Boundary;
        if (save<0) //������̫С����Ϊ0
            save=0;
        pnl->Blocks_Height[i]=save;
    }
}

void _Panel_Init(panel* pnl, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t rownum, uint8_t linenum, layer_handler layer)
{
    int i;
    
    _RO_WRITE(pnl->Layer,layer_handler,layer);
    
    pnl->Area.X=x;
    pnl->Area.Y=y;
    pnl->Area.Width=width;
    pnl->Area.Height=height;
    
    pnl->RowsNum=rownum;
    pnl->LinesNum=linenum;
    
    pnl->Boundary=0;
    
    pnl->Data_Array=Malloc(GUI_HEAP,256);
    
    //�����������ַ
    _RO_WRITE(pnl->Blocks_Width,uint16_t*,(uint16_t*)pnl->Data_Array);
    _RO_WRITE(pnl->Blocks_Height,uint16_t*,(uint16_t*)(((uint32_t)pnl->Blocks_Width)+PANEL_MAX_LINE*2));
    
    _RO_WRITE(pnl->Horizontal_Ratio,float*,(float*)(((uint32_t)pnl->Blocks_Height)+PANEL_MAX_ROW*2));
    _RO_WRITE(pnl->Vertical_Ratio,float*,(float*)(((uint32_t)pnl->Horizontal_Ratio)+PANEL_MAX_LINE*4));
    
    _RO_WRITE(pnl->Blocks_X,uint16_t*,(uint16_t*)(((uint32_t)pnl->Vertical_Ratio)+PANEL_MAX_ROW*4));
    _RO_WRITE(pnl->Blocks_Y,uint16_t*,(uint16_t*)(((uint32_t)pnl->Blocks_X)+PANEL_MAX_LINE*2));
    
    pnl->Data_Update    =   Method_Panel_Data_Update;
    pnl->Dispose        =   Method_Panel_Dispose;
    
    //Ĭ��ʹ�ñ�������
    pnl->UseRatio=Enable;
    //Ĭ��ƽ��
    pnl->Always_Average=Enable;
    //���¼�����
    _Panel_Data_Calculate(pnl);
}














/********************************************************************************************************/
/*                                              ToolBox                                                 */
/********************************************************************************************************/
void Method_ToolBox_Event_Set(toolbox* tbx, toolbox_event_type event, status_flag status)
{
    switch (event)
    {
        case TBX_PRESSDOWN:
        {
            _RO_WRITE(tbx->Event_PressDown_Enable,status_flag,status);
            tbx->TouchArea->PressDown_Enable=status;
            break;
        }
        case TBX_RELEASE:
        {
            _RO_WRITE(tbx->Event_Release_Enable,status_flag,status);
            tbx->TouchArea->Release_Enable=status;
            break;
        }
        case TBX_KEEPPRESS:
        {
            _RO_WRITE(tbx->Event_KeepPress_Enable,status_flag,status);
            tbx->TouchArea->KeepPress_Enable=status;
            break;
        }
        case TBX_CLICK:
        {
            _RO_WRITE(tbx->Event_Click_Enable,status_flag,status);
            break;
        }
        case TBX_GETFOCUS:
        {
            _RO_WRITE(tbx->Event_GetFocus_Enable,status_flag,status);
            break;
        }
        case TBX_LOSTFOCUS:
        {
            _RO_WRITE(tbx->Event_LostFocus_Enable,status_flag,status);
            break;
        }
        case TBX_REPAINT:
        {
            _RO_WRITE(tbx->Event_Repaint_Enable,status_flag,status);
            break;
        }
        default:
            break;
    }
}

void Method_ToolBox_Active_Set(toolbox* tbx, status_flag status)
{
    _RO_WRITE(tbx->Active,status_flag,status);
    tbx->TouchArea->All_Event_Enable=status;
}

void Method_ToolBox_Change_Text(toolbox* tbx, char* fmtstr, ...)
{
    va_list vl;
    
    va_start(vl,fmtstr);
    vsprintf(tbx->Text,fmtstr,vl);
    va_end(vl);
    tbx->Updated=Enable;
}

toolbox_button* Method_ToolBox_Add_Button(toolbox* tbx, char* text)
{
    int i;
    component* cm;
    toolbox_button* tbx_btn;
    
    //������ť
    cm=LinkedList_Add(&tbx->Buttons,tbx->Buttons.Nodes_Num);
    cm->Type=TOOLBOX_BUTTON;
    tbx_btn=&cm->Component.ToolBox_Button;

    //��ʼ��
    _ToolBox_Button_Init(tbx_btn,tbx->Controller->TouchDevice,tbx->Button_Wdith,tbx->Button_Height,
                            tbx->Canvas.Virtual,text);
    //������
    _ToolBox_Button_Repermutation(tbx);
    //�봥�����򴴽���ϵ
    tbx_btn->TouchArea->Association=cm;
    
    return tbx_btn;
}

void Method_ToolBox_Delete_Button(toolbox* tbx, toolbox_button* tbx_btn)
{
    component* cm=GET_COMPONENT_ADDR(tbx_btn);
    
    //ɾ������
    Graphic_Canvas_Delete(&tbx_btn->Canvas);
    //ɾ����������
    tbx->Controller->TouchDevice->Delete_Area(tbx->Controller->TouchDevice,tbx_btn->TouchArea);
    //���������Ƴ�
    LinkedList_Dispose2(&tbx->Buttons,cm);
}

void Method_ToolBox_Repaint(toolbox* tbx)
{
    //�������������
    if (tbx->Display)
    {
        if (tbx->CustomBackground)
            Graphic_Draw_BMP_All(&tbx->Canvas,0,0,tbx->Background);
        else
        {
            Graphic_Draw_Rectangle(&tbx->Canvas,0,0,&tbx->Appearance);
            Graphic_Draw_String(&tbx->Canvas,0,0,tbx->Canvas.Area.Width,tbx->Canvas.Area.Height,VerMid,HorMid,&tbx->Font,tbx->Text);
        }
        if (tbx->Event_Repaint_Enable)
            tbx->Repaint_Callback(tbx);
    }
    if (tbx->Buttons_Display&&tbx->Focus)
    {
        int i;
        toolbox_button* tbx_btn;
        //����ÿ���Ӱ���
        for (i=0;i<tbx->Buttons.Nodes_Num;i++)
        {
            tbx_btn=&((component*)LinkedList_Find(&tbx->Buttons,i))->Component.ToolBox_Button;
            if (tbx_btn->CustomBackground)
                Graphic_Draw_BMP_All(&tbx_btn->Canvas,0,0,tbx_btn->Background);
            else
            {
                Graphic_Draw_Rectangle(&tbx_btn->Canvas,0,0,&tbx_btn->Appearance);
                Graphic_Draw_String(&tbx_btn->Canvas,0,0,tbx_btn->Canvas.Area.Width,tbx_btn->Canvas.Area.Height,VerMid,HorMid,&tbx_btn->Font,tbx_btn->Text);
            }
        }
    }
}

void Method_ToolBox_Dispose(toolbox* tbx)
{
    int i;
    toolbox_button* tbx_btn;
    
    //ɾ�����а�ť
    for (i=0;i<tbx->Buttons.Nodes_Num;i++)
    {
        tbx_btn=&((component*)LinkedList_Find(&tbx->Buttons,i))->Component.ToolBox_Button;
        tbx->Delete_Button(tbx,tbx_btn);
    }
    //ɾ������
    Graphic_Canvas_Delete(&tbx->Canvas);
    //ɾ����������
    tbx->Controller->TouchDevice->Delete_Area(tbx->Controller->TouchDevice,tbx->TouchArea);
    //���������Ƴ�
    component* cm=GET_COMPONENT_ADDR(tbx);
    component_layer* cl=LinkedList_Find(&tbx->Controller->Layers,tbx->Layer);
    LinkedList_Dispose2(&cl->Components,cm);
}

void _ToolBox_Init(toolbox* tbx, touch_device* td, uint16_t x, uint16_t y, uint16_t width, uint16_t height, status_flag virt, char* text, layer_handler layer)
{
    //״̬����
    tbx->CustomBackground=Disable;
    tbx->Display=Enable;
    tbx->Buttons_Display=Enable;
    tbx->AllEvent_Enable=Disable;
    tbx->Updated=Disable;
    _RO_WRITE(tbx->Active,status_flag,Enable);
    _RO_WRITE(tbx->Focus,status_flag,Disable);
    
    //�¼�ʹ��λ
    _RO_WRITE(tbx->Event_PressDown_Enable,status_flag,Disable);
    _RO_WRITE(tbx->Event_Release_Enable,status_flag,Disable);
    _RO_WRITE(tbx->Event_KeepPress_Enable,status_flag,Disable);
    _RO_WRITE(tbx->Event_Click_Enable,status_flag,Disable);
    _RO_WRITE(tbx->Event_GetFocus_Enable,status_flag,Disable);
    _RO_WRITE(tbx->Event_LostFocus_Enable,status_flag,Disable);
    _RO_WRITE(tbx->Event_Repaint_Enable,status_flag,Disable);
    
    tbx->Font.BackColor=WHITE;
    tbx->Font.CharColor=BLACK;
    tbx->Font.Mold=BasicChar_Courier_New;
    
    tbx->Appearance.Background_Color=WHITE;
    tbx->Appearance.Display_Background=Enable;
    tbx->Appearance.Frame_Color=BLACK;
    tbx->Appearance.Frame_Thickness=5;
    tbx->Appearance.Height=height;
    tbx->Appearance.Width=width;
    
    tbx->Vertical_Align=VerMid;
    tbx->Horizontal_Align=HorMid;
    
    tbx->Button_Wdith=100;
    tbx->Button_Height=50;
    
    strcpy(tbx->Text,text);
    
    _RO_WRITE(tbx->Layer,layer_handler,layer);
    
    //�����ʼ��
    LinkedList_Prepare(&tbx->Buttons,sizeof(component));
    //�󶨷���
    tbx->Event_Set=Method_ToolBox_Event_Set;
    tbx->Active_Set=Method_ToolBox_Active_Set;
    tbx->Change_Text=Method_ToolBox_Change_Text;
    tbx->Add_Button=Method_ToolBox_Add_Button;
    tbx->Delete_Button=Method_ToolBox_Delete_Button;
    tbx->Dispose=Method_ToolBox_Dispose;
    tbx->Repaint=Method_ToolBox_Repaint;
    //��������
    Graphic_CreateCanvas(&tbx->Canvas,x,y,width,height,virt);
    //������������
    tbx->TouchArea=td->Create_Area(td,x,y,width,height);
    //���������¼�ʹ��
    tbx->TouchArea->All_Event_Enable=Enable;
    //����¼�ʹ��
    tbx->TouchArea->Click_Enable=Enable;
    //���ô����ص�����
    tbx->TouchArea->PressDown_CallBackFunc=Global_PressDown_Callback;
    tbx->TouchArea->Release_CallBackFunc=Global_Release_Callback;
    tbx->TouchArea->KeepPress_CallBackFunc=Global_KeepPress_Callback;
    tbx->TouchArea->Click_CallBackFunc=Global_Click_Callback;
}

void _ToolBox_Button_Init(toolbox_button* tbx_btn, touch_device* td, uint16_t width, uint16_t height, status_flag virt, char* text)
{
    //״̬����
    tbx_btn->CustomBackground=Disable;
    tbx_btn->AllEvent_Enable=Disable;
    _RO_WRITE(tbx_btn->Active,status_flag,Enable);
    //�¼�ʹ��λ
    _RO_WRITE(tbx_btn->Event_Click_Enable,status_flag,Disable);
    //��������
    tbx_btn->Font.BackColor=WHITE;
    tbx_btn->Font.CharColor=BLACK;
    tbx_btn->Font.Mold=BasicChar_Courier_New;
    //�������
    tbx_btn->Appearance.Background_Color=WHITE;
    tbx_btn->Appearance.Display_Background=Enable;
    tbx_btn->Appearance.Frame_Color=BLACK;
    tbx_btn->Appearance.Frame_Thickness=5;
    tbx_btn->Appearance.Height=height;
    tbx_btn->Appearance.Width=width;
    
    tbx_btn->Vertical_Align=VerMid;
    tbx_btn->Horizontal_Align=HorMid;
    
    strcpy(tbx_btn->Text,text);
        
    //�󶨷���
    tbx_btn->Event_Set=Method_ToolBox_Button_Event_Set;
    tbx_btn->Active_Set=Method_ToolBox_Button_Active_Set;
    tbx_btn->Change_Text=Method_ToolBox_Button_Change_Text;
    
    //��������
    Graphic_CreateCanvas(&tbx_btn->Canvas,0,0,width,height,virt);
    //������������
    tbx_btn->TouchArea=td->Create_Area(td,0,0,width,height);
    //���������¼�ʹ��
    tbx_btn->TouchArea->All_Event_Enable=Enable;
    //���ô����ص�����
    tbx_btn->TouchArea->Click_CallBackFunc=Global_Click_Callback;
}

void _ToolBox_Button_Repermutation(toolbox* tbx)
{
    int i;
    uint16_t x,y;
    component* cm;
    toolbox_button* tbx_btn;

    for (i=0;i<tbx->Buttons.Nodes_Num;i++)
    {
        cm=LinkedList_Find(&tbx->Buttons,i);
        tbx_btn=&cm->Component.ToolBox_Button;
        x=(tbx->Controller->Screen->Width-tbx_btn->Appearance.Width)/2;
        y=(tbx->Controller->Screen->Height-tbx->Buttons.Nodes_Num*tbx_btn->Appearance.Height)/2;
        y+=i*tbx_btn->Appearance.Height;
        tbx_btn->Canvas.Area.X=x;
        tbx_btn->Canvas.Area.Y=y;
        tbx_btn->TouchArea->X=x;
        tbx_btn->TouchArea->Y=y;
    }
}

void Method_ToolBox_Button_Event_Set(toolbox_button* tbx_btn, toolbox_event_type event, status_flag status)
{
    if (event==TBX_CLICK)
    {
        tbx_btn->TouchArea->Click_Enable=status;
        _RO_WRITE(tbx_btn->Event_Click_Enable,status_flag,status);
    }
}

void Method_ToolBox_Button_Active_Set(toolbox_button* tbx_btn, status_flag status)
{
    _RO_WRITE(tbx_btn->Active,status_flag,status);
    tbx_btn->TouchArea->All_Event_Enable=status;
}

void Method_ToolBox_Button_Change_Text(toolbox_button* tbx_btn, char* fmtstr, ...)
{
    va_list vl;
    
    va_start(vl,fmtstr);
    vsprintf(tbx_btn->Text,fmtstr,vl);
    va_end(vl);
    if (tbx_btn->ToolBox->Focus)
        tbx_btn->ToolBox->Updated=Enable;
}






























void Method_Scope_Event_Set(scope* scp, scope_event_type event, status_flag status)
{
    switch (event)
    {
        case SCP_PRESSDOWN:
        {
            _RO_WRITE(scp->Event_PressDown_Enable,status_flag,status);
            if (scp->Unfixed_Axis)
                scp->TouchArea->PressDown_Enable=Enable;
            else
                scp->TouchArea->PressDown_Enable=status;
            break;
        }
        case SCP_RELEASE:
        {
            _RO_WRITE(scp->Event_Release_Enable,status_flag,status);
            if (scp->Unfixed_Axis)
                scp->TouchArea->Release_Enable=Enable;
            else
                scp->TouchArea->Release_Enable=status;
            break;
        }
        case SCP_KEEPPRESS:
        {
            _RO_WRITE(scp->Event_KeepPress_Enable,status_flag,status);
            if (scp->Unfixed_Axis)
                scp->TouchArea->KeepPress_Enable=Enable;
            else
                scp->TouchArea->KeepPress_Enable=status;
            break;
        }
        case SCP_CLICK:
        {
            _RO_WRITE(scp->Event_Click_Enable,status_flag,status);
            break;
        }
        case SCP_GETFOCUS:
        {
            _RO_WRITE(scp->Event_GetFocus_Enable,status_flag,status);
            break;
        }
        case SCP_LOSTFOCUS:
        {
            _RO_WRITE(scp->Event_LostFocus_Enable,status_flag,status);
            break;
        }
        case SCP_REPAINT:
        {
            _RO_WRITE(scp->Event_Repaint_Enable,status_flag,status);
            break;
        }
        default:
            break;
    }
}

void Method_Scope_Active_Set(scope* scp, status_flag status)
{
    _RO_WRITE(scp->Active,status_flag,status);
    scp->TouchArea->All_Event_Enable=status;
}

void Method_Scope_Unfixed_Axis_Set(scope* scp, status_flag state)
{
    _RO_WRITE(scp->Unfixed_Axis,status_flag,state);
    scp->TouchArea->PressDown_Enable=state;
    scp->TouchArea->KeepPress_Enable=state;
    scp->TouchArea->Release_Enable=state;
}

void Method_Scope_Repaint(scope* scp)
{
    if (scp->Display)
    {
        if ((!scp->Stop)&&scp->Data_Updated)
        {
            int i;
            for (i=0;i<scp->Curve.Length;i++)
                scp->Data_Buffer[i]=scp->Curve.Data[i];
            _RO_WRITE(scp->Data_Length,uint16_t,scp->Curve.Length);
            scp->Data_Updated=Disable;
        }
        _Scope_CurveRange_Update(scp->Data_Buffer,scp->Data_Length,&scp->Axis,&scp->Curve,&scp->Curve_Range,&scp->Display_Area);
        _Scope_Data_Prepare(scp->Data_Buffer,GUI_Buffer1,&scp->Axis,&scp->Curve_Range);
        _Scope_Repaint(scp);
        if (scp->Event_Repaint_Enable&&scp->AllEvent_Enable)
            scp->Repaint_Callback(scp);
        Graphic_RefreshCanvas(&scp->Canvas);
    }
}

void Method_Scope_Dispose(scope* scp)
{
    //ɾ������
    Graphic_Canvas_Delete(&scp->Canvas);
    //ɾ����������
    scp->Controller->TouchDevice->Delete_Area(scp->Controller->TouchDevice,scp->TouchArea);
    //���������Ƴ�
    component* cm=GET_COMPONENT_ADDR(scp);
    component_layer* cl=LinkedList_Find(&scp->Controller->Layers,scp->Layer);
    LinkedList_Dispose2(&cl->Components,cm);
}
    
void _Scope_Init(scope* scp, touch_device* td, uint16_t x, uint16_t y, uint16_t width, uint16_t height, status_flag virt, uint16_t max_dlen, status_flag vscale, layer_handler layer)
{
    scp->Display=Enable;
    scp->AllEvent_Enable=Disable;
    scp->Update=Disable;
    scp->Cover=Enable;
    scp->Data_Updated=Disable;
    
    _RO_WRITE(scp->Stop,status_flag,Disable);
    _RO_WRITE(scp->Active,status_flag,Enable);
    _RO_WRITE(scp->Focus,status_flag,Disable);
    _RO_WRITE(scp->Event_PressDown_Enable,status_flag,Disable);
    _RO_WRITE(scp->Event_Release_Enable,status_flag,Disable);
    _RO_WRITE(scp->Event_KeepPress_Enable,status_flag,Disable);
    _RO_WRITE(scp->Event_Click_Enable,status_flag,Disable);
    _RO_WRITE(scp->Event_GetFocus_Enable,status_flag,Disable);
    _RO_WRITE(scp->Event_LostFocus_Enable,status_flag,Disable);
    _RO_WRITE(scp->Event_Repaint_Enable,status_flag,Disable);
    
    scp->Display_Area.Background_Color=WHITE;
    scp->Display_Area.Display_Background=Enable;
    scp->Display_Area.Frame_Color=BLACK;
    scp->Display_Area.Frame_Thickness=5;
    scp->Display_Area.Height=height;
    scp->Display_Area.Width=width;
    
    _RO_WRITE(scp->Layer,layer_handler,layer);
    
    //�������ݻ�����
    scp->Data_Buffer=Malloc(GUI_HEAP,4*max_dlen);
    
    //�󶨷���
    scp->Event_Set=Method_Scope_Event_Set;
    scp->Active_Set=Method_Scope_Active_Set;
    scp->Unfixed_Axis_Set=Method_Scope_Unfixed_Axis_Set;
    scp->Dispose=Method_Scope_Dispose;
    scp->Repaint=Method_Scope_Repaint;
    
    Graphic_CreateCanvas(&scp->Canvas,x,y,width,height,virt);
    
    scp->TouchArea=td->Create_Area(td,x,y,width,height);
    
    //���������¼�ʹ��
    scp->TouchArea->All_Event_Enable=Enable;
    
    //����¼�ʹ��
    scp->TouchArea->Click_Enable=Enable;
    
    //���ô����ص�����
    scp->TouchArea->PressDown_CallBackFunc=Global_PressDown_Callback;
    scp->TouchArea->Release_CallBackFunc=Global_Release_Callback;
    scp->TouchArea->KeepPress_CallBackFunc=Global_KeepPress_Callback;
    scp->TouchArea->Click_CallBackFunc=Global_Click_Callback;
    
    scp->Display_Area.Width=width;
	scp->Display_Area.Height=height;
	scp->Display_Area.Frame_Thickness=5;
	scp->Display_Area.Background_Color=WHITE;
	scp->Display_Area.Frame_Color=BLACK;
    scp->Display_Area.Display_Background=1;
	scp->Style=CURVE;
	
	scp->Axis.Axis_Line.Color=BLACK;
	scp->Axis.Axis_Line.Solid_Length=1;
	scp->Axis.Axis_Line.Vacancy_Length=0;
	scp->Axis.Axis_Line.Width=2;
	scp->Axis.Axis_Display=1;
	
	scp->Axis.Net_Line.Color=BLACK;
	scp->Axis.Net_Line.Width=2;
	scp->Axis.Net_Line.Solid_Length=1;
	scp->Axis.Net_Line.Vacancy_Length=0;
	scp->Axis.Net_Display=1;
	
	scp->Axis.X_Net_Density=2;
	scp->Axis.Y_Net_Density=2;
	
	scp->Axis.Is_XRange_Auto=1;
	scp->Axis.Is_YRange_Auto=1;
	scp->Axis.X_Max=10;
	scp->Axis.X_Min=0;
	scp->Axis.X_Tick_Num=10;
	scp->Axis.Y_Max=10;
	scp->Axis.Y_Min=0;
	scp->Axis.Y_Tick_Num=10;
	
	scp->Curve.Curve_Line.Color=ALING_RED;
	scp->Curve.Curve_Line.Vacancy_Length=0;
	scp->Curve.Curve_Line.Width=2;
	scp->Curve.Is_XRange_Auto=1;
	scp->Curve.Is_YRange_Auto=1;
	scp->Curve.X_Max=10;
	scp->Curve.X_Min=0;
	scp->Curve.Y_Max=10;
	scp->Curve.Y_Min=0;
	scp->Curve.Display=1;
}

void _Scope_CurveRange_Update(float* data, uint16_t dlen, axis* ax, curve* cur, curve_range* cur_range, rectangle* area)
{
    float k_axis,b_axis;
	float k_curve,b_curve;
	
	//ȷ��X��Χ
	if (ax->Is_XRange_Auto)
	{
		if (cur->Is_XRange_Auto)
		{
			ax->X_Max=area->Width-2*area->Frame_Thickness;
			ax->X_Min=0;
			cur->X_Max=ax->X_Max;
			cur->X_Min=ax->X_Min;
		}
		else
		{
			ax->X_Max=cur->X_Max;
			ax->X_Min=cur->Y_Min;
		}
	}
	else
	{
		if (cur->Is_XRange_Auto)
		{
			cur->X_Max=ax->X_Max;
			cur->X_Min=ax->X_Min;
		}
	}
	
	//��X����������ӳ�䵽����������
	k_axis=MathHelper_Slope(ax->X_Min,area->Frame_Thickness,
						ax->X_Max,area->Width-area->Frame_Thickness);
	b_axis=MathHelper_Intercept(ax->X_Min,area->Frame_Thickness,
						ax->X_Max,area->Width-area->Frame_Thickness);
	//����X���ֵ��Ԫ�����֮�������ӳ��
	k_curve=MathHelper_Slope(cur->X_Min,0,cur->X_Max,dlen-1);
	b_curve=MathHelper_Intercept(cur->X_Min,0,cur->X_Max,dlen-1);
	
	//����Begin_Index��Valid_X_Begin
	if (ax->X_Min<=cur->X_Min)
	{
		cur_range->_Begin_Index=0;
		if (ax->X_Max>cur->X_Min)
			cur_range->_Valid_X_Begin=cur->X_Min*k_axis+b_axis;
		else
			cur_range->_Valid_X_Begin=area->Width-area->Frame_Thickness;
	}
	else
	{
		cur_range->_Valid_X_Begin=area->Frame_Thickness;
		if (ax->X_Min<cur->X_Max)
			cur_range->_Begin_Index=ax->X_Min*k_curve+b_curve;
		else
			cur_range->_Begin_Index=9;
	}
	
	//����End_Index����Valid_X_End
	cur_range->_End_Index=ax->X_Max*k_curve+b_curve;
	if (ax->X_Max>=cur->X_Max)
	{
		cur_range->_End_Index=dlen-1;
		if (ax->X_Min<cur->X_Max)
			cur_range->_Valid_X_End=cur->X_Max*k_axis+b_axis;
		else
			cur_range->_Valid_X_End=area->Frame_Thickness;
	}
	else
	{
		if (ax->X_Max>cur->X_Min)
			cur_range->_End_Index=ax->X_Max*k_curve+b_curve;
		else
			cur_range->_End_Index=0;
		cur_range->_Valid_X_End=area->Width-area->Frame_Thickness-1;
	}
    
	//Y��λ��
	if (b_axis>=area->Width-area->Frame_Thickness-ax->Axis_Line.Width/2)
		ax->_Y_Position=area->Width-area->Frame_Thickness-ax->Axis_Line.Width/2;
	else if (b_axis<=area->Frame_Thickness+ax->Axis_Line.Width/2)
		ax->_Y_Position=area->Frame_Thickness+ax->Axis_Line.Width/2;
	else
		ax->_Y_Position=b_axis;
	
	cur_range->_Length=cur_range->_End_Index-cur_range->_Begin_Index+1;
	if (ax->Is_YRange_Auto)
	{
		if (cur->Is_YRange_Auto)
		{
			//k_axis=MathHelper_FindMax_Float(&cur->Data[cur_range->_Begin_Index],cur_range->_Length,NULL);
			//b_axis=MathHelper_FindMin_Float(&cur->Data[cur_range->_Begin_Index],cur_range->_Length,NULL);
            k_axis=MathHelper_FindMax_Float(&data[cur_range->_Begin_Index],cur_range->_Length,NULL);
			b_axis=MathHelper_FindMin_Float(&data[cur_range->_Begin_Index],cur_range->_Length,NULL);
			ax->Y_Max=k_axis;
			ax->Y_Min=b_axis;
			cur->Y_Max=ax->Y_Max;
			cur->Y_Min=ax->Y_Min;
		}
		else
		{
			ax->Y_Max=cur->Y_Max;
			ax->Y_Min=cur->Y_Min;
		}
	}
	else
	{
		if (cur->Is_YRange_Auto)
		{
			//k_axis=MathHelper_FindMax_Float(&cur->Data[cur_range->_Begin_Index],cur_range->_Length,NULL);
			//b_axis=MathHelper_FindMin_Float(&cur->Data[cur_range->_Begin_Index],cur_range->_Length,NULL);
            k_axis=MathHelper_FindMax_Float(&data[cur_range->_Begin_Index],cur_range->_Length,NULL);
			b_axis=MathHelper_FindMin_Float(&data[cur_range->_Begin_Index],cur_range->_Length,NULL);
			cur->Y_Max=k_axis;
			cur->Y_Min=b_axis;
		}
	}
	
	//����Valid_Y_End
	k_axis=(area->Height-2*area->Frame_Thickness)/(ax->Y_Min-ax->Y_Max);
	b_axis=area->Height-area->Frame_Thickness-1-k_axis*ax->Y_Min;
	if (ax->Y_Min<=cur->Y_Min)
	{
		if (ax->Y_Max>=cur->Y_Min)
			cur_range->_Valid_Y_End=cur->Y_Min*k_axis+b_axis;
		else
			cur_range->_Valid_Y_End=area->Frame_Thickness+1;
	}
	else
		cur_range->_Valid_Y_End=area->Height-area->Frame_Thickness-1;
	
	//����Valid_Y_Begin
	if (ax->Y_Max>=cur->Y_Max)
	{
		if (ax->Y_Min<=cur->Y_Max)
			cur_range->_Valid_Y_Begin=cur->Y_Max*k_axis+b_axis+1;
		else
			cur_range->_Valid_Y_Begin=area->Height-area->Frame_Thickness-1;
	}
	else
		cur_range->_Valid_Y_Begin=area->Frame_Thickness;
	
	if (b_axis>=area->Height-area->Frame_Thickness-ax->Axis_Line.Width/2)
		ax->_X_Position=area->Height-area->Frame_Thickness-ax->Axis_Line.Width/2;
	else if (b_axis<=area->Frame_Thickness+ax->Axis_Line.Width/2)
		ax->_X_Position=area->Frame_Thickness+ax->Axis_Line.Width/2;
	else
		ax->_X_Position=b_axis;
}

void _Scope_Data_Prepare(float* datin, float* datout, axis* ax, curve_range* cur_range)
{
    int i;
    
    for (i=0;i<cur_range->_Length;i++)
		datout[i]=datin[i+cur_range->_Begin_Index]>ax->Y_Max?ax->Y_Max:
				(datin[i+cur_range->_Begin_Index]<ax->Y_Min?ax->Y_Min:
				datin[i+cur_range->_Begin_Index]);
}

void _Scope_Draw_Waveform(canvas* c, float* dat, line* l, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t axis_pos, uint16_t dlen, uint8_t style)
{
	u16 i;
	float Max_or_Slope,Min_or_Inter;
	float Step;
	
	if ((x>=c->Area.Width)||(y>=c->Area.Height))
		return;

	//��dat�е���������ӳ�䵽������Χ��
	Max_or_Slope=dat[0];
	Min_or_Inter=Max_or_Slope;
	for (i=0;i<dlen;i++)
	{
		if (dat[i]>Max_or_Slope)
			Max_or_Slope=dat[i];
		else if (dat[i]<=Min_or_Inter)
			Min_or_Inter=dat[i];
	}
	
	if (Max_or_Slope-Min_or_Inter<0.02f)
		Max_or_Slope=-1.0f;
	else
		Max_or_Slope=(float)(height-1)/(Min_or_Inter-Max_or_Slope);
	Min_or_Inter=y+height-1-Max_or_Slope*Min_or_Inter;
    
	//���������
	if (style==CURVE)
	{
		//���ݳ��ȴ��ڵ��ڻ�ͼ�����ȣ�����ͼ
		if (dlen>=width)
		{
			u16 x_save1,x_save2;
			float y_save1,y_save2;
			
			Step=(float)dlen/width;
			x_save1=x;
			y_save1=dat[0]*Max_or_Slope+Min_or_Inter;
			
			//˵����������
			if (l->Vacancy_Length==0)
			{
				for (i=1;i<width;i++)
				{
					x_save2=i+x;
					y_save2=dat[(u16)(i*Step)]*Max_or_Slope+Min_or_Inter;
					Graphic_Draw_Line(c,x_save1,y_save1,x_save2,y_save2,l);	
					x_save1=x_save2;
					y_save1=y_save2;
				}
			}
			else//˵��������
			{
				u8 j;
				signed char save;

				for (i=1;i+l->Solid_Length<width;i+=l->Solid_Length+l->Vacancy_Length)
				{
					x_save1=i+x;
					y_save1=dat[(u16)(i*Step)]*Max_or_Slope+Min_or_Inter;
					
					//����ʵ�߲���
					for (j=0;j<l->Solid_Length;j++)
					{
						x_save2=j+i+x;
						y_save2=dat[(u16)((i+j)*Step)]*Max_or_Slope+Min_or_Inter;
						Graphic_Draw_Line(c,x_save1,y_save1,x_save2,y_save2,l);	
						x_save1=x_save2;
						y_save1=y_save2;
					}
				}

				//���һ����ʵ�߷ֿ�����
				x_save1=x+i;
				y_save1=((float*)dat)[(u16)(i*Step)]*Max_or_Slope+Min_or_Inter;
				save=width-i;
				for (j=1;j<save;j++)
				{
					x_save2=j+i+x;
					y_save2=dat[(u16)((i+j)*Step)]*Max_or_Slope+Min_or_Inter;
					Graphic_Draw_Line(c,x_save1,y_save1,x_save2,y_save2,l);	
					x_save1=x_save2;
					y_save1=y_save2;
				}

			}
		}
		else	//���ݳ���С�ڻ�ͼ����ȣ����߻�ͼ
		{
			float x_save1,x_save2,y_save1,y_save2;
			
			//�����������Ҫ�ڲ�
			if (l->Vacancy_Length!=0)
				MathHelper_Linear_Interpolation(dat,dat,dlen,width);
			else
				Step=(float)width/(dlen-1);
			x_save1=x;
			y_save1=dat[0]*Max_or_Slope+Min_or_Inter;
			
			//�����������
			if (l->Vacancy_Length==0)
			{
				for (i=1;i<dlen;i++)
				{
					x_save2=i*Step+x;
					y_save2=dat[i]*Max_or_Slope+Min_or_Inter;
					Graphic_Draw_Line(c,x_save1,y_save1,x_save2,y_save2,l);
					x_save1=x_save2;
					y_save1=y_save2;
				}
			}
			else//˵��������
			{
				u8 j;
				signed char save;
				
				for (i=1;i+l->Solid_Length<width;i+=l->Solid_Length+l->Vacancy_Length)
				{
					x_save1=x+i;
					y_save1=((float*)dat)[i]*Max_or_Slope+Min_or_Inter;
					for (j=0;j<l->Solid_Length;j++)
					{
						x_save2=i+j+x;
						y_save2=((float*)dat)[i+j]*Max_or_Slope+Min_or_Inter;
						Graphic_Draw_Line(c,x_save1,y_save1,x_save2,y_save2,l);
						x_save1=x_save2;
						y_save1=y_save2;
					}
				}
				
				x_save1=x+i;
				y_save1=((float*)dat)[i]*Max_or_Slope+Min_or_Inter;
				save=width-i;
				for (j=1;j<save;j++)
				{
					x_save2+=j+i+x;
					y_save2=((float*)dat)[i+j]*Max_or_Slope+Min_or_Inter;
					Graphic_Draw_Line(c,x_save1,y_save1,x_save2,y_save2,l);
					x_save1=x_save2;
					y_save1=y_save2;
				}
			}
		}
	}
	else if (style==SCATTER)//��ɢ��ͼ
	{
		//���ݳ��ȴ��ڵ��ڻ�ͼ�����ȣ�����������ͼ
		if (dlen>=width)
		{
			short y;
			
			Step=(float)dlen/width;
			for (i=0;i<width;i++)
			{
				y=dat[(u16)(i*Step)]*Max_or_Slope+Min_or_Inter;
				Graphic_Draw_Line(c,x+i,axis_pos,x+i,y,l);	
				if (y+5>=c->Area.Height)
					y=c->Area.Height-6;
				else if (y-5<0)
					y=5;
				Graphic_Draw_Circle(c,x+i,y,5,l->Color,2);
			}
		}
		else	//���ݳ���С�ڻ�ͼ�����
		{
			short y;
			u16 x;
			
			Step=(float)width/(dlen-1);
			for (i=0;i<dlen;i++)
			{
				x=i*Step;
				y=dat[i]*Max_or_Slope+Min_or_Inter;
				Graphic_Draw_Line(c,x+x,axis_pos,x+x,y,l);
				if (y+5>=c->Area.Height)
					y=c->Area.Height-6;
				else if (y-5<0)
					y=5;
				Graphic_Draw_Circle(c,x+x,y,5,l->Color,2);
			}
		}
	}
	else if (style==COLORED)//Ⱦɫģʽ
	{
		line save;
		save.Vacancy_Length=0;
		save.Color=l->Color;
		save.Width=1;
		
		//���ݳ��ȴ��ڵ��ڻ�ͼ�����ȣ�����������ͼ
		if (dlen>=width)
		{
			short y;

			Step=(float)dlen/width;
			for (i=0;i<width;i++)
			{
				y=dat[(u16)(i*Step)]*Max_or_Slope+Min_or_Inter;
				Graphic_Draw_Line(c,x+i,axis_pos,x+i,y,&save);	
			}
		}
		else	//���ݳ���С�ڻ�ͼ����ȣ���Ҫ���в�ֵ
		{
			u16 y;
			
			MathHelper_Linear_Interpolation(dat,dat,dlen,width);
			for (i=0;i<width;i++)
			{
				y=((float*)dat)[i]*Max_or_Slope+Min_or_Inter;
				Graphic_Draw_Line(c,x+i,axis_pos,x+i,y,&save);
			}
		}
	}
}

void _Scope_Repaint(scope* scp)
{
    u16 Division_Length;
	short i;
    
	if (scp->Cover)
	{
        Graphic_Draw_Rectangle(&scp->Canvas,0,0,&scp->Display_Area);
        
		//�����ʾ����
		if (scp->Axis.Net_Display)
		{
			Division_Length=(scp->Display_Area.Width-2*scp->Display_Area.Frame_Thickness)/scp->Axis.X_Tick_Num;
			i=Division_Length*scp->Axis.X_Net_Density+scp->Axis._Y_Position;	//������X������
			while (i<scp->Display_Area.Width)
			{
				Graphic_Draw_Line(&scp->Canvas,i,scp->Display_Area.Frame_Thickness,i,scp->Display_Area.Height,&scp->Axis.Net_Line);
				i+=Division_Length*scp->Axis.X_Net_Density;
			}
			
			i=scp->Axis._Y_Position-Division_Length*scp->Axis.X_Net_Density;	//Ȼ����X������
			while (i>=0)
			{
				Graphic_Draw_Line(&scp->Canvas,i,scp->Display_Area.Frame_Thickness,i,scp->Display_Area.Height,&scp->Axis.Net_Line);
				i-=Division_Length*scp->Axis.X_Net_Density;
			}
			
            
			Division_Length=(scp->Display_Area.Height-2*scp->Display_Area.Frame_Thickness)/scp->Axis.Y_Tick_Num;
			i=Division_Length*scp->Axis.Y_Net_Density+scp->Axis._X_Position;	//Y������
			while (i<scp->Display_Area.Height)
			{
				Graphic_Draw_Line(&scp->Canvas,scp->Display_Area.Frame_Thickness,i,scp->Display_Area.Width-scp->Display_Area.Frame_Thickness,i,&scp->Axis.Net_Line);
				i+=Division_Length*scp->Axis.Y_Net_Density;
			}
			
			i=scp->Axis._X_Position-Division_Length*scp->Axis.Y_Net_Density;	//Y������
			while (i>=0)
			{
				Graphic_Draw_Line(&scp->Canvas,scp->Display_Area.Frame_Thickness,i,scp->Display_Area.Width-scp->Display_Area.Frame_Thickness,i,&scp->Axis.Net_Line);
				i-=Division_Length*scp->Axis.Y_Net_Density;
			}
		}
	}
		
	//��ʾ����
	if (scp->Curve.Display)
	{
        if (scp->Curve_Range._Valid_Y_End<scp->Curve_Range._Valid_Y_Begin)
        {
            uint16_t save=scp->Curve_Range._Valid_Y_End;
            scp->Curve_Range._Valid_Y_End=scp->Curve_Range._Valid_Y_Begin;
            scp->Curve_Range._Valid_Y_Begin=save;
        }
		_Scope_Draw_Waveform(&scp->Canvas,GUI_Buffer1,&scp->Curve.Curve_Line,
							scp->Curve_Range._Valid_X_Begin,
							scp->Curve_Range._Valid_Y_Begin,
							scp->Curve_Range._Valid_X_End-scp->Curve_Range._Valid_X_Begin,
							scp->Curve_Range._Valid_Y_End-scp->Curve_Range._Valid_Y_Begin,
							scp->Axis._X_Position,
							scp->Curve_Range._Length,
							scp->Style);
	}
	
	//�����ʾ������
	if (scp->Axis.Axis_Display)
	{
		//����X��
		Graphic_Draw_Rectangle2(&scp->Canvas,scp->Display_Area.Frame_Thickness,scp->Axis._X_Position-scp->Axis.Axis_Line.Width/2,
									scp->Display_Area.Width-2*scp->Display_Area.Frame_Thickness,scp->Axis.Axis_Line.Width,0,65535,scp->Axis.Axis_Line.Color);
        //����Y��
		Graphic_Draw_Rectangle2(&scp->Canvas,scp->Axis._Y_Position-scp->Axis.Axis_Line.Width/2,scp->Display_Area.Frame_Thickness,
									scp->Axis.Axis_Line.Width,scp->Display_Area.Height-2*scp->Display_Area.Frame_Thickness,0,65535,scp->Axis.Axis_Line.Color);
        
		//���ƿ̶ȵ�
		Division_Length=(scp->Display_Area.Width-2*scp->Display_Area.Frame_Thickness)/scp->Axis.X_Tick_Num;
		i=Division_Length+scp->Axis._Y_Position+scp->Display_Area.Frame_Thickness;	//������X������
		signed char save;
		if (scp->Axis._X_Position>=scp->Canvas.Area.Height/2)
			save=-4-scp->Axis.Axis_Line.Width/2;
		else
			save=scp->Axis.Axis_Line.Width/2;
		while (i<scp->Display_Area.Width)
		{
			Graphic_Draw_Rectangle2(&scp->Canvas,i,scp->Axis._X_Position+save,2,4,0,65535,scp->Axis.Axis_Line.Color);
			i+=Division_Length;
		}
		
		i=scp->Axis._Y_Position-Division_Length;	//Ȼ����X������
		while (i>=scp->Display_Area.Frame_Thickness)
		{
			Graphic_Draw_Rectangle2(&scp->Canvas,i,scp->Axis._X_Position+save,2,4,0,65535,scp->Axis.Axis_Line.Color);
			i-=Division_Length;
		}
		
		Division_Length=(scp->Display_Area.Height-2*scp->Display_Area.Frame_Thickness)/scp->Axis.Y_Tick_Num;
		i=Division_Length+scp->Axis._X_Position;	//Y������
		if (scp->Axis._Y_Position>=scp->Canvas.Area.Width/2)
			save=-4-scp->Axis.Axis_Line.Width/2;
		else
			save=scp->Axis.Axis_Line.Width/2;
		while (i<scp->Display_Area.Height-scp->Display_Area.Frame_Thickness)
		{
			Graphic_Draw_Rectangle2(&scp->Canvas,scp->Axis._Y_Position+save,i,4,2,0,65535,scp->Axis.Axis_Line.Color);
			i+=Division_Length;
		}
		
		i=scp->Axis._X_Position-Division_Length;	//Y������
		while (i>=0)
		{
			Graphic_Draw_Rectangle2(&scp->Canvas,scp->Axis._Y_Position+save,i,4,2,0,65535,scp->Axis.Axis_Line.Color);
			i-=Division_Length;
		}	
	}
}


















void GUI_Init(gui_controller* guictrl, screen* sc, touch_device* td, uint16_t color)
{
    //����������
    GUI_Buffer0=Malloc(GUI_BUFFER_HEAP,GUI_BUFFER_SIZE);
	GUI_Buffer1=Malloc(GUI_BUFFER_HEAP,GUI_BUFFER_SIZE);
    
    //�󶨷���
    guictrl->Add_Layer=Method_GUI_Controller_Add_Layer;
    guictrl->Remove_Layer=Method_GUI_Controller_Remove_Layer;
    guictrl->Create_Button=Method_GUI_Controller_Create_Button;
    guictrl->Create_Label=Method_GUI_Controller_Create_Label;
    guictrl->Create_Panel=Method_GUI_COntroller_Create_Panel;
    guictrl->Create_ToolBox=Method_GUI_Controller_Create_ToolBox;
    guictrl->Create_Scope=Method_GUI_Controller_Create_Scope;
    guictrl->Process=Method_GUI_Controller_Process;
    guictrl->ActiveLayer_Set=Method_GUI_Controller_ActiveLayer_Set;
    
    _RO_WRITE(guictrl->Active_Layer,layer_handler,0);
    _RO_WRITE(guictrl->Focus_Component,component*,NULL);
    _RO_WRITE(guictrl->TouchDevice,touch_device*,td);
    guictrl->Updated=Disable;
    //����ȫ�ִ�������
    guictrl->Overall_TouchArea=td->Create_Area(td,0,0,sc->Width,sc->Height);
    guictrl->Overall_TouchArea->All_Event_Enable=Disable;
    guictrl->Overall_TouchArea->Click_Enable=Enable;
    guictrl->Overall_TouchArea->Click_CallBackFunc=Overall_Click_Callback;
    //�봥���豸��������
    td->Association=guictrl;
    //����Ļ������ϵ
    guictrl->Screen=sc;
    //ͼ�������ʼ��
    LinkedList_Prepare(&guictrl->Layers,sizeof(component_layer));
    //����������
    component_layer* cl=LinkedList_Add(&guictrl->Layers,0);
    //�������ʼ��
    cl->Active=Enable;
    cl->Color=color;
    LinkedList_Prepare(&cl->Components,sizeof(components_union));
    //���Ϊ��ʼ������
    _RO_WRITE(guictrl->IsInit,status_flag,Enable);
}











void Global_PressDown_Callback(touch_device* td, touch_area* ta)
{
    gui_controller* gui=td->Association;
    component* cm=ta->Association;
    
    if (cm->Type==BUTTON)
    {
        button* btn=&cm->Component.Button;
        if (btn->Active)    //����ؼ�������
            _RO_WRITE(btn->Event_PressDown_Happened,status_flag,Enable);
    }
    else if (cm->Type==TOOLBOX)
    {
        toolbox* tbx=&cm->Component.ToolBox;
        if (tbx->Active)
            _RO_WRITE(tbx->Event_PressDown_Happened,status_flag,Enable);
    }
    else if (cm->Type==SCOPE)
    {
        scope* scp=&cm->Component.Scope;
        if (scp->Active)
            _RO_WRITE(scp->Event_PressDown_Happened,status_flag,Enable);
    }
}

void Global_Release_Callback(touch_device* td, touch_area* ta)
{
    gui_controller* gui=td->Association;
    component* cm=ta->Association;
    
    if (cm->Type==BUTTON)
    {
        button* btn=&cm->Component.Button;
        if (btn->Active)    //����ؼ�������
            _RO_WRITE(btn->Event_Release_Happened,status_flag,Enable);
    }
    else if (cm->Type==TOOLBOX)
    {
        toolbox* tbx=&cm->Component.ToolBox;
        if (tbx->Active)    //����ؼ�������
            _RO_WRITE(tbx->Event_Release_Happened,status_flag,Enable);
    }
    else if (cm->Type==SCOPE)
    {
        scope* scp=&cm->Component.Scope;
        if (scp->Active)    //����ؼ�������
            _RO_WRITE(scp->Event_Release_Happened,status_flag,Enable);
    }
}

void Global_KeepPress_Callback(touch_device* td, touch_area* ta)
{
    gui_controller* gui=td->Association;
    component* cm=ta->Association;
    
    if (cm->Type==BUTTON)
    {
        button* btn=&cm->Component.Button;        
        if (btn->Active)    //����ؼ�������
            _RO_WRITE(btn->Event_KeepPress_Happened,status_flag,Enable);
    }
    else if (cm->Type==TOOLBOX)
    {
        toolbox* tbx=&cm->Component.ToolBox;
        if (tbx->Active)
            _RO_WRITE(tbx->Event_KeepPress_Happened,status_flag,Enable);
    }
    else if (cm->Type==SCOPE)
    {
        scope* scp=&cm->Component.Scope;
        if (scp->Active)    //����ؼ�������
            _RO_WRITE(scp->Event_KeepPress_Happened,status_flag,Enable);
    }
}

void Global_Click_Callback(touch_device* td, touch_area* ta)
{
    gui_controller* gui=td->Association;
    component* cm=ta->Association;
    
    if (cm->Type==BUTTON)
    {
        button* btn=&cm->Component.Button;
        if (btn->Active)    //����ؼ�������
            _RO_WRITE(btn->Event_Click_Happened,status_flag,Enable);
    }
    else if (cm->Type==TOOLBOX)
    {
        toolbox* tbx=&cm->Component.ToolBox;
        if (tbx->Active)
            _RO_WRITE(tbx->Event_Click_Happened,status_flag,Enable);
    }
    else if (cm->Type==TOOLBOX_BUTTON)
    {
        toolbox_button* tbx_btn=&cm->Component.ToolBox_Button;
        if (tbx_btn->Active)
            _RO_WRITE(tbx_btn->Event_Click_Happened,status_flag,Enable);
    }
    else if (cm->Type==SCOPE)
    {
        scope* scp=&cm->Component.Scope;
        if (scp->Active)    //����ؼ�������
            _RO_WRITE(scp->Event_Click_Happened,status_flag,Enable);
    }
}

void Overall_Click_Callback(touch_device* td, touch_area* ta)
{
    gui_controller* guictrl=td->Association;
    toolbox* tbx=&guictrl->Focus_Component->Component.ToolBox;
    
    //����������а�����������0
    if (tbx->Buttons.Nodes_Num>0)
    {
        toolbox_button* tbx_btn=&((component*)LinkedList_Find(&tbx->Buttons,0))->Component.ToolBox_Button;
        //�ж��Ƿ��ڰ���������
        if ((ta->PointX<tbx_btn->Canvas.Area.X+tbx_btn->Canvas.Area.Width)&&(ta->PointX>tbx_btn->Canvas.Area.X))
        {
            if (!((ta->PointY<tbx_btn->Canvas.Area.Y+tbx_btn->Canvas.Area.Height*tbx->Buttons.Nodes_Num)&&(ta->PointY>tbx_btn->Canvas.Area.Y)))
                _RO_WRITE(tbx->Event_LostFocus_Happened,status_flag,Enable);
        }
        else
            _RO_WRITE(tbx->Event_LostFocus_Happened,status_flag,Enable);
    }
    else
        _RO_WRITE(tbx->Event_LostFocus_Happened,status_flag,Enable);
}






















void GUI_Panel_Info_Print(USART_TypeDef* usart, panel* pnl)
{
    int i;
    
    USART_Printf(usart,"\nRow Number: %d\n",pnl->RowsNum);
    USART_Printf(usart,"Line Number: %d\n",pnl->LinesNum);
    
    USART_Printf(usart,"Block Width: ");
    for (i=0;i<pnl->LinesNum;i++)
        USART_Printf(usart,"%d  ",pnl->Blocks_Width[i]);
    
    USART_Printf(usart,"\nBlock Height: ");
    for (i=0;i<pnl->RowsNum;i++)
        USART_Printf(usart,"%d  ",pnl->Blocks_Height[i]);
    
    USART_Printf(usart,"\nX: ");
    for (i=0;i<pnl->LinesNum;i++)
        USART_Printf(usart,"%d  ",pnl->Blocks_X[i]);
    
    USART_Printf(usart,"\nY: ");
    for (i=0;i<pnl->RowsNum;i++)
        USART_Printf(usart,"%d  ",pnl->Blocks_Y[i]);
    USART_Printf(usart,"\n");
}

void GUI_Info_Print(USART_TypeDef* usart, gui_controller* guictrl)
{
    int i,j;
    component_layer* cl;
    component* cm;
    
    USART_Printf(usart,"\n\nGUI_Controller info:\n");
    USART_Printf(usart,"Active layer:%d\n",guictrl->Active_Layer);
    USART_Printf(usart,"Layer number:%d\n",guictrl->Layers.Nodes_Num);
    for (i=0;i<guictrl->Layers.Nodes_Num;i++)
    {
        USART_Printf(usart,"Layer%d:\n",i);
        cl=LinkedList_Find(&guictrl->Layers,i);
        USART_Printf(usart,"    Is Active:%d | Components number:%d | Color:%#x\n",cl->Active,cl->Components.Nodes_Num,cl->Color);
        for (j=0;j<cl->Components.Nodes_Num;j++)
        {
            USART_Printf(usart,"\n    Component#%d: Type:",j);
            cm=LinkedList_Find(&cl->Components,j);
            if (cm->Type==BUTTON)
                USART_Printf(usart,"Button(%#x)\n",cm);
            else if (cm->Type==PANEL)
                USART_Printf(usart,"Panel(%#x)\n",cm);
            else if (cm->Type==LABEL)
                USART_Printf(usart,"Label(%#x)\n",cm);
            else if (cm->Type==TOOLBOX)
                USART_Printf(usart,"Toolbox(%#x)\n",cm);
            else if (cm->Type==SCOPE)
                USART_Printf(usart,"Scope(%#x)\n",cm);
        }
    }
}







































/*Scope���ã���Ҫ������Curve_Range�еĸ��������Լ�X��Y������*/
void _GUI_ScopeSet(scope* Scope);

/*��ʾ��������*/
void _GUI_DisplayWaveform(canvas* Canvas, float* Data, line* Line, u16 X, u16 Y, u16 Wide, u16 Height, u16 Axis_Position, u16 Data_Length, u8 Style);




