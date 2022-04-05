#include "ESP8266.h"
#include "Memory.h"
//#include "USART.h"
#include "delay.h"
#include "DMA.h"
#include <string.h>
#include <stdlib.h>

#define DEBUG
#define DEBUG_USART	USART1

#define ESP8266_HEAP	HEAP0

#ifdef DEBUG
#define PRINT_EXCEPTION()				USART_Printf(DEBUG_USART,"Exception!\r\n")
#define PRINT_EXCEPTION_PALCE(func,num)	USART_Printf(DEBUG_USART,"Place:%s#%d\r\n",func,num)
#define PRINT_EXCEPTION_INFO(kind,info)	USART_Printf(DEBUG_USART,"Info:[%s]%s\r\n",kind,info)
#define PRINT_SUCCRESS_INFO(info)		USART_Printf(DEBUG_USART,"%s OK\r\n",info)
#else
#define PRINT_EXCEPTION()				while (0)
#define PRINT_EXCEPTION_PALCE(func,num)	while (0)
#define PRINT_EXCEPTION_INFO(kind,info)	while (0)
#define PRINT_SUCCRESS_INFO(info)		while (0)
#endif

#define SEND_AT_COMMAND(cmd)			USART_Printf(USED_USART,"AT+%s\r\n",cmd)

USART_TypeDef* USED_USART;
char* RecBuf1;
char RecBuf2[16];

u8 DevicesNum=0;
u8 APNum=0;
network_device* Ntkdvc_List=NULL;
AP* AP_List=NULL;

#ifdef DMA_MODE
tranplan tp;
u8* DMA_RecBuf;
u8 IsFull=0;
u16 Last_Index=0;
#endif

u8 _OK_Error_Check(char* Buf, char* place, u8 num, char* suc);

u8 _TimeOut_Check(u8 datlen, char* place, u8 num);

void _Get_AP_Info(char* info, AP* ap);

u8 _OK_Error_Check(char* Buf, char* place, u8 num, char* suc)
{
	if (strcmp(Buf,"OK\r\n")==0)
	{
		PRINT_SUCCRESS_INFO(suc);
		return ESP8266_OK;
	}
	if (strcmp(Buf,"ERROR\r\n")==0)
	{
		PRINT_EXCEPTION();
		PRINT_EXCEPTION_PALCE(place,num);
		PRINT_EXCEPTION_INFO("Error","");
		return ESP8266_ERROR;
	}
	PRINT_EXCEPTION();
	PRINT_EXCEPTION_PALCE(place,num);
	PRINT_EXCEPTION_INFO("ReturnError","");
	return ESP8266_RTNERR;
}

u8 _TimeOut_Check(u8 datlen, char* place, u8 num)
{
	if (datlen==0)
	{
		PRINT_EXCEPTION();
		PRINT_EXCEPTION_PALCE(place,num);
		PRINT_EXCEPTION_INFO("TimeOut","");
		return ESP8266_TIMOUT;
	}
	else
		return ESP8266_OK;
}

void _Get_AP_Info(char* info, AP* ap)
{
	u8 i,j;
	
	for (i=0;info[i]!='(';i++);
	ap->ecn=info[i+1]-0x30;
	for (i+=4,j=0;info[i]!='"';i++,j++)
		ap->ssid[j]=info[i];
	ap->ssid[j]='\0';
	ap->rssi=0;
	for (i+=3;info[i]!=',';i++)
	{
		ap->rssi*=10;
		ap->rssi-=info[i]-0x30;
	}
	for (i+=2,j=0;info[i]!='"';i++,j++)
		ap->MAC[j]=info[i];
	ap->MAC[j]='\0';
	ap->channel=0;
	for (i+=2;info[i]!=',';i++)
	{
		ap->channel*=10;
		ap->channel+=info[i]-0x30;
	}
}

