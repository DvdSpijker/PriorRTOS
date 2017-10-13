#ifndef PRIOR_PORT_H
#define PRIOR_PORT_H

#include <stdint.h>

#define PORT_WDT_EXPIRE_15_MS  0
#define PORT_WDT_EXPIRE_30_MS  1
#define PORT_WDT_EXPIRE_60_MS  2
#define PORT_WDT_EXPIRE_120MS  3
#define PORT_WDT_EXPIRE_8_S    4

typedef uint8_t IrqPriority_t;

/* Global Interrupt HAL. */
void PortGlobalIntDisable(void);

void PortGlobalIntEnable(void);


/* OS Timer HAL. */
void PortOsTimerInit(uint16_t prescaler, uint16_t ovf);

void PortOsTimerStop(void);

void PortOsTimerStart(void);

uint32_t PortOsTimerTicksGet(void);

void PortOsTimerTicksSet(uint32_t ticks);

void PortOsTimerTicksReset(void);


/* OS Tick HAL. */
void PortOsIntDisable(void);

void PortOsIntEnable(void);

void PortOsIntFlagClear(void);

extern void CoreTick(void);

void PortOsTickInit(IrqPriority_t os_tick_irq_prio);

/* Watchdog Timer HAL. */

void PortWdtInit(uint8_t wdt_mode, IrqPriority_t wdt_irq_prio);

void PortWdtEnable(uint8_t wdt_expire_opt);

void PortWdtDisable(void);

void PortWdtKick(void);

extern void CoreWdtIsr(void);




#endif