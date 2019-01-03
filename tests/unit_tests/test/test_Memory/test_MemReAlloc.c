/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/
 
/* Test framework */
#include "unity.h"
 
#include "mock_CoreDef.h"
#include "mock_LoggerDef.h"
#include "mock_Logger.h"
#include "mock_Os.h"

/* UUT */
#include "Memory.h"
#include "MemoryDef.h"

/* Dependencies */

/* Other */
#include <string.h>

 
/*******************************************************************************
 *    DEFINITIONS
 ******************************************************************************/
#define TEST_HEAP_SIZE	MEM_BLOCK_SIZE * 15
#define TEST_USER_HEAP_SIZE MEM_BLOCK_SIZE * 10
#define TEST_POOL_SIZE MEM_BLOCK_SIZE * 5

#define TEST_ALLOC_SIZE_NORMAL MEM_BLOCK_SIZE - MEM_ALLOC_SIZE_SIZE_BYTES
#define TEST_ALLOC_SIZE_SMALLER (MEM_BLOCK_SIZE / 2) - MEM_ALLOC_SIZE_SIZE_BYTES
#define TEST_ALLOC_SIZE_LARGER (MEM_BLOCK_SIZE * 2) - MEM_ALLOC_SIZE_SIZE_BYTES
#define TEST_ALLOC_SIZE_EXACT TEST_POOL_SIZE -  MEM_ALLOC_SIZE_SIZE_BYTES

#define OBJ_POOL_ID 0

/*******************************************************************************
 *    PRIVATE TYPES
 ******************************************************************************/
 
/*******************************************************************************
 *    PRIVATE DATA
 ******************************************************************************/
 
Pmb_t TestPoolTable[MEM_NUM_POOLS];
U8_t TestHeap[TEST_HEAP_SIZE];
Id_t TestPool = ID_INVALID;
 
/*******************************************************************************
 *    PRIVATE FUNCTIONS
 ******************************************************************************/
 
/*******************************************************************************
 *    SETUP, TEARDOWN
 ******************************************************************************/
 
void setUp(void)
{
	KLogInfo_Ignore();
	LogError_Ignore();
	KCoreKernelModeEnter_IgnoreAndReturn(1);
	KCoreKernelModeExit_IgnoreAndReturn(0);
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	
	TEST_ASSERT_EQUAL(KMemInit(TestHeap, TEST_HEAP_SIZE, 
			TEST_USER_HEAP_SIZE, TestPoolTable), OS_RES_OK);
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();

	TestPool = MemPoolCreate(TEST_POOL_SIZE);
	TEST_ASSERT_NOT_EQUAL(ID_INVALID, TestPool);
}
 
void tearDown(void)
{
	TestPool = ID_INVALID;
}
 
/*******************************************************************************
 *    TESTS
 ******************************************************************************/

void test_MemReAlloc_invalid_pointer(void)
{
	OsResult_t res = OS_RES_ERROR;
	
	res = MemReAlloc(TestPool, TestPool, NULL, 1);
	
	TEST_ASSERT_EQUAL(res, OS_RES_INVALID_ARGUMENT);
}

void test_MemReAlloc_invalid_size(void)
{
	OsResult_t res = OS_RES_ERROR;
	
	res = MemReAlloc(TestPool, TestPool, (void **)&res, 0);
	
	TEST_ASSERT_EQUAL(res, OS_RES_INVALID_ARGUMENT);
}

void test_MemReAlloc_invalid_id(void)
{
	OsResult_t res = OS_RES_ERROR;
	
	res = MemReAlloc(TestPool, ID_INVALID, (void **)&res, 1);
	
	TEST_ASSERT_EQUAL(res, OS_RES_INVALID_ID);
	
	res = MemReAlloc(ID_INVALID, TestPool, (void **)&res, 1);
	
	TEST_ASSERT_EQUAL(res, OS_RES_INVALID_ID);
	
	res = MemReAlloc(TestPool, MEM_NUM_POOLS + 1, (void **)&res, 1);
	
	TEST_ASSERT_EQUAL(res, OS_RES_INVALID_ID);
	
	res = MemReAlloc(MEM_NUM_POOLS + 1, TestPool, (void **)&res, 1);
	
	TEST_ASSERT_EQUAL(res, OS_RES_INVALID_ID);
}

void test_MemReAlloc_obj_pool_no_kernel_mode(void)
{
	OsResult_t res = OS_RES_ERROR;
	
	LogError_Ignore();
	
	KCoreFlagGet_ExpectAndReturn(CORE_FLAG_KERNEL_MODE, 0);

	res = MemReAlloc(OBJ_POOL_ID, TestPool, (void **)&res, 1);
	
	TEST_ASSERT_EQUAL(res, OS_RES_RESTRICTED);
	
	
	KCoreFlagGet_ExpectAndReturn(CORE_FLAG_KERNEL_MODE, 0);

	res = MemReAlloc(TestPool, OBJ_POOL_ID, (void **)&res, 1);
	
	TEST_ASSERT_EQUAL(res, OS_RES_RESTRICTED);
}

