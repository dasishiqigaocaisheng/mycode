#ifndef _AD736X_H_
#define _AD736X_H_

//代码框架
#include "Framework.h"

//是AD7366还是AD7367？
//两者的区别是12bit/14bit
typedef enum
{
	AD7366=12,
	AD7367=14
} ad736x_device_type_type;

//输出线数量
typedef enum
{
	ONE_LINE,     //单线
	TWO_LINE      //双线
} ad736x_line_num_type;


typedef struct ad736x_class
{
    //器件类型，也就是：AD7366或AD7367
	ro ad736x_device_type_type 	Device_Type;
    
    
    ro sign status_flag         Is_FPGA_Mode;  
    

    ro periperal                DMA;
		
    //双线或单线
	ro ad736x_line_num_type		Line_Num;
			
    //CNVST引脚
	ro gpio_type				CNVST_Pin;
	
    //BUSY引脚
	ro gpio_type				BUSY_Pin;
	
    //CS引脚
	ro gpio_type				CS_Pin;
	
    //SCLK引脚
	ro gpio_type				SCLK_Pin;
	
    //DOUTA引脚
	ro gpio_type				DOUTA_Pin;
	
    //DOUTB引脚
	ro gpio_type				DOUTB_Pin;
    
    ro gpio_type                START;
    
    ro gpio_type                RST;
    
    //启动一次转换
    void(*AD736x_Start_Sample)(struct ad736x_class* dev, uint32_t fs, uint16_t num, void* datbuf1, void* datbuf2, data_format_type fmt);
    
    //启动一次转换
    void(*AD736x_Start_Sample2)(struct ad736x_class* dev, uint32_t fs, uint16_t num, void* datbuf, data_format_type fmt);
} ad736x;

/***************************************************************************
*功能：ad736x构造函数                                                      *
*参数：1.dev：类的实例                                                     *
*      2.dev_t：器件类型                                                   *
*      3.ln_num：数据线数量                                                *
*      4.cnvst：CNVST引脚                                                  *
*      5.bsy：BUSY引脚                                                     *
*      6.cs：CS引脚                                                        *
*      7.sclk：SCLK引脚                                                    *
*      8.可变参数：可变参数的内容与数据线数量有关                          *
*        单数据线（ln_num==ONE_LINE）：gpio_type douta 代表DOUTA引脚       *
*        双数据线（ln_num==TWO_LINE）：gpio_type douta, gpio_type doutb    *
*        分别代表DOUTA引脚和DOUTB引脚                                      *
****************************************************************************/
void AD736x_Prepare(struct ad736x_class* dev, ad736x_device_type_type dev_t, ad736x_line_num_type ln_num,
					gpio_type cnvst, gpio_type bsy, gpio_type cs, gpio_type sclk, ...);


void AD736x_Prepare2(struct ad736x_class* dev, uint32_t value, void* reg0, void* reg1,
                     inst_type inst, gpio_type start, gpio_type rst);

#endif
