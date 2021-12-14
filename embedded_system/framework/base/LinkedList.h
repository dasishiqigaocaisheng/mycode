#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include "Framework.h"


typedef struct linkedlist_class
{
    uint16_t DataField_Size;
    uint16_t Node_Size;
    uint16_t Nodes_Num;
    void*    Head_Addr;
} linkedlist;



void LinkedList_Prepare(linkedlist* l, uint16_t obsize);
void* LinkedList_Find(linkedlist* l, int index);
void* LinkedList_Add(linkedlist* l, int index);
void LinkedList_Add2(linkedlist* l, int index, void* object);
void* LinkedList_Remove(linkedlist* l, int index);
void LinkedList_Remove2(linkedlist* l, void* object);
void LinkedList_Dispose(linkedlist* l, int index);
void LinkedList_Dispose2(linkedlist* l, void* object);

void LinkeList_InfoPrint(USART_TypeDef* usart, linkedlist* l);





#endif
