/******************************************************************************************************************************************
 *  File: Ringbuffer.h
 *  Description: Ring-buffer API.

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

#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <List.h>
#include <OsTypes.h>
#include <PriorRTOSConfig.h>
#include <EventDef.h>

/* Ring-buffer Events. */
#define RINGBUF_EVENT_CREATE         EVENT_TYPE_CREATE
#define RINGBUF_EVENT_DELETE         EVENT_TYPE_DELETE

#ifdef PRTOS_CONFIG_USE_RINGBUFFER_EVENT_DATA_IN_OUT
#define RINGBUF_EVENT_DATA_IN        (EVENT_TYPE_ACCESS | 0x00001000)
#define RINGBUF_EVENT_DATA_OUT       (EVENT_TYPE_ACCESS | 0x00002000)
#endif

#ifdef PRTOS_CONFIG_USE_RINGBUFFER_EVENT_EMPTY_FULL
#define RINGBUF_EVENT_EMPTY          (EVENT_TYPE_STATE_CHANGE | 0x00001000)
#define RINGBUF_EVENT_FULL           (EVENT_TYPE_STATE_CHANGE | 0x00002000)
#endif

#ifdef PRTOS_CONFIG_USE_RINGBUFFER_EVENT_FLUSH
#define RINGBUF_EVENT_FLUSHED        (EVENT_TYPE_STATE_CHANGE | 0x00003000)
#endif

/* Ring-buffer Base Type. */
typedef volatile  U8_t RingbufBase_t;


/******************************************************************************
 * @func: Id_t RingbufCreate(RingbufBase_t *buffer, U32_t size)
 *
 * @desc: Creates a ring-buffer of given size. Note that the width of
 * each element in the buffer is defined by RingbufBase_t (U8_t by default).
 * An external buffer can provided to store the data. If this buffer is not
 * provided it will be allocated internally (dynamic allocation is used).
 *
 * @argin: (RingbufBase_t *) buffer; External buffer. NULL for internal allocation.
 * @argin: (U32_t) size; Ring-buffer size.
 *
 * @rettype:  (Id_t); Ring-buffer ID
 * @retval:   INVALID_ID; if an error occurred during creation.
 * @retval:   Other; valid ID if the ring-buffer was created.
 ******************************************************************************/
Id_t RingbufCreate(RingbufBase_t *buffer, U32_t size);


/******************************************************************************
 * @func: OsResult_t RingbufDelete (Id_t *ringbuf_id)
 *
 * @desc: Deletes the specified ring-buffer. ringbuf_id is set to INVALID_ID if
 * the operation is successful.
 *
 * @argin: (Id_t *) ringbuf_id; ID of the ring-buffer to delete.
 *
 * @rettype:  (OsResult_t); sys call result
 * @retval:   OS_RES_OK; if the ring-buffer was deleted.
 * @retval:   OS_RES_ERROR; if the ring-buffer could not be found.
 ******************************************************************************/
OsResult_t RingbufDelete(Id_t *ringbuf_id);



/******************************************************************************
 * @func: OsResult_t RingbufWrite(Id_t ringbuf_id, RingbufBase_t *data,
 *  U32_t *length, U32_t timeout)
 *
 * @desc: Writes data of given length to the ring-buffer. The
 * amount actually written is returned.
 *
 * @argin: (Id_t) ringbuf_id; Ring-buffer ID.
 * @argin: (RingbufBase_t) data; Array of data.
 * @argin: (U32_t *) length; Length of the data.
 * @argin: (U32_t) timeout; Timeout is ms.
 * @argout: (U32_t *) length; Actual amount written.
 *
 * @rettype:  (OsResult_t); sys call result
 * @retval:   OS_RES_OK; if data was written.
 * @retval:   OS_RES_FAIL; if the buffer is empty.
 * @retval:   OS_RES_LOCKED; if the ringbuffer is already locked for writing.
 * @retval:   OS_RES_ERROR; if the ringbuffer could not be found.
 ******************************************************************************/
OsResult_t RingbufWrite(Id_t ringbuf_id, RingbufBase_t *data, U32_t *length, U32_t timeout);


