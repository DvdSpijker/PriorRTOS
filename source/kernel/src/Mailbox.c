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
 *  Copyright© 2017    D. van de Spijker
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
#include <TaskDef.h>
#include <Mm.h>

#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdbool.h>

static bool UtilTaskIsOwner(pMailbox_t mailbox, pTcb_t tcb);
Id_t MailboxIdBuffer[2];
LinkedList_t MailboxList;

OsResult_t MailboxInit(void)
{
    ListInit(&MailboxList, ID_TYPE_MAILBOX, MailboxIdBuffer, 2);

    return OS_OK;
}

Id_t MailboxCreate(U8_t mailbox_size, Id_t owner_ids[], U8_t n_owners)
{

    //Check input boundaries
    if(n_owners > MAILBOX_CONFIG_MAX_OWNERS) {
        return INVALID_ID;
    }

    void *buffer = NULL;
    pMailbox_t new_mailbox = (pMailbox_t)CoreObjectAlloc(sizeof(Mailbox_t), mailbox_size, &buffer); //Allocate memory for mailbox struct
    if(new_mailbox == NULL) {
        return INVALID_ID;    //Allocation error
    }

    new_mailbox->buffer = (MailboxBase_t *)buffer;
    new_mailbox->pend_counters = (U8_t*)CoreObjectAlloc(sizeof(U8_t) * mailbox_size, 0, NULL); //Allocate memory for mailbox data
    if(new_mailbox->pend_counters == NULL) {
        CoreObjectFree((void **)&new_mailbox, &buffer);
        return INVALID_ID;
    }

    ListNodeInit(&new_mailbox->list_node, (void*)new_mailbox);
    if(ListNodeAddSorted(&MailboxList, &new_mailbox->list_node) != OS_OK) {
        CoreObjectFree((void **)&new_mailbox->pend_counters, NULL);
        CoreObjectFree((void **)&new_mailbox, &buffer);
        return INVALID_ID;
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
    pMailbox_t mailbox = UtilMailboxFromId(*mailbox_id);
    if (mailbox != NULL) {
        ListNodeDeinit(&MailboxList, &mailbox->list_node);
        CoreObjectFree((void **)&mailbox->pend_counters, NULL);
        CoreObjectFree((void **)&mailbox, (void **)&mailbox->buffer);
        *mailbox_id = INVALID_ID;
        return OS_OK;
    }
    return OS_ERROR;
}

OsResult_t MailboxPost(Id_t mailbox_id, U8_t base_address, MailboxBase_t* data, U8_t len)
{
    if(len == 0) {
        return OS_OUT_OF_BOUNDS;    //len has to at least 1
    }
    OsResult_t result = OS_LOCKED;
    LIST_NODE_ACCESS_WRITE_BEGIN(&MailboxList, mailbox_id) {
        result = OS_OK;
        pMailbox_t mailbox = ListNodeChildGet(node);
        if(mailbox == NULL) {
            result = OS_ERROR;    //Search error
        }

        if((len > mailbox->size) || ((base_address + len) > mailbox->size)) {
            result = OS_OUT_OF_BOUNDS;    //base address and length > mailbox size
        }

        if(mailbox->pend_counters[base_address] != 0) {
            result = OS_LOCKED;
        }
        if(result == OS_OK) {
            for (U8_t i = base_address; i < (len + base_address); i++) {
                mailbox->buffer[i] = data[(i - base_address)];
                mailbox->pend_counters[i] = mailbox->n_owners;
            }


#ifdef PRTOS_CONFIG_USE_EVENT_MAILBOX_POST_PEND
            EventPublish(mailbox_id, MAILBOX_EVENT_POST(base_address), 0);
#endif
        }
    }
    LIST_NODE_ACCESS_WRITE_END();
    return result;
}

OsResult_t MailboxPend(Id_t mailbox_id,  U8_t base_address, MailboxBase_t* data, U8_t len)
{
    pTcb_t task = TcbRunning;
    if(len == 0) {
        return OS_OUT_OF_BOUNDS;    //len has to at least 1
    }

    OsResult_t result = OS_LOCKED;
    /* Lock in write because the pend counters are modified. */
    LIST_NODE_ACCESS_WRITE_BEGIN(&MailboxList, mailbox_id) {
        result = OS_OK;
        pMailbox_t mailbox = ListNodeChildGet(node);

        if(UtilTaskIsOwner(mailbox, task) == false) {
            result = OS_RESTRICTED;
        }

        if((len > mailbox->size) || ((base_address + len) > mailbox->size)) {
            result = OS_OUT_OF_BOUNDS;
        }

        if(result == OS_OK) {
            for (U8_t i = base_address; i < (len + base_address); i++) {
                data[(i-base_address)] = mailbox->buffer[i];
                if(mailbox->pend_counters[i] > 0) {
                    mailbox->pend_counters[i]--;
                } else {
                    result = OS_LOCKED;
                }
            }

#ifdef PRTOS_CONFIG_USE_EVENT_MAILBOX_POST_PEND
            if(result == OS_OK) {
                EventPublish(mailbox_id, MAILBOX_EVENT_PEND(base_address), 0);
            }
#endif
        }
    }
    LIST_NODE_ACCESS_WRITE_END();
    return result;
}

U8_t MailboxPendCounterGet(Id_t mailbox_id, U8_t address)
{
    U8_t pend_counter = 0;
    LIST_NODE_ACCESS_READ_BEGIN(&MailboxList, mailbox_id) {
        pMailbox_t mailbox = ListNodeChildGet(node);
        pend_counter = mailbox->pend_counters[address];
    }
    LIST_NODE_ACCESS_READ_END();
    return pend_counter;
}

pMailbox_t UtilMailboxFromId(Id_t mailbox_id)
{
    ListNode_t *node = ListSearch(&MailboxList, mailbox_id);
    if(node != NULL) {
        return (pMailbox_t)ListNodeChildGet(node);
    } else {
        return NULL;
    }
}

static bool UtilTaskIsOwner(pMailbox_t mailbox, pTcb_t tcb)
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