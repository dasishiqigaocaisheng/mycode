#include "sys.h"
#include "delay.h"
#include "USART.h"
#include <string.h>
#include <stdarg.h>

//���ڽ��ճ�ʱ��ֵ
#define USART_TIME_OUT_VALUE	1000000

/*#if defined STM32F407xx
#define APB1_MAX_CLOCK 42000000.0f
#define APB2_MAX_CLOCK 84000000.0f
#elif defined STM32F429xx
#define APB1_MAX_CLOCK 45000000.0f
#define APB2_MAX_CLOCK 90000000.0f
#endif*/

/*����_USART_Send_Integer��Send_Mode�����ĺ�*/
#define USART_INT_FORMAT_DEC 	1	//�з���ʮ����
#define USART_INT_FORMAT_UDEC	2	//�޷���ʮ����
#define USART_INT_FORMAT_HEX 	3	//ʮ������

/*�����ʽ�����ת���ַ�����*/
#define IO_TYPE_DEC_INT		1	//%d���з���ʮ��������
#define IO_TYPE_HEX_INT		2	//%x��ʮ����������
#define IO_TYPE_DEC_UINT	3	//%u���޷���ʮ��������
#define IO_TYPE_FLOAT		4	//%f��%.xf�������ȸ�����
#define IO_TYPE_STRING		5	//%s���ַ���
#define IO_TYPE_PERCENT		6	//%%���ٷֺ�

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
void _USART_Send_Byte(USART_TypeDef* USART, u8 DATA);

/*��ʽ������һ������*/
void _USART_Send_Integer(USART_TypeDef* USART, u8 Send_Mode, int DATA1, u32 DATA2);

/*��ʽ������һ�������ȸ�����*/
void _USART_Send_Float(USART_TypeDef* USART, float DATA, u8 ACCURACY);

/*��ʽ������һ���ַ���*/
void _USART_Send_String(USART_TypeDef* USART, char *DATA);

/*����һ���ֽ�*/
u8 _USART_Receive_Byte(USART_TypeDef* USART, u8* Data);

/*�Ӹ�ʽ���������л�ȡһ������*/
short _USART_Get_Integer(char* Cache, u8 Receive_Mode, int* DATA1, u32* DATA2);

/*�Ӹ�ʽ���������л�ȡһ�������ȸ�����*/
short _USART_Get_Float(char* Cache, u8 Bits, float* DATA);

/*�Ӹ�ʽ���������л�ȡһ���ַ���*/
short _USART_Get_String(char* Cache, char* DATA);

/*�ж�ת���ַ����Ͳ���ò���������ת���ַ�ռ�õ��ֽ���-1*/
u8 _USART_IO_Parament_Assert(char* Address, IO_Parament_Type* Parament_Info);

