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

#include "mock_Os.h"

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

void test_ListInit_invalid_list(void)
{
	OsResult_t res = ListInit(NULL, ID_GROUP_MESSAGE_QUEUE);
	TEST_ASSERT_EQUAL_MESSAGE(res, OS_RES_INVALID_ARGUMENT, "Wrong result code on invalid list.");	
}

void test_ListInit_valid_list(void)
{
	OsResult_t res = OS_RES_ERROR;
	LinkedList_t list;

	res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);
	TEST_ASSERT_EQUAL_MESSAGE(res, OS_RES_OK, "Wrong result code on valid list.");
	TEST_ASSERT_EQUAL_MESSAGE(list.head, NULL, "Head has a non-initialized value.");
	TEST_ASSERT_EQUAL_MESSAGE(list.tail, NULL, "Tail has a non-initialized value.");
	TEST_ASSERT_EQUAL_MESSAGE(list.id_group, ID_GROUP_MESSAGE_QUEUE, "ID group has a non-initialized value.");
	TEST_ASSERT_EQUAL_MESSAGE(list.size, 0, "List size is not 0.");
	TEST_ASSERT_EQUAL_MESSAGE(list.lock, 0, "List is in a locked state.");	
}
