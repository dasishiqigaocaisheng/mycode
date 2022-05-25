#include "Memory.h"
#include <string.h>
#include "Framework.h"

#ifdef MEMORY_DEBUG
#include "USART.h"
#endif

#define HEAP0_BLOCK_SIZE 256 // HEAP0块大小
#define HEAP1_BLOCK_SIZE 256 // HEAP1块大小

#ifdef USE_IRAM1
// u8 IRAM1_Heap[HEAP0_BLOCK_NUMBER*HEAP0_BLOCK_SIZE] __ALIGNED(4);	//编译器自动分配堆空间
uint8_t *IRAM1_Heap = (uint8_t *)0x20000000;
#endif

#if !defined USE_STM32H7
#define IRAM2_HEAP ((void *)0x10000000) // HEAP1首地址
#else
// u8 IRAM2_HEAP[HEAP1_BLOCK_NUMBER*HEAP1_BLOCK_SIZE] __attribute__((section(".ARM.__at_0x38000000")));
// u8 IRAM2_HEAP[HEAP1_BLOCK_NUMBER * HEAP1_BLOCK_SIZE] __attribute__((section(".ARM.__at_0x24000000")));
uint8_t *IRAM2_Heap = (uint8_t *)0x24000000;
#endif

#define HEAP5_BLOCK_SIZE 4		// HEAP5块大小
#define HEAP5_BLOCK_NUMBER 4096 // HEAP5块数量

#if !defined USE_STM32H7
#define HEAP5_ADDRESS ((void *)0x1000c000) // HEAP5首地址
#else
#define HEAP5_ADDRESS ((void *)0x38008000) // HEAP5首地址
#endif
#define HEAP5_SIZE 32768 // HEAP5总大小

heap heap0, heap1, heap2, heap3, heap4, heap5;

void Memory_Init(void)
{
	//首先初始化heap5，块大小：4，块数量：4096，这样节点列表+堆的总大小为32KB
	Memory_Heap_Init(&heap5, HeapDir_Normal, HEAP5_BLOCK_SIZE, HEAP5_BLOCK_NUMBER, HEAP5_ADDRESS, (void *)((uint32_t)HEAP5_ADDRESS + HEAP5_SIZE / 2));

#ifdef USE_IRAM1
	Memory_Heap_Init(&heap0, HeapDir_Normal, HEAP0_BLOCK_SIZE, HEAP0_BLOCK_NUMBER,
					 Memory_Malloc(&heap5, sizeof(heap_node) * HEAP0_BLOCK_NUMBER), IRAM1_Heap);
#endif

#ifdef USE_IRAM2
	Memory_Heap_Init(&heap1, HeapDir_Normal, HEAP1_BLOCK_SIZE, HEAP1_BLOCK_NUMBER,
					 Memory_Malloc(&heap5, sizeof(heap_node) * HEAP1_BLOCK_NUMBER), IRAM2_Heap);
#endif
}

void Memory_Heap_Init(heap *h, heap_dir_type dir, uint16_t blk_size, uint16_t blk_num, heap_node *nodes, void *addr)
{
	h->Node_List = nodes;
	_RO_WRITE(h->Dirction, heap_dir_type, dir);
	_RO_WRITE(h->BlockSize, uint16_t, blk_size);
	_RO_WRITE(h->BlockNum, uint16_t, blk_num);
	_RO_WRITE(h->Addr, void *, addr);

	for (int i = 0; i < blk_num; i++)
		nodes[i].Is_Free = Enable;
	if (dir == HeapDir_Normal)
		nodes[0].MemBlock_Size = blk_num;
	else if (dir == HeapDir_Reverse)
		nodes[blk_num - 1].MemBlock_Size = blk_num;
	else
		return;
}

