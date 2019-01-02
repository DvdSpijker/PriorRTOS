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

void test_ListNodeLock_invalid_node(void)
{
	OsResult_t res = ListNodeLock(NULL, LIST_LOCK_MODE_READ);
	TEST_ASSERT_EQUAL(res, OS_RES_INVALID_ARGUMENT);
}

void test_ListNodeLock_lock_read(void)
{
	OsResult_t res = OS_RES_ERROR;
	ListNode_t node;
	
	ListNodeInit(&node, NULL);
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	res = ListNodeLock(&node, LIST_LOCK_MODE_READ);
	TEST_ASSERT_EQUAL(res, OS_RES_OK);
}

void test_ListNodeLock_lock_read_twice(void)
{
	OsResult_t res = OS_RES_ERROR;
	ListNode_t node;
	
	ListNodeInit(&node, NULL);
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	res = ListNodeLock(&node, LIST_LOCK_MODE_READ);
	TEST_ASSERT_EQUAL(res, OS_RES_OK);
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	res = ListNodeLock(&node, LIST_LOCK_MODE_READ);
	TEST_ASSERT_EQUAL(res, OS_RES_OK);
}

void test_ListNodeLock_lock_read_max(void)
{
	OsResult_t res = OS_RES_ERROR;
	ListNode_t node;
	
	ListNodeInit(&node, NULL);
	
	for(int i = 0; i < LIST_LOCK_READ_COUNT_MAX; i++) {
		OsCritSectBegin_Expect();
		OsCritSectEnd_Expect();
		res = ListNodeLock(&node, LIST_LOCK_MODE_READ);
		TEST_ASSERT_EQUAL(res, OS_RES_OK);
	}
}

void test_ListNodeLock_lock_read_max_plus_one(void)
{
	OsResult_t res = OS_RES_ERROR;
	ListNode_t node;
	
	ListNodeInit(&node, NULL);
	
	for(int i = 0; i < LIST_LOCK_READ_COUNT_MAX; i++) {
		OsCritSectBegin_Expect();
		OsCritSectEnd_Expect();
		res = ListNodeLock(&node, LIST_LOCK_MODE_READ);
		TEST_ASSERT_EQUAL(res, OS_RES_OK);
	}
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	res = ListNodeLock(&node, LIST_LOCK_MODE_READ);
	TEST_ASSERT_EQUAL(res, OS_RES_LOCKED);
}

void test_ListNodeLock_lock_write(void)
{
	OsResult_t res = OS_RES_ERROR;
	ListNode_t node;
	
	ListNodeInit(&node, NULL);
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	res = ListNodeLock(&node, LIST_LOCK_MODE_WRITE);
	TEST_ASSERT_EQUAL(res, OS_RES_OK);
}

void test_ListNodeLock_lock_write_twice(void)
{
	OsResult_t res = OS_RES_ERROR;
	ListNode_t node;
	
	ListNodeInit(&node, NULL);
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	res = ListNodeLock(&node, LIST_LOCK_MODE_WRITE);
	TEST_ASSERT_EQUAL(res, OS_RES_OK);
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	res = ListNodeLock(&node, LIST_LOCK_MODE_WRITE);
	TEST_ASSERT_EQUAL(res, OS_RES_LOCKED);
}

void test_ListNodeLock_lock_read_write(void)
{
	OsResult_t res = OS_RES_ERROR;
	ListNode_t node;
	
	ListNodeInit(&node, NULL);
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	res = ListNodeLock(&node, LIST_LOCK_MODE_WRITE);
	TEST_ASSERT_EQUAL(res, OS_RES_OK);
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	res = ListNodeLock(&node, LIST_LOCK_MODE_READ);
	TEST_ASSERT_EQUAL(res, OS_RES_LOCKED);
}

void test_ListNodeLock_lock_write_read(void)
{
	OsResult_t res = OS_RES_ERROR;
	ListNode_t node;
	
	ListNodeInit(&node, NULL);
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	res = ListNodeLock(&node, LIST_LOCK_MODE_READ);
	TEST_ASSERT_EQUAL(res, OS_RES_OK);
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	res = ListNodeLock(&node, LIST_LOCK_MODE_WRITE);
	TEST_ASSERT_EQUAL(res, OS_RES_LOCKED);
}
