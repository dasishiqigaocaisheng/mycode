#include "Framework.h"
#include "TYOS_Kernel.h"

#include "USART.h"

tyos my_os;
process *recent_prs;

volatile float save = 1.5f;

process process1, process2;

extern void TYOS_SaveEnvironment(uint32_t **stack_top_r);
extern void TYOS_RebuildEnvironment(uint32_t *stack_top);
extern void TYOS_Start(task tsk, uint32_t *stack_top);

#define SYSTIMER_1MS_VALUE ((uint32_t)SYSCLK_MAX_CLOCK / 1000)

#define PROCESS_INIT(p, pri, number, stk_size, hp_size, tsk)                 \
	{                                                                        \
		(p)->ID = ((uint32_t)((pri) << 8) + (number));                       \
		(p)->Initial_Pri = (pri);                                            \
		(p)->Current_Pri = (pri);                                            \
		(p)->Stack = Memory_Malloc(my_os.Process_Heap, stk_size);            \
		(p)->StackTop = (uint32_t *)((uint32_t)(p)->Stack + (stk_size) + 4); \
		*(--(p)->StackTop) = 0x01000000;                                     \
		*(--(p)->StackTop) = (uint32_t)(tsk) | 1;                            \
		*(--(p)->StackTop) = 0;                                              \
		*(--(p)->StackTop) = (uint32_t)(p)->Stack + (stk_size) + 4;          \
		*(--(p)->StackTop) = 0;                                              \
		*(--(p)->StackTop) = 0;                                              \
		*(--(p)->StackTop) = 0;                                              \
		*(--(p)->StackTop) = 0;                                              \
		(p)->StackTop -= 8;                                                  \
		*(--(p)->StackTop) = 0xfffffff9;                                     \
		(p)->Status = PROCESS_SLEEP;                                         \
		(p)->Task = tsk;                                                     \
	}

float func1(float x, float y)
{
	return x * y;
}

void Task1(void)
{
	while (1)
	{
		USART_Printf(USART1, "task1\r\n");
		Set_PendSV_Interrupt();
		while ((SCB->ICSR >> 28) & 1)
			;
	}
}

void Task2(void)
{
	while (1)
	{
		volatile float temp = 1.2f;
		save *= temp;
		temp = func1(temp, save);
		USART_Printf(USART1, "task2\r\n");
		Set_PendSV_Interrupt();
		while ((SCB->ICSR >> 28) & 1)
			save *= temp;
	}
}

void System_Process(void)
{
	USART_Printf(USART1, "System running\r\n");
}

process_id TYOS_Create_Process(process_priority pri, task tsk)
{
	__disable_irq();
	uint8_t idx = 0;
	process_list_base *plb = LinkedList_Get_FirstNode(&my_os.Process_List);
	while (plb != NULL)
	{
		if (plb->Priority == pri)
		{
		CREATE_PROCESS:
			process *p = ArrayList_AddtoEnd(&plb->Processes);
			PROCESS_INIT(p, pri, my_os.Process_Number, TYOS_PRCS_STACK_SIZE, TYOS_PRCS_HEAP_SIZE, tsk);
			my_os.Process_Number++;
			__enable_irq();
			return p->ID;
		}
		else
		{
			if (plb->Priority > pri)
				break;
		}
		plb = LinkedList_Get_Next(&my_os.Process_List, plb);
		idx++;
	}
	plb = LinkedList_Add(&my_os.Process_List, idx);
	plb->Priority = pri;
	plb->Process_Ready_Index = -1;
	ArrayList_Prepare(&plb->Processes, sizeof(process), my_os.Process_Heap, 5);
	goto CREATE_PROCESS;
}

