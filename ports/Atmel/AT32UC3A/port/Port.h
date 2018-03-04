#ifndef PRIOR_PORT_H
#define PRIOR_PORT_H

#include <PriorRTOS_config.h>
#include <stdint.h>

#if PRTOS_CONFIG_ENABLE_WATCHDOG==1
#include <PortWdt.h>
#endif

#if PRTOS_CONFIG_ENABLE_LOGGING==1
#include <PortLog.h>
#endif

#if PRTOS_CONFIG_ENABLE_SHELL==1
#include <PortShell.h>
#endif


typedef uint8_t IrqPriority_t;

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