#include "SoftSPI.h"



void Method_Transmit_DataFlow_CPHA1_LSB(softspi* hd, void* TxD, void* RxD, uint32_t len);	/*CPHA=1，LSB先发/收时的数据交换函数*/
void Method_Transmit_DataFlow_CPHA1_MSB(softspi* hd, void* TxD, void* RxD, uint32_t len);	/*CPHA=1，MSB先发/收时的数据交换函数*/
void Method_Transmit_DataFlow_CPHA0_LSB(softspi* hd, void* TxD, void* RxD, uint32_t len);	/*CPHA=0，LSB先发/收时的数据交换函数*/
void Method_Transmit_DataFlow_CPHA0_MSB(softspi* hd, void* TxD, void* RxD, uint32_t len);	/*CPHA=0，MSB先发/收时的数据交换函数*/


void Method_Transmit_DataFlow_CPHA1_LSB(softspi* hd, void* TxD, void* RxD, uint32_t len)
{
    int i,j,k;
    uint32_t txbuffer,rxbuffer;
    uint8_t* tx_addr=TxD;
    uint8_t* rx_addr=RxD;
        
    //接收地址为空，不接收，说明只发送
    if (RxD==0)
    {
		if (hd->Mode==SOFTSPI_MASTER)   //如果是主模式
		{
			if (hd->CS_Control)
				GPIO_Write(hd->CS,0);
			for (i=0;i<len;i++)
			{
				//将一帧数据取出
				for (k=0;k<hd->Buffer_Length;k++)
				{
					txbuffer<<=8;
					txbuffer|=tx_addr[k];
				}
				//送出数据
				for (j=0;j<hd->FrameLength;j++)
				{
					GPIO_Write(hd->SCLK,~hd->CPOL);
					GPIO_Write(hd->MOSI,txbuffer>>j);
					SystemTimer_Delay_Us(hd->ValidTime);
					GPIO_Write(hd->SCLK,hd->CPOL);
					SystemTimer_Delay_Us(hd->InvalidTime);
				 }
				tx_addr+=hd->Buffer_Length;
			}
			if (hd->CS_Control)
				GPIO_Write(hd->CS,1);
		}
		else if (hd->Mode==SOFTSPI_SLAVE)   //如果是从模式
		{
			if (hd->CS_Control)
				while (GPIO_Read(hd->CS)==1);
			for (i=0;i<len;i++)
			{
				//将一帧数据取出
				for (k=0;k<hd->Buffer_Length;k++)
				{
					txbuffer<<=8;
					txbuffer|=tx_addr[k];
				}
				//送出数据
				for (j=0;j<hd->FrameLength;j++)
				{
					while (GPIO_Read(hd->SCLK)==hd->CPOL);
					GPIO_Write(hd->MISO,txbuffer>>j);
					while (GPIO_Read(hd->SCLK)!=hd->CPOL);
				 }
				tx_addr+=hd->Buffer_Length;
			}
		}
    }
    else    //要接收，可能也要发送
    {
        if (TxD==0) //只接收，不发送
        {
			if (hd->Mode==SOFTSPI_MASTER)
			{
				if (hd->CS_Control)
					GPIO_Write(hd->CS,0);
				GPIO_Write(hd->MOSI,0);
				for (i=0;i<len;i++)
				{
					rxbuffer=0;
					for (j=0;j<hd->FrameLength;j++)
					{
						GPIO_Write(hd->SCLK,~hd->CPOL);
						SystemTimer_Delay_Us(hd->ValidTime);
						GPIO_Write(hd->SCLK,hd->CPOL);
						rxbuffer|=(GPIO_Read(hd->MISO)<<j);
						SystemTimer_Delay_Us(hd->InvalidTime);
					}
					for (k=0;k<hd->Buffer_Length;k++)
					{
						rx_addr[k]=rxbuffer;
						rxbuffer>>=8;
					}
					rx_addr+=hd->Buffer_Length;
				}
				if (hd->CS_Control)
					GPIO_Write(hd->CS,1);
			}
			else if (hd->Mode==SOFTSPI_SLAVE)
			{
				if (hd->CS_Control)
					while (GPIO_Read(hd->CS)==1);
				for (i=0;i<len;i++)
				{
					rxbuffer=0;
					for (j=0;j<hd->FrameLength;j++)
					{
						while (GPIO_Read(hd->SCLK)==hd->CPOL);
						while (GPIO_Read(hd->SCLK)!=hd->CPOL);
						rxbuffer|=(GPIO_Read(hd->MOSI)<<j);
					}
					for (k=0;k<hd->Buffer_Length;k++)
					{
						rx_addr[k]=rxbuffer;
						rxbuffer>>=8;
					}
					rx_addr+=hd->Buffer_Length;
				}
			}
        }
        else    //接收且发送
        {
			if (hd->Mode==SOFTSPI_MASTER)
			{
				if (hd->CS_Control)
					GPIO_Write(hd->CS,0);
				for (i=0;i<len;i++)
				{
					for (k=0;k<hd->Buffer_Length;k++)
					{
						txbuffer<<=8;
						txbuffer|=tx_addr[k];
					}
					rxbuffer=0;
					for (j=0;j<hd->FrameLength;j++)
					{
						GPIO_Write(hd->SCLK,~hd->CPOL);
						GPIO_Write(hd->MOSI,txbuffer>>j);
						SystemTimer_Delay_Us(hd->ValidTime);
						GPIO_Write(hd->SCLK,hd->CPOL);
						rxbuffer|=(GPIO_Read(hd->MISO)<<j);
						SystemTimer_Delay_Us(hd->InvalidTime);
					}
					for (k=0;k<hd->Buffer_Length;k++)
					{
						rx_addr[k]=rxbuffer;
						rxbuffer>>=8;
					}
					tx_addr+=hd->Buffer_Length;
					rx_addr+=hd->Buffer_Length;
				}
				if (hd->CS_Control)
					GPIO_Write(hd->CS,1);
			}
			else if (hd->Mode==SOFTSPI_SLAVE)
			{
				if (hd->CS_Control)
					while (GPIO_Read(hd->CS)==1);
				for (i=0;i<len;i++)
				{
					for (k=0;k<hd->Buffer_Length;k++)
					{
						txbuffer<<=8;
						txbuffer|=tx_addr[k];
					}
					rxbuffer=0;
					for (j=0;j<hd->FrameLength;j++)
					{
						while (GPIO_Read(hd->SCLK)==hd->CPOL);
						GPIO_Write(hd->MOSI,txbuffer>>j);
						while (GPIO_Read(hd->SCLK)!=hd->CPOL);
						rxbuffer|=(GPIO_Read(hd->MOSI)<<j);
					}
					for (k=0;k<hd->Buffer_Length;k++)
					{
						rx_addr[k]=rxbuffer;
						rxbuffer>>=8;
					}
					tx_addr+=hd->Buffer_Length;
					rx_addr+=hd->Buffer_Length;
				}
			}
        }
    }
}

