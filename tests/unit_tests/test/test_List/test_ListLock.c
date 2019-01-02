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

void test_ListLock_invalid_list(void)
{
	OsResult_t res = ListLock(NULL, LIST_LOCK_MODE_READ);
	TEST_ASSERT_EQUAL(res, OS_RES_INVALID_ARGUMENT);
}

void test_ListLock_lock_read(void)
{
	LinkedList_t list;
	OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	
	res = ListLock(&list, LIST_LOCK_MODE_READ);
	TEST_ASSERT_EQUAL(res, OS_RES_OK);
}

void test_ListLock_lock_read_twice(void)
{
	LinkedList_t list;
	OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	
	res = ListLock(&list, LIST_LOCK_MODE_READ);
	TEST_ASSERT_EQUAL(res, OS_RES_OK);
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	
	res = ListLock(&list, LIST_LOCK_MODE_READ);
	TEST_ASSERT_EQUAL(res, OS_RES_OK);
}

void test_ListLock_lock_read_max(void)
{
	LinkedList_t list;
	OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);
	
	for(int i = 0; i < LIST_LOCK_READ_COUNT_MAX; i++) {
		OsCritSectBegin_Expect();
		OsCritSectEnd_Expect();
		res = ListLock(&list, LIST_LOCK_MODE_READ);
		TEST_ASSERT_EQUAL(res, OS_RES_OK);
	}
}

void test_ListLock_lock_read_max_plus_one(void)
{
	LinkedList_t list;
	OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);
	
	for(int i = 0; i < LIST_LOCK_READ_COUNT_MAX; i++) {
		OsCritSectBegin_Expect();
		OsCritSectEnd_Expect();
		res = ListLock(&list, LIST_LOCK_MODE_READ);
		TEST_ASSERT_EQUAL(res, OS_RES_OK);
	}
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	res = ListLock(&list, LIST_LOCK_MODE_READ);
	TEST_ASSERT_EQUAL(res, OS_RES_LOCKED);
}

void test_ListLock_lock_write(void)
{
	LinkedList_t list;
	OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	res = ListLock(&list, LIST_LOCK_MODE_WRITE);
	TEST_ASSERT_EQUAL(res, OS_RES_OK);
}

void test_ListLock_lock_write_twice(void)
{
	LinkedList_t list;
	OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	res = ListLock(&list, LIST_LOCK_MODE_WRITE);
	TEST_ASSERT_EQUAL(res, OS_RES_OK);
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	res = ListLock(&list, LIST_LOCK_MODE_WRITE);
	TEST_ASSERT_EQUAL(res, OS_RES_LOCKED);
}

void test_ListLock_lock_read_write(void)
{
	LinkedList_t list;
	OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	res = ListLock(&list, LIST_LOCK_MODE_READ);
	TEST_ASSERT_EQUAL(res, OS_RES_OK);
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	res = ListLock(&list, LIST_LOCK_MODE_WRITE);
	TEST_ASSERT_EQUAL(res, OS_RES_LOCKED);
}

void test_ListLock_lock_write_read(void)
{
	LinkedList_t list;
	OsResult_t res = ListInit(&list, ID_GROUP_MESSAGE_QUEUE);
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	res = ListLock(&list, LIST_LOCK_MODE_WRITE);
	TEST_ASSERT_EQUAL(res, OS_RES_OK);
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	res = ListLock(&list, LIST_LOCK_MODE_READ);
	TEST_ASSERT_EQUAL(res, OS_RES_LOCKED);
}
