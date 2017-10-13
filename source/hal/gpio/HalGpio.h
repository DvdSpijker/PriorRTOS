/*
 * HalGpio.h
 *
 * Created: 19-Apr-17 11:57:05
 *  Author: User
 */


#ifndef HALGPIO_H_
#define HALGPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <gpio.h>

enum HalGpioPinMode {
    PIN_MODE_INPUT,
    PIN_MODE_OUTPUT,
    PIN_MODE_PULL_UP, /* TODO: GPIO pin modes. */
    PIN_MODE_PULL_DOWN,
    PIN_MODE_INVALID, 
};

enum HalGpioPinState {
    PIN_STATE_LOW = 0,
    PIN_STATE_HIGH = 1,
    PIN_STATE_TOGGLE = 2,
};

typedef volatile uint8_t * HalGpioBase_t;

void HalGpioPinModeSet(HalGpioBase_t ddr, uint8_t pin_nr, enum HalGpioPinMode mode);

enum HalGpioPinMode HalGpioPinModeGet(HalGpioBase_t ddr, uint8_t pin_nr);

void HalGpioPinStateSet(HalGpioBase_t port, uint8_t pin_nr, enum HalGpioPinState state);

enum HalGpioPinState HalGpioPinStateGet(HalGpioBase_t port, uint8_t pin_nr);

#ifdef __cplusplus
}
#endif

#endif /* HALGPIO_H_ */