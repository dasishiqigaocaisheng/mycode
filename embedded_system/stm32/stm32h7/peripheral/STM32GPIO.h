#ifndef _STM32GPIO_H_
#define _STM32GPIO_H_

#include "Framework.h"

/***********************************************************************/
/*                                                                     */
/*                              MODER寄存器                            */
/*                                                                     */
/***********************************************************************/
/*****************************配置参数定义******************************/
#define MODER_INPUT 0
#define MODER_OUT 1
#define MODER_AF 2
#define MODER_ANALOG 3

#define STM32GPIO_MODER_W(x, y, z) BitClearSet((x)->MODER, y << 1, Mask2Bit, z)
#define STM32GPIO_MODER_R(x, y) BitRead((x)->MODER, y << 1, Mask2Bit)

/***********************************************************************/
/*                                                                     */
/*                              OTYPE寄存器                            */
/*                                                                     */
/***********************************************************************/
/*****************************配置参数定义******************************/
#define OTYPER_PP 0
#define OTYPER_OD 1

#define STM32GPIO_OTYPER_W(x, y, z) BitClearSet((x)->OTYPER, y, Mask1Bit, z)
#define STM32GPIO_OTYPER_R(x, y) BitRead((x)->OTYPER, y, Mask1Bit)

/***********************************************************************/
/*                                                                     */
/*                              OSPEEDR寄存器                           */
/*                                                                     */
/***********************************************************************/
/*****************************配置参数定义******************************/
#define OSPEEDR_LOW 0
#define OSPEEDR_MID 1
#define OSPEEDR_HIGH 2
#define OSPEEDR_ULTRAL 3

#define STM32GPIO_OSPEEDR_W(x, y, z) BitClearSet((x)->OSPEEDR, y << 1, Mask2Bit, z)
#define STM32GPIO_OSPEEDR_R(x, y) BitRead((x)->OSPEEDR, y << 1, Mask2Bit)

/***********************************************************************/
/*                                                                     */
/*                              PUPDR寄存器                           */
/*                                                                     */
/***********************************************************************/
/*****************************配置参数定义******************************/
#define PUPDR_NONE 0
#define PUPDR_PU 1
#define PUPDR_PD 2

#define STM32GPIO_PUPDR_W(x, y, z) BitClearSet((x)->PUPDR, y << 1, Mask2Bit, z)
#define STM32GPIO_PUPDR_R(x, y) BitRead((x)->PUPDR, y << 1, Mask2Bit)

/***********************************************************************/
/*                                                                     */
/*                              IDR寄存器                           */
/*                                                                     */
/***********************************************************************/
#define STM32GPIO_IDR_R(x, y) BitRead((x)->IDR, y, Mask1Bit)

/***********************************************************************/
/*                                                                     */
/*                              ODR寄存器                           */
/*                                                                     */
/***********************************************************************/
#define STM32GPIO_ODR_W(x, y, z) BitClearSet((x)->ODR, y, Mask1Bit, z)
#define STM32GPIO_ODR_R(x, y) BitRead((x)->ODR, y, Mask1Bit)

/***********************************************************************/
/*                                                                     */
/*                              BSRR寄存器                           */
/*                                                                     */
/***********************************************************************/
#define STM32GPIO_BSRRL_S(x, y) BitSet((x)->BSRRL, y, Mask1Bit, 1)
#define STM32GPIO_BSRRH_S(x, y) BitSet((x)->BSRRH, y, Mask1Bit, 1)

/***********************************************************************/
/*                                                                     */
/*                              AFR寄存器                           */
/*                                                                     */
/***********************************************************************/
#define AFR_AF0 0
#define AFR_AF1 1
#define AFR_AF2 2
#define AFR_AF3 3
#define AFR_AF4 4
#define AFR_AF5 5
#define AFR_AF6 6
#define AFR_AF7 7
#define AFR_AF8 8
#define AFR_AF9 9
#define AFR_AF10 10
#define AFR_AF11 11
#define AFR_AF12 12
#define AFR_AF13 13
#define AFR_AF14 14
#define AFR_AF15 15

#define STM32GPIO_AFR_W(x, y, z) BitClearSet((x)->AFR[(y) >> 3], (y % 8)<<2, Mask4Bit, z)
#define STM32GPIO_AFR_R(x, y) BitRead((x)->AFR[(y) >> 3], (y % 8)<<2, Mask4Bit)

#endif
