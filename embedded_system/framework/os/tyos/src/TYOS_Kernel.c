#include "Framework.h"
#include "TYOS_Kernel.h"

#include "USART.h"

tyos my_os;

volatile float save = 1.5f;

process *sys_p;

process *_TYOS_Process_Switch(void);
void _TYOS_SVC_Handler_C(uint32_t *stk_tp);

#define SYSTIMER_1MS_VALUE ((uint32_t)SYSCLK_MAX_CLOCK / 1000)

#define GET_PRIORITY_FROM_ID(id) (((id) >> 8) & 0xff)
#define GET_INDEX_FROM_ID(id) ((id)&0xff)

#define PROCESS_INIT(p, pm, ph, pri, stk_size, hp_size, tsk, idx)                        \
	{                                                                                    \
		(p)->ID = ((uint32_t)((pri) << 8) + (idx));                                      \
		(p)->Initial_Pri = (pri);                                                        \
		(p)->Current_Pri = (pri);                                                        \
		(p)->Stack = Memory_Malloc(my_os.Process_Heap, stk_size);                        \
		(p)->StackTop = (uint32_t *)((uint32_t)(p)->Stack + (stk_size));                 \
		*(--(p)->StackTop) = 0x01000000;                                                 \
		*(--(p)->StackTop) = (uint32_t)(tsk) | 1;                                        \
		*(--(p)->StackTop) = 0;                                                          \
		*(--(p)->StackTop) = (uint32_t)(p)->Stack + (stk_size) + 4;                      \
		*(--(p)->StackTop) = 0;                                                          \
		*(--(p)->StackTop) = 0;                                                          \
		*(--(p)->StackTop) = 0;                                                          \
		*(--(p)->StackTop) = 0;                                                          \
		*(--(p)->StackTop) = 1;                                                          \
		*(--(p)->StackTop) = 2;                                                          \
		(p)->StackTop -= 6;                                                              \
		*(--(p)->StackTop) = 0xfffffffd;                                                 \
		(p)->Status = PROCESS_SLEEP;                                                     \
		(p)->Task = tsk;                                                                 \
		(p)->Manager = pm;                                                               \
		(p)->Handler = ph;                                                               \
		(p)->Start_Time = 0;                                                             \
		(p)->Running_Time = 0;                                                           \
		ArrayList_Prepare(&((p)->Assets), sizeof(process_asset), my_os.Process_Heap, 5); \
	}

#define FIND_PM_WITH_PRI(pri, pm)                                            \
	{                                                                        \
		(pm) = LinkedList_Get_FirstObject(&my_os.Process_Managers);          \
		while ((pm) != NULL)                                                 \
		{                                                                    \
			if ((pm)->Priority == (pri))                                     \
				break;                                                       \
			else if ((pm)->Priority > (pri))                                 \
			{                                                                \
				(pm) = NULL;                                                 \
				break;                                                       \
			}                                                                \
			(pm) = LinkedList_Get_NextObject(&my_os.Process_Managers, (pm)); \
		}                                                                    \
	}

#define FIND_NEXT_RDY_PROCESS_FROM_PM(pm, rec_p, nxt_p)                            \
	{                                                                              \
		(nxt_p) = LinkedList_Get_NextObject(&(pm)->Instance, rec_p);               \
		while ((rec_p) != (nxt_p))                                                 \
		{                                                                          \
			if ((nxt_p)->Status == PROCESS_READY)                                  \
				break;                                                             \
			(nxt_p) = LinkedList_Get_NextObject(&(pm)->Instance, nxt_p);           \
		}                                                                          \
		if ((nxt_p)->Status != PROCESS_READY && (nxt_p)->Status != PROCESS_ACTIVE) \
			nxt_p = NULL;                                                          \
	}

void System_Process(void)
{
	while (1)
	{
		__disable_irq();
		USART_Printf(USART1, "S:%ld\r\n", my_os.Active_Process->Running_Time);
		__enable_irq();
	}
}

