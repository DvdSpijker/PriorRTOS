/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/
 
/* Test framework */
#include "unity.h"

/* UUT */
#include "Scheduler.h"

/* Dependencies */
#include "List.h"
#include "Event.h"
#include "IdTypeDef.h"
#include "kernel/inc/MemoryDef.h"
#include "Fletcher.h"
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
 static LinkedList_t EventList;
 
/*******************************************************************************
 *    PRIVATE FUNCTIONS
 ******************************************************************************/
 
/*******************************************************************************
 *    SETUP, TEARDOWN
 ******************************************************************************/
 
void setUp(void)
{
	LogError_Ignore();

	TEST_ASSERT_EQUAL(KSchedulerInit(&EventList), OS_RES_OK);
}
 
void tearDown(void)
{
}
 
/*******************************************************************************
 *    TESTS
 ******************************************************************************/


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

void test_KSchedulerCycle_missing_mock_event(void)
{
	OsResult_t res = OS_RES_ERROR;
	LinkedList_t tcb_idle_list;
	LinkedList_t tcb_wait_list;
	LinkedList_t *tcb_lists[2] = {&tcb_idle_list, &tcb_wait_list};
	EventList.size = 0;

	res = KSchedulerCycle(tcb_lists, 2);

	TEST_ASSERT_EQUAL(res, OS_RES_CRIT_ERROR);
}


