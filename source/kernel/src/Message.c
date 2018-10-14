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

Id_t MessageQueueCreate(U32_t max_size)
{
	OsResult_t result = OS_RES_INVALID_ARGUMENT;
	pMessageQueue_t queue = NULL;

	if(max_size > 0) {
		result = OS_RES_ERROR;
		queue = KMemAllocObject(sizeof(MessageQueue_t), 0, NULL); /* Allocate memory for the Message queue. */
		if(queue != NULL) {
			queue->max_size = max_size;
			queue->task_id = TaskIdGet();
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

OsResult_t MessageSend(Id_t msg_queue_id, Message_t *message, U32_t timeout_ms)
{
	OsResult_t result = OS_RES_INVALID_ARGUMENT;
	pMessageQueue_t queue = NULL;
	pMessageNode_t msg_node = NULL;

	if(msg_queue_id != ID_INVALID && message != NULL) {
		result = OS_RES_ERROR;

		queue = (pMessageQueue_t)ListNodeChildGet(ListSearch(&MessageQueueList, msg_queue_id));
		if(queue != NULL) {
			result = OS_RES_FAIL;
			if(queue->list.size < queue->max_size) {
				result = OS_RES_ERROR;

				msg_node = KMemAllocObject(sizeof(MessageNode_t), 0, NULL);
				if(msg_node != NULL) {
					result = ListNodeInit(&msg_node->node, msg_node);
					if(result == OS_RES_OK) {
						msg_node->msg = *message;
						ListNodeIdSet(&msg_node->node, msg_queue_id); /* Assign same ID as the message queue. This ID is never used. */
						result = ListNodeAddAtPosition(&queue->list, &msg_node->node, LIST_POSITION_TAIL);
					}

					/* Deinit the node and free the object if one of the steps failed. */
					if(result != OS_RES_OK) {
						ListNodeDeinit(&queue->list, &msg_node->node);
						KMemFreeObject((void **)&msg_node, NULL);
					}
				}
			}
		}
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
	} while(res == OS_RES_OK);

	return res;
}

//#ifdef PRTOS_CONFIG_USE_EVENTGROUP_EVENT_FLAG_SET
//	LIST_NODE_ACCESS_READ_BEGIN(&EventGroupList, eventgroup_id) {
//		U8_t flags = EventgroupFlagsGet(eventgroup_id, mask);
//		if(!flags) { /* Not all flags have been set. */
//			SYS_CALL_EVENT_REGISTER(node, eventgroup_id, EVENTGROUP_EVENT_FLAG_SET(mask), &result, timeout);
//		} else {
//			result = OS_RES_OK;
//		}
//	}
//	LIST_NODE_ACCESS_END();
//#endif


OsResult_t MessageReceive(Id_t msg_queue_id, Message_t *message, U32_t timeout_ms)
{
//	OsResult_t result = OS_RES_INVALID_ARGUMENT;
//	pMessageQueue_t queue = NULL;
//	pMessageNode_t msg_node = NULL;
//
//	SYS_CALL_EVENT_HANDLE(eventgroup_id, EVENTGROUP_EVENT_FLAG_SET(mask), &result) {
//		/* Do nothing. */
//	}
//	SYS_CALL_EVENT_HANDLE_TIMEOUT(&result) {
//		return result;
//	}
//	SYS_CALL_EVENT_HANDLE_POLL(&result) {
//		return result;
//	}
//
//	if(msg_queue_id != ID_INVALID && message != NULL) {
//		result = OS_RES_ERROR;
//
//		LIST_NODE_ACCESS_READ_BEGIN(&MessageQueueList, msg_queue_id) {
//			queue = (pMessageQueue_t)ListNodeChildGet(node);
//			if(queue != NULL) {
//				result = OS_RES_RESTRICTED;
//
//				if(queue->task_id == TaskIdGet()) { /* Check if the calling task is also the owner. */
//					result = OS_RES_FAIL;
//
//					msg_node = (pMessageNode_t)ListNodeChildGet(ListNodeRemoveFromHead(&queue->list));
//					if(msg_node != NULL) {
//						*message = msg_node->msg;
//						result = KMemFreeObject((void **)&msg_node, NULL);
//					}
//				}
//			}
//		}
//
//	}
//
//	return result;
}

