#include "Memory.h"
#include <string.h>
#include "Framework.h"

#ifdef MEMORY_DEBUG
#include "USART.h"
#endif

#define HEAP0_BLOCK_SIZE 256	//HEAP0块大小
#define HEAP1_BLOCK_SIZE 256	//HEAP1块大小

#ifdef USE_IRAM1
u8 IRAM1_Heap[HEAP0_BLOCK_NUMBER*HEAP0_BLOCK_SIZE] __ALIGNED(4);	//编译器自动分配堆空间
#endif

#if !defined USE_STM32H7
#define IRAM2_HEAP ((void*)0x10000000)	//HEAP1首地址
#else   
//u8 IRAM2_HEAP[HEAP1_BLOCK_NUMBER*HEAP1_BLOCK_SIZE] __attribute__((section(".ARM.__at_0x38000000")));
u8 IRAM2_HEAP[HEAP1_BLOCK_NUMBER*HEAP1_BLOCK_SIZE] __attribute__((section(".ARM.__at_0x24000000")));
#endif

#define HEAP5_BLOCK_SIZE   64	//HEAP5块大小
#define HEAP5_BLOCK_NUMBER 512	//HEAP5块数量

#if !defined USE_STM32H7
#define HEAP5_ADDRESS      ((void*)0x1000c000)	//HEAP5首地址
#else
#define HEAP5_ADDRESS      ((void*)0x38008000)	//HEAP5首地址
#endif
#define HEAP5_SIZE		   32768				//HEAP5总大小

//堆信息结构体
struct Heap_Info
{
	void* Address;
	u16 Block_Number;
	u16 Block_Size;
	struct block_Info* Block_List;
} Custom_Heap_List[6]={{NULL,0,0,NULL},{NULL,0,0,NULL},{NULL,0,0,NULL},{NULL,0,0,NULL},{NULL,0,0,NULL},{NULL,0,0,NULL}};

//内存块信息结构体
struct block_Info
{
	u16 Head_Address;//如果这个块已经被分配了，则该项代表该分配区域在表中的起始地址
	u16 Area_Size;//这个块所属的分配区域的大小（以块为单位），为0代表这个块没有被分配
};

/*内存分配*/
void* _Malloc(u32 Size, struct Heap_Info* Heap);

/*内存释放*/
void _Free(void* Address, struct Heap_Info* Heap);


void* _Malloc(u32 Size, struct Heap_Info* Heap)
{
	int Blocks_Needed;
	int i,j;
	
	if (Size%Heap->Block_Size==0)
		Blocks_Needed=Size/Heap->Block_Size;
	else
		Blocks_Needed=Size/Heap->Block_Size+1;
	
	for (i=0;i<Heap->Block_Number;i++)//从第一个块开始搜索
	{
		if (Heap->Block_List[i].Area_Size==0)//找到一个空块
		{
			int save=i;//记录下这个位置
			for (j=0;j<Blocks_Needed;j++,i++)//搜索所有需要分配的块
			{
				if (Heap->Block_Number-save<Blocks_Needed)//说明没用足够的块剩下
					return NULL;
				if (Heap->Block_List[i].Area_Size!=0)//有块被使用
				{
					i+=Heap->Block_List[i].Area_Size-1;//i定位到当前被分配区域的结尾,-1防溢出
					break;
				}
			}
			if (j==Blocks_Needed)//说明存在合适大小的空闲区域
			{
				for (j=0,i=save;j<Blocks_Needed;j++,i++)//对这些块进行标记
				{
					Heap->Block_List[i].Area_Size=Blocks_Needed;
					Heap->Block_List[i].Head_Address=save;
				}
				
				return (void*)((u32)Heap->Address+save*Heap->Block_Size);
			}
		}
	}
	
	return NULL;
}

void _Free(void* Address, struct Heap_Info* Heap)
{
	int i;
	u32 Block_Address;
	
	if (Address!=NULL)
	{
		Block_Address=(u32)((u8*)Address-(u8*)Heap->Address)/Heap->Block_Size;
		
		for (i=Block_Address;Heap->Block_List[i].Head_Address==Block_Address;i++)
		{
			if (i==Heap->Block_Number)
				break;
			Heap->Block_List[i].Area_Size=0;
			Heap->Block_List[i].Head_Address=0;
		}
	}
}

