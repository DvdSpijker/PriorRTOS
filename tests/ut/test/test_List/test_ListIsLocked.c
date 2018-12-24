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

/***** Test ListIsLocked *****/

void test_ListIsLocked_invalid_list(void)
{
	bool is_locked = false;
	
	is_locked = ListIsLocked(NULL);
	TEST_ASSERT_EQUAL(is_locked, false);
}

void test_ListIsLocked_locked(void)
{
	bool is_locked = false;

	LinkedList_t list;
	ListInit(&list, ID_GROUP_MESSAGE_QUEUE);
	
	ListLock(&list, LIST_LOCK_MODE_READ);
	
	is_locked = ListIsLocked(&list);
	TEST_ASSERT_EQUAL(is_locked, true);
}

void test_ListIsLocked_unlocked(void)
{
	bool is_locked = false;

	LinkedList_t list;
	ListInit(&list, ID_GROUP_MESSAGE_QUEUE);
	
	is_locked = ListIsLocked(&list);
	TEST_ASSERT_EQUAL(is_locked, false);
}

void test_ListIsLocked_unlocked_after_locked(void)
{
	bool is_locked = false;

	LinkedList_t list;
	ListInit(&list, ID_GROUP_MESSAGE_QUEUE);
	
	ListLock(&list, LIST_LOCK_MODE_READ);
	ListUnlock(&list);
	
	is_locked = ListIsLocked(&list);
	TEST_ASSERT_EQUAL(is_locked, false);
}
