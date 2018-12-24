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

void test_ListDestroy_invalid_list(void)
{
	OsResult_t res = ListDestroy(NULL);
	TEST_ASSERT_EQUAL_MESSAGE(res, OS_RES_INVALID_ARGUMENT, "Wrong result code on invalid list.");
}

void test_ListDestroy_locked_list(void)
{
	LinkedList_t list;
	OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);
	TEST_ASSERT_EQUAL_MESSAGE(res, OS_RES_OK, "Wrong result code on valid list.");
	
	list.lock = 1;
	
	res = ListDestroy(&list);
	TEST_ASSERT_EQUAL_MESSAGE(res, OS_RES_LOCKED, "Wrong result code on locked list.");
}

void test_ListDestroy_empty_list(void)
{
	LinkedList_t list;
	OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);
	TEST_ASSERT_EQUAL_MESSAGE(res, OS_RES_OK, "Wrong result code on valid list.");
	
	res = ListDestroy(&list);
	TEST_ASSERT_EQUAL_MESSAGE(res, OS_RES_OK, "Wrong result code on empty list.");
	TEST_ASSERT_EQUAL_MESSAGE(ListIsLocked(&list), true, "Detroyed list must be locked.");
	TEST_ASSERT_EQUAL_MESSAGE(list.head, NULL, "Head not deinitialized.");
	TEST_ASSERT_EQUAL_MESSAGE(list.tail, NULL, "Tail not deinitialized.");
	TEST_ASSERT_EQUAL_MESSAGE(list.size, 0, "Destroyed list must be size 0.");
}
