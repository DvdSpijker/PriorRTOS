/**********************************************************************************************************************************************
 *  File: Prior_mail.c
 *
 *  Description: Prior Mailbox module
 *
 *  OS Version: V0.4
 *  Date: 13/03/2015
 *
 *  Author(s)
 *  -----------------
 *  D. van de Spijker
 *  -----------------
 *
 *
 *  Copyrightę 2017    D. van de Spijker
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software AND associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights to use,
 *  copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
 *  and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *  this list of conditions and the following disclaimer.
 *
 *  2. The name of Prior RTOS may not be used to endorse or promote products derived
 *    from this Software without specific prior written permission.
 *
 *  3. This Software may only be redistributed and used in connection with a
 *    product in which Prior RTOS is integrated. Prior RTOS shall not be
 *    distributed, under a different name or otherwise, as a standalone product.
 *
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 *  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 **********************************************************************************************************************************************/

#include <List.h>
#include <Mailbox.h>
#include <Event.h>
#include <MailboxDef.h>
#include <CoreDef.h>
#include <MemoryDef.h>
#include <TaskDef.h>
#include <Memory.h>
#include <SystemCall.h>

#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdbool.h>

static bool ITaskIsOwner(pMailbox_t mailbox, pTcb_t tcb);

LinkedList_t MailboxList;

OsResult_t KMailboxInit(void)
{
    ListInit(&MailboxList, ID_TYPE_MAILBOX);

    return OS_RES_OK;
}

Id_t MailboxCreate(U8_t mailbox_size, Id_t owner_ids[], U8_t n_owners)
{

    //Check input boundaries
    if(n_owners > MAILBOX_CONFIG_MAX_OWNERS) {
        return OS_ID_INVALID;
    }

    void *buffer = NULL;
    pMailbox_t new_mailbox = (pMailbox_t)KMemAllocObject(sizeof(Mailbox_t), mailbox_size, &buffer); //Allocate memory for mailbox struct
    if(new_mailbox == NULL) {
        return OS_ID_INVALID;    //Allocation error
    }

    new_mailbox->buffer = (MailboxBase_t *)buffer;
    new_mailbox->pend_counters = (U8_t*)KMemAllocObject(sizeof(U8_t) * mailbox_size, 0, NULL); //Allocate memory for mailbox data
    if(new_mailbox->pend_counters == NULL) {
        KMemFreeObject((void **)&new_mailbox, &buffer);
        return OS_ID_INVALID;
    }

    ListNodeInit(&new_mailbox->list_node, (void*)new_mailbox);
    if(ListNodeAddSorted(&MailboxList, &new_mailbox->list_node) != OS_RES_OK) {
        KMemFreeObject((void **)&new_mailbox->pend_counters, NULL);
        KMemFreeObject((void **)&new_mailbox, &buffer);
        return OS_ID_INVALID;
    }

    new_mailbox->size = mailbox_size;
    new_mailbox->n_owners = n_owners;
    for (U8_t i = 0; i < n_owners; i++) {
        new_mailbox->owner_ids[i] = owner_ids[i];
    }

    return ListNodeIdGet(&new_mailbox->list_node);
}

OsResult_t MailboxDelete(Id_t *mailbox_id)
{
    pMailbox_t mailbox = KMailboxFromId(*mailbox_id);
    if (mailbox != NULL) {
        ListNodeDeinit(&MailboxList, &mailbox->list_node);
        KMemFreeObject((void **)&mailbox->pend_counters, NULL);
        KMemFreeObject((void **)&mailbox, (void **)&mailbox->buffer);
        *mailbox_id = OS_ID_INVALID;
        return OS_RES_OK;
    }
    return OS_RES_ERROR;
}

