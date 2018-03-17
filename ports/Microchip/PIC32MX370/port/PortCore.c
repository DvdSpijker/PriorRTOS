
#include "PortCore.h"

#include <xc.h>
#include <sys/attribs.h>

void PortSuperVisorModeEnable(void)
{
    
}

void PortSuperVisorModeDisable(void)
{
    
}

void PortGlobalIntDisable(void)
{
   __builtin_disable_interrupts();
}

void PortGlobalIntEnable(void)
{
   __builtin_enable_interrupts();
}

void PortOsIntDisable(void)
{
    IEC0bits.T2IE = 0;  /* Disable Timer 2 interrupt. */
}

void PortOsIntEnable(void)
{
    IEC0bits.T2IE = 1;  /* Enable Timer 2 interrupt. */
}

void PortOsIntFlagClear(void)
{
    // Reset interrupt flag
    IFS0bits.T2IF = 0;
}

void PortOsTimerInit(uint16_t prescaler, uint16_t ovf)
{
    PortOsTimerStop();
    T2CONbits.TCKPS = 7;    /* pre-scale = 1:256 (T2CLKIN = 39062.5 Hz) */
    PR2 = 390;              /* T2 period ~ 1mS */
    PortOsTimerTicksReset();
}


void PortOsTimerStop(void)
{
    T2CONbits.TON = 0;      /* turn off Timer 2 */
}

void PortOsTimerStart(void)
{
    T2CONbits.TON = 1; /* turn on Timer 2 */
}

uint32_t PortOsTimerTicksGet(void)
{
    return (uint32_t)TMR2;
}

void PortOsTimerTicksReset(void)
{
    TMR2 = 0; /* clear Timer 2 counter */
}

void PortOsTimerTicksSet(uint32_t ticks)
{
    TMR2 = ticks;
}

void __ISR(_TIMER_2_VECTOR, IPL7SRS) T2Interrupt(void)
{
    OsTick();
    PortOsIntFlagClear();
}

void PortOsTickInit(IrqPriority_t os_tick_irq_prio)
{
    IPC2bits.T2IP = os_tick_irq_prio; /* Set priority. */
    PortOsIntFlagClear();
    INTCONSET = _INTCON_MVEC_MASK;    /* Set the interrupt controller for multi-vector mode */
}







