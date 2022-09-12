#include "TYOS_DataStructure.h"
#include "TYOS_MultProcess.h"
#include "TYOS_SysCall.h"
#include <stdarg.h>

#define ASSET_METUX 1

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

#define MUTEX_RESERVE(mtx, p)         \
	{                                 \
		(mtx)->Reserve_Process = (p); \
		(p)->Mutex_Count--;           \
	}

#define MUTEX_RESERVE_CANCEL(mtx)              \
	{                                          \
		(mtx)->Reserve_Process->Mutex_Count--; \
		(mtx)->Reserve_Process = NULL;         \
	}

#define MUTEX_LOCK(mtx, p)      \
	{                           \
		(mtx)->Locked = Enable; \
		(mtx)->User = (p);      \
	}

#define MUTEX_UNLOCK(mtx)        \
	{                            \
		(mtx)->Locked = Disable; \
		(mtx)->User = NULL;      \
	}

#define Set_PendSV_Interrupt() \
	{                          \
		SCB->ICSR |= 1 << 28;  \
		__DSB();               \
		__ISB();               \
	}

extern tyos my_os;

mutex_id _TYOS_Create_Metux(process *p, uint32_t object)
{
	TYOS_Stop_Schedule();
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
	TYOS_Resume_Schedule();
	return (mutex_id)id;
}

void _TYOS_Delete_Metux(mutex_id id)
{
	TYOS_Stop_Schedule();
	//找到对应的mutex和process
	mutex *mtx;
	FIND_MUTEX_WITH_ID(id, mtx);
	process *owner = mtx->Owner;
	// mutex不能有任何预约进程
	if (mtx->Reserve_Process != NULL)
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
	ArrayList_Remove2(&owner->Assets, pa);
	//移除mutex
	ArrayList_Remove2(&my_os.Mutexes, mtx);
	TYOS_Resume_Schedule();
}

void _TYOS_Mutex_Regist(process *p, int num, ...)
{
	TYOS_Stop_Schedule();
	va_list vl;
	va_start(vl, num);
	mutex_id *mtx_ids = p->Request_Mutex_IDs.Members;
	for (int i = 0; i < num; i++)
	{
		mutex_id mtx_id = va_arg(vl, mutex_id);
		//需要检查每个id是否重复添加
		for (int j = 0; j < p->Request_Mutex_IDs.Member_Num; j++)
		{
			if (mtx_ids[j] == mtx_id)
				goto LOOP_END;
		}
		*(mutex_id *)ArrayList_AddtoEnd(&p->Request_Mutex_IDs) = mtx_id;
		p->Mutex_Count++;
	LOOP_END:
		continue;
	}
	va_end(vl);
	TYOS_Resume_Schedule();
}

void _TYOS_Mutex_Unregist(process *p, int num, ...)
{
	TYOS_Stop_Schedule();
	va_list vl;
	va_start(vl, num);
	arraylist *mtx_ids = &p->Request_Mutex_IDs;
	for (int i = 0; i < num; i++)
	{
		mutex_id mtx_id = va_arg(vl, mutex_id);
		//检查锁是否已经被注册过了
		for (int j = 0; j < mtx_ids->Member_Num; j++)
		{
			if (ArrayList_Get_Member(mtx_ids, j, mutex_id) == mtx_id)
			{
				mutex *mtx;
				FIND_MUTEX_WITH_ID(mtx_id, mtx);
				//如果此锁已被进程p预约，首先要取消预约
				if (mtx->Reserve_Process == p)
				{
					if (!mtx->Locked)
					{
						MUTEX_RESERVE_CANCEL(mtx);
						//如果等待预约的进程数不为0
						//则更新预约进程
						if (mtx->Waiting_Process.Member_Num != 0)
						{
							process *p_nxt = ArrayList_Get_Member(&mtx->Waiting_Process, 0, process *);
							MUTEX_RESERVE(mtx, p_nxt);
							ArrayList_Remove(&mtx->Waiting_Process, 0);
							//检查p_nxt是否所有锁都预约成功
							if (p_nxt->Mutex_Count == 0)
							{
								arraylist *pnxt_mtx_ids = &p_nxt->Request_Mutex_IDs;
								for (int k = 0; k < p_nxt->Request_Mutex_IDs.Member_Num; k++)
								{
									FIND_MUTEX_WITH_ID(ArrayList_Get_Member(pnxt_mtx_ids, k, mutex_id), mtx);
									MUTEX_LOCK(mtx, p_nxt);
								}
								//唤醒p_nxt
								p_nxt->Status = PROCESS_READY;
								FIND_NEXT_RDY_PROCESS_FROM_PM(p_nxt->Manager, p_nxt, p_nxt->Manager->Ready_Process);
							}
						}
					}
					else
					{
						//错误，必须先解锁才能注销
					}
				}
				else
				{
					//查找进程p是否处于等待队列中
					arraylist *p_waitqueue = &mtx->Waiting_Process;
					for (int k = 0; k < p_waitqueue->Member_Num; k++)
					{
						if (ArrayList_Get_Member(p_waitqueue, k, process *) == p)
						{
							ArrayList_Remove(p_waitqueue, k);
							break;
						}
					}
				}
				ArrayList_Remove(&p->Request_Mutex_IDs, j);
				p->Mutex_Count--;
				goto SKIP;
			}
		}
		//错误，每个锁必须已经被注册过
	SKIP:
		continue;
	}
	TYOS_Resume_Schedule();
}

