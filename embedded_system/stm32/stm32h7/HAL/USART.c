#include "sys.h"
#include "GPIO.h"
#include "USART.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

//串口接收超时阈值
#define USART_TIME_OUT_VALUE 1000000

char *STATUS_FLAG_TRUE = "true";
char *STATUS_FLAG_FALSE = "false";

u8 Time_Out_Enable[3];				// USART1/2/3是否使能超时检测
char USART_Cache[USART_CACHE_SIZE]; // Scanf接收缓存
char USART_Cache2[USART_CACHE_SIZE];
Usart_IRQHandler USART1_Interrupt_CallbackFunc;
Usart_IRQHandler USART2_Interrupt_CallbackFunc;
Usart_IRQHandler USART3_Interrupt_CallbackFunc;

/*转义字符参数结构体*/
typedef struct
{
	u8 Parament_Type; //字符类型
	u8 Float_Dec_Num; //如果是浮点数，那么该浮点数要显示的小数位数
} IO_Parament_Type;

/*发送一个字节*/
// void _USART_Send_Byte(USART_TypeDef* USART, u8 DATA);
#if !defined USE_STM32H7
#define _USART_Send_Byte(usart, data)       \
	{                                       \
		usart->DR = data;                   \
		while (((usart->SR >> 6) & 1) == 0) \
			;                               \
	}
#else
#define _USART_Send_Byte(usart, data)        \
	{                                        \
		usart->TDR = data;                   \
		while (((usart->ISR >> 6) & 1) == 0) \
			;                                \
	}
#endif

/*格式化发送一个字符串*/
void _USART_Send_String(USART_TypeDef *USART, char *DATA);

/*接收一个字节*/
u8 _USART_Receive_Byte(USART_TypeDef *USART, u8 *Data);

void _USART_Send_String(USART_TypeDef *USART, char *DATA)
{
	u32 i;

	for (i = 0; DATA[i] != '\0'; i++)
		_USART_Send_Byte(USART, ((u8 *)DATA)[i]);
}

void USART_Init(USART_TypeDef *USART, u32 Boundrate)
{
	u32 USARTDIV_Int;

	if (USART == USART1)
		USARTDIV_Int = APB2_MAX_CLOCK / Boundrate;
	else if (USART == USART2)
		USARTDIV_Int = APB1_MAX_CLOCK / Boundrate;
	else if (USART == USART3)
		USARTDIV_Int = APB1_MAX_CLOCK / Boundrate;
	if (USART == USART1)
	{
		RCC->AHB1ENR |= 1 << 0; //使能PORTA口时钟
		RCC->APB2ENR |= 1 << 4; //使能串口1时钟
		GPIO_Set(GPIOA_9, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_MID, GPIO_PUPD_PU);
		GPIO_Set(GPIOA_10, GPIO_MODE_AF, 0, 0, GPIO_PUPD_PU);
		GPIO_AF_Set(GPIOA_9, 7);  // PA9,AF7
		GPIO_AF_Set(GPIOA_10, 7); // PA10,AF7
	}
	else if (USART == USART2)
	{
		RCC->AHB1ENR |= 1 << 0; //使能PORTA口时钟
#if defined USE_STM32H7
		RCC->APB1LENR |= 1 << 17; //使能串口2时钟
#else
		RCC->APB1ENR |= 1 << 17; //使能串口2时钟
#endif
		GPIO_Set(GPIOA_2, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_MID, GPIO_PUPD_PU);
		GPIO_Set(GPIOA_3, GPIO_MODE_AF, 0, 0, GPIO_PUPD_PU);
		GPIO_AF_Set(GPIOA_2, 7); // PA2,AF7
		GPIO_AF_Set(GPIOA_3, 7); // PA3,AF7
	}
	else if (USART == USART3)
	{
		RCC->AHB1ENR |= 1 << 1; //使能PORTB口时钟
#if defined USE_STM32H7
		RCC->APB1LENR |= 1 << 18; //使能USART3时钟
#else
		RCC->APB1ENR |= 1 << 18; //使能USART3时钟
#endif
		GPIO_Set(GPIOB_10, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_MID, GPIO_PUPD_PU);
		GPIO_Set(GPIOB_11, GPIO_MODE_AF, 0, 0, GPIO_PUPD_PU);
		GPIO_AF_Set(GPIOB_10, 7); // PB10,AF7
		GPIO_AF_Set(GPIOB_11, 7); // PB11,AF7
	}

	USART->BRR = USARTDIV_Int; //波特率设置
	USART->CR1 &= ~(1 << 15);  //设置OVER8=0
	USART->CR1 |= 1 << 2;	   //串口接收使能
	USART->CR1 |= 1 << 3;	   //串口发送使能
	USART->CR1 |= 1;
}

void USART_Time_Out_Set(USART_TypeDef *USART, u8 Enable)
{
	if (USART == USART1)
		Time_Out_Enable[0] = Enable;
	else if (USART == USART2)
		Time_Out_Enable[1] = Enable;
	else if (USART == USART3)
		Time_Out_Enable[2] = Enable;
}

