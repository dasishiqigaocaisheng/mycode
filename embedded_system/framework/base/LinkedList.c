#include "LinkedList.h"
#include "Memory.h"

#include "USART.h"

#define LINKEDLIST_HEAP heap0

#define FIND_NEXTADDR(l, index) ((void **)((uint32_t)LinkedList_Find(l, index) + l->Node_Size - 4))

void *LinkedList_Find(linkedlist *l, int index)
{
    int i;
    void *next;

    next = l->Head_Addr;
    for (i = 0; i < index; i++)
    {
        if (next == NULL)
            break;
        next = (void *)(*(uint32_t *)((uint32_t)next + l->Node_Size - 4));
    }
    return next;
}

void *LinkedList_Add(linkedlist *l, int index)
{
    if (index > l->Nodes_Num)
        return NULL;

    void *save;
    if (index == 0)
    {
        save = l->Head_Addr;
        l->Head_Addr = Memory_Malloc(&LINKEDLIST_HEAP, l->Node_Size);
        *FIND_NEXTADDR(l, 0) = save;
        l->Nodes_Num++;
        return l->Head_Addr;
    }
    else
    {
        save = LinkedList_Find(l, index);
        *FIND_NEXTADDR(l, index - 1) = Memory_Malloc(&LINKEDLIST_HEAP, l->Node_Size);
        *FIND_NEXTADDR(l, index) = save;
        l->Nodes_Num++;
        return LinkedList_Find(l, index);
    }
}

void LinkedList_Add2(linkedlist *l, int index, void *object)
{
    if (index > l->Nodes_Num)
        return;

    void *save;
    if (index == 0)
    {
        save = l->Head_Addr;
        l->Head_Addr = object;
        *FIND_NEXTADDR(l, 0) = save;
        l->Nodes_Num++;
    }
    else
    {
        save = LinkedList_Find(l, index);
        *FIND_NEXTADDR(l, index - 1) = object;
        *FIND_NEXTADDR(l, index) = save;
        l->Nodes_Num++;
    }
}

void *LinkedList_AddtoEnd(linkedlist *l)
{
    return LinkedList_Add(l, l->Nodes_Num);
}

void LinkedList_AddtoEnd2(linkedlist *l, void *object)
{
    LinkedList_Add2(l, l->Nodes_Num, object);
}

void *LinkedList_Remove(linkedlist *l, int index)
{
    if (index + 1 > l->Nodes_Num)
        return NULL;

    void *save;
    if (index == 0)
    {
        save = l->Head_Addr;
        l->Head_Addr = LinkedList_Find(l, 1);
        // Free(LINKEDLIST_HEAP,save);
        l->Nodes_Num--;
    }
    else
    {
        save = LinkedList_Find(l, index);
        *FIND_NEXTADDR(l, index - 1) = LinkedList_Find(l, index + 1);
        // Free(LINKEDLIST_HEAP,save);
        l->Nodes_Num--;
    }
    return save;
}

void LinkedList_Remove2(linkedlist *l, void *object)
{
    int i;

    for (i = 0; i < l->Nodes_Num; i++)
    {
        if (LinkedList_Find(l, i) == object)
            break;
    }
    LinkedList_Remove(l, i);
}

void LinkedList_Dispose(linkedlist *l, int index)
{
    if (index + 1 > l->Nodes_Num)
        return;

    void *save;
    if (index == 0)
    {
        save = l->Head_Addr;
        l->Head_Addr = LinkedList_Find(l, 1);
        Memory_Free(&LINKEDLIST_HEAP, save);
        l->Nodes_Num--;
    }
    else
    {
        save = LinkedList_Find(l, index);
        *FIND_NEXTADDR(l, index - 1) = LinkedList_Find(l, index + 1);
        Memory_Free(&LINKEDLIST_HEAP, save);
        l->Nodes_Num--;
    }
}

void LinkedList_Dispose2(linkedlist *l, void *object)
{
    int i;

    for (i = 0; i < l->Nodes_Num; i++)
    {
        if (LinkedList_Find(l, i) == object)
            break;
    }
    LinkedList_Dispose(l, i);
}

void LinkeList_InfoPrint(USART_TypeDef *usart, linkedlist *l)
{
    int i;

    USART_Printf(usart, "\nDatafield size: %d\n", l->DataField_Size);
    USART_Printf(usart, "Node size:       %d\n", l->Node_Size);
    USART_Printf(usart, "Nodes Num:       %d\n", l->Nodes_Num);
    for (i = 0; i < l->Nodes_Num; i++)
        USART_Printf(USART1, "Node%d {Addr:%#x,NextAddr:%#x}\n", i, LinkedList_Find(l, i), *FIND_NEXTADDR(l, i));
}