void *Memory_Malloc(heap *h, uint32_t size)
{
	if (size == 0)
		return NULL;

	int idx, need_blk_num = size / h->BlockSize; //计算分配size空间最少需要的块数

	if (size % h->BlockSize != 0)
		need_blk_num++;
	idx = 0; //从第一个块（也是第一个帧）开始搜索
	while (idx < h->BlockNum)
	{
		//如果帧是空闲
		if (h->Node_List[idx].Is_Free == Enable)
		{
			//如果帧大小足够
			if (h->Node_List[idx].MemBlock_Size >= need_blk_num)
			{
				h->Node_List[idx].Is_Free = Disable;
				//如果将该帧没有被完全分配，那么需要将帧变为两段
				if (need_blk_num != h->Node_List[idx].MemBlock_Size)
				{
					h->Node_List[idx + need_blk_num].Is_Free = Enable;
					h->Node_List[idx + need_blk_num].MemBlock_Size = h->Node_List[idx].MemBlock_Size - need_blk_num;
				}
				h->Node_List[idx].MemBlock_Size = need_blk_num;

				USART_Printf(USART1, "%#x\r\n", (uint32_t)h->Addr + idx * h->BlockSize);

				//如果增长方向为反向，需要重新进行地址映射
				if (h->Dirction == HeapDir_Normal)
					return (void *)((uint32_t)h->Addr + idx * h->BlockSize);
				else
					return (void *)((uint32_t)h->Addr + (h->BlockNum - 1 - idx - need_blk_num - 1) * h->BlockSize);
			}
		}
		idx += h->Node_List[idx].MemBlock_Size;
	}
	return NULL;
}

void Memory_Free(heap *h, void *addr)
{
	if (addr == NULL)
		return;

	int idx = ((uint32_t)addr - (uint32_t)h->Addr) / h->BlockSize; //计算帧的块地址
	uint32_t blk_num = h->Node_List[idx].MemBlock_Size;			   //帧的块大小

	//如果不是最后一帧，则需要检查下一帧是否是空闲
	//如果是则要将两帧合并
	if (idx + blk_num != h->BlockNum)
	{
		if (h->Node_List[idx + blk_num].Is_Free == Enable)
			h->Node_List[idx].MemBlock_Size += h->Node_List[idx + blk_num].MemBlock_Size;
	}
	h->Node_List[idx].Is_Free = Enable;
}

float Memory_Heap_Utilization_Ratio(heap *h)
{
	int idx = 0, used_block = 0;

	while (idx < h->BlockNum)
	{
		if (h->Node_List[idx].Is_Free == Disable)
			used_block += h->Node_List[idx].MemBlock_Size;
		idx += h->Node_List[idx].MemBlock_Size;
	}
	return (float)used_block / h->BlockNum;
}

void Memory_Print_Heap_Info(heap *h)
{
	USART_Printf(USART1, "Heap Properties:\r\n");
	USART_Printf(USART1, "--Address: %#x\r\n", h->Addr);
	USART_Printf(USART1, "--Increase Direction: ");
	if (h->Dirction == HeapDir_Normal)
		USART_Printf(USART1, "Normal\r\n");
	else if (h->Dirction == HeapDir_Reverse)
		USART_Printf(USART1, "Reverse\r\n");
	USART_Printf(USART1, "--Block Size: %d\r\n", h->BlockSize);
	USART_Printf(USART1, "--Block Number: %d\r\n", h->BlockNum);
	USART_Printf(USART1, "--Node List Address: %#x\r\n", h->Node_List);
	USART_Printf(USART1, "--Utilization Ratio: %.3f%%\r\n", Memory_Heap_Utilization_Ratio(h) * 100);

	USART_Printf(USART1, "Heap Nodes Info:\r\n");
	int idx = 0, used_block_num = 0;
	while (idx < h->BlockNum)
	{
		if (h->Node_List[idx].Is_Free == Disable)
		{
			used_block_num++;
			USART_Printf(USART1, "--Frame%d: idx=%d len=%d size=%d\r\n",
						 used_block_num, idx, h->Node_List[idx].MemBlock_Size, h->Node_List[idx].MemBlock_Size * h->BlockSize);
		}
		idx += h->Node_List[idx].MemBlock_Size;
	}
	if (used_block_num == 0)
		USART_Printf(USART1, "--No Block Used\r\n");
	USART_Printf(USART1, "\r\n");
}
