#include "IIC.h"
#include "sys.h"
#include "delay.h"

//#define DEBUG

#ifdef DEBUG
#include "USART.h"
#endif

#define IIC_SCLK_GPIO	GPIOH
#define IIC_SCLK_PIN	PIN4
#define IIC_SCLK		PHout(4)

#define IIC_SDA_GPIO	GPIOH
#define IIC_SDA_PIN		PIN5
#define IIC_SDA_WRITE	PHout(5)
#define IIC_SDA_READ	PHin(5)

#define IIC_SDA_IN()  {GPIOH->MODER&=~((u32)3<<10);}	//MOSI（数据线）输入模式
#define IIC_SDA_OUT() {GPIOH->MODER&=~((u32)3<<10);GPIOH->MODER|=(u32)1<<10;} 	//MOSI（数据线）输出模式

#define IIC_Delay()	delay_us(2)

void IIC_Init(void)
{
	GPIO_Set(IIC_SCLK_GPIO,IIC_SCLK_PIN,GPIO_MODE_OUT,GPIO_OTYPE_OD,
				GPIO_SPEED_25M,GPIO_PUPD_NONE);
	GPIO_Set(IIC_SDA_GPIO,IIC_SDA_PIN,GPIO_MODE_OUT,GPIO_OTYPE_PP,
				GPIO_SPEED_25M,GPIO_PUPD_NONE);
	IIC_SDA_WRITE=1;
	IIC_SCLK=1;
}

//产生IIC起始信号
void IIC_Start(void)
{
	IIC_SDA_OUT();     //sda线输出
	IIC_SDA_WRITE=1;	  	  
	IIC_SCLK=1;
	IIC_Delay();
 	IIC_SDA_WRITE=0;//START:when CLK is high,DATA change form high to low 
	IIC_Delay();
	IIC_SCLK=0;//钳住I2C总线，准备发送或接收数据 
	IIC_Delay();
}	  

//产生IIC停止信号
void IIC_Stop(void)
{
	IIC_SDA_OUT();//sda线输出
	
	IIC_SCLK=0;
	IIC_Delay();
	IIC_SDA_WRITE=0;
	IIC_Delay();
	
	IIC_SCLK=1;
	IIC_Delay();
	IIC_SDA_WRITE=1;
	IIC_Delay();
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	
	IIC_SDA_WRITE=1;
	IIC_SDA_IN();
	IIC_Delay();	
	IIC_SCLK=1;
	IIC_Delay();
	
	while(IIC_SDA_READ)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			#ifdef DEBUG
			USART_Printf(USART1,"ACK Error\n");
			#endif
			return 1;
		} 
		IIC_Delay();
	}
	IIC_SCLK=0;//时钟输出0 	
	IIC_SDA_IN();      //SDA设置为输入  	
	return 0;  
} 
//产生ACK应答
void IIC_Ack(void)
{
	IIC_SCLK=0;
	IIC_Delay();
	IIC_SDA_OUT();
	IIC_Delay();
	IIC_SDA_WRITE=0;
	IIC_Delay();
	IIC_SCLK=1;
	IIC_Delay();
	IIC_SCLK=0;
	IIC_SDA_WRITE=1;
	IIC_Delay();
}
//不产生ACK应答		    
void IIC_NAck(void)
{
	IIC_SCLK=0;
	IIC_SDA_OUT();
	IIC_Delay();
	IIC_SDA_WRITE=1;
	IIC_Delay();
	IIC_SCLK=1;
	IIC_Delay();
	IIC_SCLK=0;
	IIC_Delay();
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	IIC_SDA_OUT(); 	    
    IIC_SCLK=0;//拉低时钟开始数据传输
	//IIC_Delay();
	for(t=0;t<8;t++)
    {              
        IIC_SDA_WRITE=(txd&0x80)>>7;
        txd<<=1; 	
		IIC_Delay();		
		IIC_SCLK=1; 
		IIC_Delay();
		IIC_SCLK=0;	
		//IIC_Delay();
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	u8 i,receive=0;
 	IIC_SDA_IN();//SDA设置为输入
	for(i=0;i<8;i++ )
	{ 
		IIC_SCLK=0; 	    	   
		IIC_Delay();
		IIC_SCLK=1;	 
		receive<<=1;
		if(IIC_SDA_READ)receive++;   
		IIC_Delay();
	}	  				 
	if (!ack)IIC_NAck();//发送nACK
	else IIC_Ack(); //发送ACK   
 	return receive;
}



