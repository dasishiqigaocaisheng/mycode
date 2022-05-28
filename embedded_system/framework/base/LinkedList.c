#include "LinkedList.h"
#include "Memory.h"

#include "USART.h"

#define LINKEDLIST_HEAP heap0

#define GET_NEXT_LINK_REG(l, obj) ((void **)((uint32_t)(obj) + (l)->Node_Size - 8))
#define GET_LAST_LINK_REG(l, obj) ((void **)((uint32_t)(obj)-4))

void *LinkedList_Find(linkedlist *l, int index)
{
    if (!l->Is_Loop)
    {
        if (index < 0 || index >= l->Nodes_Num)
            return NULL;
        void *obj = LinkedList_Get_FirstObject(l);
        for (int i = 0; i < index; i++)
        {
            obj = LinkedList_Get_NextObject(l, obj);
            if (obj == NULL)
                break;
        }
        return obj;
    }
    else
    {
        if (l->Nodes_Num == 0)
            return NULL;
        void *obj = LinkedList_Get_FirstObject(l);
        if (index >= 0)
        {
            for (int i = 0; i < index; i++)
                obj = LinkedList_Get_NextObject(l, obj);
        }
        else
        {
            for (int i = 0; i > index; i--)
                obj = LinkedList_Get_LastObject(l, obj);
        }
        return obj;
    }
}

void *LinkedList_Add(linkedlist *l, int index)
{
    if (!l->Is_Loop)
    {
        if (index < 0 || index > l->Nodes_Num)
            return NULL;

        void *new_obj = (void *)((uint32_t)Memory_Malloc(&LINKEDLIST_HEAP, l->Node_Size) + 4);
        void *next_obj = LinkedList_Find(l, index);
        void *last_obj = LinkedList_Find(l, index - 1);

        *GET_LAST_LINK_REG(l, new_obj) = last_obj;
        *GET_NEXT_LINK_REG(l, new_obj) = next_obj;
        //在非循环链表中，last_obj==NULL意味着在头部添加节点
        //而next_obj==NULL意味着在尾部添加节点
        if (last_obj != NULL)
            *GET_NEXT_LINK_REG(l, last_obj) = new_obj;
        else
            l->Head_Addr = new_obj;
        if (next_obj != NULL)
            *GET_LAST_LINK_REG(l, next_obj) = new_obj;
        l->Nodes_Num++;
        return new_obj;
    }
    else
    {
        void *new_obj = (void *)((uint32_t)Memory_Malloc(&LINKEDLIST_HEAP, l->Node_Size) + 4);
        void *next_obj = LinkedList_Find(l, index);
        void *last_obj = LinkedList_Find(l, index - 1);

        //在循环节点中last_obj==NULL||next_obj==NULL说明原来链表中没有节点，此时new_obj形成自循环
        //如果链表中有任何一个节点，那么last_obj和next_obj都不可能为NULL
        if (last_obj == NULL || next_obj == NULL)
        {
            *GET_LAST_LINK_REG(l, new_obj) = new_obj;
            *GET_NEXT_LINK_REG(l, new_obj) = new_obj;
            l->Head_Addr = new_obj;
        }
        else
        {
            *GET_NEXT_LINK_REG(l, last_obj) = new_obj;
            *GET_LAST_LINK_REG(l, new_obj) = last_obj;
            *GET_NEXT_LINK_REG(l, new_obj) = next_obj;
            *GET_LAST_LINK_REG(l, next_obj) = new_obj;
            //如果在头部添加节点
            if (index == 0)
                l->Head_Addr = new_obj;
        }
        l->Nodes_Num++;
        return new_obj;
    }
}

void LinkedList_Add2(linkedlist *l, int index, void *object)
{
    if (!l->Is_Loop)
    {
        if (index < 0 || index > l->Nodes_Num)
            return;

        void *next_obj = LinkedList_Find(l, index);
        void *last_obj = LinkedList_Find(l, index - 1);

        *GET_LAST_LINK_REG(l, object) = last_obj;
        *GET_NEXT_LINK_REG(l, object) = next_obj;
        if (last_obj != NULL)
            *GET_NEXT_LINK_REG(l, last_obj) = object;
        else
            l->Head_Addr = object;
        if (next_obj != NULL)
            *GET_LAST_LINK_REG(l, next_obj) = object;
    }
    else
    {
        void *next_obj = LinkedList_Find(l, index);
        void *last_obj = LinkedList_Find(l, index - 1);

        if (last_obj == NULL || next_obj == NULL)
        {
            *GET_LAST_LINK_REG(l, object) = object;
            *GET_NEXT_LINK_REG(l, object) = object;
            l->Head_Addr = object;
        }
        else
        {
            *GET_NEXT_LINK_REG(l, last_obj) = object;
            *GET_LAST_LINK_REG(l, object) = last_obj;
            *GET_NEXT_LINK_REG(l, object) = next_obj;
            *GET_LAST_LINK_REG(l, next_obj) = object;
            if (index == 0)
                l->Head_Addr = object;
        }
    }
    l->Nodes_Num++;
}

