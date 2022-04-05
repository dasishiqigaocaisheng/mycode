#include "SoftIIC.h"

typedef enum
{
	M2L = 0,
	L2M = 1
} softiic_signal_dir;

typedef enum
{
	ACK = 0,
	NACK,
} softiic_ack;

void SoftIIC_Generate_StartSignal(gpio sda, gpio scl, float valid_t)
{
	//GPIO_Mode_Set(sda, GPIO_MODE_OUT);

	GPIO_Write(scl, 1);
	SystemTimer_Delay_Us(valid_t);
	GPIO_Write(sda, 0);
	SystemTimer_Delay_Us(valid_t);
	//GPIO_Write(scl, 0);
}

void SoftIIC_Send_SlaveAddress(gpio sda, gpio scl, float valid_t, float invalid_t, uint8_t slv_addr, softiic_signal_dir dir)
{
	//GPIO_Mode_Set(sda, GPIO_MODE_OUT);

	slv_addr |= dir;
	for (int i = 0; i < 8; i++)
	{
		GPIO_Write(scl, 0);
		GPIO_Write(sda, (slv_addr & 0x80) >> 7);
		slv_addr <<= 1;
		SystemTimer_Delay_Us(valid_t);
		GPIO_Write(scl, 1);
		SystemTimer_Delay_Us(invalid_t);

		/*GPIO_Write(sda, (slv_addr & 0x80) >> 7);
		slv_addr <<= 1;
		GPIO_Write(scl, 1);
		SystemTimer_Delay_Us(invalid_t);
		GPIO_Write(scl, 0);
		SystemTimer_Delay_Us(valid_t);*/
	}
}

softiic_ack SoftIIC_Wait_Ack(gpio sda, gpio scl, float valid_t, float invalid_t)
{
	softiic_ack ack;

	//GPIO_Mode_Set(sda, GPIO_MODE_IN);

	GPIO_Write(scl, 0);
	GPIO_Write(sda, 1); //释放SDA
	SystemTimer_Delay_Us(valid_t);
	GPIO_Write(scl, 1);
	ack = GPIO_Read(sda);
	SystemTimer_Delay_Us(invalid_t);
	GPIO_Write(scl, 0);
	SystemTimer_Delay_Us(valid_t);
	return ack;
}

void SoftIIC_Send_Byte(gpio sda, gpio scl, float valid_t, float invalid_t, uint8_t dat)
{
	//GPIO_Mode_Set(sda, GPIO_MODE_OUT);

	for (int i = 0; i < 8; i++)
	{
		GPIO_Write(scl, 0);
		GPIO_Write(sda, (dat & 0x80) >> 7);
		dat <<= 1;
		SystemTimer_Delay_Us(valid_t);
		GPIO_Write(scl, 1);
		SystemTimer_Delay_Us(invalid_t);

		/*GPIO_Write(sda, (dat & 0x80) >> 7);
		dat <<= 1;
		GPIO_Write(scl, 1);
		SystemTimer_Delay_Us(invalid_t);
		GPIO_Write(scl, 0);
		SystemTimer_Delay_Us(valid_t);*/
	}
}

void SoftIIC_Receive_Byte(gpio sda, gpio scl, float valid_t, float invalid_t, uint8_t *dat)
{
	//GPIO_Mode_Set(sda, GPIO_MODE_IN);
	//GPIO_Write(sda, 1);
	for (int i = 0; i < 8; i++)
	{
		GPIO_Write(scl, 0);
		SystemTimer_Delay_Us(valid_t);
		GPIO_Write(scl, 1);
		(*dat) <<= 1;
		*dat |= GPIO_Read(sda);
		SystemTimer_Delay_Us(invalid_t);

		/*GPIO_Write(scl, 1);
		(*dat) <<= 1;
		*dat |= GPIO_Read(sda);
		SystemTimer_Delay_Us(invalid_t);
		GPIO_Write(scl, 0);
		SystemTimer_Delay_Us(valid_t);*/
	}
}

