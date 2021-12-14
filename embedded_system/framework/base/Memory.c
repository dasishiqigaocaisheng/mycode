#include "Memory.h"
#include <string.h>
#include "Framework.h"

#ifdef MEMORY_DEBUG
#include "USART.h"
#endif

#define HEAP0_BLOCK_SIZE 256	//HEAP0���С
#define HEAP1_BLOCK_SIZE 256	//HEAP1���С

#ifdef USE_IRAM1
u8 IRAM1_Heap[HEAP0_BLOCK_NUMBER*HEAP0_BLOCK_SIZE] __ALIGNED(4);	//�������Զ�����ѿռ�
#endif

#if !defined USE_STM32H7
#define IRAM2_HEAP ((void*)0x10000000)	//HEAP1�׵�ַ
#else   
//u8 IRAM2_HEAP[HEAP1_BLOCK_NUMBER*HEAP1_BLOCK_SIZE] __attribute__((section(".ARM.__at_0x38000000")));
u8 IRAM2_HEAP[HEAP1_BLOCK_NUMBER*HEAP1_BLOCK_SIZE] __attribute__((section(".ARM.__at_0x24000000")));
#endif

#define HEAP5_BLOCK_SIZE   64	//HEAP5���С
#define HEAP5_BLOCK_NUMBER 512	//HEAP5������

#if !defined USE_STM32H7
#define HEAP5_ADDRESS      ((void*)0x1000c000)	//HEAP5�׵�ַ
#else
#define HEAP5_ADDRESS      ((void*)0x38008000)	//HEAP5�׵�ַ
#endif
#define HEAP5_SIZE		   32768				//HEAP5�ܴ�С

//����Ϣ�ṹ��
struct Heap_Info
{
	void* Address;
	u16 Block_Number;
	u16 Block_Size;
	struct block_Info* Block_List;
} Custom_Heap_List[6]={{NULL,0,0,NULL},{NULL,0,0,NULL},{NULL,0,0,NULL},{NULL,0,0,NULL},{NULL,0,0,NULL},{NULL,0,0,NULL}};

//�ڴ����Ϣ�ṹ��
struct block_Info
{
	u16 Head_Address;//���������Ѿ��������ˣ���������÷��������ڱ��е���ʼ��ַ
	u16 Area_Size;//����������ķ�������Ĵ�С���Կ�Ϊ��λ����Ϊ0���������û�б�����
};

/*�ڴ����*/
void* _Malloc(u32 Size, struct Heap_Info* Heap);

/*�ڴ��ͷ�*/
void _Free(void* Address, struct Heap_Info* Heap);


void* _Malloc(u32 Size, struct Heap_Info* Heap)
{
	int Blocks_Needed;
	int i,j;
	
	if (Size%Heap->Block_Size==0)
		Blocks_Needed=Size/Heap->Block_Size;
	else
		Blocks_Needed=Size/Heap->Block_Size+1;
	
	for (i=0;i<Heap->Block_Number;i++)//�ӵ�һ���鿪ʼ����
	{
		if (Heap->Block_List[i].Area_Size==0)//�ҵ�һ���տ�
		{
			int save=i;//��¼�����λ��
			for (j=0;j<Blocks_Needed;j++,i++)//����������Ҫ����Ŀ�
			{
				if (Heap->Block_Number-save<Blocks_Needed)//˵��û���㹻�Ŀ�ʣ��
					return NULL;
				if (Heap->Block_List[i].Area_Size!=0)//�п鱻ʹ��
				{
					i+=Heap->Block_List[i].Area_Size-1;//i��λ����ǰ����������Ľ�β,-1�����
					break;
				}
			}
			if (j==Blocks_Needed)//˵�����ں��ʴ�С�Ŀ�������
			{
				for (j=0,i=save;j<Blocks_Needed;j++,i++)//����Щ����б��
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
	//Heap5�ڴ��ʼ��
	memset(HEAP5_ADDRESS,0,HEAP5_SIZE);
	
	//���ȳ�ʼ��Heap5����Ϊ������ڴ��
	Custom_Heap_List[5].Address=HEAP5_ADDRESS;
	Custom_Heap_List[5].Block_Number=HEAP5_BLOCK_NUMBER;
	Custom_Heap_List[5].Block_Size=HEAP5_BLOCK_SIZE;
	Custom_Heap_List[5].Block_List=HEAP5_ADDRESS;
	_Malloc(Custom_Heap_List[5].Block_Number*sizeof(struct block_Info),&Custom_Heap_List[5]);
	
	
	#ifdef USE_IRAM1
	//��ʼ��Heap0����Ϊ������ڴ��
	Custom_Heap_List[0].Address=IRAM1_Heap;
	Custom_Heap_List[0].Block_Number=HEAP0_BLOCK_NUMBER;
	Custom_Heap_List[0].Block_Size=HEAP0_BLOCK_SIZE;
	Custom_Heap_List[0].Block_List=_Malloc(Custom_Heap_List[0].Block_Number*sizeof(struct block_Info),&Custom_Heap_List[5]);
	#endif
	
	#ifdef USE_IRAM2
	//��ʼ��Heap1����Ϊ������ڴ��
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
