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
	union {
		void *ptr; /* Pointer message. */
		U32_t val; /* Value message. */
	}msg_data;
}Message_t;

/******************************************************************************
 * @func: Id_t MessageQueueCreate(U32_t max_size)
 *
 * @desc: Creates a Message Queue for the calling task. The maximum amount of
 * allowed messages is specified.
 * Messages can be send to the task's queue, the task can receive these messages
 * following FIFO semantics.
 *
 * @argin: (U32_t) max_size; Maximum amount of messages in the queue.
 *
 * @rettype:  (Id_t); message queue ID
 * @retval:   ID_INVALID; if the queue could not be created.
 * @retval:   Other; valid ID.
 ******************************************************************************/
Id_t MessageQueueCreate(U32_t max_size);

/******************************************************************************
 * @func: OsResult_t MessageSend(Id_t msg_queue_id, Message_t *message, U32_t timeout_ms)
 *
 * @desc: Sends a message to the Message Queue. The message is copied before
 * adding it to the queue.
 *
 * @argin: (Id_t) msg_queue_id; Message queue to send to.
 * @argin: (Message_t *) message; Pointer to the message to send.
 * @argin: (U32_t) timeout_ms; Timeout in milliseconds.
 *
 * @rettype:  (OsResult_t); sys call result
 * @retval:   OS_RES_OK; if the message was sent.
 * @retval:   OS_RES_ERROR; if an error occurred.
 * @retval:   OS_RES_INVALID_ARGUMENT; if the ID or the message is invalid.
 * @retval:   OS_RES_FAIL; if the message queue has reached its maximum capacity.
 ******************************************************************************/
OsResult_t MessageSend(Id_t msg_queue_id, Message_t *message, U32_t timeout_ms);

/******************************************************************************
 * @func: OsResult_t MessageMulticast(IdList_t *msg_queue_ids, Message_t *message, U32_t timeout_ms)
 *
 * @desc: Sends a message to multiple queues. The message is copied before
 * adding it to the queue.
 *
 * @argin: (IdList_t *) msg_queue_ids; List of message queues to send to.
 * @argin: (Message_t *) message; Pointer to the message to send.
 * @argin: (U32_t) timeout_ms; Timeout in milliseconds.
 *
 * @rettype:  (OsResult_t); sys call result
 * @retval:   OS_RES_OK; if the message was sent.
 * @retval:   OS_RES_ERROR; if an error occurred.
 * @retval:   OS_RES_INVALID_ARGUMENT; if the ID or the message is invalid.
 * @retval:   OS_RES_FAIL; if the message queue has reached its maximum capacity.
 ******************************************************************************/
OsResult_t MessageMulticast(IdList_t *msg_queue_ids, Message_t *message, U32_t timeout_ms);

/******************************************************************************
 * @func: OsResult_t MessageReceive(Id_t msg_queue_id, Message_t *message, U32_t timeout_ms)
 *
 * @desc: Receive a message from a Message Queue. Only the owner task (the task that created
 * the queue) may receive its messages.
 *
 * @argin: (Id_t) msg_queue_id; Message queue to receive from.
 * @argin: (Message_t *) message; Pointer to a message object. The received message
 * is copied here.
 * @argin: (U32_t) timeout_ms; Timeout in milliseconds.
 *
 * @rettype:  (OsResult_t); sys call result
 * @retval:   OS_RES_OK; if a message was received.
 * @retval:   OS_RES_ERROR; if an error occurred.
 * @retval:   OS_RES_INVALID_ARGUMENT; if the ID or the message is invalid.
 * @retval:   OS_RES_RESTRICTED; if the calling task is not the owner of the queue.
 * @retval:   OS_RES_FAIL; if the message queue contains no messages.
 ******************************************************************************/
OsResult_t MessageReceive(Id_t msg_queue_id, Message_t *message, U32_t timeout_ms);


#endif /* MESSAGE_H_ */