void Method_Transmit_DataFlow_CPHA1_MSB(softspi* hd, void* TxD, void* RxD, uint32_t len)
{
    int i,j,k;
    uint32_t txbuffer,rxbuffer;
    uint8_t* tx_addr=TxD;
    uint8_t* rx_addr=RxD;
        
    //接收地址为空，不接收，说明只发送
    if (RxD==0)
    {
		if (hd->Mode==SOFTSPI_MASTER)
		{
			if (hd->CS_Control)
				GPIO_Write(hd->CS,0);
			for (i=0;i<len;i++)
			{
				for (k=0;k<hd->Buffer_Length;k++)
				{
					txbuffer<<=8;
					txbuffer|=tx_addr[k];
				}
				for (j=hd->FrameLength-1;j>=0;j--)
				{
					GPIO_Write(hd->SCLK,~hd->CPOL);
					GPIO_Write(hd->MOSI,txbuffer>>j);
					SystemTimer_Delay_Us(hd->ValidTime);
					GPIO_Write(hd->SCLK,hd->CPOL);
					SystemTimer_Delay_Us(hd->InvalidTime);
				}
				tx_addr+=hd->Buffer_Length;
			}
			if (hd->CS_Control)
				GPIO_Write(hd->CS,1);
		}
		else if (hd->Mode==SOFTSPI_SLAVE)
		{
			if (hd->CS_Control)
				while (GPIO_Read(hd->CS)==1);
			for (i=0;i<len;i++)
			{
				for (k=0;k<hd->Buffer_Length;k++)
				{
					txbuffer<<=8;
					txbuffer|=tx_addr[k];
				}
				for (j=hd->FrameLength-1;j>=0;j--)
				{
					while (GPIO_Read(hd->SCLK)==hd->CPOL);
					GPIO_Write(hd->MISO,txbuffer>>j);
					while (GPIO_Read(hd->SCLK)!=hd->CPOL);
				}
				tx_addr+=hd->Buffer_Length;
			}
		}
    }
    else    //要接收，可能也要发送
    {
        if (TxD==0) //只接收，不发送
        {
            if (hd->Mode==SOFTSPI_MASTER)
			{
				if (hd->CS_Control)
					GPIO_Write(hd->CS,0);
                GPIO_Write(hd->MOSI,0);
                for (i=0;i<len;i++)
                {
                    rxbuffer=0;
                    for (j=hd->FrameLength-1;j>=0;j--)
                    {
                        GPIO_Write(hd->SCLK,~hd->CPOL);
                        SystemTimer_Delay_Us(hd->ValidTime);
                        GPIO_Write(hd->SCLK,hd->CPOL);
                        rxbuffer|=(GPIO_Read(hd->MISO)<<j);
                        SystemTimer_Delay_Us(hd->InvalidTime);
                    }
                    for (k=0;k<hd->Buffer_Length;k++)
                    {
                        rx_addr[k]=rxbuffer;
                        rxbuffer>>=8;
                    }
                    rx_addr+=hd->Buffer_Length;
                }
				if (hd->CS_Control)
					GPIO_Write(hd->CS,1);				
			}
			else if (hd->Mode==SOFTSPI_SLAVE)
			{
				if (hd->CS_Control)
					while (GPIO_Read(hd->CS)==1);
				for (i=0;i<len;i++)
                {
                    rxbuffer=0;
                    for (j=hd->FrameLength-1;j>=0;j--)
                    {
						while (GPIO_Read(hd->SCLK)==hd->CPOL);
						while (GPIO_Read(hd->SCLK)!=hd->CPOL);
						rxbuffer|=(GPIO_Read(hd->MOSI)<<j);
                    }
                    for (k=0;k<hd->Buffer_Length;k++)
                    {
                        rx_addr[k]=rxbuffer;
                        rxbuffer>>=8;
                    }
                    rx_addr+=hd->Buffer_Length;
                }
			}
        }
        else    //接收且发送
        {
			if (hd->Mode==SOFTSPI_MASTER)
			{
				if (hd->CS_Control)
					GPIO_Write(hd->CS,0);
                for (i=0;i<len;i++)
                {
                    for (k=0;k<hd->Buffer_Length;k++)
                    {
                        txbuffer<<=8;
                        txbuffer|=tx_addr[k];
                    }
                    rxbuffer=0;
                    for (j=hd->FrameLength-1;j>=0;j--)
                    {
                        GPIO_Write(hd->SCLK,~hd->CPOL);
                        GPIO_Write(hd->MOSI,txbuffer>>j);
                        SystemTimer_Delay_Us(hd->ValidTime);
                        GPIO_Write(hd->SCLK,hd->CPOL);
                        rxbuffer|=(GPIO_Read(hd->MISO)<<j);
                        SystemTimer_Delay_Us(hd->InvalidTime);
                    }
                    for (k=0;k<hd->Buffer_Length;k++)
                    {
                        rx_addr[k]=rxbuffer;
                        rxbuffer>>=8;
                    }
                    tx_addr+=hd->Buffer_Length;
                    rx_addr+=hd->Buffer_Length;
                }
				if (hd->CS_Control)
					GPIO_Write(hd->CS,1);
			}
			else if (hd->Mode==SOFTSPI_SLAVE)
			{
				if (hd->CS_Control)
					while (GPIO_Read(hd->CS)==1);
				for (i=0;i<len;i++)
                {
                    for (k=0;k<hd->Buffer_Length;k++)
                    {
                        txbuffer<<=8;
                        txbuffer|=tx_addr[k];
                    }
                    rxbuffer=0;
                    for (j=hd->FrameLength-1;j>=0;j--)
                    {
						while (GPIO_Read(hd->SCLK)==hd->CPOL);
						GPIO_Write(hd->MISO,txbuffer>>j);
						while (GPIO_Read(hd->SCLK)!=hd->CPOL);
						rxbuffer|=(GPIO_Read(hd->MOSI)<<j);
                    }
                    for (k=0;k<hd->Buffer_Length;k++)
                    {
                        rx_addr[k]=rxbuffer;
                        rxbuffer>>=8;
                    }
                    tx_addr+=hd->Buffer_Length;
                    rx_addr+=hd->Buffer_Length;
                }
			}
        }
    }
}