process *TYOS_Create_Process(process_priority pri, task tsk)
{
	__disable_irq();
	//寻找对应优先级的pm
	process_manager *pm;
	FIND_PM_WITH_PRI(pri, pm);
	//如果没有找到，则需要创建pm
	if (pm == NULL)
	{
		//搜索优先级高于pri的最低优先级pm
		pm = LinkedList_Get_FirstObject(&my_os.Process_Managers);
		while (pm->Priority < pri)
			pm = LinkedList_Get_NextObject(&my_os.Process_Managers, pm);
		pm = LinkedList_Get_LastObject(&my_os.Process_Managers, pm);
		//在找到的pm后面加入新的pm
		//如果pm==NULL说明pri比现有的优先级都高
		if (pm != NULL)
			pm = LinkedList_AddBehind(&my_os.Process_Managers, pm);
		else
			pm = LinkedList_Add(&my_os.Process_Managers, 0);
		// pm初始化
		pm->Priority = pri;
		pm->Ready_Process = NULL;
		LinkedList_Prepare(&pm->Handler, sizeof(process_handler), Enable);
		LinkedList_Prepare(&pm->Instance, sizeof(process), Enable);
	}
	//找到合适的index用来构成ID
	int idx = 0;
	process_handler *ph = LinkedList_Get_FirstObject(&pm->Handler);
	while (idx == GET_INDEX_FROM_ID(ph->ID) && ph != NULL)
	{
		ph = LinkedList_Get_NextObject(&pm->Handler, ph);
		idx++;
	}
	//创建process_handler
	ph = LinkedList_AddBehind(&pm->Handler, ph);
	//创建process并初始化
	process *p = LinkedList_AddtoEnd(&pm->Instance);
	PROCESS_INIT(p, pm, ph, pri, TYOS_PRCS_STACK_SIZE, TYOS_PRCS_HEAP_SIZE, tsk, idx);
	//初始化process_handler
	ph->Address = p;
	ph->ID = p->ID;
	ph->Manager = pm;
	__enable_irq();

	return p;
}

void TYOS_Process_Start(process *p)
{
	__disable_irq();
	if (p->Status == PROCESS_SLEEP)
	{
		p->Status = PROCESS_READY;
		p->Start_Time = my_os.Tick;
		if (p->Manager->Ready_Process == NULL)
			p->Manager->Ready_Process = p;
	}
	__enable_irq();
}

void TYOS_Process_Wait_Time(process *p, uint32_t time)
{
	__disable_irq();
	int i;
	waiting_time_table *wtt = my_os.Waiting_Processes.Members;

	if (p->Status == PROCESS_AWAIT_TIME)
	{
		for (i = 0; i < my_os.Waiting_Processes.Member_Num; i++)
		{
			if (wtt[i].Process == p)
				ArrayList_Remove(&my_os.Waiting_Processes, i);
		}
	}
	else if (p->Status == PROCESS_ACTIVE)
		p->Running_Time++;
	for (i = 0; i < my_os.Waiting_Processes.Member_Num; i++)
	{
		if (wtt[i].End_Tick > my_os.Tick + time)
			break;
	}
	wtt = ArrayList_Insert(&my_os.Waiting_Processes, i);
	wtt->Process = p;
	wtt->End_Tick = my_os.Tick + (uint64_t)time;
	p->Status = PROCESS_AWAIT_TIME;

	//如果当前PPL的就绪进程就是p
	//则需要重新寻找就绪进程
	if (p->Manager->Ready_Process == p)
	{
		process *next_rdy_p;
		FIND_NEXT_RDY_PROCESS_FROM_PM(p->Manager, p, next_rdy_p);
		p->Manager->Ready_Process = next_rdy_p;
	}
	__enable_irq();
	Set_PendSV_Interrupt();
}

