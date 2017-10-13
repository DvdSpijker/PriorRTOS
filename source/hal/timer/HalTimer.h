/*
 * HalTimer.h
 *
 * Created: 19-Apr-17 11:28:04
 *  Author: User
 */


#ifndef HALTIMER_H_
#define HALTIMER_H_

#ifdef __cplusplus
extern "C" {
#endif


#include <tc.h>
#include <stdint.h>

#define HAL_TIMER_CHANNEL_OS    0


#define HAL_TIMER_MODE_CTC  0x00
#define HAL_TIMER_MODE_PWM  0x01

typedef void (*HalTimerIrqHandler_t)(void);

typedef volatile avr32_tc_t * HalTimerBase_t;


struct HalTimerInstance {
    HalTimerBase_t  channel;
    uint8_t         mode;
    uint32_t        interval_us;
    uint32_t        irq_priority;
    uint32_t        irq;
    HalTimerIrqHandler_t irq_handler;
};

void HalTimerInit(struct HalTimerInstance *timer_handle);

void HalTimerStart(struct HalTimerInstance *timer_handle);

void HalTimerStop(struct HalTimerInstance *timer_handle);

uint32_t HalTimerCountGet(struct HalTimerInstance *timer_handle);

void HalTimerIntEnable(struct HalTimerInstance *timer_handle);

void HalTimerIntDisable(struct HalTimerInstance *timer_handle);

void HalTimerIntFlagClear(struct HalTimerInstance *timer_handle);

#ifdef __cplusplus
}
#endif
#endif /* HALTIMER_H_ */