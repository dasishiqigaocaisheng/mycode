#include "sys.h"
#include "Memory.h"
#include "USART.h"
#include "GPIO.h"
#include "MathHelper.h"
#include "mpu.h"
#include "ArrayList.h"
#include "TYOS_Kernel.h"
#include "TYOS_MultProcess.h"
#include "TYOS_SysCall.h"

#include <math.h>

process *p1, *p2, *p3, *p4;
mutex_id mtx1, mtx2;

int w;

void Task1(void);
void Task2(void);
void Task3(void);
void Task4(void);

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

	TYOS_Kernel_Prepare(10, &heap1);

	p1 = TYOS_Create_Process(255, Task1);
	p2 = TYOS_Create_Process(255, Task2);
	p3 = TYOS_Create_Process(1, Task3);
	p4 = TYOS_Create_Process(1, Task4);

	mtx1 = TYOS_Create_Metux(p1, 0);
	mtx2 = TYOS_Create_Metux(p1, 0);
	USART_Printf(USART1, "MutexID:%d\r\n", mtx1);
	USART_Printf(USART1, "MutexID:%d\r\n", mtx2);

	sys_call(2, 3, 4, 5, 6, 7, 1.2f, 8);

	TYOS_Process_Start(p1);
	// TYOS_Process_Start(p2);
	TYOS_Process_Start(p3);
	// TYOS_Process_Start(p4);

	TYOS_Start();

	while (1)
		;

	return 0;
}

void Task1(void)
{
	TYOS_Mutex_Regist(p1, 2, mtx1, mtx2);
	TYOS_Mutex_Lock(p1);
	TYOS_Stop_Schedule();
	USART_Printf(USART1, "T1:locked\r\n", p1->Running_Time);
	USART_Printf(USART1, "T1:sleep\r\n");
	TYOS_Resume_Schedule();
	TYOS_Process_Wait_Time(p1, 20);
	TYOS_Stop_Schedule();
	USART_Printf(USART1, "T1:wake up\r\n");
	USART_Printf(USART1, "T1:recent priority:%d\r\n", p1->Current_Pri);
	TYOS_Resume_Schedule();
	TYOS_Mutex_Release(p1, 1, mtx1);
	TYOS_Process_Wait_Time(p1, 5);
	TYOS_Mutex_Release(p1, 1, mtx2);
	TYOS_Stop_Schedule();
	USART_Printf(USART1, "T1:recent priority:%d\r\n", p1->Current_Pri);
	TYOS_Resume_Schedule();

	while (1)
	{
		TYOS_Stop_Schedule();
		USART_Printf(USART1, "T1:%ld\r\n", p1->Running_Time);
		TYOS_Resume_Schedule();
		TYOS_Schedule_Immediately();
	}
}

void Task2(void)
{
	while (1)
	{
		TYOS_Stop_Schedule();
		USART_Printf(USART1, "T2:%ld\r\n", p2->Running_Time);
		TYOS_Resume_Schedule();
		TYOS_Schedule_Immediately();
	}
}

void Task3(void)
{
	USART_Printf(USART1, "T3:sleep\r\n");
	TYOS_Process_Wait_Time(p3, 10);
	USART_Printf(USART1, "T3:wake up\r\n");
	TYOS_Mutex_Regist(p3, 2, mtx1, mtx2);
	while (!TYOS_Mutex_Lock_Try(p3))
	{
		TYOS_Stop_Schedule();
		USART_Printf(USART1, "T3:lock failed\r\n");
		TYOS_Resume_Schedule();
		TYOS_Process_Wait_Time(p3, 1);
	}
	TYOS_Stop_Schedule();
	USART_Printf(USART1, "T3:locked\r\n");
	TYOS_Resume_Schedule();

	while (1)
	{
		TYOS_Stop_Schedule();
		USART_Printf(USART1, "T3:%ld\r\n", p3->Running_Time);
		TYOS_Resume_Schedule();
		TYOS_Process_Wait_Time(p3, 1);
	}
}

void Task4(void)
{
	TYOS_Process_Wait_Time(p4, 10);

	while (1)
	{
		TYOS_Stop_Schedule();
		USART_Printf(USART1, "T4:%ld\r\n", p4->Running_Time);
		TYOS_Resume_Schedule();
		TYOS_Schedule_Immediately();
	}
}
