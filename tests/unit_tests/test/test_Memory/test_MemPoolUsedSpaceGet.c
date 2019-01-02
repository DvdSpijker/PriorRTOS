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

void test_MemPoolUsedSpaceGet_invalid_id(void)
{
	U32_t free_space = 0;
	
	LogError_Ignore();
	
	free_space = MemPoolUsedSpaceGet(ID_INVALID);
	
	TEST_ASSERT_EQUAL(0, free_space);

	free_space = MemPoolUsedSpaceGet(MEM_NUM_POOLS + 1);
	
	TEST_ASSERT_EQUAL(0, free_space);
}

void test_MemPoolUsedSpaceGet_valid_id(void)
{
	U32_t free_space = 0;
	
	LogError_Ignore();
	OsCritSectBegin_Expect();
	OsCritSectEnd_Expect();
	
	TestPoolTable[OBJ_POOL_ID].mem_left = 1;
	TestPoolTable[OBJ_POOL_ID].pool_size = 3;
	
	free_space = MemPoolUsedSpaceGet(OBJ_POOL_ID);
	
	TEST_ASSERT_EQUAL(2, free_space);
}