void TYOS_Kernel_Prepare(uint16_t t_slice, heap *p_heap)
{
	SCB->SHCSR |= 1 << 18; //用法fault中断使能
	SCB->SHCSR |= 1 << 17; //总线fault中断使能
	SCB->SHCSR |= 1 << 16; //存储器管理fault使能

	FPU->FPCCR |= 1 << 31;	  //自动保存使能
	FPU->FPCCR &= ~(1 << 30); //禁止惰性压栈

	my_os.TimeSlice = t_slice;
	my_os.Tick = 0;
	my_os.Process_Heap = p_heap;

	LinkedList_Prepare(&my_os.Process_List, sizeof(process_list_base));
	ArrayList_Prepare(&my_os.Mutexes, sizeof(metux), my_os.Process_Heap, 5);

	process_list_base *plb = LinkedList_Add(&my_os.Process_List, 0);
	plb->Priority = 255;
	plb->Process_Ready_Index = 0;

	process *p = ArrayList_AddtoEnd(&plb->Processes);
	p->Current_Pri = 255;
	p->Initial_Pri = 255;
	p->ID = 255 << 8 + 0;

	process1.ID = 0;
	process1.Stack = Memory_Malloc(&heap1, 1024);
	process1.Task = Task1;
	process1.StackTop = (uint32_t *)((uint32_t)process1.Stack + 1024 + 4);
	//	*(--process1.StackTop)=0;							//xPSR入栈
	//	*(--process1.StackTop)=(uint32_t)Task1|1;			//PC入栈
	//	*(--process1.StackTop)=0xfffffff9;					//LR入栈
	//	*(--process1.StackTop)=(uint32_t)process1.Stack+512;//R12/SP入栈
	//	*(--process1.StackTop)=0;							//R3入栈
	//	*(--process1.StackTop)=0;							//R2入栈
	//	*(--process1.StackTop)=0;							//R1入栈
	//	*(--process1.StackTop)=0;							//R0入栈
	// process1.StackTop-=8;								//R4~R11入栈

	process2.ID = 1;
	process2.Stack = Memory_Malloc(&heap1, 1024);
	process2.Task = Task2;
	process2.StackTop = (uint32_t *)((uint32_t)process2.Stack + 1024 + 4);
	*(--process2.StackTop) = 0x01000000;					  // xPSR入栈
	*(--process2.StackTop) = (uint32_t)Task2 | 1;			  // PC入栈
	*(--process2.StackTop) = 0;								  // LR入栈
	*(--process2.StackTop) = (uint32_t)process2.Stack + 1024; // R12/SP入栈
	*(--process2.StackTop) = 0;								  // R3入栈
	*(--process2.StackTop) = 0;								  // R2入栈
	*(--process2.StackTop) = 0;								  // R1入栈
	*(--process2.StackTop) = 0;								  // R0入栈
	process2.StackTop -= 8;									  // R4~R11入栈
	*(--process2.StackTop) = 0xfffffff9;					  // LR入栈

	recent_prs = &process1;

	MY_NVIC_PriorityGroupConfig(2); //中断优先级2组（两位抢占优先级，两位响应优先级）

	SysTick->CTRL |= 1 << 2;					  //使用内核时钟
	SysTick->CTRL |= 1 << 1;					  //产生异常请求
	SysTick->LOAD = t_slice * SYSTIMER_1MS_VALUE; //载入值
	// SysTick->CTRL |= 1; //开始计时

	TYOS_Start(Task1, process1.StackTop);
}

void SysTick_Handler(void)
{
	USART_Printf(USART1, "Systick int\r\n");
	Set_PendSV_Interrupt();
}

// PendSV中断，执行任务切换
// void PendSV_Handler(void)
//{
//    INT_DISABLE();
//	TYOS_SaveEnvironment(&recent_prs->StackTop);
//	recent_prs->StackTop-=8;
//	if (recent_prs==&process1)
//	{
//		TYOS_RebuildEnvironment(process2.StackTop);
//		recent_prs=&process2;
//	}
//	else
//	{
//		TYOS_RebuildEnvironment(process1.StackTop);
//		recent_prs=&process1;
//	}
//    //USART_Printf(USART1, "in pendsv %d\r\n", sizeof(metux));
//    INT_ENABLE();
//}