void Method_Transmit_DataFlow_CPHA0_LSB(softspi* hd, void* TxD, void* RxD, uint32_t len)
{
    int i,j,k;
    uint32_t txbuffer,rxbuffer;
    uint8_t* tx_addr=TxD;
    uint8_t* rx_addr=RxD;
    
    //不接收，说明只发送
    if (RxD==0)
    {
		if (hd->Mode==SOFTSPI_MASTER)
		{
			if (hd->CS_Control)
				GPIO_Write(hd->CS,0);
            for (i=0;i<len;i++)
            {
                for (k=0;k<hd->Buffer_Length;k++)
                {
                    txbuffer<<=8;
                    txbuffer|=tx_addr[k];
                }
                for (j=0;j<hd->FrameLength;j++)
                {
                    GPIO_Write(hd->MOSI,txbuffer>>j);
                    GPIO_Write(hd->SCLK,~hd->CPOL);
                    SystemTimer_Delay_Us(hd->ValidTime);
                    GPIO_Write(hd->SCLK,hd->CPOL);
                    SystemTimer_Delay_Us(hd->InvalidTime);
                }
                tx_addr+=hd->Buffer_Length;
            }   
			if (hd->CS_Control)
				GPIO_Write(hd->CS,1);
        }
		else if (hd->Mode==SOFTSPI_SLAVE)
		{
			if (hd->CS_Control)
				while (GPIO_Read(hd->CS)==1);
			for (i=0;i<len;i++)
            {
                for (k=0;k<hd->Buffer_Length;k++)
                {
                    txbuffer<<=8;
                    txbuffer|=tx_addr[k];
                }
                for (j=0;j<hd->FrameLength;j++)
                {
					GPIO_Write(hd->MISO,txbuffer>>j);
					while (GPIO_Read(hd->SCLK)==hd->CPOL);
					while (GPIO_Read(hd->SCLK)!=hd->CPOL);
                }
                tx_addr+=hd->Buffer_Length;
            } 
		}
    }   
    else    //要接收，可能也要发送
    {
        if (TxD==0) //只接收，不发送
        {
			if (hd->Mode==SOFTSPI_MASTER)
			{
				if (hd->CS_Control)
					GPIO_Write(hd->CS,0);
                GPIO_Write(hd->MOSI,0);
                for (i=0;i<len;i++)
                {
                    rxbuffer=0;
                    for (j=0;j<hd->FrameLength;j++)
                    {
                        GPIO_Write(hd->SCLK,~hd->CPOL);
                        rxbuffer|=(GPIO_Read(hd->MISO)<<j);
                        SystemTimer_Delay_Us(hd->ValidTime);
                        GPIO_Write(hd->SCLK,hd->CPOL);
                        SystemTimer_Delay_Us(hd->InvalidTime);
                    }
                    for (k=0;k<hd->Buffer_Length;k++)
                    {
                        rx_addr[k]=rxbuffer;
                        rxbuffer>>=8;
                    }
                    rx_addr+=hd->Buffer_Length;
                }   
				if (hd->CS_Control)
					GPIO_Write(hd->CS,1);
            }
			else if (hd->Mode==SOFTSPI_SLAVE)
			{
				if (hd->CS_Control)
					while (GPIO_Read(hd->CS)==1);
				for (i=0;i<len;i++)
                {
                    rxbuffer=0;
                    for (j=0;j<hd->FrameLength;j++)
                    {
						while (GPIO_Read(hd->SCLK)==hd->CPOL);
						rxbuffer|=(GPIO_Read(hd->MOSI)<<j);
						while (GPIO_Read(hd->SCLK)!=hd->CPOL);
                    }
                    for (k=0;k<hd->Buffer_Length;k++)
                    {
                        rx_addr[k]=rxbuffer;
                        rxbuffer>>=8;
                    }
                    rx_addr+=hd->Buffer_Length;
                } 
			}
        }
        else    //接收且发送
        {
			if (hd->Mode==SOFTSPI_MASTER)
			{
				if (hd->CS_Control)
					GPIO_Write(hd->CS,0);
                for (i=0;i<len;i++)
                {
                    for (k=0;k<hd->Buffer_Length;k++)
                    {
                        txbuffer<<=8;
                        txbuffer|=tx_addr[k];
                    }
                    rxbuffer=0;
                    for (j=0;j<hd->FrameLength;j++)
                    {
                        GPIO_Write(hd->MOSI,txbuffer>>j);
                        GPIO_Write(hd->SCLK,~hd->CPOL);
                        rxbuffer|=(GPIO_Read(hd->MISO)<<j);
                        SystemTimer_Delay_Us(hd->ValidTime);
                        GPIO_Write(hd->SCLK,hd->CPOL);
                        SystemTimer_Delay_Us(hd->InvalidTime);
                    }
                    for (k=0;k<hd->Buffer_Length;k++)
                    {
                        rx_addr[k]=rxbuffer;
                        rxbuffer>>=8;
                    }
                    tx_addr+=hd->Buffer_Length;
                    rx_addr+=hd->Buffer_Length;
                }
				if (hd->CS_Control)
					GPIO_Write(hd->CS,1);
			}
			else if (hd->Mode==SOFTSPI_SLAVE)
			{
				if (hd->CS_Control)
					while (GPIO_Read(hd->CS)==1);
				for (i=0;i<len;i++)
                {
                    for (k=0;k<hd->Buffer_Length;k++)
                    {
                        txbuffer<<=8;
                        txbuffer|=tx_addr[k];
                    }
                    rxbuffer=0;
                    for (j=0;j<hd->FrameLength;j++)
                    {
						GPIO_Write(hd->MISO,txbuffer>>j);
						while (GPIO_Read(hd->SCLK)==hd->CPOL);
						rxbuffer|=(GPIO_Read(hd->MOSI)<<j);
						while (GPIO_Read(hd->SCLK)!=hd->CPOL);
                    }
                    for (k=0;k<hd->Buffer_Length;k++)
                    {
                        rx_addr[k]=rxbuffer;
                        rxbuffer>>=8;
                    }
                    tx_addr+=hd->Buffer_Length;
                    rx_addr+=hd->Buffer_Length;
                }
			}
        }
    }
}

