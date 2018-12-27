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

void test_ListNodeChildGet_invalid_node(void)
{
	void *child = NULL;
	
	child = ListNodeChildGet(NULL);
	
	TEST_ASSERT_EQUAL(child, NULL);
}

void test_ListNodeChildGet_valid_node_no_child(void)
{
	void *child = NULL;
	ListNode_t node;
	
	node.child = NULL;
	
	child = ListNodeChildGet(&node);
	
	TEST_ASSERT_EQUAL(child, NULL);
}

void test_ListNodeChildGet_valid_node_w_child(void)
{
	int value = 1;
	void *child = (void *)&value;
	ListNode_t node;
	
	node.child = child;
	
	child = ListNodeChildGet(&node);
	
	TEST_ASSERT_EQUAL(child, (void *)&value);
}