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
#include <string.h>
 
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

void test_ListNodeIdSet_invalid_node(void)
{
	OsResult_t res = OS_RES_ERROR;
	const Id_t id = 2;
	
	res = ListNodeIdSet(NULL, id);
	TEST_ASSERT_EQUAL(res, OS_RES_INVALID_ARGUMENT);
}

void test_ListNodeInit_valid_node(void)
{
	OsResult_t res = OS_RES_ERROR;
	ListNode_t node;
	const Id_t id = 2;
	
	memset(&node, 1, sizeof(node));
	
	ListNodeInit(&node, NULL);
	
	res = ListNodeIdSet(&node, id);
	TEST_ASSERT_EQUAL(res, OS_RES_OK);
	TEST_ASSERT_EQUAL(node.id, id);
}