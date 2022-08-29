#ifndef _TYOS_KERNEL_H_
#define _TYOS_KERNEL_H_

#include "TYOS_DataStructure.h"

#define SVC_KILLPROCESS 0

#define TYOS_PRCS_STACK_SIZE 512
#define TYOS_PRCS_HEAP_SIZE 2048
#define Set_PendSV_Interrupt() \
    SCB->ICSR |= 1 << 28;      \
    __DSB();                   \
    __ISB()
#define TYOS_Get_Priority(id) ((id & 0xff00) >> 8)

extern void TYOS_Start(void);

void TYOS_Kernel_Prepare(uint16_t t_slice, heap *p_heap);
process *TYOS_Create_Process(process_priority pri, task tsk);
process *TYOS_Get_Process(process_id id);
void TYOS_Process_Start(process *p);
void TYOS_Process_Wait_Time(process *p, uint32_t time);

#endif
