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
#define HEAP0_BLOCK_NUMBER  256	//64KB
//#define HEAP0_BLOCK_NUMBER 192	//48KB
//#define HEAP0_BLOCK_NUMBER 128	//32KB
//#define HEAP0_BLOCK_NUMBER 64		//16KB

/*ISRAM2堆中内存块的数量（每个块默认是256B大小）
内存块的数量没有要求，但块大小应是2的整数次幂*/
//#define HEAP1_BLOCK_NUMBER 256	//64KB
#define HEAP1_BLOCK_NUMBER 192	//48KB
//#define HEAP1_BLOCK_NUMBER 128	//32KB
//#define HEAP1_BLOCK_NUMBER 64		//16KB

typedef enum
{
    HEAP0=0,  //堆0的序号，也就是IRAM1区
    HEAP1=1,  //堆1的序号，也就是IRAM2区
    HEAP2=2,  //堆2，外部存储
    HEAP3=3,  //堆3，外部存储
    HEAP4=4   //堆4，外部存储
} heap_type;

/*内存初始化
在使用内存管理前必须调用一次*/
void Memory_Init(void);

/*外部堆初始化
初始化外部堆空间，也就是HEAP2、HEAP3、HEAP4*/
float Extern_Heap_Init(heap_type heap, void* Address, u32 Block_Size, u32 Block_Number);

/*申请一块内存
参数：Size：要申请的大小（Byte）*/
void* Malloc(heap_type heap, u32 Size);

/*释放一块内存
参数：Source：指向要释放内存的首地址*/
void Free(heap_type heap, void* Address);

/*获取当前内存使用率
返回值：利用率（已使用的块数量/全部块数量）*/
float Heap_Utilization_Ratio(heap_type heap);

/*对于一个已经被分配的内存块，给出该内存块分配到的内存实际大小
参数：Heap_Order：堆序号
	  Address：内存块地址
返回值：实际大小（字节）*/
u32 Malloc_Size(heap_type heap, void* Address);

u16 Memory_Block_Size(heap_type heap);

void Memory_Print_Block_Number(heap_type heap, USART_TypeDef* USART, void* Address);

#endif
