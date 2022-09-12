#ifndef _ARRAYLIST_H_
#define _ARRAYLIST_H_

#include <stdint.h>
#include "Framework.h"
#include "Memory.h"

typedef struct
{
    vro uint16_t Member_Size;
    vro heap *Heap;
    vro uint8_t MinMalloc_Num;
    uint16_t MaxNum;
    uint16_t Member_Num;
    void *Members;
} arraylist;

#define ArrayList_Prepare(al, m_size, al_heap, mm_num)                                \
    {                                                                                 \
        _RO_WRITE((al)->Member_Size, uint16_t, m_size);                               \
        _RO_WRITE((al)->Heap, heap *, al_heap);                                       \
        _RO_WRITE((al)->MinMalloc_Num, uint8_t, mm_num);                              \
        (al)->Members = Memory_Malloc((heap *)(al_heap), m_size * (mm_num));          \
        (al)->MaxNum = Memory_Malloc_Size((heap *)(al_heap), (al)->Members) / m_size; \
        (al)->Member_Num = 0;                                                         \
    }

#define ArrayList_Insert2(al, idx, type) (*(type *)ArrayList_Insert(al, idx))
#define ArrayList_AddtoEnd(al) ArrayList_Insert(al, (al)->Member_Num)
#define ArrayList_AddtoEnd2(al, type) (*(type *)ArrayList_Insert(al, (al)->Member_Num))
#define ArrayList_RemoveLast(al) ArrayList_Remove(al, (al)->Member_Num - 1)
#define ArrayList_AddBehind(al, obj) ArrayList_Insert(al, ((uint32_t)(obj) - ((uint32_t)(al)->Members)) / (al)->Member_Size + 1)
#define ArrayList_AddBehind2(al, obj, type) (*(type *)ArrayList_Insert(al, ((uint32_t)(obj) - ((uint32_t)(al)->Members)) / (al)->Member_Size + 1))
#define ArrayList_Get_Index(al, obj) (((uint32_t)(obj) - (uint32_t)(al)->Members) / ((al)->Member_Size))
#define ArrayList_Get_Member(al, idx, type) ((type *)((al)->Members))[idx]
#define ArrayList_IsLastOne(al, obj) (((uint32_t)(obj)) == ((uint32_t)(al)->Members) + ((al)->Member_Num - 1) * (al)->Member_Size)
#define ArrayList_Clear(al)                                                                         \
    {                                                                                               \
        Memory_Free((heap *)(al)->Heap, (al)->Members);                                             \
        (al)->Members = Memory_Malloc((heap *)(al)->Heap, (al)->Member_Size * (al)->MinMalloc_Num); \
        (al)->Member_Num = 0;                                                                       \
    }

void *ArrayList_Insert(arraylist *al, uint16_t idx);
void *ArrayList_Insert_Group(arraylist *al, uint16_t idx, uint16_t num);
void ArrayList_Remove(arraylist *al, uint16_t idx);
void ArrayList_Remove2(arraylist *al, void *obj);
status_flag ArrayList_IsContain(arraylist *al, void *object);

#endif
