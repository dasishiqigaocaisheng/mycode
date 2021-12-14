#include "sys.h"
#include "GT911.h"
#include "delay.h"
#include "IIC.h"
#include "USART.h"

#define GT911_RST_GPIO	GPIOC
#define GT911_RST_PIN	PIN13
#define GT911_RST		GPIO_Write(GPIOC_13)

#define GT911_INT_GPIO	GPIOI
#define GT911_INT_PIN	PIN11
#define GT911_INT_OUT	GPIO_Write(GPIOI_11)
#define GT911_INT_IN	GPIO_Read(GPIOI_11)

//I2C读写命令	
//#define GT_CMD_WR 		0XBA     	//写命令
//#define GT_CMD_RD 		0XBB		//读命令
//#define GT_CMD_WR 		0X28     	//写命令
//#define GT_CMD_RD 		0X29		//读命令

//GT9147 部分寄存器定义 
#define GT_CTRL_REG 	0X8040   	//GT9147控制寄存器
#define GT_CFGS_REG 	0X8047   	//GT9147配置起始地址寄存器
#define GT_CHECK_REG 	0X80FF   	//GT9147校验和寄存器
#define GT_PID_REG 		0X8140   	//GT9147产品ID寄存器
#define GT_GSTID_REG 	0X814E   	//GT9147当前检测到的触摸情况

u8 GT_CMD_WR=0xba;
u8 GT_CMD_RD=0xbb;

//该数值储存了触摸屏芯片初始化所需写入的数据
/*const u8 GT911_CFG_TBL[]=
{ 
	0X61,0XE0,0X01,0X20,0X03,0X05,0X35,0X00,0X02,0X08,
	0X1E,0X08,0X50,0X3C,0X0F,0X05,0X00,0X00,0XFF,0X67,
	0X50,0X00,0X00,0X18,0X1A,0X1E,0X14,0X89,0X28,0X0A,
	0X30,0X2E,0XBB,0X0A,0X03,0X00,0X00,0X02,0X33,0X1D,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X32,0X00,0X00,
	0X2A,0X1C,0X5A,0X94,0XC5,0X02,0X07,0X00,0X00,0X00,
	0XB5,0X1F,0X00,0X90,0X28,0X00,0X77,0X32,0X00,0X62,
	0X3F,0X00,0X52,0X50,0X00,0X52,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0X0F,0X03,0X06,0X10,0X42,0XF8,0X0F,0X14,0X00,0X00,
	0X00,0X00,0X1A,0X18,0X16,0X14,0X12,0X10,0X0E,0X0C,
	0X0A,0X08,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0Xff,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X29,0X28,0X24,0X22,0X20,0X1F,0X1E,0X1D,
	0X0E,0X0C,0X0A,0X08,0X06,0X05,0X04,0X02,0X00,0XFF,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF
};*/

