#ifndef __SYS_H
#define __SYS_H	 
#include "stm32h7xx.h" 
#include "Framework.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32H7������
//ϵͳʱ�ӳ�ʼ��	
//����ʱ������/�жϹ���/GPIO���õ�
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/6/6
//�汾��V1.1
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved
//********************************************************************************
//�޸�˵��
//V1.1 20180806
//1,�޸�PLL2�ı�Ƶϵ��Ϊ440,pll2_r_ck=220Mhz
//2,������pll2_p_ck�ķ�Ƶ����,2��Ƶ,�õ�220M��ʱ��Ƶ��
//3,�޸�pll1_q_ck��ע��,��Ϊ4��Ƶ,200Mhz��Ƶ��˵��
//////////////////////////////////////////////////////////////////////////////////  


//0,��֧��OS
//1,֧��OS
#define SYSTEM_SUPPORT_OS		0		//����ϵͳ�ļ����Ƿ�֧��OS
///////////////////////////////////////////////////////////////////////////////////
//����һЩ���õ��������Ͷ̹ؼ��� 
typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;
 
typedef volatile uint32_t  vu32;
typedef volatile uint16_t vu16;
typedef volatile uint8_t  vu8; 

////////////////////////////////////////////////////////////////////////////////// 
//Ex_NVIC_Configר�ö���
#define GPIO_A 				0
#define GPIO_B 				1
#define GPIO_C				2
#define GPIO_D 				3
#define GPIO_E 				4
#define GPIO_F 				5
#define GPIO_G 				6 
#define GPIO_H 				7 
#define GPIO_I 				8 
#define GPIO_J 				9 
#define GPIO_K 				10 

#define FTIR   				1  		//�½��ش���
#define RTIR   				2  		//�����ش���

//GPIO����ר�ú궨��
#define GPIO_MODE_IN    	0		//��ͨ����ģʽ
#define GPIO_MODE_OUT		1		//��ͨ���ģʽ
#define GPIO_MODE_AF		2		//AF����ģʽ
#define GPIO_MODE_AIN		3		//ģ������ģʽ

#define GPIO_SPEED_LOW		0		//GPIO�ٶ�(����,12M)
#define GPIO_SPEED_MID		1		//GPIO�ٶ�(����,60M)
#define GPIO_SPEED_FAST		2		//GPIO�ٶ�(����,85M)
#define GPIO_SPEED_HIGH		3		//GPIO�ٶ�(����,100M)  

#define GPIO_PUPD_NONE		0		//����������
#define GPIO_PUPD_PU		1		//����
#define GPIO_PUPD_PD		2		//����
#define GPIO_PUPD_RES		3		//���� 

#define GPIO_OTYPE_PP		0		//�������
#define GPIO_OTYPE_OD		1		//��©��� 

//GPIO����λ�ö���
#define PIN0				1<<0
#define PIN1				1<<1
#define PIN2				1<<2
#define PIN3				1<<3
#define PIN4				1<<4
#define PIN5				1<<5
#define PIN6				1<<6
#define PIN7				1<<7
#define PIN8				1<<8
#define PIN9				1<<9
#define PIN10				1<<10
#define PIN11				1<<11
#define PIN12				1<<12
#define PIN13				1<<13
#define PIN14				1<<14
#define PIN15				1<<15 

#define AF_MCO_SWD_SWCLK    0
#define AF_RTC              0
#define AF_TIM1_2_16_17		1
#define AF_LPTIM1           1
#define AF_TIM3_4_5_12		2
#define AF_HRTIM1_SAI1      2
#define AF_TIM8     		3
#define AF_LPTIM2_3_4_5     3
#define AF_HRTIM1           3
#define AF_LPUART1          3
#define AF_I2C_1_2_3		4
#define AF_TIM15            4
#define AF_SPI1_2_3_4_5_6	5
#define AF_SPI3				6
#define AF_USART1_2_3		7
#define AF_USART4_5_6		8
#define AF_CAN1_2			9
#define AF_TIM12_13_14		9
#define AF_OTG_FS_OTG_HS	10
#define AF_ETH				11
#define AF_FMC				12
#define AF_SDIO_OTG_HS_1	12
#define AF_DCMI				13
#define AF_LCD              14
#define AF_EVENTOUT			15
////////////////////////////////////////////////////////////////////////////////// 

#define Stm32_Clock_Fast_Init()	Stm32_Clock_Init(160,5,2,4)//����ʱ��,400Mhz
#define GPIO_Clock_Enable() 	RCC->AHB4ENR|=0x3ff

#define GPIO_AF_Init(gpio_pin,af)					GPIO_AF_Set(_Get_GPIO_Addr(gpio_pin),GPIO_Get_Pin_Num(gpio_pin),af)

#define GPIO_Init(gpio_pin,mode,otype,speed,pupd)	GPIO_Set(_Get_GPIO_Addr(gpio_pin),GPIO_Get_Pin_Addr(gpio_pin),mode,otype,speed,pupd)

u8 Sys_Clock_Set(u32 plln,u32 pllm,u32 pllp,u32 pllq);		//ϵͳʱ������
void Stm32_Clock_Init(u32 plln,u32 pllm,u32 pllp,u32 pllq); //ʱ�ӳ�ʼ��  
void Sys_Soft_Reset(void);      							//ϵͳ��λ
void Cache_Enable(void);									//ʹ��catch
void Sys_Standby(void);         							//����ģʽ 	
void MY_NVIC_SetVectorTable(u32 NVIC_VectTab, u32 Offset);	//����ƫ�Ƶ�ַ
void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group);			//����NVIC����
void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group);//�����ж�
void Ex_NVIC_Config(u8 GPIOx,u8 BITx,u8 TRIM);				//�ⲿ�ж����ú���(ֻ��GPIOA~I)
void GPIO_AF_Set(GPIO_TypeDef* GPIOx,u8 BITx,u8 AFx);		//GPIO���ù�������
void GPIO_Set(GPIO_TypeDef* GPIOx,u32 BITx,u32 MODE,u32 OTYPE,u32 OSPEED,u32 PUPD);//GPIO���ú��� 
//void GPIO_Pin_Set(GPIO_TypeDef* GPIOx,u16 pinx,u8 status);	//����ĳ��IO�ڵ����״̬
//u8 GPIO_Pin_Get(GPIO_TypeDef* GPIOx,u16 pinx);				//��ȡĳ��IO�ڵ�����״̬
//����Ϊ��ຯ��
void WFI_SET(void);		//ִ��WFIָ��
void INTX_DISABLE(void);//�ر������ж�
void INTX_ENABLE(void);	//���������ж�
void MSR_MSP(u32 addr);	//���ö�ջ��ַ 
#endif











