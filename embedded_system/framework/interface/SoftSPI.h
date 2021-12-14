
/*Èí¼þSPIÇý¶¯*/

#ifndef _SOFTSPI_H_
#define _SOFTSPI_H_

#include "sys.h"

typedef struct softspi_class softspi;

typedef enum
{
    SOFTSPI_MASTER,
    SOFTSPI_SLAVE
} softspi_mode_type;

typedef enum
{
    CPHA0,
    CPHA1
} softspi_cpha_type;

typedef enum
{
    CPOL0=0,
    CPOL1
} softspi_cpol_type;

typedef enum
{
    MSB,
    LSB
} softspi_firstbit_type;

struct softspi_class 
{
    ro softspi_mode_type    Mode;
    ro softspi_cpha_type              CPHA;
    ro softspi_cpol_type              CPOL;
    vro uint8_t             FrameLength;
    status_flag         CS_Control;
    softspi_firstbit_type FirstBit;
    
    vro gpio_type SCLK;
    vro gpio_type MOSI;
    vro gpio_type MISO;
    vro gpio_type CS;
    
    vro  uint8_t Buffer_Length;
    volatile float ValidTime;
    volatile float InvalidTime;
    
    void (*Transmit_DataFlow)(softspi* hd, void* TxD, void* RxD, uint32_t len);
};


void SoftSPI_Prepare(softspi* hd, softspi_mode_type mode, softspi_cpha_type cpha, softspi_cpol_type cpol, uint8_t frmlen,
                     gpio_type sclk, gpio_type mosi, gpio_type miso, gpio_type cs);










#endif
