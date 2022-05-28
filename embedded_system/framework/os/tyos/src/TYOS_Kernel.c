#include "Framework.h"
#include "TYOS_Kernel.h"

#include "USART.h"

tyos my_os;
process *recent_prs;

volatile float save = 1.5f;

process process1, process2;

#define SYSTIMER_1MS_VALUE ((uint32_t)SYSCLK_MAX_CLOCK / 1000)

#define GET_PRIORITY_FROM_ID(id) (((id) >> 8) & 0xff)
#define GET_INDEX_FROM_ID(id) ((id)&0xff)
#define GET_PROCESS_FROM_INDEX_WITH_PPL(ppl, idx) (&((ppl)->Processes.Members[idx]))
#define GET_PROCESS_FROM_ID_WITH_PPL(ppl, id) GET_PROCESS_FROM_INDEX_WITH_PPL(ppl, GET_INDEX_FROM_ID(id))

#define PROCESS_INIT(p, pri, stk_size, hp_size, tsk, idx)                \
	{                                                                    \
		(p)->ID = ((uint32_t)((pri) << 8) + (idx));                      \
		(p)->Initial_Pri = (pri);                                        \
		(p)->Current_Pri = (pri);                                        \
		(p)->Stack = Memory_Malloc(my_os.Process_Heap, stk_size);        \
		(p)->StackTop = (uint32_t *)((uint32_t)(p)->Stack + (stk_size)); \
		*(--(p)->StackTop) = 0x01000000;                                 \
		*(--(p)->StackTop) = (uint32_t)(tsk) | 1;                        \
		*(--(p)->StackTop) = 0;                                          \
		*(--(p)->StackTop) = (uint32_t)(p)->Stack + (stk_size) + 4;      \
		*(--(p)->StackTop) = 0;                                          \
		*(--(p)->StackTop) = 0;                                          \
		*(--(p)->StackTop) = 0;                                          \
		*(--(p)->StackTop) = 0;                                          \
		(p)->StackTop -= 8;                                              \
		*(--(p)->StackTop) = 0xfffffff9;                                 \
		(p)->Status = PROCESS_SLEEP;                                     \
		(p)->Task = tsk;                                                 \
		(p)->PLB = ppl;                                                  \
	}

#define FIND_PPL_WITH_PRI(pri, ppl)                                        \
	{                                                                      \
		(ppl) = LinkedList_Get_FirstObject(&my_os.Process_List);           \
		while ((ppl) != NULL)                                              \
		{                                                                  \
			if ((ppl)->Priority == (pri))                                  \
				break;                                                     \
			else if ((ppl)->Priority > (pri))                              \
			{                                                              \
				(ppl) = NULL;                                              \
				break;                                                     \
			}                                                              \
			(ppl) = LinkedList_Get_NextObject(&my_os.Process_List, (ppl)); \
		}                                                                  \
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
	//寻找对应优先级的ppl
	process_pri_list *ppl;
	FIND_PPL_WITH_PRI(pri, ppl);
	//如果没有找到，则需要创建
	if (ppl == NULL)
	{
		ppl = LinkedList_Get_FirstObject(&my_os.Process_List);
		while (ppl->Priority < pri)
			ppl = LinkedList_Get_NextObject(&my_os.Process_List, ppl);
		ppl = LinkedList_Get_LastObject(&my_os.Process_List, ppl);
	}
	//找到合适的index用来构成ID
	int idx = 0;
	process *p = LinkedList_Get_FirstObject(&ppl->Processes);
	while (idx == GET_INDEX_FROM_ID(p->ID))
		idx++;
	//创建process并初始化
	p = LinkedList_AddtoEnd(&ppl->Processes);
	PROCESS_INIT(p, pri, TYOS_PRCS_STACK_SIZE, TYOS_PRCS_HEAP_SIZE, tsk, idx);

	while (ppl != NULL)
	{
	CREATE_PROCESS:
		if (ppl->Priority == pri)
		{
			//找到合适的index用来构成ID
			int idx = 0;
			process *p = LinkedList_Get_FirstObject(&ppl->Processes);
			while (idx == GET_INDEX_FROM_ID(p->ID))
				idx++;
			p = LinkedList_AddtoEnd(&ppl->Processes);
			PROCESS_INIT(p, pri, TYOS_PRCS_STACK_SIZE, TYOS_PRCS_HEAP_SIZE, tsk, idx);
			my_os.Process_Number++;
			__enable_irq();

			return p->ID;
		}
		else
		{
			if (ppl->Priority > pri)
				break;
		}
		ppl = LinkedList_Get_NextObject(&my_os.Process_List, ppl);
		idx++;
	}
	ppl = LinkedList_Add(&my_os.Process_List, idx);
	ppl->Priority = pri;
	ppl->Process_Ready_Index = -1;
	LinkedList_Prepare(&ppl->Processes, sizeof(process), Enable);
	goto CREATE_PROCESS;
}