int ESP8266_Init(USART_TypeDef* USART)
{
	int rtn=0;
	
	USART_Init(USART,115200);
	USED_USART=USART;
	USART_Time_Out_Set(USED_USART,1);
	RecBuf1=Malloc(ESP8266_HEAP,256);
	delay_ms(500);
	USART_Clear_RXNE(USED_USART);
	
	USART_Printf(USED_USART,"ATE0\r\n");
	rtn=USART_Receive_Data_Flow_EndwithCharacter(USED_USART,RecBuf1,'\n',1);
	RecBuf1[rtn]='\0';
	if (rtn==0)
	{
		PRINT_EXCEPTION();
		PRINT_EXCEPTION_PALCE("ESP8266_Init",1);
		PRINT_EXCEPTION_INFO("TimeOut","");
		return ESP8266_TIMOUT;
	}
	else if (rtn==6)
	{
		rtn=USART_Receive_Data_Flow_EndwithLength(USED_USART,RecBuf2,6);
		if (rtn==0)
		{
			PRINT_EXCEPTION();
			PRINT_EXCEPTION_PALCE("ESP8266_Init",2);
			PRINT_EXCEPTION_INFO("TimeOut","");
			return ESP8266_TIMOUT;
		}
		RecBuf2[rtn]='\0';
		if (strcmp(RecBuf2,"\r\nOK\r\n")==0)
		{
			PRINT_SUCCRESS_INFO("ATE0");
			return ESP8266_OK;
		}
		else
		{
			PRINT_EXCEPTION();
			PRINT_EXCEPTION_PALCE("ESP8266_Init",3);
			PRINT_EXCEPTION_INFO("ReturnError",RecBuf2);
			return ESP8266_RTNERR;
		}
	}
	else if (rtn==2)
	{
		rtn=USART_Receive_Data_Flow_EndwithLength(USED_USART,RecBuf2,4);
		RecBuf2[rtn]='\0';
		if (rtn==0)
		{
			PRINT_EXCEPTION();
			PRINT_EXCEPTION_PALCE("ESP8266_Init",4);
			PRINT_EXCEPTION_INFO("TimeOut","");
			return ESP8266_TIMOUT;
		}
		if (strcmp(RecBuf2,"OK\r\n")==0)
		{
			PRINT_SUCCRESS_INFO("ATE0");
			return ESP8266_OK;
		}
		PRINT_EXCEPTION();
		PRINT_EXCEPTION_PALCE("ESP8266_Init",5);
		PRINT_EXCEPTION_INFO("ReturnError",RecBuf2);
		return ESP8266_RTNERR;
	}
	else
	{
		PRINT_EXCEPTION();
		PRINT_EXCEPTION_PALCE("ESP8266_Init",6);
		PRINT_EXCEPTION_INFO("ReturnError",RecBuf1);
		return ESP8266_RTNERR;
	}

	return ESP8266_OK;
}

#ifdef INTERRUPT_MODE
void ESP8266_Interrupt_Set(u8 Group, u8 PrePriority, u8 SubPriority, Usart_IRQHandler CallBackfunc)
{
	USART_Interrupt_Set(USED_USART,Group,PrePriority,SubPriority,CallBackfunc);
	USART_Receive_Interrupt_Disable(USED_USART);
}

void ESP8266_Interrupt_Enable(void)
{
	USART_Receive_Interrupt_Enable(USED_USART);
}

void ESP8266_Interrupt_Disable(void)
{
	USART_Receive_Interrupt_Disable(USED_USART);
}
#endif

#ifdef DMA_MODE

void _DMA_CallbackFunc(void)
{
	//如果已经满了说明一直未读取
	//那么直接从头开始
	if (IsFull)
	{
		IsFull=0;
		Last_Index=0;
	}
	else
		IsFull=1;
}

void ESP8266_DMA_Set(u16 BufferSize, u16 DMA_Channel, u8 Group, u8 PrePriority, u8 SubPriority)
{
	DMA_RecBuf=Malloc(ESP8266_HEAP,BufferSize);
	
	DMA_Build_TransferPlan(&tp,DMA_Channel);
	
	tp.Controller.DataNumber=BufferSize;
	tp.Controller.Loop=1;
	tp.Controller.WordLength=BYTE;
	tp.End.Address=DMA_RecBuf;
	tp.End.IsFixed=0;
	tp.End.Type=TRANPORT_TYPE_MEM;
	tp.Start.Address=(void*)&USED_USART->DR;
	tp.Start.IsFixed=1;
	tp.Start.Type=TRANPORT_TYPE_PER;
	
	DMA_StreamController_Set(&tp);
	DMA_Interrupt_Set(&tp,Group,PrePriority,SubPriority,_DMA_CallbackFunc);
}