void Method_Transmit_DataFlow_CPHA0_MSB(softspi* hd, void* TxD, void* RxD, uint32_t len)
{
    int i,j,k;
    uint32_t txbuffer,rxbuffer;
    uint8_t* tx_addr=TxD;
    uint8_t* rx_addr=RxD;
    
    //不接收，说明只发送
    if (RxD==0)
    {
		if (hd->Mode==SOFTSPI_MASTER)
		{
			if (hd->CS_Control)
				GPIO_Write(hd->CS,0);
            for (i=0;i<len;i++)
            {
                for (k=0;k<hd->Buffer_Length;k++)
                {
                    txbuffer<<=8;
                    txbuffer|=tx_addr[k];
                }
                for (j=hd->FrameLength-1;j>=0;j--)
                {
                    GPIO_Write(hd->MOSI,txbuffer>>j);
                    GPIO_Write(hd->SCLK,~hd->CPOL);
                    SystemTimer_Delay_Us(hd->ValidTime);
                    GPIO_Write(hd->SCLK,hd->CPOL);
                    SystemTimer_Delay_Us(hd->InvalidTime);
                }
                tx_addr+=hd->Buffer_Length;
            }   
			if (hd->CS_Control)
				GPIO_Write(hd->CS,1);
        }
		else if (hd->Mode==SOFTSPI_SLAVE)
		{
			if (hd->CS_Control)
				while (GPIO_Read(hd->CS)==1);
			for (i=0;i<len;i++)
            {
                for (k=0;k<hd->Buffer_Length;k++)
                {
                    txbuffer<<=8;
                    txbuffer|=tx_addr[k];
                }
                for (j=hd->FrameLength-1;j>=0;j--)
                {
					GPIO_Write(hd->MISO,txbuffer>>j);
					while (GPIO_Read(hd->SCLK)==hd->CPOL);
					while (GPIO_Read(hd->SCLK)!=hd->CPOL);
                }
                tx_addr+=hd->Buffer_Length;
            } 
		}
    }   
    else    //要接收，可能也要发送
    {
        if (TxD==0) //只接收，不发送
        {
			if (hd->Mode==SOFTSPI_MASTER)
			{
				if (hd->CS_Control)
					GPIO_Write(hd->CS,0);
                GPIO_Write(hd->MOSI,0);
                for (i=0;i<len;i++)
                {
                    rxbuffer=0;
                    for (j=hd->FrameLength-1;j>=0;j--)
                    {
                        GPIO_Write(hd->SCLK,~hd->CPOL);
                        rxbuffer|=(GPIO_Read(hd->MISO)<<j);
                        SystemTimer_Delay_Us(hd->ValidTime);
                        GPIO_Write(hd->SCLK,hd->CPOL);
                        SystemTimer_Delay_Us(hd->InvalidTime);
                    }
                    for (k=0;k<hd->Buffer_Length;k++)
                    {
                        rx_addr[k]=rxbuffer;
                        rxbuffer>>=8;
                    }
                    rx_addr+=hd->Buffer_Length;
                }   
				if (hd->CS_Control)
					GPIO_Write(hd->CS,1);
            }
			else if (hd->Mode==SOFTSPI_SLAVE)
			{
				if (hd->CS_Control)
					while (GPIO_Read(hd->CS)==1);
				for (i=0;i<len;i++)
                {
                    rxbuffer=0;
                    for (j=hd->FrameLength-1;j>=0;j--)
                    {
						while (GPIO_Read(hd->SCLK)==hd->CPOL);
						rxbuffer|=(GPIO_Read(hd->MOSI)<<j);
						while (GPIO_Read(hd->SCLK)!=hd->CPOL);
                    }
                    for (k=0;k<hd->Buffer_Length;k++)
                    {
                        rx_addr[k]=rxbuffer;
                        rxbuffer>>=8;
                    }
                    rx_addr+=hd->Buffer_Length;
                } 
			}
        }
        else    //接收且发送
        {
			if (hd->Mode==SOFTSPI_MASTER)
			{
				if (hd->CS_Control)
					GPIO_Write(hd->CS,0);
                for (i=0;i<len;i++)
                {
                    for (k=0;k<hd->Buffer_Length;k++)
                    {
                        txbuffer<<=8;
                        txbuffer|=tx_addr[k];
                    }
                    rxbuffer=0;
                    for (j=hd->FrameLength-1;j>=0;j--)
                    {
                        GPIO_Write(hd->MOSI,txbuffer>>j);
                        GPIO_Write(hd->SCLK,~hd->CPOL);
                        rxbuffer|=(GPIO_Read(hd->MISO)<<j);
                        SystemTimer_Delay_Us(hd->ValidTime);
                        GPIO_Write(hd->SCLK,hd->CPOL);
                        SystemTimer_Delay_Us(hd->InvalidTime);
                    }
                    for (k=0;k<hd->Buffer_Length;k++)
                    {
                        rx_addr[k]=rxbuffer;
                        rxbuffer>>=8;
                    }
                    tx_addr+=hd->Buffer_Length;
                    rx_addr+=hd->Buffer_Length;
                }
				if (hd->CS_Control)
					GPIO_Write(hd->CS,1);
			}
			else if (hd->Mode==SOFTSPI_SLAVE)
			{
				if (hd->CS_Control)
					while (GPIO_Read(hd->CS)==1);
				for (i=0;i<len;i++)
                {
                    for (k=0;k<hd->Buffer_Length;k++)
                    {
                        txbuffer<<=8;
                        txbuffer|=tx_addr[k];
                    }
                    rxbuffer=0;
                    for (j=hd->FrameLength-1;j>=0;j--)
                    {
						GPIO_Write(hd->MISO,txbuffer>>j);
						while (GPIO_Read(hd->SCLK)==hd->CPOL);
						rxbuffer|=(GPIO_Read(hd->MOSI)<<j);
						while (GPIO_Read(hd->SCLK)!=hd->CPOL);
                    }
                    for (k=0;k<hd->Buffer_Length;k++)
                    {
                        rx_addr[k]=rxbuffer;
                        rxbuffer>>=8;
                    }
                    tx_addr+=hd->Buffer_Length;
                    rx_addr+=hd->Buffer_Length;
                }
			}
        }
    }
}


