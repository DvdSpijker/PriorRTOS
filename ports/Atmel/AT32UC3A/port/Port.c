
#include <PriorRTOS_config.h>
#include <Port.h>
#include <tc.h>
//#include <Types.h>

#include <intc.h>

#include <sysclk.h>


#define OS_TC_CHANNEL 0
#define OS_TC (&AVR32_TC0)

__attribute__((__interrupt__))
static void os_timer_irq(void);

//struct HalTimerInstance OsTimer = {
//.channel = OS_TC,
//.mode = 0,
//.interval_us = 0,
//.irq_priority = CONFIG_OS_TICK_IRQ_PRIORITY,
//.irq = AVR32_TC0_IRQ0,
//.irq_handler = os_timer_irq,
//};

void PortGlobalIntDisable(void)
{
    cpu_irq_disable();
}

void PortGlobalIntEnable(void)
{
    cpu_irq_enable();
}

void PortOsIntDisable(void)
{
    const tc_interrupt_t tc_interrupt = {
        .etrgs = 0,
        .ldrbs = 0,
        .ldras = 0,
        .cpcs  = 0, // Disable interrupt on RC compare alone
        .cpbs  = 0,
        .cpas  = 0,
        .lovrs = 0,
        .covfs = 0
    };
    tc_configure_interrupts(OS_TC, OS_TC_CHANNEL, &tc_interrupt);
}

void PortOsIntEnable(void)
{
    const tc_interrupt_t tc_interrupt = {
        .etrgs = 0,
        .ldrbs = 0,
        .ldras = 0,
        .cpcs  = 1, // Enable interrupt on RC compare alone
        .cpbs  = 0,
        .cpas  = 0,
        .lovrs = 0,
        .covfs = 0
    };
    tc_configure_interrupts(OS_TC, OS_TC_CHANNEL, &tc_interrupt);
}

void PortOsIntFlagClear(void)
{
    /* Read sr register => clear interrupt flag. */
    tc_read_sr(OS_TC, OS_TC_CHANNEL);
}

void PortOsTimerInit(uint16_t prescaler, uint16_t ovf)
{
    sysclk_enable_peripheral_clock(OS_TC);
// Options for waveform generation.
    const tc_waveform_opt_t waveform_opt = {
// Channel selection.
        .channel  = OS_TC_CHANNEL,
// Software trigger effect on TIOB.
        .bswtrg   = TC_EVT_EFFECT_NOOP,
// External event effect on TIOB.
        .beevt    = TC_EVT_EFFECT_NOOP,
// RC compare effect on TIOB.
        .bcpc     = TC_EVT_EFFECT_NOOP,
// RB compare effect on TIOB.
        .bcpb     = TC_EVT_EFFECT_NOOP,
// Software trigger effect on TIOA.
        .aswtrg   = TC_EVT_EFFECT_NOOP,
// External event effect on TIOA.
        .aeevt    = TC_EVT_EFFECT_NOOP,
// RC compare effect on TIOA.
        .acpc     = TC_EVT_EFFECT_NOOP,
        /* RA compare effect on TIOA.
        * (other possibilities are none, set and clear).
        */
        .acpa     = TC_EVT_EFFECT_NOOP,
        /* Waveform selection: Up mode with automatic trigger(reset)
        * on RC compare.
        */
        .wavsel   = TC_WAVEFORM_SEL_UP_MODE_RC_TRIGGER,
// External event trigger enable.
        .enetrg   = false,
// External event selection.
        .eevt     = 0,
// External event edge selection.
        .eevtedg  = TC_SEL_NO_EDGE,
// Counter disable when RC compare.
        .cpcdis   = false,
// Counter clock stopped with RC compare.
        .cpcstop  = false,
// Burst signal selection.
        .burst    = false,
// Clock inversion.
        .clki     = false,
// Internal source clock 3, connected to fPBA / 8.
        .tcclks   = TC_CLOCK_SOURCE_TC3,
    };

// Initialize the timer/counter.
    tc_init_waveform(OS_TC, &waveform_opt);

    /*
    * Set the compare triggers.
    * We configure it to count every 10 milliseconds.
    * We want: (10 / (fPBA / 8)) * RC = 1 ms, hence RC = (fPBA / 8) / 10
    * to get an interrupt every 1 ms.
    */
    tc_write_rc(OS_TC, OS_TC_CHANNEL, 2994);
}


void PortOsTimerStop(void)
{
    tc_stop(OS_TC, OS_TC_CHANNEL);
}

void PortOsTimerStart(void)
{
    tc_start(OS_TC, OS_TC_CHANNEL);
}

uint32_t PortOsTimerTicksGet(void)
{
    int tc_val = tc_read_tc(OS_TC, OS_TC_CHANNEL);
    return (uint32_t)tc_val;
}

void PortOsTimerTicksReset(void)
{
    /* Resetting TC values is not possible in AVR32. */
    return;
}

void PortOsTimerTicksSet(uint32_t ticks)
{
    Wr_bitfield(ticks, AVR32_TC_CV_MASK, OS_TC->channel[OS_TC_CHANNEL].cv);
}

__attribute__((__interrupt__))
static void os_timer_irq(void)
{
    CoreTick();


}

void PortOsTickInit(IrqPriority_t os_tick_irq_prio)
{
// Options for enabling TC interrupts
    const tc_interrupt_t tc_interrupt = {
        .etrgs = 0,
        .ldrbs = 0,
        .ldras = 0,
        .cpcs  = 1, // Enable interrupt on RC compare alone
        .cpbs  = 0,
        .cpas  = 0,
        .lovrs = 0,
        .covfs = 0,
    };
    tc_configure_interrupts(OS_TC, OS_TC_CHANNEL, &tc_interrupt);

    INTC_register_interrupt(&os_timer_irq, AVR32_TC0_IRQ0, AVR32_INTC_IPR_INTLEVEL_INT0);

}



//void PortWdtInit(U8_t wdt_mode, IrqPriority_t wdt_irq_prio)
//{
//char x = 0;
//x++;
//}

//void PortWdtEnable(U8_t wdt_expire_opt)
//{
//PortGlobalIntDisable();
//
//PortGlobalIntEnable();
//}

void PortWdtDisable()
{
    char x = 0;
    x++;
}

void PortWdtKick()
{
    char x = 0;
    x++;
}

//ISR(WDT_vect)
//{
//CoreWdtIsr();
//}




