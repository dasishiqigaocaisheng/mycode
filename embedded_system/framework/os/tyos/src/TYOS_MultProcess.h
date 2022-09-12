#include "TYOS_DataStructure.h"

#define TYOS_PROCESS_IS_LOCKED(p) ((p)->Mutex_Count == 0)

mutex_id _TYOS_Create_Metux(process *p, uint32_t object);
void _TYOS_Delete_Metux(mutex_id id);
void _TYOS_Mutex_Regist(process *p, int num, ...);
void _TYOS_Mutex_Unregist(process *p, int num, ...);
void _TYOS_Mutex_Unregist_All(process *p);
void _TYOS_Mutex_Lock(process *p);
void _TYOS_Mutex_Lock_Direct(process *p, int num, ...);
status_flag _TYOS_Mutex_Lock_Try(process *p);
status_flag _TYOS_Mutex_Lock_Direct_Try(process *p, int num, ...);
void _TYOS_Mutex_Release(process *p, int num, ...);
status_flag _TYOS_Mutex_Is_Locked(mutex_id id);
status_flag _TYOS_Mutex_Is_Registed(process *p, mutex_id id);