void SoftSPI_Prepare(softspi* hd, softspi_mode_type mode, softspi_cpha_type cpha, softspi_cpol_type cpol, uint8_t frmlen, softspi_firstbit_type fsb,
                     gpio sclk, gpio mosi, gpio miso, gpio cs)
{
    _RO_WRITE(hd->Mode,softspi_mode_type,mode);
    _RO_WRITE(hd->CPHA,softspi_cpha_type,cpha);
    _RO_WRITE(hd->CPOL,softspi_cpol_type,cpol);
    _RO_WRITE(hd->FrameLength,uint8_t,frmlen);
    
    if ((cpha==CPHA0)&&(fsb==SOFTSPI_LSB))
        hd->Transmit_DataFlow=Method_Transmit_DataFlow_CPHA0_LSB;
	else if ((cpha==CPHA0)&&(fsb==SOFTSPI_MSB))
		hd->Transmit_DataFlow=Method_Transmit_DataFlow_CPHA0_MSB;
	else if ((cpha==CPHA1)&&(fsb==SOFTSPI_LSB))
		hd->Transmit_DataFlow=Method_Transmit_DataFlow_CPHA1_LSB;
    else if ((cpha==CPHA1)&&(fsb==SOFTSPI_MSB))
        hd->Transmit_DataFlow=Method_Transmit_DataFlow_CPHA1_MSB;
    
    _RO_WRITE(hd->SCLK,gpio,sclk);
    _RO_WRITE(hd->MOSI,gpio,mosi);
    _RO_WRITE(hd->MISO,gpio,miso);
    _RO_WRITE(hd->CS,gpio,cs);
    
    if (frmlen<=8)
        _RO_WRITE(hd->Buffer_Length,uint8_t,1);
    else if (frmlen<=16)
        _RO_WRITE(hd->Buffer_Length,uint8_t,2);
    else if (frmlen<=32)
        _RO_WRITE(hd->Buffer_Length,uint8_t,4);
    
    GPIO_Write(sclk,cpol);
}