u16 ESP8266_DMA_Get_Data_Length(void)
{
	if (IsFull)
		return tp.Controller.DataNumber-Last_Index+DMA_Get_Transfer_Num(&tp);
	else
		return DMA_Get_Transfer_Num(&tp)-Last_Index;
}

void ESP8266_DMA_Get_Data(void* buf, u16 Length)
{
	u16 i;
	u8* r=buf;
	
	//如果已经满了,要看是否要读取两次
	if (IsFull)
	{
		//如果长度超出了缓冲区边界
		//要读取两次
		if (Length>tp.Controller.DataNumber-Last_Index)
		{
			for (i=0;i<tp.Controller.DataNumber-Last_Index;i++)
			{
				*r=DMA_RecBuf[Last_Index+i];
				r++;
			}
			for (i=0;i<Length-tp.Controller.DataNumber+Last_Index;i++)
			{
				*r=DMA_RecBuf[i];
				r++;
			}
			IsFull=0;
			Last_Index=Length-tp.Controller.DataNumber+Last_Index;
		}
		else//否则一次即可全部读出
		{
			for (i=0;i<Length;i++)
			{
				*r=DMA_RecBuf[Last_Index+i];
				r++;
			}
			Last_Index+=Length;
		}
	}
	else//没有满，一次读出
	{
		for (i=0;i<Length;i++)
		{
			*r=DMA_RecBuf[Last_Index+i];
			r++;
		}
		Last_Index+=Length;
	}
	//判断Last_Index是否出界
	if (Last_Index>=tp.Controller.DataNumber)
		Last_Index=0;
}

#endif

int ESP8266_Reset(void)
{
	int rtn=0;
	
	USART_Clear_RXNE(USED_USART);
	SEND_AT_COMMAND("RST");
	rtn=USART_Receive_Data_Flow_EndwithLength(USED_USART,RecBuf1,6);
	RecBuf1[rtn]='\0';
	if (rtn==0)
	{
		PRINT_EXCEPTION();
		PRINT_EXCEPTION_PALCE("ESP8266_Reset",1);
		PRINT_EXCEPTION_INFO("TimeOut","");
		return ESP8266_TIMOUT;
	}
	else if (strcmp(RecBuf1,"\r\nOK\r\n")==0)
	{
		delay_ms(500);
		PRINT_SUCCRESS_INFO("Reset");
		USART_Printf(USED_USART,"ATE0\r\n");
		rtn=USART_Receive_Data_Flow_EndwithLength(USED_USART,RecBuf1,13);
		RecBuf1[rtn]='\0';
		if (rtn==0)
		{
			PRINT_EXCEPTION();
			PRINT_EXCEPTION_PALCE("ESP8266_Reset",2);
			PRINT_EXCEPTION_INFO("TimeOut","");
			return ESP8266_TIMOUT;	
		}
		if (strcmp(RecBuf1,"\rATE0\r\n\r\nOK\r\n")==0)
		{
			PRINT_SUCCRESS_INFO("ATE0");
			return ESP8266_OK;
		}
		else
		{
			PRINT_EXCEPTION();
			PRINT_EXCEPTION_PALCE("ESP_Reset",3);
			PRINT_EXCEPTION_INFO("ReturnError",RecBuf1);
			return ESP8266_RTNERR;
		}
	}
	else
	{
		PRINT_EXCEPTION();
		PRINT_EXCEPTION_PALCE("ESP_Reset",4);
		PRINT_EXCEPTION_INFO("ReturnError",RecBuf1);
		return ESP8266_RTNERR;
	}	
}

