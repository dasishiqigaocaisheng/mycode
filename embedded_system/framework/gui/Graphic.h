/*******************************************************************************
*|-----------------------------------ͼ�ο�-----------------------------------|*
*|-------------------------------@����ʮ�߸߲���------------------------------|*
*|----------------------------------2021.1.22---------------------------------|*
*|�����Դ������ͼ�ο⣬�ṩ������ͼ�λ��ƺ���--------------------------------|*
********************************************************************************/
#ifndef _GRAPHIC_H_
#define _GRAPHIC_H_

#include "BMP.h"
#include "CharLib.h"
#include "Framework.h"

//������䣬�ڻ��ƾ���ʱ�����߿��Ȳ������ڸ�ֵʱ������������ľ���
#define RECTANGLE_FILL 65535

//������ʾ����/����������
#define INF  -1

//������ɫ
#define WHITE			0xffff  //��
#define BLACK			0x0000  //��
#define TIANYI_BLUE		0x667f  //��
#define ALING_RED		0xe800  //��
#define YANHE_GREEN		0x7f9   //��
#define LONGYA_GREEN	0x32c   //��
#define QINGXIAN_YELLOW	0xffe0  //��
#define MOKE_BLUE		0x41f   //��
#define PURE_BLUE		0x1f	//����
#define PURE_RED		0xf800	//����
#define DEEP_GRAY       0x3186  //���
#define GRAY            0x738e  //��

//���������ַ�����ˮƽ���뷽ʽ
typedef enum
{
    Left_Align,     //�����
    Right_Align,    //�Ҷ���
    HorMid          //����
} horalign_type;

//���������ַ����Ĵ�ֱ���뷽ʽ
typedef enum
{
    Top_Align,      //�϶���
    Bottom_Align,   //�¶���
    VerMid          //����
} veralign_type;

typedef struct canvas_class canvas;

/************************************************************
*�����ࣺ������Ļ�ϵ�һ���������							*
*************************************************************/
typedef struct area_class
{
    //���Ͻ�X����
    uint16_t X;
    
    //���Ͻ�Y����
    uint16_t Y;
    
    //���
    uint16_t Width;
    
    //�߶�
    uint16_t Height;
} area;

/************************************************************
*���ࣺ�����ߵ�����������������ߣ���������ʱ�������Ȼ���	*
*	   һС��ʵ�ߣ�Ȼ���ȱһС�Σ������ѭ������ֱ������	*
*	   ֱ����λ�á�											*
*************************************************************/
typedef struct line_class
{
	//�߿�
	uint8_t Width;
	
	//��ɫ
	uint16_t Color;
	
	//ʵ�߲��ֵĳ��ȣ���Vacancy_LengthΪ0ʱ����ֵ��������
	uint8_t Solid_Length;
	
	//��ȱ���ֵĳ��ȣ�����ֵΪ0ʱ��Solid_Length��������
	uint8_t Vacancy_Length;
} line;

/************************************************************
*�����ࣺ�������ε�������������ƴ��߿�ľ��Σ��߿��ȵ�	*
*		 ��0ʱֻ����ʵ�������߿��ȴ�������һ�ߵĶ���֮һ	*
*		 ʱֻ����ʾ�߿�									*
*************************************************************/
typedef struct rectangle_class
{
	//���
	uint16_t Width;
	
	//�߶�
	uint16_t Height;
	
	//������ɫ
	uint16_t Background_Color;
	
	//�߿���
	uint16_t Frame_Thickness;
	
	//�߿���ɫ
	uint16_t Frame_Color;
	
	//�Ƿ���ʾ�������������0����ֻ����ʾ�߿�
	uint8_t Display_Background;
} rectangle;

/************************************************************
*�����ࣺ��Ļ��ʾ�Ļ�����λ���κ�������Ҫ����Ļ����ʾ����   *
*		 ����Ҫ����һ��������ÿһ�������඼��Ӧ���Դ��ϵ�   *
*		 һ��ʵ�ʵ��ڴ棬�κζԻ����Ĳ������Ƕ���һ���ڴ�	*
*		 �Ĳ������κ�һ�黭��������ӵ���������ӻ���			*
*************************************************************/
struct canvas_class
{
	//�Ƿ���ʾ
	volatile status_flag Display;
    
    vro status_flag Virtual;
    
    volatile status_flag WriteThrough;
	
    //��������
	area Area;
	
	//��Ӧ���Դ��ַ
	uint16_t* Address;
};