/******************************************************************************
 * @func: OsResult_t RingbufRead(Id_t ringbuf_id, RingbufBase_t *data,
 * U32_t *amount, U32_t timeout)
 *
 * @desc: Reads a given number of data nodes from the ring-buffer. The
 * amount actually read is returned. Read data is copied to the
 * provided target array. The target array has to comply with the pre-conditions
 * stated below.
 *
 * @argin: (Id_t) ringbuf_id; Ring-buffer ID.
 * @argin: (RingbufBase_t) target; Target data array.Pre-condition: target[0] = 0xFE,
 * target[amount-1] = 0xEF
 * @argin: (U32_t *) amount; Amount of data to read.
 * @argin: (U32_t) timeout; Timeout is ms.
 * @argout:(U32_t *) amount; Actual amount of data read.
 *
 * @rettype:  (OsResult_t); sys call result
 * @retval:   OS_RES_OK; if data was read.
 * @retval:   OS_RES_LOCKED; if the ringbuffer is already locked for reading.
 * @retval:   OS_RES_FAIL; if the buffer is empty.
 * @retval:   OS_RES_OUT_OF_BOUNDS; if the target array was not compliant.
 * @retval:   OS_RES_ERROR; if the ringbuffer could not be found.
 ******************************************************************************/
OsResult_t RingbufRead(Id_t ringbuf_id, RingbufBase_t *target, U32_t *amount, U32_t timeout);


/******************************************************************************
 * @func: U32_t RingbufDump(Id_t ringbuf_id, RingbufBase_t *target)
 *
 * @desc: Dumps all data present in the ring-buffer in the target array.
 * The amount actually read is returned. The target array has to comply
 * with the pre-conditions stated below.
 *
 * @argin: (Id_t) ringbuf_id; Ring-buffer ID.
 * @argin: (RingbufBase_t) target; Target data array. Pre-condition: target[0] = 0xFE,
 * target[ring-buffer size-1] = 0xEF
 *
 * @rettype:  (U32_t); Amount read.
 * @retval: 0; if pre-conditions were not met.
 * @retval: Other; valid amount.
 ******************************************************************************/
U32_t RingbufDump(Id_t ringbuf_id, RingbufBase_t* target);


/******************************************************************************
 * @func: OsResult_t RingbufFlush(Id_t ringbuf_id)
 *
 * @desc: Resets the ring-buffer's read and write locations as well as
 * its current data count resulting in all its initial space becoming available.
 *
 * @argin: (Id_t) ringbuf_id; Ring-buffer ID.
 *
 * @rettype:  (OsResult_t); sys call result
 * @retval:   OS_RES_OK; if the ring-buffer was flushed.
 * @retval:   OS_RES_LOCKED; if the ring-buffer is locked for reading or writing.
 * @retval:   OS_RES_ERROR; if the ring-buffer could not be found.
 ******************************************************************************/
OsResult_t RingbufFlush(Id_t ringbuf_id);


/******************************************************************************
 * @func: U32_t RingbufSearch(Id_t ringbuf_id, RingbufBase_t *query,
 *  U32_t query_length)
 *
 * @desc: Searched the ring-buffer for the given query of given length.
 * The number of occurrences of the query in the buffer is returned.
 *
 * @argin: (Id_t) ringbuf_id; Ring-buffer ID.
 * @argin: (RingbufBase_t) query; Search query.
 * @argin: (U32_t) query_length; Length of the search query.
 *
 * @rettype:  (U32_t); Number of occurrences.
 * @retval: Any; valid amount.
 ******************************************************************************/
U32_t RingbufSearch(Id_t ringbuf_id, RingbufBase_t *query, U32_t query_length);

/* Returns the index at which the query first occurs. */
U32_t RingbufSearchIndex(Id_t ringbuf_id, RingbufBase_t *query, U32_t query_length);


/******************************************************************************
 * @func: U32_t RingbufDataCountGet(Id_t ringbuf_id)
 *
 * @desc: Returns the amount of data is present in the ring-
 * buffer.
 *
 * @argin: (Id_t) ringbuf_id; Ring-buffer ID.
 *
 * @rettype:  (U32_t); Number of data nodes.
 * @retval: Any; valid amount.
 ******************************************************************************/
U32_t RingbufDataCountGet(Id_t ringbuf_id);


/******************************************************************************
 * @func: U32_t RingbufDataSpaceGet(Id_t ringbuf_id)
 *
 * @desc: Returns the amount of space left in the ring-buffer.
 *
 * Arguments:
 * @argin: (Id_t) ringbuf_id; Ring-buffer ID.
 *
 * @rettype:  (U32_t); Data space left.
 * @retval: Any; valid amount.
 ******************************************************************************/
U32_t RingbufDataSpaceGet(Id_t ringbuf_id);


#ifdef __cplusplus
}
#endif
#endif /* RINGBUFFER_H_ */
