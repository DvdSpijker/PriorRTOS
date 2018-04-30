
#include "PortCore.h"

#include "stm32f1xx_hal_cortex.h"
#include "core_cm0.h"

void PortGlobalIntDisable(void)
{
    cpu_irq_disable();
}

void PortGlobalIntEnable(void)
{

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

void PortOsTimerInit(uint16_t prescaler, uint16_t ovf)
{
	uint32_t ticks = (uint32_t)(prescaler * ovf);
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
	HAL_SYSTICK_Config(ticks);
}


void PortOsTimerStop(void)
{
	SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk);
}

void PortOsTimerStart(void)
{
	SysTick->CTRL |= (SysTick_CTRL_ENABLE_Msk);
}

uint32_t PortOsTimerTicksGet(void)
{
    uint32_t tc_val = SysTick->VAL;
    return (uint32_t)tc_val;
}

void PortOsTimerTicksReset(void)
{
	SysTick->VAL = 0;
}

void PortOsTimerTicksSet(uint32_t ticks)
{
	SysTick->VAL = ticks;
}

void HAL_SYSTICK_Callback(void)
{
	OsTick();
}


void PortOsTickInit(IrqPriority_t os_tick_irq_prio)
{

}