void Memory_Init(void)
{
	//Heap5内存初始化
	memset(HEAP5_ADDRESS,0,HEAP5_SIZE);
	
	//首先初始化Heap5，并为其分配内存表
	Custom_Heap_List[5].Address=HEAP5_ADDRESS;
	Custom_Heap_List[5].Block_Number=HEAP5_BLOCK_NUMBER;
	Custom_Heap_List[5].Block_Size=HEAP5_BLOCK_SIZE;
	Custom_Heap_List[5].Block_List=HEAP5_ADDRESS;
	_Malloc(Custom_Heap_List[5].Block_Number*sizeof(struct block_Info),&Custom_Heap_List[5]);
	
	
	#ifdef USE_IRAM1
	//初始化Heap0，并为其分配内存表
	Custom_Heap_List[0].Address=IRAM1_Heap;
	Custom_Heap_List[0].Block_Number=HEAP0_BLOCK_NUMBER;
	Custom_Heap_List[0].Block_Size=HEAP0_BLOCK_SIZE;
	Custom_Heap_List[0].Block_List=_Malloc(Custom_Heap_List[0].Block_Number*sizeof(struct block_Info),&Custom_Heap_List[5]);
	#endif
	
	#ifdef USE_IRAM2
	//初始化Heap1，并为其分配内存表
	Custom_Heap_List[1].Address=IRAM2_HEAP;
	Custom_Heap_List[1].Block_Number=HEAP1_BLOCK_NUMBER;
	Custom_Heap_List[1].Block_Size=HEAP1_BLOCK_SIZE;
	Custom_Heap_List[1].Block_List=_Malloc(Custom_Heap_List[1].Block_Number*sizeof(struct block_Info),&Custom_Heap_List[5]);
	#endif
}

float Extern_Heap_Init(heap_type heap, void* Address, u32 Block_Size, u32 Block_Number)
{
	Custom_Heap_List[heap].Address=(void*)Address;
	Custom_Heap_List[heap].Block_Number=Block_Number;
	Custom_Heap_List[heap].Block_Size=Block_Size;
	Custom_Heap_List[heap].Block_List=_Malloc(Custom_Heap_List[heap].Block_Number*sizeof(struct block_Info),&Custom_Heap_List[5]);
	
	return Heap_Utilization_Ratio(5);
}

void* Malloc(heap_type heap, u32 Size)
{
	return _Malloc(Size,&Custom_Heap_List[heap]);
}

void Free(heap_type heap, void* Address)
{
	_Free(Address,&Custom_Heap_List[heap]);
}

float Heap_Utilization_Ratio(heap_type heap)
{
	int i;
	float Used=0;
	
	for (i=0;i<Custom_Heap_List[heap].Block_Number;i++)
	{
		if (Custom_Heap_List[heap].Block_List[i].Area_Size!=0)
			Used++;
	}
	
	return Used/Custom_Heap_List[heap].Block_Number;
}
	
u32 Malloc_Size(heap_type heap, void* Address)
{
	u32 Block_Address;
	
	if (Address==NULL)
		return 0;
	Block_Address=(u32)((u8*)Address-(u8*)Custom_Heap_List[heap].Address)/Custom_Heap_List[heap].Block_Size;
	
	return Custom_Heap_List[heap].Block_List[Block_Address].Area_Size*Custom_Heap_List[heap].Block_Size;
}

u16 Memory_Block_Size(heap_type heap)
{
	return Custom_Heap_List[heap].Block_Size;
}

void Memory_Print_Block_Number(heap_type heap, USART_TypeDef* USART, void* Address)
{
	int i,Block_Address;
	
	if (Address==NULL)
		return;
	Block_Address=(u32)((u8*)Address-(u8*)Custom_Heap_List[heap].Address)/Custom_Heap_List[heap].Block_Size;
	for (i=0;i<Custom_Heap_List[heap].Block_List[Block_Address].Area_Size;i++)
		USART_Printf(USART1,"%d ",Custom_Heap_List[heap].Block_List[Block_Address].Head_Address+i);
	USART_Printf(USART1,"\n");
}