void SoftIIC_Generate_Ack_NAck(gpio sda, gpio scl, float valid_t, float invalid_t, softiic_ack ack)
{
	//GPIO_Mode_Set(sda, GPIO_MODE_OUT);

	GPIO_Write(scl, 0);
	GPIO_Write(sda, ack);
	SystemTimer_Delay_Us(valid_t);
	GPIO_Write(scl, 1);
	SystemTimer_Delay_Us(invalid_t);

	/*GPIO_Write(sda, ack);
	GPIO_Write(scl, 1);
	SystemTimer_Delay_Us(invalid_t);
	GPIO_Write(scl, 0);
	SystemTimer_Delay_Us(valid_t);*/
}

void SoftIIC_Generate_StopSignal(gpio sda, gpio scl, float valid_t, float invalid_t)
{
	//GPIO_Mode_Set(sda, GPIO_MODE_OUT);

	/*GPIO_Write(scl, 0);
	SystemTimer_Delay_Us(1);
	GPIO_Write(sda, 0);
	SystemTimer_Delay_Us(1);*/

	GPIO_Write(scl, 1);
	SystemTimer_Delay_Us(invalid_t);
	GPIO_Write(sda, 1);
}

int Method_Send_Data(softiic *inst, void *txbuf, int len, status_flag send_slavaddr, status_flag gen_start, status_flag gen_stop);
int Method_Receive_Data(softiic *inst, void *rxbuf, int len, status_flag send_slavaddr, status_flag gen_start, status_flag gen_stop);

int Method_Send_Data(softiic *inst, void *txbuf, int len, status_flag send_slavaddr, status_flag gen_start, status_flag gen_stop)
{
	uint8_t *buf = (uint8_t *)txbuf;

	if (gen_start)
		SoftIIC_Generate_StartSignal(inst->SDA, inst->SCL, inst->ValidTime);

	if (send_slavaddr)
	{
		SoftIIC_Send_SlaveAddress(inst->SDA, inst->SCL, inst->ValidTime, inst->InvalidTime, inst->Slave_Address, M2L);
		SoftIIC_Wait_Ack(inst->SDA, inst->SCL, inst->ValidTime, inst->InvalidTime);
	}
	for (int i = 0; i < len; i++)
	{
		SoftIIC_Send_Byte(inst->SDA, inst->SCL, inst->ValidTime, inst->InvalidTime, buf[i]);
		SoftIIC_Wait_Ack(inst->SDA, inst->SCL, inst->ValidTime, inst->InvalidTime);
	}

	if (gen_stop)
		SoftIIC_Generate_StopSignal(inst->SDA, inst->SCL, inst->ValidTime, inst->InvalidTime);

	return 0;
}

int Method_Receive_Data(softiic *inst, void *rxbuf, int len, status_flag send_slavaddr, status_flag gen_start, status_flag gen_stop)
{
	uint8_t *buf = (uint8_t *)rxbuf;

	if (gen_start)
		SoftIIC_Generate_StartSignal(inst->SDA, inst->SCL, inst->ValidTime);

	if (send_slavaddr)
	{
		SoftIIC_Send_SlaveAddress(inst->SDA, inst->SCL, inst->ValidTime, inst->InvalidTime, inst->Slave_Address, L2M);
		SoftIIC_Wait_Ack(inst->SDA, inst->SCL, inst->ValidTime, inst->InvalidTime);
	}
	for (int i = 0; i < len; i++)
	{
		SoftIIC_Receive_Byte(inst->SDA, inst->SCL, inst->ValidTime, inst->InvalidTime, &buf[i]);
		SoftIIC_Wait_Ack(inst->SDA, inst->SCL, inst->ValidTime, inst->InvalidTime);
	}

	if (gen_stop)
		SoftIIC_Generate_StopSignal(inst->SDA, inst->SCL, inst->ValidTime, inst->InvalidTime);

	return 0;
}

void SoftIIC_Prepare(softiic *inst, gpio sda, gpio scl)
{
	_RO_WRITE(inst->SDA, gpio, sda);
	_RO_WRITE(inst->SCL, gpio, scl);

	inst->Send_Data = Method_Send_Data;
	inst->Receive_Data = Method_Receive_Data;

	GPIO_Write(inst->SDA, 1);
	GPIO_Write(inst->SCL, 1);
}
