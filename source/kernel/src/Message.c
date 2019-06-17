/*
 * Message.c
 *
 *  Created on: 2 jul. 2018
 *      Author: Dorus
 */

#include "include/Message.h"

#include "kernel/inc/TaskDef.h"
#include "kernel/inc/List.h"
#include "kernel/inc/MemoryDef.h"
#include "kernel/inc/IdTypeDef.h"
#include "kernel/inc/Event.h"
#include "kernel/inc/SystemCall.h"

#include <stdlib.h>

typedef struct {
	ListNode_t node;
	Message_t msg;
}MessageNode_t, *pMessageNode_t;

typedef struct {
	ListNode_t node;

	LinkedList_t list;
	U32_t max_size;
	Id_t task_id; /* Owner task. */
}MessageQueue_t, *pMessageQueue_t;

LinkedList_t MessageQueueList;


void KMessageInit(void)
{
	ListInit(&MessageQueueList, ID_GROUP_MESSAGE_QUEUE);
}

Id_t MessageQueueCreate(Id_t task_id, U32_t max_size)
{
	OsResult_t result = OS_RES_INVALID_ARGUMENT;
	pMessageQueue_t queue = NULL;

	if(max_size > 0) {
		result = OS_RES_ERROR;
		queue = KMemAllocObject(sizeof(MessageQueue_t), 0, NULL); /* Allocate memory for the Message queue. */
		if(queue != NULL) {
			queue->max_size = max_size;
			queue->task_id = task_id;
			result = ListNodeInit(&queue->node, queue); /* Initialize the queue node. */
			if(result == OS_RES_OK) { /* Add the queue to the list of queues. */
				result = ListNodeAddSorted(&MessageQueueList, &queue->node);
			}
			if(result == OS_RES_OK) { /* Initialize the newly created message queue. */
				ListInit(&queue->list, ID_GROUP_MESSAGE_QUEUE);
			}

			/* Free the object if one of the steps failed. */
			if(result != OS_RES_OK) {
				KMemFreeObject((void **)&queue, NULL);
			}
		}
	}

	return result;
}

U32_t MessageQueueSizeGet(Id_t msg_queue_id)
{
	U32_t size = 0;
	pMessageQueue_t queue = NULL;

	LIST_NODE_ACCESS_WRITE_BEGIN(&MessageQueueList, msg_queue_id) {
		queue = (pMessageQueue_t)ListNodeChildGet(node);
		if(queue != NULL) {
			size = ListSizeGet(&queue->list);
		}
	} LIST_NODE_ACCESS_END();

	return size;
}

OsResult_t MessageSend(Id_t msg_queue_id, Message_t *message, U32_t timeout_ms)
{
	OsResult_t result = OS_RES_INVALID_ARGUMENT;
	pMessageQueue_t queue = NULL;
	pMessageNode_t msg_node = NULL;
	void *data_cpy = NULL;

#ifdef PRTOS_CONFIG_USE_MESSAGE_EVENT_RECEIVE
	/* Handle the Receive event. */
	SYS_CALL_EVENT_HANDLE(msg_queue_id, MESSAGE_EVENT_RECEIVE, &result) {
		/* Do nothing. */
	}
	SYS_CALL_EVENT_HANDLE_TIMEOUT(&result) {
		return result;
	}
	SYS_CALL_EVENT_HANDLE_POLL(&result) {
		return result;
	}
#endif

	if(msg_queue_id != ID_INVALID && message != NULL) {
		result = OS_RES_ERROR;

		LIST_NODE_ACCESS_WRITE_BEGIN(&MessageQueueList, msg_queue_id) {
			queue = (pMessageQueue_t)ListNodeChildGet(node);
			if(queue != NULL) {
				result = OS_RES_FAIL;
				/* Check if the queue has space for a message. */
				if(queue->list.size < queue->max_size) {
					result = OS_RES_ERROR;

					/* Allocate memory for the message node, this message node contains a
					 * generic list node and storage for the message itself. */
					msg_node = KMemAllocObject(sizeof(MessageNode_t), 0, NULL);
					if(msg_node != NULL) {
						result = ListNodeInit(&msg_node->node, msg_node);
						if(result == OS_RES_OK) {
							/* Copy the message. */
							msg_node->msg = *message;
							/* Assign same ID as the message queue. This ID is never used. */
							ListNodeIdSet(&msg_node->node, msg_queue_id);
							/* Add the node to the tail of the queue to make the queue FIFO. */
							result = ListNodeAddAtPosition(&queue->list, &msg_node->node, LIST_POSITION_TAIL);
						}

						/* Deinit the node and free the object if one of the steps failed. */
						if(result != OS_RES_OK) {
							ListNodeDeinit(&queue->list, &msg_node->node);
							KMemFreeObject((void **)&msg_node, NULL);
						} else {
							/* Check if the message contains pointer data and
							 * check if this data must be copied. */
							if(message->type >= MSG_DATE_TYPE_POINTER && message->msg_data.pointer.copy) {
								/* Allocate the amount of space needed to store the data. */
								data_cpy = KMemAllocObject(message->msg_data.pointer.size, 0, NULL);
								/* Copy the data and re-assign the pointer to the allocated piece of memory. */
								if(data_cpy != NULL) {
									memcpy(data_cpy, message->msg_data.pointer.p, message->msg_data.pointer.size);
									message->msg_data.pointer.p = data_cpy;
								} else {
									ListNodeDeinit(&queue->list, &msg_node->node);
									KMemFreeObject((void **)&msg_node, NULL);
									result = OS_RES_MEMORY_ERROR;
								}
							}

							/* Emit the Send event. */
#ifdef PRTOS_CONFIG_USE_MESSAGE_EVENT_SEND
							EventEmit(msg_queue_id, MESSAGE_EVENT_SEND, EVENT_FLAG_NONE);
#endif
						}
					} else {
						result = OS_RES_MEMORY_ERROR;
					}
				}
#ifdef PRTOS_CONFIG_USE_MESSAGE_EVENT_RECEIVE
				/* Register to the Receive event. */
				else {
					SYS_CALL_EVENT_REGISTER(node, msg_queue_id, MESSAGE_EVENT_RECEIVE, &result, timeout_ms);
				}
#endif
			}
		} LIST_NODE_ACCESS_END();

	}

	return result;
}