process *TYOS_Get_Process(process_id id)
{
	process_priority pri = TYOS_Get_Priority(id);
	process_pri_list *ppl = LinkedList_Get_FirstObject(&my_os.Process_List);
	while (ppl != NULL)
	{
		if (pri == ppl->Priority)
			break;
		ppl = LinkedList_Get_NextObject(&my_os.Process_List, ppl);
	}
	process *prcs = ppl->Processes.Nodes_Num;
	for (int i = 0; i < ppl->Processes.Nodes_Num; i++)
	{
		if (prcs[i].ID == id)
			return &prcs[i];
	}
	return NULL;
}

void TYOS_Kernel_Prepare(uint16_t t_slice, heap *p_heap)
{
	SCB->SHCSR |= 1 << 18; //用法fault中断使能
	SCB->SHCSR |= 1 << 17; //总线fault中断使能
	SCB->SHCSR |= 1 << 16; //存储器管理fault使能

	FPU->FPCCR |= 1 << 31;	  //自动保存使能
	FPU->FPCCR &= ~(1 << 30); //禁止惰性压栈

	MY_NVIC_PriorityGroupConfig(2); //中断优先级2组（两位抢占优先级，两位响应优先级）

	//////////////os基本参数配置/////////////////
	my_os.TimeSlice = t_slice;
	my_os.Tick = 0;
	my_os.Process_Heap = p_heap;
	my_os.Process_Number = 1;

	LinkedList_Prepare(&my_os.Process_List, sizeof(process_pri_list), Disable);
	ArrayList_Prepare(&my_os.Mutexes, sizeof(metux), my_os.Process_Heap, 5);

	//添加第一个process队列，优先级255（最低）
	process_pri_list *ppl = LinkedList_Add(&my_os.Process_List, 0);
	ppl->Priority = 255;
	ppl->Process_Ready_Index = 0;

	//初始化队列
	LinkedList_Prepare(&ppl->Processes, sizeof(process), Enable);
	//创建系统进程，也就是第一个执行的进程
	process *p = LinkedList_AddtoEnd(&ppl->Processes);
	p->Current_Pri = 255;
	p->Initial_Pri = 255;
	p->ID = (255 << 8) + 0;
	p->Stack = Memory_Malloc(my_os.Process_Heap, TYOS_PRCS_STACK_SIZE);
	p->StackTop = (uint32_t *)((uint32_t)p->Stack + TYOS_PRCS_STACK_SIZE);
	p->Status = PROCESS_ACTIVE;
	p->Task = System_Process;
	//将当前活跃进程设定为系统进程
	my_os.Active_Process = p->ID;

	SysTick->CTRL |= 1 << 2;					  //使用内核时钟
	SysTick->CTRL |= 1 << 1;					  //产生异常请求
	SysTick->LOAD = t_slice * SYSTIMER_1MS_VALUE; //载入值
												  // SysTick->CTRL |= 1; //开始计时
}

process *_TYOS_Process_Switch(void)
{
	//找到当前线程对应的优先级列表
	process_pri_list *ppl;
	FIND_PPL_WITH_PRI(ppl, GET_PRIORITY_FROM_ID(my_os.Active_Process));
	//找到当前线程
	process *p = GET_PROCESS_FROM_ID_WITH_PPL(ppl, my_os.Active_Process);
	//对每个优先级高于p的线程列表进行搜索
	ppl = LinkedList_Get_FirstObject(&my_os.Process_List);
	while (ppl != NULL && ppl->Priority < p->Current_Pri)
	{
		//说明该列表内有就绪线程
		if (ppl->Process_Ready_Index != -1)
		{
			process *next_p = &ppl->Processes.Members[ppl->Process_Ready_Index];
			p->Status = PROCESS_ACTIVE;
			my_os.Active_Process = p->ID;
			return p;
		}
	}
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
