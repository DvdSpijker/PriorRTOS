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

 
/*******************************************************************************
 *    DEFINITIONS
 ******************************************************************************/
#define TEST_HEAP_SIZE	0x100
#define TEST_USER_HEAP_SIZE 0x50

/*******************************************************************************
 *    PRIVATE TYPES
 ******************************************************************************/
 
/*******************************************************************************
 *    PRIVATE DATA
 ******************************************************************************/
 
Pmb_t TestPoolTable[MEM_NUM_POOLS];
U8_t TestHeap[TEST_HEAP_SIZE];
 
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

void test_KMemInit_invalid_heap(void)
{
	OsResult_t res = OS_RES_ERROR;
	
	KLogInfo_Ignore();
	LogError_Ignore();
	KCoreKernelModeEnter_IgnoreAndReturn(1);
	KCoreKernelModeExit_IgnoreAndReturn(0);
	
	res = KMemInit(NULL, TEST_HEAP_SIZE, TEST_USER_HEAP_SIZE, TestPoolTable);
	
	TEST_ASSERT_EQUAL(res, OS_RES_INVALID_ARGUMENT);
}

void test_KMemInit_invalid_heap_size(void)
{
	OsResult_t res = OS_RES_ERROR;
	
	KLogInfo_Ignore();
	LogError_Ignore();
	KCoreKernelModeEnter_IgnoreAndReturn(1);
	KCoreKernelModeExit_IgnoreAndReturn(0);
	
	res = KMemInit(TestHeap, 0, TEST_USER_HEAP_SIZE, TestPoolTable);
	
	TEST_ASSERT_EQUAL(res, OS_RES_INVALID_ARGUMENT);
}

void test_KMemInit_invalid_pool_table(void)
{
	OsResult_t res = OS_RES_ERROR;
	
	KLogInfo_Ignore();
	LogError_Ignore();
	KCoreKernelModeEnter_IgnoreAndReturn(1);
	KCoreKernelModeExit_IgnoreAndReturn(0);
	
	res = KMemInit(TestHeap, TEST_HEAP_SIZE, TEST_USER_HEAP_SIZE, NULL);
	
	TEST_ASSERT_EQUAL(res, OS_RES_INVALID_ARGUMENT);
}

void test_KMemInit_no_user_heap(void)
{
	OsResult_t res = OS_RES_ERROR;
	
	TestHeap[0] = 0xFF;
	TestHeap[TEST_HEAP_SIZE - 1] = 0xFF;
	TestPoolTable[0].pool_size = 0;
	TestPoolTable[1].pool_size = 0;
	TestPoolTable[MEM_NUM_POOLS - 1].pool_size = 0;
	
	KLogInfo_Ignore();
	LogError_Ignore();
	KCoreKernelModeEnter_IgnoreAndReturn(1);
	KCoreKernelModeExit_IgnoreAndReturn(0);
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	
	res = KMemInit(TestHeap, TEST_HEAP_SIZE, TEST_USER_HEAP_SIZE, TestPoolTable);
	
	TEST_ASSERT_EQUAL(res, OS_RES_OK);
	TEST_ASSERT_EQUAL(TestHeap[0], 0);
	TEST_ASSERT_EQUAL(TestHeap[TEST_HEAP_SIZE - 1], 0);
	
	TEST_ASSERT_EQUAL(TestPoolTable[0].pool_size, KERNEL_POOL_SIZE_BYTES);
	TEST_ASSERT_EQUAL(TestPoolTable[1].pool_size, TEST_HEAP_SIZE - TEST_USER_HEAP_SIZE - KERNEL_POOL_SIZE_BYTES);
	for(int i = 2; i < MEM_NUM_POOLS; i++) {
		TEST_ASSERT_EQUAL(TestPoolTable[i].pool_size, 0);
	}
}