OsResult_t MessageMulticast(IdList_t *msg_queue_ids, Message_t *message, U32_t timeout_ms)
{
	OsResult_t res = OS_RES_FAIL;
	Id_t msg_queue_id;

	do {
		msg_queue_id = IdListIdRemove(msg_queue_ids);
		if(msg_queue_id == ID_INVALID) {
			break;
		}
		res = MessageSend(msg_queue_id, message, timeout_ms);
	} while(res == OS_RES_OK || res == OS_RES_POLL || res == OS_RES_EVENT);

	return res;
}

OsResult_t MessageReceive(Id_t msg_queue_id, Message_t *message, U32_t timeout_ms)
{
	OsResult_t result = OS_RES_INVALID_ARGUMENT;
	pMessageQueue_t queue = NULL;
	pMessageNode_t msg_node = NULL;

#ifdef PRTOS_CONFIG_USE_MESSAGE_EVENT_SEND
	/* Handle the Receive event. */
	SYS_CALL_EVENT_HANDLE(msg_queue_id, MESSAGE_EVENT_SEND, &result) {
		/* Do nothing. */
	}
	SYS_CALL_EVENT_HANDLE_TIMEOUT(&result) {
		return result;
	}
	SYS_CALL_EVENT_HANDLE_POLL(&result) {
		return result;
	}
#endif
	if(msg_queue_id != ID_INVALID && message != NULL) {
		result = OS_RES_ERROR;

		LIST_NODE_ACCESS_WRITE_BEGIN(&MessageQueueList, msg_queue_id) {
			queue = (pMessageQueue_t)ListNodeChildGet(node);
			if(queue != NULL) {
				result = OS_RES_RESTRICTED;

				if(queue->task_id == TaskIdGet()) { /* Check if the calling task is also the owner. */
					result = OS_RES_FAIL;

					/* Check if the queue contains a message. */
					if(queue->list.size > 0) {
						result = OS_RES_ERROR;

						/* Remove a message from the head of the queue. */
						msg_node = (pMessageNode_t)ListNodeChildGet(ListNodeRemoveFromHead(&queue->list));
						if(msg_node != NULL) {
							/* Copy the message and free the message node object. */
							*message = msg_node->msg;
							result = KMemFreeObject((void **)&msg_node, NULL);
#ifdef PRTOS_CONFIG_USE_MESSAGE_EVENT_RECEIVE
							/* Emit the Receive event. */
							EventEmit(msg_queue_id, MESSAGE_EVENT_RECEIVE, EVENT_FLAG_NONE);
#endif
						}
					}
#ifdef PRTOS_CONFIG_USE_MESSAGE_EVENT_SEND
					/* Register to the Send event. */
					else {
						SYS_CALL_EVENT_REGISTER(node, msg_queue_id, MESSAGE_EVENT_SEND, &result, timeout_ms);
					}
#endif
				}
			}
		} LIST_NODE_ACCESS_END();
	}

	return result;
}

OsResult_t MessageDelete(Message_t *message)
{
	if(message == NULL) {
		return OS_RES_INVALID_ARGUMENT;
	}

	if(message->msg_data.pointer.copy == true) {
		return KMemFreeObject(&message->msg_data.pointer.p, NULL);
	} else {
		return OS_RES_INVALID_ARGUMENT;
	}
}

