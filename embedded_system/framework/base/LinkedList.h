#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include "Framework.h"

typedef struct linkedlist_class
{
    uint16_t DataField_Size;
    uint16_t Node_Size;
    uint16_t Nodes_Num;
    void *Head_Addr;
} linkedlist;

#define LinkedList_Prepare(l, obsize)                                  \
    {                                                                  \
        (l)->DataField_Size = (obsize);                                \
        (l)->Node_Size = ((obsize) / 4 + obsize % 4 == 0 ? 1 : 2) * 4; \
        (l)->Nodes_Num = 0;                                            \
        (l)->Head_Addr = NULL;                                         \
    }

#define LinkedList_Get_Next(l, object) (void *)((uint32_t)(object) + (l)->Node_Size - 4)
#define LinkedList_Get_FirstNode(l) (l)->Head_Addr

void *LinkedList_Find(linkedlist *l, int index);
void *LinkedList_Add(linkedlist *l, int index);
void LinkedList_Add2(linkedlist *l, int index, void *object);
void *LinkedList_AddtoEnd(linkedlist *l);
void LinkedList_AddtoEnd2(linkedlist *l, void *object);
void *LinkedList_Remove(linkedlist *l, int index);
void LinkedList_Remove2(linkedlist *l, void *object);
void LinkedList_Dispose(linkedlist *l, int index);
void LinkedList_Dispose2(linkedlist *l, void *object);

void LinkeList_InfoPrint(USART_TypeDef *usart, linkedlist *l);

#endif
