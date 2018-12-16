
#include "MessageTest.h"

#include "PriorRTOS.h"

#include <string.h>
#include <stdbool.h>

LOG_FILE_NAME("MessageTest");

static void MessageTestTask(const void *p_arg, U32_t v_arg);

OsResult_t MessageTestInit(void)
{
	OsResult_t res = OS_RES_OK;
	Id_t tsk_message_test;
	Id_t msq_test;

	tsk_message_test = TaskCreate(MessageTestTask, TASK_CAT_HIGH, 5, TASK_PARAMETER_NONE, 0, NULL, 0);

	if(tsk_message_test == ID_INVALID) {
		res = OS_RES_ERROR;
	}
	if(res == OS_RES_OK) {
		msq_test = MessageQueueCreate(tsk_message_test, 3);
		if(msq_test == ID_INVALID) {
			res = OS_RES_ERROR;
		}
	}
	TaskNotify(tsk_message_test, msq_test);

	return res;
}

static void MessageTestTask(const void *p_arg, U32_t v_arg)
{
	OS_ARG_UNUSED(p_arg);
	Id_t msq_test = ID_INVALID;
	Message_t test_msg;

	const U32_t array_sz = 4;
	const U8_t array[] = {0xDE, 0xAD, 0xBE, 0xEF};
	const U32_t value = 0xDEADBEEF;

	TASK_INIT_BEGIN() {
		msq_test = v_arg;
	} TASK_INIT_END();

	test_msg.msg_data.value = value;
	test_msg.type = MSG_DATA_TYPE_U32;

	MessageSend(msq_test, &test_msg, OS_TIMEOUT_NONE);
	memset(&test_msg, 0, sizeof(test_msg));

	if(MessageReceive(msq_test, &test_msg, OS_TIMEOUT_NONE) == OS_RES_OK) {
		LOG_DEBUG_NEWLINE("Received message.");
		LOG_DEBUG_NEWLINE("Message type: %d | Message value(s): 0x%08X", test_msg.type, (U32_t)test_msg.msg_data.value);
	} else {
		LOG_ERROR_NEWLINE("No message in queue.");
	}

	memset(&test_msg, 0, sizeof(test_msg));

	test_msg.msg_data.pointer.p = (void *)array;
	test_msg.msg_data.pointer.size = array_sz;

	MessageSend(msq_test, &test_msg, OS_TIMEOUT_NONE);
	memset(&test_msg, 0, sizeof(test_msg));

	if(MessageReceive(msq_test, &test_msg, OS_TIMEOUT_NONE) == OS_RES_OK) {
		LOG_DEBUG_NEWLINE("Received message.");
		LOG_DEBUG_NEWLINE("Message type: %d | Message value(s):", test_msg.type);
		for(U32_t i = 0; i < test_msg.msg_data.pointer.size; i++) {
			LOG_DEBUG_APPEND(" 0x%02X |", ((U8_t *)test_msg.msg_data.pointer.p)[i]);
		}
	} else {
		LOG_ERROR_NEWLINE("No message in queue.");
	}

	TaskDelete(NULL);
}

