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

#define IIC_SDA_IN()  {GPIOH->MODER&=~((u32)3<<10);}	//MOSI�������ߣ�����ģʽ
#define IIC_SDA_OUT() {GPIOH->MODER&=~((u32)3<<10);GPIOH->MODER|=(u32)1<<10;} 	//MOSI�������ߣ����ģʽ

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

//����IIC��ʼ�ź�
void IIC_Start(void)
{
	IIC_SDA_OUT();     //sda�����
	IIC_SDA_WRITE=1;	  	  
	IIC_SCLK=1;
	IIC_Delay();
 	IIC_SDA_WRITE=0;//START:when CLK is high,DATA change form high to low 
	IIC_Delay();
	IIC_SCLK=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
	IIC_Delay();
}	  

//����IICֹͣ�ź�
void IIC_Stop(void)
{
	IIC_SDA_OUT();//sda�����
	
	IIC_SCLK=0;
	IIC_Delay();
	IIC_SDA_WRITE=0;
	IIC_Delay();
	
	IIC_SCLK=1;
	IIC_Delay();
	IIC_SDA_WRITE=1;
	IIC_Delay();
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
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
	IIC_SCLK=0;//ʱ�����0 	
	IIC_SDA_IN();      //SDA����Ϊ����  	
	return 0;  
} 
//����ACKӦ��
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
//������ACKӦ��		    
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
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	IIC_SDA_OUT(); 	    
    IIC_SCLK=0;//����ʱ�ӿ�ʼ���ݴ���
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
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	u8 i,receive=0;
 	IIC_SDA_IN();//SDA����Ϊ����
	for(i=0;i<8;i++ )
	{ 
		IIC_SCLK=0; 	    	   
		IIC_Delay();
		IIC_SCLK=1;	 
		receive<<=1;
		if(IIC_SDA_READ)receive++;   
		IIC_Delay();
	}	  				 
	if (!ack)IIC_NAck();//����nACK
	else IIC_Ack(); //����ACK   
 	return receive;
}



