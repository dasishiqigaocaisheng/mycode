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

//I2C��д����	
//#define GT_CMD_WR 		0XBA     	//д����
//#define GT_CMD_RD 		0XBB		//������
//#define GT_CMD_WR 		0X28     	//д����
//#define GT_CMD_RD 		0X29		//������

//GT9147 ���ּĴ������� 
#define GT_CTRL_REG 	0X8040   	//GT9147���ƼĴ���
#define GT_CFGS_REG 	0X8047   	//GT9147������ʼ��ַ�Ĵ���
#define GT_CHECK_REG 	0X80FF   	//GT9147У��ͼĴ���
#define GT_PID_REG 		0X8140   	//GT9147��ƷID�Ĵ���
#define GT_GSTID_REG 	0X814E   	//GT9147��ǰ��⵽�Ĵ������

u8 GT_CMD_WR=0xba;
u8 GT_CMD_RD=0xbb;

//����ֵ�����˴�����оƬ��ʼ������д�������
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

// ���������������飬�ں��� GT9XX_SendCfg() ����ã��������ô���IC����ز���
//	����GT911/GT5688���Թ̻�������Щ�����������û�һ������������ٽ�������
//	��ϸ�ļĴ���������ο� ����GT911/GT5688 �����ֲ�
//
const uint8_t GT911_CFG_TBL[] = 
{                              	 
	0X62,			// �Ĵ�����ַ��0x8047�����ܣ����ð汾��
	0X20,0X03,	// �Ĵ�����ַ��0x8048~0x8049�����ܣ�X�������ֵ����λ��ǰ
	0XE0,0X01,	// �Ĵ�����ַ��0x804A~0x804B�����ܣ�Y�������ֵ����λ��ǰ
	0X05,			// �Ĵ�����ַ��0x804C�����ܣ����������������1~5��
	0X3D,			// �Ĵ�����ַ��0x804D�����ܣ�����INT������ʽ��XY���꽻�� 
	0X00,			// �üĴ�����������
	0X03,			// �Ĵ�����ַ��0x804F�����ܣ����»��ɿ�ȥ������
	0Xc8,			// �Ĵ�����ַ��0x8050�����ܣ�ԭʼ���괰���˲�ֵ

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
*	�Ĵ�����ַ:	0x80B7~0X80C4
*	����˵��  :	�޸ĸ�Ӧͨ����Ӧ��оƬͨ���ţ����Ըı䴥�����Ĵ�ֱɨ�跽��
*******************************************************************************************/

	0X02,0X04,0X06,0X08,0X0a,0X0c,0X0e,0X10,0X12,0X14,				// ɨ�跽��� �� �� ��
	0X16,0X18,0Xff,0Xff,

//	0X18,0X16,0X14,0X12,0X10,0X0e,0X0c,0X0a,							// ɨ�跽��� �� �� ��
//	0X08,0X06,0X04,0X02,0Xff,0Xff,			

/******************************************************************************************/

	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,		// �����üĴ�������������
	0X00,0X00,0X00,0X00,0x00,0x00, 								// �����üĴ�������������
	
/*******************************************************************************************
*	�Ĵ�����ַ:	0x80D5~0X80EE
*	����˵��  :	�޸�����ͨ����Ӧ��оƬͨ���ţ����Ըı䴥������ˮƽɨ�跽��
********************************************************************************************/

	0X00,0X02,0X04,0X06,0X08,0X0a,0X0f,0X10,0X12,0X13,		// ɨ�跽��� �� �� ��
	0X16,0X18,0X1c,0X1d,0X1e,0X1f,0X20,0X21,0X22,0X24,
	0Xff,0Xff,0Xff,0Xff,0Xff,0Xff,

//	0X24,0X22,0X21,0X20,0X1f,0X1e,0X1d,0X1c,0X18,0X16,		// ɨ�跽��� �� �� ��
//	0X13,0X12,0X10,0X0f,0X0a,0X08,0X06,0X04,0X02,0X00,	
//	0Xff,0Xff,0Xff,0Xff,0Xff,0Xff,
	
/*******************************************************************************************/								

	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// ͨ������ϵ���Ĵ����������޸�
	0x00,0x00,0x00,0x00,0x00,0x00,                    	// ͨ������ϵ���Ĵ����������޸�

};



