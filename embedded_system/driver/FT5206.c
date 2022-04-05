#include "FT5206.h"
//#include "IIC.h"
#include "string.h"
#include "USART.h"

//与电容触摸屏连接的芯片引脚(未包含IIC引脚)
//IO操作函数
#define FT_RST(x) GPIO_Write(GPIOI_8, x) //FT5206复位引脚

//#define SDA GPIOI_3
//#define SCL GPIOH_6
//#define RST GPIOI_8

#define IIC_SLAVE_ADDRESS 0x70

//I2C读写命令
#define FT_CMD_WR 0X70 //写命令
#define FT_CMD_RD 0X71 //读命令

//FT5206 部分寄存器定义
#define FT_DEVIDE_MODE 0x00    //FT5206模式控制寄存器
#define FT_REG_NUM_FINGER 0x02 //触摸状态寄存器

#define FT_TP1_REG 0X03 //第一个触摸点数据地址
#define FT_TP2_REG 0X09 //第二个触摸点数据地址
#define FT_TP3_REG 0X0F //第三个触摸点数据地址
#define FT_TP4_REG 0X15 //第四个触摸点数据地址
#define FT_TP5_REG 0X1B //第五个触摸点数据地址

#define FT_ID_G_LIB_VERSION 0xA1  //版本
#define FT_ID_G_MODE 0xA4         //FT5206中断模式控制寄存器
#define FT_ID_G_THGROUP 0x80      //触摸有效值设置寄存器
#define FT_ID_G_PERIODACTIVE 0x88 //激活状态周期设置寄存器

//向FT5206写入一次数据
//reg:起始寄存器地址
//buf:数据缓缓存区
//len:写数据长度
//返回值:0,成功;1,失败.
u8 FT5206_WR_Reg(u16 reg, u8 *buf, u8 len)
{
    /*	u8 i;
	u8 ret=0;
	IIC_Start(SCL,SDA);	 
	IIC_Send_Byte(SCL,SDA,FT_CMD_WR);	//发送写命令 	 
	IIC_Wait_Ack(SCL,SDA); 	 										  		   
	IIC_Send_Byte(SCL,SDA,reg&0XFF);   	//发送低8位地址
	IIC_Wait_Ack(SCL,SDA);  
	for(i=0;i<len;i++)
	{	   
    	IIC_Send_Byte(SCL,SDA,buf[i]);  	//发数据
		ret=IIC_Wait_Ack(SCL,SDA);
		if(ret)break;  
	}
    IIC_Stop(SCL,SDA);					//产生一个停止条件	  
	return ret; */
}
//从FT5206读出一次数据
//reg:起始寄存器地址
//buf:数据缓缓存区
//len:读数据长度
void FT5206_RD_Reg(u16 reg, u8 *buf, u8 len)
{
    /*	u8 i; 
 	IIC_Start(SCL,SDA);	
 	IIC_Send_Byte(SCL,SDA,FT_CMD_WR);   	//发送写命令 	 
	IIC_Wait_Ack(SCL,SDA); 	 										  		   
 	IIC_Send_Byte(SCL,SDA,reg&0XFF);   	//发送低8位地址
	IIC_Wait_Ack(SCL,SDA);  
 	IIC_Start(SCL,SDA);  	 	   
	IIC_Send_Byte(SCL,SDA,FT_CMD_RD);   	//发送读命令		   
	IIC_Wait_Ack(SCL,SDA);	   
	for(i=0;i<len;i++)
	{	   
    	buf[i]=IIC_Read_Byte(SCL,SDA,i==(len-1)?0:1); //发数据	  
	} 
    IIC_Stop(SCL,SDA);//产生一个停止条件     */
}

