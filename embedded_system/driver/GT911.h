#ifndef _GT911_H_
#define _GT911_H_

#include "Framework.h"
#include "GPIO.h"
#include "SoftIIC.h"

/*void GT911_Init(void);	
void GT911_Read(u16 Reg,u8 *buf,u8 len);	//读触摸屏寄存器的值
u8 GT911_Send_Cfg(u8 mode);					//向触摸屏发送配置参数
u8 GT911_Write(u16 Reg,u8 *buf,u16 len);		//向触摸屏写参数

u8 GT911_Get_Points_Number(void);
u8 GT911_Get_Coordinates(u16 *DATA);*/

typedef struct gt911_class gt911;

struct gt911_class
{
	vro gpio RST;
	vro gpio PEN;

	softiic *IIC_Controller;

	void (*Read_Registers)(gt911 *inst, uint16_t reg_addr, uint8_t *buf, uint16_t len);
	void (*Write_Registers)(gt911 *inst, uint16_t reg_addr, uint8_t *buf, uint16_t len);
	void (*Send_ConfigureParament)(gt911 *inst, status_flag w_flash);
	uint8_t (*Get_Coordinates)(gt911 *inst, uint8_t max_num, uint16_t *buf);
};

void GT911_Prepare(gt911 *inst, gpio rst, gpio pen, softiic *iic);

#endif
