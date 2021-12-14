#include "SoftSPI.h"








void Method_Transmit_DataFlow_CPHA_1(softspi* hd, void* TxD, void* RxD, uint32_t len);
void Method_Transmit_DataFlow_CPHA_0(softspi* hd, void* TxD, void* RxD, uint32_t len);



void SoftSPI_Prepare(softspi* hd, softspi_mode_type mode, softspi_cpha_type cpha, softspi_cpol_type cpol, uint8_t frmlen,
                     gpio_type sclk, gpio_type mosi, gpio_type miso, gpio_type cs)
{
    _RO_WRITE(hd->Mode,softspi_mode_type,mode);
    _RO_WRITE(hd->CPHA,softspi_cpha_type,cpha);
    _RO_WRITE(hd->CPOL,softspi_cpol_type,cpol);
    _RO_WRITE(hd->FrameLength,uint8_t,frmlen);
    
    if (cpha==CPHA0)
        hd->Transmit_DataFlow=Method_Transmit_DataFlow_CPHA_0;
    else if (cpha==CPHA1)
        hd->Transmit_DataFlow=Method_Transmit_DataFlow_CPHA_1;
    
    _RO_WRITE(hd->SCLK,gpio_type,sclk);
    _RO_WRITE(hd->MOSI,gpio_type,mosi);
    _RO_WRITE(hd->MISO,gpio_type,miso);
    _RO_WRITE(hd->CS,gpio_type,cs);
    
    if (frmlen<=8)
        _RO_WRITE(hd->Buffer_Length,uint8_t,1);
    else if (frmlen<=16)
        _RO_WRITE(hd->Buffer_Length,uint8_t,2);
    else if (frmlen<=32)
        _RO_WRITE(hd->Buffer_Length,uint8_t,4);
    
    GPIO_Write(sclk,cpol);
    
}

void Method_Transmit_DataFlow_CPHA_1(softspi* hd, void* TxD, void* RxD, uint32_t len)
{
    int i,j,k;
    uint32_t txbuffer,rxbuffer;
    uint8_t* tx_addr=TxD;
    uint8_t* rx_addr=RxD;
    
    if (hd->CS_Control)
        GPIO_Write(hd->CS,0);
    
    //不接收，说明只发送
    if (RxD==0)
    {
        if (hd->FirstBit==LSB)
        {
            for (i=0;i<len;i++)
            {
                for (k=0;k<hd->Buffer_Length;k++)
                {
                    txbuffer<<=8;
                    txbuffer|=tx_addr[k];
                }
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
        }
        else if (hd->FirstBit==MSB)
        {
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
        }
    }   
    else    //要接收，可能也要发送
    {
        if (TxD==0) //只接收，不发送
        {
            if (hd->FirstBit==LSB)
            {
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
            }
            else if (hd->FirstBit==MSB)
            {
                GPIO_Write(hd->MOSI,0);
                for (i=0;i<len;i++)
                {
                    rxbuffer=0;
                    for (j=hd->FrameLength;j>=0;j--)
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
            }
        }
        else    //接收且发送
        {
            if (hd->FirstBit==LSB)
            {
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
            }
            else if (hd->FirstBit==MSB)
            {
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
            }
        }
    }
    
    if (hd->CS_Control)
        GPIO_Write(hd->CS,1);
}


void Method_Transmit_DataFlow_CPHA_0(softspi* hd, void* TxD, void* RxD, uint32_t len)
{
    int i,j,k;
    uint32_t txbuffer,rxbuffer;
    uint8_t* tx_addr=TxD;
    uint8_t* rx_addr=RxD;
    
    if (hd->CS_Control)
        GPIO_Write(hd->CS,0);
    
    //不接收，说明只发送
    if (RxD==0)
    {
        if (hd->FirstBit==LSB)
        {
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
        }
        else if (hd->FirstBit==MSB)
        {
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
        }
    }   
    else    //要接收，可能也要发送
    {
        if (TxD==0) //只接收，不发送
        {
            if (hd->FirstBit==LSB)
            {
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
            }
            else if (hd->FirstBit==MSB)
            {
                GPIO_Write(hd->MOSI,0);
                for (i=0;i<len;i++)
                {
                    rxbuffer=0;
                    for (j=hd->FrameLength;j>=0;j--)
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
            }
        }
        else    //接收且发送
        {
            if (hd->FirstBit==LSB)
            {
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
            }
            else if (hd->FirstBit==MSB)
            {
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
            }
        }
    }
    
    if (hd->CS_Control)
        GPIO_Write(hd->CS,1);
}
