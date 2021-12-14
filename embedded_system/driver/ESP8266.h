#ifndef _ESP8266_H_
#define _ESP8266_H_

#include "sys.h"
#include "USART.h"

#define DMA_MODE
//#define INTERRUPT_MODE

#define MODE_AP		1
#define MODE_STA	2

#define ESP8266_OK		0
#define ESP8266_TIMOUT	1
#define ESP8266_RTNERR	2
#define ESP8266_ERROR	3
#define ESP8266_NOTFD	4
#define ESP8266_JONFD	5

#define OPEN			0
#define WEP				1
#define WPA_PSK			2
#define WPA2_PSK		3
#define WPA_WPA2_PSK	4

typedef struct Device_Class
{
	char IP[16];
	char MAC[18];
} network_device;

typedef struct AP_Class
{
	u8 ecn;
	char ssid[48];
	signed char rssi;
	char MAC[18];
	u8 channel;
} AP;

extern network_device* Ntkdvc_List;
extern AP* AP_List;

int ESP8266_Init(USART_TypeDef* USART);

#ifdef INTERRUPT_MODE
void ESP8266_Interrupt_Set(u8 Group, u8 PrePriority, u8 SubPriority, Usart_IRQHandler CallBackfunc);
void ESP8266_Interrupt_Enable(void);
void ESP8266_Interrupt_Disable(void);
#endif

#ifdef DMA_MODE
void ESP8266_DMA_Set(u16 BufferSize, u16 DMA_Channel, u8 Group, u8 PrePriority, u8 SubPriority);
u16 ESP8266_DMA_Get_Data_Length(void);
void ESP8266_DMA_Get_Data(void* buf, u16 Length);
#endif

int ESP8266_Reset(void);
int ESP8266_Mode_Set(u8 WIFI_Mode);
int ESP8266_AP_Config(char* ssid, char* pwd, u8 channel, u8 ecn);
int ESP8266_AP_IP_Set(char* IP);
int ESP8266_AP_Get_Devices(void);
int ESP8266_Multiple_Connect(u8 Enable);
int ESP8266_Get_APs(void);
int ESP8266_Seek_AP(char* SSID, AP* ap);
int ESP8266_Join_AP(char* SSID, char* Password);
int ESP8266_Quit_AP(void);
int ESP8266_STA_IP_Set(char* IP);
int ESP8266_TCP_Client_Connect(char* IP, u16 Port);
int ESP8266_TCP_Client_Close(u8 ID);
int ESP8266_TCP_Client_Close2(void);
int ESP8266_TCP_Server_Set(u8 Enable, u16 Port);
int ESP8266_Send_Mode_Set(u8 Enable);
int ESP8266_TranspSend_Set(u8 Enable);
int ESP8266_Send_DataFlow1(u8 ID, void* Data, u16 Length);
int ESP8266_Send_DataFlow2(void* Data, u16 Length);
int ESP8266_TranpSend_DataFlow(void* Data, u16 Length);

#endif
