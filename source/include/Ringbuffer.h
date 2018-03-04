/******************************************************************************************************************************************
 *  File: Ringpacket.h
 *  Description: Ring-packet API.

 *  OS Version: V0.4
 *
 *  Author(s)
 *  -----------------
 *  D. van de Spijker
 *  -----------------
 *
 *  Copyright© 2017    D. van de Spijker
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software AND associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights to use,
 *  copy, modify, merge, publish, distribute and/or sell copies of the Software,
 *  and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *  this list of conditions and the following disclaimer.
 *
 *  2. The name of Prior RTOS may not be used to endorse or promote products derived
 *    from this Software without specific written permission.
 *
 *  3. This Software may only be redistributed and used in connection with a
 *    product in which Prior RTOS is integrated. Prior RTOS shall not be
 *    distributed or sold, under a different name or otherwise, as a standalone product.
 *
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 *  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**********************************************************************************************************************************************/

#ifndef RINGpacket_H_
#define RINGpacket_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <List.h>
#include <Types.h>
#include <PriorRTOSConfig.h>
#include <EventDef.h>

/* Ring-packet Events. */
#define RINGpacket_EVENT_CREATE         EVENT_TYPE_CREATE
#define RINGpacket_EVENT_DELETE         EVENT_TYPE_DELETE

#ifdef PRTOS_CONFIG_USE_EVENT_RINGpacket_DATA_IN_OUT
#define RINGpacket_EVENT_DATA_IN        (EVENT_TYPE_ACCESS | 0x00001000)
#define RINGpacket_EVENT_DATA_OUT       (EVENT_TYPE_ACCESS | 0x00002000)
#endif

#ifdef PRTOS_CONFIG_USE_EVENT_RINGpacket_EMPTY_FULL
#define RINGpacket_EVENT_EMPTY          (EVENT_TYPE_STATE_CHANGE | 0x00001000)
#define RINGpacket_EVENT_FULL           (EVENT_TYPE_STATE_CHANGE | 0x00002000)
#endif

#ifdef PRTOS_CONFIG_USE_EVENT_RINGpacket_FLUSH
#define RINGpacket_EVENT_FLUSHED        (EVENT_TYPE_STATE_CHANGE | 0x00003000)
#endif

/* Ring-packet Base Type. */
typedef volatile  U8_t RingpacketBase_t;


/******************************************************************************
 * @func: Id_t RingpacketCreate(U32_t size)
 *
 * @desc: Creates a ring-packet of given size. Note that the width of
 * each node in the packet is defined by RingpacketBase_t (U8_t by default).
 *
 * Arguments:
 * @argin: (U32_t) size; Ring-packet size in nodes.
 *
 * @rettype:  (Id_t) Ring-packet ID
 * @retval:   INVALID_ID; if an error occurred during creation.
 * @retval:   Other; valid ID if the ring-packet was created.
 ******************************************************************************/
Id_t RingpacketCreate(RingpacketBase_t *packet, U32_t size);


/******************************************************************************
 * @func: OsResult_t RingpacketDelete (Id_t *ringpacket_id)
 *
 * @desc: Deletes the specified ring-packet. ringpacket_id is set to INVALID_ID if
 * the operation is successful.
 *
 * Arguments:
 * @argin: (Id_t *) ringpacket_id; ID of the ring-packet to delete.
 *
 * @rettype:  (OsResult_t) sys call result
 * @retval:   OS_RES_OK; if the ring-packet was deleted.
 * @retval:   OS_RES_ERROR; if the ring-packet could not be found.
 ******************************************************************************/
OsResult_t RingpacketDelete(Id_t *ringpacket_id);



/******************************************************************************
 * @func: OsResult_t RingpacketWrite(Id_t ringpacket_id, RingpacketBase_t *data,
 *              U32_t *length)
 *
 * @desc: Writes a given number of data nodes to the ring-packet. The
 * number of nodes actually written is returned.
 *
 * Arguments:
 * @argin: (Id_t) ringpacket_id; Ring-packet ID.
 * @argin: (RingpacketBase_t) data; Array of data nodes.
 * @argin: (U32_t *) length; Length of data in nodes.
 * @argout: (U32_t *) length; Actual amount written.
 *
 * @rettype:  (OsResult_t) sys call result
 * @retval:   OS_RES_OK; if data was written.
 * @retval:   OS_RES_FAIL; if the packet is empty.
 * @retval:   OS_RES_LOCKED; if the ringpacket is already locked for writing.
 * @retval:   OS_RES_ERROR; if the ringpacket could not be found.
 ******************************************************************************/
OsResult_t RingpacketWrite(Id_t ringpacket_id, RingpacketBase_t *data, U32_t *length, U32_t timeout);


