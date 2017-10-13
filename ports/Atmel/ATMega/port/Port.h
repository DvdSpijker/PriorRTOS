#ifndef PRIOR_PORT_H
#define PRIOR_PORT_H

#include <Types.h>

#include <avr/pgmspace.h>
#include <avr/wdt.h>

#define OS_IVECT TIMER1_COMPA_vect /* OS Tick Interrupt vector. */

#define PORT_WDT_EXPIRE_15_MS  WDTO_15MS
#define PORT_WDT_EXPIRE_30_MS  WDTO_30MS
#define PORT_WDT_EXPIRE_60_MS  WDTO_60MS
#define PORT_WDT_EXPIRE_120MS  WDTO_120MS
#define PORT_WDT_EXPIRE_8_S    WDTO_8S

typedef U8_t IrqPriority_t;

/* Global Interrupt HAL. */
void PortGlobalIntDisable(void);

void PortGlobalIntEnable(void);


/* OS Timer HAL. */
void PortOsTimerInit(volatile U16_t prescaler, U16_t ovf);

void PortOsTimerStop(void);

void PortOsTimerStart(void);

U32_t PortOsTimerTicksGet(void);

void PortOsTimerTicksSet(U32_t ticks);

void PortOsTimerTicksReset(void);


/* OS Tick HAL. */
void PortOsIntDisable(void);

void PortOsIntEnable(void);

void PortOsIntFlagClear(void);

extern void CoreTick(void);

void PortOsTickInit(IrqPriority_t os_tick_irq_prio);

/* Watchdog Timer HAL. */

void PortWdtInit(U8_t wdt_mode, IrqPriority_t wdt_irq_prio);

void PortWdtEnable(U8_t wdt_expire_opt);

void PortWdtDisable(void);

void PortWdtKick(void);

extern void CoreWdtIsr(void);

/* Context Save and Restore. */
#define PortContextSave()                               \
asm volatile (  "push	r0						\n\t" \
                "in		r0, __SREG__			\n\t"   \
                "cli							\n\t"    \
                "push	r0						\n\t" \
                "push	r1						\n\t" \
                "clr	r1						\n\t"  \
                "push	r2						\n\t" \
                "push	r3						\n\t" \
                "push	r4						\n\t" \
                "push	r5						\n\t" \
                "push	r6						\n\t" \
                "push	r7						\n\t" \
                "push	r8						\n\t" \
                "push	r9						\n\t" \
                "push	r10						\n\t"    \
                "push	r11						\n\t"    \
                "push	r12						\n\t"    \
                "push	r13						\n\t"    \
                "push	r14						\n\t"    \
                "push	r15						\n\t"    \
                "push	r16						\n\t"    \
                "push	r17						\n\t"    \
                "push	r18						\n\t"    \
                "push	r19						\n\t"    \
                "push	r20						\n\t"    \
                "push	r21						\n\t"    \
                "push	r22						\n\t"    \
                "push	r23						\n\t"    \
                "push	r24						\n\t"    \
                "push	r25						\n\t"    \
                "push	r26						\n\t"    \
                "push	r27						\n\t"    \
                "push	r28						\n\t"    \
                "push	r29						\n\t"    \
                "push	r30						\n\t"    \
                "push	r31						\n\t"    \
                "lds	r26, TCB_running		\n\t"    \
                "lds	r27, TCB_running  + 1	\n\t"    \
                "in		r0, 0x3d				\n\t"  \
                "st		x+, r0					\n\t"   \
                "in		r0, 0x3e				\n\t"  \
                "st		x+, r0					\n\t"   \
);

#define PortContextRestore()                                \
asm volatile (  "lds	r26, TCB_running		\n\t"    \
                "lds	r27, TCB_running + 1	\n\t" \
                "ld		r28, x+					\n\t"  \
                "out	__SP_L__, r28			\n\t"  \
                "ld		r29, x+					\n\t"  \
                "out	__SP_H__, r29			\n\t"  \
                "pop	r31						\n\t" \
                "pop	r30						\n\t" \
                "pop	r29						\n\t" \
                "pop	r28						\n\t" \
                "pop	r27						\n\t" \
                "pop	r26						\n\t" \
                "pop	r25						\n\t" \
                "pop	r24						\n\t" \
                "pop	r23						\n\t" \
                "pop	r22						\n\t" \
                "pop	r21						\n\t" \
                "pop	r20						\n\t" \
                "pop	r19						\n\t" \
                "pop	r18						\n\t" \
                "pop	r17						\n\t" \
                "pop	r16						\n\t" \
                "pop	r15						\n\t" \
                "pop	r14						\n\t" \
                "pop	r13						\n\t" \
                "pop	r12						\n\t" \
                "pop	r11						\n\t" \
                "pop	r10						\n\t" \
                "pop	r9						\n\t"  \
                "pop	r8						\n\t"  \
                "pop	r7						\n\t"  \
                "pop	r6						\n\t"  \
                "pop	r5						\n\t"  \
                "pop	r4						\n\t"  \
                "pop	r3						\n\t"  \
                "pop	r2						\n\t"  \
                "pop	r1						\n\t"  \
                "pop	r0						\n\t"  \
                "out	__SREG__, r0			\n\t"   \
                "pop	r0						\n\t"  \
);



#endif