#include "sys.h"
#include "Memory.h"
#include "USART.h"
#include "GPIO.h"
#include "MathHelper.h"
#include "mpu.h"
#include "ArrayList.h"
#include "TYOS_Kernel.h"

#include <math.h>

linkedlist ll;

int main(void)
{
	Stm32_Clock_Fast_Init();
	GPIO_Clock_Enable();
	SCB_EnableICache();
	SCB_DisableDCache();
	// SystemTimer_Init();
	USART_Init(USART1, 115200);
	USART_Printf(USART1, "Ready\r\n");
	MPU_Memory_Protection();
	Memory_Init();

	Memory_Print_Heap_Info(&heap0);

	LinkedList_Prepare(&ll, 5, Enable);

	USART_Printf(USART1, "%#x\r\n", LinkedList_AddtoEnd(&ll));
	USART_Printf(USART1, "%#x\r\n", LinkedList_AddtoEnd(&ll));
	USART_Printf(USART1, "%#x\r\n", LinkedList_AddtoEnd(&ll));
	USART_Printf(USART1, "%#x\r\n", LinkedList_AddtoEnd(&ll));
	LinkeList_InfoPrint(USART1, &ll);

	void *p = LinkedList_Remove(&ll, 1);
	LinkedList_Add2(&ll, 2, p);
	LinkeList_InfoPrint(USART1, &ll);
	p = LinkedList_Remove(&ll, 1);
	LinkedList_Add2(&ll, 0, p);
	LinkeList_InfoPrint(USART1, &ll);

	LinkedList_Dispose(&ll, 2);
	LinkeList_InfoPrint(USART1, &ll);

	Memory_Print_Heap_Info(&heap0);

	while (1)
		;

	// USART_Printf(USART1, "%.3f\r\n",0.333f);
	// USART_Printf(USART1, "%.3f\r\n",0.443f);

	TYOS_Kernel_Prepare(10, &heap1);

	while (1)
		;

	return 0;
}
