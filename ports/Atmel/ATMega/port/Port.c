
#include "Port.h"


#include <avr/io.h>
#include <util/delay.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

U16_t OsTimerOvf;
U16_t OsTimerPrescaler;

U32_t PortOsTimerTicksGet(void)
{
    U32_t val = (U32_t)TCNT1;
    return val;
}

void PortOsTimerTicksSet(U32_t ticks)
{
    TCNT1 = ticks;
}

void PortOsTimerStop(void)
{
    TCCR1B &= ~(OsTimerPrescaler);
}

void PortOsTimerStart(void)
{
    TCCR1B |= (OsTimerPrescaler);
}

void PortGlobalIntDisable(void)
{
    asm volatile("cli");
}

void PortGlobalIntEnable(void)
{
    asm volatile("sei");
}

void PortOsIntDisable(void)
{
    TIMSK1 = 0;
}

void PortOsIntEnable(void)
{
    TIMSK1 |= (1 << OCIE1A);
}

void PortOsIntFlagClear(void)
{
    return;
}

void PortOsTimerTicksReset(void)
{
    TCNT1 = 0x0000;
}

void PortOsTimerInit(volatile U16_t prescaler, U16_t ovf)
{
    OsTimerOvf = ovf;
    OsTimerPrescaler = prescaler;
    TCCR1A = 0;
    TCCR1B |= (1 << WGM12); //Set CTC Bit
    OCR1A = OsTimerOvf;
}

void PortOsTickInit(IrqPriority_t os_tick_irq_prio)
{
    TIMSK1 |= (1 << OCIE1A);
    TIFR1 |= (1 << OCF1A);
}

ISR(OS_IVECT)
{
    CoreTick();
}

void PortWdtInit(U8_t wdt_mode, IrqPriority_t wdt_irq_prio)
{
    MCUSR = 0;
    WDTCSR |= (1<<WDCE);
    WDTCSR &= ~(1<<WDIF);
    WDTCSR |= (1<<WDCE);
    WDTCSR &= ~(1<<WDIE);
}

void PortWdtEnable(U8_t wdt_expire_opt)
{
    PortGlobalIntDisable();
    wdt_enable(wdt_expire_opt);
    //switch (wdt_expire_opt) {
    //case PORT_WDT_EXPIRE_16_MS:
    //WDTCSR |= (1 << WDCE);
    //WDTCSR &= ~(1 << WDP3) & ~(1 << WDP2);
    //WDTCSR |= (1 << WDCE);
    //WDTCSR &= ~(1 << WDP1) & ~(1 << WDP0);
    //break;
    //case PORT_WDT_EXPIRE_32_MS:
    //WDTCSR |= (1 << WDCE);
    //WDTCSR &= ~(1 << WDP3) & ~(1 << WDP2) & ~(1 << WDP1);
    //WDTCSR |= (1 << WDCE);
    //WDTCSR |= (1 << WDP0);
    //break;
    //case PORT_WDT_EXPIRE_64_MS:
    //WDTCSR |= (1 << WDCE);
    //WDTCSR &= ~(1 << WDP3) & ~(1 << WDP2) & ~(1 << WDP0);
    //WDTCSR |= (1 << WDCE);
    //WDTCSR |=  (1 << WDP1);
    //break;
    //case PORT_WDT_EXPIRE_125_MS:
    //WDTCSR |= (1 << WDCE);
    //WDTCSR &= ~(1 << WDP3) & ~(1 << WDP2);
    //WDTCSR |= (1 << WDCE);
    //WDTCSR |= (1 << WDP1) | (1 << WDP0);
    //break;
    //case PORT_WDT_EXPIRE_8_S:
    //WDTCSR |= (1 << WDCE);
    //WDTCSR &= ~(1 << WDP2) & ~(1 << WDP1);
    //WDTCSR |= (1 << WDCE);
    //WDTCSR |= (1 << WDP0) | (1 << WDP3);
    //break;
    //}
    WDTCSR |= (1<<WDCE) | (1<<WDIE);
    //PortWdtKick();
    PortGlobalIntEnable();
}

void PortWdtDisable()
{
    wdt_disable();
}

void PortWdtKick()
{
    wdt_reset();
}

#if CONFIG_ENABLE_WDT==1
ISR(WDT_vect)
{
    CoreWdtIsr();
}
#endif





