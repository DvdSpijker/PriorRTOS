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

void test_ListNodeInit_invalid_node(void)
{
	OsResult_t res = OS_RES_ERROR;
	
	res = ListNodeInit(NULL, &res);
	TEST_ASSERT_EQUAL(res, OS_RES_INVALID_ARGUMENT);
}

void test_ListNodeInit_valid_node_no_child(void)
{
	OsResult_t res = OS_RES_ERROR;
	ListNode_t node;
	
	memset(&node, 1, sizeof(node));
	
	res = ListNodeInit(&node, NULL);
	TEST_ASSERT_EQUAL(res, OS_RES_OK);
	TEST_ASSERT_EQUAL(node.child, NULL);
}

void test_ListNodeInit_valid_node_w_child(void)
{
	OsResult_t res = OS_RES_ERROR;
	ListNode_t node;
	
	memset(&node, 1, sizeof(node));
	
	res = ListNodeInit(&node, &res);
	TEST_ASSERT_EQUAL(res, OS_RES_OK);
	TEST_ASSERT_EQUAL(node.child, &res);
}