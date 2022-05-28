#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include "Framework.h"

typedef struct linkedlist_class
{
    uint16_t DataField_Size;
    uint16_t Node_Size;
    uint16_t Nodes_Num;
    status_flag Is_Loop;
    void *Head_Addr;
} linkedlist;

#define LinkedList_Prepare(l, obsize, is_loop)                             \
    {                                                                      \
        (l)->DataField_Size = (obsize);                                    \
        (l)->Node_Size = ((obsize) / 4 + ((obsize) % 4 == 0 ? 2 : 3)) * 4; \
        (l)->Nodes_Num = 0;                                                \
        (l)->Head_Addr = NULL;                                             \
        (l)->Is_Loop = is_loop;                                            \
    }

#define LinkedList_Get_NextObject(l, object) (object != NULL ? (*(void **)((uint32_t)(object) + (l)->Node_Size - 8)) : NULL)
#define LinkedList_Get_LastObject(l, object) (object != NULL ? (*(void **)((uint32_t)(object)-4)) : NULL)
#define LinkedList_Get_FirstObject(l) (l != NULL ? ((l)->Head_Addr) : NULL)

void *LinkedList_Find(linkedlist *l, int index);
void *LinkedList_Add(linkedlist *l, int index);
void LinkedList_Add2(linkedlist *l, int index, void *object);
void *LinkedList_AddBehind(linkedlist *l, void *object);
void LinkedList_AddBehind2(linkedlist *l, void *object);
void *LinkedList_AddtoEnd(linkedlist *l);
void LinkedList_AddtoEnd2(linkedlist *l, void *object);
void *LinkedList_Remove(linkedlist *l, int index);
void LinkedList_Remove2(linkedlist *l, void *object);
void LinkedList_Dispose(linkedlist *l, int index);
void LinkedList_Dispose2(linkedlist *l, void *object);

void LinkeList_InfoPrint(USART_TypeDef *usart, linkedlist *l);

#endif
