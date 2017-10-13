/*
 * HalTimer.h
 *
 * Created: 19-Apr-17 11:28:04
 *  Author: User
 */


#ifndef HALTIMER_H_
#define HALTIMER_H_

#include <stdint.h>

#define HAL_TIMER_CHANNEL_OS    0

typedef void (*HalTimerIrqHandle_t)(void);

typedef uint8_t HalTimerBase_t;


struct HalTimerInstance {
    HalTimerBase_t  channel;
    uint8_t         mode;
    uint32_t        interval_us;
    uint8_t         irq_priority;
    HalTimerIrqHandle_t irq_handle;
};

void HalTimerInit(struct HalTimerInstance *timer_handle);

void HalTimerStart(struct HalTimerInstance *timer_handle);

void HalTimerStop(struct HalTimerInstance *timer_handle);

uint32_t HalTimerCountGet(struct HalTimerInstance *timer_handle);

void HalTimerIntEnable(struct HalTimerInstance *timer_handle);

void HalTimerIntDisable(struct HalTimerInstance *timer_handle);

void HalTimerIntFlagClear(struct HalTimerInstance *timer_handle);


#endif /* HALTIMER_H_ */