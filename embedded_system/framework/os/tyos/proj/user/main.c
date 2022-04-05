#include "sys.h"
#include "Memory.h"
#include "USART.h"
#include "GPIO.h"
#include "MathHelper.h"
#include "mpu.h"
#include <math.h>


int main(void)
{
	Stm32_Clock_Fast_Init();
	GPIO_Clock_Enable();
	SCB_EnableICache();
	SCB_DisableDCache();
	SystemTimer_Init();
	USART_Init(USART1, 115200);
	MPU_Memory_Protection();
	Memory_Init();
	
	
	
	while (1);
	
	return 0;
}