int ESP8266_Mode_Set(u8 WIFI_Mode)
{
	int rtn;
	
	delay_ms(10);
	USART_Clear_RXNE(USED_USART);
	if (WIFI_Mode==MODE_AP)
	{
		SEND_AT_COMMAND("CWMODE=2");
		rtn=USART_Receive_Data_Flow_EndwithLength(USED_USART,RecBuf1,6);
		RecBuf1[rtn]='\0';
		if (rtn==0)
		{
			PRINT_EXCEPTION();
			PRINT_EXCEPTION_PALCE("ESP8266_Mode_Set",1);
			PRINT_EXCEPTION_INFO("TimeOut","");
			return ESP8266_TIMOUT;
		}
		if (strcmp(RecBuf1,"\r\nOK\r\n")==0)
		{
			PRINT_SUCCRESS_INFO("AP Mode Set");
			return ESP8266_OK;
		}
		PRINT_EXCEPTION();
		PRINT_EXCEPTION_PALCE("ESP8266_Mode_Set",2);
		PRINT_EXCEPTION_INFO("ReturnError",RecBuf1);
		return ESP8266_RTNERR;
	}
	else if (WIFI_Mode==MODE_STA)
	{
		SEND_AT_COMMAND("CWMODE=1");
		rtn=USART_Receive_Data_Flow_EndwithLength(USED_USART,RecBuf1,6);
		RecBuf1[rtn]='\0';
		if (rtn==0)
		{
			PRINT_EXCEPTION();
			PRINT_EXCEPTION_PALCE("ESP8266_Mode_Set",1);
			PRINT_EXCEPTION_INFO("TimeOut","");
			return ESP8266_TIMOUT;
		}
		if (strcmp(RecBuf1,"\r\nOK\r\n")==0)
		{
			PRINT_SUCCRESS_INFO("STA Mode Set");
			return ESP8266_OK;
		}
		PRINT_EXCEPTION();
		PRINT_EXCEPTION_PALCE("ESP8266_Mode_Set",2);
		PRINT_EXCEPTION_INFO("ReturnError",RecBuf1);
		return ESP8266_RTNERR;
	}
	return 0;
}

int ESP8266_AP_Config(char* ssid, char* pwd, u8 channel, u8 ecn)
{
	int rtn;
	
	delay_ms(10);
	USART_Clear_RXNE(USED_USART);
	USART_Printf(USED_USART,"AT+CWSAP=\"%s\",\"%s\",%d,%d\r\n",ssid,pwd,channel,ecn);
	USART_Receive_Data_Flow_EndwithLength(USED_USART,RecBuf1,2);
	rtn=USART_Receive_Data_Flow_EndwithCharacter(USED_USART,RecBuf1,'\n',1);
	RecBuf1[rtn]='\0';
	if (rtn==0)
	{
		PRINT_EXCEPTION();
		PRINT_EXCEPTION_PALCE("ESP8266_AP_Set",1);
		PRINT_EXCEPTION_INFO("TimeOut","");
		return ESP8266_TIMOUT;
	}
	return _OK_Error_Check(RecBuf1,"ESP8266_AP_Set",2,"AP Set");
}

int ESP8266_AP_IP_Set(char* IP)
{
	int rtn;
	
	delay_ms(10);
	USART_Clear_RXNE(USED_USART);
	USART_Printf(USED_USART,"AT+CIPAP=\"%s\"\r\n",IP);
	USART_Receive_Data_Flow_EndwithLength(USED_USART,RecBuf1,2);
	rtn=USART_Receive_Data_Flow_EndwithCharacter(USED_USART,RecBuf1,'\n',1);
	RecBuf1[rtn]='\0';
	if (_TimeOut_Check(rtn,"ESP8226_AP_IP_Set",1)==ESP8266_TIMOUT)
		return ESP8266_TIMOUT;
	return _OK_Error_Check(RecBuf1,"ESP8266_AP_IP_Set",2,"AP IP Set");
}

