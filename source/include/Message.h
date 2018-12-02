/*
 * Message.h
 *
 *  Created on: 27 jun. 2018
 *      Author: Dorus
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include "PriorRTOSConfig.h"
#include "OsTypes.h"
#include "EventDef.h"

/* Message Queue Events */
#define MESSAGE_EVENT_CREATE	EVENT_TYPE_CREATE
#define MESSAGE_EVENT_DELETE	EVENT_TYPE_DELETE

#ifdef PRTOS_CONFIG_USE_MESSAGE_EVENT_SEND
#define MESSAGE_EVENT_SEND		EVENT_TYPE_ACCESS | 0x00000001
#endif

#ifdef PRTOS_CONFIG_USE_MESSAGE_EVENT_RECEIVE
#define MESSAGE_EVENT_RECEIVE	EVENT_TYPE_ACCESS | 0x00000002
#endif

typedef enum {
	MSG_DATA_TYPE_INVALID = -1,

	/* Value message. */
	MSG_DATA_TYPE_U8 = 0,
	MSG_DATA_TYPE_U16,
	MSG_DATA_TYPE_U32,
	MSG_DATA_TYPE_U64,
	MSG_DATA_TYPE_S8,
	MSG_DATA_TYPE_S16,
	MSG_DATA_TYPE_S32,
	MSG_DATA_TYPE_S64,
	MSG_DATA_TYPE_FLOAT,
	MSG_DATA_TYPE_DOUBLE,

	/* Pointer message. */
	MSG_DATA_TYPE_ARRAY,
	MSG_DATA_TYPE_STRING,
}MessageDataType_t;

typedef struct {
	void *p;	/* Pointer to the array/string. */
	U32_t size;	/* Array/string size. */
}PointerMessage_t;

typedef struct {
	MessageDataType_t type; /* Message type. */
	union {
		PointerMessage_t pointer; /* Pointer message. */
		U64_t value; /* Value message. */
	}msg_data;
}Message_t;

/******************************************************************************
 * @func: Id_t MessageQueueCreate(U32_t max_size)
 *
 * @desc: Creates a Message Queue, it is owned by the specified task.
 * The maximum amount of allowed messages is specified.
 * Messages can be sent to the task's queue, the task can receive these messages
 * following FIFO semantics.
 *
 * @argin: (Id_t) task_id; Owner task ID.
 * @argin: (U32_t) max_size; Maximum amount of messages in the queue.
 *
 * @rettype:  (Id_t); message queue ID
 * @retval:   ID_INVALID; if the queue could not be created.
 * @retval:   Other; valid ID.
 ******************************************************************************/
Id_t MessageQueueCreate(Id_t task_id, U32_t max_size);

/******************************************************************************
 * @func: OsResult_t MessageSend(Id_t msg_queue_id, Message_t *message, U32_t timeout_ms)
 *
 * @desc: Sends a message to the Message Queue. The message is copied.
 *
 * @argin: (Id_t) msg_queue_id; Message queue to send to.
 * @argin: (Message_t *) message; Pointer to the message to send.
 * @argin: (U32_t) timeout_ms; Timeout in milliseconds.
 *
 * @rettype:  (OsResult_t); sys call result
 * @retval:   OS_RES_OK; if the message was sent.
 * @retval:   OS_RES_POLL; if the message queue is full, the calling task is
 * polling for a MESSAGE_EVENT_RECEIVE.
 * @retval:   OS_RES_ERROR; if an error occurred.
 * @retval:   OS_RES_INVALID_ARGUMENT; if the ID or the message is invalid.
 * @retval:   OS_RES_FAIL; if the message queue has reached its maximum capacity.
 ******************************************************************************/
OsResult_t MessageSend(Id_t msg_queue_id, Message_t *message, U32_t timeout_ms);

/******************************************************************************
 * @func: OsResult_t MessageMulticast(IdList_t *msg_queue_ids, Message_t *message, U32_t timeout_ms)
 *
 * @desc: Sends a message to multiple queues. The message is copied.
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
 * @desc: Receive a message from a Message Queue. Only the owner task may receive its messages.
 *
 * @argin: (Id_t) msg_queue_id; Message queue to receive from.
 * @argin: (Message_t *) message; Pointer to a message object. The received message
 * is copied here.
 * @argin: (U32_t) timeout_ms; Timeout in milliseconds.
 *
 * @rettype:  (OsResult_t); sys call result
 * @retval:   OS_RES_OK; if a message was received.
 * @retval:   OS_RES_POLL; if the message queue is empty, the calling task is
 * polling for a MESSAGE_EVENT_SEND.
 * @retval:   OS_RES_ERROR; if an error occurred.
 * @retval:   OS_RES_INVALID_ARGUMENT; if the ID or the message is invalid.
 * @retval:   OS_RES_RESTRICTED; if the calling task is not the owner of the queue.
 * @retval:   OS_RES_FAIL; if the message queue contains no messages.
 ******************************************************************************/
OsResult_t MessageReceive(Id_t msg_queue_id, Message_t *message, U32_t timeout_ms);


#endif /* MESSAGE_H_ */
