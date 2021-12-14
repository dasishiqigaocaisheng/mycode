#ifndef _AD7768_H_
#define _AD7768_H_
#include "Framework.h"



typedef struct ad7768_class
{
	/*==================================成员变量=======================================*/
    ro          gpio_type                   RESET;

//	/*===================================方法=======================================*/

	void 	(*Get_Data)						(struct ad7768_class* device,uint8_t* dat1,uint32_t num);
	void    (*Reset)						(struct ad7768_class* device);
	void    (*Data_Binary_to_Float)			(uint32_t* dat_before,float* dat_after,uint16_t num,uint16_t channel);

}ad7768;

void AD7768_Prepare(ad7768* dev,gpio_type reset);

#endif