/************************************************************
*�����ࣺ������������                                     	*
*************************************************************/
typedef struct FontClass
{
    //������ɫ
	uint16_t BackColor;
    
    //������ɫ
	uint16_t CharColor;
    
    //�ֿ��ַ
    const charmold* Mold;
} font;

/************************************************************
*��Ļ�ࣺ������Ļ��������                                   *
*************************************************************/
typedef struct screen_class
{
    //����
    uint16_t  Width;
    
    //����
    uint16_t  Height;
    
    //�Դ��ַ������˫�Դ�ģʽ����Ч�����ѡ���Դ�ģʽ����ֵΪ0��
    uint16_t* GRAM;
    
    //GRAM����Ӧ�Ļ���
    canvas    Canvas; 
} screen;

//��������Ļ����Ϣ
extern screen ScreenDevice;







/****************************************************************************
*���ܣ�ͼ�ο⻺���ʼ����Ϊ�������ռ�										
*������1.width������
*      2.height������
*      3.gram���ڶ��Դ�ĵ�ַ�����ѡ��˫�Դ�ģʽ��
*****************************************************************************/
void Graphic_Init(uint16_t width, uint16_t height, status_flag vrt, uint16_t* gram);

/****************************************************************************
*���ܣ���Ļˢ�£�˫�Դ棩					
*˵�������ڶ��Դ������ˢ�µ���Ļ�Դ�
*****************************************************************************/
void Graphic_Screen_Refresh(void);

/****************************************************************************
*���ܣ���Ļˢ�£�˫�Դ棩					
*˵�������Խ��ڶ��Դ������ˢ�µ���Ļ�Դ�
*****************************************************************************/
void Graphic_Screen_Refresh_Try(void);

/****************************************************************************
*���ܣ���������																*
*������1.x���������ϽǺ�����												*
*	   2.y���������Ͻ�������												*
*	   3.width�����														*
*	   4.height���߶�														*
*����ֵ�������Ļ���															*
*****************************************************************************/
void Graphic_CreateCanvas(canvas* c ,uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, status_flag vrt);
//ʹ��һ��area�ṹ����������������
void Graphic_CreateCanvas2(canvas* c ,area a, status_flag vrt);

/****************************************************************************
*���ܣ�ɾ������																*
*������1.Canvas��������ַ													*
*˵����ɾ������������ɾ���û����������ӻ����������ӻ������ӻ�������Ȼ��ɾ��	*
*	   ���������ͷ��Դ�ռ�												*
*****************************************************************************/
void Graphic_Canvas_Delete(canvas* c);

void Graphic_Virtual_Set(canvas* c, status_flag state);

/****************************************************************************
*���ܣ�ˢ�»�������															*
*������1.Canvas��������ַ													*
*˵�������ô˺������������������������ӻ������Դ�������ʾ����Ļ�ϡ����ӻ���	*
*	   �����ݻḲ�Ǹ����������ݡ��������л������ƺ�����ֻ�е��ô˺�����	*
*	   ���ݲŻ���µ���Ļ�ϡ�												*
*****************************************************************************/
void Graphic_RefreshCanvas(canvas* c);
void Graphic_RefreshCanvas_Try(canvas* c);



/****************************************************************************
*���ܣ������黭����ʾĳ����ɫ												*
*������1.c��Ҫ���ƵĻ���												*
*	   2.color����ɫ														*
*****************************************************************************/
void Graphic_Draw_Color(canvas* c, uint16_t color);

/*********************************************************************************
*���ܣ���ʾ���Σ�����ģʽ��													
*������1.c��Ҫ���ƵĻ���											
*	   2.x���������Ͻ���Ի�����x����										
*	   3.y���������Ͻ���Ի�����y����						
*	   4.r�������࣬�������˸þ��ε���ʽ						
*˵�����ú��������������ͣ������е�ֻ�ǲ��������仯���е����ܲ�ͬ
*      1.Graphic_Canvas_DisplayRectangle2����r�滻Ϊһ���ã�width����
*        height���ߣ���bcolor��������ɫ����thickness���߿��ȣ���fcolor���߿���ɫ��
*        �������ľ�������
*      2.Graphic_Canvas_DisplayRectangle3��x,y,width,height����������������
*        thickness��ʾ�߿��ȣ�fcolor��ʾ�߿���ɫ���÷���������ʾ���б���
*        �ľ��Ρ�
*      3.Graphic_Canvas_DisplayRectangle4��x,y,width,height����������������
*        color��������ɫ���÷���Ĭ�Ͼ��α߿���Ϊ0��
***********************************************************************************/
void Graphic_Draw_Rectangle(canvas* c, uint16_t re_x, uint16_t re_y, rectangle* r);
void Graphic_Draw_Rectangle2(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, uint16_t bcolor, uint16_t thickness, uint16_t fcolor);
void Graphic_Draw_Rectangle3(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, uint16_t thickness, uint16_t fcolor);
void Graphic_Draw_Rectangle4(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, uint16_t color);

