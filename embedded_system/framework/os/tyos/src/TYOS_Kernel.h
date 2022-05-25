#ifndef _TYOS_KERNEL_H_
#define _TYOS_KERNEL_H_

#include "TYOS_DataStructure.h"

#define Set_PendSV_Interrupt() SCB->ICSR |= 1 << 28
								

void TYOS_Kernel_Prepare(uint16_t t_slice, heap* p_heap);
process_id TYOS_Create_Process(process_priority pri, task tsk);


#endif
