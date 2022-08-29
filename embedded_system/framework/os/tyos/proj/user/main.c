#include "sys.h"
#include "Memory.h"
#include "USART.h"
#include "GPIO.h"
#include "MathHelper.h"
#include "mpu.h"
#include "ArrayList.h"
#include "TYOS_Kernel.h"

#include <math.h>

process *p1, *p2, *p3, *p4;

void Task1(void);
void Task2(void);
void Task3(void);
void Task4(void);

int svc_service(int a);

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

	// Memory_Print_Heap_Info(&heap0);

	// LinkedList_Prepare(&ll, 5, Enable);

	// USART_Printf(USART1, "%#x\r\n", LinkedList_AddtoEnd(&ll));
	// USART_Printf(USART1, "%#x\r\n", LinkedList_AddtoEnd(&ll));
	// USART_Printf(USART1, "%#x\r\n", LinkedList_AddtoEnd(&ll));
	// USART_Printf(USART1, "%#x\r\n", LinkedList_AddtoEnd(&ll));
	// LinkeList_InfoPrint(USART1, &ll);

	// void *p = LinkedList_Remove(&ll, 1);
	// LinkedList_Add2(&ll, 2, p);
	// LinkeList_InfoPrint(USART1, &ll);
	// p = LinkedList_Remove(&ll, 1);
	// LinkedList_Add2(&ll, 0, p);
	// LinkeList_InfoPrint(USART1, &ll);

	// LinkedList_Dispose(&ll, 2);
	// LinkeList_InfoPrint(USART1, &ll);

	// Memory_Print_Heap_Info(&heap0);

	TYOS_Kernel_Prepare(10, &heap1);

	p1 = TYOS_Create_Process(255, Task1);
	p2 = TYOS_Create_Process(255, Task2);
	p3 = TYOS_Create_Process(1, Task3);
	p4 = TYOS_Create_Process(1, Task4);

	TYOS_Process_Start(p1);
	TYOS_Process_Start(p2);
	TYOS_Process_Start(p3);
	TYOS_Process_Start(p4);

	TYOS_Start();

	while (1)
		;

	return 0;
}

void Task1(void)
{
	while (1)
	{
		__disable_irq();
		USART_Printf(USART1, "T1:%ld\r\n",p1->Running_Time);
		__enable_irq();
		//TYOS_Process_Wait_Time(p1,2);
		// Set_PendSV_Interrupt();
	}
}

void Task2(void)
{
	while (1)
	{
		__disable_irq();
		USART_Printf(USART1, "T2:%ld\r\n",p2->Running_Time);
		__enable_irq();
		//TYOS_Process_Wait_Time(p2,3);
		// Set_PendSV_Interrupt();
	}
}

void Task3(void)
{
	while (1)
	{
		__disable_irq();
		USART_Printf(USART1, "T3:%ld\r\n",p3->Running_Time);
		__enable_irq();
		TYOS_Process_Wait_Time(p3,2);
		// Set_PendSV_Interrupt();
	}
}

void Task4(void)
{
	while (1)
	{
		__disable_irq();
		USART_Printf(USART1, "T4:%ld\r\n",p4->Running_Time);
		__enable_irq();
		TYOS_Process_Wait_Time(p4,3);
		// Set_PendSV_Interrupt();
	}
}