int ESP8266_AP_Get_Devices(void)
{
	int rtn;
	u8 num,j,i;
	network_device* nd;
	
	delay_ms(10);
	USART_Clear_RXNE(USED_USART);
	RecBuf1[0]=0;
	SEND_AT_COMMAND("CWLIF");
	for (num=0,rtn=0,i=0;RecBuf1[rtn-i]!='\r';num++)
	{
		i=USART_Receive_Data_Flow_EndwithCharacter(USED_USART,RecBuf1+rtn,'\n',1);
		if (_TimeOut_Check(i,"ESP8266_AP_Get_Devices",1)==ESP8266_TIMOUT)
			return ESP8266_TIMOUT;
		else
			rtn+=i;
	}
	num--;
	rtn=USART_Receive_Data_Flow_EndwithCharacter(USED_USART,RecBuf2,'\n',1);
	RecBuf2[rtn]='\0';
	if (_TimeOut_Check(rtn,"ESP8266_AP_Get_Devices",2)==ESP8266_TIMOUT)
		return ESP8266_TIMOUT;
	if ((rtn=_OK_Error_Check(RecBuf2,"ESP8266_AP_Get_Devices",3,"Get AP Devices List"))!=ESP8266_OK)
		return rtn;
	nd=Malloc(ESP8266_HEAP,num*sizeof(network_device));
	for (i=0,rtn=0;i<num;i++)
	{
		for (j=0;RecBuf1[rtn+j]!=',';j++)
			nd[i].IP[j]=RecBuf1[rtn+j];
		nd[i].IP[j]='\0';
		rtn+=j+1;
		for (j=0;RecBuf1[rtn+j]!='\r';j++)
			nd[i].MAC[j]=RecBuf1[rtn+j];
		nd[i].MAC[j]='\0';
		rtn+=j+2;
	}
	#ifdef DEBUG
	USART_Printf(DEBUG_USART,"%d Devices Found\r\n",num);
	for (i=0;i<num;i++)
		USART_Printf(DEBUG_USART,"IP:%s  MAC:%s\r\n",nd[i].IP,nd[i].MAC);
	#endif
	Free(ESP8266_HEAP,Ntkdvc_List);
	Ntkdvc_List=nd;
	DevicesNum=num;
	return ESP8266_OK;
}

int ESP8266_Multiple_Connect(u8 Enable)
{
	int rtn;
	
	delay_ms(10);
	USART_Clear_RXNE(USED_USART);	
	USART_Printf(USED_USART,"AT+CIPMUX=%d\r\n",Enable);
	USART_Receive_Data_Flow_EndwithLength(USED_USART,RecBuf1,2);
	rtn=USART_Receive_Data_Flow_EndwithCharacter(USED_USART,RecBuf1,'\n',1);
	RecBuf1[rtn]='\0';
	if (_TimeOut_Check(rtn,"ESP8266_Multiple_Connect",1)==ESP8266_TIMOUT)
		return ESP8266_TIMOUT;
	return _OK_Error_Check(RecBuf1,"ESP8266_Multiple_Connect",2,"Multiple Connect");
}

int ESP8266_Get_APs(void)
{
	int rtn,j,k;
	u8 num,i;
	u8* recbuf;
	AP* ap;
	
	delay_ms(10);
	USART_Clear_RXNE(USED_USART);
	USART_Time_Out_Set(USED_USART,0);
	recbuf=Malloc(ESP8266_HEAP,3072);
	recbuf[0]=0;
	USART_Printf(DEBUG_USART,"Searching AP\r\n");
	SEND_AT_COMMAND("CWLAP");
	for (num=0,rtn=0,i=0;recbuf[rtn-i]!='\r';num++)
	{
		i=USART_Receive_Data_Flow_EndwithCharacter(USED_USART,recbuf+rtn,'\n',1);
		rtn+=i;
	}
	rtn=USART_Receive_Data_Flow_EndwithCharacter(USED_USART,RecBuf1,'\n',1);
	RecBuf1[rtn]='\0';
	if ((rtn=_OK_Error_Check(RecBuf1,"ESP8266_Get_APs",1,"Get APs"))!=ESP8266_OK)
		return rtn;
	num--;
	ap=Malloc(ESP8266_HEAP,num*sizeof(AP));
	for (i=0,j=0;i<num;i++,j++)
	{
		for (;recbuf[j]!='+';j++);
		_Get_AP_Info((char*)&recbuf[j],&ap[i]);
	}
	Free(ESP8266_HEAP,AP_List);
	Free(ESP8266_HEAP,recbuf);
	AP_List=ap;
	APNum=num;
	USART_Time_Out_Set(USED_USART,1);
	#ifdef DEBUG
	USART_Printf(DEBUG_USART,"%d AP Found\r\n",num);
	for (i=0;i<num;i++)
		USART_Printf(DEBUG_USART,"#%d ecn:%d ssid:%s rssi:%d mac:%s ch:%d\r\n",
						i+1,ap[i].ecn,ap[i].ssid,ap[i].rssi,ap[i].MAC,ap[i].channel);
	#endif
	return ESP8266_OK;
}

