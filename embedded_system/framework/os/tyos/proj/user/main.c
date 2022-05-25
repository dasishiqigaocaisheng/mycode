#include "sys.h"
#include "Memory.h"
#include "USART.h"
#include "GPIO.h"
#include "MathHelper.h"
#include "mpu.h"
#include "ArrayList.h"
#include "TYOS_Kernel.h"

#include <math.h>

int main(void)
{
	Stm32_Clock_Fast_Init();
	GPIO_Clock_Enable();
	SCB_EnableICache();
	SCB_DisableDCache();
	//SystemTimer_Init();
	USART_Init(USART1, 115200);
	USART_Printf(USART1, "Ready\r\n");
	MPU_Memory_Protection();
	Memory_Init();
	Memory_Print_Heap_Info(&heap5);
	
//	Memory_Malloc(&heap0,256);
//	Memory_Malloc(&heap0,257);
//	Memory_Malloc(&heap0,513);
//	
//	Memory_Malloc(&heap1,256);
//	Memory_Malloc(&heap1,257);
//	Memory_Malloc(&heap1,513);
	
	Memory_Print_Heap_Info(&heap0);
	Memory_Print_Heap_Info(&heap1);
	//while (1);
	
	//USART_Printf(USART1, "%.3f\r\n",0.333f);
	//USART_Printf(USART1, "%.3f\r\n",0.443f);
	
	TYOS_Kernel_Prepare(10, &heap1);

	
	while (1)
		Set_PendSV_Interrupt();

	return 0;
}
