/*
 * PortWdt.h
 *
 * Created: 4-3-2018 1:00:01
 *  Author: Dorus
 */ 


#ifndef PORT_WDT_H_
#define PORT_WDT_H_

#define PORT_WDT_EXPIRE_15_MS  0
#define PORT_WDT_EXPIRE_30_MS  1
#define PORT_WDT_EXPIRE_60_MS  2
#define PORT_WDT_EXPIRE_120MS  3
#define PORT_WDT_EXPIRE_8_S    4

#include <OsTypes.h>

/* Watchdog Timer Port. */

void PortWdtInit(U8_t wdt_mode);

void PortWdtEnable(U32_t timeout_ms);

void PortWdtDisable(void);

void PortWdtKick(void);

extern void OsWdtIsr(void);

#endif /* PORT_WDT_H_ */
