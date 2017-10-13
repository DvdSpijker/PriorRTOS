/*
 * HalTimer.c
 *
 * Created: 19-Apr-17 11:28:23
 *  Author: User
 */

#include "Hal_timer.h"
#include <intc.h>
#include <tc.h>
#include <sysclk.h>

void HalTimerInit(struct HalTimerInstance *timer_handle)
{
    sysclk_enable_peripheral_clock(timer_handle->channel);
    // Options for waveform generation.
    const tc_waveform_opt_t waveform_opt = {
        // Channel selection.
        .channel  = 0,
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
    tc_init_waveform(timer_handle->channel, &waveform_opt);

    /*
    * Set the compare triggers.
    * We configure it to count every 10 milliseconds.
    * We want: (1 / (fPBA / 8)) * RC = 10 ms, hence RC = (fPBA / 8) / 100
    * to get an interrupt every 10 ms.
    */
    tc_write_rc(timer_handle->channel, 0, (sysclk_get_peripheral_bus_hz(timer_handle->channel) / 576 / 100));

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
    cpu_irq_disable();
    tc_configure_interrupts(timer_handle->channel, 0, &tc_interrupt);
    INTC_register_interrupt(timer_handle->irq_handler , timer_handle->irq, (uint32_t)timer_handle->irq_priority);
    cpu_irq_enable();


}

void HalTimerStart(struct HalTimerInstance *timer_handle)
{
    tc_start(timer_handle->channel, 0);
}

void HalTimerStop(struct HalTimerInstance *timer_handle)
{
    tc_stop(timer_handle->channel, 0);
}

uint32_t HalTimerCountGet(struct HalTimerInstance *timer_handle)
{
    return tc_read_tc(timer_handle->channel, 0);
}

void HalTimerIntEnable(struct HalTimerInstance *timer_handle)
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
    tc_configure_interrupts(timer_handle->channel, 0, &tc_interrupt);
}

void HalTimerIntDisable(struct HalTimerInstance *timer_handle)
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
    tc_configure_interrupts(timer_handle->channel, 0, &tc_interrupt);
}


void HalTimerIntFlagClear(struct HalTimerInstance *timer_handle)
{
    tc_read_sr(timer_handle->channel, 0);
}