void _TYOS_Mutex_Unregist_All(process *p)
{
	TYOS_Stop_Schedule();
	mutex *mtx;
	arraylist *mtx_ids = &p->Request_Mutex_IDs;
	for (int i = mtx_ids->Member_Num; i > 0; i--)
	{
		FIND_MUTEX_WITH_ID(ArrayList_Get_Member(mtx_ids, 0, mutex_id), mtx);
		if (mtx->Reserve_Process == p)
		{
			if (!mtx->Locked)
			{
				MUTEX_RESERVE_CANCEL(mtx);
				//如果等待预约的进程数不为0
				//则更新预约进程
				if (mtx->Waiting_Process.Member_Num != 0)
				{
					process *p_nxt = ArrayList_Get_Member(&mtx->Waiting_Process, 0, process *);
					MUTEX_RESERVE(mtx, p_nxt);
					ArrayList_Remove(&mtx->Waiting_Process, 0);
					if (p_nxt->Mutex_Count == 0)
					{
						mutex_id *pres_mtx_ids = p_nxt->Request_Mutex_IDs.Members;
						for (int j = 0; j < p_nxt->Request_Mutex_IDs.Member_Num; j++)
						{
							FIND_MUTEX_WITH_ID(pres_mtx_ids[j], mtx);
							MUTEX_LOCK(mtx, p_nxt);
						}
						//唤醒p_nxt
						p_nxt->Status = PROCESS_READY;
						FIND_NEXT_RDY_PROCESS_FROM_PM(p_nxt->Manager, p_nxt, p_nxt->Manager->Ready_Process);
					}
				}
			}
			else
			{
				//错误，必须先解锁才能注销
			}
		}
		else
		{
			//查找进程p是否处于等待队列中
			arraylist *p_waitqueue = &mtx->Waiting_Process;
			for (int j = 0; j < p_waitqueue->Member_Num; j++)
			{
				if (ArrayList_Get_Member(p_waitqueue, j, process *) == p)
				{
					ArrayList_Remove(p_waitqueue, j);
					break;
				}
			}
		}
		ArrayList_Remove(mtx_ids, i);
		p->Mutex_Count--;
	}
	TYOS_Resume_Schedule();
}

//如果成功，则正常退出函数
//如果失败，则进程放弃cpu，进入等待状态
void _TYOS_Mutex_Lock(process *p)
{
	TYOS_Stop_Schedule();
	/********************************尝试预约********************************/
	mutex *mtx;
	mutex_id *mtx_id = p->Request_Mutex_IDs.Members;
	for (int i = 0; i < p->Request_Mutex_IDs.Member_Num; i++)
	{
		FIND_MUTEX_WITH_ID(mtx_id[i], mtx);
		//情况一：没有预约，此时预约成功
		if (mtx->Reserve_Process == NULL)
		{
			MUTEX_RESERVE(mtx, p);
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
						ArrayList_Insert2(&mtx->Waiting_Process, j, process *) = p_sel;
						goto INSERT_FIN0;
					}
				}
				// p是最低优先级，添加到末尾
				ArrayList_AddtoEnd2(&mtx->Waiting_Process, process *) = p_sel;
			INSERT_FIN0:
				//更新预约进程
				if (p_sel == mtx->Reserve_Process)
				{
					MUTEX_RESERVE_CANCEL(mtx);
					MUTEX_RESERVE(mtx, p);
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
						ArrayList_Insert2(&mtx->Waiting_Process, j, process *) = p;
						goto INSERT_FIN1;
					}
				}
				// p是最低优先级，添加到末尾
				ArrayList_AddtoEnd2(&mtx->Waiting_Process, process *) = p;
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
		for (int i = 0; i < p->Request_Mutex_IDs.Member_Num; i++)
		{
			FIND_MUTEX_WITH_ID(mtx_id[i], mtx);
			MUTEX_LOCK(mtx, p);
		}
	}
	else
	{
		//预约失败，当前进程放弃CPU，重新进行调度
		p->Status = PROCESS_AWAIT_MUTEX;
		FIND_NEXT_RDY_PROCESS_FROM_PM(p->Manager, p, p->Manager->Ready_Process);
		Set_PendSV_Interrupt();
	}
	TYOS_Resume_Schedule();
}

