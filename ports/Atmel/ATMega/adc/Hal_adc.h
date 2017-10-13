/*
 * Hal_adc.h
 *
 * Created: 20-Apr-17 18:00:18
 *  Author: User
 */


#ifndef HAL_ADC_H_
#define HAL_ADC_H_

#include <stdint.h>
#include <stdbool.h>

#define HAL_ADC_N_CHANNELS 8

#define HAL_ADC_RESOLUTION_8_BIT  0x00
#define HAL_ADC_RESOLUTION_10_BIT 0x01
#define HAL_ADC_RESOLUTION_12_BIT 0x02
#define HAL_ADC_RESOLUTION_16_BIT 0x03
#define HAL_ADC_RESOLUTION_24_BIT 0x04
#define HAL_ADC_RESOLUTION_32_BIT 0x05

#define HAL_ADC_MODE_SINGLE     0x00
#define HAL_ADC_MODE_CONTINUOUS 0x01

#define HAL_ADC_CHANNEL_0 0x00
#define HAL_ADC_CHANNEL_1 0x01
#define HAL_ADC_CHANNEL_2 0x02
#define HAL_ADC_CHANNEL_3 0x03
#define HAL_ADC_CHANNEL_4 0x04
#define HAL_ADC_CHANNEL_5 0x05
#define HAL_ADC_CHANNEL_6 0x06
#define HAL_ADC_CHANNEL_7 0x07

typedef uint8_t HalAdcBase_t;

struct HalAdcInstance {
    HalAdcBase_t adc;
    uint8_t     resolution;
    uint8_t     mode;
    uint16_t    f_adc_clock;
};

void HalAdcInit(struct HalAdcInstance *adc_handle);

uint8_t HalAdcConversionStart(struct HalAdcInstance *hal_handle, uint8_t channel);

bool HalAdcConversionIsDone(struct HalAdcInstance *hal_handle, uint8_t channel);

/* Returns the result of the last completed sample. Non-blocking. */
uint32_t HalAdcConversionResultGet(struct HalAdcInstance *hal_handle, uint8_t channel);

/* Waits for the current conversion to be done and returns the result. Blocking. */
uint32_t HalAdcConversionResultGetNew(struct HalAdcInstance *hal_handle, uint8_t channel);


#endif /* HAL_ADC_H_ */