OsResult_t MailboxPost(Id_t mailbox_id, U8_t address, MailboxBase_t data, U32_t timeout)
{

    OsResult_t result = OS_RES_LOCKED;

#ifdef PRTOS_CONFIG_USE_EVENT_MAILBOX_POST_PEND
    SYSTEM_CALL_WAIT_HANDLE_EVENT;

    SYSTEM_CALL_POLL_HANDLE_EVENT(mailbox_id, MAILBOX_EVENT_PEND(address), &result) {
        /* Do nothing. Normal execution flow. */
    }
    SYSTEM_CALL_POLL_HANDLE_TIMEOUT(&result) {
        return result;
    }
    SYSTEM_CALL_POLL_HANDLE_POLL(&result) {
        return result;
    }
#endif

    LIST_NODE_ACCESS_WRITE_BEGIN(&MailboxList, mailbox_id) {
        result = OS_RES_OK;
        pMailbox_t mailbox = ListNodeChildGet(node);
        if(mailbox == NULL) {
            result = OS_RES_ERROR;    //Search error
        }

        if(result == OS_RES_OK) {

            /* Validate address range. */
            if(address > (mailbox->size - 1)) {
                result = OS_RES_OUT_OF_BOUNDS;
            }

            /* Check pend counter, if this is !0 then posting at this address is not allowed.
             * wait/poll for a pend event. */
            if(result == OS_RES_OK) {
                if(mailbox->pend_counters[address] != 0) {
#ifdef PRTOS_CONFIG_USE_EVENT_MAILBOX_POST_PEND
                	if(timeout != OS_TIMEOUT_NONE) {
                		SYSTEM_CALL_POLL_WAIT_EVENT(node, mailbox_id, MAILBOX_EVENT_PEND(address), &result, timeout);
                	} else {
                		result = OS_RES_LOCKED;
                	}
#else
                    result = OS_RES_LOCKED;
#endif
                }

                /* If the pend counter is 0 the address can be accessed.
                 * After posting a post event is emitted. */
                if(result == OS_RES_OK) {
                    mailbox->buffer[address] = data;
                    mailbox->pend_counters[address] = mailbox->n_owners;
#ifdef PRTOS_CONFIG_USE_EVENT_MAILBOX_POST_PEND
                    EventEmit(mailbox_id, MAILBOX_EVENT_POST(address), EVENT_FLAG_NONE);
                    EventEmit(mailbox_id, MAILBOX_EVENT_POST_ALL, EVENT_FLAG_NONE);
#endif
                }
            }
        }
    }
    LIST_NODE_ACCESS_END();
    return result;
}

OsResult_t MailboxPend(Id_t mailbox_id, U8_t address, MailboxBase_t *data, U32_t timeout)
{

    pTcb_t task = TcbRunning;
    OsResult_t result = OS_RES_LOCKED;


#ifdef PRTOS_CONFIG_USE_EVENT_MAILBOX_POST_PEND
    SYSTEM_CALL_WAIT_HANDLE_EVENT;

    SYSTEM_CALL_POLL_HANDLE_EVENT(mailbox_id, MAILBOX_EVENT_PEND(address), &result) {
        /* Do nothing. Normal execution flow. */
    }
    SYSTEM_CALL_POLL_HANDLE_TIMEOUT(&result) {
        return result;
    }
    SYSTEM_CALL_POLL_HANDLE_POLL(&result) {
        return result;
    }
#endif


    /* Lock in write because the pend counters are modified. */
    LIST_NODE_ACCESS_WRITE_BEGIN(&MailboxList, mailbox_id) {
        result = OS_RES_OK;
        pMailbox_t mailbox = ListNodeChildGet(node);

        /* Validate ownership of this mailbox. */
        if(ITaskIsOwner(mailbox, task) == false) {
            result = OS_RES_RESTRICTED;
        }

        if(result == OS_RES_OK) {

            /* Validate address range. */
            if(address > (mailbox->size - 1)) {
                result = OS_RES_OUT_OF_BOUNDS;
            }

            if(result == OS_RES_OK) {

                /* Check pend counter, if this is 0 there is nothing to pend and we can
                 * wait/poll for a post event. */
                if(mailbox->pend_counters[address] == 0) {
#ifdef PRTOS_CONFIG_USE_EVENT_MAILBOX_POST_PEND
                    SYSTEM_CALL_POLL_WAIT_EVENT(node, mailbox_id, MAILBOX_EVENT_POST(address), &result, timeout);
#else
                    result = OS_RES_LOCKED;
#endif
                }

                /* If the pend counter != 0 the address can be accessed.
                 * After pending a pend event is emitted. */
                if(result == OS_RES_OK) {
                    *data = mailbox->buffer[address];
#ifdef PRTOS_CONFIG_USE_EVENT_MAILBOX_POST_PEND
                    EventEmit(mailbox_id, MAILBOX_EVENT_PEND(address), EVENT_FLAG_NONE);
#endif
                }
            }
        }
    }
    LIST_NODE_ACCESS_END();
    return result;
}

U8_t MailboxPendCounterGet(Id_t mailbox_id, U8_t address)
{
    U8_t pend_counter = 0;
    LIST_NODE_ACCESS_READ_BEGIN(&MailboxList, mailbox_id) {
        pMailbox_t mailbox = ListNodeChildGet(node);
        pend_counter = mailbox->pend_counters[address];
    }
    LIST_NODE_ACCESS_END();
    return pend_counter;
}

pMailbox_t KMailboxFromId(Id_t mailbox_id)
{
    ListNode_t *node = ListSearch(&MailboxList, mailbox_id);
    if(node != NULL) {
        return (pMailbox_t)ListNodeChildGet(node);
    } else {
        return NULL;
    }
}

static bool ITaskIsOwner(pMailbox_t mailbox, pTcb_t tcb)
{
    bool is_owner = false;
    for (U8_t i = 0; i < mailbox->n_owners; i++) {
        if(mailbox->owner_ids[i] == tcb->list_node.id) {
            is_owner = true;
            break;
        }
    }

    return is_owner;
}