//如果成功，则正常退出函数
//如果失败，则进程放弃cpu，进入等待状态
void _TYOS_Mutex_Lock_Direct(process *p, int num, ...)
{
	TYOS_Stop_Schedule();
	/******************************构建mutex列表******************************/
	va_list vl;
	va_start(vl, num);
	ArrayList_Clear(&p->Request_Mutex_IDs);
	mutex_id *mtx_id = ArrayList_Insert_Group(&p->Request_Mutex_IDs, 0, num);
	for (int i = 0; i < num; i++)
		mtx_id[i] = va_arg(vl, mutex_id);
	va_end(vl);
	p->Mutex_Count = num;
	/********************************尝试预约********************************/
	mutex *mtx;
	for (int i = 0; i < p->Request_Mutex_IDs.Member_Num; i++)
	{
		FIND_MUTEX_WITH_ID(mtx_id[i], mtx);
		//情况一：没有预约，此时预约成功
		if (mtx->Reserve_Process == NULL)
		{
			MUTEX_RESERVE(mtx, p);
		}
		else
		{
			//情况二：已经存在预约进程，但未锁定，优先级低的进程进入等待队列，不改变优先级
			if (!mtx->Locked)
			{
				//获取正在等待的进程队列
				process **p_waitqueue = mtx->Waiting_Process.Members;
				// p_sel将会加入等待队列
				process *p_sel = p->Initial_Pri > mtx->Reserve_Process->Initial_Pri ? p : mtx->Reserve_Process;
				//将p加入等待队列
				for (int j = 0; j < mtx->Waiting_Process.Member_Num; j++)
				{
					//寻找第一个优先级低于p的进程，在前面插入p
					if (p_waitqueue[j]->Initial_Pri > p_sel->Initial_Pri)
					{
						ArrayList_Insert2(&mtx->Waiting_Process, j, process *) = p_sel;
						goto INSERT_FIN0;
					}
				}
				// p是最低优先级，添加到末尾
				ArrayList_AddtoEnd2(&mtx->Waiting_Process, process *) = p_sel;
			INSERT_FIN0:
				//更新预约进程
				if (p_sel == mtx->Reserve_Process)
				{
					MUTEX_RESERVE_CANCEL(mtx);
					MUTEX_RESERVE(mtx, p);
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
						ArrayList_Insert2(&mtx->Waiting_Process, j, process *) = p;
						goto INSERT_FIN1;
					}
				}
				// p是最低优先级，添加到末尾
				ArrayList_AddtoEnd2(&mtx->Waiting_Process, process *) = p;
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
	va_end(vl);
	/********************************检查是否预约成功********************************/
	if (p->Mutex_Count == 0)
	{
		for (int i = 0; i < p->Request_Mutex_IDs.Member_Num; i++)
		{
			FIND_MUTEX_WITH_ID(mtx_id[i], mtx);
			MUTEX_LOCK(mtx, p);
		}
	}
	else
	{
		//预约失败，当前进程放弃CPU，重新进行调度
		p->Status = PROCESS_AWAIT_MUTEX;
		FIND_NEXT_RDY_PROCESS_FROM_PM(p->Manager, p, p->Manager->Ready_Process);
		Set_PendSV_Interrupt();
	}
	TYOS_Resume_Schedule();
}

status_flag _TYOS_Mutex_Lock_Try(process *p)
{
	TYOS_Stop_Schedule();
	/********************************检查是否可以成功预约********************************/
	status_flag res_success = Enable;
	mutex *mtx;
	for (int i = 0; i < p->Request_Mutex_IDs.Member_Num; i++)
	{
		FIND_MUTEX_WITH_ID(ArrayList_Get_Member(&p->Request_Mutex_IDs, i, mutex_id), mtx);
		if (mtx->Locked)
		{
			res_success = Disable;
			break;
		}
		else
		{
			if (mtx->Reserve_Process != NULL && mtx->Reserve_Process->Initial_Pri <= p->Initial_Pri)
			{
				res_success = Disable;
				break;
			}
		}
	}
	/********************************尝试预约********************************/
	if (res_success)
		_TYOS_Mutex_Lock(p);
	TYOS_Resume_Schedule();
	return res_success;
}

status_flag _TYOS_Mutex_Lock_Direct_Try(process *p, int num, ...)
{
	TYOS_Stop_Schedule();
	/******************************构建mutex列表******************************/
	va_list vl;
	va_start(vl, num);
	ArrayList_Clear(&p->Request_Mutex_IDs);
	mutex_id *mtx_id = ArrayList_Insert_Group(&p->Request_Mutex_IDs, 0, num);
	for (int i = 0; i < num; i++)
		mtx_id[i] = va_arg(vl, mutex_id);
	va_end(vl);
	p->Mutex_Count = num;
	/********************************检查是否可以成功预约********************************/
	status_flag res_success = Enable;
	mutex *mtx;
	for (int i = 0; i < p->Request_Mutex_IDs.Member_Num; i++)
	{
		FIND_MUTEX_WITH_ID(ArrayList_Get_Member(&p->Request_Mutex_IDs, i, mutex_id), mtx);
		if (mtx->Locked)
		{
			res_success = Disable;
			break;
		}
		else
		{
			if (mtx->Reserve_Process != NULL && mtx->Reserve_Process->Initial_Pri <= p->Initial_Pri)
			{
				res_success = Disable;
				break;
			}
		}
	}
	/********************************尝试预约********************************/
	if (res_success)
		_TYOS_Mutex_Lock(p);
	TYOS_Resume_Schedule();
	return res_success;
}

void _TYOS_Mutex_Release(process *p, int num, ...)
{
	TYOS_Stop_Schedule();
	va_list vl;
	va_start(vl, num);
	mutex *mtx;
	for (int i = 0; i < num; i++)
	{
		mutex_id mtx_id = va_arg(vl, mutex_id);
		FIND_MUTEX_WITH_ID(mtx_id, mtx);
		//锁必须被注册过
		if (TYOS_Mutex_Is_Registed(p, mtx_id))
		{
			if (mtx->Locked)
			{
				MUTEX_UNLOCK(mtx);
				MUTEX_RESERVE_CANCEL(mtx);
				if (mtx->Waiting_Process.Member_Num != 0)
				{
					//更新mtx状态
					process *p_wait = ArrayList_Get_Member(&mtx->Waiting_Process, 0, process *);
					MUTEX_RESERVE(mtx, p_wait);
					ArrayList_Remove(&mtx->Waiting_Process, 0);
					//检查p_wait是否所有锁都被预约成功了
					if (p_wait->Mutex_Count == 0)
					{
						//锁定每一个互斥锁
						for (int j = 0; j < p_wait->Request_Mutex_IDs.Member_Num; j++)
						{
							FIND_MUTEX_WITH_ID(ArrayList_Get_Member(&p_wait->Request_Mutex_IDs, j, mutex_id), mtx);
							MUTEX_LOCK(mtx, p_wait);
						}
						//唤醒p_wait
						p_wait->Status = PROCESS_READY;
						FIND_NEXT_RDY_PROCESS_FROM_PM(p_wait->Manager, p_wait, p_wait->Manager->Ready_Process);
					}
				}
			}
			else
			{
				//错误
			}
		}
		else
		{
			//错误
		}
	}
	va_end(vl);
	//最后检查p是否释放了全部的互斥锁
	if (p->Mutex_Count == p->Request_Mutex_IDs.Member_Num)
	{
		//如果是，那么恢复进程的优先级
		if (p->Current_Pri != p->Initial_Pri)
		{
			EMIGRATE_PROCESS_FROM_PM(p);
			process_manager *pm_new;
			FIND_PM_WITH_PRI(p->Initial_Pri, pm_new);
			IMMIGRATE_PROCESS_IN_PM(pm_new, p);
			p->Current_Pri = p->Initial_Pri;
			p->Manager = pm_new;
		}
	}
	TYOS_Resume_Schedule();
}

status_flag _TYOS_Mutex_Is_Locked(mutex_id id)
{
	TYOS_Stop_Schedule();
	status_flag rtn;
	mutex *mtx;
	FIND_MUTEX_WITH_ID(id, mtx);
	rtn = mtx->Locked;
	TYOS_Resume_Schedule();
	return rtn;
}

status_flag _TYOS_Mutex_Is_Registed(process *p, mutex_id id)
{
	TYOS_Stop_Schedule();
	status_flag rtn;
	mutex_id *mtx_ids = (mutex_id *)(p->Request_Mutex_IDs.Members);
	for (int i = 0; i < p->Request_Mutex_IDs.Member_Num; i++)
	{
		if (mtx_ids[i] == id)
		{
			rtn = Enable;
			goto RETURN;
		}
	}
	rtn = Disable;
RETURN:
	TYOS_Resume_Schedule();
	return rtn;
}