int ESP8266_Seek_AP(char* SSID, AP* ap)
{
	int rtn;
	u8 i,j;
	
	delay_ms(10);
	USART_Clear_RXNE(USED_USART);
	USART_Time_Out_Set(USED_USART,0);
	USART_Printf(USED_USART,"AT+CWLAP=\"%s\"\r\n",SSID);
	rtn=USART_Receive_Data_Flow_EndwithCharacter(USED_USART,RecBuf1,'\n',1);
	RecBuf1[rtn]='\0';
	USART_Time_Out_Set(USED_USART,1);
	if (rtn==2)
	{
		rtn=USART_Receive_Data_Flow_EndwithCharacter(USED_USART,RecBuf1,'\n',1);
		RecBuf1[rtn]='\0';
		if (_TimeOut_Check(rtn,"ESP8266_Seek_AP",1)!=ESP8266_OK)
			return ESP8266_TIMOUT;
		if ((rtn=_OK_Error_Check(RecBuf1,"ESP8266_Seek_AP",2,""))!=ESP8266_OK)
			return rtn;
		else
			return ESP8266_NOTFD;
		
	}
	else
	{
		USART_Receive_Data_Flow_EndwithLength(USED_USART,RecBuf2,2);
		rtn=USART_Receive_Data_Flow_EndwithCharacter(USED_USART,RecBuf2,'\n',1);
		RecBuf2[rtn]='\0';
		if (_TimeOut_Check(rtn,"ESP8266_Seek_AP",3)!=ESP8266_OK)
			return ESP8266_TIMOUT;
		if ((rtn=_OK_Error_Check(RecBuf2,"ESP8266_Seek_AP",4,"Seek AP\n"))!=ESP8266_OK)
			return rtn;
		_Get_AP_Info(RecBuf1,ap);
		#ifdef DEBUG
		USART_Printf(DEBUG_USART,"ecn:%d ssid:%s rssi:%d mac:%s ch:%d\r\n",
						ap->ecn,ap->ssid,ap->rssi,ap->MAC,ap->channel);
		#endif
		return ESP8266_OK;
	}
	delay_ms(100);
}

int ESP8266_Join_AP(char* SSID, char* Password)
{
	u8 i=0,con=0;
	AP ap;
	int rtn;
	
	delay_ms(10);
	USART_Clear_RXNE(USED_USART);
	USART_Time_Out_Set(USED_USART,0);
	#ifdef DEBUG
	USART_Printf(DEBUG_USART,"Joining AP\r\n");
	#endif
	USART_Printf(USED_USART,"AT+CWJAP=\"%s\",\"%s\"\r\n",SSID,Password);
	rtn=USART_Receive_Data_Flow_EndwithCharacter(USED_USART,RecBuf1,'\n',1);
	RecBuf1[rtn]='\0';
	rtn=USART_Receive_Data_Flow_EndwithCharacter(USED_USART,RecBuf1+32,'\n',1);
	RecBuf1[rtn+32]='\0';
	rtn=USART_Receive_Data_Flow_EndwithCharacter(USED_USART,RecBuf2,'\n',1);
	RecBuf2[rtn]='\0';
	USART_Time_Out_Set(USED_USART,1);
	rtn=USART_Receive_Data_Flow_EndwithCharacter(USED_USART,RecBuf2,'\n',1);
	RecBuf2[rtn]='\0';
	if (rtn==0)
	{
		#ifdef DEBUG
		PRINT_EXCEPTION();
		PRINT_EXCEPTION_PALCE("ESP8266_Join_AP",1);
		PRINT_EXCEPTION_INFO("TimeOut","Join AP failed");
		USART_Printf(DEBUG_USART,"%s%s",RecBuf1,RecBuf1+32);
		#endif
		return ESP8266_JONFD;
	}
	else
		return _OK_Error_Check(RecBuf2,"ESP8266_Join_AP",2,"Join AP");
}

int ESP8266_Quit_AP(void)
{
	int rtn;
	
	delay_ms(10);
	USART_Clear_RXNE(USED_USART);
	SEND_AT_COMMAND("CWQAP");
	USART_Receive_Data_Flow_EndwithLength(USED_USART,RecBuf1,2);
	rtn=USART_Receive_Data_Flow_EndwithCharacter(USED_USART,RecBuf1,'\n',1);
	RecBuf1[rtn]='\0';
	rtn=USART_Receive_Data_Flow_EndwithCharacter(USED_USART,RecBuf2,'\n',1);
	RecBuf2[rtn]='\0';
	return _OK_Error_Check(RecBuf1,"ESP8266_Quit_AP",1,"Quit AP");
}

