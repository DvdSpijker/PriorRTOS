/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/

/* Test framework */
#include "unity.h"
 
/* UUT */
#include "List.h"

/* Dependencies */
#include "IdType.h"
#include "IdTypeDef.h"

/* Other */
#include <stdio.h>
 
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

void test_ListUnlock_not_locked(void)
{
	LinkedList_t list;
	OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);
	
	res = ListUnlock(&list);
	TEST_ASSERT_EQUAL(res, OS_RES_ERROR);
}

void test_ListUnlock_once(void)
{
	LinkedList_t list;
	OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);
	
	ListLock(&list, LIST_LOCK_MODE_WRITE);
	
	res = ListUnlock(&list);
	TEST_ASSERT_EQUAL(res, OS_RES_OK);
}

void test_ListUnlock_max(void)
{
	LinkedList_t list;
	OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);
	
	for(int i = 0; i < LIST_LOCK_READ_COUNT_MAX; i++) {
		ListLock(&list, LIST_LOCK_MODE_READ);
	}
	
	for(int i = 0; i < LIST_LOCK_READ_COUNT_MAX; i++) {
		res = ListUnlock(&list);
		TEST_ASSERT_EQUAL(res, OS_RES_OK);	
	}
}

void test_ListUnlock_lock_after_unlock(void)
{
	LinkedList_t list;
	OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);
	
	ListLock(&list, LIST_LOCK_MODE_READ);
	ListLock(&list, LIST_LOCK_MODE_READ);
	
	res = ListUnlock(&list);
	TEST_ASSERT_EQUAL(res, OS_RES_OK);
	
	res = ListLock(&list, LIST_LOCK_MODE_WRITE);
	TEST_ASSERT_EQUAL(res, OS_RES_LOCKED);
}

