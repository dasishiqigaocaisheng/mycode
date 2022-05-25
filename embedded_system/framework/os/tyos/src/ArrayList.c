#include "ArrayList.h"
#include <string.h>

#define FIND_MEMBERADDR_WITH_INDEX(m_addr, m_size, idx) (((uint8_t *)(m_addr)) + (idx) * (m_size))

void ArrayList_Insert(arraylist *al, uint16_t idx)
{
    if (idx > al->Member_Num)
        return;

    if (al->Member_Num == al->MaxNum)
    {
        uint8_t *new_addr = Memory_Malloc((heap*)al->Heap, (al->MaxNum + al->MinMalloc_Num) * al->Member_Size);
        memcpy(new_addr, al->Members, idx * al->Member_Size);
        memcpy(FIND_MEMBERADDR_WITH_INDEX(new_addr, al->Member_Size, idx + 1),
               FIND_MEMBERADDR_WITH_INDEX(al->Members, al->Member_Size, idx),
               (al->Member_Num - idx) * al->Member_Size);
        al->MaxNum = Memory_Malloc_Size(al->Heap, new_addr) / al->Member_Size;
        Memory_Free((heap*)al->Heap, al->Members);
        al->Members = new_addr;
    }
    else
        memmove(FIND_MEMBERADDR_WITH_INDEX(al->Members, al->Member_Size, idx + 1),
                FIND_MEMBERADDR_WITH_INDEX(al->Members, al->Member_Size, idx),
                (al->Member_Num - idx) * al->Member_Size);
    al->Member_Num++;
}

void ArrayList_Remove(arraylist *al, uint16_t idx)
{
    if (idx > al->Member_Num)
        return;

    if (Memory_Malloc_Size(al->Heap, al->Members) - (al->Member_Num - 1) * al->Member_Size >= al->Heap->BlockSize)
    {
        uint8_t *new_addr = Memory_Malloc((heap*)al->Heap, (al->Member_Num - 1) * al->Member_Size);
        memcpy(new_addr, al->Members, idx * al->Member_Size);
        memcpy(FIND_MEMBERADDR_WITH_INDEX(new_addr, al->Member_Size, idx),
               FIND_MEMBERADDR_WITH_INDEX(new_addr, al->Member_Size, idx + 1),
               (al->Member_Num - idx - 1) * al->Member_Size);
        al->MaxNum = Memory_Malloc_Size(al->Heap, new_addr) / al->Member_Size;
        Memory_Free((heap*)al->Heap, al->Members);
        al->Members = new_addr;
    }
    else
        memmove(FIND_MEMBERADDR_WITH_INDEX(al->Members, al->Member_Size, idx),
                FIND_MEMBERADDR_WITH_INDEX(al->Members, al->Member_Size, idx + 1),
                (al->Member_Num - idx - 1) * al->Member_Size);
    al->Member_Num--;
}
