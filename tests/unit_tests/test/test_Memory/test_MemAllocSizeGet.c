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
#define TEST_POOL_SIZE MEM_BLOCK_SIZE * 12

#define TEST_ALLOC_SIZE_SINGLE_BLOCK MEM_BLOCK_SIZE - MEM_ALLOC_SIZE_SIZE_BYTES
#define TEST_ALLOC_SIZE_MULTI_BLOCK TEST_ALLOC_SIZE_SINGLE_BLOCK + MEM_BLOCK_SIZE

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

void test_MemAllocSizeGet_invalid_pointer(void)
{
	U32_t size = 0;
	
	size = MemAllocSizeGet(NULL);
	
	TEST_ASSERT_EQUAL(0, size);
}

void test_MemAllocSizeGet_oob_pointer(void)
{
	U32_t size = 0;
	
	size = MemAllocSizeGet(&TestHeap[TEST_HEAP_SIZE] + 1);
	
	TEST_ASSERT_EQUAL(0, size);
}

void test_MemAllocSizeGet_obj_pool_no_kernel_mode(void)
{
	void *addr = NULL;
	U32_t size = 0;
	
	LogError_Ignore();
	KCoreFlagGet_ExpectAndReturn(CORE_FLAG_KERNEL_MODE, 1);
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();

	addr = MemAlloc(OBJ_POOL_ID, TEST_ALLOC_SIZE_SINGLE_BLOCK);
	TEST_ASSERT_NOT_EQUAL(NULL, addr);
	
	KCoreFlagGet_ExpectAndReturn(CORE_FLAG_KERNEL_MODE, 0);

	size = MemAllocSizeGet(addr);
	
	TEST_ASSERT_EQUAL(0, size);
}


void test_MemAllocSizeGet_valid(void)
{
	void *addr = NULL;
	U32_t size = 0;
	
	LogError_Ignore();
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	
	addr = MemAlloc(TestPool, TEST_ALLOC_SIZE_SINGLE_BLOCK);
	TEST_ASSERT_NOT_EQUAL(NULL, addr);
	
	size = MemAllocSizeGet(addr);
	
	TEST_ASSERT_EQUAL(TEST_ALLOC_SIZE_SINGLE_BLOCK, size);
}

