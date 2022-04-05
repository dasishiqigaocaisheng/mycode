#ifndef _FT5206_H_
#define _FT5206_H_

#include "Framework.h"
#include "SoftIIC.h"

/*u8 FT5206_WR_Reg(u16 reg, u8 *buf, u8 len);
void FT5206_RD_Reg(u16 reg, u8 *buf, u8 len);
u8 FT5206_Init(void);
u8 FT5206_Get_Points(u16 *buf, u8 max);*/

typedef struct ft5206_class ft5206;

struct ft5206_class
{
    vro gpio RST;
    softiic *IIC_Controller;

    void (*Write_Registers)(ft5206 *inst, uint8_t reg_addr, uint8_t *buf, uint8_t len);
    void (*Read_Registers)(ft5206 *inst, uint8_t reg_addr, uint8_t *buf, uint8_t len);
    uint8_t (*Get_Coordinates)(ft5206 *inst, uint8_t max_num, uint16_t *xy);
};

void FT5206_Prepare(ft5206 *inst, gpio rst, softiic *iic);

#endif
