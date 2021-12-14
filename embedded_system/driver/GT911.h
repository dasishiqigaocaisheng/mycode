#ifndef _GT911_H_
#define _GT911_H_

#include "sys.h"

void GT911_Init(void);	
void GT911_Read(u16 Reg,u8 *buf,u8 len);	//���������Ĵ�����ֵ
u8 GT911_Send_Cfg(u8 mode);					//�������������ò���
u8 GT911_Write(u16 Reg,u8 *buf,u16 len);		//������д����

u8 GT911_Get_Points_Number(void);
u8 GT911_Get_Coordinates(u16 *DATA);






#endif