//��GT9147д��һ������
//reg:��ʼ�Ĵ�����ַ
//buf:���ݻ�������
//len:д���ݳ���
//����ֵ:0,�ɹ�;1,ʧ��.
u8 GT911_Write(u16 Reg,u8 *buf,u16 len)
{
	u8 i;
	u8 ret=0;
	IIC_Start();	
 	IIC_Send_Byte(GT_CMD_WR);   	//����д���� 	 
	IIC_Wait_Ack();
	IIC_Send_Byte(Reg>>8);   	//���͸�8λ��ַ
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(Reg&0XFF);   	//���͵�8λ��ַ
	IIC_Wait_Ack();  
	for(i=0;i<len;i++)
	{	   
    	IIC_Send_Byte(buf[i]);  	//������
		ret=IIC_Wait_Ack();
		if(ret)break;  
	}
    IIC_Stop();					//����һ��ֹͣ����	    
	return ret; 
}

//��GT9147����һ������
//reg:��ʼ�Ĵ�����ַ
//buf:���ݻ�������
//len:�����ݳ���			  
void GT911_Read(u16 Reg,u8 *buf,u8 len)
{
	u8 i; 
 	IIC_Start();	
 	IIC_Send_Byte(GT_CMD_WR);   //����д���� 	 
	IIC_Wait_Ack();
 	IIC_Send_Byte(Reg>>8);   	//���͸�8λ��ַ
	IIC_Wait_Ack(); 	 										  		   
 	IIC_Send_Byte(Reg&0XFF);   	//���͵�8λ��ַ
	IIC_Wait_Ack();  
 	IIC_Start();  	 	   
	IIC_Send_Byte(GT_CMD_RD);   //���Ͷ�����		   
	IIC_Wait_Ack();	   
	for(i=0;i<len;i++)
	{	   
    	buf[i]=IIC_Read_Byte(i==(len-1)?0:1); //������	  
	} 
    IIC_Stop();//����һ��ֹͣ����  
}

//����GT9147���ò���
//mode:0,���������浽flash
//     1,�������浽flash
u8 GT911_Send_Cfg(u8 mode)
{
	u8 buf[2];
	u16 i=0;
	buf[0]=0;
	buf[1]=mode;	//�Ƿ�д�뵽GT9147 FLASH?  ���Ƿ���籣��
	for(i=0;i<sizeof(GT911_CFG_TBL);i++)
        buf[0]+=GT911_CFG_TBL[i];//����У���
    buf[0]=(~buf[0])+1;
	GT911_Write(GT_CFGS_REG,(u8*)GT911_CFG_TBL,sizeof(GT911_CFG_TBL));//���ͼĴ�������
	GT911_Write(GT_CHECK_REG,buf,2);//д��У���,�����ø��±��
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
			GPIO_OTYPE_PP,GPIO_SPEED_2M,GPIO_PUPD_PU);										//PEN����Ϊ�������
	GPIO_Set(GT911_RST_GPIO,GT911_INT_PIN,GPIO_MODE_OUT,
			GPIO_OTYPE_PP,GPIO_SPEED_2M,GPIO_PUPD_PU); 			//TCS����Ϊ�������
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
	GT911_Read(GT_CFGS_REG,temp,1);//��ȡGT_CFGS_REG�Ĵ���
    if (temp[0]==0xff)
    {
        GT_CMD_WR=0x28;
        GT_CMD_RD=0x29;
    }
	if(temp[0]<0X62)//Ĭ�ϰ汾�Ƚϵ�,��Ҫ����flash����
	{
		//GT911_Send_Cfg(1);//���²���������
	}
	delay_ms(10);
	temp[0]=0X00;	 
}










/*
���������⺯��
���ܣ���⵱ǰ�Ĵ������
����ֵ����Ч�Ĵ������
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
�����ȡ����
������1.*DATA����ȡ����������Ҫ��ŵĵ�ַ�����ݰ����㰴�µ�ʱ��˳����������
			   ���ݴ�Ÿ�ʽΪ��DATA[0]=��һ������X���꣬DATA[1]=��һ������Y����
							   DATA[2]=�ڶ�������X���꣬DATA[3]=�ڶ�������Y����
							   ....���Դ����ƣ�
����ֵ���������
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
