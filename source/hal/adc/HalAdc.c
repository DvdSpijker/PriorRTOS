/*
 * Hal_adc.c
 *
 * Created: 20-Apr-17 18:07:42
 *  Author: User
 */
#include <adc.h>
#include "HalAdc.h"

uint32_t LastConversionResult;

void HalAdcInit(struct HalAdcInstance *adc_handle)
{
    LastConversionResult = 0;
    adc_configure(&AVR32_ADC);
    adc_enable(&AVR32_ADC, adc_handle->channel);
}

uint8_t HalAdcConversionStart(struct HalAdcInstance *hal_handle)
{

    adc_start(&AVR32_ADC);

    return 1;
}

bool HalAdcConversionIsDone(struct HalAdcInstance *hal_handle)
{
    return (adc_check_eoc(&AVR32_ADC, hal_handle->channel));
}

/* Returns the result of the last completed sample. Non-blocking. */
uint32_t HalAdcConversionResultGet(struct HalAdcInstance *hal_handle)
{
    return LastConversionResult;
}

/* Waits for the current conversion to be done and returns the result. Blocking. */
uint32_t HalAdcConversionResultGetNew(struct HalAdcInstance *hal_handle)
{
    LastConversionResult = adc_get_value(&AVR32_ADC, hal_handle->channel);
    return LastConversionResult;
}

