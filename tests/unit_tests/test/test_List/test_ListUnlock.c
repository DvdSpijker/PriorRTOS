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

void test_ListUnlock_not_locked(void)
{
	LinkedList_t list;
	OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	res = ListUnlock(&list);
	TEST_ASSERT_EQUAL(res, OS_RES_ERROR);
}

void test_ListUnlock_once(void)
{
	LinkedList_t list;
	OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	ListLock(&list, LIST_LOCK_MODE_WRITE);
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	res = ListUnlock(&list);
	TEST_ASSERT_EQUAL(res, OS_RES_OK);
}

void test_ListUnlock_max(void)
{
	LinkedList_t list;
	OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);
	
	for(int i = 0; i < LIST_LOCK_READ_COUNT_MAX; i++) {
		OsCritSectBegin_Expect();
		OsCritSectEnd_Expect();
		ListLock(&list, LIST_LOCK_MODE_READ);
	}
	
	for(int i = 0; i < LIST_LOCK_READ_COUNT_MAX; i++) {
		OsCritSectBegin_Expect();
		OsCritSectEnd_Expect();
		res = ListUnlock(&list);
		TEST_ASSERT_EQUAL(res, OS_RES_OK);	
	}
}

void test_ListUnlock_lock_after_unlock(void)
{
	LinkedList_t list;
	OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	ListLock(&list, LIST_LOCK_MODE_READ);
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	ListLock(&list, LIST_LOCK_MODE_READ);
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	res = ListUnlock(&list);
	TEST_ASSERT_EQUAL(res, OS_RES_OK);
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	res = ListLock(&list, LIST_LOCK_MODE_WRITE);
	TEST_ASSERT_EQUAL(res, OS_RES_LOCKED);
}

