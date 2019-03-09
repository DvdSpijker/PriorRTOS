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
 *  Copyright� 2017    D. van de Spijker
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
#include "include/Mailbox.h"
#include "kernel/inc/MailboxDef.h"

#include "kernel/inc/List.h"
#include "kernel/inc/Event.h"
#include "kernel/inc/CoreDef.h"
#include "kernel/inc/MemoryDef.h"
#include "kernel/inc/TaskDef.h"
#include "kernel/inc/SystemCall.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h> /* For memcpy. */

static bool ITaskIsOwner(pMailbox_t mailbox, Id_t task);
static OsResult_t IMailboxWrite(Id_t mailbox_id, U8_t address, void *data, U32_t timeout, bool force);

LinkedList_t MailboxList;

OsResult_t KMailboxInit(void)
{
    ListInit(&MailboxList, ID_GROUP_MAILBOX);

    return OS_RES_OK;
}

Id_t MailboxCreate(U8_t mailbox_size, U8_t mailbox_width, IdList_t *owners)
{
	if(mailbox_size == 0 || mailbox_width == 0 || owners == NULL) {
		return ID_INVALID;
	}

    void *buffer = NULL;

    /* Allocate memory for mailbox struct and buffer to store data.
     * The buffer size is equal to the mailbox size * width. */
    pMailbox_t new_mailbox = (pMailbox_t)KMemAllocObject(sizeof(Mailbox_t),
    		mailbox_size * mailbox_width, &buffer);
    if(new_mailbox == NULL) {
        return ID_INVALID;
    }

    new_mailbox->buffer = (U8_t *)buffer; /* Copy the buffer pointer. */

    /* Allocate memory for pend counters */
    new_mailbox->pend_counters = (U8_t*)KMemAllocObject(sizeof(U8_t) * mailbox_size, 0, NULL);
    if(new_mailbox->pend_counters == NULL) {
    	/* Free the mailbox struct in case of failed allocation. */
        KMemFreeObject((void **)&new_mailbox, &buffer);
        return ID_INVALID;
    }

    /* When all allocations were successful, initialize the list node and add the
     * new mailbox to the mailbox list. If this fails, free all allocated data. */
    ListNodeInit(&new_mailbox->list_node, (void*)new_mailbox);
    if(ListNodeAddSorted(&MailboxList, &new_mailbox->list_node) != OS_RES_OK) {
        KMemFreeObject((void **)&new_mailbox->pend_counters, NULL);
        KMemFreeObject((void **)&new_mailbox, &buffer);
        return ID_INVALID;
    }

    new_mailbox->size = mailbox_size;
    new_mailbox->width = mailbox_width;
    IdListCopy(&new_mailbox->owners, owners); /* Copy the list of owners. */

    return ListNodeIdGet(&new_mailbox->list_node);
}

OsResult_t MailboxDelete(Id_t *mailbox_id)
{
    pMailbox_t mailbox = KMailboxFromId(*mailbox_id);
    if (mailbox != NULL) {
        ListNodeDeinit(&MailboxList, &mailbox->list_node);
        KMemFreeObject((void **)&mailbox->pend_counters, NULL);
        KMemFreeObject((void **)&mailbox, (void **)&mailbox->buffer);
        *mailbox_id = ID_INVALID;
        return OS_RES_OK;
    }
    return OS_RES_ERROR;
}

OsResult_t MailboxPost(Id_t mailbox_id, U8_t address, void *data, U32_t timeout)
{
	return IMailboxWrite(mailbox_id, address, data, timeout, false);

}

OsResult_t MailboxUpdate(Id_t mailbox_id, U8_t address, void *data, U32_t timeout)
{
	return IMailboxWrite(mailbox_id, address, data, timeout, true);
}