/****************************************************************************
*���ܣ���ʾ�߶Σ�����ģʽ��										
*������1.c��Ҫ���ƵĻ���											
*	   2.re_x1����һ����Ի����ĺ�����											
*	   3.re_y1����һ����Ի�����������											
*	   4.re_x2������һ����Ի����ĺ�����										
*	   5.re_y2������һ����Ի�����������										
*	   6.Line�����࣬���������ߵ���ʽ										
*˵�����ú�����һ�������ͣ������в�ͬ��ʽ�Ĳ���
*      1.Graphic_Canvas_DisplayLine2��ʹ�ò���width���߿���color������ɫ����
*        slen��ʵ�߳��ȣ���vlen�����߳��ȣ�������l
*****************************************************************************/
void Graphic_Draw_Line(canvas* c, uint16_t re_x1, uint16_t re_y1, uint16_t re_x2, uint16_t re_y2, line* l);
void Graphic_Draw_Line2(canvas* c, uint16_t re_x1, uint16_t re_y1, uint16_t re_x2, uint16_t re_y2, uint8_t width, uint16_t color, uint8_t slen, uint8_t vlen);

/****************************************************************************
*���ܣ���ʾԲ������ģʽ��									
*������1.c��Ҫ���ƵĻ���											
*	   2.re_x��Բ����Ի����ĺ�����											
*	   3.re_y��Բ����Ի�����������											
*	   4.r���뾶															
*	   5.color����ɫ														
*	   6.thickness��Բ�����												
*****************************************************************************/
void Graphic_Draw_Circle(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t r, uint16_t color, uint8_t thickness);

/****************************************************************************
*���ܣ���ʾһ��BMP��ʽͼƬ������ģʽ��													
*������1.c��Ҫ���ƵĻ���												
*	   2.re_x��ͼƬ���Ͻ�����ڻ����ĺ�����									
*	   3.re_y��ͼƬ���Ͻ�����ڻ�����������																				
*	   4.p��ͼƬ��BMP�ṹ���ַ										
*˵�����ú������������������ͣ������ڲ����͹����϶���һ������
*      1.Graphic_Canvas_DisplayBMP2��width��heightָ����ʾ����Ŀ�͸ߣ���
*        ͼƬ��ƽ���������ڣ�����ζ��ͼ����ܻᱻ������
*      2.Graphic_Canvas_DisplayBMP3��size��ʾͼ������ű�����ͼ��ᰴ�����
*        ϵ���������š�
*****************************************************************************/
void Graphic_Draw_BMP(canvas* c, uint16_t re_x, uint16_t re_y, const BMP* p);
void Graphic_Draw_BMP2(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, const BMP* p);
void Graphic_Draw_BMP3(canvas* c, uint16_t re_x, uint16_t re_y, float size, const BMP* p);


/****************************************************************************
*���ܣ���ʾһ��rBMP��ʽͼƬ������ģʽ��											
*������1.c��Ҫ���ƵĻ���												
*	   2.re_x��ͼƬ���Ͻ�����ڻ����ĺ�����									
*	   3.re_y��ͼƬ���Ͻ�����ڻ�����������																				
*	   6.p��ͼƬ��rBMP�ṹ���ַ			
*˵�����ú��������������ͣ������ڲ����͹�������һ������
*      1.Graphic_Canvas_DisplayrBMP2��width��heightָ����ʾ����Ŀ�͸ߣ���
*        ͼƬ��ƽ���������ڣ�����ζ��ͼ����ܻᱻ������
*      2.Graphic_Canvas_DisplayrBMP3��size��ʾͼ������ű�����ͼ��ᰴ�����
*        ϵ���������š�
*****************************************************************************/
void Graphic_Draw_rBMP(canvas* c, uint16_t re_x, uint16_t re_y, const rBMP* p);
void Graphic_Draw_rBMP2(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, const rBMP* p);
void Graphic_Draw_rBMP3(canvas* c, uint16_t re_x, uint16_t re_y, float size, const rBMP* p);

