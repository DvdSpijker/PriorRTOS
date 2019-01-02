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
#define TEST_HEAP_SIZE	MEM_BLOCK_SIZE * 15
#define TEST_USER_HEAP_SIZE MEM_BLOCK_SIZE * 12

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
}
 
void tearDown(void)
{

}
 
/*******************************************************************************
 *    TESTS
 ******************************************************************************/

void test_MemOsHeapSpaceGet_all_pools_in_use(void)
{
	U32_t free_space = 0;
	int used = 0;
	
	LogError_Ignore();
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	
	for(; used < MEM_NUM_POOLS; used++) {
		TestPoolTable[used].pool_size = 2;
		TestPoolTable[used].mem_left = 1;
	}
	
	free_space = MemOsHeapFreeSpaceGet();
	
	TEST_ASSERT_EQUAL(TEST_HEAP_SIZE - used, free_space);
}

void test_MemOsHeapSpaceGet_some_pools_in_use(void)
{
	U32_t free_space = 0;
	int used = 0;
	
	LogError_Ignore();
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	
	for(; used < MEM_NUM_POOLS; used++) {
		TestPoolTable[used].pool_size = 2;
		TestPoolTable[used].mem_left = 1;
	}
	
	TestPoolTable[0].pool_size = 0;
	TestPoolTable[MEM_NUM_POOLS - 1].pool_size = 0;
	used -= 2;
	
	free_space = MemOsHeapFreeSpaceGet();
	
	TEST_ASSERT_EQUAL(TEST_HEAP_SIZE - used, free_space);
}