int ESP8266_STA_IP_Set(char* IP)
{
	int rtn;
	
	delay_ms(10);
	USART_Clear_RXNE(USED_USART);	
	USART_Printf(USED_USART,"AT+CIPSTA=\"%s\"\r\n",IP);
	USART_Receive_Data_Flow_EndwithLength(USED_USART,RecBuf1,2);
	rtn=USART_Receive_Data_Flow_EndwithCharacter(USED_USART,RecBuf1,'\n',1);
	RecBuf1[rtn]='\0';
	return _OK_Error_Check(RecBuf1,"ESP8266_STA_IP_Set",1,"STA IP Set");
}

int ESP8266_TCP_Client_Connect(char* IP, u16 Port)
{
	int rtn;
	
	delay_ms(10);
	USART_Clear_RXNE(USED_USART);	
	USART_Time_Out_Set(USED_USART,0);
	USART_Printf(USED_USART,"AT+CIPSTART=\"TCP\",\"%s\",%d\r\n",IP,Port);
	USART_Receive_Data_Flow_EndwithCharacter(USED_USART,RecBuf1,'\n',1);
	USART_Receive_Data_Flow_EndwithCharacter(USED_USART,RecBuf1,'\n',1);
	rtn=USART_Receive_Data_Flow_EndwithCharacter(USED_USART,RecBuf1,'\n',1);
	RecBuf1[rtn]='\0';
	USART_Time_Out_Set(USED_USART,1);
	return _OK_Error_Check(RecBuf1,"ESP8266_TCP_Client_Connect",1,"TCP Connect");
}

/*int ESP8266_TCP_Client_Close(u8 ID)
{

}*/

int ESP8266_TCP_Client_Close2(void)
{
	int rtn;
	
	delay_ms(10);
	USART_Clear_RXNE(USED_USART);	
	USART_Time_Out_Set(USED_USART,1);
	USART_Printf(USED_USART,"AT+CIPCLOSE\r\n");	
	USART_Receive_Data_Flow_EndwithCharacter(USED_USART,RecBuf1,'\n',1);
	USART_Receive_Data_Flow_EndwithCharacter(USED_USART,RecBuf1,'\n',1);
	rtn=USART_Receive_Data_Flow_EndwithCharacter(USED_USART,RecBuf1,'\n',1);
	RecBuf1[rtn]='\0';
	return _OK_Error_Check(RecBuf1,"ESP8266_TCP_Client_Close2",1,"Tcp Close");
}

int ESP8266_TCP_Server_Set(u8 ID, u16 Port)
{
	int rtn;
	
	delay_ms(10);
	USART_Clear_RXNE(USED_USART);	
	USART_Printf(USED_USART,"AT+CIPSERVER=%d,%d\r\n",ID,Port);
	USART_Receive_Data_Flow_EndwithLength(USED_USART,RecBuf1,2);
	rtn=USART_Receive_Data_Flow_EndwithCharacter(USED_USART,RecBuf1,'\n',1);
	RecBuf1[rtn]='\0';
	if (_TimeOut_Check(rtn,"ESP8266_TCP_Server_Set",1)!=ESP8266_OK)
		return ESP8266_TIMOUT;
	return _OK_Error_Check(RecBuf1,"ESP8266_TCP_Server_Set",2,"Server Set");
}

int ESP8266_Send_Mode_Set(u8 Enable)
{
	int rtn;
	
	delay_ms(10);
	USART_Clear_RXNE(USED_USART);	
	USART_Printf(USED_USART,"AT+CIPMODE=%d\r\n",Enable);
	USART_Receive_Data_Flow_EndwithLength(USED_USART,RecBuf1,2);
	rtn=USART_Receive_Data_Flow_EndwithCharacter(USED_USART,RecBuf1,'\n',1);
	RecBuf1[rtn]='\0';
	return _OK_Error_Check(RecBuf1,"ESP8266_Send_Mode_Set",1,"Send Mode Set");
	
}

