/*
 * HalGpio.c
 *
 * Created: 19-Apr-17 11:57:21
 *  Author: User
 */

#include "HalGpio.h"
#include <gpio.h>

void HalGpioPinModeSet(HalGpioBase_t ddr, uint8_t pin_nr, enum HalGpioPinMode mode)
{
    uint32_t flags = 0;
    if(mode == PIN_MODE_INPUT) {
        flags = GPIO_DIR_INPUT;
    } else {
        flags = GPIO_DIR_OUTPUT;
    }
    gpio_configure_pin(pin_nr, flags);
}

enum HalGpioPinMode HalGpioPinModeGet(HalGpioBase_t ddr, uint8_t pin_nr)
{
    return PIN_MODE_INVALID;
}

void HalGpioPinStateSet(HalGpioBase_t port, uint8_t pin_nr, enum HalGpioPinState state)
{
    if(state == PIN_STATE_HIGH) {
        gpio_set_pin_high(pin_nr);
    } else if(state == PIN_STATE_TOGGLE) {
        if(gpio_pin_is_high(pin_nr)) {
            gpio_set_pin_low(pin_nr);
        } else {
            gpio_set_pin_high(pin_nr);
        }
    } else {
        gpio_set_pin_low(pin_nr);
    }
}

enum HalGpioPinState HalGpioPinStateGet(HalGpioBase_t port, uint8_t pin_nr)
{
    if(gpio_pin_is_high(pin_nr)) {
        return PIN_STATE_HIGH;
    } else {
        return PIN_STATE_LOW;
    }
}