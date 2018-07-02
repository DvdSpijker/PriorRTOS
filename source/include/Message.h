/*
 * Message.h
 *
 *  Created on: 27 jun. 2018
 *      Author: Dorus
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include "OsTypes.h"

typedef enum {
	MSG_DATA_TYPE_VALUE = 0,
	MSG_DATA_TYPE_POINTER,
}MessageDataType_t;

typedef struct {
	U8_t msg_type; /* User defined message type. */
	MessageDataType_t data_type; /* Message data type. */
	U32_t length; /* Length of the Message data. */
	union {
		void *ptr; /* Pointer message. */
		U32_t val; /* Value message. */
	}msg_data;
}Message_t;

/******************************************************************************
 * @func: OsResult_t MessageQueueCreate(U32_t max_size)
 *
 * @desc: Creates a Message Queue for the calling task. The maximum amount of
 * allowed messages is specified.
 * Messages can be send to the task's queue, a task can receive these messages
 * following FIFO semantics.
 *
 * @argin: (U32_t) max_size; Maximum amount of messages in the queue.
 *
 * @rettype:  (OsResult_t); sys call result
 * @retval:   OS_RES_OK; if the message queue was created.
 * @retval:   OS_RES_ERROR; if an error occurred.
 * @retval:   OS_RES_INVALID_ARGUMENT; if max_size is 0.
 * @retval:   OS_RES_FAIL; if the task already has a message queue.
 ******************************************************************************/
OsResult_t MessageQueueCreate(U32_t max_size);

/******************************************************************************
 * @func: OsResult_t MessageSend(Id_t task_id, Message_t *message, U32_t timeout_ms)
 *
 * @desc: Sends a message to the task's Message Queue. The message is copied before
 * adding it to the queue it.
 *
 * @argin: (Id_t) task_id; Receiving task.
 * @argin: (Message_t *) message; Pointer to the message to send.
 * @argin: (U32_t) timeout_ms; Timeout in milliseconds.
 *
 * @rettype:  (OsResult_t); sys call result
 * @retval:   OS_RES_OK; if the message queue was created.
 * @retval:   OS_RES_ERROR; if an error occurred.
 * @retval:   OS_RES_INVALID_ARGUMENT; if max_size is 0.
 * @retval:   OS_RES_FAIL; if the task already has a message queue.
 ******************************************************************************/
OsResult_t MessageSend(Id_t task_id, Message_t *message, U32_t timeout_ms);

OsResult_t MessageMulticast(struct IdList_t *task_ids, Message_t *message, U32_t timeout_ms);

OsResult_t MessageReceive(Message_t *message, U32_t timeout_ms);


#endif /* MESSAGE_H_ */
