#ifndef _TYOS_DATASTRUCTURE_H_
#define _TYOS_DATASTRUCTURE_H_

#include "Framework.h"

typedef int thread_id;
typedef int metux_id;
typedef int thread_priority;

typedef enum
{
    THREAD_ACTIVE,
    THREAD_AWAIT_TIME,
    THREAD_AWAIT_MUTEX,
    THREAD_AWAIT_RESOURCE,
    THREAD_READY
} thread_status;

typedef struct __ALIGNED(4)
{
    uint8_t *Stack;
    thread_id ID;
    thread_priority Initial_Pri;
    thread_priority Current_Pri;
    thread_status Status;
    int Status_Para;
} thread;

typedef struct __ALIGNED(4)
{
    metux_id ID;
    status_flag Active;
    thread_id Owner;
    thread_id User;
} mutex;

#endif