void USART_Init(USART_TypeDef* USART, u32 Boundrate)
{
	u16 USARTDIV_Int;	//USARTDIV��������
	u8 USARTDIV_Dec;	//USARTDIVС������
	float USARTDIV;
	
	if (USART==USART1)
		USARTDIV=APB2_MAX_CLOCK/(Boundrate*16);
	else if (USART==USART2)
		USARTDIV=APB1_MAX_CLOCK/(Boundrate*16);
	else if (USART==USART3)
		USARTDIV=APB1_MAX_CLOCK/(Boundrate*16);
	
	USARTDIV_Int=USARTDIV;
	USARTDIV_Dec=(USARTDIV-USARTDIV_Int)*16;
	USARTDIV_Int<<=4;
	USARTDIV_Int+=USARTDIV_Dec;
	
	if (USART==USART1)
	{
		RCC->AHB1ENR|=1<<0;   		//ʹ��PORTA��ʱ��  
		RCC->APB2ENR|=1<<4;  		//ʹ�ܴ���1ʱ�� 
		GPIO_Set(GPIOA,PIN9|PIN10,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);//PA9,PA10,���ù���,�������
		GPIO_AF_Set(GPIOA,9,7);		//PA9,AF7
		GPIO_AF_Set(GPIOA,10,7);	//PA10,AF7  	   
	}
	else if (USART==USART2)
	{
		RCC->AHB1ENR|=1<<0;   		//ʹ��PORTA��ʱ��  
		RCC->APB1ENR|=1<<17;  		//ʹ�ܴ���2ʱ�� 
		GPIO_Set(GPIOA,PIN2|PIN3,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);//PA2,PA3,���ù���,�������
		GPIO_AF_Set(GPIOA,2,7);		//PA2,AF7
		GPIO_AF_Set(GPIOA,3,7);		//PA3,AF7  	   
	}
	else if (USART==USART3)
	{
		RCC->AHB1ENR|=1<<1;			//ʹ��PORTB��ʱ��  
		RCC->APB1ENR|=1<<18;		//ʹ��USART3ʱ��
		GPIO_Set(GPIOB,PIN10|PIN11,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);//PB10,PB11,���ù���,�������
		GPIO_AF_Set(GPIOB,10,7);		//PB10,AF7
		GPIO_AF_Set(GPIOB,11,7);		//PB11,AF7 
	}
	
	USART->BRR=USARTDIV_Int; 	//����������	 
	USART->CR1&=~(1<<15); 		//����OVER8=0 
	USART->CR1|=1<<2;  		//���ڽ���ʹ��
	USART->CR1|=1<<3;  		//���ڷ���ʹ��
	USART->CR1|=1<<13;  		//����ʹ��
		
	USARTDIV_Dec=USART->SR;	//���������ɱ�־
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

void _USART_Send_Byte(USART_TypeDef* USART, u8 DATA)
{
	USART->DR=DATA;
	while (((USART->SR>>6)&1)==0);
}
	
void USART_Send_Data_Flow(USART_TypeDef* USART, void* DATA, u32 Length)
{
	u32 i;
	
	for (i=0;i<Length;i++)
	{
		USART->DR=((u8*)DATA)[i];
		while (((USART->SR>>6)&1)==0);
	}
}

void _USART_Send_String(USART_TypeDef* USART, char *DATA)
{
	u32 i;
	
	for (i=0;DATA[i]!='\0';i++)
	{
		USART->DR=DATA[i];
		while (((USART->SR>>6)&1)==0);
	}
}

void _USART_Send_Integer(USART_TypeDef* USART,u8 Send_Mode, int DATA1, u32 DATA2)
{
	signed char i;
	u8 DATA_Bits[10];
		
	if (Send_Mode==USART_INT_FORMAT_HEX)//�����ʮ������ģʽ
	{
		_USART_Send_Byte(USART,'0');
		_USART_Send_Byte(USART,'x');
		for (i=0;i<8;i++)
		{
			DATA_Bits[0]=(DATA2>>((7-i)*4))&0xf;
			_USART_Send_Byte(USART,(DATA_Bits[0]%10)+0x30+(DATA_Bits[0]/10)*0x11);
		}
	}
	else
	{
		if (Send_Mode==USART_INT_FORMAT_DEC)//������з���ģʽ
		{
			if (DATA1<0)//�����ж�
			{
				_USART_Send_Byte(USART,'-');
				DATA1*=-1;
			}
			//�з�����
			DATA_Bits[9]=DATA1/1000000000;
			DATA_Bits[8]=(DATA1%1000000000)/100000000;
			DATA_Bits[7]=(DATA1%100000000)/10000000;
			DATA_Bits[6]=(DATA1%10000000)/1000000;
			DATA_Bits[5]=(DATA1%1000000)/100000;
			DATA_Bits[4]=(DATA1%100000)/10000;
			DATA_Bits[3]=(DATA1%10000)/1000;
			DATA_Bits[2]=(DATA1%1000)/100;
			DATA_Bits[1]=(DATA1%100)/10;
			DATA_Bits[0]=DATA1%10;
		}
		else if (Send_Mode==USART_INT_FORMAT_UDEC)//������޷���ģʽ
		{
			//���Է���λ
			DATA_Bits[9]=(u32)DATA2/1000000000;
			DATA_Bits[8]=((u32)DATA2%1000000000)/100000000;
			DATA_Bits[7]=((u32)DATA2%100000000)/10000000;
			DATA_Bits[6]=((u32)DATA2%10000000)/1000000;
			DATA_Bits[5]=((u32)DATA2%1000000)/100000;
			DATA_Bits[4]=((u32)DATA2%100000)/10000;
			DATA_Bits[3]=((u32)DATA2%10000)/1000;
			DATA_Bits[2]=((u32)DATA2%1000)/100;
			DATA_Bits[1]=((u32)DATA2%100)/10;
			DATA_Bits[0]=(u32)DATA2%10;
		}
			
		for (i=9;i>0;i--)//Ѱ����߷���λ
		{
			if (DATA_Bits[i]!=0)
				break;
		}
		
		for (;i>=0;i--)
			_USART_Send_Byte(USART,DATA_Bits[i]+0x30);
	}
}

void _USART_Send_Float(USART_TypeDef* USART, float DATA, u8 ACCURACY)
{
	u8 i;

	if (DATA<0)//�����ж�
	{
		_USART_Send_Byte(USART,'-');
		DATA*=-1;
	}
		
	_USART_Send_Integer(USART,USART_INT_FORMAT_UDEC,NULL,(u32)DATA);//������������
	_USART_Send_Byte(USART,'.');	
	
	ACCURACY&=0x07;
	DATA-=(int)DATA;
	for (i=0;i<ACCURACY;i++)//����С������
	{
		DATA*=10;
		_USART_Send_Byte(USART,(int)DATA+0x30);
		DATA-=(int)DATA;
	}
}

u8 _USART_IO_Parament_Assert(char* Address, IO_Parament_Type* Parament_Info)
{
	switch (*(Address+1))
	{
		case 'd':
		{
			Parament_Info->Parament_Type=IO_TYPE_DEC_INT;
			return 1;
		}
		case 'u':
		{
			Parament_Info->Parament_Type=IO_TYPE_DEC_UINT;
			return 1;
		}
		case 'x':
		{
			Parament_Info->Parament_Type=IO_TYPE_HEX_INT;
			return 1;
		}
		case 'f':
		{
			Parament_Info->Parament_Type=IO_TYPE_FLOAT;
			Parament_Info->Float_Dec_Num=3;//δָ��С��λ����Ĭ��3λ
			return 1;
		}
		case '.':
		{
			if ((*(Address+2)<=0x37)&&(*(Address+2)>=0x30))
			{
				if (*(Address+3)=='f')
				{
					Parament_Info->Parament_Type=IO_TYPE_FLOAT;
					Parament_Info->Float_Dec_Num=*(Address+2)-0x30;
					return 3;
				}
			}
			break;
		}
		case 's':
		{
			Parament_Info->Parament_Type=IO_TYPE_STRING;
			return 1;
		}
		case '%':
		{
			Parament_Info->Parament_Type=IO_TYPE_PERCENT;
			return 1;
		}
		default:
			break;
	}
	
	return 0;
}

void USART_Printf(USART_TypeDef* USART, char* Format_String, ...)
{
	u8 save;
	u32 i;
	va_list Parament_List;
	IO_Parament_Type Para_Info;
	
	va_start(Parament_List,Format_String);
	
	i=0;
	while (Format_String[i]!='\0')//���ֽڼ��
	{
		if (Format_String[i]!='%')
		{
			USART->DR=Format_String[i];
			while (((USART->SR>>6)&1)==0);
		}
		else//����ת���ַ�
		{
			save=_USART_IO_Parament_Assert(Format_String+i,&Para_Info);
			switch (Para_Info.Parament_Type)
			{
				case IO_TYPE_DEC_INT:
				{
					_USART_Send_Integer(USART,USART_INT_FORMAT_DEC,va_arg(Parament_List,int),NULL);
					break;
				}
				case IO_TYPE_DEC_UINT:
				{
					_USART_Send_Integer(USART,USART_INT_FORMAT_UDEC,NULL,va_arg(Parament_List,u32));
					break;
				}
				case IO_TYPE_HEX_INT:
				{
					_USART_Send_Integer(USART,USART_INT_FORMAT_HEX,NULL,va_arg(Parament_List,u32));
					break;
				}
				case IO_TYPE_FLOAT:
				{
					_USART_Send_Float(USART,va_arg(Parament_List,double),Para_Info.Float_Dec_Num);
					break;
				}
				case IO_TYPE_STRING:
				{
					_USART_Send_String(USART,va_arg(Parament_List,char*));
					break;
				}
				case IO_TYPE_PERCENT:
				{
					_USART_Send_Byte(USART,'%');
					break;
				}
				default:
				{
					_USART_Send_Byte(USART,'%');
					break;
				}
			}	
			i+=save;
		}
		i++;
	}
	va_end(Parament_List);
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
}

short _USART_Get_Integer(char* Cache, u8 Receive_Mode, int* DATA1, u32* DATA2)
{
	int i;
	
	//�з���ʮ��������
	if (Receive_Mode==USART_INT_FORMAT_DEC)
	{
		*DATA1=0;
		//����Ǹ���
		if (Cache[0]=='-')
		{
			for (i=1;(Cache[i]<='9')&&(Cache[i]>='0');i++)
			{
				*DATA1*=10;
				*DATA1+=Cache[i]-0x30;
			}	
			*DATA1*=-1;
			
			//���������0���������쳣
			if (i==1)
				return -1;
			else
				return i-1;
		}
		else//���������
		{
			for (i=0;(Cache[i]<='9')&&(Cache[i]>='0');i++)
			{
				*DATA1*=10;
				*DATA1+=Cache[i]-0x30;
			}	
			
			return i-1;
		}
	}
	else if (Receive_Mode==USART_INT_FORMAT_UDEC)//�޷���ʮ��������
	{
		*DATA2=0;
		for (i=0;(Cache[i]>='0')&&(Cache[i]<='9');i++)
		{
			*DATA2*=10;
			*DATA2+=Cache[i]-0x30;
		}
		
		return i-1;
	}
	else if (Receive_Mode==USART_INT_FORMAT_HEX)//ʮ������
	{
		*DATA2=0;
		//�������0x��0X��ͷ�����
		if (!((Cache[0]=='0')&&((Cache[1]=='x')||(Cache[1]=='X'))))
			return -1;
		
		for (i=2;((Cache[i]<='9')&&(Cache[i]>='0'))||((Cache[i]<='F')&&(Cache[i]>='A'))||((Cache[i]<='f')&&(Cache[i]>='a'));i++)
		{
			*DATA2*=16;
			if ((Cache[i]<='9')&&(Cache[i]>='0'))
				*DATA2+=Cache[i]-0x30;
			else if ((Cache[i]<='F')&&(Cache[i]>='A'))
				*DATA2+=Cache[i]-0x37;
			else
				*DATA2+=Cache[i]-0x57;
		}
		
		//���������0���������쳣
		if (i==2)
			return -1;
		else
			return i-1;
	}
	
	return -1;
}

short _USART_Get_Float(char* Cache, u8 Bits, float* DATA)
{
	signed char save1=0;
	u8 save2;
	int i;
	float Dec_Part=0;
	
	*DATA=0;
	//����Ǹ���
	if (Cache[0]=='-')
	{
		for (i=1;(Cache[i]<='9')&&(Cache[i]>='0');i++)
		{
			*DATA*=10;
			*DATA+=Cache[i]-0x30;
		}
		
		//���������0���������쳣
		if (i==1)
			return -1;
		
		*DATA*=-1;
		//�����С����
		if (Cache[i]=='.')
		{
			for (i++;(Cache[i]<='9')&&(Cache[i]>='0')&&(save1<Bits);i++)
				save1++;
			if (save1==0)
				return -1;
			save2=save1;
			for (i--;save1>0;save1--,i--)
			{
				Dec_Part+=Cache[i]-0x30;
				Dec_Part*=0.1f;
			}
			
			*DATA-=Dec_Part;
			
			//����δ��ȡ��С��λ
			for (i+=save2+1;(Cache[i]<='9')&&(Cache[i]>='0');i++);
			
			return i-1;
		}
		else
			return i-1;
	}
	else 
	{
		for (i=0;(Cache[i]<='9')&&(Cache[i]>='0');i++)
		{
			*DATA*=10;
			*DATA+=Cache[i]-0x30;
		}
		
		if (i==0)
			return -1;
		
		if (Cache[i]=='.')
		{
			for (i++;(Cache[i]<='9')&&(Cache[i]>='0')&&(save1<Bits);i++)
				save1++;
			if (save1==0)
				return -1;
			save2=save1;
			for (i--;save1>0;save1--,i--)
			{
				Dec_Part+=Cache[i]-0x30;
				Dec_Part/=10.0f;
			}
			
			*DATA+=Dec_Part;
			
			//����δ��ȡ��С��λ
			for (i+=save2+1;(Cache[i]<='9')&&(Cache[i]>='0');i++);
			
			return i-1;
		}
		else
			return i-1;
	}
	
	return -1;
}

short _USART_Get_String(char* Cache, char* DATA)
{
	int i;
	
	//��ȡ���о���ʵ��������ַ���Ҳ���Ǵ�'!'��ʼ���ַ�
	for (i=0;Cache[i]>=0x21;i++)
		DATA[i]=Cache[i];
	Cache[i]='\0';
	
	return i-1;
}

u8 USART_Scanf(USART_TypeDef* USART, char* Format_String, ...)
{
	u8 save1,num=0;
	short save2;
	u32 length,j,i=0;
	va_list Parament_List;
	IO_Parament_Type Para_Info;
	
	va_start(Parament_List,Format_String);
	
	length=USART_Receive_Data_Flow_EndwithCharacter(USART,USART_Cache,0x0a,1);
	if (length==0)
		return num;
	
	//������ͷ���еĿհ��ַ�
	while ((USART_Cache[i]<=0x20)&&(i<length))
		i++;
	
	//������ʽ���ַ�����ÿһ���ַ�
	for (j=0;(Format_String[j]!='\0')&&(i<length);j++,i++)
	{
		//�������%
		if (Format_String[j]=='%')
		{
			save1=_USART_IO_Parament_Assert(Format_String+j,&Para_Info);
			switch (Para_Info.Parament_Type)
			{
				case IO_TYPE_DEC_INT:
				{
					save2=_USART_Get_Integer(USART_Cache+i,USART_INT_FORMAT_DEC,va_arg(Parament_List,int*),NULL);
					if (save2==-1)
						return num;
					else
						num++;
					break;
				}
				case IO_TYPE_DEC_UINT:
				{
					save2=_USART_Get_Integer(USART_Cache+i,USART_INT_FORMAT_UDEC,NULL,va_arg(Parament_List,u32*));
					if (save2==-1)
						return num;
					else
						num++;
					break;
				}
				case IO_TYPE_HEX_INT:
				{
					save2=_USART_Get_Integer(USART_Cache+i,USART_INT_FORMAT_HEX,NULL,va_arg(Parament_List,u32*));
					if (save2==-1)
						return num;
					else
						num++;
					break;
				}
				case IO_TYPE_FLOAT:
				{
					save2=_USART_Get_Float(USART_Cache+i,Para_Info.Float_Dec_Num,va_arg(Parament_List,float*));
					if (save2==-1)
						return num;
					else
						num++;
					break;
				}
				case IO_TYPE_STRING:
				{
					save2=_USART_Get_String(USART_Cache+i,va_arg(Parament_List,char*));
					if (save2==-1)
						return num;
					else
						num++;
					break;
				}
				case IO_TYPE_PERCENT:
				{
					if (USART_Cache[i]!='%')
						return num;
					save2=0;
					break;
				}
				default:
					break;
			}
			i+=save2;
			j+=save1;
		}
		else//����%������յ��ַ���Ƚ�
		{
			if (Format_String[j]!=USART_Cache[i])
				return num;
		}
	}
	
	return num;
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
	USART_Receive_Data_Flow_EndwithLenth(USART,Rec_Buf,4);
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
	while (_USART_Receive_Byte(USART)!=0xff);
	USART_Printf(USART,"/************���ڲ��Խ���************/\n");
}
#endif
