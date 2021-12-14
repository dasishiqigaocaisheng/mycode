#ifndef _AD736X_H_
#define _AD736X_H_

//������
#include "Framework.h"

//��AD7366����AD7367��
//���ߵ�������12bit/14bit
typedef enum
{
	AD7366=12,
	AD7367=14
} ad736x_device_type_type;

//���������
typedef enum
{
	ONE_LINE,     //����
	TWO_LINE      //˫��
} ad736x_line_num_type;


typedef struct ad736x_class
{
    //�������ͣ�Ҳ���ǣ�AD7366��AD7367
	ro ad736x_device_type_type 	Device_Type;
    
    
    ro sign status_flag         Is_FPGA_Mode;  
    

    ro periperal                DMA;
		
    //˫�߻���
	ro ad736x_line_num_type		Line_Num;
			
    //CNVST����
	ro gpio_type				CNVST_Pin;
	
    //BUSY����
	ro gpio_type				BUSY_Pin;
	
    //CS����
	ro gpio_type				CS_Pin;
	
    //SCLK����
	ro gpio_type				SCLK_Pin;
	
    //DOUTA����
	ro gpio_type				DOUTA_Pin;
	
    //DOUTB����
	ro gpio_type				DOUTB_Pin;
    
    ro gpio_type                START;
    
    ro gpio_type                RST;
    
    //����һ��ת��
    void(*AD736x_Start_Sample)(struct ad736x_class* dev, uint32_t fs, uint16_t num, void* datbuf1, void* datbuf2, data_format_type fmt);
    
    //����һ��ת��
    void(*AD736x_Start_Sample2)(struct ad736x_class* dev, uint32_t fs, uint16_t num, void* datbuf, data_format_type fmt);
} ad736x;

/***************************************************************************
*���ܣ�ad736x���캯��                                                      *
*������1.dev�����ʵ��                                                     *
*      2.dev_t����������                                                   *
*      3.ln_num������������                                                *
*      4.cnvst��CNVST����                                                  *
*      5.bsy��BUSY����                                                     *
*      6.cs��CS����                                                        *
*      7.sclk��SCLK����                                                    *
*      8.�ɱ�������ɱ�����������������������й�                          *
*        �������ߣ�ln_num==ONE_LINE����gpio_type douta ����DOUTA����       *
*        ˫�����ߣ�ln_num==TWO_LINE����gpio_type douta, gpio_type doutb    *
*        �ֱ����DOUTA���ź�DOUTB����                                      *
****************************************************************************/
void AD736x_Prepare(struct ad736x_class* dev, ad736x_device_type_type dev_t, ad736x_line_num_type ln_num,
					gpio_type cnvst, gpio_type bsy, gpio_type cs, gpio_type sclk, ...);


void AD736x_Prepare2(struct ad736x_class* dev, uint32_t value, void* reg0, void* reg1,
                     inst_type inst, gpio_type start, gpio_type rst);

#endif