// 触摸参数配置数组，在函数 GT9XX_SendCfg() 里调用，用于配置触摸IC的相关参数
//	由于GT911/GT5688可以固化保存这些参数，所以用户一般情况下无需再进行配置
//	详细的寄存器功能请参考 由于GT911/GT5688 数据手册
//
const uint8_t GT911_CFG_TBL[] = 
{                              	 
	0X62,			// 寄存器地址：0x8047，功能：配置版本号
	0X20,0X03,	// 寄存器地址：0x8048~0x8049，功能：X坐标最大值，低位在前
	0XE0,0X01,	// 寄存器地址：0x804A~0x804B，功能：Y坐标最大值，低位在前
	0X05,			// 寄存器地址：0x804C，功能：设置最大触摸点数，1~5点
	0X3D,			// 寄存器地址：0x804D，功能：设置INT触发方式、XY坐标交换 
	0X00,			// 该寄存器无需配置
	0X03,			// 寄存器地址：0x804F，功能：按下或松开去抖次数
	0Xc8,			// 寄存器地址：0x8050，功能：原始坐标窗口滤波值

	0X28,0X0f,0X5a,0X46,0X03,0X05,0X00,0X00,0X00,0X00,		// 0X8051 ~ 0X805A
	0X00,0X00,0X00,0X17,0X19,0X1d,0X14,0X8a,0X2a,0X0c,		// 0X805B ~ 0X8064
	0X55,0X57,0Xb2,0X04,0X00,0X00,0X00,0X21,0X02,0X1d,		// 0X8065 ~ 0X806E
	0X19,0X01,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,		// 0X806F ~ 0X8078
	0X00,0X3c,0X78,0X94,0Xd5,0X02,0X07,0X00,0X00,0X04,		// 0X8079 ~ 0X8082
	0X9e,0X40,0X00,0X8d,0X4a,0X00,0X7f,0X55,0X00,0X75,		// 0X8083 ~ 0X808C
	0X61,0X00,0X6b,0X70,0X00,0X6b,0X00,0X00,0X00,0X00,		// 0X808D ~ 0X8096
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,		// 0X8097 ~ 0X80A0
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,		// 0X80A1 ~ 0X80AA
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,		// 0X80AB ~ 0X80B4
	0X00,0X00,		

/******************************************************************************************
*	寄存器地址:	0x80B7~0X80C4
*	功能说明  :	修改感应通道对应的芯片通道号，可以改变触摸面板的垂直扫描方向
*******************************************************************************************/

	0X02,0X04,0X06,0X08,0X0a,0X0c,0X0e,0X10,0X12,0X14,				// 扫描方向从 上 到 下
	0X16,0X18,0Xff,0Xff,

//	0X18,0X16,0X14,0X12,0X10,0X0e,0X0c,0X0a,							// 扫描方向从 下 到 上
//	0X08,0X06,0X04,0X02,0Xff,0Xff,			

/******************************************************************************************/

	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,		// 无作用寄存器，无需配置
	0X00,0X00,0X00,0X00,0x00,0x00, 								// 无作用寄存器，无需配置
	
/*******************************************************************************************
*	寄存器地址:	0x80D5~0X80EE
*	功能说明  :	修改驱动通道对应的芯片通道号，可以改变触摸面板的水平扫描方向
********************************************************************************************/

	0X00,0X02,0X04,0X06,0X08,0X0a,0X0f,0X10,0X12,0X13,		// 扫描方向从 左 到 右
	0X16,0X18,0X1c,0X1d,0X1e,0X1f,0X20,0X21,0X22,0X24,
	0Xff,0Xff,0Xff,0Xff,0Xff,0Xff,

//	0X24,0X22,0X21,0X20,0X1f,0X1e,0X1d,0X1c,0X18,0X16,		// 扫描方向从 右 到 左
//	0X13,0X12,0X10,0X0f,0X0a,0X08,0X06,0X04,0X02,0X00,	
//	0Xff,0Xff,0Xff,0Xff,0Xff,0Xff,
	
/*******************************************************************************************/								

	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// 通道调整系数寄存器，无需修改
	0x00,0x00,0x00,0x00,0x00,0x00,                    	// 通道调整系数寄存器，无需修改

};



//向GT9147写入一次数据
//reg:起始寄存器地址
//buf:数据缓缓存区
//len:写数据长度
//返回值:0,成功;1,失败.
u8 GT911_Write(u16 Reg,u8 *buf,u16 len)
{
	u8 i;
	u8 ret=0;
	IIC_Start();	
 	IIC_Send_Byte(GT_CMD_WR);   	//发送写命令 	 
	IIC_Wait_Ack();
	IIC_Send_Byte(Reg>>8);   	//发送高8位地址
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(Reg&0XFF);   	//发送低8位地址
	IIC_Wait_Ack();  
	for(i=0;i<len;i++)
	{	   
    	IIC_Send_Byte(buf[i]);  	//发数据
		ret=IIC_Wait_Ack();
		if(ret)break;  
	}
    IIC_Stop();					//产生一个停止条件	    
	return ret; 
}

//从GT9147读出一次数据
//reg:起始寄存器地址
//buf:数据缓缓存区
//len:读数据长度			  
void GT911_Read(u16 Reg,u8 *buf,u8 len)
{
	u8 i; 
 	IIC_Start();	
 	IIC_Send_Byte(GT_CMD_WR);   //发送写命令 	 
	IIC_Wait_Ack();
 	IIC_Send_Byte(Reg>>8);   	//发送高8位地址
	IIC_Wait_Ack(); 	 										  		   
 	IIC_Send_Byte(Reg&0XFF);   	//发送低8位地址
	IIC_Wait_Ack();  
 	IIC_Start();  	 	   
	IIC_Send_Byte(GT_CMD_RD);   //发送读命令		   
	IIC_Wait_Ack();	   
	for(i=0;i<len;i++)
	{	   
    	buf[i]=IIC_Read_Byte(i==(len-1)?0:1); //发数据	  
	} 
    IIC_Stop();//产生一个停止条件  
}

