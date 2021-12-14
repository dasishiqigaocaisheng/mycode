/*
�ڴ�����
��������ṩ5�����õĶѿռ䣬����HEAP0��HEAP1��HEAP2��HEAP3��HEAP4
������ѷֱ��Ӧ��ָ�����ڴ�����
    HEAP0��
        F4/F1/F7/H7��0x20000000��ʼ��IRAM1�����64KB������λ���ɱ������Զ�����
        ����H7�ⲿ���ڴ�ֻ��CPU��MDMA�ɷ���
	HEAP1��
        F4��0x10000000��ʼ��IRAM2�����48KB���׵�ַ0x10000000���ⲿ���ڴ�DMA�޷�����
        F7/H7��0x38000000��ʼ��SRAM4�����64KB���׵�ַ0x38000000���κ�DMA�����Ե���
    HEAP2����FSMC��FMC���Ƶ��ⲿ�洢����λ�����û�����
	HEAP3����FSMC��FMC���Ƶ��ⲿ�洢����λ�����û�����
	HEAP4����FSMC��FMC���Ƶ��ⲿ�洢����λ�����û�����
	
	���⻹��һ�������ѿռ䣬��HEAP5�������û������������ڴ��ʼ��ʱ�Զ�������
	HEAP5��
        F4��0x10000000��ʼ��IRAM2���̶�16KB����ʼ��ַ0x0x1000c000��Ҳ����IRAM2�����16KB
		   ���������������6���ѵ��ڴ��
        F7/H7��0x38000000��ʼ��SRAM4���̶�32KB��С����ʼ��ַ0x38008000����SRAM4�����32KB�����������������6���ѵ��ڴ��
*/
		   
#ifndef _MEMORY_H_
#define _MEMORY_H_
#include "sys.h"

#define MEMORY_DEBUG

#define USE_IRAM1	
#define USE_IRAM2	

#define NULL 0

/*ISRAM1�����ڴ���������ÿ����Ĭ����256B��С��
�ڴ�������û��Ҫ�󣬵����СӦ��2����������*/
#define HEAP0_BLOCK_NUMBER  256	//64KB
//#define HEAP0_BLOCK_NUMBER 192	//48KB
//#define HEAP0_BLOCK_NUMBER 128	//32KB
//#define HEAP0_BLOCK_NUMBER 64		//16KB

/*ISRAM2�����ڴ���������ÿ����Ĭ����256B��С��
�ڴ�������û��Ҫ�󣬵����СӦ��2����������*/
//#define HEAP1_BLOCK_NUMBER 256	//64KB
#define HEAP1_BLOCK_NUMBER 192	//48KB
//#define HEAP1_BLOCK_NUMBER 128	//32KB
//#define HEAP1_BLOCK_NUMBER 64		//16KB

typedef enum
{
    HEAP0=0,  //��0����ţ�Ҳ����IRAM1��
    HEAP1=1,  //��1����ţ�Ҳ����IRAM2��
    HEAP2=2,  //��2���ⲿ�洢
    HEAP3=3,  //��3���ⲿ�洢
    HEAP4=4   //��4���ⲿ�洢
} heap_type;

/*�ڴ��ʼ��
��ʹ���ڴ����ǰ�������һ��*/
void Memory_Init(void);

/*�ⲿ�ѳ�ʼ��
��ʼ���ⲿ�ѿռ䣬Ҳ����HEAP2��HEAP3��HEAP4*/
float Extern_Heap_Init(heap_type heap, void* Address, u32 Block_Size, u32 Block_Number);

/*����һ���ڴ�
������Size��Ҫ����Ĵ�С��Byte��*/
void* Malloc(heap_type heap, u32 Size);

/*�ͷ�һ���ڴ�
������Source��ָ��Ҫ�ͷ��ڴ���׵�ַ*/
void Free(heap_type heap, void* Address);

/*��ȡ��ǰ�ڴ�ʹ����
����ֵ�������ʣ���ʹ�õĿ�����/ȫ����������*/
float Heap_Utilization_Ratio(heap_type heap);

/*����һ���Ѿ���������ڴ�飬�������ڴ����䵽���ڴ�ʵ�ʴ�С
������Heap_Order�������
	  Address���ڴ���ַ
����ֵ��ʵ�ʴ�С���ֽڣ�*/
u32 Malloc_Size(heap_type heap, void* Address);

u16 Memory_Block_Size(heap_type heap);

void Memory_Print_Block_Number(heap_type heap, USART_TypeDef* USART, void* Address);

#endif
