/*
 * PortWdt.h
 *
 * Created: 4-3-2018 1:00:01
 *  Author: Dorus
 */ 


#ifndef PORT_WDT_H_
#define PORT_WDT_H_

#include <stdint.h>
#include <Types.h>

#define PORT_WDT_EXPIRE_15_MS  0
#define PORT_WDT_EXPIRE_30_MS  1
#define PORT_WDT_EXPIRE_60_MS  2
#define PORT_WDT_EXPIRE_120MS  3
#define PORT_WDT_EXPIRE_8_S    4

/* Watchdog Timer Port. */

void PortWdtInit(uint8_t wdt_mode, IrqPriority_t wdt_irq_prio);

void PortWdtEnable(uint8_t wdt_expire_opt);

void PortWdtDisable(void);

void PortWdtKick(void);

extern void OsWdtIsr(void);

#endif /* PORT_WDT_H_ */