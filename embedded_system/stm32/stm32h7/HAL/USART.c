#include "sys.h"
//#include "delay.h"
#include "USART.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

//���ڽ��ճ�ʱ��ֵ
#define USART_TIME_OUT_VALUE	1000000


/*����_USART_Send_Integer��Send_Mode�����ĺ�*/
/*#define USART_INT_FORMAT_DEC 	1	//�з���ʮ����
#define USART_INT_FORMAT_UDEC	2	//�޷���ʮ����
#define USART_INT_FORMAT_HEX 	3	//ʮ������*/

/*�����ʽ�����ת���ַ�����*/
/*#define IO_TYPE_DEC_INT		1	//%d���з���ʮ��������
#define IO_TYPE_HEX_INT		2	//%x��ʮ����������
#define IO_TYPE_DEC_UINT	3	//%u���޷���ʮ��������
#define IO_TYPE_FLOAT		4	//%f��%.xf�������ȸ�����
#define IO_TYPE_STRING		5	//%s���ַ���
#define IO_TYPE_PERCENT		6	//%%���ٷֺ�*/

u8 Time_Out_Enable[3];//USART1/2/3�Ƿ�ʹ�ܳ�ʱ���
char USART_Cache[USART_CACHE_SIZE];//Scanf���ջ���
Usart_IRQHandler USART1_Interrupt_CallbackFunc;
Usart_IRQHandler USART2_Interrupt_CallbackFunc;
Usart_IRQHandler USART3_Interrupt_CallbackFunc;

/*ת���ַ������ṹ��*/
typedef struct 
{
	u8 Parament_Type;	//�ַ�����
	u8 Float_Dec_Num;	//����Ǹ���������ô�ø�����Ҫ��ʾ��С��λ��
} IO_Parament_Type;

/*����һ���ֽ�*/
//void _USART_Send_Byte(USART_TypeDef* USART, u8 DATA);
#if !defined USE_STM32H7
#define _USART_Send_Byte(usart,data)    {usart->DR=data; \
                                        while (((usart->SR>>6)&1)==0);}
#else
#define _USART_Send_Byte(usart,data)    {usart->TDR=data; \
                                        while (((usart->ISR>>6)&1)==0);}
#endif

/*��ʽ������һ���ַ���*/
void _USART_Send_String(USART_TypeDef* USART, char *DATA);

/*����һ���ֽ�*/
u8 _USART_Receive_Byte(USART_TypeDef* USART, u8* Data);

void _USART_Send_String(USART_TypeDef* USART, char *DATA)
{
	u32 i;
	
	for (i=0;DATA[i]!='\0';i++)
        _USART_Send_Byte(USART,((u8*)DATA)[i]);
    
}