OsResult_t MailboxPend(Id_t mailbox_id, U8_t address, void *data, U32_t timeout)
{

    OsResult_t result = OS_RES_LOCKED;

#ifdef PRTOS_CONFIG_USE_MAILBOX_EVENT_POST_PEND
    SYS_CALL_EVENT_HANDLE(mailbox_id, MAILBOX_EVENT_PEND(address), &result) {
        /* Do nothing. Normal execution flow. */
    }
    SYS_CALL_EVENT_HANDLE_TIMEOUT(&result) {
        return result;
    }
    SYS_CALL_EVENT_HANDLE_POLL(&result) {
        return result;
    }
#endif

    /* Lock in write because the pend counters are modified. */
    LIST_NODE_ACCESS_WRITE_BEGIN(&MailboxList, mailbox_id) {
        result = OS_RES_OK;
        pMailbox_t mailbox = ListNodeChildGet(node);

        /* Validate ownership of this mailbox. */
        if(ITaskIsOwner(mailbox, KCoreTaskRunningGet()) == false) {
            result = OS_RES_RESTRICTED;
        }

        if(result == OS_RES_OK) {

            /* Validate address range. */
            if(address > (mailbox->size - 1)) {
                result = OS_RES_INVALID_ARGUMENT;
            }

            if(result == OS_RES_OK) {

                /* Check pend counter, if this is 0 there is nothing to pend and we can
                 * wait/poll for a post event. */
                if(mailbox->pend_counters[address] == 0) {
#ifdef PRTOS_CONFIG_USE_MAILBOX_EVENT_POST_PEND
                	if(timeout != OS_TIMEOUT_NONE) {
                    SYS_CALL_EVENT_REGISTER(node, mailbox_id, MAILBOX_EVENT_POST(address), &result, timeout);
                	} else {
                		result = OS_RES_LOCKED;
                	}
#else
                    result = OS_RES_LOCKED;
#endif
                }

                /* If the pend counter != 0 the address can be accessed.
                 * After pending a pend event is emitted. */
                if(result == OS_RES_OK) {
                	address = address * mailbox->width; /* Calculate the offset in the buffer using the mailbox width. */
                	memcpy(data, &mailbox->buffer[address], mailbox->width);
                    mailbox->pend_counters[address]--;
#ifdef PRTOS_CONFIG_USE_MAILBOX_EVENT_POST_PEND
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

static bool ITaskIsOwner(pMailbox_t mailbox, Id_t task)
{
    bool is_owner = false;
    for (U8_t i = 0; i < mailbox->owners.n; i++) {
        if(mailbox->owners.ids[i] == task) {
            is_owner = true;
            break;
        }
    }

    return is_owner;
}

static OsResult_t IMailboxWrite(Id_t mailbox_id, U8_t address, void *data, U32_t timeout, bool force)
{
	OsResult_t result = OS_RES_LOCKED;
	 pMailbox_t mailbox = NULL;

	#ifdef PRTOS_CONFIG_USE_MAILBOX_EVENT_POST_PEND
	SYS_CALL_EVENT_HANDLE(mailbox_id, MAILBOX_EVENT_PEND(address), &result) {
	    /* Do nothing. Normal execution flow. */
	}
	SYS_CALL_EVENT_HANDLE_TIMEOUT(&result) {
	    return result;
	}
	SYS_CALL_EVENT_HANDLE_POLL(&result) {
	    return result;
	}
	#endif

	LIST_NODE_ACCESS_WRITE_BEGIN(&MailboxList, mailbox_id) {
	    result = OS_RES_OK;
	    mailbox = ListNodeChildGet(node);
	    if(mailbox == NULL) {
	        result = OS_RES_ERROR;    //Search error
	    }

	    if(result == OS_RES_OK) {

	        /* Validate address range. */
	        if(address > (mailbox->size - 1)) {
	            result = OS_RES_INVALID_ARGUMENT;
	        }

	        /* Check pend counter, if this is !0 then posting at this address is not allowed.
	         * wait/poll for a pend event. */
	        if(result == OS_RES_OK) {
	            if(mailbox->pend_counters[address] != 0 && force == false) {
	#ifdef PRTOS_CONFIG_USE_MAILBOX_EVENT_POST_PEND
	            	if(timeout != OS_TIMEOUT_NONE) {
	            		SYS_CALL_EVENT_REGISTER(node, mailbox_id, MAILBOX_EVENT_PEND(address), &result, timeout);
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
	            	address = address * mailbox->width; /* Calculate the offset in the buffer using the mailbox width. */
	            	memcpy(&mailbox->buffer[address], data, mailbox->width);
	                mailbox->pend_counters[address] = IdListCount(&mailbox->owners);
	#ifdef PRTOS_CONFIG_USE_MAILBOX_EVENT_POST_PEND
	                EventEmit(mailbox_id, MAILBOX_EVENT_POST(address), EVENT_FLAG_NONE);
	                EventEmit(mailbox_id, MAILBOX_EVENT_POST_ANY, EVENT_FLAG_NONE);
	#endif
	            }
	        }
	    }
	}
	LIST_NODE_ACCESS_END();

	return result;
}

