#ifndef PORT_CORE_H
#define PORT_CORE_H

#include <OsTypes.h>
#include <stdint.h>

/* SuperVisor mode Port. */
void PortSuperVisorModeEnable(void);

void PortSuperVisorModeDisable(void);


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

void PortOsTickInit(IrqPriority_t os_tick_irq_prio);

extern void OsTick(void);


#endif
