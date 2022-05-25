#ifndef _TYOS_DATASTRUCTURE_H_
#define _TYOS_DATASTRUCTURE_H_

#include "Framework.h"
#include "LinkedList.h"
#include "ArrayList.h"
#include "Memory.h"

typedef uint32_t process_id;
typedef int32_t metux_id;
typedef uint8_t process_priority;

typedef void (*task)(void);

#define METUXID_METUXES 0
#define METUXID_PROCESSES 1
#define METUXID_HEAP0 2
#define METUXID_HEAP1 3
#define METUXID_HEAP2 4
#define METUXID_HEAP3 5
#define METUXID_HEAP4 6
#define SYS_METUXID_MAX METUXID_HEAP4

typedef enum
{
    PROCESS_ACTIVE,
    PROCESS_AWAIT_TIME,
    PROCESS_AWAIT_MUTEX,
    PROCESS_AWAIT_RESOURCE,
    PROCESS_READY,
    PROCESS_SLEEP
} process_status;

typedef struct __ALIGNED(4)
{
    uint32_t *StackTop;
    task Task;
    process_id ID;
    process_priority Initial_Pri;
    process_priority Current_Pri;
    process_status Status;
    int Status_Para;
    uint32_t *Stack;
} process;

typedef struct __ALIGNED(4)
{
    metux_id ID;
    status_flag Active;
    process_id Owner;
    process_id User;
} metux;

typedef struct
{
    process_priority Priority;
    int Process_Ready_Index;
    arraylist Processes;
} process_list_base;

typedef struct
{
    uint8_t TimeSlice; //时间片长度（ms）
    uint64_t Tick;     //从操作系统启动开始经过的tick
    process *Active_Process;
    uint8_t Process_Number;
    linkedlist Process_List;
    arraylist Mutexes;  //互斥锁列表
    heap *Process_Heap; //默认进程栈所在的堆
    metux_id ProcessStack_MutexID;
} tyos;

#endif