//发送GT9147配置参数
//mode:0,参数不保存到flash
//     1,参数保存到flash
u8 GT911_Send_Cfg(u8 mode)
{
	u8 buf[2];
	u16 i=0;
	buf[0]=0;
	buf[1]=mode;	//是否写入到GT9147 FLASH?  即是否掉电保存
	for(i=0;i<sizeof(GT911_CFG_TBL);i++)
        buf[0]+=GT911_CFG_TBL[i];//计算校验和
    buf[0]=(~buf[0])+1;
	GT911_Write(GT_CFGS_REG,(u8*)GT911_CFG_TBL,sizeof(GT911_CFG_TBL));//发送寄存器配置
	GT911_Write(GT_CHECK_REG,buf,2);//写入校验和,和配置更新标记
	return 0;
} 


void Touch_Delay(int a)
{
    int i;
	while (a --)				
	{
		for (i = 0; i < 10; i++);
	}
}

void GT911_Init(void)
{
	u8 temp[5];  

	GPIO_Set(GT911_INT_GPIO,GT911_INT_PIN,GPIO_MODE_OUT,
			GPIO_OTYPE_PP,GPIO_SPEED_2M,GPIO_PUPD_PU);										//PEN设置为推挽输出
	GPIO_Set(GT911_RST_GPIO,GT911_INT_PIN,GPIO_MODE_OUT,
			GPIO_OTYPE_PP,GPIO_SPEED_2M,GPIO_PUPD_PU); 			//TCS设置为推挽输出
 	GT911_INT_OUT=0;
	GT911_RST=1;
    IIC_Init();

	GT911_RST=0;
	delay_us(800);
	GT911_RST=1;
	delay_ms(10);
	GPIO_Set(GT911_INT_GPIO,GT911_INT_PIN,GPIO_MODE_IN,0,0,GPIO_PUPD_NONE);
	delay_ms(100);  
	
	GT911_Read(0x8140,temp,1);
	temp[0]=0X02;
	GT911_Read(GT_CFGS_REG,temp,1);//读取GT_CFGS_REG寄存器
    if (temp[0]==0xff)
    {
        GT_CMD_WR=0x28;
        GT_CMD_RD=0x29;
    }
	if(temp[0]<0X62)//默认版本比较低,需要更新flash配置
	{
		//GT911_Send_Cfg(1);//更新并保存配置
	}
	delay_ms(10);
	temp[0]=0X00;	 
}










/*
触点个数检测函数
功能：检测当前的触点个数
返回值：有效的触点个数
*/
u8 GT911_Get_Points_Number(void)
{
	u8 save[2]={0};
	
	GT911_Read(0x814e,save,1);
	//TOUCH_Write(0x814e,save+1,1);
	
	switch (save[0])
	{
		case 0:
			return 0;
		case 255:
			return 0;
		case 128:
			return 0;
		case 129:
			return 1;
		case 130:
			return 2;
		case 131:
			return 3;
		case 132:
			return 4;
		case 133:
			return 5;
		default :
			return 0;
	}
}

/*
坐标读取函数
参数：1.*DATA：读取出来的坐标要存放的地址，数据按触点按下的时间顺序线性排列
			   数据存放格式为：DATA[0]=第一个触点X坐标，DATA[1]=第一个触点Y坐标
							   DATA[2]=第二个触点X坐标，DATA[3]=第二个触点Y坐标
							   ....（以此类推）
返回值：触点个数
*/
u8 GT911_Get_Coordinates(u16 *DATA)
{
	u8 Number,i=0;
	u8 Original_Coordinates[4];
	u16 Coordinates_XY[2]={0};
	
	if ((Number=GT911_Get_Points_Number())==0)
	{
		GT911_Write(0x814e,&i,1);
		return 0;
	}
	
	for (i=0;i<Number;i++,Coordinates_XY[0]=0,Coordinates_XY[1]=0)
	{
		GT911_Read(0x8150+i*8,Original_Coordinates,4);
		Coordinates_XY[0]|=Original_Coordinates[1];
		Coordinates_XY[0]<<=8;
		Coordinates_XY[0]|=Original_Coordinates[0];
		Coordinates_XY[1]|=Original_Coordinates[3];
		Coordinates_XY[1]<<=8;
		Coordinates_XY[1]|=Original_Coordinates[2];
		DATA[i*2]=Coordinates_XY[0];
		DATA[i*2+1]=Coordinates_XY[1];
	}
	i=0;
	GT911_Write(0x814e,&i,1);
	delay_ms(15);
	
	return Number;
}
