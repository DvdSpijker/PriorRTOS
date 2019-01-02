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

void test_ListNodeChildSet_invalid_node(void)
{
	OsResult_t res = OS_RES_ERROR;
	int value = 1;
	
	res = ListNodeChildSet(NULL, (void *)&value);
	TEST_ASSERT_EQUAL(res, OS_RES_INVALID_ARGUMENT);
}

void test_ListNodeChildSet_locked_node(void)
{
	OsResult_t res = OS_RES_ERROR;
	int value = 1;
	ListNode_t node;
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	ListNodeLock(&node, LIST_LOCK_MODE_WRITE);
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	res = ListNodeChildSet(&node, (void *)&value);
	TEST_ASSERT_EQUAL(res, OS_RES_LOCKED);
}

void test_ListNodeChildSet_child_null(void)
{
	OsResult_t res = OS_RES_ERROR;
	ListNode_t node;
	node.lock = 0;
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	
	res = ListNodeChildSet(&node, NULL);
	TEST_ASSERT_EQUAL(res, OS_RES_OK);
	TEST_ASSERT_EQUAL(node.child, NULL);
	TEST_ASSERT_EQUAL(node.lock, 0);
}

void test_ListNodeChildSet_child_not_null(void)
{
	OsResult_t res = OS_RES_ERROR;
	int value = 1;
	ListNode_t node;
	node.lock = 0;
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	
	res = ListNodeChildSet(&node, (void *)&value);
	TEST_ASSERT_EQUAL(res, OS_RES_OK);
	TEST_ASSERT_EQUAL(node.child, (void *)&value);
	TEST_ASSERT_EQUAL(node.lock, 0);
}