void TYOS_Kernel_Prepare(uint16_t t_slice, heap *p_heap)
{
	FPU->FPCCR |= 1 << 31;	  //自动保存使能
	FPU->FPCCR &= ~(1 << 30); //禁止惰性压栈

	NVIC_SetPriorityGrouping(2); //中断优先级2组（两位抢占优先级，两位响应优先级）
	//使能系统异常，并设置优先级
	SCB->SHCSR |= 1 << 18; //使能使用错误中断
	SCB->SHCSR |= 1 << 17; //使能总线错误中断
	SCB->SHCSR |= 1 << 16; //使能存储器错误中断
	NVIC_SetPriority(MemoryManagement_IRQn, 0);
	NVIC_SetPriority(BusFault_IRQn, 0);
	NVIC_SetPriority(UsageFault_IRQn, 0);
	NVIC_SetPriority(SVCall_IRQn, 0);
	// NVIC_SetPriority(DebugMonitor_IRQn, 0);
	NVIC_SetPriority(PendSV_IRQn, 3);
	NVIC_SetPriority(SysTick_IRQn, 2);

	//////////////os基本参数配置/////////////////
	my_os.TimeSlice = t_slice;
	my_os.Tick = 0;
	my_os.Process_Heap = p_heap;
	my_os.Process_Number = 1;

	LinkedList_Prepare(&my_os.Process_Managers, sizeof(process_manager), Disable);
	ArrayList_Prepare(&my_os.Mutexes, sizeof(mutex), my_os.Process_Heap, 5);
	ArrayList_Prepare(&my_os.Waiting_Processes, sizeof(waiting_time_table), my_os.Process_Heap, 10);

	///////////////创建系统进程//////////////////
	//添加第一个PM，优先级255（最低）
	process_manager *pm = LinkedList_Add(&my_os.Process_Managers, 0);
	pm->Priority = 255;
	LinkedList_Prepare(&pm->Handler, sizeof(process_handler), Enable);
	LinkedList_Prepare(&pm->Instance, sizeof(process), Enable);
	pm->Ready_Process = NULL;

	//创建系统进程，也就是第一个执行的进程
	process_handler *ph = LinkedList_AddtoEnd(&pm->Handler);
	process *p = LinkedList_AddtoEnd(&pm->Instance);
	p->Current_Pri = 255;
	p->Initial_Pri = 255;
	p->ID = (255 << 8) + 0;
	p->Stack = Memory_Malloc(my_os.Process_Heap, TYOS_PRCS_STACK_SIZE);
	p->StackTop = (uint32_t *)((uint32_t)p->Stack + TYOS_PRCS_STACK_SIZE);
	p->Status = PROCESS_ACTIVE;
	p->Task = System_Process;
	p->Manager = pm;
	p->Handler = ph;
	p->Start_Time = 0;
	p->Running_Time = 0;
	//初始化process_handler
	ph->Address = p;
	ph->ID = p->ID;
	ph->Manager = pm;
	//将当前活跃进程设定为系统进程
	my_os.Active_Process = p;

	SysTick->CTRL |= 1 << 2;					  //使用内核时钟
	SysTick->CTRL |= 1 << 1;					  //产生异常请求
	SysTick->LOAD = t_slice * SYSTIMER_1MS_VALUE; //载入值
												  // SysTick->CTRL |= 1; //开始计时

	sys_p = p;
}

process *_TYOS_Process_Switch(void)
{
	//寻找第一个有就绪进程的pm
	process_manager *pm = LinkedList_Get_FirstObject(&my_os.Process_Managers);
	while (pm->Ready_Process == NULL)
		pm = LinkedList_Get_NextObject(&my_os.Process_Managers, pm);
	//进行进程切换
	process *next_p = pm->Ready_Process; //下一个将要执行的进程
	next_p->Status = PROCESS_ACTIVE;
	//如果下一个将要执行的线程不是当前的活跃进程
	if (my_os.Active_Process != next_p)
	{
		if (my_os.Active_Process->Status == PROCESS_ACTIVE)
			my_os.Active_Process->Status = PROCESS_READY;
		my_os.Active_Process = next_p;
	}
	//寻找下一个要就绪的进程
	process *next_rdy_p;
	FIND_NEXT_RDY_PROCESS_FROM_PM(pm, next_p, next_rdy_p);
	pm->Ready_Process = next_rdy_p;

	return next_p;
}

void SysTick_Handler(void)
{
	__disable_irq();
	//更新时间记录
	my_os.Tick++;
	my_os.Active_Process->Running_Time++;

	int i;
	waiting_time_table *wtt = my_os.Waiting_Processes.Members;
	//寻找最后一个到时间的wtt
	for (i = 0; i < my_os.Waiting_Processes.Member_Num; i++)
	{
		if (wtt[i].End_Tick > my_os.Tick)
			break;
	}

	//这里更新每一个到时间的wwt，对每个wwt需要做以下几件事：
	// 1.更新进程的状态为Ready
	// 2.更新进程对应PPL的就绪进程（如果为空，就将此进程设置为就绪进程；否则不进行任何操作）
	// 3.删除此wwt
	for (int j = 0; j < i; j++)
	{
		wtt->Process->Status = PROCESS_READY;
		if (wtt->Process->Manager->Ready_Process == NULL)
			wtt->Process->Manager->Ready_Process = wtt->Process;
		ArrayList_Remove(&my_os.Waiting_Processes, 0);
	}
	USART_Send_Data_Flow(USART1, "tick\r\n", 6);
	__enable_irq();
	Set_PendSV_Interrupt();
}

void _TYOS_SVC_Handler_C(uint32_t *stk_tp)
{
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
