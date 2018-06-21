
#include "PortCore.h"

#include "stm32f1xx_hal.h"
#include "core_cm3.h"

uint16_t SysTickPrescaler;

void PortSuperVisorModeEnable(void)
{
	/* Cortex M3 has no support for supervisor mode. */
}

void PortSuperVisorModeDisable(void)
{
	/* Cortex M3 has no support for supervisor mode. */
}

void PortGlobalIntDisable(void)
{
	__disable_irq();
}

void PortGlobalIntEnable(void)
{
	__enable_irq();
}

void PortOsIntDisable(void)
{
	HAL_NVIC_DisableIRQ(SysTick_IRQn);
}

void PortOsIntEnable(void)
{
	HAL_NVIC_EnableIRQ(SysTick_IRQn);
}

void PortOsIntFlagClear(void)
{
	HAL_NVIC_EnableIRQ(SysTick_IRQn);
}

void PortOsTimerInit(U16_t prescaler, U16_t ovf)
{
	uint16_t SysTickPrescaler = prescaler;
	uint32_t ticks = (uint32_t)(SysTickPrescaler * ovf);
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
	HAL_SYSTICK_Config(ticks);
	SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk);
	PortOsIntDisable();
}


void PortOsTimerDisable(void)
{
	SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk);
}

void PortOsTimerEnable(void)
{
	SysTick->CTRL |= (SysTick_CTRL_ENABLE_Msk);
}

U32_t PortOsTimerTicksGet(void)
{
    uint32_t tc_val = SysTick->VAL;
    return (uint32_t)tc_val;
}

void PortOsTimerTicksReset(void)
{
	SysTick->VAL = 0;
}

void PortOsTimerTicksSet(U32_t ticks)
{
	ticks *= SysTickPrescaler;
	SysTick->VAL = ticks;
}

void HAL_SYSTICK_Callback(void)
{
	OsTick();
}


void PortOsIntInit(IrqPriority_t os_tick_irq_prio)
{
	NVIC_SetPriority (SysTick_IRQn, (1UL << os_tick_irq_prio) - 1UL); /* set Priority for Systick Interrupt */
}






