/*
 * HalGpio.c
 *
 * Created: 19-Apr-17 11:57:21
 *  Author: User
 */

#include "Hal_gpio.h"

void HalGpioPinModeSet(HalGpioBase_t ddr, uint8_t pin_nr, enum HalGpioPinMode mode)
{
    if(mode == PIN_MODE_INPUT) {
        *ddr &= ~(1 << pin_nr);
    } else {
        *ddr |= (1 << pin_nr);
    }
}

enum HalGpioPinMode HalGpioPinModeGet(HalGpioBase_t ddr, uint8_t pin_nr)
{
    uint8_t mask = (1 << pin_nr);
    return ((*ddr & mask) ? PIN_MODE_OUTPUT : PIN_MODE_INPUT);
}

void HalGpioPinStateSet(HalGpioBase_t port, uint8_t pin_nr, enum HalGpioPinState state)
{
    if(state == PIN_STATE_HIGH) {
        *port |= (1 << pin_nr);
    } else if(state == PIN_STATE_TOGGLE) {
        uint8_t mask = (1 << pin_nr);
        if(*port & mask) {
            *port &= ~mask;
        } else {
            *port |= mask;
        }
    } else {
        *port &= ~(1 << pin_nr);
    }
}

enum HalGpioPinState HalGpioPinStateGet(HalGpioBase_t port, uint8_t pin_nr)
{
    uint8_t mask = (1 << pin_nr);
    if(*port & mask) {
        return PIN_STATE_HIGH;
    } else {
        return PIN_STATE_LOW;
    }
}