/*
 * Hal_adc.c
 *
 * Created: 20-Apr-17 18:07:42
 *  Author: User
 */

#include "Hal_adc.h"
#include <avr/io.h>

uint32_t ConversionResults[HAL_ADC_N_CHANNELS];

void HalAdcInit(struct HalAdcInstance *adc_handle)
{
    ADMUX = (1<<REFS0); // AREF = AVcc
    ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); //ADC Enable and prescaler 128
    for (uint8_t i = 0; i < HAL_ADC_N_CHANNELS; i++) {
        ConversionResults[i] = 0;
    }
}

uint8_t HalAdcConversionStart(struct HalAdcInstance *hal_handle, uint8_t channel)
{
    if(!HalAdcConversionIsDone(hal_handle, channel)) {
        return 0;
    }
    channel &= 0b00000111;  // AND operation with 7
    ADMUX = (ADMUX & 0xF8)| channel; //Select the channel with a safety mask
    ADCSRA |= (1<<ADSC); //Start single conversion, ADSC => 1
    return 1;
}

bool HalAdcConversionIsDone(struct HalAdcInstance *hal_handle, uint8_t channel)
{
    return (!(ADCSRA & (1<<ADSC)));
}

/* Returns the result of the last completed sample. Non-blocking. */
uint32_t HalAdcConversionResultGet(struct HalAdcInstance *hal_handle, uint8_t channel)
{
    if(HalAdcConversionIsDone(hal_handle, channel)) {
        HalAdcConversionResultGetNew(hal_handle, channel);
    }
    return ConversionResults[channel];
}

/* Waits for the current conversion to be done and returns the result. Blocking. */
uint32_t HalAdcConversionResultGetNew(struct HalAdcInstance *hal_handle, uint8_t channel)
{
    while(!HalAdcConversionIsDone(hal_handle, channel));
    ConversionResults[channel] = ADC;
    return ConversionResults[channel];
}

