#ifndef _ARRAYLIST_H_
#define _ARRAYLIST_H_

#include <stdint.h>
#include "Framework.h"
#include "Memory.h"

typedef struct
{
    vro uint16_t Member_Size;
    vro heap_type Heap;
    vro uint8_t MinMalloc_Num;
    uint16_t MaxNum;
    uint16_t Member_Num;
    void *Members;
} arraylist;

#define ArrayList_Prepare(al, m_size, heap, mm_num) \
    _RO_WRITE(al->Member_Size, uint16_t, m_size);   \
    _RO_WRITE(al->Heap, heap_type, heap);           \
    _RO_WRITE(al->MinMalloc_Num, uint8_t, mm_num);  \
    al->Members = Malloc(heap, m_size * mm_num);    \
    al->MaxNum = Malloc_Size(al->Members) / m_size; \
    al->Member_Num = 0;

#define ArrayList_AddtoEnd(al) ArrayList_Insert(al, al->Member_Num);

#define ArrayList_RemoveLast(al) ArrayList_Remove(al, al->Member_Num - 1);

void ArrayList_Insert(arraylist *al, uint16_t idx);
void ArrayList_Remove(arraylist *al, uint16_t idx);

#endif
