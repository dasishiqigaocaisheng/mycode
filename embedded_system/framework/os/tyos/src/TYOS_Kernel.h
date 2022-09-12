#ifndef _TYOS_KERNEL_H_
#define _TYOS_KERNEL_H_

#include "TYOS_DataStructure.h"

#define SVC_KILLPROCESS 0

#define TYOS_PROCESS_STACK_SIZE 512
#define TYOS_PROCESS_HEAP_SIZE 2048

#define TYOS_Get_Priority(id) (((id) >> 8) & 0xff)
#define TYOS_Get_Index(id) ((id)&0xff)

extern void TYOS_Start(void);
extern void sys_call(int a, int b, int c, ...);

void TYOS_Kernel_Prepare(uint16_t t_slice, heap *p_heap);
process *TYOS_Create_Process(process_priority pri, task tsk);
process *TYOS_Get_Process(process_id id);
void TYOS_Process_Start(process *p);
void TYOS_Process_Wait_Time(process *p, uint32_t time);

#endif
