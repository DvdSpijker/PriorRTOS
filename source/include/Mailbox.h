/******************************************************************************************************************************************
 *  File: Mailbox.h
 *  Description: Mailbox API.

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


#ifndef MAILBOX_H_
#define MAILBOX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <OsTypes.h>
#include <PriorRTOSConfig.h>
#include <EventDef.h>

/* Mailbox Event macros */
#define MAILBOX_EVENT_CREATE         EVENT_TYPE_CREATE
#define MAILBOX_EVENT_DELETE         EVENT_TYPE_DELETE

#ifdef PRTOS_CONFIG_USE_EVENT_MAILBOX_POST_PEND
#define MAILBOX_EVENT_POST(addr)     (EVENT_TYPE_ACCESS | 0x00001000 | (U8_t)addr)
#define MAILBOX_EVENT_PEND(addr)     (EVENT_TYPE_ACCESS | 0x00002000 | (U8_t)addr)
#define MAILBOX_EVENT_POST_ALL		 (EVENT_TYPE_ACCESS | 0x000020FF)
#endif

/* Mailbox width definition */
typedef U8_t MailboxBase_t; //Allowed to be modified by user

/******************************************************************************
 * @func: Id_t MailboxCreate(U8_t mailbox_size, Task_t owners[],
 *               U8_t n_owners)
 *
 * @desc: Creates a mailbox of the specified size with given
 * owners. Only the owners are allowed pend from the created mailbox.
 * The pend counter of each address is set to the number of owners upon
 * posting.
 * Note the width of each mailbox address is defined by MailboxBase_t (U8_t
 * by default).
 *
 * Arguments:
 * @argin: (U8_t) mailbox_size; Number of addresses reserved for this mailbox.
 * @argin: (Id_t) owner_ids; Array of owner task IDs.
 * @argin: (U8_t) n_owners; Number of owners in the owners list.
 *
 * @rettype:  (Id_t) Mailbox ID
 * @retval:   INVALID_ID; if the creation failed.
 * @retval:   Other; valid ID if the mailbox was created.
 ******************************************************************************/
Id_t MailboxCreate(U8_t mailbox_size, Id_t owner_ids[], U8_t n_owners);


/******************************************************************************
 * @func: OsResult_t MailboxDelete(Id_t *mailbox_id)
 *
 * @desc: Deletes the specified mailbox. mailbox_id is set to INVALID_ID.
 *
 * Arguments:
 *  @argout: (Id_t *) mailbox_id; ID of the mailbox to delete.
 *
 * @rettype:  (OsResult_t) sys call result
 * @retval:   OS_RES_OK; if the mailbox was deleted.
 * @retval:   OS_RES_ERROR; if the mailbox could not be found.
 ******************************************************************************/
OsResult_t MailboxDelete(Id_t *mailbox_id);


/******************************************************************************
 * @func: OsResult_t MailboxPost(Id_t mailbox_id, U8_t address, MailboxBase_t data, 
 *                               U32_t timeout)
 *
 * @desc: Post data  in the mailbox starting at the address. Data cannot be 
 * overwritten when it has not been pended by its owner i.e. the pend counter is not 0.
 * Any task can post in any mailbox.
 *
 * Arguments:
 * @argin: (Id_t) mailbox_id; ID of the mailbox to post in.
 * @argin: (U8_t) address; Address where data will be posted.
 * @argin: (MailboxBase_t) data; Data to be posted.
 *
 * @rettype:  (OsResult_t) sys call result
 * @retval:   OS_RES_OK; if the mailbox was deleted.
 * @retval:   OS_RES_ERROR; if the mailbox could not be found.
 * @retval:   OS_RES_LOCKED; if the pend counter of one of the addresses within
 *            the specified range is not 0.
 * @retval:   OS_RES_OUT_OF_BOUNDS; if the address is not part of the mailbox address range.
 ******************************************************************************/
OsResult_t MailboxPost(Id_t mailbox_id, U8_t address, MailboxBase_t data, U32_t timeout);


/******************************************************************************
 * @func: OsResult_t MailboxPend(Id_t mailbox_id, U8_t base_address,
 *               MailboxBase_t *data, U8_t len)
 *
 * @desc: Pend data with given length from the mailbox starting at the
 * base-address. Pending data decrements the pend counter. Only owner
 * tasks may pend from the mailbox.
 *
 * Arguments:
 * @argin: (Id_t) mailbox_id; ID of the mailbox to pend from.
 * @argin: (U8_t) base_address; Starting address where data will be pended.
 * @argin: (MailboxBase_t *) data; Pointer to the array where the data will
 *           be copied to.
 *
 * @rettype:  (OsResult_t) sys call result
 * @retval:   OS_RES_OK; if the mailbox was deleted.
 * @retval:   OS_RES_ERROR; if the mailbox could not be found.
 * @retval:   OS_RES_LOCKED; if the pend counter is already 0.
 * @retval:   OS_RES_RESTRICTED; if the pending task is not an owner.
 * @retval:   OS_RES_OUT_OF_BOUNDS; if the address is not part of the mailbox address range.
 ******************************************************************************/
OsResult_t MailboxPend(Id_t mailbox_id, U8_t address,  MailboxBase_t *data, U32_t timeout);


/******************************************************************************
 * @func: OsResult_t MailboxPendCounterGet(Id_t mailbox_id, U8_t address)
 *
 * @desc: Returns the pend counter of the given address. A mailbox-
 * address can only be posted to if the pend counter is 0. A mailbox-
 * address can only be pended from if the pend counter is >0.
 *
 * Arguments:
 * @argin: (Id_t) mailbox_id; ID of the mailbox.
 * @argin: (U8_t) address; Mailbox address.
 *
 * @rettype:  (U8_t) pend counter.
 ******************************************************************************/
U8_t MailboxPendCounterGet(Id_t mailbox_id, U8_t address);


#ifdef __cplusplus
}
#endif
#endif /* MAILBOX_H_ */