void *LinkedList_AddBehind(linkedlist *l, void *object)
{
    if (object == NULL)
        return NULL;
    if (!l->Is_Loop)
    {
        void *new_obj = (void *)((uint32_t)Memory_Malloc(&LINKEDLIST_HEAP, l->Node_Size) + 4);
        void *next_obj = LinkedList_Get_NextObject(l, object);

        *GET_LAST_LINK_REG(l, new_obj) = object;
        *GET_NEXT_LINK_REG(l, new_obj) = next_obj;
        *GET_NEXT_LINK_REG(l, object) = new_obj;
        if (next_obj != NULL)
            *GET_LAST_LINK_REG(l, next_obj) = new_obj;
        l->Nodes_Num++;
        return new_obj;
    }
    else
    {
        void *new_obj = (void *)((uint32_t)Memory_Malloc(&LINKEDLIST_HEAP, l->Node_Size) + 4);
        void *next_obj = LinkedList_Get_NextObject(l, object);

        //在循环节点中last_obj==NULL||next_obj==NULL说明原来链表中没有节点，此时new_obj形成自循环
        //如果链表中有任何一个节点，那么last_obj和next_obj都不可能为NULL
        *GET_NEXT_LINK_REG(l, object) = new_obj;
        *GET_LAST_LINK_REG(l, new_obj) = object;
        *GET_NEXT_LINK_REG(l, new_obj) = next_obj;
        *GET_LAST_LINK_REG(l, next_obj) = new_obj;
        l->Nodes_Num++;
        return new_obj;
    }
}

