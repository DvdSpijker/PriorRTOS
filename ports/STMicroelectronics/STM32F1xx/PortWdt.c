/*
 * PortWdt.c
 *
 * Created: 4-3-2018 1:01:30
 *  Author: Dorus
 */ 

#include "PortWdt.h"

#include "iwdg.h"
#include "stm32f1xx_hal.h"

U32_t WdtPrescaler;

void PortWdtInit(U8_t wdt_mode)
{
	(void)wdt_mode;
}

void PortWdtEnable(U32_t timeout_ms)
{
	MX_IWDG_Init();
	WdtPrescaler = hiwdg.Init.Prescaler;
	hiwdg.Init.Reload = WdtPrescaler  * timeout_ms;
	HAL_IWDG_Init(&hiwdg);
}

void PortWdtDisable()
{
	/* Cannot be disabled. */
}

void PortWdtKick()
{
	HAL_IWDG_Refresh(&hiwdg);
}

