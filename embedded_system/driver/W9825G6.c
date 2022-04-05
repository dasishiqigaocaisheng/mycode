#include "W9825G6.h"
#include "GPIO.h"

/*
39

1
PC0-WE

5
PD0-D2
PD1-D3
PD8-D13
PD9-D9
PD10-D10
PD14-D0
PD15-D1

11
PE0-NBL0
PE1-NBL1
PE7-D4
PE8-D5
PE9-D6
PE10-D7
PE11-D8
PE12-D9
PE13-D10
PE14-D11
PE15-D12

11
PF0-A0
PF1-A1
PF2-A2
PF3-A3
PF4-A4
PF5-A5
PF11-RAS
PF12-A6
PF13-A7
PF14-A8
PF15-A9

7
PG15-CAS
PG4-BA0
PG5-BA1
PG8-CLK
PG0-A10
PG1-A11
PG2-A12

2
PH6-CS
PH7-CKE

*/
#define APOLLO_STM32H743
//#define FANKE_STM32F429
//#define FANKE_STM32H743

#define A0 GPIOF_0
#define A1 GPIOF_1
#define A2 GPIOF_2
#define A3 GPIOF_3
#define A4 GPIOF_4
#define A5 GPIOF_5
#define A6 GPIOF_12
#define A7 GPIOF_13
#define A8 GPIOF_14
#define A9 GPIOF_15
#define A10 GPIOG_0
#define A11 GPIOG_1
#define A12 GPIOG_2

#define D0 GPIOD_14
#define D1 GPIOD_15
#define D2 GPIOD_0
#define D3 GPIOD_1
#define D4 GPIOE_7
#define D5 GPIOE_8
#define D6 GPIOE_9
#define D7 GPIOE_10
#define D8 GPIOE_11
#define D9 GPIOE_12
#define D10 GPIOE_13
#define D11 GPIOE_14
#define D12 GPIOE_15
#define D13 GPIOD_8
#define D14 GPIOD_9
#define D15 GPIOD_10

#if defined APOLLO_STM32H743

#define NBL0 GPIOE_0
#define NBL1 GPIOE_1
#define WE GPIOC_0
#define CAS GPIOG_15
#define RAS GPIOF_11
#define CS GPIOC_2
#define BA0 GPIOG_4
#define BA1 GPIOG_5
#define CKE GPIOC_3
#define CLK GPIOG_8

#elif defined FANKE_STM32F429

#define NBL0 GPIOE_0
#define NBL1 GPIOE_1
#define WE GPIOC_0
#define CAS GPIOG_15
#define RAS GPIOF_11
#define CS GPIOH_6
#define BA0 GPIOG_4
#define BA1 GPIOG_5
#define CKE GPIOH_7
#define CLK GPIOG_8

#elif defined FANKE_STM32H743

#define NBL0 GPIOE_0
#define NBL1 GPIOE_1
#define WE GPIOH_5
#define CAS GPIOG_15
#define RAS GPIOF_11
#define CS GPIOH_3
#define BA0 GPIOG_4
#define BA1 GPIOG_5
#define CKE GPIOH_2
#define CLK GPIOG_8

#endif

u8 W9825G6_Send_Cmd(u8 bankx, u8 cmd, u8 refresh, u16 regval);

u8 W9825G6_Send_Cmd(u8 bankx, u8 cmd, u8 refresh, u16 regval)
{
	u32 retry = 0;
	u32 tempreg = 0;
	tempreg |= cmd << 0;				   //设置指令
	tempreg |= 1 << (4 - bankx);		   //设置发送指令到bank5还是6
	tempreg |= refresh << 5;			   //设置自刷新次数
	tempreg |= regval << 9;				   //设置模式寄存器的值
	FMC_Bank5_6->SDCMR = tempreg;		   //配置寄存器
	while ((FMC_Bank5_6->SDSR & (1 << 5))) //等待指令发送完成
	{
		retry++;
		if (retry > 0X1FFFFF)
			return 1;
	}
	return 0;
}