u8 CIP[5]; //用来存放触摸IC-GT911
//初始化FT5206触摸屏
//返回值:0,初始化成功;1,初始化失败
u8 FT5206_Init(void)
{
    /*    u8 temp[5];
    u8 res=1; 
	GPIO_Init(RST,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_FAST,GPIO_PUPD_PU);//PI8设置为推挽输出
	IIC_Init(SCL,SDA);      	//初始化电容屏的I2C总线  
    GPIO_Pin_Reset(RST);
	delay_ms(20);
    GPIO_Pin_Set(RST);
	delay_ms(50);  	
	temp[0]=0;
    //读取版本号，参考值：0x3003
	FT5206_RD_Reg(FT_ID_G_LIB_VERSION,&temp[0],2);  
	if((temp[0]==0X30&&temp[1]==0X03)||temp[1]==0X01||temp[1]==0X02)//版本:0X3003/0X0001/0X0002
	{
        FT5206_WR_Reg(FT_DEVIDE_MODE,temp,1);	//进入正常操作模式 
        FT5206_WR_Reg(FT_ID_G_MODE,temp,1);		//查询模式 
        temp[0]=22;								//触摸有效值，22，越小越灵敏	
        FT5206_WR_Reg(FT_ID_G_THGROUP,temp,1);	//设置触摸有效值
        temp[0]=12;								//激活周期，不能小于12，最大14
        FT5206_WR_Reg(FT_ID_G_PERIODACTIVE,temp,1); 
		res = 0;
	}  
	return res;*/
}
const u16 FT5206_TPX_TBL[5] = {FT_TP1_REG, FT_TP2_REG, FT_TP3_REG, FT_TP4_REG, FT_TP5_REG};
u8 g_gt_tnum = 5; //默认支持的触摸屏点数(5点触摸)
//扫描触摸屏(采用查询方式)
//mode:0,正常扫描.
//返回值:当前触屏状态.
//0,触屏无触摸;1,触屏有触摸
u8 FT5206_Get_Points(u16 *buf, u8 max)
{
    u8 i, pnum;
    u16 save;
    u8 *buf_u8 = (u8 *)buf;

    FT5206_RD_Reg(0x02, &pnum, 1);
    if (pnum != 0xff)
    {
        if (pnum != 0)
        {
            if (pnum > max)
            {
                for (i = 0; i < max; i++)
                    FT5206_RD_Reg(0x03 + 0x06 * i, buf_u8 + i * 4, 4);
                for (i = 0; i < max; i++)
                {
                    buf[i * 2] = ((buf_u8[i * 4] & 0xf) << 8) + buf_u8[i * 4 + 1];
                    buf[i * 2 + 1] = ((buf_u8[i * 4 + 2] & 0xf) << 8) + buf_u8[i * 4 + 3];
                }
            }
            else
            {
                for (i = 0; i < pnum; i++)
                    FT5206_RD_Reg(0x03 + 0x06 * i, buf_u8 + i * 4, 4);
                for (i = 0; i < pnum; i++)
                {
                    buf[i * 2] = ((buf_u8[i * 4] & 0xf) << 8) + buf_u8[i * 4 + 1];
                    buf[i * 2 + 1] = ((buf_u8[i * 4 + 2] & 0xf) << 8) + buf_u8[i * 4 + 3];
                }
            }
        }
        else
            pnum = 0;

        for (i = 0; i < (pnum < max ? pnum : max); i++)
        {
            save = buf[i * 2];
            buf[i * 2] = buf[i * 2 + 1];
            buf[i * 2 + 1] = save;
        }
    }
    else
        pnum = 0;
    SystemTimer_Delay_Ms(10);

    return pnum;
}

static void Method_Write_Registers(ft5206 *inst, uint8_t reg_addr, uint8_t *buf, uint8_t len);
static void Method_Read_Registers(ft5206 *inst, uint8_t reg_addr, uint8_t *buf, uint8_t len);
static uint8_t Method_Get_Coordinates(ft5206 *inst, uint8_t max_num, uint16_t *xy);

void Method_Write_Registers(ft5206 *inst, uint8_t reg_addr, uint8_t *buf, uint8_t len)
{
    /*u8 i;
	u8 ret=0;
	IIC_Start(SCL,SDA);	 
	IIC_Send_Byte(SCL,SDA,FT_CMD_WR);	//发送写命令 	 
	IIC_Wait_Ack(SCL,SDA); 	 										  		   
	IIC_Send_Byte(SCL,SDA,reg&0XFF);   	//发送低8位地址
	IIC_Wait_Ack(SCL,SDA);  
	for(i=0;i<len;i++)
	{	   
    	IIC_Send_Byte(SCL,SDA,buf[i]);  	//发数据
		ret=IIC_Wait_Ack(SCL,SDA);
		if(ret)break;  
	}
    IIC_Stop(SCL,SDA);					//产生一个停止条件	  
	return ret;*/

    inst->IIC_Controller->Send_Data(inst->IIC_Controller, &reg_addr, 1, Enable, Enable, Disable);
    inst->IIC_Controller->Send_Data(inst->IIC_Controller, buf, len, Disable, Disable, Enable);
}

void Method_Read_Registers(ft5206 *inst, uint8_t reg_addr, uint8_t *buf, uint8_t len)
{
    /*	u8 i; 
 	IIC_Start(SCL,SDA);	
 	IIC_Send_Byte(SCL,SDA,FT_CMD_WR);   	//发送写命令 	 
	IIC_Wait_Ack(SCL,SDA); 	 										  		   
 	IIC_Send_Byte(SCL,SDA,reg&0XFF);   	//发送低8位地址
	IIC_Wait_Ack(SCL,SDA);  
 	IIC_Start(SCL,SDA);  	 	   
	IIC_Send_Byte(SCL,SDA,FT_CMD_RD);   	//发送读命令		   
	IIC_Wait_Ack(SCL,SDA);	   
	for(i=0;i<len;i++)
	{	   
    	buf[i]=IIC_Read_Byte(SCL,SDA,i==(len-1)?0:1); //发数据	  
	} 
    IIC_Stop(SCL,SDA);//产生一个停止条件     */

    inst->IIC_Controller->Send_Data(inst->IIC_Controller, &reg_addr, 1, Enable, Enable, Disable);
    inst->IIC_Controller->Receive_Data(inst->IIC_Controller, buf, len, Enable, Enable, Enable);
}

