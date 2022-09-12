#ifndef _TYOS_SYSCALL_H_
#define _TYOS_SYSCALL_H_

extern void TYOS_Global_Interrupt_Disable(void);
extern void TYOS_Global_Interrupt_Enable(void);
extern void TYOS_Stop_Schedule(void);
extern void TYOS_Resume_Schedule(void);
extern void TYOS_Schedule_Immediately(void);

extern mutex_id TYOS_Create_Metux(process *p, uint32_t object);
extern void TYOS_Delete_Metux(mutex_id id);
extern void TYOS_Mutex_Regist(process *p, int num, ...);
extern void TYOS_Mutex_Unregist(process *p, int num, ...);
extern void TYOS_Mutex_Unregist_All(process *p);
extern void TYOS_Mutex_Lock(process *p);
extern void TYOS_Mutex_Lock_Direct(process *p, int num, ...);
extern status_flag TYOS_Mutex_Lock_Try(process *p);
extern status_flag TYOS_Mutex_Lock_Direct_Try(process *p, int num, ...);
extern void TYOS_Mutex_Release(process *p, int num, ...);
extern status_flag TYOS_Mutex_Is_Locked(mutex_id id);
extern status_flag TYOS_Mutex_Is_Registed(process *p, mutex_id id);

#endif