// SDRAM初始化
void W9825G6_Init(void)
{
	u32 sdctrlreg = 0, sdtimereg = 0;
	u16 mregval = 0;

	RCC->AHB3ENR |= 1 << 12; //使能FMC时钟

	GPIO_Set(A0, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_NONE);
	GPIO_Set(A1, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_NONE);
	GPIO_Set(A2, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_NONE);
	GPIO_Set(A3, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_NONE);
	GPIO_Set(A4, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_NONE);
	GPIO_Set(A5, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_NONE);
	GPIO_Set(A6, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_NONE);
	GPIO_Set(A7, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_NONE);
	GPIO_Set(A8, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_NONE);
	GPIO_Set(A9, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_NONE);
	GPIO_Set(A10, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_NONE);
	GPIO_Set(A11, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_NONE);
	GPIO_Set(A12, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_NONE);

	GPIO_Set(D0, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_NONE);
	GPIO_Set(D1, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_NONE);
	GPIO_Set(D2, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_NONE);
	GPIO_Set(D3, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_NONE);
	GPIO_Set(D4, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_NONE);
	GPIO_Set(D5, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_NONE);
	GPIO_Set(D6, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_NONE);
	GPIO_Set(D7, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_NONE);
	GPIO_Set(D8, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_NONE);
	GPIO_Set(D9, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_NONE);
	GPIO_Set(D10, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_NONE);
	GPIO_Set(D11, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_NONE);
	GPIO_Set(D12, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_NONE);
	GPIO_Set(D13, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_NONE);
	GPIO_Set(D14, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_NONE);
	GPIO_Set(D15, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_NONE);

	GPIO_Set(NBL0, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_NONE);
	GPIO_Set(NBL1, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_NONE);
	GPIO_Set(WE, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_NONE);
	GPIO_Set(CAS, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_NONE);
	GPIO_Set(RAS, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_NONE);
	GPIO_Set(CS, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_NONE);
	GPIO_Set(BA0, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_NONE);
	GPIO_Set(BA1, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_NONE);
	GPIO_Set(CKE, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_NONE);
	GPIO_Set(CLK, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_OSPEED_ULTRAL, GPIO_PUPD_NONE);

	GPIO_AF_Set(A0, GPIO_AF_FMC);
	GPIO_AF_Set(A1, GPIO_AF_FMC);
	GPIO_AF_Set(A2, GPIO_AF_FMC);
	GPIO_AF_Set(A3, GPIO_AF_FMC);
	GPIO_AF_Set(A4, GPIO_AF_FMC);
	GPIO_AF_Set(A5, GPIO_AF_FMC);
	GPIO_AF_Set(A6, GPIO_AF_FMC);
	GPIO_AF_Set(A7, GPIO_AF_FMC);
	GPIO_AF_Set(A8, GPIO_AF_FMC);
	GPIO_AF_Set(A9, GPIO_AF_FMC);
	GPIO_AF_Set(A10, GPIO_AF_FMC);
	GPIO_AF_Set(A11, GPIO_AF_FMC);
	GPIO_AF_Set(A12, GPIO_AF_FMC);

	GPIO_AF_Set(D0, GPIO_AF_FMC);
	GPIO_AF_Set(D1, GPIO_AF_FMC);
	GPIO_AF_Set(D2, GPIO_AF_FMC);
	GPIO_AF_Set(D3, GPIO_AF_FMC);
	GPIO_AF_Set(D4, GPIO_AF_FMC);
	GPIO_AF_Set(D5, GPIO_AF_FMC);
	GPIO_AF_Set(D6, GPIO_AF_FMC);
	GPIO_AF_Set(D7, GPIO_AF_FMC);
	GPIO_AF_Set(D8, GPIO_AF_FMC);
	GPIO_AF_Set(D9, GPIO_AF_FMC);
	GPIO_AF_Set(D10, GPIO_AF_FMC);
	GPIO_AF_Set(D11, GPIO_AF_FMC);
	GPIO_AF_Set(D12, GPIO_AF_FMC);
	GPIO_AF_Set(D13, GPIO_AF_FMC);
	GPIO_AF_Set(D14, GPIO_AF_FMC);
	GPIO_AF_Set(D15, GPIO_AF_FMC);

	GPIO_AF_Set(NBL0, GPIO_AF_FMC);
	GPIO_AF_Set(NBL1, GPIO_AF_FMC);
	GPIO_AF_Set(WE, GPIO_AF_FMC);
	GPIO_AF_Set(CAS, GPIO_AF_FMC);
	GPIO_AF_Set(RAS, GPIO_AF_FMC);
	GPIO_AF_Set(CS, GPIO_AF_FMC);
	GPIO_AF_Set(BA0, GPIO_AF_FMC);
	GPIO_AF_Set(BA1, GPIO_AF_FMC);
	GPIO_AF_Set(CKE, GPIO_AF_FMC);
	GPIO_AF_Set(CLK, GPIO_AF_FMC);

#if defined FANKE_STM32F429

	sdctrlreg |= 1 << 0;			  // 9位列地址
	sdctrlreg |= 2 << 2;			  // 13位行地址
	sdctrlreg |= 1 << 4;			  // 16位数据位宽
	sdctrlreg |= 1 << 6;			  // 4个内部存区(4 BANKS)
	sdctrlreg |= 3 << 7;			  // 3个CAS延迟
	sdctrlreg |= 0 << 9;			  //允许写访问
	sdctrlreg |= 2 << 10;			  // SDRAM时钟=HCLK/2=180M/2=90M=11.11ns
	sdctrlreg |= 1 << 12;			  //使能突发访问
	sdctrlreg |= 0 << 13;			  //读通道延迟1个HCLK
	FMC_Bank5_6->SDCR[0] = sdctrlreg; // bank5和bank6都要设置
	FMC_Bank5_6->SDCR[1] = sdctrlreg; //设置FMC BANK5 SDRAM控制寄存器(BANK5和6用于管理SDRAM).

	sdtimereg = 0;
	sdtimereg |= 1 << 0;			  // TMRD，加载模式寄存器到激活时间的延迟为2个时钟周期
	sdtimereg |= 6 << 4;			  // TXSR，退出自刷新延迟为7个时钟周期
	sdtimereg |= 5 << 8;			  // TRAS，自刷新时间为6个时钟周期
	sdtimereg |= 5 << 12;			  // TRC，行循环延迟为6个时钟周期
	sdtimereg |= 1 << 16;			  // TWR，恢复延迟为2个时钟周期
	sdtimereg |= 1 << 20;			  // TRP，行预充电延迟为2个时钟周期
	sdtimereg |= 1 << 24;			  // TRCD，行到列延迟为2个时钟周期
	FMC_Bank5_6->SDTR[0] = sdtimereg; // bank5和bank6都要设置
	FMC_Bank5_6->SDTR[1] = sdtimereg; //设置FMC BANK5 SDRAM时序寄存器

	W9825G6_Send_Cmd(1, 1, 0, 0);		//时钟配置使能
	SystemTimer_Delay_Us(200);			//至少延迟200us.
	W9825G6_Send_Cmd(1, 2, 0, 0);		//对所有存储区预充电
	W9825G6_Send_Cmd(1, 3, 7, 0);		//设置自刷新次数
	mregval |= 3 << 0;					//设置突发长度:1(可以是1/2/4/8)
	mregval |= 0 << 3;					//设置突发类型:连续(可以是连续/交错)
	mregval |= 3 << 4;					//设置CAS值:2(可以是2/3)
	mregval |= 0 << 7;					//设置操作模式:0,标准模式
	mregval |= 1 << 9;					//设置突发写模式:1,单点访问
	W9825G6_Send_Cmd(1, 4, 0, mregval); //设置SDRAM的模式寄存器
	//刷新频率计数器(以SDCLK频率计数),计算方法:
	// COUNT=SDRAM刷新周期/行数-20=SDRAM刷新周期(us)*SDCLK频率(Mhz)/行数
	//我们使用的SDRAM刷新周期为64ms,SDCLK=192/2=96Mhz,行数为8192(2^13).
	//所以,COUNT=64*1000*96/8192-20=730
	FMC_Bank5_6->SDRTR = 730 << 1; //设置刷新频率计数器

#elif defined APOLLO_STM32H743

	sdctrlreg |= 1 << 0;			  // 9位列地址
	sdctrlreg |= 2 << 2;			  // 13位行地址
	sdctrlreg |= 1 << 4;			  // 16位数据位宽
	sdctrlreg |= 1 << 6;			  // 4个内部存区(4 BANKS)
	sdctrlreg |= 2 << 7;			  // 3个CAS延迟
	sdctrlreg |= 0 << 9;			  //允许写访问
	sdctrlreg |= 2 << 10;			  // SDRAM时钟=HCLK/2=180M/2=90M=11.11ns
	sdctrlreg |= 1 << 12;			  //使能突发访问
	sdctrlreg |= 0 << 13;			  //读通道延迟1个HCLK
	FMC_Bank5_6->SDCR[0] = sdctrlreg; // bank5和bank6都要设置

	sdtimereg = 0;
	sdtimereg |= 1 << 0;				// TMRD，加载模式寄存器到激活时间的延迟为2个时钟周期
	sdtimereg |= 7 << 4;				// TXSR，退出自刷新延迟为7个时钟周期
	sdtimereg |= 4 << 8;				// TRAS，自刷新时间为6个时钟周期
	sdtimereg |= 6 << 12;				// TRC，行循环延迟为6个时钟周期
	sdtimereg |= 1 << 16;				// TWR，恢复延迟为2个时钟周期
	sdtimereg |= 1 << 20;				// TRP，行预充电延迟为2个时钟周期
	sdtimereg |= 1 << 24;				// TRCD，行到列延迟为2个时钟周期
	FMC_Bank5_6->SDTR[0] = sdtimereg;	// bank5和bank6都要设置
	FMC_Bank1->BTCR[0] |= (u32)1 << 31; //使能 FMC

	W9825G6_Send_Cmd(0, 1, 0, 0);		//时钟配置使能
	delay_us(500);						//至少延迟200us.
	W9825G6_Send_Cmd(0, 2, 0, 0);		//对所有存储区预充电
	W9825G6_Send_Cmd(0, 3, 8, 0);		//设置自刷新次数
	mregval |= 3 << 0;					//设置突发长度:1(可以是1/2/4/8)
	mregval |= 0 << 3;					//设置突发类型:连续(可以是连续/交错)
	mregval |= 2 << 4;					//设置CAS值:2(可以是2/3)
	mregval |= 0 << 7;					//设置操作模式:0,标准模式
	mregval |= 1 << 9;					//设置突发写模式:1,单点访问
	W9825G6_Send_Cmd(0, 4, 0, mregval); //设置SDRAM的模式寄存器
	//刷新频率计数器(以SDCLK频率计数),计算方法:
	// COUNT=SDRAM刷新周期/行数-20=SDRAM刷新周期(us)*SDCLK频率(Mhz)/行数
	//我们使用的SDRAM刷新周期为64ms,SDCLK=192/2=96Mhz,行数为8192(2^13).
	//所以,COUNT=64*1000*96/8192-20=730
	FMC_Bank5_6->SDRTR = 878 << 1; //设置刷新频率计数器

#elif defined FANKE_STM32H743

	sdctrlreg |= 1 << 0;			  // 9位列地址
	sdctrlreg |= 2 << 2;			  // 13位行地址
	sdctrlreg |= 1 << 4;			  // 16位数据位宽
	sdctrlreg |= 1 << 6;			  // 4个内部存区(4 BANKS)
	sdctrlreg |= 2 << 7;			  // 3个CAS延迟
	sdctrlreg |= 0 << 9;			  //允许写访问
	sdctrlreg |= 2 << 10;			  // SDRAM时钟=HCLK/2=180M/2=90M=11.11ns
	sdctrlreg |= 1 << 12;			  //使能突发访问
	sdctrlreg |= 0 << 13;			  //读通道延迟1个HCLK
	FMC_Bank5_6->SDCR[0] = sdctrlreg; // bank5和bank6都要设置

	sdtimereg = 0;
	sdtimereg |= 1 << 0;				// TMRD，加载模式寄存器到激活时间的延迟为2个时钟周期
	sdtimereg |= 7 << 4;				// TXSR，退出自刷新延迟为7个时钟周期
	sdtimereg |= 4 << 8;				// TRAS，自刷新时间为6个时钟周期
	sdtimereg |= 6 << 12;				// TRC，行循环延迟为6个时钟周期
	sdtimereg |= 1 << 16;				// TWR，恢复延迟为2个时钟周期
	sdtimereg |= 1 << 20;				// TRP，行预充电延迟为2个时钟周期
	sdtimereg |= 1 << 24;				// TRCD，行到列延迟为2个时钟周期
	FMC_Bank5_6->SDTR[0] = sdtimereg;	// bank5和bank6都要设置
	FMC_Bank1->BTCR[0] |= (u32)1 << 31; //使能 FMC

	W9825G6_Send_Cmd(0, 1, 0, 0);		//时钟配置使能
	delay_us(500);						//至少延迟200us.
	W9825G6_Send_Cmd(0, 2, 0, 0);		//对所有存储区预充电
	W9825G6_Send_Cmd(0, 3, 8, 0);		//设置自刷新次数
	mregval |= 3 << 0;					//设置突发长度:1(可以是1/2/4/8)
	mregval |= 0 << 3;					//设置突发类型:连续(可以是连续/交错)
	mregval |= 2 << 4;					//设置CAS值:2(可以是2/3)
	mregval |= 0 << 7;					//设置操作模式:0,标准模式
	mregval |= 1 << 9;					//设置突发写模式:1,单点访问
	W9825G6_Send_Cmd(0, 4, 0, mregval); //设置SDRAM的模式寄存器
	//刷新频率计数器(以SDCLK频率计数),计算方法:
	// COUNT=SDRAM刷新周期/行数-20=SDRAM刷新周期(us)*SDCLK频率(Mhz)/行数
	//我们使用的SDRAM刷新周期为64ms,SDCLK=192/2=96Mhz,行数为8192(2^13).
	//所以,COUNT=64*1000*96/8192-20=730
	FMC_Bank5_6->SDRTR = 878 << 1; //设置刷新频率计数器

#endif
}