void USART_Init(USART_TypeDef* USART, u32 Boundrate)
{
    u32 USARTDIV_Int;
    
    if (USART==USART1)
		USARTDIV_Int=APB2_MAX_CLOCK/Boundrate;
	else if (USART==USART2)
		USARTDIV_Int=APB1_MAX_CLOCK/Boundrate;
	else if (USART==USART3)
		USARTDIV_Int=APB1_MAX_CLOCK/Boundrate;
	if (USART==USART1)
	{
		RCC->AHB1ENR|=1<<0;   		//ʹ��PORTA��ʱ��  
		RCC->APB2ENR|=1<<4;  		//ʹ�ܴ���1ʱ�� 
		GPIO_Set(GPIOA,PIN9|PIN10,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_MID,GPIO_PUPD_PU);//PA9,PA10,���ù���,�������
		GPIO_AF_Set(GPIOA,9,7);		//PA9,AF7
		GPIO_AF_Set(GPIOA,10,7);	//PA10,AF7	 
	}
	else if (USART==USART2)
	{
		RCC->AHB1ENR|=1<<0;   		//ʹ��PORTA��ʱ��  
        #if defined USE_STM32H7
		RCC->APB1LENR|=1<<17;  		//ʹ�ܴ���2ʱ�� 
        #else
        RCC->APB1ENR|=1<<17;  		//ʹ�ܴ���2ʱ�� 
        #endif
        
		GPIO_Set(GPIOA,PIN2|PIN3,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_MID,GPIO_PUPD_PU);//PA2,PA3,���ù���,�������
		GPIO_AF_Set(GPIOA,2,7);		//PA2,AF7
		GPIO_AF_Set(GPIOA,3,7);		//PA3,AF7  	   
	}
	else if (USART==USART3)
	{
		RCC->AHB1ENR|=1<<1;			//ʹ��PORTB��ʱ��  
        #if defined USE_STM32H7
		RCC->APB1LENR|=1<<18;		//ʹ��USART3ʱ��
        #else
        RCC->APB1ENR|=1<<18;		//ʹ��USART3ʱ��
        #endif
		GPIO_Set(GPIOB,PIN10|PIN11,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_MID,GPIO_PUPD_PU);//PB10,PB11,���ù���,�������
		GPIO_AF_Set(GPIOB,10,7);		//PB10,AF7
		GPIO_AF_Set(GPIOB,11,7);		//PB11,AF7 
	}
	
	USART->BRR=USARTDIV_Int; 	//����������	 
	USART->CR1&=~(1<<15); 		//����OVER8=0 
	USART->CR1|=1<<2;  		//���ڽ���ʹ��
	USART->CR1|=1<<3;  		//���ڷ���ʹ��
    USART->CR1|=1;
}

void USART_Time_Out_Set(USART_TypeDef* USART, u8 Enable)
{
	if (USART==USART1)
		Time_Out_Enable[0]=Enable;
	else if (USART==USART2)
		Time_Out_Enable[1]=Enable;
	else if (USART==USART3)
		Time_Out_Enable[2]=Enable;
}

void USART_Interrupt_Set(USART_TypeDef* USART, u8 Group, u8 PrePriority, u8 SubPriority, Usart_IRQHandler CallbackFunc)
{
	if (USART==USART1)
	{
		MY_NVIC_Init(PrePriority,SubPriority,USART1_IRQn,Group);
		USART1_Interrupt_CallbackFunc=CallbackFunc;
	}
	else if (USART==USART2)
	{
		MY_NVIC_Init(PrePriority,SubPriority,USART2_IRQn,Group);
		USART2_Interrupt_CallbackFunc=CallbackFunc;
	}
	else if (USART==USART3)
	{
		MY_NVIC_Init(PrePriority,SubPriority,USART3_IRQn,Group);
		USART3_Interrupt_CallbackFunc=CallbackFunc;
	}
}


	
void USART_Send_Data_Flow(USART_TypeDef* USART, void* DATA, u32 Length)
{
	u32 i;
	
	for (i=0;i<Length;i++)
        _USART_Send_Byte(USART,((u8*)DATA)[i]);
}



void USART_Printf(USART_TypeDef* USART, char* Format_String, ...)
{
	u8 save;
	u32 i;
    va_list vl;
    
    va_start(vl,Format_String);
    i=vsprintf(USART_Cache,Format_String,vl);
    USART_Cache[i]='\0';
    _USART_Send_String(USART,USART_Cache);
    va_end(vl);
}

u8 _USART_Receive_Byte(USART_TypeDef* USART, u8* Data)
{	
	u8 Time_Out;
	u32 i=0;
	
	if (USART==USART1)
		Time_Out=Time_Out_Enable[0];
	else if (USART==USART2)
		Time_Out=Time_Out_Enable[1];
	else if (USART==USART3)
		Time_Out=Time_Out_Enable[2];
	
    #if !defined USE_STM32H7
	if (Time_Out==1)
	{
		while ((((USART->SR>>5)&1)==0)&&(i<USART_TIME_OUT_VALUE))
			i++;
		if (i==USART_TIME_OUT_VALUE)
			return 1;
		else
		{
			*Data=USART->DR;
			return 0;
		}
	}
	else
	{
		while (((USART->SR>>5)&1)==0);
		*Data=USART->DR;
		return 0;
	}
    #else
    if (Time_Out==1)
	{
		while ((((USART->ISR>>5)&1)==0)&&(i<USART_TIME_OUT_VALUE))
			i++;
		if (i==USART_TIME_OUT_VALUE)
			return 1;
		else
		{
			*Data=USART->RDR;
			return 0;
		}
	}
	else
	{
		while (((USART->ISR>>5)&1)==0);
		*Data=USART->RDR;
		return 0;
	}
    #endif
}

