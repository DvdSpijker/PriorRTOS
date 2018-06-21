
#include "PortCore.h"

#include <xc.h>
#include <sys/attribs.h>

#include <stdio.h>

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
    PortOsTimerDisable();
    PortOsTimerTicksReset();
    
    T2CONbits.TCKPS = 0;    /* pre-scale = 1 */
    PR2 = prescaler * ovf; 
   
    T2CONbits.TON = 0; //stop TIMER1
    T2CONbits.TCS = 0;
    T2CONbits.TGATE = 0; //use trigger flag as compare
}


void PortOsTimerDisable(void)
{
    T2CONbits.TON = 0;      /* turn off Timer 2 */
}

void PortOsTimerEnable(void)
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

void __ISR(_TIMER_2_VECTOR, IPL7AUTO) T2Interrupt(void)
{
    OsTick();
    PortOsIntFlagClear();
}

void PortOsIntInit(IrqPriority_t os_tick_irq_prio)
{
    IPC2bits.T2IP = 7;//os_tick_irq_prio; /* Set priority. */
    IFS0bits.T2IF = 0;
    IPC2bits.T2IS = 1;
    IEC0bits.T2IE = 1;
}







