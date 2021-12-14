#ifndef _TOUCH_H_
#define _TOUCH_H_

#include "Framework.h"
#include "LinkedList.h"

/*
*�¼�����
*/
typedef enum
{
    PressDown,  //�����¼�
    Release,    //�ɿ��¼�
    KeepPress,  //���������¼�
    Click
} touch_event_type;

/*
*������Ļ����ת����
*/
typedef enum
{
    CLOCKWISE0,      //����ת
    CLOCKWISE90,     //˳ʱ��90��
    CLOCKWISE180,    //˳ʱ��180��
    CLOCKWISE270     //˳ʱ��270��
} rotation_type;

typedef struct touch_area_class     touch_area;
typedef struct touch_device_class   touch_device;

typedef void (*Touch_Event_Callback)(touch_device* td, touch_area* ta);

/*
*����������
*������������Ļ�������
*/
struct touch_area_class
{
                short        X;                     //����X����
                short        Y;                     //����Y����
                uint16_t     Width;                 //������
                uint16_t     Height;                //����߶�
    
    volatile    status_flag  All_Event_Enable;      //�����¼�ʹ��
    
    volatile    status_flag  PressDown_Enable;      //�����¼�ʹ��
    volatile    status_flag  Release_Enable;        //�ɿ��¼�ʹ��
    volatile    status_flag  KeepPress_Enable;      //���������¼�ʹ��
    volatile    status_flag  Click_Enable;
    
    vro         status_flag  PressDown_Happened;    //�����¼�����
    vro         status_flag  Release_Happened;      //�ɿ��¼�����
    vro         status_flag  KeepPress_Happened;    //���������¼�����
    vro         status_flag  Click_Happened;
    
    vro         status_flag  IsKeyDown;             //�Ƿ��д��㰴��
    vro         status_flag  IsKeyDown_Old;
    
    vro         status_flag  IsMoved;               //���㰴�º��Ƿ����ƶ�
    
    vro         short        PointX;                //����X����
    vro         short        PointY;                //����Y����
    vro         short        PointX_Old;            //����X����
    vro         short        PointY_Old;            //����Y����
    
                void*        Association;           //��������
    
    /*���¼��Ļص�����*/
    Touch_Event_Callback         PressDown_CallBackFunc;
    Touch_Event_Callback         Release_CallBackFunc;
    Touch_Event_Callback         KeepPress_CallBackFunc;
    Touch_Event_Callback         Click_CallBackFunc;
};

/*
*�����豸��
*���������豸�Ļ�������
*/
struct touch_device_class
{
    
    /************************************��Ա************************************/
    
    /*
    *����/�ߣ�����������ָ����ȡ����ԭʼ��������ķ�Χ
    *�⼸��ֵ�ɴ�������Ӳ���豸����
    */
    vro uint16_t        ScreenWidth;    //����
    vro uint16_t        ScreenHeight;   //����
    
    /*
    *��Щ����ָ���趨�����귶Χ�Լ���ת�Ƕ�
    *��Щֵ���û��Լ�����
    */
    vro short           X_Max;      //X���ֵ
    vro short           X_Min;      //X��Сֵ
    vro short           Y_Max;      //Y���ֵ
    vro short           Y_Min;      //Y��Сֵ
    vro rotation_type   Rotation;   //��ת�Ƕ�
    
    /*��������任�Ĳ���*/
    vro float           Xk;
    vro float           Xb;
    vro float           Yk;
    vro float           Yb;
    
    /*�⼸��ֵ�����趨��Ч�Ĵ�����Χ*/
        short           ValidArea_X;
        short           ValidArea_Y;
        short           ValidArea_Width;
        short           ValidArea_Height;
        
    /*�Ƿ��д��㰴��*/
    vro status_flag     IsKeyDown;
    
    /*���������*/
    vro short           PointX;
    vro short           PointY;
    
        linkedlist      TouchArea_List; //�洢���д������������
        
        void*           Association;    //��������
    
    
    
    
    
    /************************************����************************************/
    
    /***************************************************************************************************************
    *���ܣ�������������
    *������1.td�������豸��ʵ�����������򽫴����ڸ��豸��
    *      2.x�������x����
    *      3.y�������y����
    *      4.width������Ŀ��
    *      5.height������ĸ߶�
    *����ֵ��ʵ���ĵ�ַ
    ***************************************************************************************************************/
    touch_area* (*Create_Area)    (struct touch_device_class* td, short x, short y, uint16_t width, uint16_t height);
    
    /*************************************************************************************************************
    *���ܣ������������򣬸÷�����Create_Area������
    *������1.td�������豸��ʵ�����������򽫴����ڸ��豸��
    *      2.xmin��x��С����
    *      3.xmax��x�������
    *      4.ymin��y��С����
    *      5.ymax��y�������
    *����ֵ��ʵ���ĵ�ַ
    *˵�����÷�������һ����x������(xmin,xmax)��y������(ymin,ymax)�޶��Ĵ�������
    *************************************************************************************************************/
    touch_area* (*Create_Area2)   (struct touch_device_class* td, short xmin, short xmax, short ymin, short ymax);
    
    /*****************************************************************************
    *���ܣ�ɾ���������򣬲��ͷ�����Դ
    *������1.td�������豸��ʵ��
    *      2.ta��Ҫɾ���Ĵ��������ʵ��
    *****************************************************************************/
    void        (*Delete_Area)    (struct touch_device_class* td, touch_area* ta);

    /*********************************************************************************
    *���ܣ�ִ�д����������̺�����
    *������1.td�������豸��ʵ��
    *˵���������ڸú����и��´�����״̬������¼��Ƿ�����Ӧ��������ִ�иú�������
    *      ��ⴥ����״̬���Դ�����ʱ������Ӧ
    *********************************************************************************/
    void        (*Process)        (struct touch_device_class* td);
};




/*****************************************************************************************************************************************
*���ܣ������豸��ʼ��
*������1.td�������豸��ʵ��
*      2.sw������
*      3.sh������
*      4.xmax��X�������ֵ
*      5.xmin��X������Сֵ
*      6.ymax��Y�������ֵ
*      7.ymin��Y������Сֵ
*      8.rotation����ת�Ƕ�
*****************************************************************************************************************************************/
void TouchDevice_Init(touch_device* td, uint16_t sw, uint16_t sh ,short xmax, short xmin, short ymax, short ymin, rotation_type rotation);








#endif
