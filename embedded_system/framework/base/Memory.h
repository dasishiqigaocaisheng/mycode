/*
内存管理库
本库最多提供5个可用的堆空间，即：HEAP0、HEAP1、HEAP2、HEAP3、HEAP4
这五个堆分别对应于指定的内存区域
    HEAP0：
        F4/F1/F7/H7：0x20000000开始的IRAM1，最大64KB，具体位置由编译器自动分配
        对于H7这部分内存只有CPU和MDMA可访问
    HEAP1：
        F4：0x10000000开始的IRAM2，最大48KB，首地址0x10000000，这部分内存DMA无法到达
        F7/H7：0x38000000开始的SRAM4，最大64KB，首地址0x38000000，任何DMA都可以到达
    HEAP2：由FSMC或FMC控制的外部存储区，位置由用户定义
    HEAP3：由FSMC或FMC控制的外部存储区，位置由用户定义
    HEAP4：由FSMC或FMC控制的外部存储区，位置由用户定义

    此外还有一个基本堆空间，即HEAP5，它对用户不开放且在内存初始化时自动被建立
    HEAP5：
        F4：0x10000000开始的IRAM2，固定16KB，起始地址0x0x1000c000，也就是IRAM2的最后16KB
           它被用来存放所有6个堆的内存表
        F7/H7：0x38000000开始的SRAM4，固定32KB大小，起始地址0x38008000，即SRAM4的最后32KB，它被用来存放所有6个堆的内存表
*/

#ifndef _MEMORY_H_
#define _MEMORY_H_
#include "sys.h"

#define MEMORY_DEBUG

#define USE_IRAM1
#define USE_IRAM2

#define NULL 0

/*ISRAM1堆中内存块的数量（每个块默认是256B大小）
内存块的数量没有要求，但块大小应是2的整数次幂*/
#define HEAP0_BLOCK_NUMBER 256 // 64KB
//#define HEAP0_BLOCK_NUMBER 192	//48KB
//#define HEAP0_BLOCK_NUMBER 128	//32KB
//#define HEAP0_BLOCK_NUMBER 64		//16KB

/*ISRAM2堆中内存块的数量（每个块默认是256B大小）
内存块的数量没有要求，但块大小应是2的整数次幂*/
#define HEAP1_BLOCK_NUMBER 256 // 64KB
//#define HEAP1_BLOCK_NUMBER 192	//48KB
//#define HEAP1_BLOCK_NUMBER 128	//32KB
//#define HEAP1_BLOCK_NUMBER 64		//16KB

typedef enum
{
    HeapDir_Normal,
    HeapDir_Reverse
} heap_dir_type;

typedef struct
{
    status_flag Is_Free;
    uint16_t MemBlock_Size;
} heap_node;

typedef struct
{
    vro heap_dir_type Dirction;
    vro uint16_t BlockSize;
    vro uint16_t BlockNum;
    heap_node *Node_List;
    vro void* ro Addr;
} heap;

extern heap heap0,heap1,heap2,heap3,heap4,heap5;

#define Memory_Malloc_Size(h,addr) 	((h)->Dirction==HeapDir_Normal? \
									(h)->Node_List[((uint32_t)(addr)-(uint32_t)(h)->Addr)/(h)->BlockSize].MemBlock_Size: \
									(h)->Node_List[(h)->BlockNum-1-((uint32_t)(addr)-(uint32_t)(h)->Addr)/(h)->BlockSize].MemBlock_Size)

void Memory_Init(void);
void Memory_Heap_Init(heap *h, heap_dir_type dir, uint16_t blk_size, uint16_t blk_num, heap_node *nodes, void *addr);
void *Memory_Malloc(heap *h, uint32_t size);
void Memory_Free(heap *h, void *addr);
float Memory_Heap_Utilization_Ratio(heap* h);
void Memory_Print_Heap_Info(heap* h);


#endif