void USART_Interrupt_Set(USART_TypeDef *USART, u8 Group, u8 PrePriority, u8 SubPriority, Usart_IRQHandler CallbackFunc)
{
	if (USART == USART1)
	{
		MY_NVIC_Init(PrePriority, SubPriority, USART1_IRQn, Group);
		USART1_Interrupt_CallbackFunc = CallbackFunc;
	}
	else if (USART == USART2)
	{
		MY_NVIC_Init(PrePriority, SubPriority, USART2_IRQn, Group);
		USART2_Interrupt_CallbackFunc = CallbackFunc;
	}
	else if (USART == USART3)
	{
		MY_NVIC_Init(PrePriority, SubPriority, USART3_IRQn, Group);
		USART3_Interrupt_CallbackFunc = CallbackFunc;
	}
}

void USART_Send_Data_Flow(USART_TypeDef *USART, void *DATA, u32 Length)
{
	u32 i;

	for (i = 0; i < Length; i++)
		_USART_Send_Byte(USART, ((u8 *)DATA)[i]);
}

void USART_Printf(USART_TypeDef *USART, char *Format_String, ...)
{
	u8 save;
	u32 i;
	va_list vl;
	va_start(vl, Format_String);
	i = vsprintf(USART_Cache, Format_String, vl);
	//USART_Cache2[i] = '\0';
	_USART_Send_String(USART, USART_Cache);
	va_end(vl);
}

u8 _USART_Receive_Byte(USART_TypeDef *USART, u8 *Data)
{
	u8 Time_Out;
	u32 i = 0;

	if (USART == USART1)
		Time_Out = Time_Out_Enable[0];
	else if (USART == USART2)
		Time_Out = Time_Out_Enable[1];
	else if (USART == USART3)
		Time_Out = Time_Out_Enable[2];

#if !defined USE_STM32H7
	if (Time_Out == 1)
	{
		while ((((USART->SR >> 5) & 1) == 0) && (i < USART_TIME_OUT_VALUE))
			i++;
		if (i == USART_TIME_OUT_VALUE)
			return 1;
		else
		{
			*Data = USART->DR;
			return 0;
		}
	}
	else
	{
		while (((USART->SR >> 5) & 1) == 0)
			;
		*Data = USART->DR;
		return 0;
	}
#else
	if (Time_Out == 1)
	{
		while ((((USART->ISR >> 5) & 1) == 0) && (i < USART_TIME_OUT_VALUE))
			i++;
		if (i == USART_TIME_OUT_VALUE)
			return 1;
		else
		{
			*Data = USART->RDR;
			return 0;
		}
	}
	else
	{
		while (((USART->ISR >> 5) & 1) == 0)
			;
		*Data = USART->RDR;
		return 0;
	}
#endif
}

int USART_Scanf(USART_TypeDef *USART, char *Format_String, ...)
{
	int len;
	va_list vl;

	va_start(vl, Format_String);
	len = USART_Receive_Data_Flow_EndwithCharacter(USART, USART_Cache, 0x0a, 1);
	USART_Cache[len] = '\0';
	len = vsscanf(USART_Cache, Format_String, vl);
	va_end(vl);

	return len;
}

u8 USART_Receive_Data_Flow_EndwithLength(USART_TypeDef *USART, void *DATA, u32 Length)
{
	u8 time_out;
	u32 i;

	for (i = 0; i < Length; i++)
	{
		time_out = _USART_Receive_Byte(USART, &((u8 *)DATA)[i]);
		if (time_out == 1)
			return 0;
	}
	return Length;
}

u32 USART_Receive_Data_Flow_EndwithCharacter(USART_TypeDef *USART, void *DATA, char END, u8 Repeat_Time)
{
	u8 j, time_out;
	u32 i;

	i = 0;
	j = 0;
	do
	{
		time_out = _USART_Receive_Byte(USART, &((u8 *)DATA)[i]);
		if (time_out == 1)
			return 0;
		if (((u8 *)DATA)[i] == END) //检测到结束字符
			j++;
		else	   //检测到非结束字符
			j = 0; //累加器清零
		i++;
	} while (j != Repeat_Time);

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
void USART_Test(USART_TypeDef *USART)
{
	u8 i, Length;
	u8 Rec_Buf[16];

	USART_Printf(USART, "\n/************串口测试开始************/\n");
	USART_Printf(USART, "发送十进制整数-20120712：%d\n", -20120712);
	USART_Printf(USART, "发送十六进制整数0x012abcf9：%x\n", 0x012abcf9);
	USART_Printf(USART, "发送浮点数2012.0712：%.4f\n", 2012.0712f);
	USART_Printf(USART, "发送字符串“USART Test.”：%s\n", "USART Test.");
	USART_Printf(USART, "现在开始接收测试\n现在发送4个字节....");
	USART_Receive_Data_Flow_EndwithLength(USART, Rec_Buf, 4);
	USART_Printf(USART, "接收完毕\n");
	USART_Printf(USART, "接收的数据：");
	for (i = 0; i < 4; i++)
		USART_Printf(USART, "%x  ", Rec_Buf[i]);
	USART_Printf(USART, "\n现在发送以两个0x12结尾的数据流....\n");
	Length = USART_Receive_Data_Flow_EndwithCharacter(USART, Rec_Buf, 0x12, 2);
	USART_Printf(USART, "接收完毕\n");
	USART_Printf(USART, "接收的数据：");
	for (i = 0; i < Length; i++)
		USART_Printf(USART, "%x  ", Rec_Buf[i]);
	USART_Printf(USART, "\n发送0xff以结束测试\n");
	do
	{
		_USART_Receive_Byte(USART, &i);
	} while (i != 0xff);
	USART_Printf(USART, "/************串口测试结束************/\n");
}
#endif