int ESP8266_TranspSend_Set(u8 Enable)
{
	if (Enable)
	{
		SEND_AT_COMMAND("CIPSEND");
		USART_Receive_Data_Flow_EndwithLength(USED_USART,RecBuf1,9);
		
		#ifdef INTERRUPT_MODE
		USART_Receive_Interrupt_Enable(USED_USART);
		#endif
		
		#ifdef DMA_MODE
		USART_Receive_DMA_Enable(USED_USART);
		DMA_Clear_SR(&tp);
		DMA_Reload(&tp);
		DMA_Enable(&tp);
		#endif
		
		return ESP8266_OK;
	}
	else
	{
		delay_ms(30);
		USART_Printf(USED_USART,"+++");
		delay_ms(30);
		
		#ifdef INTERRUPT_MODE
		USART_Receive_Interrupt_Disable(USED_USART);
		#endif
		
		#ifdef DMA_MODE
		USART_Receive_DMA_Disable(USED_USART);
		DMA_Disable(&tp);
		while (DMA_Get_TCIF(&tp)==0);
		DMA_Clear_SR(&tp);
		#endif
		
		return ESP8266_OK;
	}
	
}

int ESP8266_Send_DataFlow1(u8 ID, void* Data, u16 Length)
{
	int rtn;
	u8 i,send_time;
	u16 j,send_len,remain_len;
	USART_Clear_RXNE(USED_USART);
	send_time=Length/2048+1;
	remain_len=Length%2048;
	for (i=0,j=0;i<send_time;i++,j+=send_len)
	{
		if (i!=send_time-1)
			send_len=2048;
		else
			send_len=remain_len;
		USART_Printf(USED_USART,"AT+CIPSEND=%d,%d\r\n",ID,send_len);
		rtn=USART_Receive_Data_Flow_EndwithLength(USED_USART,RecBuf1,8);
		if (_TimeOut_Check(rtn,"ESP8266_Send_DataFlow1",i)!=ESP8266_OK)
		{
			USART_Time_Out_Set(USED_USART,1);
			return ESP8266_TIMOUT;
		}
		USART_Send_Data_Flow(USED_USART,((u8*)Data)+j,send_len);
		USART_Receive_Data_Flow_EndwithLength(USED_USART,RecBuf1,2);
		USART_Receive_Data_Flow_EndwithCharacter(USED_USART,RecBuf1,'\n',1);
		USART_Receive_Data_Flow_EndwithCharacter(USED_USART,RecBuf1,'\n',1);
		USART_Receive_Data_Flow_EndwithCharacter(USED_USART,RecBuf1,'\n',1);
	}
	USART_Time_Out_Set(USED_USART,1);
	return ESP8266_OK;
}

int ESP8266_Send_DataFlow2(void* Data, u16 Length)
{
	int rtn;
	u8 i,send_time;
	u16 j,send_len,remain_len;
	USART_Clear_RXNE(USED_USART);
	USART_Time_Out_Set(USED_USART,0);
	send_time=Length/2048+1;
	remain_len=Length%2048;
	//USART_Printf(DEBUG_USART,"%d\n",send_time);
	for (i=0,j=0;i<send_time;i++,j+=send_len)
	{
		if (i!=send_time-1)
			send_len=2048;
		else
			send_len=remain_len;
		//USART_Printf(DEBUG_USART,"%d\n",send_len);
		USART_Printf(USED_USART,"AT+CIPSEND=%d\r\n",send_len);
		rtn=USART_Receive_Data_Flow_EndwithLength(USED_USART,RecBuf1,8);
		if (_TimeOut_Check(rtn,"ESP8266_Send_DataFlow2",i)!=ESP8266_OK)
		{
			USART_Time_Out_Set(USED_USART,1);
			return ESP8266_TIMOUT;
		}
		USART_Send_Data_Flow(USED_USART,((u8*)Data)+j,send_len);
		USART_Receive_Data_Flow_EndwithLength(USED_USART,RecBuf1,2);
		USART_Receive_Data_Flow_EndwithCharacter(USED_USART,RecBuf1,'\n',1);
		USART_Receive_Data_Flow_EndwithCharacter(USED_USART,RecBuf1,'\n',1);
		USART_Receive_Data_Flow_EndwithCharacter(USED_USART,RecBuf1,'\n',1);
	}
	USART_Time_Out_Set(USED_USART,1);
	return ESP8266_OK;
}

int ESP8266_TranpSend_DataFlow(void* Data, u16 Length)
{
	USART_Send_Data_Flow(USED_USART,Data,Length);
	return ESP8266_OK;
}
