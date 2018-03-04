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

typedef U8_t PeripheralBase_t;

typedef OsResult_t (*PeripheralCallbackStandard)(void *hal_instance);
typedef OsResult_t (*PeripheralCallbackRwSingle)(void *hal_instance, PeripheralBase_t *data);
typedef OsResult_t (*PeripheralCallbackRwBlock)(void *hal_instance, PeripheralBase_t *data, U32_t *size);

struct PeripheralDescriptor {
	
	/* In V0.4.0 packet linking is only supported for Ringpackets. */
    Id_t write_packet;	/* Must be set to OS_RES_ID_INVALID if no packet is used. */
    Id_t read_packet;	/* Must be set to OS_RES_ID_INVALID if no packet is used. */

    void *hal_instance; /* Link to a HAL instance object, set NULL if unused. */

    PeripheralCallbackStandard init;	/* Init Callback, set NULL if unused. */
    PeripheralCallbackStandard open;	/* Open Callback, set NULL if unused. */
    PeripheralCallbackStandard close;	/* Close Callback, set NULL if unused. */

    PeripheralCallbackRwSingle write_single;	/* Write Single Callback, set NULL if unused. */
    PeripheralCallbackRwBlock  write_block;		/* Write Block Callback, set NULL if unused. */

    PeripheralCallbackRwSingle read_single;		/* Read Single Callback, set NULL if unused. */
    PeripheralCallbackRwBlock  read_block;		/* Read Block Callback, set NULL if unused. */

};

OsResult_t PeripheralInit(struct PeripheralDescriptor *periph_desc);

OsResult_t PeripheralOpen(struct PeripheralDescriptor *periph_desc);

OsResult_t PeripheralClose(struct PeripheralDescriptor *periph_desc);

/* Write single data to the peripheral. Calls the peripheral's write_single callback directly if no write_packet is not used.
 * If a packet is used the data is written to the packet. */
OsResult_t PeripheralWriteSingle(struct PeripheralDescriptor *periph_desc, PeripheralBase_t *data);

/* Write a block of data of a given size to the peripheral. Calls the peripheral's write_block callback directly if no write_packet is not used.
 * If a packet is used the data is written to the packet.
 * *size must contain the block size before the call. It will contain the size actually written after the call. */
OsResult_t PeripheralWriteBlock(struct PeripheralDescriptor *periph_desc, PeripheralBase_t *data, U32_t *size);

OsResult_t PeripheralReadSingle(struct PeripheralDescriptor *periph_desc, PeripheralBase_t *target);

OsResult_t PeripheralReadBlock(struct PeripheralDescriptor *periph_desc, PeripheralBase_t *target, U32_t *size);


#ifdef __cplusplus
}
#endif
#endif /* PERIPHERAL_H_ */