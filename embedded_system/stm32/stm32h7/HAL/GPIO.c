#include "GPIO.h"
#include "STM32GPIO.h"

void GPIO_Mode_Set(gpio io, gpio_mode mode)
{
	if (io != GPIO_NONE)
		STM32GPIO_MODER_W(GPIO_Get_GroupAddr(io), GPIO_Get_PinNum(io), mode);
}

void GPIO_OutputType_Set(gpio io, gpio_otype otype)
{
	if (io != GPIO_NONE)
		STM32GPIO_OTYPER_W(GPIO_Get_GroupAddr(io), GPIO_Get_PinNum(io), otype);
}

void GPIO_OutputSpeed_Set(gpio io, gpio_ospeed ospeed)
{
	if (io != GPIO_NONE)
		STM32GPIO_OSPEEDR_W(GPIO_Get_GroupAddr(io), GPIO_Get_PinNum(io), ospeed);
}

void GPIO_PullUpDown_Set(gpio io, gpio_pupd pupd)
{
	if (io != GPIO_NONE)
		STM32GPIO_PUPDR_W(GPIO_Get_GroupAddr(io), GPIO_Get_PinNum(io), pupd);
}

void GPIO_Set(gpio io, gpio_mode mode, gpio_otype otype, gpio_ospeed ospeed, gpio_pupd pupd)
{
	if (io != GPIO_NONE)
	{
		if (mode == GPIO_MODE_IN)
		{
			STM32GPIO_MODER_W(GPIO_Get_GroupAddr(io), GPIO_Get_PinNum(io), mode);
			STM32GPIO_PUPDR_W(GPIO_Get_GroupAddr(io), GPIO_Get_PinNum(io), pupd);
		}
		else
		{
			STM32GPIO_MODER_W(GPIO_Get_GroupAddr(io), GPIO_Get_PinNum(io), mode);
			STM32GPIO_OTYPER_W(GPIO_Get_GroupAddr(io), GPIO_Get_PinNum(io), otype);
			STM32GPIO_OSPEEDR_W(GPIO_Get_GroupAddr(io), GPIO_Get_PinNum(io), ospeed);
			STM32GPIO_PUPDR_W(GPIO_Get_GroupAddr(io), GPIO_Get_PinNum(io), pupd);
		}
	}
}

void GPIO_AF_Set(gpio io, gpio_af af)
{
	if (io != GPIO_NONE)
		STM32GPIO_AFR_W(GPIO_Get_GroupAddr(io), GPIO_Get_PinNum(io), af);
}

void GPIO_Write(gpio io, uint8_t status)
{
	if (io != GPIO_NONE)
	{
		if (status & 1)
			STM32GPIO_BSRRL_S(GPIO_Get_GroupAddr(io), GPIO_Get_PinNum(io));
		else
			STM32GPIO_BSRRH_S(GPIO_Get_GroupAddr(io), GPIO_Get_PinNum(io));
	}
}

uint8_t GPIO_Read(gpio io)
{
	if (io != GPIO_NONE)
		return STM32GPIO_IDR_R(GPIO_Get_GroupAddr(io), GPIO_Get_PinNum(io));
	else
		return 0;
}

void _GPIO_Set(GPIO_TypeDef *GPIOx, uint32_t BITx, uint32_t MODE, uint32_t OTYPE, uint32_t OSPEED, uint32_t PUPD)
{
	uint32_t pinpos = 0, pos = 0, curpin = 0;
	for (pinpos = 0; pinpos < 16; pinpos++)
	{
		pos = 1 << pinpos;	 //Ò»¸ö¸öÎ»¼ì²é
		curpin = BITx & pos; //¼ì²éÒý½ÅÊÇ·ñÒªÉèÖÃ
		if (curpin == pos)	 //ÐèÒªÉèÖÃ
		{
			GPIOx->MODER &= ~(3 << (pinpos * 2)); //ÏÈÇå³ýÔ­À´µÄÉèÖÃ
			GPIOx->MODER |= MODE << (pinpos * 2); //ÉèÖÃÐÂµÄÄ£Ê½
			if ((MODE == 0X01) || (MODE == 0X02)) //Èç¹ûÊÇÊä³öÄ£Ê½/¸´ÓÃ¹¦ÄÜÄ£Ê½
			{
				GPIOx->OSPEEDR &= ~(3 << (pinpos * 2));		//Çå³ýÔ­À´µÄÉèÖÃ
				GPIOx->OSPEEDR |= (OSPEED << (pinpos * 2)); //ÉèÖÃÐÂµÄËÙ¶ÈÖµ
				GPIOx->OTYPER &= ~(1 << pinpos);			//Çå³ýÔ­À´µÄÉèÖÃ
				GPIOx->OTYPER |= OTYPE << pinpos;			//ÉèÖÃÐÂµÄÊä³öÄ£Ê½
			}
			GPIOx->PUPDR &= ~(3 << (pinpos * 2)); //ÏÈÇå³ýÔ­À´µÄÉèÖÃ
			GPIOx->PUPDR |= PUPD << (pinpos * 2); //ÉèÖÃÐÂµÄÉÏÏÂÀ­
		}
	}
}

void _GPIO_AF_Set(GPIO_TypeDef *GPIOx, uint8_t BITx, uint8_t AFx)
{
	GPIOx->AFR[BITx >> 3] &= ~(0X0F << ((BITx & 0X07) * 4));
	GPIOx->AFR[BITx >> 3] |= (uint32_t)AFx << ((BITx & 0X07) * 4);
}
