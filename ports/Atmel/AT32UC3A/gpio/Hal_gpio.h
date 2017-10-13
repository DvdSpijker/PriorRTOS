/*
 * HalGpio.h
 *
 * Created: 19-Apr-17 11:57:05
 *  Author: User
 */ 


#ifndef HALGPIO_H_
#define HALGPIO_H_

#include <stdint.h>
#include <gpio.h>

enum HalGpioPinMode { 
    PIN_MODE_INPUT = 0, 
    PIN_MODE_OUTPUT = 1,
    PIN_MODE_INVALID =2,
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



#endif /* HALGPIO_H_ */