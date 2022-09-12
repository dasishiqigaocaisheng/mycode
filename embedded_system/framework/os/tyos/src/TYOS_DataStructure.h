#ifndef _TYOS_DATASTRUCTURE_H_
#define _TYOS_DATASTRUCTURE_H_

#include "Framework.h"
#include "LinkedList.h"
#include "ArrayList.h"
#include "Memory.h"

typedef uint32_t process_id;
typedef int32_t mutex_id;
typedef int32_t semaphore_id;
typedef int32_t signal_id;
typedef uint8_t process_priority;

typedef void (*task)(void);

typedef enum process_status process_status;
typedef struct process process;
typedef struct process_handler process_handler;
typedef struct mutex mutex;
typedef struct process_manager process_manager;
typedef struct process_pri_list process_pri_list;
typedef struct waiting_time_table waiting_time_table;
typedef struct tyos tyos;
typedef union asset asset;
typedef struct process_asset process_asset;

enum process_status
{
    PROCESS_ACTIVE,
    PROCESS_AWAIT_TIME,
    PROCESS_AWAIT_MUTEX,
    PROCESS_AWAIT_RESOURCE,
    PROCESS_READY,
    PROCESS_SLEEP
};

struct process
{
    uint32_t *StackTop;
    task Task;
    process_id ID;
    process_priority Initial_Pri;
    process_priority Current_Pri;
    process_status Status;
    uint32_t Status_Para;
    uint32_t *Stack;

    process_manager *Manager;
    process_handler *Handler;

    uint64_t Start_Time;
    uint64_t Running_Time;
    arraylist Assets;

    int32_t Mutex_Count;
    arraylist Request_Mutex_IDs;
} __ALIGNED(4);

struct mutex
{
    mutex_id ID;
    process *Owner;
    status_flag Locked;
    process *User;
    uint32_t Object;

    process *Reserve_Process;
    arraylist Waiting_Process;
} __ALIGNED(4);

struct semaphore
{
    semaphore_id ID;
    process *Owner;
    uint32_t Use_Table;
    uint8_t Count;
    arraylist Waiting_Process;
};

struct process_handler
{
    process_id ID;
    process *Address;
    process_manager *Manager;
};

struct process_manager
{
    process_priority Priority;
    process *Ready_Process;
    linkedlist Handler;
    linkedlist Instance;
};

struct waiting_time_table
{
    process *Process;
    uint64_t End_Tick;
};
struct tyos
{
    process *Active_Process;     //当前占用CPU的进程
    uint8_t TimeSlice;           //时间片长度（ms）
    uint64_t Tick;               //从操作系统启动开始经过的tick
    uint8_t Process_Number;      //进程总数
    linkedlist Process_Managers; // process_pri_list
    arraylist Mutexes;           //互斥锁列表
    arraylist Semaphores;
    heap *Process_Heap; //默认进程栈所在的堆
    arraylist Waiting_Processes;
} __ALIGNED(4);

union asset
{
    mutex *Mutex;
};

struct process_asset
{
    uint8_t Asset_Type;
    asset Asset;
};

#endif