int USART_Scanf(USART_TypeDef* USART, char* Format_String, ...)
{
    int len;
    va_list vl;
    
    va_start(vl,Format_String);
    len=USART_Receive_Data_Flow_EndwithCharacter(USART,USART_Cache,0x0a,1);
    USART_Cache[len]='\0';
    len=vsscanf(USART_Cache,Format_String,vl);
    va_end(vl);
    
    return len;
}

u8 USART_Receive_Data_Flow_EndwithLength(USART_TypeDef* USART, void *DATA, u32 Length)
{
	u8 time_out;
	u32 i;

	for (i=0;i<Length;i++)
	{
		time_out=_USART_Receive_Byte(USART,&((u8*)DATA)[i]);
		if (time_out==1)
			return 0;
	}
	return Length;
}

u32 USART_Receive_Data_Flow_EndwithCharacter(USART_TypeDef* USART, void *DATA, char END, u8 Repeat_Time)
{
	u8 j,time_out;
	u32 i;

	i=0;
	j=0;
	do
	{
		time_out=_USART_Receive_Byte(USART,&((u8*)DATA)[i]);
		if (time_out==1)
			return 0;
		if (((u8*)DATA)[i]==END)//��⵽�����ַ�
			j++;
		else		//��⵽�ǽ����ַ�
			j=0;	//�ۼ�������
		i++;
	} while (j!=Repeat_Time);
	
	return i;
}

void USART1_IRQHandler(void)
{
	if (USART_Get_RXNE(USART1))
		USART1_Interrupt_CallbackFunc();
	USART_Clear_RXNE(USART1);
}

void USART2_IRQHandler(void)
{
	if (USART_Get_RXNE(USART2))
		USART2_Interrupt_CallbackFunc();
	USART_Clear_RXNE(USART2);
}

void USART3_IRQHandler(void)
{
	if (USART_Get_RXNE(USART3))
		USART3_Interrupt_CallbackFunc();
	USART_Clear_RXNE(USART3);
}

#ifdef USART_DEBUG
void USART_Test(USART_TypeDef* USART)
{
	u8 i,Length;
	u8 Rec_Buf[16];
	
	USART_Printf(USART,"\n/************���ڲ��Կ�ʼ************/\n");
	USART_Printf(USART,"����ʮ��������-20120712��%d\n",-20120712);
	USART_Printf(USART,"����ʮ����������0x012abcf9��%x\n",0x012abcf9);
	USART_Printf(USART,"���͸�����2012.0712��%.4f\n",2012.0712f);
	USART_Printf(USART,"�����ַ�����USART Test.����%s\n","USART Test.");
	USART_Printf(USART,"���ڿ�ʼ���ղ���\n���ڷ���4���ֽ�....");
	USART_Receive_Data_Flow_EndwithLength(USART,Rec_Buf,4);
	USART_Printf(USART,"�������\n");
	USART_Printf(USART,"���յ����ݣ�");
	for (i=0;i<4;i++)
		USART_Printf(USART,"%x  ",Rec_Buf[i]);
	USART_Printf(USART,"\n���ڷ���������0x12��β��������....\n");
	Length=USART_Receive_Data_Flow_EndwithCharacter(USART,Rec_Buf,0x12,2);
	USART_Printf(USART,"�������\n");
	USART_Printf(USART,"���յ����ݣ�");
	for (i=0;i<Length;i++)
		USART_Printf(USART,"%x  ",Rec_Buf[i]);
	USART_Printf(USART,"\n����0xff�Խ�������\n");
	do
    {
        _USART_Receive_Byte(USART,&i);
    }while (i!=0xff);
	USART_Printf(USART,"/************���ڲ��Խ���************/\n");
}
#endif