uint8_t Method_Get_Coordinates(ft5206 *inst, uint8_t max_num, uint16_t *xy)
{
    u8 i, pnum;
    u16 save;
    u8 *buf_u8 = (u8 *)xy;

    Method_Read_Registers(inst, 0x02, &pnum, 1);
    if (pnum != 0xff)
    {
        if (pnum != 0)
        {
            if (pnum > max_num)
            {
                for (i = 0; i < max_num; i++)
                    Method_Read_Registers(inst, 0x03 + 0x06 * i, buf_u8 + i * 4, 4);
                for (i = 0; i < max_num; i++)
                {
                    xy[i * 2] = ((buf_u8[i * 4] & 0xf) << 8) + buf_u8[i * 4 + 1];
                    xy[i * 2 + 1] = ((buf_u8[i * 4 + 2] & 0xf) << 8) + buf_u8[i * 4 + 3];
                }
            }
            else
            {
                for (i = 0; i < pnum; i++)
                    Method_Read_Registers(inst, 0x03 + 0x06 * i, buf_u8 + i * 4, 4);
                for (i = 0; i < pnum; i++)
                {
                    xy[i * 2] = ((buf_u8[i * 4] & 0xf) << 8) + buf_u8[i * 4 + 1];
                    xy[i * 2 + 1] = ((buf_u8[i * 4 + 2] & 0xf) << 8) + buf_u8[i * 4 + 3];
                }
            }
        }
        else
            pnum = 0;

        for (i = 0; i < (pnum < max_num ? pnum : max_num); i++)
        {
            save = xy[i * 2];
            xy[i * 2] = xy[i * 2 + 1];
            xy[i * 2 + 1] = save;
        }
    }
    else
        pnum = 0;
    SystemTimer_Delay_Ms(10);

    return pnum;
}

void FT5206_Prepare(ft5206 *inst, gpio rst, softiic *iic)
{
    u8 temp[5];

    inst->IIC_Controller = iic;
    inst->IIC_Controller->ValidTime = 5;
    inst->IIC_Controller->InvalidTime = 5;
    inst->IIC_Controller->Slave_Address = IIC_SLAVE_ADDRESS;

    _RO_WRITE(inst->RST, gpio, rst);

    inst->Read_Registers = Method_Read_Registers;
    inst->Write_Registers = Method_Write_Registers;
    inst->Get_Coordinates = Method_Get_Coordinates;

    GPIO_Write(inst->RST, 0);
    SystemTimer_Delay_Ms(20);
    GPIO_Write(inst->RST, 1);
    SystemTimer_Delay_Ms(50);
    temp[0] = 0;
    Method_Read_Registers(inst, FT_ID_G_LIB_VERSION, &temp[0], 2);
    if ((temp[0] == 0X30 && temp[1] == 0X03) || temp[1] == 0X01 || temp[1] == 0X02) //版本:0X3003/0X0001/0X0002
    {
        Method_Write_Registers(inst, FT_DEVIDE_MODE, temp, 1);  //进入正常操作模式
        Method_Write_Registers(inst, FT_ID_G_MODE, temp, 1);    //查询模式
        temp[0] = 22;                                           //触摸有效值，22，越小越灵敏
        Method_Write_Registers(inst, FT_ID_G_THGROUP, temp, 1); //设置触摸有效值
        temp[0] = 12;                                           //激活周期，不能小于12，最大14
        Method_Write_Registers(inst, FT_ID_G_PERIODACTIVE, temp, 1);
    }

    /*u8 res = 1;
    GPIO_Init(RST, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_FAST, GPIO_PUPD_PU); //PI8设置为推挽输出
    IIC_Init(SCL, SDA);                                                          //初始化电容屏的I2C总线
    GPIO_Pin_Reset(RST);
    delay_ms(20);
    GPIO_Pin_Set(RST);
    delay_ms(50);
    temp[0] = 0;
    //读取版本号，参考值：0x3003
    FT5206_RD_Reg(FT_ID_G_LIB_VERSION, &temp[0], 2);
    if ((temp[0] == 0X30 && temp[1] == 0X03) || temp[1] == 0X01 || temp[1] == 0X02) //版本:0X3003/0X0001/0X0002
    {
        FT5206_WR_Reg(FT_DEVIDE_MODE, temp, 1);  //进入正常操作模式
        FT5206_WR_Reg(FT_ID_G_MODE, temp, 1);    //查询模式
        temp[0] = 22;                            //触摸有效值，22，越小越灵敏
        FT5206_WR_Reg(FT_ID_G_THGROUP, temp, 1); //设置触摸有效值
        temp[0] = 12;                            //激活周期，不能小于12，最大14
        FT5206_WR_Reg(FT_ID_G_PERIODACTIVE, temp, 1);
        res = 0;
    }
    return res;*/
}
