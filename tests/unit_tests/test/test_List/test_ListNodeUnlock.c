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

void test_ListNodeUnlock_invalid_node(void)
{
	OsResult_t res = OS_RES_ERROR;
	
	res = ListNodeUnlock(NULL);
	TEST_ASSERT_EQUAL(res, OS_RES_INVALID_ARGUMENT);
}

void test_ListNodeUnlock_not_locked(void)
{
	OsResult_t res = OS_RES_ERROR;
	ListNode_t node;
	
	ListNodeInit(&node, NULL);
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	res = ListNodeUnlock(&node);
	TEST_ASSERT_EQUAL(res, OS_RES_ERROR);
}

void test_ListNodeUnlock_once(void)
{
	OsResult_t res = OS_RES_ERROR;
	ListNode_t node;
	
	ListNodeInit(&node, NULL);
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	ListNodeLock(&node, LIST_LOCK_MODE_READ);
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	res = ListNodeUnlock(&node);
	TEST_ASSERT_EQUAL(res, OS_RES_OK);
}

void test_ListNodeUnlock_max(void)
{
	OsResult_t res = OS_RES_ERROR;
	ListNode_t node;
	
	ListNodeInit(&node, NULL);
	
	for(int i = 0; i < LIST_LOCK_READ_COUNT_MAX; i++) {
		OsCritSectBegin_Expect();
		OsCritSectEnd_Expect();
		ListNodeLock(&node, LIST_LOCK_MODE_READ);
	}
	
	for(int i = 0; i < LIST_LOCK_READ_COUNT_MAX; i++) {
		OsCritSectBegin_Expect();
		OsCritSectEnd_Expect();
		res = ListNodeUnlock(&node);
		TEST_ASSERT_EQUAL(res, OS_RES_OK);	
	}
}

void test_ListNodeUnlock_lock_after_unlock(void)
{
	OsResult_t res = OS_RES_ERROR;
	ListNode_t node;
	
	ListNodeInit(&node, NULL);
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	ListNodeLock(&node, LIST_LOCK_MODE_READ);
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	ListNodeLock(&node, LIST_LOCK_MODE_READ);
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	res = ListNodeUnlock(&node);
	TEST_ASSERT_EQUAL(res, OS_RES_OK);
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	res = ListNodeLock(&node, LIST_LOCK_MODE_WRITE);
	TEST_ASSERT_EQUAL(res, OS_RES_LOCKED);
}

