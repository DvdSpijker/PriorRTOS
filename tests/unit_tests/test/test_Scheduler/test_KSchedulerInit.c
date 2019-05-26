/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/
 
/* Test framework */
#include "unity.h"

/* UUT */
#include "Scheduler.h"

/* Dependencies */
#include "mock_Event.h"
#include "mock_List.h"
#include "mock_LoggerDef.h"
#include "mock_Logger.h"
#include "mock_TaskDef.h"
#include "mock_Os.h"

/* Other */

 
/*******************************************************************************
 *    DEFINITIONS
 ******************************************************************************/

/*******************************************************************************
 *    PRIVATE TYPES
 ******************************************************************************/
 
/*******************************************************************************
 *    PRIVATE DATA
 ******************************************************************************/
 
/*******************************************************************************
 *    PRIVATE FUNCTIONS
 ******************************************************************************/
 
/*******************************************************************************
 *    SETUP, TEARDOWN
 ******************************************************************************/
 
void setUp(void)
{
}
 
void tearDown(void)
{
}
 
/*******************************************************************************
 *    TESTS
 ******************************************************************************/

void test_KSchedulerInit_invalid_event_list(void)
{
	OsResult_t res = OS_RES_ERROR;

	res = KSchedulerInit(NULL);

	TEST_ASSERT_EQUAL(res, OS_RES_INVALID_ARGUMENT);
}

void test_KSchedulerInit_list_init_error(void)
{
	OsResult_t res = OS_RES_ERROR;
	LinkedList_t event_list;

	ListInit_IgnoreAndReturn(OS_RES_ERROR);

	res = KSchedulerInit(&event_list);

	TEST_ASSERT_EQUAL(res, OS_RES_ERROR);
}

void test_KSchedulerInit_event_emit_error(void)
{
	OsResult_t res = OS_RES_ERROR;
	LinkedList_t event_list;

	ListInit_IgnoreAndReturn(OS_RES_OK);
	EventEmit_ExpectAndReturn(ID_INVALID, MOCK_EVENT, EVENT_FLAG_NONE, OS_RES_ERROR);
	LogError_Ignore();

	res = KSchedulerInit(&event_list);

	TEST_ASSERT_EQUAL(res, OS_RES_ERROR);
}

void test_KSchedulerInit_successful(void)
{
	OsResult_t res = OS_RES_ERROR;
	LinkedList_t event_list;

	ListInit_IgnoreAndReturn(OS_RES_OK);
	EventEmit_ExpectAndReturn(ID_INVALID, MOCK_EVENT, EVENT_FLAG_NONE, OS_RES_OK);
	LogError_Ignore();

	res = KSchedulerInit(&event_list);

	TEST_ASSERT_EQUAL(res, OS_RES_OK);
}

void test_KSchedulerCycle_inv_arg_tcb_lists(void)
{
	OsResult_t res = OS_RES_ERROR;

	res = KSchedulerCycle(NULL, 2);

	TEST_ASSERT_EQUAL(res, OS_RES_INVALID_ARGUMENT);
}

void test_KSchedulerCycle_inv_arg_num_lists(void)
{
	OsResult_t res = OS_RES_ERROR;
	LinkedList_t tcb_idle_list;
	LinkedList_t tcb_wait_list;
	LinkedList_t *tcb_lists[2] = {&tcb_idle_list, &tcb_wait_list};

	res = KSchedulerCycle(tcb_lists, 0);

	TEST_ASSERT_EQUAL(res, OS_RES_INVALID_ARGUMENT);
}
