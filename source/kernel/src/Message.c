/*
 * Message.c
 *
 *  Created on: 2 jul. 2018
 *      Author: Dorus
 */

#include "Message.h"

#include "TaskDef.h"
#include "List.h"
#include "Memory.h"

#include <stdlib.h>

typedef struct {
	ListNode_t node;
	Message_t msg;
}MessageNode_t, *pMessageNode_t;

typedef struct {
	ListNode_t node;

	LinkedList_t list;
	U32_t max_size;
}MessageQueue_t, *pMessageQueue_t;

LinkedList_t MessageQueueList;


void KMessageInit(void)
{
	ListInit(&MessageQueueList, ID_GROUP_TASK);
}

OsResult_t MessageQueueCreate(U32_t max_size)
{
	OsResult_t result = OS_RES_INVALID_ARGUMENT;
	pMessageQueue_t queue = NULL;
	pTcb_t task = NULL;
	Id_t task_id = ID_INVALID;

	if(max_size > 0) {
		result = OS_RES_FAIL;

		task = KTaskRunningGet(); /* Acquire current running task. */;
		if(task != NULL) {
			task_id = ListNodeIdGet(&task->list_node);
		}
		queue = (pMessageQueue_t)ListNodeChildGet(ListSearch(&MessageQueueList, task_id)); /* Check if the task already has a queue. */
		if(queue == NULL) {
			result = OS_RES_ERROR;

			queue = KMemAllocObject(sizeof(MessageQueue_t), 0, NULL); /* Allocate memory for the Message queue. */
			if(queue != NULL && task != NULL) {
				queue->max_size = max_size;
				result = ListNodeInit(&queue->node, queue); /* Initialize the queue node. */
				if(result == OS_RES_OK) { /* Assign the same ID as the owning task. */
					result = ListNodeIdSet(&queue->node, task_id);
				}
				if(result == OS_RES_OK) { /* Add the queue to the list of queues. */
					result = ListNodeAddSorted(&MessageQueueList, &queue->node);
				}
				if(result == OS_RES_OK) { /* Initialize the newly created message queue. */
					ListInit(&queue->list, ID_GROUP_MESSAGE);
				}

				/* Free the object if one of the steps failed. */
				if(result != OS_RES_OK) {
					KMemFreeObject(&queue, NULL);
				}
			}
		}
	}

	return result;
}

OsResult_t MessageSend(Id_t task_id, Message_t *message, U32_t timeout_ms)
{
	OsResult_t result = OS_RES_INVALID_ARGUMENT;
	pMessageQueue_t queue = NULL;
	pMessageNode_t msg_node = NULL;

	if(task_id != ID_INVALID && message != NULL) {
		result = OS_RES_ERROR;

		queue = (pMessageQueue_t)ListNodeChildGet(ListSearch(&MessageQueueList, task_id));
		if(queue != NULL) {
			result = OS_RES_FAIL;

			msg_node = KMemAllocObject(sizeof(MessageNode_t), 0, NULL);
			if(queue->list.size < queue->max_size && msg_node != NULL) {

				result = ListNodeInit(&msg_node->node, msg_node);
				if(result == OS_RES_OK) {
					msg_node->msg = *message;
					result = ListNodeAddAtPosition(&queue->list, &msg_node->node, LIST_POSITION_TAIL);
				}

				/* Free the object if one of the steps failed. */
				if(result != OS_RES_OK) {
					KMemFreeObject(&msg_node, NULL);
				}
			}
		}
	}

	return result;
}

OsResult_t MessageMulticast(struct IdList_t *task_ids, Message_t *message, U32_t timeout_ms)
{
	return OS_RES_CRIT_ERROR;
}

OsResult_t MessageReceive(Message_t *message, U32_t timeout_ms)
{
	OsResult_t result = OS_RES_INVALID_ARGUMENT;
	pMessageQueue_t queue = NULL;
	pMessageNode_t msg_node = NULL;
	pTcb_t task = NULL;

	if(message != ID_INVALID) {
		result = OS_RES_ERROR;

		task = KTaskRunningGet(); /* Acquire current running task. */
		queue = (pMessageQueue_t)ListNodeChildGet(ListSearch(&MessageQueueList, ListNodeIdGet(&task->list_node)));
		if(queue != NULL) {
			msg_node = (pMessageNode_t)ListNodeChildGet(ListNodeRemoveFromHead(&queue->list));
			if(msg_node != NULL) {
				*message = msg_node->msg;
				result = KMemFreeObject(&msg_node, NULL);
			}
		}
	}

	return result;
}

