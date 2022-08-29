#include "TYOS_DataStructure.h"
#include <stdarg.h>

#define ASSET_METUX 1

#define MUTEX_PREPARE(mtx, id, owner, object)                                 \
	{                                                                         \
		(mtx)->ID = id;                                                       \
		(mtx)->Locked = Disable;                                              \
		(mtx)->Owner = owner;                                                 \
		(mtx)->User = NULL;                                                   \
		(mtx)->Reserve_Process = NULL;                                        \
		(mtx)->Object = (uint32_t)(object);                                   \
		ArrayList_Prepare(&(mtx)->Waiting_Process, 4, my_os.Process_Heap, 5); \
	}

#define SEMAPHORE_PREPARE(sem, id, owner, count)                              \
	{                                                                         \
		(sem)->ID = id;                                                       \
		(sem)->Owner = owner;                                                 \
		(sem)->Use_Table = 0;                                                 \
		(sem)->Count = count;                                                 \
		ArrayList_Prepare(&(sem)->Waiting_Process, 4, my_os.Process_Heap, 5); \
	}

#define FIND_MUTEX_WITH_ID(id, mtx)                                            \
	{                                                                          \
		mtx = my_os.Mutexes.Members;                                           \
		while ((mtx)->ID != (id) && !ArrayList_IsLastOne(&my_os.Mutexes, mtx)) \
			(mtx)++;                                                           \
	}