/****************************************************************************
*���ܣ���ʾһ��pBMP��ʽ��ͼƬ												
*������1.c��Ҫ���ƵĻ���											
*	   2.re_x��ͼƬ���Ͻ�����ڻ����ĺ�����									
*	   3.re_y��ͼƬ���Ͻ�����ڻ�����������									
*	   6.p��ͼƬ��pBMP�ṹ���ַ										
*˵�����ú��������������ͣ������ڲ����͹�������һ������
*      1.Graphic_Canvas_DisplaypBMP2��width��heightָ����ʾ����Ŀ�͸ߣ���
*        ͼƬ��ƽ���������ڣ�����ζ��ͼ����ܻᱻ������
*      2.Graphic_Canvas_DisplaypBMP3��size��ʾͼ������ű�����ͼ��ᰴ�����
*        ϵ���������š�
*****************************************************************************/
void Graphic_Draw_pBMP(canvas* c, uint16_t re_x, uint16_t re_y, const pBMP* p);
void Graphic_Draw_pBMP2(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, const pBMP* p);
void Graphic_Draw_pBMP3(canvas* c, uint16_t re_x, uint16_t re_y, float size, const pBMP* p);

/****************************************************************************
*���ܣ���ʾһ��prBMP��ʽ��ͼƬ												
*������1.c��Ҫ���ƵĻ���												
*	   2.re_x��ͼƬ���Ͻ�����ڻ����ĺ�����									
*	   3.re_y��ͼƬ���Ͻ�����ڻ�����������									
*	   6.p��ͼƬ��prBMP�ṹ���ַ				
*˵�����ú��������������ͣ������ڲ����͹�������һ������
*      1.Graphic_Canvas_DisplayprBMP2��width��heightָ����ʾ����Ŀ�͸ߣ���
*        ͼƬ��ƽ���������ڣ�����ζ��ͼ����ܻᱻ������
*      2.Graphic_Canvas_DisplayprBMP3��size��ʾͼ������ű�����ͼ��ᰴ�����
*        ϵ���������š�
*****************************************************************************/
void Graphic_Draw_prBMP(canvas* c, uint16_t re_x, uint16_t re_y, const prBMP* p);
void Graphic_Draw_prBMP2(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, const prBMP* p);
void Graphic_Draw_prBMP3(canvas* c, uint16_t re_x, uint16_t re_y, float size, const prBMP* p);


void Graphic_Draw_BMP_All(canvas* c, uint16_t re_x, uint16_t re_y, const bmp* p);
void Graphic_Draw_BMP_All2(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, const bmp* p);
void Graphic_Draw_BMP_All3(canvas* c, uint16_t re_x, uint16_t re_y, float size, const bmp* p);



void Graphic_Draw_Char(canvas* c, uint16_t re_x, uint16_t re_y, uint8_t ch, uint16_t width, uint16_t height, uint16_t bcolor, uint16_t ccolor, const charmold* mold);
void Graphic_Draw_Char2(canvas* c, uint16_t re_x, uint16_t re_y, uint8_t ch, uint16_t bcolor, uint16_t ccolor, const charmold* mold);



void Graphic_Draw_String(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, uint8_t valign, uint8_t halign, font* f, char* str);
void Graphic_Draw_String2(canvas* c, uint16_t re_x, uint16_t re_y, short line, short row, uint8_t valign, uint8_t halign, font* f, char* str);
void Graphic_Draw_String3(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width ,uint16_t height, uint8_t valign, uint8_t halign, uint16_t bcolor, uint16_t ccolor, const charmold* mold, char* str);
void Graphic_Draw_String4(canvas* c, uint16_t re_x, uint16_t re_y, short line, short row, uint8_t valign, uint8_t halign, uint16_t bcolor, uint16_t ccolor, const charmold* mold, char* str);



void Graphic_Printf(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, uint8_t valign, uint8_t halign, font* f, char* fstr, ...);
void Graphic_Printf2(canvas* c, uint16_t re_x, uint16_t re_y, short line, short row, uint8_t valign, uint8_t halign, font* f, char* fstr, ...);
void Graphic_Printf3(canvas* c, uint16_t re_x, uint16_t re_y, uint16_t width, uint16_t height, uint8_t valign, uint8_t halign, uint16_t bcolor, uint16_t ccolor, const charmold* mold, char* fstr, ...);
void Graphic_Printf4(canvas* c, uint16_t re_x, uint16_t re_y, short line, short row, uint8_t valign, uint8_t halign, uint16_t bcolor, uint16_t ccolor, const charmold* mold, char* fstr, ...);



#endif