void test_MemReAlloc_no_space(void)
{
	OsResult_t res = OS_RES_ERROR;
	U8_t *ptr = NULL;
	U8_t *ptr_cpy = NULL;
	
	LogError_Ignore();
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	
	ptr = (U8_t *)MemAlloc(TestPool, TEST_ALLOC_SIZE_NORMAL);
	TEST_ASSERT_NOT_NULL(ptr);
	ptr_cpy = ptr;
	
	memset(ptr, 1, TEST_ALLOC_SIZE_NORMAL);
	
	OsCritSectBegin_Expect(); /* Lock in func. */
	OsCritSectEnd_Expect(); /* Unlock in func. */
	
	res = MemReAlloc(TestPool, TestPool, (void **)&ptr, TEST_ALLOC_SIZE_EXACT);
	
	TEST_ASSERT_EQUAL(OS_RES_FAIL, res);
	TEST_ASSERT_NOT_NULL(ptr);
	TEST_ASSERT_EQUAL(ptr_cpy, ptr);
	TEST_ASSERT_EACH_EQUAL_UINT8(1, ptr, TEST_ALLOC_SIZE_NORMAL);
}

void test_MemReAlloc_same_pool_larger(void)
{
	OsResult_t res = OS_RES_ERROR;
	U8_t *ptr = NULL;
	U8_t *ptr_cpy = NULL;
	
	LogError_Ignore();
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	
	ptr = (U8_t *)MemAlloc(TestPool, TEST_ALLOC_SIZE_NORMAL);
	TEST_ASSERT_NOT_NULL(ptr);
	ptr_cpy = ptr;
	
	memset(ptr, 1, TEST_ALLOC_SIZE_NORMAL);
	
	OsCritSectBegin_Expect(); /* Lock in func. */
	OsCritSectBegin_Expect(); /* Lock/unlock in MemAlloc. */
	OsCritSectEnd_Expect();
	OsCritSectBegin_Expect(); /* Lock/unlock in MemFree. */
	OsCritSectEnd_Expect();
	OsCritSectEnd_Expect(); /* Unlock in func. */
	
	res = MemReAlloc(TestPool, TestPool, (void **)&ptr, TEST_ALLOC_SIZE_LARGER);
	
	TEST_ASSERT_EQUAL(OS_RES_OK, res);
	TEST_ASSERT_NOT_NULL(ptr);
	TEST_ASSERT_NOT_EQUAL(ptr_cpy, ptr);
	TEST_ASSERT_EACH_EQUAL_UINT8(1, ptr, TEST_ALLOC_SIZE_NORMAL);
}

void test_MemReAlloc_same_pool_smaller(void)
{
	OsResult_t res = OS_RES_ERROR;
	U8_t *ptr = NULL;
	U8_t *ptr_cpy = NULL;
	
	LogError_Ignore();
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	
	ptr = (U8_t *)MemAlloc(TestPool, TEST_ALLOC_SIZE_NORMAL);
	TEST_ASSERT_NOT_NULL(ptr);
	ptr_cpy = ptr;
	
	memset(ptr, 1, TEST_ALLOC_SIZE_NORMAL);
	
	OsCritSectBegin_Expect(); /* Lock in func. */
	OsCritSectBegin_Expect(); /* Lock/unlock in MemAlloc. */
	OsCritSectEnd_Expect();
	OsCritSectBegin_Expect(); /* Lock/unlock in MemFree. */
	OsCritSectEnd_Expect();
	OsCritSectEnd_Expect(); /* Unlock in func. */
	
	res = MemReAlloc(TestPool, TestPool, (void **)&ptr, TEST_ALLOC_SIZE_SMALLER);
	
	TEST_ASSERT_EQUAL(OS_RES_OK, res);
	TEST_ASSERT_NOT_NULL(ptr);
	TEST_ASSERT_NOT_EQUAL(ptr_cpy, ptr);
	TEST_ASSERT_EACH_EQUAL_UINT8(1, ptr, TEST_ALLOC_SIZE_SMALLER);
}

void test_MemReAlloc_diff_pool(void)
{
	OsResult_t res = OS_RES_ERROR;
	U8_t *ptr = NULL;
	U8_t *ptr_cpy = NULL;
	
	LogError_Ignore();
	
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	
	ptr = (U8_t *)MemAlloc(TestPool, TEST_ALLOC_SIZE_NORMAL);
	TEST_ASSERT_NOT_NULL(ptr);
	ptr_cpy = ptr;
	
	memset(ptr, 1, TEST_ALLOC_SIZE_NORMAL);
	
	KCoreFlagGet_ExpectAndReturn(CORE_FLAG_KERNEL_MODE, 1);
	OsCritSectBegin_Expect(); /* Lock in func. */
	KCoreFlagGet_ExpectAndReturn(CORE_FLAG_KERNEL_MODE, 1);
	OsCritSectBegin_Expect(); /* Lock/unlock in MemAlloc. */
	OsCritSectEnd_Expect();
	OsCritSectBegin_Expect(); /* Lock/unlock in MemFree. */
	OsCritSectEnd_Expect();
	OsCritSectEnd_Expect(); /* Unlock in func. */
	
	res = MemReAlloc(TestPool, OBJ_POOL_ID, (void **)&ptr, TEST_ALLOC_SIZE_LARGER);
	
	TEST_ASSERT_EQUAL(OS_RES_OK, res);
	TEST_ASSERT_NOT_NULL(ptr);
	TEST_ASSERT_NOT_EQUAL(ptr_cpy, ptr);
	TEST_ASSERT_EACH_EQUAL_UINT8(1, ptr, TEST_ALLOC_SIZE_NORMAL);	
}