#define FIND_SEMAPHORE_WITH_ID(id, sem)                                           \
	{                                                                             \
		sem = (semaphore *)my_os.Semaphores.Members;                              \
		while ((sem)->ID != (id) && !ArrayList_IsLastOne(&my_os.Semaphores, sem)) \
			(sem)++;                                                              \
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

#define EMIGRATE_PROCESS_FROM_PM(p)                                                                                                        \
	{                                                                                                                                      \
		LinkedList_Remove2(&(p)->Manager->Instance, p);                                                                                    \
		if ((p)->Manager->Ready_Process == (p))                                                                                            \
			FIND_NEXT_RDY_PROCESS_FROM_PM((p)->Manager, LinkedList_Get_FirstObject(&(p)->Manager->Instance), (p)->Manager->Ready_Process); \
	}

#define IMMIGRATE_PROCESS_IN_PM(pm, p)                                                                      \
	{                                                                                                       \
		LinkedList_AddtoEnd2(&(pm)->Instance, p);                                                           \
		if ((pm)->Ready_Process == NULL && ((p)->Status == PROCESS_ACTIVE || (p)->Status == PROCESS_READY)) \
			(pm)->Ready_Process = p;                                                                        \
	}

#define Set_PendSV_Interrupt() \
	SCB->ICSR |= 1 << 28;      \
	__DSB();                   \
	__ISB()

extern tyos my_os;

mutex_id TYOS_Create_Metux(process *p, uint32_t object)
{
	__disable_irq();
	uint32_t id = 0, index = 0;
	mutex *mtx = my_os.Mutexes.Members; //获取mutex列表
	//寻找合适的mutex_id
	while (mtx[index].ID == id && index < my_os.Mutexes.Member_Num)
	{
		id++;
		index++;
	}
	mutex *new_mtx = ArrayList_Insert(&my_os.Mutexes, index);
	// mutex初始化
	MUTEX_PREPARE(new_mtx, id, p, object);
	//向进程中添加资产
	process_asset *pa = ArrayList_AddtoEnd(&p->Assets);
	// Asset初始化
	pa->Asset_Type = ASSET_METUX;
	pa->Asset.Mutex = new_mtx;
	__enable_irq();
}

void TYOS_Delete_Metux(mutex_id id)
{
	__disable_irq();
	//找到对应的mutex和process
	mutex *mtx;
	FIND_MUTEX_WITH_ID(id, mtx);
	process *owner = mtx->Owner;
	// mutex不能是锁住的状态
	if (mtx->Locked)
		return;
	//找到对应的process_asset
	process_asset *pa = owner->Assets.Members;
	while (!ArrayList_IsLastOne(&owner->Assets, pa))
	{
		if (pa->Asset_Type == ASSET_METUX)
		{
			if (pa->Asset.Mutex == mtx)
				break;
		}
		pa++;
	}
	//移除process_asset
	ArrayList_Remove(&owner->Assets, pa);
	//移除mutex
	ArrayList_Remove(&my_os.Mutexes, mtx);
	__enable_irq();
}

//如果成功，则正常退出函数
//如果失败，则进程放弃cpu，进入等待状态
void TYOS_Mutex_Lock(process *p, uint8_t num, ...)
{
	__disable_irq();
	/******************************构建mutex列表******************************/
	va_list vl;
	va_start(vl, num);
	ArrayList_Clear(&p->Request_Mutexes);
	mutex_id *mtx_id = ArrayList_Insert_Group(&p->Request_Mutexes, 0, num);
	for (int i = 0; i < num; i++)
		mtx_id[i] = va_arg(vl, mutex_id);
	va_end(vl);
	/********************************尝试预约********************************/
	mutex *mtx;
	p->Mutex_Count = num;
	for (int i = 0; i < num; i++)
	{
		FIND_MUTEX_WITH_ID(mtx_id[i], mtx);
		//情况一：没有预约，此时预约成功
		if (mtx->Reserve_Process == NULL)
		{
			mtx->Reserve_Process = p;
			p->Mutex_Count--;
		}
		else
		{
			//情况二：已经存在预约进程，但未锁定，优先级低的进程进入等待队列，不改变优先级
			if (!mtx->Locked)
			{
				//获取正在等待的进程队列
				process **p_waitqueue = mtx->Waiting_Process.Members;
				process *p_sel = p->Initial_Pri > mtx->Reserve_Process->Initial_Pri ? p : mtx->Reserve_Process;
				//将p加入等待队列
				for (int j = 0; j < mtx->Waiting_Process.Member_Num; j++)
				{
					//寻找第一个优先级低于p的进程，在前面插入p
					if (p_waitqueue[j]->Initial_Pri > p_sel->Initial_Pri)
					{
						ArrayList_Insert(&mtx->Waiting_Process, j);
						p_waitqueue[j] = p_sel;
						goto INSERT_FIN0;
					}
				}
				// p是最低优先级，添加到末尾
				*(process **)ArrayList_AddtoEnd(&mtx->Waiting_Process) = p_sel;
			INSERT_FIN0:
				//更新预约进程
				if (p_sel == mtx->Reserve_Process)
					mtx->Reserve_Process = p;
			}
			else
			{
				//情况三：已经存在预约进程，且已经锁定，申请进程进入等待队列，改变优先级
				//获取正在等待的进程队列
				process **p_waitqueue = mtx->Waiting_Process.Members;
				//将p加入等待队列
				for (int j = 0; j < mtx->Waiting_Process.Member_Num; j++)
				{
					//寻找第一个优先级低于p的进程，在前面插入p
					if (p_waitqueue[j]->Initial_Pri > p->Initial_Pri)
					{
						ArrayList_Insert(&mtx->Waiting_Process, j);
						p_waitqueue[j] = p;
						goto INSERT_FIN1;
					}
				}
				// p是最低优先级，添加到末尾
				*(process **)ArrayList_AddtoEnd(&mtx->Waiting_Process) = p;
			INSERT_FIN1:
				//调整预约进程的优先级
				if (mtx->Reserve_Process->Current_Pri > p->Initial_Pri)
				{
					process *p_res = mtx->Reserve_Process;
					//将p_res从原来的PM中迁出
					EMIGRATE_PROCESS_FROM_PM(p_res);
					//寻找新的PM
					process_manager *pm_new;
					FIND_PM_WITH_PRI(p->Initial_Pri, pm_new);
					//将p_res迁入新的PM
					IMMIGRATE_PROCESS_IN_PM(pm_new, p_res);
					//修改pp_res的信息
					p_res->Current_Pri = p->Initial_Pri;
					p_res->Manager = pm_new;
				}
			}
		}
	}
	/********************************检查是否预约成功********************************/
	if (p->Mutex_Count == 0)
	{
		for (int i = 0; i < num; i++)
		{
			FIND_MUTEX_WITH_ID(mtx_id[i], mtx);
			mtx->Locked = Enable;
			mtx->User = p;
		}
	}
	else
	{
		//预约失败，当前进程放弃CPU，重新进行调度
		p->Status = PROCESS_AWAIT_MUTEX;
		FIND_NEXT_RDY_PROCESS_FROM_PM(p->Manager, p, p->Manager->Ready_Process);
		Set_PendSV_Interrupt();
	}
	__enable_irq();
}

//如果成功，则正常退出函数
//如果失败，则进程放弃cpu，进入等待状态
void TYOS_Mutex_Lock_Direct(process *p, mutex_id id)
{
	/********************************尝试预约********************************/
	__disable_irq();
	mutex *mtx;
	FIND_MUTEX_WITH_ID(id, mtx);
	p->Mutex_Count = 1;
	if (!mtx->Locked)
	{
		//情况一：没有预约，此时预约成功
		if (mtx->Reserve_Process != NULL)
		{
			mtx->Reserve_Process = p;
			p->Mutex_Count--;
		}
		else
		{
			//情况二：已经存在预约进程，优先级低的进程进入等待队列，不改变优先级
			//获取正在等待的进程队列
			process **p_waitqueue = mtx->Waiting_Process.Members;
			process *p_sel = p->Initial_Pri > mtx->Reserve_Process->Initial_Pri ? p : mtx->Reserve_Process;
			//将p加入等待队列
			for (int j = 0; j < mtx->Waiting_Process.Member_Num; j++)
			{
				//寻找第一个优先级低于p的进程，在前面插入p
				if (p_waitqueue[j]->Initial_Pri > p_sel->Initial_Pri)
				{
					ArrayList_Insert(&mtx->Waiting_Process, j);
					p_waitqueue[j] = p_sel;
					goto INSERT_FIN2;
				}
			}
			// p是最低优先级，添加到末尾
			*(process **)ArrayList_AddtoEnd(&mtx->Waiting_Process) = p_sel;
		INSERT_FIN2:
			//更新预约进程
			if (p_sel == mtx->Reserve_Process)
				mtx->Reserve_Process = p;
		}
	}
	else
	{
		//情况三：已经存在预约进程，且已经锁定，申请进程进入等待队列，改变优先级
		//获取正在等待的进程队列
		process **p_waitqueue = mtx->Waiting_Process.Members;
		//将p加入等待队列
		for (int j = 0; j < mtx->Waiting_Process.Member_Num; j++)
		{
			//寻找第一个优先级低于p的进程，在前面插入p
			if (p_waitqueue[j]->Initial_Pri > p->Initial_Pri)
			{
				ArrayList_Insert(&mtx->Waiting_Process, j);
				p_waitqueue[j] = p;
				goto INSERT_FIN3;
			}
		}
		// p是最低优先级，添加到末尾
		*(process **)ArrayList_AddtoEnd(&mtx->Waiting_Process) = p;
	INSERT_FIN3:
		//调整预约进程的优先级
		if (mtx->Reserve_Process->Current_Pri > p->Initial_Pri)
		{
			process *p_res = mtx->Reserve_Process;
			//将p_res从原来的PM中迁出
			EMIGRATE_PROCESS_FROM_PM(p_res);
			//寻找新的PM
			process_manager *pm_new;
			FIND_PM_WITH_PRI(p->Initial_Pri, pm_new);
			//将p_res迁入新的PM
			IMMIGRATE_PROCESS_IN_PM(pm_new, p_res);
			//修改pp_res的信息
			p_res->Current_Pri = p->Initial_Pri;
			p_res->Manager = pm_new;
		}
	}
	/********************************检查是否预约成功********************************/
	if (p->Mutex_Count == 0)
	{
		mtx->Locked = Enable;
		mtx->User = p;
	}
	else
	{
		//预约失败，当前进程放弃CPU，重新进行调度
		p->Status = PROCESS_AWAIT_MUTEX;
		FIND_NEXT_RDY_PROCESS_FROM_PM(p->Manager, p, p->Manager->Ready_Process);
		Set_PendSV_Interrupt();
	}
	__enable_irq();
}

void TYOS_Mutex_Release(process *p, mutex_id id)
{
	__disable_irq();
	/**********************恢复优先级**********************/
	if (p->Current_Pri != p->Initial_Pri)
	{
		//将p从原来的PM中迁出
		EMIGRATE_PROCESS_FROM_PM(p);
		//寻找新的PM
		process_manager *pm_new;
		FIND_PM_WITH_PRI(p->Initial_Pri, pm_new);
		//将p_user迁入新的PM
		IMMIGRATE_PROCESS_IN_PM(pm_new, p);
		//修改p_user的信息
		p->Current_Pri = p->Initial_Pri;
		p->Manager = pm_new;
	}
	/**********************将mutex使用权交给下一个进程**********************/
	mutex *mtx;
	FIND_MUTEX_WITH_ID_0(&my_os.Mutexes, id, mtx);
	//如果有进程在等待使用权
	if (mtx->Waiting_Process.Member_Num != 0)
	{
		mtx->User = ((process **)mtx->Waiting_Process.Members)[0];
		ArrayList_Remove(&mtx->Waiting_Process, 0);
		//如果还有进程在等待使用权，那么可能需要更改占用进程的优先级
		if (mtx->Waiting_Process.Member_Num != 0)
		{
			process *p_nxt = ((process **)mtx->Waiting_Process.Members)[0];
			if (mtx->User->Initial_Pri > p_nxt->Initial_Pri)
			{
				//将p从原来的PM中迁出
				EMIGRATE_PROCESS_FROM_PM(mtx->User);
				//寻找新的PM
				process_manager *pm_new;
				FIND_PM_WITH_PRI(p_nxt->Initial_Pri, pm_new);
				//将p_user迁入新的PM
				IMMIGRATE_PROCESS_IN_PM(pm_new, mtx->User);
				//修改p_user的信息
				mtx->User->Current_Pri = p_nxt->Initial_Pri;
				mtx->User->Manager = pm_new;
			}
		}
		//更改占用进程的状态
		mtx->User->Status = PROCESS_READY;
		if (mtx->User->Manager->Ready_Process == NULL)
			mtx->User->Manager->Ready_Process = mtx->User;
	}
	else
	{
		mtx->User = NULL;
		mtx->Locked = Disable;
	}
	__enable_irq();
}