/******************************************************************************
 * @func: OsResult_t RingpacketRead(Id_t ringpacket_id, RingpacketBase_t *data,
 *              U32_t *amount)
 *
 * @desc: Reads a given number of data nodes from the ring-packet. The
 * number of nodes actually read is returned. Read data is copied to the
 * provided target array. The target array has to comply with the pre-conditions
 * stated below.
 *
 * Arguments:
 * @argin: (Id_t) ringpacket_id; Ring-packet ID.
 * @argin: (RingpacketBase_t) target; Target data array.
 *                        Pre-condition: target[0] = 0xFE, target[amount-1] = 0xEF
 * @argin: (U32_t *) amount; Amount of data to read.
 * @argout:(U32_t *) amount; Actual amount of data read.
 *
 *
 * @rettype:  (OsResult_t) sys call result
 * @retval:   OS_RES_OK; if data was read.
 * @retval:   OS_RES_LOCKED; if the ringpacket is already locked for reading.
 * @retval:   OS_RES_FAIL; if the packet is empty.
 * @retval:   OS_RES_OUT_OF_BOUNDS; if the target array was not compliant.
 * @retval:   OS_RES_ERROR; if the ringpacket could not be found.
 ******************************************************************************/
OsResult_t RingpacketRead(Id_t ringpacket_id, RingpacketBase_t *target, U32_t *amount, U32_t timeout);


/******************************************************************************
 * @func: U32_t RingpacketDump(Id_t ringpacket_id, RingpacketBase_t *target)
 *
 * @desc: Dumps all data present in the ring-packet in the target array.
 * The number of nodes actually read is returned. The target array has to comply
 * with the pre-conditions stated below.
 *
 * Arguments:
 * @argin: (Id_t) ringpacket_id; Ring-packet ID.
 * @argin: (RingpacketBase_t) target; Target data array.
 *                        Pre-condition: target[0] = 0xFE,
 *                        target[ring-packet size-1] = 0xEF
 *
 *
 * @rettype:  (U32_t) Actual amount of nodes read from the ring-packet. 0 if
 *                  pre-conditions were not compliant.
 ******************************************************************************/
U32_t RingpacketDump(Id_t ringpacket_id, RingpacketBase_t* target);


/******************************************************************************
 * @func: OsResult_t RingpacketFlush(Id_t ringpacket_id)
 *
 * @desc: Resets the ring-packet's read and write locations as well as
 * its current data count resulting in all its initial space becoming available.
 *
 * Arguments:
 * @argin: (Id_t) ringpacket_id; Ring-packet ID.
 *
 * @rettype:  (OsResult_t) sys call result
 * @retval:   OS_RES_OK; if the ring-packet was flushed.
 * @retval:   OS_RES_LOCKED; if the ring-packet is locked for reading or writing.
 * @retval:   OS_RES_ERROR; if the ring-packet could not be found.
 ******************************************************************************/
OsResult_t RingpacketFlush(Id_t ringpacket_id);


/******************************************************************************
 * @func: U32_t RingpacketSearch(Id_t ringpacket_id, RingpacketBase_t *query,
 *              U32_t query_length)
 *
 * @desc: Searched the ring-packet for the given query of given length.
 * The number of occurrences of the query in the packet is returned.
 *
 * Arguments:
 * @argin: (Id_t) ringpacket_id; Ring-packet ID.
 * @argin: (RingpacketBase_t) query; Search query.
 * @argin: (U32_t) query_length; Length of the search query.
 *
 * @rettype:  (U32_t) Number of occurrences.
 ******************************************************************************/
U32_t RingpacketSearch(Id_t ringpacket_id, RingpacketBase_t *query, U32_t query_length);

/* Returns the index at which the query first occurs. */
U32_t RingpacketSearchIndex(Id_t ringpacket_id, RingpacketBase_t *query, U32_t query_length);


/******************************************************************************
 * @func: U32_t RingpacketDataCountGet(Id_t ringpacket_id)
 *
 * @desc: Returns the amount of data nodes are present in the ring-
 * packet.
 *
 * Arguments:
 * @argin: (Id_t) ringpacket_id; Ring-packet ID.
 *
 * @rettype:  (U32_t) Number of data nodes.
 ******************************************************************************/
U32_t RingpacketDataCountGet(Id_t ringpacket_id);


/******************************************************************************
 * @func: U32_t RingpacketDataSpaceGet(Id_t ringpacket_id)
 *
 * @desc: Returns the amount of space left in the ring-packet.
 *
 * Arguments:
 * @argin: (Id_t) ringpacket_id; Ring-packet ID.
 *
 * @rettype:  (U32_t) Data space left.
 ******************************************************************************/
U32_t RingpacketDataSpaceGet(Id_t ringpacket_id);


#ifdef __cplusplus
}
#endif
#endif /* RINGpacket_H_ */
