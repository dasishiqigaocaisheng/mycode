#ifndef _SOFTIIC_H_
#define _SOFTIIC_H_

#include "sys.h"
#include "GPIO.h"

typedef struct softiic_class softiic;

struct softiic_class
{
	vro gpio SDA;
	vro gpio SCL;

	uint8_t Slave_Address;

	volatile float ValidTime;	//一个时钟周期内，有效电平时间（低电平）
	volatile float InvalidTime; //一个时钟周期内，空闲电平时间（高电平）

	int (*Send_Data)(softiic *inst, void *txbuf, int len, status_flag send_slavaddr, status_flag gen_start, status_flag gen_stop);
	int (*Receive_Data)(softiic *inst, void *rxbuf, int len, status_flag send_slavaddr, status_flag gen_start, status_flag gen_stop);
};

void SoftIIC_Prepare(softiic *inst, gpio sda, gpio scl);

#endif