void LinkedList_AddBehind2(linkedlist *l, void *object1, void *object2)
{
    if (object1 == NULL)
        return NULL;
    if (!l->Is_Loop)
    {
        void *next_obj = LinkedList_Get_NextObject(l, object1);

        *GET_LAST_LINK_REG(l, object2) = object1;
        *GET_NEXT_LINK_REG(l, object2) = next_obj;
        *GET_NEXT_LINK_REG(l, object1) = object2;
        if (next_obj != NULL)
            *GET_LAST_LINK_REG(l, next_obj) = object2;
        l->Nodes_Num++;
    }
    else
    {
        void *next_obj = LinkedList_Get_NextObject(l, object1);

        *GET_NEXT_LINK_REG(l, object1) = object2;
        *GET_LAST_LINK_REG(l, object2) = object1;
        *GET_NEXT_LINK_REG(l, object2) = next_obj;
        *GET_LAST_LINK_REG(l, next_obj) = object2;
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
    if (!l->Is_Loop)
    {
        if (index < 0 || index >= l->Nodes_Num)
            return NULL;

        void *save = LinkedList_Find(l, index);
        void *last_obj = LinkedList_Get_LastObject(l, save);
        void *next_obj = LinkedList_Get_NextObject(l, save);

        // last_obj==NULL说明移除了头部
        // next_obj==NULL说明了移除了尾部
        if (last_obj != NULL)
            *GET_NEXT_LINK_REG(l, last_obj) = next_obj;
        else
            l->Head_Addr = next_obj;
        if (next_obj != NULL)
            *GET_LAST_LINK_REG(l, next_obj) = last_obj;
        l->Nodes_Num--;
        return save;
    }
    else
    {
        if (l->Nodes_Num == 0)
            return NULL;

        void *save = LinkedList_Find(l, index);
        void *last_obj = LinkedList_Get_LastObject(l, save);
        void *next_obj = LinkedList_Get_NextObject(l, save);

        *GET_NEXT_LINK_REG(l, last_obj) = next_obj;
        *GET_LAST_LINK_REG(l, next_obj) = last_obj;
        //如果要移除头部
        if (index == 0)
            l->Head_Addr = next_obj;
        //如果last_obj==save||next_obj==save则说明链表中只有一个节点
        //删除该节点后，链表为空
        if (last_obj == save || next_obj == save)
            l->Head_Addr = NULL;
        l->Nodes_Num--;
        return save;
    }
}

void LinkedList_Remove2(linkedlist *l, void *object)
{
    if (object == NULL || l->Nodes_Num == 0)
        return;
    if (!l->Is_Loop)
    {
        void *last_obj = LinkedList_Get_LastObject(l, object);
        void *next_obj = LinkedList_Get_NextObject(l, object);

        if (last_obj != NULL)
            *GET_NEXT_LINK_REG(l, last_obj) = next_obj;
        else
            l->Head_Addr = next_obj;
        if (next_obj != NULL)
            *GET_LAST_LINK_REG(l, next_obj) = last_obj;
        l->Nodes_Num--;
    }
    else
    {
        void *last_obj = LinkedList_Get_LastObject(l, object);
        void *next_obj = LinkedList_Get_NextObject(l, object);

        *GET_NEXT_LINK_REG(l, last_obj) = next_obj;
        *GET_LAST_LINK_REG(l, next_obj) = last_obj;
        if (l->Head_Addr == object)
            l->Head_Addr = next_obj;
        if (last_obj == object || next_obj == object)
            l->Head_Addr = NULL;
        l->Nodes_Num--;
    }
}

void LinkedList_Dispose(linkedlist *l, int index)
{
    if (!l->Is_Loop)
    {
        if (index < 0 || index >= l->Nodes_Num)
            return;

        void *save = LinkedList_Find(l, index);
        void *last_obj = LinkedList_Get_LastObject(l, save);
        void *next_obj = LinkedList_Get_NextObject(l, save);

        // last_obj==NULL说明移除了头部
        // next_obj==NULL说明了移除了尾部
        if (last_obj != NULL)
            *GET_NEXT_LINK_REG(l, last_obj) = next_obj;
        else
            l->Head_Addr = next_obj;
        if (next_obj != NULL)
            *GET_LAST_LINK_REG(l, next_obj) = last_obj;
        l->Nodes_Num--;
        Memory_Free(&LINKEDLIST_HEAP, (void *)((uint32_t)save - 4));
    }
    else
    {
        if (l->Nodes_Num == 0)
            return;

        void *save = LinkedList_Find(l, index);
        void *last_obj = LinkedList_Get_LastObject(l, save);
        void *next_obj = LinkedList_Get_NextObject(l, save);

        *GET_NEXT_LINK_REG(l, last_obj) = next_obj;
        *GET_LAST_LINK_REG(l, next_obj) = last_obj;
        //如果要移除头部
        if (index == 0)
            l->Head_Addr = next_obj;
        //如果last_obj==save||next_obj==save则说明链表中只有一个节点
        //删除该节点后，链表为空
        if (last_obj == save || next_obj == save)
            l->Head_Addr = NULL;
        l->Nodes_Num--;
        Memory_Free(&LINKEDLIST_HEAP, (void *)((uint32_t)save - 4));
    }
}

void LinkedList_Dispose2(linkedlist *l, void *object)
{
    if (object == NULL || l->Nodes_Num == 0)
        return;
    if (!l->Is_Loop)
    {
        void *last_obj = LinkedList_Get_LastObject(l, object);
        void *next_obj = LinkedList_Get_NextObject(l, object);

        if (last_obj != NULL)
            *GET_NEXT_LINK_REG(l, last_obj) = next_obj;
        else
            l->Head_Addr = next_obj;
        if (next_obj != NULL)
            *GET_LAST_LINK_REG(l, next_obj) = last_obj;
        l->Nodes_Num--;
    }
    else
    {
        void *last_obj = LinkedList_Get_LastObject(l, object);
        void *next_obj = LinkedList_Get_NextObject(l, object);

        *GET_NEXT_LINK_REG(l, last_obj) = next_obj;
        *GET_LAST_LINK_REG(l, next_obj) = last_obj;
        if (l->Head_Addr == object)
            l->Head_Addr = next_obj;
        if (last_obj == object || next_obj == object)
            l->Head_Addr = NULL;
        l->Nodes_Num--;
    }
    Memory_Free(&LINKEDLIST_HEAP, (void *)((uint32_t)object - 4));
}

void LinkeList_InfoPrint(USART_TypeDef *usart, linkedlist *l)
{
    USART_Printf(usart, "\nDatafield size: %d\n", l->DataField_Size);
    USART_Printf(usart, "Node size: %d\n", l->Node_Size);
    USART_Printf(usart, "Nodes Num: %d\n", l->Nodes_Num);
    for (int i = 0; i < l->Nodes_Num; i++)
    {
        void *obj = LinkedList_Find(l, i);
        USART_Printf(USART1, "Node%d {LastAddr:%#x,Addr:%#x,NextAddr:%#x}\n", i, *GET_LAST_LINK_REG(l, obj), obj, *GET_NEXT_LINK_REG(l, obj));
    }
    USART_Printf(usart, "\r\n");
}
