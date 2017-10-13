/*
 * Peripheral.h
 *
 * Created: 2-10-2017 13:06:20
 *  Author: Dorus
 */


#ifndef PERIPHERAL_H_
#define PERIPHERAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <Types.h>

#include <PriorRTOSConfig.h>

#if PRTOS_CONFIG_ENABLE_PERPHERALS==1

#if PRTOS_CONFIG_ENABLE_GPIO==1
#include "../hal/gpio/HalGpio.h"
#endif

#if PRTOS_CONFIG_ENABLE_UART==1
#include "../hal/uart/HalUart.h"
#endif

#if PRTOS_CONFIG_ENABLE_HARDWARE_TIMERS==1
#include "../hal/timer/HalTimer.h"
#endif

#if PRTOS_CONFIG_ENABLE_ADC==1
#include "../hal/adc/HalAdc.h"
#endif

#endif


typedef U8_t PeripheralBase_t;

typedef OsResult_t (*PeripheralCallbackStandard)(void *hal_instance);
typedef OsResult_t (*PeripheralCallbackRwSingle)(void *hal_instance, PeripheralBase_t *data);
typedef OsResult_t (*PeripheralCallbackRwBlock)(void *hal_instance, PeripheralBase_t *data, U32_t length);

struct PeripheralDescriptor {


    Id_t write_buffer;
    Id_t read_buffer;

    void *hal_instance;

    PeripheralCallbackStandard init;
    PeripheralCallbackStandard open;
    PeripheralCallbackStandard close;

    PeripheralCallbackRwSingle write_single;
    PeripheralCallbackRwBlock  write_packet;

    PeripheralCallbackRwSingle read_single;
    PeripheralCallbackRwBlock  read_packet;

};

OsResult_t PeripheralInit(struct PeripheralDescriptor *periph_desc);

OsResult_t PeripheralOpen(struct PeripheralDescriptor *periph_desc);

OsResult_t PeripheralClose(struct PeripheralDescriptor *periph_desc);

OsResult_t PeripheralWriteSingle(struct PeripheralDescriptor *periph_desc, PeripheralBase_t data);

OsResult_t PeripheralWriteBlock(struct PeripheralDescriptor *periph_desc, PeripheralBase_t *data, U32_t length);

OsResult_t PeripheralReadSingle(struct PeripheralDescriptor *periph_desc, PeripheralBase_t *target);

OsResult_t PeripheralReadBlock(struct PeripheralDescriptor *periph_desc, PeripheralBase_t *target, U32_t length);

/* Initializes all peripherals. */
void HalInit (void);


#ifdef __cplusplus
}
#endif
#endif /* PERIPHERAL_H_ */